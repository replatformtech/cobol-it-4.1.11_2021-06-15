/*
  * Copyright (C) 2008-2020 Cobol-IT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */
#include "config.h"
#include "defaults.h"
#include "globaldefine.h"
#include "defaults.h"
#include "iconv.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#ifdef	HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cobc/tree.h"
#include "cobc/cobc.h"
#include "libcob/common.h"
#include "libcob/fileio.h"
#include "libcob/enterprise/debugdb.h"
#include "citenterprise_runtime.h"
#ifdef WITH_ICU
#  ifdef WITH_ICU_GENERIC
#     define UErrorCode int
#     define U_ZERO_ERROR 0
void* ucnv_open(const char *n, UErrorCode *e);
void   ucnv_close(void *c);
int    ucnv_countAvailable(void);
int    ucnv_countAliases(const char *n, UErrorCode *e);
const char* ucnv_getAlias(const char *n, int i, UErrorCode *e);
const char* ucnv_getAvailableName(int i);
const char* ucnv_getName(void *c, UErrorCode *e);
void ucnv_convertEx(void *targetCnv, void *sourceCnv,
                    char **target, const char *targetLimit,
                    const char **source, const char *sourceLimit,
                    char *pivotStart, char **pivotSource,
                    char **pivotTarget, const char *pivotLimit,
                    int reset, int flush,
                    UErrorCode *pErrorCode);
int u_strToUpper(char *dest, int destCapacity, const char *src, int srcLength, const char *locale, UErrorCode *e);
#     define U_FAILURE(x) ((x)>U_ZERO_ERROR)
#  else
#     ifndef	_WIN32
#     ifdef     WITH_ICU_CIT
#        define U_LIB_SUFFIX_C_NAME _cit
#     endif
#     endif
#     include "unicode/ucnv.h"
#     include "unicode/ustring.h"
#  endif
#endif


/* UNICODE Stuff */
#define COB_NATIONAL_CP_ID "UTF-16BE"
#ifndef WITH_ICU
static void *cb_national_iconv_cd = NULL;
#endif

COB_DLL_EXPIMP int check_citkey(char *product);
COB_DLL_EXPIMP int dump_citkey(char *product);
COB_DLL_EXPIMP int citkey_test_feature(const  char *product);
extern void enterprise_codepage_init(char *runtime_codepage, char *source_codepage);
/*CIT*/

void cob_enterprise_runtime_init(void)
{

}

#ifdef WITH_ICU
static cit_runtime_t cob_rtd = { 0 };
//COB_DLL_EXPIMP void *cob_enterprise_open_one_icu(COB_RTD, const char *name);
void *source_cpd = NULL;
void *runtime_cpd = NULL;

void
enterprise_codepage_init(char *runtime_codepage, char *source_codepage)
{

   if (runtime_codepage)
   {
      runtime_cpd = cob_enterprise_open_one_icu(&cob_rtd, runtime_codepage);
      if (!runtime_cpd)
      {
         fprintf(stderr, "cobc:0: Error: invalid codepage %s\n", runtime_codepage);
         exit(1);
      }
   }
   if (source_codepage)
   {
      source_cpd = cob_enterprise_open_one_icu(&cob_rtd, source_codepage);
      if (!source_cpd)
      {
         fprintf(stderr, "cobc:0: Error: invalid codepage %s\n", source_codepage);
         exit(1);
      }
   }
}

int
enterprise_sourcecp_to_runtimecp(char *s, char *d, size_t ssz, size_t dsz)
{
   char *src = s;
   char *dst = d;
   UErrorCode status = 0;

   if (source_cpd && runtime_cpd)
   {
      ucnv_convertEx(runtime_cpd, source_cpd,
                     &dst, dst + dsz, (const char **)&src, src + ssz, NULL, NULL, NULL, NULL, 1, 1, &status);
      if (U_FAILURE(status))
      {
         fprintf(stderr, "Internal Error: can't convert to runtime codepage %s : %s\n", s, u_errorName(status));
      }
      return dst - d;
   }
   else
   {
      memmove(d, s, ssz);
      return ssz;
   }

}


#else
/*#define COB_NATIONAL_CP_ID "UTF-16BE"*/
static const char *COB_NATIONAL_BE_ID = "UTF-16BE";
static const char *COB_NATIONAL_LE_ID = "UTF-16LE";
static const char*    enterprise__utf16_cp_id(void)
{
   if (cb_flag_utf16_le)
   {
      return COB_NATIONAL_LE_ID;
   }
   return COB_NATIONAL_BE_ID;
}

void
enterprise_codepage_init(char *runtime_codepage, char *source_codepage);
{
   char   cp_buffer[COB_SMALL_BUFF];

#  ifndef _WIN32
   strcpy(cp_buffer, iconv_canonicalize(runtime_codepage));
#  else
   strcpy(cp_buffer, code);
#  endif
   cb_national_iconv_cd = iconv_open(enterprise__utf16_cp_id(),cp_buffer);
   if (!cb_national_iconv_cd || cb_national_iconv_cd ==(iconv_t)(-1))
   {
      fprintf(stderr, "cobc:0: Error: invalid codepage %s\n", code);
      exit(1);
   }
}

int
enterprise_sourcecp_to_runtimecp(char *s, char *d, size_t ssz, size_t dsz)
{
   memmove(d, s, ssz);
   return ssz;
}

#endif

int
enterprise_extfct(unsigned short code, void *p1, void *p2, void *p3, void *p4, void *p5, void *p6)
{
   switch (code)
   {
      case 1:
         switch (check_citkey(p2))
         {
            case 0:
               return 0;
            case 1:
               return 1;
            default:
               dump_citkey(p2);
               return 0;
         }
         break;
      case 2:
         return citkey_test_feature(p2);
         break;
   }
   return 0;
}


static void
output_linkage_desc_field_picture(FILE *file, struct cb_field *f)
{
   if (f->pic)
   {
      if (f->pic->have_sign)
      {
         fprintf(file, "signed=\"TRUE\" ");
      }
      if (!f->flag_real_binary)
      {
         if (f->pic->digits)
         {
            fprintf(file, "digits=\"%d\" ", f->pic->digits);
         }
         if (f->pic->scale)
         {
            fprintf(file, "scale=\"%d\" ", f->pic->scale);
         }

      }
   }
}

static void
output_linkage_desc_field(FILE *file, struct cb_field *field)
{

   if (field->redefines &&
       (field->level != 1) && (field->level != 77))
   {
      return;
   }
   fprintf(file, "<FIELD name=\"%s\" offset=\"%d\" size=\"%d\" occurs=\"%d\" ",
           field->name, field->offset, field->size, field->occurs_max);
   if (field->children)
   {
      struct cb_field *f;

      fprintf(file, "type=\"struct\" >\n");
      for (f = field->children; f; f = f->sister)
      {
         output_linkage_desc_field(file, f);
      }
   }
   else
   {
      switch (field->usage)
      {
         case CB_USAGE_BIT:                   /* 1 */
            fprintf(file, "type=\"bit\" ");
            break;
         case CB_USAGE_BYTE_ARRAY:
         case CB_USAGE_UNSIGNED_CHAR:         /* 15 */
         case CB_USAGE_SIGNED_CHAR:           /* 16 */
         case CB_USAGE_UNSIGNED_SHORT:        /* 17 */
         case CB_USAGE_SIGNED_SHORT:          /* 18 */
         case CB_USAGE_UNSIGNED_INT:          /* 19 */
         case CB_USAGE_SIGNED_INT:            /* 20 */
         case CB_USAGE_UNSIGNED_LONG:         /* 21 */
         case CB_USAGE_SIGNED_LONG:           /* 22 */
         case CB_USAGE_BINARY:                /* 0 */
         case CB_USAGE_COMP_5:                /* 2 */
         case CB_USAGE_COMP_X:                /* 3 */
         case CB_USAGE_LENGTH:                /* 13 */
         case CB_USAGE_INDEX:                 /* 7 */
            if (field->flag_binary_swap)
            {
               fprintf(file, "type=\"binary-swapped\" ");
            }
            else
            {
               fprintf(file, "type=\"binary\" ");
            }
            output_linkage_desc_field_picture(file, field);
            break;

         case CB_USAGE_DISPLAY:               /* 4 */
            if (field->pic && field->pic->category ==  CB_CATEGORY_NUMERIC)
            {
               fprintf(file, "type=\"pic9\" ");
               output_linkage_desc_field_picture(file, field);
            }
            else if (field->pic && field->pic->category ==  CB_CATEGORY_BIT)
            {
               fprintf(file, "type=\"pic1\" ");
            }
            else
            {
               fprintf(file, "type=\"picx\" ");
            }
            break;
         case CB_USAGE_FLOAT:                 /* 5 */
            fprintf(file, "type=\"float\" ");
            break;
         case CB_USAGE_DOUBLE:                /* 6 */
            fprintf(file, "type=\"double\" ");
            break;
         case CB_USAGE_NATIONAL:                /* 6 */
            fprintf(file, "type=\"national\" ");
            break;
         case CB_USAGE_OBJECT:                /* 9 */
         case CB_USAGE_POINTER:               /* 11 */
         case CB_USAGE_PROGRAM:               /* 12 */
         case CB_USAGE_PROGRAM_POINTER:       /* 14 */
            fprintf(file, "type=\"pointer\" ");
            break;
         case CB_USAGE_PACKED:                /* 10 */
            fprintf(file, "type=\"comp-3\" ");
            output_linkage_desc_field_picture(file, field);
            break;
         case CB_USAGE_COMP_6:                /* 23 */
            fprintf(file, "type=\"comp-6\" ");
            output_linkage_desc_field_picture(file, field);
            break;
      }
      fprintf(file, ">\n");
   }
   fprintf(file, "</FIELD>\n");
}

int
cob_enterprise_gen_linkage_descriptor(char *filename, struct cb_program *prog)
{
   FILE                *f;
   struct cb_program   *p;

   if (filename[0] == '+')
   {
      filename++;
      f = fopen(filename, "a");
   }
   else
   {
      f = fopen(filename, "w");
   }
   if (!f)
   {
      cb_error("cobc:0: can't create linkage descriptor file %s\n", filename);
      return -1;
   }
   // MDC write XSD
   for (p = prog; p; p = p->next_program)
   {
      cb_tree list1 = p->entry_list;
      fprintf(f, "<PROGRAM name=\"%s\">\n", p->program_id);
      for (; list1; list1 = CB_CHAIN(list1))
      {
         struct cb_entry *entry1 = CB_ENTRY(CB_VALUE(list1));
         struct cb_label *label = CB_LABEL(entry1->label);
         cb_tree list2  = entry1->using_list;
         char *s = strdup((char *)p->program_id);
         cob_strupper(s);
         fprintf(f, "<ENTRY name=\"%s\" return=\"int\">\n", label->name);
         free(s);
         if (!prog->flag_chained)
         {
            for (; list2; list2 = CB_CHAIN(list2))
            {
               struct cb_field *field = cb_field(CB_VALUE(list2));
               fprintf(f, "<PARAM name=\"%s\" byvalue=\"%s\">\n", field->name, CB_CALL_BY(CB_PURPOSE_INT(list2)) == CB_CALL_BY_VALUE ? "TRUE" : "FALSE");
               output_linkage_desc_field(f, field);
               fprintf(f, "</PARAM>\n");
            }
         }
         fprintf(f, "</ENTRY>\n");
      }
      fprintf(f, "</PROGRAM>\n");
   }
   fclose(f);
   return 0;
}

char* cob_enterprise_get_ccmap_name(char *module)
{
   static char buffer[COB_SMALL_BUFF];

   sprintf(buffer, "%s%d", module, getpid());
   return buffer;
}

static int              ccmid = 0;
static int              ccm_statementcnt = 0;
static char             *ccprogid = NULL;

void output_ccmap_init(struct cb_program *prog)
{
   ccmid = 0;
   ccm_statementcnt = 0;
   if (cb_ccmap_file)
   {
      fprintf(cb_ccmap_file, "/* Generated from %s by cobc %s.%d (Build date - %s %s) */\n\n",
              cb_main_source_file, PACKAGE_VERSION, PATCH_LEVEL,
              __DATE__, __TIME__);
      ccprogid = (char *)prog->program_id;
   }

}
void output_ccmap_finish(struct cb_program *prog)
{
   if (cb_ccmap_file)
   {
      ccmid++;
      fprintf(cb_ccmap_file, "static int %s_ccmap_data[%d] = {0};\n", ccprogid, ccmid);

   }

}
void output_ccmap(const unsigned char *source, int line, char type)
{
   if (cb_ccmap_file && source)
   {
      if (cb_debugdb)
      {
         debugdb_add_ccmap_rec(cb_debugdb, cb_debugdb_module_id, ccmid,
                               (char *)source, line, type);
         output_line("%s_ccmap_data[%d]++;", ccprogid, ccmid);
         ccmid++;
      }
   }
}

static int FieldCount = 0;
static void
output_xdd_desc_field(FILE *file, struct cb_field *f, char *prefix, int idx)
{
   char attr[1024];
   int  doout;
   char postfix[30];

   // if (f->redefines) {
   //     return;
   // }

   doout = 1;
   if (!memcmp(f->name, CB_PREFIX_FILLER, CB_PREFIX_FILLER_LEN))
   {
      sprintf(attr, "type=\"Alphanum\" hidden=\"true\" ");
   }
   else
   {
      switch (f->usage)
      {
         case CB_USAGE_BIT:                   /* 1 */
            doout = 0;
            break;
         case CB_USAGE_BYTE_ARRAY:
         case CB_USAGE_UNSIGNED_CHAR:
         case CB_USAGE_UNSIGNED_SHORT:        /* 17 */
         case CB_USAGE_UNSIGNED_INT:          /* 19 */
         case CB_USAGE_UNSIGNED_LONG:         /* 21 */
         case CB_USAGE_LENGTH:                /* 13 */
         case CB_USAGE_INDEX:                 /* 7 */
            sprintf(attr, "type=\"NativeUnsigned\" ");
            break;
         case CB_USAGE_SIGNED_CHAR:           /* 16 */
         case CB_USAGE_SIGNED_SHORT:          /* 18 */
         case CB_USAGE_SIGNED_INT:            /* 20 */
         case CB_USAGE_SIGNED_LONG:           /* 22 */
            sprintf(attr, "type=\"NativeSigned\" ");
            break;
         case CB_USAGE_BINARY:                /* 0 */
         case CB_USAGE_COMP_5:                /* 2 */
         case CB_USAGE_COMP_X:                /* 3 */
            if (f->pic)
            {
               if (f->pic->have_sign)
               {
                  if (f->flag_binary_swap)
                  {
                     sprintf(attr, "type=\"BinarySigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                  }
                  else
                  {
                     sprintf(attr, "type=\"NativeSigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                  }
               }
               else
               {
                  if (f->flag_binary_swap)
                  {
                     sprintf(attr, "type=\"BinaryUnsigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                  }
                  else
                  {
                     sprintf(attr, "type=\"NativeUnsigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                  }
               }

            }
            else
            {
               if (f->flag_binary_swap)
               {
                  sprintf(attr, "type=\"BinaryUnsigned\" ");
               }
               else
               {
                  sprintf(attr, "type=\"NativeUnsigned\" ");
               }
            }
            break;

         case CB_USAGE_DISPLAY:               /* 4 */
            if (f->pic && f->pic->category ==  CB_CATEGORY_NUMERIC)
            {
               if (f->pic->have_sign)
               {
                  if (f->flag_sign_leading)
                  {
                     if (f->flag_sign_separate)
                     {
                        sprintf(attr, "type=\"NumSepLead\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                     }
                     else
                     {
                        sprintf(attr, "type=\"NumLeading\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                     }
                  }
                  else
                  {
                     if (f->flag_sign_separate)
                     {
                        sprintf(attr, "type=\"NumSignSep\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                     }
                     else
                     {
                        sprintf(attr, "type=\"NumSigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
                     }
                  }
               }
               else
               {
                  sprintf(attr, "type=\"NumUnsigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
               }
            }
            else
            {
               sprintf(attr, "type=\"Alphanum\" ");
            }
            break;
         case CB_USAGE_FLOAT:                 /* 5 */
         case CB_USAGE_DOUBLE:                /* 6 */
            sprintf(attr, "type=\"Float\" ");
            break;
         case CB_USAGE_NATIONAL:                /* 6 */
            sprintf(attr, "type=\"National\" ");
            break;
         case CB_USAGE_PACKED:                /* 10 */
            if (f->pic->have_sign)
            {
               sprintf(attr, "type=\"PackedSigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
            }
            else
            {
               sprintf(attr, "type=\"PackedPositive\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
            }
            break;
         case CB_USAGE_COMP_6:                /* 23 */
            sprintf(attr, "type=\"PackedUnsigned\" digits=\"%d\" scale=\"%d\" ", f->pic->digits, f->pic->scale);
            break;
         default:
            sprintf(attr, "type=\"Alphanum\" hidden=\"true\" ");
            break;
      }
   }
   if (doout)
   {
      if (cb_flag_ctree_no_full_field_name)
      {
         if (cb_flag_ctree_field_numbering)
         {
            fprintf(file, "<field name=\"F%d_%s\" offset=\"%d\" size=\"%d\" %s />\n",
                    ++FieldCount, cb_to_cname(f->name, NULL), f->offset, f->size, attr);
         }
         else
         {
            fprintf(file, "<field name=\"%s\" offset=\"%d\" size=\"%d\" %s />\n",
                    cb_to_cname(f->name, NULL), f->offset, f->size, attr);
         }
      }
      else
      {
         if (f->occurs_max > 1)
         {
            sprintf(postfix, "_%d", idx);
         }
         else
         {
            postfix[0] = 0;
         }
         if (cb_flag_ctree_field_numbering)
         {
            fprintf(file, "<field name=\"F%d_%s%s%s\" offset=\"%d\" size=\"%d\" %s />\n",
                    ++FieldCount, prefix, cb_to_cname(f->name, NULL), postfix, f->offset, f->size, attr);
         }
         else
         {
            fprintf(file, "<field name=\"%s%s%s\" offset=\"%d\" size=\"%d\" %s />\n",
                    prefix, cb_to_cname(f->name, NULL), postfix, f->offset, f->size, attr);
         }
      }
   }
}

static void
output_xdd_desc_record(FILE *file, struct cb_field *f, char *prefix)
{
   char   name[COB_MEDIUM_BUFF];
   char   name2[COB_MEDIUM_BUFF];
   char   a[20];
   int    i;

   for (; f; f = f->sister)
   {
      if (!f->redefines)
      {
         if (f->children)
         {
            strncpy(name, prefix, sizeof(name));
            if (!cb_flag_ctree_no_full_field_name || f->occurs_max > 1)
            {
               strncat(name, cb_to_cname(f->name, NULL), sizeof(name));
               strncat(name, "_", sizeof(name));
            }
            if (f->occurs_max > 1)
            {
               for (i = 1; i <= f->occurs_max; i++)
               {
                  strcpy(name2, name);
                  sprintf(a, "_%d_", i);
                  strcat(name2, a);
                  output_xdd_desc_record(file, f->children, name2);
               }
            }
            else
            {
               output_xdd_desc_record(file, f->children, name);
            }
         }
         else
         {
            for (i = 1; i <= f->occurs_max; i++)
            {
               output_xdd_desc_field(file, f, prefix, i);
            }
         }
      }
   }
}

static void
output_xdd_key_field(FILE *o, struct cb_field *f, const char *prefix)
{
   struct cb_field *c;
   char   name[COB_MEDIUM_BUFF];
   strncpy(name, prefix, sizeof(name));
   strncat(name, cb_to_cname(f->name, NULL), sizeof(name));
   if (f->children)
   {
      fprintf(o, "<part name=\"%s\" offset=\"%d\" size=\"%d\" >\n",
              name, f->offset, f->size);
      for (c = f->children; c; c = c->sister)
      {
         if (!c->redefines)
         {
            output_xdd_key_field(o, c, prefix);
         }
      }
      fprintf(o, "</part>\n");

   }
   else
   {
      fprintf(o, "<part name=\"%s\" offset=\"%d\" size=\"%d\" />\n",
              name, f->offset, f->size);
   }
}

static void
output_xdd_key_field_list(FILE *of, cb_tree x)
{

   cb_tree l;
   struct cb_object_list *o;

   o = CB_OBJECT_LIST(x);
   l = o->olist;
   for (; l; l = CB_CHAIN(l))
   {
      if (CB_REF_OR_FIELD_P(CB_VALUE(l)))
      {
         struct cb_field *f = cb_field(CB_VALUE(l));
         output_xdd_key_field(of, f, "");
      }
   }
}

static void
output_xdd_key(FILE *o, cb_tree x, int duplicate, int primary)
{
   if (x)
   {
      fprintf(o, "<key duplicate=\"%s\" primary=\"%s\" >\n", duplicate ? "true" : "false",
              primary ? "true" : "false");
      if (CB_OBJECT_LIST_P(x) && (CB_OBJECT_LIST(x)->type == CB_OBJECT_LIST_FIELD))
      {
         output_xdd_key_field_list(o, x);
      }
      else
      {
         struct cb_field *f = cb_field(x);
         output_xdd_key_field(o, f, "");
      }
      fprintf(o, "</key>\n");
   }
}

static int get_organization_to_FCD(struct cb_file *f)
{
   switch (f->organization)
   {
      case COB_ORG_LINE_SEQUENTIAL:
         return 0x00;
      case COB_ORG_SEQUENTIAL     :
         return 0x01;
      case COB_ORG_INDEXED        :
         return 0x02;
      case COB_ORG_RELATIVE       :
         return 0x03;
   }
   return 1;
}

static const char* xdd_get_org(struct cb_file      *f)
{
   switch (f->organization)
   {
      case COB_ORG_LINE_SEQUENTIAL :
         return "line-sequential";
         break;
      case COB_ORG_RELATIVE        :
         return "relative";
         break;
      case COB_ORG_INDEXED         :
         return "indexed";
         break;
      case COB_ORG_SORT            :
         return "sort";
         break;
      case COB_ORG_SEQUENTIAL      :
      default:
         return "sequential";
         break;
   }
}

static void
output_xdd_file(FILE *o, struct cb_file      *f, int FullInfo)
{
   struct cb_alt_key *l;
   struct cb_field   *r;
   int i;

   fprintf(o, "<table name=\"%s\" maxRecLen=\"%d\" minRecLen=\"%d\" >\n",
           cb_to_cname(f->oname, NULL), f->record_max, f->record_min);
   output_xdd_key(o, f->key, 0, 1);
   for (l = f->alt_key_list; l; l = l->next)
   {
      output_xdd_key(o, l->key, l->duplicates, 0);
   }
   if (FullInfo)
   {
      fprintf(o, "<select organization=\"%d\" lsmf=\"%d\" varrec=\"%d\" >\n",
              get_organization_to_FCD(f), f->ls_mfmode ? 1 : 0, (f->rec_mode == CB_REC_MODE_VARIABLE  ? 1 : 0));
      fprintf(o, "%s\n</select>\n", xdd_get_org(f));
   }
   if (f->record)
   {
      FieldCount = 0;
      for (r = f->record->sister; r; r = r->sister)
      {
         fprintf(o, "<schema name=\"%s\" size=\"%d\" >\n", cb_to_cname(r->name, NULL), r->size);
         if (!r->children)
         {
            for (i = 1; i <= r->occurs_max; i++)
            {
               output_xdd_desc_field(o, r, (char *)"", i);
            }
         }
         else
         {
            output_xdd_desc_record(o, r->children, (char *)"");
         }
         fprintf(o, "</schema>\n");
      }
   }
   fprintf(o, "</table>\n");
}

int
cob_enterprise_gen_xdd_descriptor(struct cb_program *prog)
{

   FILE                *f;
   struct cb_program   *p;
   cb_tree             l;
   struct cb_file      *fl;
   char                s[1024];

   for (p = prog; p; p = p->next_program)
   {
      for (l = p->file_list; l; l = CB_CHAIN(l))
      {
         fl = CB_FILE(CB_VALUE(l));
         if (cb_xdd_prefix) strcpy(s, cb_xdd_prefix);
         else s[0] = 0;

         strcat(s, cb_to_cname(fl->oname, NULL));
         strcat(s, ".xdd");
         f = fopen(s, "w");
         if (!f)
         {
            cb_error("cobc:0: can't create xdd descriptor file %s\n", s);
            return -1;
         }
         fprintf(f, "<?xml version=\"1.0\" encoding=\"US-ASCII\"?>\n");
         output_xdd_file(f, fl, 1);

         fclose(f);
      }
   }
   return 0;
}

void
cob_enterprise_output_xdd_file(FILE *o, struct cb_file      *f)
{

   char *fname = cobc_temp_name("cob", ".cob");
   FILE *tf;
   int c;

   tf = fopen(fname, "w");
   if (!tf)
   {
      cb_error("cobc:0: can't create xdd descriptor file %s\n", fname);
      return;
   }
   output_xdd_file(tf, f, 0);
   fclose(tf);
   tf = fopen(fname, "r");
   if (!tf)
   {
      cb_error("cobc:0: can't reopen xdd descriptor file %s\n", fname);
      return;
   }
   while (!feof(tf))
   {
      c = fgetc(tf);
      if (c > 0)
      {
         if (c == '"' || c == '\\')
         {
            fputc('\\', o);
            fputc(c, o);
         }
         else if (c == '\n')
         {
            fputs("\\n\"\n", o);
            fputs("\"", o);
         }
         else
         {
            fputc(c, o);
         }
      }
   }
   fclose(tf);
   unlink(fname);
   free(fname);
}

static void
field_memmap(FILE *outfile, struct cb_field *f, int level)
{

   char buffer[200];
   char buffer2[200] = { 0 };
   char buffer3[20] = { 0 };
   char buffer4[20] = { 0 };
   char buffer5[20] = { 0 };
   memset(buffer, 0, sizeof(buffer));
   memset(buffer, ' ', 2 * level);
   if (f->redefines)
   {
      sprintf(buffer2, " : REDEFINES : %s", f->redefines->name);
   }
   strcpy(buffer3, "------");
   if ((f->level > 1) && (f->level != 77))
   {
      sprintf(buffer3, "%6d", f->offset);
   }
   strcpy(buffer4, "      ");
   if (f->occurs_min > 0)
   {
      sprintf(buffer4, "%6d", f->occurs_min);
   }
   strcpy(buffer5, "      ");
   if (f->occurs_max > 1)
   {
      sprintf(buffer5, "%6d", f->occurs_max);
   }
   fprintf(outfile, "******* %6s : %6d : %6s - %6s : %s%s%s\n",
           buffer3, f->size, buffer4, buffer5, buffer, f->name, buffer2);
}

static void
storage_list_map(FILE *outfile, struct cb_field *f, int level)
{
   for (; f; f = f->sister)
   {
      if (!f->flag_hide_from_list || (f->count > 0))
      {
         field_memmap(outfile, f, level);
         if (f->children)
         {
            storage_list_map(outfile, f->children, level + 1);
         }
      }
   }
}

static void
do_storage_list(FILE *outfile, struct cb_field *f, const char *name)
{
   while (f && f->flag_hide_from_list && (f->count == 0))
   {
      f = f->sister;
   }
   if (f)
   {
      fprintf(outfile, "***********************************************************************\n");
      fprintf(outfile, "***********************************************************************\n");
      fprintf(outfile, "******* DATA MAP : %s\n", name);
      fprintf(outfile, "***********************************************************************\n");
      fprintf(outfile, "******* %6s : %6s : %6s - %6s : Name\n", "Offset", "Size", "Min", "Max");
      storage_list_map(outfile, f, 0);
   }
}

int
cob_enterprise_gen_list_info(FILE *outfile, struct cb_program *prog)
{

   cb_tree l;

   do_storage_list(outfile, prog->working_storage, "WORKING STORAGE");
   do_storage_list(outfile, prog->linkage_storage, "LINKAGE");
   do_storage_list(outfile, prog->local_storage,   "LOCAL STORAGE");
   for (l = prog->file_list; l; l = CB_CHAIN(l))
   {
      do_storage_list(outfile, CB_FILE(CB_VALUE(l))->record, CB_FILE(CB_VALUE(l))->name);
   }
   return 0;
}


static void
fieldname_list(FILE *outfile, struct cb_field *f, char *mainsrc)
{

   for (; f; f = f->sister)
   {
      if ((strncmp(f->name, "FILLER$", 7) != 0))
      {
         fprintf(outfile, "%s | %d | %s \n", f->name, f->common.source_line,
                 f->common.source_file ? (char *)f->common.source_file : mainsrc);
      }
      if (f->children)
      {
         fieldname_list(outfile, f->children, mainsrc);
      }
      if (f->level88_children)
      {
         fieldname_list(outfile, f->level88_children, mainsrc);
      }
   }
}

int
cob_enterprise_gen_fieldname_list(char *filename, struct cb_program *prog)
{
   FILE *outfile;
   char buff[1024];
   char *s;
   cb_tree l;
   cb_tree x;
   struct cb_file  *f;
   struct cb_field *r;
   cb_tree p;

   outfile = fopen(filename, "a");
   if (outfile)
   {
      for (l = prog->file_rec_list; l; l = CB_CHAIN(l))
      {
         p = CB_VALUE(l);
         if (p)
         {
            f = CB_FILE(CB_PAIR_X(p));
            r = CB_FIELD(CB_PAIR_Y(p));
            /*r = r->sister;  skip globa record*/
            if (r)
            {
               fieldname_list(outfile, r, prog->source_file);
            }
         }
      }
      fieldname_list(outfile, prog->working_storage, prog->source_file);
      fieldname_list(outfile, prog->linkage_storage, prog->source_file);
      fieldname_list(outfile, prog->local_storage,   prog->source_file);
      for (l = cb_list_reverse(prog->all_label_list); l; l = CB_CHAIN(l))
      {
         x = CB_VALUE(l);
         if (CB_LABEL_P(x))
         {
            s = cb_name(x);
            if (strncmp(s, "L$", 2) != 0)
            {
               fprintf(outfile, "%s | %d | %s\n", s, x->source_line, x->source_file ?
                       (char *)x->source_file : prog->source_file);
            }
         }
      }
      fclose(outfile);
   }
   else
   {
      sprintf(buff, "Can't create file %s ", filename);
      cobc_terminate(buff);
   }
   return 0;
}


static cb_tree
cb_duplicate_reference(cb_tree r)
{
   struct cb_reference *od;
   od = cobc_malloc(sizeof(struct cb_reference));
   *od = *(CB_REFERENCE(r));
   od->value = NULL;
   if (od->chain && CB_REFERENCE_P(od->chain))
   {
      od->chain = cb_duplicate_reference(od->chain);

   }
   return  CB_TREE(od);
}

static struct cb_field*
cb_find_typedef_id(struct cb_field *f, int typedef_id)
{
   struct cb_field *r;

   while (f)
   {
      if (f->typedef_id == typedef_id)
      {
         return f;
      }
      if (f->children)
      {
         r = cb_find_typedef_id(f->children, typedef_id);
         if (r)
         {
            return r;
         }
      }
      f = f->sister;
   }

   return NULL;
}

static void
cb_duplicate_field_fix_redefine(struct cb_field *f, struct cb_field *base_parent)
{
   int redef_id;
   while (f)
   {
      if (f->redefines)
      {
         redef_id = f->redefines->id;
         f->redefines = cb_find_typedef_id(base_parent, redef_id);
         if (f->redefines == NULL)
         {
            cb_error("cobc:0: TYPEDEF/REDEFINE error : %s (%d)\n", f->name, redef_id);
         }
      }
      if (f->children)
      {
         cb_duplicate_field_fix_redefine(f->children, base_parent);
      }
      if (f->level88_children)
      {
         cb_duplicate_field_fix_redefine(f->level88_children, base_parent);
      }
      
      f = f->sister;
   }


}

static struct cb_field*
cb_duplicate_field_tree(struct cb_field *f, struct cb_field *newparent, int baselevel,
                        struct cb_field *NewTopParent,
                        struct cb_field *OldTopParent)
{
   struct cb_field *r;
   struct cb_field save;
   struct cb_reference *od;

   r = CB_FIELD(cb_build_field(cb_build_reference(f->name)));
   save = *r;
   *r = *f;
   r->common       = save.common;
   r->typedef_id   = f->id;
   r->id           = save.id;
   r->storage_id   = newparent->storage_id;
   r->name         = save.name;
   r->ename        = save.ename;
   if (f->level != 88) 
      r->level     = baselevel + f->level - 1;
   else
      r->level     = f->level;
   r->count        = 0;
   r->storage      = newparent->storage;
   r->parent       = newparent;
   r->indexes      = r->indexes + newparent->indexes;
   r->sister       = NULL;
   r->flag_is_verified = 0;
   r->flag_is_typedef = 0;
   if (f->occurs_depending && CB_REFERENCE_P(f->occurs_depending))
   {
      r->occurs_depending = cb_duplicate_reference(f->occurs_depending);
      od = CB_REFERENCE(r->occurs_depending);
      while (od->chain && CB_REFERENCE(od->chain)->chain)
      {
         od = CB_REFERENCE(od->chain);
      }
      if (od->chain && (strcasecmp(CB_NAME(od->chain), OldTopParent->name) == 0))
      {
         od->chain = cb_build_reference(NewTopParent->name);
      }
   }

   if (f->children)
   {
      r->children = cb_duplicate_field_tree(f->children, r, baselevel, NewTopParent, OldTopParent);
   }
   if (f->level88_children)
   {
      r->level88_children = cb_duplicate_field_tree(f->level88_children, r, baselevel, NewTopParent, OldTopParent);
   }
   if (f->sister)
   {
      r->sister = cb_duplicate_field_tree(f->sister, newparent, baselevel, NewTopParent, OldTopParent);
   }
   return r;
}

void
cob_enterprise_map_field_from_type(struct cb_field *f, struct cb_field *type)
{

   if (f->occurs_max == 1)
   {
      f->occurs_min = type->occurs_min;
      f->occurs_max = type->occurs_max;
      f->flag_occurs = type->flag_occurs;
   }
   f->usage    = type->usage;
   f->values   = type->values;
   f->false_88 = type->false_88;
   f->index_list = type->index_list;
   if (f->pic)
   {
      cb_error(_("PICTURE not usable with user defined type"));
   }
   f->pic = type->pic;
   if (type->flag_external)
   {
      f->flag_external       = type->flag_external;
   }
   f->flag_blank_zero     = type->flag_blank_zero;
   f->flag_justified      = type->flag_justified;
   f->flag_sign_leading   = type->flag_sign_leading;
   f->flag_sign_separate  = type->flag_sign_separate;
   f->flag_synchronized   = type->flag_synchronized;
   f->flag_invalid        = type->flag_invalid;
   f->flag_binary_swap    = type->flag_binary_swap;
   f->flag_chained        = type->flag_chained;
   f->flag_real_binary    = type->flag_real_binary;
   f->flag_item_based     = type->flag_item_based;
   f->flag_item_78        = type->flag_item_78;
   f->flag_any_length     = type->flag_any_length;
   f->flag_anylen_done    = type->flag_anylen_done;
   f->flag_indexed_by     = type->flag_indexed_by;
   f->flag_is_pointer     = type->flag_is_pointer;
   f->flag_is_c_long      = type->flag_is_c_long;
   f->flag_is_pdiv_parm   = type->flag_is_pdiv_parm;
   f->flag_binary_pack    = type->flag_binary_pack;
   f->flag_is_verified    = 0;
   f->flag_is_typedef     = 0;
   f->screen_line         = type->screen_line;
   f->screen_column       = type->screen_column;
   f->screen_from         = type->screen_from;
   f->screen_to           = type->screen_to;
   f->screen_foreg        = type->screen_foreg;
   f->screen_backg        = type->screen_backg;
   f->screen_flag         = type->screen_flag;
   f->screen_prompt       = type->screen_prompt;
   f->screen_control      = type->screen_control;
   f->screen_input_size   = type->screen_input_size;



   if (type->children)
   {
      f->children = cb_duplicate_field_tree(type->children, f, f->level, f, type);
      cb_duplicate_field_fix_redefine(f->children, f);
   }
   if (type->level88_children)
   {
      f->level88_children = cb_duplicate_field_tree(type->level88_children, f, f->level, f, type);
      cb_duplicate_field_fix_redefine(f->level88_children, f);
   }

   if (type->bitfield_children)
   {
      f->bitfield_children = cb_duplicate_field_tree(type->bitfield_children, f->parent, f->level, f, type);
   }
}


static cb_tree
cob_enterprise_get_call_shadow(char *entryName, char *fieldName, struct cb_field *f)
{

   cb_tree             l;
   struct cb_field *p;
   char                buff[100];


   sprintf(buff, "%s$%s", entryName, fieldName);
   l = cb_build_reference(buff);
   for (p = current_program->working_storage; p; p = p->sister)
   {
      if (strcmp(p->name, buff) == 0)
      {
         CB_REFERENCE(l)->value = CB_TREE(p);
         return l;
      }
   }
   p = CB_FIELD(cb_build_field(l));
   cob_enterprise_map_field_from_type(p, f);
   cb_validate_field(p);
   p->count++;
   p->flag_no_init = 1;
   current_program->working_storage = cb_field_add(current_program->working_storage, p);
   return l;
}


void cob_enterprise_output_profiling(const char *fmt, ...)
{
   va_list ap;
   if (cb_profiling_file)
   {
      va_start(ap, fmt);
      vfprintf(cb_profiling_file, fmt, ap);
      va_end(ap);
   }
}

void
cob_enterprise_output_profiling_tick(void)
{
   output_line("cob_get_elapsed_tick(rtd, module);");
   output_line("%s_%s[%d].tick_cnt += module->profiling_delta.cpu_tick;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
   output_line("%s_%s[%d].tick_cnt_elaps += module->profiling_delta.elaps_tick;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
}

void cob_enterprise_output_call_header(void)
{
   if (cb_flag_context_reload_enable)
   {
      output_line("goto tmp_%s%d;", CB_PREFIX_LABEL, cb_id);
      output_line("%s%d:", CB_PREFIX_LABEL, cb_id);
      output_line("frame_ptr--;");
      output_line("tmp_%s%d:", CB_PREFIX_LABEL, cb_id);
   }
}

void cob_enterprise_output_call_footer(void)
{
   if (cb_flag_profiling)
   {
      output_indent("{");
      output_line("cob_get_elapsed_tick(rtd, module);");
      output_line("%s_%s[%d].tick_cnt += module->profiling_delta.cpu_tick;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
      output_line("%s_%s[%d].external_call_tick += module->profiling_delta.cpu_tick;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
      output_line("%s_%s[%d].tick_cnt_elaps += module->profiling_delta.elaps_tick;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
      output_line("%s_%s[%d].external_call_tick_elaps += module->profiling_delta.elaps_tick;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
      output_indent("}");
   }

   if (cb_flag_context_reload_enable)
   {
      output_line("if (rtd->cob_context_info.cob_context_mode & COB_CONTEXT_SAVE)");
      output_indent("{");
      output_line("frame_ptr++;");
      output_perform_label(CB_LABEL(cb_save_context_handler), CB_LABEL(cb_save_context_handler));

      output_indent("}");
      cb_id++;
   }
}

void
cob_enterprise_output_key_field_list_allocation(struct cb_program *prog, struct cb_file *f, cb_tree x)
{
   cb_tree l;
   int sz = 0;
   int ct = 0;
   struct cb_object_list *o;

   o = CB_OBJECT_LIST(x);
   l = o->olist;
   for (; l; l = CB_CHAIN(l))
   {
      if (CB_REF_OR_FIELD_P(CB_VALUE(l)))
      {
         struct cb_field *f = cb_field(CB_VALUE(l));
         ct++;
         sz += f->size;
      }
   }
   if (ct > COB_MAX_VBISAM_KEY_PART &&
         !(cb_isam_extfh || cb_flag_all_extfh || cb_flag_use_ctree ||  cb_flag_use_disam || cb_flag_use_vbisam)) 
   {
      fprintf(stderr, "cobc:0: Too many key part (> %d) in split key fo file %s\n", COB_MAX_VBISAM_KEY_PART, f->cname);
      ABORT();

   }
   output_line("unsigned char        i%s%s[%d];",
               CB_PREFIX_KEYS, o->cname, sz);
   output_line("cob_keypart          t%s%s[%d];",
               CB_PREFIX_KEYS, o->cname, ct);
   output_line("cob_field_attr_flist a%s%s;",
               CB_PREFIX_KEYS, o->cname);
   output_line("cob_field_list       fl_%s%s;",
               CB_PREFIX_KEYS, o->cname);

   if (f->global)
   {
      output_global("#define %s%s (COB_MDS fl_%s%s)\n",
                    CB_PREFIX_FIELDLIST, o->cname,
                    CB_PREFIX_KEYS, o->cname);
      output_global("#define b%s%s (COB_MDS i%s%s)\n",
                    CB_PREFIX_FIELDLIST, o->cname,
                    CB_PREFIX_KEYS, o->cname);
      output_global("#define t_%s%s (COB_MDS t%s%s)\n",
                    CB_PREFIX_FIELDLIST, o->cname,
                    CB_PREFIX_KEYS, o->cname);
      output_global("#define a_%s%s (COB_MDS a%s%s)\n",
                    CB_PREFIX_FIELDLIST, o->cname,
                    CB_PREFIX_KEYS, o->cname);

   }
   else
   {
      output_local("#undef %s%s\n", CB_PREFIX_FIELDLIST, o->cname);
      output_local("#define %s%s (COB_MDS %s%s.fl_%s%s)\n",
                   CB_PREFIX_FIELDLIST, o->cname,
                   CB_PREFIX_STATIC, prog->program_id,
                   CB_PREFIX_KEYS, o->cname);
      output_local("#undef b%s%s\n", CB_PREFIX_FIELDLIST, o->cname);
      output_local("#define b%s%s (COB_MDS %s%s.i%s%s)\n",
                   CB_PREFIX_FIELDLIST, o->cname,
                   CB_PREFIX_STATIC, prog->program_id,
                   CB_PREFIX_KEYS, o->cname);
      output_local("#undef t_%s%s\n", CB_PREFIX_FIELDLIST, o->cname);
      output_local("#define t_%s%s (COB_MDS %s%s.t%s%s)\n",
                   CB_PREFIX_FIELDLIST, o->cname,
                   CB_PREFIX_STATIC, prog->program_id,
                   CB_PREFIX_KEYS, o->cname);
      output_local("#undef a_%s%s\n", CB_PREFIX_FIELDLIST, o->cname);
      output_local("#define a_%s%s (COB_MDS %s%s.a%s%s)\n",
                   CB_PREFIX_FIELDLIST, o->cname,
                   CB_PREFIX_STATIC, prog->program_id,
                   CB_PREFIX_KEYS, o->cname);
   }

}

void cob_enterprise_output_rtd_init_header(void)
{
   if (cb_flag_region0)
   {
      output_line("cit_runtime_t * cur_rtd = cob_get_rtd();");
      output_line("int cur_region = cob_enterprise_get_current_region(cur_rtd);");
      output_line("cit_runtime_t * rtd = cob_enterprise_set_current_region(cur_rtd, 0);");
   }
   else
   {
      output_line("COB_RTD = cob_get_rtd();");
   }
   if (cb_flag_thread_safe)
   {
      output_line("register mds_t * const mds = cob_get_module_storage(rtd, mds_key, sizeof(mds_t));");
   }
}

void cob_enterprise_output_rtd_exit_footer(void)
{
   if (cb_flag_region0)
   {
      output_line("rtd = cob_enterprise_set_current_region(rtd, cur_region);");
   }
}

void cob_enterprise_output_internal_function_init(struct cb_program *prog)
{
   if (cb_flag_profiling)
   {
      output_line("module->profiling_info           = %s_%s;", prog->program_id, PROFILING_SUFFIX);
   }
   if (cb_file_status_map)
   {
      output_line("module->file_status_map         = &file_status_map;");
   }
   if (cb_crt_status_map)
   {
      output_line("module->crt_status_map          = &crt_status_map;");
   }
}

void
cob_enterprise_map_call_to_entry(cb_tree prog, int *call_conv, cb_tree using_list, cb_tree returning)
{
   struct cb_program  *p;
   cb_tree             l;
   cb_tree             le;
   struct cb_entry    *e = NULL;
   int                 cnt = 0;
   char                *label = (char *)"";

   if (CB_LITERAL_P(prog))
   {
      for (p = external_program_list; p && !e; p = p->next_program)
      {
         for (l = p->entry_list; l && !e; l = CB_CHAIN(l))
         {
            if (strcasecmp((char *)CB_LITERAL(prog)->data, (char *)CB_LABEL(CB_ENTRY(CB_VALUE(l))->label)->name) == 0)
            {
               e = CB_ENTRY(CB_VALUE(l));
            }
         }
      }
   }
   if (e)
   {
      if (returning && e->returning)
      {
         CB_INTEGER(CB_PAIR_X(returning))->val = CB_INTEGER(CB_PAIR_X(e->returning))->val;
      }
      if (call_conv && *call_conv != e->call_conv)
      {
         cb_error(_("External ENTRY '%s' invalid call convension"), label);
      }
      *call_conv = e->call_conv;
      label = (char *)CB_LABEL(e->label)->name;
      if (e->using_list)
      {
         cnt = cb_list_length(e->using_list);
      }
      if ((e->using_list && !using_list) ||
          (!e->using_list && using_list))
      {
         goto ParamErrorCnt;
      }
      if (!e->using_list || !using_list)
      {
         return;
      }

      if (cnt != cb_list_length(using_list))
      {
         goto ParamErrorCnt;
      }
      l = using_list;
      le = e->using_list;
      while (l && le)
      {
         cb_tree x  = CB_VALUE(l);
         cb_tree xe = CB_VALUE(le);
         /* printf("%s %d %d, %s %d %d\n", cb_name(x), CB_PURPOSE_INT (l), CB_SIZES (l),
                                        cb_name(xe), CB_PURPOSE_INT (le), CB_SIZES (le));*/
         if (x && xe)
         {
            if ((CB_CALL_BY(CB_PURPOSE_INT(l)) != CB_CALL_BY_DEFAULT) &&
                (CB_CALL_BY(CB_PURPOSE_INT(l)) !=  CB_PURPOSE_INT(le)))
            {
               cb_error(_("External ENTRY '%s', BY clause of %s incompatible with prototype "), label, cb_name(x));
            }
            if ((xe != cb_null) && (CB_TREE_CLASS(x) != CB_TREE_CLASS(xe)))
            {
               cb_error(_("External ENTRY '%s', parameter %s not compatible with prototype"), label, cb_name(x));
               return;
            }
            CB_PURPOSE(l) = CB_PURPOSE(le);
            CB_SIZES(l) = CB_SIZES(le);
            if (xe != cb_null)
            {
               if (CB_CALL_BY(CB_PURPOSE_INT(l)) == CB_CALL_BY_VALUE)
               {
                  CB_PURPOSE(l) = cb_int(CB_CALL_BY_VALUE | CB_CALL_BY_PROTOTYPED);
                  switch (cb_type(xe)->size)
                  {
                     case 0:
                        CB_SIZES(l) = CB_SIZE_4; break;
                     case 1:
                        CB_SIZES(l) = CB_SIZE_1; break;
                     case 2:
                        CB_SIZES(l) = CB_SIZE_2; break;
                     case 3:
                     case 4:
                        CB_SIZES(l) = CB_SIZE_4; break;
                     default:
                        CB_SIZES(l) = CB_SIZE_8; break;
                  }
               }
               else
               {
                  cb_tree r = NULL;
                  if (CB_TREE_CLASS(xe) == CB_CLASS_NUMERIC)
                  {
                     r = cob_enterprise_get_call_shadow(label, cb_name(xe), cb_type(xe));
                     current_statement->before_body = cb_list_add(current_statement->before_body, cb_build_move(x, r));
                     CB_VALUE(l) = r;
                  }
                  else if (!CB_LITERAL_P(x) &&
                           CB_TREE_CLASS(xe) == CB_CLASS_ALPHANUMERIC &&
                           CB_SIZES(le) & CB_SIZE_DELIMITED)
                  {
                     CB_VALUE(l) = cb_build_cstring_intrinsic(x);
                  }
                  if (r &&
                      (CB_CALL_BY(CB_PURPOSE_INT(l)) == CB_CALL_BY_DEFAULT || CB_CALL_BY(CB_PURPOSE_INT(l)) == CB_CALL_BY_REFERENCE) &&
                      CB_REFERENCE_P(x))
                  {
                     current_statement->after_body = cb_list_add(current_statement->after_body, cb_build_move(r, x));
                  }
               }
            }
         }
         l = CB_CHAIN(l);
         le = CB_CHAIN(le);
      }
   }

   return;
 ParamErrorCnt:
   cb_error(_("External ENTRY '%s' require %d parameters"), label, cnt);
   return;
}



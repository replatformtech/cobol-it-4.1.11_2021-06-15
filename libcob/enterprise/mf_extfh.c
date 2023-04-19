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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>


#include <ctype.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "mf_extfh.h"
#include "a2e.h"
#include "common.h"
#include "fileio.h"
#include "cit_extsm.c"


/* Lock Mode*/
#define MFEXCLLOCK  0x01    /* Exclusive isam file lock */
#define MFAUTOLOCK  0x02    /* Automatic row lock */
#define MFMANULOCK  0x04    /* Manual row lock */
#define MFLOCKMULTIPLE 0x80


#define KEY_DEF_BLOCK_SIZE 512



/* Global not in COBRTD because init once for all thread*/
static int global_ctree_init = 0;
static int global_use_ctree = 0;
static int global_no_ctree_map = 0;
static char *global_ctree_dbname = (char *)"ctreeSQL";
static char *global_ctree_passwd = (char *)"ADMIN";
#ifdef _WIN32
static char *global_ctree_libname = NULL;
#else
static char *global_ctree_libname = NULL;
#endif
static int (*CTEXTFH)(cit_uint8 *, mf_extfh_FCD  *) = NULL;

//void* extfh_cob_alloc_file_xdd (COB_RTD, char *xdd)
void extfh_cob_alloc_file_xdd(COB_RTD,  void *p, char *xdd)
{
//    mf_extfh_FCD               * p;
   ctree_extfh_FCD_extention   *e;
   int len;
   FLD_VAR
      mf_extfh_FCD                *fcd = (mf_extfh_FCD *)p;
   len = strlen(xdd);
//    p = extfh_cob_alloc_file(rtd);
   if (len > 0)
   {
      /*compute pos of ext to avoid align filler*/
      e = (ctree_extfh_FCD_extention   *)((char *)fcd + sizeof(mf_extfh_FCD));
      CP_FLD4(e->xdd_len, len);
      CP_FLDP(e->xdd_ptr, xdd);
      len = strlen(global_ctree_dbname);
      CP_FLD4(e->dbname_len, len);
      CP_FLDP(e->dbname_ptr, global_ctree_dbname);
      len = strlen(global_ctree_passwd);
      CP_FLD4(e->passwd_len, len);
      CP_FLDP(e->passwd_ptr, global_ctree_passwd);
   }
//    return(void*)p;
}


static int extfh_init_driver(COB_RTD)
{

   mf_extfh_FCD  FCD;
   cit_uint8 opcode[2] = { 0xc3, 0xfe };
   if (global_ctree_init == 0)
   {
      global_ctree_init++;
      memset(&FCD, 0, sizeof(FCD));
      if (global_ctree_libname)
      {
         cob_load_shared_lib(rtd, global_ctree_libname);
      }
      CTEXTFH = cob_resolve_1(rtd, "CTEXTFH");
      CTEXTFH(opcode, &FCD);
   }
   return 0;
}

static int extfh_close_driver(COB_RTD)
{

   mf_extfh_FCD  FCD;
   cit_uint8 opcode[2] = { 0xc3, 0xff };
   if ((global_ctree_init > 0) && CTEXTFH)
   {
      global_ctree_init--;
      if (global_ctree_init == 0)
      {
         memset(&FCD, 0, sizeof(FCD));
         CTEXTFH(opcode, &FCD);
      }
   }
   return 0;
}

typedef union {
    int (*func)(void *, void *);
    void    *func_void;
} unifunc_t;

unifunc_t unifunc_def  = { NULL };
unifunc_t unifunc_idx  = { NULL };
unifunc_t unifunc_flat = { NULL };

/* For external use*/

int cob_extfh_wrapper(cit_uint8 *opcode, mf_extfh_FCD  *FCD);

int
cob_extfh_wrapper(cit_uint8 *opcode, mf_extfh_FCD  *FCD)
{

   if (FCD && FCD->organization == 2)
   {
      if (unifunc_idx.func_void)
      {
         return unifunc_idx.func(opcode, FCD);
      }
   }
   else
   {
      if (unifunc_flat.func_void)
      {
         return unifunc_flat.func(opcode, FCD);
      }
   }
   return unifunc_def.func(opcode, FCD);

}

static
void cob_resolv_default_extfh_names(COB_RTD)
{
   if (rtd->default_extfh_entry)
   {
      unifunc_def.func_void = cob_resolve(rtd, rtd->default_extfh_entry);
      if (unifunc_def.func_void == NULL)
      {
         char name[COB_SMALL_BUFF];
#        ifdef _WIN32
         strcpy(name, rtd->default_extfh_entry);
         strcat(name, "_dll");
#        else
         strcpy(name, "lib");
         strcat(name, rtd->default_extfh_entry);

#        endif // _WIN32

         cob_load_shared_lib(rtd, name);
      }
      unifunc_def.func_void = cob_resolve_1(rtd, rtd->default_extfh_entry);
   }
   if (rtd->default_extfh_indexed_entry)
   {
      unifunc_idx.func_void = cob_resolve_1(rtd, rtd->default_extfh_indexed_entry);
   }
   if (rtd->default_extfh_flat_entry)
   {
      unifunc_flat.func_void = cob_resolve_1(rtd, rtd->default_extfh_flat_entry);
   }

}

void*
cob_get_extfh_func(void)
{
   unifunc_t unifunc_base;

   COB_RTD = cob_get_rtd();

   if (!rtd)
   {
      fprintf(stderr, "CIT-EXTFH rtd == NULL \n");
      return (NULL);
   }
   cob_init(rtd, 0, NULL);

   unifunc_base.func_void = cob_resolve(rtd, "EXTFH");
   unifunc_def.func_void = unifunc_base.func_void;
   cob_resolv_default_extfh_names(rtd);

   if (unifunc_def.func_void == unifunc_base.func_void &&
       !unifunc_idx.func_void && !unifunc_flat.func_void)
   {
      return unifunc_base.func_void;
   }
   if (unifunc_def.func_void)
   {
      return cob_extfh_wrapper;
   }
   else
   {
      return NULL;
   }
}

static int extfh_call_extfh(COB_RTD, cob_file_extfh *f, cit_uint16 opcode)
{
   int r;
   int save = rtd->disable_fstatus_map;
   cit_uint8  fct[2];
   mf_extfh_FCD *fcd;
   union
   {
       int (*func)(void *, void *);
       void    *func_void;
   } unifunc;
   fcd = f->extfh_ptr;

   if (f->ctree || (global_use_ctree &&  GT_FLD1(fcd->organization) == FCD_ORG_INDEXED))
   {
      extfh_init_driver(rtd);
   }
   if (global_use_ctree && CTEXTFH)
   {
      f->extfh_func = (void *)CTEXTFH;
   }

   if (!f->extfh_func)
   {
      register int organization = GT_FLD1(fcd->organization);
      if ((f->file_version >= 3) && f->extfh_name)
      {
         unifunc.func_void = cob_resolve_1(rtd, f->extfh_name);
      }
      else if (organization == FCD_ORG_INDEXED && rtd->default_extfh_indexed_entry)
      {
         unifunc.func_void = cob_resolve_1(rtd, rtd->default_extfh_indexed_entry);
      }
      else if (organization != FCD_ORG_INDEXED && rtd->default_extfh_flat_entry)
      {
         unifunc.func_void = cob_resolve_1(rtd, rtd->default_extfh_flat_entry);
      }
      else if (rtd->default_extfh_entry)
      {
         cob_resolv_default_extfh_names(rtd);
         if (!f->extfh_func) unifunc.func_void = cob_resolve_1(rtd, rtd->default_extfh_entry);
      }
      else
      {
         unifunc.func_void = cob_resolve(rtd, "EXTFH");
         if (unifunc.func_void == NULL)
         {
            rtd->default_extfh_entry = strdup("disamextfh");
            cob_resolv_default_extfh_names(rtd);
            if (!f->extfh_func) unifunc.func_void = cob_resolve_1(rtd, rtd->default_extfh_entry);
         }
      }
      f->extfh_func = unifunc.func;
   }
   rtd->disable_fstatus_map = 1;
   fct[0] = (cit_uint8)((opcode >> 8) & 0xFF);
   fct[1] = (cit_uint8)((opcode & 0xFF));
   r = f->extfh_func(fct, fcd);
   rtd->disable_fstatus_map = save;
   return r;
}


//void extfh_cob_init_fileio (COB_RTD, const struct cob_fileio_funcs *seqfunc,
//                            const struct cob_fileio_funcs *lsqfunc, const struct cob_fileio_funcs *relfunc,
//                            int (*write_opt)())
void extfh_cob_init_fileio(COB_RTD)
{
   char *s;

   s = getenv("COB_USE_CTREE");
   if (s)
   {
      global_use_ctree = 1;
   }

   s = getenv("COB_NO_CTREE_MAP");
   if (s)
   {
      global_no_ctree_map = 1;
   }
   s = getenv("COB_CTREE_DB");
   if (s)
   {
      global_ctree_dbname = strdup(s);
   }
   s = getenv("COB_CTREE_PASSWORD");
   if (s)
   {
      global_ctree_passwd = strdup(s);
   }
   s = getenv("COB_CTEXTFH_LIB");
   if (s)
   {
      global_ctree_libname = strdup(s);
   }


   /* 
     if ( sizeof(mf_extfh_FCD) != 100 ) {
       cob_runtime_error (rtd, "Sizeof (mf_extfh_FCD) != 100 please use compact structure pragma");
       cob_stop_abend(rtd, 1);
   } 
   */
#  ifndef CIT_SBE
   s = getenv("COB_EXTFH");
   if (s)
   {
      rtd->default_extfh_entry = strdup(s);
   }
   s = getenv("COB_EXTFH_INDEXED");
   if (s)
   {
      rtd->default_extfh_indexed_entry = strdup(s);
   }
   s = getenv("COB_EXTFH_FLAT");
   if (s)
   {
      rtd->default_extfh_flat_entry = strdup(s);
   }
   s = getenv("COB_EXTFH_LIB");
   if (s)
   {
      cob_load_shared_lib(rtd, s);
   }
#  endif
}

void extfh_cob_exit_fileio(COB_RTD)
{
   extfh_close_driver(rtd);
   if (rtd->default_extfh_indexed_entry)
   {
      free(rtd->default_extfh_indexed_entry);
      rtd->default_extfh_indexed_entry = NULL;
   }
   if (rtd->default_extfh_entry)
   {
      free(rtd->default_extfh_entry);
      rtd->default_extfh_entry = NULL;
   }
   if (rtd->default_extfh_flat_entry)
   {
      free(rtd->default_extfh_flat_entry);
      rtd->default_extfh_flat_entry = NULL;
   }
}

int extfh_unlock(COB_RTD, cob_file_extfh *f)
{
   mf_extfh_FCD *fcd;
   cit_uint16 opcode;
   CHECK_F_EXTFH_PTR
      opcode = 0xfa0e;
   (void)extfh_call_extfh(rtd, f, opcode);
   return COB_STATUS_00_SUCCESS;
}

int extfh_get_int_status(COB_RTD, cob_file_extfh *f)
{
   int r;
   mf_extfh_FCD *fcd;
   int m, l;
   ASSERT_F_EXTFH_PTR;


   if (f->ctree)
   {
      if (fcd->user_file_status[0] == '9')
      {
         if (fcd->user_file_status[1] == '\x44')
         {
            fcd->user_file_status[0] = '5';
            fcd->user_file_status[0] = CHAR_1;
         }
         if (fcd->user_file_status[1] == '\x41')
         {
            fcd->user_file_status[0] = '6';
            fcd->user_file_status[0] = CHAR_1;
         }
      }
   }
   m = COB_D2I_CHAR_n(fcd->user_file_status[0]);
   if (m < 0 || m > 9)
   {
      return COB_STATUS_9x_EXTFH_ASSIGNED;
   }
   l = COB_D2I_CHAR_n(fcd->user_file_status[1]);
   if (l < 0 || l > 9)
   {
      return COB_STATUS_9x_EXTFH_ASSIGNED;
   }
   r = (m * 10) + l;

   return r;
}

#define RETURN_INT_STATUS       return extfh_get_int_status(rtd, f);

//static void extfh_free_keys (mf_extfh_FCD* fcd)
//{
//    void *v;
//    FLD_VAR
//
//    v = GT_FLDP(fcd->key_def_block_ptr);
//    cob_free(v);
//    CP_FLDP(fcd->key_def_block_ptr, 0);
//}

static void set_relative_record_number_to_fcd(COB_RTD, cob_file_extfh *f, cob_field *key)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   int k;
   register int organization;

   ASSERT_F_EXTFH_PTR;

   organization = GT_FLD1(fcd->organization);
   if (organization == FCD_ORG_RELATIVE)
   {
      if (key)
      {
         k = cob_get_int(rtd, f->relative_rec_number);
         CP_FLD4_8(fcd->rel_rec_number, k);
      }
   }
   else
   {
      CP_FLD4_8(fcd->rel_rec_number, 0);
   }
}
int extfh_open(COB_RTD, cob_file_extfh *f, int mode, int flag, int lock)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16     opcode;


   CHECK_F_EXTFH_PTR;

   lock |= fcd->lock_mode;
   CP_FLD1(fcd->lock_mode, lock);

   switch (mode)
   {
      case COB_OPEN_INPUT:
         opcode = 0x00;
         break;
      case COB_OPEN_INPUT_REVERSED:
         opcode = 0x08;
         break;
      case COB_OPEN_OUTPUT:
         /*lmode = MFEXCLLOCK;*/
         opcode = 0x01;
         break;
      case COB_OPEN_I_O:
         opcode = 0x02;
         break;
      case COB_OPEN_EXTEND:
         /*lmode = MFEXCLLOCK;*/
         opcode = 0x03;
         break;
      case COB_OPEN_LOCKED:
         CP_FLD1(fcd->lock_mode, FCD_LOCK_EXCLUSIVE);
         opcode = 0x01;
         break;
      default:
         cob_runtime_error(rtd, "extfh_indexed_open : invalid open mode %d for %s", mode, f->select_name);
         cob_stop_abend(rtd, COBRE_EXTFH_OP_MODE);
         break;
   }

   if ((global_use_ctree || f->ctree) && !global_no_ctree_map)
   {
      opcode |= 0xC300;
   }
   else
   {
      opcode |= 0xFA00;
   }
   if  ((flag & COB_SHARE_MASK) == COB_SHARE_WITH_READ)
   {
      CP_FLD1(fcd->interlang_lock, 0x40);
   }
   if ((mode ==  COB_OPEN_INPUT) && f->flag_optional)
   {
      SETB_ON_FLD1(fcd->FCD_OTHER_FLAGS_BIT_OPTIONAL_INPUT);
   }
   else if ((mode !=  COB_OPEN_INPUT) && !f->flag_optional)
   {
      SETB_ON_FLD1(fcd->FCD_OTHER_FLAGS_BIT_NOT_OPTIONAL_IO);
   }
   if (f->optional_in_source)
   {
      SETB_ON_FLD1(fcd->FCD_OTHER_FLAGS_BIT_OPTIONAL_IN_SOURCE);
   }
   extfh_call_extfh(rtd, f, opcode);
   RETURN_INT_STATUS;
}

int extfh_close(COB_RTD, cob_file_extfh *f, int opt)
{
   mf_extfh_FCD *fcd;
   cit_uint16 opcode;
   CHECK_F_EXTFH_PTR;


   switch (opt)
   {
      case COB_CLOSE_NORMAL        :
         opcode = 0xFA80;     break;
      case COB_CLOSE_LOCK          :
         opcode = 0xFA81;     break;
      case COB_CLOSE_NO_REWIND     :
         opcode = 0xFA82;     break;
      case COB_CLOSE_UNIT          :
         opcode = 0xFA84;     break;
      case COB_CLOSE_UNIT_REMOVAL  :
         opcode = 0xFA85;     break;
   }

   extfh_call_extfh(rtd, f, opcode);
   RETURN_INT_STATUS;
}

static int extfh_find_keynr(cob_file_extfh *f, cob_field *key)
{
   int k, i;
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint8               *key_def_block;
   mf_extfh_global_info    *global_info;
   mf_extfh_key_def        *key_def;
   mf_extfh_comp_def       *comp_def;
   unsigned char *base_data;
   unsigned char *rec_data;
   register int offset;
   int nkeys;

   fcd = (mf_extfh_FCD *)(f->extfh_ptr);

   base_data = GT_FLDP(fcd->rec_data_ptr);
   global_info = (mf_extfh_global_info *)GT_FLDP(fcd->key_def_block_ptr);
   key_def_block = (cit_uint8 *)global_info;

   if (global_info)
   {
      nkeys = GT_FLD2(global_info->keys_count);
      key_def     = (mf_extfh_key_def *)(key_def_block + sizeof(mf_extfh_global_info));
      /* look up for the key */
      for (k = 0; k < nkeys; k++)
      {
         register int comp_count = GT_FLD2(key_def[k].comp_count);
         if (comp_count > COB_MAX_KEY_PART)
         {
            comp_count = COB_MAX_KEY_PART;
         }
         comp_def = (mf_extfh_comp_def *)((char *)key_def_block + GT_FLD2(key_def[k].comp_offset));
         for (i = 0; i < comp_count; i++)
         {
            offset = GT_FLD4(comp_def[i].offset);
            rec_data = base_data + offset;
            if (rec_data == key->data)
            {
               return k;
            }
         }
      }
   }

   return 0;
}

int extfh_start_indexed(COB_RTD, cob_file_extfh *f, int cond, int id_key, int len)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16 opcode;
   CHECK_F_EXTFH_PTR;
   CP_FLD2(fcd->key_id, id_key);
   CP_FLD2(fcd->key_length, len);
   switch (cond)
   {
      case COB_EQ:
         opcode = 0XFAE9; break;
      case COB_LT:
         opcode = 0XFAFE; break;
      case COB_LE:
         opcode = 0XFAFF; break;
      case COB_GT:
         opcode = 0XFAEA; break;
      case COB_GE:
         opcode = 0XFAEB; break;
      case COB_NE:
      default:
         cob_runtime_error(rtd, "extfh_indexed_start : invalid condition %d", cond);
         cob_stop_abend(rtd, COBRE_EXTFH_START);
         break;
   }
   extfh_call_extfh(rtd, f, opcode);
   RETURN_INT_STATUS;
}

int extfh_start_relative(COB_RTD, cob_file_extfh *f, int cond, cob_field *key)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16 opcode;
   register int organization;
   CHECK_F_EXTFH_PTR;
   organization = GT_FLD1(fcd->organization);
   set_relative_record_number_to_fcd(rtd, f, key);
   switch (cond)
   {
      case COB_EQ:
         opcode = 0XFAE9; break;
      case COB_LT:
         opcode = 0XFAFE; break;
      case COB_LE:
         opcode = 0XFAFF; break;
      case COB_GT:
         opcode = 0XFAEA; break;
      case COB_GE:
         opcode = 0XFAEB; break;
      case COB_NE:
      default:
         cob_runtime_error(rtd, "extfh_indexed_start : invalid condition %d", cond);
         cob_stop_abend(rtd, COBRE_EXTFH_START);
         break;
   }
   extfh_call_extfh(rtd, f, opcode);
   RETURN_INT_STATUS;
}

int extfh_read(COB_RTD, cob_file_extfh *f, cob_field *key, int read_opts)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   register int organization, access_mode;
   CHECK_F_EXTFH_PTR;
   organization = GT_FLD1(fcd->organization);
   access_mode  = GT_FLD1(fcd->access_mode);
   opcode = 0;

   if (key)
   {
      if (organization == FCD_ORG_INDEXED)
      {
         int key_id = extfh_find_keynr(f, key);
         CP_FLD2(fcd->key_id, key_id);
      }
      else if (organization == FCD_ORG_RELATIVE)
      {
         set_relative_record_number_to_fcd(rtd, f, key);
      }
   }

   if (!key) /*read next*/
   {
      if (read_opts & COB_READ_PREVIOUS)
      {
         switch (read_opts & COB_READ_LOCK_MASK)
         {
            case COB_READ_NO_LOCK:
               opcode = 0xFA8C; break;
            case COB_READ_LOCK:
               opcode = 0xFADE; break;
            case COB_READ_KEPT_LOCK:
               opcode = 0xFADF; break;
            default:
               opcode = 0xFAF9; break;
         }
      }
      else if (read_opts & COB_READ_NEXT)
      {
         switch (read_opts & COB_READ_LOCK_MASK)
         {
            case COB_READ_NO_LOCK:
               opcode = 0xFA8D; break;
            case COB_READ_LOCK:
               opcode = 0xFAD8; break;
            case COB_READ_KEPT_LOCK:
               opcode = 0xFAD9; break;
            default:
               opcode = 0xFAF5; break;
         }
      }
      else if (read_opts & COB_READ_FIRST)
      {
         switch (read_opts & COB_READ_LOCK_MASK)
         {
            case COB_READ_NO_LOCK:
               opcode = 0xFA92; break;
            case COB_READ_LOCK:
               opcode = 0xFAD0; break;
            case COB_READ_KEPT_LOCK:
               opcode = 0xFAD1; break;
            default:
               opcode = 0xFACC; break;
         }
      }
   }
   else
   {
      if (read_opts & COB_READ_PREVIOUS)
      {
         switch (read_opts & COB_READ_LOCK_MASK)
         {
            case COB_READ_NO_LOCK:
               opcode = 0xFA8C; break;
            case COB_READ_LOCK:
               opcode = 0xFADE; break;
            case COB_READ_KEPT_LOCK:
               opcode = 0xFADF; break;
            default:
               opcode = 0xFAF9; break;
         }
      }
      else if (read_opts & COB_READ_NEXT)
      {
         switch (read_opts & COB_READ_LOCK_MASK)
         {
            case COB_READ_NO_LOCK:
               opcode = 0xFA8D; break;
            case COB_READ_LOCK:
               opcode = 0xFAD8; break;
            case COB_READ_KEPT_LOCK:
               opcode = 0xFAD9; break;
            default:
               opcode = 0xFAF5; break;
         }
      }
      else if (read_opts & COB_READ_FIRST)
      {
         switch (read_opts & COB_READ_LOCK_MASK)
         {
            case COB_READ_NO_LOCK:
               opcode = 0xFA92; break;
            case COB_READ_LOCK:
               opcode = 0xFAD0; break;
            case COB_READ_KEPT_LOCK:
               opcode = 0xFAD1; break;
            default:
               opcode = 0xFACC; break;
         }
      }
      else if ((read_opts & COB_READ_DIR_MASK) == 0)
      {
         switch (access_mode)
         {
            default:
               opcode = 0xFAF1;
               break;
            case FCD_ACCESS_DYNAMIC:
            case FCD_ACCESS_RANDOM:
               switch (read_opts & COB_READ_LOCK_MASK)
               {
                  case COB_READ_NO_LOCK:
                     opcode = 0xFA8E; break;
                  case COB_READ_LOCK:
                     opcode = 0xFADA; break;
                  case COB_READ_KEPT_LOCK:
                     opcode = 0xFADB; break;
                  default:
                     opcode = 0xFAF6; break;
               }
               break;
               /*
               case COB_ACCESS_DYNAMIC:
                   switch ( read_opts & COB_READ_LOCK_MASK ) {
                       case COB_READ_NO_LOCK:      opcode = 0xFA8F; break;
                       case COB_READ_LOCK:         opcode = 0xFAD6; break;
                       case COB_READ_KEPT_LOCK:    opcode = 0xFAD7; break;
                       default:                    opcode = 0xFAC9; break;
                   }
                   break;
              */
         }
      }
   }

   if (opcode == 0)
   {
      cob_runtime_error(rtd, "extfh_read : invalid read option %d", read_opts);
      cob_stop_abend(rtd, COBRE_EXTFH_READ);
   }
   extfh_call_extfh(rtd, f, opcode);
   if (fcd->user_file_status[0] == CHAR_0)
   {
      if (f->record_size && FCD_VAR_REC_MODE(fcd))
      {
         cob_set_int(rtd, f->record_size, (int)GT_FLD2_4(fcd->cur_rec_length));
      }
   }
   RETURN_INT_STATUS;
}

int extfh_write(COB_RTD, cob_file_extfh *f, int opt)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16 opcode;
   int k;
   register int organization;
   CHECK_F_EXTFH_PTR;

   opcode = 0xFAF3;
   organization = GT_FLD1(fcd->organization);
   if ((organization == FCD_ORG_SEQUENTIAL) ||
       (organization == FCD_ORG_LINE_SEQUENTIAL))
   {
      if (opt & COB_WRITE_BEFORE)
      {
         if (opt & COB_WRITE_CHANNEL)   opcode = 0xFAEC;
         else if (opt & COB_WRITE_PAGE) opcode = 0xFAE5;
         else                             opcode = 0xFAE1;
      }
      else if (opt & COB_WRITE_AFTER)
      {
         if (opt & COB_WRITE_CHANNEL)   opcode = 0xFAED;
         else if (opt & COB_WRITE_PAGE) opcode = 0xFAE6;
         else                             opcode = 0xFAE2;
      }
      /*
      if ( opt & COB_WRITE_LINES ) {
          CP_FLD2(fcd->key_id, opt & COB_WRITE_MASK);
      } else if (f->organization == COB_ORG_LINE_SEQUENTIAL ) {
          CP_FLD2(fcd->key_id, 1);
      }*/
#     ifdef COB_PTR_64BITS
      CP_FLD2(fcd->line_count, opt & COB_WRITE_MASK);
#     else
      CP_FLD2(fcd->key_id, opt & COB_WRITE_MASK);
#     endif
   }
   set_relative_record_number_to_fcd(rtd, f, f->relative_rec_number);

   if (FCD_VAR_REC_MODE(fcd) && f->record_size)
   {
      k = cob_get_int(rtd, f->record_size);
      CP_FLD2_4(fcd->cur_rec_length, k);
   }

   extfh_call_extfh(rtd, f, opcode);

   RETURN_INT_STATUS;
}

int extfh_delete(COB_RTD, cob_file_extfh *f)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   CHECK_F_EXTFH_PTR;

   opcode = 0xFAF7;

   set_relative_record_number_to_fcd(rtd, f, f->relative_rec_number);

   extfh_call_extfh(rtd, f, opcode);

   RETURN_INT_STATUS;
}

int extfh_rewrite(COB_RTD, cob_file_extfh *f, int opt)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   CHECK_F_EXTFH_PTR;

   opcode = 0xFAF4;
   /*Format 3:If the RELATIVE KEY phrase is specified for the file, 
   the execution of a READ statement updates the contents of the RELATIVE KEY data item 
   so that it contains the relative record number of the record made available*/

   set_relative_record_number_to_fcd(rtd, f, f->relative_rec_number);

   extfh_call_extfh(rtd, f, opcode);

   RETURN_INT_STATUS;
}

void extfh_index_unlock(COB_RTD, cob_file_extfh *f)
{
   extfh_unlock(rtd, f);
}

int extfh_index_locate(COB_RTD, cob_file_extfh *f, char *filename)
{
   mf_extfh_FCD *fcd;
   CHECK_F_EXTFH_PTR;
   return COB_STATUS_00_SUCCESS;
}

void extfh_seqra_unlock(COB_RTD, cob_file_extfh *f)
{
   extfh_unlock(rtd, f);
}

int extfh_seqra_locate(COB_RTD, cob_file_extfh *f, char *filename)
{
   mf_extfh_FCD *fcd;
   CHECK_F_EXTFH_PTR;
   return COB_STATUS_00_SUCCESS;
}

int extfh_delete_file(COB_RTD, cob_file_extfh *f)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   CHECK_F_EXTFH_PTR;

   if ((global_use_ctree || f->ctree) && !global_no_ctree_map)
   {
      opcode = 0xC3F8;
   }
   else
   {
      opcode = 0xFAF8;
   }

   extfh_call_extfh(rtd, f, opcode);
   RETURN_INT_STATUS;
}


int extfh_commit(COB_RTD, cob_file_extfh *f)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   CHECK_F_EXTFH_PTR;
   opcode = 0xFADC;

   extfh_call_extfh(rtd, f, opcode);
   RETURN_INT_STATUS;
}

int extfh_rollback(COB_RTD, cob_file_extfh *f)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   CHECK_F_EXTFH_PTR;

   opcode = 0xFADD;

   extfh_call_extfh(rtd, f, opcode);
   RETURN_INT_STATUS;
}


/* this module implement the MicroFocus(tm) EXTSM inteface */

static int extsm_call_extsm(COB_RTD, cob_file_extfh *f, cit_uint16  opcode)
{
   mf_extfh_FCD *fcd;
   cit_uint8  fct[2];
   int r;
   union
   {
       int (*func)(void *, void *);
       void    *func_void;
   } unifunc;

   fcd = f->extfh_ptr;
   if (!f->extsm_func)
   {
      if (f->extsm_name) unifunc.func_void = cob_resolve_1(rtd, f->extsm_name);
      else
      {
         unifunc.func_void = cob_resolve(rtd, "EXTSM");
         if (!unifunc.func_void)
         {
            unifunc.func_void = cob_resolve_1(rtd, "CIT_EXTSM");
         }
      }
      f->extsm_func = unifunc.func;
   }
   fct[0] = (cit_uint8)((opcode >> 8) & 0xFF);
   fct[1] = (cit_uint8)((opcode & 0xFF));
   r = f->extsm_func(fct, fcd);
   return r;
}

void extsm_sort_init_extfh(COB_RTD, cob_file_extfh *f, int nkeys,
                           const unsigned char *collating_sequence,
                           void *sort_return, cob_field *fnstatus)
{
   mf_extfh_FCD *fcd;
   FLD_VAR;
   ASSERT_F_EXTFH_PTR;

   cob_define_key_sort(rtd, f, nkeys);
   if (collating_sequence)
   {
      CP_FLDP(fcd->sort_colating, collating_sequence);
   }
   else
   {
      CP_FLDP(fcd->sort_colating, rtd->current_module->collating_sequence);
   }
   f->extsm_data = cob_malloc(rtd, sizeof(mf_extsm_data_extfh));

   RETURN_STATUS(COB_STATUS_00_SUCCESS);
}

void    extsm_sort_close_extfh(COB_RTD, cob_file_extfh *f)
{
   mf_extfh_FCD *fcd;
   cob_field   *fnstatus = NULL;
   ASSERT_F_EXTFH_PTR;
   cob_free(f->extsm_data);
   RETURN_STATUS(extfh_get_int_status(rtd, f));
}

void    extsm_sort_using_extfh(COB_RTD, cob_file_extfh *f, cob_file_extfh *data_file)
{
   mf_extfh_FCD *fcd;
   cob_field   *fnstatus = NULL;
   int i;

   ASSERT_F_EXTFH_PTR;

   if (f->extsm_data->using_cnt >= MAX_EXTSM_USING_GIVING)
   {
      cob_runtime_error(rtd, "extsm : maximum %d USING/GIVING clause allowed in extsm for %s", MAX_EXTSM_USING_GIVING, f->select_name);
      cob_stop_abend(rtd, COBRE_EXTSM_USING_GIVING);
   }
   i = f->extsm_data->using_cnt;

   f->extsm_data->file_def_block[i] =  (mf_extfh_FCD *)data_file->extfh_ptr;
   f->extsm_data->using_cnt++;

   RETURN_STATUS(COB_STATUS_00_SUCCESS);
}

void    extsm_sort_giving_extfh(COB_RTD, cob_file_extfh *f, size_t varcnt, ...)
{
   mf_extfh_FCD *fcd;
   cob_field   *fnstatus = NULL;
   va_list     args;
   int         i, k;
   cob_file_extfh *g;

   ASSERT_F_EXTFH_PTR;

   if (f->extsm_data->using_cnt + varcnt >= MAX_EXTSM_USING_GIVING)
   {
      cob_runtime_error(rtd, "extsm : maximum %d USING/GIVING clause allowed in extsm for %s", MAX_EXTSM_USING_GIVING, f->select_name);
      cob_stop_abend(rtd, COBRE_EXTSM_USING_GIVING);
   }
   va_start(args, varcnt);
   k = f->extsm_data->using_cnt;
   for (i = 0; i < varcnt; i++)
   {
      g = va_arg(args, cob_file_extfh *);
      f->extsm_data->file_def_block[k] = (mf_extfh_FCD *)g->extfh_ptr;
      f->extsm_data->giving_cnt++;
      k++;
   }
   va_end(args);

   RETURN_STATUS(COB_STATUS_00_SUCCESS);
}

void    extsm_release_1_extfh(COB_RTD, cob_file_extfh *f, cob_field *rec)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   cob_field   *fnstatus = NULL;
   int l;
   ASSERT_F_EXTFH_PTR;

   opcode = 0xFAF3;

   if (FCD_VAR_REC_MODE(fcd) && f->record_size)
   {
      l = cob_get_int(rtd, f->record_size);
      CP_FLD2_4(fcd->cur_rec_length, l);
   }
   else
   {
      if (rec)
      {
         CP_FLD2_4(fcd->cur_rec_length, rec->size);
      }
   }
   extsm_call_extsm(rtd, f, opcode);
   RETURN_STATUS(extfh_get_int_status(rtd, f));
}

void    extsm_release_extfh(COB_RTD, cob_file_extfh *f)
{
   extsm_release_1_extfh(rtd, f, NULL);
}

void    extsm_return_extfh(COB_RTD, cob_file_extfh *f)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   cob_field   *fnstatus = NULL;
   int l;
   ASSERT_F_EXTFH_PTR;

   opcode = 0xFAF5;

   if (FCD_VAR_REC_MODE(fcd) && f->record_size)
   {
      l = cob_get_int(rtd, f->record_size);
      CP_FLD2_4(fcd->cur_rec_length, l);
   }
   extsm_call_extsm(rtd, f, opcode);
   if (fcd->user_file_status[0] == CHAR_0)
   {
      if (FCD_VAR_REC_MODE(fcd) && f->record_size)
      {
         cob_set_int(rtd, f->record_size, GT_FLD2_4(fcd->cur_rec_length));
      }
   }

   RETURN_STATUS(extfh_get_int_status(rtd, f));
}

void    extsm_process_extfh(COB_RTD, cob_file_extfh *f)
{
   FLD_VAR
      mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   cob_field   *fnstatus = NULL;

   ASSERT_F_EXTFH_PTR;

   opcode = 0xFA01;

#  ifdef COB_PTR_64BITS
   CP_FLD1(fcd->use_files[0],  f->extsm_data->using_cnt);
   CP_FLD1(fcd->give_files[0], f->extsm_data->giving_cnt);
   CP_FLDP(fcd->fildef_address, f->extsm_data->file_def_block);
#  else
   CP_FLD1(fcd->key_id[0], f->extsm_data->using_cnt);
   CP_FLD1(fcd->key_id[1], f->extsm_data->giving_cnt);
   CP_FLDP(fcd->rel_byte_addr, f->extsm_data->file_def_block);
#  endif

   extsm_call_extsm(rtd, f, opcode);

   RETURN_STATUS(extfh_get_int_status(rtd, f));

}

void    extsm_end_input_extfh(COB_RTD, cob_file_extfh *f)
{
   mf_extfh_FCD *fcd;
   cit_uint16  opcode;
   cob_field   *fnstatus = NULL;
   ASSERT_F_EXTFH_PTR;

   opcode = 0xFA10;
   extsm_call_extsm(rtd, f, opcode);

   RETURN_STATUS(extfh_get_int_status(rtd, f));
}

void    extsm_end_output_extfh(COB_RTD, cob_file_extfh *f)
{
   mf_extfh_FCD *fcd;
   cit_uint16 opcode;
   cob_field   *fnstatus = NULL;
   ASSERT_F_EXTFH_PTR;

   opcode = 0xFA88;
   extsm_call_extsm(rtd, f, opcode);

   RETURN_STATUS(extfh_get_int_status(rtd, f));
}

void*
cob_get_extfh_fcd_ptr(COB_RTD, cob_file_extfh *f)
{
   mf_extfh_FCD *fcd;
   if (f)
   {
      fcd = (mf_extfh_FCD *)(f->extfh_ptr);
      if (fcd)
      {
         return fcd;
      }
   }
   return NULL;
}

void
cob_update_return_status_to_field(COB_RTD, cob_file_extfh *f, cob_field *fnstatus)
{
   RETURN_FCD_STATUS;
}



/*********************************************************/
/*********************************************************/
/* MF COBOL Call compatible fct used by UniKix and others*/
/*********************************************************/
/*********************************************************/
typedef struct {
    int   cblte_gpi_size; /*cblt-x4-comp5.*/
    int   cblte_gpi_flags; /*cblt-x4-comp5.*/
    char *cblte_gpi_handle; /*cblt-pointer.*/
    char *cblte_gpi_prog_id; /*cblt-pointer.*/
    int   cblte_gpi_attrs; /*cblt-x4-comp5.*/
} info_param_block;

/*
typedef struct { 
        int     cobbuf[3];
        char     * cobbuf2[3];
        jmp_buf cbuf;
        char     * cobbuf3;
} cobjmp_buf;
*/

int  CBL_GET_PROGRAM_INFO(int function, info_param_block *param_block, char *name_buf, int *name_len);


#define CICS_TRACE 1
static FILE *ftrace = NULL;

static void
CICS_trace(const char *s, ...)
{
#  if CICS_TRACE
   va_list l;
   va_start(l, s);
   if (!ftrace)
   {
      ftrace = fopen("CIT_CICS.txt", "a");
   }
   if (ftrace)
   {
      vfprintf(ftrace, s, l);
      fflush(ftrace);
   }
   va_end(l);
#  endif
}


/*
call "CBL_GET_PROGRAM_INFO" using by value     function
                                  by reference param-block
                                  by reference name-buf
                                  by reference name-len
                                  returning    status-code
Parameters:
function 	cblt-x4-comp5.
param-block 	A group item predefined as cblt-prog-info-params, containing the following subordinate items:
    cblte-gpi-size 	cblt-x4-comp5.
    cblte-gpi-flags 	cblt-x4-comp5.
    cblte-gpi-handle 	cblt-pointer.
    cblte-gpi-prog-id 	cblt-pointer.
    cblte-gpi-attrs 	cblt-x4-comp5.
name-buf 	pic x(n).
name-len 	cblt-x4-comp5.
status-code 	See Key.
*/
int
CBL_GET_PROGRAM_INFO(int function, info_param_block *param_block, char *name_buf, int *name_len)
{
   CICS_trace("CBL_GET_PROGRAM_INFO function = %d\n", function);
   /*
   switch ( function ) {
       case 0:
           if ( rtd->current_module ) {
               if ( param_block->cblte_gpi_flags & 1 ) {
                   param_block->cblte_gpi_handle = (char*)rtd->current_module;
               }
               return 0;
           } else {
               return 1009;
           }

   }
   */
   return 0;
}


cit_uint8  cobget_x1_compx(const char *cbldata)
{
   FLD_VAR
   return GT_FLD1_p(cbldata);
}

cit_uint16 cobget_x2_compx(const char *cbldata)
{
   FLD_VAR
   return GT_FLD2_p(cbldata);
}
cit_uint32  cobget_x4_compx(const char *cbldata)
{
   FLD_VAR
   return GT_FLD4_p(cbldata);
}

cit_uint64  cobget_x8_compx(const char *cbldata)
{
   FLD_VAR
   return GT_FLD8_p(cbldata);
}

void* cobget_pointer(const char *cbldata)
{
   FLD_VAR
   return GT_FLDP_p(cbldata);

}
void* cobget_ppointer(const char *cbldata)
{
   return cobget_pointer(cbldata);
}


void cobput_x1_compx(char *cbldata, cit_uint8 val)
{
   FLD_VAR
      CP_FLD1_p(cbldata, val);
}

void cobput_x2_compx(char *cbldata, cit_uint16 val)
{
   FLD_VAR
      CP_FLD2_p(cbldata, val);
}

void cobput_x4_compx(char *cbldata, cit_uint32 val)
{
   FLD_VAR
      CP_FLD4_p(cbldata, val);
}

void cobput_x8_compx(char *cbldata, cit_uint64 val)
{
   FLD_VAR
      CP_FLD8_p(cbldata, val);
}

void cobput_pointer(char *cbldata, void *val)
{
   FLD_VAR
      CP_FLDP_p(cbldata, val);
}


/*CIT 4.0*/

void cob_file_set_fcd_file_name(COB_RTD, cob_file_extfh *f, cob_field *field)
{
   mf_extfh_FCD *fcd;
   FLD_VAR

      int size;
   unsigned char *filename;

   if (!f || !f->extfh_ptr || !field)
   {return; }
   fcd = (mf_extfh_FCD *)(f->extfh_ptr);
   filename = field->data;
   size = field->size;

   CP_FLDP(fcd->fname_ptr, filename);
   (void)STRING_E2A(fcd->fname_ptr, strlen((char *)fcd->fname_ptr));
   CP_FLD2(fcd->fname_length, size);


}
void cob_define_global_info_fcd(COB_RTD, void *fcd_ptr, int nkeys, cit_uint8 *key_def_block)
{
   int s;

   mf_extfh_global_info    *global_info;


   FLD_VAR
      mf_extfh_FCD *fcd = (mf_extfh_FCD *)fcd_ptr;

   /*key size in total*/
   s = sizeof(mf_extfh_global_info) + (nkeys * sizeof(mf_extfh_key_def)) +
       (nkeys * sizeof(mf_extfh_comp_def) * COB_MAX_KEY_PART);

   CP_FLDP(fcd->key_def_block_ptr, key_def_block);
   global_info = (mf_extfh_global_info *)key_def_block;
   CP_FLD2(global_info->key_def_block_len, s);
   CP_FLD2(global_info->keys_count,         nkeys);


}
void cob_define_key_def_fcd(void *fcd_ptr, int id_key,  int comp_count, int comp_offset, unsigned char key_flags, unsigned char compression_flags)
{
   size_t nkeys;

   cit_uint8               *key_def_block;
   mf_extfh_global_info    *global_info;
   mf_extfh_key_def        *key_def;
   FLD_VAR

      mf_extfh_FCD *fcd = (mf_extfh_FCD *)fcd_ptr;

   global_info = (mf_extfh_global_info *)GT_FLDP(fcd->key_def_block_ptr);
   if (!global_info)
   {
      return;
   }
   nkeys = GT_FLD2(global_info->keys_count);
   key_def_block = (cit_uint8 *)global_info;
   key_def     = (mf_extfh_key_def *)(key_def_block + sizeof(mf_extfh_global_info));
   CP_FLD1(key_def[id_key].key_flags,   key_flags);
   if (compression_flags)
   {
      CP_FLD1(key_def[id_key].compression_flags,   compression_flags);
   }
   /*Offset to first Component Definition Area for id_key, starting from Global Information Area*/

   CP_FLD2(key_def[id_key].comp_offset, comp_offset);
   CP_FLD2(key_def[id_key].comp_count,  comp_count);
}

static void key_field_attr_to_fcd(mf_extfh_FCD *fcd, int id_comp, cob_field *key, int nkeys)
{
   cit_uint8               *key_def_block;
   mf_extfh_global_info    *global_info;
   mf_extfh_comp_def       *comp_def;
   int s = 0;
   FLD_VAR

   if (!fcd)
   {
      return;
   }
   global_info = (mf_extfh_global_info *)GT_FLDP(fcd->key_def_block_ptr);

   key_def_block = (cit_uint8 *)global_info;

   comp_def    = (mf_extfh_comp_def *)(key_def_block + sizeof(mf_extfh_global_info) +
                                       (nkeys * sizeof(mf_extfh_key_def)));

   /*initialization*/
   CP_FLD1(comp_def[id_comp].filer[1],  s);

   if (COB_FIELD_IS_NUMERIC(key))
   {
      s |= 0x80;
      if (COB_FIELD_HAVE_SIGN(key)) s |= 0x40;
      if (COB_FIELD_TYPE(key) != COB_TYPE_NUMERIC_DISPLAY)
      {
         s |= 0x20;
         if ((COB_FIELD_TYPE(key) ==  COB_TYPE_NUMERIC_FLOAT && key->size <= 4) ||
             (COB_FIELD_TYPE(key) ==  COB_TYPE_NUMERIC_DOUBLE && key->size > 4))
         {
            s |= 0x4;
         }
         else if (COB_FIELD_TYPE(key) ==  COB_TYPE_NUMERIC_PACKED)
         {
            s |= 0x1;
         }

#        ifndef   WORDS_BIGENDIAN
         /*COMP-5, Otherwise Binary*/
         else if (COB_FIELD_BINARY_SWAP(key) ||
                  COB_FIELD_REAL_BINARY(key))
         {
            s |= 0x3;
         }
#        endif

      }
      else
      {
         if (COB_FIELD_HAVE_SIGN(key))
         {
            if (COB_FIELD_SIGN_SEPARATE(key))
            {
               if (COB_FIELD_SIGN_LEADING(key))
               {
                  s |= 0x3;
               }
               else
               {
                  s |= 0x1;
               }
            }
            else
            {
               if (COB_FIELD_SIGN_LEADING(key))
               {
                  s |= 0x2;
               }
               else
               {
                  s |= 0;
               }
            }
         }
      }

   }
   else
   {
      s = 1;
   }
   CP_FLD1(comp_def[id_comp].filer[1],  s);
}

void cob_define_key_component_fcd(void *fcd_ptr, int nkeys, int id_comp, int offset, cob_field *component_key)
{
   cit_uint8               *key_def_block;
   mf_extfh_global_info    *global_info;
   mf_extfh_comp_def       *comp_def;
   FLD_VAR


      mf_extfh_FCD *fcd = (mf_extfh_FCD *)fcd_ptr;
   global_info = (mf_extfh_global_info *)GT_FLDP(fcd->key_def_block_ptr);
   key_def_block = (cit_uint8 *)global_info;

   comp_def    = (mf_extfh_comp_def *)(key_def_block + sizeof(mf_extfh_global_info) +
                                       (nkeys * sizeof(mf_extfh_key_def)));

   CP_FLD4(comp_def[id_comp].length,    component_key->size);
   CP_FLD4(comp_def[id_comp].offset,     offset);
   key_field_attr_to_fcd(fcd, id_comp, component_key, nkeys);

}

/* SORT */
void cob_define_key_sort(COB_RTD, cob_file_extfh *f, int comp_count)
{
   mf_extfh_FCD *fcd;
   int comp_offset;
   cit_uint8               *key_def_block;
   mf_extfh_global_info    *global_info;
   int s;
   FLD_VAR;
   ASSERT_F_EXTFH_PTR;

   /* For sort operations, only one key is allowed, although this key can have many components*/
   if (!comp_count)
   {
      return;
   }
   s = sizeof(mf_extfh_global_info) + sizeof(mf_extfh_key_def) + (comp_count * sizeof(mf_extfh_comp_def));
   key_def_block = cob_malloc(rtd, s);

   CP_FLDP(fcd->key_def_block_ptr, key_def_block);
   global_info = (mf_extfh_global_info *)key_def_block;
   CP_FLD2(global_info->key_def_block_len, s);
   CP_FLD2(global_info->keys_count,         1);

   comp_offset = sizeof(mf_extfh_global_info) + sizeof(mf_extfh_key_def);
   cob_define_key_def_fcd(fcd, 0, comp_count, comp_offset, 0, 0);
}

void cob_define_key_component_sort(COB_RTD, cob_file_extfh *f, int id_comp, int flag, cob_field *component_key, int offset)
{
   mf_extfh_FCD *fcd;
   cit_uint8               *key_def_block;
   mf_extfh_global_info    *global_info;
   mf_extfh_comp_def       *comp_def;

   FLD_VAR;
   ASSERT_F_EXTFH_PTR;

   global_info = (mf_extfh_global_info *)GT_FLDP(fcd->key_def_block_ptr);
   key_def_block = (cit_uint8 *)global_info;
   comp_def    = (mf_extfh_comp_def *)(key_def_block + sizeof(mf_extfh_global_info) + (sizeof(mf_extfh_key_def)));
   /*IF flag = 1 then DESCENDING key*/
   CP_FLD1(comp_def[id_comp].filer[0],  (flag ? 0x40 : 0));
   CP_FLD4(comp_def[id_comp].length,    component_key->size);
   CP_FLD4(comp_def[id_comp].offset,     offset);
   key_field_attr_to_fcd(fcd, id_comp, component_key, 1);
}

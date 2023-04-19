/*
 * Copyright (C) 2001-2007 Keisuke Nishida
 * Copyright (C) 2007 Roger While
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


#include "iconv.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#ifdef  HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_FCNTL_H
#  include    <fcntl.h>
#endif
#ifdef  HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#  include <errno.h>
#endif
#include <time.h>
#include <sys/stat.h>
#ifdef  HAVE_SIGNAL_H
#  include <signal.h>
#endif

#ifdef _WIN32
#  include <windows.h>            /* for GetTempPath, GetTempFileName */
#  include <fcntl.h>
#  include <io.h>
#  define     getcwd  _getcwd
#endif

/*
#ifdef  HAVE_KPATHSEA_GETOPT_H
    #include <kpathsea/getopt.h>
#else
    #ifdef  HAVE_GETOPT_H
        #include <getopt.h>
    #else
        #include "lib/getopt.h"
    #endif
#endif
*/

#ifdef  HAVE_LOCALE_H
#  include <locale.h>
#endif

#include <libcob.h>
#include <tarstamp.h>

#include "tree.h"
#include "cobc.h"

#include "../lib/cit_getopt.h"

#include "enterprise/debugdb.h"
#ifdef WITH_ICU
COB_DLL_EXPIMP void cob_list_codepage(void);
COB_DLL_EXPIMP void cob_check_codepage(char *name);
#endif
int                      exit_status = 1;

/*
 * Global variables
 */

extern int yydebug;
int     cb_source_format = CB_FORMAT_FIXED;

#if defined(COB_EBCDIC_MACHINE) || defined(CIT_EBCDIC_CHARSET_DEFAULT)
int     cb_display_sign_ebcdic = COB_DISPLAY_SIGN_EBCDIC;      /* 1 */
#else
int     cb_display_sign_ebcdic = COB_DISPLAY_SIGN_ASCII;       /* 0 */
#endif

#undef COB_EXCEPTION
#define COB_EXCEPTION(code,tag,name,critical) {name, 0x##code, 0},
struct cb_exception cb_exception_table[] = {
   { NULL, 0, 0 },       /* CB_EC_ZERO */
#  include <libcob/exception.def>
   { NULL, 0, 0 }        /* CB_EC_MAX */
};
#undef  COB_EXCEPTION

#undef CB_FLAG
#define CB_FLAG(var,name,doc,def) int var = def;
#include "flag.def"
#undef  CB_FLAG

#undef CB_WARNDEF
#define CB_WARNDEF(var,name,wall,doc,def) int var = def;
#include "warning.def"
#undef  CB_WARNDEF

#ifdef  _MSC_VER
#  define PATHSEPS ";"
#  define PATH_MAX MAX_PATH
/*Multithread static*/
/*#define WIN_CODEGEN_MODEL " /MT " */
/*Multithread DLL*/
#  define WIN_CODEGEN_MODEL " /MD "
#else
#  define PATHSEPS ":"
#endif

int                     cb_id = 1;
int                     cb_attr_id = 1;
int                     cb_literal_id = 1;
int                     cb_field_id = 10;
int                     cb_storage_id = 1;
int                     cb_flag_main = 0;
int                     cb_main_generated = 0;
int                     cb_has_report_section = 0;
int                     has_COPY_lib_defined = 0;

int                     errorcount = 0;
int                     warningcount = 0;
int                     alt_ebcdic = 0;
int                     optimize_flag = 0;
char                    *cb_sysin_redirect = NULL;
char                    *cb_sysout_redirect = NULL;
char                    *cb_syserr_redirect = NULL;
char                    *cb_sysprint_redirect = NULL;
struct cb_file          *cb_sysin_file = NULL;
struct cb_file          *cb_sysout_file = NULL;
struct cb_file          *cb_syserr_file = NULL;
struct cb_file          *cb_sysprint_file = NULL;

char                    *cb_main_source_file = NULL;
cb_lex_source_file_info  cb_lex_source_file = { 0 };
char                    *cb_final_source_file = NULL;
char                    *cb_build_stamp = NULL;
char                    *source_demangle_name = NULL;
int                     cb_final_source_line = 0;
int                     cb_keep_final_source = 0;
FILE                    *cb_listing_file = NULL;
FILE                    *cb_storage_file = NULL;
char                    *cb_storage_file_name = NULL;
/*CIT*/
FILE                    *cb_debug_file = NULL;
char                    *cb_debug_file_name = NULL;
FILE                    *cb_profiling_file = NULL;
char                    *cb_profiling_file_name = NULL;
FILE                    *cb_local_function_file = NULL;
char                    *cb_local_function_file_name = NULL;
FILE                    *cb_global_file = NULL;
char                    *cb_global_file_name = NULL;
FILE                    *cb_ccmap_file = NULL;
char                    *cb_ccmap_file_name = NULL;
FILE                    *cb_reg_file = NULL;
char                    *cb_reg_file_name = NULL;

FILE                    *cb_depend_file = NULL;
char                    *cb_depend_target = NULL;
struct cb_text_list     *cb_depend_list = NULL;
struct cb_inc_list      *cb_include_list = NULL;
struct cb_text_list     *cb_extension_list = NULL;
int                      cb_disable_runtime_check = 0;
/*CIT*/
int                      cb_flag_all_extfh = 0;
int                      cb_flag_extsm = 0;
char                     *cb_extfh_name = (char *)"EXTFH";
char                     *cb_extsm_name=(char*)"CIT_EXTSM";
int                      cb_saveargc;
char                     **cb_saveargv;
int                      cb_call_num = 0;
int                      cb_if_level = 0;
struct cb_constant_list  *cb_constants = NULL;
int                      cb_flag_debuginfo = 0;
int                      cb_flag_runtimecheck = 0;
int                      cb_warn_undefine = 0;
int                      cb_optimize_size = 0;
int                      cb_pplinemark = 0;
static char              *used_environmen_var[200];
static char              *used_environmen_val[200];
static int               used_environmen_var_cnt = 0;
struct  cit_runtime_s *rtd = NULL;
struct cb_text_list      *cb_initcall_list = NULL;

/*
 * Local variables
 */

static const char   *const cob_csyns[] = {
   "NULL",
   "L_initextern",
   "LRET_initextern",
   "P_switch",
   "alignof",
   "asm",
   "auto",
   "break",
   "case",
   "char",
   "const",
   "continue",
   "default",
   "do",
   "double",
   "else",
   "enum",
   "exit_program",
   "extern",
   "float",
   "for",
   "frame_pointer",
   "frame_stack",
   "goto",
   "if",
   "inline",
   "int",
   "long",
   "offsetof",
   "register",
   "restrict",
   "return",
   "short",
   "signed",
   "sizeof",
   "static",
   "struct",
   "switch",
   "typedef",
   "typeof",
   "union",
   "unsigned",
   "void",
   "volatile",
   "_Bool",
   "_Complex",
   "_Imaginary",
   "main",
   "memcpy",
   "memmove",
   "memcmp",
   "exit",
   NULL
};

#define COB_NUM_CSYNS   sizeof(cob_csyns) / sizeof(char *)

enum cb_compile_level_enum           cb_compile_level = 0;
static enum cb_compile_level_enum    local_level = 0;

static size_t           iparams = 0;
static int              iargs;
static char             *cobcpy = NULL;
static char             *save_temps_dir = NULL;

static jmp_buf          cob_jmpbuf;

static int              wants_nonfinal = 0;
static int              cb_flag_module = 0;
static int              cb_flag_library = 0;
static int              code_cover = 0;
static int              save_temps = 0;
static int              save_csrc = 0;
static int              verbose_output = 0;
static int              cob_iteration = 0;

static int              cob_process_id = 0;


static int              strip_output = 1;
static char             *listing_name = NULL;
static char             *listing_dir = NULL;

static char             *program_name;
static char             *output_name = NULL;
static char             *output_dir = NULL;
static char             *output_debugdb_dir = NULL;
char                    *ext_prepro_name;
char                    *linkage_descriptor_name = NULL;
int                     cb_prepro_initial_pass = 0;

int                     cb_raw_debug_line = 0;
char                    *cobc_tmpdir = NULL;
void                    *cb_debugdb = NULL;
long long               cb_debugdb_module_id = 0;
char                    *cb_debugdb_name = NULL;

static struct filename  *file_list;

/* NOTE fcopts MUST have at least one leading space */
#if defined (__GNUC__) && (__GNUC__ >= 3)
static const char fcopts[] = " -finline-functions ";
#elif defined(__xlc__)
static const char fcopts[] = " -Q -qro -qroconst ";
#else
static const char fcopts[] = " ";
#endif
/* compilation constant patched when using gcc*/
static const char *The_COB_CFLAGS         = COB_CFLAGS;
static const char *The_COB_OPTSIZE_FLAG   = COB_OPTSIZE_FLAG;
static const char *The_COB_OPTIMIZE_FLAG  = COB_OPTIMIZE_FLAG;
static const char *The_COB_LDFLAGS        = COB_LDFLAGS;
static const char *The_COB_PIC_FLAGS      = COB_PIC_FLAGS;
static const char *The_COB_SHARED_OPT     = COB_SHARED_OPT;
static const char *The_COB_EXPORT_DYN     = COB_EXPORT_DYN;
static const char *The_COB_EXTRA_FLAGS    = COB_EXTRA_FLAGS;

#ifdef  HAVE_SIGNAL_H
typedef void (*cob_sighandler_t)(int);
static cob_sighandler_t     hupsig = NULL;
static cob_sighandler_t     intsig = NULL;
static cob_sighandler_t     qutsig = NULL;
#endif

/*static const char short_options[] = "hVvECScbmxOgGwo:t:I:L:l:D:R:p:";*/
static const char short_options[] = "o:t:I:L:l:D:R:r:";

static const struct cit_option long_options[] = {
   { "static", no_argument,             &cb_flag_static_call,   1 },
   { "dynamic", no_argument,            &cb_flag_static_call,   0 },
   { "dynopt", no_argument,             &cb_flag_static_call,   2 },
   /* abbreviations */
   { "cp", required_argument, NULL, '[' },
   { "source-cp", required_argument, NULL, ']' },
   { "h", no_argument, NULL, 'h' },
   { "p", no_argument, NULL, 'p' },
   { "v", no_argument, NULL, 'v' },
   { "V", no_argument, NULL, 'V' },
#  undef CB_LONGOPT_BOOLEAN
#  undef CB_LONGOPT_SET
#  undef CB_LONGOPT_STRING
#  undef CB_LONGOPT_SHORT
#  define CB_LONGOPT_BOOLEAN(name, val, var) {name, no_argument, NULL, val},
#  define CB_LONGOPT_SET(name, var, setval, setvar) {name, no_argument, &setvar, setval},
#  define CB_LONGOPT_STRING(name, val, var)  {name, required_argument, NULL, val},
#  define CB_LONGOPT_OPT(name, val, var)  {name, optional_argument, NULL, val},
#  define CB_LONGOPT_SHORT(name, val, var)
#  include "longopt.def"
#  undef CB_LONGOPT_BOOLEAN
#  undef CB_LONGOPT_SET
#  undef CB_LONGOPT_STRING
#  undef CB_LONGOPT_OPT
#  undef CB_LONGOPT_SHORT

#  undef CB_FLAG
#  define CB_FLAG(var,name,doc,def)                   \
        {"f"name, no_argument, &var, 1},        \
        {"fno-"name, no_argument, &var, 0},
#  include "flag.def"
#  undef  CB_FLAG
#  undef CB_WARNDEF
#  define CB_WARNDEF(var,name,wall,doc, def)           \
        {"W"name, no_argument, &var, 1},        \
        {"Wno-"name, no_argument, &var, 0},
#  include "warning.def"
#  undef  CB_WARNDEF
   { NULL, 0, NULL, 0 }
};

static char             cob_cc[COB_MEDIUM_BUFF];                 /* gcc */
static char             cob_cflags[COB_MEDIUM_BUFF];             /* -I... */
static char             cob_arflags[COB_MEDIUM_BUFF];            /* -r */
static char             cob_libs[COB_MEDIUM_BUFF];              /* -L... -lcob */
static char             cob_ldflags[COB_MEDIUM_BUFF];
static char             cob_define_flags[COB_MEDIUM_BUFF];       /* -D... */
static char             cob_copy_dir[COB_MEDIUM_BUFF];
static char             cob_export_dyn[COB_MEDIUM_BUFF];
static char             cob_shared_opt[COB_MEDIUM_BUFF];
static char             cob_pic_flags[COB_MEDIUM_BUFF];
char                    cob_config_dir[COB_MEDIUM_BUFF];
const char              *cobolit_dir;
static char             cob_ar[COB_MEDIUM_BUFF];                 /* ar */
static char             cob_Oflag[COB_MEDIUM_BUFF];              /* -O2 */
static char             cob_eflag[COB_MEDIUM_BUFF];              /* -O2 */
static char             cob_sflag[COB_MEDIUM_BUFF];              /* -Os */
static int              cob_cc_is_gcc = 0;



/* cobc functions */

/*
 * Global functions
 */
static char* cob_getenv(const char *var)
{
   char *res = getenv(var);
   if (res && (used_environmen_var_cnt < 200))
   {
      used_environmen_var[used_environmen_var_cnt] = strdup(var);
      used_environmen_val[used_environmen_var_cnt] = strdup(res);
      used_environmen_var_cnt++;
   }
   return res;
}

#undef  DEBUGMF

char*
cobc_find_constant(char *key, char *value, int is_makesyn, int *is_userconst)
{
   struct cb_constant_list  *l;
   l = cb_constants;
   while (l)
   {
      if ((strcasecmp(l->key, key) == 0) && (l->is_makesyn == is_makesyn))
      {
         if (is_userconst)
         {
            *is_userconst = l->is_userconst;
         }
         if (value)
         {
            if (l->value) strcpy(value, l->value);
            else strcpy(value, "set");
#           ifdef DEBUGMF
            printf("found const %s = %s\n", key, value);
#           endif
            return value;
         }
         else
         {
            return(char *)l->value;
         }
      }
      l = l->next;
   }
   return 0;
}

#define TOK_SEP "\" =\t\n\r()"
char*
cobc_add_constant(char *arg, int is_makesyn, int is_userconst)
{
   char                    *p;
   struct cb_constant_list *l;

   l = cb_constants;
   p = strtok(arg, TOK_SEP);
   while (l && p)
   {
      if ((strcasecmp(l->key, p) == 0) &&
          (l->is_makesyn == is_makesyn))
      {
         p = strtok(NULL, TOK_SEP);
         if (p)
         {
            l->value = strdup(p);
            if (is_userconst)
            {
               l->is_userconst = is_userconst;
            }
         }
         return p;
      }
      l = l->next;
   }

   l = cobc_malloc(sizeof(struct cb_constant_list));
   l->next = cb_constants;
   l->is_makesyn = is_makesyn;
   l->is_userconst = is_userconst;
   cb_constants = l;
   if (p)
   {
      l->key = strdup(p);
      p = strtok(NULL, TOK_SEP);
      if (p)
      {
         l->value = strdup(p);
      }
   }
   else
   {
      l->key = "";
   }
#  ifdef DEBUGMF
   printf("define constant %s %s\n", l->key, l->value);
#  endif
   return p;
}

void
cobc_abort(const char *filename, const int linenum)
{
   fprintf(stderr, "%s:%d: Internal compiler error\n", filename, linenum);
#  ifdef DEBUG
   kill(getpid(), SIGTRAP);
#  endif
   (void)longjmp(cob_jmpbuf, 1);
}

void
cobc_tree_cast_error(cb_tree x, const char *filen, const int linenum, const int tagnum)
{
   fprintf(stderr, "%s:%d: Invalid type cast from '%s' :",
           filen, linenum, x ? cb_name(x) : "null");
   fprintf(stderr, "Tag 1 %d Tag 2 %d\n", x ? CB_TREE_TAG(x) : 0,
           tagnum);
#  ifdef DEBUG
   kill(getpid(), SIGTRAP);
#  endif
   (void)longjmp(cob_jmpbuf, 1);
}

void*
cobc_malloc(const size_t size)
{
   void *mptr;

#  ifdef DEBUG
   if (size == 0)
   {
      kill(getpid(), SIGTRAP);
   }
#  endif
   mptr = calloc(1, size);
   if (!mptr)
   {
      fprintf(stderr, "cobc:0: Cannot allocate %d bytes of memory - Aborting\n", (int)size);
      fflush(stderr);
#     ifdef DEBUG
      kill(getpid(), SIGTRAP);
#     endif
      (void)longjmp(cob_jmpbuf, 1);
   }
   return (mptr);
}

void*
cobc_realloc(void *prevptr, const size_t size)
{
   void *mptr;

   mptr = realloc(prevptr, size);
   if (!mptr)
   {
      fprintf(stderr, "cobc:0: Cannot reallocate %d bytes of memory - Aborting\n", (int)size);
      fflush(stderr);
      (void)longjmp(cob_jmpbuf, 1);
   }
   return (mptr);
}

static struct cb_inc_list*
cb_include_list_add_1(struct cb_inc_list *list, const char *name, struct cb_text_list *ext_list, char *libname)
{
   struct cb_inc_list *p;
   struct cb_inc_list *l;

   p = cobc_malloc(sizeof(struct cb_inc_list));
   p->path = strdup(name);
   p->next = NULL;
   p->lib  = libname;
   if (libname && strlen(libname) > 0)
   {
      has_COPY_lib_defined = 1;
   }
   p->extlist = ext_list;
   if (!list)
   {
      return (p);
   }
   else
   {
      for (l = list; l->next; l = l->next)
      {
         ;
      }
      l->next = p;
      return (list);
   }
}

struct cb_inc_list*
cb_include_list_add(struct cb_inc_list *list, const char *name)
{
   char *p = strdup(name);
   char *path;
   char *lib;

   char *ext = NULL;
   struct cb_text_list *extl = NULL;
   path = strtok(p, "@\n");
   lib  = strtok(NULL, "\n");
   if (path && strlen(path) > 0)
   {
      path = strtok(path, "|,");
      ext = strtok(NULL, "|,");
      while (ext)
      {
         if (strcmp(".", ext) == 0)
         {
            ext = (char *)"";
         }
         extl = cb_text_list_add(extl, ext);
         ext = strtok(NULL, "|,");
      }
      list = cb_include_list_add_1(list, path, extl, lib);
   }
   return list;
}

struct cb_inc_list*
cb_include_list_file_add(struct cb_inc_list *list, const char *fname)
{
   FILE *f;
   char  Buffer[COB_MEDIUM_BUFF];
   char  *p;
   char  *q;

   f = fopen(fname, "r");
   if (!f)
   {
      cobc_terminate(fname);
   }
   while (!feof(f))
   {
      if (fgets(Buffer, sizeof(Buffer), f))
      {
         p = Buffer;
         while (*p == ' ')
         {
            p++;
         }
         q = &Buffer[strlen(Buffer)];
         while (q > p && *q == ' ')
         {
            *q = 0;
            q--;
         }
         if (*p != '#')
         {
            list = cb_include_list_add(list, p);
         }
      }
   }

   fclose(f);
   return list;
}


struct cb_text_list*
cb_text_list_add(struct cb_text_list *list, const char *text)
{
   struct cb_text_list *p;
   struct cb_text_list *l;

   p = cobc_malloc(sizeof(struct cb_text_list));
   p->text = strdup(text);
   p->next = NULL;
   if (!list)
   {
      return (p);
   }
   else
   {
      for (l = list; l->next; l = l->next)
      {
         ;
      }
      l->next = p;
      return (list);
   }
}

struct cb_text_list*
cb_text_list_prefix(struct cb_text_list *list, const char *text)
{
   struct cb_text_list *p;

   p = cobc_malloc(sizeof(struct cb_text_list));
   p->text = strdup(text);
   p->next = list;
   return (p);
}

size_t
cobc_check_valid_name(const char *name)
{
   size_t  n;

   for (n = 0; cob_csyns[n]; n++)
   {
      if (!strcasecmp(name, cob_csyns[n]))
      {
         return (1);
      }
   }
   return (0);
}

/*
 * Local functions
 */

static void
cob_patch_var(char *var, const char *def, const char *base, const char *newbase)
{

   if (base && newbase &&
       base != newbase &&
       (strcmp(base, newbase) != 0))
   {
      const char *s;
      char *d;
      int  ln, lb;
      s = def;
      d = var;
      ln = strlen(newbase);
      lb = strlen(base);
      do
      {
         if (strncmp(s, base, lb) == 0)
         {
            strcpy(d, newbase);
            d += ln;
            s += lb;
         }
         else
         {
            *d = *s;
            d++;
            s++;
         }
      }
      while (*s);

   }
   else
   {
      strcpy(var, def);
   }
}

static int
cob_init_env_var(char *var, const char *env, const char *def, const char *base, const char *newbase)
{
   char *p = cob_getenv(env);
   int vfound = 0;

   if (p)
   {
      strcpy(var, p);
      vfound = 1;
   }
   else
   {
      cob_patch_var(var, def, base, newbase);
   }
   return vfound;
}

static void
cobc_check_action(const char *name)
{
   struct stat     st;

   if (name && !cobc_stat(name, &st))
   {
      if (!save_temps)
      {
         unlink(name);
      }
   }
}

static void
cobc_clean_up(int status)
{
   struct filename         *fn;
   struct local_filename   *lf;
   int                     i;
   char                    buff[COB_SMALL_BUFF];

   if (!save_temps)
   {
      for (fn = file_list; fn; fn = fn->next)
      {
         if (fn->need_preprocess
             && (status || cb_compile_level > CB_LEVEL_PREPROCESS))
         {
            if (!cb_flag_mf_int && !cb_keep_final_source)
            {
               if ((output_name == NULL) ||
                   (strcmp(output_name, fn->preprocess) != 0))
               {
                  cobc_check_action(fn->preprocess);
               }
            }
            cobc_check_action(fn->prepro_in);
            cobc_check_action(fn->prepro_out);
            cobc_check_action(fn->prepro_rwout);
         }
         if (!save_csrc && fn->need_translate
             && (status || cb_compile_level > CB_LEVEL_TRANSLATE))
         {
            cobc_check_action(fn->translate);
            cobc_check_action(fn->trstorage);
            if (fn->localfile)
            {
               for (lf = fn->localfile; lf; lf = lf->next)
               {
                  cobc_check_action(lf->local_name);
                  cobc_check_action(lf->register_name);
                  cobc_check_action(lf->static_name);
               }
            }
            else if (fn->translate)
            {
               /* If we get syntax errors, we do not
                  know the number of local include files */
               memset(buff, 0, sizeof(buff));
               for (i = 0; i < 30; i++)
               {
                  if (i)
                  {
                     sprintf(buff, "%s.l%d.h",
                             fn->translate, i);
                  }
                  else
                  {
                     sprintf(buff, "%s.l.h",
                             fn->translate);
                  }
                  unlink(buff);
               }
            }
            cobc_check_action(fn->trstorage);
            cobc_check_action(fn->trdebug);
            cobc_check_action(fn->trccmap);
            cobc_check_action(fn->trprofiling);
            cobc_check_action(fn->trfunction);
            cobc_check_action(fn->trglobal);
         }
         if (fn->need_assemble
             && (status || cb_compile_level > CB_LEVEL_ASSEMBLE))
         {
            cobc_check_action(fn->object);
         }
      }
   }
   for (fn = file_list; fn; fn = fn->next)
   {
      cobc_check_action(fn->objmain_lib);
   }
   if (cb_debugdb)
   {
      cb_debugdb = debugdb_closedb(cb_debugdb);
   }
}

static void
cobc_atexit(void)
{
   cobc_clean_up(exit_status);

}
/*
 * manage stat failure on linux
*/

int
cobc_exist_file(const char *name)
{

   struct stat     st;
   return (cobc_stat(name, &st) == 0);
}

int
cobc_stat(const char *name, struct stat     *st)
{
   int res = stat(name, st);
#  if defined(HAVE_ERRNO_H) && defined(EOVERFLOW)
   if (errno == EOVERFLOW) res = 0;
#  endif
   return res;
}

void
cobc_terminate(const char *str)
{
   fprintf(stderr, "cobc:0: ");
   fflush(stderr);
   perror(str);
   cobc_clean_up(1);
   exit(1);
}

#ifdef  HAVE_SIGNAL_H
static void
cobc_sig_handler(int sig)
{
   save_temps = 0;
   cobc_clean_up(1);
   switch (sig)
   {
#     ifdef SIGHUP
      case SIGHUP:
         if ((hupsig != SIG_IGN) && (hupsig != SIG_DFL))
         {
            (*hupsig)(SIGHUP);
         }
         break;
#     endif
#        ifdef SIGINT
      case SIGINT:
         if ((intsig != SIG_IGN) && (intsig != SIG_DFL))
         {
            (*intsig)(SIGINT);
         }
         break;
#        endif
#        ifdef SIGQUIT
      case SIGQUIT:
         if ((qutsig != SIG_IGN) && (qutsig != SIG_DFL))
         {
            (*qutsig)(SIGQUIT);
         }
         break;
#        endif
   }
   exit(sig);
}
#endif

/*
 * Command line
 */
static void
cobc_print_version(void)
{
   cob_print_version(COB_PN_COMPILER);
   printf("COBOLITDIR=%s\n", cobolit_dir);
   printf("COB_CC=%s\n", cob_cc);

   printf("Temporay dir=%s\n", cobc_tmpdir);
   if (cob_getenv("COBITOPT")) printf("COBITOPT=%s\n", cob_getenv("COBITOPT"));
}

static void
cobc_print_usage(void)
{
   printf("Usage: %s [options] file...\n\n", program_name);
   puts(_("Options:"));
   puts(_("  --help                Display this message"));
   puts(_("  --version, -V         Display compiler version"));
   puts(_("  -v                    Display the programs invoked by the compiler"));
   puts(_("  -x                    Build an executable program"));
   puts(_("  -m                    Build a dynamically loadable module (default)"));
   puts(_("  -std=<dialect>        Compile for a specific dialect :"));
   puts(_("                          cobol2002   Cobol 2002"));
   puts(_("                          cobol85     Cobol 85"));
   puts(_("                          ibm         IBM Compatible"));
   puts(_("                          mvs         MVS Compatible"));
   puts(_("                          bs2000      BS2000 Compatible"));
   puts(_("                          mf          Micro Focus Compatible"));
   puts(_("                          default     When not specified"));
   puts(_("                        See config/default.conf and config/*.conf"));
   puts(_("  -free                 Use free source format"));
   puts(_("  -fixed                Use fixed source format (default)"));
   puts(_("  -O                    Enable optimization"));
   puts(_("  -g                    Produce debugging information in the output (COBOL Level)"));
   puts(_("  -G                    Produce debugging information in the output (C Level)"));
   puts(_("  -debug                Enable all run-time error checking"));
   puts(_("  -o <file>             Place the output into <file>"));
   puts(_("  -b                    Combine all input files into a single"));
   puts(_("                        dynamically loadable module"));
   puts(_("  -E                    Preprocess only; do not compile, assemble or link"));
   puts(_("  -C                    Translation only; convert COBOL to C"));
   puts(_("  -S                    Compile only; output assembly file"));
   puts(_("  -c                    Compile and assemble, but do not link"));
   puts(_("  -t <file>             Generate and place a program listing into <file>"));
   puts(_("  -I <directory>        Add <directory> to copy/include search path"));
   puts(_("  -L <directory>        Add <directory> to library search path"));
   puts(_("  -R <directory>        Add <directory> to runtime library search path (if supported)"));
   puts(_("  -l <lib>              Link the library <lib>"));
   puts(_("  -D <define>           Pass <define> to the C compiler"));
   puts(_("  -Wc CC_opt            Pass CC_opt to C Compiler"));
   puts(_("  -Wl LD_opt            Pass LD_opt to Linker"));
   puts(_("  -conf=[+]<file>       User defined dialect configuration - See -std="));
   puts(_("                        If + prefix filename, the configuration is added"));
   puts(_("                        to current one"));
   puts(_("  --list-reserved       Display all reserved words"));
   puts(_("  --list-intrinsics     Display intrinsic functions"));
   puts(_("  --list-mnemonics      Display mnemonic names"));
   puts(_("  -save-temps(=<dir>)   Save intermediate files (default current directory)"));
   puts(_("  -linkage-desc=<file>  Produce an XML description of the linkage section"));
   puts(_("  -preprocess=<CMD>     Run CMD after cobol preprocessing (COPY/REPLACING)"));
   puts(_("                          as external preprocessor. The CMD is called with: "));
   puts(_("                          First arg = input file ; second arg = output file"));
   puts(_("  -MT <target>          Set target file used in dependency list"));
   puts(_("  -MF <file>            Place dependency list into <file>"));
   puts(_("  -ext <extension>      Add default file extension"));
   puts(_("  -use-extfh <NAME>     Use EXTFH Files Handler (optinal specifie custom <NAME>) "));
   puts(_("  -use-extsm <NAME>     Use EXTSM Sort Module (optinal specifie custom <NAME>) "));
   puts(_("  -err <file>           Write errors and warnings to <file> instaid of stderr  "));
   puts(_("  -constant \"key=value\" define constant <key> to <value> to be tested with $if  "));
   puts(_("  -makesyn \"key=value\" define word level synonym <key> = <value>  "));
   puts(_("  -codepage \"cp_id\"     Define the runtime code page to store PIC X(n)          "));
   puts(_("  -source-codepage \"cp_id\"  Define the code page of the source file (And so of the literals)            "));
#  ifdef WITH_ICU
   puts(_("  --list-codepage       List available code page             "));
   puts(_("  --check-codepage \"cp_id\" check the code page             "));
#  endif
   puts(_(""));
   puts(_("  -Wall                 Enable all warnings"));
#  undef CB_WARNDEF
#  define CB_WARNDEF(var,name,wall,doc,def)           \
        if (def) {                                  \
            printf ("  -W%-19s %s (default)\n", name, gettext (doc));\
        } else{\
            printf ("  -W%-19s %s\n", name, gettext (doc));\
        }
#  include "warning.def"
#  undef  CB_WARNDEF
   puts("");
#  undef CB_FLAG
#  define CB_FLAG(var,name,doc, def)                                     \
        if (doc) {                                                        \
            if (def) {\
                printf ("  -f%-19s %s (default)\n", name, gettext (doc));   \
            } else {                                                        \
                printf ("  -f%-19s %s\n", name, gettext (doc));             \
            }                                                               \
        }
#  include "flag.def"
#  undef  CB_FLAG
   puts("");
}

static void
cobc_options_error(void)
{
   fprintf(stderr, "cobc:0: Only one of options 'E', 'S', 'C' 'c' may be specified\n");
   exit(1);
}

static int
is_directory(char *option)
{
   struct stat             st;

#  ifdef _MSC_VER
   char *p;
   int i = strlen(option);
   if (i > 0)
   {
      p = &option[i - 1];
      while ((i > 0) &&
             ((*p == '/') || (*p == '\\')))
      {
         i--;
         *p = 0;
         p--;
      }
   }
   if (cobc_stat(option, &st) == 0 && (st.st_mode & _S_IFDIR))
   {
#  else
   if (cobc_stat(option, &st) == 0 && (S_ISDIR(st.st_mode)))
   {
#  endif
      return 1;
   }
   return 0;
}

static int
is_valid_file(char *option)
{
   struct stat             st;

#  ifdef _MSC_VER
   if (cobc_stat(option, &st) == 0 && (st.st_mode & _S_IFREG))
   {
#  else
   if (cobc_stat(option, &st) == 0 && (S_ISREG(st.st_mode)))
   {
#  endif
      return 1;
   }
   return 0;
}


static char *cobargv[1024];
static void process_conf(void);

static void process_opt(int c, char *arg)
{
   enum cob_exception_id   ec;
   char                    ext[COB_MEDIUM_BUFF];
   char *p;
   int i;
   switch (c)
   {
      case 0:
         break;
      case '?':
         exit(1);
      case 'h':
         cobc_print_usage();
         exit(0);
      case 'V':
         cobc_print_version();
         exit(0);
      case '9':
         cb_list_reserved();
         exit(0);
      case 'Q':
         cb_list_intrinsics();
         exit(0);
      case 'q':
         cb_list_mnemonics();
         exit(0);
      case '3': /* -constant */
         cobc_add_constant(strdup(arg), 0, 1);
         break;
      case 304: /* -makesyn */
         cobc_add_constant(strdup(arg), 1, 1);
         break;
      case 305: /* -dump-config */
         cb_dump_config = 1;
         break;
      case 306:
         cob_list_codepage();
         exit(0);
         break;
      case 307:
         cob_check_codepage(arg);
         exit(0);
         break;
      case 308:
         cb_initcall_list = cb_text_list_add(cb_initcall_list, arg);
         break;
      case 309:
         if (is_directory(arg))
         {
            cb_flag_use_debugdb = 1;
            output_debugdb_dir = strdup(arg);
         }
         else
         {
            cb_debugdb = debugdb_opendb(arg, 1);
            if (!cb_debugdb)
            {
               cobc_terminate(arg);
            }
            cb_debugdb_name = strdup(arg);
         }
         break;
      case 'E': /* -E */
         if (wants_nonfinal)
         {
            cobc_options_error();
         }
         wants_nonfinal = 1;
         cb_compile_level = CB_LEVEL_PREPROCESS;
         break;
      case 'C': /* -C */
         if (wants_nonfinal)
         {
            cobc_options_error();
         }
         wants_nonfinal = 1;
         cb_compile_level = CB_LEVEL_TRANSLATE;
         break;
      case 'S': /* -S */
         if (wants_nonfinal)
         {
            cobc_options_error();
         }
         wants_nonfinal = 1;
         cb_compile_level = CB_LEVEL_COMPILE;
         break;
      case 'c': /* -c */
         if (wants_nonfinal)
         {
            cobc_options_error();
         }
         wants_nonfinal = 1;
         cb_flag_main = 0;
         cb_flag_module = 0;
         cb_flag_library = 0;
         cb_compile_level = CB_LEVEL_ASSEMBLE;
         break;
      case 'b': /* -b */
         if (cb_flag_main || cb_flag_module)
         {
            fprintf(stderr, "cobc:0: Only one of options 'm', 'x', 'b' may be specified\n");
            exit(1);
         }
         cb_flag_main = 0;
         cb_flag_module = 0;
         cb_flag_library = 1;
         break;
      case 'm': /* -m */
         if (cb_flag_main || cb_flag_library)
         {
            fprintf(stderr, "cobc:0: Only one of options 'm', 'x', 'b' may be specified\n");
            exit(1);
         }
         cb_flag_main = 0;
         cb_flag_module = 1;
         cb_flag_library = 0;
         break;
      case 'x': /* -x */
         if (cb_flag_module || cb_flag_library)
         {
            fprintf(stderr, "cobc:0: Only one of options 'm', 'x', 'b' may be specified\n");
            exit(1);
         }
         cb_flag_main = 1;
         cb_flag_module = 0;
         cb_flag_library = 0;
         break;
      case 'v': /* -verbose */
         verbose_output = 1;
         break;
      case 'p': /* -preprocess */
         ext_prepro_name = strdup(arg);
         cb_flag_split_debug_mark = 1;
         break;
      case 'o': /* -o */
         output_name = NULL;
         output_dir = NULL;
         if (is_directory(arg))
         {
            output_dir = strdup(arg);
         }
         else
         {
            output_name = strdup(arg);
         }
         break;
      case 'a': /* -linkage-desc */
         linkage_descriptor_name = strdup(arg);
         break;
      case 'O': /* -O */
         if (cb_optimize_size)
         {
            strcat(cob_cflags, " ");
            strcat(cob_cflags, cob_sflag);
            strcat(cob_cflags, " ");
         }
         optimize_flag = 1;
         cb_flag_binary_optimize = 1;
         cb_flag_cmp_optimize = 1;
         cb_flag_cmp_inline = 1;
         cb_flag_index_optimize = 1;
         cb_flag_decimal_optimize = 1;
         cb_flag_fp_optimize = 1;
         cb_flag_optimize_move = 1;
         cb_flag_optimize_move_call = 1;
         cb_flag_fast_operation = 1;
         break;
      case 312: /* -O1  */
         if (cb_optimize_size)
         {
            strcat(cob_cflags, " ");
            strcat(cob_cflags, cob_sflag);
            strcat(cob_cflags, " ");
         }
         strcat(cob_cflags, " -O ");
         optimize_flag = 1;
         cb_flag_binary_optimize = 1;
         cb_flag_cmp_optimize = 1;
         cb_flag_cmp_inline = 1;
         cb_flag_index_optimize = 1;
         cb_flag_decimal_optimize = 1;
         cb_flag_fp_optimize = 1;
         cb_flag_optimize_move = 1;
         cb_flag_optimize_move_call = 1;
         cb_flag_fast_operation = 1;
         break;

      case '2':
         strcat(cob_cflags, " ");
         strcat(cob_cflags, cob_Oflag);
         strcat(cob_cflags, " ");
         strcat(cob_cflags, fcopts);
         strcat(cob_cflags, " ");
         strcat(cob_cflags, cob_eflag);
         optimize_flag = 1;
         cb_flag_binary_optimize = 1;
         cb_flag_cmp_inline = 1;
         cb_flag_cmp_optimize = 1;
         cb_flag_index_optimize = 1;
         cb_flag_decimal_optimize = 1;
         cb_flag_fp_optimize = 1;
         cb_flag_optimize_move = 1;
         cb_flag_optimize_move_call = 1;
         cb_flag_fast_operation = 1;
         break;

      case '1':  /* -Os */
         cb_flag_binary_optimize = 1;
         cb_optimize_size = 1;
         cb_flag_auto_load_static = 1;
         break;

      case '0':
         optimize_flag = 0;
         cb_flag_call_optimize = 0;
         cb_flag_binary_optimize = 0;
         cb_flag_optimize_move = 0;
         cb_flag_cmp_optimize = 0;
         cb_flag_fp_optimize = 0;
         cb_flag_optimize_move = 0;
         cb_flag_optimize_move_call = 0;
         cb_flag_fast_operation = 0;
         cb_flag_77_optimize = 0;
         break;

      case 'G':
         save_csrc = 1;
         cb_flag_nostrip = 1;
#        ifndef _MSC_VER
         strcat(cob_cflags, " -g");
         strcat(cob_ldflags, " -g");
#        endif
         break;

      case 'g':
         cb_flag_stack_check = 1;
         cb_flag_source_location = 1;
         cb_flag_mem_info = 1;
         cb_flag_debuginfo = 1;
         break;

      case '4':       /* -debug */
         /* Turn on all exception conditions */
         for (ec = 1; ec < COB_EC_MAX; ec++)
         {
            CB_EXCEPTION_ENABLE(ec) = 1;
         }
         cb_flag_source_location = 1;
         cb_flag_stack_check = 1;
         cb_flag_mem_info = 1;
         cb_flag_runtimecheck = 1;
         break;
      case '_':   /* --save-temps */
         save_temps = 1;
         /* No break*/
      case 310:   /* -temps-dir*/
         if (arg)
         {
            if (!is_directory(arg))
            {
               fprintf(stderr, "cobc:0: Warning - '%s' is not a directory, defaulting to current directory\n", arg);
               fflush(stderr);
            }
            else
            {
               save_temps_dir = strdup(arg);
            }
         }
         break;
      case 313:   /* --xdd-prefix*/
         if (arg)
         {
            cb_xdd_prefix = strdup(arg);
         }
         break;
      case ')':   /* --code-cover */

         code_cover = 1;
         break;

      case 't':
         if (is_directory(arg))
         {
            listing_dir = strdup(arg);
         }
         else
         {
            listing_name = strdup(arg);
         }
         break;

      case 'D':       /* -D */
#        ifdef _MSC_VER
         strcat(cob_define_flags, "/D \"");
         strcat(cob_define_flags, arg);
         strcat(cob_define_flags, "\" ");
#        else
         strcat(cob_define_flags, "-D");
         strcat(cob_define_flags, arg);
         strcat(cob_define_flags, " ");
#        endif
         break;

      case '%':       /* -MT */
         cb_depend_target = strdup(arg);
         break;

      case '@':       /* -MF */

         cb_depend_file = fopen(arg, "w");
         if (!cb_depend_file)
         {
            perror(arg);
         }
         break;
      case '#':       /* -use_extfh*/
         cb_flag_all_extfh = 1;
         if (arg)
         {
            cb_extfh_name = strdup(arg);
         }
         break;
      case '*':       /* -use_extsm*/
         cb_flag_extsm = 1;
         cb_extsm_name =  strdup(arg);
         break;
      case '(':       /* -err*/
         if (arg)
         {
            int so;
            unlink(arg);
            so = open(arg, O_CREAT | O_WRONLY, 0666);
            if (so < 0)
            {
               fprintf(stderr, "cobc:0: Error: Can't open '%s' for write\n", arg);
               exit(1);
            }
            dup2(so, fileno(stderr));
            dup2(so, fileno(stdout));
         }
         break;
      case 'I':
         if (is_valid_file(arg))
         {
            cb_include_list = cb_include_list_file_add(cb_include_list, arg);
         }
         else
         {
            cb_include_list = cb_include_list_add(cb_include_list, arg);
            p = strchr(arg, ',');
            if (p)
            {
               i = p - arg;
            }
            else
            {
               i = strlen(arg);
            }
#           ifdef _MSC_VER
            strcat(cob_define_flags, "/I \"");
            strncat(cob_define_flags, arg, i);
            strcat(cob_define_flags, "\" ");
#           else
            strcat(cob_define_flags, "-I");
            strncat(cob_define_flags, arg, i);
            strcat(cob_define_flags, " ");
#           endif
         }
         break;

      case 'L':
#        ifdef _MSC_VER
         strcat(cob_libs, " /LIBPATH:");
#        else
         strcat(cob_libs, " -L");
#        endif
         strcat(cob_libs, arg);
         break;

      case 'l':
#        ifdef _MSC_VER
         strcat(cob_libs, " ");
#        else
         strcat(cob_libs, " -l");
#        endif
         strcat(cob_libs, arg);
         break;

      case 'r':
         cb_reg_file_name = strdup(arg);
         break;
      case 'R':
         strcat(cob_libs, " -Wl,-R");
         strcat(cob_libs, arg);
         break;

      case 'e':
         sprintf(ext, ".%s", arg);
         cb_extension_list = cb_text_list_add(cb_extension_list, ext);
         break;

      case 'w':
#        undef CB_WARNDEF
#        define CB_WARNDEF(var,name,wall,doc,def)   var = 0;
#        include "warning.def"
#        undef  CB_WARNDEF
         cb_disable_all_warning = 1;
         break;

      case 'W':
#        undef CB_WARNDEF
#        define CB_WARNDEF(var,name,wall,doc,def)   if (wall) var = 1;
#        include "warning.def"
#        undef  CB_WARNDEF
         cb_disable_all_warning = 0;
         break;

      case '6':
#        undef CB_WARNDEF
#        define CB_WARNDEF(var,name,wall,doc,def)   if (wall) var = 0;
#        include "warning.def"
#        undef  CB_WARNDEF
         cb_disable_all_warning = 0;
         break;

      case '8':
         if (arg)
         {
            strcat(cob_cflags, " ");
            strcat(cob_cflags, arg);
         }
         break;
      case '7':
         if (arg)
         {
#           ifdef _MSC_VER
            strcat(cob_libs, " ");
#           else
            strcat(cob_libs, " -Wl,");
#           endif
            strcat(cob_libs, arg);
         }
         break;
      case '[': /* -codepage*/
         cb_codepage = strdup(arg);
         break;
      case ']': /* -source-codepage*/
         cb_source_codepage = strdup(arg);
         break;
      case 301:    /* {"sysin" , required_argument,       NULL,           '?'},*/
         cb_sysin_redirect = strdup(arg);
         break;
      case 302:    /* {"sysout", required_argument,       NULL,           '?'},*/
         cb_sysout_redirect = strdup(arg);
         break;
      case 303:    /* {"syserr", required_argument,       NULL,           '?'},*/
         cb_syserr_redirect = strdup(arg);
         break;
      case 400:    /* {"syserr", required_argument,       NULL,           '?'},*/
         cb_sysprint_redirect = strdup(arg);
         break;
      case '$':       /* -std */
         if (cb_load_std(arg) != 0)
         {
            fprintf(stderr, _("cobc:0: Invalid option -std=%s\n"), arg);
            exit(1);
         }
         process_conf();
         break;
      case '&':       /* -conf */
         if (arg[0] == '+')
         {
            if (cb_load_conf(&arg[1], 0, 1) != 0)
            {
               exit(1);
            }
         }
         else
         {
            if (cb_load_conf(arg, 1, 1) != 0)
            {
               exit(1);
            }
         }
         process_conf();
         break;
      default:
         ABORT();
   }
}

static void process_conf(void)
{
#  undef CB_LONGOPT_BOOLEAN
#  undef CB_LONGOPT_SET
#  undef CB_LONGOPT_STRING
#  undef CB_LONGOPT_SHORT
#  define CB_LONGOPT_BOOLEAN(name, val, var) if (var) process_opt(val, NULL);
#  define CB_LONGOPT_SET(name, var, setval, setvar) if (var) setvar = setval;
#  define CB_LONGOPT_STRING(name, val, var)  if (var) process_opt(val, var);
#  define CB_LONGOPT_OPT(name, val, var)  if (var) process_opt(val, var);
#  define CB_LONGOPT_SHORT(name, val, var)  if (var) process_opt(val, var);
#  include "longopt.def"
#  undef CB_LONGOPT_BOOLEAN
#  undef CB_LONGOPT_SET
#  undef CB_LONGOPT_STRING
#  undef CB_LONGOPT_OPT
#  undef CB_LONGOPT_SHORT
}

static int
process_command_line(int *cmdargc, char **cmdargv[])
{
   int                     c, idx, cmdoffset = 0;
   char                    buff[COB_MEDIUM_BUFF];
   char                    *p, *s;
   char                    *cobitopt,  *cobopt;
   int                     cobargc = 0;
   int                     i;


   /* ADD COBITOPT options*/
   cobargv[0] = (*cmdargv)[0];
   cobargc++;
   cobitopt = cob_getenv("COBITOPT");
   if (cobitopt)
   {
      cobopt = strdup(cobitopt);
      p = strtok(cobopt, " \t");
      while (p)
      {
         cobargv[cobargc] = p;
         cobargc++;
         cmdoffset++;
         p = strtok(NULL, " \t");
         if (cobargc >= 1024)
         {
            fprintf(stderr, "More than 1024 command line option. abend\n");
            exit(-2);
         }
      }

   }
   for (i = 1; i < *cmdargc; i++)
   {
      cobargv[cobargc] = (*cmdargv)[i];
      cobargc++;
      if (cobargc >= 1024)
      {
         fprintf(stderr, "More than 1024 command line option. abend\n");
         exit(-2);
      }

   }
#  ifdef __sparc__
   cb_flag_align_8 = 1;
#  endif
#  ifdef __sparc
   cb_flag_align_8 = 1;
#  endif
#  ifdef __ia64
   cb_flag_align_8 = 1;
#  endif
#  ifdef __hppa
   cb_flag_align_8 = 1;
#  endif
#  ifdef COB_PTR_64BITS
   cb_flag_align_8 = 1;
#  endif
   /*cb_flag_initalize_filler=1;*/
   /* Enable default I/O exceptions */
   CB_EXCEPTION_ENABLE(COB_EC_I_O) = 1;
   cb_load_conf("isam.conf", 0, 0);
   cb_load_std("default");

   /* Conditional compilation constant X0->X9*/
   for (i = 0; i < 10; i++)
   {
      sprintf(buff, "X%d=OFF", i);
      cobc_add_constant(strdup(buff), 0, 0);
   }

   while ((c = cit_getopt(cobargc, cobargv, short_options, long_options, &idx, 1)) >= 0)
   {
      /* 
      if ( 1 || verbose_output ) {
          fprintf (stderr, "cobc:0: option : %c %s \n", c, cit_optarg);
      } 
      */
      process_opt(c, cit_optarg);
   }

   if (cb_flag_link_only)
   {
      cb_flag_build_main = 0;
   }
   if (cb_flag_main && cb_flag_build_main)
   {
      cb_flag_please_include_main = 1;
   }
   if (cb_flag_auto_load_static)
   {
#     ifndef _MSC_VER
      cb_load_conf("static.symb", 0, 0);
#     else
      cb_load_conf("staticwin.symb", 0, 0);
#     endif
      cb_load_conf("user.symb", 0, 0);
   }
   if (cb_first_tab_width == 0) cb_first_tab_width = cb_tab_width;
   if (cb_dump_config)
   {
      cobc_dump_config(stdout, 0, (char *)"");
   }
   if ((save_csrc || save_temps || cb_flag_source_location) && (save_temps_dir ==  NULL))
   {
      save_temps_dir = (char *)"c";
#     ifdef  _WIN32
      mkdir(save_temps_dir);
#     else
      mkdir(save_temps_dir, 0777);
#     endif
      if (!is_directory(save_temps_dir))
      {
         save_temps_dir = (char *)".";
      }
   }
   if (save_temps_dir ==  NULL)
   {
      save_temps_dir = (char *)".";
   }

#  ifndef _MSC_VER
#     if HAVE_REALPATH
   if (!cb_flag_no_realpath)
   {
      char *q = cobc_malloc(PATH_MAX);
      p = save_temps_dir;
      save_temps_dir = realpath(p, q);
      if (save_temps_dir == NULL)
      {
         cobc_terminate(p);
      }
   }
#     else
   cb_flag_no_realpath = 1;
#     endif
#  else
   p = calloc(1, PATH_MAX);
   _fullpath(p, save_temps_dir, PATH_MAX);
   save_temps_dir = p;
#  endif
   if (cb_flag_no_realpath)
   {
      strcat(cob_cflags, " -I.");
   }
   if (verbose_output)
   {
      if (cobitopt)
      {
         fprintf(stderr, "COBITOPT=%s\n", cobitopt);
      }
      fprintf(stderr, "cobc:0: Temps files to '%s'\n", save_temps_dir);
   }

   if (cb_config_name == NULL)
   {
      if (cb_load_std("default") != 0)
      {
         fprintf(stderr, "cobc:0: Error: failed to load the initial config file\n");
         exit(1);
      }
   }

   if (cb_flag_fold_copy_lower && cb_flag_fold_copy_upper)
   {
      fprintf(stderr, "cobc:0: Error: Invalid option combination\n");
      exit(1);
   }
   if (cb_compile_level == CB_LEVEL_PREPROCESS && cb_flag_syntax_only)
   {
      fprintf(stderr, "cobc:0: Error: -E & -fsyntax-only are exclusive\n");
      exit(1);
   }
   if (cob_cc_is_gcc)
   {
      /*
      strcat (cob_cflags, " -Wno-unused -fsigned-char");
  #ifdef  HAVE_PSIGN_OPT
      strcat (cob_cflags, " -Wno-pointer-sign");
  #endif     
      */
      strcat(cob_cflags, " -w -fsigned-char");
   }

   if (cb_flag_nostrip)
   {
      strip_output = 0;
   }
   if (cb_flag_traceall)
   {
      cb_flag_trace = 1;
      cb_flag_source_location = 1;
      cb_flag_mem_info = 1;
   }
   if (cob_cc_is_gcc)
   {
      if (strip_output)
      {
         strcat(cob_cflags, " -fomit-frame-pointer");
      }
   }
   if (cb_flag_cics)
   {
      cb_flag_mf_gnt = 1;
   }
   if (cb_flag_context_reload_enable)
   {
      cb_flag_goto_gcc = 0;
   }
   /* default extension list */
   cb_extension_list = cb_text_list_add(cb_extension_list, ".CPY");
   cb_extension_list = cb_text_list_add(cb_extension_list, ".CBL");
   cb_extension_list = cb_text_list_add(cb_extension_list, ".COB");
   cb_extension_list = cb_text_list_add(cb_extension_list, ".cpy");
   cb_extension_list = cb_text_list_add(cb_extension_list, ".cbl");
   cb_extension_list = cb_text_list_add(cb_extension_list, ".cob");
   cb_extension_list = cb_text_list_add(cb_extension_list, "");

   if (cb_flag_list_sources)
   {
      cb_text_column = 9999;
   }
   if (cb_utf8_sources)
   {
      if (cb_source_codepage && strcasecmp(cb_source_codepage, "UTF-8") != 0)
      {
         fprintf(stderr, "cobc:0: Error: -futf8 and -source-codepage are mutualy exclusive \n");
         exit(1);
      }
      cb_source_codepage = (char *)"UTF-8";
   }
   else if (cb_source_codepage &&
            ((strcasecmp(cb_source_codepage, "UTF-8") == 0) ||
             (strcasecmp(cb_source_codepage, "UTF8") == 0)))
   {
      cb_utf8_sources = 1;
   }

#  if !defined(CIT_EBCDIC_CHARSET)
   if (cb_flag_ebcdic_charset)
   {
      fprintf(stderr, "cobc:0: Error: -febcdic-charset not available in this version \n");
      exit(1);
   }
   if (!cb_codepage && cb_source_codepage)
   {
      cb_codepage = cb_source_codepage;
   }
#  else
   if (!cb_codepage)
   {
      if (cb_flag_ebcdic_charset) cb_codepage = (char *)"IBM-1147";
      else
      {
         if (cb_source_codepage)
         {
            cb_codepage = cb_source_codepage;
         }
         else
         {
            cb_codepage = (char *)"ASCII";
         }
      }
   }
#  endif
   enterprise_codepage_init((char *)cb_codepage, (char *)cb_source_codepage);
   if (cb_isamextfh_lib)
   {
      strcat(cob_libs, " ");
      strcat(cob_libs, cb_isamextfh_lib);
   }

   if (cb_flatextfh_lib)
   {
      strcat(cob_libs, " ");
      strcat(cob_libs, cb_flatextfh_lib);
   }

#  ifndef COB_HAS_THREAD
   if (cb_flag_thread_safe)
   {
      fprintf(stderr, "cobc:0: Error: -fthread-safe Thread safe code unsuported on this platform\n");
      exit(1);
   }
#  endif
#  ifdef _MSC_VER
   if (cb_flag_thread_safe)
   {
      OSVERSIONINFO osvi = { 0 };
      osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      GetVersionEx(&osvi);
      if (osvi.dwMajorVersion < 6)
      {
         fprintf(stderr, "cobc:0: Warning: -fthread-safe Thread safe code supported on Windows Vista and upper\n");
         /*exit (1);*/
      }
   }
#  endif
   if (cb_move_xto9_mode == CB_MOVExTO9_MF50 ||  cb_flag_displaynumeric_mf50)
   {
      if (cb_move_spaces_to_displaynumeric == CB_MOVEspTO9_ERROR)
      {
         cb_move_spaces_to_displaynumeric = CB_MOVEspTO9_ZERO;
      }
   }
   if (verbose_output)
   {
      if (cb_codepage)
      {
         printf("storage codepage = %s\n", cb_codepage);
      }
      if (cb_source_codepage)
      {
         printf("sources codepage = %s\n", cb_source_codepage);
      }
   }
   if (code_cover && !cb_debugdb)
   {
      cb_flag_use_debugdb = 1;
   }

   if (cb_flag_source_location && cb_flag_debug_exec && ext_prepro_name)
   {
      save_temps = 1;
   }
   s = cob_getenv("COB_USE_CTREE");
   if (s)
   {
      cb_flag_use_ctree = 1;
   }

   if (cb_flag_use_vbisam && cb_flag_use_disam)
   {
      fprintf(stderr, "cobc:0: Error: -fdisam and -fvbisam are exclusive\n");
      exit(1);
   }

   if (cb_flag_use_disam)
   {
      cb_extfh_name = (char *)"DISAMEXTFH";
      cb_flag_all_extfh = 1;
      if (!cb_isamextfh_lib)
      {
#        ifdef _MSC_VER
         cb_isamextfh_lib = (char *)"disamextfh_dll.lib ";
            strcat (cob_libs, " \"");
         strcat(cob_libs, cobolit_dir);
            strcat (cob_libs, "/lib/disamextfh_dll.lib\" ");
#        else
         cb_isamextfh_lib = (char *)"-ldisamextfh ";
         strcat(cob_libs, " ");
         strcat(cob_libs, cb_isamextfh_lib);
#        endif
      }
   }
   else if (cb_flag_use_vbisam)
   {
      cb_extfh_name = (char *)"VBISAMEXTFH";
      cb_flag_all_extfh = 1;
      if (!cb_isamextfh_lib)
      {
#        ifdef _MSC_VER
         cb_isamextfh_lib = (char *)"vbisamextfh_dll.lib ";
            strcat (cob_libs, " \"");
         strcat(cob_libs, cobolit_dir);
            strcat (cob_libs, "/lib/vbisamextfh_dll.lib\" ");
#        else
         cb_isamextfh_lib = (char *)"-lvbisamextfh ";
         strcat(cob_libs, " ");
         strcat(cob_libs, cb_isamextfh_lib);
#        endif
      }
   }
   if (cb_flag_all_extfh)
   {
      if (!cb_flat_extfh)
      {
         cb_flat_extfh = cb_extfh_name;
      }
      if (!cb_isam_extfh)
      {
         cb_isam_extfh = cb_extfh_name;
      }
   }

   *cmdargv = cobargv;
   *cmdargc = cobargc;
   if (cb_flag_quote_char && *cb_flag_quote_char)
   {
      cb_quote_char = *cb_flag_quote_char;
   }
   return (cit_optind);
}

static void
process_env_copy_path(void)
{
   char    *value;
   char    *token;
   struct cb_text_list *l = NULL;

   cobcpy = cob_getenv("COBCPY");
   if (cobcpy == NULL || strlen(cobcpy) == 0)
   {
      /* env. not defined: nothing to do */
      cobcpy = NULL;
      return;
   }

   /* cloning value to avoid memory corruption */
   value = strdup(cobcpy);

   /* tokenizing for path sep. */
   token = strtok(value, PATHSEPS);
   while (token)
   {
      l = cb_text_list_add(l, token);
      token = strtok(NULL, PATHSEPS);
   }
   /* do it in 2 step because of strtok in cb_include_list_add*/
   while (l)
   {
      cb_include_list = cb_include_list_add(cb_include_list, l->text);
      l = l->next;
   }

   /* releasing memory of clone(s) */
   free(value);
   return;
}

static void
file_basename(const char *filename, char *buff)
{
#  ifndef _MSC_VER
   size_t          len;
   const char      *startp, *endp;

   /* Remove directory name */
   startp = strrchr(filename, '/');
   if (startp)
   {
      startp++;
   }
   else
   {
      startp = filename;
   }

   /* Remove extension */
   endp = strrchr(filename, '.');
   if (endp > startp)
   {
      len = endp - startp;
   }
   else
   {
      len = strlen(startp);
   }

   /* Copy base name */
   strncpy(buff, startp, len);
   buff[len] = '\0';
#  else
   _splitpath(filename, NULL, NULL, buff, NULL);
#  endif
}

static const char*
file_extension(const char *filename)
{
   const char *p = strrchr(filename, '.');

   if (p)
   {
      return (p + 1);
   }
   else
   {
      return ("");
   }
}

char*
cobc_temp_name(const char *prefix, const char *ext)
{
   char    buff[COB_MEDIUM_BUFF];
#  ifdef _WIN32
   char    buff2[COB_MEDIUM_BUFF];
   char    temp[MAX_PATH];

   GetTempPath(MAX_PATH, temp);
   GetTempFileName(temp, prefix, 0, buff2);
   DeleteFile(buff2);
   buff2[strlen(buff2) - 4] = 0; /* remove .tmp*/
   DeleteFile(buff2);
   cob_iteration++;
   sprintf(buff, "%s%d_%d%s", buff2, cob_process_id, cob_iteration, ext);
#  else
   cob_iteration++;
   sprintf(buff, "%s/%s%d_%d%s", cobc_tmpdir, prefix, cob_process_id, cob_iteration, ext);
#  endif
   return (strdup(buff));
}

static struct filename*
process_filename(const char *request_filename)
{
   const char      *extension;
   struct filename *fn;
   struct filename *ffn;
   struct stat     st;
   char            basename[COB_SMALL_BUFF];
   char            outbasename[COB_SMALL_BUFF];
   char            realname[PATH_MAX];
   const char      *filename;

#  ifndef _MSC_VER
#     if HAVE_REALPATH
   if (!cb_flag_no_realpath)
   {
      filename = realpath(request_filename, realname);
      if (filename == NULL)
      {
         cobc_terminate(request_filename);
      }
   }
   else
   {
      filename = request_filename;
   }
#     else
   filename = request_filename;
#     endif
#  else
   char            fullname[COB_SMALL_BUFF];

   if (!cb_flag_no_realpath)
   {
      _fullpath(fullname, request_filename,  COB_SMALL_BUFF);
      filename = fullname;
   }
   else
   {
      filename = request_filename;
   }

#  endif

   file_basename(filename, basename);
   if (cobc_check_valid_name(basename))
   {
      fprintf(stderr, "cobc:0: Invalid file base name - %s\n", basename);
      return (NULL);
   }
   fn = cobc_malloc(sizeof(struct filename));
   fn->need_preprocess = 1;
   fn->need_translate = 1;
   fn->need_assemble = 1;
   fn->next = NULL;

   if (!file_list)
   {
      file_list = fn;
   }
   else
   {
      for (ffn = file_list; ffn->next; ffn = ffn->next)
      ;
      ffn->next = fn;
   }

   fn->demangle_source =  cb_encode_program_id(basename);
   extension = file_extension(filename);

   /* Check input file type */
   if (strcmp(extension, "i") == 0 ||
       strcmp(extension, "int") == 0)
   {
      /* already preprocessed */
      fn->need_preprocess = 0;
   }
   else if (strcmp(extension, "c") == 0 || strcmp(extension, "s") == 0)
   {
      /* already compiled */
      fn->need_preprocess = 0;
      fn->need_translate = 0;
   }
   else
#     ifdef _MSC_VER
      if (stricmp(extension, "obj") == 0 ||
          stricmp(extension, "gnt") == 0 ||
          stricmp(extension, "cit") == 0 ||
          stricmp(extension, "lib") == 0)
#     else
      if (strcmp(extension, "o") == 0 ||
          strcmp(extension, "a") == 0 ||
          strcmp(extension, "sl") == 0 ||
          strcmp(extension, "so") == 0 ||
          strcmp(extension, "obj") == 0 ||
          strcmp(extension, "cit") == 0 ||
          strcmp(extension, "gnt") == 0)
#     endif
         {
            /* already assembled */
            fn->need_preprocess = 0;
            fn->need_translate = 0;
            fn->need_assemble = 0;
         }

   if ((fn->need_preprocess || fn->need_translate || fn->need_assemble) &&
       cobc_stat(filename, &st) != 0)
   {
      cobc_terminate(filename);
   }
/* Set source filename */
   fn->source = cobc_malloc(strlen(filename) + 10);
   strcpy(fn->source, filename);

/* Set preprocess filename */
   if (!fn->need_preprocess)
   {
      fn->preprocess = strdup(fn->source);
   }
   else if ((output_dir || output_name) && cb_compile_level == CB_LEVEL_PREPROCESS)
   {
      if (output_dir)
      {
         fn->preprocess = cobc_malloc(strlen(basename) + 10 + strlen(output_dir));
         sprintf(fn->preprocess, "%s/%s.i", output_dir, basename);
      }
      else
      {
         fn->preprocess = strdup(output_name);
      }
   }
   else if (save_temps || cb_flag_mf_int || cb_flag_source_location)
   {
      fn->preprocess = cobc_malloc(strlen(basename) + 10 + strlen(save_temps_dir));
      if (cb_flag_mf_int)
      {
         sprintf(fn->preprocess, "%s/%s.int", save_temps_dir, basename);
      }
      else
      {
         sprintf(fn->preprocess, "%s/%s.i", save_temps_dir, basename);
      }
   }
   else
   {
      fn->preprocess = cobc_temp_name("cob", ".cob");
   }

/* external preprocessor*/
   if (save_temps)
   {
      fn->prepro_in = cobc_malloc(strlen(basename) + 10 + strlen(save_temps_dir));
      sprintf(fn->prepro_in, "%s/%s.i0", save_temps_dir, basename);
      fn->prepro_out = cobc_malloc(strlen(basename) + 10 + strlen(save_temps_dir));
      sprintf(fn->prepro_out, "%s/%s.i1", save_temps_dir, basename);
   }
   else
   {
      fn->prepro_in = cobc_temp_name("ppin", ".cob");
      fn->prepro_out = cobc_temp_name("ppout", ".cob");
   }
/* report writer files*/
   if (save_temps)
   {
      fn->prepro_rwout = cobc_malloc(strlen(basename) + 10 + strlen(save_temps_dir));
      sprintf(fn->prepro_rwout, "%s/%s.rw", save_temps_dir, basename);
   }
   else
   {
      fn->prepro_rwout = cobc_temp_name("rwout", ".cob");
   }

/* Set translate filename */
   if (!fn->need_translate)
   {
      fn->translate = strdup(fn->source);
   }
   else if ((output_dir || output_name) && cb_compile_level == CB_LEVEL_TRANSLATE)
   {
      if (output_dir)
      {
         fn->translate = cobc_malloc(strlen(basename) + 10 + strlen(output_dir));
         sprintf(fn->translate, "%s/%s.c", output_dir, basename);
      }
      else
      {
         fn->translate = strdup(output_name);
      }
   }
   else if (save_csrc || save_temps ||
            cb_compile_level == CB_LEVEL_TRANSLATE)
   {
      fn->translate = cobc_malloc(strlen(basename) + 10 + strlen(save_temps_dir));
      sprintf(fn->translate, "%s/%s.c", save_temps_dir, basename);
   }
   else
   {
      fn->translate = cobc_temp_name("cob", ".c");
   }

/* Set storage filename */
   if (fn->need_translate)
   {
      fn->trstorage = cobc_malloc(strlen(fn->translate) + 10);
      sprintf(fn->trstorage, "%s.h", fn->translate);
   }

/* Set object filename */
   if (output_dir)
   {
      sprintf(outbasename, "%s/%s", output_dir, basename);
   }
   else
   {
      strcpy(outbasename,  basename);
   }
/*rw listing*/
   if (listing_dir)
   {
      fn->prepro_rwlst = cobc_malloc(strlen(basename) + 10 + strlen(save_temps_dir));
      sprintf(fn->prepro_rwlst, "%s/%s.rw.lst", listing_dir, basename);
   }
   else if (output_dir)
   {
      fn->prepro_rwlst = cobc_malloc(strlen(basename) + 10 + strlen(output_dir));
      sprintf(fn->prepro_rwlst, "%s/%s.rw.lst", output_dir, basename);
   }
   else
   {
      fn->prepro_rwlst = cobc_malloc(strlen(outbasename) + 10);
      sprintf(fn->prepro_rwlst, "%s.rw.lst", outbasename);
   }

   if (!fn->need_assemble)
   {
      fn->object = strdup(fn->source);
   }
   else if ((output_dir || output_name) && cb_compile_level == CB_LEVEL_ASSEMBLE)
   {
      if (output_dir)
      {
         fn->object = cobc_malloc(strlen(outbasename) + 10);
#        ifdef  _MSC_VER
         sprintf(fn->object, "%s.obj", outbasename);
#        else
         sprintf(fn->object, "%s.o", outbasename);
#        endif
      }
      else
      {
         fn->object = strdup(output_name);
      }
   }
   else if (save_temps || cb_compile_level == CB_LEVEL_ASSEMBLE)
   {
      fn->object = cobc_malloc(strlen(outbasename) + 10);
#     ifdef  _MSC_VER
      sprintf(fn->object, "%s.obj", outbasename);
#     else
      sprintf(fn->object, "%s.o", outbasename);
#     endif
   }
   else
   {
#     ifdef  _MSC_VER
      fn->object = cobc_malloc(strlen(outbasename) + 10);
      sprintf(fn->object, "%s.obj", outbasename);
#     else
      fn->object = cobc_temp_name("cob", ".o");
#     endif
   }
   if (listing_dir)
   {
      fn->listing = cobc_malloc(strlen(basename) + 10 + strlen(listing_dir));
      sprintf(fn->listing, "%s/%s.lst", listing_dir, basename);
   }
   else if (listing_name)
   {
      fn->listing = listing_name;
   }

   return (fn);
}

static int
process(const char *cmd)
{
   char    *p;
   char    *buffptr = NULL;
   size_t  clen;
   int     ret;

   if (strchr(cmd, '$') == NULL)
   {
      if (verbose_output)
      {
         fprintf(stderr, "cobc:0: %s\n", (char *)cmd);
      }
      fflush(stderr);
      ret = system(cmd);
      if (verbose_output)
      {
         fprintf(stderr, "cobc:0: Exit code = %d\n", ret);
      }
      return (ret);
   }
   clen = strlen(cmd) + 32;
   buffptr = cobc_malloc(clen);
   p = buffptr;
   /* quote '$' */
   for (; *cmd; cmd++)
   {
      if (*cmd == '$')
      {
         p += sprintf(p, "\\$");
      }
      else
      {
         *p++ = *cmd;
      }
   }
   *p = 0;

   if (verbose_output)
   {
      fprintf(stderr, "cobc:0: %s\n", buffptr);
   }
   fflush(stderr);
   ret = system(buffptr);
   if (verbose_output)
   {
      fprintf(stderr, "cobc:0: Exit code = %d\n", ret);
   }
   free(buffptr);
   return (ret);
}

static void
out_line_truncated(FILE *out_file, char *line, int need_prefix, int startlen)
{
   char *p;
   char save1, save2;
   int  i, len, linespace;
   if ((line[0] == '*') && (line[1] == '#'))
   {
      line[1] = ' ';
   }
   if (cb_source_format != CB_FORMAT_FIXED)
   {
      if (need_prefix)
      {
         fprintf(out_file, "*%s", line);
      }
      else
      {
         fprintf(out_file, "%s", line);
      }
      return;
   }
   i = 0;
   len = strlen(line + 1);
   linespace = (cb_text_column - 8 - startlen);
   while (i < len)
   {
      p = &line[i];
      if (strlen(p) > linespace)
      {
         save1 = p[linespace - 1];
         save2 = p[linespace];
         p[linespace - 1] = '\n';
         p[linespace] = 0;
         if (need_prefix)
         {
            fprintf(out_file, "      *%s", p);
         }
         else
         {
            fprintf(out_file, "%s", p);
         }

         p[linespace - 1] = save1;
         p[linespace] = save2;
      }
      else
      {
         if (need_prefix)
         {
            fprintf(out_file, "      *%s", p);
         }
         else
         {
            fprintf(out_file, "%s", p);
         }
      }
      i += (linespace - 1);
      need_prefix = 1;
      linespace = (cb_text_column - 8);
   }
}
static void
out_environment_listing(FILE *out_file)
{
   int i;
   char buffer[1024];

   for (i = 0; i < used_environmen_var_cnt; i++)
   {
      sprintf(buffer, "  %s ='", used_environmen_var[i]);

      out_line_truncated(out_file, buffer, 1, 0);
      out_line_truncated(out_file, used_environmen_val[i], 0, strlen(buffer));
      strcpy(buffer, "'\n");
      out_line_truncated(out_file, buffer, 0, 0);
   }
}


static void
build_listing(char *in_name, FILE *out_file, int truncate_comment, int dump_info, int skip_debug)
{
   FILE       *in_file;
   int         i, l, leftspace;
   char        line[COB_MEDIUM_BUFF];
   char const *prefix = "";
   in_file = fopen(in_name, "r");
   if (cb_source_format == CB_FORMAT_FIXED)
   {
      prefix = "      ";
   }
   if (in_file)
   {
      memset(line, 0, sizeof(line));
      fprintf(out_file,
              "%s*> Generated by %s.%d \n%s*>  (Build date - %s %s)\n",
              prefix,
              PACKAGE_STRING, PATCH_LEVEL,
              prefix,
              __DATE__, __TIME__);
      if (dump_info)
      {
         fprintf(out_file,
                 "%s*> Environment :\n", prefix);
         fprintf(out_file,
                 "%s*>  actualy used TMPDIR : %s\n", prefix, cobc_tmpdir);
         out_environment_listing(out_file);
         leftspace = cb_text_column - strlen(prefix) - 2;
         l = strlen(prefix) + 20;
         fprintf(out_file,
                 "%s*> Compile command : ", prefix);
         for (i = 0; i < cb_saveargc; i++)
         {
            l += strlen(cb_saveargv[i]);
            if (l > leftspace)
            {
               fprintf(out_file,
                       "\n%s*                 : ", prefix);
               l = strlen(prefix) + 20 + strlen(cb_saveargv[i]);
            }
            fprintf(out_file, "%s ", cb_saveargv[i]);
         }
         fprintf(out_file, "\n");
         cobc_dump_config(out_file, 1, (char *)prefix);
      }
      fprintf(out_file, "\n");
      while (fgets(line, COB_MEDIUM_BUFF, in_file) != NULL)
      {
         if (skip_debug &&
             ((strncmp(line, "*#DEBUG", 7) == 0) || (strncmp(line, "*#PRAGMA", 8) == 0)))
         {
            /* continue*/

         }
         else
         {
            if (cb_source_format != CB_FORMAT_FIXED)
            {
               fprintf(out_file,
                       "%s", line);
            }
            else
            {
               if (line[0] == '\n')
               {
                  fprintf(out_file,
                          "%s", line);
               }
               else if (line[0] == ' ' &&
                        line[1] == '\n')
               {
                  fprintf(out_file,
                          "\n");
               }
               else if (line[0] == '#')
               {
                  fprintf(out_file,
                          "      *%s", line);
               }
               else if (line[0] == '$')
               {
                  fprintf(out_file,
                          "      %s", line);
               }
               else if (line[0] == '-' &&
                        (cb_prepro_initial_pass))
               {
                  fprintf(out_file,
                          "      %s", line);
               }
               else if (line[0] == '*')
               {
                  if (line[1] == '*' && line[2] == '>')
                  {
                     if (truncate_comment) out_line_truncated(out_file, line + 3, 1, 0);
                     else fprintf(out_file, "      %s", line + 1);
                  }
                  else
                  {
                     if (truncate_comment) out_line_truncated(out_file, line + 1, 1, 0);
                     else fprintf(out_file, "      %s", line);
                  }
               }
               else if (line[0] == 0)
               {
                  fprintf(out_file,
                          "%s", line);
               }
               else
               {
                  char *l = line;
                  while (cb_prepro_cut_line && strlen(l) > 66)
                  {
                     char *p = l + strlen(l) - 1;
                     char inStr = 0;
                     while (p > l && (l - p) >= 66 && (*p != ' ' || inStr))
                     {
                        if (*p == '"')
                        {
                           inStr = !inStr;
                        }
                        p--;
                     }
                     if (p > l)
                     {
                        *p = 0;
                        fprintf(out_file,
                                "       %s\n", l);
                        *p = ' ';
                        l = p;
                     }
                  }
                  if (strlen(l) > 0)
                  {

                     fprintf(out_file,
                             "       %s", l);
                  }
               }
            }
         }
         memset(line, 0, sizeof(line));
      }
      fclose(in_file);
   }
   else
   {
      cobc_terminate(in_name);
   }

}

static void
write_make_format(const char *text, FILE *out_file)
{
   char c;
   while ((c = *text++))
   {
      switch (c)
      {
         case ' ':
            fprintf(out_file, "\\ "); break;
         case '\\' :
            fputc('/', out_file); break;
         default:
            fputc(c, out_file); break;
      }
   }
}

static int
preprocess(char *in_name, char *out_name, int do_list, int truncate_listing)
{
   struct cb_text_list     *l;
   char  *fntmp = NULL;

   errorcount = 0;

   if ((output_dir || output_name) && (cb_compile_level == CB_LEVEL_PREPROCESS))
   {
      fntmp = cobc_temp_name("cob", ".i");
      ppout = fopen(fntmp, "w");
      if (!ppout)
      {
         cobc_terminate(fntmp);
      }
   }
   else
   {
      ppout = fopen(out_name, "w");
      if (!ppout)
      {
         cobc_terminate(out_name);
      }
   }

   if (ppopen(in_name, NULL) != 0)
   {
      if (ppout != stdout)
      {
         fclose(ppout);
         if (out_name)
         {
            unlink(out_name);
         }
         if (fntmp)
         {
            unlink(fntmp);
         }
      }
      exit(1);
   }

   if (verbose_output)
   {
      fprintf(stderr, "cobc:0: preprocessing %s into %s\n", in_name, out_name);
   }

   ppparse();
   if (cb_pplinemark)
   {
      fprintf(ppout, "*#DEBUG-FINAL GUBED\n");
   }

   fclose(ppout);
   if (cb_compile_level == CB_LEVEL_PREPROCESS)
   {
      if (fntmp)
      {
         ppout = fopen(out_name, "w");
         if (!ppout)
         {
            cobc_terminate(out_name);
         }
         build_listing(fntmp, ppout, truncate_listing, 0, 1);
         fclose(ppout);
         unlink(fntmp);
      }
      else
      {
         build_listing(out_name, stdout, truncate_listing, 0, 1);
      }

   }
   else
   {

      if (cb_listing_file && do_list)
      {
         build_listing(out_name, cb_listing_file, truncate_listing, 1, 1);
      }
   }
   fclose(ppin);

   if (errorcount > 0 && !cb_flag_syntax_only)
   {
      return (-1);
   }

   /* Output dependency list */
   if (cb_depend_file && do_list)
   {
      if (!cb_depend_target)
      {
         fputs(_("cobc:0: -MT must be given to specify target file\n"), stderr);
         exit(1);
      }
      fprintf(cb_depend_file, "%s: \\\n", cb_depend_target);
      for (l = cb_depend_list; l; l = l->next)
      {
         write_make_format(l->text, cb_depend_file);
         fprintf(cb_depend_file, "%s\n", l->next ? "\\" : "");
      }
      for (l = cb_depend_list; l; l = l->next)
      {
         write_make_format(l->text, cb_depend_file);
         fprintf(cb_depend_file, ":\n");
      }
      /*fclose (cb_depend_file);*/
   }

   return (0);
}

static struct cb_program*
program_list_reverse(struct cb_program *p)
{
   struct cb_program       *next;
   struct cb_program       *last = NULL;

   for (; p; p = next)
   {
      next = p->next_program;
      if (p->flag_is_external)
      {
         p->next_program = external_program_list;
         external_program_list = p;
      }
      else
      {
         p->next_program = last;
         last = p;
      }
   }
   return (last);
}

static char*
process_translate_open_h_file(char **file_name, FILE **file, const char *pattern, char *base_name)
{
   *file_name = cobc_malloc(strlen(base_name) + 20);
   sprintf(*file_name, pattern, base_name);
   *file = fopen(*file_name, "w");
   if (!*file)
   {
      cobc_terminate(*file_name);
   }
   return *file_name;
}

#define FCLOSE_EXIST(f) if(f) fclose(f)
static int
process_translate(struct filename *fn)
{
   struct cb_program       *p;
   struct cb_program       *q;
   struct cb_program       *r;
   struct handler_struct   *hstr1;
   struct handler_struct   *hstr2;
   struct local_filename   *lf;
   int                     ret;
   int                     i;

   /* initialize */
   memset(&cb_lex_source_file, 0, sizeof(cb_lex_source_file));
   cb_lex_source_file.line = 1;
   cb_final_source_line = 1;
   cb_disable_runtime_check = 0;
   cb_final_source_file = fn->preprocess;
   cobc_set_lex_file_name((unsigned char *)cb_final_source_file);
   cb_init_constants();
   cb_init_reserved();

   /* open the input file */
   yyin = fopen(fn->preprocess, "r");
   if (!yyin)
   {
      cobc_terminate(fn->preprocess);
   }

   /*yydebug = cb_flag_debug_parser;*/
   /* parse */
   ret = yyparse();
   fclose(yyin);
   if (!cb_flag_syntax_only && ret)
   {
      return (ret);
   }

   if (cb_flag_syntax_only || current_program->entry_list == NULL)
   {
      return (ret);
   }

   /* Set up USE GLOBAL handlers */
   p = current_program;
   for (q = p; q; q = q->next_program)
   {
      q->global_file_list = cb_list_reverse(q->global_file_list);
      if (q->nested_level)
      {
         for (r = q->next_program; r; r = r->next_program)
         {
            if (r->nested_level >= q->nested_level)
            {
               continue;
            }
            for (i = COB_OPEN_INPUT; i <= COB_OPEN_EXTEND; i++)
            {
               hstr1 = &q->global_handler[i];
               hstr2 = &r->global_handler[i];
               if (!hstr1->handler_label &&
                   hstr2->handler_label &&
                   hstr2->handler_label->is_global)
               {
                  hstr1->handler_label = hstr2->handler_label;
                  hstr1->handler_prog = r;
               }
            }
            if (!r->nested_level)
            {
               break;
            }
         }
      }
   }

   if (verbose_output)
   {
      fprintf(stderr, "cobc:0: translating %s into %s\n", fn->preprocess, fn->translate);
   }

   /* open the output file */
   yyout = fopen(fn->translate, "w");
   if (!yyout)
   {
      cobc_terminate(fn->translate);
   }

   /* open the common storage file */
   cb_storage_file_name = fn->trstorage;
   cb_storage_file = fopen(cb_storage_file_name, "w");
   if (!cb_storage_file)
   {
      cobc_terminate(cb_storage_file_name);
   }

   /* open the code coverage map file */
   if (code_cover)
   {
      fn->trccmap = process_translate_open_h_file(&cb_ccmap_file_name, &cb_ccmap_file, "%s.m.h", fn->translate);
   }

   /* open the debug file */
   fn->trdebug = process_translate_open_h_file(&cb_debug_file_name, &cb_debug_file, "%s.d.h", fn->translate);

   /* open the profiling file */
   fn->trprofiling = process_translate_open_h_file(&cb_profiling_file_name, &cb_profiling_file, "%s.p.h", fn->translate);

   /* open the init field file */
   fn->trfunction = process_translate_open_h_file(&cb_local_function_file_name, &cb_local_function_file, "%s.f.h", fn->translate);

   /* open the Global vars file */
   fn->trglobal = process_translate_open_h_file(&cb_global_file_name, &cb_global_file, "%s.g.h", fn->translate);

   /*reopen listing if needed*/
   if (fn->listing)
   {
      cb_listing_file = fopen(fn->listing, "a");
      if (!cb_listing_file)
      {
         cobc_terminate(fn->listing);
      }
   }

   p = program_list_reverse(current_program);
   if (!p)
   {
      fprintf(stderr, "cobc:0: no code generated %s\n",  fn->translate);
      return 1;
   }
   /* set up local storage files */
   ret = 1;
   for (q = p; q; q = q->next_program, ret++)
   {
      lf = cobc_malloc(sizeof(struct local_filename));
      lf->local_name = cobc_malloc(strlen(fn->translate) + 30);
      lf->register_name = cobc_malloc(strlen(fn->translate) + 30);
      lf->static_name = cobc_malloc(strlen(fn->translate) + 30);
      if (q == p && !q->next_program)
      {
         sprintf(lf->local_name, "%s.l.h", fn->translate);
         sprintf(lf->register_name, "%s.r.h", fn->translate);
         sprintf(lf->static_name, "%s.s.h", fn->translate);
      }
      else
      {
         sprintf(lf->local_name, "%s.l%d.h", fn->translate, ret);
         sprintf(lf->register_name, "%s.r%d.h", fn->translate, ret);
         sprintf(lf->static_name, "%s.s%d.h", fn->translate, ret);
      }
      lf->local_fp = fopen(lf->local_name, "w");
      if (!lf->local_fp)
      {
         cobc_terminate(lf->local_name);
      }
      lf->register_fp = fopen(lf->register_name, "w");
      if (!lf->register_fp)
      {
         cobc_terminate(lf->register_name);
      }
      lf->static_fp = fopen(lf->static_name, "w");
      if (!lf->static_fp)
      {
         cobc_terminate(lf->static_name);
      }
      q->local_storage_file = lf->local_fp;
      q->local_storage_name = lf->local_name;
      q->local_register_file = lf->register_fp;
      q->local_register_name = lf->register_name;
      q->static_storage_file = lf->static_fp;
      q->static_storage_name = lf->static_name;
      lf->next = fn->localfile;
      fn->localfile = lf;
   }

   /* setup debugdb */
   if (cb_ccmap_file || cb_flag_debuginfo || cb_flag_source_location || cb_flag_mem_info)
   {
      if (!cb_debugdb_name &&
          (cb_flag_use_debugdb || (cb_ccmap_file && output_debugdb_dir)))
      {
         char name[COB_MEDIUM_BUFF];
         char buff[COB_MEDIUM_BUFF];
         file_basename(fn->source, name);
         if (cb_flag_module_name_uppercase)
         {
            cob_strupper(name);
         }
         if (output_debugdb_dir)
         {
            sprintf(buff, "%s/%s", output_debugdb_dir, name);
            strcpy(name, buff);
         }
         else  if (output_dir)
         {
            sprintf(buff, "%s/%s", output_dir, name);
            strcpy(name, buff);
         }
         strcat(name, ".dbd");
         fn->debugdb = strdup(name);
         cb_debugdb = debugdb_opendb(fn->debugdb, 1);
         if (!cb_debugdb)
         {
            cobc_terminate(fn->debugdb);
         }
         cb_debugdb_name = strdup(fn->debugdb);
      }
   }
   cb_disable_runtime_check = 0;
   /* translate to C */
   enterprise_optimizer_reset_info(cb_flag_context_reload_enable);
   p->cb_has_report_section = fn->has_report_section;
   codegen(p);
   if (cb_listing_file)
   {
      cob_enterprise_gen_list_info(cb_listing_file, p);
   }
   if (linkage_descriptor_name)
   {
      if (verbose_output)
      {
         fprintf(stderr, "cobc:0: building %s\n", (char *)linkage_descriptor_name);
      }
      // MDC linkage_descriptor call
      cob_enterprise_gen_linkage_descriptor(linkage_descriptor_name, p);
   }
   if (cb_flag_gen_xdd)
   {
      cob_enterprise_gen_xdd_descriptor(p);
   }
   if (verbose_output && cb_optimize_size)
   {
      enterprise_optimizer_display_info();
   }
   /* close the files */
   FCLOSE_EXIST(cb_storage_file);
   FCLOSE_EXIST(cb_debug_file);
   FCLOSE_EXIST(cb_ccmap_file);
   FCLOSE_EXIST(cb_profiling_file);
   FCLOSE_EXIST(cb_local_function_file);
   FCLOSE_EXIST(cb_global_file);
   FCLOSE_EXIST(cb_listing_file);
   FCLOSE_EXIST(yyout);
   for (q = p; q; q = q->next_program)
   {
      FCLOSE_EXIST(q->local_storage_file);
      FCLOSE_EXIST(q->local_register_file);
      FCLOSE_EXIST(q->static_storage_file);
   }
   if (fn->debugdb)
   {
      cb_debugdb_name = NULL;
      cb_debugdb = debugdb_closedb(cb_debugdb);
   }
   return (0);
}

static int
process_compile(struct filename *fn)
{
   char buff[COB_MEDIUM_BUFF];
   char name[COB_MEDIUM_BUFF];

   if (output_name)
   {
      strcpy(name, output_name);
   }
   else
   {
      file_basename(fn->source, name);
      if (output_dir)
      {
         sprintf(buff, "%s/%s", output_dir, name);
         strcpy(name, buff);
      }
#     ifndef _MSC_VER
      strcat(name, ".s");
#     endif
   }
#  ifdef _MSC_VER
   sprintf(buff, cb_flag_nostrip ?
           "%s /c %s %s /GF /Od " WIN_CODEGEN_MODEL "/Zi /FR /c /Fa\"%s\" /Fo\"%s\" \"%s\"" :
           "%s /c %s %s /GF " WIN_CODEGEN_MODEL "  /c /Fa\"%s\" /Fo\"%s\" \"%s\" ",
           cob_cc, cob_cflags, cob_define_flags, name, name, fn->translate);
#  else
   sprintf(buff, "%s -S -o \"%s\" %s %s %s \"%s\"", cob_cc, name, cob_cflags, cob_pic_flags, cob_define_flags,
           fn->translate);
#  endif
   return (process(buff));
}

static int
process_assemble(struct filename *fn)
{
   char buff[COB_MEDIUM_BUFF];
   int ret;

#  ifdef _MSC_VER
   sprintf(buff, cb_flag_nostrip ?
           "%s /c %s %s /GF /Od " WIN_CODEGEN_MODEL " /Zi /FR /Fo\"%s\" \"%s\"" :
           "%s /c %s %s /GF " WIN_CODEGEN_MODEL " /Fo\"%s\" \"%s\" ",
           cob_cc, cob_cflags, cob_define_flags, fn->object, fn->translate);
   ret = process(buff);
#  else
/*
    if ( cb_compile_level == CB_LEVEL_MODULE ||
         cb_compile_level == CB_LEVEL_LIBRARY ) {
        sprintf (buff, "%s -c %s %s %s -o \"%s\" \"%s\"",
                 cob_cc, cob_cflags, cob_define_flags, The_COB_PIC_FLAGS, fn->object,
                 fn->translate);
    } else {
        sprintf (buff, "%s -c %s %s -o \"%s\" \"%s\"",
                 cob_cc, cob_cflags, cob_define_flags, fn->object, fn->translate);
    }
 */
   sprintf(buff, "%s -c %s %s %s -o \"%s\" \"%s\"",
           cob_cc, cob_cflags, cob_define_flags, cob_pic_flags, fn->object,
           fn->translate);
   ret = process(buff);

#     ifdef  COB_STRIP_CMD
#        ifndef  __hpux
   if (cob_cc_is_gcc && strip_output && ret == 0)
   {
      sprintf(buff, "%s \"%s\"", COB_STRIP_CMD, fn->object);
      process(buff);
   }
#        endif
#     endif

#  endif
   return ret;
}

static char*
build_objlist(struct filename *l, int include_main)
{
   struct filename *f;
   char            *objsptr;
   size_t          bufflen;
   bufflen = 1;
   for (f = l; f; f = f->next)
   {
      bufflen += strlen(f->object) + 4;
   }
   objsptr = cobc_malloc(bufflen);
   objsptr[0] = 0;
   for (f = l; f; f = f->next)
   {
      if (include_main || !f->objmain_lib)
      {
         strcat(objsptr, "\"");
         strcat(objsptr, f->object);
         strcat(objsptr, "\" ");
      }
   }
   return (objsptr);
}


static int
process_module(struct filename *fn)
{
   int     ret;
   char    buff[COB_MEDIUM_BUFF];
   char    name[COB_MEDIUM_BUFF];

   if (output_name)
   {
      strcpy(name, output_name);
      if (cb_flag_module_name_uppercase)
      {
         cob_strupper(name);
      }
#     ifndef _MSC_VER
      if (strchr(output_name, '.') == NULL)
      {
         strcat(name, ".");
         strcat(name, COB_MODULE_EXT);
      }
#     endif
   }
   else
   {
      file_basename(fn->source, name);
      if (cb_flag_module_name_uppercase)
      {
         cob_strupper(name);
      }
      if (output_dir)
      {
         sprintf(buff, "%s/%s", output_dir, name);
         strcpy(name, buff);
      }
      strcat(name, ".");
      if (cb_flag_obj_cit)
      {
         strcat(name, "cit");
      }
      else if (cb_flag_mf_gnt)
      {
         strcat(name, "gnt");
      }
      else
      {
         strcat(name, COB_MODULE_EXT);
      }
   }
#  ifdef _MSC_VER
/*
    sprintf (buff, gflag_set ? 
             "%s /Od /MD /LDd /Zi /FR /Fe%s %s %s %s" :
             "%s /MD /LD /Fe%s %s %s %s ",
             cob_cc, name, cob_ldflags, fn->object, cob_libs);
*/
   sprintf(buff, cb_flag_nostrip ?
           "%s /DEBUG /DLL /MANIFEST /out:\"%s\" %s \"%s\" %s /DEFAULTLIB:MSVCRT.LIB" :
           "%s /DLL /MANIFEST /out:\"%s\" %s \"%s\" %s  /DEFAULTLIB:MSVCRT.LIB",
           "link", name, cob_ldflags, fn->object, cob_libs);
   ret = process(buff);
#     if _MSC_VER >= 1400
   /* Embedding manifest */
   if (ret == 0)
   {
      sprintf(buff, "mt /nologo /manifest \"%s.manifest\" \"/outputresource:%s;#2\"", name, name);
      ret = process(buff);
   }
   sprintf(buff, "%s.dll.manifest", name);
   cobc_check_action(buff);
#     endif
   /*
   sprintf (buff, "%s.exp", name);
   cobc_check_action (buff);
   sprintf (buff, "%s.lib", name);
   cobc_check_action (buff);
   */
#  else   /* _MSC_VER */
   sprintf(buff, "%s %s %s %s %s -o \"%s\" \"%s\" %s",
           cob_cc, cob_shared_opt, cob_ldflags, cob_pic_flags,
           cob_export_dyn, name, fn->object, cob_libs);
   ret = process(buff);
#     ifdef  COB_STRIP_CMD
   if (strip_output && ret == 0)
   {
      sprintf(buff, "%s \"%s\"", COB_STRIP_CMD, name);
      process(buff);
   }
#     endif
#  endif  /* _MSC_VER */
   return (ret);
}

static int
endby(const char *str, const char *end)
{
   const char *pstr;
   const char *pend;
   pstr = str + strlen(str);
   pend = end + strlen(end);

   while ((pstr >= str) && (pend >= end))
   {
      if (*pstr != *pend) return 0;
      pstr--;
      pend--;
   }
   if (pstr < str) return 0;
   if (*pstr != '.') return 0;
   return 1;
}

static int
process_library(struct filename *l)
{
   char            *buffptr;
   char            *objsptr;
   size_t          bufflen;
   int             ret;
   char            buff[COB_MEDIUM_BUFF];
   char            name[COB_MEDIUM_BUFF];

   bufflen = 0;
   objsptr = build_objlist(l, 0);

   if (output_name)
   {
      strcpy(name, output_name);
#     ifdef _MSC_VER
      if (!endby(name, COB_MODULE_EXT))
      {
         strcat(name, ".");
         strcat(name, COB_MODULE_EXT);
      }
#     else
      if (strchr(output_name, '.') == NULL)
      {
         strcat(name, ".");
         strcat(name, COB_MODULE_EXT);
      }
#     endif
   }
   else
   {
      file_basename(l->source, name);
      if (output_dir)
      {
         sprintf(buff, "%s/%s", output_dir, name);
         strcpy(name, buff);
      }
      if (!endby(name, COB_MODULE_EXT))
      {
         strcat(name, ".");
         strcat(name, COB_MODULE_EXT);
      }
   }

   bufflen = strlen(cob_cc) + strlen(cob_ldflags)
             + strlen(cob_export_dyn) + strlen(cob_shared_opt)
             + strlen(name) + strlen(objsptr) + strlen(cob_libs)
             + strlen(cob_pic_flags) + 255;
   buffptr = cobc_malloc(bufflen);

#  ifdef _MSC_VER
   /*file_basename(name, buff);
   strcpy (name, buff); 
   */
   sprintf(buffptr, cb_flag_nostrip ?
           "%s /DEBUG /DLL /MANIFEST /out:\"%s\" %s \"%s\" %s /DEFAULTLIB:MSVCRT.LIB" :
           "%s /DLL /MANIFEST /out:\"%s\" %s %s %s /DEFAULTLIB:MSVCRT.LIB",
           "link", name, cob_ldflags, objsptr, cob_libs);
   ret = process(buffptr);
#     if _MSC_VER >= 1400
   /* Embedding manifest */
   if (ret == 0)
   {
      sprintf(buffptr, "mt /nologo /manifest \"%s.manifest\" \"/outputresource:%s;#2\"", name, name);
      ret = process(buffptr);
   }
   sprintf(buff, "%s.manifest", name);
   cobc_check_action(buff);
#     endif
   /*
   sprintf (buff, "%s.exp", name);
   cobc_check_action (buff);
   sprintf (buff, "%s.lib", name);
   cobc_check_action (buff);
   */
#  else   /* _MSC_VER */
   sprintf(buffptr, "%s %s %s %s %s -o \"%s\" %s %s",
           cob_cc, cob_shared_opt, cob_ldflags, cob_pic_flags,
           cob_export_dyn, name, objsptr, cob_libs);
   ret = process(buffptr);
#     ifdef  COB_STRIP_CMD
   if (strip_output && ret == 0)
   {
      sprintf(buff, "%s \"%s\"", COB_STRIP_CMD, name);
      process(buff);
   }
#     endif
#  endif  /* _MSC_VER */
   return (ret);
}

static int
process_mono_archive(struct filename *l)
{
   int             ret;
   char            buff[COB_MEDIUM_BUFF];
   char            name[COB_MEDIUM_BUFF];

   file_basename(l->object, name);
#  ifdef _MSC_VER
   //l->objmain_lib = cobc_temp_name ("lib", "_main.LIB");
   //strcpy (name, l->objmain_lib);
   l->objmain_lib = l->object;
   return (0);
#  else
   sprintf(name, "%s/lib%s%d_main.a", cobc_tmpdir, l->demangle_source, cob_process_id);
   l->objmain_lib = strdup(name);
#  endif

   unlink(l->objmain_lib);
#  ifdef _MSC_VER
   sprintf(buff, "%s %s /OUT:\"%s\" \"%s\" ",
           cob_ar, cob_arflags, name, l->object);
#  else   /* _MSC_VER */
   sprintf(buff, "%s %s \"%s\" \"%s\" ",
           cob_ar, cob_arflags, name, l->object);
#  endif  /* _MSC_VER */
   ret = process(buff);
   return (ret);
}

static int
process_link(struct filename *l)
{
   char            *buffptr;
   char            *objsptr;
   struct filename *f;
   size_t          bufflen;
   int             ret;
   char            buff[COB_MEDIUM_BUFF];
   char            name[COB_MEDIUM_BUFF];
   char            mainlib[COB_MEDIUM_BUFF] = "\0";

   objsptr = build_objlist(l, cb_flag_main_as_object);

   if (!cb_flag_main_as_object)
   {
      for (f = l; f; f = f->next)
      {
         if (f->objmain_lib)
         {
#           ifdef _MSC_VER
            strcat(mainlib, "\"");
            strcat(mainlib, f->objmain_lib);
            strcat(mainlib, "\" ");
#           else
            sprintf(buff, "-L\"%s\" -l%s%d_main ", cobc_tmpdir, l->demangle_source, cob_process_id);
            strcat(mainlib, buff);
#           endif
         }
      }
   }

   if (output_name)
   {
      strcpy(name, output_name);
   }
   else
   {
      file_basename(l->source, name);
      if (output_dir)
      {
         sprintf(buff, "%s/%s", output_dir, name);
         strcpy(name, buff);
      }
   }

   bufflen = strlen(cob_cc) + strlen(cob_ldflags) + strlen(cob_export_dyn)
             + strlen(name) + strlen(objsptr) + 2 * strlen(cob_libs) +  strlen(mainlib)
             + 255;
   buffptr = cobc_malloc(bufflen);
#  ifdef _MSC_VER
   /*file_basename(name, buff);
   strcpy (name, buff);
   */
   sprintf(buffptr, cb_flag_nostrip ?
           "%s  /SUBSYSTEM:CONSOLE /MANIFEST /DEBUG \"/out:%s.exe\" %s %s %s %s /DEFAULTLIB:MSVCRT.LIB" :
           "%s /SUBSYSTEM:CONSOLE /MANIFEST \"/out:%s.exe\" %s %s %s %s /DEFAULTLIB:MSVCRT.LIB",
           /*cob_cc*/"link", name, cob_ldflags, objsptr, cob_libs, mainlib);
   ret = process(buffptr);
#     if _MSC_VER >= 1400
   /* Embedding manifest */
   if (ret == 0)
   {
      sprintf(buff, "mt /nologo /manifest \"%s.exe.manifest\" \"/outputresource:%s.exe;#2\"", name, name);
      ret = process(buff);
   }
   sprintf(buff, "%s.dll.manifest", name);
   cobc_check_action(buff);
#     endif
#  else   /* _MSC_VER */
   sprintf(buffptr, "%s %s %s -o \"%s\" %s %s %s %s",
           cob_cc, cob_ldflags, cob_export_dyn, name, objsptr, cob_libs,
           mainlib, (strlen(mainlib) ? cob_libs : ""));

   ret = process(buffptr);
#     ifdef  __hpux
   if (ret == 0)
   {
      sprintf(buff, "chatr -s +s enable %s%s 1>/dev/null 2>&1", name, COB_EXEEXT);
      process(buff);
   }
#     endif
#     ifdef  COB_STRIP_CMD
   if (strip_output && ret == 0)
   {
      sprintf(buff, "%s \"%s%s\"", COB_STRIP_CMD, name, COB_EXEEXT);
      process(buff);
   }
#     endif
#  endif  /* _MSC_VER */
   free(objsptr);
   free(buffptr);
   return (ret);
}


static int
process_build_main(struct filename *l, int param_count)
{
   struct filename *fn;
   struct filename *ffn;
   char            basename[COB_MEDIUM_BUFF];
   FILE            *file;
   int             ret;
   char            callname[COB_MEDIUM_BUFF];


   fn = cobc_malloc(sizeof(struct filename));
   fn->need_preprocess = 0;
   fn->need_translate = 1; /* just to erase the temp files*/
   fn->need_assemble = 1;
   fn->demangle_source = strdup(l->demangle_source);
   fn->next = NULL;

   if (!file_list)
   {
      file_list = fn;
   }
   else
   {
      for (ffn = file_list; ffn->next; ffn = ffn->next)
      ;
      ffn->next = fn;
   }
   file_basename(l->source, basename);
   if (save_temps || save_csrc)
   {
      fn->translate = cobc_malloc(strlen(basename) + 30 + strlen(save_temps_dir));
      fn->object    = cobc_malloc(strlen(basename) + 30);
#     ifdef  _MSC_VER
      sprintf(fn->object, "%s_main.obj", basename);
#     else
      sprintf(fn->object, "%s_main.o", basename);
#     endif
      sprintf(fn->translate, "%s_main.c", basename);
   }
   else
   {
#     ifdef  _MSC_VER
      fn->translate = cobc_malloc(strlen(basename) + 30);
      fn->object = cobc_malloc(strlen(basename) + 30);
      sprintf(fn->object, "%s_main.obj", basename);
      sprintf(fn->translate, "%s_main.c", basename);
#     else
      fn->translate = cobc_temp_name("cob", "_main.c");
      fn->object = cobc_temp_name("cob", "_main.o");
#     endif
   }
   if (verbose_output)
   {
      fprintf(stderr, "cobc:0: Building main entry point\n");
   }
   fn->source = fn->translate;
   file = fopen(fn->translate, "w");
   if (!file)
   {
      cobc_terminate(fn->translate);
   }
   strcpy(callname,  l->demangle_source);
   cob_strupper(callname);
   output_source_header(file, NULL, l->demangle_source);
   fprintf(file, "#include <libcob.h>\n\n");
   fprintf(file, "extern %s ();\n\n", callname);
   output_main_symbol(file, callname, cb_flag_param_C_like ? 2 : 1);
   fclose(file);
   ret = process_assemble(fn);
   if (ret != 0)
   {
      return (ret);
   }
   if (cb_flag_main_as_object)
   {
      fn->objmain_lib = fn->object;
      return ret;
   }
   else
   {
      return (process_mono_archive(fn));
   }
}

static void
prepro_reformat(char *fin, char *fout)
{
   FILE *flist;
   flist = fopen(fout, "w");
   if (!flist)
   {
      cobc_terminate(fout);
   }
   if (verbose_output)
   {
      fprintf(stderr, "cobc:0: reformation %s into %s\n", fin, fout);
   }
   build_listing(fin, flist, 0, 0, 0);
   fclose(flist);

}

static char*
prepro_reduce_rw_path(char *cwd, char *fin)
{
   int lcwd = strlen(cwd);
   int lin  = strlen(fin);

   if (lin > lcwd)
   {
      if (memcmp(cwd, fin, lcwd) == 0)
      {
         fin = &fin[lcwd];
         while (*fin == '/' || *fin == '\\')
         {
            fin++;
         }
      }
   }
   lin  = strlen(fin);
   if (lin > 79)
   {
      fprintf(stderr, "cobc:0: Report Writer require filename smaller than 79 chars '%s'\n", (char *)fin);
      cobc_clean_up(3);
      exit(3);
   }
   return fin;
}

static void
prepro_exec_rw(char *fin, char *fout, char *flst)
{
   int r;
   char  cwdbuff[COB_SMALL_BUFF];
   char  *cwd;
   char  buff[COB_MEDIUM_BUFF];
   char  buffopt[COB_MEDIUM_BUFF];
   char *s;

   if (cb_has_report_section)
   {
      cwd = getcwd(cwdbuff, COB_SMALL_BUFF);
      if (cwd)
      {
         fin = prepro_reduce_rw_path(cwd, fin);
         fout = prepro_reduce_rw_path(cwd, fout);
         flst = prepro_reduce_rw_path(cwd, flst);
      }
      s =  cob_getenv("COB_SPCRW2_OPT");
      if (!s)
      {
         s = (char *)"";
      }
      if (cb_flag_rw_mode_nopf)
      {
         sprintf(buffopt, "+MO:NOPF %s", s);
      }
      if (cb_flag_rw_mode_nopf_dos)
      {
         sprintf(buffopt, "+MO:NOFD %s", s);
      }
      else
      {
         sprintf(buffopt, "%s", s);
      }
      sprintf(buff, "SPCRW2 %s %s %s %s", buffopt, fin,  fout, flst);
      r =  process(buff);
      if (r < 0 || r > 65000)
      {
         cobc_clean_up(exit_status);
         exit(exit_status);
      }
   }
}


#define FFLUSH_EXIST(f) if(f) fflush(f)
int
main(int argc, char *argv[])
{
   struct filename     *fn;
   char                *p;
   int                 create_list = 1;
   int                 mainparamcnt = 0;
   int                 year;
   int                 day;
   int                 found;
   char                month[32];
   char                buff[COB_MEDIUM_BUFF];

   atexit(cobc_atexit);

#  ifdef  HAVE_SIGNAL_H
#     ifdef SIGINT
   if ((intsig = signal(SIGINT, cobc_sig_handler)) == SIG_IGN)
   {
      (void)signal(SIGINT, SIG_IGN);
   }
#     endif
#     ifdef SIGHUP
   if ((hupsig = signal(SIGHUP, cobc_sig_handler)) == SIG_IGN)
   {
      (void)signal(SIGHUP, SIG_IGN);
   }
#     endif
#     ifdef SIGQUIT
   if ((qutsig = signal(SIGQUIT, cobc_sig_handler)) == SIG_IGN)
   {
      (void)signal(SIGQUIT, SIG_IGN);
   }
#     endif
#  endif

   cb_saveargc = argc;
   cb_saveargv = argv;

#  ifdef  HAVE_SETLOCALE
   setlocale(LC_ALL, "");
#  endif

#  ifdef  ENABLE_NLS
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);
#  endif

   cob_process_id = getpid();

   /* Initialize the global variables */
   memset(buff, 0, sizeof(buff));
   memset(month, 0, sizeof(month));
   day = 0;
   year = 0;
   sscanf(__DATE__, "%s %d %d", month, &day, &year);
   if (day && year)
   {
      sprintf(buff, "%s %2.2d %4.4d %s", month, day, year, __TIME__);
   }
   else
   {
      sprintf(buff, "%s %s", __DATE__, __TIME__);
   }
   cb_build_stamp = cobc_malloc(strlen(buff) + 1);
   strcpy(cb_build_stamp, buff);

#  ifdef _WIN32
   /* for Stdout and StdErr as binary if needed */
   if (cob_getenv("COB_STDUNIX") != NULL)
   {
      _setmode(_fileno(stdout), _O_BINARY);
      _setmode(_fileno(stdin), _O_BINARY);
      _setmode(_fileno(stderr), _O_BINARY);
   }
   /* Initialize program_name */
   program_name = strrchr(argv[0], '\\');
#  else
   /* Initialize program_name */
   program_name = strrchr(argv[0], '/');
#  endif
   if (program_name)
   {
      program_name++;
   }
   else
   {
      program_name = argv[0];
   }

   output_name = NULL;

   if ((p = cob_getenv("COBCTMP")) != NULL)
   {
      cobc_tmpdir = p;
   }
   else if ((p = cob_getenv("TMPDIR")) != NULL)
   {
      cobc_tmpdir = p;
   }
   else if ((p = cob_getenv("TMP")) != NULL)
   {
      cobc_tmpdir = p;
      sprintf(buff, "TMPDIR=%s", p);
      p = strdup(buff);
      putenv(p);
   }
   else
   {
      cobc_tmpdir = (char *)"/tmp";
      putenv((char *)"TMPDIR=/tmp");
   }

/*CobolIT*/
   cob_enterprise_runtime_init();
   (void)cob_getenv("COBOLIT_LICENSE"); /*just to record value for listing*/
   (void)cob_getenv("PATH"); /*just to record value for listing*/
   (void)cob_getenv("LD_LIBRARY_PATH"); /*just to record value for listing*/
   cobolit_dir = cob_getenv("COBOLITDIR");
   if (cobolit_dir == NULL)
   {
      cobolit_dir = COB_BASEDIR;
   }
   found = cob_init_env_var(cob_cc, "COB_CC", COB_CC, COB_BASEDIR, cobolit_dir);
   /*check is gcc required*/
   p = strstr(cob_cc, "gcc");
   if (p != NULL)
   {
      cb_flag_gen_gcc = 1;
      cb_flag_goto_gcc = 1;
      cob_cc_is_gcc = 1;
      /* is the the original configurated C compiler*/
      if (found && (strcmp(cob_cc, COB_CC) != 0))
      {
         const char *model = "";

#        ifdef __ia64
         if (sizeof(char *) > 4)
         {
            model = "lp";
         }
         else
         {
            model = "ilp";
         }
#        endif

#        ifdef _AIX
         model = "aix";
#        endif
         /* No, then set default to gcc*/
#        ifdef COB_HAS_THREAD
         sprintf(buff, "-I%s/include -m%s%d -DCOB_HAS_THREAD ", cobolit_dir, model, (int)(sizeof(char *) * 8));
#        else
         sprintf(buff, "-I%s/include -m%s%d ", cobolit_dir, model, (int)(sizeof(char *) * 8));
#        endif

         The_COB_CFLAGS         = strdup(buff);
#        ifdef _AIX
         /* sprintf(buff, "-L%s/lib -Wl,-bexpall -Wl,-brtl -m%s%d", cobolit_dir, model, (int)(sizeof(char*) *8));*/
         sprintf(buff, "-L%s/lib %s -m%s%d", cobolit_dir, cob_export_dyn, model, (int)(sizeof(char *) * 8));
#        else
         sprintf(buff, "-L%s/lib -m%s%d", cobolit_dir, model, (int)(sizeof(char *) * 8));
#        endif
         The_COB_LDFLAGS        = strdup(buff);
         The_COB_OPTIMIZE_FLAG  = "-O2";
         The_COB_OPTSIZE_FLAG   = "-Os";
         The_COB_PIC_FLAGS      = "-fPIC -DPIC";
         The_COB_SHARED_OPT     = "-shared";
         The_COB_EXPORT_DYN     = "";
         The_COB_EXTRA_FLAGS    = " -finline-functions ";
      }
   }
   if (cob_cc_is_gcc)
   {
      strcat(cob_cc, " -pipe");
   }
   cob_init_env_var(cob_sflag,   "COB_OPTSIZE_FLAG",  The_COB_OPTSIZE_FLAG,      COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_Oflag,   "COB_OPTIMIZE_FLAG", The_COB_OPTIMIZE_FLAG,     COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_eflag,   "COB_EXTRA_FLAGS",   The_COB_EXTRA_FLAGS,       COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_cflags,     "COB_CFLAGS",     The_COB_CFLAGS, COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_ldflags,    "COB_LDFLAGS",    The_COB_LDFLAGS, COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_ar,         "COB_AR",         COB_AR,         COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_arflags,    "COB_ARFLAGS",    COB_ARFLAGS,    COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_libs,       "COB_LIBS",       COB_LIBS,       COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_config_dir, "COB_CONFIG_DIR", COB_CONFIG_DIR, COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_copy_dir,   "COB_COPY_DIR",   COB_COPY_DIR,   COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_export_dyn, "COB_EXPORT_DYN", The_COB_EXPORT_DYN,        COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_shared_opt, "COB_SHARED_OPT", The_COB_SHARED_OPT,        COB_BASEDIR, cobolit_dir);
   cob_init_env_var(cob_pic_flags,  "COB_PIC_FLAGS",  The_COB_PIC_FLAGS,         COB_BASEDIR, cobolit_dir);

   memset(cob_define_flags, 0, sizeof(cob_define_flags));
   p = cob_getenv("COB_SUNSTUDIO12");
   if (p)
   {
      cob_patch_var(buff, cob_cflags,  "-xarch=generic64", "-m64");
      strcpy(cob_cflags, buff);
      cob_patch_var(buff, cob_ldflags, "-xarch=generic64", "-m64");
      strcpy(cob_ldflags, buff);
   }

   p = cob_getenv("COB_LDADD");
   if (p)
   {
      strcat(cob_libs, " ");
      strcat(cob_libs, p);
   }
   p = cob_getenv("COB_EBCDIC");
   if (p && (*p == 'F' || *p == 'f'))
   {
      alt_ebcdic = 1;
   }

   /* NOT valid for GNU license */
   cob_enterprise_load(0);


#  if defined(CIT_EBCDIC_CHARSET)
#     if defined(CIT_EBCDIC_CHARSET_DEFAULT)
   cb_flag_ebcdic_charset = 1;
#     endif
#     if defined(COB_EBCDIC_MACHINE)
   cb_flag_ebcdic_charset = 1;
#     endif
#  endif

   /* Process command line arguments */
   iargs = process_command_line(&argc, &argv);

   if (!cb_flag_syntax_only)
   {
      rtd = cob_get_rtd();
      cob_init(rtd, 0, NULL);

      cob_check_version_1(rtd, "cobc", PACKAGE_VERSION, PATCH_LEVEL, cb_flag_ebcdic_charset);
   }

   /* Check the filename */
   if (iargs == argc)
   {
      fprintf(stderr, "cobc:0: No input files\n");
      exit(1);
   }

   /* processes COBCPY environment variable */
   process_env_copy_path();

   cb_include_list = cb_include_list_add(cb_include_list, cob_copy_dir);

   file_list = NULL;

   if (setjmp(cob_jmpbuf) != 0)
   {
      fprintf(stderr, "%s:%d: Aborting compile \n",
              cb_lex_source_file.file_name, cb_lex_source_file.line);
      FFLUSH_EXIST(stderr);
      FFLUSH_EXIST(yyout);
      FFLUSH_EXIST(cb_storage_file);
      FFLUSH_EXIST(cb_debug_file);
      FFLUSH_EXIST(cb_ccmap_file);
      FFLUSH_EXIST(cb_profiling_file);
      FFLUSH_EXIST(cb_local_function_file);
      FFLUSH_EXIST(cb_global_file);
      exit_status = 1;
      cobc_clean_up(exit_status);
      return (exit_status);
   }

   /* Defaults are set here */
   if (!cb_flag_syntax_only)
   {
      if (!wants_nonfinal)
      {
         if (cb_flag_main)
         {
            cb_compile_level = CB_LEVEL_EXECUTABLE;
         }
         if (cb_flag_module)
         {
            cb_compile_level = CB_LEVEL_MODULE;
         }
         if (cb_flag_library)
         {
            cb_compile_level = CB_LEVEL_LIBRARY;
         }
      }
      if (cb_compile_level == 0 && !wants_nonfinal)
      {
         cb_compile_level = CB_LEVEL_MODULE;
         cb_flag_module = 1;
      }
      if (wants_nonfinal && cb_compile_level != CB_LEVEL_PREPROCESS &&
          !cb_flag_main && !cb_flag_module && !cb_flag_library)
      {
         cb_flag_module = 1;
      }
      if (cb_flag_source_validate)
      {
         cb_compile_level = CB_LEVEL_TRANSLATE;
      }
   }
   else
   {
      cb_compile_level = CB_LEVEL_TRANSLATE;
   }

   /* NOT valid for GNU License */
   if (cb_compile_level > CB_LEVEL_TRANSLATE)
   {
      cob_enterprise_load(1);
   }


   if ( output_name && cb_compile_level < CB_LEVEL_LIBRARY &&
       (argc - iargs) > 1 )
   {
      fprintf(stderr, "%s:0: -o option invalid in this combination\n", program_name);
      exit(1);
   }
   if (cb_flag_sign_ascii && cb_flag_sign_ebcdic)
   {
      fprintf(stderr, "cobc:0: Only one of -fsign-ascii or -fsign-ebcdic may be specified\n");
      exit(1);
   }

   if (cb_flag_sign_ascii)
   {
      cb_display_sign_ebcdic = COB_DISPLAY_SIGN_ASCII;
   }
   else if (cb_flag_ebcdic_charset)
   {
      cb_flag_sign_ebcdic = 1;
      cb_flag_sign_ascii = 0;
   }

   if (cb_flag_sign_ebcdic)
   {
      cb_display_sign_ebcdic = COB_DISPLAY_SIGN_EBCDIC;
   }
   if (cb_flag_notrunc)
   {
      cb_binary_truncate = 0;
      cb_pretty_display &= ~COB_NUMERIC_PRETTY_DISPLAY;
   }

   while (iargs < argc)
   {
      fn = process_filename(argv[iargs++]);
      if (fn->listing)
      {
         if (listing_name && !create_list)
         {
            /* only one listing file -> append */
            cb_listing_file = fopen(fn->listing, "a");
         }
         else
         {
            cb_listing_file = fopen(fn->listing, "w");
            create_list = 0;
         }

         if (!cb_listing_file)
         {
            perror(cit_optarg);
         }
      }

      if (!fn)
      {
         exit_status = 1;
         cobc_clean_up(exit_status);
         return (exit_status);
      }
      /* Preprocess */
      cb_has_report_section = 0;
      if (cb_compile_level >= CB_LEVEL_PREPROCESS && fn->need_preprocess)
      {
         cb_main_source_file = fn->source;
         cb_raw_debug_line = 0;
         if (ext_prepro_name)
         {
            char *infname;
            cb_prepro_initial_pass = 1;
            cb_pplinemark = cb_flag_keep_org_line;
            if (preprocess(fn->source, fn->prepro_out, 1, cb_truncate_listing) != 0)
            {
               cobc_clean_up(exit_status);
               return (exit_status);
            }
            cb_raw_debug_line = 1;
            prepro_reformat(fn->prepro_out, fn->prepro_in);
            cb_pplinemark = 0;
            unlink(fn->prepro_out);
            infname = fn->prepro_in;
            if (cb_flag_rw_auto && cb_has_report_section && !cb_flag_rw_after_prepro)
            {
               prepro_exec_rw(fn->prepro_in, fn->prepro_rwout, fn->prepro_rwlst);
               infname = fn->prepro_rwout;
            }

            cb_prepro_initial_pass = (cb_compile_level == CB_LEVEL_PREPROCESS);

            sprintf(buff, "%s %s %s", ext_prepro_name, infname,  fn->prepro_out);
            if (process(buff) != 0)
            {
               cobc_clean_up(exit_status);
               return (exit_status);
            }
            infname = fn->prepro_out;
            if (cb_flag_rw_auto && cb_has_report_section && cb_flag_rw_after_prepro)
            {
               prepro_exec_rw(infname, fn->prepro_rwout, fn->prepro_rwlst);
               infname = fn->prepro_rwout;
            }

            cb_pplinemark = cb_flag_debug_exec;
            if (preprocess(infname, fn->preprocess, 0, 0) != 0)
            {
               cobc_clean_up(exit_status);
               return (exit_status);
            }
            cb_pplinemark = 0;
         }
         else
         {
            cb_prepro_initial_pass = (cb_compile_level == CB_LEVEL_PREPROCESS);
            cb_pplinemark = 1;
            if (preprocess(fn->source, fn->preprocess, 1, cb_truncate_listing) != 0)
            {
               cobc_clean_up(exit_status);
               return (exit_status);
            }
            cb_pplinemark = 0;
            if (cb_has_report_section)
            {
               cb_prepro_initial_pass = 1;
               cb_pplinemark = cb_flag_keep_org_line;
               unlink(fn->preprocess);
               if (preprocess(fn->source, fn->prepro_in, 1, cb_truncate_listing) != 0)
               {
                  cobc_clean_up(exit_status);
                  return (exit_status);
               }
               cb_pplinemark = 0;
               cb_raw_debug_line = 1;
               prepro_reformat(fn->prepro_in, fn->prepro_out);
               prepro_exec_rw(fn->prepro_out, fn->prepro_rwout, fn->prepro_rwlst);
               cb_prepro_initial_pass = (cb_compile_level == CB_LEVEL_PREPROCESS);
               cb_pplinemark = cb_flag_debug_exec;
               if (preprocess(fn->prepro_rwout, fn->preprocess, 0, 0) != 0)
               {
                  cobc_clean_up(exit_status);
                  return (exit_status);
               }
            }

         }
      }
      fn->has_report_section = cb_has_report_section;
      if (cb_listing_file)
      {
         fclose(cb_listing_file);
         cb_listing_file = NULL;
      }
   }
   if (cb_reg_file_name)
   {
      unlink(cb_reg_file_name);
   }
   for (fn = file_list; fn; fn = fn->next)
   {
      if (verbose_output)
      {
         if (fn->demangle_source)
         {
            fprintf(stderr, "cobc:0: Processing %s\n", (char *)fn->demangle_source);
         }
         else
         {
            fprintf(stderr, "cobc:0: Processing %s\n", (char *)fn->object);
         }
      }
      cb_id = 1;
      cb_attr_id = 1;
      cb_literal_id = 1;
      cb_field_id = 10;
      cb_storage_id = 1;
      iparams++;
      source_demangle_name = fn->demangle_source;
      if (iparams > 1 && cb_compile_level == CB_LEVEL_EXECUTABLE &&
          !cb_flag_syntax_only)
      {
         local_level = cb_compile_level;
         cb_flag_main = 0;
         cb_compile_level = CB_LEVEL_ASSEMBLE;
      }
      /* Translate */
      if (cb_compile_level >= CB_LEVEL_TRANSLATE && fn->need_translate)
      {
         if (process_translate(fn) != 0 && !cb_reg_file_name)
         {
            cobc_clean_up(exit_status);
            return (exit_status);
         }
         if (fn == file_list)
         {
            mainparamcnt = cb_list_length(current_program->parameter_list);
         }
      }
      if (cb_reg_file_name && current_program)
      {
         cob_enterprise_gen_fieldname_list(cb_reg_file_name, current_program);
      }
      if (cb_flag_syntax_only)
      {
         continue;
      }

      /* Compile */
      if (cb_compile_level == CB_LEVEL_COMPILE)
      {
         if (process_compile(fn) != 0)
         {
            cobc_clean_up(exit_status);
            return (exit_status);
         }
      }

      /* Build module */
      /*if ( cb_compile_level == CB_LEVEL_MODULE && fn->need_assemble ) {
       if ( process_module_direct (fn) != 0 ) {
           cobc_clean_up (status);
           return status;
       }
   } else */{
         /* Assemble */
         if (cb_compile_level >= CB_LEVEL_ASSEMBLE && fn->need_assemble)
         {
            if (process_assemble(fn) != 0)
            {
               cobc_clean_up(exit_status);
               return (exit_status);
            }
         }

         /* Build module */
         if (cb_compile_level == CB_LEVEL_MODULE)
         {
            if (process_module(fn) != 0)
            {
               cobc_clean_up(exit_status);
               return (exit_status);
            }
         }
      }
   }

   if (!cb_flag_syntax_only)
   {
      /* Link */
      if (local_level == CB_LEVEL_EXECUTABLE)
      {
         cb_compile_level = CB_LEVEL_EXECUTABLE;
         cb_flag_main = 1;
      }
      if (cb_compile_level == CB_LEVEL_LIBRARY)
      {
         if (process_library(file_list) != 0)
         {
            cobc_clean_up(exit_status);
            return (exit_status);
         }
      }
      else if (cb_compile_level == CB_LEVEL_EXECUTABLE)
      {
         if (cb_flag_main &&
             cb_flag_build_main &&
             !cb_main_generated)
         {
            if (process_build_main(file_list, mainparamcnt) != 0)
            {
               cobc_clean_up(exit_status);
               return (exit_status);
            }
            cb_main_generated = 1;
         }
         if (process_link(file_list) != 0)
         {
            cobc_clean_up(exit_status);
            return (exit_status);
         }
      }
   }

   /* We have successfully completed */
   exit_status = 0;

   return (exit_status);
}

int
cobc_hash_fname(const unsigned char *s)
{
   int val = 0;

   while (*s)
   {
      val += toupper(*s++);
      if (*s)
      {
         val += (toupper(*s++) << 8);
         if (*s)
         {
            val += (toupper(*s++) << 16);
            if (*s)
            {
               val += (toupper(*s++) << 24);
            }
         }
      }
   }
   if (val < 0)
   {
      return -val;
   }
   return val;
}

void
cobc_set_lex_file_name(const unsigned char *s)
{
   int h;
   cb_lex_source_file_info *l;
   if (!cb_lex_source_file.file_name || strcmp((char *)cb_lex_source_file.file_name, (char *)s) != 0)
   {
      l = cb_lex_source_file.next;
      h = cobc_hash_fname(s);
      while (l)
      {
         if (h == l->file_name_hash && strcmp((char *)l->file_name, (char *)s) == 0) break;
         l = l->next;
      }
      cb_lex_source_file.file_name = (unsigned char *)strdup((char *)s);
      cb_lex_source_file.file_name_hash = h;
      if (!l)
      {
         l = malloc(sizeof(cb_lex_source_file_info));
         *l = cb_lex_source_file;
         cb_lex_source_file.next = l;
      }
   }
}

void
cobc_find_lex_hash(const int h)
{
   cb_lex_source_file_info *l;
   if (cb_lex_source_file.file_name_hash != h)
   {
      l = cb_lex_source_file.next;
      while (l)
      {
         if (l->file_name_hash == h)
         {
            cb_lex_source_file.file_name = l->file_name;
            cb_lex_source_file.file_name_hash = l->file_name_hash;
         }
         l = l->next;
      }
   }

}


#ifdef _MSC_VER
int strncasecmp(const char *_Str1, const char *_Str2,  size_t _MaxCount)
{
   return (_strnicmp(_Str1, _Str2, _MaxCount));
}

int strcasecmp(const char *_Str1, const char *_Str2)
{
   return (_stricmp(_Str1, _Str2));
}

char* strcasestr(const char *haystack, const char *needle)
{
   int nlen = strlen(needle);
   int hlen = strlen(haystack) - nlen + 1;
   int i;
   for (i = 0; i < hlen; i++)
   {
      int j;
      for (j = 0; j < nlen; j++)
      {
         unsigned char c1 = haystack[i + j];
         unsigned char c2 = needle[j];
         if (toupper(c1) != toupper(c2)) goto next;
      }
      return(char *)haystack + i;
    next:
      ;
   }
   return NULL;
}
#endif


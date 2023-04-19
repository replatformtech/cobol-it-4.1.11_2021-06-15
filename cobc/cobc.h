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

#ifndef CB_COBC_H
#define CB_COBC_H

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <setjmp.h>
#include <sys/stat.h>

#include "lib/gettext.h"
#ifdef _MSC_VER

    #ifndef _CRT_SECURE_NO_DEPRECATE 
        #define _CRT_SECURE_NO_DEPRECATE 1
    #endif
	#define COB_DLL_EXPIMP __declspec(dllexport) extern 

    extern char *strcasestr(const char *haystack, const char *needle);
    extern int strcasecmp  (const char * _Str1, const char * _Str2);
    extern int strncasecmp (const char * _Str1, const char * _Str2,  size_t _MaxCount);

#else
    #define COB_DLL_EXPIMP extern 
#endif

/* DO NOT Change these value*/
#define CB_MAX_CNAME            8096
#define COB_TINY_BUFF           256
#define COB_SMALL_BUFF          1024
#define COB_MEDIUM_BUFF         8192
#define COB_LARGE_BUFF          16384

/* Compile level */
typedef enum cb_compile_level_enum {
    CB_LEVEL_PREPROCESS = 1,
    CB_LEVEL_TRANSLATE,
    CB_LEVEL_COMPILE,
    CB_LEVEL_ASSEMBLE,
    CB_LEVEL_MODULE,
    CB_LEVEL_LIBRARY,
    CB_LEVEL_EXECUTABLE
} cb_compile_level_enum_t;

extern enum cb_compile_level_enum    cb_compile_level;

#if !defined(__i386__) && !defined(__x86_64__) && !defined(__powerpc__) && !defined(__powerpc64__) && !defined(__ppc__) && !defined(__amd64__) && \
    !defined(__i386) && !defined(__amd64) && !defined(_M_AMD64) && !defined(_M_X64) && !defined(_M_I86) && !defined(__powerpc) && \
    !defined(__powerpc64) && !defined(_ARCH_PPC)
    #define COB_NON_ALIGNED
/* Some DEC Alphas can only directly load shorts at 4-byte aligned addresses */
    #ifdef  __alpha
        #define COB_SHORT_BORK
    #endif
#endif

#define ABORT()         cobc_abort(__FILE__,__LINE__)

#define CB_FORMAT_FIXED 0
#define CB_FORMAT_FREE  1

extern int      cb_source_format;

extern int      cb_display_sign_ebcdic;

extern struct cb_exception {
    const char      *name;                  /* exception name */
    const int       code;                   /* exception code */
    int             enable;                 /* if turned on */
} cb_exception_table[];

#define CB_EXCEPTION_NAME(id)   cb_exception_table[id].name
#define CB_EXCEPTION_CODE(id)   cb_exception_table[id].code
#define CB_EXCEPTION_ENABLE(id) cb_exception_table[id].enable

#undef CB_FLAG
#define CB_FLAG(var,name,doc,def) extern int var;
#include "flag.def"
#undef  CB_FLAG

#undef CB_WARNDEF
#define CB_WARNDEF(var,name,wall,doc, def) extern int var;
#include "warning.def"
#undef  CB_WARNDEF

struct cb_inc_list {
    const char              *path;
    const char              *lib;
    struct cb_text_list     *extlist;
    struct cb_inc_list      *next;
};

struct cb_text_list {
    const char              *text;
    struct cb_text_list     *next;
};

struct cb_replace_list {
    struct cb_text_list     *old_text;
    struct cb_text_list     *new_text;
    int                     global;
    struct cb_replace_list  *next;
};

struct local_filename {
    struct local_filename   *next;
    char                    *local_name;
    FILE                    *local_fp;
    char                    *register_name;
    FILE                    *register_fp;
    char                    *static_name;
    FILE                    *static_fp;
};

struct filename {
    struct filename *next;
    char            *source;                        /* foo.cob */
    char            *preprocess;                    /* foo.i */
    char            *prepro_in;                     /* foo.i0 */
    char            *prepro_out;                    /* foo.i1 */
    char            *prepro_rwout;                  /* foo.rw */
    char            *prepro_rwlst;                  /* foo.rw */
    char            *translate;                     /* foo.c */
    char            *trstorage;                     /* foo.c.h */
    char            *trdebug;                       /* foo.c.d.h */
    char            *trprofiling;                   /* foo.c.p.h */
    char            *trfunction;                    /* foo.c.f.h */
    char            *trglobal;                      /* foo.c.g.h */
    char            *object;                        /* foo.o */
    char            *debugdb;                       /* foo.dbd */
    char            *trccmap;                       /* foo.c.m.h */
    char            *listing;                       /* foo.lst */
    char            *objmain_lib;                   /* libfoo_main.a */
    char            *demangle_source;               /* foo */
    struct local_filename   *localfile;             /* foo.c.l[n].h foo.c.s[n].h foo.c.r[n].h*/
    int             need_preprocess;
    int             need_translate;
    int             need_assemble;
    int             has_report_section;
};

struct cb_constant_list {
    const char              *key;
    const char              *value;
    char                     is_makesyn;
    char                     is_userconst;
    struct cb_constant_list *next;
};

struct cb_file_status_map_list {
    struct cb_file_status_map_list *next;
    union {
        unsigned int crt_status;
        unsigned char f_status[2];
    } cit_status;
    union {
        unsigned int crt_status;
        unsigned char f_status[2];
    } custome_status;
    char*        org_map;
};

struct attr_list {
    struct attr_list    *next;
    unsigned char       *pic;
    int                 id;
    int                 type;
    int                 digits;
    int                 scale;
    int                 flags;
    int                 lenstr;
} ;

extern int                      cb_id;
extern int                      cb_attr_id;
extern int                      cb_literal_id;
extern int                      cb_field_id;
extern int                      cb_storage_id;
extern int                      cb_flag_main;
extern int                      cb_main_generated;
extern int                      cb_has_report_section;

extern int                      errorcount;
extern int                      warningcount;
extern int                      alt_ebcdic;
extern int                      optimize_flag;
extern int                      has_external;
extern int                      has_COPY_lib_defined;

extern char                     *cb_build_stamp;
extern char                     *cb_main_source_file;
extern char                     *cb_final_source_file;
extern int                      cb_final_source_line;
extern int                      cb_keep_final_source;

extern char                     cob_config_dir[];

extern char                     *cb_sysin_redirect;
extern char                     *cb_sysout_redirect;
extern char                     *cb_syserr_redirect;
extern char                     *cb_sysprint_redirect;
extern struct cb_file           *cb_sysin_file;
extern struct cb_file           *cb_sysout_file;
extern struct cb_file           *cb_syserr_file;
extern struct cb_file           *cb_sysprint_file;
extern char                     *source_demangle_name;
extern FILE                     *cb_storage_file;
extern char                     *cb_storage_file_name;

extern FILE                     *cb_debug_file;
extern char                     *cb_debug_file_name;
extern FILE                     *cb_profiling_file;
extern char                     *cb_profiling_file_name;
extern FILE                     *cb_local_function_file;
extern char                     *cb_local_function_file_name;
extern FILE                     *cb_global_file;
extern char                     *cb_global_file_name;
extern FILE                     *cb_ccmap_file;
extern char                     *cb_ccmap_file_name;
extern char                     *cb_reg_file_name;
extern char                     **cb_saveargv;
extern int                      cb_saveargc;
extern FILE                     *cb_cics_file;
extern FILE                     *cb_sql_file;
extern FILE                     *cb_dl1_file;

extern FILE                     *cb_depend_file;
extern char                     *cb_depend_target;
extern struct cb_text_list      *cb_depend_list;
extern struct cb_inc_list       *cb_include_list;
extern struct cb_text_list      *cb_extension_list;

extern struct cb_program        *external_program_list;
extern struct cb_program        *current_program;
extern struct cb_statement      *current_statement;
extern struct cb_label          *current_section;
extern struct cb_label          *current_paragraph;
extern struct cb_program        *current_codegen_prog;
extern size_t                   functions_are_all;
extern struct cb_field          *typedef_storage;
extern int                       cb_disable_runtime_check;

/* db2 parser (in db2lexer.l, db2parser.y)  by K.I Metrixware maghreb*/
extern FILE		*mmdin;
extern FILE		*mmdout;
extern int		mmdlex (void);
extern int		mmdparse (void);

/* Cics parser (in CicsLexer.l, CicsParser.y) by K.I Metrixware maghreb */
extern FILE		*mmcin;
extern FILE		*mmcout; 
extern int		mmclex (void);
extern int		mmcparse (void);

/* DLI parser (in Dl1Lexer.l, Dl1Parser.y) by L.H.I Metrixware maghreb */
extern FILE		*mmdlin;
extern FILE		*mmdlout;
extern int		mmdllex (void);
extern int		mmdlparse (void);

/*CobolIt*/
extern int                      cb_flag_all_extfh;
extern int                      cb_flag_extsm;
extern char                     *cb_extfh_name;
extern char                     *cb_extsm_name;
extern int                      cb_call_num;
extern int                      cb_if_level;
extern char                     *ext_prepro_name;
extern char                     *linkage_descriptor_name;
extern int                      cb_prepro_initial_pass;
extern int                      cb_raw_debug_line ;
extern struct cb_constant_list  *cb_constants;
extern int                      cb_flag_debuginfo;
extern int                      cb_flag_runtimecheck;
extern int                      cb_warn_undefine;
extern int                      cb_optimize_size;
extern int                      cb_pplinemark;
extern int                      cur_profiling_idx; 
extern struct cb_text_list      *cb_once_list;
extern char                     *cb_cics_include;
extern struct cb_text_list      *cb_initcall_list;
extern int                      exit_status;
extern int                      cb_generating_initialize;
extern void                     *cb_debugdb;
extern long long                cb_debugdb_module_id;
extern char                     *cb_debugdb_name;

extern struct cb_file_status_map_list  *cb_file_status_map;
extern struct cb_file_status_map_list  *cb_crt_status_map;

extern struct cb_inc_list       *cb_include_list_file_add (struct cb_inc_list *list, const char *fname);
extern struct cb_inc_list       *cb_include_list_add (struct cb_inc_list *list, const char *name);
extern struct cb_text_list      *cb_text_list_prefix (struct cb_text_list *list, const char *text);
extern struct cb_text_list      *cb_text_list_add (struct cb_text_list *list, const char *name);
extern void                     *cobc_malloc (const size_t size);
extern void                     *cobc_realloc (void *prevptr, const size_t size);

extern void     cobc_terminate (const char *str);
#ifdef  __GNUC__
extern void     cobc_abort (const char *filename, const int linenum) __attribute__ ((noreturn));
#else
extern void     cobc_abort (const char *filename, const int linenum);
#endif

extern size_t                   cobc_check_valid_name (const char *name);

/* config.c */

struct noreserve {
    struct  noreserve       *next;
    char                    *noresword;
};

extern struct noreserve *norestab;

/*CIT*/
struct staticlink {
    struct  staticlink      *next;
    char                    *staticlink_symb;
    int                      external_link;
    int                      static_link;
};
extern struct staticlink *staticlinktab;
extern struct staticlink * cobc_find_static_symb (const char * val);
extern int cobc_confvalue (const char *value_name);
extern char* cobc_find_constant (char *key, char *value, int is_makesyn, int *is_userconst);
extern int cobc_dump_config (FILE *f, int listing, char *prefix);
#include "stringutils.h"


enum cb_assign_clause {
    /*CB_ASSIGN_COBOL2002,            COBOL 2002 standard */
    CB_ASSIGN_MF,                   /* Micro Focus COBOL compatibility */
    CB_ASSIGN_IBM,                  /* IBM COBOL compatibility */
    CB_ASSIGN_EXTERNAL,             /* Micro Focus COBOL compatibility ASSIGN"EXTERNAL"*/
    CB_ASSIGN_DYNAMIC               /* Micro Focus COBOL compatibility ASSIGN"DYNAMIC"*/

};

enum cb_binary_byteorder {
    CB_BYTEORDER_NATIVE,
    CB_BYTEORDER_BIG_ENDIAN
};

enum cb_binary_size {
    CB_BINARY_SIZE_2_4_8,           /* 2,4,8 bytes */
    CB_BINARY_SIZE_1_2_4_8,         /* 1,2,4,8 bytes */
    CB_BINARY_SIZE_1__8             /* 1,2,3,4,5,6,7,8 bytes */
};

enum cb_operation_type {
    CB_OPERATION_READ,
    CB_OPERATION_WRITE,
    CB_OPERATION_ASSIGN
};

enum cb_support {
    CB_OK,
    CB_WARNING,
    CB_ARCHAIC,
    CB_OBSOLETE,
    CB_SKIP,
    CB_IGNORE,
    CB_ERROR,
    CB_UNCONFORMABLE
};

/*CIT*/
enum cb_sticky_linkage_mode {
    CB_STICKY_NONE = 0,            /* DEFAULT */
    CB_STICKY_FIX = 1,             /* Micro Focus COBOL compatibility */
    CB_STICKY_VARIABLE = 2         /* IBM COBOL compatibility */
};

enum cb_move_x_to_9_mode {
    CB_MOVExTO9_NONE = 0,             /* DEFAULT */
    CB_MOVExTO9_MF50 = 1,             /* Micro Focus 5.0 COBOL compatibility */
    CB_MOVExTO9_MVS  = 2,             /* IBM MVS COBOL compatibility */
    CB_MOVExTO9_RAW  = 3,             /* Raw move pad with space */
    CB_MOVExTO9_ISO  = 4,             /* Force normal move   */
    CB_MOVExTO9_MF40 = 5,             /* Micro Focus 4.0  (copy low digit)   */
    CB_MOVExTO9_RAW_0  = 6            /* Raw move pad with 0  */
};

enum cb_move_sp_to_9_mode {
    CB_MOVEspTO9_ERROR  = 0,           /* DEFAULT not allowed */
    CB_MOVEspTO9_ZERO   = 1,           /* Change SPACE in ZERO */
    CB_MOVEspTO9_SPACE  = 2            /* Micro Focus COBOL compatibility move SPACE*/
};

enum cb_move_high_low_to_9_mode {
    CB_MOVEhlTO9_ERROR  = 0,           /* ERROR not allowed */
    CB_MOVEhlTO9_ZERO   = 1,           /* Change high low to ZERO */
    CB_MOVEhlTO9_VALUE  = 2            /* DEFAULT*/
};

#undef CB_CONFIG_ANY
#undef CB_CONFIG_INT
#undef CB_CONFIG_STRING
#undef CB_CONFIG_BOOLEAN
#undef CB_CONFIG_SUPPORT
#undef CB_CONFIG_ANY_OPT
#undef CB_CONFIG_INT_OPT
#undef CB_CONFIG_STRING_OPT
#undef CB_CONFIG_BOOLEAN_OPT
#undef CB_CONFIG_SUPPORT_OPT

#define CB_CONFIG_ANY(type,var,name)    extern type var;
#define CB_CONFIG_INT(var,name)         extern int var;
#define CB_CONFIG_STRING(var,name)      extern char *var;
#define CB_CONFIG_BOOLEAN(var,name)     extern int var;
#define CB_CONFIG_SUPPORT(var,name)     extern enum cb_support var;
/*CIT*/
#define CB_CONFIG_ANY_OPT(type,var,name,default)    extern type var;
#define CB_CONFIG_INT_OPT(var,name,default)         extern int var;
#define CB_CONFIG_STRING_OPT(var,name,default)      extern char *var;
#define CB_CONFIG_BOOLEAN_OPT(var,name,default)     extern int var;
#define CB_CONFIG_SUPPORT_OPT(var,name,default)     extern enum cb_support var;
#include "config.def"
#undef  CB_CONFIG_ANY
#undef  CB_CONFIG_INT
#undef  CB_CONFIG_STRING
#undef  CB_CONFIG_BOOLEAN
#undef  CB_CONFIG_SUPPORT

#undef CB_LONGOPT_BOOLEAN
#undef CB_LONGOPT_SET
#undef CB_LONGOPT_STRING
#undef CB_LONGOPT_OPT
#undef CB_LONGOPT_SHORT
#define CB_LONGOPT_BOOLEAN(name, val, var) extern int var;                                   
#define CB_LONGOPT_SET(name, var, setval, setvar) extern int var;                                   
#define CB_LONGOPT_STRING(name, val, var)  extern char *var;                                   
#define CB_LONGOPT_OPT(name, val, var)  extern char *var;                                   
#define CB_LONGOPT_SHORT(name, val, var)  extern char *var;                                   
#include "longopt.def" 
#undef CB_LONGOPT_BOOLEAN
#undef CB_LONGOPT_SET
#undef CB_LONGOPT_STRING
#undef CB_LONGOPT_OPT
#undef CB_LONGOPT_SHORT

extern int      cb_load_std (const char *name);
extern int      cb_load_conf (const char *fname, int check_nodef, int complain);

/* preprocessor (in pplex.l, ppparse.y) */
extern FILE     *ppin;
extern FILE     *ppout;
extern int      pplex (void);
extern int      ppparse (void);
extern int      ppopen (char *name, struct cb_replace_list *replace_list);
extern int      ppcopy (char *name, const char *lib, struct cb_replace_list *replace_list);
extern void     pp_set_replace_list (struct cb_replace_list *replace_list, int add, int global);

/* parser (in scanner.l, parser.y) */
extern FILE     *yyin;
extern FILE     *yyout;
extern int      yylex (void);
extern int      yyparse (void);

/* typeck.c */
extern size_t   sending_id;
extern size_t   suppress_warn;

/* error.c */
#ifdef __GNUC__
extern void     cb_warning (const char *fmt, ...)
__attribute__ ((__format__ (__printf__, 1, 2)));
extern void     cb_error (const char *fmt, ...)
__attribute__ ((__format__ (__printf__, 1, 2)));
#else
extern void     cb_warning (const char *fmt, ...);
extern void     cb_error (const char *fmt, ...);
#endif

extern int      cb_verify (enum cb_support tag, const char *feature);

/* codegen.c */
#include "tree.h"
extern cb_lex_source_file_info  cb_lex_source_file;

extern struct  cit_runtime_s * rtd;
extern void     cob_strlower(char *s) ;
extern void     cob_strupper (char *s);
extern int      lookup_string (const unsigned char * data, int size);
extern struct attr_list *cb_get_attr (int id);
extern int      cb_build_attr (cb_tree x, int is_bitarray);
extern unsigned get_runtime_flags (void) ;
extern unsigned get_runtime_call_flags (void) ;

extern void     output (const char *fmt, ...);
extern void     output_line (const char *fmt, ...);
extern void     output_indent (const char *str);
extern void     output_global (const char *fmt, ...);
extern void     output_local (const char *fmt, ...);
extern void     output_local_register (const char *fmt, ...);
extern void     output_local_function (const char *fmt, ...);
extern void     output_static (const char *fmt, ...);
extern void     output_storage (const char *fmt, ...);
extern void     output_debug (const char *fmt, ...) ;
extern void     output_perform_label (struct cb_label *lb, struct cb_label *le);
/* CIT Enterprise stuff */
/* enterprise.c */

extern char * cobc_temp_name (const char *prefix, const char *ext);
extern char * cobc_add_constant (char*arg, int is_makesyn, int is_userconst);
extern int cobc_stat (const char *name, struct stat     *st);
extern int cobc_exist_file(const char *name);
extern int cobc_hash_fname (const unsigned char *s);
extern void cobc_find_lex_hash(const int h);
extern void cobc_set_lex_file_name(const unsigned char *s);


#include "libcitenterprise/citenterprise_runtime.h"
#include "enterprise/a2e.h"

#endif /* CB_COBC_H */

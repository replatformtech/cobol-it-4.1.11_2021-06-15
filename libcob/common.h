/*
 * Copyright (C) 2002-2007 Keisuke Nishida
 * Copyright (C) 2007 Roger While
  * Copyright (C) 2008-2020 Cobol-IT
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

#ifndef COB_COMMON_H
#define COB_COMMON_H

#ifdef _MSC_VER

    #ifndef _CRT_SECURE_NO_DEPRECATE 
        #define _CRT_SECURE_NO_DEPRECATE 1
    #endif
    #include <stdio.h>
    #include <stdlib.h>
    #define inline _inline
    #define COB_INLINE _inline
    #include <malloc.h>
    #include <io.h>
    #include <fcntl.h>
    #include <math.h>
    #pragma warning(disable: 4996)
    #define __attribute__(x)
    #define __i386__
    #define snprintf _snprintf

    #define COB_CALL_TARGET __declspec(dllexport) 
    #ifdef LIBCOB_EXPORTS
        #define COB_DLL_EXPIMP __declspec(dllexport) 
        #define COB_DLL_EXPORT __declspec(dllexport) 
    #else /* LIBCOB_EXPORTS */
        #define COB_DLL_EXPIMP __declspec(dllimport) extern 
        #define COB_DLL_EXPORT
    #endif /* LIBCOB_EXPORTS */

#else /* _MSC_VER */
    #include <stdio.h>
    #include <math.h>

    #define COB_CALL_TARGET  
    #define COB_DLL_EXPIMP extern 
    #define COB_DLL_EXPORT

    #ifdef  __370__
        #define inline __inline
        #define COB_INLINE __inline
    #elif defined(COB_HAS_INLINE)
        #define COB_INLINE inline
    #else
        #define COB_INLINE
    #endif
    #if defined(__GNUC__) && (__GNUC__ >= 3)
        #if !defined(__stdcall)
            #define __stdcall __attribute__((__stdcall__))
        #endif
    #else
        #if !defined(__stdcall)
            #define __stdcall 
        #endif
    #endif


#endif /* _MSC_VER */

#if defined(__GNUC__) && (__GNUC__ >= 3)
    #define likely(x)   __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
    #if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
        #define COB_NOINLINE    __attribute__((noinline))
    #else
        #define COB_NOINLINE
    #endif
#else
    #define likely(x)   (x)
    #define unlikely(x) (x)
    #define COB_NOINLINE
#endif

#if ' ' == 0x40
    #define COB_EBCDIC_MACHINE
#endif

#include "cit_rtinterface.h"
#include "citenterprise.h"
#include "cit_types.h"

typedef unsigned char * ucharptr;

#define COB_MINI_MAX        (COB_MINI_BUFF - 1)
#define COB_SMALL_MAX       (COB_SMALL_BUFF - 1)
#define COB_NORMAL_MAX      (COB_NORMAL_BUFF - 1)
#define COB_MEDIUM_MAX      (COB_MEDIUM_BUFF - 1)
#define COB_LARGE_MAX       (COB_LARGE_BUFF - 1)
#ifndef COB_STACK_SIZE
    #define COB_STACK_SIZE      255
#endif

#define COB_MAX_FIELD_PARAMS    64

/*
 * External
 */

struct cob_external {
    struct cob_external *next;
    char                *ext_alloc;
    char                *ename;
    int                 esize;
};

/*
 * Field
 */

/* field types */

#define COB_TYPE_UNKNOWN                0x00
#define COB_TYPE_GROUP                  0x01
#define COB_TYPE_BOOLEAN                0x02
#define COB_TYPE_BITS                   0x05
#define COB_TYPE_BITS_ARRAY             0x06
#define COB_TYPE_MASTER_MASK            0xF0
#define COB_TYPE_SUB_MASK               0x0F

#define COB_TYPE_NUMERIC                0x10
#define COB_TYPE_NUMERIC_DISPLAY        0x10
#define COB_TYPE_NUMERIC_BINARY         0x11
#define COB_TYPE_NUMERIC_PACKED         0x12
#define COB_TYPE_NUMERIC_FLOAT          0x13
#define COB_TYPE_NUMERIC_DOUBLE         0x14
#define COB_TYPE_NUMERIC_BITS           0x15
#define COB_TYPE_NUMERIC_EDITED         0x24

#define COB_TYPE_ALPHANUMERIC           0x21
#define COB_TYPE_ALPHANUMERIC_ALL       0x22
#define COB_TYPE_ALPHANUMERIC_EDITED    0x23
#define COB_TYPE_ALPHANUMERIC_BITS      0x25

#define COB_TYPE_NATIONAL               0x40
#define COB_TYPE_NATIONAL_EDITED        0x41
#define COB_TYPE_FIELD_LIST             0x80


/* field flags */

#define COB_FLAG_HAVE_SIGN              0x01
#define COB_FLAG_PACKED_SIGN_MISSING    0x02   /* COMP-6 */
#define COB_FLAG_BINARY_NOTRUNC         0x02   /* COMP-X */
#define COB_FLAG_SIGN_SEPARATE          0x02
#define COB_FLAG_SIGN_LEADING           0x04
#define COB_FLAG_BLANK_ZERO             0x08
#define COB_FLAG_JUSTIFIED              0x10
#define COB_FLAG_BINARY_SWAP            0x20
#define COB_FLAG_REAL_BINARY            0x40
#define COB_FLAG_IS_POINTER             0x80

/* Runtime behave flag */
#define COB_FLAG_RT_MF_SPZERO           0x00000001
#define COB_FLAG_RT_DISPLAY_MF50        0x00000002
#define COB_FLAG_RT_MOVE_XTO9_MVS       0x00000004
#define COB_FLAG_RT_ROUND_FP            0x00000008
#define COB_FLAG_RT_MOVE_XTO9_RAW       0x00000010
#define COB_FLAG_RT_MOVE_XTO9_ISO       0x00000020
#define COB_FLAG_RT_ZERO_LEN_TRIM       0x00000040
#define COB_FLAG_RT_MF_HOSTNUMCOMPARE_1 0x00000080
#define COB_FLAG_RT_GMP_ONLY_MATH       0x00000100
#define COB_FLAG_RT_MOVE_XTO9_MF40      0x00000200
#define COB_FLAG_RT_CAREALIA_SIGN       0x00000400
#define COB_FLAG_RT_COMPUTE_IBM         0x00000800
#define COB_UNSTRING_MOVE               0x00001000
#define COB_FLAG_RT_DISPLAY_IBM         0x00002000
#define COB_FLAG_RT_OPTIMIZE_OPERATION  0x00004000
#define COB_FLAG_RT_STRICT_COMPARE_LOW  0x00008000
#define COB_FLAG_RT_MOVE_XTO9_RAW_0     0x00010000
#define COB_FLAG_RT_DISPLAY_VMS         0x00020000

/* setting action code */
#define COB_SAC_CLEAR_CURSOR_POS        1

#define COB_FIELD_HAVE_SIGN(f)          ((f)->attr->flags & COB_FLAG_HAVE_SIGN)
#define COB_FIELD_PACKED_SIGN_MISSING(f) ((f)->attr->flags & COB_FLAG_PACKED_SIGN_MISSING)
#define COB_FIELD_SIGN_SEPARATE(f)      ((f)->attr->flags & COB_FLAG_SIGN_SEPARATE)
#define COB_FIELD_SIGN_LEADING(f)       ((f)->attr->flags & COB_FLAG_SIGN_LEADING)
#define COB_FIELD_BLANK_ZERO(f)         ((f)->attr->flags & COB_FLAG_BLANK_ZERO)
#define COB_FIELD_JUSTIFIED(f)          ((f)->attr->flags & COB_FLAG_JUSTIFIED)
#define COB_FIELD_BINARY_SWAP(f)        ((f)->attr->flags & COB_FLAG_BINARY_SWAP)
#define COB_FIELD_REAL_BINARY(f)        ((f)->attr->flags & COB_FLAG_REAL_BINARY)
#define COB_FIELD_IS_POINTER(f)         ((f)->attr->flags & COB_FLAG_IS_POINTER)

#define COB_ATTR_INIT(v,w,x,y,z)        do { \
        attr.type = v; \
        attr.digits = w; \
        attr.scale = x; \
        attr.flags = y; \
        attr.pic = z; \
        } while (0)

#define COB_AS_COB_FIELD(f)   ((cob_field*)f)

#define COB_FIELD_TYPE(f)       ((f)->attr->type)
#define COB_FIELD_DIGITS(f)     ((f)->attr->digits)
#define COB_FIELD_SCALE(f)      ((f)->attr->scale)
#define COB_FIELD_FLAGS(f)      ((f)->attr->flags)
#define COB_FIELD_PIC(f)        ((f)->attr->pic)
#define COB_FIELD_DATA(f)                                                   \
  ((f)->data +                                                              \
   ((COB_FIELD_SIGN_SEPARATE (f) && COB_FIELD_SIGN_LEADING (f)) ? 1 : 0))
#define COB_FIELD_SIZE(f)                                                   \
  ((f)->size - (COB_FIELD_SIGN_SEPARATE (f) ? 1 : 0))

#define COB_FIELD_IS_NUMERIC(f)  ((COB_FIELD_TYPE (f) & COB_TYPE_MASTER_MASK) == COB_TYPE_NUMERIC)
#define COB_FIELD_IS_NUMERIC_OR_EDITED(f)  (COB_FIELD_IS_NUMERIC(f) || (COB_FIELD_TYPE (f)  == COB_TYPE_NUMERIC_EDITED))
#define COB_FIELD_IS_NATIONAL(f) ((COB_FIELD_TYPE (f) & COB_TYPE_MASTER_MASK) == COB_TYPE_NATIONAL)
#define COB_FIELD_IS_BITS(f) ((COB_FIELD_TYPE (f) & COB_TYPE_SUB_MASK) == COB_TYPE_BITS)
#define COB_FIELD_IS_BITSARRAY(f) ((COB_FIELD_TYPE (f) & COB_TYPE_SUB_MASK) == COB_TYPE_BITS_ARRAY)
#define COB_FIELD_IS_PACKED(f)    (COB_FIELD_TYPE (f)  == COB_TYPE_NUMERIC_PACKED)
#define COB_FIELD_IS_DISPLAY(f)    (COB_FIELD_TYPE (f) == COB_TYPE_NUMERIC_DISPLAY)


/* SIGN */

/*
 * positive: 0123456789
 * negative: pqrstuvwxy
 */
/*#define GET_SIGN_ASCII(x) x = 0x40*/      
                                            /*p = -0*/ 
#define PUT_SIGN_ASCII(x) x = ((x & 0x0F) | 0x70)
#define PUT_SIGN_CAREALIA(x) x = ((x & 0x0F) | 0x20)

#define COB_DISPLAY_SIGN_ASCII  0
#define COB_DISPLAY_SIGN_EBCDIC 1
/*
 * Flag Pretty display
 */
#define COB_NUMERIC_PRETTY_DISPLAY      0x01
#define COB_NUMERIC_RAW_DISPLAY         0x02
/*
 * Module
 */
/* structure for FILE Status translate*/
typedef struct cob_file_status_map_item {
    union {
        int crt_status;
        unsigned char f_status[2];
    } cit_status;
    union {
        int crt_status;
        unsigned char f_status[2];
    } custome_status;
} cob_file_status_map_item;

typedef struct cob_crt_status_map_item {
    unsigned int cit_status;
    unsigned int custome_status;
} cob_crt_status_map_item;

typedef struct cob_file_status_map {
    unsigned short                     count;
    const cob_file_status_map_item    *items;
} cob_file_status_map;

typedef struct cob_crt_status_map {
    unsigned short                     count;
    const cob_crt_status_map_item     *items;
} cob_crt_status_map;

/* field structure debug info*/
typedef struct cob_field_debug_info {
    const int                       field_id;
    const char                      *name;
    const int                       occur;
    const char                      *redefine_name;
    struct cob_field_debug_info     *child;
    const char                      *source_file;
    const int                       line_nr;      
}cob_field_debug_info;

typedef struct cob_lines_debug_info {
    int                             line_nr; 
    const char                      *source_file;
    const char                      *label;
    char                            debug_status;
    char                            is_section;
    struct cob_condition_debug_info *condition;

} cob_lines_debug_info;


typedef struct cob_stack_debug_info {
    int                             line_nr; 
    const char                      *source_file;
} frame_ptr;

/* structure for Profiling*/
typedef struct cob_paragraph_profiling_times {
    unsigned long long              cpu_tick;                   
    unsigned long long              elaps_tick;                   

} cob_paragraph_profiling_times;

typedef struct cob_paragraph_profiling_info {
    const char *                    name;
    unsigned long                   entry_cnt;
    unsigned long                   tick_cnt;
    unsigned long                   external_call_tick;
    unsigned long                   tick_cnt_elaps;
    unsigned long                   external_call_tick_elaps;
} cob_paragraph_profiling_info;


typedef cob_field * (*get_cob_field_t)(void*, int);

enum cob_device_name {
    COB_DEVICE_SYSIN,
    COB_DEVICE_SYSOUT,
    COB_DEVICE_SYSERR,
    COB_DEVICE_CONSOLE,
    COB_DEVICE_PRINTER
};

#define LINES_DEBUG_INFO_VERSION 1
typedef struct __cob_module {
    struct __cob_module             *next;
    unsigned char                   *collating_sequence;
    cob_field                       *crt_status;
    cob_field                       *cursor_pos;
    cob_field                       **cob_procedure_parameters;
    unsigned char                   display_sign;
    unsigned char                   decimal_point;
    unsigned char                   currency_symbol;
    unsigned char                   numeric_separator;
    unsigned char                   flag_filename_mapping;
    unsigned char                   flag_binary_truncate;
    unsigned char                   flag_pretty_display;
    unsigned char                   module_version;
    char                            *module_name;
    char                            *source_file;
    char                            *cur_source_file;
    unsigned int                    cur_line;
    unsigned int                    cur_line_debug_idx;

    /* debug and dump info*/
    struct cob_field_debug_info     *fields_debug_info;
    get_cob_field_t                 get_cob_field;
    void                            *data_storage;

    cob_lines_debug_info            *lines_debug_info;
    unsigned long                    profiling_last_cpu_tick_v0;
    struct cob_paragraph_profiling_info *profiling_info;
    unsigned int                     runtime_flags;
    const char                     * default_cp;
/* VERSION > 0 */
    const cob_file_status_map      * file_status_map;
    cob_paragraph_profiling_times    profiling_last;
    cob_paragraph_profiling_times    profiling_delta;
/* VERSION > 1 */
    void                            *sysin_file;
    void                            *sysout_file;
    void                            *syserr_file;
    const cob_crt_status_map        * crt_status_map;
    struct data_list                * cbl_allocated_list; 
    struct cob_stack_debug_info     *debug_stack_frame_start;
    struct cob_stack_debug_info     *debug_stack_frame_curpos;
    const char                      *debug_cur_source_file;
    void                            *xmldata;
    void                            *sysprint_file;
    int                             *ccmapdata;
    char                            *build_stamp;
    struct cob_field_debug_info     *debugdb_fields_debug_info;
    struct cob_field_debug_info     *debugdb_fdi_store;
    char                            *debugdb_name;
    char                            *debugdb_cachepath;
    struct data_list                ** cbl_allocated_list_ptr;
    void                            *dummyptr[6];
    char                            utf16_le;
    char                            ebcdic_charset;
    char                            lines_debug_info_version;
    char                            dummychar[1];
    char                            no_cbl_error_proc;
    char                            display_dos;
    char                            xparse_event;
    char                            emulate_vms;
    int                             debug_cur_line;
    int                             module_age;
    int                             debugdb_fdi_count;
    int                             debugdb_lines_count;
    int                             ccmapdata_size;
    int                             dummyint[2];
/* VERSION > 2 */
    int                             numval_validate;
    int                             dummyintv3[4];
    long long                       debugdb_moduleid;
    long long                       dummyll[10];
} cob_module;

/*
 * Global variables (all thread shared)
 */

COB_DLL_EXPIMP cob_field     cob_zero;       /* ZERO */
COB_DLL_EXPIMP cob_field     cob_space;      /* SPACE */
COB_DLL_EXPIMP cob_field     cob_high;       /* HIGH-VALUE */
COB_DLL_EXPIMP cob_field     cob_low;        /* LOW-VALUE */
COB_DLL_EXPIMP cob_field     cob_quote;      /* QUOTE */
COB_DLL_EXPIMP cob_field     cob_one;        /* Numeric ONE */

/*
 * Exception
 */

/* Exception identifier */
#undef  COB_EXCEPTION
#define COB_EXCEPTION(code,tag,name,critical)   tag,

enum cob_exception_id {
    COB_EC_ZERO,
#include <libcob/exception.def>
    COB_EC_MAX
};

#undef  COB_EXCEPTION

/*
 * Fatal error
 */

#define COB_FERROR_INITIALIZED  0
#define COB_FERROR_CODEGEN      1
#define COB_FERROR_CHAINING     2
#define COB_FERROR_STACK        3
#define COB_FERROR_UNCOMPRESS   4
#define COB_FERROR_NEGSCALE     5
#define COB_FERROR_UNCATCH_EXP  6


/*
 * for the debuger
        int (*func)();
*/
#define COB_DBCALLBACK_ENTER_MODULE     0x01
#define COB_DBCALLBACK_LEAVE_MODULE     0x02
#define COB_DBCALLBACK_SETLOCATION      0x03
#define COB_DBCALLBACK_EXIT             0x05
#define COB_DBCALLBACK_PERFORM_ENTER    0x06
#define COB_DBCALLBACK_PERFORM_EXIT     0x07
#define COB_DBCALLBACK_ENTER_EXCEPTION  0x08
#define COB_DBCALLBACK_CPU_EXCEPTION    0x09

/* convert a digit (e.g., '0') into an integer (e.g., 0) */
#define COB_IS_DDIGIT(x)      ((x == CHAR_SP) || (x == 0) || (x >= CHAR_0 && x <= CHAR_9) )

/* convert a digit (e.g., '0') into an integer (e.g., 0) */
#define COB_D2I_CHAR(x)        ((x == CHAR_SP) || (x == 0) ? 0: (x) - CHAR_0)
#define COB_D2I_CHAR_n(x)      ((x == ' ') || (x == 0) ? 0: (x) - '0')
#define COB_D2I(x)      (x & 0x0f)
#define COB_D2I_MF50(x) ((x) - CHAR_0)

/* convert an integer (e.g., 0) into a digit (e.g., '0') */
#define COB_I2D(x)        ((x) + CHAR_0)
#define COB_I2D_l(x)        ((x) + c0)   /* suppose c0 local intialized to CHAR_0*/
#define COB_I2D_n(x)      ((x) + '0')

#include "cit_rtinterface.h"

COB_DLL_EXPIMP void          (*cob_debug_callback)(COB_RTD, int action_code);

/*
 * Function declaration
 */

/* thread safe runtime and modules memory */  
COB_DLL_EXPIMP cit_runtime_t *       cob_get_rtd            (void);
COB_DLL_EXPIMP void                  cob_set_rtd            (COB_RTD);
COB_DLL_EXPIMP void                  cob_rtd_tidy           (COB_RTD);
COB_DLL_EXPIMP void                  *cob_get_module_storage(COB_RTD, char *key , int size);
COB_DLL_EXPIMP void                  cob_free_module_storage(COB_RTD, char *key , int size);
COB_DLL_EXPIMP void                  cob_set_exit_rtd_proc  (COB_RTD, cob_rtd_exit_proc cob_exit_proc);
/*COB_DLL_EXPIMP int                   cob_setjmp (COB_RTD);*/


/* General functions */
COB_DLL_EXPIMP void cob_init                (COB_RTD, int argc, char **argv);
COB_DLL_EXPIMP void cob_set_command_line    (COB_RTD, const char *prgname, const char *cmdl);
COB_DLL_EXPIMP void cob_cmdline_to_argv     (COB_RTD, const char *prgname, const char *cmdl, int *argc, char **argv, int argvsize);
COB_DLL_EXPIMP void cob_set_sysin_file      (COB_RTD, char *filename);
COB_DLL_EXPIMP void cob_set_sysout_file     (COB_RTD, char *filename);
COB_DLL_EXPIMP void cob_set_syserr_file     (COB_RTD, char *filename);
COB_DLL_EXPIMP void cob_redirect_sysfile    (COB_RTD, void *file, int filenr) ;

COB_DLL_EXPIMP void cob_module_enter        (COB_RTD, cob_module *module);
COB_DLL_EXPIMP void cob_module_leave        (COB_RTD, cob_module *module);
COB_DLL_EXPIMP void cob_terminate_exec      (COB_RTD);
#ifdef __GNUC__
extern void cobexit                         (int status) __attribute__ ((noreturn));
extern void cob_stop_run                    (COB_RTD, const int status) __attribute__ ((noreturn));
extern void cob_stop_abend                  (COB_RTD, const runtime_error_code_t status)  __attribute__ ((noreturn));
extern void cob_fatal_error                 (COB_RTD, const unsigned int fatal_error) __attribute__ ((noreturn));
extern void *cob_malloc                     (COB_RTD, const size_t size)__attribute__ ((malloc));
#else
COB_DLL_EXPIMP void cob_stop_run            (COB_RTD, const int status);
COB_DLL_EXPIMP void cob_stop_abend          (COB_RTD, const runtime_error_code_t status) ;
COB_DLL_EXPIMP void cob_fatal_error         (COB_RTD, const unsigned int fatal_error);
COB_DLL_EXPIMP void *cob_malloc             (COB_RTD, const size_t size);
#endif

COB_DLL_EXPIMP void coblib_free             (void *mptr);
#define cob_free(x) {if (x) {coblib_free(x); (x) = NULL;}}

COB_DLL_EXPIMP void cob_runtime_error       (COB_RTD, const char *fmt, ...);
COB_DLL_EXPIMP void cob_set_errormsg_proc   (COB_RTD, cob_errmsg_proc proc);
COB_DLL_EXPIMP void cob_set_errorfile       (COB_RTD, FILE* errfile);
COB_DLL_EXPIMP void cob_free_all_module_storage  (COB_RTD);

/* dump info */
COB_DLL_EXPIMP int           cob_get_last_status    (COB_RTD);
COB_DLL_EXPIMP char *        cob_get_last_errormsg  (COB_RTD);
COB_DLL_EXPIMP cob_module *  cob_get_current_module (COB_RTD);
COB_DLL_EXPIMP void          cob_print_version (const char*product);



COB_DLL_EXPIMP const char *cob_get_exception_name (COB_RTD, const int exception_code);
COB_DLL_EXPIMP int  cob_set_exception       (COB_RTD, const int id);
COB_DLL_EXPIMP void cob_check_version       (COB_RTD, const char *prog, const char *packver, const int patchlev);
COB_DLL_EXPIMP void cob_check_version_1     (COB_RTD, const char *prog, const char *packver, const int patchlev, int ebcdic_charset);
COB_DLL_EXPIMP void cob_accept_date         (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_date_yyyymmdd(COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_day          (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_day_yyyyddd  (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_day_of_week  (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_time         (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_display_command_line(COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_command_line (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_set_environment     (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void cob_display_environment (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_get_environment     (COB_RTD, cob_field *envname, cob_field *envval);
COB_DLL_EXPIMP void cob_accept_environment  (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_display_env_value   (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_display_arg_number  (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_arg_number   (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_accept_arg_value    (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_chain_setup         (COB_RTD, void *data, const size_t parm, const size_t size);
COB_DLL_EXPIMP void cob_allocate            (COB_RTD, unsigned char **dataptr, cob_field *retptr,
                                             cob_field *sizefld);
COB_DLL_EXPIMP void cob_free_alloc          (COB_RTD, unsigned char **ptr1, unsigned char *ptr2);
COB_DLL_EXPIMP char *cob_strerror           (COB_RTD, int err);
COB_DLL_EXPIMP char *cob_linkage_command_line (COB_RTD);
COB_DLL_EXPIMP char *cob_tmp_dir (void);
COB_DLL_EXPIMP char *cob_debug_tmp_dir (void);
COB_DLL_EXPIMP void cob_add_to_datalist (COB_RTD, void *f, struct data_list **cachehead);
COB_DLL_EXPIMP void cob_remove_from_datalist (COB_RTD, void *f, struct data_list **cachehead);
COB_DLL_EXPIMP void cob_remove_all_from_datalist (COB_RTD, struct data_list **cachehead);
COB_DLL_EXPIMP void *cob_malloc_cbl_allocation (COB_RTD, int size, int thread, int module);
COB_DLL_EXPIMP void cob_free_cbl_allocation (COB_RTD, void *data);

/* MF Compatible */
COB_DLL_EXPIMP int  cobinit (void);
COB_DLL_EXPIMP int  cobtidy (void);
COB_DLL_EXPIMP void cobexit (const int status);
COB_DLL_EXPIMP void *cobcommandline (int flags, int *pargc, char ***pargv,
                                     char ***penvp, char **pname);
COB_DLL_EXPIMP char *cobgetenv (const char *name);
COB_DLL_EXPIMP int  cobputenv (char *name);

/* System routines */
COB_DLL_EXPIMP int rtd_CBL_ALLOC_MEM      (COB_RTD, void  *pptr, int size, int flag) ;
COB_DLL_EXPIMP int rtd_CBL_FREE_MEM       (COB_RTD, void  *ptr);
COB_DLL_EXPIMP int rtd_CBL_ERROR_PROC     (COB_RTD, unsigned char *x, unsigned char *pptr);
COB_DLL_EXPIMP int rtd_CBL_EXIT_PROC      (COB_RTD, unsigned char *x, unsigned char *pptr);
COB_DLL_EXPIMP int rtd_SYSTEM             (COB_RTD, const unsigned char *cmd);
COB_DLL_EXPIMP int rtd_CBL_AND            (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int rtd_CBL_OR             (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int rtd_CBL_NOR            (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int rtd_CBL_XOR            (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int rtd_CBL_IMP            (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int rtd_CBL_NIMP           (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int rtd_CBL_EQ             (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int rtd_CBL_NOT            (COB_RTD, unsigned char *data_1, const int length);
COB_DLL_EXPIMP int rtd_CBL_XAF            (COB_RTD, unsigned char *code, unsigned char *param) ;
COB_DLL_EXPIMP int rtd_CBL_XF4            (COB_RTD, unsigned char *data_1, unsigned char *data_2);
COB_DLL_EXPIMP int rtd_CBL_XF5            (COB_RTD, unsigned char *data_1, unsigned char *data_2);
COB_DLL_EXPIMP int rtd_CBL_X91            (COB_RTD, unsigned char *result, const unsigned char *func, unsigned char *parm);
COB_DLL_EXPIMP int rtd_CBL_TOUPPER        (COB_RTD, unsigned char *data, const int length);
COB_DLL_EXPIMP int rtd_CBL_TOLOWER        (COB_RTD, unsigned char *data, const int length);
COB_DLL_EXPIMP int rtd_cob_caller_name    (COB_RTD, unsigned char *data) ;
COB_DLL_EXPIMP int rtd_cob_program_id     (COB_RTD, unsigned char *data) ;
COB_DLL_EXPIMP int rtd_cob_return_args    (COB_RTD, unsigned char *data);
COB_DLL_EXPIMP int rtd_CBL_OC_NANOSLEEP   (COB_RTD, unsigned char *data);
COB_DLL_EXPIMP int rtd_cob_parameter_size (COB_RTD, unsigned char *data);
COB_DLL_EXPIMP int rtd_cob_parameter_type (COB_RTD, unsigned char *data);
COB_DLL_EXPIMP int rtd_cob_acuw_sleep     (COB_RTD, unsigned char *data);
COB_DLL_EXPIMP int rtd_cob_acuw_justify   (COB_RTD, unsigned char *data, ...);
COB_DLL_EXPIMP int rtd_cob_getpid         (COB_RTD, unsigned char *data);
COB_DLL_EXPIMP int rtd_cob_c_check_rw     (COB_RTD);
COB_DLL_EXPIMP int rtd_cob_debug_activate (COB_RTD, unsigned char *data);
COB_DLL_EXPIMP int rtd_CBL_FFND_REPORT    (COB_RTD, int rtserr, int terminate);
COB_DLL_EXPIMP int rtd_CBL_DEBUGBREAK     (COB_RTD);

COB_DLL_EXPIMP int CBL_ALLOC_MEM    (void  *pptr, int flag, int size);
COB_DLL_EXPIMP int CBL_FREE_MEM     (void  *ptr);
COB_DLL_EXPIMP int CBL_ERROR_PROC   (unsigned char *x, unsigned char *pptr);
COB_DLL_EXPIMP int CBL_EXIT_PROC    (unsigned char *x, unsigned char *pptr);
COB_DLL_EXPIMP int SYSTEM           (const unsigned char *cmd);
COB_DLL_EXPIMP int CBL_AND          (unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int CBL_OR           (unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int CBL_NOR          (unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int CBL_XOR          (unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int CBL_IMP          (unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int CBL_NIMP         (unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int CBL_EQ           (unsigned char *data_1, unsigned char *data_2, const int length);
COB_DLL_EXPIMP int CBL_NOT          (unsigned char *data_1, const int length);
COB_DLL_EXPIMP int CBL_XAF          (unsigned char *data_1, unsigned char *data_2);
COB_DLL_EXPIMP int CBL_XF4          (unsigned char *data_1, unsigned char *data_2);
COB_DLL_EXPIMP int CBL_XF5          (unsigned char *data_1, unsigned char *data_2);
COB_DLL_EXPIMP int CBL_X91          (unsigned char *result, const unsigned char *func, unsigned char *parm);
COB_DLL_EXPIMP int CBL_TOUPPER      (unsigned char *data, const int length);
COB_DLL_EXPIMP int CBL_TOLOWER      (unsigned char *data, const int length);
COB_DLL_EXPIMP int cob_caller_name  (unsigned char *data) ;
COB_DLL_EXPIMP int cob_program_id   (unsigned char *data) ;
COB_DLL_EXPIMP int cob_return_args  (unsigned char *data);
COB_DLL_EXPIMP int CBL_OC_NANOSLEEP (unsigned char *data);
COB_DLL_EXPIMP int cob_parameter_size (unsigned char *data);
COB_DLL_EXPIMP int cob_parameter_type (unsigned char *data);
COB_DLL_EXPIMP int cob_acuw_sleep   (unsigned char *data);
COB_DLL_EXPIMP int cob_acuw_justify (unsigned char *data, ...);
COB_DLL_EXPIMP int cob_getpid       (unsigned char *data);
COB_DLL_EXPIMP int cob_c_check_rw   (void);
COB_DLL_EXPIMP int cob_debug_activate (unsigned char *data);
COB_DLL_EXPIMP int CBL_FFND_REPORT  ( int rtserr, int terminate);
COB_DLL_EXPIMP int CBL_DEBUGBREAK   (void);

COB_DLL_EXPIMP int cob_debug_acl_alluser (COB_RTD, int flag) ;


/* Utilities */

#define COB_DUP_GET_SIGN(f) (COB_FIELD_HAVE_SIGN (f) ? cob_real_get_sign (rtd, f = cob_dup_field_if_needed(rtd, f)) : 0)
#define COB_GET_SIGN(f) (COB_FIELD_HAVE_SIGN (f) ? cob_real_get_sign (rtd, f) : 0)
#define COB_PUT_SIGN(f,s) if (COB_FIELD_HAVE_SIGN (f)) cob_real_put_sign (rtd, f, s)

COB_DLL_EXPIMP unsigned char *cob_external_addr_wstatus (COB_RTD, const char *exname, const int exlength, int *init_status);
COB_DLL_EXPIMP unsigned char *cob_external_addr (COB_RTD, const char *exname, const int exlength);
COB_DLL_EXPIMP unsigned char *cob_external_addr_from_fld (COB_RTD, cob_field *f, const int exlength);
COB_DLL_EXPIMP unsigned char *cob_get_pointer   (COB_RTD, const unsigned char *srcptr);
COB_DLL_EXPIMP void *cob_get_prog_pointer       (COB_RTD, const unsigned char *srcptr);
COB_DLL_EXPIMP void cob_set_location            (COB_RTD, const unsigned int line_debug_idx, const char * cur_source,
                                                 const unsigned int sline, const char *cstatement);
COB_DLL_EXPIMP void cob_ready_section_trace     (COB_RTD);
COB_DLL_EXPIMP void cob_reset_section_trace     (COB_RTD);
COB_DLL_EXPIMP int  cob_get_section_trace       (COB_RTD);
COB_DLL_EXPIMP void cob_ready_trace             (COB_RTD);
COB_DLL_EXPIMP void cob_reset_trace             (COB_RTD);
COB_DLL_EXPIMP char *cob_str_cur_timestamp (void);

COB_DLL_EXPIMP unsigned long cob_get_elapsed_tick (COB_RTD, cob_module *module);
/* Switch */

COB_DLL_EXPIMP int cob_get_switch (COB_RTD, const int n);
COB_DLL_EXPIMP void cob_set_switch(COB_RTD, const int n, const int flag);

/* Comparison */

COB_DLL_EXPIMP int cob_cmp (COB_RTD, cob_field *f1, cob_field *f2);


/* Class check */

COB_DLL_EXPIMP int cob_is_omitted   (COB_RTD, const cob_field *f);
COB_DLL_EXPIMP int cob_is_numeric   (COB_RTD, cob_field *f);
COB_DLL_EXPIMP int cob_is_alpha     (COB_RTD, const cob_field *f);
COB_DLL_EXPIMP int cob_is_upper     (COB_RTD, const cob_field *f);
COB_DLL_EXPIMP int cob_is_lower     (COB_RTD, const cob_field *f);

/* MSC compatibility*/
COB_DLL_EXPIMP int cob_is_success (COB_RTD, const cob_field *f);
COB_DLL_EXPIMP int cob_is_failure (COB_RTD, const cob_field *f);


/* Table sort */

COB_DLL_EXPIMP void cob_table_sort_init (COB_RTD, const int nkeys, const unsigned char *collating_sequence);
COB_DLL_EXPIMP void cob_table_sort_init_key (COB_RTD, const int flag, cob_field *field, size_t offset);
COB_DLL_EXPIMP void cob_table_sort (COB_RTD, cob_field *f, const int n);

/* Run-time error checking */

COB_DLL_EXPIMP void cob_check_numeric   (COB_RTD, cob_field *f, const char *name);
COB_DLL_EXPIMP void cob_check_based     (COB_RTD, const unsigned char *x, const char *name);
COB_DLL_EXPIMP void cob_check_odo       (COB_RTD, const int i, const int minv, const int maxv, const char *name);
COB_DLL_EXPIMP int  cob_check_odo_1     (COB_RTD, const int currentv, const int odoval, const int minodo, const int maxodo, const char *odoname, const char *valname);
COB_DLL_EXPIMP void cob_check_subscript (COB_RTD, const int i, const int minv, const int maxv, const char *name);
COB_DLL_EXPIMP int  cob_check_subscript_1 (COB_RTD, const int i, const int minv, const int maxv, const char *name);
COB_DLL_EXPIMP void cob_check_ref_mod   (COB_RTD, const int offset, const int length, const int size, const char *name);

/* Comparison functions */
COB_DLL_EXPIMP int cob_numeric_cmp      (COB_RTD, cob_field *f1, cob_field *f2);

COB_DLL_EXPIMP void cob_field_to_string (COB_RTD, const cob_field *f, char *s);
COB_DLL_EXPIMP void cob_string_to_C(COB_RTD, char *s, int size);
COB_DLL_EXPIMP void cob_field_to_string_1 (COB_RTD, const cob_field *f, char *s, int s_size);

COB_DLL_EXPIMP cob_field * cob_dup_field_if_needed(COB_RTD, cob_field *f);
COB_DLL_EXPIMP int cob_test_sign(COB_RTD, cob_field *f);
COB_DLL_EXPIMP int cob_get_sign_all (COB_RTD, unsigned char *p);

COB_DLL_EXPIMP void cob_qsort   (void *a, size_t n, size_t es, int (*cmp) (COB_RTD, const void *, const void *), void * contex);

/* context handeler*/
COB_DLL_EXPIMP void cob_set_context_server_prefix       (COB_RTD, char *fname);
COB_DLL_EXPIMP void cob_set_context_appli_prefix        (COB_RTD, char *fname);
COB_DLL_EXPIMP void cob_set_context_appli_prefix_field  (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_open_context_file               (COB_RTD, char * key);
COB_DLL_EXPIMP void cob_close_context_file              (COB_RTD);
COB_DLL_EXPIMP void cob_init_context_save               (COB_RTD, cob_field *exit_flag);
COB_DLL_EXPIMP void cob_read_context_data               (COB_RTD, void *p, int size);
COB_DLL_EXPIMP void cob_write_context_data              (COB_RTD, void *p, int size);
COB_DLL_EXPIMP void cob_set_context_mode                (COB_RTD, int imode);
COB_DLL_EXPIMP void cob_set_auto_erase_context_file     (COB_RTD, int on);
COB_DLL_EXPIMP void cob_free_module_allocated           (COB_RTD, cob_module *module);

COB_DLL_EXPIMP int  cob_validate_range(int _min,int _max, int _v);
COB_DLL_EXPIMP void cob_setting_action(COB_RTD, int action_code);
COB_DLL_EXPIMP int filename_replace_env(char * src, char * dst, int max_sz);
COB_DLL_EXPIMP unsigned char *    cob_utf16_char      (COB_RTD, unsigned char c);
COB_DLL_EXPIMP const unsigned char *    cob_utf16_space      (COB_RTD);
COB_DLL_EXPIMP void cob_fill_national(COB_RTD, cob_field *f, char c);
COB_DLL_EXPIMP char * cob_bool_getenv(const char *name);

#endif /* COB_COMMON_H */

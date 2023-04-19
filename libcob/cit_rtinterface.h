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
#ifndef _CIT_RTINTERFACE_H_
#define _CIT_RTINTERFACE_H_

#include <gmp.h>
#include <setjmp.h>

#define COB_MAX_RTD_REGION 100

/* field attributes */
#define COB_MINI_BUFF       256
#define COB_SMALL_BUFF      1024
#define COB_NORMAL_BUFF     2048
#define COB_MEDIUM_BUFF     8192
#define COB_LARGE_BUFF      16384

/*
 * WARNING cob_field_list        and cob_field MUST MATCH 
 * WARNING cob_field_attr_flist  and cob_field_attr MUST MATCH 
 */
#define COB_MAX_KEY_PART 64
#define COB_MAX_VBISAM_KEY_PART 8
typedef struct  cob_keypart {
    short       start;   /* Starting byte of key part */
    short       leng;    /* Length in bytes */
    struct cob_field   *field;
}cob_keypart ;

typedef struct {
    unsigned char   type;
    unsigned char   fcount;
    signed   char   dummy1;
    unsigned char   dummy2;
    cob_keypart     *part;
} cob_field_attr_flist;

typedef struct cob_field_list {
    size_t                  size;
    unsigned char           *data;
    cob_field_attr_flist    *attr;
} cob_field_list;

typedef struct {
    unsigned char   type;
    unsigned char   digits;
    signed   char   scale;
    unsigned char   flags;
    const unsigned char *pic;
} cob_field_attr;

/* field structure */
typedef struct cob_bitsarray_field {
    size_t                  size;
    unsigned char           *data;
    const cob_field_attr    *attr;
    size_t                  index;
} cob_bitsarray_field;

typedef struct cob_field {
    size_t                  size;
    unsigned char           *data;
    const cob_field_attr    *attr;
} cob_field;

typedef union cob_ufield {
    cob_field               fld;
    cob_bitsarray_field     ba_fld;
} cob_ufield;


/*
 * Global variables (private by thread)
 */
struct cob_exception {
    const char  *name;
    const int   code;
    const int   critical;
};

struct cob_alloc_cache {
    struct cob_alloc_cache  *next;
    void                    *cob_pointer;
    size_t                  size;
};

/* Runtime exit handling */
struct exit_handlerlist {
    struct exit_handlerlist *next;
    int         (*proc)(void);
};

/* Runtime error handling */
struct handlerlist {
    struct handlerlist  *next;
    int         (*proc_cob)(char *s);
} ;

/* data pointer list*/
struct data_list {
    struct data_list        *next;
    void                    *data;
    size_t                  len;                         
};
/* common.c stored modules data*/
struct cob_module_data {
    struct cob_module_data  *next;
    char                    *name;
    int                      size;
    void                    *data;
};

struct call_hash {
    struct call_hash    *next;
    char                *name;
    void                *func;
    char                flag_is_valid;
};

struct module_hash {
    struct module_hash  *next;
    char                *name;
    void                *func;
    void                *cancel;
    void                *handle;
    char                flag_is_valid;
    char                is_unloadable;
    char                region_use[COB_MAX_RTD_REGION];
};

struct dlm_struct {
    cob_field   uns_dlm;
    int         uns_all;
};

/* equivalent to cob_decimal without the const */
typedef struct {
    mpz_t   v_mpz;
    int                         scale;
} cob_decimal_rw;

typedef struct {
    mpz_t   v_mpz;
    int     scale;
} cob_decimal;

typedef struct {
    mpf_t   v_mpf;
} cob_float;


typedef        void          (*cob_rtd_exit_proc) (void *, int );
typedef        int           (*cob_errmsg_proc)   (void *, char *);

#define COB_DEBUG_BR_LINENR_MODULE_ENTRY -1
#define COB_DEBUG_REGULAR_BREAKPOINT 1
#define COB_DEBUG_TEMPORARY_BREAKPOINT 2
#define COB_DEBUG_VALUE_CHANGE_BREAKPOINT 3

typedef struct cob_condition_debug_info {
    int                 valid_condition;
    int                 no_break_count; 
    char               *varname;
    unsigned char      *refvalue;
    unsigned char      *curvalue;
    int                size;
} cob_condition_debug_info;

typedef struct break_point {
    int                 br_nr;
    int                 linenr;
    int                 br_type;
    char                *source;
    char                *function;
    struct cob_condition_debug_info condition;
    struct break_point  *next;
} break_point_t;

typedef struct {
    int                 flag_trace_mi;
    int                 flag_stop_next_statement;
    int                 flag_go_next_statement;
    int                 flag_new_windows;
    int                 flag_mode_mi;
    int                 flag_exit_debug;
    int                 flag_continue_running;
    int                 current_stack_frame;
    int                 breap_point_id;
    int                 perform_count;
    const char          *prompt;
    struct break_point  *break_point;
    int                 external_pid;
} NU_debug_context; /*keep for rtd size compatibility*/

typedef struct cmd_buffer {
    char * buffer;
    int    size;
} cmd_buffer_t;

typedef struct replace_buffer { 
    char * oldprefix;
    char * newprefix;
    struct replace_buffer * next;
} replace_buffer_t;

#define COB_MAX_VALUE_CONDITION 5
typedef struct {
    int                 flag_debugger_active;
    int                 flag_trace_mi;
    int                 flag_stop_next_statement;
    int                 flag_go_next_statement;
    int                 flag_new_windows;
    int                 flag_exit_debug;
    int                 flag_continue_running;
    int                 flag_need_reload_breakpoint;
    int                 flag_debugger_closing;
    int                 flag_machine_command;
    int                 flag_force_readline_command;
    int                 flag_disable_source;
    int                 current_stack_frame;
    int                 breap_point_id;
    int                 perform_count;
    const char          *prompt;
    struct break_point  *break_point;
    int                 external_did;
    int                 external_pid;
    char                *remote_gui;
    FILE                *cob_db_trace ;
    FILE                *cob_db_out   ;
    FILE                *cob_db_event_out ;
    FILE                *cob_db_in    ;
    int                 cob_debug_sig;
    char                namein[COB_SMALL_BUFF];
    char                nameout[COB_SMALL_BUFF];
    char                nameenv[COB_SMALL_BUFF];
    char                nameinfo[COB_SMALL_BUFF];
    int                 startingReadingThread;
    void                *debugger_thread;
    char                modulename[COB_SMALL_BUFF];
    void                *testmemory_thread;
    int                 testmemory_fd[2];
    const char         *source_fname;
    FILE               *source;
    int                 source_linenr;
    int                 external_fout;
    int                 cob_debug_thread_started;
    int                 cob_debug_inside_init;
    cmd_buffer_t        command_buff;
    int                 external_thread_no;
    volatile int        current_region;
    volatile int        setting_current_region;
    struct cob_condition_debug_info *value_condition[COB_MAX_VALUE_CONDITION];
    replace_buffer_t   *replace_list; 
    char               *tracefilename;
    int                max_debug_data_size;
} debug_context;

typedef struct {
    const char   *cp;
    int          startCode;
    int          endCode;
} RTL_info_type;

typedef struct {
    char *name;
    int bitfield_index;
    void *fdi;
} debug_fld_reques_type;

typedef unsigned char CharMapType[256];

typedef struct {	
    unsigned short  dsc_w_length;
	unsigned char   dsc_b_dtype;
	unsigned char	dsc_b_class;
	void           *dsc_a_pointer;    
} call_by_descriptor_struct;

/* CALL Flags */
#define COB_FULL_CANCEL                 0x0001
#define COB_LOAD_PRIORITY               0x0002
#define COB_FULL_CANCEL_ALL             0x0004
#define COB_FLAG_DOT_CIT_FIRST          0x0008

/*FILEIO Flags f->fileio_runtime_flags  && rtd->fileio_runtime_flags */
#define FILEIO_NO_CRASH_CLOSE       0x01
#define FILEIO_TRANSACTION          0x02
//#define FILEIO_VAR_REC_PAD          0x04
//#define FILEIO_INDEX_DUP_NO_BUG     0x08
#define FILEIO_INDEX_COMPAT         0x10
//#define FILEIO_MAINFRAME_VAR_REC    0x20
//#define FILEIO_REL_SIZE_BE          0x40
//#define FILEIO_FILE_OPTIONAL_MF     0x80
/*
* context informations  
*/
#define COB_CONTEXT_NONE     0
#define COB_CONTEXT_RELOAD   0x01      /* Reload context */
#define COB_CONTEXT_SAVE     0x02      /* Save   context */
#define COB_CONTEXT_CONTINUE 0x10      /* continue execution */
typedef struct {
    int           cob_context_mode;
    int           cob_next_context_father;
    char          *cob_context_server_prefix;
    char          *cob_context_appli_prefix;
    FILE          *cob_context_file;
    int           cob_auto_erase_context_file;
} cob_context_info_t;

#define SWITCH_COUNT 36
typedef struct  cit_runtime_s {
    /* DO NOT modify field order, DONOT change Field Size, DO NOT Erase Field*/

    /*Common public value*/
    int                     cob_disable_signal_handler;
    int                     cob_initialized;
    int                     cob_exception_code;

    struct __cob_module    *current_module;

    int                     cob_call_params;
    int                     cob_save_call_params;
    int                     cob_initial_external;
    FILE                    *cob_err_file;  /* stderr or user define*/
    char                    *cob_tmpdir;    /* /tmp */
    cob_context_info_t      cob_context_info;


    /* Private value used in common.c*/
    int                     cob_rtd_allocated;
    jmp_buf                 cob_longjmp;
    int                     cob_longjmp_defined;
    cob_rtd_exit_proc       cob_exit_proc;
    cob_errmsg_proc         cob_errormessage_proc;
    int                     cob_last_status;
    int                     cob_argc;
    char                    **cob_argv;
    struct cob_alloc_cache  *cob_alloc_base;    
    char                    *cob_local_env;
    int                     current_arg;
    unsigned char           *commlnptr;
    size_t                  commlncnt;    
    char                    *locale_save;    
    size_t                  sort_nkeys;
    struct cob_file_key     *sort_keys;
    const unsigned char     *sort_collate;    
    const char              *cob_source_statement;
    size_t                  cob_line_trace;
    int                     cob_got_exception;                       
    const char              *cob_orig_statement;
    const char              *cob_orig_program_id;
    const char              *cob_orig_section;
    const char              *cob_orig_paragraph;
    unsigned int            cob_orig_line;
#define DUPLICAT_COUNT  16
    cob_field               duplicat_field[DUPLICAT_COUNT];
    size_t                  duplicat_size[DUPLICAT_COUNT];
    int                     duplicat_current;

    int                     unused_cob_switch[8];
    struct exit_handlerlist *exit_hdlrs;
    struct handlerlist      *hdlrs;
    char                    strerrbuffer[1024];
    char                    *runtime_err_str;
    char                    *cmdlcopy;
    char                    *locargv[64];
    struct cob_external     *cob_baseexternal;
    struct cob_module_data  *modules_data;

    /*private data from call.c*/
    struct data_list        *pre_handle;
    char                    **resolve_path;
    char                    *resolve_error;
    char                    *resolve_error_buff;
    void                    *mainhandle;
    size_t                  name_convert;
    size_t                  resolve_size;
    size_t                  cobjmp_primed;
    char                    call_namebuff[COB_MEDIUM_BUFF];
    struct call_hash        **call_table;
    size_t                  call_buffer_lastsize;
    void                    *call_buffer;
    #define                 COB_RTD_VERSION_PRESENT 0x0711FEFE
    int                     usused_call_flag; /* USED TO INDICATE  RTD Version  ins present !!! */
                                              /* the field is not renamed to enable source compatibility*/

    /*private data from fileio.c*/
    void                    *cob_error_file; /*cobfile * */
    size_t                  eop_status;
    int                     unused_cob_do_sync;
    int                     cob_sort_memory;
    struct data_list        *file_cache;
    char                    *unused_cob_file_path;
    char                    *old_cob_ls_nulls;
    char                    *old_cob_ls_fixed;
    char                    *unused_file_open_env;
    char                    *file_open_name;
    char                    *file_open_buff;
    char                    runtime_buffer[COB_SMALL_BUFF];
    int                     trace_extfh;


    /*private data from intrinsic.c*/
    char                    *instrinsic_buff;
    cob_decimal             instrinsic_d1, instrinsic_d2, instrinsic_d3, instrinsic_d4, instrinsic_d5;
    /* Stack definitions for created fields */
    /* Stacked field level */
#define OLD_DEPTH_LEVEL	8    
    int                     curr_entry;
    cob_field               *curr_field;
    cob_field_attr          *curr_attr;
    cob_field               old_calc_field[OLD_DEPTH_LEVEL];
    cob_field_attr          old_calc_attr[OLD_DEPTH_LEVEL];
    size_t                  old_calc_size[OLD_DEPTH_LEVEL];
    void                    *national_of_current_iconv_cd;
    char                    *national_of_current_cp;
    void                    *display_of_current_iconv_cd;
    char                    *display_of_current_cp;

    /*private data from move.c*/
    size_t                  move_lastsize;
    unsigned char           *move_lastdata;

    /*private data from numeric.c*/
#define COB_MAX_NUMBER_DIGIT  64
#define COB_MAX_NUMBER_BIT    64
    cob_decimal             cob_d1;
    cob_decimal             cob_d2;
    cob_decimal             cob_d3;
    cob_decimal             cob_d4;
    mpz_t                   cob_mexp;
    mpz_t                   cob_mpzt;
    mpz_t                   unusedcob_mpze10[36];
    unsigned char           packed_value[20];

    /*private data from screen.c*/
    int                     cob_screen_mode;
    int                     cob_screen_initialized;

    void                    *cob_base_inp;
    size_t                  curr_index;
    size_t                  totl_index;
    size_t                  cob_has_color;
    size_t                  cob_extended_status;
    size_t                  cob_use_esc;
    int                     global_return;
    int                     cob_current_y;
    int                     cob_current_x;
    int                     cob_max_y;
    int                     cob_max_x;
    short                   fore_color;
    short                   back_color;

    /*private data form strings.c*/

    unsigned char           *figptr;
    size_t                  figsize;
    cob_field               *inspect_var;
    unsigned char           *inspect_data;
    unsigned char           *inspect_start;
    unsigned char           *inspect_end;
    int                     *inspect_mark;
    size_t                  inspect_lastsize;
    size_t                  inspect_size;
    int                     inspect_replacing;
    int                     inspect_sign;
    cob_field               inspect_var_copy;

    cob_field               *string_dst;
    cob_field               *string_ptr;
    cob_field               *string_dlm;
    cob_field               string_dst_copy;
    cob_field               string_ptr_copy;
    cob_field               string_dlm_copy;
    int                     string_offset;

    struct dlm_struct       *dlm_list;
    cob_field               *unstring_src;
    cob_field               *unstring_ptr;
    cob_field               unstring_src_copy;
    cob_field               unstring_ptr_copy;
    int                     unstring_offset;
    int                     unstring_count;
    int                     unstring_ndlms;

    cob_field_attr          alpha_attr;
    cob_field               alpha_fld;

    /*private data from termio.c*/
    unsigned char           *term_buff;

    /*private data debug.c not used any more*/
    NU_debug_context        NU_cob_debug_context;
    FILE                    *NU_cob_db_trace ;
    FILE                    *NU_cob_db_out   ;
    FILE                    *NU_cob_db_in    ;    
    int                     NU_debug_sig;
    char                    *auto_break_modules;
    int                     NU_debug_inside_init;
    /* new switch*/
    int                     cob_switch[SWITCH_COUNT];
    int                     call_flag;
    int                     debug_byte_in_buffer;
    int                     debug_prompted;
    int                     fileio_rollback_log_usecnt;
    int                     fileio_runtime_flags;
    int                     udlmcount ;
    int                     cmp_packed_lastval;
    int                     term_buff_size;
    int                     display_buff_size;
    int                     display_buff_offset;
    int                     last_crtstatus;
    int                     ctree_init;
    char                    *debug_strtok_save;
    char                    *debug_input_buffer;
    char                    *fileio_rollback_log_filename;
    void                    *extfh_rtd;
    RTL_info_type           *RTL_info;
    void                    *win_console;
    unsigned char           *display_buff;
    cob_field               *return_field;
    char                    disable_fstatus_map;
    char                    profiler_elapsed_time;
    char                    sysin_redirected;
    char                    sysout_redirected;
    char                    syserr_redirected;
    void                    *sysin_file;
    void                    *sysout_file;
    void                    *syserr_file;
    char                    *default_extfh_entry;
    char                    *default_extfh_indexed_entry;
    char                    *default_extfh_flat_entry;
    char                    cursorpos_shadow[6];
    const char              *current_utf16_cp_id;
    int                     screen_insert_mode;
    char                    *ls_enable_double_lf_unused;
    char                    cob_screen_key_raw;
    char                    cob_screen_update_first_key_erase;
    char                    cob_screen_disable_reformat;
    cob_field               screen_edit_field; 
    cob_field_attr          screen_edit_attr;
    /*CIT001*/

    /* Up to here field may not be moved or removed.*/
    /* New fixed field must be added after CIT001*/

    /* Begin of Movable AREA */
    /* Field order and position after this point is point is not guaranty*/
    /* Only runtime may access them */

    struct data_list      * cbl_allocated_list; 
    /* debug data */
    unsigned char           *field_textual_rep;
    int                     field_textual_rep_capacity;
    int                     field_textual_rep_size;
    unsigned char           *dummy_field_hex_rep;
    int                     dummy_field_hex_rep_capacity;
    int                     dummy_field_hex_rep_size;
    char                    last_resolved_name_n[COB_SMALL_BUFF];
    mpz_t                   cob_mpze10[COB_MAX_NUMBER_DIGIT];
    unsigned int            current_rtd_region;
    struct  cit_runtime_s   *rtd_region_data[COB_MAX_RTD_REGION];
    struct  cit_runtime_s   *parent_rtd;
    char                    inspect_national;
    char                    string_national;
    char                    unstring_national;
    char                    warning_disable;
#define DEPTH_LEVEL 64
    cob_field               calc_field[DEPTH_LEVEL];
    cob_field_attr          calc_attr[DEPTH_LEVEL];
    size_t                  calc_size[DEPTH_LEVEL];
    size_t                  debug_input_buffer_size;
    struct module_hash      **module_table;
    int                     channel_table[16];
    char                    *debug_strtok_save2;
    char                    call_match_exact_case;
    char                    call_match_upper_case;
    char                    call_match_lower_case;
    char                    load_match_exact_case;
    char                    load_match_upper_case;
    char                    load_match_lower_case;
    int                     mypid;
    int                     tmp_uid;
    char                    cob_screen_disable_rightjustify;
    char                    cob_screen_eol;
    char                    accept_to_uppercase;
    char                    *display_code_page;
    unsigned int            accept_key_as_fkey;
    cob_field               *accept_timeout;
    unsigned char           utf16_cmp[4];
    char                    last_call_resolved_name[COB_SMALL_BUFF];
    void *                  last_call_resolved_func;
    void *                  debug_field_cache;
    int                     debug_field_cache_idx;
#define EBCDICtoASCII_BUFFERS 10
    int                     ebcdic2ascii_idx;
    unsigned char           *ebcdic2ascii_buff[EBCDICtoASCII_BUFFERS];
    unsigned int            ebcdic2ascii_size[EBCDICtoASCII_BUFFERS];
    int                     console_ebcdic;
    char                   *debug_codepage;
    void                   *debug_codepage_iconv_cd;
    int                     ebcdic_charset;
    int                     ebcdic_charset_init;
    CharMapType             charset_map;
    char                   *console_codepage;
    void                   *console_codepage_iconv_cd;
    char                   *console_buffer;
    size_t                  console_buffer_size;
    void                   *mf_cobjmp_buf;
    unsigned int            rtd_region_nr;
    unsigned int            debug_age;
    int                     cob_sigvec_signal_handler_set;
    int                     cob_argv_allocated;
    int                     debug_acl_alluser;
    debug_context           cob_debug_context;
    char                    *runtime_version_string;
    #define  COB_RTD_VERSION_CNT  1
    int                     runtime_version_cnt;
    char                    *debug_current_module_name;
    void                    *sysprint_file;
    void                    *utf8_codepage_iconv_cd;
    char                    disable_profiling_files;
    char                    runtime_debug_started;
    int                     string_overflow;
    char                    *xml_code_page;
    char                    call_match_exact_symbol;
    char                    cob_file_case;
    char                    cob_filemap_case;
    int                     debugger_clear_field_cache;
    int                     cob_section_trace;
    int                     cob_disable_trace;
} cit_runtime_t;

#define COB_NEXT_MODULE(m) (m==m->next ? NULL: m->next)
#define COB_RTD cit_runtime_t * const rtd 
#define cob_setjmp(rtdata)           (rtdata->cob_longjmp_defined = 1, setjmp(rtdata->cob_longjmp))

typedef enum runtime_error_code {
    COBRE_CALL_NOT_FOUND    =128,       /* CALL Unresolved                                              */
    COBRE_CANCEL_ERROR      =129,       /* "NULL name parameter passed to 'cobcancel'"                  */
    COBRE_COBCALL_RTD       =130,       /* "'cobcall' - Runtime has not been initialized"               */
    COBRE_COBCALL_PR_CNT    =131,       /* "Invalid number of arguments to 'cobcall'"                   */
    COBRE_COBCALL_NULL      =132,       /* "NULL name parameter passed to 'cobcall'"                    */
    COBRE_COBFUNC_RTD       =133,       /* "'cobfunc' - Runtime has not been initialized"               */
    COBRE_COBSAVENV_NULL    =134,       /* "NULL name parameter passed to 'cobsavenv'"                  */
    COBRE_COBLONGJMP_NULL   =135,       /* "NULL name parameter passed to 'coblongjmp'"                 */
    COBRE_OUT_OF_MEMORY     =136,       /* "Cannot acquire %d bytes of memory - Aborting"               */
    COBRE_BASED_NULL        =137,       /* "BASED/LINKAGE item '%s' has NULL address"                   */
    COBRE_NOT_NUMERIC       =138,       /* "'%s' not numeric: '%s'"                                     */
    COBRE_OCCUR_DEP         =139,       /* "OCCURS DEPENDING ON '%s' out of bounds: %d"                 */
    COBRE_SUBSCRIPT         =140,       /* "Subscript of '%s' out of bounds: %d"                        */
    COBRE_OFFSET            =141,       /* "Offset of '%s' out of bounds: %d"                           */
    COBRE_LENGTH            =142,       /* "Length of '%s' out of bounds: %d"                           */
    COBRE_EXTERNAL          =143,       /* "EXTERNAL item '%s' has size > %d"                           */
    COBRE_CMD_LINE          =144,       /* "'cobcommandline' - Runtime has not been initialized"        */
    COBRE_SYSTEM            =145,       /* "Parameter to SYSTEM call is larger than 8192 characters"    */
    COBRE_CTX_RD_INV        =146,       /* "Invalid context file %s for reading"                        */
    COBRE_CTX_RD_OPEN       =147,       /* "Can't open context file %s for reading"                     */
    COBRE_CTX_WRITE         =148,       /* "Can't write context file %s key "                           */
    COBRE_CTX_WR_OPEN       =149,       /* "Can't open context file %s for writing"                     */
    COBRE_CTX_MODE          =150,       /* "Unexpected context mode %d"                                 */
    COBRE_CTX_NOT_OPEN      =151,       /* "Context file not open for read/write"                       */
    COBRE_CTX_READ          =152,       /* "Can't read context file %s data"                            */
    COBRE_TMP_FILE          =153,       /* "Runtime is unable to acquire temporary file"                */
    COBRE_EXTFH_OP_MODE     =154,       /* "extfh_indexed_open : invalid open mode %d for %s            */
    COBRE_EXTFH_START       =155,       /* "extfh_indexed_start : invalid condition                     */ 
    COBRE_EXTFH_READ        =156,       /* "extfh_read : invalid read option %d"                        */
    COBRE_EXTSM_USING_GIVING=157,       /* "extsm/SORT : maximum USING/GIVING clauses exceded"          */
    COBRE_EXTFH_PTR         =158,       /* "extfh/extsm Internal error                                  */
    COBRE_CURSES_FAIL       =159,       /* "Failed to initialize curses"                                */
    COBRE_COB_INIT          =160,       /* "cob_init() has not been called"                             */
    COBRE_CODE_GEN          =161,       /* "Codegen error - Please report this to support@cobol-it.com" */
    COBRE_CHAINING          =162,       /* ERROR - Recursive call of chained program"                   */
    COBRE_STACK             =163,       /* "Stack overflow, possible PERFORM depth exceeded"            */
    COBRE_PARAM_CHECK       =164,       /* CBL_xxx CALL Insufisent parameters count                     */
    COBRE_BDB_VERSION       =165,       /* Incorrect BDB Version                                        */
    COBRE_USER_REQUEST      =166,       /* USER RESUEST ABEND                                           */


    COBRE_ENTERPRISE        =253,       /* Function only available in enterprise version                */
    COBRE_FATAL_ERROR       =254,       /* see messages                                                 */
    COBRE_UNDEFINED         =255
} runtime_error_code_t;

typedef union cob_reg_union
{
    signed long long   si;
    unsigned long long ui;
    float              sf;
    double             sd;
} cob_reg_type;

#endif /*_CIT_RTINTERFACE_H_*/

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
#ifndef CIT_DEBUG_H
#define CIT_DEBUG_H
#define DEBUG_INFO_SUFFIX "fdi"
#include <stdlib.h>
#include <libcob/common.h>

COB_DLL_EXPIMP cob_ufield * cob_debug_find_cob_field     (COB_RTD, 
                                                          cob_module *mod, char * field_name, 
                                                          cob_ufield * field_buffer, char * full_field_name );

/* execute a cmd return 1 if exit request*/
COB_DLL_EXPIMP int cob_runtime_debugger_exec_command    (COB_RTD, char *cmd) ;
COB_DLL_EXPIMP int cob_runtime_debugger_readprompt      (COB_RTD, cmd_buffer_t *cmd_buffer, int blocking);
COB_DLL_EXPIMP int cob_runtime_debugger_init            (void);
COB_DLL_EXPIMP int cob_runtime_debugger_init_rtd        (COB_RTD);
COB_DLL_EXPIMP void cob_runtime_debugger_set_module_name (char *name);
COB_DLL_EXPIMP int cob_runtime_remote_debugger_init     (void);
COB_DLL_EXPIMP int cob_runtime_debugger_activate        (COB_RTD, int did);
COB_DLL_EXPIMP int cob_runtime_set_debug_region_separated (COB_RTD, int activate);
COB_DLL_EXPIMP char *cob_runtime_debugger_info_name     (debug_context * dbc, char *buffer);
COB_DLL_EXPIMP char *cob_runtime_debugger_pipe_name     (debug_context * dbc, int bin, char *buffer);
COB_DLL_EXPIMP char *cob_runtime_debugger_info_base     (COB_RTD, char *buffer);
COB_DLL_EXPIMP int cob_runtime_debugger_cleanup         (COB_RTD);
COB_DLL_EXPIMP int rtd_cob_runtime_debugger_activate    (void);
COB_DLL_EXPIMP void cob_runtime_debugger_read_info      (COB_RTD, debug_context * const dbc);
COB_DLL_EXPIMP void cob_runtime_debugger_write_info     (COB_RTD, debug_context * const dbc);
COB_DLL_EXPIMP void cob_debug_identify_socket           (COB_RTD);
COB_DLL_EXPIMP void debug_clear_field_cache             (COB_RTD);
COB_DLL_EXPIMP void cob_runtime_debugger_output_event   (COB_RTD,const char * reason) ;
COB_DLL_EXPIMP int cob_runtime_debugger_open_debugdb    (COB_RTD, cob_module *mod);
COB_DLL_EXPIMP cob_field_debug_info* cob_runtime_debugger_open_field_debugdb (COB_RTD, cob_module *mod, cob_field_debug_info* fdi);
COB_DLL_EXPIMP cob_lines_debug_info* cob_runtime_debugger_open_lines_debugdb (COB_RTD, cob_module *mod);
COB_DLL_EXPIMP void cob_runtime_debugger_switch_region(COB_RTD) ;
COB_DLL_EXPIMP void cob_runtime_debugger_clear_field_cache (COB_RTD);

COB_DLL_EXPIMP void cob_runtime_debugger_close_module(COB_RTD, cob_module *mod);

COB_DLL_EXPIMP void cob_profiling_dump                  (COB_RTD, cob_module *module);
COB_DLL_EXPIMP void cob_profiling_dump_one              (COB_RTD, FILE *f, struct cob_paragraph_profiling_info *p) ;
COB_DLL_EXPIMP void cob_profiling_register              (COB_RTD, struct cob_paragraph_profiling_info *p);
COB_DLL_EXPIMP void cob_profiling_unregister            (COB_RTD, struct cob_paragraph_profiling_info *p);
COB_DLL_EXPIMP void cob_profiling_unregister_all        (COB_RTD) ;
COB_DLL_EXPIMP int cob_profiling_dump_all               (COB_RTD, FILE *f) ;
COB_DLL_EXPIMP int cob_profiling_dump_all_to_file       (COB_RTD, char *tag) ;
COB_DLL_EXPIMP char*cob_runtime_debugger_info_name_pid(debug_context *dbc, char *buffer);
COB_DLL_EXPIMP char*cob_runtime_debugger_info_name_did(debug_context *dbc, char *buffer);



COB_DLL_EXPIMP debug_context * cob_get_debug_context    (COB_RTD);
COB_DLL_EXPIMP void cob_lock_mutex(void);
COB_DLL_EXPIMP void cob_unlock_mutex(void);
COB_DLL_EXPIMP int cob_runtime_debugger_did_parse       (debug_context * dbc, char *s);
COB_DLL_EXPIMP char* cob_basename (const char *name) ;

COB_DLL_EXPIMP void* cob_runtime_debugger_debugdb;
COB_DLL_EXPIMP cob_field * cob_debug_true_false (COB_RTD, int val);
COB_DLL_EXPIMP cob_field * cob_debug_true_false (COB_RTD, int val);
/*#define RCPProto "ncacn_np"*/
#endif /*CIT_DEBUG_H*/

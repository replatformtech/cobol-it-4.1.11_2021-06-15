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
#ifndef CIT_ENTERPRISE_RUNTIME_H
#define CIT_ENTERPRISE_RUNTIME_H 1


#define ENTERPRISE_CC_INIT  1
#define ENTERPRISE_CC_STEP  2
#define ENTERPRISE_CC_CLOSE 3

/* CIT enterprise stuff*/
extern void cob_enterprise_runtime_init(void);
extern void enterprise_optimizer_display_info (void);
extern void enterprise_optimizer_reset_info (int flags);
extern cb_tree enterprise_optimizer_mark_linear_statement (struct cb_program *current_program, cb_tree l) ;
extern cb_tree enterprise_pipe_optimizer  (struct cb_program *current_program, cb_tree l) ;
extern int  enterprise_optimizer_enable_location (cb_tree x);
extern void enterprise_codepage_init (char *runtime_codepage, char *source_codepage);
extern int enterprise_sourcecp_to_runtimecp(char * s, char * d, size_t ssz, size_t dsz) ;
extern int  enterprise_extfct (unsigned short code, void*p1, void *p2, void*p3, void *p4, void *p5, void *p6);
extern int  cob_enterprise_load (int codegen);
extern void output_ccmap_init (struct cb_program *prog);
extern void output_ccmap_finish (struct cb_program *prog);
extern void cob_enterprise_output_xdd_file (FILE *o, struct cb_file      *f);
extern int  cob_enterprise_gen_list_info (FILE * outfile, struct cb_program *prog);
extern int  cob_enterprise_gen_fieldname_list (char *filename, struct cb_program *prog) ;

extern int   cob_enterprise_gen_linkage_descriptor (char * filename, struct cb_program *prog);
extern char* cob_enterprise_get_ccmap_name (char * module);
extern void  output_ccmap (const unsigned char *source, int line , char type);
extern int   cob_enterprise_gen_xdd_descriptor (struct cb_program *prog);
extern void  cob_enterprise_map_field_from_type (struct cb_field *f, struct cb_field *type );
extern void  cob_enterprise_output_profiling_tick (void);
extern void  cob_enterprise_output_profiling (const char *fmt, ...);
extern void  cob_enterprise_output_call_header(void);
extern void  cob_enterprise_output_key_field_list_allocation (struct cb_program *prog, struct cb_file *f, cb_tree x);
extern void  cob_enterprise_output_rtd_init_header(void) ;
extern void  cob_enterprise_output_rtd_exit_footer(void) ;
extern void  cob_enterprise_output_internal_function_init(struct cb_program * prog);
extern void  cob_enterprise_output_call_footer(void) ;
extern void  cob_enterprise_map_call_to_entry (cb_tree prog, int *call_conv, cb_tree using_list, cb_tree returning);

#endif

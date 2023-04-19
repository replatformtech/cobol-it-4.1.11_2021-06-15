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
#ifndef _CITENTERPRISE_H_
    #define _CITENTERPRISE_H_ 1


COB_DLL_EXPIMP void cob_dump_debug_info(COB_RTD);
COB_DLL_EXPIMP void cob_enterprise_move_bits_to_field(COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP unsigned long long cob_enterprise_bits_get_ull(COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_enterprise_move_display_to_bits(COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP int cob_enterprise_intr_national_of(COB_RTD, char * s, char * d, char *cp_buffer, size_t ssz, size_t dsz);
COB_DLL_EXPIMP int cob_enterprise_intr_display_of(COB_RTD, char * s, char * d, char *cp_buffer, size_t ssz, size_t dsz);
COB_DLL_EXPIMP int cob_enterprise_debugcp_to_display(COB_RTD, char * s, char * d, char *display_cp_buffer, void * debug_iconv_cp, size_t ssz, size_t dsz);
COB_DLL_EXPIMP int cob_enterprise_display_to_debugcp(COB_RTD, char * s, char * d, char *display_cp_buffer, void * debug_iconv_cp, size_t ssz, size_t dsz);
COB_DLL_EXPIMP int cob_enterprise_int_uppercase_lowercase(COB_RTD, unsigned char *d, unsigned char *s, size_t dsize, size_t ssize, int isUpperCase);

COB_DLL_EXPIMP void cob_enterprise_free_national(COB_RTD);
COB_DLL_EXPIMP void cob_enterprise_move_display_to_abit(COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP unsigned long long cob_enterprise_bits_get_ull(COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_enterprise_set_bits_ull(COB_RTD, cob_field *f, unsigned long long val);
COB_DLL_EXPIMP void cob_enterprise_move_abit_to_field(COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void cob_enterprise_free_rtd_allocated(COB_RTD);
COB_DLL_EXPIMP int  cob_enterprise_map_ctr_status(COB_RTD, int fret, unsigned int *stat);
COB_DLL_EXPIMP unsigned char *cob_enterprise_RTLGetReversedString(COB_RTD, unsigned char *p, int size);
COB_DLL_EXPIMP void * cob_enterprise_open_one_icu(COB_RTD, const char *name);
COB_DLL_EXPIMP void * cob_enterprise_close_one_icu (void * code_cp);
COB_DLL_EXPIMP int  cob_enterprise_open_icu(COB_RTD, const char *name);
COB_DLL_EXPIMP void cob_list_codepage (void);
COB_DLL_EXPIMP void cob_check_codepage (char *name);
COB_DLL_EXPIMP void cob_enterprise_clear_all_module_cache (COB_RTD);



COB_DLL_EXPIMP unsigned int          cob_enterprise_get_current_region (COB_RTD);
COB_DLL_EXPIMP cit_runtime_t *       cob_enterprise_set_current_region (COB_RTD, unsigned int region);
COB_DLL_EXPIMP void                  cob_enterprise_cancel_region(COB_RTD, unsigned int region, int full_cancel) ;
/*private*/
COB_DLL_EXPIMP cit_runtime_t *       cob_enterprise_get_region_0 (COB_RTD);
COB_DLL_EXPIMP cit_runtime_t *       cob_enterprise_get_region (COB_RTD, unsigned int region);
COB_DLL_EXPIMP void                  cob_enterprise_terminate_region (COB_RTD);
COB_DLL_EXPIMP void                  cob_enterprise_xml_parse_init (COB_RTD, cob_field *f,cob_field *xml_code, cob_field *xml_text, cob_field *xml_ntext, cob_field *xml_event);
COB_DLL_EXPIMP void                  cob_enterprise_xml_parse_init_1(COB_RTD, cob_field *f, cob_field *xml_code, cob_field *xml_text, 
                                                                     cob_field *xml_ntext, cob_field *xml_event, cob_field *xml_encoding);
COB_DLL_EXPIMP void                  cob_enterprise_xml_parse_close (COB_RTD);
COB_DLL_EXPIMP void                  cob_enterprise_xml_parse_next (COB_RTD, cob_field *xml_code,cob_field *xml_text, cob_field *xml_ntext, cob_field *xml_event);
COB_DLL_EXPIMP void                  cob_enterprise_xml_generate_name (COB_RTD, cob_field *dest,cob_field *name,cob_field *cnt, cob_field *xml_code, int closetag);
COB_DLL_EXPIMP void                  cob_enterprise_xml_generate_data (COB_RTD, cob_field *dest,cob_field *f,cob_field *cnt, cob_field *xml_code);
COB_DLL_EXPIMP void                  cob_enterprise_ccmap_dump (COB_RTD, void *module);



#endif

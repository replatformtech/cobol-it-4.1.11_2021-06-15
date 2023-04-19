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

#ifndef COB_MOVE_H
#define COB_MOVE_H

#include <libcob/common.h>

COB_DLL_EXPIMP const int    cob_exp10[10];
COB_DLL_EXPIMP const long long  cob_exp10LL[19];
COB_DLL_EXPIMP const double  cob_exp10_double[19];

COB_DLL_EXPIMP void     cob_move    (COB_RTD, cob_field *src, cob_field *dst);
COB_DLL_EXPIMP void     cob_move_field_list(COB_RTD, cob_field * src, cob_field * dst);
COB_DLL_EXPIMP void     cob_set_int (COB_RTD, cob_field *f, int n);
COB_DLL_EXPIMP void     cob_set_Cstring (COB_RTD, cob_field *f, char *s);
COB_DLL_EXPIMP void     cob_set_debugcp_string (COB_RTD, cob_field *f, char *s);
COB_DLL_EXPIMP void     cob_set_sll (COB_RTD, cob_field *f, signed long long n);
COB_DLL_EXPIMP void     cob_set_ull (COB_RTD, cob_field *f, unsigned long long n);
COB_DLL_EXPIMP void     cob_set_float (COB_RTD, cob_field *f, float n) ;
COB_DLL_EXPIMP void     cob_set_double (COB_RTD, cob_field *f, double n) ;
COB_DLL_EXPIMP void     cb_fix_display_value (COB_RTD, cob_field *src);
COB_DLL_EXPIMP double   cob_get_double (COB_RTD, cob_field *f) ;
COB_DLL_EXPIMP float    cob_get_float (COB_RTD, cob_field *f) ;
COB_DLL_EXPIMP int      cob_get_int (COB_RTD, cob_field *f);
COB_DLL_EXPIMP long long cob_get_long_long (COB_RTD, cob_field *f);
COB_DLL_EXPIMP unsigned long long cob_get_ull (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void     cob_fieldlist_from_field (COB_RTD, cob_field *f, char * recdata, char*target);
COB_DLL_EXPIMP void     cob_fieldlist_to_field (COB_RTD, cob_field *f);
COB_DLL_EXPIMP long long cob_binary_mget_int64 (COB_RTD, const cob_field * const f);
COB_DLL_EXPIMP void     cob_binary_mset_int64 (COB_RTD, cob_field *f, long long n) ;
COB_DLL_EXPIMP void     cob_display_set_double (COB_RTD, cob_field *f2, double v);
COB_DLL_EXPIMP void     cob_move_display_to_fp (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_display_to_fp_opt (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_all (COB_RTD, cob_field *src, cob_field *dst);
COB_DLL_EXPIMP void     cob_move_alphanum_to_alphanum (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_national_to_national (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_display_to_display (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_numeric_display_to_numeric_display (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_display_to_packed (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_display_to_binary (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_display_to_edited (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_packed_to_display (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_packed_to_packed (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_packed_to_binary (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_packed_to_fp (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_binary_to_display (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_binary_to_binary (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_binary_to_packed (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_binary_to_fp (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_edited_to_display (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_alphanum_to_edited (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_fp_to_fp (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_fp_to_display (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_fp_to_display_opt (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_fp_to_binary (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_alphanum_to_national (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_national_to_alphanum (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void     cob_move_all_char_to_edited(COB_RTD, cob_field *f1, cob_field *f2);


COB_DLL_EXPIMP unsigned long long cob_display_get_ull (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_put_sign_ebcdic (COB_RTD, unsigned char *p, const int sign);
COB_DLL_EXPIMP void cob_put_sign_ascii (COB_RTD, unsigned char *p, const int sign);

#endif /* COB_MOVE_H */

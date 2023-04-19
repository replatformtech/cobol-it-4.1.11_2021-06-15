/*
 * Copyright (C) 2005-2007 Roger While
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

#ifndef COB_INTRINSIC_H
#define COB_INTRINSIC_H

#include <libcob/common.h>

COB_DLL_EXPIMP cob_field    *cob_intr_binop (COB_RTD, cob_field *f1, int op, cob_field *f2);
COB_DLL_EXPIMP cob_field    *cob_intr_current_date (COB_RTD, const int offset, const int length);
COB_DLL_EXPIMP cob_field    *cob_intr_when_compiled (COB_RTD, const int offset, const int length, cob_field *f);
COB_DLL_EXPIMP cob_field    *cob_intr_exception_file (COB_RTD);
COB_DLL_EXPIMP cob_field    *cob_intr_exception_location (COB_RTD);
COB_DLL_EXPIMP cob_field    *cob_intr_exception_status (COB_RTD);
COB_DLL_EXPIMP cob_field    *cob_intr_exception_statement (COB_RTD);
COB_DLL_EXPIMP cob_field    *cob_intr_char (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_ord (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_stored_char_length (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_combined_datetime (COB_RTD, cob_field *srcdays, cob_field *srctime);
COB_DLL_EXPIMP cob_field    *cob_intr_date_of_integer (COB_RTD, cob_field *srcdays);
COB_DLL_EXPIMP cob_field    *cob_intr_day_of_integer (COB_RTD, cob_field *srcdays);
COB_DLL_EXPIMP cob_field    *cob_intr_integer_of_date (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_integer_of_day (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_test_date_yyyymmdd (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_test_day_yyyyddd (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_factorial (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_exp (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_exp10 (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_abs (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_acos (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_asin (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_atan (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_cos (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_log (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_log10 (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_sin (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_sqrt (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_tan (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_upper_case (COB_RTD, const int offset, const int length,
                                                  cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_lower_case (COB_RTD, const int offset, const int length,
                                                  cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_reverse (COB_RTD, const int offset, const int length,
                                               cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_concatenate       (COB_RTD, const int, const int,
                                                         const int, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_substitute        (COB_RTD, const int, const int,
                                                         const int, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_substitute_case   (COB_RTD, const int, const int,
                                                         const int, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_trim (COB_RTD, const int offset, const int length,
                                            cob_field *srcfield, const int direction);
COB_DLL_EXPIMP cob_field    *cob_intr_triml(COB_RTD, const int offset, const int length,
                                            cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_trimr(COB_RTD, const int offset, const int length,
                                            cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_length (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_byte_length (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_integer (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_integer_part (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_fraction_part (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_sign (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_numval (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_numval_c (COB_RTD, cob_field *srcfield, cob_field *currency);
COB_DLL_EXPIMP cob_field    *cob_intr_annuity (COB_RTD, cob_field *srcfield1, cob_field *srcfield2);
COB_DLL_EXPIMP cob_field    *cob_intr_mod (COB_RTD, cob_field *srcfield1, cob_field *srcfield2);
COB_DLL_EXPIMP cob_field    *cob_intr_rem (COB_RTD, cob_field *srcfield1, cob_field *srcfield2);
COB_DLL_EXPIMP cob_field    *cob_intr_sum (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_ord_min (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_ord_max (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_min (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_max (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_midrange (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_median (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_mean (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_range (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_random (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_variance (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_standard_deviation (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_present_value (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_year_to_yyyy (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_date_to_yyyymmdd (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_day_to_yyyyddd (COB_RTD, int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_seconds_past_midnight (COB_RTD);
COB_DLL_EXPIMP cob_field    *cob_intr_seconds_from_formatted_time (COB_RTD, cob_field *format, cob_field *value);
COB_DLL_EXPIMP cob_field    *cob_intr_locale_date (COB_RTD, const int offset, const int length,
                                                   cob_field *srcfield, cob_field *locale_field);
COB_DLL_EXPIMP cob_field    *cob_intr_locale_time (COB_RTD, const int offset, const int length,
                                                   cob_field *srcfield, cob_field *locale_field);
COB_DLL_EXPIMP cob_field    *cob_intr_lcl_time_from_secs (COB_RTD, const int offset, const int length,
                                                          cob_field *srcfield, cob_field *locale_field);
COB_DLL_EXPIMP cob_field    *cob_intr_national_of (COB_RTD, const int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_display_of (COB_RTD, const int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_national_of_1 (COB_RTD, const int offset, const int length,
                                                   const int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_display_of_1 (COB_RTD, const int offset, const int length,
                                                    const int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_debugcp_to_display_1 (COB_RTD, const int offset, const int length, const int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_utf8_to_display_1 (COB_RTD, const int offset, const int length, const int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_display_to_debugcp_1 (COB_RTD, const int offset, const int length, const int params, ...);
COB_DLL_EXPIMP cob_field    *cob_intr_display_to_utf8_1 (COB_RTD, const int offset, const int length, const int params, ...);
COB_DLL_EXPIMP const char   *cob_get_codepage (COB_RTD);
COB_DLL_EXPIMP cob_field    *cob_intr_cstring (COB_RTD, cob_field *srcfield);
COB_DLL_EXPIMP cob_field    *cob_intr_integer_from_int (COB_RTD, int val);


#endif /* COB_INTRINSIC_H */

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

#ifndef COB_NUMERIC_H
#define COB_NUMERIC_H

#include <gmp.h>
#include <libcob/common.h>


#define COB_STORE_ROUND			0x01
#define COB_STORE_KEEP_ON_OVERFLOW	0x02
#define COB_STORE_TRUNC_ON_OVERFLOW	0x04
#define COB_STORE_ROUND_FP    	0x08

/* Only Intel X86 CPU allow  non alligned access*/
 

/*
 * Internal representation of decimal numbers.
 *
 *   n = value / 10^scale
 */

COB_DLL_EXPIMP void cob_decimal_init (cob_decimal * const d);
COB_DLL_EXPIMP void cob_decimal_and (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_or  (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_xor (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_not (COB_RTD, cob_decimal * const d1, cob_decimal * const ignored);
COB_DLL_EXPIMP void cob_decimal_sign (COB_RTD, cob_decimal * const d1);
COB_DLL_EXPIMP void cob_decimal_sign_1 (COB_RTD, cob_decimal * const dr,  cob_decimal * const d1);

COB_DLL_EXPIMP void cob_decimal_add_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_sub_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_mul_1 (cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_div_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_div_ibm_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2,  int maxscale);
COB_DLL_EXPIMP void cob_decimal_mod_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_pow_1 (cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2);

COB_DLL_EXPIMP void cob_decimal_add (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_sub (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_mul (cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_div (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_div_ibm (COB_RTD, cob_decimal * const d1, cob_decimal * const d2, int maxscale);
COB_DLL_EXPIMP void cob_decimal_pow (cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP int  cob_decimal_cmp (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_mod (COB_RTD, cob_decimal * const d1, cob_decimal * const d2);
COB_DLL_EXPIMP void cob_decimal_clear (cob_decimal * const d);
COB_DLL_EXPIMP void cob_decimal_set_int (cob_decimal * const d, const int n);
COB_DLL_EXPIMP void cob_decimal_set_int_1 (cob_decimal * const d, const int n, const int gmp_flags);
COB_DLL_EXPIMP void cob_decimal_set_uint (cob_decimal * const d, const unsigned int n);
COB_DLL_EXPIMP void cob_decimal_set_uint_1 (cob_decimal * const d, const unsigned int n, const int gmp_flags);
COB_DLL_EXPIMP void cob_decimal_set (cob_decimal * const dst, cob_decimal * const src);
COB_DLL_EXPIMP double cob_decimal_get_double (cob_decimal * const d);
COB_DLL_EXPIMP void cob_shift_decimal (COB_RTD, cob_decimal * const d, const int n);
COB_DLL_EXPIMP void cob_shift_decimal_no_scale (COB_RTD, cob_decimal * const d, const int n);
COB_DLL_EXPIMP void cob_decimal_trunc (COB_RTD, cob_decimal * const d, const int n);
COB_DLL_EXPIMP void cob_decimal_set_field (COB_RTD, cob_decimal * const d, cob_field *f);
COB_DLL_EXPIMP void cob_decimal_set_field_1 (COB_RTD, cob_decimal * const d, cob_field *f, int gmp_flags);
COB_DLL_EXPIMP int  cob_decimal_get_field (COB_RTD, cob_decimal * const d, cob_field *f, const int opt);
COB_DLL_EXPIMP int  cob_decimal_get_field_1 (COB_RTD, cob_decimal *  const d, cob_field * const f, const int opt, int copy_data);
COB_DLL_EXPIMP int cob_decimal_digits_count (cob_decimal * const d) ;


COB_DLL_EXPIMP int cob_display_add_int (COB_RTD, cob_field *f, int n);
COB_DLL_EXPIMP int cob_unsign_display_fast_add_small_int (COB_RTD, cob_field * const f, int n);
COB_DLL_EXPIMP int cob_add (COB_RTD, cob_field *f1, cob_field *f2, const int opt);
COB_DLL_EXPIMP int cob_sub (COB_RTD, cob_field *f1, cob_field *f2, const int opt);
COB_DLL_EXPIMP int cob_mul (COB_RTD, cob_field *f1, cob_field *f2, const int opt);
COB_DLL_EXPIMP int cob_div (COB_RTD, cob_field *f1, cob_field *f2, const int opt);
COB_DLL_EXPIMP int cob_add_int (COB_RTD, cob_field *f, const int n);
COB_DLL_EXPIMP int cob_sub_int (COB_RTD, cob_field *f, const int n);
COB_DLL_EXPIMP int cob_div_quotient (COB_RTD, cob_field *dividend, cob_field *divisor, cob_field *quotient, const int opt);
COB_DLL_EXPIMP int cob_div_remainder (COB_RTD, cob_field *fld_remainder, const int opt);

COB_DLL_EXPIMP int cob_cmp_int (COB_RTD, cob_field *f1, const int n);
COB_DLL_EXPIMP int cob_cmp_uint (COB_RTD, cob_field *f1, const unsigned int n);
COB_DLL_EXPIMP int cob_cmp_packed (COB_RTD, cob_field *f, int n);
COB_DLL_EXPIMP int cob_cmp_numdisp (COB_RTD, const unsigned char *data, const size_t size, const int n);
COB_DLL_EXPIMP int cob_cmp_display_mf50 (COB_RTD, const unsigned char *data1, const size_t size1, 
                                         const unsigned char *data2, const size_t size2);
COB_DLL_EXPIMP int cob_cmp_sign_numdisp (COB_RTD, const unsigned char *data, const size_t size, const int n);
COB_DLL_EXPIMP int cob_cmp_long_numdisp (COB_RTD, const unsigned char *data, const size_t size, const int n);
COB_DLL_EXPIMP int cob_cmp_long_sign_numdisp (COB_RTD, const unsigned char *data, const size_t size, const int n);
COB_DLL_EXPIMP void cob_set_packed_zero (cob_field *f);
COB_DLL_EXPIMP void cob_set_packed_int (cob_field *f, const int val);
COB_DLL_EXPIMP void cob_set_packed_long_long (cob_field *f, const long long val);
COB_DLL_EXPIMP unsigned long long cob_packed_extract_ull(unsigned char *p, int digits, int comp_6 );
COB_DLL_EXPIMP unsigned int cob_packed_extract_uint(unsigned char *p, int digits, int comp_6 );

COB_DLL_EXPIMP void cob_decimal_add_sll (COB_RTD, cob_decimal *d1, const long long d2);
COB_DLL_EXPIMP void cob_decimal_sub_sll (COB_RTD, cob_decimal *d1, const long long d2);
COB_DLL_EXPIMP void cob_decimal_mul_sll (COB_RTD, cob_decimal *d1, const long long d2);
COB_DLL_EXPIMP void cob_decimal_add_ull (COB_RTD, cob_decimal *d1, const unsigned long long d2);
COB_DLL_EXPIMP void cob_decimal_sub_ull (COB_RTD, cob_decimal *d1, const unsigned long long d2);
COB_DLL_EXPIMP void cob_decimal_mul_ull (COB_RTD, cob_decimal *d1, const unsigned long long d2);

COB_DLL_EXPIMP void cob_decimal_add_sll_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const long long d2);
COB_DLL_EXPIMP void cob_decimal_sub_sll_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const long long d2);
COB_DLL_EXPIMP void cob_decimal_mul_sll_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const long long d2);

COB_DLL_EXPIMP void cob_decimal_add_ull_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const unsigned long long d2);
COB_DLL_EXPIMP void cob_decimal_sub_ull_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const unsigned long long d2);
COB_DLL_EXPIMP void cob_decimal_mul_ull_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const unsigned long long d2);

COB_DLL_EXPIMP void cob_decimal_mul_int (cob_decimal * const d1, const int d2);
COB_DLL_EXPIMP void cob_decimal_mul_uint (cob_decimal * const d1, const unsigned int d2);
COB_DLL_EXPIMP void cob_decimal_div_uint (COB_RTD, cob_decimal * const d1,  const unsigned int d2);
COB_DLL_EXPIMP void cob_decimal_mod_uint (COB_RTD, cob_decimal * const d1,  const unsigned int d2);
COB_DLL_EXPIMP void cob_decimal_pow_uint (cob_decimal * const d1,  const unsigned int d2);


COB_DLL_EXPIMP void cob_decimal_set_cob_float (cob_decimal *d, cob_float *v);
COB_DLL_EXPIMP void cob_decimal_set_double (cob_decimal *d, const double v);
COB_DLL_EXPIMP void cob_decimal_set_float  (cob_decimal *d, const float v);
COB_DLL_EXPIMP void cob_decimal_set_sll (cob_decimal *d, const signed long long  n) ;
COB_DLL_EXPIMP void cob_decimal_set_ull (cob_decimal *d, const unsigned long long  n) ;
COB_DLL_EXPIMP void cob_decimal_set_sll_1 (cob_decimal *d, const signed long long  n, const int gmp_flags) ;
COB_DLL_EXPIMP void cob_decimal_set_ull_1 (cob_decimal *d, const unsigned long long  n, const int gmp_flags) ;
COB_DLL_EXPIMP signed long long cob_decimal_get_sll (cob_decimal *d);
COB_DLL_EXPIMP unsigned long long cob_decimal_get_ull (cob_decimal *d);

#define cob_binary_and(_x,_y) (_x) = (_x) & (_y)
#define cob_binary_or(_x,_y)  (_x) = (_x) | (_y)
#define cob_binary_xor(_x,_y) (_x) = (_x) ^ (_y)
#define cob_binary_not(_x,_y) (_x) = (~(_x) & (_y))
#define cob_binary_add(_x,_y) (_x) = (_x) + (_y)
#define cob_binary_sub(_x,_y) (_x) = (_x) - (_y)
#define cob_binary_mul(_x,_y) (_x) = (_x) * (_y)
#define cob_binary_div(_x,_y) (_x) = (_x) / (_y) 
#define cob_binary_mod(_x,_y) (_x) = (_x) % (_y)
#define cob_binary_assign(_x,_y) (_x) = (_y)
#define cob_binary_sign(_x) (_x) = -(_x)

#define cob_binary_and_1(_z,_x,_y) (_z) = (_x) & (_y)
#define cob_binary_or_1(_z,_x,_y)  (_z) = (_x) | (_y)
#define cob_binary_xor_1(_z,_x,_y) (_z) = (_x) ^ (_y)
#define cob_binary_not_1(_z,_x,_y) (_z) = (~(_x) & (_y))
#define cob_binary_add_1(_z,_x,_y) (_z) = (_x) + (_y)
#define cob_binary_sub_1(_z,_x,_y) (_z) = (_x) - (_y)
#define cob_binary_mul_1(_z,_x,_y) (_z) = (_x) * (_y)
#define cob_binary_div_1(_z,_x,_y) (_z) = (_x) / (_y) 
#define cob_binary_mod_1(_z,_x,_y) (_z) = (_x) % (_y)
#define cob_binary_sign_1(_z, _x) (_z) = -(_x)

#define cob_binary_div_c_1(_z,_x,_y) ((_y != 0) ? (_z) = (_x) / (_y) : cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE))
#define cob_binary_mod_c_1(_z,_x,_y) ((_y != 0) ? (_z) = (_x) % (_y) : cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE)) 

#define cob_binary_pow_1(_z,_x,_y) (_z) = pow((_x), (_y))
/*
#define cob_binary_div_c(_x,_y) {if (_y != 0) {(_x) = (_x) / (_y); } else{ cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE);}}                               
#define cob_binary_mod_c(_x,_y) {if (_y != 0) {(_x) = (_x) % (_y); } else{ cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE);}}
*/ 
#define cob_binary_div_c(_x,_y) ((_y != 0) ? (_x) = (_x) / (_y) : cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE))
#define cob_binary_mod_c(_x,_y) ((_y != 0) ? (_x) = (_x) % (_y) : cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE)) 
 
#define cob_binary_pow(_x,_y) (_x) = pow((_x), (_y))
#define cob_binary_set_int(_x,_v) (_x) = (_v)
#define cob_binary_set_uint(_x,_v) (_x) = (_v)
#define cob_binary_cmp(_x,_v)  ((_x) < (_v) ? -1 : ((_x) > (_v)) ? 1 : 0)

#define cob_binary_set_float(_x,_v) (_x) = (float)(_v)
#define cob_binary_set_double(_x,_v) (_x) = (double)(_v)
#define cob_binary_double_set_decimal(_x,_d)  (_x) = cob_decimal_get_double(_d)
#define cob_binary_max(_x,_y) ((_x) < (_y) ? (_y) : (_x))
#define cob_binary_min(_x,_y) ((_x) > (_y) ? (_y) : (_x))
#define cob_binary_abs(_x) ((_x) < 0 ? -(_x) : (_x))
#define cob_binary_abs_1(_x) (_x) = ((_x) < 0 ? -(_x) : (_x))

/* cob_float */

COB_DLL_EXPIMP void cob_float_init (cob_float * const f,  double d);
COB_DLL_EXPIMP void cob_float_add_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2);
COB_DLL_EXPIMP void cob_float_sub_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2);
COB_DLL_EXPIMP void cob_float_mul_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2);
COB_DLL_EXPIMP void cob_float_div_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2);
COB_DLL_EXPIMP void cob_float_mod_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2);
COB_DLL_EXPIMP void cob_float_pow_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2);
#define cob_float_add(a,b) cob_float_add_1(a,a,b) 
#define cob_float_sub(a,b) cob_float_sub_1(a,a,b)
#define cob_float_mul(a,b) cob_float_mul_1(a,a,b)
#define cob_float_div(a,b) cob_float_div_1(a,a,b)
#define cob_float_pow(a,b) cob_float_pow_1(a,a,b)
#define cob_float_mod(a,b) cob_float_mod_1(a,a,b)



COB_DLL_EXPIMP void cob_float_clear (cob_float * const d);
COB_DLL_EXPIMP void cob_float_set (cob_float * const dst, cob_float * const src);
COB_DLL_EXPIMP void cob_float_set_str (cob_float * const dst, char * const src, int scale, int sign);
COB_DLL_EXPIMP double cob_float_get_double (cob_float * const d);
COB_DLL_EXPIMP void cob_float_set_double (cob_float *d, const double v);

#endif /* COB_NUMERIC_H */

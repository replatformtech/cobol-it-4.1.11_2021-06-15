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

#ifndef CIT_BIGINT_H
#define CIT_BIGINT_H

#define CIT_BIGINT_NUM_DIGITS 64
typedef struct {
        unsigned char * start_digits;
        unsigned char * end_digits; 
        unsigned char * original_data;
        unsigned char * start_buffer;
        unsigned char * end_buffer; 
        unsigned char   prebuffer[CIT_BIGINT_NUM_DIGITS];
        unsigned char   buffer[CIT_BIGINT_NUM_DIGITS+1];
        unsigned char   postbuffer[8];
        int             phantom_digits;
        char            negate;
        char            local_copy;
        char            overflow;
        char            zero_flag;
} cit_bigint_t;

typedef cit_bigint_t * const pcit_bigint_t;
#define CIT_BIGINT_USEDDIGIT(d) ((d->end_digits - d->start_digits) +1)
#define CIT_BIGINT_SET_STARTEND(d,ptr,len) d->start_digits = ptr; d->end_digits = ptr + len -1;
#define CIT_BIGINT_SET_BUFFER_STARTEND(d,start,end) d->start_buffer = start; d->end_buffer = end;

int cit_bigint_cmp(pcit_bigint_t d1, pcit_bigint_t d2); 
int cit_bigint_add(pcit_bigint_t d1, pcit_bigint_t d2); 
int cit_bigint_add_int(pcit_bigint_t d1, int i2, int i2_scale); 
int cit_bigint_sub(pcit_bigint_t d1, pcit_bigint_t d2); 
int cit_bigint_mul(pcit_bigint_t d1, pcit_bigint_t d2);
int cit_bigint_mul_smallint(pcit_bigint_t d1, int i2); /* i2 must be >= -9 and <= 9*/

int cit_bigint_set_from_string(pcit_bigint_t d1, unsigned char *str, int str_size, int sign);
int cit_bigint_copy_from_string(pcit_bigint_t d1, unsigned char *str, int str_size, int sign);
int cit_bigint_get_to_string(pcit_bigint_t d1, unsigned char *str, int str_size, int *sign);
int cit_bigint_get_sign (pcit_bigint_t d1);
int cit_bigint_get_to_BCD(pcit_bigint_t d1, unsigned char *data, int digits, int has_sign, int *sign);
int cit_bigint_set_from_BCD(pcit_bigint_t d1, unsigned char *data, int sz, int sign);

int cit_bigint_set_from_uint(pcit_bigint_t d1, unsigned int i);
int cit_bigint_set_from_int(pcit_bigint_t d1, int i);
int cit_bigint_set_from_ull(pcit_bigint_t d1, unsigned long long i);
int cit_bigint_set_from_sll(pcit_bigint_t d1, long long i);

int cit_bigint_trunc (pcit_bigint_t d1, int shift);
int cit_bigint_shift (pcit_bigint_t d, int shift);
int cit_bigint_digits_count (pcit_bigint_t d);


char * cit_bigint_to_string (pcit_bigint_t d, char * buffer, int *len);
void cit_bigint_copy (pcit_bigint_t d1, pcit_bigint_t d2) ;
void cit_bigint_ensure_local_copy (pcit_bigint_t d);

void cit_bigint_init (unsigned char zerochar);

#endif /* CIT_BIGINT_H*/
/*CIT_END_ENTERPRISE*/


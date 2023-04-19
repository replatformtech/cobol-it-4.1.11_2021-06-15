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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "citbigint.h"

#ifdef _MSC_VER
    #define inline _inline
#else 
    #if (defined(__hpux__) || defined(__hpux))
    #define inline __inline
    #endif 
#endif
static unsigned char cit_bigintzeroChar = '0';
static unsigned int  cit_bigintzeroChar_i =  0x30303030;
#ifndef min
    #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
    #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define LOW_NIBBLE(a) (a & 0x0F) 
/*#define LOW_NIBBLE(a) (a)*/
#define FAST_MUL_10(a) ((a << 3) + (a << 1))
/*#define FAST_MUL_10(a) (a*10)*/

#define ASSERT_STARTEND(d) assert(d->start_digits >= d->start_buffer); assert(d->end_digits < d->end_buffer);
#define GET_NEGATE(sign) (sign < 0 ? 1 : 0)
static const unsigned char packed_value[] = {
/*         0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A 0x0b 0x0C 0x0D 0x0E 0x0F */
/*0x00*/      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   00,  00,  00,  00,  00,  00,   
/*0x10*/     10,   11,   12,   13,   14,   15,   16,   17,   18,   19,   10,  10,  10,  10,  10,  10,   
/*0x20*/     20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   20,  20,  20,  20,  20,  20,   
/*0x30*/     30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   30,  30,  30,  30,  30,  30,   
/*0x40*/     40,   41,   42,   43,   44,   45,   46,   47,   48,   49,   40,  40,  40,  40,  40,  40,   
/*0x50*/     50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   50,  50,  50,  50,  50,  50,   
/*0x60*/     60,   61,   62,   63,   64,   65,   66,   67,   68,   69,   60,  60,  60,  60,  60,  60,   
/*0x70*/     70,   71,   72,   73,   74,   75,   76,   77,   78,   79,   70,  70,  70,  70,  70,  70,   
/*0x80*/     80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   80,  80,  80,  80,  80,  80,   
/*0x90*/     90,   91,   92,   93,   94,   95,   96,   97,   98,   99,   90,  90,  90,  90,  90,  90,   
/*0xA0*/      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   00,  00,  00,  00,  00,  00,   
/*0xB0*/      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   00,  00,  00,  00,  00,  00,   
/*0xC0*/      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   00,  00,  00,  00,  00,  00,   
/*0xD0*/      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   00,  00,  00,  00,  00,  00,   
/*0xE0*/      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   00,  00,  00,  00,  00,  00,   
/*0xF0*/      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   00,  00,  00,  00,  00,  00
};
#define BCD_VAL(a) packed_value[a]

void cit_bigint_init (unsigned char zerochar) {
    unsigned int i;
    cit_bigintzeroChar = zerochar;
    i = sizeof(int);
    cit_bigintzeroChar_i= zerochar;
    for (i = 1; i < sizeof(int); i++) {
        cit_bigintzeroChar_i = (cit_bigintzeroChar_i << 8) | zerochar;
    }
}

static inline void cit_bigint_not_negate(pcit_bigint_t d1)
{
    if (d1->negate) {
        d1->negate = 0;
    } else {
        d1->negate = 1;
    }
}
int cit_bigint_set_from_string(pcit_bigint_t d1, unsigned char *str, int str_size, int sign)
{
    d1->local_copy      = 0;
    CIT_BIGINT_SET_STARTEND(d1,str,str_size);
    CIT_BIGINT_SET_BUFFER_STARTEND(d1,str, str+str_size);
    d1->original_data   = str;
    d1->overflow        = 0;
    d1->phantom_digits  = 0;
    d1->negate          = GET_NEGATE(sign);
    return 0;
}

int cit_bigint_set_from_BCD(pcit_bigint_t d1, unsigned char *data, int digit, int sign)
{
    register unsigned char *d = data;
    register unsigned char *q = d1->buffer;
    register unsigned char c;
    d1->local_copy      = 1;
    if (sign != 0) {
        if ((digit &0x01) == 0) {
            *q++ = (*d++ & 0x0F) | cit_bigintzeroChar_i;
            digit --;
        }
    }else {
        if ((digit &0x01) == 1) {
            *q++ = (*d++ & 0x0F) | cit_bigintzeroChar_i;
            digit --;
        }
    }
    while (digit) {
        c = *d++;
        *q++ = ((c & 0xF0) >> 4 ) | cit_bigintzeroChar_i;
        digit --;
        if (digit) {
            *q++ = ((c & 0x0F) ) | cit_bigintzeroChar_i;
            digit--;
        }
    }
    d1->start_digits = d1->buffer;
    d1->end_digits = q-1;
    CIT_BIGINT_SET_BUFFER_STARTEND(d1,d1->prebuffer, d1->postbuffer); 
    d1->original_data   = NULL;
    d1->overflow        = 0;
    d1->phantom_digits  = 0;
    d1->negate          = GET_NEGATE(sign);
    return 0;
}

#define BASE_CONST_BUFFER_INIT(format)     \
    if (i < 10) {  d1->buffer[0]   = cit_bigintzeroChar + i; d1->buffer[1]   = 0; len = 1;    } \
    else { sprintf ((char*)d1->buffer, format, i); len = (unsigned int)strlen((char*)d1->buffer); } \
    CIT_BIGINT_SET_STARTEND(d1,d1->buffer,len); \
    CIT_BIGINT_SET_BUFFER_STARTEND(d1,d1->prebuffer, d1->postbuffer); \
    d1->local_copy      = 1; \
    d1->original_data   = NULL; \
    d1->overflow        = 0; \
    d1->phantom_digits  = 0;

int cit_bigint_set_from_int(pcit_bigint_t d1, int i)
{
    int len;
    d1->negate = GET_NEGATE(i);
    if (d1->negate) {
        i = -i;
    } 

    BASE_CONST_BUFFER_INIT("%d");
    return 0;
}

int cit_bigint_set_from_sll(pcit_bigint_t d1, long long i)
{
    int len;
    d1->negate = GET_NEGATE(i);
    if (d1->negate) {
        i = -i;
    } 

    BASE_CONST_BUFFER_INIT("%lld");
    return 0;
}

int cit_bigint_set_from_uint(pcit_bigint_t d1, unsigned int i)
{
    int len;
    d1->negate = 0;

    BASE_CONST_BUFFER_INIT("%u");
    return 0;
}

int cit_bigint_set_from_ull(pcit_bigint_t d1, unsigned long long i)
{
    int len;
    d1->negate = 0;

    BASE_CONST_BUFFER_INIT("%llu");
    return 0;
}

static void inline fast_to_localbuffer (pcit_bigint_t d1, unsigned char * const src, int sz)
{
    register unsigned int *q = (unsigned int *)d1->buffer;
    register int i = (sz/4) +1 ;
    memcpy(q, src, sz);    
    while (i--) {
        *q = (*q & 0x0F0F0F0F) | cit_bigintzeroChar_i;
        q++;
    } 
    CIT_BIGINT_SET_STARTEND(d1,d1->buffer, sz);
    CIT_BIGINT_SET_BUFFER_STARTEND(d1,d1->prebuffer, d1->postbuffer);
    d1->local_copy      = 1;
    d1->original_data   = NULL;


}

int cit_bigint_copy_from_string(pcit_bigint_t d1, unsigned char *str, int str_size, int sign)
{   
    fast_to_localbuffer(d1,str, str_size);
    d1->overflow        = 0;
    d1->phantom_digits  = 0;
    d1->negate          = GET_NEGATE(sign);
    return 0;

}

void cit_bigint_ensure_local_copy (pcit_bigint_t d)
{
    if (!d->local_copy) {
        fast_to_localbuffer(d,d->start_digits, CIT_BIGINT_USEDDIGIT(d));
    } 
    if (d->phantom_digits) {
        memset (d->end_digits + 1, cit_bigintzeroChar, d->phantom_digits);
        d->end_digits += d->phantom_digits;
        d->phantom_digits = 0;
    }

}

static int inline cit_bigint_cmp_abs(pcit_bigint_t d1, pcit_bigint_t d2) 
{
    register unsigned char *q1;
    register unsigned char *q2;
    register unsigned char *q1_end;
    register unsigned char *q2_end;
    int delta;
    int delta_phantom;
    int ph1, ph2;

    q1 = d1->start_digits;
    q2 = d2->start_digits;
    q1_end = d1->end_digits;
    q2_end = d2->end_digits;
    ph1 = d1->phantom_digits;
    ph2 = d2->phantom_digits;    
    delta_phantom = ph1 - ph2;
    if (delta_phantom) {
        if (delta_phantom > 0 ) {
            ph1 = delta_phantom;
            ph2 = 0;
        } else {
            ph1 = 0;
            ph2 = -delta_phantom;
        }
    }
    delta = (CIT_BIGINT_USEDDIGIT(d1) + ph1) - (CIT_BIGINT_USEDDIGIT(d2) + ph2);
    if (delta > 0) {
        while ( delta && q1 <= q1_end) {
            if (LOW_NIBBLE(*q1) != 0) {
                return 1;
            }
            q1++;
            delta--;
        }
    } else {
        while ( delta && q2 <= q2_end) {
            if (LOW_NIBBLE(*q2) != 0) {
                return -1;
            }
            q2++;
            delta++;
        }
    }
    if (delta == 0) {
        while (q1 <= q1_end) {
            register int c;
            if ( q2 <= q2_end) {
                c = (int)LOW_NIBBLE(*q1) - (int)LOW_NIBBLE(*q2);
                if (c) {
                    if (c < 0) {
                        return -1;
                    } else {
                        return 1;
                    }
                } 
            } else if (LOW_NIBBLE(*q1) != 0) {
                return 1;
            }
            q1++;
            q2++;
        }
    } 
    if (delta_phantom > 0) {
        /* only phantom digit in q1 left */
        while ( q2 <= q2_end) {
            if (LOW_NIBBLE(*q2) != 0) {
                return -1;
            }
            q2++;
        }
    } else {
        /* only phantom digit in q2 left */
        while ( q1 <= q1_end) {
            if (LOW_NIBBLE(*q1) != 0) {
                return 1;
            }
            q1++;
        }
    }
    
    return 0;

}



int cit_bigint_cmp(pcit_bigint_t d1, pcit_bigint_t d2) 
{
    int res;
    if (d1->negate != d2->negate) {
        if (d1->negate) {
            return -1;
        } else {
            return 1; 
        }
    }
    if (d1->negate) {
        res = cit_bigint_cmp_abs(d2,d1);
    } else {
        res = cit_bigint_cmp_abs(d1,d2);
    }
    return res;
}

#define STRIP_ZERO_BEFORE(q1_end,q1_start)    while (LOW_NIBBLE(*q1_start) == 0 && q1_start <= q1_end) { q1_start++; }
#define STRIP_Q2_ZERO_AFTER(q1_end,q1_start,q2_end,q2_start)  while (LOW_NIBBLE(*q2_end) == 0 && q2_start <= q2_end && q1_start <= q1_end) { q2_end--; q1_end--;}



static int inline cit_bigint_add_abs(pcit_bigint_t d1, pcit_bigint_t d2) 
{
    int carry = 0;
    register unsigned char *q1;
    register unsigned char *q2;
    register unsigned char *q1_start;
    register unsigned char *q2_start;
    register unsigned char *q1_start_buffer;
    int phantom_delta;
    int delta;

    phantom_delta = d1->phantom_digits - d2->phantom_digits;
    if (phantom_delta > 0 ) {
        /*d1 has more phantom than d2*/
        cit_bigint_ensure_local_copy(d1);
    }
    q1 = d1->end_digits ;
    q2 = d2->end_digits ;
    q1_start = d1->start_digits ;
    q2_start = d2->start_digits ;
    q1_start_buffer = d1->start_buffer; 

    if (phantom_delta < 0 ) {
        /*d2 has more phantom than d1*/
        q1 -= (-phantom_delta);
        delta = q1_start - q1;
        if (delta > 0) {
            if (q1 < q1_start_buffer) {
                q1 = q1_start_buffer -1;
                delta = q1_start - q1;
                carry = 1;
            } 
            if (delta > 1) {
                memset (q1+1, cit_bigintzeroChar, delta -1);
            }
        }
    }

    STRIP_ZERO_BEFORE(q1, q1_start);
    STRIP_ZERO_BEFORE(q2, q2_start);
    STRIP_Q2_ZERO_AFTER(q1, q1_start, q2, q2_start);

    if (q2 >= q2_start) {
        while (q1 >= q1_start_buffer)
        {
            unsigned char c;
            register unsigned int k; 
            if (q1 >= q1_start) {
                c = LOW_NIBBLE(*q1);
            } else {
                c = 0;
            }
            if (q2 >= q2_start) {
                k = (unsigned int)c + (unsigned int)LOW_NIBBLE(*q2) + carry;
            } else {
                if (!carry) {
                    break;
                }
                k = (unsigned int)c + carry;
            }
            if (k >= 10)
            {
                k -= 10;
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            *q1 = cit_bigintzeroChar | k;
            q1--;
            q2--;
        }
    }
    if (q1 < d1->start_digits) {
        d1->start_digits = q1+ 1;
    }
    ASSERT_STARTEND(d1);
    d1->overflow |= carry;
    return carry;
}

static int inline cit_bigint_add_abs_int(pcit_bigint_t d1, int i2, int i2_scale) 
{
    int carry = 0;
    register unsigned char *q1;
    register unsigned char *q1_start;
    register unsigned char *q1_start_buffer;
    int phantom_delta;
    int delta;

    phantom_delta = d1->phantom_digits - i2_scale;
    if (phantom_delta > 0 ) {
        /*d1 has more phantom than d2*/
        cit_bigint_ensure_local_copy(d1);
    }
    q1 = d1->end_digits ;
    q1_start = d1->start_digits ;
    q1_start_buffer = d1->start_buffer; 

    if (phantom_delta < 0 ) {
        /*d2 has more phantom than d1*/
        q1 -= (-phantom_delta);
        delta = q1_start - q1;
        if (delta > 0) {
            if (q1 < q1_start_buffer) {
                q1 = q1_start_buffer -1;
                delta = q1_start - q1;
                carry = 1;
            } 
            if (delta > 1) {
                memset (q1+1, cit_bigintzeroChar, delta -1);
            }
        }
    }

    STRIP_ZERO_BEFORE(q1, q1_start);
    carry = i2;
    while (carry && q1 >= q1_start_buffer)
    {
        register unsigned int k; 
        if (q1 >= q1_start) {
            k = LOW_NIBBLE(*q1) + carry;
        } else {
            if (!carry) {
                break;
            }
            k = carry;
        }
        if (k >= 10)
        {
            carry = k / 10;
            k = k - FAST_MUL_10(carry); /*k % 10*/
        }
        else
        {
            carry = 0;
        }
        *q1 = cit_bigintzeroChar | k;
        q1--;
    }
    if (q1 < d1->start_digits) {
        d1->start_digits = q1+ 1;
    }
    ASSERT_STARTEND(d1);
    d1->overflow |= carry;
    return carry;
}


static int inline cit_bigint_sub_abs(pcit_bigint_t in1, pcit_bigint_t in2) 
{
    int carry = 0;
    int sign;
    cit_bigint_t           *d1, *d2;
    register unsigned char *t;
    register unsigned char *q1;
    register unsigned char *q2;
    register unsigned char *q1_start;
    register unsigned char *q2_start;
    register unsigned char *t_start;
    int fast_exit_flag;

    cit_bigint_ensure_local_copy(in1);
    sign = cit_bigint_cmp_abs(in1, in2);

    in1->zero_flag =0;
    if (sign > 0) {
        d1 = in1;
        d2 = in2;
        fast_exit_flag = 1;
        t  = in1->end_digits - d2->phantom_digits;
        q1 = t;
    } else if (sign < 0) {
        d2 = in1;
        d1 = in2;
        fast_exit_flag = 0;
        if (d1->phantom_digits) {
            cit_bigint_ensure_local_copy(d1);
        }
        t  = in1->end_digits;
        q1 = d1->end_digits;
    } else {
        cit_bigint_set_from_int(in1, 0);
        in1->zero_flag =1;
        return 0;
    }
    q2 = d2->end_digits;
    q1_start = d1->start_digits;
    q2_start = d2->start_digits;
    t_start  = in1->start_buffer;
    while( t >= t_start)
    {
        unsigned char c ;
        register int k; 
        if (q1 >= q1_start) {
            c = *q1;
        } else {
            c = 0;
            fast_exit_flag = 1;
        }

        if (q2 >= q2_start) {
            k = (int)LOW_NIBBLE(c) - (int)LOW_NIBBLE(*q2) - carry;
        } else {
            if (!carry && fast_exit_flag) {
                break;
            }
            k = (int)LOW_NIBBLE(c) - carry;
        }
        if (k < 0)
        {
            k += 10;
            carry = 1;
        }
        else
        {
            carry = 0;
        }
        *t = cit_bigintzeroChar | k;
        q1--;
        q2--;
        t--;
    }
    if (t < in1->start_digits) {
        in1->start_digits = t + 1;
    }
    ASSERT_STARTEND(in1);

    in1->overflow |= carry;
    in1->negate = GET_NEGATE(sign) ;
    return carry;
}


int cit_bigint_add(pcit_bigint_t d1, cit_bigint_t * d2) 
{
    int carry;
    if (d1->negate != d2->negate) {
        if (d1->negate) {
            d1->negate = 0;
            carry = cit_bigint_sub_abs(d1, d2);
            if (!d1->zero_flag) {
                cit_bigint_not_negate(d1);
            }
        } else  {
            carry = cit_bigint_sub_abs(d1, d2);
        }
    } else {
        carry = cit_bigint_add_abs(d1, d2);
    }
    return carry;
}

int cit_bigint_add_int(pcit_bigint_t d1, int i2, int i2_scale) 
{
    int carry;
    int d2_negate = i2 < 0 ? 1 : 0;
    if (d2_negate) {
        i2 = -i2;
    }
    if (d1->negate != d2_negate) {
        cit_bigint_t d2;
        cit_bigint_set_from_int(&d2, i2);
        if (d1->negate) {
            d1->negate = 0;
            carry = cit_bigint_sub_abs(d1, &d2);
            if (!d1->zero_flag) {
                cit_bigint_not_negate(d1);
            }
        } else  {
            carry = cit_bigint_sub_abs(d1, &d2);
        }
    } else {
        carry = cit_bigint_add_abs_int(d1, i2, i2_scale);
    }
    return carry;
}


int cit_bigint_sub(pcit_bigint_t d1, pcit_bigint_t d2) 
{
    int carry;
    if (d1->negate != d2->negate) {
        carry = cit_bigint_add_abs(d1, d2);
    } else {
        if (d1->negate) {
            d1->negate = 0;
            carry = cit_bigint_sub_abs(d1, d2);
            if (!d1->zero_flag) {
                cit_bigint_not_negate(d1);
            }
        } else  {
            carry = cit_bigint_sub_abs(d1, d2);
        } 
    }

    return carry;
}


static int inline cit_bigint_mul_abs(pcit_bigint_t d1, pcit_bigint_t d2) 
{
    register int carry = 0;
    register unsigned char *q;
    register unsigned char *q1;
    register unsigned char *q2;
    register unsigned char *q1_start;
    register unsigned char *q2_start;
    register unsigned char *q1_start_buffer;
    cit_bigint_t res;
    register unsigned char *t;
    int i;

    d1->phantom_digits = d1->phantom_digits + d2->phantom_digits;
    q1 = d1->end_digits ;
    q2 = d2->end_digits ;
    q1_start = d1->start_digits ;
    q2_start = d2->start_digits ;
    q1_start_buffer = d1->start_buffer; 

    STRIP_ZERO_BEFORE(q1, q1_start);
    STRIP_ZERO_BEFORE(q2, q2_start);
    while (q2_start <= q2 && LOW_NIBBLE(*q2) == 0 ) { 
        q2--; 
        d1->phantom_digits++;
        }

    if (q2 >= q2_start) {
        res.zero_flag = 0;
        memset (&res, 0, sizeof(res));
        cit_bigint_set_from_int(&res, 0);
        i = min(CIT_BIGINT_NUM_DIGITS-1, CIT_BIGINT_USEDDIGIT(d1) + CIT_BIGINT_USEDDIGIT(d2));
        res.buffer[i] = cit_bigintzeroChar;
        CIT_BIGINT_SET_STARTEND((&res), &res.buffer[i], 1);
        i = 0;
        while (q2 >= q2_start) {
            register unsigned int k; 
            register unsigned int c = (unsigned int)LOW_NIBBLE(*q2);
            carry = 0;
            t = res.end_digits - i; 
            q = q1 ;
            while  ( q >= q1_start)
            {
                k = (unsigned int)LOW_NIBBLE(*t) + ((unsigned int)LOW_NIBBLE(*q) * c) + carry;
                if (k >= 10)
                {
                    carry = k / 10;
                    k = k - FAST_MUL_10(carry); /*k % 10*/
                }
                else
                {
                    carry = 0;
                }
                if (t >= res.start_buffer) {
                    *t = cit_bigintzeroChar | k;
                } else {
                    res.overflow = 1;
                    goto nextloop;
                }

                q--;
                t--;
            }
            while (carry != 0) {
                k = (unsigned int)LOW_NIBBLE(*t) + carry;
                if (k >= 10)
                {
                    carry = k / 10;
                    k = k - FAST_MUL_10(carry); /*k % 10*/
                }
                else
                {
                    carry = 0;
                }
                if (t >= res.start_buffer) {
                    *t = cit_bigintzeroChar | k;
                } else {
                    res.overflow = 1;
                    goto nextloop;
                }
                t--;
            }
            nextloop:
            res.start_digits = t+1;
            i++;
            q2--;
        }
    } else {
        /* q2 is Zero */
        cit_bigint_set_from_int(d1, 0);
        res.zero_flag = 1;
        return 0;

    }
    cit_bigint_copy(d1,&res);
    ASSERT_STARTEND(d1);
    return res.overflow;
}

static int inline cit_bigint_mul_abs_smallint(pcit_bigint_t d1, int i2) 
{
    register int carry = 0;
    register unsigned char *q;
    register unsigned char *q1;
    register unsigned char *q1_start;
    register unsigned char *q1_start_buffer;
    cit_bigint_t res;
    register unsigned char *t;
    register unsigned int k; 
    int i;

    q1 = d1->end_digits ;
    q1_start = d1->start_digits ;
    q1_start_buffer = d1->start_buffer; 

    STRIP_ZERO_BEFORE(q1, q1_start);
    if (i2) {
        res.zero_flag = 0;
        memset (&res, 0, sizeof(res));
        cit_bigint_set_from_int(&res, 0);
        i = CIT_BIGINT_USEDDIGIT(d1);
        res.buffer[i] = cit_bigintzeroChar;
        CIT_BIGINT_SET_STARTEND((&res), &res.buffer[i], 1);
        i = 0;
        carry = 0;
        t = res.end_digits; 
        q = q1 ;
        while  ( q >= q1_start)
        {
            k = (unsigned int)LOW_NIBBLE(*t) + ((unsigned int)LOW_NIBBLE(*q) * i2) + carry;
            if (k >= 10)
            {
                carry = k / 10;
                k = k - FAST_MUL_10(carry); /*k % 10*/
            }
            else
            {
                carry = 0;
            }
            if (t >= res.start_buffer) {
                *t = cit_bigintzeroChar | k;
            } else {
                res.overflow = 1;
                goto nextloop;
            }

            q--;
            t--;
        }
        while (carry != 0) {
            k = (unsigned int)LOW_NIBBLE(*t) + carry;
            if (k >= 10)
            {
                carry = k / 10;
                k = k - FAST_MUL_10(carry); /*k % 10*/
            }
            else
            {
                carry = 0;
            }
            if (t >= res.start_buffer) {
                *t = cit_bigintzeroChar | k;
            } else {
                res.overflow = 1;
                goto nextloop;
            }
            t--;
        }
        nextloop:
        res.start_digits = t+1;
    } else {
        /* q2 is Zero */
        cit_bigint_set_from_int(d1, 0);
        res.zero_flag = 1;
        return 0;

    }
    cit_bigint_copy(d1,&res);
    ASSERT_STARTEND(d1);
    return res.overflow;
}

int cit_bigint_mul(pcit_bigint_t d1, pcit_bigint_t d2) 
{
    int carry;

    carry = cit_bigint_mul_abs(d1, d2);
    if (!d1->zero_flag) {
        d1->negate = d1->negate ^ d2->negate ;
    }
    return carry;
}

int cit_bigint_mul_smallint(pcit_bigint_t d1, int i2) 
{
    int carry;
    int i2_negate = i2 < 0 ? 1 : 0;

    if (i2_negate) {
        i2 = -i2;
    }
    if (i2 > 9) {
        cit_bigint_t d2;
        cit_bigint_set_from_int(&d2, i2);
        carry = cit_bigint_mul(d1, &d2);
    } else {
        carry = cit_bigint_mul_abs_smallint(d1, i2);
    }
    if (!d1->zero_flag) {
        d1->negate = d1->negate ^ i2_negate ;
    }
    return carry;
}



static void inline cit_bigint_convert_to_string(char * const target, unsigned char *const buffer, int size)
{
    register unsigned char *q;
    register unsigned char *t;
    t = (unsigned char*)target;
    q = buffer;

    while (size--) {
        *t = LOW_NIBBLE(*q) | cit_bigintzeroChar;
        q++;
        t++;
    }

}

int cit_bigint_get_to_string(pcit_bigint_t d1, unsigned char *str, int str_size, int *sign)
{
    int delta_size;
    unsigned char * d;
    register unsigned char * q;
    int d_size;
    int res = d1->overflow;
    register int i;
    d = d1->start_digits;
    d_size = CIT_BIGINT_USEDDIGIT(d1);
    *sign=d1->negate ? -1 : 1;
    if (!d1->local_copy && d1->phantom_digits && (str == d1->original_data)) {
        cit_bigint_ensure_local_copy(d1);
    }
    if ( d1->local_copy || (str != d1->original_data)) {
        memset (str, cit_bigintzeroChar, str_size);
        if (d1->phantom_digits) {
            if (d1->phantom_digits > str_size) {
                res = 1;
                return res;
            }
            str_size -= d1->phantom_digits;
        }
        delta_size = (d_size) - str_size;
        if (delta_size) {
            if (delta_size > 0) {
                i = delta_size;
                q = d;
                while (i--) {
                    if (LOW_NIBBLE(*q) != 0) {
                        res = 1;
                        break;
                    }
                    q++;
                }
                d = d + delta_size;
                d_size -= delta_size;
            } else {
                delta_size = -delta_size;
                str = str + delta_size;
                str_size -= delta_size;
            }            
        }
        assert (d_size==str_size);
        memcpy (str, d, str_size);
    }
    return res;
}
 
int cit_bigint_get_to_BCD(pcit_bigint_t d1, unsigned char *data, int digits, int has_sign, int *sign)
{
    register unsigned char *d;
    register unsigned char *q = d1->end_digits;
    register unsigned char c;
    int i;

    if (has_sign) {
        d = data + (digits/2);
    } else {
        d = data + ((digits-1)/2) ;
    }
    *sign=d1->negate ? -1 : 1;
    d1->overflow = 0;
    if (d1->phantom_digits) {
        i = d1->phantom_digits;
        if (has_sign) {
            *d-- = 0;
            has_sign= 0;
            i--;
            digits --;
        }
        while ((digits > 0) && (i> 0)) {
            c = 0;
            has_sign= 1;
            i--;
            digits --;
            if ((digits > 0) && (i > 0)) {
                has_sign= 0;
                i--;
                digits --;
                *d-- = 0;
            } else {
                *d = 0;
            }
        }

    }
    if (has_sign && digits > 0) {
        c = *d & 0x0F;
        c |= (LOW_NIBBLE(*q--) << 4);
        *d-- = c;
        digits --;
    }
    while ((digits > 0) && (q >= d1->start_digits)) {
        register unsigned char c;
        c = LOW_NIBBLE(*q--);
        digits--;
        if ((digits> 0) && (q >= d1->start_digits)) {
            c |= (LOW_NIBBLE(*q--) << 4);
            digits--;
        }
        *d-- = c;
    }
    while (q >= d1->start_digits) {
        if (LOW_NIBBLE(*q--) != 0) {
            d1->overflow = 1;
        }
    }
    return d1->overflow;
}


int cit_bigint_trunc (pcit_bigint_t d, int shift)
{
    if (shift > 0) {
        /* Left , add digit after */
        /*
        cit_bigint_ensure_local_copy(d);
        memset (d->end_digits + 1, cit_bigintzeroChar, shift);
        d->end_digits += shift;
        ASSERT_STARTEND(d); 
        */
        d->phantom_digits += shift; 
    } else {
        /* Rigth , remove digit*/
        shift = -shift;
        if (d->phantom_digits) {
            d->phantom_digits -= shift;
            if (d->phantom_digits < 0) {
                shift = - d->phantom_digits;
            } else {
                shift = 0;
            }
        }
        if (shift) {
            d->end_digits -= shift;
            if (d->end_digits < d->start_digits) {
                d->end_digits = d->start_digits;
                *(d->start_digits) = cit_bigintzeroChar;
            }
        }
    }
    return 0;
}

int cit_bigint_shift (pcit_bigint_t d, int shift)
{
    cit_bigint_ensure_local_copy(d);
    if (shift > 0) {
        /* Left , add digit before*/
        d->start_digits -= shift;
        ASSERT_STARTEND(d);
        memset (d->start_digits, cit_bigintzeroChar, shift);
    } else {
        /* Rigth , remove digit before*/
        shift = -shift;
        d->start_digits += shift;
        ASSERT_STARTEND(d);
    }
    return 0;
}

int cit_bigint_digits_count (pcit_bigint_t d)
{
    return CIT_BIGINT_USEDDIGIT(d) + d->phantom_digits;
}

char * cit_bigint_to_string (pcit_bigint_t d, char * buffer, int *len)
{
    int l;
    *len = cit_bigint_digits_count(d); 
    if (d->local_copy) {
        if (d->phantom_digits) {
            memset (d->end_digits+1, cit_bigintzeroChar, d->phantom_digits);
        }
        *(d->end_digits+d->phantom_digits+1) = 0;
        return (char*)d->start_digits;
    } else {
        l = CIT_BIGINT_USEDDIGIT(d);
        buffer[l] = 0;
        cit_bigint_convert_to_string(buffer, d->start_digits, l);
        if (d->phantom_digits) {
            memset (&buffer[l], cit_bigintzeroChar, d->phantom_digits);
            buffer[l+d->phantom_digits] = 0;;
        }
    }
    return (char*) buffer;
}

void cit_bigint_copy (pcit_bigint_t d1, pcit_bigint_t d2) 
{
    int delta;
    *d1 = *d2;
    if (d2->local_copy) {
        delta = d2->start_digits - d2->buffer;
        d1->start_digits = d1->buffer + delta;
        delta = d2->end_digits - d2->buffer;
        d1->end_digits = d1->buffer + delta;
        CIT_BIGINT_SET_BUFFER_STARTEND(d1,d1->prebuffer, d1->postbuffer);

    } else {
        cit_bigint_ensure_local_copy (d1);
    }
    d1->original_data = NULL;

}

int cit_bigint_get_sign (pcit_bigint_t d1)
{
    register unsigned char *q1 = d1->start_digits;
    register unsigned char *q1_end = d1->end_digits;

    while (q1 <= q1_end ) {
        if (LOW_NIBBLE(*q1) != 0) {
            if (d1->negate) {
                return -1;
            } else {
                return 1;
            }

        }
        q1++;
    }
    return 0;
}

/*CIT_END_ENTERPRISE*/


/*
 * Copyright (C) 2001-2007 Keisuke Nishida
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

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "common.h"
#include "coblocal.h"
#include "move.h"
#include "numeric.h"
#include "byteswap.h"
#include "a2e.h"
#include "cob_decimal.h"

#define COB_LIB_INCLUDE
#include "codegen.h"

#define DECIMAL_NAN     -128
#define DECIMAL_CHECK(d1,d2) \
  if (unlikely(d1->scale == DECIMAL_NAN || d2->scale == DECIMAL_NAN)) { \
      d1->scale = DECIMAL_NAN; \
      return; \
    }

#define DECIMAL_CHECK_GMP(d1,d2) \
  if (unlikely(d1->scale == DECIMAL_NAN || d2->scale == DECIMAL_NAN)) { \
      d1->scale = DECIMAL_NAN; \
      return; \
    } 


static const unsigned char packed_bytes[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99
};
static const unsigned char packed_bytes_sign_positive[] = {
    0x0c, 0x1c, 0x2c, 0x3c, 0x4c, 0x5c, 0x6c, 0x7c, 0x8c, 0x9c
};
static const unsigned char packed_bytes_sign_negative[] = {
    0x0d, 0x1d, 0x2d, 0x3d, 0x4d, 0x5d, 0x6d, 0x7d, 0x8d, 0x9d
};
static const unsigned char packed_bytes_sign_none[] = {
    0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f, 0x8f, 0x9f
};

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

#if GMP_NAIL_BITS != 0
    #error NAILS not supported
#endif

#define COB_MAX_LL      9223372036854775807LL

static inline void
mpz_set_ull (mpz_ptr dest, const unsigned long long val)
{
    size_t                  size;

    size = (val != 0);
    dest->_mp_d[0] = val & GMP_NUMB_MASK;
#if     GMP_LIMB_BITS < 64
    if ( val > GMP_NUMB_MAX ) {
        dest->_mp_d[1] = val >> GMP_NUMB_BITS;
        size = 2;
    }
#endif
    dest->_mp_size = size;
}

static COB_INLINE void
mpz_set_sll (mpz_ptr dest, const signed long long val)
{
    register size_t                  size;
    register unsigned long long      vtmp;

    vtmp = (unsigned long long)(val >= 0 ? val : -val);
    size = (vtmp != 0);
    dest->_mp_d[0] = vtmp & GMP_NUMB_MASK;
#if     GMP_LIMB_BITS < 64
    if ( vtmp > GMP_NUMB_MAX ) {
        dest->_mp_d[1] = vtmp >> GMP_NUMB_BITS;
        size = 2;
    }
#endif
    dest->_mp_size = (val >= 0) ? size : -size;
}

/*
static unsigned int
mpz_byte_size (const mpz_ptr src){
    return mpz_size (src) * (GMP_LIMB_BITS / 8);
}
*/

static COB_INLINE unsigned long long
mpz_get_ull (const mpz_ptr src)
{
    size_t                  size;

    size = mpz_size (src);
    if ( !size ) {
        return 0;
    }
#if     GMP_LIMB_BITS > 32
    return(unsigned long long)src->_mp_d[0];
#else
    if ( size < 2 ) {
        return(unsigned long long)src->_mp_d[0];
    }
    return(unsigned long long)src->_mp_d[0] |
        ((unsigned long long)src->_mp_d[1] << GMP_NUMB_BITS);
#endif
}

static COB_INLINE signed long long
mpz_get_sll (const mpz_ptr src)
{
    int                     size;
    unsigned long long      vtmp;

    size = src->_mp_size;
    if ( !size ) {
        return 0;
    }
    vtmp = (unsigned long long)src->_mp_d[0];
#if     GMP_LIMB_BITS < 64
    if ( mpz_size (src) > 1 ) {
        vtmp |= (unsigned long long)src->_mp_d[1] << GMP_NUMB_BITS;
    }
#endif
    if ( size > 0 ) {
        return(signed long long) vtmp & COB_MAX_LL;
    }
    return ~(((signed long long) vtmp - 1LL) & COB_MAX_LL);
}

static COB_INLINE void
num_byte_memcpy (unsigned char *s1, const unsigned char *s2, size_t size)
{
    do {
        *s1++ = *s2++;
    } while ( --size );
}

static long long
cob_binary_get_int64 (const cob_field * const f)
{
    long long       n = 0;
    size_t          fsiz = 8 - f->size;

/* Experimental code - not activated */
#if 0
    unsigned char   *s;

    if ( (COB_FIELD_BINARY_SWAP (f) && !COB_FIELD_HAVE_SIGN (f)) ||
         (!COB_FIELD_BINARY_SWAP (f) && COB_FIELD_HAVE_SIGN (f)) ) {
        s = (unsigned char *)&n + fsiz;
    } else {
        s = (unsigned char *)&n;
    }
    num_byte_memcpy (s, f->data, f->size);
    if ( COB_FIELD_BINARY_SWAP (f) ) {
        n = COB_BSWAP_64 (n);
    }
    if ( COB_FIELD_HAVE_SIGN (f) ) {
        n >>= 8 * fsiz; /* shift with sign */
    }
#endif
#ifndef WORDS_BIGENDIAN
    if ( COB_FIELD_BINARY_SWAP (f) ) {
        if ( COB_FIELD_HAVE_SIGN (f) ) {
            num_byte_memcpy ((unsigned char *)&n, f->data, f->size);
            n = COB_BSWAP_64 (n);
            n >>= 8 * fsiz; /* shift with sign */
        } else {
            num_byte_memcpy (((unsigned char *)&n) + fsiz, f->data, f->size);
            n = COB_BSWAP_64 (n);
        }
    } else {
        if ( COB_FIELD_HAVE_SIGN (f) ) {
            num_byte_memcpy (((unsigned char *)&n) + fsiz, f->data, f->size);
            n >>= 8 * fsiz; /* shift with sign */
        } else {
            num_byte_memcpy ((unsigned char *)&n, f->data, f->size);
        }
    }
#else   /* WORDS_BIGENDIAN */
    if ( COB_FIELD_HAVE_SIGN (f) ) {
        num_byte_memcpy ((unsigned char *)&n, f->data, f->size);
        n >>= 8 * fsiz; /* shift with sign */
    } else {
        num_byte_memcpy (((unsigned char *)&n) + fsiz, f->data, f->size);
    }
#endif  /* WORDS_BIGENDIAN */
    return n;
}

static unsigned long long
cob_binary_get_uint64 (const cob_field * const f)
{
    unsigned long long      n = 0;
    size_t                  fsiz = 8 - f->size;

#ifndef WORDS_BIGENDIAN
    if ( COB_FIELD_BINARY_SWAP (f) ) {
        num_byte_memcpy (((unsigned char *)&n) + fsiz, f->data, f->size);
        n = COB_BSWAP_64 (n);
    } else {
        num_byte_memcpy ((unsigned char *)&n, f->data, f->size);
    }
#else   /* WORDS_BIGENDIAN */
    num_byte_memcpy (((unsigned char *)&n) + fsiz, f->data, f->size);
#endif  /* WORDS_BIGENDIAN */
    return n;
}

static void
cob_binary_set_uint64 (cob_field * const f, unsigned long long n)
{
#ifndef WORDS_BIGENDIAN
    unsigned char   *s;

    if ( COB_FIELD_BINARY_SWAP (f) ) {
        n = COB_BSWAP_64 (n);
        s = ((unsigned char *)&n) + 8 - f->size;
    } else {
        s = (unsigned char *)&n;
    }
    num_byte_memcpy (f->data, s, f->size);
#else   /* WORDS_BIGENDIAN */
    num_byte_memcpy (f->data, ((unsigned char *)&n) + 8 - f->size, f->size);
#endif  /* WORDS_BIGENDIAN */
}

static void
cob_binary_set_int64 (cob_field * const f, long long n)
{
#ifndef WORDS_BIGENDIAN
    {
    unsigned char   *s;

    if ( COB_FIELD_BINARY_SWAP (f) ) {
        n = COB_BSWAP_64 (n);
        s = ((unsigned char *)&n) + 8 - f->size;
    } else {
        s = (unsigned char *)&n;
    }
    num_byte_memcpy (f->data, s, f->size);
    }
#else   /* WORDS_BIGENDIAN */
    num_byte_memcpy (f->data, ((unsigned char *)&n) + 8 - f->size, f->size);
#endif  /* WORDS_BIGENDIAN */
}

/*
 * Decimal number
 */


#define FAST_MUL_10(a) ((a << 3) + (a << 1))
/*#define FAST_MUL_10(a) (a*10)*/
static void 
cob_decimal_set_string_gmp(cob_decimal *const d, int sz, unsigned char * datain, int copy) 
{
    char buff[COB_MINI_BUFF];
    register unsigned char *data = datain;
    register int size = sz;
    /*CIT*/
    /* skip leading zeros */
    while ( size  && ((*data & 0x0F) == 0) ) {
        size--;
        data++;
    }
    if (size) {
        /* set value */

        if (size < 10 ) {
            register unsigned int    n;
            n = 0;
            while ( size-- ) {
                n = FAST_MUL_10(n) + COB_D2I (*data); /*!! cob_d2i is MACRO */
                data++;
            }
            mpz_set_ui ((d->v_mpz), n);
        } else if (size < 19 ) {
            register unsigned long long    nl;
            nl = 0;
            while ( size-- ) {
                nl = FAST_MUL_10(nl) + COB_D2I (*data); 
                data++;
            }
            mpz_set_ull ((d->v_mpz), nl);
        } else {
            if (copy ) {
                memcpy (buff, data, size);
                buff[size] = 0;
                mpz_set_str ((d->v_mpz), (char *)buff, 10);
            } else {
                mpz_set_str ((d->v_mpz), (char *)data, 10);
            }
        }
        /* set sign and scale */
    } else {
        mpz_set_ui ((d->v_mpz), 0);        
    }
}


void
cob_decimal_init (cob_decimal * const d)
{
    cob_decimal_rw *d_rw = (cob_decimal_rw*)d;
    mpz_init2 ((d->v_mpz), 64);
    d_rw->scale = 0;
}

void
cob_decimal_clear (cob_decimal * const d)
{
    cob_decimal_rw *d_rw = (cob_decimal_rw*)d;
    mpz_clear ((d_rw->v_mpz));
    d_rw->scale = 0;
}


static void 
get_ui_pow_ui(COB_RTD, int n)
{
    if ( n >= COB_MAX_NUMBER_DIGIT ) {
        mpz_ui_pow_ui (rtd->cob_mexp, 10, n);
    } else {
        mpz_set (rtd->cob_mexp, rtd->cob_mpze10[n]);
    }
}

/* d->value *= 10^n, d->scale += n */
static void inline 
cob_shift_decimal_ (COB_RTD, cob_decimal * const d, const int n)
{
    if ( n == 0 ) {
        return;
    }
    /*debug*/
    /*gmp_printf ("Entry cob_shift_decimal_ %Zd, %d\n",  (d->v_mpz), n); */
    if ( n > 0 ) {
        get_ui_pow_ui(rtd,n);
        mpz_mul ((d->v_mpz), (d->v_mpz), rtd->cob_mexp);
    } else {
        get_ui_pow_ui(rtd,-n);
        mpz_tdiv_q ((d->v_mpz), (d->v_mpz), rtd->cob_mexp);
    }
    /*gmp_printf ("Exit cob_shift_decimal_ %Zd, %Zd\n",  (d->v_mpz), rtd->cob_mexp); */
    d->scale += n;
}

void 
cob_shift_decimal (COB_RTD, cob_decimal * const d, const int n) {
    cob_shift_decimal_(rtd,d,n);
}

/* truncate scale */
void
cob_decimal_trunc (COB_RTD, cob_decimal * const d, const int n)
{
    if ( d->scale  <= n ) {
        return;
    }
    cob_shift_decimal_(rtd,d, -(d->scale-n) );
}

/* d->value *= 10^n*/
void
cob_shift_decimal_no_scale (COB_RTD, cob_decimal * const d, const int n)
{
    int save_scale;
    if ( n == 0 ) {
        return;
    }
    save_scale = d->scale;
    cob_shift_decimal_(rtd,d,n);
    d->scale = save_scale;
}

static inline void
align_decimal (COB_RTD, cob_decimal *const d1, cob_decimal *const d2)
{
    if ( d1->scale < d2->scale ) {
        cob_shift_decimal_ (rtd, d1, d2->scale - d1->scale);
    } else if ( d1->scale > d2->scale ) {
        cob_shift_decimal_ (rtd, d2, d1->scale - d2->scale);
    }
}

/*
 * Decimal set/get
 */
static void 
cob_decimal_round_to_scale(COB_RTD, cob_decimal * const d, int scale)
{
    if (d->scale >= scale) {
        int sign = mpz_sgn (d->v_mpz);
        if ( sign != 0 ) {
            if (d->scale > scale) {
                cob_shift_decimal_ (rtd, d, scale - d->scale );
            }
            if ( sign > 0 ) {
                mpz_add_ui ((d->v_mpz), (d->v_mpz), 5);
            } else {
                mpz_sub_ui ((d->v_mpz), (d->v_mpz), 5);
            }
        }
    }
}

static void
cob_decimal_set_mpf (cob_decimal * const d,  mpf_t vf)
{
    mp_exp_t exp;
    char *c;
    COB_RTD = cob_get_rtd();

    c = mpf_get_str(NULL, &exp, 10, 0, vf);
    if (strlen(c) > 0) {
        mpz_set_str((d->v_mpz), c, 10);
        if (*c == '-') {
            d->scale = strlen(c+1) - exp;            
        } else {
            d->scale = strlen(c) - exp;
        }
    } else {
        mpz_set_si((d->v_mpz), 0);
        d->scale = 0;
    }
    if (d->scale>18) {
        cob_decimal_round_to_scale(rtd, d, 19);
        cob_shift_decimal_(rtd, d, -1);
    }
    free(c);
}

void
cob_decimal_set (cob_decimal * const d1, cob_decimal * const d2)
{
    mpz_set ((d1->v_mpz), (d2->v_mpz));
    d1->scale = d2->scale;
}

void cob_decimal_set_cob_float (cob_decimal *d, cob_float *v)
{
    cob_decimal_set_mpf(d,v->v_mpf);
}
/* double */

void
cob_decimal_set_double (cob_decimal * const d, const double v)
{
    char buffer[100];
    char buffer2[100];
    int l, i, j, scale =0;

    sprintf(buffer,"%0.17lf", v);
    l = strlen(buffer);
    for (i = 0, j=0; i < l; i++) {
        if (buffer[i] != CHAR_COMMA && buffer[i] != CHAR_DOT) {
            buffer2[j] = buffer[i];
            j++;
            if (scale ) {
                scale ++;
            }
        } else {
            scale =1;
        }
    }
    while ( scale > 1 ) {
        if ( buffer2[j-1] == CHAR_0) {
            j--;
            scale --;
        } else {
            break;
        }
    }
    buffer2[j] = 0;
    mpz_set_str((d->v_mpz), buffer2, 10);
    d->scale =scale ? scale -1 : 0 ;
    if (d->scale>18) {
        COB_RTD = cob_get_rtd();
        cob_decimal_round_to_scale(rtd, d, 19);
        cob_shift_decimal_(rtd, d, -1);
    }

}

void
cob_decimal_set_float (cob_decimal * const d, const float v)
{
    cob_decimal_set_double(d,(double)v);
    if ((d->scale) > 0) {
        cob_shift_decimal_(cob_get_rtd(), d, 9-(d->scale));
    }
}

double
cob_decimal_get_double (cob_decimal * const d)
{
    int     n = d->scale;
    int i;
    double  v;

    v= mpz_get_d ((d->v_mpz));

    if (n > 0) {
        while (n > 0) {
            i = min(18,n);
            v /= cob_exp10_double[i];
            n -=i;
        }
    } else if (n < 0) {
        while (n < 0) {
            i = min(18,-n);
            v *= cob_exp10_double[i];
            n +=i;
        }
    }
    return v;
}

/* DISPLAY */

static inline void
cob_decimal_set_display (COB_RTD, cob_decimal *const d, cob_field * f)
{
    int             sign;
    int             size = COB_FIELD_SIZE (f);
    unsigned char   * data = COB_FIELD_DATA (f);
    unsigned char c = *data;

    if ( unlikely(c == 255) ) {
        mpz_ui_pow_ui ((d->v_mpz), 10, size);
        d->scale = COB_FIELD_SCALE(f);
        return;
    }

    if ((c == 0) && !(rtd->current_module->runtime_flags & COB_FLAG_RT_MF_HOSTNUMCOMPARE_1)) {
        mpz_ui_pow_ui ((d->v_mpz), 10, size);
        mpz_neg ((d->v_mpz), (d->v_mpz));
        d->scale = COB_FIELD_SCALE(f);
        return;
    }

    {
        if ((unlikely(rtd->current_module->display_sign) || rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) && 
            !rtd->ebcdic_charset) {
            sign = COB_DUP_GET_SIGN (f);
            data = COB_FIELD_DATA (f);
            size = COB_FIELD_SIZE (f); 
        } else {
            sign = cob_display_get_sign(rtd,f,&data,&size);
        }
        cob_decimal_set_string_gmp(d, size, data, 1);
        if ( sign < 0 ) {
            mpz_neg ((d->v_mpz), (d->v_mpz));
        }
    }
    d->scale = COB_FIELD_SCALE(f); 
    /*CIT*/
}

static inline int
cob_decimal_get_display (COB_RTD, cob_decimal *const d, cob_field *const f, const int opt)
{
    int             diff;
    int             sign ;
    int             size;
    unsigned char   * const data = COB_FIELD_DATA (f);
    char            buff[COB_MINI_BUFF];

    /* build string */
    sign = mpz_sgn ((d->v_mpz));
    mpz_abs ((d->v_mpz), (d->v_mpz));
    if ((d->v_mpz)->_mp_size > 8) {
        if (mpz_sizeinbase((d->v_mpz), 10) > COB_MINI_BUFF -2) {
            cob_set_exception (rtd, COB_EC_SIZE_OVERFLOW);
            return rtd->cob_exception_code;
        }
    }
    if (rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION && (d->v_mpz)->_mp_size <=1 ) {  
        unsigned long long l =  mpz_get_ull((d->v_mpz));     
        size = u64toa_branchlut(l, (char*)buff);
    } else {
        mpz_get_str((char *)buff, 10, (d->v_mpz));
        size = strlen ((char *)buff);
    }
#ifdef CIT_EBCDIC_CHARSET
    (void)STRING_A2E(buff,size);
#endif

    /* store number */
    diff = (int)(COB_FIELD_SIZE (f) - size);
    if ( unlikely(diff < 0) ) {
        /* overflow */
        cob_set_exception (rtd, COB_EC_SIZE_OVERFLOW);

        /* if the statement has ON SIZE ERROR or NOT ON SIZE ERROR,
           then throw an exception */
        if ( opt & COB_STORE_KEEP_ON_OVERFLOW ) {
            return rtd->cob_exception_code;
        }

        /* othersize, truncate digits */
        memcpy (data, buff - diff, COB_FIELD_SIZE (f));
    } else {
        /* no overflow */
        memset (data, CHAR_0, (size_t)diff);
        memcpy (data + diff, buff, size);
    }

    COB_PUT_SIGN (f, sign);
    return 0;
}

/* BINARY */

static void
cob_decimal_set_binary (cob_decimal * const d, cob_field *f)
{
#ifdef  COB_LI_IS_LL
    if ( COB_FIELD_HAVE_SIGN (f) ) {
        mpz_set_si ((d->v_mpz), cob_binary_get_int64 (f));
    } else {
        mpz_set_ui ((d->v_mpz), cob_binary_get_uint64 (f));
    }
#else   
    if ( COB_FIELD_HAVE_SIGN (f) ) {
        mpz_set_sll ((d->v_mpz), cob_binary_get_int64 (f));
    } else {
        mpz_set_ull ((d->v_mpz), cob_binary_get_uint64 (f));
    }
#endif
    d->scale = COB_FIELD_SCALE(f);
}


static int
cob_decimal_get_binary (COB_RTD, cob_decimal * const d, cob_field *f, const int opt)
{
    size_t                  overflow;
    size_t                  digits;
    size_t                  sign;
    size_t                  bitnum;
    int                     compute_ibm = (rtd->current_module && rtd->current_module->runtime_flags & COB_FLAG_RT_COMPUTE_IBM);
    int                     neg = 0;

    if ( unlikely(mpz_size ((d->v_mpz)) == 0) ) {
        memset (f->data, 0, f->size);
        return 0;
    }
    overflow = 0;
    digits = COB_FIELD_DIGITS(f);
    sign = COB_FIELD_HAVE_SIGN (f);
    neg  = mpz_sgn (d->v_mpz);
    if ( neg < 0 ) {
        mpz_abs ((d->v_mpz), (d->v_mpz));
    }
    
    bitnum = (f->size * 8) - sign;
    if ( unlikely(mpz_sizeinbase ((d->v_mpz), 2) > bitnum) ) {
        if ( opt & COB_STORE_KEEP_ON_OVERFLOW ) {
            goto overflow;
        }
        overflow = 1;
    }
    if (compute_ibm) {
        if ((f->attr->flags & COB_FLAG_BINARY_NOTRUNC)) {
            mpz_fdiv_r_2exp ((d->v_mpz), (d->v_mpz), (f->size * 8));
            if (mpz_cmpabs ((d->v_mpz), (rtd->cob_mpze10)[digits]) >= 0 ) {
                /* overflow */
                if ( opt & COB_STORE_KEEP_ON_OVERFLOW ) {
                    goto overflow;
                }
                overflow = 1;
            }
        } else {
            if (mpz_cmpabs ((d->v_mpz), (rtd->cob_mpze10)[digits]) >= 0 ) {
                /* overflow */
                if ( opt & COB_STORE_KEEP_ON_OVERFLOW ) {
                    goto overflow;
                }
                overflow = 1;
            }
            if ( overflow && (opt & COB_STORE_TRUNC_ON_OVERFLOW)) {
                mpz_tdiv_r ((d->v_mpz), (d->v_mpz), (rtd->cob_mpze10)[digits]);
            }
        }

    } else {
        if ( opt && rtd->current_module->flag_binary_truncate && !(f->attr->flags & COB_FLAG_BINARY_NOTRUNC)) {
            if ( mpz_cmpabs ((d->v_mpz), (rtd->cob_mpze10)[digits]) >= 0 ) {
                /* overflow */
                if ( opt & COB_STORE_KEEP_ON_OVERFLOW ) {
                    goto overflow;
                }
                overflow = 1;
            }
        }
        if ( overflow ) {
            /* TRUNC_ON_OVERFLOW is only set for binary_truncate */
            if ( opt & COB_STORE_TRUNC_ON_OVERFLOW ) {
                mpz_tdiv_r ((d->v_mpz), (d->v_mpz), (rtd->cob_mpze10)[digits]);
            } else {
                mpz_fdiv_r_2exp ((d->v_mpz), (d->v_mpz), (f->size * 8));
            }
        }
    }
    if (neg < 0 && mpz_sgn (d->v_mpz) != 0) {
        mpz_neg((d->v_mpz), (d->v_mpz));
    }

#ifdef  COB_LI_IS_LL
    if ( !sign || (!compute_ibm && overflow)) {
        cob_binary_set_uint64 (f, mpz_get_ui ((d->v_mpz)));
    } else {
        cob_binary_set_int64 (f, mpz_get_si ((d->v_mpz)));
    }
#else   
    if ( !sign || (!compute_ibm && overflow)) {
        cob_binary_set_uint64 (f, mpz_get_ull ((d->v_mpz)));
    } else {
        cob_binary_set_int64 (f, mpz_get_sll ((d->v_mpz)));
    }
#endif
    if ( !overflow ) {
        return 0;
    }

    overflow:
    cob_set_exception (rtd, COB_EC_SIZE_OVERFLOW);
    return rtd->cob_exception_code;
}

/* PACKED-DECIMAL */

int
cob_packed_get_sign (const cob_field * const f)
{
    unsigned char *p;
    if ( COB_FIELD_PACKED_SIGN_MISSING(f) )
        return 1;
    p = f->data + (COB_FIELD_DIGITS(f) / 2);
    return((*p & 0x0f) == 0x0d) ? -1 : 1;
}

/* THIS is bugged ...
static void
cob_add_packed (cob_field *f, int val)
{
    int             sign;
    unsigned char   *p;
    int             ndigs;
    unsigned int    msn;
    int             tval;
    int             carry = 0;
    unsigned int    subtr = 0;
    unsigned int    zeroes = 0;
    unsigned int    origdigs;

    ndigs = COB_FIELD_DIGITS(f) - COB_FIELD_SCALE(f);
    if ( ndigs <= 0 ) {
        return;
    }
    sign = COB_FIELD_HAVE_SIGN (f) ? cob_packed_get_sign (f) : 0;
    if (COB_FIELD_PACKED_SIGN_MISSING(f) ){
        msn = (COB_FIELD_SCALE(f) % 2);
    } else {
        msn = 1 - (COB_FIELD_SCALE(f) % 2);
    }

    // * -x +v = -(x - v), -x -v = -(x + v) * /
    if ( sign < 0 ) {
        val = -val;
    }
    if ( val < 0 ) {
        val = -val;
        subtr = 1;
    }
    p = f->data + (ndigs / 2) - (1 - msn);
    origdigs = ndigs;
    while ( ndigs-- ) {
        if ( !msn ) {
            tval = *p & 0x0f;
        } else {
            tval = (*p & 0xf0) >> 4;
        }
        if ( val ) {
            carry += (val % 10);
            val /= 10;
        }
        if ( subtr ) {
            tval -= carry;
            if ( tval < 0 ) {
                tval += 10;
                carry = 1;
            } else {
                carry = 0;
            }
        } else {
            tval += carry;
            if ( tval > 9 ) {
                tval %= 10;
                carry = 1;
            } else {
                carry = 0;
            }
        }
        if ( tval == 0 ) {
            zeroes++;
        }
        if ( !msn ) {
            *p = (*p & 0xf0) | tval;
            msn = 1;
        } else {
            *p = (*p & 0x0f) | (tval << 4);
            msn = 0;
            p--;
        }
    }
    if ( sign ) {
        p = f->data + f->size - 1;
        if ( origdigs == zeroes ) {
            *p = (*p & 0xf0) | 0x0c;
        } else if ( subtr && carry ) {
            sign = -sign;
            if ( sign < 0 ) {
                *p = (*p & 0xf0) | 0x0d;
            } else {
                *p = (*p & 0xf0) | 0x0c;
            }
        }
    }
}
*/

unsigned int 
cob_packed_extract_uint(unsigned char *p, int dg, int comp_6 )
{
    register unsigned int    val = 0;
    register int digits = dg;
    register unsigned char c;

    while ( digits > 1 ) {
        val *= 100;
        c = *p;
        if ( c ) {
            /*val += ((*p >> 4) * 10) + (*p & 0x0f);*/
            val += packed_value[c];
        }
        digits -= 2;
        p++;
    }
    if ( comp_6 ) {
        if (digits > 0) {
            val *= 100;
            c = *p;
            if ( c ) {
                /*val += ((*p >> 4) * 10) + (*p & 0x0f);*/
                val += packed_value[c];
            }
        }
    } else {
        if ( val ) {
            val *= 10;
        }
        val += *p >> 4;
    }
    return val;
}

unsigned long long 
cob_packed_extract_ull(unsigned  char *p, int dg, int comp_6 )
{
    register unsigned long long    val = 0;
    register int digits = dg;
    register unsigned char c;
    while ( digits > 1 ) {
        val *= 100;
        c = *p;
        if ( c ) {
            /*val += ((*p >> 4) * 10) + (*p & 0x0f);*/
            val += packed_value[c];
        }
        digits -= 2;
        p++;
    }
    if ( comp_6 ) {
        if (digits > 0) {
            if ( val ) {
                val *= 100;
            }
            c = *p;
            if ( c ) {
                /*val += ((*p >> 4) * 10) + (*p & 0x0f);*/
                val += packed_value[c];
            }
        }
    } else {
        if ( val ) {
            val *= 10;
        }
        val += *p >> 4;
    }
    return val;
}


static void
cob_decimal_set_packed (cob_decimal * const d, cob_field * const f)
{
    register unsigned char   *p = f->data;
    register int             digits = COB_FIELD_DIGITS(f);
    int             sign;
    unsigned int    valseen;
    sign = COB_FIELD_HAVE_SIGN (f) ? cob_packed_get_sign (f) : 0;
    if (COB_FIELD_PACKED_SIGN_MISSING(f)) {
        if (!(digits & 0x01) ) { /* digits %2 == 0*/
            while ( digits > 0 && *p == 0 ) {
                digits -= 2;
                p++;
            }
        } else {
            if (*p == 0) {
                digits -= 1;
                p++;
                while (digits > 0 && *p == 0 ) {
                    digits -= 2;
                    p++;
                }
            }
        }
    } else {
        if (digits & 0x01 ) { /* digits %2 == 1*/
            while (digits > 0 && *p == 0 ) {
                digits -= 2;
                p++;
            }
        } else {
            if (*p == 0) {
                digits -= 1;
                p++;
                while (digits > 0 && *p == 0 ) {
                    digits -= 2;
                    p++;
                }
            }
        }
    }
    if (digits <= 0) {
        mpz_set_ui ((d->v_mpz), 0);
    } else if ( digits < 9 ) {
        mpz_set_ui ((d->v_mpz), cob_packed_extract_uint(p, digits, COB_FIELD_PACKED_SIGN_MISSING(f)));
    } else if ( digits < 19 ) {
        mpz_set_ull ((d->v_mpz), cob_packed_extract_ull(p, digits, COB_FIELD_PACKED_SIGN_MISSING(f)));
    } else {
        valseen = 0;
        mpz_set_ui ((d->v_mpz), 0);
        while ( digits > 1 ) {
            if ( valseen ) {
                mpz_mul_ui ((d->v_mpz), (d->v_mpz), 100);
            }
            if ( *p ) {
                mpz_add_ui ((d->v_mpz), (d->v_mpz), packed_value[*p]);
                valseen = 1;
            }
            digits -= 2;
            p++;
        }
        if (COB_FIELD_PACKED_SIGN_MISSING(f)) {
            if (digits > 0) {
                if ( valseen ) {
                    mpz_mul_ui ((d->v_mpz), (d->v_mpz), 100);
                }
                if ( *p ) {
                    mpz_add_ui ((d->v_mpz), (d->v_mpz), packed_value[*p]);
                }
            }
        } else {
            if ( valseen ) {
                mpz_mul_ui ((d->v_mpz), (d->v_mpz), 10);
            }
            mpz_add_ui ((d->v_mpz), (d->v_mpz), (*p >> 4));
        }
    }

    if ( sign < 0 ) {
        mpz_neg ((d->v_mpz), (d->v_mpz));
    }

    d->scale = COB_FIELD_SCALE(f);
}

static int
cob_decimal_get_packed (COB_RTD, cob_decimal * const d, cob_field * const f, const int opt)
{
    int             diff;
    int             sign;
    size_t          size, sz;

    size_t          i;
    int             digits = COB_FIELD_DIGITS(f);
    unsigned char   *data = f->data;
    unsigned char   *p;
    unsigned char   *q;
    unsigned char   x;
    char            buff[COB_MINI_BUFF];


    /* build string */
    sign = mpz_sgn ((d->v_mpz));
    mpz_abs ((d->v_mpz), (d->v_mpz));
    if ((d->v_mpz)->_mp_size > 8) {
        if (mpz_sizeinbase((d->v_mpz), 10) > COB_MINI_BUFF -2) {
            cob_set_exception (rtd, COB_EC_SIZE_OVERFLOW);
            return rtd->cob_exception_code;
        }
    }
    if (rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION && (d->v_mpz)->_mp_size <=1 ) {        
        unsigned long long l = mpz_get_ull((d->v_mpz));
        size = u64toa_branchlut(l, (char*)buff);
    } else {
        mpz_get_str ((char *)buff, 10, (d->v_mpz));
        size = strlen ((char *)buff);
    }
    q = (unsigned char *)buff;

    /* store number */
    diff = (int)(digits - size);
    if ( diff < 0 ) {
        /* overflow */
        cob_set_exception (rtd, COB_EC_SIZE_OVERFLOW);

        /* if the statement has ON SIZE ERROR or NOT ON SIZE ERROR,
           then throw an exception */
        if ( opt & COB_STORE_KEEP_ON_OVERFLOW ) {
            return rtd->cob_exception_code;
        }
        q += size - digits;
        size = digits;
        diff = 0;
    }
    memset (data, 0, f->size);
    if ( COB_FIELD_PACKED_SIGN_MISSING(f) ) {
        p = data + ((digits-1) >>1) /*((digits-1) / 2)*/ - ((size-1) / 2);
        diff = (int)(size & 0x01) /*(int)(size % 2)*/;
    } else {
        p = data + ((digits) >>1) /*((digits) / 2)*/ - (size / 2);
        diff = 1 - (int)(size & 0x01) /*(int)(size % 2)*/;
    }
    sz = size + diff;
    for (i=diff ;i < sz; i++, q++ ) {
        x = COB_D2I (*q);
        if (x) {
            if ( (int)(i & 0x01) /*(int)(i % 2)*/ == 0 ) {
                *p = x << 4;
            } else {
                *p++ |= x;
            }
        } else {
            if ( (int)(i & 0x01) /*(int)(i % 2)*/ != 0 ) p++;
        }
    }
    p = data + (digits / 2);
    if ( !COB_FIELD_PACKED_SIGN_MISSING(f) ) {
        if ( !COB_FIELD_HAVE_SIGN (f) ) {
            *p = (*p & 0xf0) | 0x0f;
        } else {
            if ( sign < 0 ) {
                *p = (*p & 0xf0) | 0x0d;
            } else {
                *p = (*p & 0xf0) | 0x0c;
            }
        }
    }

    return 0;
}

void
cob_set_packed_zero (cob_field * const f)
{
    memset (f->data, 0, f->size);
    if ( !COB_FIELD_PACKED_SIGN_MISSING(f) ) {
        if ( !COB_FIELD_HAVE_SIGN (f) ) {
            *(f->data + f->size - 1) |= 0x0f;
        } else {
            *(f->data + f->size - 1) |= 0x0c;
        }
    }
}

void
cob_set_packed_int (cob_field * const f, const int val)
{
    unsigned char   *p;
    size_t          sign = 0;
    int             n;

    if ( val < 0 ) {
        n = -val;
        sign = 1;
    } else {
        n = val;
    }
    memset (f->data, 0, f->size);
    p = f->data + f->size - 1;
    if ( !COB_FIELD_PACKED_SIGN_MISSING(f) ) {

        *p = (n % 10) << 4;
        if ( !COB_FIELD_HAVE_SIGN (f) ) {
            *p |= 0x0f;
        } else if ( sign ) {
            *p |= 0x0d;
        } else {
            *p |= 0x0c;
        } 
        /*
        if ( !COB_FIELD_HAVE_SIGN (f) ) {
            *p = packed_bytes_sign_none[n%10] ;
        } else if ( sign ) {
            *p = packed_bytes_sign_negative[n%10] ;
        } else {
            *p = packed_bytes_sign_positive[n%10] ;
        } 
         */
        n /= 10;
        p--;
    }
    if ( n ) {
        for ( ; n && p >= f->data; n /= 100, p-- ) {
            *p = packed_bytes[n % 100];
        }
        if ( COB_FIELD_PACKED_SIGN_MISSING(f) ) {
            if ( (COB_FIELD_DIGITS(f) % 2) == 1 ) {
                *(f->data) &= 0x0f;
            }
        } else {
            if ( (COB_FIELD_DIGITS(f) % 2) == 0 ) {
                *(f->data) &= 0x0f;
            }
        }
    }
    /* return n;*/
}

void
cob_set_packed_long_long (cob_field * const f, const long long val)
{
    unsigned char   *p;
    size_t          sign = 0;
    long long       n;

    if ( val < 0 ) {
        n = -val;
        sign = 1;
    } else {
        n = val;
    }
    memset (f->data, 0, f->size);
    p = f->data + f->size - 1;
    if ( !COB_FIELD_PACKED_SIGN_MISSING(f) ) {
        *p = (n % 10) << 4;
        if ( !COB_FIELD_HAVE_SIGN (f) ) {
            *p |= 0x0f;
        } else if ( sign ) {
            *p |= 0x0d;
        } else {
            *p |= 0x0c;
        }
        n /= 10;
        p--;
    }
    for ( ; n && p >= f->data; n /= 100, p-- ) {
        *p = packed_bytes[n % 100];
    }
    if ( COB_FIELD_PACKED_SIGN_MISSING(f) ) {
        if ( (COB_FIELD_DIGITS(f) % 2) == 1 ) {
            *(f->data) &= 0x0f;
        }
    } else {
        if ( (COB_FIELD_DIGITS(f) % 2) == 0 ) {
            *(f->data) &= 0x0f;
        }
    }
    /*return n;*/
}

/* General field */

void
cob_decimal_set_field (COB_RTD, cob_decimal * const d, cob_field * const f)
{
    float   fval;
    double  dval;
    unsigned long long l;

    switch ( COB_FIELD_TYPE (f) ) {
        case COB_TYPE_NUMERIC_BINARY:
        case COB_TYPE_NUMERIC_BITS:
            cob_decimal_set_binary (d, f);
            break;
        case COB_TYPE_NUMERIC_PACKED:
            cob_decimal_set_packed (d, f);
            break;
        case COB_TYPE_NUMERIC_FLOAT:
            memcpy ((ucharptr)&fval, f->data, sizeof(float));
            cob_decimal_set_float (d, fval);
            break;
        case COB_TYPE_NUMERIC_DOUBLE:
            memcpy ((ucharptr)&dval, f->data, sizeof(double));
            cob_decimal_set_double (d, dval);
            break;
/*CIT_BEGIN_ENTERPRISE*/
        case COB_TYPE_ALPHANUMERIC_BITS:
        case COB_TYPE_BITS:
        case COB_TYPE_BITS_ARRAY:
            l = cob_get_ull(rtd,f);
            cob_decimal_set_ull(d,l);
            break;
/*CIT_END_ENTERPRISE*/
        default:
            cob_decimal_set_display (rtd, d, f);
            break;
    }
}

void
cob_decimal_set_field_1 (COB_RTD, cob_decimal * const d, cob_field * const f, int gmp_flags )
{
    cob_decimal_set_field(rtd,d,f);
}


int
cob_decimal_get_field_1 (COB_RTD, cob_decimal * d, cob_field * const f, const int opt, int copy_data)
{
    cob_field       temp;
    cob_field_attr  attr;
    double          val;
    float           fval;
    unsigned long long l;
    unsigned char   data[64];
    int             fixshift = 0;
    int             noround =0;

    if ( unlikely(d->scale == DECIMAL_NAN) ) {
        if ( !rtd->cob_exception_code ) {
            cob_set_exception (rtd, COB_EC_SIZE_OVERFLOW);
        }
        return rtd->cob_exception_code;
    }

/*CIT_BEGIN_ENTERPRISE*/
    switch (COB_FIELD_TYPE (f)) {
        case COB_TYPE_BITS:
        case COB_TYPE_BITS_ARRAY:
        case COB_TYPE_ALPHANUMERIC_BITS:
            l = cob_decimal_get_ull(d);
            cob_set_ull(rtd,f,l);
            return 0;
        case COB_TYPE_NUMERIC_FLOAT:
            fixshift = 8; 
            noround=1;
            break;
        case COB_TYPE_NUMERIC_DOUBLE:
            fixshift = 17;
            noround=1;
            break;
        default:
            break;
    }
    /* if ( copy_data && (((COB_FIELD_SCALE(f) - d->scale) != 0) || (opt & COB_STORE_ROUND))) */
    /* Because the Decimal optimizer , any decimal is suposed to be reused ... so always copy*/
    {
        /* work copy */
        if ( d != &(rtd->cob_d1) ) {
            cob_decimal_set (&(rtd->cob_d1), d);
            d = &(rtd->cob_d1);
        }
    }
    {
        /* rounding */
        if (!noround &&  opt & COB_STORE_ROUND ) {
            /*
            if ( opt & COB_STORE_ROUND_FP ) {
                if ( COB_FIELD_SCALE(f) < d->scale-1 ) {
                    cob_decimal_round_to_scale(rtd, d, COB_FIELD_SCALE(f) + 2);
                }
            } 
            */ 
            if ( COB_FIELD_SCALE(f) < d->scale ) {
                cob_decimal_round_to_scale(rtd, d, COB_FIELD_SCALE(f) + 1);
            }
        }

        /* append or truncate decimal digits */
        if (!fixshift) {
            cob_shift_decimal_ (rtd, d, COB_FIELD_SCALE(f) - d->scale);
        } else {
            if (fixshift < d->scale) {
                cob_shift_decimal_ (rtd, d, fixshift - d->scale);
            }
        }

        /* store number */
        switch ( COB_FIELD_TYPE (f) ) {
            case COB_TYPE_NUMERIC_BINARY:
            case COB_TYPE_NUMERIC_BITS:
                return cob_decimal_get_binary (rtd, d, f, opt);
            case COB_TYPE_NUMERIC_PACKED:
                return cob_decimal_get_packed (rtd, d, f, opt);
            case COB_TYPE_NUMERIC_DISPLAY:
                return cob_decimal_get_display (rtd, d, f, opt);
            case COB_TYPE_NUMERIC_FLOAT:
                fval = (float) cob_decimal_get_double ( d);
                memcpy (f->data, (ucharptr)&fval, sizeof (float));
                return 0;
            case COB_TYPE_NUMERIC_DOUBLE:
                val = cob_decimal_get_double ( d);
                memcpy (f->data, (ucharptr)&val, sizeof (double));
                return 0;
            default:
                COB_ATTR_INIT (COB_TYPE_NUMERIC_DISPLAY, COB_FIELD_DIGITS(f),
                               COB_FIELD_SCALE(f), COB_FLAG_HAVE_SIGN, NULL);
                temp.size = COB_FIELD_DIGITS(f);
                temp.data = data;
                temp.attr = &attr;
                if ( cob_decimal_get_display (rtd, d, &temp, opt) == 0 ) {
                    cob_move (rtd, &temp, f);
                }
                return rtd->cob_exception_code;
        }
    }
}


int
cob_decimal_get_field (COB_RTD, cob_decimal * const d, cob_field * const f, const int opt)
{
    return cob_decimal_get_field_1(rtd,d,f,opt,1);
}

/*
 * Decimal arithmetic
 */
static inline void 
cob_decimal_sub_common(COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2){

    /*gmp_printf ("Entry cob_decimal_sub_common %Zd, %Zd\n",  (d1->v_mpz), (d2->v_mpz)); */
    align_decimal (rtd, d1, d2);
    /*gmp_printf ("Align cob_decimal_sub_common %Zd, %Zd\n",  (d1->v_mpz), (d2->v_mpz)); */
    DECIMAL_CHECK_GMP (d1, d2);
    if (dr) {
        dr->scale = d1->scale;
        mpz_sub ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
    } else {
        mpz_sub ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));

    }
    /*gmp_printf ("Exit cob_decimal_sub_common %Zd, %Zd\n",  (d1->v_mpz), (d2->v_mpz)); */
}
static inline void
cob_decimal_add_common (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    align_decimal (rtd, d1, d2);
    DECIMAL_CHECK_GMP (d1, d2);
    if (dr) {
        dr->scale = d1->scale;
        mpz_add ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
    } else {
        mpz_add ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
    }
}
static void
cob_decimal_mul_common (cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    DECIMAL_CHECK_GMP (d1, d2);
    if (dr) {
        dr->scale = d1->scale +d2->scale;
        mpz_mul ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
    } else {
        d1->scale += d2->scale;
        mpz_mul ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
    }
}

#define COB_DIV_MAX_SCALE 37
static void inline 
cob_decimal_div_scale(COB_RTD, cob_decimal * const d1)
{
    if ((d1->scale < 0)) {
        cob_shift_decimal_ (rtd, d1, COB_DIV_MAX_SCALE + -d1->scale);
    } else {
        if ((d1->scale < COB_DIV_MAX_SCALE)) {
            cob_shift_decimal_ (rtd, d1, COB_DIV_MAX_SCALE);
        }
    }

}


static void
cob_decimal_div_common (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    DECIMAL_CHECK_GMP (d1, d2);

    /* check for division by zero */
    if ( unlikely(mpz_sgn ((d2->v_mpz)) == 0) ) {
        if (dr) {
            dr->scale = DECIMAL_NAN;
        } else
            d1->scale = DECIMAL_NAN;
        cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE);
        return;
    }


    if (dr) {
        /*cob_shift_decimal_ (rtd, dr, 37 + ((dr->scale < 0) ? -dr->scale : 0));*/
        cob_decimal_set(dr, d1);
        dr->scale -= d2->scale;
        cob_decimal_div_scale(rtd,dr);
        mpz_tdiv_q ((dr->v_mpz), (dr->v_mpz), (d2->v_mpz));

    } else {
        d1->scale -= d2->scale;
        if (d1->scale < 0)
        {
        }
        /*cob_shift_decimal_ (rtd, d1, COB_DIV_MAX_SCALE + ((d1->scale < 0) ? -d1->scale : 0));*/
        cob_decimal_div_scale(rtd, d1);
        mpz_tdiv_q ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
    }
}
static void
cob_decimal_div_ibm_common (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2,  int maxscale)
{
    int scale = maxscale;
    DECIMAL_CHECK_GMP (d1, d2);

    if ( unlikely(mpz_sgn ((d2->v_mpz)) == 0) ) {
        if (dr) {
            dr->scale = DECIMAL_NAN;
        }

        else
            d1->scale = DECIMAL_NAN;
        cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE);
        return;
    }
    if (d1->scale >= 0) {
        if (d1->scale > maxscale) {
            maxscale = d1->scale;
        }
        scale = d2->scale - d1->scale;
        if (scale < maxscale) {
            scale = maxscale;
        }
    }

    if (scale > COB_DIV_MAX_SCALE) {
        scale = COB_DIV_MAX_SCALE;
    }

    if (dr) {
        cob_decimal_set(dr, d1);
        dr->scale -= d2->scale;
        cob_shift_decimal_ (rtd, dr, scale + ((dr->scale < 0) ? -dr->scale : 0));
        mpz_tdiv_q ((dr->v_mpz), (dr->v_mpz), (d2->v_mpz));
        if (dr->scale > maxscale) {
            cob_shift_decimal_ (rtd, dr, -(dr->scale - maxscale));
        }
    } else {
        d1->scale -= d2->scale;
        cob_shift_decimal_ (rtd, d1, scale + ((d1->scale < 0) ? -d1->scale : 0));

        mpz_tdiv_q ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
        if (d1->scale > maxscale) {
            cob_shift_decimal_ (rtd, d1, -(d1->scale - maxscale));
        }
    }
}

static void
cob_decimal_mod_commmon (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    DECIMAL_CHECK_GMP (d1, d2);

    /* check for division by zero */
    if ( unlikely(mpz_sgn ((d2->v_mpz)) == 0) ) {
        d1->scale = DECIMAL_NAN;
        cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE);
        return;
    }

    if (dr) {
        dr->scale = d1->scale;
        dr->scale -= d2->scale;
        mpz_tdiv_r ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
    } else {
        d1->scale -= d2->scale;
        mpz_tdiv_r ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
    }
}


static void
cob_decimal_pow_common (cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    unsigned int    n;

    DECIMAL_CHECK_GMP (d1, d2);

    if ( d2->scale == 0 && mpz_fits_ulong_p ((d2->v_mpz)) ) {
        n = mpz_get_ui ((d2->v_mpz));
        if (dr) {
            dr->scale = d1->scale;
            mpz_pow_ui ((dr->v_mpz), (d1->v_mpz), n);
            dr->scale *=n;
        } else {
            mpz_pow_ui ((d1->v_mpz), (d1->v_mpz), n);
            d1->scale *= n;
        }

    } else {
        if (dr) {
            dr->scale = d1->scale;
            cob_decimal_set_double (dr, pow (cob_decimal_get_double (d1),
                                             cob_decimal_get_double (d2)));
        } else
            cob_decimal_set_double (d1, pow (cob_decimal_get_double (d1),
                                             cob_decimal_get_double (d2)));
    }
}

void
cob_decimal_and (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    DECIMAL_CHECK_GMP (d1, d2);
    align_decimal (rtd, d1, d2);
    mpz_and ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}
void
cob_decimal_or (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    DECIMAL_CHECK_GMP (d1, d2);
    align_decimal (rtd, d1, d2);
    mpz_ior ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void
cob_decimal_xor (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    DECIMAL_CHECK_GMP (d1, d2);
    align_decimal (rtd, d1, d2);
    mpz_xor ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void
cob_decimal_not (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    DECIMAL_CHECK_GMP (d1, d2);
    align_decimal (rtd, d1, d2);
    mpz_com ((d1->v_mpz), (d1->v_mpz));
    mpz_and ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void
cob_decimal_sign (COB_RTD, cob_decimal * const d1)
{
    mpz_neg((d1->v_mpz), (d1->v_mpz));
}

void
cob_decimal_sign_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1)
{
    mpz_neg((dr->v_mpz), (d1->v_mpz));
}



void
cob_decimal_add_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_add_common(rtd, dr,d1,d2);
}

void
cob_decimal_sub_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_sub_common(rtd,dr,d1,d2);
}



void
cob_decimal_mul_1 (cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_mul_common(dr,d1,d2);
}

void
cob_decimal_div_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_div_common(rtd,dr,d1,d2);
}

void
cob_decimal_div_ibm_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2,  int maxscale)
{
    cob_decimal_div_ibm_common(rtd,dr,d1,d2,maxscale);
}

void
cob_decimal_mod_1 (COB_RTD, cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_mod_commmon(rtd, dr, d1, d2);
} 

void
cob_decimal_pow_1 (cob_decimal * const dr, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_pow_common(dr,d1,d2);
}

void
cob_decimal_add (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_add_common(rtd, NULL,d1,d2);
}

void
cob_decimal_sub (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_sub_common(rtd,NULL,d1,d2);
}

void
cob_decimal_mul (cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_mul_common(NULL,d1,d2);
}

void
cob_decimal_div (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_div_common(rtd, NULL, d1, d2);
}

void
cob_decimal_div_ibm (COB_RTD, cob_decimal * const d1, cob_decimal * const d2,  int maxscale)
{
    cob_decimal_div_ibm_common(rtd,NULL,d1,d2,maxscale);
}

void
cob_decimal_mod (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_mod_commmon(rtd, NULL, d1, d2);
}

void
cob_decimal_pow (cob_decimal * const d1, cob_decimal * const d2)
{
    cob_decimal_pow_common(NULL,d1,d2);
}

int
cob_decimal_cmp (COB_RTD, cob_decimal * const d1, cob_decimal * const d2)
{
    align_decimal (rtd, d1, d2);
    return mpz_cmp ((d1->v_mpz), (d2->v_mpz));
}

void
cob_decimal_mul_int (cob_decimal * const d1, const int d2)
{
    if ( unlikely(d1->scale == DECIMAL_NAN) ) {
        return; 
    }
    mpz_mul_si ((d1->v_mpz), (d1->v_mpz), d2);
}


void
cob_decimal_mul_uint (cob_decimal * const d1, const unsigned int d2)
{
    if ( unlikely(d1->scale == DECIMAL_NAN) ) {
        return; 
    }
    mpz_mul_ui ((d1->v_mpz), (d1->v_mpz), d2);
}

void
cob_decimal_div_uint (COB_RTD, cob_decimal * const d1,  const unsigned int d2)
{
    if ( unlikely(d1->scale == DECIMAL_NAN) ) {
        return; 
    }

    /* check for division by zero */
    if ( d2 == 0 ) {
        d1->scale = DECIMAL_NAN;
        cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE);
        return;
    }

    /*cob_shift_decimal_ (rtd, d1, COB_DIV_MAX_SCALE + ((d1->scale < 0) ? -d1->scale : 0));*/
    cob_decimal_div_scale(rtd,d1);

    mpz_tdiv_q_ui ((d1->v_mpz), (d1->v_mpz), d2);
}


void
cob_decimal_mod_uint (COB_RTD, cob_decimal * const d1,  const unsigned int d2)
{
    if ( unlikely(d1->scale == DECIMAL_NAN) ) {
        return; 
    }

    /* check for division by zero */
    if ( d2 == 0 ) {
        d1->scale = DECIMAL_NAN;
        cob_set_exception (rtd, COB_EC_SIZE_ZERO_DIVIDE);
        return;
    }

    mpz_tdiv_r_ui ((d1->v_mpz), (d1->v_mpz), d2);
}

void
cob_decimal_pow_uint (cob_decimal * const d1,  const unsigned int d2)
{
    if ( unlikely(d1->scale == DECIMAL_NAN) ) {
        return; 
    }

    mpz_pow_ui ((d1->v_mpz), (d1->v_mpz), d2);
    d1->scale *= d2;
}



/*
 * Optimized arithmetic for DISPLAY
 */

static int
display_add_int (COB_RTD, unsigned char *data, const size_t size, unsigned int n_in, int *zero)
{
    register unsigned char   *sp = data + size;
    register size_t          carry = 0;
    register int             i, n;
    register int             is;

    n = n_in;
    while ( n > 0 ) {
        i = n % 10;
        n /= 10;

        /* check for overflow */
        if ( unlikely(--sp < data) ) {
            if ( !rtd->current_module->flag_binary_truncate ) {
                return 0;
            }
            return 1;
        }

        /* perform addition */
        is = (*sp & 0x0F) + i + carry;
        if ( is > 9 ) {
            carry = 1;
            *sp = CHAR_0 + (is - 10);
        } else {
            carry = 0;
            *sp = CHAR_0 + is;
        }
        *zero |= is;
    }
    if ( carry == 0 ) {
        goto okxt;
    }

    /* carry up */
    while ( --sp >= data ) {
        /* perform addition */
        is = (*sp & 0x0F) + 1;
        if ( is > 9 ) {
            *sp = CHAR_0;
        } else {
            *sp = CHAR_0 + is;
            *zero |= is;

            goto okxt;
        }
    }
    if ( !rtd->current_module->flag_binary_truncate ) {
        return 0;
    }
    return 1;
    okxt:
    while ( --sp >= data ) {
        /* normalize field */
        is = (*sp & 0x0F) ;
        if ( is > 9 ) {
            *sp = CHAR_0;
        } else {
            *sp = CHAR_0 + is;
        }
        *zero |= is;
    }
    return 0;
}

static int
display_sub_int (COB_RTD, unsigned char *data, const size_t size, unsigned int n, int *zero)
{
    unsigned char   *sp = data + size;
    size_t          carry = 0;
    int             i;
    int             is;

    while ( n > 0 ) {
        i = n % 10;
        n /= 10;

        /* check for overflow */
        if ( unlikely(--sp < data) ) {
            return 1;
        }

        /* perform subtraction */
        if ( (*sp -= i + carry) < CHAR_0 ) {
            carry = 1;
            *sp += 10;
        } else {
            carry = 0;
        }
        *zero |= *sp & 0x0F;
    }
    if ( carry == 0 ) {
        goto okxt;
    }

    /* carry up */
    while ( --sp >= data ) {
        if ( (*sp -= 1) >= CHAR_0 ) {
            goto okxt;
        }
        *sp = CHAR_9;
        *zero |= CHAR_9;
    }

    return 1;
    okxt:
    while ( --sp >= data ) {
        /* normalize field */
        is = (*sp & 0x0F) ;
        if ( is > 9 ) {
            *sp = CHAR_0;
        } else {
            *sp = CHAR_0 + is;
            *zero |= is;
        }
    }
    return 0;
}


int
cob_unsign_display_fast_add_small_int (COB_RTD, cob_field * const f, int n)
{
    register unsigned char   *sp = f->data + f->size -1;
    register size_t          carry = 0;
    register int             is;

    if (n < 0) {
        return cob_display_add_int(rtd,f,n);
    }
    is = (*sp & 0x0F) + n + carry;
    if ( is > 9 ) {
        carry = 1;
        *sp = CHAR_0 + (is - 10);
    } else {
        carry = 0;
        *sp = CHAR_0 + is;
    }
    /* carry up */
    if (carry) {
        while ( --sp >= f->data ) {
            /* perform addition */
            is = (*sp & 0x0F) + 1;
            if ( is > 9 ) {
                *sp = CHAR_0;
            } else {
                *sp = CHAR_0 + is;
                break;;
            }
        }
    }
    return 0;
}

int
cob_display_add_int (COB_RTD, cob_field * const f, int n)
{
    int             sign;
    int             osize;
    int             i;
    unsigned char   *data = COB_FIELD_DATA (f);
    int             size = COB_FIELD_SIZE (f);
    int             scale = COB_FIELD_SCALE (f);
    int             zero=0;
    /*unsigned char   tfield[64];*/

    if (n == 0) {
        return 0;
    }

    if ( scale > 0 ) {
        return cob_add_int(rtd,f,n);
    }
    osize = size;
    /* memcpy (tfield, data, osize);*/
    sign = COB_GET_SIGN (f);
    /* -x + n = -(x - n) */
    if ( sign < 0 ) {
        n = -n;
    }

    if ( unlikely(scale < 0) ) {
        /* PIC 9(n)P(m) */
        if ( -scale < 10 ) {
            while ( scale++ ) {
                n /= 10;
            }
        } else {
            n = 0;
        }
    } else {
        /* PIC 9(n)V9(m) */
        size -= scale;
        /* Following can never be true as size is unsigned ?? */
        /* Comment out
        if (size < 0) {
                cob_put_sign (f, sign);
                goto overflow;
        }
        */
    }

    if ( n > 0 ) {
        /* add n to the field */

        if ( display_add_int (rtd, data, size, n, &zero) != 0 ) {
            /* if there was an overflow, recover the last value */
            /* memcpy (data, tfield, osize);*/
            goto overflow;
        }

    } else if ( n < 0 ) {
        /* subtract n from the field */
        if ( display_sub_int (rtd, data, size, -n, &zero) != 0 ) {
            for ( i = 0; i < size; i++ ) {
                data[i] = COB_I2D (9 - COB_D2I (data[i]));
            }
            zero=0;
            display_add_int (rtd, data, size, 1, &zero);
            sign = -sign;
        }
    }

    if (zero == 0) {
        sign = 0;
    }
    COB_PUT_SIGN (f, sign);
    return 0;


    overflow:
    COB_PUT_SIGN (f, sign);
    cob_set_exception (rtd, COB_EC_SIZE_OVERFLOW);
    return rtd->cob_exception_code; 

}

/*
 * Convenience functions
 */

int
cob_add (COB_RTD, cob_field * const f1, cob_field * const f2, const int opt)
{
    cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
    cob_decimal_set_field (rtd, &(rtd->cob_d2), f2);
    cob_decimal_add (rtd, &(rtd->cob_d1), &(rtd->cob_d2));
    return cob_decimal_get_field_1 (rtd, &(rtd->cob_d1), f1, opt, 0);
}

int
cob_sub (COB_RTD, cob_field * const f1, cob_field * const f2, const int opt)
{
    cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
    cob_decimal_set_field (rtd, &(rtd->cob_d2), f2);
    cob_decimal_sub (rtd, &(rtd->cob_d1), &(rtd->cob_d2));
    return cob_decimal_get_field_1 (rtd, &(rtd->cob_d1), f1, opt, 0);
}

/*CIT*/
int
cob_mul (COB_RTD, cob_field * const f1, cob_field * const f2, const int opt)
{
    cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
    cob_decimal_set_field (rtd, &(rtd->cob_d2), f2);
    cob_decimal_mul (&(rtd->cob_d1), &(rtd->cob_d2));
    return cob_decimal_get_field_1 (rtd, &(rtd->cob_d1), f1, opt, 0);
}

/*CIT*/
int
cob_div (COB_RTD, cob_field * const f1, cob_field * const f2, const int opt)
{
    cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
    cob_decimal_set_field (rtd, &(rtd->cob_d2), f2);
    cob_decimal_div (rtd, &(rtd->cob_d1), &(rtd->cob_d2));
    return cob_decimal_get_field_1 (rtd, &(rtd->cob_d1), f1, opt, 0);
}

int
cob_add_int (COB_RTD, cob_field * const f, const int ni)
{
    int scale = COB_FIELD_SCALE (f);
    int n = ni;
    if ( unlikely(n == 0) ) {
        return 0;
    }

    if ( (COB_FIELD_TYPE (f) == COB_TYPE_NUMERIC_DISPLAY)  &&
         (scale <= 0) ) {
        return cob_display_add_int(rtd,f,n);
    }
    if ( scale < 0 ) {
        cob_fatal_error(rtd,COB_FERROR_NEGSCALE);
    }

    cob_decimal_set_field (rtd, &(rtd->cob_d1), f);
    mpz_set_si (((rtd->cob_d2).v_mpz), n);
    (rtd->cob_d2).scale = 0;
    if ( (rtd->cob_d1).scale ) {
        mpz_ui_pow_ui ((rtd->cob_mexp), 10, (unsigned int)(rtd->cob_d1).scale);
        mpz_mul (((rtd->cob_d2).v_mpz), ((rtd->cob_d2).v_mpz), (rtd->cob_mexp));
        (rtd->cob_d2).scale = (rtd->cob_d1).scale;
    }
    mpz_add ((rtd->cob_d1.v_mpz), (rtd->cob_d1.v_mpz), ((rtd->cob_d2).v_mpz));

    return cob_decimal_get_field_1 (rtd, &(rtd->cob_d1), f, 0, 0);
}

int
cob_sub_int (COB_RTD, cob_field * const f, const int n)
{
    if ( unlikely(n == 0) ) {
        return 0;
    }
    return cob_add_int (rtd, f, -n);
}

int
cob_div_quotient (COB_RTD, cob_field * const dividend, cob_field * const divisor, cob_field * const quotient, const int opt)
{
    int ret;

    cob_decimal_set_field (rtd, &(rtd->cob_d1), dividend);
    cob_decimal_set_field (rtd, &(rtd->cob_d2), divisor);
    cob_decimal_set (&(rtd->cob_d3), &(rtd->cob_d1));

    /* compute quotient */
    cob_decimal_div (rtd, &(rtd->cob_d1), &(rtd->cob_d2));
    if ( (rtd->cob_d1).scale == DECIMAL_NAN ) {
        (rtd->cob_d3).scale = DECIMAL_NAN;
        return rtd->cob_exception_code;
    }

    /* set quotient */
    cob_decimal_set (&(rtd->cob_d4), &(rtd->cob_d1));
    ret = cob_decimal_get_field_1 (rtd, &(rtd->cob_d1), quotient, opt, 0);

    /* truncate digits from the quotient */
    cob_shift_decimal_ (rtd, &(rtd->cob_d4), COB_FIELD_SCALE(quotient) - (rtd->cob_d4).scale);

    /* compute remainder */
    cob_decimal_mul (&(rtd->cob_d4), &(rtd->cob_d2));
    cob_decimal_sub (rtd, &(rtd->cob_d3), &(rtd->cob_d4));

    return ret;
}

int
cob_div_remainder (COB_RTD, cob_field * const fld_remainder, const int opt)
{
    return cob_decimal_get_field_1 (rtd, &(rtd->cob_d3), fld_remainder, opt, 0);
}

int
cob_cmp_int (COB_RTD, cob_field * const f1, const int n)
{
    if (n == 0) {
        cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
        return mpz_sgn (((rtd->cob_d1).v_mpz));
    }
    cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
    mpz_set_si (((rtd->cob_d2).v_mpz), n);
    (rtd->cob_d2).scale = 0;
    return cob_decimal_cmp (rtd, &(rtd->cob_d1), &(rtd->cob_d2));
}

int
cob_cmp_uint (COB_RTD, cob_field * const f1, const unsigned int n)
{

    if (n == 0) {
        cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
        return mpz_sgn (((rtd->cob_d1).v_mpz));
    }
    cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
    mpz_set_ui (((rtd->cob_d2).v_mpz), n);
    (rtd->cob_d2).scale = 0;
    return cob_decimal_cmp (rtd, &(rtd->cob_d1), &(rtd->cob_d2));
}

int
cob_numeric_cmp (COB_RTD, cob_field * const f1, cob_field * const f2)
{
    cob_decimal_set_field (rtd, &(rtd->cob_d1), f1);
    cob_decimal_set_field (rtd, &(rtd->cob_d2), f2);
    return cob_decimal_cmp (rtd, &(rtd->cob_d1), &(rtd->cob_d2));
}

int
cob_cmp_packed (COB_RTD, cob_field * const f, int n)
{
    int                     sign;
    size_t                  size;
    size_t                  inc = 0;
    unsigned char           *p;
    unsigned char           val1[20];

    sign = COB_FIELD_HAVE_SIGN (f) ? cob_packed_get_sign (f) : 0;
    /* Field positive, value negative */
    if ( sign >= 0 && n < 0 ) {
        return 1;
    }
    /* Field negative, value positive */
    if ( sign < 0 && n >= 0 ) {
        return -1;
    }
    /* Both positive or both negative */
    p = f->data;
    for ( size = 0; size < 20; size++ ) {
        if ( size < 20 - f->size ) {
            val1[size] = 0;
        } else {
            val1[size] = p[inc++];
        }
    }
    val1[19] &= 0xf0;
    if ( (COB_FIELD_DIGITS(f) % 2) == 0 ) {
        val1[20 - f->size] &= 0x0f;
    }
    if ( n != rtd->cmp_packed_lastval ) {
        rtd->cmp_packed_lastval = n;
        if ( n < 0 ) {
            n = -n;
        }
        memset (&(rtd->packed_value)[14], 0, 6);
        if ( n ) {
            p = &(rtd->packed_value)[19];
            *p =  (n % 10) << 4;
            p--;
            n /= 10;
            for ( ; n; ) {
                size = n % 100;
                *p = (unsigned char)((size % 10) | ((size / 10) << 4));
                n /= 100;
                p--;
            }
        }
    }
    for ( size = 0; size < 20; size++ ) {
        if ( val1[size] != (rtd->packed_value)[size] ) {
            if ( sign < 0 ) {
                return(rtd->packed_value)[size] - val1[size];
            } else {
                return val1[size] - (rtd->packed_value)[size];
            }
        }
    }
    return 0;
}

void
cob_init_numeric (COB_RTD)
{
    size_t  i;

    cob_decimal_init (&(rtd->cob_d1));
    cob_decimal_init (&(rtd->cob_d2));
    cob_decimal_init (&(rtd->cob_d3));
    cob_decimal_init (&(rtd->cob_d4));
    cit_bigint_init(CHAR_0);
    mpz_init2 ((rtd->cob_mpzt), 256);
    mpz_init2 ((rtd->cob_mexp), 512);
    for ( i = 0; i < COB_MAX_NUMBER_DIGIT; i++ ) {
        mpz_init ((rtd->cob_mpze10)[i]);
        mpz_ui_pow_ui ((rtd->cob_mpze10)[i], 10, i);
    }
    memset ((rtd->packed_value), 0, sizeof((rtd->packed_value)));
}

void
cob_clear_numeric (COB_RTD)
{
    size_t  i;

    cob_decimal_clear (&(rtd->cob_d1));
    cob_decimal_clear (&(rtd->cob_d2));
    cob_decimal_clear (&(rtd->cob_d3));
    cob_decimal_clear (&(rtd->cob_d4));
    mpz_clear ((rtd->cob_mpzt));
    mpz_clear ((rtd->cob_mexp));
    for ( i = 0; i < COB_MAX_NUMBER_DIGIT; i++ ) {
        mpz_clear ((rtd->cob_mpze10)[i]);
    }
}

/* Numeric Display compares */

static int
cob_all_sp_or_zero (COB_RTD, const unsigned char * const data1, const size_t size1)
{
    const unsigned char     *p = data1;
    size_t                  inc;
    int all0  = 1;
    int allsp = !(rtd->current_module->runtime_flags & COB_FLAG_RT_MF_SPZERO);

    for ( inc = 0; inc < size1; inc++, p++ ) {
        if ( *p != 0 ) {
            all0 =0;
        }
        if ( *p != CHAR_SP ) {
            allsp = 0;
        }
    }
    if ( all0 || allsp ) {
        return 1;
    }
    return 0;
}

int
cob_cmp_display_mf50 (COB_RTD, const unsigned char * const data1, const size_t size1, 
                      const unsigned char * const data2, const size_t size2)
{
    unsigned char buffer1[100];
    unsigned char buffer2[100];
    int a1 = cob_all_sp_or_zero(rtd,data1,size1);
    int a2 = cob_all_sp_or_zero(rtd,data2,size2);
    int s;

    if (a1 && a2) {
        return 0;
    } else if (a1) {
        return -1;
    } else if (a2) {
        return 1;
    }
    s = cob_max_int(size1,size2);
    memset (buffer1, CHAR_0, s);
    memset (buffer2, CHAR_0, s);
    memcpy(buffer1+(s-size1), data1, size1);
    memcpy(buffer2+(s-size2), data2, size2);
    return memcmp(STRING_A2E(buffer1, s), STRING_A2E(buffer2, s), s);       
}

int
cob_cmp_numdisp (COB_RTD, const unsigned char * const data, const size_t size, const int n)
{
    register int            val = 0;
    register int            inc;
    register const unsigned char     *p;
    char str[100];
    unsigned char buffer[100];
    unsigned char buffer2[100];

    p = data;

    if ( (rtd->current_module->runtime_flags & COB_FLAG_RT_STRICT_COMPARE_LOW) && !n ) {
        int all0  = 1;
        inc = size;
        for ( ; inc ; inc--, p++ ) {
            if ( *p != 0 ) {
                all0 =0;
                break;
            }
        }
        if ( all0 ) {
            return -1;
        }
    }

    if ( (rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) ) {
        int all0  = 1;
        int allsp = !(rtd->current_module->runtime_flags & COB_FLAG_RT_MF_SPZERO);
        int len;
        inc = size;
        for ( ; inc ; inc--, p++ ) {
            if ( *p != 0 ) {
                all0 =0;
            }
            if ( *p != CHAR_SP ) {
                allsp = 0;
            }
        }
        if ( all0 || allsp ) {
            return -1;
        }
        sprintf(str,"%%0%dd", (int)size);
        sprintf((char*)buffer,str,n);
        len = strlen ((char*)buffer);
        inc =  len - size;
        p = data;
        if (inc > 0) {
            memset (buffer2, CHAR_0, sizeof(buffer2));
            memcpy (buffer2 + inc , data, size);
            p = buffer2;
        }
        return memcmp(p, STRING_A2E(buffer, len), len);       
    } else

    {
        inc = size;
        for ( ; inc; inc--, p++ ) {
            val = FAST_MUL_10(val) + COB_D2I (*p);
        }
    }
    return(val < n) ? -1 : (val > n);
}


int
cob_cmp_long_numdisp (COB_RTD, const unsigned char * const data, const size_t size, const int n)
{
    long long               val = 0;
    size_t                  inc;
    const unsigned char     *p;
    char str[100];
    unsigned char buffer[100];

    p = data;

    if ( (rtd->current_module->runtime_flags & COB_FLAG_RT_STRICT_COMPARE_LOW) && !n ) {
        int all0  = 1;
        inc = size;
        for ( ; inc ; inc--, p++ ) {
            if ( *p != 0 ) {
                all0 =0;
                break;
            }
        }
        if ( all0 ) {
            return -1;
        }
    }

    if ( (rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) ) {
        int all0  = 1;
        int allsp = !(rtd->current_module->runtime_flags & COB_FLAG_RT_MF_SPZERO);
        for ( inc = 0; inc < size; inc++, p++ ) {
            if ( *p != 0 ) {
                all0 =0;
            }
            if ( *p != CHAR_SP ) {
                allsp = 0;
            }
            sprintf(str,"%%0%dd", (int)size);
            sprintf((char*)buffer,str,n);
            return memcmp(data, STRING_A2E(buffer, strlen((char*)buffer)), size);
        }
        if ( all0 || allsp ) {
            return -1;
        }
    } else {
        for ( inc = 0; inc < size; inc++, p++ ) {
            val = (val * 10) + COB_D2I (*p);
        }
    }

    return(val < n) ? -1 : (val > n);
}




int
cob_cmp_sign_numdisp (COB_RTD, const unsigned char * const data, const size_t size, const int n)
{
    register int            val = 0;
    size_t                  inc;
    const unsigned char     *p;

    p = data;
    for ( inc = 0; inc < size - 1; inc++, p++ ) {
        val = (val * 10) + COB_D2I(*p);
    }
    val *= 10;
    if ( COB_IS_DDIGIT(*p) ) {
        val += COB_D2I(*p);
    } else {
        unsigned char sign = *p;
        if ( cob_get_sign_all(rtd, &sign) < 0 ) {
            val += sign -CHAR_0;
            val = -val;
        } else {
            val += sign -CHAR_0;
        }
    }
    return(val < n) ? -1 : (val > n);
}


int
cob_cmp_long_sign_numdisp (COB_RTD, const unsigned char * const data, const size_t size, const int n)
{
    long long               val = 0;
    size_t                  inc;
    const unsigned char     *p;

    p = data;
    for ( inc = 0; inc < size - 1; inc++, p++ ) {
        val = (val * 10) + COB_D2I(*p);
    }
    val *= 10;
    if ( COB_IS_DDIGIT(*p) ) {
        val += COB_D2I(*p);
    } else {
        unsigned char sign = *p;
        if ( cob_get_sign_all(rtd, &sign) < 0 ) {
            val += sign -CHAR_0;
            val = -val;
        } else {
            val += sign -CHAR_0;
        }
    }
    return(val < n) ? -1 : (val > n);
}


void
cob_decimal_set_int (cob_decimal * const d, const int n)
{
    mpz_set_si ((d->v_mpz), n);
    d->scale = 0;
}

void
cob_decimal_set_int_1 (cob_decimal * const d, const int n, const int gmp_flags)
{
    cob_decimal_set_int(d,n);
}

void
cob_decimal_set_uint_1 (cob_decimal * const d, const unsigned int n, const int gmp_flags)
{
    mpz_set_ui ((d->v_mpz), n);
    d->scale = 0;
}

void
cob_decimal_set_uint (cob_decimal * const d, const unsigned int n)
{
    mpz_set_ui ((d->v_mpz), n);
    d->scale = 0;
}

void
cob_decimal_set_sll_1 (cob_decimal * const d, const signed long long  n, const int gmp_flags)
{
    mpz_set_sll ((d->v_mpz), n);
    d->scale = 0;
}

void
cob_decimal_set_sll (cob_decimal * const d, const signed long long  n)
{
    mpz_set_sll ((d->v_mpz), n);
    d->scale = 0;
}

void
cob_decimal_set_ull_1 (cob_decimal * const d, const unsigned long long  n, const int gmp_flags)
{
    mpz_set_ull ((d->v_mpz), n);
    d->scale = 0;
}

void
cob_decimal_set_ull (cob_decimal * const d, const unsigned long long  n)
{
    mpz_set_ull ((d->v_mpz), n);
    d->scale = 0;
}

signed long long
cob_decimal_get_sll (cob_decimal * const d)
{
    return mpz_get_sll ((d->v_mpz));
}

unsigned long long
cob_decimal_get_ull (cob_decimal * const d)
{
    return mpz_get_ull ((d->v_mpz));
}

int 
cob_decimal_digits_count (cob_decimal * const d) 
{
    return mpz_sizeinbase ((d->v_mpz), 10);
}
void cob_decimal_add_sll_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_sll((d2->v_mpz), l2);
    align_decimal(rtd,d1, d2);
    dr->scale = d1->scale;
    mpz_add ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
}
void cob_decimal_add_sll (COB_RTD, cob_decimal *d1, const long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_sll((d2->v_mpz), l2);
    align_decimal(rtd,d1, d2);
    mpz_add ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}
void cob_decimal_sub_sll_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_sll((d2->v_mpz), l2);
    align_decimal(rtd,d1, d2);
    dr->scale = d1->scale;
    mpz_sub ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
}
void cob_decimal_sub_sll (COB_RTD, cob_decimal *d1, const long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_sll((d2->v_mpz), l2);
    align_decimal(rtd,d1, d2);
    mpz_sub ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_mul_sll_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_sll((d2->v_mpz), l2);
    dr->scale = d1->scale;
    mpz_mul ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_mul_sll (COB_RTD, cob_decimal *d1, const long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_sll((d2->v_mpz), l2);
    mpz_mul ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_add_ull_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const unsigned long long l2){
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_ull((d2->v_mpz), l2);
    align_decimal(rtd,d1, d2);
    dr->scale = d1->scale;
    mpz_add ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_add_ull (COB_RTD, cob_decimal *d1, const unsigned long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_ull((d2->v_mpz), l2);
    align_decimal(rtd,d1, d2);
    mpz_add ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_sub_ull_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const unsigned long long l2){
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_ull((d2->v_mpz), l2);
    align_decimal(rtd,d1, &(rtd->cob_d2));
    dr->scale = d1->scale;
    mpz_sub ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_sub_ull (COB_RTD, cob_decimal *d1, const unsigned long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_ull((d2->v_mpz), l2);
    align_decimal(rtd,d1, &(rtd->cob_d2));
    mpz_sub ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_mul_ull_1 (COB_RTD, cob_decimal *dr, cob_decimal *d1, const unsigned long long l2){
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_ull((d2->v_mpz), l2);
    dr->scale = d1->scale;
    mpz_mul ((dr->v_mpz), (d1->v_mpz), (d2->v_mpz));
}

void cob_decimal_mul_ull (COB_RTD, cob_decimal *d1, const unsigned long long l2)
{
    cob_decimal *d2 = &rtd->cob_d2;
    mpz_set_ull((d2->v_mpz), l2);
    mpz_mul ((d1->v_mpz), (d1->v_mpz), (d2->v_mpz));
}


void cob_float_init (cob_float * const f, double d)
{
    if (d) {
        mpf_init2(f->v_mpf, 56);
    } else {
        mpf_init2(f->v_mpf, 24);
    }
}

void cob_float_add_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2)
{
    mpf_add(dr->v_mpf, d1->v_mpf, d2->v_mpf);
}
void cob_float_sub_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2)
{
    mpf_sub(dr->v_mpf, d1->v_mpf, d2->v_mpf);
}
void cob_float_mul_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2)
{
    mpf_mul(dr->v_mpf, d1->v_mpf, d2->v_mpf);
}
void cob_float_div_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2)
{
    mpf_div(dr->v_mpf, d1->v_mpf, d2->v_mpf);
}

void cob_float_pow_1 (cob_float * const dr, cob_float * const d1, cob_float * const d2)
{
    if (mpf_fits_ulong_p (d2->v_mpf) ) {
        mpf_pow_ui (dr->v_mpf, d1->v_mpf, mpf_get_ui(d2->v_mpf));
    } else {
        cob_float_set_double (dr, pow (cob_float_get_double (d1),
                                       cob_float_get_double (d2)));
    }
}

void cob_float_clear (cob_float * const d)
{
    mpf_clear(d->v_mpf);
}

double cob_float_get_double (cob_float * const d)
{
    return mpf_get_d(d->v_mpf);
}

void cob_float_set_double (cob_float *d, const double v)
{
    mpf_set_d(d->v_mpf, v);
}

void cob_float_set (cob_float * const dst, cob_float * const src)
{
    mpf_set(dst->v_mpf, src->v_mpf);
}

void cob_float_set_str (cob_float * const dst, char * const src, int scale, int sign)
{
    char buffer[200];
    if (scale) {
        sprintf (buffer,"%s@%d",src, -scale);
        mpf_set_str(dst->v_mpf, buffer, 10);
    } else
        mpf_set_str(dst->v_mpf, src, 10);
    if (sign < 0) {
        mpf_neg(dst->v_mpf, dst->v_mpf);
    }
}



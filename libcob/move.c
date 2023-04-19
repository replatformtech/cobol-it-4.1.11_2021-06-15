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

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef  HAVE_LOCALE_H
    #include <locale.h>
#endif

#include "grisu.h"
#include "move.h"
#include "coblocal.h"
#include "byteswap.h"
#include "numeric.h"
#include "intrinsic.h"
#include "a2e.h"

const int    cob_exp10[10] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000
};

const long long  cob_exp10LL[19] = {
    1LL,
    10LL,
    100LL,
    1000LL,
    10000LL,
    100000LL,
    1000000LL,
    10000000LL,
    100000000LL,
    1000000000LL,
    10000000000LL,
    100000000000LL,
    1000000000000LL,
    10000000000000LL,
    100000000000000LL,
    1000000000000000LL,
    10000000000000000LL,
    100000000000000000LL,
    1000000000000000000LL
};

const unsigned long long  cob_maskLL[9] = {
    0LL,
    0xFFLL,
    0xFFFFLL,
    0xFFFFFFLL,
    0xFFFFFFFFLL,
    0xFFFFFFFFFFLL,
    0xFFFFFFFFFFFFLL,
    0xFFFFFFFFFFFFFFLL,
    0xFFFFFFFFFFFFFFFFLL,
};


const double  cob_exp10_double[19] = {
    1L,
    10L,
    100L,
    1000L,
    10000L,
    100000L,
    1000000L,
    10000000L,
    100000000L,
    1000000000L,
    10000000000L,
    100000000000L,
    1000000000000L,
    10000000000000L,
    100000000000000L,
    1000000000000000L,
    10000000000000000L,
    100000000000000000L,
    1000000000000000000L
};



/* Prototype */
unsigned long long
cob_display_get_ull(COB_RTD, cob_field *f);
unsigned long long
cob_display_get_ull_opt(COB_RTD, cob_field *f);
static long long
cob_display_get_sll(COB_RTD, cob_field *f);
static long long
cob_display_get_sll_opt(COB_RTD, cob_field *f);
static long long
cob_packed_get_sll(COB_RTD, cob_field *f1, int trunc);
static unsigned long long
cob_packed_get_ull(COB_RTD, cob_field *f1, int trunc);
static void
indirect_move(COB_RTD, void (*func)(COB_RTD, cob_field *src, cob_field *dst),
              cob_field *src, cob_field *dst, size_t size, int scale, int Alpha);
static void
indirect_move_same(COB_RTD, cob_field *src, cob_field *dst);

int
cob_min_int(const int x, const int y) {
    if (x < y) {
        return x;
    }
    return y;
}

int
cob_max_int(const int x, const int y) {
    if (x > y) {
        return x;
    }
    return y;
}

#if __linux__
    #define own_byte_memcpy(a,b,c) memcpy(a,b,c)
#else
static COB_INLINE void
own_byte_memcpy(unsigned char *s1, const unsigned char *s2, size_t size) {
    do {
        *s1++ = *s2++;
    } while (--size);
}
#endif

static int
cob_store_common_region(COB_RTD, cob_field *f, const unsigned char *data,
                        const size_t size, const int scale, int rawcopy) {
    const unsigned char *p;
    unsigned char       *q;
    int                 csize;
    int                 cinc;
    int                 lf1 = -scale;
    int                 lf2 = -COB_FIELD_SCALE(f);
    int                 hf1 = (int)size;
    int                 hf2 = (int)COB_FIELD_SIZE(f);
    int                 lcf;
    int                 gcf;
    int                 allzero = 1;
    unsigned char       zero_char = CHAR_0;
    unsigned char       sp_char = CHAR_SP;

    hf1 = hf1 + lf1;
    hf2 = hf2 + lf2;
    lcf = cob_max_int(lf1, lf2);
    gcf = cob_min_int(hf1, hf2);
    memset(COB_FIELD_DATA(f), zero_char, COB_FIELD_SIZE(f));
    if (gcf > lcf) {
        csize = (gcf - lcf);
        p = data + hf1 - gcf;
        q = COB_FIELD_DATA(f) + hf2 - gcf;
        for (cinc = 0; cinc < csize; ++cinc, ++p, ++q) {
            unsigned char c = *p;
            if (c == zero_char || unlikely(c == sp_char || c == 0)) {
                /* this not needed because the memset when not rawcopy*/
               if (rawcopy)
               {
                  *q = *p;
               }
            } else {
                *q = *p;
                allzero = 0;
            }
        }
    }
    return allzero;
}

long long
cob_binary_mget_int64(COB_RTD, const cob_field *const f) {
    long long   n = 0;
    size_t      fsiz = 8 - f->size;

/* Experimental code - not activated */
#if 0
    unsigned char   *s;

    if ( (COB_FIELD_BINARY_SWAP (f) && !COB_FIELD_HAVE_SIGN (f)) ||
        (!COB_FIELD_BINARY_SWAP (f) && COB_FIELD_HAVE_SIGN (f)) ) {
        s = (unsigned char *)&n + fsiz;
    } else {
        s = (unsigned char *)&n;
    }
    own_byte_memcpy (s, f->data, f->size);
    if ( COB_FIELD_BINARY_SWAP (f) ) {
        n = COB_BSWAP_64 (n);
    }
    if ( COB_FIELD_HAVE_SIGN (f) ) {
        n >>= 8 * fsiz; /* shift with sign */
    }
#endif
#ifndef WORDS_BIGENDIAN
    if (COB_FIELD_BINARY_SWAP(f)) {
        if (COB_FIELD_HAVE_SIGN(f)) {
            own_byte_memcpy((unsigned char *)&n, f->data, f->size);
            n = COB_BSWAP_64(n);
            n >>= 8 * fsiz; /* shift with sign */
        } else {
            own_byte_memcpy(((unsigned char *)&n) + fsiz, f->data, f->size);
            n = COB_BSWAP_64(n);
        }
    } else {
        if (COB_FIELD_HAVE_SIGN(f)) {
            own_byte_memcpy(((unsigned char *)&n) + fsiz, f->data, f->size);
            n >>= 8 * fsiz; /* shift with sign */
        } else {
            own_byte_memcpy((unsigned char *)&n, f->data, f->size);
        }
    }
#else   /* WORDS_BIGENDIAN */
    if (COB_FIELD_HAVE_SIGN(f)) {
        own_byte_memcpy((unsigned char *)&n, f->data, f->size);
        n >>= 8 * fsiz; /* shift with sign */
    } else {
        own_byte_memcpy(((unsigned char *)&n) + fsiz, f->data, f->size);
    }
#endif  /* WORDS_BIGENDIAN */
    return n;
}

void
cob_binary_mset_int64(COB_RTD, cob_field *f, long long n) {
#ifndef WORDS_BIGENDIAN
    unsigned char   *s;

    if (COB_FIELD_BINARY_SWAP(f)) {
        n = COB_BSWAP_64(n);
        s = ((unsigned char *)&n) + 8 - f->size;
    } else {
        s = (unsigned char *)&n;
    }
    own_byte_memcpy(f->data, s, f->size);
#else   /* WORDS_BIGENDIAN */
    own_byte_memcpy(f->data, ((unsigned char *)&n) + 8 - f->size, f->size);
#endif  /* WORDS_BIGENDIAN */
}

/*
 * Display
 */

static void
cob_move_alphanum_to_display(COB_RTD, cob_field *f1, cob_field *f2) {
    int             sign, count, size;
    unsigned char   *p;
    unsigned char   *s1 = f1->data;
    unsigned char   *s2 = COB_FIELD_DATA(f2);
    unsigned char   *e1 = s1 + f1->size;
    unsigned char   *e2 = s2 + COB_FIELD_SIZE(f2);
    unsigned char   c;
    unsigned char   dp;
    unsigned char   ds;
    unsigned char   sp0 = 0;
    unsigned char   ovbuff[COB_SMALL_BUFF];

    if (f1->size < COB_SMALL_BUFF) {
        /*avoid overlap problem*/
        memcpy(ovbuff, s1, f1->size);
        s1 = ovbuff;
        e1 = s1 + f1->size;
    }
    /* initialize */
    memset(f2->data, CHAR_0, f2->size);

    /* skip white spaces */
    for (; s1 < e1; ++s1) {
        if (!cob_isspace_char(rtd, *s1)) {
            break;
        }
    }

    /* check for sign */
    sign = 0;
    if (s1 != e1) {
        if (*s1 == CHAR_PLUS || *s1 == CHAR_MINUS) {
            sign = (*s1++ == CHAR_PLUS) ? 1 : -1;
        }
    }

    /* count the number of digits before decimal point */
    count = 0;
    dp = CHAR_DOT;
    ds = CHAR_COMMA;
    if (rtd->current_module) {
        dp = rtd->current_module->decimal_point;
        ds = rtd->current_module->numeric_separator;
        sp0 = (rtd->current_module->runtime_flags & COB_FLAG_RT_MF_SPZERO);
    }
    for (p = s1; p < e1 && *p != dp; ++p) {
        c = *p;
        if (cob_isdigit_char(rtd, c) || (sp0 && c == CHAR_SP)) {
            ++count;
        }
    }

    /* find the start position */
    size = (int)COB_FIELD_SIZE(f2) - COB_FIELD_SCALE(f2);
    if (count < size) {
        s2 += size - count;
    } else {
        while (count-- > size) {
            while (!cob_isdigit_char(rtd, *s1++)) {
                ;
            }
        }
    }

    /* move */
    count = 0;
    for (; s1 < e1 && s2 < e2; ++s1) {
        c = *s1;
        if (c == CHAR_SP && sp0) {
            c = CHAR_0;
        }
        if (cob_isdigit_char(rtd, c)) {
            *s2++ = c;
        } else if (c == dp) {
            if (count++ > 0) {
                goto error;
            }
        } else if (!(cob_isspace_char(rtd, c) || c == ds)) {
            goto error;
        }
    }

    COB_PUT_SIGN(f2, sign);
    return;

error:
    memset(f2->data, CHAR_0, f2->size);
    COB_PUT_SIGN(f2, 0);
}

static void
cob_move_alphanum_to_display_mf40(COB_RTD, cob_field *f1, cob_field *f2) {
    size_t      size1 = f1->size;
    size_t      size2 = f2->size;
    unsigned char   *data1 = f1->data;
    unsigned char   *data2 = f2->data;

    memset(data2, CHAR_0, size2);
    if (f2->attr->scale > 0) {
        size2 -= f2->attr->scale;
    }
    data1 += (size1 - 1);
    data2 += (size2 - 1);
    while (size1 > 0 && size2 > 0) {
        *data2 = (*data1 & 0x0F) |  CHAR_0;
        data1--;
        data2--;
        size1--;
        size2--;
    }
    COB_PUT_SIGN(f2, 0);
}


void
cob_move_display_to_display(COB_RTD, cob_field *f1, cob_field *f2) {
    /*COBOL-IT*/
    int allzero;
    int sign = COB_DUP_GET_SIGN(f1);
    int rawcopy = (rtd->current_module && (rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_VMS));

    allzero = cob_store_common_region(rtd, f2, COB_FIELD_DATA(f1), COB_FIELD_SIZE(f1),
                                      COB_FIELD_SCALE(f1), rawcopy);

    if (allzero && (sign < 0)) {
        sign = 1;
    }
    COB_PUT_SIGN(f2, sign);
}

void
cob_move_numeric_display_to_numeric_display(COB_RTD, cob_field *f1, cob_field *f2) {
    /*COBOL-IT*/
    int allzero;
    int sign = COB_DUP_GET_SIGN(f1);
    if (COB_FIELD_HAVE_SIGN(f2)) {
        allzero = cob_store_common_region(rtd, f2, COB_FIELD_DATA(f1), COB_FIELD_SIZE(f1),
                                          COB_FIELD_SCALE(f1), 0);
        if (allzero && (sign < 0)) {
            sign = 1;
        }
        COB_PUT_SIGN(f2, sign);
    } else {
        cob_move_display_to_display(rtd, f1, f2);
    }
}

static void
cob_move_display_to_alphanum(COB_RTD, cob_field *f1, cob_field *f2) {
    int             diff;
    int             zero_size;
    /*COBOL-IT*/
    cob_field       temp;
    cob_field_attr  attr;
    unsigned char   data[COB_MAX_NUMBER_DIGIT * 2];
    int             sign1 = COB_DUP_GET_SIGN(f1);
    size_t          size1 = COB_FIELD_SIZE(f1);
    size_t          scale1 = COB_FIELD_SCALE(f1);
    size_t          size2 = f2->size;
    unsigned char   *data1 = COB_FIELD_DATA(f1);
    unsigned char   *data2 = f2->data;
    unsigned char   *pdata2;

    sign1++; /* just to avoid warning */

    if (rtd->current_module &&
        ((rtd->current_module->runtime_flags & COB_FLAG_RT_MF_SPZERO) ||
         ((rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) && COB_FIELD_HAVE_SIGN(f1)))) {
        /* Flag spzero fix space and byte zero*/
        /*
        */
        if ((rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) &&
            COB_FIELD_HAVE_SIGN(f1) &&
            COB_FIELD_SIGN_SEPARATE(f1) && !COB_FIELD_SIGN_LEADING(f1)) {
            /*avoid trailing separeted sign problems*/
            COB_ATTR_INIT(COB_TYPE_NUMERIC_DISPLAY, size1, scale1,
                          0, NULL);
            temp.size = size1;
            temp.data = data;
            temp.attr = &attr;
            cob_move(rtd, f1, &temp);
            cob_move(rtd, &temp, f2);
            return;
        }
        attr = *(f1->attr);
        temp.size = size1;
        temp.data = data;
        temp.attr = &attr;
        cob_store_common_region(rtd, &temp, data1, size1, scale1, 0);
        f1 = &temp;
        data1 = COB_FIELD_DATA(f1);
    }

    if (size1 >= size2) {
        if (COB_FIELD_JUSTIFIED(f2)) {
            COB_MEMCPY(data2, data1 + size1 - size2, size2);
        } else {
            COB_MEMCPY(data2, data1, size2);
        }
    } else {
        diff = (int)(size2 - size1);
        zero_size = 0;
        if (COB_FIELD_JUSTIFIED(f2)) {
            pdata2 = data2 + diff;
            if (COB_FIELD_SCALE(f1) < 0) {
                zero_size = cob_min_int((int)-COB_FIELD_SCALE(f1), diff);
                pdata2 = pdata2 - zero_size;
            }
        } else {
            pdata2 = data2;
        }
        /* move */
        COB_MEMCPY(pdata2, data1, size1);
        /* implied 0 ('P's) */
        if (COB_FIELD_SCALE(f1) < 0) {
            zero_size = cob_min_int((int)-COB_FIELD_SCALE(f1), diff);
            memset(pdata2 + size1, CHAR_0, (size_t)zero_size);
        }
        /* padding */
        if (diff - zero_size > 0) {
            if (COB_FIELD_JUSTIFIED(f2)) {
                memset(data2, CHAR_SP, (size_t)(diff - zero_size));
            } else {
                memset(data2 + size1 + zero_size, CHAR_SP, (size_t)(diff - zero_size));
            }
        }
    }

}

static void
cob_mvs_fix_lastdigit(COB_RTD, cob_field *f) {
    unsigned char *p;
    if (!COB_FIELD_HAVE_SIGN(f)) {
        p = f->data + (f->size - 1);
        if (!cob_isdigit_char(rtd, *p)) {
            cob_get_sign_all(rtd, p);
        }

    }

}

void
cob_fill_national(COB_RTD, cob_field *f, char c) {
    unsigned char *n, a, b;
    unsigned char *data = f->data;
    int           size = f->size;

    n = cob_utf16_char(rtd, c);
    a = *n;
    b = *(n + 1);
    while (size > 1) {
        *data++ = a;
        *data++ = b;
        size -= 2;
    }
}

static void
cob_move_alphanum_to_alphanum_1(COB_RTD, cob_field *f1, cob_field *f2, int force_justified, int fillchar) {
    size_t      size1 = f1->size;
    size_t      size2 = f2->size;
    unsigned char   *data1 = f1->data;
    unsigned char   *data2 = f2->data;
    /*unsigned char    fillchar= COB_FIELD_IS_NUMERIC_OR_EDITED(f2)? CHAR_0 :CHAR_SP; */

    if (size1 >= size2) {
        /* move string with truncation */
        if (COB_FIELD_JUSTIFIED(f2) || force_justified) {
            COB_MEMCPY(data2, data1 + size1 - size2, size2);
        } else {
            COB_MEMCPY(data2, data1, size2);
        }
    } else {
        /* move string with padding */
        if (COB_FIELD_JUSTIFIED(f2) || force_justified) {
            memset(data2, fillchar, size2 - size1);
            COB_MEMCPY(data2 + size2 - size1, data1, size1);
        } else {
            COB_MEMCPY(data2, data1, size1);
            memset(data2 + size1, fillchar, size2 - size1);
        }
    }
}

void
cob_move_alphanum_to_alphanum(COB_RTD, cob_field *f1, cob_field *f2) {
    cob_move_alphanum_to_alphanum_1(rtd, f1, f2, 0, CHAR_SP);
}
/* national */
void
cob_move_national_to_national(COB_RTD, cob_field *f1, cob_field *f2) {
    int      size1 = f1->size;
    int      size2 = f2->size;
    int      i, utf16_le = 0;
    unsigned char   *p;
    unsigned char   *data1 = f1->data;
    unsigned char   *data2 = f2->data;

    if (rtd->current_module && (rtd->current_module->module_version > 1)) {
        utf16_le  = rtd->current_module->utf16_le;
    }
    if (size1 >= size2) {
        /* move string with truncation */
        if (COB_FIELD_JUSTIFIED(f2)) {
            COB_MEMCPY(data2, data1 + size1 - size2, size2);
        } else {
            COB_MEMCPY(data2, data1, size2);
        }
    } else {
        /* move string with padding */
        i = 0;
        p = COB_FIELD_JUSTIFIED(f2) ? data2 : data2 + size1;
        while (i <  size2 - size1) {
            if (utf16_le) {
                *p = 0x20;   /*UTF-16LE SPACE IS 0X2000 */
                p++;
                *p = 0;
            } else {
                *p = 0;
                p++;
                *p = 0x20; /*UTF-16BE SPACE IS 0X0020 */
            }
            p++;
            i += 2;
        }
        if (COB_FIELD_JUSTIFIED(f2)) {
            COB_MEMCPY(data2 + size2 - size1, data1, size1);
        } else {
            COB_MEMCPY(data2, data1, size1);
        }
    }
}

void
cob_move_alphanum_to_national(COB_RTD, cob_field *f1, cob_field *f2) {

    cob_move_national_to_national(rtd, cob_intr_national_of(rtd, 1, f1), f2);
}

void
cob_move_national_to_alphanum(COB_RTD, cob_field *f1, cob_field *f2) {

    cob_move_alphanum_to_alphanum(rtd, cob_intr_display_of(rtd, 1, f1), f2);
}


/*
 * Packed decimal
 */

void
cob_move_display_to_packed(COB_RTD, cob_field *f1, cob_field *f2) {
    size_t          i;
    size_t          offset, startat, endat;
    /*COBOL-IT*/
    int             sign = COB_DUP_GET_SIGN(f1);
    size_t          digits1 = COB_FIELD_DIGITS(f1);
    size_t          digits2 = COB_FIELD_DIGITS(f2);
    int             scale1 = COB_FIELD_SCALE(f1);
    int             scale2 = COB_FIELD_SCALE(f2);
    unsigned char   *data1 = COB_FIELD_DATA(f1);
    unsigned char   *data2 = f2->data;
    unsigned char   *p;
    unsigned char   *lim1;
    unsigned char   n;
    int             allzero = 1;
    unsigned char   csp = CHAR_SP;

    memset(f2->data, 0, f2->size);
    if (COB_FIELD_PACKED_SIGN_MISSING(f2)) {
        offset = (digits2 % 2);
    } else {
        offset = 1 - (digits2 % 2);
    }
    startat = offset;
    endat = digits2 + offset;
    p = data1 + (digits1 - scale1) - (digits2 - scale2);
    lim1 = data1 + digits1;
    while ((data1 <= p && p < lim1) && COB_D2I(*p) == 0) {
        p++;
        startat++;
    }
    for (i = startat; i < endat; ++i, ++p) {
        if (data1 <= p && p < lim1) {
            if (*p == csp) {
                n = 0;
            } else {
                n = COB_D2I(*p);
            }
        } else {
            n = 0;
        }
        if (i % 2 == 0) {
            data2[i / 2] = n << 4;
        } else {
            data2[i / 2] |= n;
        }
        if (n != 0) {
            allzero = 0;
        }
    }

    if (allzero && (sign < 0)) {
        sign = 1;
    }
    if (!COB_FIELD_PACKED_SIGN_MISSING(f2)) {
        if (!COB_FIELD_HAVE_SIGN(f2)) {
            data2[digits2 / 2] |= 0x0f;
        } else {
            cob_real_put_sign(rtd, f2, sign);
        }
    }
}

void
cob_move_binary_to_packed(COB_RTD, cob_field *f1, cob_field *f2) {
    size_t              offset;
    /*COBOL-IT*/
    size_t              digits1 = COB_FIELD_DIGITS(f1);
    size_t              digits2 = COB_FIELD_DIGITS(f2);
    int                 scale1 = COB_FIELD_SCALE(f1);
    int                 scale2 = COB_FIELD_SCALE(f2);
    unsigned char       *data1;
    unsigned char       *data2 = f2->data;
    unsigned char       *p;
    unsigned char       n;
    int                 i, sign;
    unsigned long long  val;
    unsigned int        vali;
    long long           val2;
    unsigned char       buff[64];   /* long long is at most 20 digits */

    sign = 1;
    /* get value */
    if (COB_FIELD_HAVE_SIGN(f1)) {
        val2 = cob_binary_mget_int64(rtd, f1);
        if (val2 < 0) {
            sign = -1;
            val = -val2;
        } else {
            val = val2;
        }
    } else {
        val = cob_binary_mget_int64(rtd, f1);
    }

    if (scale1 ==  scale2) {
        cob_set_packed_long_long(f2, val);
    } else {
        /* convert to pseudo string */
        i = digits1;
        memset(buff, 0, sizeof(buff));
        if (f1->size > sizeof(unsigned int)) {
            while (val > 0 && i) {
                buff[--i] =  (val % 10);
                val /= 10;
            }
        } else {
            vali = (unsigned int)val;
            while (vali > 0 && i) {
                buff[--i] =  (vali % 10);
                vali /= 10;
            }
        }
        data1 = buff;
        p = data1 + (digits1 - scale1) - (digits2 - scale2);
        /* pack value */
        memset(f2->data, 0, f2->size);
        if (COB_FIELD_PACKED_SIGN_MISSING(f2)) {
            offset = (digits2 % 2);
        } else {
            offset = 1 - (digits2 % 2);
        }
        for (i = offset; i < digits2 + offset; i++, p++) {
            n = (data1 <= p && p < data1 + digits1) ? (*p) : 0;
            if (i % 2 == 0) {
                data2[i / 2] = n << 4;
            } else {
                data2[i / 2] |= n;
            }
        }
    }

    if (!COB_FIELD_PACKED_SIGN_MISSING(f2)) {
        if (!COB_FIELD_HAVE_SIGN(f2)) {
            data2[digits2 / 2] |= 0x0f;
        } else {
            cob_real_put_sign(rtd, f2, sign);
        }
    }
}

void
cob_move_packed_to_packed(COB_RTD, cob_field *f1, cob_field *f2) {
    size_t              offset1;
    size_t              offset2;
    /*COBOL-IT*/
    size_t              digits1 = COB_FIELD_DIGITS(f1);
    size_t              digits2 = COB_FIELD_DIGITS(f2);
    int                 scale1 = COB_FIELD_SCALE(f1);
    int                 scale2 = COB_FIELD_SCALE(f2);
    unsigned char       *data1 = f1->data;
    unsigned char       *data2 = f2->data;
    unsigned char       *p;
    unsigned char       n;
    int                 i, j;
    int                 allzero = 1;

    /* copy pack value */
    memset(f2->data, 0, f2->size);
    if (COB_FIELD_PACKED_SIGN_MISSING(f1)) {
        offset1 = (digits1 % 2);
    } else {
        offset1 = 1 - (digits1 % 2);
    }
    if (COB_FIELD_PACKED_SIGN_MISSING(f2)) {
        offset2 = (digits2 % 2);
    } else {
        offset2 = 1 - (digits2 % 2);
    }
    j = offset1 + (digits1 - scale1) - (digits2 - scale2);
    for (i = offset2; i < digits2 + offset2; i++, j++) {
        if ((j >= 0) && (j <  digits1 + offset1)) {
            if (j % 2 == 0) {
                n = (data1[j / 2]) >> 4;
            } else {
                n = (data1[j / 2]) & 0x0f;
            }
        } else {
            n = 0;
        }
        if (i % 2 == 0) {
            data2[i / 2] = n << 4;
        } else {
            data2[i / 2] |= n;
        }
        if (n != 0) {
            allzero = 0;
        }
    }

    if (!COB_FIELD_PACKED_SIGN_MISSING(f2)) {
        p = &data2[digits2 / 2];
        if (!COB_FIELD_HAVE_SIGN(f2)) {
            *p |= 0x0f;
        } else {
            if (COB_FIELD_PACKED_SIGN_MISSING(f1) || !COB_FIELD_HAVE_SIGN(f1)) {
                *p = (*p & 0xf0) | 0x0c;
            } else {
                if (allzero) {
                    *p = (*p & 0xf0) | 0x0c;
                } else {
                    *p |= (data1[digits1 / 2] & 0x0f);
                }
            }
        }
    }
}

#if !defined(CIT_DUAL_CHARSET)
    #if defined(CIT_EBCDIC_CHARSET)
static const packed_value_t *packed_char_value = packed_char_value_EBCDIC;
    #else
static const packed_value_t *packed_char_value = packed_char_value_ASCII;
    #endif
#else
static const packed_value_t *packed_char_value = packed_char_value_ASCII;
#endif

void
cob_move_init_charset_const(COB_RTD) {
#if defined(CIT_DUAL_CHARSET)
    if (rtd->ebcdic_charset) {
        packed_char_value = packed_char_value_EBCDIC;
    } else {
        packed_char_value = packed_char_value_ASCII;
    }
#endif
}

void
cob_move_packed_to_display(COB_RTD, cob_field *f1, cob_field *f2) {
    int             offset;
    /*COBOL-IT*/
    int             sign = COB_DUP_GET_SIGN(f1);
    register int    digits = COB_FIELD_DIGITS(f1);
    register unsigned char   *data = f1->data;
    unsigned char   buff[COB_MAX_NUMBER_DIGIT * 2];
    int             allzero;
    register unsigned char   *p;
    unsigned char   c0 = CHAR_0;
    int    i;

    /* unpack string */
    if (COB_FIELD_PACKED_SIGN_MISSING(f1)) {
        offset = (digits % 2);
    } else {
        offset = 1 - (digits % 2);
    }

    p = buff;
    if (offset) {
        i = *data & 0x0f;
        if (i > 9) {
            i = 0;
        }
        *p = COB_I2D_l(i);
        data++;
        p++;
        digits--;
    }

    while (digits > 0) {
        if (*data == 0) {
            *p = c0;
            p++;
            *p = c0;
            p++;
        } else {
            packed_value_t pv = packed_char_value[*data];
            *p = pv.t;
            p++;
            *p = pv.u;
            p++;
        }
        digits -= 2;
        data++;
    }

    /* store */
    allzero = cob_store_common_region(rtd, f2, buff, COB_FIELD_DIGITS(f1), COB_FIELD_SCALE(f1), 0);

    if (allzero && (sign < 0)) {
        sign = 1;
    }
    COB_PUT_SIGN(f2, sign);
}

void
cob_move_packed_to_binary(COB_RTD, cob_field *f1, cob_field *f2) {
    int                 sign;
    unsigned long long  val_ull;
    long long           val_sll;

    if (f1->attr->scale == f2->attr->scale) {

        sign = 1;
        /* get value */
        if (COB_FIELD_HAVE_SIGN(f1)) {
            val_sll = cob_packed_get_sll(rtd, f1, 0);
            if (val_sll < 0) {
                sign = -1;
                val_ull = -val_sll;
            } else {
                val_ull = val_sll;
            }
        } else {
            val_ull = cob_packed_get_ull(rtd, f1, 0);
        }
        if (rtd->current_module && rtd->current_module->flag_binary_truncate &&
            !COB_FIELD_REAL_BINARY(f2) && !(f2->attr->flags & COB_FLAG_BINARY_NOTRUNC)) {
            val_ull %= cob_exp10LL[(int)COB_FIELD_DIGITS(f2)];
        }

        if (sign < 0 && COB_FIELD_HAVE_SIGN(f2)) {
            val_sll = -val_ull;
            cob_binary_mset_int64(rtd, f2, val_sll);
        } else {
            cob_binary_mset_int64(rtd, f2, val_ull);
        }

        /* store */
    } else {
        indirect_move(rtd, cob_move_packed_to_display, f1, f2,
                      COB_FIELD_DIGITS(f1), COB_FIELD_SCALE(f1), 0);
    }
}

void
cob_move_packed_to_fp(COB_RTD, cob_field *f1, cob_field *f2) {
    unsigned long long  val_ull;
    long long           val_sll;
    double              d;
    int                 scale = COB_FIELD_SCALE(f1);

    /* get value */
    if (COB_FIELD_HAVE_SIGN(f1)) {
        val_sll = cob_packed_get_sll(rtd, f1, 0);
        d = val_sll;
    } else {
        val_ull = cob_packed_get_ull(rtd, f1, 0);
        d = val_ull;
    }
    if (scale > 0) {
        d = d / cob_exp10_double[scale];
    } else if (scale < 0) {
        d = d * cob_exp10_double[scale];
    }
    if (COB_FIELD_TYPE(f2) == COB_TYPE_NUMERIC_FLOAT) {
        float   flval = (float)d;
        memcpy(f2->data, (ucharptr)&flval, sizeof(float));
    } else {
        memcpy(f2->data, (ucharptr)&d, sizeof(double));
    }
}

/*
 * Floating point
 */
void
cob_move_binary_to_fp(COB_RTD, cob_field *f1, cob_field *f2) {
    double              d;
    int                 scale = COB_FIELD_SCALE(f1);

    /* get value */
    if (COB_FIELD_HAVE_SIGN(f1)) {
        d = cob_binary_mget_int64(rtd, f1);
    } else {
        d = (unsigned long long)cob_binary_mget_int64(rtd, f1);
    }
    if (scale > 0) {
        d = d / cob_exp10_double[scale];
    } else if (scale < 0) {
        d = d * cob_exp10_double[scale];
    }
    if (COB_FIELD_TYPE(f2) == COB_TYPE_NUMERIC_FLOAT) {
        float   flval = (float)d;
        memcpy(f2->data, (ucharptr)&flval, sizeof(float));
    } else {
        memcpy(f2->data, (ucharptr)&d, sizeof(double));
    }
}


void
cob_move_display_to_fp(COB_RTD, cob_field *f1, cob_field *f2) {
    double      val;
    int         size;
    /*COBOL-IT*/
    int         sign = COB_DUP_GET_SIGN(f1);
    int         size1 = COB_FIELD_SIZE(f1);
    char        *data1;
    char        *src;
    char        *x;
    char        local[256];
    char        buff2[COB_MAX_NUMBER_DIGIT * 2];

    memset((ucharptr)buff2, 0, sizeof(buff2));
    size = size1 - COB_FIELD_SCALE(f1);
    if (sign < 0) {
        buff2[0] = '-'; /*KEEP IN ASCII*/
        data1 = &buff2[1];
    } else {
        data1 = buff2;
    }
#if !defined(COB_EBCDIC_MACHINE) && defined(CIT_EBCDIC_CHARSET)
    src = (char *)STRING_E2A_DUP(rtd, (unsigned char *)f1->data, size1);
#else
    src = (char *)f1->data;
#endif
    if (COB_FIELD_SCALE(f1) <= 0) {
        sprintf(data1, "%*.*s.0", (int)size, (int)size, src);
    } else {
        sprintf(data1, "%*.*s.%*.*s", (int)size, (int)size, src,
                COB_FIELD_SCALE(f1), COB_FIELD_SCALE(f1), src + size);
    }
#ifdef	HAVE_SETLOCALE
    x = setlocale(LC_NUMERIC, NULL);
    if (x) {
        strcpy(local, x);
    } else {
        local[0] = 0;
    }
    setlocale(LC_NUMERIC, "C");
#endif
    sscanf(buff2, "%lf", &val);
#ifdef	HAVE_SETLOCALE
    if (local[0]) {
        setlocale(LC_NUMERIC, local);
    }
#endif
    if (COB_FIELD_TYPE(f2) == COB_TYPE_NUMERIC_FLOAT) {
        float   flval = (float)val;

        memcpy(f2->data, (ucharptr)&flval, sizeof(float));
    } else {
        memcpy(f2->data, (ucharptr)&val, sizeof(double));
    }
}

#define FAST_MUL_10(a) ((a << 3) + (a << 1))
static unsigned long long
cob_display_get_ull_raw(COB_RTD, unsigned char *data, int size) {
    register int                      i;
    register unsigned long long       val = 0;

    /* skip preceding zeros */
    for (i = 0; i < size; ++i, data++) {
        if (COB_D2I(*data) != 0) {
            break;
        }
    }

    /* get value */
    for (; i < size; ++i, data++) {
        val = FAST_MUL_10(val) + COB_D2I(*data);
    }
    return val;
}

void
cob_move_display_to_fp_opt(COB_RTD, cob_field *f1, cob_field *f2) {
    double      val;
    double      frac;
    /*COBOL-IT*/
    int         sign;
    int         size;
    int         scale = COB_FIELD_SCALE(f1);
    unsigned char  *data;

    if ((unlikely(rtd->current_module->display_sign) || rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) &&
        !rtd->ebcdic_charset) {
        sign = COB_DUP_GET_SIGN(f1);
        data = COB_FIELD_DATA(f1);
        size = COB_FIELD_SIZE(f1);
    } else {
        sign = cob_display_get_sign(rtd, f1, &data, &size);
    }

    val = (double)cob_display_get_ull_raw(rtd, data, size - scale);
    if (scale) {
        frac = (double)cob_display_get_ull_raw(rtd, data + (size - scale), scale);
        frac = frac / cob_exp10_double[scale];
        val = val + frac;
    }
    if (sign < 0) {
        val = -val;
    }
    if (COB_FIELD_TYPE(f2) == COB_TYPE_NUMERIC_FLOAT) {
        float   flval = (float)val;
        memcpy(f2->data, (ucharptr)&flval, sizeof(float));
    } else {
        memcpy(f2->data, (ucharptr)&val, sizeof(double));
    }
}

static double
cob_fp_get_double(cob_field *f1) {
    double val;
    if (COB_FIELD_TYPE(f1) == COB_TYPE_NUMERIC_FLOAT) {
        float   flval;

        memcpy((ucharptr)&flval, f1->data, sizeof(float));
        val = flval;
    } else {
        memcpy((ucharptr)&val, f1->data, sizeof(double));
    }
    return val;
}

void
cob_move_fp_to_binary(COB_RTD, cob_field *f1, cob_field *f2) {
    cob_field           temp;
    cob_field_attr      attr;
    int                 scale =  COB_FIELD_SCALE(f2);
    double              d;
    int                 sign = 1;

    if (scale <= 0) {
        d = cob_fp_get_double(f1);
        if (d < 0) {
            sign = -1;
            d = -d;
        }
        if (rtd->current_module &&  (rtd->current_module->runtime_flags & COB_FLAG_RT_ROUND_FP)) {
            d = d + 0.5;
        }
        if (COB_FIELD_HAVE_SIGN(f2)) {
            long long           vall;
            COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, 18, scale, COB_FLAG_HAVE_SIGN, NULL);
            temp.size = sizeof(vall);
            temp.data = (unsigned char *)&vall;
            temp.attr = &attr;
            vall = d;
            if (scale < 0) {
                vall = vall / cob_exp10LL[scale];
            }
            if (sign < 0) {
                vall = -vall;
            }
            cob_move_binary_to_binary(rtd, &temp, f2);
        } else {
            unsigned long long  valul;

            COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, 18, scale, 0, NULL);
            temp.size = sizeof(valul);
            temp.data = (unsigned char *)&valul;
            temp.attr = &attr;
            valul = d;
            if (scale < 0) {
                valul = valul / cob_exp10LL[scale];
            }
            cob_move_binary_to_binary(rtd, &temp, f2);
        }
    } else {
        if (rtd->current_module && rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION)
            indirect_move(rtd, cob_move_fp_to_display_opt, f1, f2, 40, (int)COB_FIELD_SCALE(f2), 0);
        else
            indirect_move(rtd, cob_move_fp_to_display, f1, f2, 40, (int)COB_FIELD_SCALE(f2), 0);
    }

}

void
cob_move_fp_to_display(COB_RTD, cob_field *f1, cob_field *f2) {
    double      val;
    double      frac;
    double      intgr;
    int         sign;
    int         decs;
    long long   res;
    unsigned char  *x, *y;
    unsigned char  buff[1024];
    unsigned char  buff2[1024];
    int         doround = 0;
    int         scale, allzero;

    if (COB_FIELD_TYPE(f1) == COB_TYPE_NUMERIC_FLOAT) {
        float   flval;

        memcpy((ucharptr)&flval, f1->data, sizeof(float));
        val = flval;
    } else {
        memcpy((ucharptr)&val, f1->data, sizeof(double));
    }
    sign = 1;
    if (val < 0) {
        sign = -1;
        val = -val;
    }
    frac = modf(val, &intgr);
    res = (long long)intgr;

    decs = 0;
    for (; res; res /= 10) {
        ++decs;
    }

    scale = 18 - decs;
    /*
#ifdef 	HAVE_SETLOCALE
    x = (unsigned char*)setlocale (LC_NUMERIC, NULL);
    if ( x ) {
        strcpy (local, (char*)x);
    } else {
        local[0] = 0;
    }
    setlocale (LC_NUMERIC, "C");
#endif
*/
    sprintf((char *)buff2, "0%-18.*lf", scale, val);
/*
#ifdef	HAVE_SETLOCALE
    if (local[0]) {
        setlocale (LC_NUMERIC, local);
    }
#endif
*/
#if !defined(COB_EBCDIC_MACHINE) && defined(CIT_EBCDIC_CHARSET)
    (void)STRING_A2E(buff2, strlen((char *)buff2));
#endif
    y = buff;
    for (x = buff2; *x; ++x) {
        if (*x == CHAR_DOT || *x == CHAR_COMMA) {
            if (rtd->current_module &&  (rtd->current_module->runtime_flags & COB_FLAG_RT_ROUND_FP)) {
                doround = 1;
            }
            continue;
        }
        if (*x == CHAR_SP) {
            continue;
        }
        *y++ = *x;
    }
    *y = 0;
    if (doround) {
        if (scale > COB_FIELD_SCALE(f2)) {
            int l = scale - COB_FIELD_SCALE(f2);
            if (l > 0) {
                l = strlen((char *)buff) - l;
                if (l > 0) {
                    if (buff[l] >= CHAR_5 && buff[l] <= CHAR_9) {
                        do {
                            l--;
                            buff[l]++;
                            if (buff[l] > CHAR_9) {
                                buff[l] = CHAR_0;
                            }
                        } while (l && buff[l] == CHAR_0);
                    }
                }
            }
        }
    }
    allzero = cob_store_common_region(rtd, f2, (ucharptr)buff, strlen((char *)buff), scale, 0);
    if (allzero && (sign < 0)) {
        sign = 1;
    }
    COB_PUT_SIGN(f2, sign);
}


void
cob_move_fp_to_display_opt(COB_RTD, cob_field *f1, cob_field *f2) {
    double      val;
    int         size;
    int         sign;
    int         carry = 0;
    unsigned char  buff[1024];
    int         doround = 0;
    int         scale, allzero;
    unsigned char *x;

    if (COB_FIELD_TYPE(f1) == COB_TYPE_NUMERIC_FLOAT) {
        float   flval;

        memcpy((ucharptr)&flval, f1->data, sizeof(float));
        val = flval;
    } else {
        memcpy((ucharptr)&val, f1->data, sizeof(double));
    }
    sign = 1;
    if (val < 0) {
        sign = -1;
        val = -val;
    }
    /*size = fpconv_dtoa(val,buff2);*/
    grisu2(val, (char *)buff, &size, &scale);
    buff[size] = 0;
    scale = -scale;

#if !defined(COB_EBCDIC_MACHINE) && defined(CIT_EBCDIC_CHARSET)
    (void)STRING_A2E(buff, size);
#endif
    if (rtd->current_module &&  (rtd->current_module->runtime_flags & COB_FLAG_RT_ROUND_FP)) {
        doround = scale > 0;
    }
    if (doround) {
        if (scale > COB_FIELD_SCALE(f2)) {
            int l = scale - COB_FIELD_SCALE(f2);
            if (l > 0) {
                l = size - l;
                if (l > 0) {
                    x = &buff[l];
                    if (*x >= CHAR_5 && *x <= CHAR_9) {
                        do {
                            l--;
                            x--;
                            (*x)++;
                            if (*x > CHAR_9) {
                                *x = CHAR_0;
                                carry=1;
                            } else
                                carry=0;
                        } while (l && carry);
                    }
                }
            }
        }
    }
    allzero = cob_store_common_region(rtd, f2, (ucharptr)buff, size, scale, 0);
    if (allzero && (sign < 0)) {
        sign = 1;
    }
    COB_PUT_SIGN(f2, sign);
}

/*
 * Binary integer
 */

void
cob_move_display_to_binary(COB_RTD, cob_field *f1, cob_field *f2) {
    int                 i;
    int                 size;
    int                 start  = 0;
    long long           val = 0;
    unsigned long long  val2 = 0;
    /*COBOL-IT*/
    int                 sign = COB_DUP_GET_SIGN(f1);
    int                 size1 = COB_FIELD_SIZE(f1);
    int                 size2 = COB_FIELD_DIGITS(f2);
    int                 scale1 = COB_FIELD_SCALE(f1);
    int                 scale2 = COB_FIELD_SCALE(f2);
    unsigned char       *data1 = COB_FIELD_DATA(f1);
    unsigned char       *p;
    int                 sizemin;

    /* get value */
    size = size1 - scale1 + scale2;
    if (rtd->current_module && rtd->current_module->flag_binary_truncate &&
        !COB_FIELD_REAL_BINARY(f2) && !(f2->attr->flags & COB_FLAG_BINARY_NOTRUNC)) {
        start = (size1 - scale1) - (size2 - scale2);
        if (start < 0) {
            start = 0;
        }
    } else {
        if (size > 18) {
            start = size - 18;
        }
    }
    if (rtd->current_module && rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION) {
        p = &data1[start];
        sizemin = min(size, size1);
        for (i = start;  (i < sizemin); ++i) {
            val = val * 10 + COB_D2I(*p);
            p++;
        }
        if (i < size) {
            val = val * cob_exp10LL[size - i];
        }
    } else {
        for (i = start; i < size; ++i) {
            if (i < size1) {
                val = val * 10 + COB_D2I(data1[i]);
            } else {
                val = val * 10;
            }
        }
    }
    /*
    if ( rtd->current_module && rtd->current_module->flag_binary_truncate &&
         !COB_FIELD_REAL_BINARY(f2) ) {
        val %= cob_exp10LL[(int)COB_FIELD_DIGITS(f2)];
    }
    */
    /* store */
    if (sign < 0 && COB_FIELD_HAVE_SIGN(f2)) {
        val2 = -val;
        cob_binary_mset_int64(rtd, f2, val2);
    } else {
        cob_binary_mset_int64(rtd, f2, val);
    }


}

#include "branchlut.c"
void
cob_move_binary_to_display(COB_RTD, cob_field *f1, cob_field *f2) {
    int                 i, sign, allzero;
    unsigned long long  val;
    long long           val2;
    char                buff[64];   /* long long is at most 20 digits */
    unsigned char       c0 = CHAR_0;

    sign = 1;
    /* get value */
    if (COB_FIELD_HAVE_SIGN(f1)) {
        val2 = cob_binary_mget_int64(rtd, f1);
        if (val2 < 0) {
            sign = -1;
            val = -val2;
        } else {
            val = val2;
        }
    } else {
        val = cob_binary_mget_int64(rtd, f1);
    }

    /* convert to string */
    if (rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION) {
        i = u64toa_branchlut(val, buff);
        /* store */
        allzero = cob_store_common_region(rtd, f2, (ucharptr)buff, i,
                                          COB_FIELD_SCALE(f1), 0);
    } else {
        i = 20;
        while (val > 0) {
            buff[--i] = (char)COB_I2D_l(val % 10);
            val /= 10;
        }

        /* store */
        allzero = cob_store_common_region(rtd, f2, (ucharptr)buff + i, (size_t)(20 - i),
                                          COB_FIELD_SCALE(f1), 0);
    }

    if (allzero && (sign < 0)) {
        sign = 1;
    }
    COB_PUT_SIGN(f2, sign);
}

/*
 * Edited
 */
int
cob_isdigit_char(COB_RTD, unsigned char c) {
    switch (SWITCH_CHAR(c)) {
        case CASE_CHAR(CHAR_0):
        case CASE_CHAR(CHAR_1):
        case CASE_CHAR(CHAR_2):
        case CASE_CHAR(CHAR_3):
        case CASE_CHAR(CHAR_4):
        case CASE_CHAR(CHAR_5):
        case CASE_CHAR(CHAR_6):
        case CASE_CHAR(CHAR_7):
        case CASE_CHAR(CHAR_8):
        case CASE_CHAR(CHAR_9):
            return 1;
        default:
            return 0;
    }
}

int
cob_isspace_char(COB_RTD, unsigned char c) {
    switch (SWITCH_CHAR(c)) {
        case CASE_CHAR(CHAR_SP):
        case CASE_CHAR(CHAR_TAB):
        case CASE_CHAR(CHAR_LF):
        case CASE_CHAR(CHAR_VT):
        case CASE_CHAR(CHAR_FF):
        case CASE_CHAR(CHAR_CR):
            return 1;
        default:
            return 0;
    }
}

int
cob_iszero_char(COB_RTD, cob_field *f, unsigned char c, int with_sp0) {
    int res = (c == CHAR_0);
    if (!res &&
        (COB_FIELD_BLANK_ZERO(f) ||
         ((with_sp0 && rtd->current_module && (rtd->current_module->runtime_flags & COB_FLAG_RT_MF_SPZERO))))
        && (c == CHAR_SP)) {
        res = 1;
    }
    return res;
}
static int isNotZero(COB_RTD, cob_field *f, unsigned char x) {
    return (x != 0 && !cob_iszero_char(rtd, f, x, 1));
}

void
cob_move_display_to_edited(COB_RTD, cob_field *f1, cob_field *f2) {
    const unsigned char *p;
    unsigned char   *min, *max, *src, *dst, *end;
    unsigned char   *decimal_point = NULL;
    /*COBOL-IT*/
    int             sign = COB_DUP_GET_SIGN(f1);
    int             neg = (sign < 0) ? 1 : 0;
    int             count = 0;
    int             count_sign = 1;
    int             count_curr = 1;
    int             trailing_sign = 0;
    int             trailing_curr = 0;
    int             is_zero = 1;
    int             suppress_zero = 1;
    int             sign_first = 0;
    int             p_is_left = 0;
    int             repeat;
    int             n;
    unsigned char   pad = CHAR_SP;
    unsigned char   pad_sign =  1;
    unsigned char   x;
    unsigned char   c;
    unsigned char   comma_pad = 0;
    unsigned char   next_comma_pad = 0;
    unsigned char   sign_symbol = 0;
    unsigned char   zero_sign_symbol = 0;
    unsigned char   curr_symbol = 0;
    unsigned char   ds, dp, cu, dp_a, ds_a, cu_a;
    int             sign_repeated = 0;


    dp = CHAR_DOT;
    ds = CHAR_COMMA;
    cu = CHAR_DOLLAR;
    if (rtd->current_module) {
        dp = rtd->current_module->decimal_point;
        ds = rtd->current_module->numeric_separator;
        cu = rtd->current_module->currency_symbol;
    }
    dp_a = E2A(dp);
    ds_a = E2A(ds);
    cu_a = E2A(cu);

    /* count the number of digit places before decimal point */
    for (p = COB_FIELD_PIC(f2); *p; p += 5) {
        c = p[0];
        memcpy((unsigned char *)&repeat, p + 1, sizeof(int));
        if (c == '9' || c == 'Z' || c == '*') {
            count += repeat;
            count_sign = 0;
            count_curr = 0;
        } else if (count_curr && c == cu_a) {
            count += repeat;
            sign_repeated += repeat;
        } else if (count_sign && (c == '+' || c == '-')) {
            count += repeat;
            sign_repeated += repeat;
        } else if (c == 'P') {
            if (count == 0) {
                p_is_left = 1;
                break;
            } else {
                count += repeat;
                count_sign = 0;
                count_curr = 0;
            }
        } else if (c == 'V' || c == dp_a) {
            break;
        }
    }
    comma_pad = pad;
    min = COB_FIELD_DATA(f1);
    max = min + COB_FIELD_SIZE(f1);
    src = max - COB_FIELD_SCALE(f1) - count;
    dst = f2->data;
    end = f2->data + f2->size;
    for (p = COB_FIELD_PIC(f2); *p;) {
        c = *p++;   /* PIC char */
        memcpy((unsigned char *)&n, p, sizeof(int));   /* PIC char count */
        p += sizeof(int);
        for (; n > 0; n--, ++dst) {
            next_comma_pad = pad;
            switch (c) {
                case '0':
                case '/':
                    *dst = A2E(c);
                    break;

                case 'B':
                    *dst = suppress_zero ? comma_pad : CHAR_B;
                    break;

                case 'P':
                    if (p_is_left) {
                        ++src;
                        --dst;
                    }
                    break;

                case '9':
                    *dst = (min <= src && src < max) ? *src++ : (src++, CHAR_0);
                    *dst = (isNotZero(rtd, f2, *dst)) ?   *dst : CHAR_0;
                    if (*dst != CHAR_0) {
                        is_zero = suppress_zero = 0;
                    }
                    suppress_zero = 0;
                    trailing_sign = 1;
                    trailing_curr = 1;
                    break;

                case 'V':
                    --dst;
                    decimal_point = dst;
                    break;

                case '.':
                case ',':
                    if (!decimal_point) {
                        if (c == dp_a) {
                            *dst = dp;
                            decimal_point = dst;
                        } else {
                            *dst = suppress_zero ? comma_pad : A2E(c);
                        }
                    } else {
                        *dst = A2E(c);
                    }

                    break;

                case 'C':
                case 'D':
                    end = dst;
                    if (neg) {
                        if (c == 'C') {
                            *dst++ = CHAR_C;
                            *dst   = CHAR_R;
                        } else {
                            *dst++ = CHAR_D;
                            *dst   = CHAR_B;
                        }
                    } else {
                        *dst++ = CHAR_SP;
                        *dst   = CHAR_SP;
                    }
                    /* memcpy (dst++, neg ? (c == CHAR_C ? "CR" : "DB") : "  ", 2);*/
                    break;

                case 'Z':
                case '*':
                    x = (min <= src && src < max) ? *src++ : (src++, CHAR_0);
//                    if ( !cob_iszero_char(rtd, f2, x, 1) && x!= 0) {
                    x = isNotZero(rtd, f2, x) ? x : CHAR_0;
                    if (x != CHAR_0) {
                        is_zero = suppress_zero = 0;
                    }
                    next_comma_pad = pad = (c == '*') ? CHAR_STAR : CHAR_SP;
                    *dst = suppress_zero ? pad : x;
                    trailing_sign = 1;
                    trailing_curr = 1;
                    break;

                case '+':
                case '-':
                    x = (min <= src && src < max) ? *src++ : (src++, CHAR_0);
//                    if ( !cob_iszero_char(rtd, f2, x, 1)) {
                    x = isNotZero(rtd, f2, x) ? x : CHAR_0;
                    if (sign_repeated == 1) {
                        /*ignore this one**/
                        sign_repeated = 0;
                    } else if (x != CHAR_0) {
                        is_zero = suppress_zero = 0;
                    }
                    if (trailing_sign) {
                        *dst = neg && !is_zero ? CHAR_MINUS : (c == '+') ? CHAR_PLUS : CHAR_SP;
                        --end;
                    } else if (dst == f2->data || suppress_zero) {
                        *dst = pad == CHAR_SP ? pad_sign : pad;
                        next_comma_pad = *dst;
                        zero_sign_symbol = (c == '+') ? CHAR_PLUS : CHAR_SP;
                        sign_symbol = neg ? CHAR_MINUS : zero_sign_symbol;
                        if (!curr_symbol) {
                            ++sign_first;
                        }
                    } else {
                        *dst = x;
                    }
                    break;

                default:
                    if (c == cu_a) {
                        x = (min <= src && src < max) ? *src++ : (src++, CHAR_0);
//                        if ( !cob_iszero_char(rtd, f2, x, 1)) {
                        x = isNotZero(rtd, f2, x) ? x : CHAR_0;
                        if (x != CHAR_0) {
                            is_zero = suppress_zero = 0;
                        }
                        if (trailing_curr) {
                            *dst = cu;
                            --end;
                        } else if (dst == f2->data || suppress_zero) {
                            *dst = pad == CHAR_SP ? pad_sign : pad;
                            next_comma_pad = *dst;
                            curr_symbol = cu;
                        } else {
                            *dst = x;
                        }
                        break;
                    }

                    *dst = CHAR_QUESTION; /* invalid PIC */
            }
            comma_pad = next_comma_pad;
        }
    }
    if (is_zero && zero_sign_symbol && sign_symbol) {
        sign_symbol = zero_sign_symbol;

    }
    if (suppress_zero || (is_zero && COB_FIELD_BLANK_ZERO(f2))) {
        /* all digits are zeros */
        if (pad == CHAR_SP || COB_FIELD_BLANK_ZERO(f2)) {
            memset(f2->data, CHAR_SP, f2->size);
        } else {
            for (dst = f2->data; dst < f2->data + f2->size; ++dst) {
                if (*dst != dp) {
                    *dst = pad;
                }
            }
        }
    } else {
        /* put zero after the decimal point if necessary */
        if (decimal_point) {
            for (dst = decimal_point + 1; dst < end; ++dst) {
                unsigned char d = *dst;
                if (!cob_isdigit_char(rtd, d) && (d != CHAR_DOT) && (d != CHAR_COMMA)
                    && (d != CHAR_PLUS) && (d != CHAR_MINUS) && (d != CHAR_SLASH) && (d != CHAR_B)) { /*!strchr (".,+-/B", *dst)*/
                    *dst = CHAR_0;
                }
            }
        }

        /* put sign or currency symbol at the beginning */
        if (sign_symbol || curr_symbol) {
            for (dst = end - 1; dst > f2->data; --dst) {
                if (*dst == pad_sign) {
                    break;
                }
            }
            if (sign_symbol && curr_symbol) {
                if (sign_first) {
                    *dst = curr_symbol;
                    --dst;
                    if (dst >= f2->data) {
                        *dst = sign_symbol;
                    }
                } else {
                    *dst = sign_symbol;
                    --dst;
                    if (dst >= f2->data) {
                        *dst = curr_symbol;
                    }
                }
            } else if (sign_symbol) {
                *dst = sign_symbol;
            } else {
                *dst = curr_symbol;
            }
        }
        for (dst = end - 1; dst >= f2->data; --dst) {
            if (*dst == pad_sign) {
                *dst = CHAR_SP;
            }
        }

        /* replace all 'B's by pad */
        count = 0;
        for (dst = f2->data; dst < end; ++dst) {
            if (*dst == CHAR_B) {
                if (count == 0) {
                    *dst = pad;
                } else {
                    *dst = CHAR_SP;
                }
            } else {
                ++count;
            }
        }
    }

}

void
cob_move_edited_to_display(COB_RTD, cob_field *f1, cob_field *f2) {
    size_t          i;
    int             sign = 0, allzero;
    int             scale = 0;
    int             count = 0;
    int             have_point = 0;
    int             cp;
    int             n;
    unsigned char   *p;
    const unsigned char      *p1;
    unsigned char   c;
    unsigned char   buff[COB_MAX_NUMBER_DIGIT * 2];
    unsigned char   dp;

    dp = CHAR_DOT;
    if (rtd->current_module) {
        dp = rtd->current_module->decimal_point;
    }

    p = buff;
    /* de-edit */
    for (i = 0; i < f1->size; ++i) {
        cp = f1->data[i];
        switch (SWITCH_CHAR(cp)) {
            case CASE_CHAR(CHAR_0):
            case CASE_CHAR(CHAR_1):
            case CASE_CHAR(CHAR_2):
            case CASE_CHAR(CHAR_3):
            case CASE_CHAR(CHAR_4):
            case CASE_CHAR(CHAR_5):
            case CASE_CHAR(CHAR_6):
            case CASE_CHAR(CHAR_7):
            case CASE_CHAR(CHAR_8):
            case CASE_CHAR(CHAR_9):
                *p++ = cp;
                if (have_point) {
                    ++scale;
                }
                break;
            case CASE_CHAR(CHAR_DOT):
            case CASE_CHAR(CHAR_COMMA):
                if (cp == dp) {
                    have_point = 1;
                }
                break;
            case CASE_CHAR(CHAR_MINUS):
            case CASE_CHAR(CHAR_C):
                sign = -1;
                break;
        }
    }
    /* count the number of digit places after decimal point in case of 'V', 'P' */
    if (scale == 0 && !have_point) {
        for (p1 = COB_FIELD_PIC(f1); *p1; p1 += 5) {
            c = p1[0];
            memcpy((unsigned char *)&n, p1 + 1, sizeof(int));
            if (c == '9'  || c == '0' || c == 'Z' || c == '*') {
                if (have_point) {
                    scale += n;
                } else {
                    count += n;
                }
            } else if (c == 'P') {
                if (count == 0) {
                    have_point = 1;
                    scale += n;
                } else {
                    scale -= n;
                }
            } else if (c == 'V') {
                have_point = 1;
            }
        }
    }

    /* store */
    allzero = cob_store_common_region(rtd, f2, buff, (size_t)(p - buff), scale, 0);

    if (allzero && (sign < 0)) {
        sign = 1;
    }
    COB_PUT_SIGN(f2, sign);
}

void
cob_move_alphanum_to_edited(COB_RTD, cob_field *f1, cob_field *f2) {
    const unsigned char      *p;
    unsigned char   *max, *src, *dst;

    int     n;
    unsigned char   c;
    /*COBOL-IT*/
    COB_DUP_GET_SIGN(f1);
    src = COB_FIELD_DATA(f1);
    max = src + COB_FIELD_SIZE(f1);
    dst = f2->data;
    for (p = COB_FIELD_PIC(f2); *p;) {
        c = *p++;   /* PIC char */
        memcpy((unsigned char *)&n, p, sizeof(int));   /* PIC char count */
        p += sizeof(int);
        for (; n > 0; --n) {
            switch (c) {
                case 'A':
                case 'X':
                case '9':
                    *dst++ = (src < max) ? *src++ : CHAR_SP;
                    break;
                case '0':
                case '/':
                    *dst++ = A2E(c);
                    break;
                case 'B':
                    *dst++ = CHAR_SP;
                    break;
                default:
                    *dst++ = CHAR_QUESTION;   /* invalid PIC */
            }
        }
    }
}

void
cob_move_all_char_to_edited(COB_RTD, cob_field *f1, cob_field *f2) {
    const unsigned char      *p;
    unsigned char   *max, *src, *dst, *src_save;

    int     n;
    unsigned char   c;
    /*COBOL-IT*/
    COB_DUP_GET_SIGN(f1);
    src_save = src = COB_FIELD_DATA(f1);
    max = src + COB_FIELD_SIZE(f1);
    dst = f2->data;
    for (p = COB_FIELD_PIC(f2); *p;) {
        c = *p++;   /* PIC char */
        memcpy((unsigned char *)&n, p, sizeof(int));   /* PIC char count */
        p += sizeof(int);
        for (; n > 0; --n) {
            switch (c) {
                case 'A':
                case 'X':
                case '9':
                    *dst++ = *src++;
                    if (src >= max)
                    {
                       src = src_save;
                    }
                    break;
                case '0':
                case '/':
                    *dst++ = A2E(c);
                    break;
                case 'B':
                    *dst++ = CHAR_SP;
                    break;
                default:
                    *dst++ = CHAR_QUESTION;   /* invalid PIC */
            }
        }
    }
}
/*
 * MOVE dispatcher
 */

static void
indirect_move(COB_RTD, void (*func)(COB_RTD, cob_field *src, cob_field *dst),
              cob_field *src, cob_field *dst, size_t size, int scale, int Alpha) {
    cob_field       temp;
    cob_field_attr  attr;
    unsigned char   data[COB_MAX_NUMBER_DIGIT * 2];
    unsigned char   *p = NULL;

    if (Alpha) {
        COB_ATTR_INIT(COB_TYPE_ALPHANUMERIC, size + 5, 0,
                      0, NULL);
        p = cob_malloc(rtd, size + 5);
        temp.data = p;
    } else {
        COB_ATTR_INIT(COB_TYPE_NUMERIC_DISPLAY, size, scale,
                      COB_FLAG_HAVE_SIGN, NULL);
        temp.data = data;
    }
    temp.size = size;
    temp.attr = &attr;
    func(rtd, src, &temp);
    cob_move(rtd, &temp, dst);
    if (p) {
        cob_free(p);
    }
}

static void
indirect_move_same(COB_RTD, cob_field *src, cob_field *dst) {
    cob_field       temp;
    cob_field_attr  attr;
    unsigned char   *p = NULL;

    attr = *(dst->attr);
    p = cob_malloc(rtd, dst->size);
    temp.data = p;
    temp.size = dst->size;
    temp.attr = &attr;
    cob_move(rtd, src, &temp);
    cob_move(rtd, &temp, dst);
    if (p) {
        cob_free(p);
    }
}


void
cob_move_all(COB_RTD, cob_field *src, cob_field *dst) {
    size_t              i;
    size_t              digcount;
    cob_field           temp;
    cob_field_attr      attr;

    COB_ATTR_INIT(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    if (COB_FIELD_IS_NUMERIC(dst)) {
        digcount = 18;
        attr.type = COB_TYPE_NUMERIC_DISPLAY;
        attr.digits = 18;
/*
        if (COB_FIELD_TYPE(dst) & COB_TYPE_NUMERIC_EDITED) {
            digcount = dst->size;
        } else {
            digcount = COB_FIELD_DIGITS(dst);
        }
*/
    } else {
        digcount = dst->size;
    }
    if (digcount > rtd->move_lastsize) {
        cob_free((rtd->move_lastdata));
        (rtd->move_lastdata) = cob_malloc(rtd, digcount);
        rtd->move_lastsize = digcount;
    }
    temp.size = digcount;
    temp.data = (rtd->move_lastdata);
    temp.attr = &attr;
    if (likely(src->size == 1)) {
        memset((rtd->move_lastdata), src->data[0], digcount);
    } else {
        for (i = 0; i < digcount; ++i) {
            (rtd->move_lastdata)[i] = src->data[i % src->size];
        }
    }

    cob_move(rtd, &temp, dst);
}

void
cob_move_binary_to_binary(COB_RTD, cob_field *f1, cob_field *f2) {
    int                 sign;
    unsigned long long  val;
    long long           val2;

    if (f1->attr->scale == f2->attr->scale) {

        sign = 1;
        /* get value */
        if (COB_FIELD_HAVE_SIGN(f1)) {
            val2 = cob_binary_mget_int64(rtd, f1);
            if (val2 < 0) {
                sign = -1;
                val = -val2;
            } else {
                val = val2;
            }
        } else {
            val = cob_binary_mget_int64(rtd, f1);
        }
        if (rtd->current_module) {
            if (rtd->current_module->runtime_flags & COB_FLAG_RT_COMPUTE_IBM) {
                if ((f2->attr->flags & COB_FLAG_BINARY_NOTRUNC)) {
                    val &= cob_maskLL[f2->size];
                } else {
                    if (rtd->current_module->flag_binary_truncate &&
                        !COB_FIELD_REAL_BINARY(f2)) {
                        val %= cob_exp10LL[(int)COB_FIELD_DIGITS(f2)];
                    }
                }
            } else {
                if (rtd->current_module->flag_binary_truncate &&
                    !COB_FIELD_REAL_BINARY(f2) && !(f2->attr->flags & COB_FLAG_BINARY_NOTRUNC)) {
                    val %= cob_exp10LL[(int)COB_FIELD_DIGITS(f2)];
                }
            }
        }

        /* store */
        if (sign < 0 && COB_FIELD_HAVE_SIGN(f2)) {
            val2 = -val;
            cob_binary_mset_int64(rtd, f2, val2);
        } else {
            cob_binary_mset_int64(rtd, f2, val);
        }

    } else {
        indirect_move(rtd, cob_move_binary_to_display, f1, f2,
                      20, COB_FIELD_SCALE(f1), 0);
    }
}

void
cob_move_fp_to_fp(COB_RTD, cob_field *f1, cob_field *f2) {
    double d;
    float f;

    if (COB_FIELD_TYPE(f1) == COB_TYPE_NUMERIC_FLOAT) {
        memcpy((ucharptr)&f, f1->data, sizeof(float));
        d = f;
    } else {
        memcpy((ucharptr)&d, f1->data, sizeof(double));
        f = d;
    }
    if (COB_FIELD_TYPE(f2) == COB_TYPE_NUMERIC_FLOAT) {
        memcpy(f2->data, (ucharptr)&f, sizeof(float));
    } else {
        memcpy(f2->data, (ucharptr)&d, sizeof(double));
    }
}

/*CIT*/
void
cb_fix_display_value(COB_RTD, cob_field *f) {
    int i, sign;

    switch (COB_FIELD_TYPE(f)) {
        case COB_TYPE_NUMERIC_DISPLAY :
            /*case COB_TYPE_NUMERIC_EDITED  :*/
            sign = COB_GET_SIGN(f);
            for (i = 0; i < f->size; i++) {
                if (f->data[i] == CHAR_SP) {
                    f->data[i] = CHAR_0;
                }
            }
            COB_PUT_SIGN(f, sign);
            break;
    }

}
void
cob_fieldlist_to_field(COB_RTD, cob_field *f) {
    int i;
    int p;
    if (f->attr->type == COB_TYPE_FIELD_LIST) {
        cob_field_list *fl = (cob_field_list *)f;
        p = 0;
        for (i = 0; i < fl->attr->fcount && p < fl->size; i++) {
            cob_keypart *kp = &fl->attr->part[i];
            COB_MEMCPY(kp->field->data, fl->data + p, kp->field->size);
            p += kp->field->size;
        }
    }
}

void
cob_fieldlist_from_field(COB_RTD, cob_field *f, char *recdata, char *target) {
    int i;
    int p;
    if (f->attr->type == COB_TYPE_FIELD_LIST) {
        cob_field_list *fl = (cob_field_list *)f;
        if (target == NULL) {
            target = (char *)fl->data;
        }
        p = 0;
        for (i = 0; i < fl->attr->fcount && p < fl->size; i++) {
            cob_keypart *kp = &fl->attr->part[i];
            if (recdata) {
                COB_MEMCPY(target + p, recdata + kp->start, kp->field->size);
            } else {
                COB_MEMCPY(target + p, kp->field->data, kp->field->size);
            }
            p += kp->field->size;
        }
    }
}


void cob_move_field_list(COB_RTD, cob_field *src, cob_field *dst) {
    if (COB_FIELD_TYPE(src) == COB_TYPE_FIELD_LIST) {
        cob_fieldlist_from_field(rtd, src, NULL, NULL);
    }
    cob_move_alphanum_to_alphanum(rtd, src, dst);
    if (COB_FIELD_TYPE(dst) == COB_TYPE_FIELD_LIST) {
        cob_fieldlist_to_field(rtd, dst);
    }
    return;
}

void
cob_move(COB_RTD, cob_field *src, cob_field *dst) {
    if (COB_FIELD_TYPE(src) == COB_TYPE_ALPHANUMERIC_ALL) {
        cob_move_all(rtd, src, dst);
        return;
    }
    if (dst->size == 0) {
        return;
    }
    if (src->size == 0) {
       if (COB_FIELD_IS_NUMERIC(dst)) {
          src = &cob_zero;
       }
       else
       {
          src = &cob_space;
       }
    }
    if ((src == dst) ||
        ((src->data == dst->data) && !COB_FIELD_IS_BITSARRAY(src))) {
        indirect_move_same(rtd, src, dst);
        return;
    }

    /* elementary move */
    switch (COB_FIELD_TYPE(src)) {
        case COB_TYPE_GROUP:
            cob_move_alphanum_to_alphanum(rtd, src, dst);
            break;
        case COB_TYPE_FIELD_LIST:
            cob_move_field_list(rtd, src, dst);
            break;
        case COB_TYPE_NUMERIC_DISPLAY:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_NUMERIC_FLOAT:
                case COB_TYPE_NUMERIC_DOUBLE:
                    cob_move_display_to_fp(rtd, src, dst);
                    return;
                case COB_TYPE_NUMERIC_DISPLAY:
                    cob_move_numeric_display_to_numeric_display(rtd, src, dst);
                    return;
                case COB_TYPE_NUMERIC_PACKED:
                    cob_move_display_to_packed(rtd, src, dst);
                    return;
                case COB_TYPE_NUMERIC_BINARY:
                case COB_TYPE_NUMERIC_BITS:
                    cob_move_display_to_binary(rtd, src, dst);
                    return;
                case COB_TYPE_NUMERIC_EDITED:
                    cob_move_display_to_edited(rtd, src, dst);
                    return;
                case COB_TYPE_ALPHANUMERIC_BITS:
                    cob_enterprise_move_display_to_abit(rtd, src, dst);
                    return;
                case COB_TYPE_BITS:
                case COB_TYPE_BITS_ARRAY:
                    cob_enterprise_move_display_to_bits(rtd, src, dst);
                    break;
                case COB_TYPE_ALPHANUMERIC_EDITED:
                    if (COB_FIELD_SCALE(src) < 0 ||
                        COB_FIELD_SCALE(src) > COB_FIELD_DIGITS(src)) {
                        /* expand P's */
                        indirect_move(rtd, cob_move_display_to_display, src, dst,
                                      (size_t)cob_max_int((int)COB_FIELD_DIGITS(src), (int)COB_FIELD_SCALE(src)),
                                      cob_max_int(0, (int)COB_FIELD_SCALE(src)), 0);
                        return;
                    } else {
                        cob_move_alphanum_to_edited(rtd, src, dst);
                        return;
                    }
                    break;
                case COB_TYPE_NATIONAL:
                case COB_TYPE_NATIONAL_EDITED:
                    indirect_move(rtd, cob_move_display_to_alphanum, src, dst,
                                  COB_FIELD_DIGITS(src), (int)COB_FIELD_SCALE(src), 1);
                    break;
                default:
                    if (rtd->current_module && (rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) &&
                        (!COB_FIELD_HAVE_SIGN(src))) {
                        cob_move_alphanum_to_alphanum_1(rtd, src, dst, 0, CHAR_SP);
                    } else {
                        cob_move_display_to_alphanum(rtd, src, dst);
                    }
                    return;
            }
            break;
        case COB_TYPE_NUMERIC_PACKED:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_NUMERIC_DISPLAY:
                    cob_move_packed_to_display(rtd, src, dst);
                    return;
                case COB_TYPE_NUMERIC_PACKED:
                    cob_move_packed_to_packed(rtd, src, dst);
                    return;
                case COB_TYPE_NUMERIC_BINARY:
                case COB_TYPE_NUMERIC_BITS:
                    cob_move_packed_to_binary(rtd, src, dst);
                    return;
                default:
                    indirect_move(rtd, cob_move_packed_to_display, src, dst,
                                  COB_FIELD_DIGITS(src), COB_FIELD_SCALE(src), 0);
                    return;
            }

        case COB_TYPE_NUMERIC_BINARY:
        case COB_TYPE_NUMERIC_BITS:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_NUMERIC_DISPLAY:
                    cob_move_binary_to_display(rtd, src, dst);
                    return;

                case COB_TYPE_NUMERIC_BINARY:
                case COB_TYPE_NUMERIC_BITS:
                    cob_move_binary_to_binary(rtd, src, dst);
                    return;

                case COB_TYPE_NUMERIC_PACKED:
                    cob_move_binary_to_packed(rtd, src, dst);
                    return;

                case COB_TYPE_NUMERIC_EDITED:
                case COB_TYPE_NUMERIC_FLOAT:
                case COB_TYPE_NUMERIC_DOUBLE:
                    indirect_move(rtd, cob_move_binary_to_display, src, dst,
                                  20, COB_FIELD_SCALE(src), 0);
                    return;
                default:
                    indirect_move(rtd, cob_move_binary_to_display, src, dst,
                                  COB_FIELD_DIGITS(src), COB_FIELD_SCALE(src), 0);
                    return;
            }

        case COB_TYPE_NUMERIC_EDITED:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_NUMERIC_DISPLAY:
                    cob_move_edited_to_display(rtd, src, dst);
                    return;
                case COB_TYPE_NUMERIC_PACKED:
                case COB_TYPE_NUMERIC_BINARY:
                case COB_TYPE_NUMERIC_EDITED:
                case COB_TYPE_NUMERIC_FLOAT:
                case COB_TYPE_NUMERIC_DOUBLE:
                case COB_TYPE_NUMERIC_BITS:
                    indirect_move(rtd, cob_move_edited_to_display, src, dst, 36, 18, 0);
                    return;
                case COB_TYPE_ALPHANUMERIC_EDITED:
                    cob_move_alphanum_to_edited(rtd, src, dst);
                    return;
                case COB_TYPE_NATIONAL:
                case COB_TYPE_NATIONAL_EDITED:
                    indirect_move(rtd, cob_move_alphanum_to_alphanum, src, dst,
                                  COB_FIELD_DIGITS(src), (int)COB_FIELD_SCALE(src), 1);

                default:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    return;
            }

        case COB_TYPE_NUMERIC_FLOAT:
        case COB_TYPE_NUMERIC_DOUBLE:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_NUMERIC_FLOAT:
                case COB_TYPE_NUMERIC_DOUBLE:
                    cob_move_fp_to_fp(rtd, src, dst);
                    break;
                default:
                    indirect_move(rtd, cob_move_fp_to_display, src, dst, 40, (int)COB_FIELD_SCALE(dst), 0);
                    break;
            }
            return;
        case COB_TYPE_ALPHANUMERIC_BITS:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_ALPHANUMERIC_BITS:
                    cob_move_alphanum_to_alphanum_1(rtd, src, dst, 1, CHAR_0);
                    break;
                default:
                    cob_enterprise_move_abit_to_field(rtd, src, dst);
                    break;
            }
            return;
            break;

        case COB_TYPE_BITS:
        case COB_TYPE_BITS_ARRAY:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                default:
                    cob_enterprise_move_bits_to_field(rtd, src, dst);
                    break;
            }
            return;
            break;

        case COB_TYPE_NATIONAL:
        case COB_TYPE_NATIONAL_EDITED:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_NATIONAL:
                case COB_TYPE_NATIONAL_EDITED:
                    cob_move_national_to_national(rtd, src, dst);
                    break;
                case COB_TYPE_ALPHANUMERIC:
                    cob_move_national_to_alphanum(rtd, src, dst);
                    break;
                default:
                    indirect_move(rtd, cob_move_national_to_alphanum, src, dst, src->size, 0, 1);
                    break;
            }
            break;
        default:
            switch (COB_FIELD_TYPE(dst)) {
                case COB_TYPE_FIELD_LIST:
                    cob_move_field_list(rtd, src, dst);
                    break;
                case COB_TYPE_GROUP:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    break;
                case COB_TYPE_NUMERIC_DISPLAY:
                    if (rtd->current_module) {
                        if ((rtd->current_module->runtime_flags & COB_FLAG_RT_MOVE_XTO9_ISO)) {
                            cob_move_alphanum_to_display(rtd, src, dst);
                        } else if (rtd->current_module->runtime_flags & COB_FLAG_RT_MOVE_XTO9_MVS &&
                                   (!COB_FIELD_HAVE_SIGN(dst))) {
                            cob_move_alphanum_to_alphanum_1(rtd, src, dst, 1, CHAR_SP);
                            cob_mvs_fix_lastdigit(rtd, dst);

                        } else if ((rtd->current_module->runtime_flags & COB_FLAG_RT_MOVE_XTO9_RAW) &&
                                   (!COB_FIELD_HAVE_SIGN(dst))) {
                            cob_move_alphanum_to_alphanum_1(rtd, src, dst, 1, CHAR_SP);
                        } else if ((rtd->current_module->runtime_flags & COB_FLAG_RT_MOVE_XTO9_RAW_0) &&
                                   (!COB_FIELD_HAVE_SIGN(dst))) {
                            cob_move_alphanum_to_alphanum_1(rtd, src, dst, 1, CHAR_0);
                        } else if ((rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) &&
                                   (!COB_FIELD_HAVE_SIGN(dst))) {
                            cob_move_alphanum_to_alphanum_1(rtd, src, dst, 1, CHAR_0);

                        } else if ((rtd->current_module->runtime_flags & COB_FLAG_RT_MOVE_XTO9_MF40) ||
                                   (rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_VMS)) {
                            cob_move_alphanum_to_display_mf40(rtd, src, dst);
                        } else {
                            cob_move_alphanum_to_display(rtd, src, dst);
                        }
                    } else {
                        cob_move_alphanum_to_display(rtd, src, dst);
                    }
                    return;
                case COB_TYPE_NUMERIC_PACKED:
                case COB_TYPE_NUMERIC_BINARY:
                case COB_TYPE_NUMERIC_EDITED:
                case COB_TYPE_NUMERIC_FLOAT:
                case COB_TYPE_NUMERIC_DOUBLE:
               case COB_TYPE_NUMERIC_BITS:
                  if (rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_VMS)
                     indirect_move(rtd, cob_move_alphanum_to_display_mf40, src, dst, 36, COB_FIELD_SCALE(dst), 0);
                  else
                     indirect_move(rtd, cob_move_alphanum_to_display, src, dst, 36, COB_FIELD_SCALE(dst), 0);
                    return;
                case COB_TYPE_ALPHANUMERIC_BITS:
                case COB_TYPE_BITS:
                case COB_TYPE_BITS_ARRAY:
                    indirect_move(rtd, cob_move_alphanum_to_display, src, dst, 40, 0, 0);
                    return;
                case COB_TYPE_ALPHANUMERIC_EDITED:
                    cob_move_alphanum_to_edited(rtd, src, dst);
                    return;
                case COB_TYPE_NATIONAL:
                case COB_TYPE_NATIONAL_EDITED:
                    cob_move_alphanum_to_national(rtd, src, dst);
                    return;
                default:
                    cob_move_alphanum_to_alphanum(rtd, src, dst);
                    return;
            }
    }
}

/*
 * Convenience functions
 */

static int
cob_packed_get_int(COB_RTD, cob_field *f1) {
    int             val = 0;
    int             sign = cob_packed_get_sign(f1);
    int             digits;
    int             flag;
    unsigned char   *data = f1->data;

    if (COB_FIELD_SCALE(f1) > 0) {
        digits = COB_FIELD_DIGITS(f1) - COB_FIELD_SCALE(f1);
        if (COB_FIELD_PACKED_SIGN_MISSING(f1)) {
            flag = 1 - COB_FIELD_SCALE(f1) % 2;
        } else {
            flag = COB_FIELD_SCALE(f1) % 2;
        }
    } else {
        digits = COB_FIELD_DIGITS(f1);
        flag = COB_FIELD_PACKED_SIGN_MISSING(f1);
    }
    val = (int)cob_packed_extract_uint(data, digits, flag);
    if (sign < 0) {
        val = -val;
    }
    return val;
}

static long long
cob_packed_get_sll(COB_RTD, cob_field *f1, int trunc) {
    long long       val = 0;
    int             sign = cob_packed_get_sign(f1);
    int             digits;
    int             flag;
    unsigned char   *data = f1->data;

    if (trunc && COB_FIELD_SCALE(f1) > 0) {
        digits = COB_FIELD_DIGITS(f1) - COB_FIELD_SCALE(f1);
        if (COB_FIELD_PACKED_SIGN_MISSING(f1)) {
            flag = 1 - COB_FIELD_SCALE(f1) % 2;
        } else {
            flag = COB_FIELD_SCALE(f1) % 2;
        }
    } else {
        digits = COB_FIELD_DIGITS(f1);
        flag = COB_FIELD_PACKED_SIGN_MISSING(f1);
    }
    val = (long long)cob_packed_extract_ull(data, digits, flag);
    if (sign < 0) {
        val = -val;
    }
    return val;
}

static unsigned long long
cob_packed_get_ull(COB_RTD, cob_field *f1, int trunc) {
    unsigned long long  val = 0;
    int             digits;
    int             flag;
    unsigned char   *data = f1->data;

    if (trunc && COB_FIELD_SCALE(f1) > 0) {
        digits = COB_FIELD_DIGITS(f1) - COB_FIELD_SCALE(f1);
        if (COB_FIELD_PACKED_SIGN_MISSING(f1)) {
            flag = 1 - COB_FIELD_SCALE(f1) % 2;
        } else {
            flag = COB_FIELD_SCALE(f1) % 2;
        }
    } else {
        digits = COB_FIELD_DIGITS(f1);
        flag = COB_FIELD_PACKED_SIGN_MISSING(f1);
    }
    val = cob_packed_extract_ull(data, digits, flag);
    return val;
}


static unsigned int
cob_display_get_uint_raw(COB_RTD, unsigned char *data, int size) {
    register int                      i;
    register unsigned int       val = 0;

    /* skip preceding zeros */
    for (i = 0; i < size; ++i, data++) {
        if (COB_D2I(*data) != 0) {
            break;
        }
    }

    /* get value */
    for (; i < size; ++i, data++) {
        val = FAST_MUL_10(val) + COB_D2I(*data);
    }
    return val;
}

static int
cob_display_get_int_opt(COB_RTD, cob_field *f) {
    int             val = 0;
    /*COBOL-IT*/
    int             sign;
    int             size;
    unsigned char   *data;
    int             scale = COB_FIELD_SCALE(f);

    if ((unlikely(rtd->current_module->display_sign) || rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) &&
        !rtd->ebcdic_charset) {
        sign = COB_DUP_GET_SIGN(f);
        data = COB_FIELD_DATA(f);
        size = COB_FIELD_SIZE(f);
    } else {
        sign = cob_display_get_sign(rtd, f, &data, &size);
    }
    /* get value */
    if (scale < 0) {
        val = (int)cob_display_get_uint_raw(rtd, data, size);
        val *= cob_exp10[(int)-scale];
    } else {
        size -= scale;
        val = (int)cob_display_get_uint_raw(rtd, data, size);
    }
    if (sign < 0) {
        val = -val;
    }

    return val;
}

static long long
cob_display_get_sll_opt(COB_RTD, cob_field *f) {
    long long       val = 0;
    /*COBOL-IT*/
    int             sign;
    int             size;
    unsigned char   *data;
    int             scale = COB_FIELD_SCALE(f);

    if ((unlikely(rtd->current_module->display_sign) || rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) &&
        !rtd->ebcdic_charset) {
        sign = COB_DUP_GET_SIGN(f);
        data = COB_FIELD_DATA(f);
        size = COB_FIELD_SIZE(f);
    } else {
        sign = cob_display_get_sign(rtd, f, &data, &size);
    }
    if (scale < 0) {
        val = (long long)cob_display_get_ull_raw(rtd, data, size);
        val *= cob_exp10LL[(int)-scale];
    } else {
        size -= scale;
        val = (long long)cob_display_get_ull_raw(rtd, data, size);
    }
    if (sign < 0) {
        val = -val;
    }
    return val;
}

unsigned long long
cob_display_get_ull_opt(COB_RTD, cob_field *f) {
    unsigned long long  val = 0;
    /*COBOL-IT*/
    int                 sign;
    int                 size;
    unsigned char       *data;
    int                 scale = COB_FIELD_SCALE(f);

    if ((unlikely(rtd->current_module->display_sign) || rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) &&
        !rtd->ebcdic_charset) {
        sign = COB_DUP_GET_SIGN(f);
        data = COB_FIELD_DATA(f);
        size = COB_FIELD_SIZE(f);
    } else {
        sign = cob_display_get_sign(rtd, f, &data, &size);
    }

    /* get value */
    if (scale < 0) {
        val = (long long)cob_display_get_ull_raw(rtd, data, size);
        val *= cob_exp10LL[(int)-scale];
    } else {
        size -= scale;
        val = (long long)cob_display_get_ull_raw(rtd, data, size);
    }
    if (sign < 0) {
        val = -val;
    }

    return val;
}

static int
cob_display_get_int(COB_RTD, cob_field *f) {
    size_t          i;
    int             val = 0;
    /*COBOL-IT*/
    int             sign = COB_DUP_GET_SIGN(f);
    size_t          size = COB_FIELD_SIZE(f);
    unsigned char   *data = COB_FIELD_DATA(f);

    /* skip preceding zeros */
    for (i = 0; i < size; ++i) {
        if (COB_D2I(data[i]) != 0) {
            break;
        }
    }

    /* get value */
    if (COB_FIELD_SCALE(f) < 0) {
        for (; i < size; ++i) {
            val = val * 10 + COB_D2I(data[i]);
        }
        val *= cob_exp10[(int)-COB_FIELD_SCALE(f)];
    } else {
        size -= COB_FIELD_SCALE(f);
        for (; i < size; ++i) {
            val = val * 10 + COB_D2I(data[i]);
        }
    }
    if (sign < 0) {
        val = -val;
    }

    return val;
}

static long long
cob_display_get_sll(COB_RTD, cob_field *f) {
    size_t          i;
    long long       val = 0;
    /*COBOL-IT*/
    int             sign = COB_DUP_GET_SIGN(f);
    size_t          size = COB_FIELD_SIZE(f);
    unsigned char   *data = COB_FIELD_DATA(f);

    /* skip preceding zeros */
    for (i = 0; i < size; ++i) {
        if (COB_D2I(data[i]) != 0) {
            break;
        }
    }

    /* get value */
    if (COB_FIELD_SCALE(f) < 0) {
        for (; i < size; ++i) {
            val = val * 10 + COB_D2I(data[i]);
        }
        val *= cob_exp10LL[(int)-COB_FIELD_SCALE(f)];
    } else {
        size -= COB_FIELD_SCALE(f);
        for (; i < size; ++i) {
            val = val * 10 + COB_D2I(data[i]);
        }
    }
    if (sign < 0) {
        val = -val;
    }

    return val;
}

unsigned long long
cob_display_get_ull(COB_RTD, cob_field *f) {
    size_t              i;
    unsigned long long  val = 0;
    /*COBOL-IT*/
    int                 sign = COB_DUP_GET_SIGN(f);
    size_t              size = COB_FIELD_SIZE(f);
    unsigned char       *data = COB_FIELD_DATA(f);

    /* skip preceding zeros */
    for (i = 0; i < size; ++i) {
        if (COB_D2I(data[i]) != 0) {
            break;
        }
    }

    /* get value */
    if (COB_FIELD_SCALE(f) < 0) {
        for (; i < size; ++i) {
            val = val * 10 + COB_D2I(data[i]);
        }
        val *= cob_exp10LL[(int)-COB_FIELD_SCALE(f)];
    } else {
        size -= COB_FIELD_SCALE(f);
        for (; i < size; ++i) {
            val = val * 10 + COB_D2I(data[i]);
        }
    }
    if (sign < 0) {
        val = -val;
    }

    return val;
}

void
cob_set_Cstring(COB_RTD, cob_field *f, char *s) {
    cob_field   temp;
    cob_field_attr  attr;

    COB_ATTR_INIT(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    temp.size = strlen(s);
    temp.data = (unsigned char *)s;
    temp.attr = &attr;
    cob_move(rtd, &temp, f);
}

void
cob_set_debugcp_string(COB_RTD, cob_field *f, char *s) {
    cob_field           temp;
    cob_field_attr      attr;
    char               *save_cp = rtd->display_code_page;
    int                 restore = 0;
    char                cp_buffer[COB_SMALL_BUFF];
    size_t              vallen;
    unsigned char      *d = NULL;

    COB_ATTR_INIT(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    temp.attr = &attr;
    if (COB_FIELD_IS_NATIONAL(f)) {
        temp.size = strlen(s);
        temp.data = (unsigned char *)s;
        rtd->display_code_page = (char *)"UTF-8";
        restore = 1;
    } else {
        vallen = strlen(s);
        d = cob_malloc(rtd, vallen * 4);
        strcpy(cp_buffer, cob_get_codepage(rtd));
        (void)cob_enterprise_open_icu(rtd, cp_buffer);
        temp.size = cob_enterprise_debugcp_to_display(rtd, s, (char *)d, cp_buffer, rtd->debug_codepage_iconv_cd, vallen, vallen * 4);
        temp.data = d;
    }
    cob_move(rtd, &temp, f);
    if (d) {
        free(d);
    }
    if (restore) {
        rtd->display_code_page =  save_cp;
    }
}

void
cob_set_int(COB_RTD, cob_field *f, int n) {
    cob_field       temp;
    cob_field_attr  attr;
    int             size = 9;

    /*
    cob_field       temp2;
    cob_field_attr  attr2;

    if (COB_FIELD_TYPE (f) == COB_TYPE_GROUP) {
        COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, size, 0, 0, NULL);
        attr2 = attr;
        temp2.size = f->size;;
        temp2.data = (unsigned char *)f->data;
        temp2.attr = &attr2;
        f = &temp2;
    }
    */

    if (f->size < size && (COB_FIELD_TYPE(f) == COB_TYPE_ALPHANUMERIC)) {
        size = f->size;
    }
    COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, size, 0, COB_FLAG_HAVE_SIGN, NULL);
    temp.size = 4;
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    cob_move(rtd, &temp, f);
}

void
cob_set_sll(COB_RTD, cob_field *f, signed long long n) {
    cob_field       temp;
    cob_field_attr  attr;
    int             size = 24;

    /*
    cob_field       temp2;
    cob_field_attr  attr2;

    if (COB_FIELD_TYPE (f) == COB_TYPE_GROUP) {
        COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, size, 0, 0, NULL);
        attr2 = attr;
        temp2.size = f->size;;
        temp2.data = (unsigned char *)f->data;
        temp2.attr = &attr2;
        f = &temp2;
    }
    */

    if (f->size < size && (COB_FIELD_TYPE(f) == COB_TYPE_ALPHANUMERIC)) {
        size = f->size;
    }

    COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, size, 0, COB_FLAG_HAVE_SIGN, NULL);
    temp.size = 8;
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    cob_move(rtd, &temp, f);
}

void
cob_set_ull(COB_RTD, cob_field *f, unsigned long long n) {
    cob_field       temp;
    cob_field_attr  attr;
    int             size = 24;

    /*
    cob_field       temp2;
    cob_field_attr  attr2;

    if (COB_FIELD_TYPE (f) == COB_TYPE_GROUP) {
        COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, size, 0, 0, NULL);
        attr2 = attr;
        temp2.size = f->size;;
        temp2.data = (unsigned char *)f->data;
        temp2.attr = &attr2;
        f = &temp2;
    }
    */
    if (f->size < size && (COB_FIELD_TYPE(f) == COB_TYPE_ALPHANUMERIC)) {
        size = f->size;
    }

    COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, size, 0, 0, NULL);
    temp.size = 8;
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    cob_move(rtd, &temp, f);
}

void
cob_set_float(COB_RTD, cob_field *f, float n) {
    cob_field   temp;
    cob_field_attr  attr;

    COB_ATTR_INIT(COB_TYPE_NUMERIC_FLOAT, f->attr->digits, f->attr->scale, COB_FLAG_HAVE_SIGN, NULL);
    temp.size = sizeof(float);
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    cob_move(rtd, &temp, f);
}

void
cob_set_double(COB_RTD, cob_field *f, double n) {
    cob_field   temp;
    cob_field_attr  attr;

    COB_ATTR_INIT(COB_TYPE_NUMERIC_DOUBLE, f->attr->digits, f->attr->scale, COB_FLAG_HAVE_SIGN, NULL);
    temp.size = sizeof(double);
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    cob_move(rtd, &temp, f);
}

double
cob_get_double(COB_RTD, cob_field *f) {
    cob_field   temp;
    cob_field_attr  attr;
    double n;

    COB_ATTR_INIT(COB_TYPE_NUMERIC_DOUBLE, f->attr->digits, f->attr->scale, COB_FLAG_HAVE_SIGN, NULL);
    temp.size = sizeof(double);
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    cob_move(rtd, f, &temp);
    return n;
}

float
cob_get_float(COB_RTD, cob_field *f) {
    return(float)cob_get_double(rtd, f);
}

int
cob_get_int(COB_RTD, cob_field *f) {
    int     n;
    cob_field   temp;
    cob_field_attr  attr;

    switch (COB_FIELD_TYPE(f)) {
        case COB_TYPE_GROUP:
        case COB_TYPE_NUMERIC_DISPLAY:
            if (rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION) {
                return cob_display_get_int_opt(rtd, f);
            } else {
                return cob_display_get_int(rtd, f);
            }
        case COB_TYPE_NUMERIC_BINARY:
        case COB_TYPE_NUMERIC_BITS:
            return(int)cob_binary_mget_int64(rtd, f);
        case COB_TYPE_NUMERIC_PACKED:
            return cob_packed_get_int(rtd, f);
        default:
            COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, 9, 0,
                          COB_FLAG_HAVE_SIGN, NULL);
            temp.size = 4;
            temp.data = (unsigned char *)&n;
            temp.attr = &attr;
            cob_move(rtd, f, &temp);
            return n;
    }
}

long long
cob_get_long_long(COB_RTD, cob_field *f) {
    long long   n;
    cob_field   temp;
    cob_field_attr  attr;

    switch (COB_FIELD_TYPE(f)) {
        case COB_TYPE_NUMERIC_DISPLAY:
            if (rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION) {
                return cob_display_get_sll_opt(rtd, f);
            } else {
                return cob_display_get_sll(rtd, f);
            }
        case COB_TYPE_NUMERIC_BINARY:
        case COB_TYPE_NUMERIC_BITS:
            return cob_binary_mget_int64(rtd, f);
        case COB_TYPE_NUMERIC_PACKED:
            return cob_packed_get_sll(rtd, f, 1);
        default:
            COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, 18, 0,
                          COB_FLAG_HAVE_SIGN, NULL);
            temp.size = 8;
            temp.data = (unsigned char *)&n;
            temp.attr = &attr;
            cob_move(rtd, f, &temp);
            return n;
    }
}

unsigned long long
cob_get_ull(COB_RTD, cob_field *f) {
    unsigned long long   n;
    cob_field           temp;
    cob_field_attr      attr;

    switch (COB_FIELD_TYPE(f)) {
        case COB_TYPE_NUMERIC_DISPLAY:
            if (rtd->current_module->runtime_flags & COB_FLAG_RT_OPTIMIZE_OPERATION) {
                return cob_display_get_ull_opt(rtd, f);
            } else {
                return cob_display_get_ull(rtd, f);
            }
        case COB_TYPE_NUMERIC_BINARY:
        case COB_TYPE_NUMERIC_BITS:
            return cob_binary_mget_int64(rtd, f);
        case COB_TYPE_NUMERIC_PACKED:
            return cob_packed_get_ull(rtd, f, 1);
        case COB_TYPE_BITS:
        case COB_TYPE_BITS_ARRAY:
            return cob_enterprise_bits_get_ull(rtd, f);
        default:
            COB_ATTR_INIT(COB_TYPE_NUMERIC_BINARY, 18, 0,
                          COB_FLAG_HAVE_SIGN, NULL);
            temp.size = 8;
            temp.data = (unsigned char *)&n;
            temp.attr = &attr;
            cob_move(rtd, f, &temp);
            return n;
    }
}

void
cob_init_move(COB_RTD) {
    (rtd->move_lastdata) = cob_malloc(rtd, COB_SMALL_BUFF);
    rtd->move_lastsize = COB_SMALL_BUFF;
}

void
cob_clear_move(COB_RTD) {
    cob_free(rtd->move_lastdata);
}

void
cob_put_sign_ascii(COB_RTD, unsigned char *p, const int sign) {
    int c = *p;
    switch (SWITCH_CHAR(c)) {
        case CASE_CHAR(CHAR_0):
            *p = (unsigned char)CHAR_p;
            return;
        case CASE_CHAR(CHAR_1):
            *p = (unsigned char)CHAR_q;
            return;
        case CASE_CHAR(CHAR_2):
            *p = (unsigned char)CHAR_r;
            return;
        case CASE_CHAR(CHAR_3):
            *p = (unsigned char)CHAR_s;
            return;
        case CASE_CHAR(CHAR_4):
            *p = (unsigned char)CHAR_t;
            return;
        case CASE_CHAR(CHAR_5):
            *p = (unsigned char)CHAR_u;
            return;
        case CASE_CHAR(CHAR_6):
            *p = (unsigned char)CHAR_v;
            return;
        case CASE_CHAR(CHAR_7):
            *p = (unsigned char)CHAR_w;
            return;
        case CASE_CHAR(CHAR_8):
            *p = (unsigned char)CHAR_x;
            return;
        case CASE_CHAR(CHAR_9):
            *p = (unsigned char)CHAR_y;
            return;
    }
}

void
cob_put_sign_ebcdic(COB_RTD, unsigned char *p, const int sign) {
    int c = *p;
    if (sign < 0) {
        switch (SWITCH_CHAR(c)) {
            case CASE_CHAR(CHAR_0):
                *p = (unsigned char)CHAR_CLOSEAC /*'}'*/;
                return;
            case CASE_CHAR(CHAR_1):
                *p = (unsigned char)CHAR_J;
                return;
            case CASE_CHAR(CHAR_2):
                *p = (unsigned char)CHAR_K;
                return;
            case CASE_CHAR(CHAR_3):
                *p = (unsigned char)CHAR_L;
                return;
            case CASE_CHAR(CHAR_4):
                *p = (unsigned char)CHAR_M;
                return;
            case CASE_CHAR(CHAR_5):
                *p = (unsigned char)CHAR_N;
                return;
            case CASE_CHAR(CHAR_6):
                *p = (unsigned char)CHAR_O;
                return;
            case CASE_CHAR(CHAR_7):
                *p = (unsigned char)CHAR_P;
                return;
            case CASE_CHAR(CHAR_8):
                *p = (unsigned char)CHAR_Q;
                return;
            case CASE_CHAR(CHAR_9):
                *p = (unsigned char)CHAR_R;
                return;
            default:
                /* What to do here */
                *p = (unsigned char)CHAR_CLOSEAC /*'}'*/;
                return;
        }
    }
    switch (SWITCH_CHAR(c)) {
        case CASE_CHAR(CHAR_0):
            *p = (unsigned char)CHAR_OPENAC /*'{'*/;
            return;
        case CASE_CHAR(CHAR_1):
            *p = (unsigned char)CHAR_A;
            return;
        case CASE_CHAR(CHAR_2):
            *p = (unsigned char)CHAR_B;
            return;
        case CASE_CHAR(CHAR_3):
            *p = (unsigned char)CHAR_C;
            return;
        case CASE_CHAR(CHAR_4):
            *p = (unsigned char)CHAR_D;
            return;
        case CASE_CHAR(CHAR_5):
            *p = (unsigned char)CHAR_E;
            return;
        case CASE_CHAR(CHAR_6):
            *p = (unsigned char)CHAR_F;
            return;
        case CASE_CHAR(CHAR_7):
            *p = (unsigned char)CHAR_G;
            return;
        case CASE_CHAR(CHAR_8):
            *p = (unsigned char)CHAR_H;
            return;
        case CASE_CHAR(CHAR_9):
            *p = (unsigned char)CHAR_I;
            return;
        default:
            /* What to do here */
            *p = (unsigned char)CHAR_OPENAC /*'{'*/;
            return;
    }
/* NOT REACHED */
}


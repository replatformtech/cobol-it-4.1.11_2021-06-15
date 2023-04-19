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

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
    #include <sys/time.h>
#endif
#include <errno.h>
#include <math.h>
#if defined(_WIN32) || defined(__CYGWIN__)
    #undef	HAVE_LANGINFO_CODESET
    #define WINDOWS_LEAN_AND_MEAN
    #include <windows.h>
#endif
#ifdef	HAVE_LANGINFO_CODESET
    #include <langinfo.h>
#endif
#ifdef	HAVE_LOCALE_H
    #include <locale.h>
#endif
#ifdef	_WIN32
    #include <sys/timeb.h>
#endif

#include "byteswap.h"
#include "common.h"
#include "coblocal.h"
#include "move.h"
#include "numeric.h"
#include "fileio.h"
#include "intrinsic.h"
#include "citenterprise.h"
#include "a2e.h"
#include "cob_decimal.h"
#define COB_FIELD_INIT(x,y,z)	do { \
	field.size = x; \
	field.data = y; \
	field.attr = z; \
	} while (0)


/* Constants for date/day calculations */
static const int normal_days[]      = {0,31,59,90,120,151,181,212,243,273,304,334,365};
static const int leap_days[]        = {0,31,60,91,121,152,182,213,244,274,305,335,366};
static const int normal_month_days[]= {0,31,28,31,30,31,30,31,31,30,31,30,31};
static const int leap_month_days[]  = {0,31,29,31,30,31,30,31,31,30,31,30,31};

/* Locale name to Locale ID table */
#if defined(_WIN32) || defined(__CYGWIN__)

struct winlocale {
    const char  *winlocalename;
    const int   winlocaleid;
};

static const struct winlocale   wintable[] =
{
    { "af_ZA",      0x0436},
    { "am_ET",      0x045e},
    { "ar_AE",      0x3801},
    { "ar_BH",      0x3c01},
    { "ar_DZ",      0x1401},
    { "ar_EG",      0x0c01},
    { "ar_IQ",      0x0801},
    { "ar_JO",      0x2c01},
    { "ar_KW",      0x3401},
    { "ar_LB",      0x3001},
    { "ar_LY",      0x1001},
    { "ar_MA",      0x1801},
    { "ar_OM",      0x2001},
    { "ar_QA",      0x4001},
    { "ar_SA",      0x0401},
    { "ar_SY",      0x2801},
    { "ar_TN",      0x1c01},
    { "ar_YE",      0x2401},
    { "arn_CL",     0x047a},
    { "as_IN",      0x044d},
    { "az_Cyrl_AZ",     0x082c},
    { "az_Latn_AZ",     0x042c},
    { "ba_RU",      0x046d},
    { "be_BY",      0x0423},
    { "bg_BG",      0x0402},
    { "bn_IN",      0x0445},
    { "bo_BT",      0x0851},
    { "bo_CN",      0x0451},
    { "br_FR",      0x047e},
    { "bs_Cyrl_BA",     0x201a},
    { "bs_Latn_BA",     0x141a},
    { "ca_ES",      0x0403},
    { "cs_CZ",      0x0405},
    { "cy_GB",      0x0452},
    { "da_DK",      0x0406},
    { "de_AT",      0x0c07},
    { "de_CH",      0x0807},
    { "de_DE",      0x0407},
    { "de_LI",      0x1407},
    { "de_LU",      0x1007},
    { "dsb_DE",     0x082e},
    { "dv_MV",      0x0465},
    { "el_GR",      0x0408},
    { "en_029",     0x2409},
    { "en_AU",      0x0c09},
    { "en_BZ",      0x2809},
    { "en_CA",      0x1009},
    { "en_GB",      0x0809},
    { "en_IE",      0x1809},
    { "en_IN",      0x4009},
    { "en_JM",      0x2009},
    { "en_MY",      0x4409},
    { "en_NZ",      0x1409},
    { "en_PH",      0x3409},
    { "en_SG",      0x4809},
    { "en_TT",      0x2c09},
    { "en_US",      0x0409},
    { "en_ZA",      0x1c09},
    { "en_ZW",      0x3009},
    { "es_AR",      0x2c0a},
    { "es_BO",      0x400a},
    { "es_CL",      0x340a},
    { "es_CO",      0x240a},
    { "es_CR",      0x140a},
    { "es_DO",      0x1c0a},
    { "es_EC",      0x300a},
    { "es_ES",      0x0c0a},
    { "es_GT",      0x100a},
    { "es_HN",      0x480a},
    { "es_MX",      0x080a},
    { "es_NI",      0x4c0a},
    { "es_PA",      0x180a},
    { "es_PE",      0x280a},
    { "es_PR",      0x500a},
    { "es_PY",      0x3c0a},
    { "es_SV",      0x440a},
    { "es_US",      0x540a},
    { "es_UY",      0x380a},
    { "es_VE",      0x200a},
    { "et_EE",      0x0425},
    { "eu_ES",      0x042d},
    { "fa_IR",      0x0429},
    { "fi_FI",      0x040b},
    { "fil_PH",     0x0464},
    { "fo_FO",      0x0438},
    { "fr_BE",      0x080c},
    { "fr_CA",      0x0c0c},
    { "fr_CH",      0x100c},
    { "fr_FR",      0x040c},
    { "fr_LU",      0x140c},
    { "fr_MC",      0x180c},
    { "fy_NL",      0x0462},
    { "ga_IE",      0x083c},
    { "gbz_AF",     0x048c},
    { "gl_ES",      0x0456},
    { "gsw_FR",     0x0484},
    { "gu_IN",      0x0447},
    { "ha_Latn_NG",     0x0468},
    { "he_IL",      0x040d},
    { "hi_IN",      0x0439},
    { "hr_BA",      0x101a},
    { "hr_HR",      0x041a},
    { "hu_HU",      0x040e},
    { "hy_AM",      0x042b},
    { "id_ID",      0x0421},
    { "ig_NG",      0x0470},
    { "ii_CN",      0x0478},
    { "is_IS",      0x040f},
    { "it_CH",      0x0810},
    { "it_IT",      0x0410},
    { "iu_Cans_CA",     0x045d},
    { "iu_Latn_CA",     0x085d},
    { "ja_JP",      0x0411},
    { "ka_GE",      0x0437},
    { "kh_KH",      0x0453},
    { "kk_KZ",      0x043f},
    { "kl_GL",      0x046f},
    { "kn_IN",      0x044b},
    { "ko_KR",      0x0412},
    { "kok_IN",     0x0457},
    { "ky_KG",      0x0440},
    { "lb_LU",      0x046e},
    { "lo_LA",      0x0454},
    { "lt_LT",      0x0427},
    { "lv_LV",      0x0426},
    { "mi_NZ",      0x0481},
    { "mk_MK",      0x042f},
    { "ml_IN",      0x044c},
    { "mn_Cyrl_MN",     0x0450},
    { "mn_Mong_CN",     0x0850},
    { "moh_CA",     0x047c},
    { "mr_IN",      0x044e},
    { "ms_BN",      0x083e},
    { "ms_MY",      0x043e},
    { "mt_MT",      0x043a},
    { "nb_NO",      0x0414},
    { "ne_NP",      0x0461},
    { "nl_BE",      0x0813},
    { "nl_NL",      0x0413},
    { "nn_NO",      0x0814},
    { "ns_ZA",      0x046c},
    { "oc_FR",      0x0482},
    { "or_IN",      0x0448},
    { "pa_IN",      0x0446},
    { "pl_PL",      0x0415},
    { "ps_AF",      0x0463},
    { "pt_BR",      0x0416},
    { "pt_PT",      0x0816},
    { "qut_GT",     0x0486},
    { "quz_BO",     0x046b},
    { "quz_EC",     0x086b},
    { "quz_PE",     0x0c6b},
    { "rm_CH",      0x0417},
    { "ro_RO",      0x0418},
    { "ru_RU",      0x0419},
    { "rw_RW",      0x0487},
    { "sa_IN",      0x044f},
    { "sah_RU",     0x0485},
    { "se_FI",      0x0c3b},
    { "se_NO",      0x043b},
    { "se_SE",      0x083b},
    { "si_LK",      0x045b},
    { "sk_SK",      0x041b},
    { "sl_SI",      0x0424},
    { "sma_NO",     0x183b},
    { "sma_SE",     0x1c3b},
    { "smj_NO",     0x103b},
    { "smj_SE",     0x143b},
    { "smn_FI",     0x243b},
    { "sms_FI",     0x203b},
    { "sq_AL",      0x041c},
    { "sr_Cyrl_BA",     0x1c1a},
    { "sr_Cyrl_CS",     0x0c1a},
    { "sr_Latn_BA",     0x181a},
    { "sr_Latn_CS",     0x081a},
    { "sv_FI",      0x081d},
    { "sv_SE",      0x041d},
    { "sw_KE",      0x0441},
    { "syr_SY",     0x045a},
    { "ta_IN",      0x0449},
    { "te_IN",      0x044a},
    { "tg_Cyrl_TJ",     0x0428},
    { "th_TH",      0x041e},
    { "tk_TM",      0x0442},
    { "tmz_Latn_DZ",    0x085f},
    { "tn_ZA",      0x0432},
    { "tr_IN",      0x0820},
    { "tr_TR",      0x041f},
    { "tt_RU",      0x0444},
    { "ug_CN",      0x0480},
    { "uk_UA",      0x0422},
    { "ur_PK",      0x0420},
    { "uz_Cyrl_UZ",     0x0843},
    { "uz_Latn_UZ",     0x0443},
    { "vi_VN",      0x042a},
    { "wen_DE",     0x042e},
    { "wo_SN",      0x0488},
    { "xh_ZA",      0x0434},
    { "yo_NG",      0x046a},
    { "zh_CN",      0x0804},
    { "zh_HK",      0x0c04},
    { "zh_MO",      0x1404},
    { "zh_SG",      0x1004},
    { "zh_TW",      0x0404},
    { "zu_ZA",      0x0435}
};

    #define	WINLOCSIZE	sizeof(wintable) / sizeof(struct winlocale)

#endif


/* Local functions */
static unsigned char * 
make_entry_allocate(COB_RTD, int sz) {
    unsigned char *s;
    rtd->curr_field = &(rtd->calc_field[rtd->curr_entry]);
    rtd->curr_attr = &(rtd->calc_attr[rtd->curr_entry]);
    if (rtd->curr_field->data == NULL || rtd->calc_size[rtd->curr_entry] < sz) {
        rtd->calc_size[rtd->curr_entry] =  sz;
        if (rtd->curr_field->data) {
            cob_free(rtd->curr_field->data);
        }
        s = cob_malloc(rtd, rtd->calc_size[rtd->curr_entry]);
        rtd->curr_field->data = s;
    } else {
        s = rtd->curr_field->data;
        memset(s, 0, sz);
    }
    return s;
}

static void COB_NOINLINE
make_double_entry (COB_RTD)
{
    unsigned char       *s;
    int  sz = sizeof (double);

    s = make_entry_allocate(rtd, sz +1);

    rtd->curr_attr->type = COB_TYPE_NUMERIC_DOUBLE;
    rtd->curr_attr->digits = 18;
    rtd->curr_attr->scale = 9;
    rtd->curr_attr->flags = COB_FLAG_HAVE_SIGN;
    rtd->curr_attr->pic = NULL;

    rtd->curr_field->size = sz;
    rtd->curr_field->data = s;
    rtd->curr_field->attr = rtd->curr_attr;

    if ( ++(rtd->curr_entry) >= DEPTH_LEVEL ) {
        rtd->curr_entry = 0;
    }
}


static void COB_NOINLINE
make_comp3_entry (COB_RTD, int digit, int scale)
{
    unsigned char       *s;
    int  sz;

    if (scale > COB_MAX_NUMBER_DIGIT/2) {
        scale = COB_MAX_NUMBER_DIGIT/2;
    }
    if (digit > COB_MAX_NUMBER_DIGIT) {
        digit = COB_MAX_NUMBER_DIGIT;
    }
    sz= ((digit) /2) +1;
    s = make_entry_allocate(rtd, sz);
    rtd->curr_attr->type = COB_TYPE_NUMERIC_PACKED;
    rtd->curr_attr->digits = digit;
    rtd->curr_attr->scale = scale;
    rtd->curr_attr->flags = COB_FLAG_HAVE_SIGN;
    rtd->curr_attr->pic = NULL;

    rtd->curr_field->size = sz;
    rtd->curr_field->data = s;
    rtd->curr_field->attr = rtd->curr_attr;

    if ( ++(rtd->curr_entry) >= DEPTH_LEVEL ) {
        rtd->curr_entry = 0;
    }
}

static void COB_NOINLINE
make_field_entry_base (COB_RTD, cob_field *f, int min_size)
{
    unsigned char *s;

    rtd->curr_field = &rtd->calc_field[rtd->curr_entry];
    rtd->curr_attr = &rtd->calc_attr[rtd->curr_entry];
    min_size +=10;
    /*keep space for final 0 */

    s = make_entry_allocate(rtd, min_size);

    *(rtd->curr_field) = *f;
    *(rtd->curr_attr) = *(f->attr);
    rtd->curr_field->data = s;
    rtd->curr_field->attr = rtd->curr_attr;

    if ( ++(rtd->curr_entry) >= DEPTH_LEVEL ) {
        rtd->curr_entry = 0;
    }
}

static void COB_NOINLINE
make_field_entry (COB_RTD, cob_field *f)
{
    make_field_entry_base(rtd,f,(f->size));
}

static int
leap_year (COB_RTD, const int year)
{
    return((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 1 : 0;
}

/* Leave in
static void
intr_set_double (cob_decimal *d, double v)
{
    mpz_set_d (d->value, v * 1.0e9);
    d->scale = 9;
}
*/

static double COB_NOINLINE
intr_get_double (COB_RTD, cob_decimal *d)
{
    double  v;
    int n;

    v = mpz_get_d (d->v_mpz);
    n = d->scale;
    if (n > 0) {
        while (n > 4) {
            v /= 1000;
            n -= 3;
        }
        for ( ; n > 0; --n ) v /= 10;
    } else {
        while (n < -4) {
            v *= 1000;
            n += 3;
        }
        for ( ; n < 0; ++n ) v *= 10;
    }
    return v;
}

static int
comp_field (COB_RTD, const void *m1, const void *m2)
{
    cob_field   *f1;
    cob_field   *f2;

    f1 = *(cob_field **) m1;
    f2 = *(cob_field **) m2;
    return cob_cmp (rtd, f1, f2);
}

static void COB_NOINLINE
calc_ref_mod (COB_RTD, cob_field *f, int offset, int length)
{
    size_t      calcoff;
    size_t      size;
    size_t      step=1;

    if (COB_FIELD_IS_NATIONAL(f) ) {
        offset *=2;
        length *=2;
        step=2;
    }
    if ( (size_t)offset <= f->size ) {
        calcoff = (size_t)offset - step;
        size = f->size - calcoff;
        if ( length > 0 && (size_t)length < size ) {
            size = (size_t)length;
        }
        f->size = size;
        if ( calcoff > 0 ) {
            memmove (f->data, f->data + calcoff, size);
        }
    }
}

/* Global functions */

/* Numeric expressions */

cob_field *
cob_intr_binop (COB_RTD, cob_field *f1, int op, cob_field *f2)
{
/* RXW
    size_t		bitnum;
    size_t		sign;
    size_t		attrsign;
    cob_field_attr	attr;
    cob_field	field;
*/  
    if (!f2 ) {
        return f1;
    }

    cob_decimal_set_field(rtd, &rtd->instrinsic_d1, f1);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f2);
    switch ( op ) {
        case '+':
            cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
            break;
        case '-':
            cob_decimal_sub (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
            break;
        case '*':
            cob_decimal_mul (&rtd->instrinsic_d1, &rtd->instrinsic_d2);
            break;
        case '/':
            cob_decimal_div (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
            break;
        case '^':
            cob_decimal_pow (&rtd->instrinsic_d1, &rtd->instrinsic_d2);
            break;
        default:
            break;
    }
    make_comp3_entry(rtd, cob_decimal_digits_count(&rtd->instrinsic_d1), rtd->instrinsic_d1.scale);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);

    return rtd->curr_field;
}

/* Intrinsics */

cob_field *
cob_intr_length (COB_RTD, cob_field *srcfield)
{
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    if ( COB_FIELD_IS_NATIONAL(srcfield) ) {
        cob_set_int (rtd, rtd->curr_field, (int)srcfield->size/2);
    } else {
        cob_set_int (rtd, rtd->curr_field, (int)srcfield->size);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_byte_length (COB_RTD, cob_field *srcfield)
{
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    cob_set_int (rtd, rtd->curr_field, (int)srcfield->size);
    return rtd->curr_field;
}

cob_field *
cob_intr_integer (COB_RTD, cob_field *srcfield)
{
    int scale, s,n,r;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    make_field_entry (rtd, &field);

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);

    if ( mpz_sgn (rtd->instrinsic_d1.v_mpz) >= 0 ) {
        cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
        return rtd->curr_field;
    }
    scale = 1;
    s = rtd->instrinsic_d1.scale;

    while (s > 0) {
        n = s;
        if (n > 9) {
            n = 9;
        }
        scale = cob_exp10[n];    
        r = mpz_fdiv_q_ui (rtd->instrinsic_d1.v_mpz, rtd->instrinsic_d1.v_mpz, (unsigned int)scale);
        s -= n;
    }
    cob_shift_decimal_no_scale(rtd,&rtd->instrinsic_d1,rtd->instrinsic_d1.scale);

    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_integer_part (COB_RTD, cob_field *srcfield)
{
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    make_field_entry (rtd, &field);

    cob_move (rtd, srcfield, rtd->curr_field);
    return rtd->curr_field;
}

cob_field *
cob_intr_fraction_part (COB_RTD, cob_field *srcfield)
{
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 18, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    make_field_entry (rtd, &field);

    cob_move (rtd, srcfield, rtd->curr_field);
    return rtd->curr_field;
}

cob_field *
cob_intr_sign (COB_RTD, cob_field *srcfield)
{
    int     n;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    cob_set_int (rtd, rtd->curr_field, 0);
    n = cob_cmp (rtd, srcfield, rtd->curr_field);
    if ( n < 0 ) {
        cob_set_int (rtd, rtd->curr_field, -1);
    } else if ( n > 0 ) {
        cob_set_int (rtd, rtd->curr_field, 1);
    }

    return rtd->curr_field;
}

cob_field *
cob_intr_upper_case (COB_RTD, const int offset, const int length, cob_field *srcfield)
{
    size_t      i, ssize, dsize;

    ssize = srcfield->size;
    dsize = ssize * 2;
    make_field_entry_base (rtd, srcfield, dsize);

    if(!COB_FIELD_IS_NATIONAL(srcfield)) {
        for ( i = 0; i < ssize; ++i ) {
           rtd->curr_field->data[i] = A2E(toupper (E2A(srcfield->data[i])));
        }
    }
    else{//NATIONAL FIELD
        /*Have to think about the dst size*/
        rtd->curr_field->size = cob_enterprise_int_uppercase_lowercase(rtd,rtd->curr_field->data,srcfield->data,dsize,ssize,1);
    }
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_lower_case (COB_RTD, const int offset, const int length, cob_field *srcfield)
{
    size_t      i, ssize, dsize;

    ssize = srcfield->size;
    dsize = ssize * 2;
    make_field_entry_base (rtd, srcfield, dsize);

    ssize = srcfield->size;
    if(!COB_FIELD_IS_NATIONAL(srcfield)) {
        for ( i = 0; i < ssize; ++i ) {
            rtd->curr_field->data[i] = A2E(tolower (E2A(srcfield->data[i])));
        }
    }
    else{
        rtd->curr_field->size = cob_enterprise_int_uppercase_lowercase(rtd,rtd->curr_field->data,srcfield->data,dsize,ssize,0);
    }
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_reverse (COB_RTD, const int offset, const int length, cob_field *srcfield)
{
    size_t      i, size, step;

    make_field_entry (rtd, srcfield);
    size = srcfield->size;
    step=1;
    if (COB_FIELD_IS_NATIONAL(srcfield)) {
        step=2;
    }

    for ( i = 0; i < size; i += step ) {
        rtd->curr_field->data[i] = srcfield->data[srcfield->size - i - step];
        if (step > 1) {
            rtd->curr_field->data[i+1] = srcfield->data[srcfield->size - i - step +1];
        }
    }
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_concatenate (COB_RTD, const int offset, const int length, const int params, ...)
{
    cob_field   **f;
    unsigned char   *p;
    size_t      calcsize;
    int     i;
    cob_field_attr  attr;
    cob_field   field;
    va_list     args;

    f = cob_malloc (rtd, params * sizeof (cob_field *));

    va_start (args, params);

    /* Extract args / calculate size */
    calcsize = 0;
    for ( i = 0; i < params; ++i ) {
        f[i] = va_arg (args, cob_field *);
        calcsize += f[i]->size;
    }

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (calcsize, NULL, &attr);
    make_field_entry (rtd, &field);

    p = rtd->curr_field->data;
    for ( i = 0; i < params; ++i ) {
        memcpy (p, f[i]->data, f[i]->size);
        p += f[i]->size;
    }

    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    cob_free (f);
    return rtd->curr_field;
}

cob_field *
cob_intr_substitute (COB_RTD, const int offset, const int length, const int params, ...)
{
    cob_field   *var;
    cob_field   **f1;
    cob_field   **f2;
    unsigned char   *p1;
    unsigned char   *p2;
    size_t      varsize;
    size_t      calcsize;
    size_t      n;
    size_t      found;
    int         numreps;
    int         i;
    cob_field_attr  attr;
    cob_field   field;
    va_list     args;

    numreps = params / 2;
    f1 = cob_malloc (rtd, numreps * sizeof (cob_field *));
    f2 = cob_malloc (rtd, numreps * sizeof (cob_field *));

    va_start (args, params);

    var = va_arg (args, cob_field *);
    varsize = var->size;

    /* Extract args */
    for ( i = 0; i < params - 1; ++i ) {
        if ( (i % 2) == 0 ) {
            f1[i / 2] = va_arg (args, cob_field *);
        } else {
            f2[i / 2] = va_arg (args, cob_field *);
        }
    }

    /* Calculate required size */
    calcsize = 0;
    found = 0;
    p1 = var->data;
    for ( n = 0; n < varsize; ) {
        for ( i = 0; i < numreps; ++i ) {
            if ( n + f1[i]->size <= varsize ) {
                if ( !memcmp (p1, f1[i]->data, f1[i]->size) ) {
                    p1 += f1[i]->size;
                    n += f1[i]->size;
                    calcsize += f2[i]->size;
                    found = 1;
                    break;
                }
            }
        }
        if ( found ) {
            found = 0;
            continue;
        }
        ++n;
        ++p1;
        ++calcsize;
    }

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    field.size = calcsize;
    make_field_entry (rtd, &field);

    found = 0;
    p1 = var->data;
    p2 = rtd->curr_field->data;
    for ( n = 0; n < varsize; ) {
        for ( i = 0; i < numreps; ++i ) {
            if ( n + f1[i]->size <= varsize ) {
                if ( !memcmp (p1, f1[i]->data, f1[i]->size) ) {
                    memcpy (p2, f2[i]->data, f2[i]->size);
                    p1 += f1[i]->size;
                    p2 += f2[i]->size;
                    n += f1[i]->size;
                    found = 1;
                    break;
                }
            }
        }
        if ( found ) {
            found = 0;
            continue;
        }
        ++n;
        *p2++ = *p1++;
    }
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    cob_free (f1);
    cob_free (f2);
    return rtd->curr_field;
}

cob_field *
cob_intr_substitute_case (COB_RTD, const int offset, const int length, const int params, ...)
{
    cob_field   *var;
    cob_field   **f1;
    cob_field   **f2;
    unsigned char   *p1;
    unsigned char   *p2;
    size_t      varsize;
    size_t      calcsize;
    size_t      n;
    size_t      found;
    int         numreps;
    int         i;
    cob_field_attr  attr;
    cob_field   field;
    va_list     args;

    numreps = params / 2;
    f1 = cob_malloc (rtd, numreps * sizeof (cob_field *));
    f2 = cob_malloc (rtd, numreps * sizeof (cob_field *));

    va_start (args, params);

    var = va_arg (args, cob_field *);
    varsize = var->size;

    /* Extract args */
    for ( i = 0; i < params - 1; ++i ) {
        if ( (i % 2) == 0 ) {
            f1[i / 2] = va_arg (args, cob_field *);
        } else {
            f2[i / 2] = va_arg (args, cob_field *);
        }
    }

    /* Calculate required size */
    calcsize = 0;
    found = 0;
    p1 = var->data;
    for ( n = 0; n < varsize; ) {
        for ( i = 0; i < numreps; ++i ) {
            if ( n + f1[i]->size <= varsize ) {
                if ( !strncasecmp ((const char *)p1,
                                   (const char *)(f1[i]->data),
                                   f1[i]->size) ) {
                    p1 += f1[i]->size;
                    n += f1[i]->size;
                    calcsize += f2[i]->size;
                    found = 1;
                    break;
                }
            }
        }
        if ( found ) {
            found = 0;
            continue;
        }
        ++n;
        ++p1;
        ++calcsize;
    }

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    field.size = calcsize;
    make_field_entry (rtd, &field);

    found = 0;
    p1 = var->data;
    p2 = rtd->curr_field->data;
    for ( n = 0; n < varsize; ) {
        for ( i = 0; i < numreps; ++i ) {
            if ( n + f1[i]->size <= varsize ) {
                if ( !strncasecmp ((const char *)p1,
                                   (const char *)(f1[i]->data),
                                   f1[i]->size) ) {
                    memcpy (p2, f2[i]->data, f2[i]->size);
                    p1 += f1[i]->size;
                    p2 += f2[i]->size;
                    n += f1[i]->size;
                    found = 1;
                    break;
                }
            }
        }
        if ( found ) {
            found = 0;
            continue;
        }
        ++n;
        *p2++ = *p1++;
    }
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    cob_free (f1);
    cob_free (f2);
    return rtd->curr_field;
}

cob_field *
cob_intr_trim (COB_RTD, const int offset, const int length,
               cob_field *srcfield, const int direction)
{
    unsigned char   *begin;
    unsigned char   *end;
    size_t          i;
    size_t          size = 0;

    make_field_entry (rtd, srcfield);

    if (COB_FIELD_IS_NATIONAL(srcfield) ) {
        const unsigned char *sp = cob_utf16_space(rtd); 
        for ( i = 0; i < srcfield->size-1; i+=2 ) {
            if ( srcfield->data[i] != *sp || 
                 srcfield->data[i+1] != *(sp+1) ) {
                break;
            }
        }
        if ( i == srcfield->size ) {
            if ( (rtd->current_module->runtime_flags & COB_FLAG_RT_ZERO_LEN_TRIM) ) {
                rtd->curr_field->size = 0;
            } else {
                rtd->curr_field->size = 2;
            }
            rtd->curr_field->data[0] = *sp;
            rtd->curr_field->data[1] = *(sp+1);
            return rtd->curr_field;
        }
        begin = srcfield->data;
        if ( direction != 2 ) {
            for ( ; *begin == *sp && *(begin+1) == *(sp+1); begin+=2 ) ;
        }
        end = srcfield->data + srcfield->size - 2;
        if ( direction != 1 ) {
            for ( ; *end == *sp && *(end+1) == *(sp+1); end-=2 ) ;
        }
        end++;
    } else {
        for ( i = 0; i < srcfield->size; ++i ) {
            if ( srcfield->data[i] != CHAR_SP ) {
                break;
            }
        }
        if ( i == srcfield->size ) {
            if ( (rtd->current_module->runtime_flags & COB_FLAG_RT_ZERO_LEN_TRIM) ) {
                rtd->curr_field->size = 0;
            } else {
                rtd->curr_field->size = 1;
            }
            rtd->curr_field->data[0] = CHAR_SP;
            return rtd->curr_field;
        }
        begin = srcfield->data;
        if ( direction != 2 ) {
            for ( ; *begin == CHAR_SP; ++begin ) ;
        }
        end = srcfield->data + srcfield->size - 1;
        if ( direction != 1 ) {
            for ( ; *end == CHAR_SP; end-- ) ;
        }
        for ( i = 0; begin <= end; ++begin, ++i ) {
            rtd->curr_field->data[i] = *begin;
            ++size;
        }
    }
    for ( i = 0; begin <= end; ++begin, ++i ) {
        rtd->curr_field->data[i] = *begin;
        ++size;
    }
    rtd->curr_field->size = size;
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_triml (COB_RTD, const int offset, const int length, cob_field *srcfield)
{
    return cob_intr_trim(rtd,offset,length,srcfield,1);
}

cob_field *
cob_intr_trimr (COB_RTD, const int offset, const int length, cob_field *srcfield)
{
    return cob_intr_trim(rtd,offset,length,srcfield,2);
}

cob_field *
cob_intr_exception_file (COB_RTD)
{
    size_t          flen;
    cob_field_attr  attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    if ( rtd->cob_exception_code == 0 || !rtd->cob_error_file ||
         (rtd->cob_exception_code & 0x0500) != 0x0500 ) {
        field.size = 2;
        make_field_entry (rtd, &field);
        memcpy (rtd->curr_field->data, "00", 2);
    } else {
        mf_extfh_FCD  *fcd = (mf_extfh_FCD *)((cob_file_extfh*)(rtd->cob_error_file))->extfh_ptr;
        flen = strlen (((cob_file_extfh*)(rtd->cob_error_file))->select_name);
        field.size = flen + 2;
        make_field_entry (rtd, &field);
        memcpy (rtd->curr_field->data, fcd->user_file_status, 2);
        memcpy (&(rtd->curr_field->data[2]), ((cob_file_extfh*)(rtd->cob_error_file))->select_name, flen);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_exception_location (COB_RTD)
{
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    if ( !rtd->cob_got_exception || !rtd->cob_orig_program_id ) {
        field.size = 1;
        make_field_entry (rtd, &field);
        *(rtd->curr_field->data) = CHAR_SP;
        return rtd->curr_field;
    }
    memset (rtd->instrinsic_buff, 0, COB_SMALL_BUFF);
    if ( rtd->cob_orig_section && rtd->cob_orig_paragraph ) {
        snprintf (rtd->instrinsic_buff, COB_SMALL_MAX, "%s; %s OF %s; %d",
                  rtd->cob_orig_program_id, rtd->cob_orig_paragraph,
                  rtd->cob_orig_section, rtd->cob_orig_line);
    } else if ( rtd->cob_orig_section ) {
        snprintf (rtd->instrinsic_buff, COB_SMALL_MAX, "%s; %s; %d",
                  rtd->cob_orig_program_id, rtd->cob_orig_section, rtd->cob_orig_line);
    } else if ( rtd->cob_orig_paragraph ) {
        snprintf (rtd->instrinsic_buff, COB_SMALL_MAX, "%s; %s; %d",
                  rtd->cob_orig_program_id, rtd->cob_orig_paragraph, rtd->cob_orig_line);
    } else {
        snprintf (rtd->instrinsic_buff, COB_SMALL_MAX, "%s; ; %d",
                  rtd->cob_orig_program_id, rtd->cob_orig_line);
    }
    field.size = strlen (rtd->instrinsic_buff);
    make_field_entry (rtd, &field);
    memcpy (rtd->curr_field->data, rtd->instrinsic_buff, field.size);
    return rtd->curr_field;
}

cob_field *
cob_intr_exception_status (COB_RTD)
{
    const char  *except_name;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (31, NULL, &attr);
    make_field_entry (rtd, &field);

    memset (rtd->curr_field->data, CHAR_SP, 31);
    if ( rtd->cob_exception_code ) {
        except_name = cob_get_exception_name (rtd, rtd->cob_exception_code);
        if ( except_name == NULL ) {
            except_name = "EXCEPTION-OBJECT";
        }
        memcpy (rtd->curr_field->data, except_name, strlen (except_name));
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_exception_statement (COB_RTD)
{
    size_t      flen;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (31, NULL, &attr);
    make_field_entry (rtd, &field);

    memset (rtd->curr_field->data, CHAR_SP, 31);
    if ( rtd->cob_exception_code && rtd->cob_orig_statement ) {
        flen = strlen (rtd->cob_orig_statement);
        if ( flen > 31 ) {
            memcpy (rtd->curr_field->data, rtd->cob_orig_statement, 31);
        } else {
            memcpy (rtd->curr_field->data, rtd->cob_orig_statement, flen);
        }
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_when_compiled (COB_RTD, const int offset, const int length, cob_field *f)
{
    make_field_entry (rtd, f);

    memcpy (rtd->curr_field->data, f->data, f->size);
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_current_date (COB_RTD, const int offset, const int length)
{
#if	defined(_WIN32) && !defined(__CYGWIN__)
    long        contz;
    struct tm   *tmptr;
    struct _timeb   tmb;
    cob_field_attr  attr;
    cob_field   field;
#else
#if !defined(__linux__) && !defined(__CYGWIN__) && !defined(COB_STRFTIME) && defined(HAVE_TIMEZONE)
    struct tm   *tmptr;
    long        contz;
#endif
    time_t      curtime;
    cob_field_attr  attr;
    cob_field   field;
#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)
    struct timeval  tmv;
    char        buff2[8];
#endif
#endif	/* _WIN32 */
    char        buff[24];

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (21, NULL, &attr);
    make_field_entry (rtd, &field);
    memset (buff, 0, sizeof(buff));

#if	defined(_WIN32) && !defined(__CYGWIN__)
    _ftime (&tmb);
    tmptr = cob_localtime (&(tmb.time));
    if ( tmb.timezone <= 0 ) {
        contz = -tmb.timezone;
        snprintf (buff, 23,
                  "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d%2.2d+%2.2ld%2.2ld",
                  tmptr->tm_year + 1900, tmptr->tm_mon + 1, tmptr->tm_mday,
                  tmptr->tm_hour, tmptr->tm_min, tmptr->tm_sec,
                  tmb.millitm / 100, contz / 60, contz % 60);
    } else {
        contz = tmb.timezone;
        snprintf (buff, 23,
                  "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d%2.2d-%2.2ld%2.2ld",
                  tmptr->tm_year + 1900, tmptr->tm_mon + 1, tmptr->tm_mday,
                  tmptr->tm_hour, tmptr->tm_min, tmptr->tm_sec,
                  tmb.millitm / 100, contz / 60, contz % 60);
    }
#else
#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)
    gettimeofday (&tmv, NULL);
    curtime = tmv.tv_sec;
#else
    curtime = time (NULL);
#endif

#if defined(__linux__) || defined(__CYGWIN__) || defined(COB_STRFTIME)
    strftime (buff, 22, "%Y%m%d%H%M%S00%z", cob_localtime (&curtime));
#elif defined(HAVE_TIMEZONE)
    tmptr = cob_localtime (&curtime);
    strftime (buff, 17, "%Y%m%d%H%M%S00", tmptr);
    /* RXW - Hack for DST - Need something better */
    if ( tmptr->tm_isdst > 0 ) {
        timezone -= 3600;
    }
    if ( timezone <= 0 ) {
        contz = -timezone;
        buff[16] = '+';
    } else {
        contz = timezone;
        buff[16] = '-';
    }
    sprintf(&buff[17], "%2.2ld%2.2ld", contz / 3600, (contz % 3600) / 60);
#else
    strftime (buff, 22, "%Y%m%d%H%M%S0000000", cob_localtime (&curtime));
#endif

#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)
    snprintf(buff2, 7, "%2.2ld", tmv.tv_usec / 10000);
    memcpy (&buff[14], buff2, 2);
#endif
#endif	/* _WIN32 */

    (void)STRING_A2E((unsigned char*)buff,21);
    memcpy (rtd->curr_field->data, buff, 21);
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_char (COB_RTD, cob_field *srcfield)
{
    int             i;
    cob_field_attr  attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (1, NULL, &attr);
    make_field_entry (rtd, &field);

    i = cob_get_int (rtd, srcfield);
    if ( i < 1 || i > 256 ) {
        *rtd->curr_field->data = 0;
    } else {
        *rtd->curr_field->data = i - 1;
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_cstring (COB_RTD, cob_field *srcfield)
{
    make_field_entry (rtd, srcfield);
    cob_move(rtd, srcfield, rtd->curr_field);
    return rtd->curr_field;
}

cob_field *
cob_intr_ord (COB_RTD, cob_field *srcfield)
{
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    cob_set_int (rtd, rtd->curr_field, (int)(*srcfield->data + 1));
    return rtd->curr_field;
}

cob_field *
cob_intr_stored_char_length (COB_RTD, cob_field *srcfield)
{
    unsigned char   *p;
    int     count;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    count = srcfield->size;
    p = srcfield->data + srcfield->size - 1;
    for ( ; count > 0; count--, p-- ) {
        if ( *p != CHAR_SP ) {
            break;
        }
    }
    cob_set_int (rtd, rtd->curr_field, count);
    return rtd->curr_field;
}

cob_field *
cob_intr_combined_datetime (COB_RTD, cob_field *srcdays, cob_field *srctime)
{
    int     srdays;
    int     srtime;
    cob_field_attr  attr;
    cob_field   field;
    char        buff[16];

    COB_ATTR_INIT (COB_TYPE_NUMERIC_DISPLAY, 12, 5, 0, NULL);
    COB_FIELD_INIT (12, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    srdays = cob_get_int (rtd, srcdays);
    if ( srdays < 1 || srdays > 3067671 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        memset (rtd->curr_field->data, CHAR_0, 12);
        return rtd->curr_field;
    }
    srtime = cob_get_int (rtd, srctime);
    if ( srtime < 1 || srtime > 86400 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        memset (rtd->curr_field->data, CHAR_0, 12);
        return rtd->curr_field;
    }
    snprintf (buff, 15, "%7.7d%5.5d", srdays, srtime);
    memcpy (rtd->curr_field->data, buff, 12);
    return rtd->curr_field;
}

cob_field *
cob_intr_date_of_integer (COB_RTD, cob_field *srcdays)
{
    int     i;
    int     days;
    int     baseyear = 1601;
    int     leapyear = 365;
    cob_field_attr  attr;
    cob_field   field;
    char        buff[16];

    COB_ATTR_INIT (COB_TYPE_NUMERIC_DISPLAY, 8, 0, 0, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    /* Base 1601-01-01 */
    days = cob_get_int (rtd, srcdays);
    if ( days < 1 || days > 3067671 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        memset (rtd->curr_field->data, CHAR_0, 8);
        return rtd->curr_field;
    }
    while ( days > leapyear ) {
        days -= leapyear;
        ++baseyear;
        if ( leap_year (rtd, baseyear) ) {
            leapyear = 366;
        } else {
            leapyear = 365;
        }
    }
    for ( i = 0; i < 13; ++i ) {
        if ( leap_year (rtd, baseyear) ) {
            if ( days <= leap_days[i] ) {
                days -= leap_days[i-1];
                break;
            }
        } else {
            if ( days <= normal_days[i] ) {
                days -= normal_days[i-1];
                break;
            }
        }
    }
    snprintf (buff, 15, "%4.4d%2.2d%2.2d", baseyear, i, days);
    memcpy (rtd->curr_field->data, buff, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_day_of_integer (COB_RTD, cob_field *srcdays)
{
    int     days;
    int     baseyear = 1601;
    int     leapyear = 365;
    cob_field_attr  attr;
    cob_field   field;
    char        buff[16];

    COB_ATTR_INIT (COB_TYPE_NUMERIC_DISPLAY, 7, 0, 0, NULL);
    COB_FIELD_INIT (7, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    /* Base 1601-01-01 */
    days = cob_get_int (rtd, srcdays);
    if ( days < 1 || days > 3067671 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        memset (rtd->curr_field->data, CHAR_0, 7);
        return rtd->curr_field;
    }
    while ( days > leapyear ) {
        days -= leapyear;
        ++baseyear;
        if ( leap_year (rtd, baseyear) ) {
            leapyear = 366;
        } else {
            leapyear = 365;
        }
    }
    snprintf (buff, 15, "%4.4d%3.3d", baseyear, days);
    memcpy (rtd->curr_field->data, buff, 7);
    return rtd->curr_field;
}

cob_field *
cob_intr_integer_of_date (COB_RTD, cob_field *srcfield)
{
    int     indate;
    int     days;
    int     totaldays;
    int     month;
    int     year;
    int     baseyear = 1601;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    /* Base 1601-01-01 */
    indate = cob_get_int (rtd, srcfield);
    year = indate / 10000;
    if ( year < 1601 || year > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    indate %= 10000;
    month = indate / 100;
    if ( month < 1 || month > 12 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    days = indate % 100;
    if ( days < 1 || days > 31 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    if ( leap_year (rtd, year) ) {
        if ( days > leap_month_days[month] ) {
            cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
            cob_set_int (rtd, rtd->curr_field, 0);
            return rtd->curr_field;
        }
    } else {
        if ( days > normal_month_days[month] ) {
            cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
            cob_set_int (rtd, rtd->curr_field, 0);
            return rtd->curr_field;
        }
    }
    totaldays = 0;
    while ( baseyear != year ) {
        if ( leap_year (rtd, baseyear) ) {
            totaldays += 366;
        } else {
            totaldays += 365;
        }
        ++baseyear;
    }
    if ( leap_year (rtd, baseyear) ) {
        totaldays += leap_days[month - 1];
    } else {
        totaldays += normal_days[month - 1];
    }
    totaldays += days;
    cob_set_int (rtd, rtd->curr_field, totaldays);
    return rtd->curr_field;
}

cob_field *
cob_intr_integer_of_day (COB_RTD, cob_field *srcfield)
{
    int     indate;
    int     days;
    int     totaldays;
    int     year;
    int     baseyear = 1601;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    /* Base 1601-01-01 */
    indate = cob_get_int (rtd, srcfield);
    year = indate / 1000;
    if ( year < 1601 || year > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    days = indate % 1000;
    if ( days < 1 || days > 365 + leap_year (rtd, year) ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    totaldays = 0;
    while ( baseyear != year ) {
        if ( leap_year (rtd, baseyear) ) {
            totaldays += 366;
        } else {
            totaldays += 365;
        }
        ++baseyear;
    }
    totaldays += days;
    cob_set_int (rtd, rtd->curr_field, totaldays);
    return rtd->curr_field;
}

cob_field *
cob_intr_test_date_yyyymmdd (COB_RTD, cob_field *srcfield)
{
    int     indate;
    int     days;
    int     month;
    int     year;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    /* Base 1601-01-01 */
    indate = cob_get_int (rtd, srcfield);
    year = indate / 10000;
    if ( year < 1601 || year > 9999 ) {
        cob_set_int (rtd, rtd->curr_field, 1);
        return rtd->curr_field;
    }
    indate %= 10000;
    month = indate / 100;
    if ( month < 1 || month > 12 ) {
        cob_set_int (rtd, rtd->curr_field, 2);
        return rtd->curr_field;
    }
    days = indate % 100;
    if ( days < 1 || days > 31 ) {
        cob_set_int (rtd, rtd->curr_field, 3);
        return rtd->curr_field;
    }
    if ( leap_year (rtd, year) ) {
        if ( days > leap_month_days[month] ) {
            cob_set_int (rtd, rtd->curr_field, 3);
            return rtd->curr_field;
        }
    } else {
        if ( days > normal_month_days[month] ) {
            cob_set_int (rtd, rtd->curr_field, 3);
            return rtd->curr_field;
        }
    }
    cob_set_int (rtd, rtd->curr_field, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_test_day_yyyyddd (COB_RTD, cob_field *srcfield)
{
    int     indate;
    int     days;
    int     year;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    /* Base 1601-01-01 */
    indate = cob_get_int (rtd, srcfield);
    year = indate / 1000;
    if ( year < 1601 || year > 9999 ) {
        cob_set_int (rtd, rtd->curr_field, 1);
        return rtd->curr_field;
    }
    days = indate % 1000;
    if ( days < 1 || days > 365 + leap_year (rtd, year) ) {
        cob_set_int (rtd, rtd->curr_field, 2);
        return rtd->curr_field;
    }
    cob_set_int (rtd, rtd->curr_field, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_factorial (COB_RTD, cob_field *srcfield)
{
    int     srcval;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, 0, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    srcval = cob_get_int (rtd, srcfield);
    if ( srcval < 0 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    rtd->instrinsic_d1.scale = 0;
    mpz_fac_ui (rtd->instrinsic_d1.v_mpz, (unsigned int)srcval);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_exp (COB_RTD, cob_field *srcfield)
{
    double      mathd2;

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_double_entry (rtd);

    errno = 0;
    mathd2 = pow (2.7182818284590452354, intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    memcpy (rtd->curr_field->data, (char *)&mathd2, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_exp10 (COB_RTD, cob_field *srcfield)
{
    double      mathd2;

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_double_entry (rtd);

    errno = 0;
    mathd2 = pow (10.0, intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    memcpy (rtd->curr_field->data, (char *)&mathd2, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_abs (COB_RTD, cob_field *srcfield)
{

    make_field_entry (rtd, srcfield);

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    mpz_abs (rtd->instrinsic_d1.v_mpz, rtd->instrinsic_d1.v_mpz);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_acos (COB_RTD, cob_field *srcfield)
{
    unsigned long long  result;
    double          mathd2;
    int         i, tempres;
    cob_field_attr      attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 17, 0, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_field_entry (rtd, &field);

    errno = 0;
    mathd2 = acos (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }

    result = (unsigned long long) mathd2;
    mathd2 -= result;
    for ( i = 0; i < 17; ++i ) {
        mathd2 *= 10;
        tempres = (int) mathd2;
        result *= 10;
        result += tempres;
        mathd2 -= tempres;
    }
    memcpy (rtd->curr_field->data, (char *)&result, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_asin (COB_RTD, cob_field *srcfield)
{
    long long       result;
    double          mathd2;
    int         i, tempres;
    cob_field_attr      attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 17, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_field_entry (rtd, &field);

    errno = 0;
    mathd2 = asin (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    result = (long long) mathd2;
    mathd2 -= result;
    for ( i = 0; i < 17; ++i ) {
        mathd2 *= 10;
        tempres = (int) mathd2;
        result *= 10;
        result += tempres;
        mathd2 -= tempres;
    }
    memcpy (rtd->curr_field->data, (char *)&result, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_atan (COB_RTD, cob_field *srcfield)
{
    long long       result;
    double          mathd2;
    int         i, tempres;
    cob_field_attr      attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 17, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_field_entry (rtd, &field);

    errno = 0;
    mathd2 = atan (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    result = (long long) mathd2;
    mathd2 -= result;
    for ( i = 0; i < 17; ++i ) {
        mathd2 *= 10;
        tempres = (int) mathd2;
        result *= 10;
        result += tempres;
        mathd2 -= tempres;
    }
    memcpy (rtd->curr_field->data, (char *)&result, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_cos (COB_RTD, cob_field *srcfield)
{
    long long       result;
    double          mathd2;
    int         i, tempres;
    cob_field_attr      attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 17, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_field_entry (rtd, &field);

    errno = 0;
    mathd2 = cos (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    result = (long long) mathd2;
    mathd2 -= result;
    for ( i = 0; i < 17; ++i ) {
        mathd2 *= 10;
        tempres = (int) mathd2;
        result *= 10;
        result += tempres;
        mathd2 -= tempres;
    }
    memcpy (rtd->curr_field->data, (char *)&result, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_log (COB_RTD, cob_field *srcfield)
{
    double      mathd2;

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_double_entry (rtd);

    errno = 0;
    mathd2 = log (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    memcpy (rtd->curr_field->data, (char *)&mathd2, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_log10 (COB_RTD, cob_field *srcfield)
{
    double      mathd2;

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_double_entry (rtd);

    errno = 0;
    mathd2 = log10 (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    memcpy (rtd->curr_field->data, (char *)&mathd2, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_sin (COB_RTD, cob_field *srcfield)
{
    long long       result;
    double          mathd2;
    int         i, tempres;
    cob_field_attr      attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 17, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_field_entry (rtd, &field);

    errno = 0;
    mathd2 = sin (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    result = (long long) mathd2;
    mathd2 -= result;
    for ( i = 0; i < 17; ++i ) {
        mathd2 *= 10;
        tempres = (int) mathd2;
        result *= 10;
        result += tempres;
        mathd2 -= tempres;
    }
    memcpy (rtd->curr_field->data, (char *)&result, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_sqrt (COB_RTD, cob_field *srcfield)
{
    double      mathd2;

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_double_entry (rtd);

    errno = 0;
    mathd2 = sqrt (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    memcpy (rtd->curr_field->data, (char *)&mathd2, 8);
    return rtd->curr_field;
}

cob_field *
cob_intr_tan (COB_RTD, cob_field *srcfield)
{
    double      mathd2;

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield);
    make_double_entry (rtd);

    errno = 0;
    mathd2 = tan (intr_get_double (rtd, &rtd->instrinsic_d1));
    if ( errno ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    memcpy (rtd->curr_field->data, (char *)&mathd2, 8);
    return rtd->curr_field;
}

static int test_numval(COB_RTD, unsigned char * data, int size, char has_cs, cob_field *currency)
{
    int i;
    char state = 1;
    char sign = 0;
    char cr[3];
    char db[3];
    char cx[3];
    /******************************************************************************** 
     * 
     *  state:
     * 
     *    1      2     3     4    5      6      7      8      9     10    11
     *  space--sign--space--cs--space--digit--point--digit--space--ind--space
     *
     *  sign : + -               
     *  ind  : + - CR DB         
     *  digit: 1-9
     *  point: decimal point 
     *  cs   : currency symbol (NUMVAL-C only)
     * 
     ********************************************************************************/ 
    // printf("data : %.*s\n", size, data); 
    strcpy (cr, "CR");
    strcpy (db, "DB");
    (void)STRING_A2E((unsigned char*)cr, 2);
    (void)STRING_A2E((unsigned char*)db, 2);
    for (i = 0; i < size; ++i) {
        // printf("state: %d\n", state);
        switch (state) {
        case 1: /*space*/
            if (data[i] == CHAR_SP) {
                continue;
            }
            if (data[i] == CHAR_PLUS || data[i] == CHAR_MINUS) {
                sign  = 1;
                state = 2;
                continue;
            }
            if (has_cs) {
                if (currency) {
                    if ( i < (size - currency->size) ) {
                        if ( memcmp (&data[i], currency->data, currency->size) == 0 ) {
                            i += (currency->size - 1);
                            state = 4;
                            continue;
                        }
                    }

                }
                if (data[i] == rtd->current_module->currency_symbol) {
                    state = 4;
                    continue;
                }
            }
            if (data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                state = 6;
                continue;
            }
            if (data[i] == rtd->current_module->decimal_point) {
                state = 7;
                continue;
            }
            break;
        case 2: /*sign*/
            if (data[i] == CHAR_SP) {
                state = 3;
                continue;
            }
            if (has_cs) {
                if (currency) {
                    if ( i < (size - currency->size) ) {
                        if ( memcmp (&data[i], currency->data, currency->size) == 0 ) {
                            i += (currency->size - 1);
                            state = 4;
                            continue;
                        }
                    }

                }
                if (data[i] == rtd->current_module->currency_symbol) {
                    state = 4;
                    continue;
                }
            }
            if (data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                state = 6;
                continue;
            }
            if (data[i] == rtd->current_module->decimal_point) {
                state = 7;
                continue;
            }
            break;
        case 3: /*space*/
            if (data[i] == CHAR_SP) {
                continue;
            }
            if (has_cs) {
                if (currency) {
                    if ( i < (size - currency->size) ) {
                        if ( memcmp (&data[i], currency->data, currency->size) == 0 ) {
                            i += (currency->size - 1);
                            state = 4;
                            continue;
                        }
                    }

                }
                if (data[i] == rtd->current_module->currency_symbol) {
                    state = 4;
                    continue;
                }
            }
            if (data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                state = 6;
                continue;
            }
            if (data[i] == rtd->current_module->decimal_point) {
                state = 7;
                continue;
            }
            break;
        case 4: /*cs*/
            if (data[i] == CHAR_SP) {
                state = 5;
                continue;
            }
            if ( data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                state = 6;
                continue;
            }
            if (data[i] == rtd->current_module->decimal_point) {
                state = 7;
                continue;
            }
            break;
        case 5: /*space*/
            if (data[i] == CHAR_SP) {
                continue;
            }
            if ( data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                state = 6;
                continue;
            }
            if (data[i] == rtd->current_module->decimal_point) {
                state = 7;
                continue;
            }
            break;
        case 6: /*digit*/
            if (data[i] == CHAR_SP) {
                state = 9;
                continue;
            }
            if (data[i] == CHAR_PLUS || data[i] == CHAR_MINUS) {
                if (!sign) {
                    state = 10;
                    continue;
                }
            }
            if (i < (size - 1)) {
                cx[0] = data[i];
                cx[1] = data[i + 1];
                cx[2] = 0;
                if ((strcasecmp (cx, cr) == 0) || (strcasecmp (cx, db) == 0)) {
                    if (!sign) {
                        state = 10;
                        ++i;
                        continue;
                    }
                }
            }
            if ( data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                continue;
            }
            if (data[i] == rtd->current_module->decimal_point) {
                state = 7;
                continue;
            }
            break;
        case 7: /*point*/
            if ( data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                state = 8;
                continue;
            }
        case 8: /*digit*/
            if (data[i] == CHAR_SP) {
                state = 9;
                continue;
            }
            if (data[i] == CHAR_PLUS || data[i] == CHAR_MINUS) {
                if (!sign) {
                    state = 10;
                    continue;
                }
            }
            if (i < (size - 1)) {
                cx[0] = data[i];
                cx[1] = data[i + 1];
                cx[2] = 0;
                if ((strcasecmp (cx, cr) == 0) || (strcasecmp (cx, db) == 0)) {
                    if (!sign) {
                        state = 10;
                        ++i;
                        continue;
                    }
                }
            }
            if ( data[i] >= CHAR_0 && data[i] <= CHAR_9 ) {
                continue;
            }
            break;
        case 9: /*space*/
            if (data[i] == CHAR_SP) {
                continue;
            }
            if (data[i] == CHAR_PLUS || data[i] == CHAR_MINUS) {
                if (!sign) {
                    state = 10;
                    continue;
                }
            }
            if (i < (size - 1)) {
                cx[0] = data[i];
                cx[1] = data[i + 1];
                cx[2] = 0;
                if ((strcasecmp (cx, cr) == 0) || (strcasecmp (cx, db) == 0)) {
                    if (!sign) {
                        state = 10;
                        ++i;
                        continue;
                    }
                }
            }
            break;
        case 10: /*ind*/
            if (data[i] == CHAR_SP) {
                state = 11;
                continue;
            }
        case 11: /*space*/
            if (data[i] == CHAR_SP) {
                continue;
            }
            break;
        }
        break;
    }
    //printf("i    : %d\n", i);
    return i != size;
}

cob_field *
cob_intr_numval (COB_RTD, cob_field *srcfield)
{
    long long   llval = 0;
    double      val;
    size_t      i;
    int         integer_digits = 0;
    int         decimal_digits = 0;
    int         sign = 0;
    int         decimal_seen = 0;
    cob_field_attr  attr;
    cob_field   field;
    unsigned char   integer_buff[64];
    unsigned char   decimal_buff[64];
    unsigned char   final_buff[64];
    char cr[3];
    char db[3];
    char cx[3];
    strcpy (cr, "CR");
    strcpy (db, "DB");
    (void)STRING_A2E((unsigned char*)cr, 2);
    (void)STRING_A2E((unsigned char*)db, 2);

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    memset (integer_buff, 0, sizeof (integer_buff));
    memset (decimal_buff, 0, sizeof (decimal_buff));
    memset (final_buff, 0, sizeof (final_buff));

    if (rtd->current_module->numval_validate && test_numval(rtd, srcfield->data, srcfield->size, 0, NULL)) {
        char s[30] = {0};
        memcpy(s, srcfield->data, srcfield->size < 30 ? srcfield->size : 29);
        cob_runtime_error (rtd, "NUMVAL argument 1 is invalid : %s", s);
        cob_stop_abend (rtd, COBRE_NOT_NUMERIC);
        /* cob_set_exception (rtd, COB_EC_SIZE);
           attr.scale = decimal_digits;
           make_field_entry (rtd, &field);
           memcpy (rtd->curr_field->data, (char *)&llval, 8);
           return rtd->curr_field;                          
        */
    }
    
    for (i = 0; i < srcfield->size; ++i) {
        if ( i < (srcfield->size - 1) ) {
            cx[0] = srcfield->data[i];
            cx[1] = srcfield->data[i + 1];
            cx[2] = 0;
            if ((strcasecmp (cx, cr) == 0) || (strcasecmp (cx, db) == 0)) {
                sign = 1;
                break;
            }
        }
        if ( srcfield->data[i] == CHAR_SP ) {
            continue;
        }
        if ( srcfield->data[i] == CHAR_PLUS ) {
            continue;
        }
        if ( srcfield->data[i] == CHAR_MINUS ) {
            sign = 1;
            continue;
        }
        if ( srcfield->data[i] == rtd->current_module->decimal_point ) {
            decimal_seen = 1;
            continue;
        }
        if ( srcfield->data[i] >= CHAR_0 && srcfield->data[i] <= CHAR_9 ) {
            llval *= 10;
            llval += srcfield->data[i] - CHAR_0;
            if ( decimal_seen ) {
                decimal_buff[decimal_digits++] = srcfield->data[i];
            } else {
                integer_buff[integer_digits++] = srcfield->data[i];
            }
        }
        if ( (integer_digits + decimal_digits) > 30 ) {
            break;
        }
    }
    if ( !integer_digits ) {
        integer_buff[0] = CHAR_0;
    }
    if ( !decimal_digits ) {
        decimal_buff[0] = CHAR_0;
    }
    if ( sign ) {
        llval = -llval;
    }
    if ( (integer_digits + decimal_digits) <= 18 ) {
        attr.scale = decimal_digits;
        make_field_entry (rtd, &field);
        memcpy (rtd->curr_field->data, (char *)&llval, 8);
    } else {
        snprintf ((char *)final_buff, 63, "%s%s.%s", sign ? "-" : "",
                  integer_buff, decimal_buff);
        sscanf ((char *)final_buff, "%lf", &val);
        make_double_entry (rtd);
        memcpy (rtd->curr_field->data, (char *)&val, sizeof (double));
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_numval_c (COB_RTD, cob_field *srcfield, cob_field *currency)
{
    unsigned char   *currency_data;
    long long   llval = 0;
    double      val;
    size_t      i;
    int         integer_digits = 0;
    int         decimal_digits = 0;
    int         sign = 0;
    int         decimal_seen = 0;
    cob_field_attr  attr;
    cob_field   field;
    unsigned char   integer_buff[64];
    unsigned char   decimal_buff[64];
    unsigned char   final_buff[64];
    char cr[3];
    char db[3];
    char cx[3];

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    memset (integer_buff, 0, sizeof (integer_buff));
    memset (decimal_buff, 0, sizeof (decimal_buff));
    memset (final_buff, 0, sizeof (final_buff));

    currency_data = NULL;
    if ( currency ) {
        if ( currency->size < srcfield->size ) {
            currency_data = currency->data;
        }
    }
    strcpy (cr, "CR");
    strcpy (db, "DB");
    (void)STRING_A2E((unsigned char*)cr, 2);
    (void)STRING_A2E((unsigned char*)db, 2);

    if (rtd->current_module->numval_validate && test_numval(rtd, srcfield->data, srcfield->size, 1, currency)) {
        char s[30] = {0};
        memcpy(s, srcfield->data, srcfield->size < 30 ? srcfield->size : 29);
        cob_runtime_error (rtd, "NUMVAL argument 1 is invalid : %s", s);
        cob_stop_abend (rtd, COBRE_NOT_NUMERIC);
        /* cob_set_exception (rtd, COB_EC_SIZE);
           attr.scale = decimal_digits;
           make_field_entry (rtd, &field);
           memcpy (rtd->curr_field->data, (char *)&llval, 8);
           return rtd->curr_field;                          
        */
    }

    for ( i = 0; i < srcfield->size; ++i ) {
        if ( i < (srcfield->size - 1) ) {
            cx[0] = srcfield->data[i];
            cx[1] = srcfield->data[i + 1];
            cx[2] = 0;
            if ((strcasecmp (cx, cr) == 0) || (strcasecmp (cx, db) == 0)) {
                sign = 1;
                break;
            }
        }
        if ( currency_data ) {
            if ( i < (srcfield->size - currency->size) ) {
                if ( memcmp ((char *)&srcfield->data[i], currency_data,
                             currency->size) == 0 ) {
                    i += (currency->size - 1);
                    continue;
                }
            }
        }
        if ( srcfield->data[i] == CHAR_SP ) {
            continue;
        }
        if ( srcfield->data[i] == CHAR_PLUS ) {
            continue;
        }
        if ( srcfield->data[i] == CHAR_MINUS ) {
            sign = 1;
            continue;
        }
        if ( srcfield->data[i] == rtd->current_module->decimal_point ) {
            decimal_seen = 1;
            continue;
        }
        if ( srcfield->data[i] == rtd->current_module->currency_symbol ) {
            continue;
        }
        if ( srcfield->data[i] >= CHAR_0 && srcfield->data[i] <= CHAR_9 ) {
            llval *= 10;
            llval += srcfield->data[i] - CHAR_0;
            if ( decimal_seen ) {
                decimal_buff[decimal_digits++] = srcfield->data[i];
            } else {
                integer_buff[integer_digits++] = srcfield->data[i];
            }
        }
        if ( (integer_digits + decimal_digits) > 30 ) {
            break;
        }
    }
    if ( !integer_digits ) {
        integer_buff[0] = CHAR_0;
    }
    if ( !decimal_digits ) {
        decimal_buff[0] = CHAR_0;
    }
    if ( sign ) {
        llval = -llval;
    }
    if ( (integer_digits + decimal_digits) <= 18 ) {
        attr.scale = decimal_digits;
        make_field_entry (rtd, &field);
        memcpy (rtd->curr_field->data, (char *)&llval, 8);
    } else {
        snprintf ((char *)final_buff, 63, "%s%s.%s", sign ? "-" : "",
                  integer_buff, decimal_buff);
        sscanf ((char *)final_buff, "%lf", &val);
        make_double_entry (rtd);
        memcpy (rtd->curr_field->data, (char *)&val, sizeof (double));
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_annuity (COB_RTD, cob_field *srcfield1, cob_field *srcfield2)
{
    double      mathd1, mathd2;

    make_double_entry (rtd);

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield1);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d2, srcfield2);

    mathd1 = intr_get_double (rtd, &rtd->instrinsic_d1);
    mathd2 = intr_get_double (rtd, &rtd->instrinsic_d2);
    if ( mathd1 == 0 ) {
        mathd1 = 1.0 / mathd2;
        memcpy (rtd->curr_field->data, (char *)&mathd1, sizeof (double));
        return rtd->curr_field;
    }
    mathd1 /= (1.0 - pow (mathd1 + 1.0, 0.0 - mathd2));
    memcpy (rtd->curr_field->data, (char *)&mathd1, sizeof (double));
    return rtd->curr_field;
}

cob_field *
cob_intr_sum (COB_RTD, const int params, ...)
{
    cob_field   *f;
    va_list     args;
    int     i;
    int     digits = 0;
    int     scale = 0;
    /*cob_field_attr  attr;*/
    /*cob_field   field;*/

/*
    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);                                         
*/
    cob_decimal_set_int(&rtd->instrinsic_d1, 0);

    va_start (args, params);

    for ( i = 0; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        if ( (COB_FIELD_DIGITS(f) - COB_FIELD_SCALE(f)) > digits ) {
            digits = COB_FIELD_DIGITS(f) - COB_FIELD_SCALE(f);
        }
        if ( COB_FIELD_SCALE(f) > scale ) {
            scale = COB_FIELD_SCALE(f);
        }
        cob_decimal_set_field_1 (rtd, &rtd->instrinsic_d2, f, 0);

        cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);

    }
    va_end (args);

    //   attr.scale = scale;
    //   make_field_entry (rtd, &field);
    make_comp3_entry(rtd,cob_decimal_digits_count(&rtd->instrinsic_d1), rtd->instrinsic_d1.scale);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_ord_min (COB_RTD, const int params, ...)
{
    cob_field   *f, *basef;
    int         i;
    int         ordmin = 0;
    va_list     args;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    if ( params <= 1 ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }

    va_start (args, params);

    basef = va_arg (args, cob_field *);
    for ( i = 1; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        if ( cob_cmp (rtd, f, basef) < 0 ) {
            basef = f;
            ordmin = i;
        }
    }
    va_end (args);

    cob_set_int (rtd, rtd->curr_field, ordmin + 1);
    return rtd->curr_field;
}

cob_field *
cob_intr_ord_max (COB_RTD, const int params, ...)
{
    cob_field   *f, *basef;
    int         ordmin = 0;
    int         i;
    va_list     args;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    if ( params <= 1 ) {
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }

    va_start (args, params);

    basef = va_arg (args, cob_field *);
    for ( i = 1; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        if ( cob_cmp (rtd, f, basef) > 0 ) {
            basef = f;
            ordmin = i;
        }
    }
    va_end (args);

    cob_set_int (rtd, rtd->curr_field, ordmin + 1);
    return rtd->curr_field;
}

cob_field *
cob_intr_min (COB_RTD, const int params, ...)
{
    cob_field   *f, *basef;
    va_list     args;
    int     i;

    va_start (args, params);

    basef = va_arg (args, cob_field *);
    for ( i = 1; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        if ( cob_cmp (rtd, f, basef) < 0 ) {
            basef = f;
        }
    }
    va_end (args);

    return basef;
}

cob_field *
cob_intr_max (COB_RTD, const int params, ...)
{
    cob_field   *f, *basef;
    va_list     args;
    int     i;

    va_start (args, params);

    basef = va_arg (args, cob_field *);
    for ( i = 1; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        if ( cob_cmp (rtd, f, basef) > 0 ) {
            basef = f;
        }
    }
    va_end (args);

    return basef;
}

cob_field *
cob_intr_midrange (COB_RTD, const int params, ...)
{
    cob_field   *f, *basemin, *basemax;
    va_list     args;
    int     i;

    make_double_entry (rtd);
    va_start (args, params);

    basemin = va_arg (args, cob_field *);
    basemax = basemin;
    for ( i = 1; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        if ( cob_cmp (rtd, f, basemin) < 0 ) {
            basemin = f;
        }
        if ( cob_cmp (rtd, f, basemax) > 0 ) {
            basemax = f;
        }
    }
    va_end (args);

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, basemin);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d2, basemax);
    cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
    cob_decimal_set_int(&rtd->instrinsic_d2, 2);
    cob_decimal_div (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_median (COB_RTD, const int params, ...)
{
    cob_field   *f;
    cob_field   **field_alloc;
    va_list     args;
    int         i;

    va_start (args, params);

    f = va_arg (args, cob_field *);
    if ( params == 1 ) {
        va_end (args);
        return f;
    }

    field_alloc = cob_malloc (rtd, params * sizeof (cob_field *));
    field_alloc[0] = f;

    for ( i = 1; i < params; ++i ) {
        field_alloc[i] = va_arg (args, cob_field *);
    }
    va_end (args);

    cob_qsort (field_alloc, (size_t)params, (size_t)sizeof (cob_field *), comp_field, rtd);

    i = params / 2;
    if ( params % 2 ) {
        f = field_alloc[i];
    } else {
        make_double_entry (rtd);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d1, field_alloc[i-1]);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d2, field_alloc[i]);
        cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
        cob_decimal_set_int(&rtd->instrinsic_d2, 2);
        cob_decimal_div (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
        cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
        f = rtd->curr_field;
    }

    cob_free (field_alloc);
    return f;
}

cob_field *
cob_intr_mean (COB_RTD, const int params, ...)
{
    cob_field   *f;
    va_list     args;
    long long   n;
    union {
        unsigned char   data[8];
        long long   datall;
    } datun;
    int         i;
    cob_field_attr  attr;
    cob_field   field;


    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    cob_decimal_set_uint(&rtd->instrinsic_d1, 0);

    va_start (args, params);
    for ( i = 0; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f);
        cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
    }
    va_end (args);

    cob_decimal_set_uint(&rtd->instrinsic_d2, (unsigned int)params);
    cob_decimal_div (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
    field.data = datun.data;
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, &field, 0, 1);
    n = datun.datall;
    for ( i = 0; n; n /= 10, ++i ) ;
    field.data = NULL;
    if ( i <= 18 ) {
        attr.scale = 18 - i;
    }
    make_field_entry (rtd, &field);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_mod (COB_RTD, cob_field *srcfield1, cob_field *srcfield2)
{
    cob_field   *f1;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    make_field_entry (rtd, &field);

    f1 = cob_intr_integer (rtd, cob_intr_binop (rtd, srcfield1, '/', srcfield2));

    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield2);         
    cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f1);                
    cob_decimal_mul (&rtd->instrinsic_d2, &rtd->instrinsic_d1);          
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield1);         
    cob_decimal_sub (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);     
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_range (COB_RTD, const int params, ...)
{
    cob_field   *f, *basemin, *basemax;
    va_list     args;
    int         i;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    va_start (args, params);

    basemin = va_arg (args, cob_field *);
    basemax = basemin;
    for ( i = 1; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        if ( cob_cmp (rtd, f, basemin) < 0 ) {
            basemin = f;
        }
        if ( cob_cmp (rtd, f, basemax) > 0 ) {
            basemax = f;
        }
    }
    va_end (args);

    attr.scale = COB_FIELD_SCALE(basemin);
    if ( COB_FIELD_SCALE(basemax) > attr.scale ) {
        attr.scale = COB_FIELD_SCALE(basemax);
    }
    make_field_entry (rtd, &field);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, basemax);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d2, basemin);
    cob_decimal_sub (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d1, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_rem (COB_RTD, cob_field *srcfield1, cob_field *srcfield2)
{
    /*cob_field       *f1;*/
    cob_field_attr  attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    cob_decimal_set_field (rtd, &(rtd->instrinsic_d1), srcfield1);
    cob_decimal_set_field (rtd, &(rtd->instrinsic_d2), srcfield2);
    cob_decimal_set (&(rtd->instrinsic_d3), &(rtd->instrinsic_d1));

    /* compute quotient */
    cob_decimal_div (rtd, &(rtd->instrinsic_d1), &(rtd->instrinsic_d2));
    /* truncate digits from the quotient */
    /* cob_shift_decimal (rtd, &(rtd->instrinsic_d1), COB_FIELD_SCALE(srcfield1) - (rtd->instrinsic_d1).scale); */
    if (rtd->instrinsic_d1.scale > 0)
        cob_shift_decimal (rtd, &(rtd->instrinsic_d1), -(rtd->instrinsic_d1).scale);
    /* compute remainder */
    cob_decimal_mul (&(rtd->instrinsic_d1), &(rtd->instrinsic_d2));
    cob_decimal_sub (rtd, &(rtd->instrinsic_d3), &(rtd->instrinsic_d1));

    /*f1 = cob_intr_integer_part (rtd, cob_intr_binop (rtd, srcfield1, '/', srcfield2));
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield2);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f1);
    cob_decimal_mul (&rtd->instrinsic_d2, &rtd->instrinsic_d1);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, srcfield1);
    cob_decimal_sub (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);

    */ 
    attr.scale = COB_FIELD_SCALE(srcfield1);
    if ( COB_FIELD_SCALE(srcfield2) > attr.scale ) {
        attr.scale = COB_FIELD_SCALE(srcfield2);
    }
    make_field_entry (rtd, &field);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d3, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_random (COB_RTD, const int params, ...)
{
    cob_field   *f;
    va_list     args;
    int         seed = 1;
    int         randnum;
    int         i;
    int         exp10;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 9, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    va_start (args, params);

    if ( params ) {
        f = va_arg (args, cob_field *);
        seed = cob_get_int (rtd, f);
        if ( seed < 0 ) {
            seed = 0;
        }
        srand ((unsigned int)seed);
    }
    va_end (args);

    randnum = rand ();
    exp10 = 1;
    for ( i = 0; i < 10; ++i ) {
        if ( (randnum / exp10) == 0 ) {
            break;
        }
        exp10 *= 10;
    }
    if ( i == 0 ) {
        i = 1;
    }
    attr.scale = i;
    make_field_entry (rtd, &field);
    *(long long *)rtd->curr_field->data = (long long)randnum;
    return rtd->curr_field;
}

cob_field *
cob_intr_variance (COB_RTD, const int params, ...)
{
    cob_field   *f;
    va_list     args;
    long long   n;
    union {
        unsigned char   data[8];
        long long   datall;
    } datun;
    int     i;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 18, 0, COB_FLAG_HAVE_SIGN, NULL);
    COB_FIELD_INIT (8, NULL, &attr);
    if ( params == 1 ) {
        make_field_entry (rtd, &field);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }

    /* MEAN for all params */
    cob_decimal_set_uint(&rtd->instrinsic_d1, 0);

    va_start (args, params);
    for ( i = 0; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f);
        cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
    }
    va_end (args);
    cob_decimal_set_uint(&rtd->instrinsic_d2, (unsigned int)params);
    cob_decimal_div (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);

    /* Got the MEAN in rtd->instrinsic_d1, iterate again */

    cob_decimal_set_uint(&rtd->instrinsic_d4, 0);

    va_start (args, params);

    for ( i = 0; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f);
        cob_decimal_sub (rtd, &rtd->instrinsic_d2, &rtd->instrinsic_d1);
        cob_decimal_mul (&rtd->instrinsic_d2, &rtd->instrinsic_d2);
        cob_decimal_add (rtd, &rtd->instrinsic_d4, &rtd->instrinsic_d2);
    }
    va_end (args);

    cob_decimal_set_uint(&rtd->instrinsic_d3, (unsigned int)params);
    cob_decimal_div (rtd, &rtd->instrinsic_d4, &rtd->instrinsic_d3);
    field.data = datun.data;
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d4, &field, 0, 1);
    n = datun.datall;
    for ( i = 0; n; n /= 10, ++i ) ;
    field.data = NULL;
    if ( i <= 18 ) {
        attr.scale = 18 - i;
    }
    make_field_entry (rtd, &field);
    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d4, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_standard_deviation (COB_RTD, const int params, ...)
{
    cob_field   *f;
    va_list     args;
    int     i;

    va_start (args, params);
    make_double_entry (rtd);

    if ( params == 1 ) {
        va_end (args);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }

    /* MEAN for all params */
    cob_decimal_set_uint(&rtd->instrinsic_d1, 0);

    for ( i = 0; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f);
        cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);
    }
    va_end (args);
    cob_decimal_set_uint(&rtd->instrinsic_d2, (unsigned int)params);
    cob_decimal_div (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);

    /* Got the MEAN in rtd->instrinsic_d1, iterate again */

    cob_decimal_set_uint(&rtd->instrinsic_d4, 0);

    va_start (args, params);

    for ( i = 0; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f);
        cob_decimal_sub (rtd, &rtd->instrinsic_d2, &rtd->instrinsic_d1);
        cob_decimal_mul (&rtd->instrinsic_d2, &rtd->instrinsic_d2);
        cob_decimal_add (rtd, &rtd->instrinsic_d4, &rtd->instrinsic_d2);
    }
    va_end (args);

    cob_decimal_set_uint(&rtd->instrinsic_d3, (unsigned int)params);
    cob_decimal_div (rtd, &rtd->instrinsic_d4, &rtd->instrinsic_d3);
    /* We have the VARIANCE in rtd->instrinsic_d4, sqrt = STANDARD-DEVIATION */

/* Do not know why this does not work
    rtd->instrinsic_d5.scale = rtd->instrinsic_d4.scale;
    mpz_mul_ui (rtd->instrinsic_d5.value, rtd->instrinsic_d4.value, 1000000000);
    mpz_mul_ui (rtd->instrinsic_d4.value, rtd->instrinsic_d5.value, 1000000000);
    mpz_sqrt (rtd->instrinsic_d5.value, rtd->instrinsic_d4.value);
    mpz_div_ui (rtd->instrinsic_d4.value, rtd->instrinsic_d5.value, 1000000000);
    cob_decimal_get_field_1 (&rtd->instrinsic_d4, rtd->curr_field, 0, 0);
    return rtd->curr_field;
*/

    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d4, rtd->curr_field, 0, 0);
    f = cob_intr_sqrt (rtd, rtd->curr_field);
    return f;
}

cob_field *
cob_intr_present_value (COB_RTD, const int params, ...)
{
    cob_field   *f;
    va_list     args;
    int     i;

    va_start (args, params);
    make_double_entry (rtd);

    if ( params < 2 ) {
        va_end (args);
        cob_runtime_error (rtd, "Wrong number of parameters for FUNCTION PRESENT-VALUE");
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    f = va_arg (args, cob_field *);
    cob_decimal_set_field (rtd, &rtd->instrinsic_d1, f);
    cob_decimal_set_uint(&rtd->instrinsic_d2, 1);
    cob_decimal_add (rtd, &rtd->instrinsic_d1, &rtd->instrinsic_d2);

    cob_decimal_set_uint(&rtd->instrinsic_d4, 0);

    for ( i = 1; i < params; ++i ) {
        f = va_arg (args, cob_field *);
        cob_decimal_set_field (rtd, &rtd->instrinsic_d2, f);
        cob_decimal_set(&rtd->instrinsic_d3, &rtd->instrinsic_d1);
        if ( i > 1 ) {
            cob_decimal_set_uint(&rtd->instrinsic_d5, (unsigned int)i);
            cob_decimal_pow (&rtd->instrinsic_d3, &rtd->instrinsic_d5);
        }
        cob_decimal_div (rtd, &rtd->instrinsic_d2, &rtd->instrinsic_d3);
        cob_decimal_add (rtd, &rtd->instrinsic_d4, &rtd->instrinsic_d2);
    }
    va_end (args);

    cob_decimal_get_field_1 (rtd, &rtd->instrinsic_d4, rtd->curr_field, 0, 0);
    return rtd->curr_field;
}

cob_field *
cob_intr_year_to_yyyy (COB_RTD, const int params, ...)
{
    cob_field   *f;
    struct tm   *timeptr;
    va_list     args;
    time_t      t;
    int     year;
    int     interval;
    int     xqtyear;
    int     maxyear;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    va_start (args, params);
    f = va_arg (args, cob_field *);
    year = cob_get_int (rtd, f);
    if ( params > 1 ) {
        f = va_arg (args, cob_field *);
        interval = cob_get_int (rtd, f);
    } else {
        interval = 50;
    }
    if ( params > 2 ) {
        f = va_arg (args, cob_field *);
        xqtyear = cob_get_int (rtd, f);
    } else {
        t = time (NULL);
        timeptr = localtime (&t);
        xqtyear = 1900 + timeptr->tm_year;
    }
    va_end (args);

    if ( year < 0 || year > 99 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    if ( xqtyear < 1601 || xqtyear > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    maxyear = xqtyear + interval;
    if ( maxyear < 1700 || maxyear > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    if ( maxyear % 100 >= year ) {
        year += 100 * (maxyear / 100);
    } else {
        year += 100 * ((maxyear / 100) - 1);
    }
    cob_set_int (rtd, rtd->curr_field, year);
    return rtd->curr_field;
}

cob_field *
cob_intr_date_to_yyyymmdd (COB_RTD, const int params, ...)
{
    cob_field   *f;
    struct tm   *timeptr;
    va_list     args;
    time_t      t;
    int         year;
    int         mmdd;
    int         interval;
    int         xqtyear;
    int         maxyear;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    va_start (args, params);
    f = va_arg (args, cob_field *);
    year = cob_get_int (rtd, f);
    mmdd = year % 10000;
    year /= 10000;
    if ( params > 1 ) {
        f = va_arg (args, cob_field *);
        interval = cob_get_int (rtd, f);
    } else {
        interval = 50;
    }
    if ( params > 2 ) {
        f = va_arg (args, cob_field *);
        xqtyear = cob_get_int (rtd, f);
    } else {
        t = time (NULL);
        timeptr = localtime (&t);
        xqtyear = 1900 + timeptr->tm_year;
    }
    va_end (args);

    if ( year < 0 || year > 999999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    if ( xqtyear < 1601 || xqtyear > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    maxyear = xqtyear + interval;
    if ( maxyear < 1700 || maxyear > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    if ( maxyear % 100 >= year ) {
        year += 100 * (maxyear / 100);
    } else {
        year += 100 * ((maxyear / 100) - 1);
    }
    year *= 10000;
    year += mmdd;
    cob_set_int (rtd, rtd->curr_field, year);
    return rtd->curr_field;
}

cob_field *
cob_intr_day_to_yyyyddd (COB_RTD, const int params, ...)
{
    cob_field   *f;
    struct tm   *timeptr;
    va_list     args;
    time_t      t;
    int         year;
    int         days;
    int         interval;
    int         xqtyear;
    int         maxyear;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    va_start (args, params);
    f = va_arg (args, cob_field *);
    year = cob_get_int (rtd, f);
    days = year % 1000;
    year /= 1000;
    if ( params > 1 ) {
        f = va_arg (args, cob_field *);
        interval = cob_get_int (rtd, f);
    } else {
        interval = 50;
    }
    if ( params > 2 ) {
        f = va_arg (args, cob_field *);
        xqtyear = cob_get_int (rtd, f);
    } else {
        t = time (NULL);
        timeptr = localtime (&t);
        xqtyear = 1900 + timeptr->tm_year;
    }
    va_end (args);

    if ( year < 0 || year > 999999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    if ( xqtyear < 1601 || xqtyear > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    maxyear = xqtyear + interval;
    if ( maxyear < 1700 || maxyear > 9999 ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    if ( maxyear % 100 >= year ) {
        year += 100 * (maxyear / 100);
    } else {
        year += 100 * ((maxyear / 100) - 1);
    }
    year *= 1000;
    year += days;
    cob_set_int (rtd, rtd->curr_field, year);
    return rtd->curr_field;
}

cob_field *
cob_intr_seconds_past_midnight (COB_RTD)
{
    struct tm   *timeptr;
    time_t      t;
    int         seconds;
    cob_field_attr  attr;
    cob_field   field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    t = time (NULL);
    timeptr = localtime (&t);
    seconds = (timeptr->tm_hour * 3600) + (timeptr->tm_min * 60) +
              timeptr->tm_sec;
    cob_set_int (rtd, rtd->curr_field, seconds);
    return rtd->curr_field;
}

cob_field *
cob_intr_seconds_from_formatted_time (COB_RTD, cob_field *format, cob_field *value)
{
    unsigned char   *p1;
    unsigned char   *p2;
    size_t          n;
    int             seconds = 0;
    int             minutes = 0;
    int             hours = 0;
    int             seconds_seen = 0;
    int             minutes_seen = 0;
    int             hours_seen = 0;
    cob_field_attr  attr;
    cob_field       field;

    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    COB_FIELD_INIT (4, NULL, &attr);
    make_field_entry (rtd, &field);

    rtd->cob_exception_code = 0;
    if ( value->size < format->size ) {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        cob_set_int (rtd, rtd->curr_field, 0);
        return rtd->curr_field;
    }
    p1 = format->data;
    p2 = value->data;
    for ( n = 0; n < format->size - 1; ++n, ++p1, ++p2 ) {
        if ( !memcmp (p1, "hh", 2) && !hours_seen ) {
            if ( *p2 >= CHAR_0 && *p2 <= CHAR_9 &&
                 *(p2 + 1) >= CHAR_0 && *(p2 + 1) <= CHAR_9 ) {
                hours = ((*p2 - CHAR_0) * 10) + (*(p2 + 1) - CHAR_0);
                hours_seen = 1;
                continue;
            }
        }
        if ( !memcmp (p1, "mm", 2) && !minutes_seen ) {
            if ( *p2 >= CHAR_0 && *p2 <= CHAR_9 &&
                 *(p2 + 1) >= CHAR_0 && *(p2 + 1) <= CHAR_9 ) {
                minutes = ((*p2 - CHAR_0) * 10) + (*(p2 + 1) - CHAR_0);
                minutes_seen = 1;
                continue;
            }
        }
        if ( !memcmp (p1, "ss", 2) && !seconds_seen ) {
            if ( *p2 >= CHAR_0 && *p2 <= CHAR_9 &&
                 *(p2 + 1) >= CHAR_0 && *(p2 + 1) <= CHAR_9 ) {
                seconds = ((*p2 - CHAR_0) * 10) + (*(p2 + 1) - CHAR_0);
                seconds_seen = 1;
                continue;
            }
        }
    }
    if ( hours_seen && minutes_seen && seconds_seen ) {
        seconds += (hours * 3600) + (minutes * 60);
    } else {
        cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
        seconds = 0;
    }
    cob_set_int (rtd, rtd->curr_field, seconds);
    return rtd->curr_field;
}

cob_field *
cob_intr_locale_date (COB_RTD, const int offset, const int length, 
                      cob_field *srcfield, cob_field *locale_field)
{
    cob_field_attr  attr;
    cob_field       field;
#if defined(_WIN32) || defined(__CYGWIN__) || defined(HAVE_LANGINFO_CODESET)
    size_t          len;
    int             indate;
    int             days;
    int             month;
    int             year;
#ifdef	HAVE_LANGINFO_CODESET
    unsigned char   *p;
    char            *deflocale = NULL;
    char            *localep = NULL;
    char            *localep2;
    struct tm       tstruct;
    char            buff2[128];
#else
    char            *p;
    LCID            localeid = LOCALE_USER_DEFAULT;
    SYSTEMTIME      syst;
#endif
    char            buff[128];
#endif

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    rtd->cob_exception_code = 0;

#if defined(_WIN32) || defined(__CYGWIN__) || defined(HAVE_LANGINFO_CODESET)
    if ( COB_FIELD_IS_NUMERIC (srcfield) ) {
        indate = cob_get_int (rtd, srcfield);
    } else {
        if ( srcfield->size < 8 ) {
            goto derror;
        }
        p = srcfield->data;
        indate = 0;
        for ( len = 0; len < 8; ++len, ++p ) {
            if ( cob_isdigit_char (rtd, *p) ) {
                indate *= 10;
                indate += (*p - CHAR_0);
            } else {
                goto derror;
            }
        }
    }
    year = indate / 10000;
    if ( year < 1601 || year > 9999 ) {
        goto derror;
    }
    indate %= 10000;
    month = indate / 100;
    if ( month < 1 || month > 12 ) {
        goto derror;
    }
    days = indate % 100;
    if ( days < 1 || days > 31 ) {
        goto derror;
    }
    if ( leap_year (rtd, year) ) {
        if ( days > leap_month_days[month] ) {
            goto derror;
        }
    } else {
        if ( days > normal_month_days[month] ) {
            goto derror;
        }
    }
#ifdef	HAVE_LANGINFO_CODESET
    month--;

    memset ((void *)&tstruct, 0, sizeof(struct tm));
    tstruct.tm_year = year - 1900;
    tstruct.tm_mon = month;
    tstruct.tm_mday = days;
    if ( locale_field ) {
        if ( locale_field->size >= COB_SMALL_BUFF ) {
            goto derror;
        }
        cob_field_to_string (rtd, locale_field, rtd->instrinsic_buff);
        deflocale = rtd->instrinsic_buff;
        localep2 = setlocale (LC_TIME, NULL);
        if ( localep2 ) {
            localep = strdup (localep2);
        }
        (void) setlocale (LC_TIME, deflocale);
    }
    memset (buff2, 0, sizeof(buff2));
    snprintf(buff2, sizeof(buff2) - 1, "%s", nl_langinfo(D_FMT));
    if ( deflocale ) {
        if ( localep ) {
            (void) setlocale (LC_TIME, localep);
        }
    }
    strftime (buff, sizeof(buff), buff2, &tstruct);
#else
    memset ((void *)&syst, 0, sizeof(syst));
    syst.wYear = year;
    syst.wMonth = month;
    syst.wDay = days;
    if ( locale_field ) {
        if ( locale_field->size >= COB_SMALL_BUFF ) {
            goto derror;
        }
        cob_field_to_string (rtd, locale_field, rtd->instrinsic_buff);
        for ( p = rtd->instrinsic_buff; *p; ++p ) {
            if ( isalnum(*p) || *p == '_' ) {
                continue;
            }
            break;
        }
        *p = 0;
        for ( len = 0; len < WINLOCSIZE; ++len ) {
            if ( !strcmp(rtd->instrinsic_buff, wintable[len].winlocalename) ) {
                localeid = wintable[len].winlocaleid;
                break;
            }
        }
        if ( len == WINLOCSIZE ) {
            goto derror;
        }
    }
    if ( !GetDateFormat (localeid, DATE_SHORTDATE, &syst, NULL, buff, sizeof(buff)) ) {
        goto derror;
    }
#endif
    len = strlen (buff);
    field.size = len;
    make_field_entry (rtd, &field);
    (void)STRING_A2E((unsigned char*)buff,len);
    memcpy (rtd->curr_field->data, buff, len);
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
    derror:
#endif
    field.size = 10;
    make_field_entry (rtd, &field);
    memset (rtd->curr_field->data, CHAR_SP, 10);
    cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
    return rtd->curr_field;
}

cob_field *
cob_intr_locale_time (COB_RTD, const int offset, const int length,
                      cob_field *srcfield, cob_field *locale_field)
{
    cob_field_attr  attr;
    cob_field       field;
#if defined(_WIN32) || defined(__CYGWIN__) || defined(HAVE_LANGINFO_CODESET)
    size_t          len;
    int             indate;
    int             hours;
    int             minutes;
    int             seconds;
#ifdef	HAVE_LANGINFO_CODESET
    unsigned char   *p;
    char            *deflocale = NULL;
    char            *localep = NULL;
    char            *localep2;
    struct tm       tstruct;
    char            buff2[128];
#else
    char            *p;
    LCID            localeid = LOCALE_USER_DEFAULT;
    SYSTEMTIME      syst;
#endif
    char            buff[128];
#endif

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    rtd->cob_exception_code = 0;

#if defined(_WIN32) || defined(__CYGWIN__) || defined(HAVE_LANGINFO_CODESET)
    if ( COB_FIELD_IS_NUMERIC (srcfield) ) {
        indate = cob_get_int (rtd, srcfield);
    } else {
        if ( srcfield->size < 6 ) {
            goto derror;
        }
        p = srcfield->data;
        indate = 0;
        for ( len = 0; len < 6; ++len, ++p ) {
            if ( cob_isdigit_char (rtd, *p) ) {
                indate *= 10;
                indate += (*p - CHAR_0);
            } else {
                goto derror;
            }
        }
    }
    hours = indate / 10000;
    if ( hours < 0 || hours > 24 ) {
        goto derror;
    }
    indate %= 10000;
    minutes = indate / 100;
    if ( minutes < 0 || minutes > 59 ) {
        goto derror;
    }
    seconds = indate % 100;
    if ( seconds < 0 || seconds > 59 ) {
        goto derror;
    }

#ifdef	HAVE_LANGINFO_CODESET
    memset ((void *)&tstruct, 0, sizeof(struct tm));
    tstruct.tm_hour = hours;
    tstruct.tm_min = minutes;
    tstruct.tm_sec = seconds;
    if ( locale_field ) {
        if ( locale_field->size >= COB_SMALL_BUFF ) {
            goto derror;
        }
        cob_field_to_string (rtd, locale_field, rtd->instrinsic_buff);
        deflocale = rtd->instrinsic_buff;
        localep2 = setlocale (LC_TIME, NULL);
        if ( localep2 ) {
            localep = strdup (localep2);
        }
        (void) setlocale (LC_TIME, deflocale);
    }
    memset (buff2, 0, sizeof(buff2));
    snprintf(buff2, sizeof(buff2) - 1, "%s", nl_langinfo(T_FMT));
    if ( deflocale ) {
        if ( localep ) {
            (void) setlocale (LC_TIME, localep);
        }
    }
    strftime (buff, sizeof(buff), buff2, &tstruct);
#else
    memset ((void *)&syst, 0, sizeof(syst));
    syst.wHour = hours;
    syst.wMinute = minutes;
    syst.wSecond = seconds;
    if ( locale_field ) {
        if ( locale_field->size >= COB_SMALL_BUFF ) {
            goto derror;
        }
        cob_field_to_string (rtd, locale_field, rtd->instrinsic_buff);
        for ( p = rtd->instrinsic_buff; *p; ++p ) {
            if ( isalnum(*p) || *p == '_' ) {
                continue;
            }
            break;
        }
        *p = 0;
        for ( len = 0; len < WINLOCSIZE; ++len ) {
            if ( !strcmp(rtd->instrinsic_buff, wintable[len].winlocalename) ) {
                localeid = wintable[len].winlocaleid;
                break;
            }
        }
        if ( len == WINLOCSIZE ) {
            goto derror;
        }
    }
    if ( !GetTimeFormat (localeid, LOCALE_NOUSEROVERRIDE, &syst, NULL, buff, sizeof(buff)) ) {

        goto derror;
    }
#endif
    len = strlen (buff);
    (void) STRING_A2E((unsigned char*)buff, len);
    field.size = len;
    make_field_entry (rtd, &field);
    memcpy (rtd->curr_field->data, buff, len);
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
    derror:
#endif
    field.size = 10;
    make_field_entry (rtd, &field);
    memset (rtd->curr_field->data, CHAR_SP, 10);
    cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
    return rtd->curr_field;
}

cob_field *
cob_intr_lcl_time_from_secs (COB_RTD, const int offset, const int length,
                             cob_field *srcfield, cob_field *locale_field)
{
    cob_field_attr  attr;
    cob_field       field;
#if defined(_WIN32) || defined(__CYGWIN__) || defined(HAVE_LANGINFO_CODESET)
    size_t          len;
    int             indate;
    int             hours;
    int             minutes;
    int             seconds;
#ifdef	HAVE_LANGINFO_CODESET
    char            *deflocale = NULL;
    char            *localep = NULL;
    char            *localep2;
    struct tm       tstruct;
    char            buff2[128];
#else
    char            *p;
    LCID            localeid = LOCALE_USER_DEFAULT;
    SYSTEMTIME      syst;
#endif
    char            buff[128];
#endif

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    rtd->cob_exception_code = 0;

#if defined(_WIN32) || defined(__CYGWIN__) || defined(HAVE_LANGINFO_CODESET)
    if ( COB_FIELD_IS_NUMERIC (srcfield) ) {
        indate = cob_get_int (rtd, srcfield);
    } else {
        goto derror;
    }
    if ( indate > 86400 ) {
        goto derror;
    }
    hours = indate / 3600;
    indate %= 3600;
    minutes = indate / 60;
    seconds = indate % 60;

#ifdef	HAVE_LANGINFO_CODESET
    memset ((void *)&tstruct, 0, sizeof(struct tm));
    tstruct.tm_hour = hours;
    tstruct.tm_min = minutes;
    tstruct.tm_sec = seconds;
    if ( locale_field ) {
        if ( locale_field->size >= COB_SMALL_BUFF ) {
            goto derror;
        }
        cob_field_to_string (rtd, locale_field, rtd->instrinsic_buff);
        deflocale = rtd->instrinsic_buff;
        localep2 = setlocale (LC_TIME, NULL);
        if ( localep2 ) {
            localep = strdup (localep2);
        }
        (void) setlocale (LC_TIME, deflocale);
    }
    memset (buff2, 0, sizeof(buff2));
    snprintf(buff2, sizeof(buff2) - 1, "%s", nl_langinfo(T_FMT));
    if ( deflocale ) {
        if ( localep ) {
            (void) setlocale (LC_TIME, localep);
        }
    }
    strftime (buff, sizeof(buff), buff2, &tstruct);
#else
    memset ((void *)&syst, 0, sizeof(syst));
    syst.wHour = hours;
    syst.wMinute = minutes;
    syst.wSecond = seconds;
    if ( locale_field ) {
        if ( locale_field->size >= COB_SMALL_BUFF ) {
            goto derror;
        }
        cob_field_to_string (rtd, locale_field, rtd->instrinsic_buff);
        for ( p = rtd->instrinsic_buff; *p; ++p ) {
            if ( isalnum(*p) || *p == '_' ) {
                continue;
            }
            break;
        }
        *p = 0;
        for ( len = 0; len < WINLOCSIZE; ++len ) {
            if ( !strcmp(rtd->instrinsic_buff, wintable[len].winlocalename) ) {
                localeid = wintable[len].winlocaleid;
                break;
            }
        }
        if ( len == WINLOCSIZE ) {
            goto derror;
        }
    }
    if ( !GetTimeFormat (localeid, LOCALE_NOUSEROVERRIDE, &syst, NULL, buff, sizeof(buff)) ) {

        goto derror;
    }
#endif
    len = strlen (buff);
    field.size = len;
    make_field_entry (rtd, &field);
    (void)STRING_A2E((unsigned char*)buff,len);
    memcpy (rtd->curr_field->data, buff, len);
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
    derror:
#endif
    field.size = 10;
    make_field_entry (rtd, &field);
    memset (rtd->curr_field->data, CHAR_SP, 10);
    cob_set_exception (rtd, COB_EC_ARGUMENT_FUNCTION);
    return rtd->curr_field;
}

const char  
*cob_get_codepage (COB_RTD)
{
    if ( rtd->display_code_page ) {
        return rtd->display_code_page;
    }
    if ( rtd->current_module ) {
        if ( !rtd->current_module->default_cp ) {
            if ( rtd->ebcdic_charset ) {
                rtd->current_module->default_cp = "IBM-1147";
            } else {
                rtd->current_module->default_cp = "ASCII";
            }
        }
        return(rtd->current_module->default_cp);
    } else {
#if defined(CIT_EBCDIC_CHARSET_DEFAULT)
        return "IBM-1147";
#else
        return "ASCII";
#endif
    }

}

static cob_field *
cob_intr_national_of_v (COB_RTD, const int offset, const int length, const int params, va_list args)
{
    cob_field       *f;

    cob_field_attr  attr;
    cob_field       field;
    char            *s;
    char            *d;
    char            cp_buffer[COB_SMALL_BUFF];
    size_t          ssz;
    size_t          dsz;

    COB_ATTR_INIT (COB_TYPE_NATIONAL, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    rtd->cob_exception_code = 0;
    f = va_arg (args, cob_field *);
    if ( COB_FIELD_IS_NUMERIC(f) ) {
        cob_field_to_string(rtd,f,rtd->instrinsic_buff);
        s = rtd->instrinsic_buff;
        ssz=strlen(s);
    } else {
        s = (char*)f->data;
        ssz = f->size;
    }
    if ( params > 1 ) {
        f = va_arg (args, cob_field *);
        if ( COB_FIELD_IS_NUMERIC(f) ) {
            sprintf (cp_buffer, "%d", cob_get_int(rtd,f));
        } else {
            cob_field_to_string(rtd,f,cp_buffer);
        }
        (void)STRING_E2A((unsigned char*)cp_buffer, strlen(cp_buffer));
    } else {
        strcpy(cp_buffer, cob_get_codepage(rtd));
    }
    dsz=ssz*4;
    field.size = dsz;
    make_field_entry (rtd, &field);
    d = (char*)rtd->curr_field->data;
    rtd->curr_field->size = cob_enterprise_intr_national_of(rtd, s, d, cp_buffer, ssz, dsz);
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_national_of_1 (COB_RTD, const int offset, const int length, const int params, ...)
{
    va_list         args;
    cob_field       *f;    

    va_start (args, params);
    f = cob_intr_national_of_v(rtd,offset, length, params, args);
    va_end(args);
    return f;
}

cob_field *
cob_intr_national_of (COB_RTD,const int params, ...)
{
    va_list         args;
    cob_field       *f;    

    va_start (args, params);
    f = cob_intr_national_of_v(rtd,0, -1, params, args);
    va_end(args);
    return f;
}


static cob_field *
cob_intr_display_of_v (COB_RTD, const int offset, const int length, const int params, va_list args)
{
    cob_field       *f;    
    cob_field_attr  attr;
    cob_field       field;
    char            *s;
    char            *d;
    char            cp_buffer[COB_SMALL_BUFF];
    size_t          ssz;
    size_t          dsz;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    rtd->cob_exception_code = 0;
    f = va_arg (args, cob_field *);
    if ( COB_FIELD_IS_NUMERIC(f) ) {
        cob_field_to_string(rtd,f,rtd->instrinsic_buff);
        s = rtd->instrinsic_buff;
        ssz=strlen(s);
    } else {
        s = (char*)f->data;
        ssz = f->size;
    }
    if ( params > 1 ) {
        f = va_arg (args, cob_field *);
        if ( COB_FIELD_IS_NUMERIC(f) ) {
            sprintf (cp_buffer, "%d", cob_get_int(rtd,f));
        } else {
            cob_field_to_string(rtd,f,cp_buffer);
        }
        (void)STRING_E2A((unsigned char*)cp_buffer, strlen(cp_buffer));
    } else {
        strcpy(cp_buffer, cob_get_codepage(rtd));
    }

    dsz=ssz*2;  /* Possible UTF8 with 4 bytes for each character*/
    field.size = dsz;
    make_field_entry (rtd, &field);
    d = (char*)rtd->curr_field->data;
    rtd->curr_field->size = cob_enterprise_intr_display_of(rtd, s, d, cp_buffer, ssz, dsz);
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_display_of (COB_RTD, const int params, ...)
{
    va_list         args;
    cob_field       *f;    

    va_start (args, params);
    f = cob_intr_display_of_v(rtd,0, -1, params, args);
    va_end(args);
    return f;
}

cob_field *
cob_intr_display_of_1 (COB_RTD, const int offset, const int length, const int params, ...)
{
    va_list         args;
    cob_field       *f;    

    va_start (args, params);
    f = cob_intr_display_of_v(rtd,offset, length, params, args);
    va_end(args);
    return f;
}

/* Initialization routine */
static cob_field *
cob_intr_display_to_debugcp_v (COB_RTD, const int offset, const int length, const int params, va_list args, int to)
{
    cob_field       *f;    
    cob_field_attr  attr;
    cob_field       field;
    char            *s;
    char            *d;
    char            cp_buffer[COB_SMALL_BUFF];
    size_t          ssz;
    size_t          dsz;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    COB_FIELD_INIT (0, NULL, &attr);
    rtd->cob_exception_code = 0;
    f = va_arg (args, cob_field *);
    if ( COB_FIELD_IS_NUMERIC(f) ) {
        cob_field_to_string(rtd,f,rtd->instrinsic_buff);
        s = rtd->instrinsic_buff;
        ssz=strlen(s);
    } else {
        s = (char*)f->data;
        ssz = f->size;
    }
    if ( params > 1 ) {
        f = va_arg (args, cob_field *);
        if ( COB_FIELD_IS_NUMERIC(f) ) {
            sprintf (cp_buffer, "%d", cob_get_int(rtd,f));
        } else {
            cob_field_to_string(rtd,f,cp_buffer);
        }
        (void)STRING_E2A((unsigned char*)cp_buffer, strlen(cp_buffer));
    } else {
        strcpy(cp_buffer, cob_get_codepage(rtd));
    }

    dsz=ssz*4;  /* Possible UTF8 with 4 bytes for each character*/
    field.size = dsz;
    make_field_entry (rtd, &field);
    d = (char*)rtd->curr_field->data;
    (void)cob_enterprise_open_icu(rtd,cp_buffer);
    if ( to ) {
        rtd->curr_field->size = cob_enterprise_display_to_debugcp(rtd, s, d, cp_buffer, rtd->debug_codepage_iconv_cd,
                                                                  ssz, dsz);
    } else {
        rtd->curr_field->size = cob_enterprise_debugcp_to_display(rtd, s, d, cp_buffer, rtd->debug_codepage_iconv_cd, 
                                                                  ssz, dsz);
    }
    if ( unlikely(offset > 0) ) {
        calc_ref_mod (rtd, rtd->curr_field, offset, length);
    }
    return rtd->curr_field;
}

cob_field *
cob_intr_display_to_debugcp_1 (COB_RTD, const int offset, const int length, const int params, ...)
{
    va_list         args;
    cob_field       *f;    

    va_start (args, params);
    f = cob_intr_display_to_debugcp_v(rtd,offset, length, params, args, 1);
    va_end(args);
    return f;
}

cob_field *
cob_intr_display_to_utf8_1 (COB_RTD, const int offset, const int length, const int params, ...)
{
    va_list         args;
    cob_field       *f;    

    va_start (args, params);
    f = cob_intr_display_to_debugcp_v(rtd,offset, length, params, args, 1);
    va_end(args);
    return f;
}

cob_field *
cob_intr_debugcp_to_display_1 (COB_RTD, const int offset, const int length, const int params, ...)
{
    va_list         args;
    cob_field       *f;    
    void * save     =rtd->debug_codepage_iconv_cd;

    va_start (args, params);
    rtd->debug_codepage_iconv_cd = rtd->utf8_codepage_iconv_cd;
    f = cob_intr_display_to_debugcp_v(rtd,offset, length, params, args, 0);
    rtd->debug_codepage_iconv_cd = save;
    va_end(args);
    return f;
}

cob_field *
cob_intr_utf8_to_display_1 (COB_RTD, const int offset, const int length, const int params, ...)
{
    va_list         args;
    cob_field       *f;    
    void * save     =rtd->debug_codepage_iconv_cd;

    va_start (args, params);
    rtd->debug_codepage_iconv_cd = rtd->utf8_codepage_iconv_cd;
    f = cob_intr_display_to_debugcp_v(rtd,offset, length, params, args, 0);
    rtd->debug_codepage_iconv_cd = save;
    va_end(args);
    return f;
}

void
cob_init_intrinsic (COB_RTD)
{
    /*size_t      i;*/

    cob_decimal_init (&rtd->instrinsic_d1);
    cob_decimal_init (&rtd->instrinsic_d2);
    cob_decimal_init (&rtd->instrinsic_d3);
    cob_decimal_init (&rtd->instrinsic_d4);
    cob_decimal_init (&rtd->instrinsic_d5);
    /* mpz_init2 (mp, 256); */
    memset ((char *)&rtd->calc_field[0], 0, sizeof (rtd->calc_field));
    memset ((char *)&rtd->calc_attr[0], 0, sizeof (rtd->calc_attr));
    memset ((char *)&rtd->calc_size[0], 0, sizeof (rtd->calc_size));

    /*for ( i = 0; i < DEPTH_LEVEL; ++i ) {
        rtd->calc_field[i].data = cob_malloc (rtd, 64);
        rtd->calc_field[i].size = 64;
        rtd->calc_size[i] = 64;
    } 
    */
    rtd->instrinsic_buff = cob_malloc (rtd, COB_SMALL_BUFF);
    rtd->display_code_page = getenv("COB_DISPLAY_CODEPAGE");
    rtd->xml_code_page = getenv("COB_XML_CODEPAGE");
}

void
cob_clear_intrinsic (COB_RTD)
{
    size_t      i;

    cob_decimal_clear (&rtd->instrinsic_d1);
    cob_decimal_clear (&rtd->instrinsic_d2);
    cob_decimal_clear (&rtd->instrinsic_d3);
    cob_decimal_clear (&rtd->instrinsic_d4);
    cob_decimal_clear (&rtd->instrinsic_d5);
    /* mpz_init2 (mp, 256); */
    for ( i = 0; i < DEPTH_LEVEL; ++i ) {
        if ( rtd->calc_field[i].data ) {
            cob_free(rtd->calc_field[i].data);
        }
        rtd->calc_field[i].data = 0;
        rtd->calc_size[i] =  0;
    }
    cob_free(rtd->instrinsic_buff);
    cob_enterprise_free_national(rtd);
}



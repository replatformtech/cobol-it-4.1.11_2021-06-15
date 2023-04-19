/*
 * Copyright (C) 2006-2007 Roger While
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
 * License along with this library; see the file COPYING.LIB.  If not write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

#ifndef COB_CODEGEN_H
#define COB_CODEGEN_H

#if !defined(__i386__) && !defined(__x86_64__) && !defined(__powerpc__) && !defined(__powerpc64__) && !defined(__ppc__) && !defined(__amd64__)
    #if defined(_MSC_VER)
        #define ALLOW_MISALIGNED
        #define MISALIGNED __unaligned
    #else
        #define MISALIGNED
    #endif
#else
    #define ALLOW_MISALIGNED
    #define MISALIGNED
#endif

#ifdef  COB_LOCAL_INLINE
    #define     COB_STATIC static COB_INLINE 
    #define     COB_PROTO_STATIC static COB_INLINE 
#else
    #define     COB_STATIC 
    #define		COB_PROTO_STATIC  COB_DLL_EXPIMP
COB_DLL_EXPIMP int cob_get_numdisp_1  (COB_RTD, const unsigned char *data, const size_t size);
COB_DLL_EXPIMP int cob_get_numdisp_2  (COB_RTD, const unsigned char *data, const size_t size);
COB_DLL_EXPIMP int cob_get_numdisp    (const unsigned char *data, const size_t size);
COB_DLL_EXPIMP int cob_cmp_packed_int (const cob_field *f, const int n);
COB_DLL_EXPIMP int cob_get_packed_int (const cob_field *f);
COB_DLL_EXPIMP int cob_add_packed_int (COB_RTD, cob_field *f, const int val);
COB_DLL_EXPIMP int cob_cmp_double_int (const double val, const int n);
COB_DLL_EXPIMP int cob_cmp_double_double (const double val, const double n);

COB_DLL_EXPIMP int cob_cmp_u8_binary  (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s8_binary  (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_u16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_u24_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s24_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_u32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_u40_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s40_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_u48_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s48_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_u56_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s56_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_u64_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_s64_binary (const unsigned char *p, const int n);

   /* #ifndef     ALLOW_MISALIGNED */
COB_DLL_EXPIMP int cob_cmp_align_u16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_align_s16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_align_u32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_align_s32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_align_u64_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmp_align_s64_binary (const unsigned char *p, const int n);
  /*  #endif */

    #ifndef WORDS_BIGENDIAN
COB_DLL_EXPIMP int cob_cmpswp_u16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_s16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_u24_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_s24_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_u32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_s32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_u40_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_s40_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_u48_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_s48_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_u56_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_s56_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_u64_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_s64_binary (const unsigned char *p, const int n);

      /*  #ifndef     ALLOW_MISALIGNED */
COB_DLL_EXPIMP int cob_cmpswp_align_u16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_align_s16_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_align_u32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_align_s32_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_align_u64_binary (const unsigned char *p, const int n);
COB_DLL_EXPIMP int cob_cmpswp_align_s64_binary (const unsigned char *p, const int n);
      /*  #endif    */                                       
COB_DLL_EXPIMP void cob_setswp_u16_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_s16_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_u24_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_s24_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_u32_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_s32_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_u40_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_s40_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_u48_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_s48_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_u56_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_s56_binary (unsigned char *p, const int val);
COB_DLL_EXPIMP void cob_setswp_u64_binary (unsigned char *p, const long long val);
COB_DLL_EXPIMP void cob_setswp_s64_binary (unsigned char *p, const long long val);
    #endif

    #ifndef COB_LIB_INCLUDE
/* Add/Subtract/multiply/divise */
        #define COB_OPER_NAME(suff) cob_add ## suff
        #define COB_OPER +=
        #include "gen_oper.h"

        #undef COB_OPER_NAME
        #undef COB_OPER 

        #define COB_OPER_NAME(suff) cob_sub ## suff
        #define COB_OPER -=
        #include "gen_oper.h"

        #undef COB_OPER_NAME
        #undef COB_OPER 

        #define COB_OPER_NAME(suff) cob_mul ## suff
        #define COB_OPER *=
        #include "gen_oper.h"

        #undef COB_OPER_NAME
        #undef COB_OPER 

        #define COB_OPER_NAME(suff) cob_div ## suff
        #define COB_OPER /=
        #include "gen_oper.h"

        #undef COB_OPER_NAME
        #undef COB_OPER 
    #endif

#endif

#if     defined(COB_LOCAL_INLINE) || defined(COB_LIB_INCLUDE)

COB_STATIC int
cob_get_numdisp_1 (COB_RTD, const unsigned char *data, const size_t size)
{
    int     retval = 0;
    size_t  n;

    for ( n = 0; n < size; n++, data++ ) {
        retval *= 10;
        /**/
        if ( *data > CHAR_9 ) {
            retval += 10;
        } else if (*data >= CHAR_0 ) {
            retval += (*data - (unsigned char)CHAR_0);
        }
    }
    return retval;
}

COB_STATIC int
cob_get_numdisp_2 (COB_RTD, const unsigned char *data, const size_t size)
{
    int     retval = 0;
    size_t  n;

    for ( n = 0; n < size; n++, data++ ) {
        retval *= 10;
        
        retval += (*data & 0x0f);
    }
    return retval;
}

COB_STATIC int
cob_get_numdisp (const unsigned char *data, const size_t size)
{
    return cob_get_numdisp_1(cob_get_rtd(),data,size);
}

COB_STATIC int
cob_cmp_packed_int (const cob_field *f, const int n)
{
    unsigned char   *p;
    int     val;

    p = f->data;
    /* 
    { 
        size_t       size;
        for ( size = 0; size < f->size - 1; size++, p++ ) {
            val *= 10;
            val += *p >> 4;
            val *= 10;
            val += *p & 0x0f;
        }
        val *= 10;
        val += *p >> 4; 
    } 
    */
    val = cob_packed_extract_uint(p, f->attr->digits, 0) ;
    p+=f->size-1;
    if ( (*p & 0x0f) == 0x0d ) {
        val = -val;
    }
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_get_packed_int (const cob_field *f)
{
    unsigned char   *p;
    register int val = 0;

    
    p = f->data;
    /* 
    { 
        size_t       size;
        size_t       sz;
        sz = f->size - 1;
        for ( size = 0; size < sz; size++, p++ ) {
            val *= 10;
            val += *p >> 4;
            val *= 10;
            val += *p & 0x0f;
        }
        val *= 10;
        val += *p >> 4;
     }
    */
    val = cob_packed_extract_uint(p, f->attr->digits, 0) ;
    p+=f->size-1;
    if ( (*p & 0x0f) == 0x0d ) {
        val = -val;
    }
    return val;
}

COB_STATIC int
cob_add_packed_int (COB_RTD, cob_field *f, const int val)
{
    unsigned char   *p;
    size_t          size;
    int             carry = 0;
    int             n;
    int             inc;
    unsigned char   sign;

    if ( val == 0 ) {
        return 0;
    }
    p = f->data + f->size - 1;
    sign = *p & 0x0f; 
    if ( sign == 0x0d ) {
        if ( val > 0 ) {
            return cob_add_int (rtd, f, val);
        }
        n = -val;
    } else {
        if ( val < 0 ) {
            return cob_add_int (rtd, f, val);
        }
        n = val;
        if ((sign != 0x0c) && (sign != 0x0f) ) {
            if ( !COB_FIELD_HAVE_SIGN (f) ) {
                *p = (*p &0xf0) | 0x0f;            
            } else {
                *p = (*p &0xf0) | 0x0c;            
            }
        }

    }
    inc = (*p >> 4) + (n % 10);
    n /= 10;
    carry = inc / 10;
    *p = ((inc % 10) << 4) | (*p & 0x0f);
    p--;

    for ( size = 0; size < f->size - 1; size++, p-- ) {
        if ( !carry && !n ) {
            break;
        }
        inc = ((*p >> 4) * 10) + (*p & 0x0f) + carry + (n % 100);
        carry = inc / 100;
        n /= 100;
        inc %= 100;
        *p = ((inc / 10) << 4) | (inc % 10);
    }
    if ( f->attr->digits %2 ==0 ) {
        /*need to check overflow*/
        p = f->data;
        if ( *p & 0xf0 ) {
            /* overflow */
            /* if the statement has ON SIZE ERROR or NOT ON SIZE ERROR,
               then throw an exception,
               This speed up procedure is oly call is NO SIZE ERROR is PROVIDED
               Just truncate the field */
            *p = *p & 0x0F;
        }
    }
    return 0;
}

/* Aligned variants */

/*    #ifndef     ALLOW_MISALIGNED */

/* Aligned compares */

COB_STATIC int
cob_cmp_align_u16_binary (const unsigned char *p, const int n)
{
    unsigned short  val;

    if ( n < 0 ) {
        return 1;
    }
    val = *(unsigned short MISALIGNED *)p;
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_align_s16_binary (const unsigned char *p, const int n)
{
    short   val;

    val = *(short MISALIGNED *)p;
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_align_u32_binary (const unsigned char *p, const int n)
{
    unsigned int    val;

    if ( n < 0 ) {
        return 1;
    }
    val = *(unsigned int MISALIGNED *)p;
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_align_s32_binary (const unsigned char *p, const int n)
{
    int val;

    val = *(int MISALIGNED *)p;
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_align_u64_binary (const unsigned char *p, const int n)
{
    unsigned long long  val;

    if ( n < 0 ) {
        return 1;
    }
    val = *(unsigned long long MISALIGNED *)p;
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_align_s64_binary (const unsigned char *p, const int n)
{
    long long   val;

    val = *(long long MISALIGNED *)p;
    return(val < n) ? -1 : (val > n);
}


        #ifndef WORDS_BIGENDIAN
COB_STATIC int
cob_cmpswp_align_u16_binary (const unsigned char *p, const int n)
{
    unsigned short  val;

    if ( n < 0 ) {
        return 1;
    }
    val = COB_BSWAP_16 (*(unsigned short MISALIGNED *)p);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_align_s16_binary (const unsigned char *p, const int n)
{
    short   val;

    val = COB_BSWAP_16 (*(short MISALIGNED *)p);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_align_u32_binary (const unsigned char *p, const int n)
{
    unsigned int    val;

    if ( n < 0 ) {
        return 1;
    }
    val = COB_BSWAP_32 (*(unsigned int MISALIGNED *)p);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_align_s32_binary (const unsigned char *p, const int n)
{
    int val;

    val = COB_BSWAP_32 (*(int MISALIGNED *)p);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_align_u64_binary (const unsigned char *p, const int n)
{
    unsigned long long  val;

    if ( n < 0 ) {
        return 1;
    }
    val = COB_BSWAP_64 (*(unsigned long long MISALIGNED *)p);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_align_s64_binary (const unsigned char *p, const int n)
{
    long long   val;

    val = COB_BSWAP_64 (*(long long MISALIGNED *)p);
    return(val < n) ? -1 : (val > n);
}

        #endif  /* WORDS_BIGENDIAN */
/*    #endif   */   /* ALLOW_MISALIGNED */

/* Binary compare */

COB_STATIC int
cob_cmp_u8_binary (const unsigned char *p, const int n)
{
    if ( n < 0 ) {
        return 1;
    }
    return(*p < n) ? -1 : (*p > n);
}

COB_STATIC int
cob_cmp_s8_binary (const unsigned char *p, const int n)
{
    return(*(const signed char *)p < n) ? -1 : (*(const signed char *)p > n);
}

COB_STATIC int
cob_cmp_u16_binary (const unsigned char *p, const int n)
{
#ifndef ALLOW_MISALIGNED
    unsigned char   *x;
#endif
    unsigned short  val;

    if ( n < 0 ) {
        return 1;
    }
#ifdef ALLOW_MISALIGNED
    val = *(const unsigned short MISALIGNED *)p;
#else
    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_double_int (const double val, const int n)
{
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_double_double (const double val, const double n)
{
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_s16_binary (const unsigned char *p, const int n)
{
    short   val;

#ifdef ALLOW_MISALIGNED
    val = *(const short MISALIGNED *)p;
#else
    unsigned char   *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_u24_binary (const unsigned char *p, const int n)
{
    unsigned char   *x;
    unsigned int    val = 0;

    if ( n < 0 ) {
        return 1;
    }
#ifdef      WORDS_BIGENDIAN
    x = ((unsigned char *)&val) + 1;
#else
    x = (unsigned char *)&val;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_s24_binary (const unsigned char *p, const int n)
{
    unsigned char   *x;
    int     val = 0;

#ifdef      WORDS_BIGENDIAN
    x = (unsigned char *)&val;
#else
    x = ((unsigned char *)&val) + 1;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    val >>= 8;  /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_u32_binary (const unsigned char *p, const int n)
{
#ifndef ALLOW_MISALIGNED
    unsigned char   *x;
#endif
    unsigned int    val;

    if ( n < 0 ) {
        return 1;
    }
#ifdef ALLOW_MISALIGNED
    val = *(const unsigned int MISALIGNED *)p;
#else
    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_s32_binary (const unsigned char *p, const int n)
{
    int val;

#ifdef ALLOW_MISALIGNED
    val = *(const int MISALIGNED *)p;
#else
    unsigned char   *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_u40_binary (const unsigned char *p, const int n)
{
    unsigned long long  val = 0;
    unsigned char       *x;

    if ( n < 0 ) {
        return 1;
    }
#ifdef      WORDS_BIGENDIAN
    x = ((unsigned char *)&val) + 3;
#else
    x = (unsigned char *)&val;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_s40_binary (const unsigned char *p, const int n)
{
    long long       val = 0;
    unsigned char       *x;

#ifdef      WORDS_BIGENDIAN
    x = (unsigned char *)&val;
#else
    x = ((unsigned char *)&val) + 3;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    val >>= 24; /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_u48_binary (const unsigned char *p, const int n)
{
    unsigned long long  val = 0;
    unsigned char       *x;

    if ( n < 0 ) {
        return 1;
    }
#ifdef      WORDS_BIGENDIAN
    x = ((unsigned char *)&val) + 2;
#else
    x = (unsigned char *)&val;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_s48_binary (const unsigned char *p, const int n)
{
    long long       val = 0;
    unsigned char       *x;

#ifdef      WORDS_BIGENDIAN
    x = (unsigned char *)&val;
#else
    x = ((unsigned char *)&val) + 2;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    val >>= 16; /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_u56_binary (const unsigned char *p, const int n)
{
    unsigned long long  val = 0;
    unsigned char       *x;

    if ( n < 0 ) {
        return 1;
    }
#ifdef      WORDS_BIGENDIAN
    x = ((unsigned char *)&val) + 1;
#else
    x = (unsigned char *)&val;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_s56_binary (const unsigned char *p, const int n)
{
    long long       val = 0;
    unsigned char       *x;

#ifdef      WORDS_BIGENDIAN
    x = (unsigned char *)&val;
#else
    x = ((unsigned char *)&val) + 1;
#endif
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    val >>= 8;  /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_u64_binary (const unsigned char *p, const int n)
{
#ifndef ALLOW_MISALIGNED
    unsigned char       *x;
#endif
    unsigned long long  val;

    if ( n < 0 ) {
        return 1;
    }
#ifdef ALLOW_MISALIGNED
    val = *(const unsigned long long MISALIGNED *)p;
#else
    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    *(x + 7) = *(p + 7);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmp_s64_binary (const unsigned char *p, const int n)
{
    long long   val;

#ifdef ALLOW_MISALIGNED
    val = *(const long long MISALIGNED *)p;
#else
    unsigned char       *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    *(x + 7) = *(p + 7);
#endif
    return(val < n) ? -1 : (val > n);
}

/* Add/Subtract/multiply/divise */
    #define COB_OPER_NAME(suff) cob_add ## suff
    #define COB_OPER +=
    #include "gen_oper.h"

    #undef COB_OPER_NAME
    #undef COB_OPER 

    #define COB_OPER_NAME(suff) cob_sub ## suff
    #define COB_OPER -=
    #include "gen_oper.h"

    #undef COB_OPER_NAME
    #undef COB_OPER 

    #define COB_OPER_NAME(suff) cob_mul ## suff
    #define COB_OPER *=
    #include "gen_oper.h"

    #undef COB_OPER_NAME
    #undef COB_OPER 

    #define COB_OPER_NAME(suff) cob_div ## suff
    #define COB_OPER /=
    #include "gen_oper.h"

    #undef COB_OPER_NAME
    #undef COB_OPER 

    #ifndef WORDS_BIGENDIAN

/* Binary swapped compare */
COB_STATIC int
cob_cmpswp_u16_binary (const unsigned char *p, const int n)
{
#ifndef ALLOW_MISALIGNED
    unsigned char   *x;
#endif
    unsigned short  val;

    if ( n < 0 ) {
        return 1;
    }
#ifdef ALLOW_MISALIGNED
    val = COB_BSWAP_16 (*(unsigned short MISALIGNED *)p);
#else
    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    val = COB_BSWAP_16 (val);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_s16_binary (const unsigned char *p, const int n)
{
    short   val;

#ifdef ALLOW_MISALIGNED
    val = COB_BSWAP_16 (*(short MISALIGNED *)p);
#else
    unsigned char   *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    val = COB_BSWAP_16 (val);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_u24_binary (const unsigned char *p, const int n)
{
    unsigned char   *x;
    unsigned int    val = 0;

    if ( n < 0 ) {
        return 1;
    }
    x = ((unsigned char *)&val) + 1;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    val = COB_BSWAP_32 (val);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_s24_binary (const unsigned char *p, const int n)
{
    unsigned char   *x;
    int     val = 0;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    val = COB_BSWAP_32 (val);
    val >>= 8;  /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_u32_binary (const unsigned char *p, const int n)
{
#ifndef ALLOW_MISALIGNED
    unsigned char   *x;
#endif
    unsigned int    val;

    if ( n < 0 ) {
        return 1;
    }
#ifdef ALLOW_MISALIGNED
    val = COB_BSWAP_32 (*(const unsigned int MISALIGNED *)p);
#else
    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    val = COB_BSWAP_32 (val);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_s32_binary (const unsigned char *p, const int n)
{
    int val;

#ifdef ALLOW_MISALIGNED
    val = COB_BSWAP_32 (*(const int MISALIGNED *)p);
#else
    unsigned char   *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    val = COB_BSWAP_32 (val);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_u40_binary (const unsigned char *p, const int n)
{
    unsigned long long  val = 0;
    unsigned char   *x;

    if ( n < 0 ) {
        return 1;
    }
    x = ((unsigned char *)&val) + 3;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    val = COB_BSWAP_64 (val);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_s40_binary (const unsigned char *p, const int n)
{
    long long   val = 0;
    unsigned char   *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    val = COB_BSWAP_64 (val);
    val >>= 24; /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_u48_binary (const unsigned char *p, const int n)
{
    unsigned long long  val = 0;
    unsigned char   *x;

    if ( n < 0 ) {
        return 1;
    }
    x = ((unsigned char *)&val) + 2;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    val = COB_BSWAP_64 (val);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_s48_binary (const unsigned char *p, const int n)
{
    long long   val = 0;
    unsigned char   *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    val = COB_BSWAP_64 (val);
    val >>= 16; /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_u56_binary (const unsigned char *p, const int n)
{
    unsigned long long  val = 0;
    unsigned char   *x;

    if ( n < 0 ) {
        return 1;
    }
    x = ((unsigned char *)&val) + 1;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    val = COB_BSWAP_64 (val);
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_s56_binary (const unsigned char *p, const int n)
{
    long long   val = 0;
    unsigned char   *x;

    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    val = COB_BSWAP_64 (val);
    val >>= 8;  /* shift with sign */
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_u64_binary (const unsigned char *p, const int n)
{
#ifndef ALLOW_MISALIGNED
    unsigned char       *x;
#endif
    unsigned long long  val;

    if ( n < 0 ) {
        return 1;
    }
#ifdef ALLOW_MISALIGNED
    val = COB_BSWAP_64 (*(const unsigned long long MISALIGNED *)p);
#else
    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    *(x + 7) = *(p + 7);
    val = COB_BSWAP_64 (val);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC int
cob_cmpswp_s64_binary (const unsigned char *p, const int n)
{
    long long   val;

#ifdef ALLOW_MISALIGNED
    val = COB_BSWAP_64 (*(const long long MISALIGNED *)p);
#else
    unsigned char   *x;
    x = (unsigned char *)&val;
    *x = *p;
    *(x + 1) = *(p + 1);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 4);
    *(x + 5) = *(p + 5);
    *(x + 6) = *(p + 6);
    *(x + 7) = *(p + 7);
    val = COB_BSWAP_64 (val);
#endif
    return(val < n) ? -1 : (val > n);
}

COB_STATIC void
cob_setswp_u16_binary (unsigned char *p, const int val)
{
    unsigned short  n;

#ifdef ALLOW_MISALIGNED
    n = val;
    *(unsigned short MISALIGNED *)p = COB_BSWAP_16(n);
#else
    n = val;
    p[0] = (unsigned char)(n >> 8);
    p[1] = (unsigned char)n;
#endif
}

COB_STATIC void
cob_setswp_s16_binary (unsigned char *p, const int val)
{
    short       n;

#ifdef ALLOW_MISALIGNED
    n = val;
    *(short MISALIGNED *)p = COB_BSWAP_16(n);
#else
    n = val;
    p[0] = (unsigned char)(n >> 8);
    p[1] = (unsigned char)n;
#endif
}

COB_STATIC void
cob_setswp_u24_binary (unsigned char *p, const int val)
{
    unsigned char   *x;
    unsigned int    n;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 2);
    *(p + 1) = *(x + 1);
    *(p + 2) = *x;
}

COB_STATIC void
cob_setswp_s24_binary (unsigned char *p, const int val)
{
    unsigned char   *x;
    int     n;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 2);
    *(p + 1) = *(x + 1);
    *(p + 2) = *x;
}

COB_STATIC void
cob_setswp_u32_binary (unsigned char *p, const int val)
{
    unsigned int    n;

#ifdef ALLOW_MISALIGNED
    n = val;
    *(unsigned int MISALIGNED *)p = COB_BSWAP_32(n);
#else
    n = val;
    *p++ = (unsigned char)(n >> 24);
    *p++ = (unsigned char)(n >> 16);
    *p++ = (unsigned char)(n >> 8);
    *p = (unsigned char)n;
#endif
}

COB_STATIC void
cob_setswp_s32_binary (unsigned char *p, const int val)
{
    int     n;

#ifdef ALLOW_MISALIGNED
    n = val;
    *(int MISALIGNED *)p = COB_BSWAP_32(n);
#else
    n = val;
    *p++ = (unsigned char)(n >> 24);
    *p++ = (unsigned char)(n >> 16);
    *p++ = (unsigned char)(n >> 8);
    *p = (unsigned char)n;
#endif
}

COB_STATIC void
cob_setswp_u40_binary (unsigned char *p, const int val)
{
    unsigned long long  n;
    unsigned char       *x;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 4);
    *(p + 1) = *(x + 3);
    *(p + 2) = *(x + 2);
    *(p + 3) = *(x + 1);
    *(p + 4) = *x;
}

COB_STATIC void
cob_setswp_s40_binary (unsigned char *p, const int val)
{
    long long       n;
    unsigned char       *x;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 4);
    *(p + 1) = *(x + 3);
    *(p + 2) = *(x + 2);
    *(p + 3) = *(x + 1);
    *(p + 4) = *x;
}

COB_STATIC void
cob_setswp_u48_binary (unsigned char *p, const int val)
{
    unsigned long long  n;
    unsigned char       *x;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 5);
    *(p + 1) = *(x + 4);
    *(p + 2) = *(x + 3);
    *(p + 3) = *(x + 2);
    *(p + 4) = *(x + 1);
    *(p + 5) = *x;
}

COB_STATIC void
cob_setswp_s48_binary (unsigned char *p, const int val)
{
    long long       n;
    unsigned char       *x;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 5);
    *(p + 1) = *(x + 4);
    *(p + 2) = *(x + 3);
    *(p + 3) = *(x + 2);
    *(p + 4) = *(x + 1);
    *(p + 5) = *x;
}

COB_STATIC void
cob_setswp_u56_binary (unsigned char *p, const int val)
{
    unsigned long long  n;
    unsigned char       *x;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 6);
    *(p + 1) = *(x + 5);
    *(p + 2) = *(x + 4);
    *(p + 3) = *(x + 3);
    *(p + 4) = *(x + 2);
    *(p + 5) = *(x + 1);
    *(p + 6) = *x;
}

COB_STATIC void
cob_setswp_s56_binary (unsigned char *p, const int val)
{
    long long       n;
    unsigned char       *x;

    n = val;
    x = (unsigned char *)&n;
    *p = *(x + 6);
    *(p + 1) = *(x + 5);
    *(p + 2) = *(x + 4);
    *(p + 3) = *(x + 3);
    *(p + 4) = *(x + 2);
    *(p + 5) = *(x + 1);
    *(p + 6) = *x;
}

COB_STATIC void
cob_setswp_u64_binary (unsigned char *p, const long long val)
{
#ifdef ALLOW_MISALIGNED
    unsigned long long  n;

    n = val;
    *(unsigned long long MISALIGNED *)p = COB_BSWAP_64(n);
#else
    size_t  i;
    union {
        unsigned long long  n;
        unsigned char       c[8];
    } u;

    u.n = val;
    for ( i = 0; i < 8; ++i ) {
        p[i] = u.c[7-i];
    }
#endif
}

COB_STATIC void
cob_setswp_s64_binary (unsigned char *p, const long long val)
{
#ifdef ALLOW_MISALIGNED
    long long   n;

    n = val;
    *(long long MISALIGNED *)p = COB_BSWAP_64(n);
#else
    size_t  i;
    union {
        long long   n;
        unsigned char   c[8];
    } u;

    u.n = val;
    for ( i = 0; i < 8; ++i ) {
        p[i] = u.c[7-i];
    }
#endif
}

    #endif      /* WORDS_BIGENDIAN */

#endif  /* COB_LOCAL_INLINE || COB_LIB_INCLUDE */

#endif  /* COB_CODEGEN_H */

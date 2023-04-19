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
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

#include "common.h"
#include "move.h"
#include "coblocal.h"
#include "numeric.h"
#include "strings.h"
#include "intrinsic.h"

#define INSPECT_ALL         0
#define INSPECT_LEADING     1
#define INSPECT_FIRST       2
#define INSPECT_TRAILING    3

#define DLM_DEFAULT_NUM     8

#ifndef memmem
    #ifndef HAVE_MEMMEM
/* Return the first occurrence of NEEDLE in HAYSTACK. */
static void *
memmem (const unsigned char *haystack, size_t haystack_len, const unsigned char *needle, size_t needle_len)
{
    /* not really Rabin-Karp, just using additive hashing */
    unsigned char* haystack_ = (unsigned char*)haystack;
    unsigned char* needle_ = (unsigned char*)needle;
    int hash = 0;     /* this is the static hash value of the needle */
    int hay_hash = 0; /* rolling hash over the haystack */
    unsigned char* last;
    size_t i;

    if (haystack_len < needle_len)
        return NULL;

    if (!needle_len)
        return haystack_;

    /* initialize hashes */
    for (i = needle_len; i; --i) {
        hash += *needle_++;
        hay_hash += *haystack_++;
    }

    /* iterate over the haystack */
    haystack_ = (unsigned char*)haystack;
    needle_ = (unsigned char*)needle;
    last = haystack_+(haystack_len - needle_len + 1);
    for (; haystack_ < last; ++haystack_) {
        if (unlikely(hash == hay_hash) &&
            *haystack_ == *needle_ && /* prevent calling memcmp, was a optimization from existing glibc */
            !memcmp (haystack_, needle_, needle_len))
            return haystack_;

        /* roll the hash */
        hay_hash -= *haystack_;
        hay_hash += *(haystack_+needle_len);
    }

    return NULL;
}
    #else
void * memmem (const void *haystack, size_t haystack_len, const void *needle, size_t needle_len);
    #endif

#endif

#include <string.h>

static void
alloc_figurative (COB_RTD, const cob_field *f1, const cob_field *f2)
{
    unsigned char   *s;
    size_t          size1;
    size_t          size2;
    size_t          n;

    size2 = f2->size;
    if ( size2 > rtd->figsize ) {
        if ( rtd->figptr ) {
            cob_free (rtd->figptr);
        }
        rtd->figptr = cob_malloc (rtd, size2);
        rtd->figsize = size2;
    }
    size1 = 0;
    s = rtd->figptr;
    for ( n = 0; n < size2; n++, s++ ) {
        *s = f1->data[size1];
        size1++;
        if ( size1 >= f1->size ) {
            size1 = 0;
        }
    }
    (rtd->alpha_fld).size = size2;
    (rtd->alpha_fld).data = rtd->figptr;
}

static cob_field *
check_national (COB_RTD, char national,  const cob_field *f)
{

    if ( national && !COB_FIELD_IS_NATIONAL(f) ) {
        return cob_intr_national_of(rtd,1,f);
    }
    if ( !national && COB_FIELD_IS_NATIONAL(f) ) {
        return cob_intr_display_of(rtd,1,f);
    }

    return(cob_field *)f;
}

static void
inspect_common (COB_RTD, cob_field *f1, cob_field *f2, const int type)
{
    int     *mark;
    size_t  n = 0;
    size_t  j;
    int     i;
    int     len;
    cob_field *orgf1;
    unsigned char * inspect_start;
    unsigned char * inspect_end;
    int inspect_replacing ;

    if ( unlikely(!f1) ) {
        f1 = &cob_low;
    }
    if ( unlikely(!f2) ) {
        f2 = &cob_low;
    }

    orgf1 = f1;
    f1 = check_national(rtd,rtd->inspect_national, f1);
    f2 = check_national(rtd,rtd->inspect_national, f2);
    if ( (rtd->inspect_replacing) && f1->size != f2->size ) {
        if ( COB_FIELD_TYPE (orgf1) == COB_TYPE_ALPHANUMERIC_ALL ) {
            alloc_figurative (rtd, f1, f2);
            f1 = &(rtd->alpha_fld);
        } else {
            cob_set_exception (rtd, COB_EC_RANGE_INSPECT_SIZE);
            return;
        }
    }

    mark = &(rtd->inspect_mark)[(rtd->inspect_start) - (rtd->inspect_data)];
    inspect_start = rtd->inspect_start;
    inspect_end = rtd->inspect_end;
    inspect_replacing = rtd->inspect_replacing;
    len = (int)((inspect_end) - (inspect_start));
    if ( type == INSPECT_TRAILING ) {
        for ( i = len - f2->size; i >= 0; i-- ) {
            /* find matching substring */
            if ( memcmp ((inspect_start) + i, f2->data, f2->size) == 0 ) {
                /* check if it is already marked */
                for ( j = 0; j < f2->size; j++ ) {
                    if ( mark[i + j] != -1 ) {
                        break;
                    }
                }
                /* if not, mark and count it */
                if ( j == f2->size ) {
                    for ( j = 0; j < f2->size; j++ ) {
                        mark[i + j] = (inspect_replacing) ? f1->data[j] : 1;
                    }
                    i -= f2->size - 1;
                    n++;
                }
            } else {
                break;
            }
        }
    } else {
        int l = (int)(len - f2->size + 1);
        for ( i = 0; i < l; i++ ) {
            /* find matching substring */
            int tested_i = i;
            unsigned char *p =memmem((inspect_start) + i, len-i, (f2->data),  f2->size); 
            if (p) {
                i = p- rtd->inspect_start;
                //if ( memcmp ((rtd->inspect_start) + i, f2->data, f2->size) == 0 ) 
                {
                    if ( type == INSPECT_LEADING  && i != tested_i) {
                        break;
                    }                     /* check if it is already marked */
                    for ( j = 0; j < f2->size; j++ ) {
                        if ( mark[i + j] != -1 ) {
                            break;
                        }
                    }
                    /* if not, mark and count it */
                    if ( j == f2->size ) {
                        for ( j = 0; j < f2->size; j++ ) {
                            mark[i + j] = (inspect_replacing) ? f1->data[j] : 1;
                        }
                        i += f2->size - 1;
                        n++;
                        if ( type == INSPECT_FIRST ) {
                            break;
                        }
                    }
                } 
                /*
                else if ( type == INSPECT_LEADING ) {
                    break;
                } 
                */ 
            } else {
                break;
            }
        }

    }

    if ( n > 0 && !(rtd->inspect_replacing) ) {
        /*if ( rtd->inspect_national ) {
            n /= 2;
        }*/
        cob_add_int (rtd, orgf1, (int) n);
    }
}

/*
 * INSPECT
 */

void
cob_inspect_init (COB_RTD, cob_field *var, const int replacing)
{
    size_t      i;
    size_t      digcount;

    rtd->inspect_var_copy = *var;
    rtd->inspect_var = &(rtd->inspect_var_copy);
    rtd->inspect_replacing = replacing;
    rtd->inspect_sign = COB_GET_SIGN (var);
    rtd->inspect_size = COB_FIELD_SIZE (var);
    rtd->inspect_data = COB_FIELD_DATA (var);
    rtd->inspect_start = NULL;
    rtd->inspect_end = NULL;
    rtd->inspect_national = COB_FIELD_IS_NATIONAL(var);
    digcount = (rtd->inspect_size) * sizeof (int);
    if ( digcount > (rtd->inspect_lastsize) ) {
        free ((rtd->inspect_mark));
        (rtd->inspect_mark) = cob_malloc (rtd, digcount);
        (rtd->inspect_lastsize) = digcount;
    }
    for ( i = 0; i < (rtd->inspect_size); i++ ) {
        (rtd->inspect_mark)[i] = -1;
    }
    rtd->cob_exception_code = 0;
}

void
cob_inspect_start (COB_RTD)
{
    (rtd->inspect_start) = (rtd->inspect_data);
    (rtd->inspect_end) = (rtd->inspect_data) + (rtd->inspect_size);
}

void
cob_inspect_before (COB_RTD, cob_field *str)
{
    unsigned char   *p;

    str = check_national(rtd, rtd->inspect_national,str);
    for ( p = (rtd->inspect_start); p < (rtd->inspect_end) - str->size + 1; p++ ) {
        if ( memcmp (p, str->data, str->size) == 0 ) {
            (rtd->inspect_end) = p;
            return;
        }
    }
}

void
cob_inspect_after (COB_RTD, cob_field *str)
{
    unsigned char   *p;

    str = check_national(rtd, rtd->inspect_national,str);

    for ( p = (rtd->inspect_start); p < (rtd->inspect_end) - str->size + 1; p++ ) {
        if ( memcmp (p, str->data, str->size) == 0 ) {
            (rtd->inspect_start) = p + str->size;
            return;
        }
    }
    (rtd->inspect_start) = (rtd->inspect_end);
}

void
cob_inspect_characters (COB_RTD, cob_field *f1)
{
    int *mark;
    int i;
    int n;
    int len;
    cob_field *orgf1 =  f1;

    f1 = check_national(rtd, rtd->inspect_national,f1);

    mark = &(rtd->inspect_mark)[(rtd->inspect_start) - (rtd->inspect_data)];
    len = (int)((rtd->inspect_end) - (rtd->inspect_start));
    if ( (rtd->inspect_replacing) ) {
        /* INSPECT REPLACING CHARACTERS f1 */
        for ( i = 0; i < len; i++ ) {
            if ( mark[i] == -1 ) {
                mark[i] = f1->data[0];
            }
        }
    } else {
        /* INSPECT TALLYING f1 CHARACTERS */
        n = 0;
        for ( i = 0; i < len; i++ ) {
            if ( mark[i] == -1 ) {
                mark[i] = 1;
                n++;
            }
        }
        if ( n > 0 ) {
            if ( rtd->inspect_national ) {
                n /= 2;
            }
            cob_add_int (rtd, orgf1, n);
        }
    }
}

void
cob_inspect_all (COB_RTD, cob_field *f1, cob_field *f2)
{
    inspect_common (rtd, f1, f2, INSPECT_ALL);
}

void
cob_inspect_leading (COB_RTD, cob_field *f1, cob_field *f2)
{
    inspect_common (rtd, f1, f2, INSPECT_LEADING);
}

void
cob_inspect_first (COB_RTD, cob_field *f1, cob_field *f2)
{
    inspect_common (rtd, f1, f2, INSPECT_FIRST);
}

void
cob_inspect_trailing (COB_RTD, cob_field *f1, cob_field *f2)
{
    inspect_common (rtd, f1, f2, INSPECT_TRAILING);
}

void
cob_inspect_converting (COB_RTD, cob_field *f1, cob_field *f2)
{
    size_t  i;
    size_t  j;
    size_t  len;

    f1 = check_national(rtd, rtd->inspect_national,f1);
    f2 = check_national(rtd, rtd->inspect_national,f2);

    len = (size_t)((rtd->inspect_end) - (rtd->inspect_start));
    for ( j = 0; j < f1->size; j++ ) {
        for ( i = 0; i < len; i++ ) {
            if ( (rtd->inspect_mark)[i] == -1 && (rtd->inspect_start)[i] == f1->data[j] ) {
                (rtd->inspect_start)[i] = f2->data[j%(f2->size)];
                (rtd->inspect_mark)[i] = 1;
            }
        }
    }
}

void
cob_inspect_finish (COB_RTD)
{
    size_t  i;

    if ( (rtd->inspect_replacing) ) {
        for ( i = 0; i < (rtd->inspect_size); i++ ) {
            if ( (rtd->inspect_mark)[i] != -1 ) {
                (rtd->inspect_data)[i] = (rtd->inspect_mark)[i];
            }
        }
    }

    COB_PUT_SIGN ((rtd->inspect_var), (rtd->inspect_sign));
}

/*
 * STRING
 */

void
cob_string_init (COB_RTD, cob_field *dst, cob_field *ptr)
{
    rtd->string_national = COB_FIELD_IS_NATIONAL(dst);
    rtd->string_dst_copy = *dst;
    rtd->string_dst = &(rtd->string_dst_copy);
    rtd->string_ptr = NULL;
    if ( ptr ) {
        (rtd->string_ptr_copy) = *ptr;
        (rtd->string_ptr) = &(rtd->string_ptr_copy);
    }
    rtd->string_offset = 0;
    rtd->cob_exception_code = 0;
    rtd->string_overflow=0;

    if ( rtd->string_ptr ) {
        (rtd->string_offset) = cob_get_int (rtd, (rtd->string_ptr)) - 1;
        if ( rtd->string_national ) {
            rtd->string_offset *=2;
        }
        if ( (rtd->string_offset) < 0 || (rtd->string_offset) >= (int)(rtd->string_dst)->size ) {
            rtd->string_overflow = 1;
            cob_set_exception (rtd, COB_EC_OVERFLOW_STRING);
        }
    }
}

void
cob_string_delimited (COB_RTD, cob_field *dlm)
{
    (rtd->string_dlm) = NULL;
    if ( dlm ) {
        dlm = check_national(rtd, rtd->string_national,dlm);
        (rtd->string_dlm_copy) = *dlm;
        (rtd->string_dlm) = &(rtd->string_dlm_copy);
    }
}

void
cob_string_append (COB_RTD, cob_field *src)
{
    size_t  src_size;
    int i;
    int size;

    if ( rtd->string_overflow || rtd->cob_exception_code ) {
        if (rtd->string_overflow)
            cob_set_exception (rtd, COB_EC_OVERFLOW_STRING);
        return;
    }

    src = check_national(rtd, rtd->string_national,src);
    src_size = src->size;
    if ( (rtd->string_dlm) ) {
        size = (int)(src_size - (rtd->string_dlm)->size + 1);
        for ( i = 0; i < size; i++ ) {
            if ( memcmp (src->data + i, (rtd->string_dlm)->data, (rtd->string_dlm)->size) == 0 ) {
                src_size = i;
                break;
            }
        }
    }

    if ( src_size <= (rtd->string_dst)->size - (rtd->string_offset) ) {
        memcpy ((rtd->string_dst)->data + (rtd->string_offset), src->data, src_size);
        (rtd->string_offset) += (int) src_size;
    } else {
        size = (((int)(rtd->string_dst)->size) - ((int)(rtd->string_offset)));
        if (size > 0) {
            memcpy((rtd->string_dst)->data + (rtd->string_offset), src->data, (size_t)size);
            (rtd->string_offset) += size;
        }
        cob_set_exception (rtd, COB_EC_OVERFLOW_STRING);
    }
}

void
cob_string_finish (COB_RTD)
{
    if ( (rtd->string_ptr) ) {
        if ( rtd->string_national ) {
            cob_set_int (rtd, (rtd->string_ptr), (rtd->string_offset/2) + 1);
        } else {
            cob_set_int (rtd, (rtd->string_ptr), (rtd->string_offset) + 1);
        }
    }
}

/*
 * UNSTRING
 */

void
cob_unstring_init (COB_RTD, cob_field *src, cob_field *ptr, const size_t num_dlm)
{
    rtd->unstring_national = COB_FIELD_IS_NATIONAL(src);
    (rtd->unstring_src_copy) = *src;
    (rtd->unstring_src) = &(rtd->unstring_src_copy);
    (rtd->unstring_ptr) = NULL;
    if ( ptr ) {
        (rtd->unstring_ptr_copy) = *ptr;
        (rtd->unstring_ptr) = &(rtd->unstring_ptr_copy);
    }

    (rtd->unstring_offset) = 0;
    (rtd->unstring_count) = 0;
    (rtd->unstring_ndlms) = 0;
    rtd->cob_exception_code = 0;
    if ( !(rtd->dlm_list) ) {
        if ( num_dlm <= DLM_DEFAULT_NUM ) {
            (rtd->dlm_list) = cob_malloc (rtd, DLM_DEFAULT_NUM * sizeof(struct dlm_struct));
            rtd->udlmcount = DLM_DEFAULT_NUM;
        } else {
            (rtd->dlm_list) = cob_malloc (rtd, num_dlm * sizeof(struct dlm_struct));
            rtd->udlmcount = num_dlm;
        }
    } else {
        if ( num_dlm > rtd->udlmcount ) {
            cob_free ((rtd->dlm_list));
            (rtd->dlm_list) = cob_malloc (rtd, num_dlm * sizeof(struct dlm_struct));
            rtd->udlmcount = num_dlm;
        }
    }

    if ( (rtd->unstring_ptr) ) {
        (rtd->unstring_offset) = cob_get_int (rtd, (rtd->unstring_ptr)) - 1;
        if ( (rtd->unstring_offset) < 0 || (rtd->unstring_offset) >= (int)(rtd->unstring_src)->size ) {
            cob_set_exception (rtd, COB_EC_OVERFLOW_UNSTRING);
        }
    }
}

void
cob_unstring_delimited (COB_RTD, cob_field *dlm, const int all)
{
    if ( dlm ) {
        dlm = check_national(rtd, rtd->unstring_national,dlm);
        (rtd->dlm_list)[(rtd->unstring_ndlms)].uns_dlm = *dlm;
        (rtd->dlm_list)[(rtd->unstring_ndlms)].uns_all = all;
        (rtd->unstring_ndlms)++;
    }
}

void
cob_unstring_into (COB_RTD, cob_field *dst, cob_field *dlm, cob_field *cnt)
{
    unsigned char   *p;
    unsigned char   *dp;
    unsigned char   *s;
    unsigned char   *dlm_data;
    unsigned char   *start;
    size_t          dlm_size = 0;
    int             i;
    int             srsize;
    int             dlsize;
    int             match_size = 0;
    int             brkpt = 0;

    if ( rtd->cob_exception_code ) {
        return;
    }

    if ( (rtd->unstring_offset) >= (int)(rtd->unstring_src)->size ) {
        return;
    }

    start = (rtd->unstring_src)->data + (rtd->unstring_offset);
    dlm_data = NULL;
    if ( (rtd->unstring_ndlms) == 0 ) {
        match_size = cob_min_int ((int)COB_FIELD_SIZE (dst),
                                  (int)(rtd->unstring_src)->size - (rtd->unstring_offset));
        if (rtd->unstring_national) {
            cob_memcpy_national(rtd, dst, start, match_size);
        }else if (rtd->current_module && rtd->current_module->runtime_flags & COB_UNSTRING_MOVE) {
            cob_memcpy_string (rtd, dst, start, match_size);
        } else 
            cob_memcpy (rtd, dst, start, match_size);
        (rtd->unstring_offset) += match_size;
    } else {

        srsize = (int) (rtd->unstring_src)->size;
        s = (rtd->unstring_src)->data + srsize;
        for ( p = start; p < s; p++ ) {
            for ( i = 0; i < (rtd->unstring_ndlms); i++ ) {
                dlsize = (int) (rtd->dlm_list)[i].uns_dlm.size;
                dp = (rtd->dlm_list)[i].uns_dlm.data;
                if ( p + dlsize > s ) {
                    break;
                }
                if ( !memcmp (p, dp, (size_t)dlsize) ) {
                    match_size = (int)(p - start);

                    if (rtd->unstring_national) {
                        cob_memcpy_national(rtd, dst, start, match_size);
                    } else if (rtd->current_module && rtd->current_module->runtime_flags & COB_UNSTRING_MOVE ) {
                        cob_memcpy_string (rtd, dst, start, match_size);
                    } else 
                        cob_memcpy (rtd, dst, start, match_size);
                    (rtd->unstring_offset) += match_size + dlsize;
                    dlm_data = dp;
                    dlm_size = dlsize;
                    if ( (rtd->dlm_list)[i].uns_all ) {
                        for ( p+=dlsize ; p < s; p+=dlsize ) {
                            if ( p + dlsize > s ) {
                                break;
                            }
                            if ( memcmp (p, dp, (size_t)dlsize) ) {
                                break;
                            }
                            (rtd->unstring_offset) += dlsize;
                        }
                    }
                    brkpt = 1;
                    break;
                }
            }
            if ( brkpt ) {
                break;
            }
        }
        if ( !brkpt ) {
            /* no match */
            match_size = (int)((rtd->unstring_src)->size - (rtd->unstring_offset));
            if (rtd->unstring_national) {
                cob_memcpy_national(rtd, dst, start, match_size);
            } else if (rtd->current_module && rtd->current_module->runtime_flags & COB_UNSTRING_MOVE ) {
                cob_memcpy_string (rtd, dst, start, match_size);
            } else 
                cob_memcpy (rtd, dst, start, match_size);
            (rtd->unstring_offset) = (int) (rtd->unstring_src)->size;
            dlm_data = NULL;
        }
    }
    (rtd->unstring_count)++;

    if ( dlm ) {
        if ( dlm_data ) {
            if (rtd->unstring_national) {
                cob_memcpy_national(rtd, dlm, dlm_data, (int) dlm_size);
            } else 
                cob_memcpy (rtd, dlm, dlm_data, (int) dlm_size);
        } else if ( COB_FIELD_IS_NUMERIC (dlm) ) {
            cob_move (rtd, &cob_zero, dlm);
        } else {
            cob_move (rtd, &cob_space, dlm);
        }
    }

    if ( cnt ) {
        cob_set_int (rtd, cnt, match_size);
    }
}

void
cob_unstring_tallying (COB_RTD, cob_field *f)
{
    cob_add_int (rtd, f, (rtd->unstring_count));
}

void
cob_unstring_finish (COB_RTD)
{
    if ( (rtd->unstring_offset) < (int)(rtd->unstring_src)->size ) {
        cob_set_exception (rtd, COB_EC_OVERFLOW_UNSTRING);
    }

    if ( (rtd->unstring_ptr) ) {
        cob_set_int (rtd, (rtd->unstring_ptr), (rtd->unstring_offset) + 1);
    }
}

/* Initialization */

void
cob_init_strings (COB_RTD)
{
    (rtd->inspect_mark) = cob_malloc (rtd, COB_MEDIUM_BUFF);
    (rtd->inspect_lastsize) = COB_MEDIUM_BUFF;
    (rtd->alpha_attr).type = COB_TYPE_ALPHANUMERIC;
    (rtd->alpha_attr).digits = 0;
    (rtd->alpha_attr).scale = 0;
    (rtd->alpha_attr).flags = 0;
    (rtd->alpha_attr).pic = NULL;
    (rtd->alpha_fld).size = 0;
    (rtd->alpha_fld).data = NULL;
    (rtd->alpha_fld).attr = &(rtd->alpha_attr);
}

void
cob_clear_strings (COB_RTD)
{
    if ( rtd->figptr ) {
        cob_free (rtd->figptr);
    }
    if ( rtd->dlm_list ) {
        cob_free (rtd->dlm_list);
    }
    cob_free(rtd->inspect_mark);
    rtd->inspect_lastsize=0;
}


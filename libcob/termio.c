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

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#ifdef  HAVE_UNISTD_H
    #include <unistd.h>
#endif
#include <time.h>

#include "move.h"
#include "coblocal.h"
#include "termio.h"
#include "screenio.h"
#include "fileio.h"
#include "intrinsic.h"
#include "mf_extfh.h"
#include "a2e.h"
#ifdef _WIN32
    #include <windows.h>            /* for GetTempPath, GetTempFileName */
    #include <fcntl.h>
    #include <io.h>
#endif

static const int        bin_digits[] = { 1, 3, 5, 8, 10, 13, 15, 17, 20};

#ifdef CIT_EBCDIC_CHARSET 
#define LOC_E2A(a) (!rtd->ebcdic_charset || rtd->console_ebcdic ? (a) : E2A(a))
#else
#define LOC_E2A(a) (a)
#endif

static unsigned char * LOC_STRING_E2A(COB_RTD, unsigned char *str, int *size)
{
    int newsize = *size;
    if ( rtd->console_codepage_iconv_cd ) { 
        char * cp = (char*)cob_get_codepage(rtd);       
        if ( newsize *4 > rtd->console_buffer_size ) {
            cob_free(rtd->console_buffer);
            rtd->console_buffer = cob_malloc(rtd, newsize * 5);
            rtd->console_buffer_size = newsize * 5;
        }
        
        newsize = cob_enterprise_display_to_debugcp(rtd, (char*)str, rtd->console_buffer, 
                                                cp, rtd->console_codepage_iconv_cd,
                                                newsize, rtd->console_buffer_size);
        *size = newsize;
        return (unsigned char *)rtd->console_buffer;
    }
    if ( rtd->ebcdic_charset && !rtd->console_ebcdic ) {
        return STRING_E2A(str, newsize);
    }
    return str;
}

static unsigned char * LOC_STRING_E2A_DUP(COB_RTD, unsigned char *str, int *size)
{
    if ( rtd->console_codepage_iconv_cd ) {
        return LOC_STRING_E2A(rtd,str,size);
    }
    if ( rtd->ebcdic_charset && !rtd->console_ebcdic ) {
        return STRING_E2A_DUP(rtd, str, *size);
    }
    return str;
}

static unsigned char * LOC_STRING_A2E(COB_RTD, unsigned char *str, int size)
{
    if ( rtd->console_codepage_iconv_cd ) {        
        char * cp = (char*)cob_get_codepage(rtd);       
        if ( size *4 > rtd->console_buffer_size ) {
            cob_free(rtd->console_buffer);
            rtd->console_buffer = cob_malloc(rtd, size * 5);
            rtd->console_buffer_size = size * 5;
        }
        (void)cob_enterprise_debugcp_to_display(rtd, (char*)str, rtd->console_buffer, 
                                                cp, rtd->console_codepage_iconv_cd,
                                                size, rtd->console_buffer_size);
        return (unsigned char*)rtd->console_buffer;
    }
    if ( rtd->ebcdic_charset && !rtd->console_ebcdic ) {
        return STRING_A2E(str, size);
    }
    return str;
}


static RTL_info_type RTLTable[] = {
    { "1255"    , 224,  250},
    { "8859-8"  , 224,  250},
    { "862"     , 128,  154},
    { "unicode" , 1488, 1514},
    {0, 0, 0}
};
/*
 * DISPLAY
 */

static void
char_to_hex(unsigned char c, unsigned char *dst) {
    static const char hex_char[] = "0123456789ABCDEF";
    dst[0] = hex_char[c >> 4];
    dst[1] = hex_char[c & 0xf];
}

static void
add_to_display_buffer (COB_RTD, unsigned char *p, int size)
{
    if ( rtd->display_buff_offset + size +1 >= rtd->display_buff_size ) {
        rtd->display_buff_size = rtd->display_buff_size + size + 2048;
        rtd->display_buff = realloc(rtd->display_buff, rtd->display_buff_size);
    }
    memcpy(rtd->display_buff+rtd->display_buff_offset, p, size);
    rtd->display_buff_offset += size;
    rtd->display_buff[rtd->display_buff_offset] = 0;
}

static void
field_debug_data_realloc_buffer(unsigned char **buffer, int *size, int target_size) {
    if ( !*buffer ) {
        *buffer = calloc(1, target_size+1);
        *size = target_size;
        return;
    }
    if ( target_size < *size ) return;
    if ( !*size ) *size = 1;
    while ( target_size >= *size ) *size *= 2;
    *buffer = realloc(*buffer, *size);
}

static void
set_field_debug_data(COB_RTD, unsigned char *p_n, int size) {
    /*int i;*/
    if ( !p_n ) {
        p_n = (unsigned char*)"<NULL POINTER>";
    }

    field_debug_data_realloc_buffer(&rtd->field_textual_rep, &rtd->field_textual_rep_capacity, size+1);
    memcpy(rtd->field_textual_rep, p_n, size);
    rtd->field_textual_rep_size = size;
    /*
    field_debug_data_realloc_buffer(&rtd->field_hex_rep, &rtd->field_hex_rep_capacity, size*2+1);
    for ( i = 0; i<size; ++i ) {
        char_to_hex(p_n[i], rtd->field_hex_rep + (i*2));
    }
    rtd->field_hex_rep_size = size * 2; 
    */ 
}

static void
display_numeric (COB_RTD, cob_field *f, FILE *fp, int mode)
{
    int             digits;
    int             scale;
    int             size;
    cob_field_attr  attr;
    cob_field       temp;
    unsigned char   data[128];

    if ( f->size == 0 ) {
        return;
    }
    if (rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) {
        digits = COB_FIELD_DIGITS (f);
        scale = COB_FIELD_SCALE (f);
        size = digits;
        COB_ATTR_INIT (COB_TYPE_NUMERIC_DISPLAY, digits, scale, 0, NULL);
        temp.size = size;
        temp.data = data;
        temp.attr = &attr;
    } else {
        digits = COB_FIELD_DIGITS (f);
        scale = COB_FIELD_SCALE (f);
        size = digits + (COB_FIELD_HAVE_SIGN (f) ? 1 : 0);
        COB_ATTR_INIT (COB_TYPE_NUMERIC_DISPLAY, digits, scale, 0, NULL);
        temp.size = size;
        temp.data = data;
        temp.attr = &attr;
        if ( COB_FIELD_HAVE_SIGN (f) ) {
            attr.flags = COB_FLAG_HAVE_SIGN | COB_FLAG_SIGN_SEPARATE;
            if ( COB_FIELD_SIGN_LEADING (f)
                 || COB_FIELD_TYPE (f) == COB_TYPE_NUMERIC_BINARY ) {
                attr.flags |= COB_FLAG_SIGN_LEADING;
            }
        }
    }

    cob_move (rtd, f, &temp);
 
    mode = mode & DISPLAY_ONE_FIELD_MODE_MASK;
    if ( mode == DISPLAY_ONE_FIELD_MODE_FILE ) {
        if ( fp ) {
            unsigned char * p = LOC_STRING_E2A(rtd, data, &size);
            fwrite (p, size, 1, fp);
        }
    } else if ( mode == DISPLAY_ONE_FIELD_MODE_DEBUG ) {
        set_field_debug_data(rtd, STRING_E2A(data, size), size);
    } else if ( mode == DISPLAY_ONE_FIELD_MODE_BUFFER ) {
        unsigned char * p = LOC_STRING_E2A(rtd, data, &size);
        add_to_display_buffer(rtd, p, size);
    }
}

static void
pretty_display_numeric (COB_RTD, cob_field *f, FILE *fp, int mode)
{
    unsigned char   *p;
    int             i;
    int             digits;
    int             scale;
    int             size;
    cob_field_attr  attr;
    cob_field       temp;
    unsigned char   pic[64];
    unsigned char   data[256];

    if ( f->size == 0 ) {
        return;
    }
/* RXW
        if (COB_FIELD_TYPE(f) == COB_TYPE_NUMERIC_BINARY) {
                digits = bin_digits[f->size];
        } else {
*/
    digits = COB_FIELD_DIGITS (f);
/* RXW
        }
*/
    scale = COB_FIELD_SCALE (f);
    size = (digits + (COB_FIELD_HAVE_SIGN (f) ? 1 : 0)
            + (scale > 0 ? 1 : 0));
    p = pic;
    temp.size = size;
    temp.data = data;
    temp.attr = &attr;
    COB_ATTR_INIT (COB_TYPE_NUMERIC_EDITED, digits, scale, 0, pic);
    memset (pic, 0, sizeof (pic));
    memset (data, 0, sizeof (data));
    if ((mode & DISPLAY_ONE_FIELD_SOURCE_MASK) == DISPLAY_ONE_FIELD_SOURCE_XML) {
        if ( COB_FIELD_HAVE_SIGN (f) ) {
            *p++ = '-';
            i = 1;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
        }
        if (scale > 0) {
            if ( COB_FIELD_HAVE_SIGN (f) ) 
                *p++ = '-';
            else 
                *p++ = 'Z';
            i = (digits - scale) ;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
            *p++ = LOC_E2A(rtd->current_module->decimal_point);
            i = 1;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
            *p++ = '9';
            i = scale;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
        } else {
            if ( COB_FIELD_HAVE_SIGN (f) ) 
                *p++ = '-';
            else 
                *p++ = 'Z';
            i = (digits - scale) -1;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
            *p++ = '9';
            i =  1;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
        }
    } else {
        if ( COB_FIELD_HAVE_SIGN (f) ) {
            *p++ = '+';
            i = 1;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
        }
        if (scale > 0) {
            *p++ = '9';
            i = digits - scale;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
            *p++ = LOC_E2A(rtd->current_module->decimal_point);
            i = 1;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
            *p++ = '9';
            i = scale;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
        } else {
            *p++ = '9';
            i = digits;
            memcpy (p, (unsigned char *)&i, sizeof(int));
            p += sizeof(int);
        }
    }

    cob_move (rtd, f, &temp);

    mode = mode & DISPLAY_ONE_FIELD_MODE_MASK;
    if ( mode == DISPLAY_ONE_FIELD_MODE_FILE ) {
        if ( fp ) {
            unsigned char * p = LOC_STRING_E2A(rtd, data, &size);
            fwrite (p, size, 1, fp);
        }
    } else if ( mode == DISPLAY_ONE_FIELD_MODE_DEBUG ) {
        set_field_debug_data(rtd, STRING_E2A(data, size), size);
    } else if ( mode == DISPLAY_ONE_FIELD_MODE_BUFFER ) {
        unsigned char * p = LOC_STRING_E2A(rtd, data, &size);
        add_to_display_buffer(rtd, p, size);
    }
}

static int
is_same(unsigned char c, unsigned char const *data, int size )
{
    while (size--) {
        if (c != *data++) {
            return 0;
        }
    }
    return 1;
}

static COB_INLINE int 
is_low_high_value (cob_field *f)
{
    register unsigned char c = *(f->data + f->size - 1);

    return ((c == 0 || c== 0xff) && is_same(c, f->data, f->size ));
}

static void 
display_low_high (COB_RTD, cob_field *f, FILE *fp, int mode)
{
    unsigned char c = *f->data;
    mode = mode & DISPLAY_ONE_FIELD_MODE_MASK;
    if (c) {
        cob_one_field_display_prinf (rtd, fp, (char*)"<HIGH-VALUE>", mode);
    } else {
        cob_one_field_display_prinf (rtd, fp, (char*)"<LOW-VALUE>", mode);
    }
}

static void
display_alnum (COB_RTD, cob_field *f, FILE *fp, int mode)
{
    char * save_cp = NULL;

    mode = mode & DISPLAY_ONE_FIELD_MODE_MASK;
    if (is_low_high_value(f)) {
        display_low_high(rtd,f,fp,mode);
        return;
    }
    if ( COB_FIELD_IS_NATIONAL(f) ) {
        if ( mode == DISPLAY_ONE_FIELD_MODE_DEBUG ) {
            save_cp=rtd->display_code_page;
            if ( rtd->debug_codepage ) {
                rtd->display_code_page = rtd->debug_codepage;
            } else {
                rtd->display_code_page = (char*)"UTF-8";
            }
        }
        f = cob_intr_display_of(rtd,1,f);
        if ( mode == DISPLAY_ONE_FIELD_MODE_DEBUG ) {
            rtd->display_code_page= save_cp;
        }
    } else {
        
        if ( mode == DISPLAY_ONE_FIELD_MODE_DEBUG ) {
            f = cob_intr_display_to_debugcp_1(rtd,-1, 0, 1,f);
        } 
         
    }

    if ( mode == DISPLAY_ONE_FIELD_MODE_FILE ) {
        if ( fp ) {
            unsigned char *p;
            int size = f->size;
            p = cob_enterprise_RTLGetReversedString(rtd, f->data, f->size);
            p = LOC_STRING_E2A_DUP(rtd, p, &size);
            fwrite (p, size, 1, fp);
        }
    } else if ( mode == DISPLAY_ONE_FIELD_MODE_DEBUG ) {
        unsigned char *p ;
        p = cob_enterprise_RTLGetReversedString(rtd, f->data, f->size);
        set_field_debug_data(rtd,  p, f->size);

    } else if ( mode == DISPLAY_ONE_FIELD_MODE_BUFFER ) {        
        int size = f->size;
        unsigned char *p = LOC_STRING_E2A_DUP(rtd, f->data, &size);
        add_to_display_buffer(rtd, p, size);
    }

}

void
cob_one_field_display_prinf (COB_RTD, FILE *fp, char *s, int mode)
{
    mode = mode & DISPLAY_ONE_FIELD_MODE_MASK;
    if ( mode == DISPLAY_ONE_FIELD_MODE_FILE ) {
        if ( fp ) fprintf(fp, s);
    } else if ( mode == DISPLAY_ONE_FIELD_MODE_DEBUG ) {
        set_field_debug_data(rtd, (unsigned char *)s, strlen(s));
    } else if ( mode == DISPLAY_ONE_FIELD_MODE_BUFFER ) {
        add_to_display_buffer(rtd, (unsigned char *)s, strlen(s));
    }
}

void
cob_one_field_display (COB_RTD, cob_field *f, FILE *fp, int data_offset)  {
    cob_one_field_display_extended(rtd, f, fp, data_offset, fp ? DISPLAY_ONE_FIELD_MODE_FILE : DISPLAY_ONE_FIELD_MODE_BUFFER);
}

void
cob_one_field_display_extended (COB_RTD, cob_field *f, FILE *fp, int data_offset, int mode)
{
    unsigned char   *p;
    unsigned char   *save_data;
    int             n;
    cob_field       temp;
    cob_field_attr  attr;
    char            b[200];


    if ( f == NULL ) {
        cob_one_field_display_prinf (rtd, fp, (char*)"<INVALID FIELD>", mode);
        return;
    }
    if ( f->data == NULL ) {
        cob_one_field_display_prinf (rtd, fp, (char*)"<NULL POINTER>", mode);
        return;
    }
    if ( f->data == (void*)-1 ) {
        cob_one_field_display_prinf (rtd, fp, (char*)"<INVALID POINTER>", mode);
        return;
    }

    save_data= f->data;
    f->data += data_offset;

    if ( COB_FIELD_TYPE (f) == COB_TYPE_NUMERIC_DOUBLE ) {
        double f1doub;

        memcpy ((char *)&f1doub, f->data, sizeof (double));
        sprintf (b, "%-.18lf", f1doub);
        cob_one_field_display_prinf(rtd, fp, b, mode);
    } else if ( COB_FIELD_TYPE (f) == COB_TYPE_NUMERIC_FLOAT ) {
        float f1float;

        memcpy ((char *)&f1float, f->data, sizeof (float));
        sprintf (b, "%-.18lf", (double)f1float);
        cob_one_field_display_prinf(rtd, fp, b, mode);
    } else if ( COB_FIELD_IS_POINTER (f) ) {
        unsigned char *str = (unsigned char *)(&b[2]);
        sprintf(b, "0x");
#ifdef WORDS_BIGENDIAN
        p = f->data;
        for ( n = 0; n < sizeof(void *); ++n, ++p ) {
#else
        p = f->data + sizeof(void *) - 1;
        for ( n = sizeof(void *) - 1; n >= 0; --n, --p ) {
#endif
            char_to_hex(*p, str);
            str += 2;
        }
        *str=0; 
        cob_one_field_display_prinf(rtd, fp, b, mode);
    } else if ( COB_FIELD_IS_BITS (f) || COB_FIELD_IS_BITSARRAY(f) ) {
        unsigned char   data[64];
        COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC_BITS, f->attr->digits, 0, 0, NULL);
        temp.size = f->attr->digits;
        temp.data = data;
        temp.attr = &attr;
        cob_move(rtd,f,&temp);
        display_alnum (rtd, &temp, fp, mode);
    } else if ( COB_FIELD_REAL_BINARY(f) ||
                (COB_FIELD_TYPE(f) == COB_TYPE_NUMERIC_BINARY &&
                 !(rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) && 
                 !(rtd->current_module->flag_pretty_display & COB_NUMERIC_PRETTY_DISPLAY)) ) {
        attr = *f->attr;
        temp = *f;
        attr.digits = bin_digits[f->size];
        temp.attr = &attr;
        display_numeric (rtd, &temp, fp, mode);
    } else if ( COB_FIELD_IS_NUMERIC (f) ) {
        if ((COB_FIELD_TYPE(f) == COB_TYPE_NUMERIC_EDITED || 
             COB_FIELD_TYPE(f) == COB_TYPE_NUMERIC_DISPLAY ||
             (COB_FIELD_TYPE(f) == COB_TYPE_NUMERIC_PACKED && COB_FIELD_HAVE_SIGN (f))) && 
             is_low_high_value(f)) {
            display_low_high(rtd,f,fp,mode);
        } else if ( (rtd->current_module->flag_pretty_display & COB_NUMERIC_RAW_DISPLAY) &&
             (COB_FIELD_TYPE(f) == COB_TYPE_NUMERIC_DISPLAY) && 
             !COB_FIELD_HAVE_SIGN (f) && (COB_FIELD_SCALE (f)==0)) {
            display_alnum (rtd, f, fp, mode);
        }else if ( rtd->current_module->flag_pretty_display & COB_NUMERIC_PRETTY_DISPLAY) {
            pretty_display_numeric (rtd, f, fp, mode);
        } else {
            display_numeric (rtd, f, fp, mode);
        }
    } else {
        display_alnum (rtd, f, fp, mode);
    }
    f->data = save_data;
}

unsigned char * cob_get_field_display_image   (COB_RTD, cob_field *f)
{
    rtd->display_buff_offset = 0;
    cob_one_field_display(rtd,f,NULL,0);
    return(rtd->display_buff);
}

void
cob_display (COB_RTD, const int upon_device, const int neednewline, const int varcnt, ...)
{
    FILE            *fp;
    FILE            *RTLfile;
    cob_field       *f;
    cob_file_extfh        *file=NULL;
    int             i, size;
    int             offset = 0;
    va_list         args;
    int             do_close = 0;

    FLD_VAR

    fp = stderr;
    if ( !rtd->cob_screen_initialized ) {
        if ( upon_device ==  COB_DEVICE_SYSERR ) {
            if ( rtd->cob_err_file ) {
                fp = rtd->cob_err_file;
            }
        } else {
            fp = stdout;
        }
    } else {
        fp = NULL;
    }

    RTLfile = fp;
    if ( rtd->sysout_file && (upon_device == COB_DEVICE_SYSOUT) ) {
        RTLfile = NULL;
        file = rtd->sysout_file;
    } else if ( rtd->sysprint_file && (upon_device == COB_DEVICE_PRINTER) ) {
        RTLfile = NULL;
        file = rtd->sysprint_file;
        do_close = 1;
    } else if ( rtd->syserr_file && (upon_device == COB_DEVICE_SYSERR) ) {
        RTLfile = NULL;
        file = rtd->syserr_file;
    }
    if ( rtd->RTL_info ) {
        RTLfile = NULL;
    }

    va_start (args, varcnt);
    for ( i = 0; i < varcnt; ++i ) {
        f = va_arg (args, cob_field *);
        if ( COB_FIELD_IS_NATIONAL(f) ) {
            f = cob_intr_display_of(rtd,1,f);
        }
        cob_one_field_display (rtd, f, RTLfile, 0);
    }
    va_end (args);
    if ( !file && rtd->cob_screen_initialized ) {
        if ( rtd->display_buff_offset >0 ) {
            cob_screen_append(rtd,(char*)rtd->display_buff, rtd->display_buff_offset);
        }
        rtd->display_buff_offset = 0;
    }
    if ( file ) {
        cob_field rec;
        cob_field_attr attr;
        mf_extfh_FCD  *fcd = (mf_extfh_FCD *)(file->extfh_ptr);
        register int record_max     = GT_FLD2_4(fcd->max_rec_length);
        register int organization   = GT_FLD1(fcd->organization);
        register int record_min     = GT_FLD2_4(fcd->min_rec_length);
        register int open_mode      = GT_FLD1(fcd->open_mode);
        /*Attention: Do NOT confuse open mode of FCD and mode parameter of CIT*/
        if ( open_mode == FCD_OPEN_CLOSED ) {
            cob_open_extfh(rtd,file,COB_OPEN_EXTEND,0,NULL,0);
            if ( fcd->user_file_status[0] != CHAR_0 ) {
                cob_open_extfh(rtd,file,COB_OPEN_OUTPUT,0,NULL,0);
                if ( fcd->user_file_status[0] != CHAR_0 ) {
                    cob_runtime_error(rtd, "Unable to open SYS file : %s (%s).", file->select_name , cob_strerror(rtd,errno));
                }
            }
        }
        COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
        rec.attr = &attr;
        /*
        if ((file->organization == COB_ORG_LINE_SEQUENTIAL)) {
            rec.data = rtd->display_buff;
            rec.size = rtd->display_buff_offset;
            if ((file->record_max < rec.size+5)) {
                file->record_max = rec.size+5;
            } 
            file->record = &rec;
            cob_write(rtd,file,&rec,COB_WRITE_BEFORE | COB_WRITE_LINES | 1,NULL);
        } else {
            field_debug_data_realloc_buffer(&(rtd->display_buff), &(rtd->display_buff_size), file->record_max);
            rec.data = rtd->display_buff;
            rec.size = rtd->display_buff_offset;
            if (rec.size > file->record_max) {
                rec.size = file->record_max;
            }
            file->record = &rec;
            cob_write(rtd,file,&rec,0,NULL);
        } 
        */
        offset = 0;
        rec.data = cob_malloc(rtd, record_max);
        while ( offset < rtd->display_buff_offset ) {
            size = (rtd->display_buff_offset - offset) ;
            if ( (size > record_max) && (record_max !=0) ) {
                size = record_max ;
            }
            if ( (organization == FCD_ORG_LINE_SEQUENTIAL) ) {
                memset (rec.data, ' ', record_max);
            } else {
                memset (rec.data, 0, record_max);
            }
            memcpy (rec.data, rtd->display_buff + offset, size);
            rec.size = size;
            if ( rec.size < record_min ) {
                rec.size = record_min;
            }
            /*
            if ((file->organization == COB_ORG_LINE_SEQUENTIAL)) {
                cob_write(rtd,file,&rec,COB_WRITE_BEFORE | COB_WRITE_LINES | 1,NULL);
            } else {
                cob_write(rtd,file,&rec,0,NULL);
            } 
            */
            CP_FLDP(fcd->rec_data_ptr, rec.data);
            CP_FLD2_4(fcd->cur_rec_length, rec.size);
            if ( neednewline ) {
                cob_write_extfh(rtd,file,&rec,0,NULL);
            } else {
                cob_write_extfh(rtd,file,&rec,COB_WRITE_BEFORE | COB_WRITE_LINES | 0,NULL);
            }
            offset += size;
        }
        cob_free(rec.data);
        CP_FLD2_4(fcd->cur_rec_length, 0);
        CP_FLDP(fcd->rec_data_ptr, NULL);
        rtd->display_buff_offset = 0;
        if (do_close) {
            cob_close_extfh(rtd,file, 0, 0);
        }
    } else {
        if ( neednewline ) {
            if ( fp ) {

                if (rtd->current_module && rtd->current_module->display_dos) {
                    cob_one_field_display_prinf(rtd, RTLfile, (char *)"\r\n", RTLfile ? DISPLAY_ONE_FIELD_MODE_FILE : DISPLAY_ONE_FIELD_MODE_BUFFER);
                } else {
                    cob_one_field_display_prinf(rtd, RTLfile, (char *)"\n", RTLfile ? DISPLAY_ONE_FIELD_MODE_FILE : DISPLAY_ONE_FIELD_MODE_BUFFER);
                }
                if ( rtd->RTL_info ) {
                    int size = rtd->display_buff_offset;
                    unsigned char * p = LOC_STRING_E2A(rtd, cob_enterprise_RTLGetReversedString(rtd,rtd->display_buff, size), &size);
                    fwrite (p , size, 1, fp);        
                    rtd->display_buff_offset = 0;
                }
                fflush (fp);                
            } else if ( rtd->cob_screen_initialized ) {
                cob_screen_newline(rtd);
            }
        }
    }
}

/*
 * ACCEPT
 */

void
cob_accept (COB_RTD, const int upon_device, cob_field *f)
{
/* RXW
        size_t          size;
*/
    cob_field_attr  attr;
    cob_field       temp;
    cob_field_attr  attr2;
    cob_field       temp2;
    int             readlen = 0;

    FLD_VAR

    if ( (rtd->cob_screen_initialized) ) {
        cob_field_accept (rtd, f, NULL, NULL, NULL, NULL, NULL, 0, 0);
        return;
    }
    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 8, 0, 0, NULL);
    attr2 = attr;
    temp2.data = (void*)(&readlen);
    temp2.attr = &attr2;
    temp2.size = sizeof(readlen);

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    temp.data = rtd->term_buff;
    temp.size = rtd->term_buff_size;
    temp.attr = &attr;
    /* read a line */
    if ( rtd->sysin_file && (upon_device != COB_DEVICE_CONSOLE) ) {
        cob_file_extfh *file = rtd->sysin_file;
        mf_extfh_FCD  *fcd = (mf_extfh_FCD *)(file->extfh_ptr);
        register int open_mode = GT_FLD1(fcd->open_mode);
        if ( open_mode == FCD_OPEN_CLOSED ) {
            cob_open_extfh(rtd,file,COB_OPEN_INPUT,0,NULL,0);
            if ( fcd->user_file_status[0] != CHAR_0 ) {
                cob_runtime_error(rtd, "Unable to open SYS file : %s (%s).", file->select_name , cob_strerror(rtd,errno));
            }
        }
        CP_FLD2_4(fcd->max_rec_length, rtd->term_buff_size);
        CP_FLDP(fcd->rec_data_ptr, temp.data);
        CP_FLD2_4(fcd->cur_rec_length, temp.size);
        file->record_size = &temp2;
        cob_read_extfh(rtd,file, NULL, NULL, COB_READ_NEXT);
        temp.size = cob_get_int(rtd,&temp2);
        CP_FLD2_4(fcd->cur_rec_length, 0);
        CP_FLDP(fcd->rec_data_ptr, NULL);
        file->record_size = NULL;
    } else {
        if ( fgets ((char *)(rtd->term_buff), rtd->term_buff_size, stdin) == NULL ) {
            temp.size = 1;
            (rtd->term_buff)[0] = CHAR_SP;
            (rtd->term_buff)[1] = 0;
        } else {
            temp.size = strlen ((char *)(rtd->term_buff));
            while ( temp.size && 
                    ((rtd->term_buff[temp.size-1] == 0x0A) || (rtd->term_buff[temp.size-1] == 0x0D)) ) {
                temp.size --;
            }
            (void)LOC_STRING_A2E(rtd, rtd->term_buff, temp.size);
        }
    }
    if ( COB_FIELD_TYPE(f) == COB_TYPE_NUMERIC_DISPLAY ) {
        int maxsize = f->size;
        if (f->attr->scale) {
            maxsize++;
        } 

        if (temp.size > maxsize) {
            temp.size = maxsize;
        }

    }

    cob_move (rtd, &temp, f);
/* RXW
        if (isatty (fileno (stdin))) {
                temp.size = strlen ((char *)(rtd->term_buff)) - 1;
                cob_move (&temp, f);
        } else {
                size = strlen ((char *)(rtd->term_buff)) - 1;
                if (size > f->size) {
                        size = f->size;
                }
                memcpy (f->data, (rtd->term_buff), size);
                memset (f->data + size, CHAR_SP, f->size - size);
        }
*/
}
static void cob_file_initialization(COB_RTD, char * filename){
    cob_file_extfh *file_printer;
    mf_extfh_FCD *fcd;
    int size;
    FLD_VAR

    file_printer = cob_malloc(rtd, sizeof(cob_file_extfh));
    file_printer->extfh_ptr = &(file_printer->FCD);
    fcd = (mf_extfh_FCD*)(file_printer->extfh_ptr);

    size = strlen(filename);
    CP_FLDP(fcd->fname_ptr         , filename);
    (void) STRING_E2A(fcd->fname_ptr, size);
    CP_FLD2(fcd->fname_length      , size);    


    file_printer->select_name = (const char *)"file_SYSPRINT";



    CP_FLD2_4(fcd->cur_rec_length, 0);
    CP_FLDP(fcd->rec_data_ptr, NULL);
    CP_FLD2_4(fcd->max_rec_length, 132);
    CP_FLD2_4(fcd->min_rec_length, 0);

    file_printer->record_size = NULL;

    CP_FLDP(fcd->key_def_block_ptr, 0);

    file_printer->file = NULL;
    CP_FLD1(fcd->organization, FCD_ORG_LINE_SEQUENTIAL);
    CP_FLD1(fcd->access_mode, FCD_ACCESS_SEQUENTIAL);



    CP_FLD1(fcd->open_mode, FCD_OPEN_CLOSED);
    file_printer->flag_optional = 0;
    file_printer->last_open_mode = 0;



    file_printer->flag_needs_top = 0;
#ifdef COB_PTR_64BITS
    CP_FLD1(fcd->version           , 1);
#else
    CP_FLD1(fcd->version           , 0);
#endif


    file_printer->auto_close = 1;

    SETB_ON_FLD1(fcd->FCD_OTHER_FLAGS_BIT_LineAdv);
    CP_FLD1(fcd->recording_mode, 1);


    cob_redirect_sysfile(rtd, file_printer, 3);
}
void
cob_init_termio (COB_RTD)
{
    char *s= NULL;
    RTL_info_type *p;


    rtd->term_buff = cob_malloc (rtd, COB_MEDIUM_BUFF);
    rtd->term_buff_size = COB_MEDIUM_BUFF;
    rtd->display_buff = cob_malloc (rtd, COB_MEDIUM_BUFF);
    rtd->display_buff_size = COB_MEDIUM_BUFF;
    rtd->display_buff_offset = 0;
    rtd->console_buffer = cob_malloc (rtd, COB_SMALL_BUFF);
    rtd->console_buffer_size = COB_SMALL_BUFF;

    rtd->field_textual_rep = 0;
    rtd->field_textual_rep_capacity = 0;
    rtd->field_textual_rep_size = 0;
    /*rtd->field_hex_rep = 0;
    rtd->field_hex_rep_capacity = 0;
    rtd->field_hex_rep_size = 0;
    */

    s = getenv ("COB_CONSOLE_CP");
    /*
    if ( !s ) {
#if defined(_WIN32) 
        int cp = GetConsoleOutputCP();
        if ( cp == 0 ) {
            cp = 1251;
        }
        sprintf (rtd->term_buff, "%d", cp);
        s = strdup(rtd->term_buff);
#else
        s = "UTF-8";
#endif
    } 
    */ 
#ifdef WITH_ICU
    if ( s && (strlen (s)>0)) {
        rtd->console_codepage = s;
        rtd->console_codepage_iconv_cd=cob_enterprise_open_one_icu(rtd,rtd->console_codepage);
    }
#endif     
     
#if defined(_WIN32) 
    if ( s ) {
        UINT cp = atoi(s);
        if ( cp ) {
            SetConsoleOutputCP(cp);
            SetConsoleCP(cp);
        }
    } 
#endif
    if ( (s = getenv ("COB_RTL_CP")) != NULL ) {
        p = RTLTable;
        while ( p && p->cp ) {
            if ( strcmp(s, p->cp) == 0 ) {
                rtd->RTL_info = p;
                break;
            }
            p++;
        }
    }
    if ( (s = cob_bool_getenv ("COB_CONSOLE_EBCDIC")) != NULL ) {
        if (*s == '1' || *s == 'Y' || *s =='y') {
            rtd->console_ebcdic=1;
        }
    }
    if ( (s = getenv ("COB_DEBUG_CP")) != NULL ) {
        rtd->debug_codepage=s;
    } else  {
        rtd->debug_codepage=(char*)"UTF-8";
    }
    s = getenv ("COB_DISPLAY_PRINTER");
    if( s ) {
        cob_file_initialization(rtd, s);
    }
#ifdef _WIN32
    rtd->win_console = GetStdHandle(STD_OUTPUT_HANDLE);

    if ( GetFileType(rtd->win_console) != FILE_TYPE_CHAR ) {
        rtd->win_console = NULL;
    }
#endif

}

void
cob_clear_termio (COB_RTD)
{
    cob_free (rtd->term_buff);
    cob_free (rtd->display_buff);
    cob_free (rtd->console_buffer);
    rtd->term_buff_size = 0;
    rtd->display_buff_size = 0;
    rtd->console_buffer_size = 0;

    cob_free(rtd->field_textual_rep);
    rtd->field_textual_rep = 0;
    rtd->field_textual_rep_capacity = 0;
    rtd->field_textual_rep_size = 0;
    rtd->console_codepage_iconv_cd=cob_enterprise_close_one_icu(rtd->console_codepage_iconv_cd);
    /* 
    cob_free(rtd->field_hex_rep);
    rtd->field_hex_rep = 0;
    rtd->field_hex_rep_capacity = 0;
    rtd->field_hex_rep_size = 0;
    */
}

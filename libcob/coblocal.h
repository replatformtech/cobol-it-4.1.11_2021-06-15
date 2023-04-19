/*
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

#ifndef COB_LOCAL_H
#define COB_LOCAL_H

/* We use this file to prototype things that should not be
   exported to user space
*/
#define _GNU_SOURCE / * glibc2 needs this */
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>

#if defined(__GNUC__) && defined(linux) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#define COB_HIDDEN      __attribute__ ((visibility("hidden")))
#else
#define COB_HIDDEN
#endif

#ifdef  COB_PARAM_CHECK
#define COB_CHK_PARMS(x, z)     \
        do { \
                if (rtd->cob_call_params < z) { \
                        cob_runtime_error (rtd, parm_msg, #x, z); \
                        cob_stop_abend (rtd, COBRE_PARAM_CHECK); \
                } \
        } while (0)
#else
#define COB_CHK_PARMS(x, z)
#endif
COB_HIDDEN extern void      cob_memcpy              (COB_RTD, cob_field *,
                                                     unsigned char *,
                                                     const int);
COB_HIDDEN extern void      cob_memcpy_string       (COB_RTD, cob_field *,
                                                     unsigned char *,
                                                     const int);
COB_HIDDEN extern void      cob_memcpy_national     (COB_RTD, cob_field *dst, unsigned char *src, const int size) ;
COB_HIDDEN extern void      cob_exit_fileio         (COB_RTD, int panic);
COB_HIDDEN extern void      cob_init_numeric        (COB_RTD);
COB_HIDDEN extern void      cob_clear_numeric       (COB_RTD);
COB_HIDDEN extern void      cob_init_termio         (COB_RTD);
COB_HIDDEN extern void      cob_clear_termio        (COB_RTD);
COB_HIDDEN extern void      cob_init_fileio         (COB_RTD);
COB_HIDDEN extern void      cob_clear_fileio        (COB_RTD);
COB_HIDDEN extern void      cob_init_call           (COB_RTD);
COB_HIDDEN extern void      cob_clear_call          (COB_RTD);
COB_HIDDEN extern void      cob_init_intrinsic      (COB_RTD);
COB_HIDDEN extern void      cob_clear_intrinsic     (COB_RTD);
COB_HIDDEN extern void      cob_init_strings        (COB_RTD);
COB_HIDDEN extern void      cob_clear_strings       (COB_RTD);
COB_HIDDEN extern void      cob_init_move           (COB_RTD);
COB_HIDDEN extern void      cob_move_init_charset_const (COB_RTD);
COB_HIDDEN extern void      cob_clear_move           (COB_RTD);
COB_HIDDEN extern void      cob_screen_terminate    (COB_RTD);
COB_HIDDEN extern void      cob_screen_set_mode     (COB_RTD, size_t);
COB_HIDDEN extern int       cob_real_get_sign       (COB_RTD, cob_field *);
COB_HIDDEN extern void      cob_real_put_sign       (COB_RTD, cob_field *, const int);
COB_HIDDEN extern int       cob_display_get_sign    (COB_RTD, cob_field *f, unsigned char **start, int *size) ;
COB_HIDDEN extern char *    cob_str_strip           (char*p, char * buffer);
COB_HIDDEN extern unsigned long  cob_clock(cob_paragraph_profiling_times *time);
COB_HIDDEN extern const char * cob_utf16_cp_id      (COB_RTD);
COB_HIDDEN extern struct tm  *cob_localtime (time_t  *t);
COB_HIDDEN extern void debug_callback_fct (COB_RTD, int code) ;
COB_HIDDEN extern void cob_screen_newline (COB_RTD);
COB_HIDDEN extern void cob_screen_append (COB_RTD, char * str, int size);
COB_HIDDEN extern int cob_packed_get_sign (const cob_field *f);
COB_HIDDEN extern int cob_iszero_char  (COB_RTD, cob_field *f, unsigned char c, int with_sp0);
COB_HIDDEN extern int cob_isdigit_char (COB_RTD, unsigned char c);
COB_HIDDEN extern int cob_isspace_char (COB_RTD, unsigned char c);
COB_HIDDEN extern int cob_min_int (const int x, const int y);
COB_HIDDEN extern int cob_max_int (const int x, const int y);
COB_HIDDEN extern int cob_stat (const char *name, struct stat     *st);
COB_HIDDEN extern int u32toa_branchlut(unsigned int value, char* buffer);
COB_HIDDEN extern int u64toa_branchlut(unsigned long long value, char* buffer);



#include "stringutils.h"

#ifdef HAVE_MEMMOVE
#define COB_MEMCPY memmove
#else
#define COB_MEMCPY memcpy
#endif
#ifdef	_MSC_VER
    #define PATHSEPC ';'
    #define PATHSEPS ";"
    #define DIRSEPC  '\\'
    #define DIRSEPS  "\\"
#else
    #define PATHSEPC ':'
    #define PATHSEPS ":"
    #define DIRSEPC  '/'
    #define DIRSEPS  "/"
#endif

#endif /* COB_LOCAL_H */

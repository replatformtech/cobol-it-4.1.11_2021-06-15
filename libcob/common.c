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
#include "defaults.h"

#define _GNU_SOURCE / * glibc2 needs this */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#ifdef	HAVE_UNISTD_H
    #include <unistd.h>
#endif
#include <time.h>
#ifdef HAVE_SYS_TIME_H
    #include <sys/time.h>
#endif
#ifdef	_WIN32
    #define WINDOWS_LEAN_AND_MEAN
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
    #include <sys/types.h> 
    #include <sys/timeb.h>
    #undef	HAVE_SIGNAL_H
    #define		SIGQUIT SIGABRT
#endif
#ifdef HAVE_PTHREAD_H
    #include <pthread.h>
#endif

#ifdef	HAVE_SIGNAL_H
    #include <signal.h>
#endif

#ifdef	HAVE_LOCALE_H
    #include <locale.h>
#endif

#include "common.h"
#include "coblocal.h"
#include "a2e.h"
#ifndef WORDS_BIGENDIAN
    #include "byteswap.h"
#endif
#include "debug.h"
#include "move.h"
#include "numeric.h"
#include "termio.h"
#include "fileio.h"
#include "call.h"
#include "screenio.h"
#include "intrinsic.h"
#include "lib/gettext.h"
#ifdef __GNUC__
    #include "malloc.h"
#endif
#include "citkey.h"
#include "cobsetjmp.h"

#include "mf_extfh.h"
#define COB_ERRBUF_SIZE 2048

/* Local variables */

#ifdef	HAVE_SIGNAL_H
typedef void (*cob_sighandler_t) (int);
static cob_sighandler_t         hupsig = NULL;
static cob_sighandler_t         intsig = NULL;
static cob_sighandler_t         qutsig = NULL;
static cob_sighandler_t         abrtsig = NULL;
static cob_sighandler_t         vecsig = NULL;
static cob_sighandler_t         bussig = NULL;
static cob_sighandler_t         fpesig = NULL;
#endif

/*CIT*/
void  (*cob_debug_callback)(COB_RTD, int action_code) = NULL;

#ifdef	COB_PARAM_CHECK
static const char   parm_msg[] = "CALL to %s requires %d parameters";
#endif

#undef	COB_EXCEPTION
#define COB_EXCEPTION(code,tag,name,critical)	name,
static const char       * const cob_exception_tab_name[] = {
    NULL,       /* COB_EC_ZERO */
#include "exception.def"
    NULL        /* COB_EC_MAX */
};

#undef	COB_EXCEPTION
#define COB_EXCEPTION(code,tag,name,critical)	0x##code,
static const int        cob_exception_tab_code[] = {
    0,      /* COB_EC_ZERO */
#include "exception.def"
    0       /* COB_EC_MAX */
};

#undef	COB_EXCEPTION

#define EXCEPTION_TAB_SIZE	sizeof(cob_exception_tab_code) / sizeof(int)

static void cob_init_rtd_charset (COB_RTD, int ebcdic_charset);
static cob_field_attr   all_attr = { COB_TYPE_ALPHANUMERIC_ALL, 0, 0, 0, NULL};
static cob_field_attr   one_attr = { COB_TYPE_NUMERIC, 1, 0, 0, NULL};
static char str_zero[] = {ASCII_CHAR_0, 0};
static char str_space[] = {ASCII_CHAR_SP, 0};
static char str_quote[] = {ASCII_CHAR_DQUOTE, 0};
static char str_one[] = {ASCII_CHAR_1, 0};
static int cob_enterprise_mode = 0;
/* Global variables */

cob_field       cob_zero = { 1, (ucharptr)str_zero, &all_attr};
cob_field       cob_space = { 1, (ucharptr)str_space, &all_attr};
cob_field       cob_high = { 1, (ucharptr)"\xff", &all_attr};
cob_field       cob_low = { 1, (ucharptr)"\0", &all_attr};
cob_field       cob_quote = { 1, (ucharptr)str_quote, &all_attr};
cob_field       cob_one = { 1, (ucharptr)str_one, &one_attr};

/* Local functions */
static int inTerminate = 0;

#ifdef	HAVE_SIGNAL_H
static void COB_NOINLINE
cob_sig_handler (int sig) {
    if ( ! inTerminate ) {
        COB_RTD = cob_get_rtd();
        inTerminate = 1;
        if ( rtd ) {
            if ( sig == SIGSEGV ) {
                if ( rtd->cob_err_file ) {
                    if ( rtd->current_module && rtd->current_module->cur_source_file ) {
                        fprintf (rtd->cob_err_file, "%s:%d: ", 
                                 rtd->current_module->cur_source_file, rtd->current_module->cur_line);
                    }
                    fprintf (rtd->cob_err_file, "Attempt to reference unallocated memory (Signal SIGSEGV)\n");
                    fprintf (rtd->cob_err_file, "Abnormal termination - File contents may be incorrect\n");
                    fflush (rtd->cob_err_file);
                }

#ifdef DEBUG
                if (!cob_bool_getenv("COB_NO_SIGTRAP")) {
                    fprintf(rtd->cob_err_file,  "\n\n");
                    fprintf(rtd->cob_err_file,  "Cobol crash .. Debugmode core dump\n");
                    raise(SIGTRAP);
                }
#endif
            }
            if (cob_debug_callback) cob_debug_callback(rtd,COB_DBCALLBACK_CPU_EXCEPTION);
            if ( rtd->cob_initialized ) {
                if ( rtd->cob_err_file ) {
                    fprintf (rtd->cob_err_file, "Abnormal termination - File contents may not be correct\n");
                    fflush (rtd->cob_err_file);
                }
                cob_dump_debug_info(rtd);
                cob_screen_terminate (rtd);
                cob_exit_fileio (rtd, 1);
                cob_runtime_debugger_cleanup(rtd);
            }
        }
        switch ( sig ) {
#ifdef SIGHUP
            case SIGHUP:
                if ( (hupsig != SIG_IGN) && (hupsig != SIG_DFL) ) {
                    (*hupsig) (SIGHUP);
                }
                break;
#endif
            case SIGINT:
                if ( (intsig != SIG_IGN) && (intsig != SIG_DFL) ) {
                    (*intsig) (SIGINT);
                }
                break;
#ifdef SIGQUIT
            case SIGQUIT:
                if ( (qutsig != SIG_IGN) && (qutsig != SIG_DFL) ) {
                    (*qutsig) (SIGQUIT);
                }
                break;
#endif
#ifdef SIGABRT
            case SIGABRT:
                if ( (abrtsig != SIG_IGN) && (abrtsig != SIG_DFL) ) {
                    (*abrtsig) (SIGQUIT);
                } 

                break;
#endif
            case SIGFPE:
                if ( (fpesig != SIG_IGN) && (fpesig != SIG_DFL) ) {
                    (*fpesig) (SIGFPE);
                }
                break;
            case SIGSEGV:
                if ( (vecsig != SIG_IGN) && (vecsig != SIG_DFL) ) {
                    (*vecsig)(SIGSEGV);
                }
                break;
#ifdef SIGBUS
            case SIGBUS:
                if ( (bussig != SIG_IGN) && (bussig != SIG_DFL) ) {
                    (*bussig)(SIGBUS);
                }
                break;
#endif
        }
    }
    exit (sig);
}
#endif

static void
cob_set_signal (void) {
#ifdef	HAVE_SIGNAL_H
    if ( (intsig = signal (SIGINT, cob_sig_handler)) == SIG_IGN ) {
        (void)signal (SIGINT, SIG_IGN);
    }
#ifdef SIGPIPE
    if ( (hupsig = signal (SIGPIPE, cob_sig_handler)) == SIG_IGN ) {
        (void)signal (SIGPIPE, SIG_IGN);
    }
#endif
#ifdef SIGHUP
    if ( (hupsig = signal (SIGHUP, cob_sig_handler)) == SIG_IGN ) {
        (void)signal (SIGHUP, SIG_IGN);
    }
#endif
#ifdef SIGQUIT
    if ( (qutsig = signal (SIGQUIT, cob_sig_handler)) == SIG_IGN ) {
        (void)signal (SIGQUIT, SIG_IGN);
    }
#endif
#ifdef SIGABRT
    if ( (abrtsig = signal (SIGABRT, cob_sig_handler)) == SIG_IGN ) {
        (void)signal (SIGABRT, SIG_IGN);
    }
#endif
    if ( (fpesig = signal (SIGFPE, cob_sig_handler)) == SIG_IGN ) {
        (void)signal (SIGFPE, SIG_IGN);
    }
#ifdef SIGUSR1
    (void)signal (SIGUSR1, SIG_IGN);
#endif
    /* Take direct control of segementation violation */
    vecsig = signal (SIGSEGV, cob_sig_handler);
#ifdef SIGBUS
    vecsig = signal (SIGBUS, cob_sig_handler);
#endif
#endif
}

void
cob_add_to_datalist (COB_RTD, void *f, struct data_list **cachehead)
{
    struct data_list    *l;

    for ( l = *cachehead; l; l = l->next ) {
        if ( (void*)f == l->data ) {
            return;
        }
    }
    l = ( struct data_list *) cob_malloc (rtd, sizeof (struct data_list));
    l->data = (void*)f;
    l->next = *cachehead;
    *cachehead = l;
}

void
cob_remove_from_datalist (COB_RTD, void *f, struct data_list **cachehead)
{
    struct data_list    *l;
    struct data_list    *p=NULL;

    for ( l = *cachehead; l; l = l->next ) {
        if ( (void*)f == l->data ) {
            break;
        }
        p = l;
    }
    if (l) {
        if (p) {
            p->next = l->next;
        } else {
            *cachehead = l->next;
        }
        cob_free (l);
    }
}

void
cob_remove_all_from_datalist (COB_RTD, struct data_list **cachehead)
{
    struct data_list    *l;
    struct data_list    *p=NULL;

    for ( l = *cachehead; l;  ) {
        p = l;
        l = l->next;
        cob_free (p);
    }
    *cachehead = NULL;
}


static int
cob_test_sign_ascii (COB_RTD, unsigned char *p) {
    int c = *p;
    switch ( SWITCH_CHAR(c) ) {
        case CASE_CHAR(CHAR_p):
        case CASE_CHAR(CHAR_q):
        case CASE_CHAR(CHAR_r):
        case CASE_CHAR(CHAR_s):
        case CASE_CHAR(CHAR_t):
        case CASE_CHAR(CHAR_u):
        case CASE_CHAR(CHAR_v):
        case CASE_CHAR(CHAR_w):
        case CASE_CHAR(CHAR_x):
        case CASE_CHAR(CHAR_y):
            return(1);
        default:
            return(0);
    }
}

static inline int 
cob_get_sign_unchanged_ (COB_RTD, unsigned char *p) {
    int c = *p;
    switch ( SWITCH_CHAR(c) ) {
        /* EBCDIC coding*/
        case CASE_CHAR(CHAR_AT): /*'@'*/
        case CASE_CHAR(CHAR_CLOSEAC): /*'}'*/
        case CASE_CHAR(CHAR_J):
        case CASE_CHAR(CHAR_K):
        case CASE_CHAR(CHAR_L):
        case CASE_CHAR(CHAR_M):
        case CASE_CHAR(CHAR_N):
        case CASE_CHAR(CHAR_O):
        case CASE_CHAR(CHAR_P):
        case CASE_CHAR(CHAR_Q):
        case CASE_CHAR(CHAR_R):
            /*return(-1); */

            /*ASCII Coding*/
        case CASE_CHAR(CHAR_p):
        case CASE_CHAR(CHAR_q):
        case CASE_CHAR(CHAR_r):
        case CASE_CHAR(CHAR_s):
        case CASE_CHAR(CHAR_t):
        case CASE_CHAR(CHAR_u):
        case CASE_CHAR(CHAR_v):
        case CASE_CHAR(CHAR_w):
        case CASE_CHAR(CHAR_x):
        case CASE_CHAR(CHAR_y):
            /*CA Realia Coding*/
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
            return(-1);
            /* EBCDIC coding*/
            /*
            case CASE_CHAR(CHAR_OPENAC): / *'{'* /
            case CASE_CHAR(CHAR_A):
            case CASE_CHAR(CHAR_B):
            case CASE_CHAR(CHAR_C):
            case CASE_CHAR(CHAR_D):
            case CASE_CHAR(CHAR_E):
            case CASE_CHAR(CHAR_F):
            case CASE_CHAR(CHAR_G):
            case CASE_CHAR(CHAR_H):
            case CASE_CHAR(CHAR_I):
                return(1);
                / *ASCII Coding* /
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
                return(1);
            */
        default:
            /* What to do here */
            return(1);
    }
/* NOT REACHED */
    return(1);
}

static inline int 
cob_get_sign_all_ (COB_RTD, unsigned char *p) {
    int c = *p;
    switch ( SWITCH_CHAR(c) ) {
        /* EBCDIC coding*/
        case CASE_CHAR(CHAR_OPENAC): /*'{'*/
            *p = (unsigned char)CHAR_0;
            return(1);
        case CASE_CHAR(CHAR_A):
            *p = (unsigned char)CHAR_1;
            return(1);
        case CASE_CHAR(CHAR_B):
            *p = (unsigned char)CHAR_2;
            return(1);
        case CASE_CHAR(CHAR_C):
            *p = (unsigned char)CHAR_3;
            return(1);
        case CASE_CHAR(CHAR_D):
            *p = (unsigned char)CHAR_4;
            return(1);
        case CASE_CHAR(CHAR_E):
            *p = (unsigned char)CHAR_5;
            return(1);
        case CASE_CHAR(CHAR_F):
            *p = (unsigned char)CHAR_6;
            return(1);
        case CASE_CHAR(CHAR_G):
            *p = (unsigned char)CHAR_7;
            return(1);
        case CASE_CHAR(CHAR_H):
            *p = (unsigned char)CHAR_8;
            return(1);
        case CASE_CHAR(CHAR_I):
            *p = (unsigned char)CHAR_9;
            return(1);
        case CASE_CHAR(CHAR_AT): /*'@'*/
            *p = (unsigned char)CHAR_0;
            return(-1);
        case CASE_CHAR(CHAR_CLOSEAC): /*'}'*/
            *p = (unsigned char)CHAR_0;
            return(-1);
        case CASE_CHAR(CHAR_J):
            *p = (unsigned char)CHAR_1;
            return(-1);
        case CASE_CHAR(CHAR_K):
            *p = (unsigned char)CHAR_2;
            return(-1);
        case CASE_CHAR(CHAR_L):
            *p = (unsigned char)CHAR_3;
            return(-1);
        case CASE_CHAR(CHAR_M):
            *p = (unsigned char)CHAR_4;
            return(-1);
        case CASE_CHAR(CHAR_N):
            *p = (unsigned char)CHAR_5;
            return(-1);
        case CASE_CHAR(CHAR_O):
            *p = (unsigned char)CHAR_6;
            return(-1);
        case CASE_CHAR(CHAR_P):
            *p = (unsigned char)CHAR_7;
            return(-1);
        case CASE_CHAR(CHAR_Q):
            *p = (unsigned char)CHAR_8;
            return(-1);
        case CASE_CHAR(CHAR_R):
            *p = (unsigned char)CHAR_9;
            return(-1);

            /*ASCII Coding*/
        case CASE_CHAR(CHAR_p):
            *p = (unsigned char)CHAR_0;
            return(-1);
        case CASE_CHAR(CHAR_q):
            *p = (unsigned char)CHAR_1;
            return(-1);
        case CASE_CHAR(CHAR_r):
            *p = (unsigned char)CHAR_2;
            return(-1);
        case CASE_CHAR(CHAR_s):
            *p = (unsigned char)CHAR_3;
            return(-1);
        case CASE_CHAR(CHAR_t):
            *p = (unsigned char)CHAR_4;
            return(-1);
        case CASE_CHAR(CHAR_u):
            *p = (unsigned char)CHAR_5;
            return(-1);
        case CASE_CHAR(CHAR_v):
            *p = (unsigned char)CHAR_6;
            return(-1);
        case CASE_CHAR(CHAR_w):
            *p = (unsigned char)CHAR_7;
            return(-1);
        case CASE_CHAR(CHAR_x):
            *p = (unsigned char)CHAR_8;
            return(-1);
        case CASE_CHAR(CHAR_y):
            *p = (unsigned char)CHAR_9;
            return(-1);
        case CASE_CHAR(CHAR_0):
            return(1);
        case CASE_CHAR(CHAR_1):
            return(1);
        case CASE_CHAR(CHAR_2):
            return(1);
        case CASE_CHAR(CHAR_3):
            return(1);
        case CASE_CHAR(CHAR_4):
            return(1);
        case CASE_CHAR(CHAR_5):
            return(1);
        case CASE_CHAR(CHAR_6):
            return(1);
        case CASE_CHAR(CHAR_7):
            return(1);
        case CASE_CHAR(CHAR_8):
            return(1);
        case CASE_CHAR(CHAR_9):
            return(1);

            /*CA Realia Coding*/
        case 0x20:
            *p = (unsigned char)CHAR_0;
            return(-1);
        case 0x21:
            *p = (unsigned char)CHAR_1;
            return(-1);
        case 0x22:
            *p = (unsigned char)CHAR_2;
            return(-1);
        case 0x23:
            *p = (unsigned char)CHAR_3;
            return(-1);
        case 0x24:
            *p = (unsigned char)CHAR_4;
            return(-1);
        case 0x25:
            *p = (unsigned char)CHAR_5;
            return(-1);
        case 0x26:
            *p = (unsigned char)CHAR_6;
            return(-1);
        case 0x27:
            *p = (unsigned char)CHAR_7;
            return(-1);
        case 0x28:
            *p = (unsigned char)CHAR_8;
            return(-1);
        case 0x29:
            *p = (unsigned char)CHAR_9;
            return(-1);

        default:
            /* What to do here */
            *p = (unsigned char)CHAR_0;
            return(1);
    }
/* NOT REACHED */
    return(1);
}

int 
cob_get_sign_all (COB_RTD, unsigned char *p) {
    return cob_get_sign_all_(rtd,p);
}

static inline  int 
cob_is_sign_digit (COB_RTD, unsigned char *p) {
    int c = *p;
    switch ( SWITCH_CHAR(c) ) {
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
            return(1);
    }
    return(0);
}

static int COB_NOINLINE
cob_test_sign_ebcdic (COB_RTD, unsigned char *p) {
    int c = *p;
    switch ( SWITCH_CHAR(c) ) {
        case CASE_CHAR(CHAR_OPENAC) /*'{'*/:
        case CASE_CHAR(CHAR_A):
        case CASE_CHAR(CHAR_B):
        case CASE_CHAR(CHAR_C):
        case CASE_CHAR(CHAR_D):
        case CASE_CHAR(CHAR_E):
        case CASE_CHAR(CHAR_F):
        case CASE_CHAR(CHAR_G):
        case CASE_CHAR(CHAR_H):
        case CASE_CHAR(CHAR_I):
        case CASE_CHAR(CHAR_AT) /* @ */:
        case CASE_CHAR(CHAR_CLOSEAC) /* }*/:
        case CASE_CHAR(CHAR_J):
        case CASE_CHAR(CHAR_K):
        case CASE_CHAR(CHAR_L):
        case CASE_CHAR(CHAR_M):
        case CASE_CHAR(CHAR_N):
        case CASE_CHAR(CHAR_O):
        case CASE_CHAR(CHAR_P):
        case CASE_CHAR(CHAR_Q):
        case CASE_CHAR(CHAR_R):
            return(1);
        default:
            return(0);
    }
/* NOT REACHED */
    return(0);
}

static const char * COB_UTF16_BE_ID = "UTF-16BE";
static const char * COB_UTF16_LE_ID = "UTF-16LE";
const char *    cob_utf16_cp_id      (COB_RTD)
{
    if ( rtd->current_module && (rtd->current_module->module_version > 1) ) {
        if ( rtd->current_module->utf16_le ) {
            return COB_UTF16_LE_ID;
        }
    }
    return COB_UTF16_BE_ID;
}

static const unsigned char * COB_UTF16_BE_SPACE = (unsigned char*)"\000\040";
static const unsigned char * COB_UTF16_LE_SPACE = (unsigned char*)"\040\000";
const unsigned char *    cob_utf16_space      (COB_RTD)
{
    if ( rtd->current_module && (rtd->current_module->module_version > 1) ) {
        if ( rtd->current_module->utf16_le ) {
            return COB_UTF16_LE_SPACE;
        }
    }
    return COB_UTF16_BE_SPACE;
}

unsigned char *    cob_utf16_char      (COB_RTD, unsigned char c)
{
    if (c ==  CHAR_SP) {
        return(unsigned char *) cob_utf16_space(rtd);
    }
    if ( rtd->current_module && (rtd->current_module->module_version > 1) && ( rtd->current_module->utf16_le )) {
        rtd->utf16_cmp[1] = 0;
        rtd->utf16_cmp[0] = E2A(c);
    } else {
        rtd->utf16_cmp[0] = 0;
        rtd->utf16_cmp[1] = E2A(c);
    }
    return rtd->utf16_cmp;
}

int 
cob_test_sign(COB_RTD, cob_field *f) {
    int ret = 1;
    unsigned char   *p;

    switch ( COB_FIELD_TYPE (f) ) {
        case COB_TYPE_NUMERIC_DISPLAY:
            /* locate sign */
            if ( unlikely(COB_FIELD_SIGN_LEADING (f)) ) {
                p = f->data;
            } else {
                p = f->data + f->size - 1;
            }

            /* get sign */
            if ( unlikely(COB_FIELD_SIGN_SEPARATE (f)) ) {
                if ( *p != CHAR_PLUS && *p != CHAR_MINUS ) {
                    ret = 0;
                }
            } else {
                if ( *p >= CHAR_0 && *p <= CHAR_9 ) {
                    ret = 1;
                } else if ( *p == CHAR_SP ) {
                    ret = 1;
                } else if (cob_test_sign_ascii (rtd, p)) {
                    ret = 1;
                } else if (cob_test_sign_ebcdic (rtd, p)) {
                    ret = 1;
                } else {
                    ret = 0;
                }
            }
            break;
        default:
            ret = 1;
            break;
    }
    return(ret);
}

static int
common_cmpc (COB_RTD, const unsigned char *s1, const unsigned int c, const size_t size) {
    const unsigned char *s;
    size_t          i;
    int         ret;

    s = rtd->current_module->collating_sequence;
    if ( unlikely(s) ) {
        for ( i = 0; i < size; ++i ) {
            if ( (ret = s[s1[i]] - s[c]) != 0 ) {
                return(ret);
            }
        }
    } else {
        for ( i = 0; i < size; ++i ) {
            if ( (ret = s1[i] - c) != 0 ) {
                return(ret);
            }
        }
    }
    return(0);
}

static int
common_cmpn (COB_RTD, const unsigned char *s1, const unsigned char *s2, const size_t size) {
    size_t      i;

    int         ret;

    for ( i = 0; i < size; ++i ) {
        if ( (ret = s1[i] - s2[i%2]) != 0 ) {
            return(ret);
        }
    }
    return(0);
}

static int
common_cmps (COB_RTD, const unsigned char *s1, const unsigned char *s2, const size_t size,
             const unsigned char *col) {
    size_t          i;
    int         ret;

    if ( unlikely(col) ) {
        for ( i = 0; i < size; ++i ) {
            if ( (ret = col[s1[i]] - col[s2[i]]) != 0 ) {
                return(ret);
            }
        }
    } else {
        for ( i = 0; i < size; ++i ) {
            if ( (ret = s1[i] - s2[i]) != 0 ) {
                return(ret);
            }
        }
    }
    return(0);
}

cob_field * 
cob_dup_field_if_needed(COB_RTD, cob_field *f) {
    cob_field * ret = f;
    register int  i;
    unsigned char *p;
    if ( COB_FIELD_TYPE (f) == COB_TYPE_NUMERIC_DISPLAY ) {
        if ( unlikely(COB_FIELD_SIGN_LEADING (f)) ) {
            p = f->data;
        } else {
            p = f->data + f->size - 1;
        }
        if (!cob_is_sign_digit(rtd, p)) {
            i = rtd->duplicat_current;
            ret = &(rtd->duplicat_field[i]);
            if ( rtd->duplicat_size[i] < f->size ) {
                if ( rtd->duplicat_field[i].data ) {
                    cob_free(rtd->duplicat_field[i].data);
                }
                rtd->duplicat_field[i].data = malloc(f->size);
                rtd->duplicat_size[i] = f->size;
            }
            ret->attr = f->attr;
            ret->size = f->size;
            memcpy(ret->data, f->data, f->size);
            rtd->duplicat_current ++;
            if ( rtd->duplicat_current >= DUPLICAT_COUNT ) {
                rtd->duplicat_current = 0;
            }
        }
    }
    return(ret);
}

static int
cob_cmp_char (COB_RTD, cob_field *f, const unsigned int c) {
    /*COBOL_IT*/
    int sign;
    int ret;

    sign = COB_DUP_GET_SIGN (f);
    if (COB_FIELD_IS_NATIONAL(f)) {
        ret = common_cmpn (rtd, f->data, cob_utf16_char(rtd,c), f->size);

    } else {
        ret = common_cmpc (rtd, f->data, c, f->size);
    }
    return(ret);
}

static int
cob_cmp_all (COB_RTD, cob_field *f1, cob_field *f2) {
    const unsigned char *s;
    int                 ret = 0;
    /*COBOL_IT*/
    int                 sign= COB_DUP_GET_SIGN (f1);
    size_t              size = f1->size;
    unsigned char       *data = f1->data;

    sign++; /* Avaoid Warning*/
    s = rtd->current_module->collating_sequence;
    while ( size >= f2->size ) {
        if ( (ret = common_cmps (rtd, data, f2->data, f2->size, s)) != 0 ) {
            goto end;
        }
        size -= f2->size;
        data += f2->size;
    }
    if ( size > 0 ) {
        ret = common_cmps (rtd, data, f2->data, size, s);
    }

    end:
    /*COBOL_IT*/
    return(ret);
}

static int
cob_cmp_alnum (COB_RTD, cob_field *f1, cob_field *f2) {
    const unsigned char *s;
    int                 ret;
    size_t              min = (f1->size < f2->size) ? f1->size : f2->size;
    s = rtd->current_module->collating_sequence;

    /*COBOL_IT*/
    COB_DUP_GET_SIGN (f1);
    COB_DUP_GET_SIGN (f2);

    /* compare common substring */
    if ( (ret = common_cmps (rtd, f1->data, f2->data, min, s)) != 0 ) {
        goto end;
    }

    /* compare the rest (if any) with spaces */
    if ( f1->size > f2->size ) {
        ret = common_cmpc (rtd, f1->data + min, CHAR_SP, f1->size - min);
    } else if ( f1->size < f2->size ) {
        ret = -common_cmpc (rtd, f2->data + min, CHAR_SP, f2->size - min);
    }

    end:
    /*COBOL_IT*/
    return(ret);
}

static int
cob_cmp_national (COB_RTD, cob_field *f1, cob_field *f2) {
    int                 ret;
    size_t              min = (f1->size < f2->size) ? f1->size : f2->size;

    /*COBOL_IT*/
    COB_DUP_GET_SIGN (f1);
    COB_DUP_GET_SIGN (f2);

    /* compare common substring */
    if ( (ret = common_cmps (rtd, f1->data, f2->data, min, NULL)) != 0 ) {
        goto end;
    }

    /* compare the rest (if any) with spaces */
    if ( f1->size > f2->size ) {
        ret = common_cmpn (rtd, f1->data + min, cob_utf16_space(rtd), f1->size - min);
    } else if ( f1->size < f2->size ) {
        ret = -common_cmpn (rtd, f2->data + min,cob_utf16_space(rtd), f2->size - min);
    }

    end:
    /*COBOL_IT*/
    return(ret);
}

static int
sort_cmps (COB_RTD, const unsigned char *s1, const unsigned char *s2, const size_t size,
           const unsigned char *col) {
    size_t          i;
    int             ret;

    if ( col ) {
        for ( i = 0; i < size; ++i ) {
            if ( (ret = col[s1[i]] - col[s2[i]]) != 0 ) {
                return(ret);
            }
        }
    } else {
        for ( i = 0; i < size; ++i ) {
            if ( (ret = s1[i] - s2[i]) != 0 ) {
                return(ret);
            }
        }
    }
    return(0);
}

static int
sort_compare (COB_RTD, const void *data1, const void *data2) {
    size_t      i;
    int         cmp;
    cob_field   f1;
    cob_field   f2;

    for ( i = 0; i < rtd->sort_nkeys; ++i ) {
        f1 = f2 = *(rtd->sort_keys[i]).field;
        f1.data = (unsigned char *)data1 + rtd->sort_keys[i].offset;
        f2.data = (unsigned char *)data2 + rtd->sort_keys[i].offset;
        if ( COB_FIELD_IS_NUMERIC(&f1) ) {
            cmp = cob_numeric_cmp (rtd, &f1, &f2);
        } else {
            cmp = sort_cmps (rtd, f1.data, f2.data, f1.size, rtd->sort_collate);
        }
        if ( cmp != 0 ) {
            return(rtd->sort_keys[i].flag == COB_ASCENDING) ? cmp : -cmp;
        }
    }
    return(0);
}

/*
 * Global functions
 */
void *
cob_malloc (COB_RTD, const size_t size) {
    void *mptr;
    int sz = size+(2*sizeof(size_t));

    mptr = calloc (1,sz);
    if ( unlikely(!mptr) ) {
        cob_runtime_error (rtd, "Cannot acquire %d bytes of memory - Aborting", size);
        cob_stop_abend (rtd, COBRE_OUT_OF_MEMORY);
    }
    return(mptr);
}

/* avoid for windows DLL trouble */
void
coblib_free (void *mptr)
{
    free(mptr);
}

void
cob_set_location (COB_RTD, const unsigned int line_debug_idx, const  char * cur_source,
                  const unsigned int sline, const char *cstatement) {
    register cob_module *mod = rtd->current_module;
    mod->cur_line_debug_idx = line_debug_idx;
    mod->cur_line = sline;
    mod->cur_source_file = (char*)cur_source;
    if ( cstatement ) {
        rtd->cob_source_statement = cstatement;
    }
    if ( rtd->cob_line_trace && rtd->cob_err_file && !rtd->cob_disable_trace) {
        fprintf (rtd->cob_err_file, "PROGRAM-ID: %s \tLine: %d \tStatement: %s\n",
                 (char *)mod->module_name, sline, cstatement ? (char *)cstatement : "Unknown");
        fflush (rtd->cob_err_file);
    }
    if (line_debug_idx)
    {
       /* only call debugger if code gen generate debug_idx*/
       debug_callback_fct(rtd, COB_DBCALLBACK_SETLOCATION);
    }
}

void
cob_ready_section_trace (COB_RTD) {
    rtd->cob_section_trace = 1;
}

void
cob_reset_section_trace (COB_RTD) {
    rtd->cob_section_trace = 0;
}

int
cob_get_section_trace (COB_RTD) {
    return rtd->cob_section_trace;
}

void
cob_ready_trace (COB_RTD) {
    rtd->cob_line_trace = 1;
}

void
cob_reset_trace (COB_RTD) {
    rtd->cob_line_trace = 0;
}

unsigned char *
cob_get_pointer (COB_RTD, const unsigned char *srcptr) {
    unsigned char   *tmptr;

    memcpy (&tmptr, srcptr, sizeof (void *));
    return(tmptr);
}

void *
cob_get_prog_pointer (COB_RTD, const unsigned char *srcptr) {
    void    *tmptr;

    memcpy (&tmptr, srcptr, sizeof (void *));
    return(tmptr);
}

void
cob_memcpy (COB_RTD, cob_field *dst, unsigned char *src, const int size) {
    cob_field       temp;
    cob_field_attr  attr;

    if ( COB_FIELD_TYPE(dst) == COB_TYPE_NUMERIC_DISPLAY ) {
        COB_ATTR_INIT (COB_TYPE_NUMERIC_DISPLAY, size, dst->attr->scale, dst->attr->flags, NULL);
    } else {
        COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    }
    temp.size = size;
    temp.data = src;
    temp.attr = &attr;
    cob_move (rtd, &temp, dst);
}

void
cob_memcpy_string (COB_RTD, cob_field *dst, unsigned char *src, const int size) {
    cob_field       temp;
    cob_field_attr  attr;

    COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    temp.size = size;
    temp.data = src;
    temp.attr = &attr;
    cob_move (rtd, &temp, dst);
}

void
cob_memcpy_national (COB_RTD, cob_field *dst, unsigned char *src, const int size) {
    cob_field       temp;
    cob_field_attr  attr;

    COB_ATTR_INIT (COB_TYPE_NATIONAL, 0, 0, 0, NULL);
    temp.size = size;
    temp.data = src;
    temp.attr = &attr;
    cob_move (rtd, &temp, dst);
}

const char *
cob_get_exception_name (COB_RTD, const int exception_code) {
    size_t  n;

    for ( n = 0; n < EXCEPTION_TAB_SIZE; ++n ) {
        if ( exception_code == cob_exception_tab_code[n] ) {
            return(cob_exception_tab_name[n]);
        }
    }
    return(NULL);
}

int
cob_set_exception (COB_RTD, const int id) {
    rtd->cob_exception_code = cob_exception_tab_code[id];
    if ( rtd->cob_exception_code ) {
        rtd->cob_got_exception = 1;
        rtd->cob_orig_statement = rtd->cob_source_statement;
        if ( rtd->current_module ) {
            rtd->cob_orig_line = rtd->current_module->cur_line;
            rtd->cob_orig_program_id = rtd->current_module->module_name;
            if ( rtd->current_module->lines_debug_info && rtd->current_module->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION) {
                cob_lines_debug_info *p = rtd->current_module->lines_debug_info;
                while ( p && p->line_nr ) {
                    if ( p->line_nr > (int)rtd->cob_orig_line ) {
                        break;
                    }
                    if ( p->label ) {
                        if ( p->is_section ) {
                            rtd->cob_orig_section = p->label;
                        } else {
                            rtd->cob_orig_paragraph = p->label;
                        }
                    }
                    p++;
                }

            }
        }
    }
    return 0;
}

static char *
cob_system_tmp_dir (void)
{

#ifdef _WIN32
    static char    buff[MAX_PATH];

    GetTempPath (MAX_PATH, buff);
    return buff;
#else
    return(char *)"/tmp";
#endif
}


char *
cob_tmp_dir (void) {
    char * s;
    if ( (s = getenv ("TMPDIR")) != NULL ) {
        return s;
    } else if ( (s = getenv ("TMP")) != NULL ) {
        return s;
    } else if ( (s = getenv ("TEMP")) != NULL ) {
        return s;
    } else {
        return(char *)cob_system_tmp_dir();;
    }
}

char *
cob_debug_tmp_dir (void) {
    char * s;
    if ( (s = getenv ("COB_DEBUG_TMP")) != NULL ) {
        return s;
    } else {
        return(char *)cob_system_tmp_dir();
    }
}

char *
cob_strerror (COB_RTD, int err) {
#ifdef _WIN32
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0,rtd->strerrbuffer, 1024,NULL);
    return(rtd->strerrbuffer);
#else
#ifdef HAVE_STRERROR
    sprintf (rtd->strerrbuffer, "%d : %s", err, strerror(err));
    return(rtd->strerrbuffer);
#else
    sprintf (rtd->strerrbuffer, "Error code %d", err);
    return(rtd->strerrbuffer);
#endif
#endif
}


void
cob_init (COB_RTD, int argc, char **argv) {
    char    *s;
    size_t  i;

    char    buff[32];

    if ( !rtd->cob_initialized ) {
        cob_lock_mutex();
        if ( !citkey_test_feature(COB_PN_RUNTIME) ) {
            if ( !citkey_test_feature("COMMUNITY") ) {
                check_citkey(COB_PN_RUNTIME);
            }
        } else {
            cob_enterprise_mode = 1;
        }
        rtd->usused_call_flag = COB_RTD_VERSION_PRESENT;  /* mark the precense of rtd_version*/
        rtd->runtime_version_string = (char*)COB_VERSION;
        rtd->runtime_version_cnt = COB_RTD_VERSION_CNT;
        if (cob_bool_getenv ("COB_NO_SIGNAL"))
            rtd->cob_disable_signal_handler = 1;
        /* ONLY Set it on REgion 0 */
        if ( !rtd->cob_disable_signal_handler && !rtd->parent_rtd) {
            cob_set_signal();
        }

        rtd->current_arg = 1;
        rtd->cob_argc = argc;
#if defined(CIT_EBCDIC_CHARSET)
#if !defined(CIT_DUAL_CHARSET)
        rtd->ebcdic_charset = 1;
#endif
        if (!check_citkey("EBCDIC"))
            exit (-1);
        rtd->cob_argv = cob_malloc(rtd, sizeof(char*) * (argc+1));        
        rtd->cob_argv_allocated = 1;
        for ( i = 0; i < argc; i++ ) {
            rtd->cob_argv[i] = (char*)STRING_A2E((unsigned char*)strdup(argv[i]), strlen(argv[i]));
        }
#else
        rtd->cob_argv = argv;
#endif
        cob_init_rtd_charset(rtd, rtd->ebcdic_charset);
        /* Get emergency buffer */
        (rtd->runtime_err_str) = cob_malloc (rtd, COB_ERRBUF_SIZE);

#ifdef	HAVE_SETLOCALE
        setlocale (LC_ALL, "");
        setlocale (LC_NUMERIC, "C");
        s = setlocale (LC_ALL, NULL);
        if ( s ) {
            rtd->locale_save = strdup (s);
        }
#endif
#ifdef	ENABLE_NLS
        bindtextdomain (PACKAGE, LOCALEDIR);
        textdomain (PACKAGE);
#endif

/* Dirty hack until we implement something better */
#if defined(_WIN32) /* && !defined(_MSC_VER)*/
        if (cob_bool_getenv ("COB_CONSOLE_CRLF") == NULL) {
            _setmode (_fileno (stdin), _O_BINARY);
            _setmode (_fileno (stdout), _O_BINARY);
            _setmode (_fileno (stderr), _O_BINARY);
        }
#endif

        if ( rtd->cob_err_file == NULL ) {
            s = getenv ("COB_ERROR_FILE");
            rtd->cob_err_file = stderr;
            if ( s ) {
                FILE *f;
                f = fopen(s, "a");
                if ( f ) {
                    rtd->cob_err_file = f;
                }
            }
        }
        rtd->cob_tmpdir = cob_tmp_dir();

        cob_init_numeric (rtd);
        memset (rtd->duplicat_field, 0, sizeof(rtd->duplicat_field));
        memset (rtd->duplicat_size, 0, sizeof(rtd->duplicat_size));
        for ( i = 0; i < DUPLICAT_COUNT; i++ ) {
            rtd->duplicat_field[i].data = cob_malloc (rtd, 256);
            rtd->duplicat_field[i].size = 256;
            rtd->duplicat_size[i] = 256;
        }
        cob_init_strings (rtd);
        cob_init_move (rtd);
        cob_init_intrinsic (rtd);
        cob_init_fileio (rtd);
        cob_init_termio (rtd);
        cob_init_call (rtd);

        for (i = 0; i < SWITCH_COUNT; ++i) {
            memset (buff, 0, sizeof (buff));
            sprintf (buff, "COB_SWITCH_%d", (int)(i));
            s = getenv (buff);
            if ( s && strcasecmp (s, "ON") == 0 ) {
                rtd->cob_switch[i] = 1;
            } else {
                rtd->cob_switch[i] = 0;
            }
        }

        /* Micro Focus format for setting switches 0-8 */
        s = getenv("COBSW");
        if (s) {
            for (i = 0; (i * 2) < (strlen(s) - 1); ++i) {
                switch (s[i * 2 + 1]) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    if (s[i * 2] == '+') {
                        rtd->cob_switch[ (int) (s[i * 2 + 1] - '0') ] = 1;
                        break;
                    }
                    if (s[i * 2] == '-') {
                        rtd->cob_switch[ (int) (s[i * 2 + 1] - '0')] = 0;
                        break;
                    }
                    break;
                }
            }
        }

        s = cob_bool_getenv ("COB_LINE_TRACE");
        if ( s && (*s == 'Y' || *s == 'y') ) {
            rtd->cob_line_trace = 1;
        }
        s = cob_bool_getenv("COB_DISABLE_TRACE");
        if ( s ) rtd->cob_disable_trace = 1;

        rtd->cob_initialized = 1;
        s = getenv("COB_DEBUG_MODULES");
        if ( s ) rtd->auto_break_modules = strdup(s);

        s = cob_bool_getenv("COB_PROFILING_ELAPSED");
        if ( s ) rtd->profiler_elapsed_time=1;
        s = cob_bool_getenv("COB_PROFILING_EACH_MODULE");
        if ( s ) {
            rtd->disable_profiling_files =0;
        } else {
            rtd->disable_profiling_files =1;
        }
        s = getenv ("COB_WARNING");
        if ( s && (*s == 'N' || *s == 'n' || *s == '0') ) {
            rtd->warning_disable = 1;
        }
        cob_unlock_mutex();
    }
}

void 
cob_cmdline_to_argv (COB_RTD, const char *prgname, const char *cmdl, int *argc, char **argv, int argvsize) {
    char *p;
    char *saveptr=NULL;

    *argc = 0;
    argv[*argc] = (char*)prgname;
    (*argc)++;

    if ( rtd->cmdlcopy ) {
        free(rtd->cmdlcopy);
    }
    rtd->cmdlcopy = strdup(cmdl);
    p = COB_STRTOK (rtd->cmdlcopy, " \t", &saveptr);
    while ( p && (*argc < argvsize) ) {
        argv[*argc]=(char*)STRING_A2E((unsigned char*)p, strlen(p));;
        (*argc)++;
        p = COB_STRTOK (NULL, " \t", &saveptr);
    }

}

void 
cob_set_command_line (COB_RTD, const char *prgname, const char *cmdl) {

    rtd->cob_argc = 0;
    rtd->cob_argv = rtd->locargv;

    memset (rtd->locargv, 0, sizeof(rtd->locargv));
    cob_cmdline_to_argv(rtd, prgname,cmdl, &(rtd->cob_argc), rtd->cob_argv, 64);
}

unsigned long 
cob_clock(cob_paragraph_profiling_times *time)
{
    clock_t l = clock();
    if ( time ) {
#ifdef _WIN32
        struct __timeb64 t;
        _ftime64(&t);
        time->elaps_tick = (long long)t.time * 1000000LL + (long long)t.millitm * 1000LL;

#else
        struct timeval t;
        gettimeofday(&t, NULL);
        time->elaps_tick = (long long)t.tv_sec * 1000000LL + (long long)t.tv_usec; 
#endif
        time->cpu_tick = l;
    }
    return l;
}

extern int cob_debug_sig;
void
cob_module_enter (COB_RTD, cob_module *module) {
    if ( unlikely(!rtd->cob_initialized) ) {
        cob_init (rtd, 0, NULL);
        if ( rtd->cob_err_file && !rtd->warning_disable ) {
            fputs ("Warning: cob_init expected in the main program\n", rtd->cob_err_file);
        }
    }
    if (module) {
        cob_init_rtd_charset(rtd,module->ebcdic_charset);
    }
    if ( module->profiling_info ) {
        if (!rtd->runtime_debug_started) {
            cob_runtime_debugger_init_rtd(rtd);
        }
        if ( module->module_version > 0 ) {
            cob_clock(&module->profiling_last);
        } else {
            module->profiling_last_cpu_tick_v0 = cob_clock(NULL);
        }
    }
    if ( module->module_version > 1 ) {
        module->module_age = rtd->debug_age++;
    }
    if (rtd->current_module == module) {
        rtd->current_module = NULL;
    }
    module->next = rtd->current_module;
    if ( rtd->auto_break_modules && module->module_name) {
        char *p =strstr(rtd->auto_break_modules, module->module_name);
        if (p) {
            if (p > rtd->auto_break_modules && 
             ! (*(p-1) == 0 || *(p-1)==';' || *(p-1) ==',' || *(p-1)==':' )) {
                /*continue*/
            } else {
                int i = strlen(module->module_name);
                if (p[i] == 0 || p[i]==';' || p[i] ==',' || p[i]==':' ) {
                    cob_runtime_debugger_activate(rtd, 0);
                }
            }
        }
    }
    rtd->current_module = module;
    debug_callback_fct(rtd, COB_DBCALLBACK_ENTER_MODULE);
    if ( !module->default_cp ) {
        if ( rtd->ebcdic_charset ) {
            module->default_cp = "IBM-1147";
        } else {
            module->default_cp = "ASCII";
        }
    }
}

void
cob_module_leave (COB_RTD, cob_module *module) {
    struct __cob_module    *curmod = rtd->current_module;
    if ( curmod ) {
        if ( curmod->module_version > 1 ) {
            if (rtd->sysin_file && ( curmod->sysin_file !=  rtd->sysin_file)) {
                cob_close_extfh(rtd,rtd->sysin_file, 0, NULL);
            }
            if (rtd->sysout_file && (curmod->sysout_file !=  rtd->sysout_file)) {
                cob_close_extfh(rtd,rtd->sysout_file, 0, NULL);
            }
            if (rtd->syserr_file && (curmod->syserr_file !=  rtd->syserr_file)) {
                cob_close_extfh(rtd,rtd->syserr_file, 0, NULL);
            }
            if (rtd->sysprint_file && (curmod->sysprint_file !=  rtd->sysprint_file)) {
                cob_close_extfh(rtd,rtd->sysprint_file, 0, NULL);
            }
            rtd->sysin_file  = curmod->sysin_file;  
            rtd->sysout_file = curmod->sysout_file;  
            rtd->syserr_file = curmod->syserr_file;  
            rtd->sysprint_file = curmod->sysprint_file;  
            cob_runtime_debugger_close_module(rtd,curmod);
        }
        rtd->current_module = curmod->next;
        curmod->next = NULL;
        debug_callback_fct(rtd, COB_DBCALLBACK_LEAVE_MODULE);
    } else {
        if (rtd->sysin_file) {
            cob_close_extfh(rtd,rtd->sysin_file, 0, NULL);
        }
        if (rtd->sysout_file) {
            cob_close_extfh(rtd,rtd->sysout_file, 0, NULL);
        }
        if (rtd->syserr_file) {
            cob_close_extfh(rtd,rtd->syserr_file, 0, NULL);
        }
        if (rtd->sysprint_file) {
            cob_close_extfh(rtd,rtd->sysprint_file, 0, NULL);
        }
        rtd->sysin_file  = NULL;  
        rtd->sysout_file = NULL;  
        rtd->syserr_file = NULL;  
        rtd->sysprint_file = NULL;  
    }
    //cob_profiling_dump(rtd, module);
    cob_enterprise_ccmap_dump(rtd,module);
    if (rtd->current_module) {
        cob_init_rtd_charset(rtd,rtd->current_module->ebcdic_charset);
    }
}

static void cob_close_exec (COB_RTD);

void
cob_stop_run (COB_RTD, const int status) {
    cob_module * mod;
    cob_module * mod1;
    cit_runtime_t * myrtd = rtd;
    myrtd->cob_last_status =  status;
    mod = rtd->current_module ;
    mod1 = mod;
    while ( mod) {
        //cob_profiling_dump(rtd, mod);
        cob_enterprise_ccmap_dump(rtd,mod);
        mod = COB_NEXT_MODULE(mod);
        if (mod == mod1) {
            break;
        }
    }

    cob_close_exec(rtd);
    myrtd = cob_enterprise_get_region_0(rtd);
    if ( myrtd->cob_exit_proc ) {
        cob_rtd_exit_proc p = myrtd->cob_exit_proc;
        myrtd->cob_exit_proc = NULL;
        p(rtd, status);
    }
    cob_enterprise_terminate_region(myrtd);
    cob_terminate_exec(myrtd);
    cob_call_close_all_handles(myrtd);

    if ( myrtd->cob_longjmp_defined ) {
        longjmp(myrtd->cob_longjmp, status ? status: 1);  
    }
    if (myrtd->cobjmp_primed && myrtd->mf_cobjmp_buf) {
        coblongjmp(myrtd->mf_cobjmp_buf);  
    }
    exit (status);
}

void
cob_stop_abend (COB_RTD, const runtime_error_code_t status) {
    int s = status;

    cob_dump_debug_info(rtd);
    if ( s < 127 ) {
        s =  (256-(status%255));
    }
    cob_stop_run (rtd, s);
}

static int cob_check_stop_abend(COB_RTD);

void COB_NOINLINE
cob_runtime_error (COB_RTD, const char *fmt, ...) {
    struct handlerlist  *h, *e;
    char                *p;
    va_list             ap;

    if ( (rtd->runtime_err_str) ) {
        p = (rtd->runtime_err_str);
        if ( rtd->current_module && rtd->current_module->cur_source_file ) {
            sprintf ((rtd->runtime_err_str), "%s:%d: ", rtd->current_module->cur_source_file, rtd->current_module->cur_line);
            p = (rtd->runtime_err_str) + strlen ((rtd->runtime_err_str));
        }
        va_start (ap, fmt);
        vsprintf (p, fmt, ap);
        va_end (ap);
        if ( rtd->cob_errormessage_proc ) {
            rtd->cob_errormessage_proc(rtd, rtd->runtime_err_str);
        }
    }
    if (rtd->hdlrs != NULL && cob_check_stop_abend(rtd) ) {
        h = rtd->hdlrs;
        while ( h != NULL ) {
            if ( h->proc_cob ) {
                if ( rtd->runtime_err_str ) {
                    h->proc_cob (rtd->runtime_err_str);    
                } else {
                    h->proc_cob ((char *)"Malloc error");    
                }
            }
            e = h;
            h = h->next;
            cob_free(e);
        }
        rtd->hdlrs = NULL;
    }

    /* prefix */
    if ( rtd->cob_err_file ) {
        if ( rtd->current_module && rtd->current_module->cur_source_file ) {
            fprintf (rtd->cob_err_file, "%s:%d: ", rtd->current_module->cur_source_file, rtd->current_module->cur_line);
        }
        fputs ("libcob: ", rtd->cob_err_file);

        /* body */
        va_start (ap, fmt);
        vfprintf (rtd->cob_err_file, fmt, ap);
        va_end (ap);

        /* postfix */
        fputs ("\n", rtd->cob_err_file);
        fflush (rtd->cob_err_file);
    }
}

void
cob_fatal_error (COB_RTD, const unsigned int fatal_error) {
    char *p;
    switch ( fatal_error ) {
        case COB_FERROR_INITIALIZED:
            cob_runtime_error (rtd, "cob_init() has not been called");
            cob_stop_abend (rtd, COBRE_COB_INIT);
            break;
        case COB_FERROR_CODEGEN:
            cob_runtime_error (rtd, "Codegen error - Please report this");
            cob_stop_abend (rtd, COBRE_CODE_GEN);
            break;
        case COB_FERROR_CHAINING:
            cob_runtime_error (rtd, "ERROR - Recursive call of chained program");
            cob_stop_abend (rtd, COBRE_CHAINING);
            break;
        case COB_FERROR_STACK:
            cob_runtime_error (rtd, "Stack overflow, possible PERFORM depth exceeded");
            cob_stop_abend (rtd, COBRE_STACK);
            break;
        case COB_FERROR_UNCOMPRESS:
            cob_runtime_error (rtd, "Uncompress data failure");
            cob_stop_abend (rtd, COBRE_FATAL_ERROR);
            break;
        case COB_FERROR_NEGSCALE:
            cob_runtime_error (rtd, "Codegen error - Non Display negative scale. Please report this");
            cob_stop_abend (rtd, COBRE_FATAL_ERROR);
            break;
        case COB_FERROR_UNCATCH_EXP:
            p = (char*)cob_get_exception_name (rtd, (const int)rtd->cob_exception_code);
            if (p) {
                cob_runtime_error (rtd, "Uncatched exception: %s", p);
            } else {
                cob_runtime_error (rtd, "Uncatched exception");
            }
            break;
        default:
            cob_runtime_error (rtd, "Unknown failure : %d", (int)fatal_error);
            break;
    }
    cob_stop_abend (rtd, COBRE_FATAL_ERROR);
}

static void 
cob_abend_version (COB_RTD, const char *prog, const char *packver, const int patchlev)
{
    cob_runtime_error (rtd, "Warning - Version mismatch");
    cob_runtime_error (rtd, "%s has version/patch level %s/%d", prog, packver,
                       patchlev);
    cob_runtime_error (rtd, "Library has version/patch level %s/%d", PACKAGE_VERSION,
                       PATCH_LEVEL);
    cob_stop_abend (rtd, COBRE_FATAL_ERROR);
}

static void 
cob_extract_version (char *buffer, int *v1, int *v2, int *v3)
{
    char *p;
    char *saveptr=NULL;

    *v1=*v2=*v3=0;

    p = COB_STRTOK(buffer, ".-", &saveptr);
    if ( p ) {
        *v1 = atoi(p);
        p = COB_STRTOK(NULL, ".-", &saveptr);
        if ( p ) {
            *v2 = atoi(p);
            p = COB_STRTOK(NULL, ".-", &saveptr);
            if ( p ) {
                *v3 = atoi(p);
            }
        }
    }

}

static void 
init_charset_const (COB_RTD) {
    if (rtd->ebcdic_charset) {
        str_zero[0]  = EBCDIC_CHAR_0;
        str_space[0] = EBCDIC_CHAR_SP;
        str_quote[0] = EBCDIC_CHAR_DQUOTE;
        str_one[0]   = EBCDIC_CHAR_1;
    } else {
        str_zero[0]  = ASCII_CHAR_0;
        str_space[0] = ASCII_CHAR_SP;
        str_quote[0] = ASCII_CHAR_DQUOTE;
        str_one[0]   = ASCII_CHAR_1;
    }
    cob_move_init_charset_const(rtd);
}

static void 
cob_init_rtd_charset (COB_RTD, int ebcdic_charset)
{
    if ( !rtd->ebcdic_charset_init || ebcdic_charset != rtd->ebcdic_charset) {
        rtd->ebcdic_charset_init = 1;
        rtd->ebcdic_charset = ebcdic_charset;
        init_charset_const(rtd);
#if defined(CIT_DUAL_CHARSET)
        {
            int i;
            if ( ebcdic_charset ) {
                for (i = 0; i <= 255; i++  ) {
                    rtd->charset_map[i] =  A2E_F(i);
                }
            } else {
                for (i = 0; i <= 255; i++  ) {
                    rtd->charset_map[i] =  i;
                }
            }
        }
#endif
    }
}


void
cob_check_version_1 (COB_RTD, const char *prog, const char *packver, const int patchlev, int ebcdic_charset) 
{
    char buffer[255];
    int  r1, r2, r3;
    int  p1, p2, p3;

    if ( strcmp (packver, PACKAGE_VERSION) || patchlev > PATCH_LEVEL ) {
        /*printf("%s / %s", packver, PACKAGE_VERSION);*/
        strcpy(buffer, packver);
        cob_extract_version(buffer, &p1, &p2, &p3);
        strcpy(buffer, PACKAGE_VERSION);
        cob_extract_version(buffer, &r1, &r2, &r3);
        if ( p1 != r1 ) {
            cob_abend_version(rtd,prog,packver,patchlev);
        }
        if (p2 < COB_MINIMAL_MINOR_VERSION) {
            cob_abend_version(rtd,prog,packver,patchlev);
        }
        if ( r2 > p2 ) {
            return;
        }
        if ( r2 < p2 ) {
            cob_abend_version(rtd,prog,packver,patchlev);
        }
        if ( r3 < p3 ) {
            cob_abend_version(rtd,prog,packver,patchlev);
        }
    }
    cob_init_rtd_charset(rtd,ebcdic_charset);
    return;
}

void
cob_check_version (COB_RTD, const char *prog, const char *packver, const int patchlev) {
    cob_check_version_1(rtd,prog,packver,patchlev,0);
}
/*
 * Sign
 */


int
cob_real_get_sign (COB_RTD, cob_field *f) {
    unsigned char   *p;

    switch ( COB_FIELD_TYPE (f) ) {
        case COB_TYPE_NUMERIC_DISPLAY:
            /* locate sign */
            if ( unlikely(COB_FIELD_SIGN_LEADING (f)) ) {
                p = f->data;
            } else {
                p = f->data + f->size - 1;
            }

            /* get sign */
            if ( unlikely(COB_FIELD_SIGN_SEPARATE (f)) ) {
                return(*p == CHAR_MINUS) ? -1 : 1;
            } else {
                return(cob_get_sign_all_ (rtd, p));
/*
                if ( *p >= CHAR_0 && *p <= CHAR_9 ) {
                    return(1);
                }
                if ( *p == CHAR_SP ) {
                    *p = (unsigned char)CHAR_0;
                    return(1);
                }
                if ( unlikely(rtd->current_module->display_sign) ) {
                    return(cob_get_sign_ebcdic (p));
                } else {
#ifdef	COB_EBCDIC_MACHINE
                    cob_get_sign_ascii (p);
#else
                    GET_SIGN_ASCII (*p);
#endif
                    return(-1);
                }
*/ 
            }
        case COB_TYPE_NUMERIC_PACKED:
            if ( COB_FIELD_PACKED_SIGN_MISSING(f) ) {
                return 1;
            }
            p = f->data + COB_FIELD_DIGITS(f) / 2;
            return((*p & 0x0f) == 0x0d) ? -1 : 1;
        default:
            return(0);
    }
}

int
cob_display_get_sign (COB_RTD, cob_field *f, unsigned char **start, int *size) {
    unsigned char   *p;

    switch ( COB_FIELD_TYPE (f) ) {
        case COB_TYPE_NUMERIC_DISPLAY:
            /* locate sign */
            *start = f->data;
            *size = f->size;
            if ( unlikely(COB_FIELD_SIGN_LEADING (f)) ) {
                p = f->data;
            } else {
                p = f->data + f->size - 1;
            }

            /* get sign */
            if ( unlikely(COB_FIELD_SIGN_SEPARATE (f)) ) {
                *size  =  *size -1;
                if ( unlikely(COB_FIELD_SIGN_LEADING (f)) ) {
                    *start = *start +1;
                }
                return(*p == CHAR_MINUS) ? -1 : 1;
            } else {
                return(cob_get_sign_unchanged_ (rtd, p));
            }
        case COB_TYPE_NUMERIC_PACKED:
            if ( COB_FIELD_PACKED_SIGN_MISSING(f) ) {
                return 1;
            }
            p = f->data + COB_FIELD_DIGITS(f) / 2;
            return((*p & 0x0f) == 0x0d) ? -1 : 1;
        default:
            return(0);
    }
}


void
cob_real_put_sign (COB_RTD, cob_field *f, const int sign) {
    unsigned char   *p;
    int     c;

    switch ( COB_FIELD_TYPE (f) ) {
        case COB_TYPE_NUMERIC_DISPLAY:
            /* locate sign */
            if ( unlikely(COB_FIELD_SIGN_LEADING (f)) ) {
                p = f->data;
            } else {
                p = f->data + f->size - 1;
            }

            /* put sign */
            if ( unlikely(COB_FIELD_SIGN_SEPARATE (f)) ) {
                c = (sign < 0) ? CHAR_MINUS : CHAR_PLUS;
                if ( *p != c ) {
                    *p = c;
                }
            } else if ( unlikely(rtd->current_module && rtd->current_module->display_sign) ) {
                cob_put_sign_ebcdic (rtd, p, sign);
            } else if ( sign < 0 ) {
                if ( rtd->ebcdic_charset ) {
                    cob_put_sign_ascii (rtd, p, sign);
                } else if (rtd->current_module && rtd->current_module->runtime_flags & COB_FLAG_RT_CAREALIA_SIGN) {
                    PUT_SIGN_CAREALIA (*p);
                } else if (rtd->current_module && rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_IBM) {
                    cob_put_sign_ebcdic (rtd, p, sign);
                } else {
                    PUT_SIGN_ASCII (*p);
                }
            }
            return;
        case COB_TYPE_NUMERIC_PACKED:
            if ( COB_FIELD_PACKED_SIGN_MISSING(f) ) {
                return;
            }
            p = f->data + COB_FIELD_DIGITS(f) / 2;
            if ( sign < 0 ) {
                *p = (*p & 0xf0) | 0x0d;
            } else {
                *p = (*p & 0xf0) | 0x0c;
            }
            return;
        default:
            return;
    }
}

void 
cob_string_to_C(COB_RTD, char *s, int size){
    int i;
    for ( i = (int) size - 1; i >= 0; i-- ) {
        if ( s[i] != CHAR_SP && s[i] != 0 ) {
            break;
        }
    }
    s[i + 1] = '\0';
}

void
cob_field_to_string (COB_RTD, const cob_field *f, char *s) {
    if (f) {
        memcpy(s, f->data, f->size);
        cob_string_to_C(rtd, s, f->size);
    } else {
        *s=0;
    }
}

void
cob_field_to_string_1 (COB_RTD, const cob_field *f, char *s, int s_size) {

    if (f) {
        int sz = min(f->size, s_size) - 1;
        memcpy (s, f->data, sz);
        cob_string_to_C(rtd, s, sz);
    } else {
        *s=0;
    }
}

/*
 * Switch
 */

int
cob_get_switch (COB_RTD, const int n) {
    return(rtd->cob_switch[n]);
}

void
cob_set_switch (COB_RTD, const int n, const int flag) {
    if ( flag == 0 ) {
        rtd->cob_switch[n] = 0;
    } else if ( flag == 1 ) {
        rtd->cob_switch[n] = 1;
    }
}



static int 
cob_get_same_char (COB_RTD, cob_field * f1)
{
    const unsigned char     *p = f1->data;
    size_t                  inc = f1->size;
    int first  = *p;

    p++;
    inc--;
    while ( inc ) {
        if ( *p != first ) {
            return -1;
        }
        p++;
        inc--;
    }
    return first;

}

static int 
cob_cmp_1(COB_RTD, cob_field * f1, cob_field * f2, int isf1num) {
    if ( f2 == &cob_zero && isf1num ) {
        return(cob_cmp_int (rtd, f1, 0));
    } else if (isf1num  && f2->size == 1 &&
               rtd->current_module->runtime_flags & COB_FLAG_RT_MF_HOSTNUMCOMPARE_1) {
        if (COB_FIELD_TYPE (f1) == COB_TYPE_NUMERIC_DISPLAY &&
            COB_FIELD_SCALE(f1) == 0) {
            int c = cob_get_same_char(rtd,f1);
            if ( c >=0 ) {
                /*
                if ( c==CHAR_0 && (f2 == &cob_low)  ) {
                    return 0;
                } 
                */ 
                return f2->data[0] - c;
            } else {
                return(cob_cmp_char (rtd, f1, f2->data[0]));
            }
        } else {
            if (f2 == &cob_space) {
                return -1;
            } else if (f2 == &cob_low) {
                return 1;
            }
            return(cob_cmp_char (rtd, f1, f2->data[0]));
        }
    } else if ( f2->size == 1 ) {
        if ( (rtd->current_module->runtime_flags & COB_FLAG_RT_MF_SPZERO ) && 
             (f2->data[0] == CHAR_SP) && isf1num ) {
            return(-1);
        } else {
            return(cob_cmp_char (rtd, f1, f2->data[0]));
        }
    } else {
        return(cob_cmp_all (rtd, f1, f2));
    }
}

int
cob_cmp (COB_RTD, cob_field *f1, cob_field *f2) {
    cob_field       temp;
    cob_field_attr  attr;
    unsigned char   buff[48];
    int isf1num=COB_FIELD_IS_NUMERIC (f1);
    int isf2num=COB_FIELD_IS_NUMERIC (f2);

    if ( isf1num && isf2num ) {
        return(cob_numeric_cmp (rtd, f1, f2));
    }
    if ( COB_FIELD_TYPE (f2) == COB_TYPE_ALPHANUMERIC_ALL ) {
        return cob_cmp_1(rtd, f1, f2, isf1num);
    } else if ( COB_FIELD_TYPE (f1) == COB_TYPE_ALPHANUMERIC_ALL ) {
        return -cob_cmp_1(rtd, f2, f1, isf2num);
    }
    if ( isf1num
         && ((COB_FIELD_TYPE (f1) != COB_TYPE_NUMERIC_DISPLAY) || 
             ((rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) && COB_FIELD_HAVE_SIGN(f1))) ) {
        temp.size = COB_FIELD_DIGITS(f1);
        temp.data = buff;
        temp.attr = &attr;
        attr = *f1->attr;
        attr.type = COB_TYPE_NUMERIC_DISPLAY;
        attr.flags &= ~COB_FLAG_HAVE_SIGN;
        if ( COB_FIELD_IS_PACKED(f1) ) {
            attr.flags &= ~COB_FLAG_PACKED_SIGN_MISSING;
        }
        cob_move (rtd, f1, &temp);
        f1 = &temp;
    }
    if ( isf2num
         && ((COB_FIELD_TYPE (f2) != COB_TYPE_NUMERIC_DISPLAY) || 
             ((rtd->current_module->runtime_flags & COB_FLAG_RT_DISPLAY_MF50) && COB_FIELD_HAVE_SIGN(f2))) ) {
        temp.size = COB_FIELD_DIGITS(f2);
        temp.data = buff;
        temp.attr = &attr;
        attr = *f2->attr;
        attr.type = COB_TYPE_NUMERIC_DISPLAY;
        attr.flags &= ~COB_FLAG_HAVE_SIGN;
        if ( COB_FIELD_IS_PACKED(f2) ) {
            attr.flags &= ~COB_FLAG_PACKED_SIGN_MISSING;
        }
        cob_move (rtd, f2, &temp);
        f2 = &temp;
    }
    if ( COB_FIELD_IS_NATIONAL(f1) || COB_FIELD_IS_NATIONAL(f2) ) {
        return cob_cmp_national(rtd, COB_FIELD_IS_NATIONAL(f1) ? f1 : cob_intr_national_of(rtd, 1, f1), 
                                COB_FIELD_IS_NATIONAL(f2) ? f2 : cob_intr_national_of(rtd, 1, f2));
    }
    return(cob_cmp_alnum (rtd, f1, f2));
}

/*
 * Class check
 */

int
cob_is_omitted (COB_RTD, const cob_field *f) {
    return(f->data == NULL);
}

static int
cob_is_numeric_1 (COB_RTD, cob_field *f, int check_spzero) {
    unsigned char   *data;
    size_t          size;
    size_t          i;
    int             sign;

    switch ( COB_FIELD_TYPE (f) ) {
        case COB_TYPE_NUMERIC_BINARY:
        case COB_TYPE_NUMERIC_BITS:
        case COB_TYPE_NUMERIC_FLOAT:
        case COB_TYPE_NUMERIC_DOUBLE:
            return(1);
        case COB_TYPE_NUMERIC_PACKED:
            /* check digits */
            for ( i = 0; i < f->size - 1; ++i ) {
                if ( (f->data[i] & 0xf0) > 0x90 || (f->data[i] & 0x0f) > 0x09 ) {
                    return(0);
                }
            }
            if ( (f->data[i] & 0xf0) > 0x90 ) {
                return(0);
            }
            if ( COB_FIELD_PACKED_SIGN_MISSING(f) ) {
                if ( (f->data[i] & 0x0f) > 0x09 ) {
                    return(0);
                }
                return(1);
            } else {
                /* check sign */
                sign = f->data[i] & 0x0f;
                if ( sign == 0x0f || sign == 0x0c ) {
                    return(1);
                }
                if ( COB_FIELD_HAVE_SIGN (f) ) {
                    if ( sign == 0x0d ) {
                        return(1);
                    }
                }
            }
            return(0);
        case COB_TYPE_NUMERIC_DISPLAY:
            if ( !cob_test_sign(rtd, f) ) {
                return(0);
            }
            sign = COB_DUP_GET_SIGN (f);
            size = COB_FIELD_SIZE (f);
            data = COB_FIELD_DATA (f);
            for ( i = 0; i < size; ++i ) {
                if ( !cob_isdigit_char (rtd,data[i]) ) {

                    if (!cob_iszero_char(rtd,f,data[i], check_spzero)) {
                        return(0);
                    }
                }
            }
            return(f->size > 0 ? 1 : 0);
        case COB_TYPE_NATIONAL:
        case COB_TYPE_NATIONAL_EDITED:
            return cob_is_numeric(rtd, cob_intr_display_of(rtd, 1, f));
            break;
        case COB_TYPE_BITS:
            return 1;
            break;
        default:
            for ( i = 0; i < f->size; ++i ) {
                if ( !cob_isdigit_char (rtd, f->data[i]) ) {
                    return(0);
                }
            }
            return(f->size > 0 ? 1 : 0);
    }
}

int
cob_is_numeric (COB_RTD, cob_field *f) {
    return cob_is_numeric_1(rtd, f, 0);
}

int
cob_is_alpha (COB_RTD, const cob_field *f) {
    size_t  i;

    for ( i = 0; i < f->size; ++i ) {
        unsigned char c= E2A(f->data[i]);
        if ( !isspace(c) && !isalpha (c) ) {
            return(0);
        }
    }
    return(1);
}

int
cob_is_upper (COB_RTD, const cob_field *f) {
    size_t  i;

    for ( i = 0; i < f->size; ++i ) {
        unsigned char c= E2A(f->data[i]);
        if ( !isspace (c) && !isupper (c) ) {
            return(0);
        }
    }
    return(1);
}

int
cob_is_lower (COB_RTD, const cob_field *f) {
    size_t  i;

    for ( i = 0; i < f->size; ++i ) {
        unsigned char c= E2A(f->data[i]);
        if ( !isspace(c) && !islower (c) ) {
            return(0);
        }
    }
    return(1);
}

int
cob_is_success (COB_RTD, const cob_field *f) {
    return (cob_get_int(rtd,f) & 1);
}

int
cob_is_failure (COB_RTD, const cob_field *f) {
    return (!cob_is_success(rtd,f));
}


/*
 * Table sort
 */

void
cob_table_sort_init (COB_RTD, const int nkeys, const unsigned char *collating_sequence) {
    rtd->sort_nkeys = 0;
    rtd->sort_keys = cob_malloc (rtd, nkeys * sizeof (struct cob_file_key));
    if ( collating_sequence ) {
        rtd->sort_collate = collating_sequence;
    } else {
        rtd->sort_collate = rtd->current_module->collating_sequence;
    }
}

void
cob_table_sort_init_key (COB_RTD, const int flag, cob_field *field, size_t offset) {
    rtd->sort_keys[rtd->sort_nkeys].flag = flag;
    rtd->sort_keys[rtd->sort_nkeys].field = field;
    rtd->sort_keys[rtd->sort_nkeys].offset = offset;
    rtd->sort_nkeys++;
}

void
cob_table_sort (COB_RTD, cob_field *f, const int n) {
    cob_qsort (f->data, (size_t) n, f->size, sort_compare, rtd);
    cob_free (rtd->sort_keys);
}

/*
 * Run-time error checking
 */

void
cob_check_based (COB_RTD, const unsigned char *x, const char *name) {
    if ( !x ) {
        cob_runtime_error (rtd, "BASED/LINKAGE item '%s' has NULL address", name);
        cob_stop_abend (rtd, COBRE_BASED_NULL);
    }
}

static int 
cob_check_stop_abend(COB_RTD){
    char *p = cob_bool_getenv ("COB_RUNTIME_CHECK_TRACE");
    if (rtd->current_module && rtd->current_module->no_cbl_error_proc) {
        return 0;
    }
    if (p) {
        if ((*p == 'Y' || *p == '1' || *p=='y') && *(p+1) == 0) {
            return 0;
        }
        if (rtd->current_module) {
            char *save=NULL;
            char *c = strdup(p);
            char *w;
            w = COB_STRTOK (c, ";:", &save);
            do {
                if (strcasecmp(w, rtd->current_module->module_name) == 0) {
                    free(c);
                    return 0;
                }
                w = COB_STRTOK (NULL, ";:", &save);
            }while (w);
            free(c);
        }
    }
    return 1;
}


void
cob_check_numeric (COB_RTD, cob_field *f, const char *name) {
    size_t          i;
    unsigned char   *data;
    unsigned char   *p;
    unsigned char   c;
    unsigned char   buff[COB_SMALL_BUFF];

    if ( !cob_is_numeric_1 (rtd, f, 1) ) {
        p = buff;
        data = f->data;
        for ( i = 0; i < f->size; i++ ) {
            c = E2A(data[i]);
            if ( isprint (c) ) {
                *p++ = c;
            } else {
                p += sprintf ((char*)p, "\\%03o", c);
            }
        }
        *p = '\0';
        cob_runtime_error (rtd, "'%s' not numeric: '%s'", name, buff);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_NOT_NUMERIC);
    }
}

void
cob_check_odo (COB_RTD, const int i, const int minv, const int maxv, const char *name) {
    /* check the OCCURS DEPENDING ON item */
    if ( i < minv || maxv < i ) {
        cob_set_exception (rtd, COB_EC_BOUND_ODO);
        cob_runtime_error (rtd, "OCCURS DEPENDING ON '%s' out of bounds: %d", name, i);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_OCCUR_DEP);
    }
}

int
cob_check_odo_1 (COB_RTD, const int currentv, const int odoval, const int minodo, const int maxodo, 
                          const char *odoname, const char *valname) {
    /* check the OCCURS DEPENDING ON item */
    if ( odoval < minodo || maxodo < odoval ) {
        cob_set_exception (rtd, COB_EC_BOUND_ODO);
        cob_runtime_error (rtd, "OCCURS DEPENDING ON '%s' out of bounds: %d", odoname, odoval);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_OCCUR_DEP);
    }
    /* check the subscript */
    if (currentv < 1 || odoval < currentv) {
        cob_set_exception (rtd, COB_EC_BOUND_SUBSCRIPT);
        cob_runtime_error(rtd, "Subscript of '%s' out of bounds: %d", valname, currentv);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_SUBSCRIPT);
    }
    return currentv;
}

void
cob_check_subscript (COB_RTD, const int i, const int minv, const int maxv, const char *name) {
    /* check the subscript */
    if ( i < minv || maxv < i ) {
        cob_set_exception (rtd, COB_EC_BOUND_SUBSCRIPT);
        cob_runtime_error (rtd, "Subscript of '%s' out of bounds: %d", name, i);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_SUBSCRIPT);
    }
}

int
cob_check_subscript_1 (COB_RTD, const int i, const int minv, const int maxv, const char *name) {
    /* check the subscript */
    if ( i < minv || maxv < i ) {
        cob_set_exception (rtd, COB_EC_BOUND_SUBSCRIPT);
        cob_runtime_error (rtd, "Subscript of '%s' out of bounds: %d", name, i);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_SUBSCRIPT);
    }
    return i;
}

void
cob_check_ref_mod (COB_RTD, const int offset, const int length, const int size, const char *name) {
    /* check the offset */
    if ( offset < 1 || offset > size ) {
        cob_set_exception (rtd, COB_EC_BOUND_REF_MOD);
        cob_runtime_error (rtd, "Offset of '%s' out of bounds: %d", name, offset);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_OFFSET);
    }

    /* check the length */
    if ( length < 1 || offset + length - 1 > size ) {
        cob_set_exception (rtd, COB_EC_BOUND_REF_MOD);
        cob_runtime_error (rtd, "Length of '%s' out of bounds: %d", name, length);
        if (cob_check_stop_abend (rtd))
            cob_stop_abend (rtd, COBRE_LENGTH);
    }
}

unsigned char *
cob_external_addr (COB_RTD, const char *exname, const int exlength) {
    struct cob_external *eptr;
    for ( eptr = rtd->cob_baseexternal; eptr; eptr = eptr->next ) {
        if ( !strcmp (exname, eptr->ename) ) {
            if ( exlength > eptr->esize ) {
                cob_runtime_error (rtd, "EXTERNAL item '%s' has size > %d",
                                   exname, exlength);
                cob_stop_abend (rtd, COBRE_EXTERNAL);
            }
            rtd->cob_initial_external = 0;
            return(ucharptr)eptr->ext_alloc;
        }
    }
    eptr = (struct cob_external *) cob_malloc (rtd, sizeof (struct cob_external));
    eptr->next = rtd->cob_baseexternal;
    eptr->esize = exlength;
    eptr->ename = strdup(exname);
    eptr->ext_alloc = cob_malloc (rtd, (size_t)exlength);
    rtd->cob_baseexternal = eptr;
    rtd->cob_initial_external = 1;
    return(ucharptr)eptr->ext_alloc;
}

unsigned char *
cob_external_addr_from_fld (COB_RTD, cob_field *f, const int exlength) {

    if ( f->size < COB_SMALL_BUFF  ) {
        char buff[COB_SMALL_BUFF];
        cob_field_to_string(rtd, f, buff);
        return cob_external_addr(rtd, buff, exlength);
    } else {
        char *p =  cob_malloc(rtd, f->size+10);
        unsigned char *res;
        cob_field_to_string(rtd, f, p);
        res= cob_external_addr(rtd, p, exlength);
        cob_free(p);
        return res;

    }
}

unsigned char *
cob_external_addr_wstatus (COB_RTD, const char *exname, const int exlength, int *init_status) {
    unsigned char * p = cob_external_addr(rtd,exname,exlength);
    if ( init_status ) {
        *init_status = rtd->cob_initial_external;
    }
    return p;
}

char *
cob_str_cur_timestamp () {
    static char ts[20];
    time_t t = time(NULL);
    strftime(ts, sizeof(ts),  "%Y-%m-%d %H:%M:%S", localtime(&t));
    return ts;
}

/* Extended ACCEPT/DISPLAY */
struct tm *
cob_localtime (time_t  *t) {

    struct tm *tmstruct = localtime (t);

    struct tm exptm;
    static char *s = NULL;
    static int date_set = 0;

    if ( !date_set ) {
        s = getenv("COB_CURRENT_DATE");
        date_set = 1;
    }
    if ( s ) {
        if ( cob_strptime(s, "%Y-%m-%d", &exptm) != NULL ) {
            time_t  nt; 

            exptm.tm_sec  = tmstruct->tm_sec ;
            exptm.tm_min  = tmstruct->tm_min ;
            exptm.tm_hour = tmstruct->tm_hour ;          
            nt =  mktime(&exptm);
            tmstruct = localtime (&nt);

        }
    }
    return tmstruct;
}


void
cob_accept_date (COB_RTD, cob_field *f) {
    time_t  t = time (NULL);
    unsigned char  s[8];

    strftime ((char*)s, 7, "%y%m%d", cob_localtime (&t));
    (void)STRING_A2E(s,6);
    cob_memcpy (rtd, f, (ucharptr)s, 6);
}

void
cob_accept_date_yyyymmdd (COB_RTD, cob_field *f) {
    time_t  t = time (NULL);
    unsigned char    s[12];

    strftime ((char*)s, 9, "%Y%m%d", cob_localtime (&t));
    (void)STRING_A2E(s,8);
    cob_memcpy (rtd, f, (ucharptr)s, 8);
}

void
cob_accept_day (COB_RTD, cob_field *f) {
    time_t  t = time (NULL);
    unsigned char    s[8];

    strftime ((char*)s, 6, "%y%j", cob_localtime (&t));
    (void)STRING_A2E(s,5);
    cob_memcpy (rtd, f, (ucharptr)s, 5);
}

void
cob_accept_day_yyyyddd (COB_RTD, cob_field *f) {
    time_t  t = time (NULL);
    unsigned char    s[12];

    strftime ((char*)s, 8, "%Y%j", cob_localtime (&t));
    (void)STRING_A2E(s,7);
    cob_memcpy (rtd, f, (ucharptr)s, 7);
}

void
cob_accept_day_of_week (COB_RTD, cob_field *f) {
    time_t  t = time (NULL);
    unsigned char  s[4];

#if defined(_MSC_VER)
    /*sprintf(s, "%d", cob_localtime(&t)->tm_wday + 1);*/
    s[0] = (char)(cob_localtime(&t)->tm_wday);
    if (s[0] == 0) s[0] = 7;
    s[0] += CHAR_0;
#else
    strftime ((char*)s, 2, "%u", cob_localtime (&t));
    (void)STRING_A2E(s,strlen((char*)s));
#endif
    cob_memcpy (rtd, f, (ucharptr)s, 1);
}

void
cob_accept_time (COB_RTD, cob_field *f) {
#ifdef _WIN32
    SYSTEMTIME  syst;
#else
    time_t      t;
#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)
    struct timeval  tmv;
    char        buff2[8];
#endif
#endif
    unsigned char s[12];

#ifdef _WIN32
    GetLocalTime (&syst);
    sprintf (s, "%2.2d%2.2d%2.2d%2.2d", syst.wHour, syst.wMinute,
             syst.wSecond, syst.wMilliseconds / 10);
#else
#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)
    gettimeofday (&tmv, NULL);
    t = tmv.tv_sec;
#else
    t = time (NULL);
#endif
    strftime ((char*)s, 9, "%H%M%S00", localtime (&t));
#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)
    sprintf(buff2, "%2.2ld", (long int)(tmv.tv_usec / 10000));
    memcpy (&s[6], buff2, 2);
#endif
#endif
    (void)STRING_A2E(s,8);
    cob_memcpy (rtd, f, (ucharptr)s, 8);
}

void
cob_display_command_line (COB_RTD, cob_field *f) {
    if ( rtd->commlnptr ) {
        free (rtd->commlnptr);
    }
    rtd->commlnptr = cob_malloc (rtd, f->size);
    rtd->commlncnt = f->size;
    memcpy (rtd->commlnptr, f->data, rtd->commlncnt);
}

void
cob_accept_command_line (COB_RTD, cob_field *f) {
    int  i, size = 0;
    int  len;
    char    buff[COB_MEDIUM_BUFF] = "";

    if ( rtd->commlncnt ) {
        cob_memcpy (rtd, f, rtd->commlnptr, (int)rtd->commlncnt);
        return;
    }
    for ( i = 1; i < rtd->cob_argc; i++ ) {
        len = strlen (rtd->cob_argv[i]);
        if ( size + len >= COB_MEDIUM_BUFF ) {
            /* overflow */
            break;
        }
        memcpy (buff + size, rtd->cob_argv[i], len);
        size += len;
        if (i+1 < rtd->cob_argc) {
            buff[size++] = CHAR_SP;
        }
    }
    cob_memcpy (rtd, f, (ucharptr)buff, (int)size);
}

static char * cob_linkage_command_line_data = NULL;

char *
cob_linkage_command_line (COB_RTD) {
    size_t  i;
    short   len, sl;
    char    *p;

    if ( !cob_linkage_command_line_data ) {
        len = 0;
        for ( i = 1; i < (size_t)rtd->cob_argc; i++ ) {
            len += strlen (rtd->cob_argv[i]) +1 ;
        }
        cob_linkage_command_line_data = cob_malloc(rtd, len+sizeof(short));
        p = cob_linkage_command_line_data;
        memset (p, CHAR_SP, len+sizeof(short));
        /* forget last 0*/
        len --;
#ifndef WORDS_BIGENDIAN
        sl = COB_BSWAP_16(len);
#else
        sl = len;
#endif
        memcpy (p, &sl, sizeof(short));
        p += sizeof(short);
        for ( i = 1; i < (size_t)rtd->cob_argc; i++ ) {
            len = strlen (rtd->cob_argv[i]);
            memcpy (p, rtd->cob_argv[i], len);
            p += len +1;
        }
    }
    return(cob_linkage_command_line_data);
}

/*
 * Argument number
 */

void
cob_display_arg_number (COB_RTD, cob_field *f) {
    int             n;
    cob_field_attr  attr;
    cob_field       temp;

    temp.size = 4;
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 9, 0, 0, NULL);
    cob_move (rtd, f, &temp);
    if ( n < 0 || n >= rtd->cob_argc ) {
        cob_set_exception (rtd, COB_EC_IMP_DISPLAY);
        return;
    }
    rtd->current_arg = n;
}

void
cob_accept_arg_number (COB_RTD, cob_field *f) {
    int             n = rtd->cob_argc - 1;
    cob_field_attr  attr;
    cob_field       temp;

    temp.size = 4;
    temp.data = (unsigned char *)&n;
    temp.attr = &attr;
    COB_ATTR_INIT (COB_TYPE_NUMERIC_BINARY, 9, 0, 0, NULL);
    cob_move (rtd, &temp, f);
}

void
cob_accept_arg_value (COB_RTD, cob_field *f) {
    if ( rtd->current_arg >= rtd->cob_argc ) {
        cob_set_exception (rtd, COB_EC_IMP_ACCEPT);
        return;
    }
    cob_memcpy (rtd, f, (ucharptr)rtd->cob_argv[rtd->current_arg], (int) strlen (rtd->cob_argv[rtd->current_arg]));
    rtd->current_arg++;
}

/*
 * Environment variable
 */

void
cob_display_environment (COB_RTD, cob_field *f) {
    if ( !rtd->cob_local_env ) {
        rtd->cob_local_env = cob_malloc (rtd, COB_SMALL_BUFF);
    }
    if ( f->size > COB_SMALL_MAX ) {
        cob_set_exception (rtd, COB_EC_IMP_DISPLAY);
        return;
    }
    cob_field_to_string (rtd, f, rtd->cob_local_env);
}

void
cob_display_env_value (COB_RTD, cob_field *f) {
    char    *p;
    char    *env2;
    size_t  len;

    if ( !rtd->cob_local_env ) {
        cob_set_exception (rtd, COB_EC_IMP_DISPLAY);
        return;
    }
    if ( !*(rtd->cob_local_env) ) {
        cob_set_exception (rtd, COB_EC_IMP_DISPLAY);
        return;
    }
    env2 = cob_malloc (rtd, f->size + 1);
    cob_field_to_string (rtd, f, env2);
    len = strlen (rtd->cob_local_env) + strlen (env2) + 3;
    p = cob_malloc (rtd, len);
    sprintf (p, "%s=%s", rtd->cob_local_env, env2);
    if ( putenv (p) != 0 ) {
        cob_set_exception (rtd, COB_EC_IMP_DISPLAY);
    }
    free (env2);
}

void
cob_set_environment (COB_RTD, cob_field *f1, cob_field *f2) {
    cob_display_environment (rtd, f1);
    cob_display_env_value (rtd, f2);
}

void
cob_get_environment (COB_RTD, cob_field *envname, cob_field *envval) {
    const char  *p;
    char        *buff;

    if ( envname->size < COB_SMALL_BUFF ) {
        buff = cob_malloc (rtd, COB_SMALL_BUFF);
        cob_field_to_string (rtd, envname, buff);
        p = getenv (buff);
        if ( !p ) {
            cob_set_exception (rtd, COB_EC_IMP_ACCEPT);
            p = " ";
        }
        cob_memcpy (rtd, envval, (ucharptr)p, (int) strlen (p));
        free (buff);
    } else {
        cob_set_exception (rtd, COB_EC_IMP_ACCEPT);
        p = " ";
        cob_memcpy (rtd, envval, (ucharptr)p, (int) strlen (p));
    }
}

void
cob_accept_environment (COB_RTD, cob_field *f) {
    const char *p = NULL;

    if ( rtd->cob_local_env ) {
        p = getenv (rtd->cob_local_env);
    }
    if ( !p ) {
        cob_set_exception (rtd, COB_EC_IMP_ACCEPT);
        p = " ";
    }
    cob_memcpy (rtd, f, (ucharptr)p, (int) strlen (p));
}

void
cob_chain_setup (COB_RTD, void *data, const size_t parm, const size_t size) {
    size_t  len;

    memset (data, CHAR_SP, size);
    if ( parm <= (size_t)rtd->cob_argc - 1 ) {
        len = strlen (rtd->cob_argv[parm]);
        if ( len <= size ) {
            memcpy (data, rtd->cob_argv[parm], len);
        } else {
            memcpy (data, rtd->cob_argv[parm], size);
        }
    } else {
        memset (data, CHAR_SP, size);
    }
    rtd->cob_call_params = rtd->cob_argc - 1;
}

void
cob_allocate (COB_RTD, unsigned char **dataptr, cob_field *retptr, cob_field *sizefld) {
    void                    *mptr = NULL;
    struct cob_alloc_cache  *cache_ptr;
    int                     fsize;

    rtd->cob_exception_code = 0;
    fsize = cob_get_int (rtd, sizefld);
    if ( fsize > 0 ) {
        cache_ptr = cob_malloc (rtd, sizeof (struct cob_alloc_cache));
        mptr = calloc (1, (size_t)fsize);
        if ( !mptr ) {
            cob_set_exception (rtd, COB_EC_STORAGE_NOT_AVAIL);
            free (cache_ptr);
        } else {
            cache_ptr->cob_pointer = mptr;
            cache_ptr->size = (size_t)fsize;
            cache_ptr->next = rtd->cob_alloc_base;
            rtd->cob_alloc_base = cache_ptr;
        }
    }
    if ( dataptr ) {
        *dataptr = (unsigned char *)mptr;
    }
    if ( retptr ) {
        *(void **)(retptr->data) = mptr;
    }
}

void
cob_free_alloc (COB_RTD, unsigned char **ptr1, unsigned char *ptr2) {
    struct cob_alloc_cache  *cache_ptr;

    rtd->cob_exception_code = 0;
    if ( ptr1 && *ptr1 ) {
        for ( cache_ptr = rtd->cob_alloc_base; cache_ptr; cache_ptr= cache_ptr->next ) {
            if ( *(void **)ptr1 == cache_ptr->cob_pointer ) {
                cache_ptr->cob_pointer = NULL;
                free (*ptr1);
                *ptr1 = NULL;
                return;
            }
        }
        cob_set_exception (rtd, COB_EC_STORAGE_NOT_ALLOC);
        return;
    }
    if ( ptr2 && *(void **)ptr2 ) {
        for ( cache_ptr = rtd->cob_alloc_base; cache_ptr; cache_ptr= cache_ptr->next ) {
            if ( *(void **)ptr2 == cache_ptr->cob_pointer ) {
                cache_ptr->cob_pointer = NULL;
                free (*(void **)ptr2);
                *(void **)ptr2 = NULL;
                return;
            }
        }
        cob_set_exception (rtd, COB_EC_STORAGE_NOT_ALLOC);
        return;
    }
}

char *
cobgetenv (const char *name) {
    if ( name ) {
        return(getenv (name));
    }
    return(NULL);
}

int
cobputenv (char *name) {
    int res = -1;
#if (defined(__hpux__)  || defined(__hpux))
    res = putenv(strdup(name));
#else
    if ( name ) {
        char *saveptr=NULL;
        char *p = strdup(name);
        char *var, *val;
        var = COB_STRTOK(p,"=", &saveptr);
        if (var) {
            val = COB_STRTOK(NULL,"", &saveptr);
#ifdef _WIN32 
            res = SetEnvironmentVariable(var, val);
#else 
            res = setenv(var, val, 1);
#endif
        }
        free(p);
    }
#endif
    return(res);
}

int
cobinit (void) {
    cob_init (cob_get_rtd(), 0, NULL);
    return(0);
}

void *
cobcommandline (int flags, int *pargc, char ***pargv, char ***penvp, char **pname) {
    char    **spenvp;
    char    *spname;
    int     sflags;
    COB_RTD = cob_get_rtd();


    if ( !rtd->cob_initialized ) {
        cob_runtime_error (rtd, "'cobcommandline' - Runtime has not been initialized");
        cob_stop_abend (rtd, COBRE_CMD_LINE);
    }
    if ( pargc && pargv ) {
        rtd->cob_argc = *pargc;
        rtd->cob_argv = *pargv;
    }
    /* Shut up the compiler */
    sflags = flags;
    if ( penvp ) {
        spenvp = *penvp;
    }
    if ( pname ) {
        spname = *pname;
    }
    /* What are we supposed to return here? */
    return(NULL);
}

void
cobexit (const int status) {
    cob_stop_run (cob_get_rtd(), status);
}


/* System routines */

int
CBL_EXIT_PROC (unsigned char *x, unsigned char *pptr) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_EXIT_PROC(rtd, x, pptr);
}

int
rtd_CBL_EXIT_PROC (COB_RTD, unsigned char *x, unsigned char *pptr) {
    struct exit_handlerlist *hp = NULL;
    struct exit_handlerlist *h = rtd->exit_hdlrs;
    int         (**p)(void) = NULL;

    COB_CHK_PARMS (CBL_EXIT_PROC, 2);

    memcpy (&p, &pptr, sizeof (void *));
    if ( !p || !*p ) {
        return(-1);
    }
    /* remove handler anyway */
    while ( h != NULL ) {
        if ( h->proc == *p ) {
            if ( hp != NULL ) {
                hp->next = h->next;
            } else {
                rtd->exit_hdlrs = h->next;
            }
            cob_free (h);
            break;
        }
        hp = h;
        h = h->next;
    }
    if ( *x != 0 && *x != 2 && *x != 3 ) {  /* remove handler */
        return(0);
    }
    h = cob_malloc (rtd, sizeof(struct exit_handlerlist));
    h->next = rtd->exit_hdlrs;
    h->proc = *p;
    rtd->exit_hdlrs = h;
    return(0);
}

static void 
cob_add_errormsg_proc_1 (COB_RTD, void* proc)
{
    struct handlerlist  *h;
    if ( proc ) {
        h = cob_malloc (rtd, sizeof(struct handlerlist));
        h->next = rtd->hdlrs;
        h->proc_cob = proc;
        rtd->hdlrs = h;
    }
}

void 
cob_set_errormsg_proc (COB_RTD, cob_errmsg_proc proc)
{
    rtd->cob_errormessage_proc = proc;
}

void 
cob_set_errorfile (COB_RTD, FILE* errfile)
{
    rtd->cob_err_file =  errfile;
}


int
CBL_ERROR_PROC (unsigned char *x, unsigned char *pptr) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_ERROR_PROC(rtd, x, pptr);
}

int
rtd_CBL_ERROR_PROC (COB_RTD, unsigned char *x, unsigned char *pptr) {
    struct handlerlist  *hp = NULL;
    struct handlerlist  *h = rtd->hdlrs;
    int         (**p)(char *s) = NULL;

    memcpy (&p, &pptr, sizeof (void *));
    if ( !p || !*p ) {
        return(-1);
    }
    /* remove handler anyway */
    while ( h != NULL ) {
        if ( h->proc_cob == *p ) {
            if ( hp != NULL ) {
                hp->next = h->next;
            } else {
                rtd->hdlrs = h->next;
            }
            cob_free (h);
            break;
        }
        hp = h;
        h = h->next;
    }
    if ( *x != 0 ) {  /* remove handler */
        return(0);
    }
    cob_add_errormsg_proc_1(rtd, *p);
    return(0);
}

int
CBL_FFND_REPORT (int rtserr, int terminate) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_FFND_REPORT(rtd, rtserr, terminate);
}

int 
rtd_CBL_FFND_REPORT (COB_RTD, int rtserr, int terminate)
{
    cob_stop_abend (rtd, rtserr);
    return 0;
}

int CBL_DEBUGBREAK (void)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_DEBUGBREAK(rtd);
}

int rtd_CBL_DEBUGBREAK (COB_RTD)
{
    return cob_runtime_debugger_activate(rtd, 0);
}

int
SYSTEM (const unsigned char *cmd) {
    COB_RTD = cob_get_rtd();
    return rtd_SYSTEM(rtd, cmd);
}

int
rtd_SYSTEM (COB_RTD, const unsigned char *cmd) {
    unsigned char    *buff;
    int i;
    unsigned char sp = CHAR_SP;

    COB_CHK_PARMS (SYSTEM, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        i = (int)rtd->current_module->cob_procedure_parameters[0]->size;
        if ( i > COB_MEDIUM_MAX ) {
            cob_runtime_error (rtd, "Parameter to SYSTEM call is larger than 8192 characters");
            cob_stop_abend (rtd, COBRE_SYSTEM);
        }
        i--;
        for ( ; i >= 0; i-- ) {
            if ( cmd[i] != sp && cmd[i] != 0 ) {
                break;
            }
        }
        if ( i >= 0 ) {
            buff = cob_malloc (rtd, (size_t)(i + 2));
            memcpy (buff, cmd, (size_t)(i + 1));
            if ( (rtd->cob_screen_initialized) ) {
                cob_screen_set_mode (rtd, 0);
            }
            i = system ((char*)STRING_E2A(buff, strlen((char*)buff)));
            free (buff);
            if ( (rtd->cob_screen_initialized) ) {
                cob_screen_set_mode (rtd, 1);
            }
            return(i);
        }
    }
    return(1);
}

int
CBL_AND (unsigned char *data_1, unsigned char *data_2, const int length) {
    COB_RTD =cob_get_rtd();
    return rtd_CBL_AND (rtd, data_1,data_2, length);
}

int
rtd_CBL_AND (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_AND, 3);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_2[n] &= data_1[n];
    }
    return(0);
}

int
CBL_OR (unsigned char *data_1, unsigned char *data_2, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_OR(rtd, data_1, data_2, length);
}

int
rtd_CBL_OR (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_OR, 3);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_2[n] |= data_1[n];
    }
    return(0);
}

int
CBL_NOR (unsigned char *data_1, unsigned char *data_2, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_NOR(rtd, data_1, data_2, length);
}

int
rtd_CBL_NOR (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_NOR, 3);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_2[n] = ~(data_1[n] | data_2[n]);
    }
    return(0);
}

int
CBL_XOR (unsigned char *data_1, unsigned char *data_2, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_XOR(rtd, data_1, data_2, length);
}

int
rtd_CBL_XOR (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_XOR, 3);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_2[n] ^= data_1[n];
    }
    return(0);
}

int
CBL_IMP (unsigned char *data_1, unsigned char *data_2, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_IMP(rtd, data_1, data_2, length);
}

int
rtd_CBL_IMP (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_IMP, 3);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_2[n] = (~data_1[n]) | data_2[n];
    }
    return(0);
}

int
CBL_NIMP (unsigned char *data_1, unsigned char *data_2, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_NIMP(rtd, data_1, data_2, length);
}

int
rtd_CBL_NIMP (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_NIMP, 3);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_2[n] = data_1[n] & (~data_2[n]);
    }
    return(0);
}

int
CBL_EQ (unsigned char *data_1, unsigned char *data_2, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_EQ(rtd, data_1, data_2, length);
}

int
rtd_CBL_EQ (COB_RTD, unsigned char *data_1, unsigned char *data_2, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_EQ, 3);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_2[n] = ~(data_1[n] ^ data_2[n]);
    }
    return(0);
}

int
CBL_NOT (unsigned char *data_1, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_NOT(rtd, data_1, length);
}

int
rtd_CBL_NOT (COB_RTD, unsigned char *data_1, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_NOT, 2);

    if ( length <= 0 ) {
        return(0);
    }
    for ( n = 0; n < (size_t)length; ++n ) {
        data_1[n] = ~data_1[n];
    }
    return(0);
}

int
CBL_XF4 (unsigned char *data_1, unsigned char *data_2) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_XF4(rtd, data_1, data_2);
}

int
rtd_CBL_XF4 (COB_RTD, unsigned char *data_1, unsigned char *data_2) {
    size_t  n;

    COB_CHK_PARMS (CBL_XF4, 2);

    *data_1 = 0;
    for ( n = 0; n < 8; ++n ) {
        *data_1 |= (data_2[n] ? 1 : 0) << (7 - n);
    }
    return(0);
}

int
CBL_XF5 (unsigned char *data_1, unsigned char *data_2) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_XF5(rtd, data_1, data_2);
}

int
rtd_CBL_XF5 (COB_RTD, unsigned char *data_1, unsigned char *data_2) {
    size_t  n;

    COB_CHK_PARMS (CBL_XF5, 2);

    for ( n = 0; n < 8; ++n ) {
        data_2[n] = (*data_1 & (1 << (7 - n))) ? 0xFF : 0;
    }
    return(0);
}

int
CBL_X91 (unsigned char *result, const unsigned char *func, unsigned char *parm) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_X91(rtd, result, func, parm);
}

int
rtd_CBL_X91 (COB_RTD, unsigned char *result, const unsigned char *func, unsigned char *parm) {
    unsigned char   *p;
    size_t          i, j, len;
    char            buffer[257];

    COB_CHK_PARMS (CBL_X91, 3);
    switch ( *func ) {
        case 11:
            /* Set switches */
            p = parm;
            for ( i = 0; i < 8; ++i, ++p ) {
                if ( *p == 0 ) {
                    rtd->cob_switch[i] = 0;
                } else if ( *p == 1 ) {
                    rtd->cob_switch[i] = 1;
                }
            }
            *result = 0;
            break;
        case 12:
            /* Get switches */
            p = parm;
            for ( i = 0; i < 8; ++i, ++p ) {
                *p = rtd->cob_switch[i];
            }
            *result = 0;
            break;
        case 15:
            /* test program exist */
            memset (buffer, 0, sizeof(buffer));

            p = parm;
            len = *p;
            p++;
            for ( i = 0; i < len && i < (sizeof(buffer)-1); ++i, ++p ) {
                buffer[i]=*p ;
            }
            cob_string_to_C(rtd, buffer, i);
            if (cob_resolve(rtd, buffer)) {
                p = &parm[1];
                strncpy((char*)p, rtd->last_resolved_name_n, len);
                j = strlen ((char*)p);
                for ( i = j; i < len; i++ ) {
                    p[i] = CHAR_SP;
                }
                *result = 0;
            } else {
                *result = 1;
            }
            break;
        case 16:
            /* Return number of call parameters */
            *parm = rtd->cob_save_call_params;
            *result = 0;
            break;
        default:
            *result = 1;
            break;
    }
    return(0);
}

int
CBL_TOUPPER (unsigned char *data, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_TOUPPER(rtd, data, length);
}

int
rtd_CBL_TOUPPER (COB_RTD, unsigned char *data, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_TOUPPER, 2);

    if ( length > 0 ) {
        for ( n = 0; n < (size_t)length; ++n ) {
            if ( islower (data[n]) ) {
                data[n] = toupper (data[n]);
            }
        }
    }
    return(0);
}

int
CBL_TOLOWER (unsigned char *data, const int length) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_TOLOWER(rtd, data, length);
}

int
rtd_CBL_TOLOWER (COB_RTD, unsigned char *data, const int length) {
    size_t  n;

    COB_CHK_PARMS (CBL_TOLOWER, 2);

    if ( length > 0 ) {
        for ( n = 0; n < (size_t)length; ++n ) {
            unsigned char c = E2A(data[n]);
            if ( isupper (c) ) {
                data[n] = A2E(tolower (c));
            }
        }
    }
    return(0);
}

int
CBL_OC_NANOSLEEP (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_OC_NANOSLEEP(rtd, data);
}

int
rtd_CBL_OC_NANOSLEEP (COB_RTD, unsigned char *data) {
    long long   nsecs;
#ifdef	_WIN32
#if 0
    struct timeval  tv;
#else
    unsigned int    msecs;
#endif
#else
    struct timespec tsec;
#endif

    COB_CHK_PARMS (CBL_OC_NANOSLEEP, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        nsecs = cob_get_long_long (rtd, rtd->current_module->cob_procedure_parameters[0]);
        if ( nsecs > 0 ) {
#ifdef	_WIN32
#if 0
            nsecs /= 1000;
            if ( nsecs > 0 ) {
                tv.tv_sec = (long)(nsecs / 1000000);
                tv.tv_usec = (long)(nsecs % 1000000);
                select (0, (void *)0, (void *)0, (void *)0, &tv);
            }
#else
            msecs = (unsigned int)(nsecs / 1000000);
            if ( msecs > 0 ) {
                Sleep (msecs);
            }
#endif
#else
            tsec.tv_sec = nsecs / 1000000000;
            tsec.tv_nsec = nsecs % 1000000000;
            nanosleep (&tsec, NULL);
#endif
        }
    }
    return(0);
}

int
cob_caller_name (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_caller_name(rtd, data);
}

int
rtd_cob_caller_name (COB_RTD, unsigned char *data) {
    COB_CHK_PARMS (C$CALLERNAME, 1);

    if ( rtd->current_module && rtd->current_module->next && rtd->current_module->next->module_name ) {
        cob_set_Cstring(rtd, rtd->current_module->cob_procedure_parameters[0], rtd->current_module->next->module_name);
    } else {
        cob_set_Cstring(rtd, rtd->current_module->cob_procedure_parameters[0], (char*)"UNDEFINED");
    }

    return(0);
}

int
cob_program_id (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_program_id(rtd, data);
}

int
rtd_cob_program_id (COB_RTD, unsigned char *data) {
    COB_CHK_PARMS (C$PROGRAM_ID, 1);

    if ( rtd->current_module && rtd->current_module->module_name ) {
        cob_set_Cstring(rtd, rtd->current_module->cob_procedure_parameters[0], rtd->current_module->module_name);
    } else {
        cob_set_Cstring(rtd, rtd->current_module->cob_procedure_parameters[0], (char*)"UNDEFINED");
    }

    return(0);
}

int
cob_return_args (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_return_args(rtd, data);
}

int
rtd_cob_return_args (COB_RTD, unsigned char *data) {
    COB_CHK_PARMS (C$NARG, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        cob_set_int (rtd, rtd->current_module->cob_procedure_parameters[0], rtd->cob_save_call_params);
    }
    return(0);
}

int
cob_parameter_size (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_parameter_size(rtd, data);
}

int
rtd_cob_parameter_size (COB_RTD, unsigned char *data) {
    int n;

    COB_CHK_PARMS (C$PARAMSIZE, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        n = cob_get_int (rtd, rtd->current_module->cob_procedure_parameters[0]);
        if ( n > 0 && n <= rtd->cob_save_call_params ) {
            n--;
            if ( rtd->current_module->next &&
                 rtd->current_module->next->cob_procedure_parameters[n] ) {
                return(rtd->current_module->next->cob_procedure_parameters[n]->size);
            }
        }
    }
    return(0);
}

int
cob_parameter_type (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_parameter_type(rtd, data);
}

int
rtd_cob_parameter_type (COB_RTD, unsigned char *data) {
    int n;

    COB_CHK_PARMS (C$PARAMTYPE, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        n = cob_get_int (rtd, rtd->current_module->cob_procedure_parameters[0]);
        if ( n > 0 && n <= rtd->cob_save_call_params ) {
            n--;
            if ( rtd->current_module->next &&
                 rtd->current_module->next->cob_procedure_parameters[n] ) {
                return(rtd->current_module->next->cob_procedure_parameters[n]->attr->type );
            }
        }
    }
    return(0);
}

int
cob_acuw_sleep (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_acuw_sleep(rtd, data);
}

int
rtd_cob_acuw_sleep (COB_RTD, unsigned char *data) {
    int n;

    COB_CHK_PARMS (C$SLEEP, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        n = cob_get_int (rtd, rtd->current_module->cob_procedure_parameters[0]);
        if ( n > 0 && n < 3600*24*7 ) {
#ifdef	_WIN32
            Sleep (n*1000);
#else
            sleep ((unsigned int)n);
#endif
        }
    }
    return(0);
}

int
cob_acuw_justify (unsigned char *data, ...) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_acuw_justify(rtd, data);
}

int
rtd_cob_acuw_justify (COB_RTD, unsigned char *data, ...) {
    unsigned char   *direction;
    size_t          datalen;
    int             n;
    int             shifting = 0;
    size_t          left = 0;
    size_t          right = 0;
    size_t          movelen;
    size_t          centrelen;
    va_list         args;

    COB_CHK_PARMS (C$JUSTIFY, 1);

    datalen = rtd->current_module->cob_procedure_parameters[0]->size;
    if ( datalen < 2 ) {
        return(0);
    }
    if ( data[0] != CHAR_SP && data[datalen - 1] != CHAR_SP ) {
        return(0);
    }
    for ( n = 0; n < (int)datalen; ++n, ++left ) {
        if ( data[n] != CHAR_SP ) {
            break;
        }
    }
    if ( n == (int)datalen ) {
        return(0);
    }
    left = n;
    for ( n = (int)datalen - 1; n >= 0; --n, ++right ) {
        if ( data[n] != CHAR_SP ) {
            break;
        }
    }
    movelen = datalen - left - right;
    if ( rtd->cob_call_params > 1 ) {
        va_start (args, data);
        direction = va_arg (args, unsigned char *);
        va_end (args);
        if ( *direction == 'L' ) {
            shifting = 1;
        } else if ( *direction == 'C' ) {
            shifting = 2;
        }
    }
    switch ( shifting ) {
        case 1:
            memmove (data, &data[left], movelen);
            memset (&data[movelen], CHAR_SP, datalen - movelen);
            break;
        case 2:
            centrelen = (left + right) / 2;
            memmove (&data[centrelen], &data[left], movelen);
            memset (data, CHAR_SP, centrelen);
            if ( (left + right) % 2 ) {
                memset (&data[centrelen + movelen], CHAR_SP, centrelen + 1);
            } else {
                memset (&data[centrelen + movelen], CHAR_SP, centrelen);
            }
            break;
        default:
            memmove (&data[left + right], &data[left], movelen);
            memset (data, CHAR_SP, datalen - movelen);
            break;
    }
    return(0);
}

unsigned long 
cob_get_elapsed_tick (COB_RTD, cob_module *module) {
    unsigned long long t;
    unsigned long long l;
    cob_paragraph_profiling_times tms;

    t = cob_clock(&tms);
    if ( module->module_version > 0) {
        l = tms.cpu_tick -module->profiling_last.cpu_tick;
        module->profiling_last.cpu_tick = tms.cpu_tick;
        module->profiling_delta.cpu_tick = (long)l;

        l = tms.elaps_tick -module->profiling_last.elaps_tick ;
        module->profiling_last.elaps_tick = tms.elaps_tick;
        module->profiling_delta.elaps_tick = (long)l; 

        return(module->profiling_delta.cpu_tick);
    } else {
        l = t -module->profiling_last_cpu_tick_v0 ;
        module->profiling_last_cpu_tick_v0 = t;
        return((unsigned long)l);
    }
}

#ifdef _MSC_VER
int strncasecmp (const char * _Str1, const char * _Str2,  size_t _MaxCount) {
    return(_strnicmp(_Str1, _Str2, _MaxCount));
}

int strcasecmp  (const char * _Str1, const char * _Str2) {
    return(_stricmp(_Str1, _Str2));
}
#endif

int
cob_getpid (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_getpid(rtd, data);
}

int
rtd_cob_getpid (COB_RTD, unsigned char *data) {
    COB_CHK_PARMS (C$PID, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        cob_set_int (rtd, rtd->current_module->cob_procedure_parameters[0], getpid());
    }
    return(0);
}

int
cob_c_check_rw (void) {

    COB_RTD = cob_get_rtd();
    return rtd_cob_c_check_rw(rtd);
}

int
rtd_cob_c_check_rw (COB_RTD) {
    static int done = 0;
    if (!done) {
        check_citkey("REPORT-WRITER");
#ifdef _MSC_VER
        cob_load_shared_lib(cob_enterprise_get_region_0(rtd),(char*)"SPCRW2LIBR0.dll");
#else
        cob_load_shared_lib(cob_enterprise_get_region_0(rtd),(char*)"SPCRW2LIBR0.so");
#endif
        done = 1;
    }
    return(0);
}

int
cob_debug_activate (unsigned char *data) {
    COB_RTD = cob_get_rtd();
    return rtd_cob_debug_activate(rtd,data);
}

int cob_debug_acl_alluser (COB_RTD, int flag) 
{
    rtd->debug_acl_alluser = flag;
    return 0;
}

int
rtd_cob_debug_activate (COB_RTD, unsigned char *data) {
    int pid = getpid();
    COB_CHK_PARMS (C$DEBUG, 1);

    if ( rtd->current_module->cob_procedure_parameters[0] ) {
        pid = cob_get_int (rtd, rtd->current_module->cob_procedure_parameters[0]);
    }
    cob_runtime_debugger_activate(rtd, pid);
    return(0);
}

void
cob_set_context_appli_prefix (COB_RTD, char *fname) {
    if ( rtd->cob_context_info.cob_context_appli_prefix ) {
        free(rtd->cob_context_info.cob_context_appli_prefix);
    }
    rtd->cob_context_info.cob_context_appli_prefix = strdup(fname);
}

void
cob_set_context_server_prefix (COB_RTD, char *fname) {
    if ( rtd->cob_context_info.cob_context_server_prefix ) {
        free(rtd->cob_context_info.cob_context_server_prefix);
    }
    rtd->cob_context_info.cob_context_server_prefix = strdup(fname);
}



void *
cob_get_module_storage   (COB_RTD, char *key , int size) {
    struct cob_module_data *p;

    for ( p = rtd->modules_data; p; p = p->next ) {
        if ( (strcmp(key, p->name) == 0) && ( p->size == size) ) {
            return p->data;
        }
    }
    /*fprintf (stderr, "alloc module %s", key);*/
    p = cob_malloc(rtd, sizeof (struct cob_module_data));
    p->next = rtd->modules_data;
    rtd->modules_data = p;
    p->size = size;
    p->name = strdup(key);
    p->data = cob_malloc(rtd, size);
    return p->data;
}

void 
cob_free_module_storage  (COB_RTD, char *key , int size) {
    struct cob_module_data  *p;
    struct cob_module_data  **pp;

    pp = &(rtd->modules_data);
    for ( p = *pp; p; pp = &p->next, p = p->next ) {
        if ( (strcmp(key, p->name) == 0) && ( p->size == size) ) {
            if ( p->data ) {
                free(p->data);
                p->data=NULL;
            }
            if ( p->name ) {
                free(p->name);
                p->name = NULL;
            }
            *pp = p->next;
            free (p);
            /*fprintf (stderr, "free  module %s", key);*/
            break;
        }
    }

}

void 
cob_free_all_module_storage  (COB_RTD) {
    struct cob_module_data  *p;
    struct cob_module_data  *p1;
    struct cob_external *eptr;
    struct cob_external *eptr1;
    for ( eptr = rtd->cob_baseexternal; eptr;  ) {
        if (eptr->ext_alloc) {
            cob_free(eptr->ext_alloc);
        }
        if (eptr->ename) {
            cob_free(eptr->ename);
        }
        eptr1 = eptr;
        eptr = eptr->next;
        cob_free(eptr1);
    }
    rtd->cob_baseexternal = NULL;
    for ( p = rtd->modules_data; p;  ) {
        if ( p->data ) {
            free(p->data);
            p->data=NULL;
        }
        if ( p->name ) {
            free(p->name);
            p->name = NULL;
        }
        p1 = p;
        p = p1->next;
        free (p1);
    }
    rtd->modules_data = NULL;

}

int
cobtidy (void) {
    COB_RTD = cob_get_rtd();
    cob_rtd_tidy(rtd);
    cob_set_rtd(NULL);
    return(0);
}

static void 
cob_close_exec (COB_RTD) {
    struct exit_handlerlist *h;
    struct handlerlist *e, *p;
    cob_profiling_dump_all_to_file(rtd,(char*)"_final");
    debug_callback_fct(rtd, COB_DBCALLBACK_EXIT);
    cob_runtime_debugger_cleanup(rtd);
    //cob_debug_callback = NULL;
    if ( rtd->exit_hdlrs != NULL ) {
        h = rtd->exit_hdlrs;
        rtd->exit_hdlrs = NULL;
        while ( h != NULL ) {
            e = (struct handlerlist *)h;
            h->proc ();
            h = h->next;
            cob_free(e);
        }
    }
    if ( rtd->hdlrs != NULL ) {
        p = rtd->hdlrs;
        while ( p != NULL ) {
            e = p;
            p = p->next;
            cob_free(e);
        }
        rtd->hdlrs = NULL;
    }
    inTerminate =1;
    cob_screen_terminate (rtd);
    cob_exit_fileio (rtd, 0);
}

void 
cob_terminate_exec (COB_RTD) {
    cob_close_exec(rtd);
    cob_runtime_debugger_cleanup(rtd);
    cob_enterprise_free_rtd_allocated(rtd);
    rtd->current_module = NULL;
}


void cob_set_exit_rtd_proc  (COB_RTD, cob_rtd_exit_proc cob_exit_proc)
{
    rtd->cob_exit_proc = cob_exit_proc;
}

extern int cob_dump_memory_usage (void);
int cob_dump_memory_usage (void)
{
#ifdef DEBUG
#ifdef __GNUC__
    struct mallinfo info;
    info = mallinfo();
    printf("allocated memory = %d / %d  Bytes \n", (int)info.uordblks, (int)info.arena);
#endif
#endif
    return 0;
}

int   cob_get_last_status    (COB_RTD)
{
    return rtd->cob_last_status;
}
char * cob_get_last_errormsg  (COB_RTD)
{
    return rtd->runtime_err_str;
}

cob_module * cob_get_current_module (COB_RTD)
{
    return rtd->current_module;
}

int cob_validate_range(int _min,int _max,int _v) {
    return((_v) < (_min) ? (_min) : (_v) > (_max) ? (_max) : (_v));
}


void cob_set_sysin_file      (COB_RTD, char *filename)
{
    if ( !rtd->sysin_redirected ) {
        if (freopen(filename,"r",stdin) == NULL) {
            cob_runtime_error(rtd, "Unable to open SYSIN : %s (%s).", filename, cob_strerror(rtd,errno));
        }
        rtd->sysin_redirected =1;
    }
}

void cob_set_sysout_file     (COB_RTD, char *filename)
{
    if ( !rtd->sysout_redirected ) {
        if (freopen(filename,"r",stdout) == NULL) {
            cob_runtime_error(rtd, "Unable to open SYSOUT : %s (%s).", filename, cob_strerror(rtd,errno));
        }
        rtd->sysout_redirected =1;
    }
}

void cob_set_syserr_file     (COB_RTD, char *filename)
{
    if ( !rtd->syserr_redirected ) {
        if (freopen(filename,"r",stderr) == NULL) {
            cob_runtime_error(rtd, "Unable to open SYSERR : %s (%s).", filename, cob_strerror(rtd,errno));
        }
        rtd->syserr_redirected =1;
    }
}

static int
is_same_sysfile (COB_RTD, cob_file_extfh * rtdfile, cob_file_extfh *newfile) {
    char *curfname1, *curfname2;
    int res = 0;
    mf_extfh_FCD *rtd_fcd;
    mf_extfh_FCD *newfile_fcd;
    FLD_VAR
    if ( !rtdfile || !newfile) {
        return 0;
    }
    rtd_fcd     = (mf_extfh_FCD *) rtdfile->extfh_ptr;
    newfile_fcd = (mf_extfh_FCD *) newfile->extfh_ptr;
    curfname1 = GT_FLDP(rtd_fcd->fname_ptr);
    curfname2 = GT_FLDP(newfile_fcd->fname_ptr);
    res = strcmp(curfname1, curfname2);
    return(res == 0);
}

void cob_redirect_sysfile(COB_RTD, void *file, int filenr) {

    switch ( filenr ) {
        case 0:
            if ( rtd->current_module ) {
                rtd->current_module->sysin_file = rtd->sysin_file;
            }
            if ( ! is_same_sysfile(rtd,rtd->sysin_file, file)) {
                rtd->sysin_file = file;
            }
            break;
        case 1:
            if ( rtd->current_module ) {
                rtd->current_module->sysout_file = rtd->sysout_file;
            }
            if ( !is_same_sysfile(rtd,rtd->sysout_file, file) ) {
               if (rtd->sysout_file)
               {
                  extfh_close(rtd, rtd->sysout_file, 0);
               }
                rtd->sysout_file = file;
            }
            break;
        case 2:
            if ( rtd->current_module ) {
                rtd->current_module->syserr_file = rtd->syserr_file;
            }
            if ( !is_same_sysfile(rtd,rtd->syserr_file, file) ) {
                rtd->syserr_file = file;
            }
            break;
        case 3:
            if ( rtd->current_module ) {
                rtd->current_module->sysprint_file = rtd->sysprint_file;
            }
            if ( !is_same_sysfile(rtd,rtd->sysprint_file, file) && file != NULL) {
                rtd->sysprint_file = file;
            }
            break;
    }
}

void cob_setting_action(COB_RTD, int action_code)
{
    switch ( action_code ) {
        case COB_SAC_CLEAR_CURSOR_POS:
            memset (rtd->cursorpos_shadow, 0, sizeof(rtd->cursorpos_shadow));
            break;
        default:
            break;
    }
}

extern int dump_citkey (const char*product);
void dump_citkey_message(int v);
void
cob_print_version (const char*product) {
#  ifdef CIT_ENTERPRISE
char * ent="Enterprise";
#else
char * ent="";
#endif
    printf ("cobc (%s) %s\nVersion %s (%d bits)\nBuild date %s %s\n",
          PACKAGE_NAME, 
            ent,
            COB_VERSION, (int)(sizeof(void*)*8), __DATE__, __TIME__);
#ifdef _MSC_VER
    printf ("Build with Microsoft(c) Compiler version %d\n", _MSC_VER);
#endif
    dump_citkey (product);
#ifdef COB_HAS_THREAD
    puts ("Thread safe programs and runtime");
#endif

#ifdef CIT_DUAL_CHARSET
    puts ("Dual ASCII and EBCDIC runtime");
#else 
#ifdef CIT_EBCDIC_CHARSET
    puts ("EBCDIC runtime");
#endif
#endif
    puts ("Copyright (C) 2001-2008 Keisuke Nishida / Roger While for OPEN-COBOL parts");
    puts ("Copyright (C) 2008-2019 COBOL-IT\n");
}

void *
cob_malloc_cbl_allocation (COB_RTD, int size, int thread, int module)
{
    struct data_list *l;
    void *data;

    data = calloc(1,size);
    if ( data && rtd) {
        if (thread || module ) {
            l = rtd->cbl_allocated_list;
            while ( l ) {
                if ( l->data == NULL ) {
                    l->data = data;
                    l->len  = size;
                    break;
                }
                l = l->next;
            }
            if ( l == NULL ) {
                l = rtd->cbl_allocated_list;
                rtd->cbl_allocated_list = calloc(1,sizeof(struct data_list));
                if ( !rtd->cbl_allocated_list ) {
                    free (data);
                    return NULL;
                }
                rtd->cbl_allocated_list->data=data;
                rtd->cbl_allocated_list->len=size;
                rtd->cbl_allocated_list->next=l;
            }

            if ( module && rtd->current_module ) {
                if (rtd->current_module->cbl_allocated_list_ptr) {
                    rtd->current_module->cbl_allocated_list = *(rtd->current_module->cbl_allocated_list_ptr);
                }
                l = rtd->current_module->cbl_allocated_list;
                while ( l ) {
                    if ( l->data == NULL ) {
                        l->data = data;
                        l->len  = size;
                        break;
                    }
                    l = l->next;
                }
                if ( l ==  NULL ) {
                    l = rtd->current_module->cbl_allocated_list;
                    rtd->current_module->cbl_allocated_list = calloc(1,sizeof(struct data_list));
                    if ( !rtd->current_module->cbl_allocated_list ) {
                        free (data);
                        return NULL;
                    }
                    rtd->current_module->cbl_allocated_list->data=data;
                    rtd->current_module->cbl_allocated_list->len=size;
                    rtd->current_module->cbl_allocated_list->next=l;
                    if (rtd->current_module->cbl_allocated_list_ptr) {
                        *(rtd->current_module->cbl_allocated_list_ptr) = rtd->current_module->cbl_allocated_list;
                    }
                }
            }
        }
    }
    return data;
}

void 
cob_free_cbl_allocation (COB_RTD, void *data)
{
    struct data_list *l;
    cob_module *m;
    if ( data ) {
        if ( rtd ) {
            l = rtd->cbl_allocated_list;
            while ( l ) {
                if ( l->data == data) {
                    l->data = NULL;
                    l->len = 0;
                    break;
                }
                l = l->next;
            }
            for ( m = rtd->current_module; m; m = COB_NEXT_MODULE(m)) {
                if (m->cbl_allocated_list_ptr) {
                    m->cbl_allocated_list = *(m->cbl_allocated_list_ptr);
                }
                l = m->cbl_allocated_list;
                while ( l ) {
                    if ( l->data == data) {
                        l->data = NULL;
                        l->len = 0;
                        break;
                    }
                    l = l->next;
                }
            }
        }
        free(data);
    }
}

char * 
cob_bool_getenv(const char *name)
{
    char *s;
    char * res = NULL;
    if ((s = getenv (name)) != NULL ) {
        if ( *s != 'N' && *s != 'n' && *s != '0' ) {
            res = s;
        } 
    }
    return res;
}


#include "stringutils.c"
/*#define COB_NATIONAL_CP_ID "UTF-16BE"*/


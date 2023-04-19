/*
 * Copyright (C) 2001-2007 Keisuke Nishida
 * Copyright (C) 2007 Roger While
  * Copyright (C) 2008-2020 Cobol-IT
 *
 * This library is cob_free software; you can redistribute it and/or
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

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"
#include "defaults.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef	HAVE_UNISTD_H
    #include <unistd.h>
#endif
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "a2e.h"
/*	NOTE - The following variable should be uncommented when
    it is known that dlopen(NULL) is borked.
    This is known to be true for some PA-RISC HP-UX 11.11 systems.
    This is fixed with HP patch PHSS_28871. (There are newer but this
    fixes dlopen/dlsym problems)
*/
/* #define COB_BORKED_DLOPEN */

#ifdef	USE_LIBDL

    #define __USE_GNU	1
    #include <dlfcn.h>

    #define lt_dlopen(x)	dlopen(x, RTLD_LAZY | RTLD_GLOBAL)
    #define lt_dlsym(x, y)	dlsym(x, y)
    #define lt_dlclose(x)	dlclose(x)
    #define lt_dlerror()	dlerror()
    #define lt_dlhandle	void *

#elif	defined(_WIN32)

    #define WINDOWS_LEAN_AND_MEAN
    #include <windows.h>
/* Prototype */
static char *   lt_dlerror (void);

static HMODULE
lt_dlopen (const char *x) {

    //char buff[2048];
    //char *p;
    if ( x == NULL ) {
        return GetModuleHandle (NULL);
    }
    //strcpy (buff, x);
    //p = buff;
    //while (*p) {
    //	if (*p == '/' )
    //		*p= '\\';
    //	p++;
    //}
    //return LoadLibrary(buff);
    return LoadLibrary(x);
}
    #define lt_dlsym(x, y)	GetProcAddress(x, y)
    #define lt_dlclose(x)	FreeLibrary(x)

static char *
lt_dlerror (void) {
    static char call_errbuf[1024];
    sprintf(call_errbuf, "LoadLibrary/GetProcAddress error %d", (int)GetLastError());
    return call_errbuf;
}
    #define	lt_dlinit()
    #define lt_dlhandle	HMODULE

#else

    #define LT_NON_POSIX_NAMESPACE 1
    #include <ltdl.h>

#endif

#include "call.h"
#include "common.h"
#include "coblocal.h"
#include "debug.h"
#include "fileio.h"


#define	COB_MAX_COBCALL_PARMS	16

/* Local const variables */
static const char               *resolve_bin_ext_dll[] = {COB_MODULE_EXT, CIT_BIN_EXTENTION, MF_BIN_EXTENTION, NULL};  
static const char               *resolve_bin_ext_cit[] = {CIT_BIN_EXTENTION, COB_MODULE_EXT, MF_BIN_EXTENTION, NULL};  
static const char               **resolve_bin_ext = resolve_bin_ext_dll;  

int cob_debug_call = 0;
#define debug_call_message if (cob_debug_call) printf

/*
 * Call table
 */


struct system_table {
    const char      *syst_name;
    void            *syst_call;
};


#undef	COB_SYSTEM_GEN
#define	COB_SYSTEM_GEN(x, y, z)		{ x, z },
static const struct system_table    system_tab[] = {
#include "system.def"
    { NULL, NULL}
};
#undef	COB_SYSTEM_GEN

void cob_dlclose (void*handle)
{
    lt_dlclose(handle);
}
/* Local functions */

static void * COB_NOINLINE
cob_strdup (COB_RTD, const void *stptr) {
    void    *mptr;
    size_t  len;

    len = strlen (stptr) + 1;
    mptr = cob_malloc (rtd, len);
    memcpy (mptr, stptr, len);
    return mptr;
}

void
cob_set_library_path (COB_RTD, const char *path) {
    char        *p;
    size_t      i;
    char *saveptr=NULL;

    /* clear the previous path */
    if ( rtd->resolve_path ) {
        cob_free (rtd->resolve_path[0]);
        cob_free (rtd->resolve_path);
    }

    /* count the number of ':'s */
    rtd->resolve_size = 1;
    for ( p = strchr (path, PATHSEPC); p; p = strchr (p + 1, PATHSEPC) ) {
        rtd->resolve_size++;
    }

    /* build path array */
    p = cob_strdup (rtd, path);
    rtd->resolve_path = cob_malloc (rtd, sizeof (char *) * (rtd->resolve_size));
    rtd->resolve_path[0] = COB_STRTOK (p, PATHSEPS, &saveptr);
    for ( i = 1; i < rtd->resolve_size; ++i ) {
        rtd->resolve_path[i] = COB_STRTOK (NULL, PATHSEPS, &saveptr);
    }
}

static void *
cob_get_buff (COB_RTD, const size_t buffsize) {
    if ( !rtd->call_buffer ) {
        if ( buffsize <= COB_SMALL_BUFF ) {
            rtd->call_buffer = cob_malloc (rtd, COB_SMALL_BUFF);
            rtd->call_buffer_lastsize = COB_SMALL_BUFF;
        } else {
            rtd->call_buffer = cob_malloc (rtd, buffsize);
            rtd->call_buffer_lastsize = buffsize;
        }
    } else {
        if ( buffsize > rtd->call_buffer_lastsize ) {
            cob_free (rtd->call_buffer);
            rtd->call_buffer = cob_malloc (rtd, buffsize);
            rtd->call_buffer_lastsize = buffsize;
        }
    }
    return rtd->call_buffer;
}

#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
static void
cache_handle (COB_RTD, lt_dlhandle libhandle) {
    struct data_list    *newhandle;

    newhandle = cob_malloc (rtd, sizeof (struct data_list));
    newhandle->data = (void*)libhandle;
    newhandle->next = rtd->pre_handle;
    rtd->pre_handle = newhandle;
}
#endif

static COB_INLINE size_t
hash_prog_name (const unsigned char *s) {
    size_t      val = 0;

    while ( *s ) {
        /*
        if ( *s == '.' )
            return val % HASH_SIZE;

        if ( *s == '/' || *s == '\\' ) {
            val = 0;
        } else {
            val += *s;
        }
        s++; 
        */ 
        val += toupper(*s++);
    }
    return val % HASH_SIZE;
}

static COB_INLINE size_t
hash_prog_entry (const void *s) {
    size_t      val = (size_t) s;

    /*val >>= 3;*/
    val = val & 0xFFFF;
    return val % HASH_SIZE;
}

static struct module_hash *
lookup_module_entry (cit_runtime_t * const rtd_rb, const char *name) {
    struct call_hash      *p;
    struct module_hash    *m;
    struct module_hash    *res = NULL;
    void *found = NULL;
    cit_runtime_t * rtd_r0 = cob_enterprise_get_region_0(rtd_rb);

    for ( p = rtd_r0->call_table[hash_prog_name ((const unsigned char *)name)]; p; p = p->next ) {
        /*
        if (p->fullpath && (strcasecmp (name, p->fullpath) == 0) && p->flag_is_valid) {
            return p->func;
        } 
        */ 
        if ( (strcasecmp (name, p->name) == 0) && p->flag_is_valid) {
            found = p->func;
            break;
        }
    }
    if ( found ) {
        for ( m = rtd_r0->module_table[hash_prog_entry (found)]; m; m = m->next ) {
            if ( (m->func == found) &&  m->flag_is_valid ) {
                res = m;
                break;
            }
        }
    }
    return res;
}

static void 
invalidate_module_entry (cit_runtime_t * const rtd_rb, const char *name, void *cancel_entry) {
    struct call_hash      *p;
    struct module_hash    *m;
    struct module_hash  **mm;
    void *found = NULL;
    int i;
    cit_runtime_t * rtd_r0 = cob_enterprise_get_region_0(rtd_rb);

    for ( p = rtd_r0->call_table[hash_prog_name ((const unsigned char *)name)]; p; p = p->next ) {
        /*
        if (p->fullpath && (strcasecmp (name, p->fullpath) == 0) && p->flag_is_valid) {
            return p->func;
        } 
        */ 
        if ( (strcasecmp (name, p->name) == 0) ) {
            p->flag_is_valid = 0;
            found = p->func;
            break;
        }
    }
    if ( found ) {
        for ( m = rtd_r0->module_table[hash_prog_entry (found)]; m; m = m->next ) {
            if ( (m->func == found) ) {
                if (m->handle) {
                    cob_dlclose(m->handle);
                }
                cob_clear_module_hash(m);

                break;
            }
        }
    }
    if (cancel_entry) {
        for ( i = 0; i < HASH_SIZE; i++ ) {
            mm = &(rtd_r0->module_table[i]);
            for ( m = *mm; m; mm = &(m->next), m = m->next ) {
                if (m && (m->cancel == cancel_entry) ) {
                    if (m->handle) {
                        cob_dlclose(m->handle);
                    }
                    cob_clear_module_hash(m);
                }
            }
        }
    }
}

void *
cob_lookup_module_function (cit_runtime_t * const rtd_rb, const char *name) {

    struct module_hash    *m;
    cit_runtime_t * rtd_r0 = cob_enterprise_get_region_0(rtd_rb);

    if (rtd_r0->last_call_resolved_func && strcmp(name,rtd_r0->last_call_resolved_name) == 0) {
        return rtd_r0->last_call_resolved_func;
    }
    m = lookup_module_entry(rtd_r0,name);
    if ( m ) {
        strcpy (rtd_r0->last_call_resolved_name, name);
        rtd_r0->last_call_resolved_func = m->func;
        return m->func;
    }
    return NULL;
}


static void
insert_name (cit_runtime_t * const rtd_rb, const char *name, void *func) {
    struct call_hash    *p;
    size_t              val;
    cit_runtime_t * rtd_r0 = cob_enterprise_get_region_0(rtd_rb);

    val = hash_prog_name ((const unsigned char *)name);
    for ( p = rtd_r0->call_table[val]; p; p = p->next ) {
        if ( (strcasecmp (name, p->name) == 0) ) {
            p->func = func;
            p->flag_is_valid = 1;
            return;
        }
    }

    p = cob_malloc (rtd_r0, sizeof (struct call_hash));
    p->name = cob_strdup (rtd_r0, name);
    p->func = func;
    p->flag_is_valid = 1;
    p->next = rtd_r0->call_table[val];
    rtd_r0->call_table[val] = p;

}

void 
cob_clear_module_hash (struct module_hash     *m)
{
    void * next = m->next;
    memset (m, 0, sizeof(struct module_hash));
    m->next = next;
}


static void
insert (cit_runtime_t * rtd_rb, const char *name, void *func, void *cancel, lt_dlhandle handle, int is_unloadable, const char*fullpath) {
    struct module_hash  *m = NULL;
    size_t              val;
    cit_runtime_t *     rtd_r0 = cob_enterprise_get_region_0(rtd_rb);
    unsigned int        currn = rtd_rb->rtd_region_nr;

    if (currn > COB_MAX_RTD_REGION) {
        currn = 0;
    }

    val = hash_prog_entry (func);
    for ( m = rtd_r0->module_table[val]; m; m = m->next ) {
        if ( m->func == func ) {
            break;
        }
    }
    if ( !m && name) {
        m = lookup_module_entry(rtd_r0,name);
    }
    if ( !m && fullpath) {
        m = lookup_module_entry(rtd_r0,fullpath);
    }

    if ( !m) {
        for ( m = rtd_r0->module_table[val]; m; m = m->next ) {
            if ( !m->flag_is_valid ) {
                cob_clear_module_hash (m);
                break;
            }
        }
    }
    if ( m ) {
        if (!m->flag_is_valid) {
            if (func) {
                m->func = func;
            }
            if (cancel ) {
                m->cancel = cancel;
            }
            if (handle) {
                if (m->handle) {
                    cob_dlclose(m->handle);
                }
                m->handle =  handle;
            }
            m->name = (char*)name;
            m->flag_is_valid = 1;
            m->is_unloadable = m->handle ? is_unloadable : 0 ;
        } else {            
            if (cancel && m->cancel && m->cancel != cancel) {
                goto anew;
            }
            if (cancel ) {
                m->cancel = cancel;
            }
            if (m->func) {
                func = m->func;
            }
        }
        insert_name(rtd_r0,name,func);
        if ( fullpath ) {
            insert_name(rtd_r0,fullpath,func);
        } 
        m->region_use[currn] =1;
        return;
    }
    anew:
    m = cob_malloc (rtd_r0, sizeof (struct module_hash));
    m->func = func;
    m->name = (char*)name;
    m->cancel = cancel;
    m->handle =  handle;
    m->flag_is_valid = 1;
    m->is_unloadable = handle ? is_unloadable : 0;
    insert_name(rtd_r0,name,func);
    if ( fullpath ) {
        insert_name(rtd_r0,fullpath,func);
    }

    m->region_use[currn] =1;
    m->next = rtd_r0->module_table[val];
    rtd_r0->module_table[val] = m;

}

const char *
cob_resolve_error (COB_RTD) {
    const char  *p = rtd->resolve_error;

    rtd->resolve_error = NULL;
    return p;
}

void
cob_call_error (COB_RTD) {
    const char  *s;

    s = cob_resolve_error (rtd);
    if ( !s ) {
        s = "Unknown error";
    }
    cob_runtime_error (rtd, "%s", s);
    cob_stop_abend (rtd, COBRE_CALL_NOT_FOUND);
}

void
cob_set_cancel (COB_RTD, const char *name, void *entry, void *cancel) {

    insert (rtd, name, entry, cancel, NULL, 1, NULL);
}

static void *
load_resolv_file (COB_RTD, const char * filename, const char * fct_name, const char * cache_name) {
    void*       func;
    void*       handle;

    debug_call_message("CALL Try load  :%s:\n", filename);
    if ( (handle = lt_dlopen (filename)) != NULL ) {
        debug_call_message("CALL Try symbol  :%s:\n", fct_name);
        if ( (func = lt_dlsym (handle, (char *)fct_name)) != NULL ) {
            strncpy(rtd->last_resolved_name_n, filename, sizeof(rtd->last_resolved_name_n));
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
            /* Candidate for future calls */
            cache_handle (rtd, handle);
#endif
            insert (rtd, cache_name, func, NULL, handle, 1, filename);
            rtd->resolve_error = NULL;
            return(func);
        }
    }
    strcpy (rtd->resolve_error_buff, lt_dlerror ());

/*CobolIT*/
    strcat (rtd->resolve_error_buff, " - file : ");
    strcat (rtd->resolve_error_buff, filename);
    strcat (rtd->resolve_error_buff, ":");
    if ( handle != NULL ) {
        strcat (rtd->resolve_error_buff, " - symbol :");
        strcat (rtd->resolve_error_buff, fct_name);
        strcat (rtd->resolve_error_buff, ":");
        cob_dlclose (handle);
    }

    rtd->resolve_error = rtd->resolve_error_buff;
    debug_call_message("%s\n",rtd->resolve_error);
    return(NULL);
}

static void
setup_resolve_bin_ext (COB_RTD){
    if (rtd->call_flag & COB_FLAG_DOT_CIT_FIRST) {
        resolve_bin_ext = resolve_bin_ext_cit;
    } else {
        resolve_bin_ext = resolve_bin_ext_dll;
    }
}

static char*
basename (char *name) {
#ifndef _MSC_VER
    char *base = name;
    char *p;
    int  j;
    char ext[100];

    while ( *name ) {
        if ( *name == '/' || *name == '\\' ) {
            base =name+1;
        }
        name++;
    }
    for ( j=0; resolve_bin_ext[j]; j++ ) {
        sprintf(ext,".%s", resolve_bin_ext[j]);
        p = base + (strlen(base));
        p -= strlen (ext);
        if ( p > base ) {
            if ( strcmp(p, ext) == 0 ) {
                *p = 0;
                break;
            }
        }
    }
    return(char *) base;
#else
    unsigned char       buff[COB_SMALL_BUFF];
    _splitpath(name, NULL, NULL, buff, NULL);
    strcpy(name, buff);
    return name;
#endif
}

static void * 
try_load_resolv_file (COB_RTD, char *filename, char *symbole, const char *cachename) {
    char                *s;
    unsigned char       wrksymbole[COB_SMALL_BUFF];
    struct stat         st;
    void                *res = NULL;

    debug_call_message("CALL Looking file :%s:\n", filename);
    if ( cob_stat ((char*)filename, &st) == 0 ) {
        debug_call_message("CALL Looking file :%s: Found\n", filename);
        strcpy ((char*)wrksymbole, symbole);
        s = (char*)wrksymbole;
        if (rtd->call_match_exact_case) {
            res = load_resolv_file(rtd, filename, s, cachename);            
            if ( res!=NULL ) {
                debug_call_message("CALL symbol :%s: found\n", s);
                return(res);
            }
        }
        if (rtd->call_match_upper_case) {
            cob_strupper(s);
            res = load_resolv_file(rtd, filename, s, cachename);            
            if ( res!=NULL ) {
                debug_call_message("CALL symbol :%s: found\n", s);
                return(res);
            }
        }
        if (rtd->call_match_lower_case) {
            cob_strlower(s);
            res = load_resolv_file(rtd, filename, s, cachename);            
            if ( res!=NULL ) {
                debug_call_message("CALL symbol :%s: found\n", s);
                return(res);
            }
        }
    }
    return NULL;
}

static void * 
try_symbol (COB_RTD, char *symbole, const char *name) {
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
    struct data_list    *chkhandle;
#endif
    void                *func = NULL;

    /* search the main program */
    if ( rtd->mainhandle != NULL && (func = lt_dlsym (rtd->mainhandle, symbole)) != NULL ) {
        insert (rtd, name, func, NULL, NULL, 1, NULL);
        rtd->resolve_error = NULL;
        strncpy(rtd->last_resolved_name_n, symbole, sizeof(rtd->last_resolved_name_n));
        return func;
    }

    /* Search preloaded modules */
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
    for ( chkhandle = rtd->pre_handle; chkhandle; chkhandle = chkhandle->next ) {
        if ( chkhandle->data &&  (func = lt_dlsym (chkhandle->data, symbole)) != NULL ) {
            insert (rtd, name, func, NULL, NULL, 0, NULL);
            strncpy(rtd->last_resolved_name_n, symbole, sizeof(rtd->last_resolved_name_n));
            rtd->resolve_error = NULL;
            return func;
        }
    }
#endif
#if	defined(USE_LIBDL) && defined (RTLD_DEFAULT)
    if ( (func = lt_dlsym (RTLD_DEFAULT, symbole)) != NULL ) {
        insert (rtd, name, func, NULL, NULL, 1, NULL);
        strncpy(rtd->last_resolved_name_n, symbole, sizeof(rtd->last_resolved_name_n));
        rtd->resolve_error = NULL;
        return func;
    }
#endif

    return NULL;
}

/*
#ifdef _MSC_VER
static void * 
try_symbol (COB_RTD, char *symbole, const char *name) {
    unsigned char       buff[COB_SMALL_BUFF];
    void                * res=try_symbol1(rtd,symbole,name);
    
    if ( res ) {
        return res;
    }
    sprintf (buff,"_%s", symbole);
    return try_symbol1(rtd,buff,name); 
}
#else
#define try_symbol try_symbol1
#endif
*/

static void * 
cob_resolve_try_module(COB_RTD, const char * name, const unsigned char * s, size_t i_path, size_t j_bin_ext) 
{
    void * func = NULL;
    struct stat st;
    char   filename[COB_SMALL_BUFF];

    if ( rtd->resolve_path[i_path] == NULL ) {
        sprintf (filename, "%s.%s", s, resolve_bin_ext[j_bin_ext]);
    } else {
        sprintf (filename, "%s%c%s.%s", rtd->resolve_path[i_path], DIRSEPC, s, resolve_bin_ext[j_bin_ext]);
    }
    debug_call_message("CALL try a file :%s:\n", filename);
    if ( cob_stat (filename, &st) == 0 ) {
        debug_call_message("CALL file exist :%s:\n", filename);
        func = try_load_resolv_file(rtd, filename,basename((char*)(rtd->call_namebuff)),name);            
    }
    return(func);
}

void *
cob_resolve (COB_RTD, const char *name) {
    unsigned char       *p;
    const unsigned char *s;
    void*               func;
    void*               func_inmem;
    size_t              i, j;
    struct stat         st;
    unsigned char       buff2[COB_SMALL_BUFF];
    unsigned char       callbuff2[COB_SMALL_BUFF];
    char                filename[COB_SMALL_BUFF];

/* Checked in generated code
    if (!rtd->cob_initialized) {
        fputs ("cob_init() must be called before cob_resolve()", stderr);
        cob_stop_run (1);
    }
*/

    /* search the cache */
    rtd->cob_exception_code = 0;
    strcpy(rtd->resolve_error_buff, "");
    strncpy(rtd->last_resolved_name_n, name, sizeof(rtd->last_resolved_name_n));

    func = cob_lookup_module_function (rtd, name);
    if ( func ) {
        debug_call_message("CALL Found in memory  :%s:\n", name);
        return func;
    }

    /* encode program name */
    p = (unsigned char*)rtd->call_namebuff;
    s = (const unsigned char *)name;
    if ( unlikely(isdigit (*s)) ) {
        p += sprintf ((char *)p, "_%02X", *s++);
    }
    setup_resolve_bin_ext(rtd);
    /*First look up into memory for the symbol exact (system call etc) */
    if (rtd->call_match_exact_symbol) {
        strcpy(rtd->call_namebuff, name);
        func = try_symbol(rtd, (char *)rtd->call_namebuff, name);
    }
    /* then encode for cobol convention */
    if (!func) {
        for (; *s; s++) {
            if ( likely(isalnum (*s) || *s == '_' || *s == '.' || *s == '/'|| *s == '\\'|| *s == ':') ) {
                *p++ = *s;
            } else if ( *s == '-' ) {
                *p++ = '_';
            } else {
                p += sprintf ((char *)p, "_%02X", *s);
            }
        }
        *p = 0;
        /*look up into memory for the symbol in exact case*/
        if (rtd->call_match_exact_case) {
            func = try_symbol(rtd, (char *)rtd->call_namebuff, name);
        }
    }

    if (!func && rtd->call_match_upper_case) {
        strcpy ((char*)callbuff2, (char *)rtd->call_namebuff);
        cob_strupper((char*)callbuff2);
        func = try_symbol(rtd, (char*)callbuff2, name);
    }
    if (!func && rtd->call_match_lower_case) {
        strcpy ((char*)callbuff2, (char *)rtd->call_namebuff);
        cob_strlower((char*)callbuff2);
        func = try_symbol(rtd, (char*)callbuff2, name);
    }
    if ((func != NULL) && !(rtd->call_flag & COB_LOAD_PRIORITY) ) {
        return func;
    }
    func_inmem=func;

    /* did I recieve A binary file name ?*/
    s = (const unsigned char *)name;
    strcpy (filename, (char*)s);
    strcpy ((char*)callbuff2, (char*)basename( filename));
    p = callbuff2;
    for ( ; *p; p++ ) {
        if ( *p == '-' ) {
            *p = '_';
        }
    }
    p = callbuff2;
    if ( cob_stat ((char*)s, &st) == 0 ) {
        func = try_load_resolv_file(rtd, (char*)s, (char*)p, (char*)name);            
        if ( func!=NULL ) {
            return(func);
        }

    }

    /* search external modules */
    if ( likely(rtd->name_convert == 0) ) {
        s = (const unsigned char *)name;
    } else {
        s = (const unsigned char *)name;
        p = buff2;
        for ( ; *s; s++ ) {
            if ( rtd->name_convert == 1 && isupper (*s) ) {
                *p++ = tolower (*s);
            } else if ( rtd->name_convert == 2 && islower (*s) ) {
                *p++ = toupper (*s);
            } else {
                *p++ = *s;
            }
        }
        *p = 0;
        s = buff2;
    }


    /* no let try load modules from path */
    for ( j =0 ; resolve_bin_ext[j]; j++ ) {
        for ( i = 0; i < rtd->resolve_size; i++ ) {
            strcpy ((char*)callbuff2, (char*)s);
            if (rtd->load_match_exact_case) {
                func = cob_resolve_try_module(rtd, name, callbuff2, i, j);
                if ( func ) return func;
            }
            if (rtd->load_match_upper_case) {
                cob_strupper((char*)callbuff2);
                func = cob_resolve_try_module(rtd, name, callbuff2, i, j);
                if ( func ) return(func);
            }
            if (rtd->load_match_lower_case) {
                cob_strlower((char*)callbuff2);
                func = cob_resolve_try_module(rtd, name, callbuff2, i, j);
                if ( func ) return(func);
            }
        }
    }
    if (func_inmem != NULL) {
        return func_inmem;
    }

    if ( strlen(rtd->resolve_error_buff)== 0 ) {
        sprintf (rtd->resolve_error_buff, "Cannot find module '%s'", name);
    }
    rtd->resolve_error = rtd->resolve_error_buff;
    cob_set_exception (rtd, COB_EC_PROGRAM_NOT_FOUND);
    return NULL;
}

void *
cob_resolve_1 (COB_RTD, const char *name) 
{
    void    *p;

    p = cob_resolve (rtd, name);
    if ( unlikely(!p) ) {
        cob_call_error (rtd);
    }
    return p;
}

void *
cob_call_resolve (COB_RTD, const cob_field *f) 
{
    char    *buff;

    buff = cob_get_buff (rtd, f->size + 1);
    cob_field_to_string (rtd, f, buff);
    (void) STRING_E2A((unsigned char*)buff,f->size);
    return cob_resolve (rtd, buff);
}

void *
cob_call_resolve_1 (COB_RTD, const cob_field *f) 
{
    void    *p;

    p = cob_call_resolve (rtd, f);
    if ( unlikely( !p) ) {
        cob_call_error (rtd);
    }
    return p;
}

void *
cob_call_resolve_2 (COB_RTD, const cob_field *f) 
{
    void    *p;

    p = cob_call_resolve (rtd, f);
    if ( unlikely( !p) ) {
        char    *buff;

        buff = cob_get_buff (rtd, f->size + 1);
        cob_field_to_string (rtd, f, buff);
        (void) STRING_E2A((unsigned char*)buff,f->size);
        if (cob_load_shared_lib_1 (rtd, buff, 0))
            p = (void*)-1;        
    }
    return p;
}

int 
cob_is_module_unloadable (struct module_hash     *m, unsigned int cancel_region_nr) {

    int i;
    if (m->handle && m->is_unloadable) {
        if (cancel_region_nr == COB_MAX_RTD_REGION) {
            return 1;
        }
        if (cancel_region_nr < COB_MAX_RTD_REGION && m->region_use[cancel_region_nr]) {
            for (i = 0; i < COB_MAX_RTD_REGION; i++) {
                if (i != cancel_region_nr && m->region_use[i]) {
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

void 
cob_cancel(cit_runtime_t * const rtd_rb, const char *name) 
{
    struct module_hash     *m;
    struct module_hash    **mm;
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
    struct data_list    *chkhandle;
    struct data_list    **p_chkhandle;
#endif
    int    clean_hash = 0;
    int    i;
    cit_runtime_t * rtd_r0 = cob_enterprise_get_region_0(rtd_rb);
    int    currn =rtd_rb->rtd_region_nr;
    union {
        int (*cancel_func)(int, ...);
        void    *cancel_void;
    } unicanc;
    void * cleaned_func =NULL;

    rtd_r0->last_call_resolved_func = NULL;
    debug_call_message("\nCALL cob_cancel %s\n", name);

    if ( unlikely(!name) ) {
        cob_runtime_error (rtd_rb, "NULL name parameter passed to 'cobcancel'");
        cob_stop_abend (rtd_rb, COBRE_CANCEL_ERROR);
    }
    debug_clear_field_cache(rtd_r0);
    m = lookup_module_entry(rtd_r0,name);
    if ( m ) {
        if ( m->cancel ) {
            debug_call_message("CALL cob_cancel Module Found :%s: Call cancel\n", name);
            unicanc.cancel_void = m->cancel;
            if (rtd_r0->call_flag & COB_FULL_CANCEL_ALL) {
                for (i = 0; i < COB_MAX_RTD_REGION; i++) {
                    if (m->region_use[i] && rtd_r0->rtd_region_data[i] && rtd_r0->rtd_region_data[i]->cob_initialized) {
                        cob_enterprise_set_current_region(rtd_r0,i);
                        unicanc.cancel_func (-1, NULL, NULL, NULL, NULL,                            
                                             NULL, NULL, NULL, NULL);
                    }
                }
                cob_enterprise_set_current_region(rtd_r0,currn);
            } else {
                unicanc.cancel_func (-1, NULL, NULL, NULL, NULL,                            
                                     NULL, NULL, NULL, NULL);
            }
        }
        if ( (rtd_r0->call_flag & COB_FULL_CANCEL) && 
              cob_is_module_unloadable(m, rtd_r0->call_flag & COB_FULL_CANCEL_ALL ? COB_MAX_RTD_REGION : currn)) {
            cleaned_func = m->cancel;
            m->region_use[currn] =0;
            debug_call_message("     cob_cancel close handle\n");

            cob_dlclose(m->handle);
            m->handle=NULL;
            /*lt_dlclose(m->handle);*/
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
            p_chkhandle = &(rtd_r0->pre_handle);
            for ( chkhandle = *p_chkhandle; chkhandle; p_chkhandle = &chkhandle->next, chkhandle = chkhandle->next ) {
                if ( chkhandle->data == m->handle ) {
                    *p_chkhandle = chkhandle->next;
                    cob_free (chkhandle);
                    break;
                }
            }
#endif
            cob_clear_module_hash (m);
            clean_hash =1;
        }

    }
    /* Has we did unload a module we must clean the hash table ...
       We do not know if other symbols are in the unloaded module*/

    if ( clean_hash ) {
        invalidate_module_entry(rtd_r0, name, unicanc.cancel_void);
        for ( i = 0; i < HASH_SIZE; i++ ) {
            mm = &(rtd_r0->module_table[i]);
            for ( m = *mm; m; m = m->next ) {
                if ( !m->handle && !m->cancel && m->func) {
                    cob_clear_module_hash (m);
                }
            }
        }
    }

}

static void
cob_init_env_var (COB_RTD, char *var, const char *env, const char *def, const char * base, const char * newbase) {
    char *p = getenv (env);

    if ( p ) {
        strcat (var, p);
    } else {
        if ( base && newbase ) {
            const char *s;
            char *d;
            int  ln, lb;
            s = def;
            d = var + strlen(var);
            ln = (int)strlen(newbase);
            lb = (int)strlen(base);
            do {
                if ( strncmp(s,base, lb) == 0 ) {
                    strcpy(d, newbase);
                    d += ln;
                    s += lb;
                } else {
                    *d = *s;
                    d++;
                    s++;
                }
            } while ( *s ) ;

        } else {
            strcat (var, def);
        }
    }
}

void
cob_field_cancel (COB_RTD, const cob_field *f) {
    char    *name;

    name = cob_get_buff (rtd, f->size + 1);
    cob_field_to_string (rtd, f, name);
    (void) STRING_E2A((unsigned char*)name,f->size);
    cob_cancel (rtd, name);
}

int 
cob_load_shared_lib_1 (COB_RTD, char *fname, int dumperror)
{
    char                *s;
    char                *saveptr=NULL;
    char                *p;
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
    lt_dlhandle         libhandle;
#endif
    size_t              i, j;
    struct stat         st;
    char                filename[COB_SMALL_BUFF];
    int                 loaded = 0;

    setup_resolve_bin_ext(rtd);
    p = cob_strdup (rtd, fname);
    s = COB_STRTOK (p, PATHSEPS, &saveptr);
    for ( ; s; s = COB_STRTOK (NULL, PATHSEPS, &saveptr) ) {
        loaded = 0;
        for ( i = 0; (i <= rtd->resolve_size) && !loaded; i++ ) {
            if ((i >= rtd->resolve_size) || 
                (rtd->resolve_path[i] == NULL) ) {
                strcpy (filename, s);
            } else {
                sprintf (filename, "%s%c%s", rtd->resolve_path[i], DIRSEPC, s);
            }
            debug_call_message("CALL Preload : try %s \n", filename);
            if ( cob_stat (filename, &st) == 0 ) {
                debug_call_message("CALL Preload : %s found \n", filename);
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
                if ( (libhandle = lt_dlopen (filename)) != NULL ) {
                    cache_handle (rtd, libhandle);
#else
                if ( lt_dlopen (filename) != NULL ) {
#endif
                    debug_call_message("CALL Preload : %s loaded\n", filename);
                    loaded = 1;
                } else {
                    strcpy (rtd->resolve_error_buff, lt_dlerror ());
                    strcat (rtd->resolve_error_buff, " - file : ");
                    strcat (rtd->resolve_error_buff, filename);
                    rtd->resolve_error = rtd->resolve_error_buff;
                    debug_call_message("%s\n",rtd->resolve_error);
                }

            } else {
                for ( j=0; resolve_bin_ext[j]; j++ ) {
                    if ((i >= rtd->resolve_size) ||  
                        (rtd->resolve_path[i] == NULL) ) {
                        sprintf (filename, "%s.%s", s, resolve_bin_ext[j]);
                    } else {
                        sprintf (filename, "%s%c%s.%s", rtd->resolve_path[i], DIRSEPC, s, resolve_bin_ext[j]);
                    }
                    debug_call_message("CALL Preload : try %s \n", filename);
                    if ( cob_stat (filename, &st) == 0 ) {
                        debug_call_message("CALL Preload : %s found \n", filename);
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
                        if ( (libhandle = lt_dlopen (filename)) != NULL ) {
                            cache_handle (rtd, libhandle);
                            debug_call_message("CALL Preload : %s loaded\n", filename);
                            loaded = 1;
                            break;
                        }
#else
                        if ( lt_dlopen (filename) != NULL ) {
                            debug_call_message("CALL Preload : %s loaded\n", filename);
                            loaded = 1;
                            break;
                        }
#endif
                        else {
                            strcpy (rtd->resolve_error_buff, lt_dlerror ());
                            strcat (rtd->resolve_error_buff, " - file : ");
                            strcat (rtd->resolve_error_buff, filename);
                            rtd->resolve_error = rtd->resolve_error_buff;
                            debug_call_message("%s\n",rtd->resolve_error);
                        }
                    }
                }
            }
        }
        if (dumperror && !loaded ) {
            fprintf (rtd->cob_err_file, "Warning: Not found load library : %s\n", s);
        }
    }
    cob_free (p);
    return loaded;
}

void 
cob_load_shared_lib (COB_RTD, char *fname)
{
    cob_load_shared_lib_1(rtd,fname, 1);
}

void
cob_init_call (COB_RTD) {
    char                *s;
    const char          *cobolit_dir;
    char                *p;
    int                 l;
    const struct system_table   *psyst;
    cit_runtime_t *     rtd_r0 = cob_enterprise_get_region_0(rtd);
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
    lt_dlhandle         libhandle;
#endif

#ifndef	USE_LIBDL
    lt_dlinit ();
#endif

    s = cob_bool_getenv ("COB_DEBUG_CALL");
    if ( s != NULL ) {
        cob_debug_call =1;
    }

    /* big enough for anything from libdl/libltdl */
    rtd->resolve_error_buff = cob_malloc (rtd, 256);

    rtd->call_table = (struct call_hash **)cob_malloc (rtd, sizeof (struct call_hash *) * HASH_SIZE);
    rtd->module_table = (struct module_hash **)cob_malloc (rtd, sizeof (struct module_hash *) * HASH_SIZE);
    rtd->call_match_exact_symbol = rtd->call_match_exact_case = rtd->call_match_upper_case = rtd->call_match_lower_case = 1;
    rtd->load_match_exact_case = rtd->load_match_upper_case = rtd->load_match_lower_case = 1;

    s = getenv ("COB_LOAD_CASE");
    if ( s != NULL ) {
        l = strlen(s);
        if ( strcasecmp (s, "LOWER") == 0 ) {
            rtd->name_convert = 1;
        } else if ( strcasecmp (s, "UPPER") == 0 ) {
            rtd->name_convert = 2;
        } else if (l >= 1) {
            if (s[0] == 'N' || s[0] == 'n') {
                rtd->load_match_exact_case = 0;
            }
            if (l >= 2) {
                if (s[1] == 'N' || s[1] == 'n') {
                    rtd->load_match_upper_case = 0;
                }
                if (l >= 3) {
                    if (s[2] == 'N' || s[2] == 'n') {
                        rtd->load_match_lower_case = 0;
                    }
                }
            }
        }
    }
    s = getenv ("COB_CALL_CASE");
    if ( s != NULL ) {
        l = strlen(s);
        if (l >= 1) {
            if (s[0] == 'N' || s[0] == 'n') {
                rtd->call_match_exact_case = 0;
                rtd->call_match_exact_symbol=0;
            }
            if (l >= 2) {
                if (s[1] == 'N' || s[1] == 'n') {
                    rtd->call_match_upper_case = 0;
                }
                if (l >= 3) {
                    if (s[2] == 'N' || s[2] == 'n') {
                        rtd->call_match_lower_case = 0;
                    }
                    if (l >= 4) {
                        if (s[3] == 'N' || s[3] == 'n') {
                            rtd->call_match_exact_symbol = 0;
                        }
                    }
                }
            }
        }
    }

    cobolit_dir = getenv ("COBOLITDIR");
    if (cobolit_dir == NULL)
    {
       cobolit_dir = COB_BASEDIR;
    }
    memset(rtd->call_namebuff, 0, sizeof(rtd->call_namebuff));
    cob_init_env_var (rtd, rtd->call_namebuff,     "COB_LIBRARY_PATH",     COB_LIBRARY_PATH,     COB_BASEDIR, cobolit_dir);
    strcat (rtd->call_namebuff, PATHSEPS);
    cob_init_env_var (rtd, rtd->call_namebuff,     "COB_LIBS_DIR",         COB_LIBS_DIR,         COB_BASEDIR, cobolit_dir);
    p = getenv ("PATH");
    if ( p ) {
        strcat (rtd->call_namebuff, PATHSEPS);
        strcat (rtd->call_namebuff, p);
    }
    p = getenv ("COB_CTREE_PATH");
    if ( p ) {
        strcat (rtd->call_namebuff, PATHSEPS);
        strcat (rtd->call_namebuff, p);
    }
    p = getenv ("COB_EXTFH_PATH");
    if ( p ) {
        strcat (rtd->call_namebuff, PATHSEPS);
        strcat (rtd->call_namebuff, p);
    }
#ifndef _WIN32
    strcat (rtd->call_namebuff, PATHSEPS);
    strcat (rtd->call_namebuff, cobolit_dir);
    strcat (rtd->call_namebuff, DIRSEPS);
    strcat (rtd->call_namebuff, "lib");
    strcat (rtd->call_namebuff, PATHSEPS);
#endif
    cob_set_library_path (rtd, rtd->call_namebuff);

#ifndef	COB_BORKED_DLOPEN
    rtd->mainhandle = lt_dlopen (NULL);
#endif

    s = cob_bool_getenv ("COB_FULL_CANCEL");
    if ( s != NULL ) {
        rtd->call_flag |= COB_FULL_CANCEL;
    }
    s = cob_bool_getenv ("COB_LOAD_PRIORITY");
    if ( s != NULL ) {
        rtd->call_flag |= COB_LOAD_PRIORITY;
    }

    s = cob_bool_getenv ("COB_CALL_CIT_FIRST");
    if ( s != NULL ) {
        rtd->call_flag |= COB_FLAG_DOT_CIT_FIRST;
    }

    s = getenv ("COB_PRE_LOAD");
    if ( s != NULL ) {
        cob_load_shared_lib (rtd, s);
    }
    for ( psyst = (struct system_table *)&system_tab[0]; psyst->syst_name; psyst++ ) {
        insert (rtd_r0, psyst->syst_name, psyst->syst_call, NULL, NULL, 0, NULL);
    }
}

void
cob_clear_call (COB_RTD) {
    struct call_hash       *p;
    struct call_hash        h;
    struct call_hash      **pp;
    struct module_hash     *m;
    struct module_hash      hm;
    struct module_hash    **mm;
    int i;


    if ( rtd->mainhandle ) {
        cob_dlclose(rtd->mainhandle);
        rtd->mainhandle=NULL;
    }


    if ( rtd->resolve_path ) {
        cob_free (rtd->resolve_path[0]);
        cob_free (rtd->resolve_path);
    }
    cob_free (rtd->call_buffer);

    cob_free (rtd->resolve_error_buff);
    rtd->resolve_error_buff = NULL;
    for ( i = 0; i < HASH_SIZE; i++ ) {
        mm = &(rtd->module_table[i]);
        for ( m = *mm; m; m = hm.next ) {
            hm = *m;

            /* this crash the multithread 
            if ( m->handle ) {
                lt_dlclose(m->handle);
            }
            */ 
            cob_free(m);                
        }
        rtd->module_table[i] =  NULL;
    }

    for ( i = 0; i < HASH_SIZE; i++ ) {
        pp = &(rtd->call_table[i]);
        for ( p = *pp; p; p = h.next ) {
            h = *p;
            if ( p->name ) {
                cob_free(p->name);
            }
            cob_free(p);                
        }
        rtd->call_table[i] =  NULL;
    }
#if	defined (_WIN32) || !defined (RTLD_DEFAULT)
    {
        struct data_list        *chkhandle, *p;

        for ( chkhandle = rtd->pre_handle; chkhandle; chkhandle = p) {
            p = chkhandle->next;
            cob_free (chkhandle);
        }
        rtd->pre_handle=NULL;
    }
#endif

    cob_free (rtd->call_table);
    rtd->call_table = NULL;
    cob_free (rtd->module_table);
    rtd->module_table = NULL;
}

void 
cobcancel   (const char *name) {
    COB_RTD = cob_get_rtd(); 
    cob_cancel(rtd,name);
}

int
cobcall (const char *name, const int argc, void **argv) {
    int i;
    union {
        void    *(*funcptr)();
        int     (*funcint)();
        void    *func_void;
    } unifunc;
    void            *pargv[16];
    char            filename[COB_SMALL_BUFF];
    cit_runtime_t   *rtd = cob_get_rtd(); 


    if ( unlikely(!rtd->cob_initialized) ) {
        if ( rtd->warning_disable ) {
            cob_init(rtd,0,NULL);
        } else {
            cob_runtime_error (rtd, "'cobcall' - Runtime has not been initialized");
            cob_stop_abend (rtd, COBRE_COBCALL_RTD);
        }
    }
    if ( argc < 0 || argc > 16 ) {
        cob_runtime_error (rtd, "Invalid number of arguments to 'cobcall'");
        cob_stop_abend (rtd, COBRE_COBCALL_PR_CNT);
    }
    if ( unlikely(!name) ) {
        cob_runtime_error (rtd, "NULL name parameter passed to 'cobcall'");
        cob_stop_abend (rtd, COBRE_COBCALL_NULL);
    }
    unifunc.func_void = cob_resolve (rtd, name);
    if ( unifunc.func_void == NULL ) {
        sprintf (filename, "%s.%s", name, COB_MODULE_EXT);
        unifunc.func_void = cob_resolve (rtd, filename);
        if ( unifunc.func_void == NULL ) {
            sprintf (filename, "%s.%s", name, MF_BIN_EXTENTION);
            unifunc.func_void = cob_resolve (rtd, filename);
            if ( unifunc.func_void == NULL ) {
                cob_call_error (rtd);
            }
        }
    }
    memset (pargv, 0, sizeof(pargv));
    rtd->cob_call_params = argc;
    for ( i = 0; i < argc; i++ ) {
        pargv[i] = argv[i];
    }
    return unifunc.funcint (pargv[0], pargv[1], pargv[2], pargv[3],
                            pargv[4], pargv[5], pargv[6], pargv[7],
                            pargv[8], pargv[9], pargv[10], pargv[11],
                            pargv[12], pargv[13], pargv[14], pargv[15]);
}

int
cobfunc (const char *name, const int argc, void **argv) {
    int             ret;
    cit_runtime_t   *rtd = cob_get_rtd(); 

    if ( unlikely(!rtd->cob_initialized) ) {
        if ( rtd->warning_disable ) {
            cob_init(rtd,0,NULL);
        } else {
            cob_runtime_error (rtd, "'cobfunc' - Runtime has not been initialized");
            cob_stop_abend (rtd, COBRE_COBFUNC_RTD);
        }
    }
    ret = cobcall (name, argc, argv);
    cob_cancel (rtd, name);
    return ret;
}

void *
cobsavenv (struct cobjmp_buf *jbuf) {

    if ( unlikely(!jbuf) ) {
        cit_runtime_t * rtd  = cob_get_rtd();
        cob_runtime_error (rtd, "NULL name parameter passed to 'cobsavenv'");
        cob_stop_abend (rtd, COBRE_COBSAVENV_NULL);
    }
    return jbuf->cbj_jmp_buf;
}

void *
cobsavenv2 (struct cobjmp_buf *jbuf, const int jsize) {
    int jtemp;

    /* Shut up compiler */
    jtemp = jsize;
    return cobsavenv (jbuf);
}

void
coblongjmp (struct cobjmp_buf *jbuf) {
    if ( unlikely(!jbuf) ) {
        cit_runtime_t   *rtd = cob_get_rtd(); 
        cob_runtime_error (rtd, "NULL name parameter passed to 'coblongjmp'");
        cob_stop_abend (rtd, COBRE_COBLONGJMP_NULL);
    }
    /*COBOL-IT*/
    /*
    if ( !rtd->cobjmp_primed ) {
        cob_runtime_error ("Call to 'coblongjmp' with no prior 'cobsetjmp'");
        cob_stop_run (1);
    }
    rtd->cobjmp_primed = 0;
    */
    longjmp (jbuf->cbj_jmp_buf, 1);
}

typedef struct cob_sig_info {
    int sig;
    int pri;
    void * prev_handler;
    char buff[30];
} cob_sig_info_t;

typedef int (*sigfunc_t)(int sig, void*p);

void * cobpostsighandler (int sig, int priority, sigfunc_t handler);
void * 
cobpostsighandler (int sig, int priority, sigfunc_t handler) {
    cit_runtime_t   *rtd = cob_get_rtd(); 

    cob_sig_info_t *cob_sig_info = cob_malloc(rtd, sizeof(cob_sig_info_t));
    cob_sig_info->sig = sig;
    cob_sig_info->pri = priority;
    cob_sig_info->prev_handler = (void*)signal(sig,(void*) handler);
    return(void*)cob_sig_info;
}

void cobremovesighandler (cob_sig_info_t *sighandler);
void 
cobremovesighandler (cob_sig_info_t *sighandler) {
    signal(sighandler->sig, sighandler->prev_handler);
}

void cob_call_close_all_handles (COB_RTD) {
    struct module_hash     *m;
    struct module_hash    **mm;
    int i;


    
    if ( rtd->mainhandle ) {
        cob_dlclose(rtd->mainhandle);
        rtd->mainhandle=NULL;
    }
    
    for ( i = 0; i < HASH_SIZE; i++ ) {
        mm = &(rtd->module_table[i]);
        for ( m = *mm; m; m = m->next ) {
            if ( m->handle ) {
                cob_dlclose(m->handle);
                m->handle=NULL;
            }
        }
    }
}


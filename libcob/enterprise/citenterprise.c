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
#include "config.h"
#include "defaults.h"
#include "globaldefine.h"
#include "defaults.h"
#include <e4c.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "iconv.h"
#ifdef	HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif
#ifdef	_MSC_VER
#  define WINDOWS_LEAN_AND_MEAN
#  include <windows.h>
#  include <io.h>
#  include <fcntl.h>
#  include <sys/types.h>
#  include <sys/timeb.h>
#  undef	HAVE_SIGNAL_H
#endif

#ifdef	HAVE_SIGNAL_H
#  include <signal.h>
#endif

#ifdef	HAVE_LOCALE_H
#  include <locale.h>
#endif

#include "common.h"
#include "coblocal.h"
#ifndef WORDS_BIGENDIAN
#  include "byteswap.h"
#endif
#include "debug.h"
#include "debugdb.h"
#include "move.h"
#include "numeric.h"
#include "termio.h"
#include "fileio.h"
#include "call.h"
#include "screenio.h"
#include "intrinsic.h"
#include "lib/gettext.h"
#ifdef __GNUC__
#  include "malloc.h"
#endif
#include "citkey.h"
#include "citenterprise.h"
#include "a2e.h"
#ifdef _MSC_VER
#  define XML_STATIC 1
#endif
#include <expat.h>

#ifdef WITH_ICU
#  ifdef WITH_ICU_GENERIC
#     define UErrorCode int
#     define U_ZERO_ERROR 0
void* ucnv_open(const char *n, UErrorCode *e);
void   ucnv_close(void *c);
int    ucnv_countAvailable(void);
int    ucnv_countAliases(const char *n, UErrorCode *e);
const char* ucnv_getAlias(const char *n, int i, UErrorCode *e);
const char* ucnv_getAvailableName(int i);
const char* ucnv_getName(void *c, UErrorCode *e);
void ucnv_convertEx(void *targetCnv, void *sourceCnv,
                    char **target, const char *targetLimit,
                    const char **source, const char *sourceLimit,
                    char *pivotStart, char **pivotSource,
                    char **pivotTarget, const char *pivotLimit,
                    int reset, int flush,
                    UErrorCode *pErrorCode);
char* u_errorName(UErrorCode e);
typedef unsigned short UChar;
int u_strToUpper(UChar *dest, int destCapacity, const UChar *src, int srcLength, const char *locale, UErrorCode *e);
int u_strToLower(UChar *dest, int destCapacity, const UChar *src, int srcLength, const char *locale, UErrorCode *e);
#     define U_FAILURE(x) ((x)>U_ZERO_ERROR)
#  else
#     ifndef	_MSC_VER
#     ifdef     WITH_ICU_CIT
#        define U_LIB_SUFFIX_C_NAME _cit
#     endif
#     endif
#     include "unicode/ucnv.h"
#     include "unicode/ustring.h"
#  endif
#endif

void
cob_set_context_mode(COB_RTD, int imode)
{
   rtd->cob_context_info.cob_context_mode = imode;
}

void
cob_set_context_appli_prefix_field(COB_RTD, cob_field *f)
{
   cob_display_environment(rtd, f);
   cob_set_context_appli_prefix(rtd, rtd->cob_local_env);
}

static void
get_context_file_name(COB_RTD, char *buffer)
{
   char buff[1024];

   buffer[0] = 0;
   buff[0] = 0;
   if (rtd->cob_context_info.cob_context_server_prefix)
   {
      strcpy(buffer, rtd->cob_context_info.cob_context_server_prefix);
   }
   if (!rtd->cob_context_info.cob_context_appli_prefix)
   {
      sprintf(buff, "%s.ctx", rtd->current_module->module_name);
   }
   else
   {
      sprintf(buff, "%s%s.ctx", rtd->cob_context_info.cob_context_appli_prefix, rtd->current_module->module_name);
   }
   strcat(buffer, buff);
}

void
cob_open_context_file(COB_RTD, char *key)
{
   char fname[1024];
   char buff[1024];
   int key_size =  strlen(key);

   get_context_file_name(rtd, fname);

   switch (rtd->cob_context_info.cob_context_mode & 0x0F)
   {
      case COB_CONTEXT_RELOAD:
         rtd->cob_context_info.cob_context_file = fopen(fname, "rb");
         if (rtd->cob_context_info.cob_context_file)
         {
            if ((fread(buff, key_size, 1, rtd->cob_context_info.cob_context_file) == 1) && (memcmp(key, buff, key_size) == 0))
            {
               return;
            }
            else
            {
               cob_runtime_error(rtd, "Invalid context file %s for reading", fname);
               cob_stop_abend(rtd, COBRE_CTX_RD_INV);
            }
         }
         else
         {
            cob_runtime_error(rtd, "Can't open context file %s for reading", fname);
            cob_stop_abend(rtd, COBRE_CTX_RD_OPEN);
         }
         break;
      case COB_CONTEXT_SAVE:
         rtd->cob_context_info.cob_context_file = fopen(fname, "wb");
         if (rtd->cob_context_info.cob_context_file)
         {
            if (fwrite(key, key_size, 1, rtd->cob_context_info.cob_context_file) != 1)
            {
               cob_runtime_error(rtd, "Can't write context file %s key ", fname);
               cob_stop_abend(rtd, COBRE_CTX_WRITE);
            }
         }
         else
         {
            cob_runtime_error(rtd, "Can't open context file %s for writing", fname);
            cob_stop_abend(rtd, COBRE_CTX_WR_OPEN);
         }
         break;
      default:
         cob_runtime_error(rtd, "Unexpected context mode %d", rtd->cob_context_info.cob_context_mode);
         cob_stop_abend(rtd, COBRE_CTX_MODE);
         break;
   }
}

void
cob_close_context_file(COB_RTD)
{
   char fname[1024];

   if (rtd->cob_context_info.cob_context_file)
   {
      fclose(rtd->cob_context_info.cob_context_file);
      /* printf("cob close context %d %d \n" , cob_context_info.cob_auto_erase_context_file, cob_context_info.cob_context_mode);*/

      if (rtd->cob_context_info.cob_auto_erase_context_file &&
          (rtd->cob_context_info.cob_context_mode == COB_CONTEXT_RELOAD))
      {
         get_context_file_name(rtd, fname);
         /*printf("Erasing %s\n", fname);*/
         unlink(fname);
      }
   }
   rtd->cob_context_info.cob_context_file = NULL;
}

void
cob_read_context_data(COB_RTD, void *p, int size)
{
   if (rtd->cob_context_info.cob_context_file == NULL)
   {
      cob_runtime_error(rtd, "Context file not open");
      cob_stop_abend(rtd, COBRE_CTX_NOT_OPEN);
   }
   if (fread(p, size, 1, rtd->cob_context_info.cob_context_file) != 1)
   {
      char fname[1024];
      get_context_file_name(rtd, fname);
      cob_runtime_error(rtd, "Can't read context file %s data", fname);
      cob_stop_abend(rtd, COBRE_CTX_READ);
   }
}

void
cob_write_context_data(COB_RTD, void *p, int size)
{
   if (rtd->cob_context_info.cob_context_file == NULL)
   {
      cob_runtime_error(rtd, "Context file not open");
      cob_stop_abend(rtd, COBRE_CTX_NOT_OPEN);
   }
   if (fwrite(p, size, 1, rtd->cob_context_info.cob_context_file) != 1)
   {
      char fname[1024];
      get_context_file_name(rtd, fname);
      cob_runtime_error(rtd, "Can't write context file %s data", fname);
      cob_stop_abend(rtd, COBRE_CTX_WRITE);
   }
}

void
cob_init_context_save(COB_RTD, cob_field *exit_flag)
{
   cob_close_context_file(rtd);
   if (exit_flag ==  &cob_zero)
   {
      cob_set_context_mode(rtd, COB_CONTEXT_SAVE | COB_CONTEXT_CONTINUE);
   }
   else
   {
      cob_set_context_mode(rtd, COB_CONTEXT_SAVE);
   }
   rtd->cob_context_info.cob_next_context_father  = 0;
}

void
cob_set_auto_erase_context_file(COB_RTD, int on)
{
   rtd->cob_context_info.cob_auto_erase_context_file = on;
}

void
cob_free_module_allocated(COB_RTD, cob_module *module)
{
   struct data_list *l, *n;
   if (module && module->module_version > 1)
   {
      if (module->cbl_allocated_list_ptr)
      {
         module->cbl_allocated_list = *(module->cbl_allocated_list_ptr);
      }
      l = module->cbl_allocated_list;
      while (l)
      {
         cob_free_cbl_allocation(rtd, l->data);
         l = l->next;
      }
      l = module->cbl_allocated_list;
      module->cbl_allocated_list = NULL;
      if (module->cbl_allocated_list_ptr)
      {
         *(module->cbl_allocated_list_ptr) = module->cbl_allocated_list;
      }
      while (l)
      {
         n = l;
         l = l->next;
         free(n);
      }
   }
}

void
cob_enterprise_free_rtd_allocated(COB_RTD)
{
   struct exit_handlerlist *h;
   struct handlerlist *e, *p;
   struct data_list *l, *n;
   cob_module *m;
   if (rtd)
   {
      m = rtd->current_module;
      while (m)
      {
         cob_free_module_allocated(rtd, m);
         cob_runtime_debugger_close_module(rtd, m);
         m = COB_NEXT_MODULE(m);
      }
      l = rtd->cbl_allocated_list;
      while (l)
      {
         cob_free_cbl_allocation(rtd, l->data);
         l = l->next;
      }
      l = rtd->cbl_allocated_list;
      rtd->cbl_allocated_list = NULL;
      while (l)
      {
         n = l;
         l = l->next;
         free(n);
      }
   }
   if (rtd->exit_hdlrs != NULL)
   {
      h = rtd->exit_hdlrs;
      rtd->exit_hdlrs = NULL;
      while (h != NULL)
      {
         e = (void *)h;
         h = h->next;
         cob_free(e);
      }
   }
   if (rtd->hdlrs != NULL)
   {
      p = rtd->hdlrs;
      while (p != NULL)
      {
         e = p;
         p = p->next;
         cob_free(e);
      }
      rtd->hdlrs = NULL;
   }
}

int
CBL_ALLOC_MEM(void  *pptr, int size, int flag)
{
   COB_RTD = cob_get_rtd();
   return rtd_CBL_ALLOC_MEM(rtd, pptr, size, flag);
}

int
rtd_CBL_ALLOC_MEM(COB_RTD, void  *pptr, int size, int flag)
{

   int shared, thread, module;
   void *p;
   COB_CHK_PARMS(CBL_ALLOC_MEM, 3);

   shared = flag & 1;
   module = !(flag & (1 << 2));
   thread = flag & (1 << 3);

   if (shared)
   {
      return 181;
   }
   p = cob_malloc_cbl_allocation(rtd, size, thread, module);
   memcpy(pptr, &p, sizeof(void *));
   if (!p)
   {
      return 157;
   }
   return (0);
}

int
CBL_FREE_MEM(void  *ptr)
{
   COB_RTD = cob_get_rtd();
   return rtd_CBL_FREE_MEM(rtd, ptr);
}

int
rtd_CBL_FREE_MEM(COB_RTD, void  *ptr)
{

   COB_CHK_PARMS(CBL_ALLOC_MEM, 1);

   cob_free_cbl_allocation(rtd, ptr);
   return (0);
}


#ifdef _MSC_VER
#  define COB_THREAD __declspec( thread )
#  define HAVE__THEAD_ATTR 1
COB_THREAD cit_runtime_t *cob_runtime_data = NULL;
static cit_runtime_t cob_runtime_data_xp = { 0 };
static OSVERSIONINFO osvi = { 0 };
#else
#  ifdef HAVE__THEAD_ATTR
__thread cit_runtime_t *cob_runtime_data = NULL;
#  elif defined(HAVE_PTHREAD_H)
#     include <pthread.h>
static pthread_key_t tlsKey;
static pthread_once_t tlsIndex_once = PTHREAD_ONCE_INIT;
#  else
cit_runtime_t *cob_runtime_data = NULL;
#  endif
#endif
#define RTD_ALLOCATE_SIGNATURE  0x1011

static void
cob_init_rtd_key(void)
{
#  if defined(HAVE_PTHREAD_H) && !defined(HAVE__THEAD_ATTR)
   pthread_key_create(&tlsKey, NULL);
#  else
   if (cob_runtime_data == NULL)
   {
      cob_runtime_data = calloc(1, sizeof(cit_runtime_t));
      cob_runtime_data->cob_rtd_allocated = RTD_ALLOCATE_SIGNATURE;
      cob_runtime_data->rtd_region_data[0] = cob_runtime_data;
   }
#  endif
}

cit_runtime_t*
cob_get_rtd(void)
{
   cit_runtime_t *base_runtime_data;
   unsigned int current_rtd_region;
#  if defined(HAVE_PTHREAD_H) && !defined(HAVE__THEAD_ATTR)
   int err;

   (void)pthread_once(&tlsIndex_once, cob_init_rtd_key);
   base_runtime_data = (cit_runtime_t *const)pthread_getspecific(tlsKey);;
   if (base_runtime_data == NULL)
   {
      base_runtime_data = calloc(1, sizeof(cit_runtime_t));
      base_runtime_data->cob_rtd_allocated = RTD_ALLOCATE_SIGNATURE;
      base_runtime_data->rtd_region_data[0] = base_runtime_data;
      if (!base_runtime_data)
      {
         fprintf(stderr, "cob_get_rtd :Memory allocation failed\n");
         exit(-1);
      }
      if ((err = pthread_setspecific(tlsKey, base_runtime_data)) != 0)
      {
         fprintf(stderr, "Internal Error: pthread_setspecific (key=%d) failed (error: 0%d)\n", tlsKey, err);
         exit(-1);
      }
   }
#  else
#     ifdef _MSC_VER
   if (osvi.dwOSVersionInfoSize == 0)
   {
      osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      GetVersionEx(&osvi);
   }
   if (osvi.dwMajorVersion < 6)
   {
      base_runtime_data =  &cob_runtime_data_xp;
      base_runtime_data->rtd_region_data[0] = base_runtime_data;

   }
   else

#     endif
   {
      if (cob_runtime_data == NULL)
      {
         cob_init_rtd_key();
      }
      base_runtime_data = (cit_runtime_t *const)cob_runtime_data;
   }
#  endif
   current_rtd_region = base_runtime_data->current_rtd_region;
   if (current_rtd_region >= COB_MAX_RTD_REGION)
   {
      current_rtd_region = COB_MAX_RTD_REGION - 1;
   }
   if (base_runtime_data->rtd_region_data[current_rtd_region] == NULL)
   {
      base_runtime_data->rtd_region_data[current_rtd_region] = calloc(1, sizeof(cit_runtime_t));
      base_runtime_data->rtd_region_data[current_rtd_region]->cob_rtd_allocated = RTD_ALLOCATE_SIGNATURE;
      base_runtime_data->rtd_region_data[current_rtd_region]->rtd_region_data[0] = base_runtime_data->rtd_region_data[current_rtd_region];
      base_runtime_data->rtd_region_data[current_rtd_region]->parent_rtd = base_runtime_data;
      base_runtime_data->rtd_region_data[current_rtd_region]->rtd_region_nr = current_rtd_region;
   }
   return base_runtime_data->rtd_region_data[current_rtd_region];
}

/*
static int 
cob_enterprise_count_module_use (COB_RTD, void*fct)
{
    int r;
    int cnt = 0;
    cit_runtime_t * rtd_r0 = cob_enterprise_get_region_0(rtd);
    for (r = 0; r < COB_MAX_RTD_REGION; r++) {
        cit_runtime_t * rtd_r = rtd_r0->rtd_region_data[r];
        if (rtd_r) {
            struct module_hash  *p;
            struct module_hash  **pp;
            int i;
            for ( i = 0; i < HASH_SIZE; i++ ) {
                pp = &(rtd_r->module_table[i]);
                for ( p = *pp; p; pp = &(p->next), p = p->next ) {
                    if ( p->func == fct ) {
                        cnt++;
                        goto NextMod;
                    }
                }
            }
            NextMod:
                i++;
        }
    }
    return cnt;
}

static void 
cob_enterprise_transfert_module_handle (COB_RTD, void*func, void*handle)
{
    int r;
    cit_runtime_t * rtd_r0 = cob_enterprise_get_region_0(rtd);
    for (r = 0; r < COB_MAX_RTD_REGION; r++) {
        cit_runtime_t * rtd_r = rtd_r0->rtd_region_data[r];
        if (rtd_r && rtd_r != rtd) {
            struct module_hash  *p;
            struct module_hash  **pp;
            int i;
            for ( i = 0; i < HASH_SIZE; i++ ) {
                pp = &(rtd_r->module_table[i]);
                for ( p = *pp; p; pp = &(p->next), p = p->next ) {
                    if ( p->func == func && p->flag_is_valid && !p->name) {
                        p->handle = handle;
                        p->is_unloadable =1;
                        return ;
                    }
                }
            }
        }
    }
}
*/

void
cob_enterprise_clear_all_module_cache(COB_RTD)
{
   int r;
   cit_runtime_t *rtd_r0 = cob_enterprise_get_region_0(rtd);
   for (r = 0; r < COB_MAX_RTD_REGION; r++)
   {
      cit_runtime_t *rtd_r = rtd_r0->rtd_region_data[r];
      if (rtd_r)
      {
         struct module_hash  *p;
         struct module_hash  **pp;
         int i;
         rtd_r->last_call_resolved_func = NULL;
         for (i = 0; i < HASH_SIZE; i++)
         {
            pp = &(rtd_r->module_table[i]);
            for (p = *pp; p; pp = &(p->next), p = p->next)
            {
               if (!p->handle && !p->cancel && p->func)
               {
                  cob_clear_module_hash(p);
               }
            }
         }
      }
   }
}


static int
cob_enterprise_invalidate_module_use(COB_RTD, void *fct, void *cancel)
{
   int                 i;
   struct call_hash    *c;
   struct module_hash  *p;
   struct module_hash  **pp;
   for (i = 0; i < HASH_SIZE; i++)
   {
      pp = &(rtd->module_table[i]);
      for (p = *pp; p; pp = &(p->next), p = p->next)
      {
         if (p->func == fct)
         {
            cob_clear_module_hash(p);
         }
         if (p->cancel == cancel)
         {
            cob_clear_module_hash(p);
         }
      }
   }
   for (i = 0; i < HASH_SIZE; i++)
   {
      for (c = rtd->call_table[i]; c; c = c->next)
      {
         if ((c->func == fct))
         {
            c->flag_is_valid = 0;
         }
      }
   }
   return 0;
}

void
cob_enterprise_cancel_region(cit_runtime_t *const rtd_current, unsigned int region, int full_cancel)
{
   struct module_hash        *p;
   struct module_hash        h;
   struct module_hash        **pp;
#  if	defined (_MSC_VER) || !defined (RTLD_DEFAULT)
   struct data_list    *chkhandle;
   struct data_list    **p_chkhandle;
#  endif
   int    i,  current_region;
   union
   {
       int (*cancel_func)(int, ...);
       void    *cancel_void;
   } unicanc;
   cit_runtime_t *rtd_region;
   cit_runtime_t *rtd_r0 = cob_enterprise_get_region_0(rtd_current);


   rtd_r0->last_call_resolved_func = NULL;
   current_region = cob_enterprise_get_current_region(rtd_current);

   rtd_region = cob_enterprise_set_current_region(rtd_current, region);

   for (i = 0; i < HASH_SIZE; i++)
   {
      pp = &(rtd_r0->module_table[i]);
      for (p = *pp; p; pp = &(p->next), p = p->next)
      {
         if (p->flag_is_valid)
         {
            if (p->cancel)
            {
               unicanc.cancel_void = p->cancel;
               unicanc.cancel_func(-1, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, NULL);
            }
         }
      }
   }

   if (full_cancel)
   {
      for (i = 0; i < HASH_SIZE; i++)
      {
         pp = &(rtd_r0->module_table[i]);
         for (p = *pp; p; p = h.next)
         {
            h = *p;
            if (p->handle)
            {
               if (p->func)
               {
                  if (cob_is_module_unloadable(p, region))
                  {
                     cob_dlclose(p->handle);
#                    if	defined (_MSC_VER) || !defined (RTLD_DEFAULT)
                     p_chkhandle = &(rtd_r0->pre_handle);
                     for (chkhandle = *p_chkhandle; chkhandle; p_chkhandle = &chkhandle->next, chkhandle = chkhandle->next)
                     {
                        if (chkhandle->data == p->handle)
                        {
                           *p_chkhandle = chkhandle->next;
                           cob_free(chkhandle);
                           break;
                        }
                     }
#                    endif
                     cob_enterprise_invalidate_module_use(rtd_r0, p->func, p->cancel);
                     cob_clear_module_hash(p);
                  }
                  else
                  {
                     p->region_use[region] = 0;
                  }

               }
            }
         }
      }
   }

   rtd_region->current_module = NULL;
   cob_enterprise_set_current_region(rtd_r0, current_region);
   cob_enterprise_free_rtd_allocated(rtd_region);
   if (full_cancel)
   {
      cob_free_all_module_storage(rtd_region);
      cob_enterprise_clear_all_module_cache(rtd_r0);
      cob_profiling_unregister_all(rtd_r0);
   }
}

static void
free_rtd_data(COB_RTD)
{
   int i;
   if (rtd)
   {
      for (i = 1; i < COB_MAX_RTD_REGION; i++)
      {
         free_rtd_data(rtd->rtd_region_data[i]);
      }
      if (rtd->cob_rtd_allocated == RTD_ALLOCATE_SIGNATURE)
      {
         free(rtd);
      }
   }
}

cit_runtime_t*
cob_enterprise_get_region_0(COB_RTD)
{
   cit_runtime_t *myrtd = rtd;
   while (myrtd->parent_rtd)
   {
      myrtd = myrtd->parent_rtd;
   }
   return myrtd;
}

unsigned int
cob_enterprise_get_current_region(COB_RTD)
{
   cit_runtime_t *myrtd = cob_enterprise_get_region_0(rtd);
   return myrtd->current_rtd_region;
}

cit_runtime_t*
cob_enterprise_set_current_region(COB_RTD, unsigned int region)
{
   cit_runtime_t *myrtd = rtd;

   if (region >= COB_MAX_RTD_REGION)
   {
      return NULL;
   }
   myrtd = cob_enterprise_get_region_0(rtd);
   myrtd->current_rtd_region = region;
   myrtd = cob_get_rtd();
   if (myrtd)
   {
      cob_init(myrtd, 0, NULL);
      cob_runtime_debugger_switch_region(myrtd);
      return myrtd;
   }
   else
   {
      return NULL;
   }
}

cit_runtime_t*
cob_enterprise_get_region(COB_RTD, unsigned int region)
{
   cit_runtime_t *myrtd = rtd;

   if (region >= COB_MAX_RTD_REGION)
   {
      return NULL;
   }
   myrtd = cob_enterprise_get_region_0(rtd);
   myrtd = myrtd->rtd_region_data[region];
   return myrtd;
}

void
cob_enterprise_terminate_region(COB_RTD)
{
   cit_runtime_t *myrtd = rtd;
   int i;
   myrtd = cob_enterprise_get_region_0(rtd);
   for (i = 1; i < COB_MAX_RTD_REGION; i++)
   {
      if (rtd->rtd_region_data[i])
      {
         cob_terminate_exec(rtd->rtd_region_data[i]);
      }
   }
}


void
cob_set_rtd(COB_RTD)
{
#  if defined(HAVE_PTHREAD_H) && !defined(HAVE__THEAD_ATTR)
   int err;
   cit_runtime_t *cob_runtime_data;
   (void)pthread_once(&tlsIndex_once, cob_init_rtd_key);
   cob_runtime_data = (cit_runtime_t *)cob_get_rtd();
#  endif
#  ifdef _MSC_VER
   if (osvi.dwMajorVersion >= 6)
#  endif
   {
      free_rtd_data(cob_runtime_data);
      cob_runtime_data = rtd;
   }
#  if defined(HAVE_PTHREAD_H) && !defined(HAVE__THEAD_ATTR)
   if ((err = pthread_setspecific(tlsKey, cob_runtime_data)) != 0)
   {
      fprintf(stderr, "Internal Error: pthread_setspecific (key=%d) failed (error: 0%d)\n", tlsKey, err);
      exit(-1);
   }
#  endif
}
#define free_str(x) if(x){ free(x); x=NULL;}
void
cob_rtd_tidy(COB_RTD)
{
   int i;

   if (rtd)
   {
      if (rtd->cob_initialized)
      {
         debug_clear_field_cache(rtd);
         cob_terminate_exec(rtd);
         cob_free_all_module_storage(rtd);
         free_str(rtd->runtime_err_str);
         free_str(rtd->locale_save);
         if (rtd->cob_argv_allocated)
         {
            free_str(rtd->cob_argv);
            rtd->cob_argv_allocated = 0;
         }

         cob_clear_numeric(rtd);
         for (i = 0; i < DUPLICAT_COUNT; i++)
         {
            free_str(rtd->duplicat_field[i].data);
         }
         free_str(rtd->auto_break_modules);
         free_str(rtd->cob_context_info.cob_context_server_prefix);
         free_str(rtd->cob_context_info.cob_context_appli_prefix);
         free_str(rtd->cmdlcopy);
         cob_clear_strings(rtd);
         cob_clear_move(rtd);
         cob_clear_intrinsic(rtd);
         cob_clear_fileio(rtd);
         cob_clear_termio(rtd);
         cob_clear_call(rtd);
         Clear_a2e(rtd);
         rtd->debug_codepage_iconv_cd = cob_enterprise_close_one_icu(rtd->debug_codepage_iconv_cd);
         rtd->national_of_current_iconv_cd = cob_enterprise_close_one_icu(rtd->national_of_current_iconv_cd);
         rtd->display_of_current_iconv_cd = cob_enterprise_close_one_icu(rtd->display_of_current_iconv_cd);
         rtd->utf8_codepage_iconv_cd = cob_enterprise_close_one_icu(rtd->utf8_codepage_iconv_cd);

      }
      rtd->cob_initialized = 0;
   }
}
#ifdef WITH_ICU
static void*
cob_enterprise_open_one_icu_prefix(COB_RTD, const char *name, const char *prefix, UErrorCode   *ret_status)
{
   UErrorCode      status = U_ZERO_ERROR;
   void *conv;
   char buffer[256];

   if (prefix)
   {
      strncpy(buffer, prefix, sizeof(buffer));
      strncat(buffer, name, sizeof(buffer));
      conv = ucnv_open(buffer, &status);
   }
   else
   {
      conv = ucnv_open(name, &status);
   }
   if (ret_status)
   {
      *ret_status = status;
   }
   return conv;
}

void*
cob_enterprise_open_one_icu(COB_RTD, const char *name)
{
   UErrorCode      status = U_ZERO_ERROR;
   void *conv;
   conv = cob_enterprise_open_one_icu_prefix(rtd, name, NULL, &status);
   if (!conv)
   {
      conv = cob_enterprise_open_one_icu_prefix(rtd, name, "ibm-", NULL);
      if (!conv)
      {
         conv = cob_enterprise_open_one_icu_prefix(rtd, name, "cp", NULL);
         if (!conv)
         {
            conv = cob_enterprise_open_one_icu_prefix(rtd, name, "windows-", NULL);
         }
      }
   }
   if (!conv)
   {
      if (rtd && rtd->cob_err_file)
      {
         fprintf(rtd->cob_err_file, "Internal Error: can't open converteur %s : %s\n", name, u_errorName(status));
      }
      else
      {
         fprintf(stderr, "Internal Error: can't open converteur %s : %s\n", name, u_errorName(status));
      }
   }
   return conv;
}

void*
cob_enterprise_close_one_icu(void *code_cp)
{
   if (code_cp)
   {
      ucnv_close(code_cp);
   }
   return NULL;
}

int
cob_enterprise_open_icu(COB_RTD, const char *name)
{
   if (!runtime_check_enterprise_key())
   {
      fprintf(stderr, "\n\n");
      fprintf(stderr, "Cobol UTF8 and NATIONAL support available in Enterprise edition only\n");
      return 0;
   }
   if (!rtd->debug_codepage_iconv_cd)
   {
      rtd->debug_codepage_iconv_cd = cob_enterprise_open_one_icu(rtd, rtd->debug_codepage);
      if (!rtd->debug_codepage_iconv_cd)
      {
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
         return 0;
      }
   }
   if (!rtd->utf8_codepage_iconv_cd)
   {
      rtd->utf8_codepage_iconv_cd = cob_enterprise_open_one_icu(rtd, "UTF-8");
      if (!rtd->utf8_codepage_iconv_cd)
      {
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
         return 0;
      }
   }
   if (rtd->national_of_current_iconv_cd)
   {
      if ((cob_utf16_cp_id(rtd) != rtd->current_utf16_cp_id))
      {
         ucnv_close(rtd->national_of_current_iconv_cd);
         rtd->national_of_current_iconv_cd = NULL;
      }
   }
   if (!rtd->national_of_current_iconv_cd)
   {
      rtd->current_utf16_cp_id = cob_utf16_cp_id(rtd);
      rtd->national_of_current_iconv_cd = cob_enterprise_open_one_icu(rtd, rtd->current_utf16_cp_id);
      if (!rtd->national_of_current_iconv_cd)
      {
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
         return 0;
      }
   }
   if (rtd->display_of_current_iconv_cd && rtd->display_of_current_cp)
   {
      if (strcmp(name, rtd->display_of_current_cp) != 0)
      {
         ucnv_close(rtd->display_of_current_iconv_cd);
         free(rtd->display_of_current_cp);
         rtd->display_of_current_iconv_cd = NULL;
         rtd->display_of_current_cp = NULL;
      }
   }
   if (!rtd->display_of_current_iconv_cd)
   {
      rtd->display_of_current_iconv_cd = cob_enterprise_open_one_icu(rtd, name);
      rtd->display_of_current_cp = strdup(name);
      if (!rtd->display_of_current_iconv_cd)
      {
         free(rtd->display_of_current_cp);
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
         rtd->display_of_current_cp = NULL;
         return 0;
      }
   }
   return 1;
}

void
cob_list_codepage(void)
{
   int a, i = 0;
   int n = ucnv_countAvailable();
   const char *s;
   UErrorCode err = U_ZERO_ERROR;
   for (; i <= n; i++)
   {
      s = ucnv_getAvailableName(i);
      if (s)
      {
         int num_aliases;
         err = U_ZERO_ERROR;
         num_aliases = ucnv_countAliases(s, &err);
         if (num_aliases <= 1)
         {
            printf("%s\n", s);
         }
         else
         {
            printf("%s : ", s);
            for (a = 0; a < num_aliases; ++a)
            {
               const char *alias = ucnv_getAlias(s, a, &err);
               printf("%s ", alias);
            }
            printf("\n");
         }
      }
   }
}

void
cob_check_codepage(char *name)
{
   UErrorCode err = U_ZERO_ERROR;
   void *p = cob_enterprise_open_one_icu(NULL, name);
   if (p)
   {
      printf("%s is found as %s\n", name, ucnv_getName(p, &err));
      if (U_FAILURE(err))
      {
         fprintf(stderr, "Internal Error: can't retreive name %s : %s\n", name, u_errorName(err));
      }
   }
}
#endif

void cob_enterprise_free_national(COB_RTD)
{
   if (rtd->national_of_current_iconv_cd)
   {
#     ifdef WITH_ICU
      ucnv_close(rtd->national_of_current_iconv_cd);
#     else
      iconv_close(rtd->national_of_current_iconv_cd);
#     endif
      rtd->national_of_current_iconv_cd = NULL;
   }
   if (rtd->national_of_current_cp)
   {
      cob_free(rtd->national_of_current_cp);
   }
   if (rtd->display_of_current_iconv_cd)
   {
#     ifdef WITH_ICU
      ucnv_close(rtd->display_of_current_iconv_cd);
#     else
      iconv_close(rtd->display_of_current_iconv_cd);
#     endif
      rtd->display_of_current_iconv_cd = NULL;
   }
   if (rtd->display_of_current_cp)
   {
      cob_free(rtd->display_of_current_cp);
   }
}

int cob_enterprise_display_to_debugcp(COB_RTD, char *s, char *d,
                                      char *display_cp_buffer, void *debug_iconv_cd,
                                      size_t ssz, size_t dsz)
{
#  ifdef WITH_ICU
   char *src = s;
   char *dst = d;
   UErrorCode status = 0;

   if (cob_enterprise_open_icu(rtd, display_cp_buffer))
   {
      ucnv_convertEx(debug_iconv_cd, rtd->display_of_current_iconv_cd,
                     &dst, dst + dsz, (const char **)&src, src + ssz, NULL, NULL, NULL, NULL, 1, 1, &status);
      if (U_FAILURE(status))
      {
         if (rtd->cob_err_file)
         {
            fprintf(rtd->cob_err_file, "Internal Error: can't convert to utf-8 %s : %s\n", s, u_errorName(status));
         }
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
      }
   }
   return dst - d;

#  else
#     error "cob_enterprise_display_to_utf8 not implemented with iconv"
#  endif
}

int cob_enterprise_debugcp_to_display(COB_RTD, char *s, char *d, char *display_cp_buffer, void *debug_iconv_cd, size_t ssz, size_t dsz)
{
#  ifdef WITH_ICU
   char *src = s;
   char *dst = d;
   UErrorCode status = 0;

   if (cob_enterprise_open_icu(rtd, display_cp_buffer))
   {
      ucnv_convertEx(rtd->display_of_current_iconv_cd, debug_iconv_cd,
                     &dst, dst + dsz, (const char **)&src, src + ssz, NULL, NULL, NULL, NULL, 1, 1, &status);
      if (U_FAILURE(status))
      {
         if (rtd->cob_err_file)
         {
            fprintf(rtd->cob_err_file, "Internal Error: can't convert from utf-8 %s : %s\n", s, u_errorName(status));
         }
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
      }
   }
   return dst - d;

#  else
#     error "cob_enterprise_display_to_utf8 not implemented with iconv"
#  endif
}


static unsigned char* belec(unsigned  char *p, int ssize, int isCopy)
{
   unsigned char temp;
   int i;
   unsigned char *src = p;
   if (isCopy)
   {
      src = malloc(ssize);
      for (i = 0; i < ssize; i++)
      {
         src[i] = *p++;
      }
   }
   for (i = 0; i < ssize - 1; i += 2)
   {
      temp = src[i];
      src[i] = src[i + 1];
      src[i + 1] = temp;
   }
   return src;
}
int cob_enterprise_int_uppercase_lowercase(COB_RTD, unsigned char *d, unsigned  char *s, size_t dsize, size_t ssize, int isUpperCase)
{
   int length = 0;
   unsigned char *src = s;

#  ifdef WITH_ICU
   UErrorCode status = 0;
#     ifndef WORDS_BIGENDIAN
   if (!rtd->current_module->utf16_le)
   {
      src = belec(src, ssize, 1);
   }
#     else
   if (rtd->current_module->utf16_le)
   {
      src = belec(src, ssize, 1);
   }
#     endif
   if (isUpperCase)
   {
      length = u_strToUpper((UChar *)d, dsize / 2, (UChar *)src, ssize / 2, NULL, &status);
   }
   else
   {
      length = u_strToLower((UChar *)d, dsize / 2, (UChar *)src, ssize / 2, NULL, &status);
   }
   length *= 2;
   if (U_FAILURE(status) || (length > dsize))
   {
      if (rtd->cob_err_file)
      {
         if (U_FAILURE(status)) fprintf(rtd->cob_err_file, "Internal Error: can not %s %s : %s\n", (isUpperCase) ? "uppercase" : "lowercase", s, u_errorName(status));
         else fprintf(rtd->cob_err_file, "Internal Error: can not %s %s : %s\n", (isUpperCase) ? "uppercase" : "lowercase", s, "Destination's buffer overflow");
      }
      cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
   }
#     ifndef WORDS_BIGENDIAN
   if (!rtd->current_module->utf16_le)
   {
      d = belec(d, length, 0);
      free(src);
   }
#     else
   if (rtd->current_module->utf16_le)
   {
      d = belec(d, length, 0);
      free(src);
   }
#     endif
#  endif
   return length;
}
int cob_enterprise_intr_national_of(COB_RTD, char *s, char *d, char *cp_buffer, size_t ssz, size_t dsz)
{
#  ifdef WITH_ICU
   char *src = s;
   char *dst = d;
   UErrorCode status = 0;

   if (cob_enterprise_open_icu(rtd, cp_buffer))
   {
      ucnv_convertEx(rtd->national_of_current_iconv_cd, rtd->display_of_current_iconv_cd,
                     &dst, dst + dsz, (const char **)&src, src + ssz, NULL, NULL, NULL, NULL, 1, 1, &status);
      if (U_FAILURE(status))
      {
         if (rtd->cob_err_file)
         {
            fprintf(rtd->cob_err_file, "Internal Error: can't convert to national %s : %s\n", s, u_errorName(status));
         }
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
      }
   }
   return dst - d;

#  else
   int res;
   size_t dszo = dsz;
#     ifndef _MSC_VER
   char cp_buffer2[COB_SMALL_BUFF];

   strcpy(cp_buffer2, cp_buffer);
   strcpy(cp_buffer, iconv_canonicalize(cp_buffer2));
#     endif
   if (rtd->national_of_current_iconv_cd && rtd->national_of_current_cp)
   {
      if ((strcmp(cp_buffer, rtd->national_of_current_cp) != 0) ||
          (cob_utf16_cp_id(rtd) != rtd->current_utf16_cp_id))
      {
         iconv_close(rtd->national_of_current_iconv_cd);
         free(rtd->national_of_current_cp);
         rtd->national_of_current_iconv_cd = NULL;
         rtd->national_of_current_cp = NULL;
      }
   }
   if (!rtd->national_of_current_iconv_cd)
   {
      rtd->current_utf16_cp_id = cob_utf16_cp_id(rtd);
      rtd->national_of_current_iconv_cd = iconv_open(rtd->current_utf16_cp_id, cp_buffer);
      rtd->national_of_current_cp = strdup(cp_buffer);
      if (!rtd->national_of_current_iconv_cd || rtd->national_of_current_iconv_cd == (iconv_t)(-1))
      {
         free(rtd->national_of_current_cp);
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
         rtd->national_of_current_cp = NULL;
         rtd->national_of_current_iconv_cd = NULL;
         return 0;
      }
   }

   res = iconv(rtd->national_of_current_iconv_cd, &s, &ssz, &d, &dsz);
   if (res < 0)
   {
      cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
   }
   return dszo - dsz;
#  endif
}


int cob_enterprise_intr_display_of(COB_RTD, char *s, char *d, char *cp_buffer, size_t ssz, size_t dsz)
{
#  ifdef WITH_ICU
   char *src = s;
   char *dst = d;
   UErrorCode status = 0;

   if (cob_enterprise_open_icu(rtd, cp_buffer))
   {
      ucnv_convertEx(rtd->display_of_current_iconv_cd, rtd->national_of_current_iconv_cd,
                     &dst, dst + dsz, (const char **)&src, src + ssz, NULL, NULL, NULL, NULL, 1, 1, &status);
      if (U_FAILURE(status))
      {
         if (rtd->cob_err_file)
         {
            fprintf(rtd->cob_err_file, "Internal Error: can't convert to displayl : %s\n",  u_errorName(status));
         }
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
      }
   }
   return dst - d;

#  else
   int res;
   size_t dszo = dsz;
#     ifndef _MSC_VER
   char            cp_buffer2[COB_SMALL_BUFF];
   strcpy(cp_buffer2, cp_buffer);
   strcpy(cp_buffer, iconv_canonicalize(cp_buffer2));
#     endif
   if (rtd->display_of_current_iconv_cd && rtd->display_of_current_cp)
   {
      if ((strcmp(cp_buffer, rtd->display_of_current_cp) != 0) ||
          (cob_utf16_cp_id(rtd) != rtd->current_utf16_cp_id))
      {
         iconv_close(rtd->display_of_current_iconv_cd);
         free(rtd->display_of_current_cp);
         rtd->display_of_current_iconv_cd = NULL;
         rtd->display_of_current_cp = NULL;
      }
   }
   if (!rtd->display_of_current_iconv_cd)
   {
      rtd->current_utf16_cp_id = cob_utf16_cp_id(rtd);
      rtd->display_of_current_iconv_cd = iconv_open(cp_buffer, rtd->current_utf16_cp_id);
      rtd->display_of_current_cp = strdup(cp_buffer);
      if (!rtd->display_of_current_iconv_cd || rtd->display_of_current_iconv_cd == (iconv_t)(-1))
      {
         free(rtd->display_of_current_cp);
         cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
         rtd->display_of_current_cp = NULL;
         rtd->display_of_current_iconv_cd = NULL;
         return 0;
      }
   }

   res = iconv(rtd->display_of_current_iconv_cd, &s, &ssz, &d, &dsz);
   if (res < 0)
   {
      cob_set_exception(rtd, COB_EC_ARGUMENT_FUNCTION);
   }
   return dszo - dsz;
#  endif
}

void static
dump_level_tabs(FILE *fp, int level)
{
   int l;
   for (l = 0; l < level; l++)
   {
      fprintf(fp, "\t");
   }
}

int static
dump_find_same(COB_RTD, FILE *fp, cob_field_debug_info *fdi, int size, int level, void * ptr, int l)
{
   void *next_ptr;
   int r = 1;
   while (ptr && (l+r) < fdi->occur)
   {
      next_ptr = (unsigned char *)ptr + (r*size);
      try
      {
         if (memcmp(ptr, next_ptr, size) == 0)
         {
            r++;
         } else
            ptr=NULL;

      }
      catch (RuntimeException)
      {
         ptr = NULL;
      }
   }
   if (r >2)
   {
      dump_level_tabs(fp, level);
      fprintf(fp, "%s(%03d) to (%03d) = <same as above>\n", fdi->name, l + 2,  l + r);
      return r-1;
   }
   return 0;
}

void static
dump_field_info(COB_RTD, FILE *fp, cob_module *mod,
                cob_field_debug_info *fdi, int level, int base_offset)
{
   int i, l, sz;
   cob_field *f;
   void *ptr;
   unsigned char *f_data;
   fdi = cob_runtime_debugger_open_field_debugdb(rtd, mod, fdi);

   for (i = 0; fdi[i].name; i++)
   {
      if (!fdi[i].child)
      {
         if (fdi[i].occur > 1)
         {
            for (l = 0; l < fdi[i].occur; l++)
            {
               try
               {
                  f = mod->get_cob_field(mod->data_storage, fdi[i].field_id);
               }
               catch (RuntimeException)
               {
                  f = NULL;
               }
               if (f)
               {
                  try
                  {
                     f_data = f->data;
                     sz=f->size;
                     ptr = f_data + (base_offset + (l * sz));
                  }
                  catch (RuntimeException)
                  {
                     ptr = NULL;
                  }
                  dump_level_tabs(fp, level);
                  fprintf(fp, "%s(%03d) [%p]", fdi[i].name, l + 1, ptr);
                  fprintf(fp, " = ");
                  try
                  {
                     cob_one_field_display(cob_get_rtd(), f, fp, base_offset + (l * sz));
                  }
                  catch (RuntimeException)
                  {
                     fprintf(fp, "<invalid pointer %p >", ptr);
                  }
                  fprintf(fp, "\n");
                  if (ptr)
                  {
                     l += dump_find_same(rtd, fp, &fdi[i], sz, level, ptr, l);
                  }
               }
            }
         }
         else
         {
            try
            {
               f = mod->get_cob_field(mod->data_storage, fdi[i].field_id);
            }
            catch (RuntimeException)
            {
               f = NULL;
            }
            if (f)
            {
               try
               {
                  f_data = f->data;
                  sz=f->size;
                  ptr = f_data + base_offset;
               }
               catch (RuntimeException)
               {
                  ptr = NULL;
               }
               dump_level_tabs(fp, level);
               fprintf(fp, "%s [%p]", fdi[i].name, ptr);
               fprintf(fp, " = ");
               try
               {
                  cob_one_field_display(cob_get_rtd(), f, fp, base_offset);
               }
               catch (RuntimeException)
               {
                  fprintf(fp, "<invalid pointer %p >", ptr);
               }
               fprintf(fp, "\n");
            }
         }
      }
      else
      {
         if (fdi[i].occur > 1)
         {
            try
            {
               f = mod->get_cob_field(mod->data_storage, fdi[i].field_id);
            }
            catch (RuntimeException)
            {
               f = NULL;
            }
            if (f)
            {
               try
               {
                  sz = f->size;
                  f_data = f->data;
               }
               catch (RuntimeException)
               {
                  f_data = NULL;
               }

               for (l = 0; f_data && l < fdi[i].occur; l++)
               {
                  dump_level_tabs(fp, level);
                  ptr = f_data + (base_offset + (l * sz));
                  fprintf(fp, "%s(%03d) [%p]\n", fdi[i].name, l + 1, ptr);
                  dump_field_info(rtd, fp, mod, fdi[i].child, level + 1, base_offset + (l * sz));
                  if (ptr)
                  {
                     l += dump_find_same(rtd, fp, &fdi[i], sz, level, ptr, l);
                  }
               }
            }
         }
         else
         {
            if (fdi[i].field_id <= 0)
            {
               dump_level_tabs(fp, level);
               fprintf(fp, "%s \n", fdi[i].name);
            }
            else
            {
               dump_level_tabs(fp, level);
               try
               {
                  f = mod->get_cob_field(mod->data_storage, fdi[i].field_id);
                  sz = f->size;
                  f_data = f->data;
                  ptr = f_data + base_offset;
               }
               catch (RuntimeException)
               {
                  ptr = NULL;
               }
               fprintf(fp, "%s [%p]\n", fdi[i].name, ptr);
            }
            dump_field_info(rtd, fp, mod, fdi[i].child, level + 1, base_offset);
         }
      }
   }
}

void cob_dump_debug_info(COB_RTD)
{
   cob_module *module;
   char  *fname;
   int   doclose = 0;
   int   modcnt = 0;
   int   cur_line;
   static int in_cob_dump_debug_info = 0;
   FILE *fp = NULL;
   int   contextSet = 0;
#  ifdef HAVE_PTHREAD_H
   sigset_t set, oldset;
#  endif

   if (!runtime_check_enterprise_key())
   {
      fprintf(fp, "\n\n");
      fprintf(fp, "Cobol memory dump available in Enterprise edition only\n");
      return;
   }
   if (in_cob_dump_debug_info) return;
   in_cob_dump_debug_info = 1;
   module = rtd->current_module;
   fname = getenv("COB_DUMP");
   if (fname)
   {
      if (strcasecmp(fname, "NO") == 0)
      {
         return;
      }
      fp = fopen(fname, "w");
      if (fp)
      {
         doclose = 1;
         if (rtd->cob_err_file)
         {
            fprintf(rtd->cob_err_file ? rtd->cob_err_file : stderr, "\n\n dumping cobol memory to file %s \n", fname);
         }

      }
   }
   if (!fp)
   {
      fp = rtd->cob_err_file;
      if (!fp)
      {
         return;
      }
   }
#  ifdef DEBUG
   fprintf(fp, "\n\n");
   fprintf(fp, "Cobol crash .. Debugmode core dump\n");
   if (!getenv("COB_NO_SIGTRAP"))
   {
      raise(SIGTRAP);
   }
#  endif

   if (!e4c_context_is_ready())
   {
      e4c_context_begin(E4C_TRUE);
      contextSet = 1;

   }
#  ifdef HAVE_PTHREAD_H

   /* UnBlock SIGVEC and SIGBUS; and avoid core dump is invalid pointer */

   sigemptyset(&set);
   sigaddset(&set, SIGSEGV);
#     ifdef SIGBUS
   sigaddset(&set, SIGBUS);
#     endif
   pthread_sigmask(SIG_UNBLOCK, &set, &oldset);
#  endif

   fprintf(fp, "\n\n");
   fprintf(fp, "Cobol memory dump\n");
   fprintf(fp, "+++++++++++++++++\n\n");
   while (module && (modcnt++ < 30))
   {
      try
      {
         cur_line = module->cur_line;
         fprintf(fp, "PID: %d\n", getpid());
         fprintf(fp, "PROGRAM ID : %s  (%s) \n Current line : %s:%d \n",
                 module->module_name ? module->module_name : "<unknown>",
                 module->source_file ? module->source_file : "<unknown>",
                 module->cur_source_file ? module->cur_source_file : "<unknown>", cur_line);
         fprintf(fp, "----------------------------\n");
         if (module->fields_debug_info)
         {
            dump_field_info(rtd, fp,  module, module->fields_debug_info, 1, 0);
         }
         else
         {
            fprintf(fp, "not compiled with -g or -fmem-info\n");
         }
         fprintf(fp, "----------------------------\n\n");
      }
      catch (RuntimeException)
      {
      }
      try
      {
         if (module != module->next)
         {
            module = module->next;
         }
         else
         {
            module->next = NULL;
            module = NULL;
         }
      }
      catch (RuntimeException)
      {
         module = NULL;
      }
   }
   if (contextSet)
   {
      e4c_context_end();
   }
   if (doclose)
   {
      fclose(fp);
   }
   in_cob_dump_debug_info = 0;
#  ifdef HAVE_PTHREAD_H
   pthread_sigmask(SIG_SETMASK, &oldset, NULL);
#  endif
}

static struct data_list    *profiling_list = NULL;


void cob_profiling_register(COB_RTD, struct cob_paragraph_profiling_info *p)
{
   if (p)
   {
      cob_lock_mutex();
      cob_add_to_datalist(rtd, (void *)p, &profiling_list);
      cob_unlock_mutex();
   }

}

void cob_profiling_unregister(COB_RTD, struct cob_paragraph_profiling_info *p)
{
   if (p)
   {
      cit_runtime_t *rtd_r0 = cob_enterprise_get_region_0(rtd);
      if (rtd_r0->call_flag & COB_FULL_CANCEL_ALL)
      {
         cob_lock_mutex();
         cob_remove_from_datalist(rtd, (void *)p, &profiling_list);
         cob_unlock_mutex();
      }
   }
}

void cob_profiling_unregister_all(COB_RTD)
{
   cob_lock_mutex();
   cob_remove_all_from_datalist(rtd, &profiling_list);
   cob_unlock_mutex();
}

void cob_profiling_dump_one(COB_RTD, FILE *f, struct cob_paragraph_profiling_info *p)
{
   int i;

   i = 1; /* start a 1 not at 0 because the first on is dummy*/
   while (p[i].name)
   {
      fprintf(f, "%s\t%lu\t%lu\t%lu\t%lu\t%lu\n", p[i].name, p[i].entry_cnt, p[i].tick_cnt, p[i].external_call_tick, p[i].tick_cnt_elaps, p[i].external_call_tick_elaps);
      i++;
   }

}

int cob_profiling_dump_all(COB_RTD, FILE *f)
{
   if (profiling_list)
   {
      struct data_list    *p;
#     ifdef CLOCKS_PER_SEC
      fprintf(f, "Paragraph\tEntry count\tTotal CPU (1/%ld sec)\tExternal call CPU(1/%ld sec)\tTotal Elaps (1/1000000 sec)\tExternal call Elaps(1/1000000 sec)\n", (long int)CLOCKS_PER_SEC, (long int)CLOCKS_PER_SEC);
#     else
      fprintf(f, "Paragraph\tEntry count\tTotal CPU tick\tExternal call CPU tick\tTotal Elaps tick\tExternal call Elaps tick\n");
#     endif
      cob_lock_mutex();
      for (p =  profiling_list; p; p = p->next)
      {
         cob_profiling_dump_one(rtd, f, (struct cob_paragraph_profiling_info *)p->data);
      }
      cob_unlock_mutex();

      return 1;
   }
   else
   {
      return 0;
   }
}
int cob_profiling_dump_all_to_file(COB_RTD, char *tag)
{
   char *s;
   FILE *f;
   char            buffer[COB_SMALL_BUFF];

   if (profiling_list)
   {
      s = getenv("COB_PROFILING_DIR");
      if (!s)
      {
         s = (char *)"";
      }
      sprintf(buffer, "%scob_profiling_%d%s.xls", s, getpid(), tag);
      f = fopen(buffer, "w");
      if (f)
      {
         cob_profiling_dump_all(rtd, f);
         fclose(f);
      }
   }
   return 0;
}

void cob_profiling_dump(COB_RTD, cob_module *module)
{
   FILE *f;
   struct cob_paragraph_profiling_info *p;
   char            buffer[COB_SMALL_BUFF];
   unsigned long long tcpu;
   unsigned long long telaps;
   unsigned long long lcpu;
   unsigned long long lelaps;
   cob_module      *m;
   char *s;
   int store = 1;
   int i;
   cob_paragraph_profiling_times tms;


   if (!rtd->disable_profiling_files && module && module->profiling_info)
   {
      {
         unsigned long cnt = module->profiling_info->entry_cnt++;
         if (cnt >= 10000)
         {
            store = (cnt % 10000) == 0;
         }
         else if (cnt >= 1000)
         {
            store = (cnt % 1000) == 0;
         }
         else if (cnt >= 100)
         {
            store = (cnt % 100) == 0;
         }
         if (cnt >= 10)
         {
            store = (cnt % 10) == 0;
         }
      }
      if (store)
      {
         tcpu = cob_clock(&tms);
         telaps = tms.elaps_tick;
         s = getenv("COB_PROFILING_DIR");
         if (!s)
         {
            s = (char *)"";
         }
         sprintf(buffer, "%s%s_%d_profile.xls", s, module->module_name, getpid());
         f = fopen(buffer, "w");
         if (f)
         {
            if (module->module_version > 0)
            {
#              ifdef CLOCKS_PER_SEC
               fprintf(f, "Paragraph\tEntry count\tTotal CPU (1/%ld sec)\tExternal call CPU(1/%ld sec)\tTotal Elaps (1/1000000 sec)\tExternal call Elaps(1/1000000 sec)\n", (long int)CLOCKS_PER_SEC, (long int)CLOCKS_PER_SEC);
#              else
               fprintf(f, "Paragraph\tEntry count\tTotal CPU tick\tExternal call CPU tick\tTotal Elaps tick\tExternal call Elaps tick\n");
#              endif
               p = module->profiling_info;
               cob_profiling_dump_one(rtd, f, p);
            }
            else
            {
#              ifdef CLOCKS_PER_SEC
               fprintf(f, "Paragraph\tEntry count\tTotal CPU (1/%ld sec)\tExternal call CPU(1/%ld sec)\n", (long int)CLOCKS_PER_SEC, (long int)CLOCKS_PER_SEC);
#              else
               fprintf(f, "Paragraph\tEntry count\tTotal CPU tick\tExternal call CPU tick\n");
#              endif
               p = module->profiling_info;
               i = 1; /* start a 1 not at 0 because the first one is dummy*/
               while (p[i].name)
               {
                  fprintf(f, "%s\t%lu\t%lu\t%lu\n", p[i].name, p[i].entry_cnt, p[i].tick_cnt, p[i].external_call_tick);
                  i++;
               }
            }
            fclose(f);
         }
         m = COB_NEXT_MODULE(module);
         lcpu = cob_clock(&tms) - tcpu;
         lelaps = tms.elaps_tick - telaps;
         while (m)
         {
            if (m->module_version > 0)
            {
               m->profiling_last.cpu_tick += (long)lcpu;
               m->profiling_last.elaps_tick += (long)lelaps;
            }
            else
            {
               m->profiling_last_cpu_tick_v0 += (long)lcpu;
            }
            m = COB_NEXT_MODULE(m);
         }
      }
   }
}

void  cob_enterprise_map_fstatus(COB_RTD, cob_file_extfh *f, unsigned char *dst_status ) {
    mf_extfh_FCD *fcd;
    fcd = (mf_extfh_FCD *) f->extfh_ptr;
    if(!fcd) 
        return;
    if (!rtd->disable_fstatus_map &&
        rtd->current_module &&
        rtd->current_module->module_version > 0  &&
        rtd->current_module->file_status_map) {
        int i;
        const cob_file_status_map  *map = rtd->current_module->file_status_map;
        for (i = 0; i < map->count; i++) {
            char cit_0 = (map->items[i].cit_status.f_status[0] );
            char cit_1 = (map->items[i].cit_status.f_status[1] );
            if (cit_0 == fcd->user_file_status[0] && cit_1 == fcd->user_file_status[1]) {
                dst_status[0] = (map->items[i].custome_status.f_status[0]);
                dst_status[1] = (map->items[i].custome_status.f_status[1]);
                if (f->file_trace) {
                    cob_runtime_error (rtd, "File %s operation status = %c%c translated to custom status %c%c", f->select_name,
                                       fcd->user_file_status[0], fcd->user_file_status[1], dst_status[0], dst_status[1] );
                }
                break;
            }
        }
    }

}


void
cob_enterprise_move_abit_to_field(COB_RTD, cob_field *f1, cob_field *f2)
{
   size_t              size1 = f1->size;
   unsigned char       *data1 = f1->data;
   int                 i;
   unsigned long long  val = 0;

   for (i = 0; i < size1; i++)
   {
      if (*data1 == CHAR_1)
      {
         val = (val << 1) + 1;
      }
      else
      {
         val = (val << 1);
      }
   }
   cob_set_ull(rtd, f2, val);
}

void
cob_enterprise_move_display_to_abit(COB_RTD, cob_field *f1, cob_field *f2)
{
   size_t              size2 = f2->size;
   unsigned char       *data2 = f2->data;
   int                 i;
   unsigned long long  val = cob_display_get_ull(rtd, f1);

   for (i = size2 - 1; i >= 0; i--)
   {
      if (val & 1)
      {
         data2[i] = CHAR_1;
      }
      else
      {
         data2[i] = CHAR_0;
      }
      val = val >> 1;
   }
}

static void
cob_enterprise_set_bit(COB_RTD, unsigned char *base, unsigned int bit, int msb, int val)
{
   unsigned char *p;
   p = base + (bit / 8);
   bit = bit % 8;
   if (msb)
   {
      if (val)
      {
         switch (bit)
         {
            case 0:
               *p |= 0x80; break;
            case 1:
               *p |= 0x40; break;
            case 2:
               *p |= 0x20; break;
            case 3:
               *p |= 0x10; break;
            case 4:
               *p |= 0x08; break;
            case 5:
               *p |= 0x04; break;
            case 6:
               *p |= 0x02; break;
            case 7:
               *p |= 0x01; break;
         }
      }
      else
      {
         switch (bit)
         {
            case 0:
               *p &= ~0x80; break;
            case 1:
               *p &= ~0x40; break;
            case 2:
               *p &= ~0x20; break;
            case 3:
               *p &= ~0x10; break;
            case 4:
               *p &= ~0x08; break;
            case 5:
               *p &= ~0x04; break;
            case 6:
               *p &= ~0x02; break;
            case 7:
               *p &= ~0x01; break;
         }
      }
   }
   else
   {
      if (val)
      {
         *p |= 1 << bit;
      }
      else
      {
         *p &= ~(1 << bit);
      }
   }
}

static int
cob_enterprise_get_bit(COB_RTD, unsigned char *base, unsigned int bit, int msb)
{
   unsigned char *p;
   unsigned char  v;
   p = base + (bit / 8);
   bit = bit % 8;
   if (msb)
   {
      switch (bit)
      {
         case 0:
            v = 0x80; break;
         case 1:
            v = 0x40; break;
         case 2:
            v = 0x20; break;
         case 3:
            v = 0x10; break;
         case 4:
            v = 0x08; break;
         case 5:
            v = 0x04; break;
         case 6:
            v = 0x02; break;
         case 7:
            v = 0x01; break;
      }
   }
   else
   {
      v = 1 << bit;
   }
   return (*p & v ? 1 : 0);
}

void
cob_enterprise_set_bits_ull(COB_RTD, cob_field *f, unsigned long long val)
{

   unsigned long long  val1 = 0;
   int i;
   int bit, digit, msb;
   cob_bitsarray_field *fba;

   digit = f->attr->digits;
   if (f->attr->type == COB_TYPE_BITS_ARRAY)
   {
      fba = (cob_bitsarray_field *)f;
      bit = f->attr->scale + (fba->index * digit);

   }
   else
   {
      bit = f->attr->scale;
   }
   msb = COB_FIELD_BINARY_SWAP(f);
   if (digit > 64)
   {
      digit = 64;
   }
   val1 = (unsigned long long)1 << (digit - 1);
   for (i = 0; i < digit; i++)
   {
      cob_enterprise_set_bit(rtd, f->data, bit + i, msb, val & val1 ? 1 : 0);
      val1 >>= 1;
   }
}

void
cob_enterprise_move_display_to_bits(COB_RTD, cob_field *f1, cob_field *f2)
{
   unsigned long long  val1 = cob_display_get_ull(rtd, f1);
   cob_enterprise_set_bits_ull(rtd, f2, val1);
}

unsigned long long
cob_enterprise_bits_get_ull(COB_RTD, cob_field *f)
{
   unsigned long long  val1 = 0;
   int i;
   int bit, digit, msb;
   cob_bitsarray_field *fba;

   digit = f->attr->digits;
   if (f->attr->type == COB_TYPE_BITS_ARRAY)
   {
      fba = (cob_bitsarray_field *)f;
      bit = f->attr->scale + (fba->index * digit);

   }
   else
   {
      bit = f->attr->scale;
   }
   msb = COB_FIELD_BINARY_SWAP(f);
   if (digit > 64)
   {
      digit = 64;
   }
   for (i = 0; i < digit; i++)
   {
      val1 <<= 1;
      val1 |= (unsigned long long)cob_enterprise_get_bit(rtd, f->data, bit + i, msb);
   }
   return val1;
}

void cob_enterprise_move_bits_to_field(COB_RTD, cob_field *f1, cob_field *f2)
{
   cob_set_ull(rtd, f2, cob_enterprise_bits_get_ull(rtd, f1));
}

int  cob_enterprise_map_ctr_status(COB_RTD, int fret, unsigned int *stat)
{
   if (rtd->current_module->module_version > 0  &&
       rtd->current_module->crt_status_map && stat)
   {
      int i;

      const cob_crt_status_map  *map = rtd->current_module->crt_status_map;

      for (i = 0; i < map->count; i++)
      {
         if (map->items[i].cit_status == fret)
         {
            *stat = map->items[i].custome_status;
            if (rtd->current_module->crt_status)
            {
               if (COB_FIELD_IS_NUMERIC(rtd->current_module->crt_status))
               {
                  cob_set_int(rtd, rtd->current_module->crt_status, *stat);
               }
               else
               {
                  memcpy(rtd->current_module->crt_status->data, stat, min(rtd->current_module->crt_status->size, 4));
               }
            }
            return 1;
         }
      }
   }
   return 0;
}

static int
is_english(int ch)
{
#  if CIT_EBCDIC_CHARSET
   return 1;
#  else
   if (ch >= 'a' && ch <= 'Z') return 1;
   if (ch >= 'A' && ch <= 'Z') return 1;
#  endif
   return 0;
}

static int
is_RTL(COB_RTD, int c)
{
#  if CIT_EBCDIC_CHARSET
   return 0;
#  else
   return ((c >= rtd->RTL_info->startCode) && (c <= rtd->RTL_info->endCode));
#  endif
}

static int
RTL_str_size(COB_RTD, unsigned char *p, int size)
{
   int pos = 0;
   int res = 0;
   while (pos < size)
   {
      if (is_english(*p))
      {
         return 0;
      }
      if (is_RTL(rtd, *p))
      {
         return pos;
      }
      pos++;
      if (!cob_isspace_char(rtd, *p)) res = pos;
      p++;
   }

   return res;
}

static unsigned char*
RTLReversString(COB_RTD, unsigned char *p, unsigned char *d, int size)
{

   int  j, cnt;
   unsigned char *t;
   unsigned char c;
   unsigned char *end;

   if (size == 0)
   {
      return d;
   }
   end = p - size;
   while (p > end)
   {
      c = *p;
      if ((c >= CHAR_0 && c <= CHAR_9))
      {
         /* do not revers number*/
         cnt = 0;
         do
         {
            if ((c >= CHAR_0 && c <= CHAR_9) || c == CHAR_DOT || c == CHAR_COMMA)
            {
               /*do not revers*/
               cnt++;
               p--;
            }
            else
            {
               break;
            }
            c = *p;
         }
         while (p > end);
         t = p + 1;
         for (j = 0; j < cnt; j++)
         {
            *d = *t;
            d++;
            t++;
         }
      }
      else
      {
         *d = *p;
         switch (SWITCH_CHAR(*d))
         {
            case CASE_CHAR(CHAR_OPENPAR):
               *d = CHAR_CLOSEPAR; break;
            case CASE_CHAR(CHAR_CLOSEPAR):
               *d = CHAR_OPENPAR; break;
            case CASE_CHAR(CHAR_OPENTAB):
               *d = CHAR_CLOSETAB; break;
            case CASE_CHAR(CHAR_CLOSETAB):
               *d = CHAR_OPENTAB; break;
            case CASE_CHAR(CHAR_OPENAC):
               *d = CHAR_CLOSEAC; break;
            case CASE_CHAR(CHAR_CLOSEAC):
               *d = CHAR_OPENAC; break;
            case CASE_CHAR(CHAR_LESS):
               *d = CHAR_GREAT; break;
            case CASE_CHAR(CHAR_GREAT):
               *d = CHAR_LESS; break;
            default:
               break;
         }
         d++;
         p--;
      }
   }
   return d;
}

unsigned char*
cob_enterprise_RTLGetReversedString(COB_RTD, unsigned char *p, int size)
{
   unsigned char *res = p;
#  ifndef CIT_SBE
   int i;
   unsigned char *d;
   int in_rev = 0, pos;

   if (rtd->RTL_info)
   {
      if ((size + 2) >= rtd->term_buff_size)
      {
         while ((size + 2) >= rtd->term_buff_size)
         {
            rtd->term_buff_size *= 2;
         }
         cob_free(rtd->term_buff);
         rtd->term_buff = cob_malloc(rtd, rtd->term_buff_size);
      }
      d = rtd->term_buff;
      in_rev = 0;
      for (pos = 0; pos < size; pos++, p++)
      {
         if (is_RTL(rtd, *p))
         {
            in_rev = pos;
            break;
         }
         else if (is_english(*p))
         {
            break;
         }
      }
      if (in_rev == 0)
      {
         memcpy(d, p - pos, pos);
         d += pos;
      }
      for (; pos < size; pos++, p++)
      {
         if (is_RTL(rtd, *p))
         {
            in_rev++;
         }
         else
         {
            i = 0;
            if (in_rev)
            {
               i  = RTL_str_size(rtd, p, size - pos);
            }
            if (in_rev &&  i)
            {
               p += i - 1;
               pos += i - 1;
               in_rev += i;
            }
            else
            {
               if (in_rev)
               {
                  d = RTLReversString(rtd, p - 1, d, in_rev);
                  in_rev = 0;
               }
               *d = *p;
               d++;
            }
         }
      }
      if (in_rev)
      {
         d = RTLReversString(rtd, p - 1, d, in_rev);
         in_rev = 0;
      }
      res = rtd->term_buff;
   }
#  endif
   return res;
}
/*XML structures*/
typedef struct XMLEventElem {
    struct XMLEventElem *next;
    const char          *event;
    char                *text;     /* UTF8 text returned by XML_lib*/
    int                 len;
    int                 error;
    char                *ntext;    /* NATIONAL converted  */
    int                 nlen;
    char                *dtext;    /* DISPLAY converted  */
    int                 dlen;
} XMLEventElem;

typedef struct XMLModuleData {
    XML_Parser          parser;
    struct XMLEventElem *elem;
    struct XMLEventElem *head;
    char                *codepage;
} XMLModuleData;



static
XMLEventElem* cob_list_xml_reverse(XMLEventElem *head)
{
   XMLEventElem *newhead = NULL;
   XMLEventElem *temp;

   while ((head))
   {
      temp = newhead;
      newhead = head;
      head = head->next;
      newhead->next = temp;
   }
   return newhead;
}
static char* copyArray(const char *data)
{
   char *res = NULL;
   if (data)
   {
      int len = strlen(data);
      res = malloc(len + 1);
      strncpy(res, data, len);
      res[len] = '\0';
   }
   else
   {
      res = malloc(1);
      res[0] = 0;
   }
   return res;
}
static
void push_XMLEventElem(const char *event, const char *name, COB_RTD)
{
   XMLModuleData *xmldata = (XMLModuleData *)rtd->current_module->xmldata;
   XMLEventElem *Elem = cob_malloc(rtd, sizeof(XMLEventElem));
   Elem->event = event;
   Elem->text =  (char *)name;
   Elem->len = name ? strlen(name) : 0;
   Elem->next = xmldata->elem;
   xmldata->elem = Elem;
}
/***********/
static void
XMLCALL xmlDeclHandler(void *userData, const char  *version, const char  *encoding, int standalone)
{
   COB_RTD = (cit_runtime_t *)userData;
   XMLModuleData *xmldata = (XMLModuleData *)rtd->current_module->xmldata;
   if (version)
   {
      push_XMLEventElem("VERSION-INFORMATION", copyArray(version), rtd);
   }
   if (encoding)
   {
      push_XMLEventElem("ENCODING-DECLARATION", copyArray(encoding), rtd);
      xmldata->codepage = strdup(encoding);
   }
   if (standalone)
   {
      push_XMLEventElem("STANDALONE-DECLARATION", ((standalone) ? copyArray("yes") : copyArray("no")), rtd);
   }

}

static void
XMLCALL xmlComment(void *userData, const char *data)
{
   COB_RTD = (cit_runtime_t *)userData;
   char *name = copyArray(data);
   if (name)
   {
      push_XMLEventElem("COMMENT", name, rtd);
   }
}
static void
XMLCALL xmlProcessingInstruction(void *userData,
                                 const char *target,
                                 const char *data)
{
   COB_RTD = (cit_runtime_t *)userData;
   char *targetC = copyArray(target);
   char *dataC = copyArray(data);
   if (targetC)
   {
      push_XMLEventElem("PROCESSING-INSTRUCTION-TARGET", targetC, rtd);
   }
   if (dataC)
   {
      push_XMLEventElem("PROCESSING-INSTRUCTION-DATA", dataC, rtd);
   }

}
static void
XMLCALL xmlStartDoctypeDecl(void *userData,
                            const char *doctypeName,
                            const char *sysid,
                            const char *pubid,
                            int   has_internal_subset)
{

   COB_RTD = (cit_runtime_t *)userData;

   char res[BUFSIZ] = "<!DOCTYPE ";

   strcat(res, doctypeName);
   if (sysid != NULL)
   {
      strcat(res, " SYSTEM \"");
      strcat(res, sysid);
      strcat(res, "\"");
   }
   else if (pubid != NULL)
   {
      strcat(res, " PUBLIC \"");
      strcat(res, pubid);
      strcat(res, "\"");
   }
   strcat(res, ">");

   push_XMLEventElem("DOCUMENT-TYPE-DECLARATION", copyArray(res), rtd);
}

static void
XMLCALL xmlStartCdataSection(void *userData)
{
   COB_RTD = (cit_runtime_t *)userData;

   push_XMLEventElem("START-OF-CDATA-SECTION", copyArray("<![CDATA["), rtd);

}
static void
XMLCALL xmlEndCdataSection(void *userData)
{
   COB_RTD = (cit_runtime_t *)userData;

   push_XMLEventElem("END-OF-CDATA-SECTION", copyArray("]]>"), rtd);

}
static void extractText(COB_RTD, char *src, int n, const char *eventText)
{
   char *dst = malloc(n + 1);

   memset(dst, 0, n + 1);
   strncpy(dst, src, n + 1);
   push_XMLEventElem(eventText, dst, rtd);
}
static void
XMLCALL characterData(void *userData,
                      const char *s,
                      int len)
{

   COB_RTD = (cit_runtime_t *)userData;
   char *tmp;
   char *res;
   int n;

   if (s != NULL && len > 0)
   {
      res = malloc(len + 1);
      res[len] = '\0';
      strncpy(res, s, len);
      if (len == 1 && (strcmp(res, "&") == 0 || strcmp(res, "'") == 0 ||  strcmp(res, "\"") == 0 ||  strcmp(res, ">") == 0 ||  strcmp(res, "<") == 0))
      {
         push_XMLEventElem("CONTENT-CHARACTER", res, rtd);

      }
      else if (len == 1 && *res == '\n')
      {
         push_XMLEventElem("UNKNOWN-REFERENCE-IN-CONTENT", res, rtd);
      }
      else
      {
         while (1)
         {

            tmp = strstr(res, "&#");
            if (tmp == NULL)
            {
               extractText(rtd, res, strlen(res), "CONTENT-CHARACTERS");
               break;
            }
            n = tmp - res;
            if (n != 0)
            {
               extractText(rtd,  res, n, "CONTENT-CHARACTERS");
            }
            res = tmp;
            tmp = strstr(res, " ");
            n = tmp - res;
            extractText(rtd, res, n, "CONTENT-NATIONAL-CHARACTER");
            res = tmp;
         }
         cob_free(res);
      }

   }
}

static void XMLCALL
startElement(void *userData, const char *name, const char **atts)
{
   COB_RTD = (cit_runtime_t *)userData;
   int i;
   push_XMLEventElem("START-OF-ELEMENT",  copyArray(name), rtd);

   for  (i = 0; atts[i] != NULL; i += 2)
   {
      push_XMLEventElem("ATTRIBUTE-NAME", copyArray(atts[i]), rtd);
      if (atts[i + 1] != NULL)
      {
         /*TODO: ATTRIBUTE-CHARACTER and ATTRIBUTE-NATIONAL-CHARACTER 
          How to detect an ATTRIBUTE-CHARACTER and an UNKNOWN-REFERENCE-IN-ATTRIBUTE?
           */
         push_XMLEventElem("ATTRIBUTE-CHARACTERS", copyArray(atts[i + 1]), rtd);
      }
   }


}

static void XMLCALL
endElement(void *userData, const char *name)
{
   COB_RTD = (cit_runtime_t *)userData;
   push_XMLEventElem("END-OF-ELEMENT", copyArray(name), rtd);

}

static int XMLCALL
XML_RawEncodingHandler(void *encodingHandlerData,  const XML_Char *name, XML_Encoding *info)
{
   int i;
   unsigned char s[2];
   UErrorCode status;
   UChar d[4];
   UChar *dp;
   char *sp;
   COB_RTD = encodingHandlerData;
   if (cob_enterprise_open_icu(rtd, name))
   {
      memset(info, 0, sizeof(XML_Encoding));
      for (i = 1; i < 255; i++)
      {
         s[0] = i;
         sp = (char *)s;
         dp = d;
         status = 0;
         ucnv_toUnicode(rtd->display_of_current_iconv_cd,  &dp, d + 4, (const char **)&sp, (char *)(s + 1), NULL, 1, &status);
         if (U_FAILURE(status))
         {
            if (rtd->cob_err_file)
            {
               /*fprintf(rtd->cob_err_file, "Internal Error: can't convert from %s %d : %s\n", name, i, u_errorName(status));*/
            }
         }
         else
         {
            info->map[i] = d[0];
         }
      }
      return XML_STATUS_OK;
   }
   else
   {
      return XML_STATUS_ERROR;
   }
}

void
cob_enterprise_xml_parse_init(COB_RTD, cob_field *f, cob_field *xml_code, cob_field *xml_text,
                              cob_field *xml_ntext, cob_field *xml_event)
{
   cob_enterprise_xml_parse_init_1(rtd, f, xml_code, xml_text, xml_ntext, xml_event, NULL);
}

void
cob_enterprise_xml_parse_init_1(COB_RTD, cob_field *f, cob_field *xml_code, cob_field *xml_text,
                                cob_field *xml_ntext, cob_field *xml_event, cob_field *xml_encoding)
{
   cob_module *mod = rtd->current_module;
   XMLModuleData *xmldata;
   XML_Parser parser;

   (void)cob_enterprise_open_icu(rtd, "UTF-8");
   if (mod)
   {
      mod->xmldata = cob_malloc(rtd, sizeof(XMLModuleData));
      xmldata = (XMLModuleData *)mod->xmldata;
      if (xml_encoding)
      {
         cob_field *en = cob_intr_cstring(rtd, xml_encoding);
         xmldata->codepage = strdup((char *)en->data);
         cob_str_strip(xmldata->codepage, NULL);
      }
      else
      {
         if (rtd->xml_code_page)
         {
            xmldata->codepage = strdup(rtd->xml_code_page);
         }
         else xmldata->codepage = NULL;
      }
      parser = XML_ParserCreate(xmldata->codepage);

      xmldata->elem = NULL;
      xmldata->parser = parser;

      XML_SetUserData(parser, rtd);
      XML_SetXmlDeclHandler(parser, xmlDeclHandler);
      XML_SetElementHandler(parser, startElement, endElement);
      XML_SetCommentHandler(parser, xmlComment);
      XML_SetProcessingInstructionHandler(parser, xmlProcessingInstruction);
      XML_SetStartDoctypeDeclHandler(parser, xmlStartDoctypeDecl);
      XML_SetCdataSectionHandler(parser, xmlStartCdataSection, xmlEndCdataSection);
      XML_SetCharacterDataHandler(parser, characterData);
      XML_SetUnknownEncodingHandler(parser, XML_RawEncodingHandler, rtd);

      if (mod->xparse_event)
      {
         push_XMLEventElem("START-OF-DOCUMENT", copyArray(" "), rtd);
      }

      if (XML_Parse(parser, (const char *)f->data, (int)f->size, 1) == XML_STATUS_ERROR)
      {
         const char *eventXML;
         const char *p;
         int offset, size;
         int error_code = XML_GetErrorCode(parser);
         switch (error_code)
         {

            case XML_ERROR_UNDEFINED_ENTITY:
               /*Find XML TEXT*/
            {
               char *res;
               int i = 0;

               p = XML_GetInputContext(parser, &offset, &size);
               p += offset;
               res = calloc(1, size + 255);
               if (*p != '&')
               {
                  p = strstr(p, "&");
                  eventXML = "UNKNOWN-REFERENCE-IN-ATTRIBUTE";
               }
               else
               {
                  eventXML = "UNKNOWN-REFERENCE-IN-CONTENT";
               }
               while (*p != ';' && size >= 0)
               {
                  res[i] = (*p);
                  p++; i++;
                  size--;
               }
               res[i] = *p;
               res[i + 1] = '\0';
               push_XMLEventElem(eventXML, res, rtd);
            }
               break;
            default:
            {
               char *text;
               p = XML_GetInputContext(parser, &offset, &size);
               text = calloc(1, offset + 2);
               if (p)
               {
                  strncpy(text, p, offset + 1);
               }
               text[offset + 1] = '\0';
               /* fprintf (stderr, "%s %d %d Error %d \n", text, offset, size, error_code);*/
               push_XMLEventElem("EXCEPTION", text, rtd);
               xmldata->elem->error = COB_EC_XML;
            }
               break;
         }
      }
      else
      {
         if (mod->xparse_event)
         {
            push_XMLEventElem("END-OF-DOCUMENT", copyArray(" "), rtd);
         }
      }
      xmldata->elem = cob_list_xml_reverse(xmldata->elem);
      /*keep trace on the list'head to deallocate*/
      xmldata->head = xmldata->elem;
      cob_enterprise_xml_parse_next(rtd, xml_code, xml_text, xml_ntext, xml_event);
   }
}

void
cob_enterprise_xml_parse_next(COB_RTD, cob_field *xml_code, cob_field *xml_text, cob_field *xml_ntext, cob_field *xml_event)
{
   cob_module *mod = rtd->current_module;

   int xcode;

   xcode = cob_get_int(rtd, xml_code);
   if (xcode != 0)
   {
      cob_set_exception(rtd, COB_EC_XML);
      return;
   }
   if (mod)
   {
      XMLModuleData *xmldata = ((XMLModuleData *)mod->xmldata);
      XMLEventElem *Elem = xmldata->elem;
      if (Elem)
      {
         if (xml_ntext)
         {
            Elem->ntext = cob_malloc(rtd, Elem->len * 2);
            xml_ntext->data = (unsigned char *)Elem->ntext;
            xml_ntext->size = cob_enterprise_intr_national_of(rtd, Elem->text, Elem->ntext, (char *)"UTF-8", Elem->len, Elem->len * 2);
         }

         if (xmldata->codepage)
         {
            Elem->dtext = cob_malloc(rtd, Elem->len + 1);
            xml_text->data = (unsigned char *)Elem->dtext;
            xml_text->size = cob_enterprise_debugcp_to_display(rtd, Elem->text, Elem->dtext, (char *)xmldata->codepage, rtd->utf8_codepage_iconv_cd, Elem->len, Elem->len);
         }
         else
         {
            if ((unsigned char *)Elem->text)
            {
               xml_text->data = (unsigned char *)Elem->text;
               xml_text->size = Elem->len;
            }
            else
            {
               Elem->dtext = cob_malloc(rtd, 1);
               xml_text->size = 0;
            }
         }
         xml_event->data = (unsigned char *)Elem->event;
         xml_event->size = strlen(Elem->event);
         cob_set_int(rtd, xml_code, Elem->error);
         if (Elem->error)
         {
            cob_set_exception(rtd, Elem->error);
         }
         xmldata->elem = Elem->next;

      }
      else
      {
         cob_set_int(rtd, xml_code, -1);
      }
   }

}

void
cob_enterprise_xml_parse_close(COB_RTD)
{
   cob_module *mod = rtd->current_module;
   if (mod)
   {
      XMLModuleData *xmldata = ((XMLModuleData *)mod->xmldata);
      if (xmldata)
      {
         XMLEventElem *Elem = xmldata->head;

         while (Elem)
         {
            if (Elem->text)
            {
               cob_free(Elem->text);
            }
            if (Elem->dtext)
            {
               cob_free(Elem->dtext);
            }
            if (Elem->ntext)
            {
               cob_free(Elem->ntext);
            }
            xmldata->head = Elem->next;
            cob_free(Elem);
            Elem = xmldata->head;
         }
         if (xmldata->codepage)
         {
            cob_free(xmldata->codepage);
         }
         XML_ParserFree(xmldata->parser);
         cob_free(xmldata);
         mod->xmldata = NULL;
      }

   }
}


void
cob_enterprise_xml_generate_name(COB_RTD, cob_field *dest, cob_field *name, cob_field *cnt, cob_field *xml_code, int closetag)
{
   int i;
   unsigned char *p;
   i = cob_get_int(rtd, xml_code);
   if (i != 0)
   {
      return;
   }
   i = cob_get_int(rtd, cnt);
   if (dest->size < name->size + 3 + i)
   {
      cob_set_int(rtd, xml_code, 400);
      cob_set_exception(rtd, COB_EC_XML_COUNT);
      return;
   }
   p = dest->data + i;
   *p++ = '<';
   if (closetag)
   {
      *p++ = '/';
   }
   memcpy(p, name->data, name->size);
   p += name->size;
   *p++ = '>';
   cob_set_int(rtd, cnt, p - dest->data);

}

void
cob_enterprise_xml_generate_data(COB_RTD, cob_field *dest, cob_field *f, cob_field *cnt, cob_field *xml_code)
{
   int i;
   unsigned char *p;
   unsigned char *t;
   unsigned char *end;
   unsigned char *pdest;
   unsigned char *destend;
   char *save_cp = NULL;
   void *save_icov_cp = NULL;
   int tlen;
   int save_display_flag;
   i = cob_get_int(rtd, xml_code);
   if (i != 0)
   {
      return;
   }
   i = cob_get_int(rtd, cnt);

   save_cp = rtd->display_code_page;
   rtd->display_code_page = (char *)"UTF-8";
   save_display_flag = rtd->current_module->flag_pretty_display;
   rtd->current_module->flag_pretty_display = COB_NUMERIC_PRETTY_DISPLAY;
   save_icov_cp = rtd->console_codepage_iconv_cd;
   rtd->console_codepage_iconv_cd = rtd->utf8_codepage_iconv_cd;
   rtd->display_buff_offset = 0;
   cob_one_field_display_extended(rtd, f, NULL, 0, DISPLAY_ONE_FIELD_MODE_BUFFER | DISPLAY_ONE_FIELD_SOURCE_XML);
   p = rtd->display_buff;
   if (COB_FIELD_IS_NUMERIC(f))
   {
      while (*p == CHAR_SP)
      {
         p++;
      }
      if (*p == 0)
      {
         p--;
      }
   }
   rtd->display_code_page = save_cp;
   rtd->current_module->flag_pretty_display = save_display_flag;
   rtd->console_codepage_iconv_cd = save_icov_cp;
   end = p + strlen((char *)p);
   while (end > p + 1)
   {
      if (*(end - 1) == CHAR_SP)
      {
         end--;
      }
      else break;
   }

   pdest = dest->data + i;
   destend = dest->data + dest->size;

   while (p < end)
   {
      switch (SWITCH_CHAR(*p))
      {
         case CASE_CHAR(CHAR_DQUOTE):
            t = (unsigned char *)"&quot;";  tlen = 6;
            break;
         case CASE_CHAR(CHAR_SQUOTE):
            t = (unsigned char *)"&apos;";  tlen = 6;
            break;
         case CASE_CHAR(CHAR_AMP):
            t = (unsigned char *)"&amp;";   tlen = 5;
            break;
         case CASE_CHAR(CHAR_LESS):
            t = (unsigned char *)"&lt;";    tlen = 4;
            break;
         case CASE_CHAR(CHAR_GREAT):
            t = (unsigned char *)"&gt;";    tlen = 4;
            break;
         default:
            t = p;        tlen = 1;
            break;
      }
      if (pdest + tlen >= destend)
      {
         cob_set_int(rtd, xml_code, 400);
         cob_set_exception(rtd, COB_EC_XML_COUNT);
         return;
      }
      if (tlen == 1)
      {
         *pdest = *t;
      }
      else
      {
         memcpy(pdest, t, tlen);
      }

      p++;
      pdest += tlen;
   }
   cob_set_int(rtd, cnt, pdest - dest->data);
}

void cob_enterprise_ccmap_dump(COB_RTD, void *mod)
{
   register cob_module *module = mod;
   if (module && module->ccmapdata)
   {
      if (cob_runtime_debugger_open_debugdb(rtd, module) && module->debugdb_moduleid > 0)
      {
         debugdb_add_ccmaprun_rec(cob_runtime_debugger_debugdb, module->debugdb_moduleid, module->ccmapdata, module->ccmapdata_size);
         memset(module->ccmapdata, 0, module->ccmapdata_size);
      }
   }
}

#ifdef _MSC_VER
BOOL WINAPI DllMain(
   void *hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
   )
{
   cit_runtime_t *rtd;
   cit_runtime_t *rtd_r0;
   int i;

   switch (fdwReason)
   {
      case DLL_THREAD_DETACH:
         rtd = cob_get_rtd();
         cob_rtd_tidy(rtd);
         break;
      case DLL_PROCESS_DETACH:
      {
         rtd = cob_get_rtd();
         rtd_r0 = cob_enterprise_get_region_0(rtd);
         for (i = 1; i < COB_MAX_RTD_REGION; i++)
         {
            cob_rtd_tidy(rtd->rtd_region_data[i]);
         }
         cob_rtd_tidy(rtd_r0);
         cob_set_rtd(NULL);
         free_citkey();
      }
         break;
   }
   return TRUE;
}

#endif

/*CobolIT*/



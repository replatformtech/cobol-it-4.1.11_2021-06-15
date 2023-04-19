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
 * License along with this library; see the file COPYING.LIB.  If not write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

#ifndef COB_CALL_H
#define COB_CALL_H

#include <libcob/common.h>
#include <cobsetjmp.h>
#define MF_BIN_EXTENTION "gnt"
#define CIT_BIN_EXTENTION "cit"
#define HASH_SIZE	251    

extern int cob_debug_call;
COB_DLL_EXPIMP void cob_cancel      (COB_RTD, const char *name);
COB_DLL_EXPIMP void cob_set_cancel  (COB_RTD, const char *name, void *entry, void *cancel);
COB_DLL_EXPIMP void *cob_resolve    (COB_RTD, const char *name);
COB_DLL_EXPIMP void *cob_resolve_1  (COB_RTD, const char *name);
COB_DLL_EXPIMP const char *cob_resolve_error (COB_RTD);
COB_DLL_EXPIMP void *cob_lookup_module_function (COB_RTD, const char *name);
COB_DLL_EXPIMP void cob_set_library_path (COB_RTD, const char *path);


COB_DLL_EXPIMP void *cob_call_resolve   (COB_RTD, const cob_field *f);
COB_DLL_EXPIMP void *cob_call_resolve_1 (COB_RTD, const cob_field *f);
COB_DLL_EXPIMP void *cob_call_resolve_2 (COB_RTD, const cob_field *f); 
COB_DLL_EXPIMP void cob_field_cancel    (COB_RTD, const cob_field *f);
COB_DLL_EXPIMP void cob_load_shared_lib (COB_RTD, char *fname);
COB_DLL_EXPIMP int  cob_load_shared_lib_1 (COB_RTD, char *fname, int dumperror);

COB_DLL_EXPIMP void cob_dlclose (void*handle);
COB_DLL_EXPIMP int  cob_is_module_unloadable (struct module_hash     *m, unsigned int cancel_region_nr);
COB_DLL_EXPIMP void cob_clear_module_hash (struct module_hash     *m);
COB_DLL_EXPIMP void cob_call_close_all_handles (COB_RTD);


#ifdef __GNUC__
extern void cob_call_error (COB_RTD) __attribute__ ((noreturn));
#else
COB_DLL_EXPIMP void cob_call_error (COB_RTD);
#endif

#endif /* COB_CALL_H */

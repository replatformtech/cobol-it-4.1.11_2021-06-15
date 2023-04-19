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
#ifndef _CITKEY_H
#define _CITKEY_H 1
#ifdef _MSC_VER
    #ifdef LIBCOB_EXPORTS
        #define COB_DLL_EXPIMP __declspec(dllexport) 
        #define COB_DLL_EXPORT __declspec(dllexport) 
    #else /* LIBCOB_EXPORTS */
        #define COB_DLL_EXPIMP __declspec(dllimport) extern 
        #define COB_DLL_EXPORT
    #endif /* LIBCOB_EXPORTS */
#else /* _MSC_VER */
    #define COB_DLL_EXPIMP extern 
    #define COB_DLL_EXPORT
#endif /* _MSC_VER */
#include "stdio.h"
#include "stdlib.h"
#include <ctype.h>
#include "time.h"
COB_DLL_EXPIMP int check_citkey (const char*product);
COB_DLL_EXPIMP int citkey_test_feature (const  char*product);
COB_DLL_EXPIMP int runtime_check_enterprise_key (void);
COB_DLL_EXPIMP void free_citkey(void);

COB_DLL_EXPIMP int dump_citkey (const char*product);
COB_DLL_EXPIMP char * cob_strptime(const char *buf, const char *fmt, struct tm *tm);

#endif


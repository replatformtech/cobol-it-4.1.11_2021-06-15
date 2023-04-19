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
/*
**  ASCII <=> EBCDIC conversion functions
*   This is the fake module to replace the Enterprise third party module
*/
 
#ifndef A2E__H
#define A2E__H
/*CIT_BEGIN_ENTERPRISE*/

#include "common.h"
#include "cit_types.h"
/*
unsigned char ASCIItoEBCDIC(const unsigned char c);  
unsigned char EBCDICtoASCII(const unsigned char c);   
*/ 
#ifdef _MSC_VER
#define EXPORT_IMPORT __declspec(dllimport) extern 
#else
#define EXPORT_IMPORT extern 
#endif

EXPORT_IMPORT const unsigned char ascii2ebcdic[256];                         /* Toascii.C      */
EXPORT_IMPORT const unsigned char ebcdic2ascii[256];                         /* Toascii.C      */
#define ASCIItoEBCDIC(c) ascii2ebcdic[((unsigned char )(c) & 0xff)]
#define EBCDICtoASCII(c) ebcdic2ascii[((unsigned char )(c) & 0xff)]

COB_DLL_EXPIMP unsigned char * StringEBCDICtoASCII(unsigned char *str, int size);
COB_DLL_EXPIMP unsigned char * StringEBCDICtoASCII_DUP(COB_RTD, unsigned char *str, int size);
COB_DLL_EXPIMP unsigned char * StringASCIItoEBCDIC(unsigned char *str, int size);
COB_DLL_EXPIMP void Clear_a2e(COB_RTD);
 
#if CIT_EBCDIC_CHARSET 
#if !defined(CIT_DUAL_CHARSET)
#define STRING_E2A(a,s) StringEBCDICtoASCII(a,s)
#define STRING_E2A_DUP(rtd,a,s) StringEBCDICtoASCII_DUP(rtd,a,s)
#define STRING_A2E(a,s) StringASCIItoEBCDIC(a,s)
#define A2E(c) ASCIItoEBCDIC(c)
#define E2A(c) EBCDICtoASCII(c)
#else /*!defined(CIT_DUAL_CHARSET)*/
#define STRING_E2A(a,s) (rtd->ebcdic_charset ? StringEBCDICtoASCII((unsigned char *)a,s) : (unsigned char *)(a))
#define STRING_E2A_DUP(rtd,a,s) (rtd->ebcdic_charset ? StringEBCDICtoASCII_DUP(rtd,(unsigned char *)a,s) : (unsigned char *)(a))
#define STRING_A2E(a,s) (rtd->ebcdic_charset ? StringASCIItoEBCDIC((unsigned char *)a,s) : (unsigned char *)(a))
#define A2E(c) (rtd->ebcdic_charset ? (unsigned char)(ASCIItoEBCDIC(c)) : (unsigned char)(c))
#define E2A(c) (rtd->ebcdic_charset ? (unsigned char)(EBCDICtoASCII(c)) : (unsigned char)(c))
#endif
#define A2E_F(c) ASCIItoEBCDIC(c)
#define E2A_F(c) EBCDICtoASCII(c)
#else
/*CIT_END_ENTERPRISE*/

#define STRING_E2A(a,s) (a)
#define STRING_E2A_DUP(rtd,a,s) (a)
#define STRING_A2E(a,s) (a)
#define A2E(c) (c)
#define E2A(c) (c)
#define A2E_F(c) (c)
#define E2A_F(c) (c)
/*CIT_BEGIN_ENTERPRISE*/
#endif 
/*CIT_END_ENTERPRISE*/
#endif /* A2E__H */

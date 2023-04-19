/*
  * Copyright (C) 2008-2020 Cobol-IT
 * 
 * This code is proprety of COBOL-IT . 
 * Usage of this code or any part of it require a explicit writen authoristion of COBOL-IT
 */

/* Emulation of MF functions cobsetjump */
#ifndef _CIT_COB_SETJMP_H_
#define _CIT_COB_SETJMP_H_
#ifndef LIBCOB_EXPORTS
#ifdef _MSC_VER
    #ifndef _CRT_SECURE_NO_DEPRECATE 
        #define _CRT_SECURE_NO_DEPRECATE 1
    #endif
    #ifdef LIBCOB_EXPORTS
        #define COB_DLL_EXPIMP __declspec(dllexport) extern 
    #else /* LIBCOB_EXPORTS */
        #define COB_DLL_EXPIMP __declspec(dllimport) extern 
    #endif /* LIBCOB_EXPORTS */

#else /* _MSC_VER */
    #define COB_DLL_EXPIMP extern 

#endif /* _MSC_VER */
#endif
#include <setjmp.h>
/*
* MF API
*/
struct cobjmp_buf {
    int      cbj_int[4];
    void    *cbj_ptr[4];
    jmp_buf  cbj_jmp_buf;
    void    *cbj_ptr_rest[2];
};
COB_DLL_EXPIMP void cobcancel   (const char *name);
COB_DLL_EXPIMP int  cobcall     (const char *name, const int argc, void **argv);
COB_DLL_EXPIMP int  cobfunc     (const char *name, const int argc, void **argv);
COB_DLL_EXPIMP void *cobsavenv  (struct cobjmp_buf *jbuf);
COB_DLL_EXPIMP void *cobsavenv2 (struct cobjmp_buf *jbuf, const int jsize);
COB_DLL_EXPIMP void coblongjmp  (struct cobjmp_buf *jbuf);
#define	cobsetjmp(x)	setjmp  (cobsavenv (x))

#endif

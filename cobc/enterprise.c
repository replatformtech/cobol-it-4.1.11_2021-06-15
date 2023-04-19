/*
  * Copyright (C) 2008-2020 Cobol-IT
 *
 * This code is proprety of COBOL-IT . 
 * Usage of this code or any part of it require a explicit writen authoristion of COBOL-IT
 */
#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "cobc.h"

extern const char              *cobolit_dir;


extern int  enterprise_extfct (unsigned short code, void*p1, void *p2, void*p3, void *p4, void *p5, void *p6);

int 
cob_enterprise_load (int codegen)
{
    if (!codegen &&
        enterprise_extfct(2, (void*)cobolit_dir, (void*)COB_PN_COMPILER, NULL, NULL, NULL, NULL) != 1) {
        if (enterprise_extfct(2, (void*)cobolit_dir, (void*)COB_PN_SYNTAX, NULL, NULL, NULL, NULL)  == 1) {
            return 1;
        } else if (enterprise_extfct(1, (void*)cobolit_dir, (void*)"COMMUNITY", NULL, NULL, NULL, NULL)  == 1) {
            return 0;
        }
    }
    if (enterprise_extfct(2, (void*)cobolit_dir, (void*)COB_PN_COMPILER, NULL, NULL, NULL, NULL) ) {
#if defined(CIT_EBCDIC_CHARSET)
        if (enterprise_extfct(1, (void*)cobolit_dir, (void*)"EBCDIC", NULL, NULL, NULL, NULL) )
#endif
            return 1;
    } else if (enterprise_extfct(1, (void*)cobolit_dir, (void*)"COMMUNITY", NULL, NULL, NULL, NULL)  == 1) {
        return 0;
    }
    return 0;
}




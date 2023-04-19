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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
    #include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "mf_extfh.h"
#include "fileio.h"

/*CIT_BEGIN_ENTERPRISE*/
#ifdef _MSC_VER

    #ifndef _CRT_SECURE_NO_DEPRECATE 
        #define _CRT_SECURE_NO_DEPRECATE 1
    #endif
    #define COB_DLL_EXPIMP __declspec(dllexport) extern 
#else
    #define COB_DLL_EXPIMP extern 
#endif

COB_DLL_EXPIMP int CIT_EXTSM(cit_uint8 *opcode , mf_extfh_FCD  *FCD);

#define local_cob_open          cob_open
#define local_cob_close         cob_close
#define local_cob_unlock_file   cob_unlock_file
#define local_cob_delete_file   cob_delete_file
#define local_cob_delete        cob_delete
#define local_cob_start         cob_start
#define local_cob_read          cob_read
#define local_cob_rewrite       cob_rewrite
#define local_cob_write         cob_write
#define local_cob_rollback      cob_rollback
#define local_cob_commit        cob_commit
#define local_flag_no_close_cache 1
//#include "../libcitextfh/cit_extfh_util.c"

static int using_cpt = 0;
typedef struct {
    cob_file_extfh        fsort;
    int             nusing;
    int             ngiving;
    cob_file_extfh        *files_using[MAX_EXTSM_USING_GIVING];
    cob_file_extfh        *files_giving[MAX_EXTSM_USING_GIVING];
    cob_field       fnstatus;
    cob_field_attr  attr;
} extsm_data;

static int set_int_status_FCD (COB_RTD, mf_extfh_FCD *FCD, int status) {

    FCD->user_file_status[0] = COB_I2D_n(status/10) ;
    FCD->user_file_status[1] = COB_I2D_n(status%10);
    return(status);
}

//static int open_EXTSM(COB_RTD, mf_extfh_FCD  *FCD, int sort)
//{
//    mf_extfh_FCD    ** file_def_block;
//
//    extsm_data      *smdata;
//    int             idx = 0;
//    int             i;
//    FLD_VAR;
//    mf_extfh_FCD   *fcd_file_using;
//
//    smdata = cob_malloc(rtd, sizeof(extsm_data));
//#ifdef COB_PTR_64BITS
//    smdata->nusing      = GT_FLD1(FCD->use_files[0] );
//    smdata->ngiving     = GT_FLD1(FCD->give_files[0]);
//    file_def_block  = GT_FLDP(FCD->fildef_address);
//#else
//    smdata->nusing      = GT_FLD1(FCD->key_id[0]);
//    smdata->ngiving     = GT_FLD1(FCD->key_id[1]);
//    file_def_block  = GT_FLDP(FCD->rel_byte_addr);
//#endif
//
//    smdata->fsort.extfh_ptr = FCD;
//
//    for ( i = 0; i < smdata->nusing; i++, idx++ ) {
//        smdata->files_using [i] =  cob_malloc(rtd, sizeof(cob_file_extfh));
//        smdata->files_using [i]->extfh_ptr = file_def_block[idx];
//        fcd_file_using = smdata->files_using [i]->extfh_ptr;
//        if ( GT_FLD2_4(FCD->max_rec_length) < GT_FLD2_4(fcd_file_using->max_rec_length) ) {
//            CP_FLD2_4(FCD->max_rec_length, GT_FLD2_4(fcd_file_using->max_rec_length));
//        }
//    }
//    for ( i = 0; i < smdata->ngiving; i++, idx++ ) {
//        smdata->files_giving [i] = cob_malloc(rtd, sizeof(cob_file_extfh));
//        smdata->files_giving [i]->extfh_ptr = file_def_block[idx];
//    }
//    CP_FLD1(FCD->organization, COB_ORG_SORT);
//
//    if (GT_FLD2_4(FCD->max_rec_length) < GT_FLD2_4(FCD->cur_rec_length)) {
//        CP_FLD2_4(FCD->max_rec_length, GT_FLD2_4(FCD->cur_rec_length));
//    } else {
//        CP_FLD2_4(FCD->cur_rec_length, GT_FLD2_4(FCD->max_rec_length));
//    }
//
//    CP_FLDP(FCD->handle_x, smdata);
//    smdata->fnstatus.size = 2;
//    smdata->fnstatus.data = FCD->user_file_status;
//    smdata->fnstatus.attr = &(smdata->attr);
//    cobsort_init(rtd, &(smdata->fsort), NULL, &(smdata->fnstatus));
//    if ( smdata->nusing ) {
//        for ( i = 0; i < smdata->nusing; i++) {
//            cob_file_sort_using_extfh(rtd,&(smdata->fsort), smdata->files_using[i]);
//        }
//        if ( smdata->ngiving && (!rtd->cob_exception_code) ) {
//            cob_file_sort_giving_table_extfh(rtd,&(smdata->fsort), smdata->ngiving, smdata->files_giving);
//        }
//
//    }
//    return 0;
//}

static int open_EXTSM(COB_RTD, mf_extfh_FCD  *FCD, int sort)
{
    mf_extfh_FCD    ** file_def_block;

    extsm_data      *smdata;
    int             idx = using_cpt;
    int             i   = using_cpt;
	FLD_VAR
    mf_extfh_FCD   *fcd_file_using;
    if (GT_FLDP(FCD->handle_x) == NULL) {
        smdata = cob_malloc(rtd, sizeof(extsm_data));

        smdata->fsort.extfh_ptr = FCD;

        CP_FLDP(FCD->handle_x, smdata);
        smdata->fnstatus.size = 2;
        smdata->fnstatus.data = FCD->user_file_status;
        smdata->fnstatus.attr = &(smdata->attr);
        cobsort_init(rtd, &(smdata->fsort), NULL, &(smdata->fnstatus));
    }
    else
        smdata = GT_FLDP(FCD->handle_x);  

#ifdef COB_PTR_64BITS
    smdata->nusing      = GT_FLD1(FCD->use_files[0] );
    smdata->ngiving     = GT_FLD1(FCD->give_files[0]);
    file_def_block  = GT_FLDP(FCD->fildef_address);
#else
    smdata->nusing      = GT_FLD1(FCD->key_id[0]);
    smdata->ngiving     = GT_FLD1(FCD->key_id[1]);
    file_def_block  = GT_FLDP(FCD->rel_byte_addr);
#endif
    if (smdata->nusing != 255) {
        for (i = using_cpt; i < smdata->nusing; i++, idx++) {
            smdata->files_using [i] =  cob_malloc(rtd, sizeof(cob_file_extfh));
            smdata->files_using [i]->extfh_ptr = file_def_block[idx];
            fcd_file_using = smdata->files_using [i]->extfh_ptr;
            if ( GT_FLD2_4(FCD->max_rec_length) < GT_FLD2_4(fcd_file_using->max_rec_length) ) {
                CP_FLD2_4(FCD->max_rec_length, GT_FLD2_4(fcd_file_using->max_rec_length));
            }
        }
    }
    idx = i;
    for ( i = 0; i < smdata->ngiving; i++, idx++ ) {
        smdata->files_giving [i] = cob_malloc(rtd, sizeof(cob_file_extfh));
        smdata->files_giving [i]->extfh_ptr = file_def_block[idx];
    }
    CP_FLD1(FCD->organization, COB_ORG_SORT);

    if (GT_FLD2_4(FCD->max_rec_length) < GT_FLD2_4(FCD->cur_rec_length)) {
        CP_FLD2_4(FCD->max_rec_length, GT_FLD2_4(FCD->cur_rec_length));
    } else {
        CP_FLD2_4(FCD->cur_rec_length, GT_FLD2_4(FCD->max_rec_length));
    }

    if ( smdata->nusing > 0 && smdata->nusing != 255) {
        for ( i = using_cpt; i < smdata->nusing; i++) {
            cob_file_sort_using_extfh(rtd,&(smdata->fsort), smdata->files_using[i]);
        }
        if ( smdata->ngiving && (!rtd->cob_exception_code) ) {
            cob_file_sort_giving_table_extfh(rtd,&(smdata->fsort), smdata->ngiving, smdata->files_giving);
        }

    }
    if (smdata->ngiving > 0) {
        using_cpt = 0;
    }
    else{
        using_cpt = idx;
    }
    /*a particular case for INPUT PROCEDURE GIVING FILE while executing 3.x sources. 
      because of the extsm_end_input is not generated with 3.x sources*/
    if (smdata->nusing == 255) {
        if ( smdata->ngiving) {
            cob_file_sort_giving_table_extfh(rtd,&(smdata->fsort), smdata->ngiving, smdata->files_giving);
            using_cpt = 0;
        }
    }
    return 0;
}

static int close_EXTSM(COB_RTD, mf_extfh_FCD  *FCD)
{
    int i;
    extsm_data *smdata;
    FLD_VAR;

    smdata = GT_FLDP(FCD->handle_x);    
    if ( !smdata ) {
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }
    cob_file_sort_close_extfh(rtd,&(smdata->fsort));

    for ( i = 0; i < smdata->nusing; i++ ) {
        cob_free(smdata->files_using[i]);
    }
    for ( i = 0; i < smdata->ngiving; i++ ) {
        cob_free(smdata->files_giving[i]);
    }
    cob_free(smdata);
    CP_FLDP(FCD->handle_x, NULL);
    using_cpt = 0;
    return 0;
}

static int input_EXTSM(COB_RTD, mf_extfh_FCD  *FCD)
{
    extsm_data *smdata;
    register int min_rec, cur_rec;
    FLD_VAR;

    smdata = GT_FLDP(FCD->handle_x);    
    if ( !smdata ) {
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }

    cur_rec = GT_FLD2_4(FCD->cur_rec_length);
    min_rec = GT_FLD2_4(FCD->min_rec_length);
    if ( min_rec > cur_rec ) {
        CP_FLD2_4(FCD->cur_rec_length, min_rec);
    }
    cob_file_release_extfh (rtd,&(smdata->fsort));   
    return 0;
}

static int input_end_EXTSM(COB_RTD, mf_extfh_FCD  *FCD)
{
    extsm_data *smdata;
    FLD_VAR;

    smdata = GT_FLDP(FCD->handle_x);    
    if ( !smdata ) {
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }
    if ( smdata->ngiving ) {
        cob_file_sort_giving_table_extfh(rtd,&(smdata->fsort), smdata->ngiving, smdata->files_giving);
    }
    return 0;
}

static int output_EXTSM(COB_RTD, mf_extfh_FCD  *FCD)
{
    extsm_data *smdata;
    register int min_rec, cur_rec;
    FLD_VAR;

    smdata = GT_FLDP(FCD->handle_x);    
    if ( !smdata ) {
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }

    cur_rec = GT_FLD2_4(FCD->cur_rec_length);
    min_rec = GT_FLD2_4(FCD->min_rec_length);
    if ( min_rec > cur_rec ) {
        CP_FLD2_4(FCD->cur_rec_length, min_rec);
    }
    cob_file_return_extfh (rtd,&(smdata->fsort));   
    return 0;
}

static int BASE_EXTSM(COB_RTD, cit_uint8 *opcode , mf_extfh_FCD  *FCD)
{
    cit_uint8 grp, code;


    if ( !opcode ) {
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }
#ifdef COB_PTR_64BITS
    if ( GT_FLD1(FCD->version) != 1 ) {
        fprintf(stderr, "CIT-EXTSM 64 bits Only support FCD 3 \n");
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }
#else
    if ( GT_FLD1(FCD->version) != 0 ) {
        fprintf(stderr, "CIT-EXTSM 32 bits Only support FCD 2 \n");
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }
#endif
    grp  = opcode[0];
    code = opcode[1];
    if ( grp == 0xFA ) {
        switch ( code ) {
            case 0x01: return(open_EXTSM(rtd, FCD, 1));
            case 0x40: return(open_EXTSM(rtd, FCD, 0));
            case 0xF3: return(input_EXTSM(rtd, FCD));
            case 0x10: return(input_end_EXTSM(rtd, FCD));
            case 0xF5: return(output_EXTSM(rtd, FCD));
            case 0x88: return(close_EXTSM(rtd, FCD));

        }
    }
    return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
}

int CIT_EXTSM(cit_uint8 *opcode , mf_extfh_FCD  *FCD)
{
    COB_RTD = cob_get_rtd(); 

    if ( !rtd ) {
        fprintf(stderr, "CIT-EXTSM rtd == NULL \n");
        return(set_int_status_FCD(rtd, FCD,COB_STATUS_30_PERMANENT_ERROR));
    }
    cob_init(rtd, 0, NULL);

    return BASE_EXTSM(rtd, opcode, FCD);
}

/*CIT_END_ENTERPRISE*/
int dummyEXTSM(void);
int dummyEXTSM(void)
{
    /* for stupid AIX Linker*/
    return 0;
}


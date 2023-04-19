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

#ifndef MF_EXTFH_H
#  define MF_EXTFH_H

/* #include "config.h"
#include "defaults.h"
#include "globaldefine.h"*/
/* this module implement the MicroFocus(tm) EXTFH inteface */
/* DEFINE COB_PTR_64BITS if build for 64 bits */
#  include "cit_types.h"
#  include "libcob.h"


#  define INTSZ  sizeof(int)
/*EXTFH VALUES*/
/* Organization */
#  define FCD_ORG_LINE_SEQUENTIAL 0
#  define FCD_ORG_SEQUENTIAL      1
#  define FCD_ORG_INDEXED         2
#  define FCD_ORG_RELATIVE        3

/* Access mode */
#  define FCD_ACCESS_SEQUENTIAL   0
#  define FCD_ACCESS_RANDOM       4
#  define FCD_ACCESS_DYNAMIC      8

/*Lock mode*/
#  define FCD_LOCK_EXCLUSIVE      0x01
#  define FCD_LOCK_AUTOMATIC      0x02
#  define FCD_LOCK_MANUAL         0x04
#  define FCD_LOCK_MULTIPLE       0x80

/* Open mode */
//#define FCD_OPEN_INPUT          0x00
//#define FCD_OPEN_OUTPUT         0x01
//#define FCD_OPEN_I_O            0x02
//#define FCD_OPEN_EXTEND         0x03
#  define FCD_OPEN_CLOSED         0x80

/* Device Flag */
#  define FCD_DEVICE_NORMAL       0
#  define FCD_DEVICE_DEVICE       1
#  define FCD_DEVICE_STDIN        2
#  define FCD_DEVICE_STDOUT       3
#  define FCD_DEVICE_STDERR       4

/*BIT VALUES*/
/* status type */
#  define FCD_STATUS_TYPE_BIT_INSERT_NULL      status_type,1
#  define FCD_STATUS_TYPE_BIT_INSERT_TAB       status_type,2
#  define FCD_STATUS_TYPE_BIT_LS_REC_DELIM     status_type,3

/*adv_flags*/
#  define FCD_ADV_FLAGS_BIT_MODE_TRUNC      adv_flags,0 /*CIT behaviour*/
#  define FCD_ADV_FLAGS_BIT_UTF16           adv_flags,1 /*CIT behaviour*/
#  define FCD_ADV_FLAGS_BIT_UTF16_LE        adv_flags,2 /*CIT behaviour*/
#  define FCD_ADV_FLAGS_BIT_EMULATE_VMS     adv_flags,4 /*CIT behaviour*/

/* other_flags */
#  define FCD_OTHER_FLAGS_BIT_OPTIONAL_INPUT      other_flags,7
#  define FCD_OTHER_FLAGS_BIT_IGNORELOCK          other_flags,6
#  define FCD_OTHER_FLAGS_BIT_NOT_OPTIONAL_IO     other_flags,5
#  define FCD_OTHER_FLAGS_BIT_SELECT_EXTERNAL     other_flags,4
#  define FCD_OTHER_FLAGS_BIT_LineAdv             other_flags,0
#  define FCD_OTHER_FLAGS_BIT_OPTIONAL_IN_SOURCE  other_flags,3
/* flags_1 */
#  define FCD_FLAG_1_BIT_MAINFRAME_FILES flags_1,2
#  define FCD_FLAG_1_BIT_TRACE_ON        flags_1,1
/* file_share_flag */
#  define FCD_FILE_SHARE_FLAG_FLAG_WITH_ROLLBACK file_share_flag,7


typedef struct {
    /*  Offset  Size    Description of the Field               */
    cit_uint8 key_def_block_len[2]; /*  0       2       Length of the Key Definition Block     */
    cit_uint8 filler_1[4];         /*  2       4       Reserved. Must be set to binary zeros  */
    cit_uint8 keys_count[2];       /*  6       2       Number of keys                         */
    cit_uint8 filler_2[6];         /*  8       6       Reserved. Must be set to binary zeros. */

} mf_extfh_global_info;

typedef struct {
    /*  Offset  Size    Description of the Field                                 */
    cit_uint8 comp_count[2];      /*  0       2       Component count.                                         */
    /*                  For ordinary keys, the component count is 1.             */
    /*                  For split keys, the component count is the number        */
    /*                  of components making up the split key.                   */
    cit_uint8 comp_offset[2];     /*  2       2       Offset to first Component Definition area for this key.  */
    /*                  This offset is relative to the start of the Global       */
    /*                    Information area, starting at 0.                       */
    cit_uint8 key_flags;          /*  4       1       Key flags:                                               */
    /*                      Bit 7 - Reserved. Must be set to binary zeros        */
    /*                      Bit 6 - Duplicates allowed                           */
    /*                      Bit 5 - Reserved. Must be set to binary zeros        */
    /*                      Bit 4 - Set to indicate that this is the prime key.  */
    /*                              If this is not set for any key, the file     */
    /*                              handler assumes that the first key is the    */
    /*                              prime key.                                   */
    /*                      Bit 3 - Reserved. Must be set to binary zeros        */
    /*                      Bit 2 - Reserved. Must be set to binary zeros        */
    /*                      Bit 1 - Sparse key. See offset 6                     */
    /*                      Bit 0 - Reserved. Must be set to binary zeros.       */
    cit_uint8 compression_flags;  /*  5       1       Compression flags:                                       */
    /*                      Bits 7-3  - Reserved. Must be set to binary zeros    */
    /*                      Bit 2  - Compression of trailing spaces              */
    /*                      Bit 1  - Compression of leading characters           */
    /*                      Bit 0  - Compression of duplicates.                  */
    cit_uint8 spare_char;         /*  6       1       Sparse character.                                        */
    /*                      If bit 1 of the key flags (offset 4) is set,         */
    /*                          the key is suppressed                            */
    /*                      if it is entirely made up of this character.         */
    cit_uint8 filler[9];          /*  7       9       Reserved. Must be set to binary zeros.                   */

} mf_extfh_key_def;

typedef struct {
    /*  Offset  Size    Description of the Field                           */
    cit_uint8   filer[2];         /*  0       2       Reserved. Must be set to binary zeros.             */
    cit_uint8   offset[4];        /*  2       4       Offset of component in the record (starting at 0). */
    cit_uint8   length[4];        /*  6       4       Length of the component (in bytes).                */
} mf_extfh_comp_def;

/*typedef char sort_filename[COB_MEDIUM_BUFF];*/




#  define CP_FLD1_p(d,s)                 \
    *(d) = (cit_uint8)(s);

#  define GT_FLD1_p(d)   (*((cit_uint8 *)(d)))
#  ifdef __GNUC__
#     define UNUSEDATT __attribute__((unused))
#  else
#     define UNUSEDATT
#  endif

#  define FLD_VAR   UNUSEDATT cit_uint16 _s16=0 ; \
                      UNUSEDATT cit_uint32 _s32=0 ; \
                      UNUSEDATT cit_uint64 _s64=0 ; \
                      UNUSEDATT void *     _spt=NULL;

#  define FLD_VAR_INIT


#  ifdef WORDS_BIGENDIAN
#     define CP_FLD2_p(d,s)                 \
    {                           \
    _s16 = (cit_uint16)(s);       \
    memcpy((d), &(_s16), 2);      \
    }

#     define GT_FLD2_p(d)   ( memcpy(&_s16, (d), 2), _s16)

#     define CP_FLD4_p(d,s)                 \
    {                           \
    _s32 = (cit_uint32)(s);       \
    memcpy((d), &(_s32), 4);      \
    }

#     define GT_FLD4_p(d)   ( memcpy(&_s32, (d), 4), _s32)

#     define CP_FLD8_p(d,s)                 \
    {                           \
    _s64 = (cit_uint64)(s);       \
    memcpy((d), &(_s64), 8);      \
    }

#     define GT_FLD8_p(d)   ( memcpy(&_s64, (d), 8), _s64)

#  else

#     define CP_FLD2_p(d,s)                 \
    {                           \
    _s16=COB_BSWAP_16((cit_uint16)s);        \
    memcpy((d), &(_s16), 2);   \
    }

#     define GT_FLD2_p(d)   ( memcpy(&_s16, (d), 2), COB_BSWAP_16(_s16))

#     define CP_FLD4_p(d,s)                 \
    {                           \
    _s32=COB_BSWAP_32((cit_uint32)s);          \
    memcpy((d), &(_s32), 4);      \
    }

#     define GT_FLD4_p(d)   (memcpy(&_s32, (d), 4), COB_BSWAP_32(_s32)  )

#     define CP_FLD8_p(d,s)                 \
    {                           \
    _s64=COB_BSWAP_64((cit_uint64)s);          \
    memcpy((d), &(_s64), 8);      \
    }

#     define GT_FLD8_p(d)   (memcpy(&_s64, (d), 8), COB_BSWAP_64(_s64)  )

#  endif /*WORDS_BIGENDIAN*/

#  define CP_FLDP_p(d,s)                   \
    {                                          \
    _spt = (void*)(s);                         \
    memcpy((d), &(_spt), sizeof(void*));      \
    }

#  define GT_FLDP_p(d)    ( memcpy(&_spt, (d), sizeof(void*)), _spt)

#  define CP_FLD1(d,s)   CP_FLD1_p(&(d),s)
#  define CP_FLD2(d,s)   CP_FLD2_p(&(d),s)
#  define CP_FLD4(d,s)   CP_FLD4_p(&(d),s)
#  define CP_FLD8(d,s)   CP_FLD8_p(&(d),s)
#  define CP_FLDP(d,s)   CP_FLDP_p(&(d),s)
#  define GT_FLD1(d)     GT_FLD1_p(&(d))
#  define GT_FLD2(d)     GT_FLD2_p(&(d))
#  define GT_FLD4(d)     GT_FLD4_p(&(d))
#  define GT_FLD8(d)     GT_FLD8_p(&(d))
#  define GT_FLDP(d)     GT_FLDP_p(&(d))


#  ifdef COB_PTR_64BITS
#     define GT_FLD2_4(d)   GT_FLD4(d)
#     define GT_FLD4_8(d)   GT_FLD8(d)
#     define CP_FLD2_4(d,s) CP_FLD4(d,s)
#     define CP_FLD4_8(d,s) CP_FLD8(d,s)
#  else
#     define GT_FLD2_4(d)   GT_FLD2(d)
#     define GT_FLD4_8(d)   GT_FLD4(d)
#     define CP_FLD2_4(d,s) CP_FLD2(d,s)
#     define CP_FLD4_8(d,s) CP_FLD4(d,s)
#  endif

static COB_INLINE void set_bit_on(cit_uint8 *d, int b)
{
   *d |= (1 << b);
}

static COB_INLINE void set_bit_off(cit_uint8 *d, int b)
{
   *d &= ~(1 << b);
}

static COB_INLINE int is_bit(cit_uint8 d, int b)
{
   return (d & (1 << b));
}
/*Operatons on fcd bits values*/
#  define SETB_ON_FLD1(d)             set_bit_on(&d)
#  define SETB_OFF_FLD1(d)            set_bit_off(&d)
#  define ISB_FLD1_M(d,b)             (GT_FLD1(d) & (1 << b))
#  define ISB_FLD1(d)                 is_bit(d)
#  define IS_VAR_REC_LONG(fcd)        (GT_FLD2_4(fcd->max_rec_length) > 0xFFF)
#  define IS_VAR_REC_SHORT(fcd)       !(IS_VAR_REC_LONG(fcd))
#  define IS_VAR_REC_MAINFRAME(fcd)   ( ISB_FLD1(fcd->FCD_FLAG_1_BIT_MAINFRAME_FILES))
#  define FCD_VAR_REC_MODE(fcd)       ISB_FLD1_M(fcd->recording_mode, 0)

#  ifdef __GNUC__
extern void* extfh_cob_alloc_file(COB_RTD) __attribute__((malloc)); /*3x functions*/
#  else
COB_DLL_EXPIMP void* extfh_cob_alloc_file(COB_RTD); /*3x functions*/
#  endif
//COB_DLL_EXPIMP void extfh_cob_init_fileio   (COB_RTD, const struct cob_fileio_funcs *seqfunc,
//                                             const struct cob_fileio_funcs *lsqfunc, const struct cob_fileio_funcs *relfunc,
//                                             int (*write_opt)());
COB_DLL_EXPIMP void extfh_cob_init_fileio(COB_RTD);
COB_DLL_EXPIMP void extfh_cob_exit_fileio(COB_RTD);

COB_DLL_EXPIMP int extfh_unlock(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void extfh_index_unlock(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP int extfh_index_locate(COB_RTD, cob_file_extfh *f, char *filename);

COB_DLL_EXPIMP void extfh_seqra_unlock(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP int extfh_seqra_locate(COB_RTD, cob_file_extfh *f, char *filename);

COB_DLL_EXPIMP int extfh_open(COB_RTD, cob_file_extfh *f, int mode, int opt, int lock);
COB_DLL_EXPIMP int extfh_read(COB_RTD, cob_file_extfh *f, cob_field *key, int read_opts);
COB_DLL_EXPIMP int extfh_write(COB_RTD, cob_file_extfh *f, int opt);
COB_DLL_EXPIMP int extfh_close(COB_RTD, cob_file_extfh *f, int opt);
COB_DLL_EXPIMP int extfh_start_indexed(COB_RTD, cob_file_extfh *f, int cond, const int id_key, const int len);
COB_DLL_EXPIMP int extfh_start_relative (COB_RTD, cob_file_extfh *f, int cond, cob_field *key);
COB_DLL_EXPIMP int extfh_rewrite(COB_RTD, cob_file_extfh *f, int opt);
COB_DLL_EXPIMP int extfh_delete(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP int extfh_get_int_status(COB_RTD, cob_file_extfh *f);

COB_DLL_EXPIMP int extfh_delete_file(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP int extfh_commit(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP int extfh_rollback(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void * cob_get_extfh_func(void);
COB_DLL_EXPIMP void * cob_get_extfh_fcd_ptr(COB_RTD, cob_file_extfh *f);

COB_DLL_EXPIMP void cob_update_return_status_to_field(COB_RTD, cob_file_extfh *f, cob_field *fnstatus);

/* this module implement the MicroFocus(tm) EXTSM inteface */

COB_DLL_EXPIMP void extsm_sort_init_extfh(COB_RTD, cob_file_extfh *f, int nkeys,
                                          const unsigned char *collating_sequence,
                                          void *sort_return, cob_field *fnstatus);

COB_DLL_EXPIMP void extsm_sort_close_extfh(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void extsm_sort_using_extfh(COB_RTD, cob_file_extfh *sort_file, cob_file_extfh *data_file);
COB_DLL_EXPIMP void extsm_sort_giving_extfh(COB_RTD, cob_file_extfh *sort_file, size_t varcnt, ...);
COB_DLL_EXPIMP void extsm_release_extfh(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void extsm_release_1_extfh(COB_RTD, cob_file_extfh *f, cob_field *rec);
COB_DLL_EXPIMP void extsm_return_extfh(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void extsm_process_extfh(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void extsm_end_output_extfh(COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void extsm_end_input_extfh(COB_RTD, cob_file_extfh *f);

cit_uint8   cobget_x1_compx(const char *cbldata);
cit_uint16  cobget_x2_compx(const char *cbldata);
cit_uint32  cobget_x4_compx(const char *cbldata);
cit_uint64  cobget_x8_compx(const char *cbldata);
void       *cobget_pointer(const char *cbldata);
void       *cobget_ppointer(const char *cbldata);

void cobput_x1_compx(char *cbldata, cit_uint8 val);
void cobput_x2_compx(char *cbldata, cit_uint16 val);
void cobput_x4_compx(char *cbldata, cit_uint32 val);
void cobput_x8_compx(char *cbldata, cit_uint64 val);
void cobput_pointer(char *cbldata, void *val);


#endif  /*MF_EXTFH_H*/

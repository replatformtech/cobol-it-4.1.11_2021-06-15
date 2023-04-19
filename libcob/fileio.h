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
 * License along with this library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

#ifndef COB_FILEIO_H
#define COB_FILEIO_H

#include <limits.h>
#include <libcob/common.h>
/* Current File version */
#define COB_FILE_VERSION        4

#define COB_EQ                  1       /* x == y */
#define COB_LT                  2       /* x <  y */
#define COB_LE                  3       /* x <= y */
#define COB_GT                  4       /* x >  y */
#define COB_GE                  5       /* x >= y */
#define COB_NE                  6       /* x != y */

#define COB_ASCENDING           0
#define COB_DESCENDING          1

#define COB_FILE_MODE           0644

/* Organization */

#define COB_ORG_SEQUENTIAL      0
#define COB_ORG_LINE_SEQUENTIAL 1
#define COB_ORG_RELATIVE        2
#define COB_ORG_INDEXED         3
#define COB_ORG_SORT            4
#define COB_ORG_RELATIVE_MF     5
#define COB_ORG_MAX             6

/* Access mode */

#define COB_ACCESS_SEQUENTIAL   1
#define COB_ACCESS_DYNAMIC      2
#define COB_ACCESS_RANDOM       3

/* SELECT features */

#define COB_SELECT_FILE_STATUS  0x01
#define COB_SELECT_EXTERNAL     0x02
#define COB_SELECT_LINAGE       0x04
#define COB_SELECT_SPLITKEY     0x08
#define COB_SELECT_FORCE_EXTERNAL 0x10

/* Sharing mode */

#define COB_SHARE_NONE          0
#define COB_SHARE_WITH_NO_OTHER 1
#define COB_SHARE_WITH_READ     2
#define COB_SHARE_WITH_ALL      3
#define COB_SHARE_MASK          3

/* Lock mode */

#define COB_LOCK_EXCLUSIVE      1
#define COB_LOCK_MANUAL         2
#define COB_LOCK_AUTOMATIC      4
#define COB_LOCK_MULTIPLE       8
#define COB_LOCK_MASK           0x7

/* Open mode */

#define COB_OPEN_CLOSED         0x00
#define COB_OPEN_INPUT          0x01
#define COB_OPEN_OUTPUT         0x02
#define COB_OPEN_I_O            0x03
#define COB_OPEN_EXTEND         0x04
#define COB_OPEN_LOCKED         0x05
#define COB_OPEN_INPUT_REVERSED 0x06

#define COB_OPEN_IGNORE_OPTIONAL 0x0100
#define COB_OPEN_MODE_MASK       0x00FF
#define COB_OPEN_OPTION_MASK     0xFF00
/* Close options */

#define COB_CLOSE_NORMAL        0
#define COB_CLOSE_LOCK          1
#define COB_CLOSE_NO_REWIND     2
#define COB_CLOSE_UNIT          3
#define COB_CLOSE_UNIT_REMOVAL  4

/* Write options */

#define COB_WRITE_MASK          0x0000ffff
#define COB_WRITE_LINES         0x00010000
#define COB_WRITE_PAGE          0x00020000
#define COB_WRITE_CHANNEL       0x00040000
#define COB_WRITE_AFTER         0x00100000
#define COB_WRITE_BEFORE        0x00200000
#define COB_WRITE_EOP           0x00400000
#define COB_WRITE_LOCK          0x00800000

/* Read options */
#define COB_READ_NEXT           0x01
#define COB_READ_PREVIOUS       0x02
#define COB_READ_FIRST          0x04
#define COB_READ_LAST           0x08
#define COB_READ_DIR_MASK       0x0F
#define COB_READ_LOCK           0x10
#define COB_READ_NO_LOCK        0x20
#define COB_READ_KEPT_LOCK      0x40
#define COB_READ_WAIT_LOCK      0x80
#define COB_READ_LOCK_MASK      0xF0
#define COB_READ_IGNORE_LOCK    0x100

/* Key Compress mode */
#define COB_KEY_COMP_DUP        0x01 /*Duplicate key compression.*/
#define COB_KEY_COMP_LEAD       0x02 /*Leading character compression.*/
#define COB_KEY_COMP_TRAIL_SP   0x04 /*Trailing space compression*/
#define COB_KEY_COMP_TRAIL_ZR   0x08 /*Trailing null compression.*/
#define COB_KEY_COMP_ALL        0xFF /*All compression*/
/* I-O status */

#define COB_STATUS_00_SUCCESS                   00
#define COB_STATUS_02_SUCCESS_DUPLICATE         02
#define COB_STATUS_04_SUCCESS_INCOMPLETE        04
#define COB_STATUS_05_SUCCESS_OPTIONAL          05
#define COB_STATUS_07_SUCCESS_NO_UNIT           07
#define COB_STATUS_10_END_OF_FILE               10
#define COB_STATUS_14_OUT_OF_KEY_RANGE          14
#define COB_STATUS_21_KEY_INVALID               21
#define COB_STATUS_22_KEY_EXISTS                22
#define COB_STATUS_23_KEY_NOT_EXISTS            23
#define COB_STATUS_24_NOT_A_KEY                 24
#define COB_STATUS_30_PERMANENT_ERROR           30
#define COB_STATUS_31_INCONSISTENT_FILENAME     31
#define COB_STATUS_34_BOUNDARY_VIOLATION        34
#define COB_STATUS_35_NOT_EXISTS                35
#define COB_STATUS_37_PERMISSION_DENIED         37
#define COB_STATUS_38_CLOSED_WITH_LOCK          38
#define COB_STATUS_39_CONFLICT_ATTRIBUTE        39
#define COB_STATUS_41_ALREADY_OPEN              41
#define COB_STATUS_42_NOT_OPEN                  42
#define COB_STATUS_43_READ_NOT_DONE             43
#define COB_STATUS_44_RECORD_OVERFLOW           44
#define COB_STATUS_46_READ_ERROR                46
#define COB_STATUS_47_INPUT_DENIED              47
#define COB_STATUS_48_OUTPUT_DENIED             48
#define COB_STATUS_49_I_O_DENIED                49
#define COB_STATUS_51_RECORD_LOCKED             51
#define COB_STATUS_52_EOP                       52
#define COB_STATUS_57_I_O_LINAGE                57
#define COB_STATUS_61_FILE_SHARING              61
#define COB_STATUS_91_NOT_AVAILABLE             91
#define COB_STATUS_9x_EXTFH_ASSIGNED            100
#define COB_STATUS_xx_EXTFH_ASSIGNED            101

/* Special status */

/* Need some value that does not conflict with errno for OPEN/LINAGE */
#define COB_LINAGE_INVALID      16384
/* Need value that does not conflict with errno 30 (EROFS) for OPEN */
#define COB_NOT_CONFIGURED      32768
#define COB_TRACE_FILE 0x10

/*CIT*/

/* Flag_optional*/
#define COB_FILE_OPTIONAL_IN_SOURCE   0x10
#define COB_FILE_OPTIONAL             0x01
#define COB_FILE_NOT_OPTIONAL         0x00
#define COB_FILE_OPTIONAL_MASK        0x0F

#define PTRSZ  8
/* ExtFH FCD3*/
typedef struct {
    cit_uint8 user_file_status[2];            /*0*/
    cit_uint8 fcd_len[2];                      
    cit_uint8 version;                
    cit_uint8 organization;           
    cit_uint8 access_mode;                 
    cit_uint8 open_mode;              
    cit_uint8 recording_mode;         
    cit_uint8 file_format;            
    cit_uint8 device_flag;                       
    cit_uint8 filler4;                       
    cit_uint8 data_compression;          
    cit_uint8 blocking;               
    cit_uint8 idxcache_size;          
    cit_uint8 percent;                
    cit_uint8 filler5[1];                       
    cit_uint8 flags_1;                
    cit_uint8 filler6[1];                       
    cit_uint8 mvs_flags[1];                       
    cit_uint8 status_type;            
    cit_uint8 other_flags;               
    cit_uint8 trans_log;              
    cit_uint8 interlang_lock;              
    cit_uint8 file_share_flag;               
    cit_uint8 config_flags;           
    cit_uint8 misc_flags;             
    cit_uint8 adv_flags;          
    cit_uint8 lock_mode;              
    cit_uint8 filler8[1];                       
    cit_uint8 idxcache_buffs;         
    cit_uint8 filler9[1];                       
    cit_uint8 filler10[1];                      
    cit_uint8 filler11[15];                     
    cit_uint8 filler12[2];                      
    cit_uint8 file_handle1[2];             
    cit_uint8 filler13[2];                      
    cit_uint8 fname_length[2];            
    cit_uint8 idxname_length[2];                      
    cit_uint8 filler15[2];                      
    cit_uint8 key_id[2];                 
    cit_uint8 line_count[2];                      
    cit_uint8 use_files[1];                      
    cit_uint8 give_files[1];                      
    cit_uint8 key_length[2];             
    cit_uint8 filler19[20];                     
    cit_uint8 cur_rec_length[4];         
    cit_uint8 min_rec_length[4];         
    cit_uint8 max_rec_length[4];         
    cit_uint8 fs_session_id[4];          
    cit_uint8 filler20[24];                    
    cit_uint8 rel_byte_addr[8];   
    cit_uint8 max_rel_key[8];            
    cit_uint8 rel_rec_number[8];           
    cit_uint8 handle_x[PTRSZ];                   
    cit_uint8 rec_data_ptr[PTRSZ];           
    cit_uint8 fname_ptr[PTRSZ];             
    cit_uint8 index_name[PTRSZ] ;                     
    cit_uint8 key_def_block_ptr[PTRSZ];           
    cit_uint8 sort_colating[PTRSZ];          
    cit_uint8 fildef_address[PTRSZ] ;                     
    cit_uint8 dfsort_address[PTRSZ];                      

} mf_extfh_FCD3;                                                                                 

/* ExtFH FCD2*/
typedef struct {
    cit_uint8 user_file_status[2];/*  0      2   User file status.  Also see offsets 6 and      */
                                  /*             33.                                            */
                                  /*                                                            */
                                  /*             After every operation except a COMMIT or       */
                                  /*             ROLLBACK operation, this field will be         */
                                  /*             updated with a standard file status value      */
                                  /*             telling you the status of the operation.       */
                                  /*             See your COBOL System Reference for            */
                                  /*             details of file status.                        */
                                  /*                                                            */
    cit_uint8 fcd_len[2];         /*  2      2   Reserved.  Must contain binary zeros.          */
    cit_uint8 version;            /*  4      1   must be zero.                                  */
                                  /*                                                            */
    cit_uint8 organization;       /*  5      1   File organization.                             */
                                  /*                                                            */
                                  /*             0x00 - Line sequential                         */
                                  /*             0x01 - Sequential                              */
                                  /*             0x02 - Indexed                                 */
                                  /*             0x03 - Relative                                */
                                  /*             0x13 - Relative  MF                            */
                                  /*                                                            */
    cit_uint8 access_mode;        /*  6      1   Access Mode and User Status indicators.        */
                                  /*                                                            */
                                  /*             Access mode is indicated by bits 6-0:          */
                                  /*                                                            */  
                                  /*             0 - Sequential access mode                     */
                                  /*             4 - Random access mode                         */
                                  /*             8 - Dynamic access mode                        */
                                  /*                                                            */
                                  /*             User Status is indicated by bit 7.  The        */
                                  /*             bit is set if you have defined a file          */
                                  /*             status.  Defining a file status can affect     */
                                  /*             how some operations (such as RETRYLOCK)        */
                                  /*             are performed.                                 */
                                  /*             Note:                                          */
                                  /*                                                            */
                                  /*             When calling the File Handler directly,        */
                                  /*             this bit should be set.  However, file         */
                                  /*             status is obtained directly from offset 0.     */
                                  /*                                                            */
    cit_uint8 open_mode;          /*  7      1   Open mode:                                     */
                                  /*                                                            */
                                  /*             0 - INPUT                                      */
                                  /*             1 - OUTPUT                                     */
                                  /*             2 - I/O                                        */
                                  /*             3 - EXTEND                                     */
                                  /*             128 - File is closed.                          */
                                  /*                                                            */
                                  /*             You must set this field to 128 before          */
                                  /*             opening a file.                                */
                                  /*                                                            */
    cit_uint8 flller_2[3];        /*  8      3   Reserved.  Must contain binary zeros.          */
                                  /*                                                            */
    cit_uint8 fname_length[2];    /*  11     2   Length of file-name.                           */
    cit_uint8 device_flag;        /*  13     1   device flag                                    */
                                  /*                0 	Normal                                  */
                                  /*                1 	Device                                  */  
                                  /*                2 	Stdin                                   */  
                                  /*                3 	Stdout                                  */  
                                  /*                4 	Stderr                                  */  
                                  /*                5 	Badname                                 */  
                                  /*                6 	Input pipe                              */  
                                  /*                7 	Output pipe                             */  
                                  /*                8 	I/O pipe                                */  
                                  /*                9 	Library                                 */  
                                  /*                10 	Disk file                               */  
                                  /*                11 	Null                                    */  
                                  /*                12 	Disk redir                              */  
                                  /*                13 	No map                                  */  
                                  /*                                                            */
    cit_uint8 idxname_length[2];  /*         2   Index Name Lenght                              */
                                  /*                                                            */
    cit_uint8 index_name[4];      /*         4   Index Name                                     */
                                  /*                                                            */
    cit_uint8 flller_3[2];        /*             Reserved.  Must contain binary zeros.          */
                                  /*                                                            */
    cit_uint8 flller_4[2];        /*  22     1   Reserved.  Must contain binary zeros.          */
                                  /*                                                            */
                                  /*  23     1   Reserved.  Must contain binary zeros.          */
                                  /*                                                            */
    cit_uint8 lock_mode;          /*  24     1   Lock mode flags for shareable files:           */
                                  /*                                                            */
                                  /*             Bit 7 = Lock on multiple records               */
                                  /*             Bit 6 = WRITELOCK directive enabled            */
                                  /*             Bit 5 = Reserved                               */
                                  /*             Bit 4 = SKIPLOCK directive enabled             */
                                  /*             Bit 3 = RETRYLOCK directive enabled            */
                                  /*             Bit 2 = Lock mode MANUAL                       */
                                  /*             Bit 1 = Lock mode AUTOMATIC                    */
                                  /*             Bit 0 = Lock mode EXCLUSIVE                    */
                                  /*                                                            */
    cit_uint8 other_flags;        /*  25     1   Other flags:                                   */
                                  /*                                                            */
                                  /*             Bit 7 = OPTIONAL file (OPEN INPUT)             */
                                  /*             Bit 6 = Reserved                               */
                                  /*             Bit 5 = Not Optional (OPEN I/O and EXTEND)     */
                                  /*             Bit 4 = File-name is EXTERNAL                  */
                                  /*             Bit 3 = Reserved                               */
                                  /*             Bit 2 = NODETECTLOCK directive enabled         */
                                  /*             Bit 1 = Multiple Reel file (Record             */
                                  /*             Sequential)                                    */
                                  /*             Bit 0 = Line Advancing file (Record            */
                                  /*             Sequential)                                    */
                                  /*                                                            */
    cit_uint8 flller_5[2];        /*  26     2   Reserved.  Must be set to binary zeros.        */
                                  /*                                                            */
    cit_uint8 handle_x[4];        /*  28     4   File handle.                                   */
                                  /*                                                            */
    cit_uint8 filler_6;           /*  32     1   Reserved.  Must be set to binary zeros.        */
                                  /*                                                            */
    cit_uint8 status_type;        /*  33     1   File status type.  Also see offsets 0 and      */
                                  /*             6:                                             */
                                  /*                                                            */
                                  /*             Bit 7 = set for ANSI'85 file status by         */
                                  /*             default, if not ANSI'74                        */
                                  /*             Bits 6-3 = Reserved                            */
                                  /*             Bit 2 = enables tab insertion for line         */
                                  /*             sequential files                               */
                                  /*             Bit 1 = enables null insertion for line        */
                                  /*             sequential files                               */
                                  /*             Bit 0 = Reserved                               */
                                  /*                                                            */
                                  /*             Bits 1 and 2 are set/unset at open time,       */
                                  /*             depending on the settings of the N and T       */
                                  /*             run-time switches.  These bits can be          */
                                  /*             set/unset after an OPEN operation to           */
                                  /*             override the N and T run-time switches.        */
                                  /*                                                            */
                                  /*             Bits 0-6 are reserved.                         */
                                  /*                                                            */
    cit_uint8 file_format;        /*  34     1   File format:                                   */
                                  /*                                                            */
                                  /*             0 - Default format                             */
                                  /*             1 - C-ISAM format                              */
                                  /*             2 - LEVEL II format                            */
                                  /*             3 - File format used by this COBOL system      */
                                  /*             4 - IDXFORMAT"4" format                        */
                                  /*             DOS and OS/2 only:                             */
                                  /*             5 - Btrieve format with ANSI emulation         */
                                  /*             6 - Btrieve format without ANSI emulation.     */
                                  /*                                                            */
    cit_uint8 filer_7[3];         /*  35     3   Reserved.  Must be set to binary zeros.        */
                                  /*                                                            */
    cit_uint8 max_rec_length[2];  /*  38     2   Maximum record length (in bytes).              */
                                  /*                                                            */
    cit_uint8 filer_8[2];         /*  40     3   Reserved.  Must be set to binary zeros.        */
    cit_uint8 mvs_flags;          /*  42     1   Mvs Flag                                       */
                                  /*                                                            */
                                  /*                                                            */
                                  /*                                                            */
    cit_uint8 rel_rec_number[4];  /*  43     4   Relative record number.                        */
                                  /*                                                            */
    cit_uint8 recording_mode;     /*  47     1   Recording mode:                                */
                                  /*                                                            */
                                  /*             Bits 7-1 = Reserved                            */
                                  /*             Bit 0 = 0 - Fixed                              */
                                  /*                     1 - Variable.                          */
                                  /*                                                            */
    cit_uint8 cur_rec_length[2];  /*  48     2   Current record length (in bytes).              */
                                  /*                                                            */
    cit_uint8 min_rec_length[2];  /*  50     2   Minimum record length (in bytes).              */
                                  /*                                                            */
    cit_uint8 key_id[2];          /*  52     2   Key identifier or Key-of-Reference             */
                                  /*             (indexed files).                               */
                                  /*                                                            */
                                  /*             This field identifies the                      */
                                  /*             Key-of-reference, used for random READ. To     */
                                  /*             specify the prime key, set this field to       */
                                  /*             zero.  To specify the first alternate key      */
                                  /*             defined, use a value 1; second alternate       */
                                  /*             key defined uses value 2, and so on.           */
                                  /*                                                            */
                                  /*             Or:                                            */
                                  /*                                                            */
                                  /*             Line count (Line Sequential files)             */
                                  /*                                                            */
                                  /*             This field specifies the number of lines       */
                                  /*             to skip when writing a file (for example,      */
                                  /*             WRITE AFTER ADVANCING line-count LINES).       */
                                  /*                                                            */
    cit_uint8 key_length[2];      /*  54     2   Effective key length.                          */
                                  /*                                                            */
                                  /*             When using START on indexed files, it is       */
                                  /*             possible for you to specify only the           */
                                  /*             leading part of a key instead of the whole     */
                                  /*             key.  You should set this field to the         */
                                  /*             number of bytes to be used in the START        */
                                  /*             comparison.  It must be greater than zero,     */
                                  /*             and no bigger than the key being used.         */
                                  /*                                                            */
    cit_uint8 rec_data_ptr[4];    /*  56     4   Pointer to the record area.  This and the      */
                                  /*             following pointers are USAGE POINTER           */
                                  /*             items, set using the COBOL syntax:             */
                                  /*                                                            */
                                  /*             set pointer to address of data-item            */
                                  /*                                                            */
                                  /*             where:                                         */
                                  /*                                                            */
                                  /*             pointer is one of these fields                 */
                                  /*                                                            */
                                  /*             data-item is the relevant data area.           */
                                  /*                                                            */
    cit_uint8 fname_ptr[4];       /*  60     4   Pointer to the file-name area.                 */
                                  /*                                                            */
    cit_uint8 key_def_block_ptr[4]; /*  64     4   Pointer to key definition block.               */
    /*                                                            */
    cit_uint8 sort_colating[4];   /*  68     4   Reserved.  Must be set to binary zeros.        */
                                  /*                                                            */
    cit_uint8 rel_byte_addr[4];   /*  72     4   Relative Byte Address                          */
                                  /*                                                            */
    cit_uint8 filler_10[2];       /*  76     2   Reserved.  Must be set to binary zeros.        */
                                  /*                                                            */
    cit_uint8 data_compression;   /*  78     1   Data compression routine indicator:            */
                                  /*                                                            */
                                  /*             0 = no compression                             */
                                  /*             1-127 = Micro Focus data compression           */
                                  /*             routine number (1=CBLDC001)                    */
                                  /*             128-255 = User defined data compression        */
                                  /*             routine number                                 */
                                  /*                                                            */
    cit_uint8 fs_session_id[4];   /*  79     4   Fileshare 2 session-id                         */
                                  /*                                                            */
    cit_uint8 file_share_fid[2];  /*  83     2   Fileshare 2 file-id                            */
                                  /*                                                            */
    cit_uint8 max_rel_key[4];
    cit_uint8 flags_1;            /*             Bit1 =  TRACE ON*/    
    cit_uint8 blocking;               
                                  /*                                                            */
    cit_uint8 interlang_lock;     /*  91     1   Bit 7 = Interlanguage locking (LOCKTYPE 1)     */
                                  /*             Bits 6-0 = Reserved                            */
                                  /*                                                            */
    cit_uint8 file_share_flag;    /*  92     1   Fileshare flags:                               */
                                  /*                                                            */
                                  /*             Bit 7 = Transaction logging                    */
                                  /*             Bits 6-0 = Reserved                            */
                                  /*                                                            */
    cit_uint8 config_flags;       /*  93     1   Configuration flags:                           */
                                  /*                                                            */
                                  /*             Bit 7 = WRITETHRU                              */
                                  /*             Bit 6 = Use Relative Byte Address              */
                                  /*             Bit 5 = Update current record pointer          */
                                  /*             Bits 4-1 = Reserved                            */
                                  /*             Bit 0 = Set if IGNORELOCK required             */
                                  /*                                                            */
    cit_uint8 filler_12;          /*  94     1   Reserved.  Must be set to binary zeros.        */
                                  /*                                                            */
    cit_uint8 adv_flags;          /*  95     1   Bit 7 = Use EBCDIC Collating Sequence          */
                                  /*             Bit 6 = Set if file is to have WRITE AFTER     */
                                  /*             ADVANCING                                      */
                                  /*             Bit 5 = Set if file is to have WRITE           */
                                  /*             BEFORE ADVANCING                               */
                                  /*             Bit 4 = ADV byte                               */
                                  /*             Bit 3 = Ignore minimum length checking on      */
                                  /*             variable length files                          */
                                  /*             Bits 2-0 = Reserved                            */
                                  /*                                                            */
    cit_uint8 filler_13[4];       /*  96     3   Reserved.  Must be set to binary zeros.        */
                                  /*                                                            */
} mf_extfh_FCD2;                                                                                 

typedef struct ctree_extfh_FCD_extention{
    cit_uint8   xdd_len[4];
#ifdef COB_PTR_64BITS
    cit_uint8   xdd_ptr[8];
#else
    cit_uint8   xdd_ptr[4];
#endif
    cit_uint8   dbname_len[4];
#ifdef COB_PTR_64BITS
    cit_uint8   dbname_ptr[8];
#else
    cit_uint8   dbname_ptr[4];
#endif
    cit_uint8   passwd_len[4];
#ifdef COB_PTR_64BITS
    cit_uint8   passwd_ptr[8];
#else
    cit_uint8   passwd_ptr[4];
#endif
} ctree_extfh_FCD_extention;

#ifdef COB_PTR_64BITS
#define mf_extfh_FCD mf_extfh_FCD3
#else
#define mf_extfh_FCD mf_extfh_FCD2
#endif

#ifdef _MSC_VER
    #ifndef PATH_MAX
        #define PATH_MAX			COB_SMALL_BUFF
    #endif
#endif

#define MAX_EXTSM_USING_GIVING 20
typedef struct {
    int                     using_cnt;
    int                     giving_cnt;
    mf_extfh_FCD            *file_def_block[MAX_EXTSM_USING_GIVING];
    void                    *fextfh_block[MAX_EXTSM_USING_GIVING];/*for sort in 3x uses*/
} mf_extsm_data_extfh;


#define ASSERT_F_EXTFH_PTR      if (!f || !f->extfh_ptr) { \
                                        cob_runtime_error (rtd, "EXTFH/EXTSM extfh_ptr not initialised file %s", (f ? f->select_name : "*undef*"));\
                                        cob_stop_abend(rtd, COBRE_EXTFH_PTR);\
                                        }\
                                    fcd = (mf_extfh_FCD*)(f->extfh_ptr);

#define CHECK_F_EXTFH_PTR       if (!f || !f->extfh_ptr) { return COB_NOT_CONFIGURED; } \
                                    fcd = (mf_extfh_FCD*)(f->extfh_ptr);

/* File connector */

struct cob_file_key {
    cob_field   *field; /* key field */
    int         flag;   /* WITH DUPLICATES (for RELATIVE/INDEXED) */
                        /* ASCENDING/DESCENDING (for SORT) */
    size_t      offset; /* Offset of field */
};

struct linage_struct {
    cob_field       *linage;        /* LINAGE */
    cob_field       *linage_ctr;    /* LINAGE-COUNTER */
    cob_field       *latfoot;       /* LINAGE FOOTING */
    cob_field       *lattop;        /* LINAGE AT TOP */
    cob_field       *latbot;        /* LINAGE AT BOTTOM */
    int             lin_lines;      /* Current Linage */
    int             lin_foot;       /* Current Footage */
    int             lin_top;        /* Current Top */
    int             lin_bot;        /* Current Bottom */
};

typedef struct  {
    const char      *select_name;           /* Name in SELECT */
    cob_field       *record_size;           /* record size depending on */
    void            *file;                  /* file specific data pointer */
    void            *linorkeyptr;           /* LINAGE pointer or SPLIT KEY */
    void            *extfh_ptr;             /* For EXTFH usage */
    char            flag_optional;          /* OPTIONAL for input*/
    char            last_open_mode;         /* open mode given by OPEN */
    char            flag_select_features;   /* SELECT features */
    char            flag_needs_top;         /* Linage needs top */
    char            file_version;           /* File I/O version */

/*CobolIT*/
    char            flag_no_close_cache;    /* No file cache for close*/
    int             (*extfh_func)(void *, void *);
    int             (*extsm_func)(void *, void *);
/*additional files info file_version =1 */
    long            oldummy[3];
    int             fileio_runtime_flags;   /*To be removed?*/
    char            ctree;                  /* use ctree EXTFH extention*/
    char            auto_close;             /* used for sysfile redirect*/
/* NO additional files info file_version =2 */
/*additional files info file_version =3 */
    char           *ctree_xdd;
    char           *extfh_name;
    char           *extsm_name;
    char           *pdummy[9];
    unsigned       file_trace:1;
    unsigned       dummy2:1;
    unsigned       optional_in_source:1;
    char           cdummy[7];
    int            env_fileio_runtime_flags;
    int            idummy[5];
    long long      ldummy[9];
    cob_field      *relative_rec_number;
    mf_extsm_data_extfh           *extsm_data;
    mf_extfh_FCD   FCD ;                 /*extfh pointer: FCD*/
    ctree_extfh_FCD_extention cTreeFCDExt;
} cob_file_extfh;

/*Declarations for CIT 3.x*/
/* Line Sequential file mode */
#define COB_LS_MODE_BINARY   0x01
#define COB_LS_MODE_MF       0x02
#define COB_LS_MODE_UTF16    0x04
#define COB_LS_MODE_DOS      0x08
#define COB_LS_MODE_EXPAND_TAB 0x10
#define COB_LS_MODE_PRINTER  0x20
#define COB_LS_MODE_GCOS     0x40
#define COB_LS_MODE_NOTRUNC  0x80
/* flag_rec_varying_size */

#define COB_VAR_REC_SIZE(f) (f->flag_rec_varying_size & 0x0f)
#define COB_VAR_REC_OPTION(f)  ((f->flag_rec_varying_size & 0xf0) )
typedef struct  {
    const char      *select_name;           /* Name in SELECT */
    unsigned char   *file_status;           /* FILE STATUS */
    cob_field       *assign;                /* ASSIGN TO */
    cob_field       *record;                /* record area */
    cob_field       *record_size;           /* record size depending on */
    struct cob_file_key *keys;              /* RELATIVE/RECORD/SORT keys */
    void            *file;                  /* file specific data pointer */
    void            *linorkeyptr;           /* LINAGE pointer or SPLIT KEY */
    const unsigned char *sort_collating;    /* SORT collating */
    void            *extfh_ptr;             /* For EXTFH usage */
    size_t          record_min;             /* record min size */
    size_t          record_max;             /* record max size */
    size_t          nkeys;                  /* the number of keys */
    char            organization;           /* ORGANIZATION */
    char            access_mode;            /* ACCESS MODE */
    char            lock_mode;              /* Current LOCKMODE */
    char            base_lock_mode;         /* Base LOCKMODE */
    char            open_mode;              /* OPEN MODE */
    char            flag_optional;          /* OPTIONAL for input*/
    char            last_open_mode;         /* open mode given by OPEN */
    char            special;                /* Special file */
    char            flag_nonexistent;       /* nonexistent file */
    char            flag_end_of_file;       /* reached the end of file */
    char            flag_begin_of_file;     /* reached beginning of file */
    char            flag_first_read;        /* first READ after OPEN/START */
    char            flag_read_done;         /* last READ successfully done */
    char            flag_select_features;   /* SELECT features */
    char            flag_needs_nl;          /* UNUSED  */
    char            flag_needs_top;         /* Linage needs top */
    char            file_version;           /* File I/O version */

/*CobolIT*/
    char            flag_no_close_cache;    /* No file cache for close*/
    int             (*extfh_func)(void *, void *);
    int             (*extsm_func)(void *, void *);
    int             line_sequential_mode;   
    int             flag_rec_varying_size;
/*additional files info file_version =1 */
    long            old_last_rw_pos;
    long            linecount;
    long            oldummy[3];
	int             ls_b_read_idx;
	int             ls_b_read_len;
    int             ls_b_write_idx;
    int             ls_next_char;
    int             fileio_runtime_flags;
    char            flag_with_rollback;
    char            data_compression_mode;
    char            ctree;                  /* use ctree EXTFH extention*/
    char            auto_close;             /* used for sysfile redirect*/
    char            ls_next_char_flag;
    char            flag_cr_emited;
/* NO additional files info file_version =2 */
/*additional files info file_version =3 */
    char           *ctree_xdd;
    char           *extfh_name;
	unsigned char  *unused_ls_buffer_read;
    unsigned char  *ls_buffer_write;    
    long long      last_rw_pos;
    char           *pdummy[10];
    unsigned char  default_status[4];
    char           flag_needs_cr;          /* Need a CR at close  */
    char           cob_do_sync;
    char           cob_ls_dos;
    char           cob_ls_nulls;
    unsigned       cob_ls_fixed:1;          /* Strip Space off*/
    unsigned       file_trace:1;
    unsigned       dummy2:1;
    unsigned       emulate_modulo_bug:1;
    unsigned       file_is_relative_mf:1;
    unsigned       cob_ls_var:1;
    unsigned       optional_in_source:1;
    unsigned       cob_ls_fillspace_off:1;
    char           flag_needs_eor;          /* Need a LF at close  */
    char           cdummy[6];
    char           ls_next_char_inszero;    /* MDC no (0), leading (-1) or trailing (+) zero */
    int            header_offset;
    int            env_fileio_runtime_flags;
    int            last_rw_size;
    int            ls_b_rw_cnt;
    int            nl_cnt;
    int            idummy[5];
    long long      ls_b_read_fpos;
    long long      ldummy[9];
} cob_file;

/* File I-O functions */

struct cob_fileio_funcs {
    int (*open)         (COB_RTD, cob_file_extfh *f, char *filename, const int mode,
                         const int sharing);
    int (*close)        (COB_RTD, cob_file_extfh *f, const int opt);
    int (*start)        (COB_RTD, cob_file_extfh *f, const int cond, cob_field *key);
    int (*read)         (COB_RTD, cob_file_extfh *f, cob_field *key, int read_opts);
    int (*read_next)    (COB_RTD, cob_file_extfh *f, int read_opts);
    int (*write)        (COB_RTD, cob_file_extfh *f, const int opt);
    int (*rewrite)      (COB_RTD, cob_file_extfh *f, const int opt);
    int (*fdelete)      (COB_RTD, cob_file_extfh *f);
};


COB_DLL_EXPIMP void cob_default_error_handle (COB_RTD);

COB_DLL_EXPIMP void cob_open_extfh    (COB_RTD, cob_file_extfh *f, const int mode, const int sharing,                        
                                 cob_field *fnstatus, int lock);
COB_DLL_EXPIMP void cob_close_extfh   (COB_RTD, cob_file_extfh *f, const int opt, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_read_extfh    (COB_RTD, cob_file_extfh *f, cob_field *key, cob_field *fnstatus,
                                 int read_opts);
COB_DLL_EXPIMP void cob_write_extfh   (COB_RTD, cob_file_extfh *f, cob_field *rec, const int opt,
                                 cob_field *fnstatus);
COB_DLL_EXPIMP void cob_rewrite_extfh (COB_RTD, cob_file_extfh *f, cob_field *rec, const int opt,
                                 cob_field *fnstatus);
COB_DLL_EXPIMP void cob_delete_extfh  (COB_RTD, cob_file_extfh *f, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_delete_file_extfh (COB_RTD, cob_file_extfh *f, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_start_extfh_relative (COB_RTD, cob_file_extfh *f, const int cond, cob_field *key, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_start_extfh_indexed(COB_RTD, cob_file_extfh *f, const int cond, const int id_key, const int len, cob_field *fnstatus);


COB_DLL_EXPIMP void cob_unlock_file_extfh (COB_RTD, cob_file_extfh *f, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_commit_extfh  (COB_RTD);
COB_DLL_EXPIMP void cob_rollback_extfh(COB_RTD);

/* System routines */
COB_DLL_EXPIMP int rtd_CBL_OPEN_FILE    (COB_RTD, unsigned char *file_name, unsigned char *file_access, 
                                         unsigned char *file_lock, unsigned char *file_dev,
                                         unsigned char *file_handle);
COB_DLL_EXPIMP int rtd_CBL_OPEN_VFILE   (COB_RTD, unsigned char *file_handle, unsigned char *status);
COB_DLL_EXPIMP int rtd_CBL_CREATE_FILE  (COB_RTD, unsigned char *file_name, unsigned char *file_access,
                                         unsigned char *file_lock, unsigned char *file_dev,
                                         unsigned char *file_handle);
COB_DLL_EXPIMP int rtd_CBL_CREATE_TMP_FILE  (COB_RTD, unsigned char *file_handle);
COB_DLL_EXPIMP int rtd_CBL_READ_FILE    (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                                         unsigned char *file_len, unsigned char *flags,
                                         unsigned char *buf);
COB_DLL_EXPIMP int rtd_CBL_WRITE_FILE   (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                                         unsigned char *file_len, unsigned char *flags,
                                         unsigned char *buf);
COB_DLL_EXPIMP int rtd_CBL_CLOSE_FILE   (COB_RTD, unsigned char *file_handle);
COB_DLL_EXPIMP int rtd_CBL_READ_VFILE   (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                                         unsigned char *file_len, unsigned char *buf);
COB_DLL_EXPIMP int rtd_CBL_WRITE_VFILE  (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                                         unsigned char *file_len, unsigned char *buf);
COB_DLL_EXPIMP int rtd_CBL_CLOSE_VFILE  (COB_RTD, unsigned char *file_handle);
COB_DLL_EXPIMP int rtd_CBL_FLUSH_FILE   (COB_RTD, unsigned char *file_handle);
COB_DLL_EXPIMP int rtd_CBL_DELETE_FILE  (COB_RTD, unsigned char *file_name);
COB_DLL_EXPIMP int rtd_CBL_COPY_FILE    (COB_RTD, unsigned char *fname1, unsigned char *fname2);
COB_DLL_EXPIMP int rtd_CBL_CHECK_FILE_EXIST (COB_RTD, unsigned char *file_name, unsigned char *file_info);
COB_DLL_EXPIMP int rtd_CBL_RENAME_FILE  (COB_RTD, unsigned char *fname1, unsigned char *fname2);
COB_DLL_EXPIMP int rtd_CBL_GET_CURRENT_DIR (COB_RTD, const int flags, const int dir_length,
                                            unsigned char *dir);

COB_DLL_EXPIMP int rtd_CBL_CHANGE_DIR   (COB_RTD, unsigned char *dir);
COB_DLL_EXPIMP int rtd_CBL_CREATE_DIR   (COB_RTD, unsigned char *dir);
COB_DLL_EXPIMP int rtd_CBL_DELETE_DIR   (COB_RTD, unsigned char *dir);
COB_DLL_EXPIMP int rtd_cob_acuw_chdir   (COB_RTD, unsigned char *dir, unsigned char *status);
COB_DLL_EXPIMP int rtd_cob_acuw_mkdir   (COB_RTD, unsigned char *dir);
COB_DLL_EXPIMP int rtd_cob_acuw_copyfile(COB_RTD, unsigned char *fname1, unsigned char *fname2,
                                         unsigned char *file_type);
COB_DLL_EXPIMP int rtd_cob_acuw_file_info (COB_RTD, unsigned char *file_name, unsigned char *file_info);
COB_DLL_EXPIMP int rtd_cob_acuw_file_delete (COB_RTD, unsigned char *file_name, unsigned char *file_type);

COB_DLL_EXPIMP int CBL_OPEN_FILE    (unsigned char *file_name, unsigned char *file_access, 
                                     unsigned char *file_lock, unsigned char *file_dev,
                                     unsigned char *file_handle);
COB_DLL_EXPIMP int CBL_OPEN_VFILE   (unsigned char *file_handle, unsigned char *status);
COB_DLL_EXPIMP int CBL_CREATE_FILE  (unsigned char *file_name, unsigned char *file_access,
                                     unsigned char *file_lock, unsigned char *file_dev,
                                     unsigned char *file_handle);
COB_DLL_EXPIMP int CBL_CREATE_TMP_FILE  (unsigned char *file_handle);
COB_DLL_EXPIMP int CBL_READ_FILE    (unsigned char *file_handle, unsigned char *file_offset,
                                     unsigned char *file_len, unsigned char *flags,
                                     unsigned char *buf);
COB_DLL_EXPIMP int CBL_WRITE_FILE   (unsigned char *file_handle, unsigned char *file_offset,
                                     unsigned char *file_len, unsigned char *flags,
                                     unsigned char *buf);
COB_DLL_EXPIMP int CBL_CLOSE_FILE   (unsigned char *file_handle);
COB_DLL_EXPIMP int CBL_READ_VFILE   (unsigned char *file_handle, unsigned char *file_offset,
                                     unsigned char *file_len, unsigned char *buf);
COB_DLL_EXPIMP int CBL_WRITE_VFILE  (unsigned char *file_handle, unsigned char *file_offset,
                                     unsigned char *file_len, unsigned char *buf);
COB_DLL_EXPIMP int CBL_CLOSE_VFILE  (unsigned char *file_handle);
COB_DLL_EXPIMP int CBL_FLUSH_FILE   (unsigned char *file_handle);
COB_DLL_EXPIMP int CBL_DELETE_FILE  (unsigned char *file_name);
COB_DLL_EXPIMP int CBL_COPY_FILE    (unsigned char *fname1, unsigned char *fname2);
COB_DLL_EXPIMP int CBL_CHECK_FILE_EXIST (unsigned char *file_name, unsigned char *file_info);
COB_DLL_EXPIMP int CBL_RENAME_FILE  (unsigned char *fname1, unsigned char *fname2);
COB_DLL_EXPIMP int CBL_GET_CURRENT_DIR (const int flags, const int dir_length,
                                        unsigned char *dir);
COB_DLL_EXPIMP int CBL_CHANGE_DIR   (unsigned char *dir);
COB_DLL_EXPIMP int CBL_CREATE_DIR   (unsigned char *dir);
COB_DLL_EXPIMP int CBL_DELETE_DIR   (unsigned char *dir);
COB_DLL_EXPIMP int cob_acuw_chdir   (unsigned char *dir, unsigned char *status);
COB_DLL_EXPIMP int cob_acuw_mkdir   (unsigned char *dir);
COB_DLL_EXPIMP int cob_acuw_copyfile(unsigned char *fname1, unsigned char *fname2,
                                     unsigned char *file_type);
COB_DLL_EXPIMP int cob_acuw_file_info (unsigned char *file_name, unsigned char *file_info);
COB_DLL_EXPIMP int cob_acuw_file_delete (unsigned char *file_name, unsigned char *file_type);

/* SORT 4.x*/
COB_DLL_EXPIMP void cobsort_init(COB_RTD, cob_file_extfh *f, void *sort_return, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_file_sort_close_extfh (COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void cob_file_sort_using_extfh (COB_RTD, cob_file_extfh *sort_file, cob_file_extfh *data_file);
COB_DLL_EXPIMP void cob_file_sort_giving_extfh (COB_RTD, cob_file_extfh *sort_file,
                                          const size_t varcnt, ...);
COB_DLL_EXPIMP void cob_file_sort_giving_table_extfh (COB_RTD, cob_file_extfh *sort_file, 
                                                const size_t varcnt, cob_file_extfh  **fbase);
COB_DLL_EXPIMP void cob_file_release_extfh (COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP void cob_file_release_1_extfh (COB_RTD, cob_file_extfh *f, cob_field *rec);
COB_DLL_EXPIMP void cob_file_return_extfh (COB_RTD, cob_file_extfh *f);
COB_DLL_EXPIMP int  cob_get_fileio_runtime_flags (COB_RTD, cob_file_extfh *f);

/*CobolIT*/
#define RETURN_STATUS(x)        do { cob_save_fstatus_extfh (rtd, f, x, fnstatus); return; } while (0)
#define RETURN_FCD_STATUS       {cob_save_fstatus_extfh (rtd, f, -1, fnstatus); return;}
COB_DLL_EXPIMP void cob_save_fstatus_extfh (COB_RTD, cob_file_extfh *f, int cit_status, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_cache_file (COB_RTD, cob_file_extfh *f, struct data_list **cachehead);
COB_DLL_EXPIMP void cob_uncache_file (COB_RTD, cob_file_extfh *f, struct data_list **cachehead);


#ifdef __GNUC__
extern void* extfh_cob_alloc_file (COB_RTD) __attribute__ ((malloc));/*3x functions*/
extern void extfh_cob_alloc_file_xdd (COB_RTD, void *, char *xdd);
extern void* extfh_cob_alloc_file_auto (COB_RTD, int indexed) __attribute__ ((malloc));/*3x functions*/
#else
COB_DLL_EXPIMP void* extfh_cob_alloc_file (COB_RTD);/*3x functions*/
COB_DLL_EXPIMP void* extfh_cob_alloc_file_auto (COB_RTD, int indexed);/*3x functions*/
COB_DLL_EXPIMP void extfh_cob_alloc_file_xdd (COB_RTD, void *, char *xdd);
#endif 

extern void  cob_enterprise_map_fstatus(COB_RTD, cob_file_extfh * f, unsigned char *dst_status ); 
/* for internal use only */

COB_DLL_EXPIMP void cob_file_set_fcd_file_name(COB_RTD, cob_file_extfh *, cob_field *f);
COB_DLL_EXPIMP void cob_define_key_component_fcd(void *, int nkeys, int id_comp, int offset, cob_field *component_key);
COB_DLL_EXPIMP void cob_define_key_def_fcd(void *, int id_key,  int comp_count, int comp_offset, unsigned char key_flags, unsigned char compression_flags);
COB_DLL_EXPIMP void cob_define_global_info_fcd(COB_RTD, void *p, int nkeys, cit_uint8 *key_def_block);
COB_DLL_EXPIMP void cob_define_key_sort (COB_RTD, cob_file_extfh *f, int comp_count);
COB_DLL_EXPIMP void cob_define_key_component_sort (COB_RTD, cob_file_extfh *f, int id_comp, int flag, cob_field *component_key, int offset);
COB_DLL_EXPIMP cob_field_attr * FCD_to_key_field_attr (COB_RTD, int FCD_field_type, int FCD_field_size);

/*3x functions*/
COB_DLL_EXPIMP void cob_update_extfh_fcd_from_file (COB_RTD, cob_file *f);
COB_DLL_EXPIMP void * cob_get_extfh_fcd_ptr_updated (COB_RTD, cob_file *f);
COB_DLL_EXPIMP int rtd_cob_update_file_from_fcd(COB_RTD, cob_file *f);
COB_DLL_EXPIMP void cob_update_file_from_extfh_fcd (COB_RTD, cob_file *f, cob_field *fnstatus);
/*3x sort extsm functions*/

COB_DLL_EXPIMP void extsm_sort_init     (COB_RTD, cob_file *f, int nkeys,
                                         const unsigned char *collating_sequence,
                                         void *sort_return, cob_field *fnstatus);
COB_DLL_EXPIMP void extsm_sort_init_key (COB_RTD, cob_file *f, int flag,
                                         cob_field *field, size_t offset);
COB_DLL_EXPIMP void extsm_sort_close    (COB_RTD, cob_file *f);
COB_DLL_EXPIMP void extsm_sort_using    (COB_RTD, cob_file *sort_file, cob_file *data_file);
COB_DLL_EXPIMP void extsm_sort_giving   (COB_RTD, cob_file *sort_file, size_t varcnt, ...);
COB_DLL_EXPIMP void extsm_release       (COB_RTD, cob_file *f);
COB_DLL_EXPIMP void extsm_release_1     (COB_RTD, cob_file *f, cob_field *rec);
COB_DLL_EXPIMP void extsm_return        (COB_RTD, cob_file *f);
COB_DLL_EXPIMP void extsm_process       (COB_RTD, cob_file *f);
COB_DLL_EXPIMP void extsm_end_output    (COB_RTD, cob_file *f);
COB_DLL_EXPIMP void extsm_end_input     (COB_RTD, cob_file *f);

/*3x SORT functions*/

COB_DLL_EXPIMP void cob_file_sort_init (COB_RTD, cob_file *f, const int nkeys,
                                        const unsigned char *collating_sequence,
                                        void *sort_return, cob_field *fnstatus);
COB_DLL_EXPIMP void cob_file_sort_init_key (COB_RTD, cob_file *f, const int flag,
                                            cob_field *field, size_t offset);
COB_DLL_EXPIMP void cob_file_sort_close (COB_RTD, cob_file *f);
COB_DLL_EXPIMP void cob_file_sort_using (COB_RTD, cob_file *sort_file, cob_file *data_file);
COB_DLL_EXPIMP void cob_file_sort_giving (COB_RTD, cob_file *sort_file,
                                          const size_t varcnt, ...);

COB_DLL_EXPIMP void cob_file_release_1 (COB_RTD, cob_file *f, cob_field *rec);
COB_DLL_EXPIMP void cob_file_return (COB_RTD, cob_file *f);
#endif /* COB_FILEIO_H */

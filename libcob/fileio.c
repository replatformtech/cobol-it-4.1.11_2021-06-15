/*
 * Copyright (C) 2002-2007 Keisuke Nishida
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
 * License along with this library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#ifdef  HAVE_SYS_TYPES_H
    #include <sys/types.h>
#endif
#ifdef  HAVE_UNISTD_H
    #include <unistd.h>
#endif
#include <sys/stat.h>

#ifdef _WIN32
    #define WINDOWS_LEAN_AND_MEAN
    #include <windows.h>                /* for GetTempPath, GetTempFileName */
    #include <direct.h>
    #define     fsync   _commit
    #define     getcwd  _getcwd
    #define     chdir   _chdir
    #define     mkdir   _mkdir
    #define     rmdir   _rmdir
#endif

#ifdef  HAVE_FCNTL_H
    #include <fcntl.h>
#endif

#include <signal.h>

#ifdef  WITH_CISAM
    #include <isam.h>
    #define IS_CHAR char
    #define LOCK_FAIL_MOVE_ANYWAY 0
#endif

#ifdef  WITH_DISAM
    #include <disam.h>
    #define LOCK_FAIL_MOVE_ANYWAY 1
#endif

#ifdef  WITH_VBISAM
    #include <vbisam.h>
    #define IS_CHAR VB_CHAR
    #define iserrno vb_rtd->iserrno
    #define isreclen vb_rtd->isreclen
    #define isrecnum vb_rtd->isrecnum
    #define LOCK_FAIL_MOVE_ANYWAY 1
extern int    ivbenter (const int ihandle, const int imodifying);
extern int    ivbexit (const int ihandle);


#endif

#ifdef  _MSC_VER   
    #include <io.h>
    #if _MSC_VER >= 1600 
        #define COB_FSEEK_stdio _fseeki64_nolock
        #define COB_FTELL_stdio _ftelli64_nolock
        #define COB_LSEEK _lseeki64
        #define COB_FREAD_stdio _fread_nolock
        #define COB_FWRITE_stdio _fwrite_nolock
    #else
        #define COB_FSEEK_stdio _fseeki64_nolock
        #define COB_FTELL_stdio _ftelli64
        #define COB_LSEEK _lseeki64
        #define COB_FREAD_stdio fread
        #define COB_FWRITE_stdio fwrite
    #endif
    #define  COB_O_BINARY _O_BINARY
    #ifndef COB_OFF_T 
        #define COB_OFF_T __int64
    #endif
#else
    #define COB_FSEEK_stdio fseeko
    #define COB_FTELL_stdio ftello
    #define COB_LSEEK lseek
    #define COB_FREAD_stdio fread
    #define COB_FWRITE_stdio fwrite
    #define COB_OFF_T long long
    #define  COB_O_BINARY 0
#endif

#include "common.h"
#include "coblocal.h"
#include "move.h"
#include "numeric.h"
#include "fileio.h"
#include "byteswap.h"
#include "zlib.h"
#include "mf_extfh.h"
#include "a2e.h"
/*
#include "lib/gettext.h"
*/
#define LS_BUFFER_SIZE 1024
#if !defined(__linux__)
    #define SEEK_INIT(f)        /*COB_FSEEK ((COB_FILE*)f->file, (COB_OFF_T)0, SEEK_CUR)*/
#else
    #define SEEK_INIT(f)
#endif

#ifndef O_BINARY
    #define     O_BINARY        0
#endif

#ifndef O_LARGEFILE
    #define     O_LARGEFILE     0
#endif

#ifdef _WIN32
    #define INITIAL_FLAGS       O_BINARY
#else
    #define INITIAL_FLAGS       0
#endif

/* HP UX need use of F_SETLK64*/
#if defined(F_SETLK64)
    #define VB_F_SETLK      F_SETLK64
    #define VB_F_GETLK      F_GETLK64
    #define VB_F_SETLKW     F_SETLKW64
    #define VB_flock        flock64
#else
    #define VB_F_SETLK      F_SETLK
    #define VB_F_GETLK      F_GETLK
    #define VB_F_SETLKW     F_SETLKW
    #define VB_flock        flock
#endif

#define COB_EOF -1
#define COB_ERROR -2
typedef struct COB_FILE {
    int fh;
    int status;
    FILE *stdfile;
    int         buffer_size;         
    unsigned char *buffer;
    int         used_bytes;
    int         is_modified;
    COB_OFF_T   filepos;
    COB_OFF_T   buffer_head_pos;
    COB_OFF_T   buffer_end_pos;
    COB_OFF_T   buffer_used_bytes_pos;
    FILE *stdfile_foropen;

} COB_FILE;

/* SORT definitions */

#define COBSORTEND        1
#define COBSORTABORT      2
#define COBSORTFILEERR    3
#define COBSORTNOTOPEN    4

#define HEADER_REC_VAR_SIZE 128
struct cobitem {
    struct cobitem      *next;
    size_t              end_of_block;
    unsigned char       block_byte;
    unsigned char       unique[sizeof(size_t)];
    unsigned char       actual_size[sizeof(size_t)];
    unsigned char       item[1];
};

struct memory_struct {
    struct cobitem  *first;
    struct cobitem  *last;
    size_t      count;
};

struct file_struct {
    COB_FILE       *fp;
    char        *filename;
    size_t      count;  /* count of items in temporary files */
};

struct cobsort {
    void            *pointer;
    struct cobitem  *empty;
    void            *sort_return;
    cob_field       *fnstatus;
    size_t          unique;
    size_t          retrieving;
    size_t          files_used;
    size_t          size;
    size_t          r_size;
    size_t          w_size;
    size_t          memory;
    int             destination_file;
    int             retrieval_queue;
    struct memory_struct    queue[4];
    struct file_struct  file[4];
};

/* End SORT definitions */


/*#define RETURN_STATUS(x)      do { save_status (f, x, fnstatus); return; } while (0)*/

static const int    status_exception[] = {
    0,                              /* 0x */
    COB_EC_I_O_AT_END,              /* 1x */
    COB_EC_I_O_INVALID_KEY,         /* 2x */
    COB_EC_I_O_PERMANENT_ERROR,     /* 3x */
    COB_EC_I_O_LOGIC_ERROR,         /* 4x */
    COB_EC_I_O_RECORD_OPERATION,    /* 5x */
    COB_EC_I_O_FILE_SHARING,        /* 6x */
    COB_EC_I_O,                     /* unused */
    COB_EC_I_O,                     /* unused */
    COB_EC_I_O_IMP                  /* 9x */
};

static int            no_indexed_dat_ext=0;
static const char   * const prefix[] = { "DD_", "dd_", ""};
#define NUM_PREFIX      sizeof(prefix) / sizeof(char *)

#ifdef  COB_PARAM_CHECK
static const char   parm_msg[] = "CALL to %s requires %d parameters";
#endif

static char * COB_NOINLINE cob_tmpfile_name (COB_RTD);

/* Isam File handler packet */

typedef struct comp_info {
    int recsize;
    char mode;
    char flag1:1;
    char flag2:1;
    char flag3:1;
    char flag4:1;
    char flag5:1;
    char flag6:1;
    char flag7:1;
    char flag8:1;
} comp_info_t;


#ifdef _MSC_VER
int
ftruncate (int fd, unsigned int size)
{
    HANDLE hfile;
    unsigned int curpos;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    hfile = (HANDLE) _get_osfhandle (fd);
    curpos = SetFilePointer (hfile, 0, NULL, FILE_CURRENT);
    if (curpos == ~0
        || SetFilePointer (hfile, size, NULL, FILE_BEGIN) == ~0
        || !SetEndOfFile (hfile)) {
        int error = GetLastError (); 
        switch (error) {
            case ERROR_INVALID_HANDLE:
                errno = EBADF;
                break;
            default:
                errno = EIO;
                break;
        }
        return -1;
    }
    return 0;
}
#endif

#ifdef _MSC_VER
    #define FMODE  (_S_IREAD | _S_IWRITE )
#else 
    #define FMODE 0666
#endif

static int cob_use_stdio = 0;
static int cob_use_citbuffer_size = 0;
static int cob_use_fopen = 0;
#define COB_BUFFSIZE (1024*8)

static int is_fifo (int fh)
{
    #ifdef S_ISFIFO
    struct stat st;
    if (!fstat(fh , &st) && S_ISFIFO(st.st_mode))
        return 1;
    #endif
    return 0;
}

static COB_FILE* COB_FOPEN (char * filename, const char* fmode)  {
    int n = 0;
    int fh = 0;
    int append = 0;
    int stdio = cob_use_stdio;
    COB_FILE* res = NULL;
    FILE *f;

    if (fmode[0] == 'f') {
        stdio = 1;
        fmode++;
    }


    if (fmode[0] == 'w' ) {
        if (fmode[1] == '+') {
            n = O_CREAT | O_RDWR | O_TRUNC;
        } else {
            n = O_CREAT | O_WRONLY | O_TRUNC;
        }
    } else if (fmode[0] == 'a' ) {
        if (fmode[1] == '+') {
            n = O_RDWR;
        } else {
            n = O_WRONLY;
        }
        append = 1;
    } else {
        if (fmode[1] == '+') {
            n= O_RDWR;
        }

    }
    if (stdio || cob_use_fopen) {
        f = fopen(filename, fmode);
        if (f) {
            res = calloc(1,sizeof(COB_FILE));
            if (stdio || is_fifo(fileno(f))) {
                res->stdfile = f;
            } else {
                res->stdfile_foropen = f;
                res->fh = fileno(f);
                res->buffer_size = cob_use_citbuffer_size;
                if (res->buffer_size) {
                    res->buffer = calloc(1,res->buffer_size);
                }
            }
        }
    } else {
        fh = open(filename, n | COB_O_BINARY , FMODE);
        if ((fh < 0) && (append)) {
            fh = open(filename, O_CREAT | O_RDWR | COB_O_BINARY , FMODE);
        }
        if (fh > 0 ) {
            res = calloc(1,sizeof(COB_FILE));
            /*memset (res, 0, sizeof(sizeof(COB_FILE)));*/
            if (stdio || is_fifo(fh)) {
                res->stdfile = fdopen(fh, fmode);
            } else {
                res->fh = fh;
                res->buffer_size = cob_use_citbuffer_size;
                if (res->buffer_size) {
                    res->buffer = calloc(1,res->buffer_size);
                }
            }
        }
    }
    return res;
}


static void inline COB_SET_used_bytes(COB_FILE *file, int bytes) {
    file->used_bytes = bytes;
    file->buffer_used_bytes_pos = file->buffer_head_pos + file->used_bytes - (COB_OFF_T)1;
}

static void inline COB_SET_buffer_head_pos (COB_FILE *file, COB_OFF_T pos) {
    file->buffer_head_pos = pos;
    file->buffer_end_pos = file->buffer_head_pos + (COB_OFF_T)(file->buffer_size -1);
    file->buffer_used_bytes_pos = file->buffer_head_pos + file->used_bytes - (COB_OFF_T)1;
}

static int COB_FFlushBuffer (COB_FILE *file) {
    int res; 
    COB_OFF_T resseek;
    if (file && file->fh && file->buffer_size) {
        if ( file->is_modified && (file->used_bytes > 0)) {
            file->status = 0;
            resseek = COB_LSEEK(file->fh, file->buffer_head_pos, SEEK_SET);        
            if (resseek < 0) {
                file->status = COB_ERROR;
                return -1;
            } else {
                res = write(file->fh, file->buffer, file->used_bytes);
                if (res <  0) {
                    file->status = COB_ERROR;
                    return -1;
                }
            }
        }
        file->is_modified = 0;
        COB_SET_used_bytes(file,0);
        COB_SET_buffer_head_pos(file,file->filepos);
    }
    return 0;
}

static int COB_FCLOSE (COB_FILE *file) {
    if (file) {
        if (COB_FFlushBuffer(file)) {
            return -1;
        }
        if (file->fh) {
            file->status = 0;
            close(file->fh);
        } else if (file->stdfile) {
            fclose(file->stdfile);
            file->stdfile_foropen = NULL;
        }
        if (file->stdfile_foropen) {
            fclose(file->stdfile_foropen);
        }
        if (file->buffer) {
            free(file->buffer);
        }
        free(file);
    }
    return 0;
}

static int COB_REWIND (COB_FILE * const file) {
    if (file) {
        if (file->fh) {
            if (COB_FFlushBuffer(file)) {
                return -1;
            }
            file->status = 0;
            COB_LSEEK(file->fh,0,SEEK_SET);
            file->filepos =  0;
        } else if (file->stdfile) {
            rewind(file->stdfile);
        }
    }
    return 0;
}

static int COB_FREAD_Buffer (void * ptr , int size , COB_FILE *const file) {
    int res = -1;
    COB_OFF_T r;
    if (size > file->buffer_size) {
        if (ptr) {
            if (COB_FFlushBuffer(file)) {
                return -1;
            }
            r = COB_LSEEK(file->fh, file->filepos, SEEK_SET);
            if (r >= 0) {
                res = read(file->fh, ptr, size);
                if (res > 0) {
                    file->filepos =  file->filepos + (COB_OFF_T)res;
                }
            }
        }
    } else {
        COB_OFF_T end_used_buff_pos = -1;
        COB_OFF_T end_read_pos = -1;

        if (file->filepos >= 0) {
            if (file->used_bytes) {
                end_used_buff_pos = file->buffer_used_bytes_pos;
                end_read_pos = file->filepos + (COB_OFF_T)(size - 1);

                if (end_used_buff_pos > 0 && (file->filepos >= file->buffer_head_pos) && (end_read_pos <= end_used_buff_pos)) {
                    int inbuf = file->filepos - file->buffer_head_pos;
                    if (ptr) {
                        memcpy(ptr, &(file->buffer[inbuf]), size);
                        file->filepos = file->filepos +size;
                    }
                    return size;
                }
            }
            if (COB_FFlushBuffer(file)) {
                return -1;
            }
            r = COB_LSEEK(file->fh, file->filepos, SEEK_SET);
            if (r >= 0) {
                res = read(file->fh, file->buffer, file->buffer_size);
                if (res > 0) {
                    COB_SET_buffer_head_pos(file,file->filepos);
                    COB_SET_used_bytes(file,res);

                    if (res > size) {
                        if (ptr) {
                            memcpy(ptr, file->buffer, size);
                            file->filepos = file->filepos + (COB_OFF_T)size;
                        }
                        res = size;
                    } else {
                        if (ptr) {
                            memcpy(ptr, file->buffer, res);
                            file->filepos = file->filepos + (COB_OFF_T)res;
                        }
                    }
                }
            }
        }
    }
    return res;
}

static int COB_FWRITE_Buffer (void * ptr , int size , COB_FILE *const file) {
    int res = -1;
    COB_OFF_T resseek;
    if (size > file->buffer_size) {
        if (COB_FFlushBuffer(file)) {
            return -1;
        }
        COB_SET_used_bytes(file,0);
        resseek = COB_LSEEK(file->fh, file->filepos, SEEK_SET);
        if (resseek >= 0) {
            res = write(file->fh, ptr, size);
            if (res > 0) {
                file->filepos =  file->filepos + (COB_OFF_T)res;
            }
        }
    } else {
        COB_OFF_T end_buff_pos = -1;
        COB_OFF_T end_buff_curpos = -1; 
        COB_OFF_T end_write_pos = -1;

        if (file->filepos >= 0) {
            if (file->used_bytes) {
                end_buff_curpos = file->buffer_used_bytes_pos ;
                end_buff_pos = file->buffer_head_pos + (COB_OFF_T)(file->buffer_size -1);
                end_write_pos = file->filepos + (COB_OFF_T)(size - 1);

                if (end_buff_curpos > 0 && (file->filepos >= file->buffer_head_pos) && (end_write_pos <= end_buff_pos)) {
                    int inbuf = file->filepos - file->buffer_head_pos;
                    memcpy(&(file->buffer[inbuf]),ptr, size);
                    file->is_modified = 1;
                    file->filepos = file->filepos +size;
                    if (end_write_pos > end_buff_curpos) {
                        COB_OFF_T p = end_write_pos - file->buffer_head_pos +1 ;
                        COB_SET_used_bytes(file, p );
                    }
                    return size;
                }
            }
            if (COB_FFlushBuffer(file)) {
                return -1;
            }
            resseek = COB_LSEEK(file->fh, file->filepos, SEEK_SET);
            if (resseek >= 0) {
                /*res = write(file->fh, ptr, size);*/
                memcpy(file->buffer,ptr, size);
                COB_SET_buffer_head_pos(file,file->filepos);
                COB_SET_used_bytes(file,size);
                file->is_modified = 1;
                file->filepos = file->filepos +(COB_OFF_T)size;
                res=size;
            }
        }
    }
    return res;
}

static int COB_FREAD (void * ptr , int size , int cnt , COB_FILE *const file) {
    int res = 0;
    int r;
    int s;
    if (file) {
        if (file->fh) {
            file->status = 0;
            s = size *cnt;
            if (file->buffer_size) {
                r = COB_FREAD_Buffer(ptr,s,file);
            } else {
                r = read(file->fh, ptr, s);
            }
            if (r > 0) {
                res = r / size;
            } else {
                if (r == 0) {
                    file->status = COB_EOF;
                } else {
                    file->status = COB_ERROR;
                }
            }
        } else if (file->stdfile) {
            res = COB_FREAD_stdio(ptr, size, cnt, file->stdfile);
        }
    }
    return res;
}

static int COB_FWRITE (void * ptr , int size , int cnt , COB_FILE *const file) {
    int res = 0;
    int r;
    int s;
    if (file) {
        if (file->fh) {
            file->status = 0;
            s = size *cnt;
            if (file->buffer_size) {
                r = COB_FWRITE_Buffer(ptr,s,file);
            } else {
                r = write(file->fh, ptr, s);
            }
            if (r > 0) {
                res = r / size;
            } else {
                file->status = COB_ERROR;
            }
        } else if (file->stdfile) {
            res = COB_FWRITE_stdio(ptr, size, cnt, file->stdfile);
        }
    }
    return res;

}

static int inline COB_PUTC (unsigned char c ,  COB_FILE *const file) {
    int res = 0;
    if (file) {
        if (file->fh) {
            file->status = 0;
            if (file->buffer_size) {
                if (file->used_bytes && (file->filepos == file->buffer_used_bytes_pos +1) &&
                                        (file->filepos >= file->buffer_head_pos) && 
                                        (file->filepos < file->buffer_end_pos) ) {
                    int inbuf = file->used_bytes;
                    file->buffer[inbuf] = c;
                    file->is_modified = 1;
                    file->filepos++;
                    COB_SET_used_bytes(file,file->used_bytes+1);
                    res = 1;
                } else 
                    res = COB_FWRITE_Buffer(&c, 1,file);
            } else {
                res = write(file->fh, &c, 1);
            } 
            if (res <= 0) {
                file->status = COB_ERROR;
            }
        } else if (file->stdfile) {
            res = putc(c, file->stdfile);
        }
    }
    return res;
}

static int inline COB_GETC (COB_FILE *const file) {
    int res = -1;
    unsigned char c;
    if (file) {
        if (file->fh) {
            file->status = 0;
            if (file->buffer_size) {
                if (file->used_bytes && (file->filepos >= file->buffer_head_pos) && 
                                        (file->filepos <= file->buffer_used_bytes_pos) ) {
                    int inbuf = file->filepos - file->buffer_head_pos;
                    c = file->buffer[inbuf];
                    file->filepos++;
                    res = 1;
                } else 
                    res = COB_FREAD_Buffer(&c, 1, file);
            } else {
                res = read(file->fh, &c, 1);
            }
            if (res <= 0) {
                if (res == 0) {
                    file->status = COB_EOF;
                } else {
                    file->status = COB_ERROR;
                }
                res = -1;
            } else
                res = c;
        } else if (file->stdfile) {
            res = getc(file->stdfile);
        }
    }
    return res;
}


/*
 * manage stat failure on linux
*/
int 
cob_stat (const char *name, struct stat     *st)
{
    int res = stat(name, st);
#if defined(HAVE_ERRNO_H) && defined(EOVERFLOW)
    if (errno == EOVERFLOW)
        res=0;
#endif
    return res;
}

/*
    Restore ISAM file positioning
*/

void
cob_cache_file (COB_RTD, cob_file_extfh *f, struct data_list **cachehead)
{
    if ( f->flag_no_close_cache )    /* No file cache for close*/
        return;
    cob_add_to_datalist(rtd,f,cachehead);
}

void
cob_uncache_file (COB_RTD, cob_file_extfh *f, struct data_list **cachehead)
{
    if ( f->flag_no_close_cache )    /* No file cache for close*/
        return;
    cob_remove_from_datalist(rtd,f,cachehead);
}

void
cob_save_fstatus_extfh (COB_RTD, cob_file_extfh *f, int cit_status, cob_field *fnstatus)
{
    mf_extfh_FCD *fcd;

    cit_uint8 status[2];
    if( !f  || !f->extfh_ptr ) 
        return;
    fcd = (mf_extfh_FCD *) f->extfh_ptr;

    if(cit_status == -1) {
        status[0] = fcd->user_file_status[0];
        status[1] = fcd->user_file_status[1];
    }
    else{
        status[0] = COB_I2D (cit_status / 10);
        status[1] = COB_I2D (cit_status % 10);
    }

    cob_enterprise_map_fstatus(rtd, f, status);


    if(!rtd->cob_exception_code) {
        rtd->cob_error_file = (void*)f;
    }
    if(likely( status[0] == CHAR_0 && status[1] == CHAR_0)) {
        rtd->cob_exception_code = 0;
    }
    else if(cit_status == -1 && likely(!(fcd->user_file_status[0] == CHAR_5 && fcd->user_file_status[1] == CHAR_2))) {
        int x = COB_D2I_CHAR(fcd->user_file_status[0]);
        if ( x > 9 ) {
            x = 9;
        }
        cob_set_exception (rtd, status_exception[x]);
    }
    else if(likely(cit_status != -1 && cit_status != COB_STATUS_52_EOP)) {
        int x = cit_status / 10;
        if ( x > 9 ) {
            x = 9;
        }
        cob_set_exception (rtd, status_exception[x]);
    }

    if ( fnstatus ) {
        fnstatus->data[0] = status[0];
        fnstatus->data[1] = status[1];
    }
    if ( f->file_trace ) {
        cob_runtime_error (rtd, "File %s operation status = %c%c", f->select_name, E2A(status[0]), E2A(status[1]));
    }
}

/*
 * Regular file
 */

size_t COB_NOINLINE
file_linage_check (COB_RTD, cob_file_extfh *f)
{
    struct linage_struct    *lingptr;

    lingptr = (struct linage_struct *)(f->linorkeyptr);
    lingptr->lin_lines = cob_get_int (rtd, lingptr->linage);
    if ( lingptr->lin_lines < 1 ) {
        goto linerr;
    }
    if ( lingptr->latfoot ) {
        lingptr->lin_foot = cob_get_int (rtd, lingptr->latfoot);
        if ( lingptr->lin_foot < 1 || lingptr->lin_foot > lingptr->lin_lines ) {
            goto linerr;
        }
    } else {
        lingptr->lin_foot = 0;
    }
    if ( lingptr->lattop ) {
        lingptr->lin_top = cob_get_int (rtd, lingptr->lattop);
        if ( lingptr->lin_top < 0 ) {
            goto linerr;
        }
    } else {
        lingptr->lin_top = 0;
    }
    if ( lingptr->latbot ) {
        lingptr->lin_bot = cob_get_int (rtd, lingptr->latbot);
        if ( lingptr->lin_bot < 0 ) {
            goto linerr;
        }
    } else {
        lingptr->lin_bot = 0;
    }
    return(0);
    linerr:
    cob_set_int (rtd, lingptr->linage_ctr, 0);
    return(1);
}

static COB_INLINE int 
cob_get_fileio_runtime_flags_1 (COB_RTD, cob_file_extfh *f)
{
    if ( f->file_version > 0 ) {
        return f->env_fileio_runtime_flags  | f->fileio_runtime_flags;
    } else {
        return rtd->fileio_runtime_flags ;
    }
}

int 
cob_get_fileio_runtime_flags (COB_RTD, cob_file_extfh *f)
{
    return cob_get_fileio_runtime_flags_1(rtd,f);
}


/*
 * Public interface
 */

void
cob_unlock_file_extfh (COB_RTD, cob_file_extfh *f, cob_field *fnstatus)
{
    mf_extfh_FCD* fcd;
	int ret;
    ASSERT_F_EXTFH_PTR
    ret = extfh_unlock (rtd, f);
    RETURN_FCD_STATUS;
}

static void
cob_init_env_flags (COB_RTD, cob_file_extfh *f) {
    f->file_trace = 0;
    if ( cob_bool_getenv ("COB_FILE_TRACE") ) {
        f->file_trace = 1;
    }
    if (rtd->trace_extfh & COB_TRACE_FILE) {
        f->file_trace = 1;
    }
    f->env_fileio_runtime_flags = rtd->fileio_runtime_flags;
}

void
cob_open_extfh (COB_RTD, cob_file_extfh *f, const int mode, const int sharing, cob_field *fnstatus, int lock)
{

    int         ret;

    mf_extfh_FCD *fcd;
    FLD_VAR
    ASSERT_F_EXTFH_PTR

    cob_init_env_flags(rtd, f);

    if ( f->file_trace ) {
        cob_runtime_error (rtd, "opening EXTFH file %s: FCD%s", f->select_name, GT_FLDP(fcd->fname_ptr));
    }
    f->last_open_mode = mode;
    ret = extfh_open (rtd, f, mode, sharing, lock);
    switch ( ret ) {
        case COB_STATUS_00_SUCCESS:
        case COB_STATUS_05_SUCCESS_OPTIONAL:
            cob_cache_file (rtd, f, &(rtd->file_cache));

            if ( unlikely(f->flag_select_features & COB_SELECT_LINAGE) ) {
                struct linage_struct    *lingptr;
                if ( file_linage_check (rtd, f) ) {
                    RETURN_STATUS(COB_LINAGE_INVALID);
                }
                lingptr = (struct linage_struct *)(f->linorkeyptr);
                f->flag_needs_top = cob_get_int (rtd, lingptr->lattop);
                cob_set_int (rtd, lingptr->linage_ctr, 1);
            }
            if ( f->file_trace ) {
                cob_runtime_error (rtd, "opening EXTFH file %s: FCD%s succesfull", f->select_name, GT_FLDP(fcd->fname_ptr));
            }
            break;
        default:
            if ( f->file_trace ) {
                cob_runtime_error (rtd, "opening EXTFH file %s: FCD%s status = %d", f->select_name, GT_FLDP(fcd->fname_ptr), ret);
            }
            break;
    }

    RETURN_FCD_STATUS;

}

void
cob_close_extfh (COB_RTD, cob_file_extfh *f, const int opt, cob_field *fnstatus)
{
    int ret;
    mf_extfh_FCD *fcd;
    register int open_mode;
    ASSERT_F_EXTFH_PTR;
    open_mode = GT_FLD1(fcd->open_mode);
    if ( open_mode == FCD_OPEN_CLOSED ) {
        if ( f->file_trace ) {
            cob_runtime_error (rtd, "closing %s : file yet closed", f->select_name);
        }
        RETURN_STATUS (COB_STATUS_42_NOT_OPEN);
    }
    cob_uncache_file(rtd, f, &(rtd->file_cache));
    ret = extfh_close(rtd, f, opt);
    RETURN_FCD_STATUS;
}

void
cob_start_extfh_indexed (COB_RTD, cob_file_extfh *f, const int cond, const int id_key, const int len, cob_field *fnstatus)
{
    int ret;
    mf_extfh_FCD *fcd;
    ASSERT_F_EXTFH_PTR;
    ret = extfh_start_indexed(rtd, f, cond, id_key, len);

    RETURN_FCD_STATUS;
}

void
cob_start_extfh_relative (COB_RTD, cob_file_extfh *f, const int cond, cob_field *key, cob_field *fnstatus)
{
    int ret;
    mf_extfh_FCD *fcd;
    ASSERT_F_EXTFH_PTR;
    ret = extfh_start_relative(rtd, f, cond, key);

    RETURN_FCD_STATUS;
}

void
cob_read_extfh (COB_RTD, cob_file_extfh *f, cob_field *key, cob_field *fnstatus, int read_opts)
{
    int ret;
    mf_extfh_FCD *fcd;
    register int organization;
    FLD_VAR
    ASSERT_F_EXTFH_PTR
    ret = extfh_read(rtd, f, key, read_opts);
    organization = GT_FLD1(fcd->organization);
    /*update key number*/
    if ( !key && ( organization == FCD_ORG_RELATIVE ) ) {
        if( f->relative_rec_number ) {
            register int relnum = GT_FLD4_8(fcd->rel_rec_number);
            if ( fcd->user_file_status[0] == CHAR_0 ) {
                cob_set_int (rtd, f->relative_rec_number, 0);
                if( cob_add_int(rtd, f->relative_rec_number, relnum) != 0) {
                    RETURN_STATUS(COB_STATUS_14_OUT_OF_KEY_RANGE);
                }
            } else {
                cob_set_int (rtd, f->relative_rec_number, relnum);
            }
        }
    }
    RETURN_FCD_STATUS;
}

static int 
cob_compute_linage_lines(COB_RTD, cob_file_extfh *f,
                       struct linage_struct    *lingptr, int cnt, int flag_eop)
{
    int         i, n;
    int         linetoskip = 0;

    n = cob_get_int (rtd, lingptr->linage_ctr);
    if ( n == 0 ) {
        return(-1);
    }
    cob_add_int (rtd, lingptr->linage_ctr, cnt );
    i = cob_get_int (rtd, lingptr->linage_ctr);
    if ( flag_eop && lingptr->lin_foot ) {
        if ( i >= lingptr->lin_foot ) {
            rtd->eop_status = 1;
        }
    }
    if ( i > lingptr->lin_lines ) {
        if ( flag_eop ) {
            rtd->eop_status = 1;
        }
        for ( ; n <= lingptr->lin_lines; n++ ) {
            linetoskip++;
        }
        linetoskip+= lingptr->lin_bot;
        if ( file_linage_check (rtd, f) ) {
            return(-1);
        }
        linetoskip+=lingptr->lin_top;
        cob_set_int (rtd, lingptr->linage_ctr, 1);
    } else {
        linetoskip = cnt;
    }
    return linetoskip;
}

static int COB_NOINLINE
cob_compute_linage (COB_RTD, cob_file_extfh *f, const int opt)
{
    struct linage_struct    *lingptr;
    int         i, n;
    int         linetoskip = 0;
    int         lines = 0;

    lingptr = (struct linage_struct *)(f->linorkeyptr);
    if (unlikely(opt & COB_WRITE_PAGE)) {
        i = cob_get_int (rtd, lingptr->linage_ctr);
        if ( i == 0 ) {
            return(-1);
        }
        n = lingptr->lin_lines;
        for ( ; i <= n; i++ ) {
            lines++;
        }
        lines+=lingptr->lin_bot;
        if ( file_linage_check (rtd, f) ) {
            return(-1);
        }
        lines+=lingptr->lin_top;
        cob_set_int (rtd, lingptr->linage_ctr, 1);
    } else if (opt & COB_WRITE_LINES) {
        lines += cob_compute_linage_lines(rtd, f, lingptr, (opt & COB_WRITE_MASK), (opt & COB_WRITE_EOP));
    } else if ( opt & COB_WRITE_CHANNEL ) {
        i = opt & COB_WRITE_MASK;
        if ( i < 0 || i>= 16 ) {
            i = 0;
        }
        if ( i == 0 ) {
            lines += cob_compute_linage(rtd,f,  COB_WRITE_PAGE | 1);
        } else {
            n = rtd->channel_table[i];
            i = cob_get_int (rtd, lingptr->linage_ctr);
            if ( i >= n ) {
                lines+=cob_compute_linage(rtd,f,  COB_WRITE_PAGE | 1);
                if (linetoskip < 0) {
                    return (linetoskip);
                }
                i = 0;
            }
            if ( i < n ) {
                lines += cob_compute_linage_lines(rtd, f, lingptr, n-i, (opt & COB_WRITE_EOP));
            }
        }
    }
    linetoskip += lines;
    return (linetoskip);
}

void
cob_write_extfh (COB_RTD, cob_file_extfh *f, cob_field *rec, const int opt, cob_field *fnstatus)
{
    int ret;
    int actual_opt = opt;
    mf_extfh_FCD *fcd;
    register int size, maxs, mins;
    FLD_VAR;
    ASSERT_F_EXTFH_PTR;

    maxs = GT_FLD2_4(fcd->max_rec_length);
    mins = GT_FLD2_4(fcd->min_rec_length);

    /*meaning if record_size > maxs, there's no error*/
    if ( FCD_VAR_REC_MODE(fcd) && f->record_size ) {
        size = min(maxs, cob_get_int (rtd, f->record_size));
    } else {
        size = rec->size;
    }
    if( (FCD_VAR_REC_MODE(fcd) && (size < mins)) || maxs < size) {
        RETURN_STATUS (COB_STATUS_44_RECORD_OVERFLOW);
    }

    size = max (mins, size);
    CP_FLD2_4(fcd->cur_rec_length, size);

    if ( unlikely(f->flag_select_features & COB_SELECT_LINAGE) ) {
        int lines = 0;
        void *saverec = NULL;

        if (f->flag_needs_top) {
            unsigned char *data;

            lines = f->flag_needs_top;
            f->flag_needs_top=0;
            saverec = GT_FLDP(fcd->rec_data_ptr);
            data = cob_malloc(rtd,  GT_FLD2_4(fcd->cur_rec_length));
            memset (data, CHAR_SP, GT_FLD2_4(fcd->cur_rec_length));
            CP_FLDP(fcd->rec_data_ptr, data);
            actual_opt = COB_WRITE_LINES | COB_WRITE_AFTER | lines ;
            ret = extfh_write(rtd, f, actual_opt);
            cob_free(data);
            CP_FLDP(fcd->rec_data_ptr, saverec);
        }
        actual_opt = opt;
        if ((opt & ~COB_WRITE_EOP )== 0 ) {
            actual_opt = opt | COB_WRITE_LINES | COB_WRITE_AFTER | 1;
        } 
        lines = cob_compute_linage(rtd, f, actual_opt);
        if (lines < 0) {
             RETURN_STATUS (COB_STATUS_57_I_O_LINAGE); 
        }
        actual_opt = COB_WRITE_LINES | ( actual_opt & COB_WRITE_AFTER) | ( actual_opt & COB_WRITE_BEFORE) |
            (opt & COB_WRITE_EOP) | lines ; 
    }

    ret = extfh_write(rtd, f, actual_opt);

    if ((ret == 0) &&  unlikely(rtd->eop_status) ) {
        rtd->eop_status = 0;
        rtd->cob_exception_code = 0x0502;
        RETURN_STATUS(COB_STATUS_52_EOP);
    }
    RETURN_FCD_STATUS;
}

void
cob_rewrite_extfh (COB_RTD, cob_file_extfh *f, cob_field *rec, const int opt, cob_field *fnstatus)
{
    int ret;
    mf_extfh_FCD *fcd;
    register int organization;

    FLD_VAR;
    ASSERT_F_EXTFH_PTR;

    organization = GT_FLD1(fcd->organization);

    if ( organization == FCD_ORG_SEQUENTIAL ) {
        register int cur_rec_length = GT_FLD2_4(fcd->cur_rec_length);
        if (FCD_VAR_REC_MODE(fcd) && f->record_size) {
            if ( cur_rec_length != (size_t)cob_get_int (rtd, f->record_size) ) {
                RETURN_STATUS (COB_STATUS_44_RECORD_OVERFLOW);
            }
        } else if ( cur_rec_length != rec->size ) {
            RETURN_STATUS (COB_STATUS_44_RECORD_OVERFLOW);
        }
    }


    /*CIT*/
    if ( f->record_size ) {
        CP_FLD2_4(fcd->cur_rec_length, cob_get_int (rtd, f->record_size));
    } else {
        if ((FCD_VAR_REC_MODE(fcd) || organization == FCD_ORG_SEQUENTIAL) && rec) {
            CP_FLD2_4(fcd->cur_rec_length, rec->size);

        } 

    }


    ret = extfh_rewrite (rtd, f, opt);

    RETURN_FCD_STATUS;
}

void
cob_delete_extfh (COB_RTD, cob_file_extfh *f, cob_field *fnstatus)
{
    int ret;
    mf_extfh_FCD *fcd;
    ASSERT_F_EXTFH_PTR
    ret = extfh_delete(rtd, f);

    RETURN_FCD_STATUS;
}

void
cob_delete_file_extfh (COB_RTD, cob_file_extfh *f, cob_field *fnstatus)
{
    int ret;
    mf_extfh_FCD *fcd;
    ASSERT_F_EXTFH_PTR;
    ret = extfh_delete_file (rtd, f);
    RETURN_FCD_STATUS;
}

void
cob_commit_extfh (COB_RTD) {
    struct data_list    *l;
    void* extfhdrv = NULL;

    for ( l = rtd->file_cache; l; l = l->next ) {
        cob_file_extfh *f = (cob_file_extfh*)l->data;
        mf_extfh_FCD *fcd;
        ASSERT_F_EXTFH_PTR;
        if ( f->extfh_func ) {
            if ( (extfhdrv != f->extfh_func) ) {
              extfh_commit(rtd, f);
              extfhdrv = f->extfh_func;
            }
        }
    }
}

static void
cob_do_rollback (COB_RTD, struct data_list *file_cache)
{
    struct data_list    *l;

    void* extfhdrv = NULL;
    for ( l = file_cache; l; l = l->next ) {
        mf_extfh_FCD *fcd;
        cob_file_extfh *f = (cob_file_extfh*)l->data;
        ASSERT_F_EXTFH_PTR;
        if ( f->extfh_func ) {
            if ( (extfhdrv != f->extfh_func) ) {
                  extfh_rollback(rtd, f);
                  extfhdrv = f->extfh_func;
            }
        }

    }
}

void
cob_rollback_extfh (COB_RTD)
{
    cob_do_rollback(rtd,rtd->file_cache);
}

static void cob_copy_fcd_fname_to_string(COB_RTD, mf_extfh_FCD *fcd, char *s, int size){
    FLD_VAR
    unsigned char * fname = (unsigned char *)GT_FLDP(fcd->fname_ptr);
    if(fname) {
        memcpy (s, fname, size);
        cob_string_to_C(rtd, s, size);
    }
    else
        *s=0;
}

void
cob_default_error_handle (COB_RTD)
{
    const char      *msg = NULL;

    char            *filename;
    int             status;
    mf_extfh_FCD   *fcd;
    cob_file_extfh        *f = (cob_file_extfh*) rtd->cob_error_file;
    FLD_VAR;
    ASSERT_F_EXTFH_PTR;

    status = extfh_get_int_status(rtd, f);
    switch ( status ) {
        case COB_STATUS_10_END_OF_FILE:
            msg = "End of file";
            break;
        case COB_STATUS_14_OUT_OF_KEY_RANGE:
            msg = "Key out of range";
            break;
        case COB_STATUS_21_KEY_INVALID:
            msg = "Key order not ascending";
            break;
        case COB_STATUS_22_KEY_EXISTS:
            msg = "Record key already exists";
            break;
        case COB_STATUS_23_KEY_NOT_EXISTS:
            msg = "Record key does not exist";
            break;
        case COB_STATUS_30_PERMANENT_ERROR:
            msg = "Permanent file error";
            break;
        case COB_STATUS_35_NOT_EXISTS:
            msg = "File does not exist";
            break;
        case COB_STATUS_37_PERMISSION_DENIED:
            msg = "Permission denied";
            break;
        case COB_STATUS_41_ALREADY_OPEN:
            msg = "File already open";
            break;
        case COB_STATUS_42_NOT_OPEN:
            msg = "File not open";
            break;
        case COB_STATUS_43_READ_NOT_DONE:
            msg = "READ must be executed first";
            break;
        case COB_STATUS_44_RECORD_OVERFLOW:
            msg = "Record overflow";
            break;
        case COB_STATUS_46_READ_ERROR:
            msg = "Failed to read";
            break;
        case COB_STATUS_47_INPUT_DENIED:
            msg = "READ/START not allowed";
            break;
        case COB_STATUS_48_OUTPUT_DENIED:
            msg = "WRITE not allowed";
            break;
        case COB_STATUS_49_I_O_DENIED:
            msg = "DELETE/REWRITE not allowed";
            break;
        case COB_STATUS_51_RECORD_LOCKED:
            msg = "Record locked by another file connector";
            break;
        case COB_STATUS_52_EOP:
            msg = "A page overflow condition occurred";
            break;
        case COB_STATUS_57_I_O_LINAGE:
            msg = "LINAGE values invalid";
            break;
        case COB_STATUS_61_FILE_SHARING:
            msg = "File sharing conflict";
            break;
        case COB_STATUS_91_NOT_AVAILABLE:
            msg = "Runtime library is not configured for this operation";
            break;
        default:
            msg = "Unknown file error";
            break;
    }

    filename = cob_malloc (rtd, COB_MEDIUM_BUFF);

    cob_copy_fcd_fname_to_string(rtd, fcd, filename, GT_FLD2(fcd->fname_length));
    cob_runtime_error (rtd, "%s (STATUS = %02d) COB_FILE: '%s'", msg,
                       status, STRING_E2A((unsigned char*)filename, strlen(filename)));
    cob_free (filename);
}

static void
cob_decode_lp_form(COB_RTD, char *s)
{
    char *p = strdup(s);
    char *q;
    int i = 1;
    int v;

    q = strtok(p, ":");
    while ( q && i < 16 ) {
        v = atoi(q);
        if ( v > 0 ) {
            rtd->channel_table[i] =v-1;
        } else {
            rtd->channel_table[i] = 0;
        }
        q = strtok(NULL, ":");
        i++;
    }
    free(p);
}


void
cob_init_fileio (COB_RTD)
{
    char *s;
    int n;


    rtd->cob_sort_memory = 64*1024*1024;
    if ( (s = getenv ("COB_SORT_MEMORY")) != NULL ) {
        n = atoi (s);
        if ( n >= 1024*1024 ) {
            rtd->cob_sort_memory = n;
        }
    }
    /* rtd->ls_enable_double_lf = getenv ("COB_LS_DOUBLE_LF"); */

    rtd->file_open_name = cob_malloc(rtd, COB_MEDIUM_BUFF);
    rtd->file_open_buff = cob_malloc (rtd, COB_MEDIUM_BUFF);

    if ( (s = getenv ("COB_TRACE_EXTFH")) != NULL ) {
        rtd->trace_extfh = atoi(s);
    }

    s = cob_bool_getenv ("COB_NO_DOT_DAT");
    if ( s && (*s == 'Y' || *s == 'y' || *s == '1') ) {
        no_indexed_dat_ext = 1;
        #if defined(WITH_VBISAM) 
        set_fname_extention((char*)"", (char*)".idx");
        #endif
    }

    rtd->warning_disable = 1;
    s = cob_bool_getenv ("COB_CLOSE_WARNING");
    if ( s && (*s == 'Y' || *s == 'y' || *s == '1') ) {
        rtd->warning_disable = 0;
    }
    s = getenv ("COB_LP_FORM");
    if ( !s ) {
        s = getenv ("COBLPFORM");
    }
    if ( s ) {
        cob_decode_lp_form(rtd,s);
    }
#if !(defined(_MSC_VER) || defined(__sun)) 
    s = getenv ("COB_USE_STDIO");
    if ( s && (*s == 'Y' || *s == 'y' || *s == '1') ) {
        cob_use_stdio = 1;    
    } else if (s)
#endif
    {
        cob_use_stdio = 0;
    }
#if defined(__sun)
    cob_use_fopen =1;
#endif
    s = getenv ("COB_USE_FOPEN");
    if ( s && (*s == 'Y' || *s == 'y' || *s == '1') ) {
        cob_use_fopen = 1;    
    }
    if ( s && (*s == 'N' || *s == 'n' || *s == '0') ) {
        cob_use_fopen = 0;    
    }
    s = cob_bool_getenv ("COB_FORCE_STDIO");
    if ( s && (*s == 'Y' || *s == 'y' || *s == '1') ) {
        cob_use_stdio = 1;    
    }
    s = getenv ("COB_FILEIO_BUFFER");
    if ( s ) {
        if (*s == 'Y' || *s == 'y' )
            cob_use_citbuffer_size = COB_BUFFSIZE;
        else if (*s == 'N' || *s == 'n' )
            cob_use_citbuffer_size = 0;
        else
            cob_use_citbuffer_size = atoi(s);
    } else {
        cob_use_citbuffer_size = COB_BUFFSIZE;
    }

    extfh_cob_init_fileio (rtd);

}

void
cob_clear_fileio (COB_RTD)
{

    cob_free(rtd->file_open_name);
    cob_free(rtd->file_open_buff);

}

void
cob_exit_fileio (COB_RTD, int panic)
{
    struct data_list    *l;
    struct data_list    *next;
    /*struct data_list    *base;*/
    struct data_list    *cpt;
    void   *errhdlrs;
    register int open_mode;
    FLD_VAR;
    /* base = rtd->file_cache;*/
    /* rtd->file_cache = NULL;*/
    for ( l= rtd->file_cache; l; l = next ) {
        cob_file_extfh *f;
		mf_extfh_FCD *fcd;
		f = l->data;
		ASSERT_F_EXTFH_PTR;
		next = l->next;
        if ( f && 
             (f->file_version > 0) &&
              (ISB_FLD1(fcd->FCD_FILE_SHARE_FLAG_FLAG_WITH_ROLLBACK)) ) {
            open_mode = GT_FLD1(fcd->open_mode);
            cob_do_rollback(rtd,rtd->file_cache);
            if ( (open_mode != COB_OPEN_CLOSED ) &&
                 (open_mode != COB_OPEN_LOCKED ) ) {
                int sz = min(GT_FLD2(fcd->fname_length), sizeof (rtd->runtime_buffer))-1;
                cob_copy_fcd_fname_to_string(rtd, fcd, rtd->runtime_buffer, sz);
                if ( !rtd->warning_disable ) {
                    errhdlrs = rtd->hdlrs;
                    rtd->hdlrs = NULL;
                    cob_runtime_error (rtd, "WARNING - Implicit CLOSE of %s (\"%s\") declared WITH ROLLBACK... ROLLBACK preformed",
                                       ((cob_file_extfh*)(l->data))->select_name, rtd->runtime_buffer);
                    rtd->hdlrs=errhdlrs;
                }
                if ( !(cob_get_fileio_runtime_flags_1(rtd, (cob_file_extfh*)(l->data)) & FILEIO_NO_CRASH_CLOSE) ) {
                    cob_close_extfh (rtd, (cob_file_extfh*)(l->data), 0, NULL);
                }
            }
            break;
        }
    }
    for ( l= rtd->file_cache; l; l=next ) {
		cob_file_extfh *f;
		mf_extfh_FCD *fcd;
		f = l->data;
		ASSERT_F_EXTFH_PTR
		next = l->next;
        open_mode = GT_FLD1(fcd->open_mode);
        if ( f &&
             (open_mode != COB_OPEN_CLOSED ) &&
             (open_mode != COB_OPEN_LOCKED ) ) {
            int sz = min(GT_FLD2(fcd->fname_length), sizeof (rtd->runtime_buffer))-1;
            cob_copy_fcd_fname_to_string(rtd, fcd, rtd->runtime_buffer, sz);
            if ( !rtd->warning_disable ) {
                if ( !((cob_file_extfh*)(l->data))->auto_close ) {
                    errhdlrs = rtd->hdlrs;
                    rtd->hdlrs = NULL;
                    cob_runtime_error (rtd, "WARNING - Implicit CLOSE of %s (\"%s\")",
                                       f->select_name, rtd->runtime_buffer);
                    rtd->hdlrs=errhdlrs;
                }
            }
            if ( !(cob_get_fileio_runtime_flags_1(rtd, (cob_file_extfh*)(l->data)) & FILEIO_NO_CRASH_CLOSE) ) {
                cob_close_extfh (rtd, (cob_file_extfh*)(l->data), 0, NULL);
            }

        }
    }
    extfh_cob_exit_fileio (rtd); 
    cpt = NULL;
    for ( l= rtd->file_cache; l; l = cpt ) {
        cpt = l->next;
        cob_free(l);
    }
    rtd->file_cache = NULL;
}

/* System routines */

static void * COB_NOINLINE
cob_str_from_filename_fld (COB_RTD, const cob_field *f)
{
    void        *mptr;
    unsigned char   *s;
    int     i;
    int     n;
    int     sz; 
    int     quote_switch;

    if ( !f ) {
        return(cob_malloc (rtd, 1));
    }
    sz = min(f->size, (COB_SMALL_BUFF-1));
    mptr = cob_malloc (rtd, (size_t)(COB_SMALL_BUFF+1));
    quote_switch = 0;
    s = (unsigned char   *)rtd->file_open_buff ;

    i = -1;
    for ( n = 0; n < sz; n++ ) {
        if ( f->data[n] == CHAR_DQUOTE ) {
            quote_switch = !quote_switch;
            continue;
        }
        i++;
        s[i] = f->data[n];
        if ( quote_switch ) {
            continue;
        }
        if ( s[i] == CHAR_SP || s[i] == 0 ) {
            s[i] = 0;
            break;
        }
    }
    s[i+1]=0;
    /* expand envoronment variables */
    /* ex. "$TMPDIR/foo" -> "/tmp/foo" */

    cobc_filename_replace_env((char*)STRING_E2A(s, strlen((char*)s)), mptr, COB_SMALL_BUFF, 0);
    return(mptr);
}

static int COB_NOINLINE
open_cbl_file (COB_RTD, unsigned char *file_name, unsigned char *file_access,
               unsigned char *file_handle, const int file_flags)
{
    char    *fn;
#ifdef  O_BINARY
    int flag = O_BINARY;
#else
    int flag = 0;
#endif
    int fd;

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        memset (file_handle, -1, 4);
        return(-1);
    }
    flag |= file_flags;
    switch ( *file_access & 0x3f ) {
        case 1:
            flag |= O_RDONLY;
            break;
        case 0:
        case 2:
            flag |= O_CREAT | O_TRUNC | O_WRONLY;
            break;
        case 3:
            flag |= O_RDWR;
            break;
        default:
            memset (file_handle, -1, 4);
            return(-1);
    }
    fn = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    fd = open (fn, flag | COB_O_BINARY, 0660);
    if ( fd < 0 ) {
        cob_free (fn);
        memset (file_handle, -1, 4);
        return(35);
    }
    cob_free (fn);
    memcpy (file_handle, &fd, 4);
    return(0);
}

int
CBL_OPEN_FILE (unsigned char *file_name, unsigned char *file_access,
               unsigned char *file_lock, unsigned char *file_dev,
               unsigned char *file_handle)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_OPEN_FILE (rtd, file_name, file_access,
                              file_lock, file_dev,
                              file_handle);
}

int
rtd_CBL_OPEN_FILE (COB_RTD, unsigned char *file_name, unsigned char *file_access,
                   unsigned char *file_lock, unsigned char *file_dev,
                   unsigned char *file_handle)
{
    COB_CHK_PARMS (CBL_OPEN_FILE, 5);

    return(open_cbl_file (rtd, file_name, file_access, file_handle, 0));
}

int
CBL_CREATE_FILE (unsigned char *file_name, unsigned char *file_access,
                 unsigned char *file_lock, unsigned char *file_dev,
                 unsigned char *file_handle)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_CREATE_FILE (rtd, file_name, file_access,
                                file_lock, file_dev,
                                file_handle);

}

int
rtd_CBL_CREATE_FILE (COB_RTD, unsigned char *file_name, unsigned char *file_access,
                     unsigned char *file_lock, unsigned char *file_dev,
                     unsigned char *file_handle)
{
    COB_CHK_PARMS (CBL_CREATE_FILE, 5);

    return(open_cbl_file (rtd, file_name, file_access, file_handle, O_CREAT | O_TRUNC));
}

int
CBL_CREATE_TMP_FILE (unsigned char *file_handle)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_CREATE_TMP_FILE (rtd, file_handle);

}

static FILE *tmpfiles[256] = {NULL};

int
rtd_CBL_CREATE_TMP_FILE (COB_RTD, unsigned char *file_handle)
{
    int h = 0;
    int i;
    FILE *tmp;

    COB_CHK_PARMS (CBL_CREATE_TMP_FILE, 1);
#ifdef _WIN32
    {
        char *fname;

        tmp = tmpfile();
        if (!tmp) {
            fname = cob_tmpfile_name(rtd);
            tmp = fopen(fname, "wb+");
        }
        if (!tmp) {
            perror( "Could not open new temporary file ");
            perror (fname);
        }

    }
#else
    tmp = tmpfile();
#endif
    if (tmp) {
        for (i=0; i < 256; i++) {
            if (!tmpfiles[i]) {
                tmpfiles[i]=tmp;
                h = fileno(tmp);
                break;
            }
        }
    }

    if (h) {
        memcpy (file_handle, &h, 4);
        return 0;
    } else {
        memset (file_handle, -1, 4);
        return(35);
    }
}

int 
rtd_CBL_OPEN_VFILE   (COB_RTD, unsigned char *file_handle, unsigned char *status)
{
    int s= rtd_CBL_CREATE_TMP_FILE(rtd, file_handle);
    status[0] = COB_I2D (s / 10);
    status[1] = COB_I2D (s % 10);

    return s;
}

int 
CBL_OPEN_VFILE   (unsigned char *file_handle, unsigned char *status)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_OPEN_VFILE (rtd, file_handle,status);
}

int
CBL_READ_FILE (unsigned char *file_handle, unsigned char *file_offset,
               unsigned char *file_len, unsigned char *flags, unsigned char *buf)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_READ_FILE (rtd, file_handle, file_offset,
                              file_len, flags, buf);

}

int
CBL_READ_VFILE (unsigned char *file_handle, unsigned char *file_offset,
                unsigned char *file_len, unsigned char *buf)
{    
    COB_RTD = cob_get_rtd();
    return rtd_CBL_READ_VFILE (rtd, file_handle, file_offset,
                               file_len,  buf);

}

int
rtd_CBL_READ_VFILE (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                    unsigned char *file_len, unsigned char *buf)
{
    return rtd_CBL_READ_FILE (rtd, file_handle, file_offset,
                              file_len, NULL, buf);

}

int
rtd_CBL_READ_FILE (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                   unsigned char *file_len, unsigned char *flags, unsigned char *buf)
{
    long long   off;
    int     fd;
    int     len;
    int     rc = 0;
    struct stat st;

    memcpy (&fd, file_handle, 4);
    memcpy (&off, file_offset, 8);
    memcpy (&len, file_len, 4);
#ifndef WORDS_BIGENDIAN
    off = COB_BSWAP_64 (off);
    len = COB_BSWAP_32 (len);
#endif
    if ( lseek (fd, (COB_OFF_T)off, SEEK_SET) < 0 ) {
        return(-1);
    }
    if ( len > 0 ) {
        rc = read (fd, buf, (size_t)len);
        if ( rc < 0 ) {
            rc = -1;
        } else if ( rc == 0 ) {
            rc = 10;
        } else {
            rc = 0;
        }
    }
    if (flags && ((*flags & 0x80) != 0) ) {
        if ( fstat (fd, &st) < 0 ) {
            return(-1);
        }
        off = st.st_size;
#ifndef WORDS_BIGENDIAN
        off = COB_BSWAP_64 (off);
#endif
        memcpy (file_offset, &off, 8);
    }
    return(rc);
}

int
CBL_WRITE_FILE (unsigned char *file_handle, unsigned char *file_offset,
                unsigned char *file_len, unsigned char *flags, unsigned char *buf)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_WRITE_FILE (rtd, file_handle, file_offset,
                               file_len, flags, buf);

}

int
CBL_WRITE_VFILE (unsigned char *file_handle, unsigned char *file_offset,
                 unsigned char *file_len,  unsigned char *buf)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_WRITE_VFILE (rtd, file_handle, file_offset,
                                file_len,  buf);

}

int
rtd_CBL_WRITE_VFILE (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                     unsigned char *file_len,  unsigned char *buf)
{
    return rtd_CBL_WRITE_FILE (rtd, file_handle, file_offset,
                               file_len, NULL, buf);

}

int
rtd_CBL_WRITE_FILE (COB_RTD, unsigned char *file_handle, unsigned char *file_offset,
                    unsigned char *file_len, unsigned char *flags, unsigned char *buf)
{
    long long   off;
    int     fd;
    int     len;
    int     rc;

    memcpy (&fd, file_handle, 4);
    memcpy (&off, file_offset, 8);
    memcpy (&len, file_len, 4);
#ifndef WORDS_BIGENDIAN
    off = COB_BSWAP_64 (off);
    len = COB_BSWAP_32 (len);
#endif
    if ( lseek (fd, (COB_OFF_T)off, SEEK_SET) < 0 ) {
        return(-1);
    }
    rc = write (fd, buf, (size_t)len);
    if ( rc < 0 ) {
        return(30);
    }
    return(0);
}

int
CBL_CLOSE_VFILE (unsigned char *file_handle)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_CLOSE_VFILE (rtd, file_handle);
}

int
rtd_CBL_CLOSE_VFILE (COB_RTD, unsigned char *file_handle)
{
    int fd;
    int i;
    int res = 0;
    memcpy (&fd, file_handle, 4);
    for (i = 0; i < 256; i++) {
        if (tmpfiles[i] && fileno(tmpfiles[i]) == fd) {
            res = fclose(tmpfiles[i]);            
            tmpfiles[i] = NULL;
            return res;
        }
    }

    return(close (fd));
}

int
CBL_CLOSE_FILE (unsigned char *file_handle)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_CLOSE_VFILE (rtd, file_handle);
}

int
rtd_CBL_CLOSE_FILE (COB_RTD, unsigned char *file_handle)
{
    return rtd_CBL_CLOSE_VFILE (rtd, file_handle);
}

int
CBL_FLUSH_FILE (unsigned char *file_handle)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_FLUSH_FILE (rtd, file_handle);
}

int
rtd_CBL_FLUSH_FILE (COB_RTD, unsigned char *file_handle)
{
    int fd;
    COB_CHK_PARMS (CBL_FLUSH_FILE, 1);
    memcpy (&fd, file_handle, 4);
#ifdef _WIN32
    _commit(fd);
#else 
#if HAVE_FSYNC
    fsync(fd);
#elif HAVE_FDATASYNC
    fdatasync(fd);
#elif HAVE_SYNC
    sync();
#endif
#endif
    return(0);
}

int
CBL_DELETE_FILE (unsigned char *file_handle)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_DELETE_FILE (rtd, file_handle);
}


int
rtd_CBL_DELETE_FILE (COB_RTD, unsigned char *file_name)
{
    char    *fn;
    int ret;

    COB_CHK_PARMS (CBL_DELETE_FILE, 1);

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        return(-1);
    }
    fn = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    ret = unlink (fn);
    cob_free (fn);
    if ( ret ) {
        return(128);
    }
    return(0);
}

int
CBL_COPY_FILE (unsigned char *fname1, unsigned char *fname2)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_COPY_FILE (rtd, fname1, fname2);
}

int
rtd_CBL_COPY_FILE (COB_RTD, unsigned char *fname1, unsigned char *fname2)
{
    char    *fn1;
    char    *fn2;
#ifdef  O_BINARY
    int flag = O_BINARY;
#else
    int flag = 0;
#endif
    int ret;
    int i;
    int fd1, fd2;
    char Buffer[1024];
    struct stat st; 

    COB_CHK_PARMS (CBL_COPY_FILE, 2);

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        return(-1);
    }
    if ( !rtd->current_module->cob_procedure_parameters[1] ) {
        return(-1);
    }
    fn1 = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    flag |= O_RDONLY;
    fd1 = open (fn1, flag| COB_O_BINARY, 0);
    cob_free (fn1);
    if ( fd1 < 0 ) { 
        return(-1);
    }
    fn2 = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[1]);
    flag &= ~O_RDONLY;
    flag |= O_CREAT | O_TRUNC | O_WRONLY;
#ifndef _MSC_VER
    if ( fstat(fd1, &st) < 0 ) 
        fd2 = open (fn2, flag| COB_O_BINARY, 0660);
    else
        fd2 = open (fn2, flag| COB_O_BINARY, st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
#else
    fd2 = open (fn2, flag| COB_O_BINARY, 0660);
#endif
    if ( fd2 < 0 ) {
        close (fd1);
        cob_free (fn2);
        return(-1);
    }
    cob_free (fn2);
    ret = 0;
    while ( (i = read (fd1, Buffer, sizeof(Buffer))) > 0 ) {
        if ( write (fd2, Buffer, (size_t)i) < 0 ) {
            ret = -1;
            break;
        }
    }
    close (fd1);
    close (fd2);
    return(ret);
}

int
CBL_CHECK_FILE_EXIST (unsigned char *file_name, unsigned char *file_info)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_CHECK_FILE_EXIST (rtd, file_name, file_info);

}

int
rtd_CBL_CHECK_FILE_EXIST (COB_RTD, unsigned char *file_name, unsigned char *file_info)
{
    char        *fn;
    struct tm   *tm;
    long long   sz;
    struct stat st;
    short       y;
    char        d, m, hh, mm, ss;

    COB_CHK_PARMS (CBL_CHECK_FILE_EXIST, 2);

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        return(-1);
    }
    fn = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    if ( cob_stat (fn, &st) < 0 ) {
        cob_free (fn);
        return(35);
    }
    cob_free (fn);
    sz = st.st_size;
    tm = localtime (&st.st_mtime);
    d = (char) tm->tm_mday;
    m = (char) tm->tm_mon + 1;
    y = tm->tm_year + 1900;
    hh = (char) tm->tm_hour;
    mm = (char) tm->tm_min;
    ss = (char) tm->tm_sec;

#ifndef WORDS_BIGENDIAN
    sz = COB_BSWAP_64 (sz);
    y = COB_BSWAP_16 (y);
#endif
    memcpy (file_info, &sz, 8);
    file_info[8] = d;
    file_info[9] = m;
    memcpy (file_info+10, &y, 2);
    file_info[12] = hh;
    file_info[13] = mm;
    file_info[14] = ss;
    file_info[15] = 0;
    return(0);
}

int
CBL_RENAME_FILE (unsigned char *fname1, unsigned char *fname2)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_RENAME_FILE (rtd, fname1, fname2);
}

int
rtd_CBL_RENAME_FILE (COB_RTD, unsigned char *fname1, unsigned char *fname2)
{
    char    *fn1;
    char    *fn2;
    int ret;

    COB_CHK_PARMS (CBL_RENAME_FILE, 2);

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        return(-1);
    }
    if ( !rtd->current_module->cob_procedure_parameters[1] ) {
        return(-1);
    }
    fn1 = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    fn2 = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[1]);
    ret = rename (fn1, fn2);
    cob_free (fn1);
    cob_free (fn2);
    if ( ret ) {
        return(128);
    }
    return(0);
}

int
CBL_GET_CURRENT_DIR (const int flags, const int dir_length, unsigned char *dir)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_GET_CURRENT_DIR (rtd, flags, dir_length, dir);
}

int
rtd_CBL_GET_CURRENT_DIR (COB_RTD, const int flags, const int dir_length, unsigned char *dir)
{
    char    *dirname;
    int dir_size;
    int has_space;
    char buff[COB_SMALL_BUFF];
    char *dirlen_s;
    int dirlen_i;

    COB_CHK_PARMS (CBL_GET_CURRENT_DIR, 3);

    if ( dir_length < 1 ) {
        return(128);
    }
    if ( flags ) {
        return(129);
    }
    dirlen_s = (char *) &dir_length;
    dirlen_i = atoi(dirlen_s);
    memset (dir, ' ', (size_t)dirlen_i);
    dirname = getcwd (buff, COB_SMALL_BUFF);
    if ( dirname == NULL ) {
        return(128);
    }
    dir_size = (int) strlen (dirname);
    has_space = 0;
    if ( strchr (dirname, ' ') ) {
        has_space = 2;
    }
    if ( dir_size + has_space > dirlen_i ) {
        return(128);
    }
    if ( has_space ) {
        *dir = '"';
        memcpy (&dir[1], dirname, (size_t)dir_size);
        dir[dir_size + 1] = '"';
    } else {
        memcpy (dir, dirname, (size_t)dir_size);
    }
    return(0);
}

int
CBL_CREATE_DIR (unsigned char *dir)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_CREATE_DIR (rtd, dir);
}

int
rtd_CBL_CREATE_DIR (COB_RTD, unsigned char *dir)
{
    char    *fn;
    int ret;

    COB_CHK_PARMS (CBL_CREATE_DIR, 1);

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        return(-1);
    }
    fn = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
#ifdef  _WIN32
    ret = mkdir (fn);
#else
    ret = mkdir (fn, 0770);
#endif
    cob_free (fn);
    if ( ret ) {
        return(128);
    }
    return(0);
}

int
CBL_CHANGE_DIR (unsigned char *dir)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_CHANGE_DIR (rtd, dir);
}

int
rtd_CBL_CHANGE_DIR (COB_RTD, unsigned char *dir)
{
    char    *fn;
    int ret;

    COB_CHK_PARMS (CBL_CHANGE_DIR, 1);

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        return(-1);
    }
    fn = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    ret = chdir (fn);
    cob_free (fn);
    if ( ret ) {
        return(128);
    }
    return(0);
}

int
CBL_DELETE_DIR (unsigned char *dir)
{
    COB_RTD = cob_get_rtd();
    return rtd_CBL_DELETE_DIR (rtd, dir);
}

int
rtd_CBL_DELETE_DIR (COB_RTD, unsigned char *dir)
{
    char    *fn;
    int ret;

    COB_CHK_PARMS (CBL_DELETE_DIR, 1);

    if ( !rtd->current_module->cob_procedure_parameters[0] ) {
        return(-1);
    }
    fn = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    ret = rmdir (fn);
    cob_free (fn);
    if ( ret ) {
        return(128);
    }
    return(0);
}

int
cob_acuw_mkdir (unsigned char *dir)
{
    COB_RTD = cob_get_rtd();
    return rtd_cob_acuw_mkdir (rtd, dir);
}

int
rtd_cob_acuw_mkdir (COB_RTD, unsigned char *dir)
{
    int     ret;

    ret = rtd_CBL_CREATE_DIR (rtd, dir);
    if ( ret < 0 ) {
        ret = 128;
    }
    return(ret);
}

int
cob_acuw_chdir (unsigned char *dir, unsigned char *status)
{
    COB_RTD = cob_get_rtd();
    return rtd_cob_acuw_chdir (rtd, dir, status);
}

int
rtd_cob_acuw_chdir (COB_RTD, unsigned char *dir, unsigned char *status)
{
    int     ret;

    COB_CHK_PARMS (C$CHDIR, 2);

    ret = rtd_CBL_CHANGE_DIR (rtd, dir);
    if ( ret < 0 ) {
        ret = 128;
    }
    cob_set_int (rtd, rtd->current_module->cob_procedure_parameters[1], ret);
    return(ret);
}

int
cob_acuw_copyfile (unsigned char *fname1, unsigned char *fname2, unsigned char *file_type)
{
    COB_RTD = cob_get_rtd();
    return rtd_cob_acuw_copyfile (rtd, fname1, fname2, file_type);
}

int
rtd_cob_acuw_copyfile (COB_RTD, unsigned char *fname1, unsigned char *fname2, unsigned char *file_type)
{
    int ret = 128;

    /* RXW - Type is not yet evaluated */

    COB_CHK_PARMS (C$COPY, 3);

    if ( rtd->cob_call_params < 3 ) {
        return(128);
    }
    ret = rtd_CBL_COPY_FILE (rtd, fname1, fname2);
    if ( ret < 0 ) {
        ret = 128;
    }
    return(ret);
}

int
cob_acuw_file_info (unsigned char *file_name, unsigned char *file_info)
{
    COB_RTD = cob_get_rtd();
    return rtd_cob_acuw_file_info (rtd, file_name, file_info);
}

int
rtd_cob_acuw_file_info (COB_RTD, unsigned char *file_name, unsigned char *file_info)
{
    char            *fn;
    struct tm       *tm;
    unsigned long long  sz;
    unsigned int        dt;
    short           y;
    short           d, m, hh, mm, ss;
    struct stat     st;

    COB_CHK_PARMS (C$FILEINFO, 2);

    if ( rtd->cob_call_params < 2 || !rtd->current_module->cob_procedure_parameters[0] ) {
        return(128);
    }
    fn = cob_str_from_filename_fld (rtd, rtd->current_module->cob_procedure_parameters[0]);
    if ( cob_stat (fn, &st) < 0 ) {
        cob_free (fn);
        return(35);
    }
    cob_free (fn);
    sz = st.st_size;
    tm = localtime (&st.st_mtime);
    d = tm->tm_mday;
    m = tm->tm_mon + 1;
    y = tm->tm_year + 1900;
    hh = tm->tm_hour;
    mm = tm->tm_min;
    ss = tm->tm_sec;

#ifndef WORDS_BIGENDIAN
    sz = COB_BSWAP_64 (sz);
#endif
    memcpy (file_info, &sz, 8);
    dt = (y * 10000) + (m * 100) + d;
#ifndef WORDS_BIGENDIAN
    dt = COB_BSWAP_32 (dt);
#endif
    memcpy (file_info + 8, &dt, 4);
    dt = (hh * 1000000) + (mm * 10000) + (ss * 100);
#ifndef WORDS_BIGENDIAN
    dt = COB_BSWAP_32 (dt);
#endif
    memcpy (file_info + 12, &dt, 4);
    return(0);
}

int
cob_acuw_file_delete (unsigned char *file_name, unsigned char *file_type)
{
    COB_RTD = cob_get_rtd();
    return rtd_cob_acuw_file_delete (rtd, file_name, file_type);
}

int
rtd_cob_acuw_file_delete (COB_RTD, unsigned char *file_name, unsigned char *file_type)
{
    int ret;

    /* RXW - Type is not yet evaluated */
    COB_CHK_PARMS (C$DELETE, 2);

    if ( rtd->cob_call_params < 2 || !rtd->current_module->cob_procedure_parameters[0] ) {
        return(128);
    }
    ret = rtd_CBL_DELETE_FILE (rtd, file_name);
    if ( ret < 0 ) {
        ret = 128;
    }
    return(ret);
}

/* SORT */

static int
sort_cmps (COB_RTD, const unsigned char *s1, const unsigned char *s2, const size_t size,
           const unsigned char *col)
{
    size_t          i;
    int         ret;

    if ( unlikely(col) ) {
        for ( i = 0; i < size; i++ ) {
            if ( (ret = col[s1[i]] - col[s2[i]]) != 0 ) {
                return(ret);
            }
        }
    } else {
        for ( i = 0; i < size; i++ ) {
            if ( (ret = s1[i] - s2[i]) != 0 ) {
                return(ret);
            }
        }
    }
    return(0);
}

static COB_INLINE void
unique_copy (unsigned char *s1, unsigned char *s2)
{
    size_t  size = sizeof(size_t);

    do {
        *s1++ = *s2++;
    } while ( --size );
}

cob_field_attr * FCD_to_key_field_attr (COB_RTD, int FCD_field_type, int FCD_field_size)
{
    cob_field_attr *attr;

    attr = cob_malloc(rtd, sizeof(cob_field_attr)); 
    if ( FCD_field_type & 0x80 ) {
        /*NUMERIC*/
        attr->type = COB_TYPE_NUMERIC;
        if ( FCD_field_type & 0x40 ) {
            attr->flags |= COB_FLAG_HAVE_SIGN;
        }
        if ( FCD_field_type & 0x20 ) {
            /*BINARY*/
            attr->digits = 36;
            if ( FCD_field_type & 0x4 ) {
                if ( FCD_field_size <= 4) {
                    attr->type = COB_TYPE_NUMERIC_FLOAT;
                } else {
                    attr->type = COB_TYPE_NUMERIC_DOUBLE;
                }
            } else {
                switch ( FCD_field_type & 0x3 ) {
                    case 1: 
                        attr->type = COB_TYPE_NUMERIC_PACKED; 
                        break;
                    case 3: 
                        attr->type = COB_TYPE_NUMERIC_BINARY; 
#ifndef   WORDS_BIGENDIAN
                        attr->flags |= COB_FLAG_BINARY_SWAP;
#endif
                        break;
                    default:
                        attr->type = COB_TYPE_NUMERIC_BINARY; 
                        break;
                }
            }

        } else {
            /*DISPLAY*/
            if ( FCD_field_type & 0x40 ) {
                /*SIGNED*/
                switch ( FCD_field_type & 0x3 ) {
                    case 0: /*no flag*/ break;
                    case 1: 
                        attr->flags |= COB_FLAG_SIGN_SEPARATE;
                        break;
                    case 2:
                        attr->flags |= COB_FLAG_SIGN_LEADING;
                        break;
                    case 3:
                        attr->flags |= COB_FLAG_SIGN_SEPARATE;
                        attr->flags |= COB_FLAG_SIGN_LEADING;
                        break;
                }
            }
        }
    }
    return attr;
}


static int
cob_file_sort_compare (COB_RTD, struct cobitem *k1, struct cobitem *k2, void *pointer)
{
    cob_file_extfh    *f;
    int         i;
    int         cmp;
    size_t      u1;
    size_t      u2;
    cob_field   f1;
    cob_field   f2;
    mf_extfh_FCD *fcd;
    int comp_count;
    cit_uint8               *key_def_block ;     
    mf_extfh_global_info    *global_info;
    mf_extfh_comp_def       *comp_def;
    mf_extfh_key_def        *key_def;
    cob_field_attr          *attr;
	FLD_VAR
    f = pointer;
    ASSERT_F_EXTFH_PTR;

    global_info = (mf_extfh_global_info*)GT_FLDP(fcd->key_def_block_ptr);

    key_def_block = (cit_uint8*)global_info;
    key_def     = (mf_extfh_key_def*)(key_def_block + sizeof(mf_extfh_global_info));

    comp_def = (mf_extfh_comp_def*)((char*)key_def_block + GT_FLD2(key_def[0].comp_offset));
    comp_count = GT_FLD2(key_def[0].comp_count);
    for ( i = 0; i < comp_count; i++ ) {
        f1.data = k1->item + GT_FLD4(comp_def[i].offset);
        f2.data = k2->item + GT_FLD4(comp_def[i].offset);
        f1.size = GT_FLD4(comp_def[i].length);
        f2.size = GT_FLD4(comp_def[i].length);
        attr = FCD_to_key_field_attr(rtd, GT_FLD1(comp_def[i].filer[1]), f1.size);
        f1.attr = attr;
        f2.attr = attr;

        if ( COB_FIELD_IS_NUMERIC(&f1) ) {
            cmp = cob_numeric_cmp (rtd, &f1, &f2);
        } else
        {
            cmp = sort_cmps (rtd, f1.data, f2.data, GT_FLD4(comp_def[i].length), GT_FLDP(fcd->sort_colating));
        }
        if ( cmp != 0 ) {/* (value x"40") indicates DESCENDING key*/
            return(GT_FLD1(comp_def[i].filer[0]) & 0x40) ? -cmp : cmp;
        }
        cob_free(attr);
    }
    unique_copy ((unsigned char *)&u1, k1->unique);
    unique_copy ((unsigned char *)&u2, k2->unique);
    if ( u1 < u2 ) {
        return(-1);
    }
    return(1);
}

static void
cob_free_list (COB_RTD, struct cobitem *q)
{
    struct cobitem  *next;

    while ( q != NULL ) {
        next = q->next;
        cob_free (q);
        q = next;
    }
}

static struct cobitem *
cob_new_item (COB_RTD, struct cobsort *hp, size_t size) {
    struct cobitem *q;

    if ( hp->empty != NULL ) {
        q = hp->empty;
        hp->empty = q->next;
    } else {
        q = cob_malloc (rtd, size);
    }
    return(q);
}

static char * COB_NOINLINE
cob_tmpfile_name (COB_RTD)
{
    const char  *s;
    char        *filename;

#ifdef _WIN32
    char        *tmpdir;
#endif

    filename = cob_malloc (rtd, COB_SMALL_BUFF);

#ifdef _WIN32
    tmpdir = cob_malloc (rtd, COB_SMALL_BUFF);
    GetTempPath (COB_SMALL_BUFF, tmpdir);
    GetTempFileName (tmpdir, "cob", 0, filename);
    cob_free (tmpdir);
#else
    if ( (s = getenv ("TMPDIR")) == NULL && (s = getenv ("TMP")) == NULL ) {
        s = "/tmp";
    }
    if ( rtd->mypid == 0 ) {
        rtd->mypid = getpid ();
    }
    sprintf (filename, "%s/cobsort%d_%d", s, rtd->mypid, rtd->tmp_uid);
    rtd->tmp_uid++;
#endif
    return(filename);
}

static int COB_NOINLINE
cob_get_temp_file (COB_RTD, struct cobsort *hp, const int n)
{
    if ( hp->file[n].fp == NULL ) {
        hp->file[n].filename = cob_tmpfile_name (rtd);
        unlink (hp->file[n].filename);
        hp->file[n].fp = COB_FOPEN (hp->file[n].filename, "fw+b");
        if ( hp->file[n].fp == NULL ) {
            cob_runtime_error (rtd, "SORT is unable to acquire temporary file");
            cob_stop_abend (rtd, COBRE_TMP_FILE);
        }
    } else {
        if (COB_REWIND (hp->file[n].fp)) {
            cob_runtime_error (rtd, "SORT is unable to acquire temporary file");
            cob_stop_abend (rtd, COBRE_TMP_FILE);
        }
    }
    hp->file[n].count = 0;
    return(hp->file[n].fp == NULL);
}

static int
cob_sort_queues (COB_RTD, struct cobsort *hp)
{
    struct cobitem  *q;
    int     source = 0;
    int     destination;
    int     move;
    int     n;
    int     end_of_block[2];

    while ( hp->queue[source + 1].count != 0 ) {
        destination = source ^ 2;
        hp->queue[destination].count = hp->queue[destination + 1].count = 0;
        hp->queue[destination].first = hp->queue[destination + 1].first = NULL;
        while ( 1 ) {
            end_of_block[0] = hp->queue[source].count == 0;
            end_of_block[1] = hp->queue[source + 1].count == 0;
            if ( end_of_block[0] && end_of_block[1] ) {
                break;
            }
            while ( !end_of_block[0] || !end_of_block[1] ) {
                if ( end_of_block[0] ) {
                    move = 1;
                } else if ( end_of_block[1] ) {
                    move = 0;
                } else {
                    n = cob_file_sort_compare(rtd, 
                                              hp->queue[source].first,
                                              hp->queue[source + 1].first,
                                              hp->pointer);
                    move = n < 0 ? 0 : 1;
                }
                q = hp->queue[source + move].first;
                if ( q->end_of_block ) {
                    end_of_block[move] = 1;
                }
                hp->queue[source + move].first = q->next;
                if ( hp->queue[destination].first == NULL ) {
                    hp->queue[destination].first = q;
                } else {
                    hp->queue[destination].last->next = q;
                }
                hp->queue[destination].last = q;
                hp->queue[source + move].count--;
                hp->queue[destination].count++;
                q->next = NULL;
                q->end_of_block = 0;
            }
            hp->queue[destination].last->end_of_block = 1;
            destination ^= 1;
        }
        source = destination & 2;
    }
    return(source);
}

static int
cob_read_item (COB_RTD, struct cobsort *hp, const int n)
{
    COB_FILE   *fp = hp->file[n].fp;

    if ( COB_GETC (fp) != 0 ) {
        hp->queue[n].first->end_of_block = 1;
    } else {
        hp->queue[n].first->end_of_block = 0;
        if ( unlikely(COB_FREAD (hp->queue[n].first->unique, hp->r_size, 1, fp) != 1) ) {
            return(1);
        }
    }
    return(0);
}

static int
cob_write_block (COB_RTD, struct cobsort *hp, const int n)
{
    struct cobitem  *q;
    COB_FILE       *fp = hp->file[hp->destination_file].fp;

    while ( 1 ) {
        q = hp->queue[n].first;
        if ( q == NULL ) {
            break;
        }
        if ( unlikely(COB_FWRITE (&(q->block_byte), hp->w_size, 1, fp) != 1) ) {
            return(1);
        }
        hp->queue[n].first = q->next;
        q->next = hp->empty;
        hp->empty = q;
    }
    hp->queue[n].count = 0;
    hp->file[hp->destination_file].count++;
    if ( COB_PUTC (1, fp) != 1 ) {
        return(1);
    }
    return(0);
}

static void
cob_copy_check (COB_RTD, unsigned char  *toptr,
                size_t         tosize,
                unsigned char  *fromptr,
                size_t         fromsize)
{
    if ( unlikely(tosize > fromsize) ) {
        memcpy (toptr, fromptr, fromsize);
        memset (toptr + fromsize, CHAR_SP, tosize - fromsize);
    } else {
        memcpy (toptr, fromptr, fromsize);
    }
}

static int
cob_file_sort_process (COB_RTD, struct cobsort *hp)
{
    int i;
    int source;
    int destination;
    int n;
    int move;
    int res;

    hp->retrieving = 1;
    n = cob_sort_queues (rtd, hp);
/* RXW - Cannot be true
    if (unlikely(n < 0)) {
        return COBSORTABORT;
    }
*/
    if ( likely(!hp->files_used) ) {
        hp->retrieval_queue = n;
        return(0);
    }
    if ( unlikely(cob_write_block (rtd, hp, n)) ) {
        return(COBSORTFILEERR);
    }
    for ( i = 0; i < 4; i++ ) {
        hp->queue[i].first = hp->empty;
        hp->empty = hp->empty->next;
        hp->queue[i].first->next = NULL;
    }
    if (unlikely(COB_REWIND (hp->file[0].fp))) {
        return(COBSORTFILEERR);
    }
    if (unlikely(COB_REWIND (hp->file[1].fp))) {
        return(COBSORTFILEERR);
    }
    if ( unlikely(cob_get_temp_file (rtd, hp, 2)) ) {
        return(COBSORTFILEERR);
    }
    if ( unlikely(cob_get_temp_file (rtd, hp, 3)) ) {
        return(COBSORTFILEERR);
    }
    source = 0;
    while ( hp->file[source].count > 1 ) {
        destination = source ^ 2;
        hp->file[destination].count = 0;
        hp->file[destination + 1].count = 0;
        while ( hp->file[source].count > 0 ) {
            if ( unlikely(cob_read_item (rtd, hp, source)) ) {
                return(COBSORTFILEERR);
            }
            if ( hp->file[source + 1].count > 0 ) {
                if ( unlikely(cob_read_item (rtd, hp, source + 1)) ) {
                    return(COBSORTFILEERR);
                }
            } else {
                hp->queue[source + 1].first->end_of_block = 1;
            }
            while ( !hp->queue[source].first->end_of_block
                    || !hp->queue[source + 1].first->end_of_block ) {
                if ( hp->queue[source].first->end_of_block ) {
                    move = 1;
                } else if ( hp->queue[source + 1].first->end_of_block ) {
                    move = 0;
                } else {
                    res = cob_file_sort_compare(rtd, 
                                                hp->queue[source].first,
                                                hp->queue[source + 1].first,
                                                hp->pointer);
                    move = res < 0 ? 0 : 1;
                }
                if ( unlikely(COB_FWRITE (
                                         &(hp->queue[source + move].first->block_byte),
                                         hp->w_size, 1,
                                         hp->file[destination].fp) != 1) ) {
                    return(COBSORTFILEERR);
                }
                if ( unlikely(cob_read_item (rtd, hp, source + move)) ) {
                    return(COBSORTFILEERR);
                }
            }
            hp->file[destination].count++;
            if ( unlikely(COB_PUTC (1, hp->file[destination].fp) != 1) ) {
                return(COBSORTFILEERR);
            }
            hp->file[source].count--;
            hp->file[source + 1].count--;
            destination ^= 1;
        }
        source = destination & 2;
        if (unlikely(COB_REWIND (hp->file[0].fp))) {
            return(COBSORTFILEERR);
        }
        if (unlikely(COB_REWIND (hp->file[1].fp))) {
            return(COBSORTFILEERR);
        }
        if (unlikely(COB_REWIND (hp->file[2].fp))) {
            return(COBSORTFILEERR);
        }
        if (unlikely(COB_REWIND (hp->file[3].fp))) {
            return(COBSORTFILEERR);
        }
    }
    hp->retrieval_queue = source;
    if ( unlikely(cob_read_item (rtd, hp, source)) ) {
        return(COBSORTFILEERR);
    }
    if ( unlikely(cob_read_item (rtd, hp, source + 1)) ) {
        return(COBSORTFILEERR);
    }
    return(0);
}

static int
cob_file_sort_submit (COB_RTD, cob_file_extfh *f, const unsigned char *p, size_t actual_size)
{
    struct cobsort          *hp;
    struct cobitem          *q;
    struct memory_struct    *z;
    int                     n;

    hp = f->file;
    if ( unlikely(!hp) ) {
        return(COBSORTNOTOPEN);
    }
    if ( unlikely(hp->retrieving) ) {
        return(COBSORTABORT);
    }
    if ( hp->queue[0].count + hp->queue[1].count >= hp->memory ) {
        if ( !hp->files_used ) {
            if ( unlikely(cob_get_temp_file (rtd, hp, 0)) ) {
                return(COBSORTFILEERR);
            }
            if ( unlikely(cob_get_temp_file (rtd, hp, 1)) ) {
                return(COBSORTFILEERR);
            }
            hp->files_used = 1;
            hp->destination_file = 0;
        }
        n = cob_sort_queues (rtd, hp);
        if ( unlikely(cob_write_block (rtd, hp, n)) ) {
            return(COBSORTFILEERR);
        }
        hp->destination_file ^= 1;
    }
    q = cob_new_item (rtd, hp, sizeof (struct cobitem) + hp->size);
    q->end_of_block = 1;
    unique_copy (q->unique, (unsigned char *)&(hp->unique));
    hp->unique++;
    memcpy (q->item, p, hp->size);
    if ( hp->size > actual_size ) {
        memset (q->item + actual_size, CHAR_SP, hp->size - actual_size);
    }
    unique_copy (q->actual_size, (unsigned char *)&actual_size) ;
    if ( hp->queue[0].count <= hp->queue[1].count ) {
        z = &hp->queue[0];
    } else {
        z = &hp->queue[1];
    }
    q->next = z->first;
    z->first = q;
    z->count++;
    return(0);
}

static int
cob_file_sort_retrieve (COB_RTD, cob_file_extfh *f, unsigned char *p, size_t *pactual_size)
{
    struct cobsort      *hp;
    struct cobitem      *next;
    struct memory_struct    *z;
    int         move;
    int         source;
    int         res;

    hp = f->file;
    if ( unlikely(!hp) ) {
        return(COBSORTNOTOPEN);
    }
    if ( unlikely(!hp->retrieving) ) {
        res = cob_file_sort_process (rtd, hp);
        if ( res ) {
            return(res);
        }
    }
    if ( unlikely(hp->files_used) ) {
        source = hp->retrieval_queue;
        if ( hp->queue[source].first->end_of_block ) {
            if ( hp->queue[source + 1].first->end_of_block ) {
                return(COBSORTEND);
            }
            move = 1;
        } else if ( hp->queue[source + 1].first->end_of_block ) {
            move = 0;
        } else {
            res = cob_file_sort_compare (rtd, hp->queue[source].first,
                                         hp->queue[source + 1].first,
                                         hp->pointer);
            move = res < 0 ? 0 : 1;
        }
        memcpy (p, hp->queue[source + move].first->item, hp->size);
        unique_copy((unsigned char*)pactual_size, hp->queue[source + move].first->actual_size);
        if ( unlikely(cob_read_item (rtd, hp, source + move)) ) {
            return(COBSORTFILEERR);
        }
    } else {
        z = &hp->queue[hp->retrieval_queue];
        if ( z->first == NULL ) {
            return(COBSORTEND);
        }
        memcpy (p, z->first->item, hp->size);
        unique_copy((unsigned char*)pactual_size, z->first->actual_size);
        next = z->first->next;
        z->first->next = hp->empty;
        hp->empty = z->first;
        z->first = next;
    }
    return(0);
}

void
cob_file_sort_using_extfh (COB_RTD, cob_file_extfh *sort_file, cob_file_extfh *data_file)
{
    int     ret;
    struct cobsort  *hp;
    cob_field       *fnstatus = NULL;   
    mf_extfh_FCD *sort_fcd, *data_fcd;
    FLD_VAR

    if(!sort_file->extfh_ptr || !data_file->extfh_ptr) 
        return;
    sort_fcd = ((mf_extfh_FCD *) sort_file->extfh_ptr);
    data_fcd = ((mf_extfh_FCD *) data_file->extfh_ptr);

    hp = sort_file->file;
    if ( likely(hp) ) {
        fnstatus = hp->fnstatus;
    }
    
    cob_open_extfh (rtd, data_file, COB_OPEN_INPUT, 0, NULL, 0);
    if ( data_fcd->user_file_status[0] != CHAR_0 ) {
        sort_fcd->user_file_status[0] = data_fcd->user_file_status[0];
        sort_fcd->user_file_status[1] = data_fcd->user_file_status[1];
        if ( likely(hp) && hp->sort_return ) {
            *(int *)(hp->sort_return) = 16;
        }
        return;
    }
    while ( 1 ) {
        cob_read_extfh (rtd, data_file, NULL, NULL, COB_READ_NEXT);
        if ( data_fcd->user_file_status[0] != CHAR_0 ) {
            if (data_fcd->user_file_status[0] != CHAR_1 ) {
                sort_fcd->user_file_status[0] = data_fcd->user_file_status[0];
                sort_fcd->user_file_status[1] = data_fcd->user_file_status[1];
                if ( likely(hp) && hp->sort_return ) {
                    *(int *)(hp->sort_return) = 16;
                }
            }
            break;
        }
        {
           unsigned char *to = (unsigned char*) GT_FLDP(sort_fcd->rec_data_ptr);
           size_t toSz = GT_FLD2_4(sort_fcd->cur_rec_length);
           unsigned char *from = GT_FLDP(data_fcd->rec_data_ptr);
           size_t fromSz = GT_FLD2_4(data_fcd->cur_rec_length);
           cob_copy_check (rtd,to, toSz, from, fromSz /*sort_file->record->size*/);
        }
        ret = cob_file_sort_submit (rtd, sort_file, (unsigned char*) GT_FLDP(sort_fcd->rec_data_ptr), GT_FLD2_4(data_fcd->cur_rec_length) );
        if ( ret ) {
            break;
        }
    }
    cob_close_extfh (rtd, data_file, COB_CLOSE_NORMAL, NULL);
}

void
cob_file_sort_giving_table_extfh (COB_RTD, cob_file_extfh *sort_file, const size_t varcnt, cob_file_extfh  **fbase)
{
    struct cobsort  *hp;
    int             i;
    int             ret;
    int             opt;
    size_t          actual_size;
    cob_field *rec;
    mf_extfh_FCD *sort_fcd, *fbase_fcd;
    FLD_VAR
    if(!sort_file->extfh_ptr) 
        return;

    sort_fcd  = ((mf_extfh_FCD *) sort_file->extfh_ptr);
    
    hp = sort_file->file;
    for ( i = 0; i < varcnt; i++ ) {
        cob_open_extfh (rtd, fbase[i], COB_OPEN_OUTPUT, 0, NULL, 0);
        fbase_fcd = ((mf_extfh_FCD *) fbase[i]->extfh_ptr);

        if ( fbase_fcd->user_file_status[0] != CHAR_0 ) {
            sort_fcd->user_file_status[0] = fbase_fcd->user_file_status[0];
            sort_fcd->user_file_status[1] = fbase_fcd->user_file_status[1];
            if ( likely(hp) && hp->sort_return ) {
                *(int *)(hp->sort_return) = 16;
            }
            for ( i = 0; i < varcnt; i++ ) {
                cob_close_extfh (rtd, fbase[i], COB_CLOSE_NORMAL, NULL);
            }
            return;
        }
    }
    while ( 1 ) {
        ret = cob_file_sort_retrieve (rtd, sort_file,(unsigned char *) GT_FLDP(sort_fcd->rec_data_ptr), &actual_size);

        if ( ret ) {
            if ( ret == COBSORTEND ) {
                sort_fcd->user_file_status[0] = CHAR_1;
                sort_fcd->user_file_status[1] = CHAR_0;
            } else {
                if ( hp->sort_return ) {
                    *(int *)(hp->sort_return) = 16;
                }
                sort_fcd->user_file_status[0] = CHAR_3;
                sort_fcd->user_file_status[1] = CHAR_0;
            }
            break;
        }
        for ( i = 0; i < varcnt; i++ ) {
            fbase_fcd = ((mf_extfh_FCD *) fbase[i]->extfh_ptr);

            if ( GT_FLD1(fbase_fcd->device_flag) /*||
                  fbase[i]->organization == COB_ORG_LINE_SEQUENTIAL */ ) {
                opt = COB_WRITE_BEFORE | COB_WRITE_LINES | 1;
            } else {
                opt = 0;
            }
            if (FCD_VAR_REC_MODE(fbase_fcd)) {
                //int sz = min(fbase[i]->record_max, sort_file->record->size);
                register int record_max = GT_FLD2_4(fbase_fcd->max_rec_length);

                int sz = min(record_max, actual_size);
                if ( fbase[i]->record_size ) {
                    cob_set_int(rtd, fbase[i]->record_size, sz);
                }
                CP_FLD2_4(fbase_fcd->cur_rec_length, sz);
            }
            {
               unsigned char *to = (unsigned char*) GT_FLDP(fbase_fcd->rec_data_ptr);
               size_t toSz =GT_FLD2_4(fbase_fcd->cur_rec_length);
               unsigned char *from = (unsigned char*)GT_FLDP(sort_fcd->rec_data_ptr);
               size_t fromSz = actual_size;
               cob_copy_check (rtd,to, toSz, from, fromSz /*sort_file->record->size*/);
            }
            /*create artifically record field to replace fbase[i]->record*/
            rec = cob_malloc(rtd, sizeof(cob_field));
            rec->size      = GT_FLD2_4(fbase_fcd->cur_rec_length);
            cob_write_extfh (rtd, fbase[i], rec, opt, NULL);
            cob_free(rec);

        }
    }
    for ( i = 0; i < varcnt; i++ ) {
        cob_close_extfh (rtd, fbase[i], COB_CLOSE_NORMAL, NULL);
    }
}

void
cob_file_sort_giving_extfh (COB_RTD, cob_file_extfh *sort_file, const size_t varcnt, ...)
{
    cob_file_extfh    **fbase;
    int         i;
    va_list     args;

    fbase = cob_malloc (rtd, varcnt * sizeof(cob_file_extfh *));
    va_start (args, varcnt);
    for ( i = 0; i < varcnt; i++ ) {
        fbase[i] = va_arg (args, cob_file_extfh *);
    }
    va_end (args);
    cob_file_sort_giving_table_extfh (rtd,sort_file,varcnt,fbase);
    cob_free (fbase);
}
void cobsort_init(COB_RTD, cob_file_extfh *f, void *sort_return, cob_field *fnstatus)
{
    struct cobsort  *sort_object;
    mf_extfh_FCD   *fcd;
	FLD_VAR
    ASSERT_F_EXTFH_PTR


    sort_object = cob_malloc (rtd, sizeof (struct cobsort));
    sort_object->fnstatus = fnstatus;
    sort_object->size = GT_FLD2_4(fcd->max_rec_length);
    sort_object->r_size = sort_object->size + (sizeof(size_t) * 2) ;
    sort_object->w_size = sort_object->size + (sizeof(size_t) * 2) + 1;
    sort_object->pointer = f;
    if ( sort_return ) {
        sort_object->sort_return = sort_return;
        *(int *)sort_return = 0;
    }
    sort_object->memory = (size_t)rtd->cob_sort_memory / (sort_object->size + sizeof(struct cobitem));
    f->file= sort_object;
}

void
cob_file_sort_close_extfh (COB_RTD, cob_file_extfh *f)
{
    struct cobsort  *hp;
    cob_field   *fnstatus = NULL;
    int         i;
    void        *keys;
    mf_extfh_FCD *fcd;
	FLD_VAR
    ASSERT_F_EXTFH_PTR


    hp = f->file;
    if ( likely(hp) ) {
        fnstatus = hp->fnstatus;
        cob_free_list (rtd, hp->empty);
        for ( i = 0; i < 4; i++ ) {
            cob_free_list (rtd, hp->queue[i].first);
            if ( hp->file[i].fp != NULL ) {
                COB_FCLOSE (hp->file[i].fp);
            }
            if ( hp->file[i].filename != NULL ) {
                unlink (hp->file[i].filename);
                cob_free(hp->file[i].filename);
            }
        }
        cob_free (hp);
    }
    f->file= NULL;

    keys = GT_FLDP(fcd->key_def_block_ptr);
    cob_free(keys);
    CP_FLDP(fcd->key_def_block_ptr, 0);
    /*RETURN_STATUS (COB_STATUS_00_SUCCESS);*/
}

void
cob_file_release_1_extfh (COB_RTD, cob_file_extfh *f, cob_field *rec)
{
    struct cobsort  *hp;
    cob_field       *fnstatus = NULL;
    int             ret;
    mf_extfh_FCD *fcd;
	FLD_VAR
    ASSERT_F_EXTFH_PTR


    hp = f->file;
    if ( likely(hp) ) {
        fnstatus = hp->fnstatus;
    }
    if ( f->record_size ) {
        register int record_max = GT_FLD2_4(fcd->max_rec_length);
        register int size = min (record_max, cob_get_int (rtd, f->record_size));
        CP_FLD2_4(fcd->cur_rec_length, size);
    } else if (rec) {
        CP_FLD2_4(fcd->cur_rec_length, rec->size);
    }
    ret = cob_file_sort_submit (rtd, f, (unsigned char*) GT_FLDP(fcd->rec_data_ptr), GT_FLD2_4(fcd->cur_rec_length));
    switch ( ret ) {
        case 0:
            RETURN_STATUS (COB_STATUS_00_SUCCESS);
            break;
        default:
            if ( likely(hp) && hp->sort_return ) {
                *(int *)(hp->sort_return) = 16;
            }
            RETURN_STATUS (COB_STATUS_30_PERMANENT_ERROR);
            break;
    }
}

void
cob_file_release_extfh (COB_RTD, cob_file_extfh *f)
{
    cob_file_release_1_extfh(rtd, f, NULL);
}

void
cob_file_return_extfh (COB_RTD, cob_file_extfh *f)
{
    struct cobsort  *hp;
    cob_field       *fnstatus = NULL;
    int             ret;
    size_t          actual_size;
    mf_extfh_FCD *fcd;
	FLD_VAR
    ASSERT_F_EXTFH_PTR


    hp = f->file;
    if ( likely(hp) ) {
        fnstatus = hp->fnstatus;
    }
    ret = cob_file_sort_retrieve (rtd, f, (unsigned char *) GT_FLDP(fcd->rec_data_ptr), &actual_size);
    switch ( ret ) {
        case 0:
            if (FCD_VAR_REC_MODE(fcd)) {
                
                register int record_max = GT_FLD2_4(fcd->max_rec_length);
                int sz = min(record_max, actual_size);
                if ( f->record_size ) {
                    cob_set_int(rtd, f->record_size, sz);
                }
                CP_FLD2_4(fcd->cur_rec_length, sz);
            }
            if ( GT_FLD2_4(fcd->cur_rec_length) > actual_size ) {
				register int size = GT_FLD2_4(fcd->cur_rec_length) - actual_size;
				unsigned char *data_ptr = ((unsigned char *) GT_FLDP(fcd->rec_data_ptr) + actual_size);
                memset (data_ptr, CHAR_SP, size);
            }
            RETURN_STATUS (COB_STATUS_00_SUCCESS);
            break;
        case COBSORTEND:
            RETURN_STATUS (COB_STATUS_10_END_OF_FILE);
            break;
        default:
            if ( likely(hp) && hp->sort_return ) {
                *(int *)(hp->sort_return) = 16;
            }
            RETURN_STATUS (COB_STATUS_30_PERMANENT_ERROR);
            break;
    }
}




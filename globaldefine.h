#ifndef _GLOBAL_DEFINE_H_
#define _GLOBAL_DEFINE_H_ 1

#define _LFS64_LARGEFILE                1
#define _LFS64_STDIO                    1
#define _FILE_OFFSET_BITS               64
#define _LARGEFILE64_SOURCE             1
#define _LARGE_FILES                    1
#define _LARGE_FILE_API                 1

#if defined(__hpux__) && !defined(__LP64__)
    #define _APP32_64BIT_OFF_T          1
#endif

#ifdef __MINGW32__
    #define __USE_MINGW_FSEEK           1
#endif 
#define E4C_THREADSAFE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#endif

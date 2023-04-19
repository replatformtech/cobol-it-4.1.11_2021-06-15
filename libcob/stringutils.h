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
#ifndef COB_STRINGUTILS_H
#define COB_STRINGUTILS_H
#ifndef min
    #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
    #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifdef _WIN32
    #define COB_STRTOK(a,b,c) strtok_s(a,b,c)
#else
    #ifdef _REENTRANT
        #define COB_STRTOK(a,b,c) strtok_r(a,b,c)
    #else
        #define COB_STRTOK(a,b,c) strtok(a,b)
    #endif
#endif

#ifndef STRCASEEQ
#define STRCASEEQ(A, B) \
    (strcasecmp((A), (B)) == 0)
#endif

extern char *cob_str_strip(char*p, char * buffer);
extern int cobc_filename_replace_env(char * src, char * dst, int max_sz, char insert_dir_sep);
extern void cob_strupper (char *s);
extern void cob_strlower (char *s);


#endif

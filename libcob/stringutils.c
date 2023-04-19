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
#ifndef COB_STRINGUTILS_C
#define COB_STRINGUTILS_C

char *
cob_str_strip(char*p, char * buffer) {
    char *q;
    /* !!! Must not convert to CHAR_SPACE , this is used only for system ASCII file Name*/
    while ( *p == ' ' ) {
        p++;
    }
    if (buffer) {
        strcpy(buffer, p);
    } else {
        buffer = p;
    }
    q = buffer + strlen(p) - 1 ;
    while ( (q >= buffer) && *q == ' ' ) {
        *q = 0;
        q--;
    }
    return(buffer);
}

/*
int 
cobc_filename_replace_env(char * src, char * dst, int max_sz)
{
    char *      p;
    size_t      i;
    size_t      simple = 1;
    char        buff[COB_SMALL_BUFF];
    char        file_open_env[COB_SMALL_BUFF];
    while ( *src && (max_sz > 1)) {
        if ( !isalnum (*src) && *src != '_' && *src != '-' && *src != '.' ) {
            simple = 0;
        }
        if ( *src == '$' ) {
            for ( i = 1; ; i++ ) {
                if ( !isalnum (src[i]) && src[i] != '_' && *src != '-' ) {
                    break;
                }
            }
            memcpy (file_open_env, src + 1, i - 1);
            file_open_env[i - 1] = 0;
            if ( (p = getenv (file_open_env)) != NULL ) {
                int j; 
                cob_str_strip(p, buff);
                j = strlen(buff);
                strncpy (dst, buff , max_sz);
                dst += min(j, max_sz);
                max_sz -= j;
            }
            src += i;
        } else {
            *dst++ = *src++;
            max_sz--;
        }
    }
    *dst = 0;
    return simple;
}
*/
int 
cobc_filename_replace_env(char * src, char * dst, int max_sz, char insert_dir_sep)
{
    char *      p;
    size_t      i;
    size_t      simple = 1;
    char        buff[COB_SMALL_BUFF];
    char        file_open_env[COB_SMALL_BUFF];
    char *      dststart = dst;
    #ifdef _WIN32
    if (*src && isalpha(*src) && *(src+1) == ':' && max_sz > 1) {
        *dst++ = *src++;
        *dst++ = *src++;
        max_sz-=2;
    }
    #endif
    while (*src && (max_sz > 1)) {
        if ( !isalnum (*src) && *src != '_' && *src != '-' && *src != '.' ) {
            simple = 0;
        }
        if ( *src == '$' ) {
            for ( i = 1; ; i++ ) {
                if ( !isalnum (src[i]) && src[i] != '_' && *src != '-' ) {
                    break;
                }
            }
            memcpy (file_open_env, src + 1, i - 1);
            file_open_env[i - 1] = 0;
            if ( (p = getenv (file_open_env)) != NULL ) {
                int j; 
                cob_str_strip(p, buff);
                j = strlen(buff);
                strncpy (dst, buff , max_sz);
                dst += min(j, max_sz);
                max_sz -= j;
            }
            src += i;
        } else {
            if (*src == ':') {

                *dst = 0;
                dst = dststart;
                strcpy(file_open_env, dststart);
                if ( (p = getenv (file_open_env)) != NULL ) {
                    int j; 
                    cob_str_strip(p, buff);
                    j = strlen(buff);
                    if (insert_dir_sep) {
                        #ifdef	_MSC_VER
                        buff[j] = '\\';
                        #else
                        buff[j] = '/';
                        #endif
                        buff[++j] = 0;
                    }
                    strncpy (dst, buff , max_sz);
                    dst += min(j, max_sz);
                    max_sz -= j;
                }
                src += 1;
                dststart = dst;
            } else {
                *dst++ = *src++;
                max_sz--;
            }
        }
    }
    *dst = 0;
    return simple;
}

void 
cob_strupper (char *s) {
    for ( ; *s; s++ ) {
        *s = toupper (*s);
    }
}

void 
cob_strlower (char *s) {
    for ( ; *s; s++ ) {
        *s = tolower (*s);
    }
}

#endif

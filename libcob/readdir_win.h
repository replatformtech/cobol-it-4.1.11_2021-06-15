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

#ifdef _MSC_VER

/* Code From LT tools*/
    #define dirent win_dirent
    #define DIR win_DIR

struct dirent {
    char d_name[2048];
    int  d_namlen;
};

typedef struct _DIR {
    HANDLE hSearch;
    WIN32_FIND_DATA Win32FindData;
    BOOL firsttime;
    struct dirent file_info;
} DIR;

static void closedir (DIR *entry)
{
    FindClose(entry->hSearch);
    free(entry);
}


static DIR * opendir (const char *path)
{
    char file_specification[MAX_PATH];
    DIR *entry;

    /* allow space for: path + '\\CHAR_SP\\' '*' '.' '*' + '\0' */
    (void) strncpy (file_specification, path, MAX_PATH-6);
    file_specification[MAX_PATH-6] = 0;
    (void) strcat(file_specification,"\\");
    entry = calloc (1, sizeof(DIR));
    if (entry != (DIR *) 0) {
        entry->firsttime = TRUE;
        entry->hSearch = FindFirstFile(file_specification,&entry->Win32FindData);
    }
    if (entry->hSearch == INVALID_HANDLE_VALUE) {
        (void) strcat(file_specification,"\\*.*");
        entry->hSearch = FindFirstFile(file_specification,&entry->Win32FindData);
        if (entry->hSearch == INVALID_HANDLE_VALUE) {
            free (entry);
            return(DIR *) 0;
        }
    }
    return(entry);
}


static struct dirent *readdir (DIR *entry) {
    int
    status;

    if (entry == (DIR *) 0)
        return((struct dirent *) 0);
    if (!entry->firsttime) {
        status = FindNextFile(entry->hSearch,&entry->Win32FindData);
        if (status == 0)
            return((struct dirent *) 0);
    }
    entry->firsttime = FALSE;
    (void) strncpy(entry->file_info.d_name,entry->Win32FindData.cFileName,
                   MAX_PATH-1);
    entry->file_info.d_name[MAX_PATH - 1] = 0;
    entry->file_info.d_namlen = strlen(entry->file_info.d_name);
    return(&entry->file_info);
}
#else
    #include    <dirent.h>
#endif

// Function that matches input str with 
// given wildcard pattern 
static char strmatch(char *str, char *pattern, 
              int n, int m) 
{ 
   int s=0, p=0;
   int match = 1;
    // empty pattern can only match with 
    // empty string 
    if (m == 0) 
        return (n == 0); 

  
    // lookup table for storing results of 
    // subproblems 
    while (match && s < n && p < m) {
       if (pattern[p] == '*')
       {
          if (p < m-1)
          {
             char nextc = pattern[p+1];
             while (s < n && str[s] != nextc)
             {
                s++;
             }
          } else
          {
             s =n;
          }
          p++;
       } else{
          match = str[s] == pattern[p];
          s++;
          p++;
       }             
    }
    match = match && (s == n && p == m);
    return match;
} 

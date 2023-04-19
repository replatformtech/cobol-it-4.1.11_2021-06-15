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
#ifndef DEBUGDB_H
#define DEBUGDB_H
#ifdef _MSC_VER
    #ifdef LIBCOB_EXPORTS
        #define COB_DLL_EXPIMP __declspec(dllexport) 
        #define COB_DLL_EXPORT __declspec(dllexport) 
    #else /* LIBCOB_EXPORTS */
        #define COB_DLL_EXPIMP __declspec(dllimport) extern 
        #define COB_DLL_EXPORT
    #endif /* LIBCOB_EXPORTS */
#else /* _MSC_VER */
    #define COB_DLL_EXPIMP extern 
    #define COB_DLL_EXPORT
#endif /* _MSC_VER */

COB_DLL_EXPIMP void * debugdb_opendb(char* dbpath, int create);
COB_DLL_EXPIMP void * debugdb_closedb(void*dbi);
COB_DLL_EXPIMP long long debugdb_add_module (void*dbi,const char * name,  const char * source_name, const char *timestamp);
COB_DLL_EXPIMP void debugdb_add_field_rec(void*dbi, int idx, long long moduleid, int fieldid, char * name, 
                                int occurs, char *redefine_name, int groupid, 
                                int childid, char *fullname, int storageid);
COB_DLL_EXPIMP long long debugdb_find_module (void*dbi,const char * name,  const char * source_name, const char *timestamp, int *fieldcnt, int *linescnt);
COB_DLL_EXPIMP int debugdb_find_field (void*dbi,long long moduleid, const char * name, int *occurs, char *fullname);
COB_DLL_EXPIMP void debugdb_start_listfield (void*dbi,long long moduleid);
COB_DLL_EXPIMP int debugdb_listfield (void*dbi, int *occurs, char *name, int * groupid, int * childid, int * childcnt);
COB_DLL_EXPIMP int debugdb_reset_listfield (void*dbi);
COB_DLL_EXPIMP void debugdb_start_listlines (void*dbi,long long moduleid);
COB_DLL_EXPIMP int debugdb_listlines (void*dbi, int *idx, int *linenr, char * label, int * issection, char ** sourcepath);
COB_DLL_EXPIMP int debugdb_reset_listlines (void*dbi);
COB_DLL_EXPIMP void debugdb_update_module (void*dbi,long long moduleid, int childcnt, int linescnt);
COB_DLL_EXPIMP int debugdb_find_field (void*dbi,long long moduleid, const char * name, int *occurs, char *fullname);
COB_DLL_EXPIMP void debugdb_start_transaction (void*dbi);
COB_DLL_EXPIMP void debugdb_commit_transaction (void*dbi);
COB_DLL_EXPIMP void debugdb_add_line_rec(void*dbi, long long moduleid, int idx, char * sourcepath, int line_nr, char * label, int is_section);
COB_DLL_EXPIMP void debugdb_add_ccmap_rec(void*dbi, long long moduleid, int idx, char * sourcepath, int line_nr,  char type);
COB_DLL_EXPIMP void debugdb_add_ccmaprun_rec(void*dbi, long long moduleid, int * data , int datasize);
COB_DLL_EXPIMP void debugdb_update_ccmap_rec(void*dbi, long long moduleid, int idx, int cnt);
COB_DLL_EXPIMP void debugdb_consolidate_ccmap(void*dbi, int verbose);



#endif

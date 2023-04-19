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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

#include "debugdb.h"
typedef struct debugdb_info {
    sqlite3 *db;
    sqlite3_stmt * insert_field;
    sqlite3_stmt * insert_line;
    sqlite3_stmt * insert_sourceid;
    sqlite3_stmt * insert_ccmap;
    sqlite3_stmt * insert_ccmaprun;
    sqlite3_stmt * update_ccmap;
    sqlite3_stmt * find_module;
    sqlite3_stmt * find_one_module;
    sqlite3_stmt * find_field;
    sqlite3_stmt * find_all_field;
    sqlite3_stmt * find_all_lines;
    sqlite3_stmt * find_sourceid;
    sqlite3_stmt * find_sourcepath;
    char *errmsg;
}debugdb_info;

static char sqlcreate[] =
"CREATE TABLE IF NOT EXISTS modules  (   "
"    moduleid INTEGER PRIMARY KEY,      "
"    name       CHAR,                    "
"    sourcepath CHAR,                    "
"    timestamp  CHAR,                    "
"    fieldcnt   INT,                     "
"    linescnt   INT                      "
");                                      "
"CREATE TABLE IF NOT EXISTS sources  (   "
"    sourceid INTEGER PRIMARY KEY,      "
"    sourcepath CHAR                     "
");                                      "
"CREATE TABLE IF NOT EXISTS ccmap  (   "
"    moduleid  INTEGER,  "
"    idx       INTEGER,  "
"    sourceid  INTEGER,  "
"    linenr    INTEGER,  "
"    type      CHAR   ,  "
"    execcnt     INTEGER  DEFAULT 0 "
");                                      "
"CREATE TABLE IF NOT EXISTS ccmaprun  (   "
"    ccmaprunid INTEGER PRIMARY KEY ASC, "
"    moduleid  INTEGER,  "
"    idxstart  INTEGER,  "
"    idxcnt    INTEGER,  "
"    ccmapruntime CHAR, "
"    data      BLOB "
");                                      "
"CREATE TABLE IF NOT EXISTS fields (     "
"    idx          INT,       "
"    id           INT,       "
"    moduleid     INTEGER,   "
"    name         CHAR,  "
"    goupid       INT,   "
"    childgroupid INT,   "
"    occurs       INT,   "
"    fullname     CHAR,  "
"    childcnt     INT    "
");  "
"CREATE TABLE IF NOT EXISTS linemap (    "
"    moduleid  INTEGER,  "
"    idx       INTEGER,  "
"    sourceid  INTEGER,  "
"    linenr    INTEGER,  "
"    label     CHAR, "
"    issection BOOLEAN   "
");  "
"CREATE INDEX IF NOT EXISTS idx_modules ON modules (     "
"    sourcepath, "
"    timestamp   "
");  "
"CREATE INDEX IF NOT EXISTS idx_sources ON sources (     "
"    sourcepath "
");  "
"CREATE INDEX IF NOT EXISTS idx_field_idx ON fields  (   "
"    moduleid,   "
"    idx    "
");  "
"CREATE INDEX IF NOT EXISTS idx_linemap_idx ON linemap (     "
"    moduleid,   "
"    idx     "
");  "
"CREATE INDEX IF NOT EXISTS idx_ccmap_idx ON ccmap (     "
"    moduleid,   "
"    idx     "
");  "
"CREATE INDEX IF NOT EXISTS idx_ccmaprun_idx ON ccmaprun (     "
"    ccmapruntime,   "
"    moduleid,    "
"    idxstart     "
");  "
"CREATE view IF NOT EXISTS view_ccmaprun AS"
"    SELECT * from ccmaprun, modules "
"             where ccmaprun.moduleid = modules.ROWID ;  "
;

void * debugdb_opendb(char* dbpath, int create)
{
    int rc;
    debugdb_info *db_info;
    db_info = calloc(1,sizeof(debugdb_info));

    rc = sqlite3_open_v2(dbpath, &db_info->db, create ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE : SQLITE_OPEN_READWRITE, NULL);
    if ( rc ) {
        fprintf(stderr, "Can't open debugdb: %s\n", sqlite3_errmsg(db_info->db));
        sqlite3_close(db_info->db);
        free (db_info);
        return(NULL);
    }
    if (sqlite3_exec (db_info->db, sqlcreate, NULL, NULL, &db_info->errmsg) != SQLITE_OK){
        fprintf(stderr, "Error creating debugdb: %s\n", db_info->errmsg ? db_info->errmsg : sqlite3_errmsg(db_info->db));
        sqlite3_close(db_info->db);
        free (db_info);
        return(NULL);
    }

    return db_info;
}

void * debugdb_closedb(void*dbi)
{
    debugdb_info *db_info= dbi; 
    if (db_info) {
        if (db_info->insert_field) {
            sqlite3_finalize(db_info->insert_field);
            db_info->insert_field = NULL;
        }
        if (db_info->db) {
            sqlite3_close(db_info->db);
            db_info->db=NULL;
        }
        free(db_info);
    }
    return NULL;
}

void debugdb_start_transaction (void*dbi)
{
    debugdb_info *db_info= dbi; 
    char sql[1024];
    if (!db_info) {
        return;
    }
    sprintf (sql, "BEGIN TRANSACTION;");
    if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK) {
        fprintf(stderr, "Error begin transaction in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
}

void debugdb_commit_transaction (void*dbi)
{
    debugdb_info *db_info= dbi; 
    char sql[1024];
    if (!db_info) {
        return;
    }
    sprintf (sql, "COMMIT TRANSACTION;");
    if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK) {
        fprintf(stderr, "Error commit transaction in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
}

long long debugdb_find_module (void*dbi,const char * name,  const char * source_name, const char *timestamp,int *fieldcnt, int *linescnt)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int res;
    long long rowid = 0;

    if (!db_info) {
        return 0;
    }
    if (timestamp) {
        if (!db_info->find_one_module) {
            if (sqlite3_prepare_v2(db_info->db," SELECT ROWID, [fieldcnt], [linescnt]  FROM modules WHERE "
                                               "[sourcepath] = ?1 AND "   /* 1 */
                                               "[name] = ?2       AND "   /* 2 */
                                               "[timestamp]  = ?3;"       /* 3 */
                                   , -1, &db_info->find_one_module, NULL) != SQLITE_OK) {
                fprintf(stderr, "Can't prepare find one module : %s\n", sqlite3_errmsg(db_info->db));
                return 0;
            }
        }

        stmt = db_info->find_one_module;
    } else {
        if (!db_info->find_module) {
            if (sqlite3_prepare_v2(db_info->db," SELECT ROWID FROM modules WHERE "
                                               "[sourcepath] = ?1 AND "   /* 1 */
                                               "[name] = ?2; "            /* 2 */
                                   , -1, &db_info->find_module, NULL) != SQLITE_OK) {
                fprintf(stderr, "Can't prepare find module : %s\n", sqlite3_errmsg(db_info->db));
                return 0;
            }
        }
        stmt = db_info->find_module;
    }
    sqlite3_bind_text (stmt, 1, source_name, -1, NULL);
    sqlite3_bind_text (stmt, 2, name, -1, NULL);
    if (timestamp) {
        sqlite3_bind_text (stmt, 3, timestamp, -1, NULL);
    }

    res= sqlite3_step(stmt);
    if (res == SQLITE_ROW) {
        rowid =  sqlite3_column_int64(stmt, 0);
        if (fieldcnt) {
            *fieldcnt = sqlite3_column_int(stmt, 1);
        }
        if (linescnt) {
            *linescnt = sqlite3_column_int(stmt, 2);
        }
    }
    sqlite3_reset(stmt);
    return rowid;
}

long long debugdb_add_module (void*dbi,const char * name,  const char * source_name, const char *timestamp)
{
    debugdb_info *db_info= dbi; 
    char sql[1024];
    long long rowid;

    if (!db_info) {
        return 0;
    }
    rowid = debugdb_find_module(dbi,name,source_name,NULL, NULL, NULL);
    if (rowid) {
        sprintf (sql, "DELETE FROM modules WHERE [ROWID] = %lld;", rowid);
        if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK) {
            fprintf(stderr, "Error delete module in debugdb: %s\n", sqlite3_errmsg(db_info->db));
        }
        sprintf (sql, "DELETE FROM fields WHERE [moduleid] = %lld;", rowid);
        if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK) {
            fprintf(stderr, "Error delete fields in debugdb: %s\n", sqlite3_errmsg(db_info->db));
        }
        sprintf (sql, "DELETE FROM linemap WHERE [moduleid] = %lld;", rowid);
        if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK) {
            fprintf(stderr, "Error delete linemap in debugdb: %s\n", sqlite3_errmsg(db_info->db));
        }
        sprintf (sql, "DELETE FROM ccmap WHERE [moduleid] = %lld;", rowid);
        if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK) {
            fprintf(stderr, "Error delete ccmap in debugdb: %s\n", sqlite3_errmsg(db_info->db));
        }
        sprintf (sql, "DELETE FROM ccmaprun WHERE [moduleid] = %lld;", rowid);
        if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK) {
            fprintf(stderr, "Error delete ccmaprun in debugdb: %s\n", sqlite3_errmsg(db_info->db));
        }
    }

    sprintf (sql, "INSERT INTO modules ([name], [sourcepath], [timestamp]) VALUES ('%s', '%s', '%s');", name, source_name, timestamp);
    if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK){
        fprintf(stderr, "Error inserting module in debugdb: %s\n", sqlite3_errmsg(db_info->db));
        return 0;
    }
    return sqlite3_last_insert_rowid(db_info->db);
}

void debugdb_update_module (void*dbi,long long moduleid, int childcnt, int linescnt)
{
    debugdb_info *db_info= dbi; 
    char sql[1024];

    if (!db_info) {
        return ;
    }

    sprintf (sql, "UPDATE modules SET [fieldcnt] = %d , [linescnt] = %d WHERE ROWID = %lld;", childcnt, linescnt, moduleid);
    if (sqlite3_exec (db_info->db, sql, NULL, NULL, &db_info->errmsg) != SQLITE_OK){
        fprintf(stderr, "Error updating module in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
}

void 
debugdb_add_field_rec(void*dbi, int idx, long long moduleid, int fieldid, char * name, 
                                int occurs, char *redefine_name, int groupid, 
                                int childid, char *fullname, int storageid)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int res;

    if (!db_info) {
        return;
    }
    if (!db_info->insert_field) {
        if (sqlite3_prepare_v2(db_info->db," INSERT into fields ("
                                       "[idx], "                  /* 1 */
                                       "[id], "                   /* 2 */
                                       "[moduleid],"              /* 3 */
                                       "[name],"                  /* 4 */
                                       "[goupid], "               /* 5 */
                                       "[childgroupid], "         /* 6 */
                                       "[occurs], "               /* 7 */
                                       "[fullname], "             /* 8 */
                                       "[childcnt]) "             /* 9 */
                                       " VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9);", -1, &db_info->insert_field, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare insert field : %s\n", sqlite3_errmsg(db_info->db));
            return;
        }
    }

    stmt = db_info->insert_field;
    if (!stmt) {
        return ;
    }
    sqlite3_bind_int  (stmt, 1, idx);
    sqlite3_bind_int  (stmt, 2, fieldid);
    sqlite3_bind_int64(stmt, 3, moduleid);
    sqlite3_bind_text (stmt, 4, name, -1, NULL);
    sqlite3_bind_int  (stmt, 5, groupid);
    sqlite3_bind_int  (stmt, 6, childid);
    sqlite3_bind_int  (stmt, 7, occurs);
    sqlite3_bind_text (stmt, 8, fullname, -1, NULL);
    sqlite3_bind_int  (stmt, 9, storageid);
    res= sqlite3_step(stmt);
    if (res != SQLITE_DONE) {
        fprintf(stderr, "Error inserting field in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
    sqlite3_reset(stmt);
}

static int
debugdb_get_sourceid (debugdb_info *db_info, char * sourcepath, int create)
{
    long long rowid = 0;
    int  res;
    sqlite3_stmt * stmt;
    static char *lastpath = NULL;
    static int   lastid = 0;

    if (!sourcepath) {
        return 0;
    }

    if (lastpath) {
        if ( strcmp(sourcepath, lastpath) == 0) {
            return lastid;
        }
        free(lastpath);
        lastpath = NULL;
    }

    if (!db_info->find_sourceid) {
        if (sqlite3_prepare_v2(db_info->db," SELECT ROWID FROM sources "
                                       "WHERE [sourcepath] = ?1;"            /* 1 */
                                       , -1, &db_info->find_sourceid, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare find_sourceid : %s\n", sqlite3_errmsg(db_info->db));
            return 0;
        }
    }
    stmt = db_info->find_sourceid;
    if (!stmt) {
        return 0;
    }
    sqlite3_bind_text(stmt, 1, sourcepath, -1, NULL);
    res= sqlite3_step(stmt);
    if (res == SQLITE_ROW) {
        rowid =  sqlite3_column_int64(stmt, 0);
    }
    sqlite3_reset(stmt);
    if (!rowid && create) {
        if (!db_info->insert_sourceid) {
            if (sqlite3_prepare_v2(db_info->db," INSERT into sources ("
                                           "[sourcepath] )"            /* 1 */
                                           " VALUES (?1);", -1, &db_info->insert_sourceid, NULL) != SQLITE_OK) {
                fprintf(stderr, "Can't prepare insert_sourceid : %s\n", sqlite3_errmsg(db_info->db));
                return 0;
            }
        }
        stmt = db_info->insert_sourceid;
        if (!stmt) {
            return 0;
        }
        sqlite3_bind_text(stmt, 1, sourcepath, -1, NULL);
        res= sqlite3_step(stmt);
        if (res != SQLITE_DONE) {
            fprintf(stderr, "Error inserting sourceid in debugdb: %s\n", sqlite3_errmsg(db_info->db));
        } else {
            rowid = sqlite3_last_insert_rowid(db_info->db);
        }
        sqlite3_reset(stmt);
    }
    lastpath = strdup(sourcepath);
    lastid = rowid;
    return (int) rowid;

}

static char *
debugdb_get_sourcepath (debugdb_info *db_info, int sourceid )
{
    int  res;
    char *p;
    sqlite3_stmt * stmt;
    static char *lastpath = NULL;
    static int   lastid = 0;

    if (!sourceid) {        
        return NULL;
    }

    if (lastpath) {
        if (sourceid == lastid) {
            return lastpath;
        }
        free(lastpath);
        lastpath = NULL;
    }
    if (!db_info->find_sourcepath) {
        if (sqlite3_prepare_v2(db_info->db," SELECT [sourcepath]  FROM sources "
                                       "WHERE  ROWID = ?1;"            /* 1 */
                                       , -1, &db_info->find_sourcepath, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare find_sourcepath : %s\n", sqlite3_errmsg(db_info->db));
            return NULL;
        }
    }
    stmt = db_info->find_sourcepath;
    if (!stmt) {
        return NULL;
    }
    sqlite3_bind_int(stmt, 1, sourceid);
    res= sqlite3_step(stmt);
    if (res == SQLITE_ROW) {
        p =  (char*)sqlite3_column_text(stmt, 0);
        if (p) {
            lastid = sourceid;
            lastpath = strdup(p);
        } 
    }
    sqlite3_reset(stmt);
    
    return lastpath;

}

void 
debugdb_add_line_rec(void*dbi, long long moduleid, int idx, char * sourcepath, int line_nr, char * label, int is_section)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int sid;
    int res;

    if (!db_info) {
        return;
    }
    if (!db_info->insert_line) {
        if (sqlite3_prepare_v2(db_info->db," INSERT into linemap ("
                                       "[idx], "                  /* 1 */
                                       "[moduleid],"              /* 2 */
                                       "[linenr],"                /* 3 */
                                       "[label], "                /* 4 */
                                       "[issection],"             /* 5 */
                                       "[sourceid] ) "            /* 6 */
                                       " VALUES (?1, ?2, ?3, ?4, ?5, ?6);", -1, &db_info->insert_line, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare insert line : %s\n", sqlite3_errmsg(db_info->db));
            return;
        }
    }
    stmt = db_info->insert_line;
    if (!stmt) {
        return ;
    }
    sid = debugdb_get_sourceid (db_info, sourcepath, 1);

    sqlite3_bind_int  (stmt, 1, idx);
    sqlite3_bind_int64(stmt, 2, moduleid);
    sqlite3_bind_int  (stmt, 3, line_nr);
    sqlite3_bind_text (stmt, 4, label, -1, NULL);
    sqlite3_bind_int  (stmt, 5, is_section);
    sqlite3_bind_int  (stmt, 6, sid);
    res= sqlite3_step(stmt);
    if (res != SQLITE_DONE) {
        fprintf(stderr, "Error inserting line in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
    sqlite3_reset(stmt);
}

int debugdb_find_field (void*dbi,long long moduleid, const char * name, int *occurs, char *fullname)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int res;
    char *p;
    int fieldid = 0;

    if (!db_info) {
        return 0;
    }
    if (!db_info->find_field) {
        if (sqlite3_prepare_v2(db_info->db," SELECT [id], [fullname], [occurs] FROM fields WHERE "
                                           "[moduleid] = ?1 AND "           /* 1 */
                                           "[fullname] like ?2 AND "           /* 2 */
                                           "[id] > 0  ; "
                               , -1, &db_info->find_field, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare find field : %s\n", sqlite3_errmsg(db_info->db));
            return 0;
        }                                    /*       0      1      2        3          4               5       */
    }

    stmt = db_info->find_field;
    if (!stmt) {
        return 0;
    }
    sqlite3_bind_int64(stmt, 1, moduleid);
    sqlite3_bind_text (stmt, 2, name, -1, NULL);

    res= sqlite3_step(stmt);
    if (res == SQLITE_ROW) {
        fieldid =  sqlite3_column_int(stmt, 0);
        p = (char *)sqlite3_column_text(stmt, 1);
        if (p) {
            strcpy (fullname, (char*)p);
        } else {
            strcpy (fullname, name);
        }
        *occurs =  sqlite3_column_int(stmt, 2);
    } 
    sqlite3_reset(stmt);
    return fieldid;
}


void debugdb_start_listfield (void*dbi,long long moduleid)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;

    if (!db_info) {
        return ;
    }
    if (!db_info->find_all_field) {
        if (sqlite3_prepare_v2(db_info->db," SELECT [id], [name], [occurs], [goupid], [childgroupid], [idx] FROM fields WHERE "
                                           "[moduleid] = ?1  ORDER BY [idx] ; "
                               , -1, &db_info->find_all_field, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare list field : %s\n", sqlite3_errmsg(db_info->db));
            return;
        }                                    /*       0      1         2        3          4               5       */
    }
    stmt = db_info->find_all_field;
    if (!stmt) {
        return ;
    }
    sqlite3_bind_int64(stmt, 1, moduleid);
}

int debugdb_listfield (void*dbi, int *occurs, char *name, int * groupid, int * childidx, int * idx)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int fieldid = 0;
    int res;
    char *p;

    if (!db_info) {
        return 0;
    }
    stmt = db_info->find_all_field;
    if (!stmt) {
        return 0;
    }
    res= sqlite3_step(stmt);
    if (res == SQLITE_ROW) {
        fieldid =  sqlite3_column_int(stmt, 0);
        p = (char *)sqlite3_column_text(stmt, 1);
        if (p) {
            strcpy (name, (char*)p);
        } 
        *occurs =  sqlite3_column_int(stmt, 2);
        *groupid =  sqlite3_column_int(stmt, 3);
        *childidx =  sqlite3_column_int(stmt, 4);
        *idx =  sqlite3_column_int(stmt, 5);
    }

    return fieldid;
}

int debugdb_reset_listfield (void*dbi)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;

    if (!db_info) {
        return 0;
    }
    stmt = db_info->find_all_field;
    if (!stmt) {
        return 0;
    }

    sqlite3_reset(stmt);
    return 0;
}

void debugdb_start_listlines (void*dbi,long long moduleid)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;

    if (!db_info) {
        return ;
    }
    if (!db_info->find_all_lines) {
        if (sqlite3_prepare_v2(db_info->db," SELECT [idx], [linenr], [label], [issection], [sourceid] FROM linemap WHERE "
                                           "[moduleid] = ?1  ORDER BY [idx] ; "
                               , -1, &db_info->find_all_lines, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare list lines : %s\n", sqlite3_errmsg(db_info->db));
            return;
        }
    }
    stmt = db_info->find_all_lines;
    sqlite3_bind_int64(stmt, 1, moduleid);
}

int debugdb_listlines (void*dbi, int *idx, int *linenr, char * label, int * issection, char ** sourcepath)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int res;
    int sid;
    char *p;

    if (!db_info) {
        return 0;
    }
    stmt = db_info->find_all_lines;
    if (!stmt) {
        return 0;
    }
    res= sqlite3_step(stmt);
    if (res == SQLITE_ROW) {
        *idx =  sqlite3_column_int(stmt, 0);
        *linenr =  sqlite3_column_int(stmt, 1);
        p = (char *)sqlite3_column_text(stmt, 2);
        if (p) {
            strcpy (label, (char*)p);
        } else {
            label[0] = 0; 
        }
        *issection =  sqlite3_column_int(stmt, 3);
        sid = sqlite3_column_int(stmt, 4);
        *sourcepath = debugdb_get_sourcepath(db_info,sid);
    } else {
        *linenr = 0;
        *idx = 0;
        sourcepath = NULL;
    }

    return *linenr;
}

int debugdb_reset_listlines (void*dbi)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;

    if (!db_info) {
        return 0;
    }
    stmt = db_info->find_all_lines;
    if (!stmt) {
        return 0;
    }
    sqlite3_reset(stmt);
    return 0;
}

void 
debugdb_add_ccmap_rec(void*dbi, long long moduleid, int idx, char * sourcepath, int line_nr,  char type)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int sid;
    int res;

    if (!db_info) {
        return;
    }
    if (!db_info->insert_ccmap) {
        if (sqlite3_prepare_v2(db_info->db," INSERT into ccmap ("
                                       "[idx], "                  /* 1 */
                                       "[moduleid],"              /* 2 */
                                       "[linenr],"                /* 3 */
                                       "[type],"                  /* 4 */
                                       "[sourceid] ) "            /* 5 */
                                       " VALUES (?1, ?2, ?3, ?4, ?5);", -1, &db_info->insert_ccmap, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare insert line : %s\n", sqlite3_errmsg(db_info->db));
            return;
        }
    }
    stmt = db_info->insert_ccmap;
    if (!stmt) {
        return ;
    }
    sid = debugdb_get_sourceid (db_info, sourcepath, 1);

    sqlite3_bind_int  (stmt, 1, idx);
    sqlite3_bind_int64(stmt, 2, moduleid);
    sqlite3_bind_int  (stmt, 3, line_nr);
    sqlite3_bind_text (stmt, 4, &type, 1, NULL);
    sqlite3_bind_int  (stmt, 5, sid);
    res= sqlite3_step(stmt);
    if (res != SQLITE_DONE) {
        fprintf(stderr, "Error inserting ccmap in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
    sqlite3_reset(stmt);
}

void 
debugdb_add_ccmaprun_rec(void*dbi, long long moduleid, int * data , int datasize)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int res;

    if (!db_info) {
        return;
    }
    if (!db_info->insert_ccmaprun) {
        if (sqlite3_prepare_v2(db_info->db," INSERT into ccmaprun ("
                               "[moduleid],"              /* 1 */
                               "[idxstart],"              /* 2 */
                               "[idxcnt],"                /* 3 */
                               "[ccmapruntime],"             /* 4 */
                               "[data] ) "                /* 5 */
                               " VALUES (?1, ?2, ?3, datetime('now'), ?4);", -1, &db_info->insert_ccmaprun, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare add ccmap run : %s\n", sqlite3_errmsg(db_info->db));
            return;
        }
    }
    stmt = db_info->insert_ccmaprun;
    if (!stmt) {
        return ;
    }
 
    sqlite3_bind_int64(stmt, 1, moduleid);
    sqlite3_bind_int  (stmt, 2, 0);
    sqlite3_bind_int  (stmt, 3, datasize / sizeof(int));
    sqlite3_bind_blob (stmt, 4, data, datasize, SQLITE_STATIC);
    res= sqlite3_step(stmt);
    if (res != SQLITE_DONE) {
        fprintf(stderr, "Error insert ccmap run in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
    sqlite3_reset(stmt);
}

void 
debugdb_update_ccmap_rec(void*dbi, long long moduleid, int idx, int cnt)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    int res;

    if (!db_info) {
        return;
    }
    if (!db_info->update_ccmap) {
        if (sqlite3_prepare_v2(db_info->db," UPDATE ccmap SET [execcnt]  = [execcnt] + ?1 WHERE [moduleid] = ?2 AND [idx] = ?3; "
                                       , -1, &db_info->update_ccmap, NULL) != SQLITE_OK) {
            fprintf(stderr, "Can't prepare update ccmap : %s\n", sqlite3_errmsg(db_info->db));
            return;
        }
    }
    stmt = db_info->update_ccmap;
    if (!stmt) {
        return ;
    }
 
    sqlite3_bind_int  (stmt, 1, cnt);
    sqlite3_bind_int64(stmt, 2, moduleid);
    sqlite3_bind_int  (stmt, 3, idx);
    res= sqlite3_step(stmt);
    if (res != SQLITE_DONE) {
        fprintf(stderr, "Error update ccmap in debugdb: %s\n", sqlite3_errmsg(db_info->db));
    }
    sqlite3_reset(stmt);
}

void 
debugdb_consolidate_ccmap(void*dbi, int verbose)
{
    debugdb_info *db_info= dbi; 
    sqlite3_stmt * stmt;
    sqlite3_stmt * stmtup;
    int res , resup;

    if (!db_info) {
        return ;
    }                                           /* 0              1                 2          3                 4                 5 */
    if (sqlite3_prepare_v2(db_info->db," SELECT [moduleid], [ccmapruntime], [data], [name], [sourcepath], [ccmaprunid] "
                                       "        FROM view_ccmaprun ; "
                           , -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Can't prepare select from view_ccmaprun : %s\n", sqlite3_errmsg(db_info->db));
        return;
    }
    if (sqlite3_prepare_v2(db_info->db," DELETE FROM ccmaprun WHERE ccmaprunid = ?1 ;"
                           , -1, &stmtup, NULL) != SQLITE_OK) {
        fprintf(stderr, "Can't prepare delete from ccmaprun   : %s\n", sqlite3_errmsg(db_info->db));
        return;
    }
    res= sqlite3_step(stmt);
    if (res != SQLITE_ROW) {
        fprintf(stderr, "No ccmaprun data. Did you run the progrem or cobccmap yet run.  \n");
    }
    while (res == SQLITE_ROW) {
        long long moduleid =  sqlite3_column_int64(stmt, 0);
        long long recid =  sqlite3_column_int64(stmt, 5);
        const int * data = sqlite3_column_blob(stmt, 2);
        int   datasize = sqlite3_column_bytes(stmt, 2) /sizeof(int);
        int i;
        
        if (verbose) {
            printf ("Modules : %s  Execution time : %s\n", sqlite3_column_text(stmt, 3), sqlite3_column_text(stmt, 1));
        }
        for (i = 0; i < datasize; i ++) {
            if (*data) {
                debugdb_update_ccmap_rec(dbi, moduleid, i, *data);
            }
            data++;
        }
        res= sqlite3_step(stmt);

        /* detete rec*/
        sqlite3_bind_int64(stmtup, 1, recid);
        resup= sqlite3_step(stmtup);
        if (resup != SQLITE_DONE) {
            fprintf(stderr, "Error delete from ccmaprun  debugdb: %s\n", sqlite3_errmsg(db_info->db));
        }
        sqlite3_reset(stmtup);


    }
    sqlite3_reset(stmt);



}


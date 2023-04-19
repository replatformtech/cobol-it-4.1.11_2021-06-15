/*
 * Copyright (C) 2003-2007 Keisuke Nishida
 * Copyright (C) 2007 Roger While
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libcob.h"
#include "cobc.h"

#undef CB_CONFIG_ANY
#undef CB_CONFIG_INT
#undef CB_CONFIG_STRING
#undef CB_CONFIG_BOOLEAN
#undef CB_CONFIG_SUPPORT
#undef CB_CONFIG_ANY_OPT
#undef CB_CONFIG_INT_OPT
#undef CB_CONFIG_STRING_OPT
#undef CB_CONFIG_BOOLEAN_OPT
#undef CB_CONFIG_SUPPORT_OPT
#define CB_CONFIG_ANY(type,var,name)    type var;
#define CB_CONFIG_INT(var,name)         int var=0;
#define CB_CONFIG_STRING(var,name)      char *var=NULL;
#define CB_CONFIG_BOOLEAN(var,name)     int var=0;
#define CB_CONFIG_SUPPORT(var,name)     enum cb_support var;

/*CIT*/
#define CB_CONFIG_ANY_OPT(type,var,name,default)    type var=default;
#define CB_CONFIG_INT_OPT(var,name,default)         int var=default;
#define CB_CONFIG_STRING_OPT(var,name,default)      char *var=default;
#define CB_CONFIG_BOOLEAN_OPT(var,name,default)     int var=default;
#define CB_CONFIG_SUPPORT_OPT(var,name,default)     enum cb_support var=default;
#include "config.def"

#undef CB_LONGOPT_BOOLEAN
#undef CB_LONGOPT_SET
#undef CB_LONGOPT_STRING
#undef CB_LONGOPT_OPT
#undef CB_LONGOPT_SHORT
#define CB_LONGOPT_BOOLEAN(name, val, var) int var=0;
#define CB_LONGOPT_SET(name, var, setval, setvar) int var=0;
#define CB_LONGOPT_STRING(name, val, var)  char *var=NULL;
#define CB_LONGOPT_OPT(name, val, var)  char *var=NULL;
#define CB_LONGOPT_SHORT(name, val, var)   char *var=NULL;
#include "longopt.def"

#undef CB_FLAG
#define CB_FLAG(var,name,doc,def) extern int var;
#include "flag.def"
#undef  CB_FLAG


enum cb_config_type {
    CB_ANY,
    CB_INT,                    /* integer */
    CB_STRING,                 /* "..." */
    CB_BOOLEAN,                /* 'yes', 'no' */
    CB_YES,                    /* 'yes' */
    CB_SUPPORT                 /* 'ok/yes', 'archaic', 'obsolete',
                               'skip', 'ignore', 'unconformable' */
};

struct noreserve        *norestab = NULL;
char                    *cb_cics_include = (char*)"CICS.CPY";
struct cb_file_status_map_list  *cb_file_status_map = NULL;
struct cb_file_status_map_list  *cb_crt_status_map = NULL;

/*CIT*/
struct staticlink *staticlinktab = NULL;

typedef struct {
    const enum cb_config_type       type;
    const char                      *name;
    void                            *var;
    char                            *val;
    int                             optional;
} config_table_type;
static config_table_type config_table[] = {
    {CB_STRING, "include", NULL, NULL, 1},
    {CB_STRING, "not-reserved", NULL, NULL, 1},
    {CB_STRING, "static-link", NULL, NULL, 1},
    {CB_STRING, "staticx-link", NULL, NULL, 1},
    {CB_STRING, "external-link", NULL, NULL, 1},
    {CB_STRING, "fstatus-map", NULL, NULL, 1},
    {CB_STRING, "crtstatus-map", NULL, NULL, 1},
    {CB_STRING, "exec-wrapper", NULL, NULL, 1},
    {CB_STRING, "include-once", NULL, NULL, 1},
    {CB_STRING, "cics-include", NULL, NULL, 1},
    {CB_STRING, "constant", NULL, NULL, 1},
    {CB_STRING, "makesyn", NULL, NULL, 1},
    {CB_STRING, "initcall", NULL, NULL, 1},
#undef CB_CONFIG_ANY
#undef CB_CONFIG_INT
#undef CB_CONFIG_STRING
#undef CB_CONFIG_BOOLEAN
#undef CB_CONFIG_SUPPORT
#undef CB_CONFIG_ANY_OPT
#undef CB_CONFIG_INT_OPT
#undef CB_CONFIG_STRING_OPT
#undef CB_CONFIG_BOOLEAN_OPT
#undef CB_CONFIG_SUPPORT_OPT
#define CB_CONFIG_ANY(type,var,name)    {CB_ANY, name, &var, NULL, 0},
#define CB_CONFIG_INT(var,name)         {CB_INT, name, &var, NULL, 0},
#define CB_CONFIG_STRING(var,name)      {CB_STRING, name, &var, NULL, 0},
#define CB_CONFIG_BOOLEAN(var,name)     {CB_BOOLEAN, name, &var, NULL, 0},
#define CB_CONFIG_SUPPORT(var,name)     {CB_SUPPORT, name, &var, NULL, 0},

/*CIT*/
#define CB_CONFIG_ANY_OPT(type,var,name,default){CB_ANY, name, &var, NULL, 1},
#define CB_CONFIG_INT_OPT(var,name,default)     {CB_INT, name, &var, NULL, 1},
#define CB_CONFIG_STRING_OPT(var,name,default)  {CB_STRING, name, &var, NULL, 1},
#define CB_CONFIG_BOOLEAN_OPT(var,name,default) {CB_BOOLEAN, name, &var, NULL, 1},
#define CB_CONFIG_SUPPORT_OPT(var,name,default) {CB_SUPPORT, name, &var, NULL, 1},
#include "config.def"

#undef CB_LONGOPT_BOOLEAN
#undef CB_LONGOPT_SET
#undef CB_LONGOPT_STRING
#undef CB_LONGOPT_OPT
#undef CB_LONGOPT_SHORT
#define CB_LONGOPT_BOOLEAN(name, val, var) {CB_YES, name, &var, NULL, 1}, 
#define CB_LONGOPT_SET(name, var, setval, setvar) {CB_YES, name, &var, NULL, 1},
#define CB_LONGOPT_STRING(name, val, var) {CB_STRING, name, &var, NULL, 1},
#define CB_LONGOPT_OPT(name, val, var) {CB_STRING, name, &var, NULL, 1},
#define CB_LONGOPT_SHORT(name, val, var) {CB_STRING, name, &var, NULL, 1},
#include "longopt.def"
#undef CB_WARNDEF
#define CB_WARNDEF(var, name, wall, doc, val) {CB_BOOLEAN, "W"name, &var, NULL, 1},
#include "warning.def"

#undef CB_FLAG
#define CB_FLAG(var,name,doc,def)       {CB_BOOLEAN, name, &var, NULL, 1},
#include "flag.def"
#undef  COB_EXCEPTION
#define COB_EXCEPTION(code,tag,name,critical) {CB_BOOLEAN, name, &cb_exception_table[tag].enable, NULL, 1},
#include "libcob/exception.def"
#undef  COB_EXCEPTION

    {0, NULL, NULL, NULL}
};
#define NUM_CONFIG_ENTRY      ((sizeof (config_table) / sizeof (config_table_type)) -1)

extern void enterprise_error (const char *msg);
#define TOK_SEP " =\t"
static int 
cobc_add_crtstatus_map (char*arg, struct cb_file_status_map_list **file_status_map) {
    int cit, cust;
    char                    *p;
    char                    *b;    
    struct cb_file_status_map_list *l;

    b = strdup(arg);
    p = strtok(b, TOK_SEP);
    if ( p ) {
        cit = atoi(p);
        p = strtok(NULL, TOK_SEP);
        if ( p ) {
            if ( *p =='x' || *p == 'X') {
                p++;
                sscanf(p, "%x", &cust);
                #ifndef WORDS_BIGENDIAN
                cust = COB_BSWAP_32(cust);
                #endif
            } else {
                cust = atoi(p);
            }
            if ( cit == cust ) {
                goto fmap_err;
            }
            l = cobc_malloc(sizeof(struct cb_file_status_map_list));
            l->cit_status.crt_status = cit;
            l->custome_status.crt_status = cust;
            l->org_map = strdup(arg);
            l->next = *file_status_map;
            *file_status_map = l;
        } else {
            goto fmap_err;
        }
    } else {
        goto fmap_err;
    }

    return 0;
    fmap_err:
    fprintf (stderr, "Invalid File crt status map directive : %s\n", arg);
    return -1;

}
static int
cobc_add_fstatus_map (char *arg, struct cb_file_status_map_list **file_status_map) {
    char                    *p;
    char                    *b;    
    char           *cit, *cust;
    struct cb_file_status_map_list *l;

    b = strdup(arg);
    p = strtok(b, TOK_SEP);

    if ( p ) {
        cit = strdup(p);
        p = strtok(NULL, TOK_SEP);
        if ( p ) {
            cust = strdup(p);

            if ( strcasecmp(cit,cust) == 0) {
                goto fmap_err;
            }
            l = cobc_malloc(sizeof(struct cb_file_status_map_list));
            l->cit_status.f_status[0] = cit[0];
            l->cit_status.f_status[1] = cit[1];
            l->custome_status.f_status[0] = cust[0];
            l->custome_status.f_status[1] = cust[1];
            l->org_map = strdup(arg);
            l->next = *file_status_map;
            *file_status_map = l;
        } else {
            goto fmap_err;
        }
    } else {
        goto fmap_err;
    }

    return 0;
    fmap_err:
    fprintf (stderr, "Invalid File status map directive : %s\n", arg);
    return -1;
}
static char *
read_string (const char *text)
{
    char    *p;
    char    *s = strdup (text);

    if ( *s == '\"' ) {
        s++;
    }
    for ( p = s; *p; p++ ) {
        if ( *p == '\"' ) {
            *p = '\0';
        }
    }
    return s;
}

int
cb_load_std (const char *name)
{
    char    fname[COB_SMALL_BUFF];

    sprintf (fname, "%s/%s.conf", cob_config_dir, name);
    return cb_load_conf (fname, 1, 1);
}

static int is_yes (const char *val) 
{
    return (strcasecmp (val, "yes") == 0) || (strcasecmp (val, "true") == 0) || (strcasecmp (val, "ok") == 0);
}

static int is_no (const char *val) 
{
    return (strcasecmp (val, "no") == 0) || (strcasecmp (val, "false") == 0);
}

int
cb_load_conf (const char *fname, int check_nodef, int complain)
{
    int                     i, j, ret, line;
    char                    *s, *e;
    const char              *name, *val;
    void                    *var;
    FILE                    *fp;
    char                    *nores;
    struct noreserve        *noresptr;
    struct staticlink       *statlinkptr;
    char                    f2name[COB_MEDIUM_BUFF];
    char                    buff[COB_MEDIUM_BUFF];
    char                    nfname[COB_SMALL_BUFF];

    /* initialize the config table */
    if ( check_nodef ) {
        for ( i = 0; config_table[i].name; i++ ) {
            config_table[i].val = NULL;
        }
    }

    /* open the config file */
    fp = fopen (fname, "r");
    if ( fp == NULL ) {
        sprintf (nfname, "%s/%s", cob_config_dir, fname);
        fp = fopen (nfname, "r");
        if ( fp == NULL ) {
            if ( complain ) {
                perror (fname);
            }
            return -1;
        }
    }

    /* read the config file */
    ret = 0;
    line = 0;
    while ( fgets (buff, COB_MEDIUM_BUFF, fp) ) {
        line++;

        /* skip comments */
        if ( buff[0] == '#' ) {
            continue;
        }

        /* skip blank lines */
        for ( s = buff; *s; s++ ) {
            if ( isgraph (*s) ) {
                break;
            }
        }
        if ( !*s ) {
            continue;
        }

        /* get the tag */
        s = strpbrk (buff, " \t:=");
        if ( !s ) {
            fprintf (stderr, "%s:%d: invalid line\n", fname, line);
            ret = -1;
            continue;
        }
        *s = 0;

        /* find the entry */
        for ( i = 0; config_table[i].name; i++ ) {
            if ( strcasecmp (buff, config_table[i].name) == 0 ) {
                break;
            }
        }
        if ( !config_table[i].name ) {
            fprintf (stderr, "%s:%d: unknown tag '%s'\n", fname, line, buff);
            ret = -1;
            continue;
        }

        /* get the value */
        for ( s++; *s && strchr (" \t:=", *s); s++ ) ;
        for ( e = s + strlen (s) - 1; e >= s && strchr (" \t\r\n", *e); e-- ) ;
        e[1] = 0;
        config_table[i].val = s;

        /* set the value */
        name = config_table[i].name;
        var = config_table[i].var;
        val = config_table[i].val;
        switch ( config_table[i].type ) {
            case CB_ANY:
                {
                    if ( strcasecmp (name, "assign-clause") == 0 ) {
                        /*if ( strcasecmp (val, "cobol2002") == 0 ) {
                            goto unsupported_value;
                        } else */
                        if ( strcasecmp (val, "mf") == 0 ) {
                            cb_assign_clause = CB_ASSIGN_MF;
                        } else if ( strcasecmp (val, "ibm") == 0 ) {
                            cb_assign_clause = CB_ASSIGN_IBM;
                        } else if ( strcasecmp (val, "external") == 0 ) {
                            cb_assign_clause = CB_ASSIGN_EXTERNAL;
                        } else if ( strcasecmp (val, "dynamic") == 0 ) {
                            cb_assign_clause = CB_ASSIGN_DYNAMIC;
                        } else {
                            goto invalid_value;
                        }
                    } else if ( strcasecmp (name, "binary-size") == 0 ) {
                        if ( strcmp (val, "2-4-8") == 0 ) {
                            cb_binary_size = CB_BINARY_SIZE_2_4_8;
                        } else if ( strcmp (val, "1-2-4-8") == 0 ) {
                            cb_binary_size = CB_BINARY_SIZE_1_2_4_8;
                        } else if ( strcmp (val, "1--8") == 0 ) {
                            cb_binary_size = CB_BINARY_SIZE_1__8;
                        } else {
                            goto invalid_value;
                        }
                    } else if ( strcasecmp (name, "binary-byteorder") == 0 ) {
                        if ( strcasecmp (val, "native") == 0 ) {
                            cb_binary_byteorder = CB_BYTEORDER_NATIVE;
                        } else if ( strcasecmp (val, "big-endian") == 0 ) {
                            cb_binary_byteorder = CB_BYTEORDER_BIG_ENDIAN;
                        } else {
                            goto invalid_value;
                        }
                    } else if ( strcasecmp (name, "comp5-byteorder") == 0 ) {
                        if ( strcasecmp (val, "native") == 0 ) {
                            cb_comp5_byteorder = CB_BYTEORDER_NATIVE;
                        } else if ( strcasecmp (val, "big-endian") == 0 ) {
                            cb_comp5_byteorder = CB_BYTEORDER_BIG_ENDIAN;
                        } else {
                            goto invalid_value;
                        }
                    } else if ( strcasecmp (name, "sticky-linkage") == 0 ) {
                        if ( is_no(val)) {
                            cb_sticky_linkage = CB_STICKY_NONE;
                        } else if ( strcasecmp (val, "fixed") == 0 ) {
                            cb_sticky_linkage = CB_STICKY_FIX;
                        } else if ( (strcasecmp (val, "variable") == 0) || (strcasecmp (val, "yes") == 0) || strcasecmp (val, "true") == 0 ) {
                            cb_sticky_linkage = CB_STICKY_VARIABLE;
                        } else {
                            goto invalid_value;
                        }
                    } else if ( strcasecmp (name, "move-picx-to-pic9") == 0 ) {
                        if ( strcasecmp (val, "cit") == 0 || (strcasecmp (val, "none") == 0) ) {
                            cb_move_xto9_mode = CB_MOVExTO9_NONE;
                        } else if ( strcasecmp (val, "iso") == 0 ) {
                            cb_move_xto9_mode = CB_MOVExTO9_ISO;
                        } else if ( strcasecmp (val, "mf50") == 0 ) {
                            cb_move_xto9_mode = CB_MOVExTO9_MF50;
                        } else if ( (strcasecmp (val, "mvs") == 0) ) {
                            cb_move_xto9_mode = CB_MOVExTO9_MVS;
                        } else if ( (strcasecmp (val, "raw") == 0) ) {
                            cb_move_xto9_mode = CB_MOVExTO9_RAW;
                        } else if ( (strcasecmp (val, "raw0") == 0) ) {
                            cb_move_xto9_mode = CB_MOVExTO9_RAW_0;
                        } else if ( (strcasecmp (val, "mf") == 0) || (strcasecmp (val, "mf40") == 0)) {
                            cb_move_xto9_mode = CB_MOVExTO9_MF40;
                        } else {
                            goto invalid_value;
                        }
                    } else if (strcasecmp (name, "move-spaces-to-displaynumeric") == 0 ) {
                        if ( is_no(val) || (strcasecmp (val, "error") == 0) ) {
                            cb_move_spaces_to_displaynumeric = CB_MOVEspTO9_ERROR;
                        } else if ( is_yes(val) || (strcasecmp (val, "zero") == 0) ) {
                            cb_move_spaces_to_displaynumeric = CB_MOVEspTO9_ZERO;
                        } else if ( strcasecmp (val, "mf50") == 0 || (strcasecmp (val, "space") == 0)) {
                            cb_move_spaces_to_displaynumeric = CB_MOVEspTO9_SPACE;
                        }  else {
                            goto invalid_value;
                        }
                    } else if (strcasecmp (name, "move-spaces-to-comp3") == 0 ) {
                        if ( is_no(val) || (strcasecmp (val, "error") == 0) ) {
                            cb_move_spaces_to_comp3 = CB_MOVEspTO9_ERROR;
                        } else if ( is_yes(val) || (strcasecmp (val, "zero") == 0) ) {
                            cb_move_spaces_to_comp3 = CB_MOVEspTO9_ZERO;
                        } else if ( (strcasecmp (val, "space") == 0)) {
                            cb_move_spaces_to_comp3 = CB_MOVEspTO9_SPACE;
                        }  else {
                            goto invalid_value;
                        }
                    } else if (strcasecmp (name, "move-high-low-to-displaynumeric") == 0 ) {
                        if ( (strcasecmp (val, "error") == 0) ) {
                            cb_move_high_low_to_displaynumeric = CB_MOVEhlTO9_ERROR;
                        } else if ( (strcasecmp (val, "zero") == 0) ) {
                            cb_move_high_low_to_displaynumeric = CB_MOVEhlTO9_ZERO;
                        } else if ( (strcasecmp (val, "value") == 0)) {
                            cb_move_high_low_to_displaynumeric = CB_MOVEhlTO9_VALUE;
                        }  else {
                            goto invalid_value;
                        }
                    }
                    break;
                }
            case CB_INT:
                {
                    for ( j = 0; val[j]; j++ ) {
                        if ( !isdigit (val[j]) ) {
                            goto invalid_value;
                        }
                    }
                    *((int *)var) = atoi (val);
                    if ( strcasecmp (name, "defaultbyte") == 0 ) {
                        cb_use_defaultbyte = 1;
                    }
                    if ( strcasecmp (name, "rtncode-size") == 0 ) {
                        j = *((int *)var);
                        if ( j != 2 && j != 4 && j != 8 ) {
                            goto invalid_value;
                        }
                    }
                    break;
                }
            case CB_STRING:
                {
                    val = read_string (val);

                    if ( strcasecmp (name, "include") == 0 ) {
                        /* include another conf file */
                        sprintf (f2name, "%s/%s", cob_config_dir, val);
                        if ( cb_load_conf (f2name, 0, 1) != 0 ) {
                            if ( cb_load_conf (val, 0, 1) != 0 ) {
                                return -1;
                            }
                        }
                    } else if ( strcasecmp (name, "not-reserved") == 0 ) {
                        nores = read_string (val);
                        noresptr =
                        (struct noreserve *)cobc_malloc (sizeof (struct noreserve));
                        noresptr->noresword = cobc_malloc (strlen (nores) + 1);
                        strcpy (noresptr->noresword, nores);
                        noresptr->next = norestab;
                        norestab = noresptr;
                    } else if ( strcasecmp (name, "constant") == 0 ) {
                        nores = read_string (val);
                        cobc_add_constant(nores, 0, 1);
                    } else if ( strcasecmp (name, "makesyn") == 0 ) {
                        nores = read_string (val);
                        cobc_add_constant(nores, 1, 1);                        
                    } else if ( strcasecmp (name, "static-link") == 0 ) {
                        nores = cb_encode_program_id(read_string (val));
                        statlinkptr = cobc_find_static_symb(nores);
                        if (!statlinkptr) {
                            statlinkptr = 
                            (struct staticlink *)cobc_malloc (sizeof (struct staticlink));
                            statlinkptr->staticlink_symb = cobc_malloc (strlen (nores) + 1);
                            strcpy (statlinkptr->staticlink_symb, nores);
                            statlinkptr->next = staticlinktab;
                            staticlinktab = statlinkptr;
                        }
                        statlinkptr->static_link = 1;
                    } else if ( strcasecmp (name, "external-link") == 0 ) {
                        nores = cb_encode_program_id(read_string (val));
                        statlinkptr = cobc_find_static_symb(nores);
                        if (!statlinkptr) {
                            statlinkptr =
                            (struct staticlink *)cobc_malloc (sizeof (struct staticlink));
                            statlinkptr->staticlink_symb = cobc_malloc (strlen (nores) + 1);
                            strcpy (statlinkptr->staticlink_symb, nores);
                            statlinkptr->next = staticlinktab;
                            staticlinktab = statlinkptr;
                        }
                        statlinkptr->external_link = 1;
                    } else if ( strcasecmp (name, "internal-link") == 0 ) {
                        nores = cb_encode_program_id(read_string (val));
                        statlinkptr = cobc_find_static_symb(nores);
                        if (!statlinkptr) {
                            statlinkptr =
                            (struct staticlink *)cobc_malloc (sizeof (struct staticlink));
                            statlinkptr->staticlink_symb = cobc_malloc (strlen (nores) + 1);
                            strcpy (statlinkptr->staticlink_symb, nores);
                            statlinkptr->next = staticlinktab;
                            staticlinktab = statlinkptr;
                        }
                        statlinkptr->external_link = -1;
                    } else if ( strcasecmp (name, "staticx-link") == 0 ) {
                        nores = cb_encode_program_id(read_string (val));
                        statlinkptr = cobc_find_static_symb(nores);
                        if (!statlinkptr) {
                            statlinkptr =
                            (struct staticlink *)cobc_malloc (sizeof (struct staticlink));
                            statlinkptr->staticlink_symb = cobc_malloc (strlen (nores) + 1);
                            strcpy (statlinkptr->staticlink_symb, nores);
                            statlinkptr->next = staticlinktab;
                            staticlinktab = statlinkptr;
                        }
                        statlinkptr->static_link = 1;
                        statlinkptr->external_link = 1;
                    } else if ( strcasecmp (name, "cics-include") == 0 ) {
                        cb_cics_include = strdup (val);
                        cb_once_list = cb_text_list_add(cb_once_list, cb_cics_include);
                    } else if ( strcasecmp (name, "include-once") == 0 ) {
                        cb_once_list = cb_text_list_add(cb_once_list, val);
                    } else if ( strcasecmp (name, "initcall") == 0 ) {
                        cb_initcall_list = cb_text_list_add(cb_initcall_list, val);
                    } else if ( strcasecmp (name, "fstatus-map") == 0 ) {
                        nores = read_string (val);
                        if ( cobc_add_fstatus_map(nores, &cb_file_status_map) < 0) {
                            return -1;
                        }
                    } else if ( strcasecmp (name, "crtstatus-map") == 0 ) {
                        nores = read_string (val);
                        if ( cobc_add_crtstatus_map(nores, &cb_crt_status_map) < 0) {
                            return -1;
                        }
                    } else if (var) {
                        *((const char **)var) = val;
                    }
                    break;
                }
            case CB_BOOLEAN:
                {
                    if ( is_yes(val)) {
                        *((int *)var) = 1;
                    } else if ( is_no(val)) {
                        *((int *)var) = 0;
                    } else {
                        goto invalid_value;
                    }
                    break;
                }
            case CB_YES:
                {
                    if ( is_yes(val)) {
                        *((int *)var) = 1;
                    } else {
                        goto invalid_value;
                    }
                    break;
                }
            case CB_SUPPORT:
                {
                    if ( is_yes(val)) {
                        *((enum cb_support *)var) = CB_OK;
                    } else if ( strcasecmp (val, "warning") == 0 ) {
                        *((enum cb_support *)var) = CB_WARNING;
                    } else if ( strcasecmp (val, "archaic") == 0 ) {
                        *((enum cb_support *)var) = CB_ARCHAIC;
                    } else if ( strcasecmp (val, "obsolete") == 0 ) {
                        *((enum cb_support *)var) = CB_OBSOLETE;
                    } else if ( strcasecmp (val, "skip") == 0 ) {
                        *((enum cb_support *)var) = CB_SKIP;
                    } else if ( strcasecmp (val, "ignore") == 0 ) {
                        *((enum cb_support *)var) = CB_IGNORE;
                    } else if ( (strcasecmp (val, "error") == 0) || is_no(val)) {
                        *((enum cb_support *)var) = CB_ERROR;
                    } else if ( strcasecmp (val, "unconformable") == 0 ) {
                        *((enum cb_support *)var) = CB_UNCONFORMABLE;
                    } else {
                        goto invalid_value;
                    }
                    break;
                }
                invalid_value:
                fprintf (stderr, _("%s:%d: invalid value for '%s' : '%s' \n"),
                         fname, line, name, val);
                ret = -1;
                break;
                /*
                unsupported_value:
                fprintf (stderr, _("%s:%d: '%s' not supported yet\n"),
                         fname, line, val);
                ret = -1;
                break; 
                */ 
        }   
    }
    fclose (fp);

    /* checks for no definition */
    if ( check_nodef ) {
        for ( i = 2; config_table[i].name; i++ ) {
            if ( config_table[i].val == NULL && !config_table[i].optional ) {
                fprintf (stderr, "%s: no definition of '%s'\n",
                         fname, config_table[i].name);
                ret = -1;
            }
        }
    }

    return ret;
}

struct staticlink * 
cobc_find_static_symb (const char * val) {
    struct staticlink *p = staticlinktab;
    while ( p ) {
        if ( strcasecmp(val, p->staticlink_symb) == 0 ) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

int cobc_confvalue (const char *value_name)
{
    int i;
    /* find the entry */
    for ( i = 0; config_table[i].name; i++ ) {
        if ( strcasecmp (value_name, config_table[i].name) == 0 &&
             (config_table[i].type == CB_INT || config_table[i].type == CB_BOOLEAN || config_table[i].type == CB_YES) && 
             config_table[i].var ) {
            return *((int*)(config_table[i].var));
        }
    }
    return 0;
}

static int
cmpconfig(const void *p1, const void *p2)
{
   return strcmp(((config_table_type*)p1)->name, ((config_table_type*)p2)->name);
}

int cobc_dump_config (FILE *f, int listing, char *prefix)
{
    int i;
    struct noreserve *nr;
    struct staticlink *sl;
    struct cb_file_status_map_list *sm;
    struct cb_constant_list *lc;

    /* find the entry */
    if (listing) {
        fprintf(f,"%s****************************************\n", prefix);
        fprintf(f,"%s********** CONFIGURATION FLAGS *********\n", prefix);
        fprintf(f,"%s****************************************\n", prefix);
    }
    qsort(config_table, NUM_CONFIG_ENTRY, sizeof(config_table_type), cmpconfig);
    for ( i = 0; config_table[i].name; i++ ) {
        if (strcasecmp (config_table[i].name, "name") == 0) {
            continue;
        }
        if (config_table[i].type == CB_BOOLEAN &&
            strncmp (config_table[i].name, "EC-", 3) == 0 &&
            config_table[i].var && 
            *((int*)(config_table[i].var)) == 0) {
            continue;
        }
        if (config_table[i].type == CB_STRING &&
            config_table[i].var && 
            *((char**)(config_table[i].var)) == NULL) {
            continue;
        }
        if (config_table[i].var) {
            if (listing) {
                fprintf(f,"%s* %s:", prefix, config_table[i].name);
            } else {
                fprintf(f,"%s:", config_table[i].name);        
            }
            switch (config_table[i].type) {
                case CB_ANY:
                    if ( strcasecmp (config_table[i].name, "assign-clause") == 0 ) {
                        switch (cb_assign_clause) {
                            case CB_ASSIGN_MF:            fprintf(f," mf\n"); break;
                            case CB_ASSIGN_IBM:           fprintf(f," ibm\n"); break;
                            case CB_ASSIGN_EXTERNAL:      fprintf(f," external\n"); break;
                            case CB_ASSIGN_DYNAMIC:       fprintf(f," dynamic\n"); break;
                            default:                      fprintf(f," %d\n", cb_assign_clause); break;
                        }
                    } else if ( strcasecmp (config_table[i].name, "binary-size") == 0 ) {
                        switch (cb_binary_size) {
                            case CB_BINARY_SIZE_2_4_8:    fprintf(f," 2-4-8\n");        break;
                            case CB_BINARY_SIZE_1_2_4_8:  fprintf(f," 1-2-4-8\n");      break;  
                            case CB_BINARY_SIZE_1__8:     fprintf(f," 1--8\n");         break;
                            default:                      fprintf(f," %d\n", cb_binary_size); break;
                        }
                    } else if ( strcasecmp (config_table[i].name, "binary-byteorder") == 0 ) {
                        if (cb_binary_byteorder == CB_BYTEORDER_NATIVE) {
                            fprintf(f," native\n");        
                        } else {
                            fprintf(f," big-endian\n");        
                        }
                    } else if ( strcasecmp (config_table[i].name, "comp5-byteorder") == 0 ) {
                        if (cb_comp5_byteorder == CB_BYTEORDER_NATIVE) {
                            fprintf(f," native\n");        
                        } else {
                            fprintf(f," big-endian\n");        
                        }
                    } else if ( strcasecmp (config_table[i].name, "sticky-linkage") == 0 ) {
                        switch (cb_sticky_linkage) {
                            case CB_STICKY_NONE:     fprintf(f," no\n");        break;
                            case CB_STICKY_FIX:      fprintf(f," fixed\n");     break;   
                            case CB_STICKY_VARIABLE: fprintf(f," variable\n");  break;      
                            default:                 fprintf(f," %d\n", cb_sticky_linkage); break;
                        }
                    } else if ( strcasecmp (config_table[i].name, "move-picx-to-pic9") == 0 ) {
                        switch (cb_move_xto9_mode) {
                            case CB_MOVExTO9_NONE:   fprintf(f," none\n");       break; 
                            case CB_MOVExTO9_MF50:   fprintf(f," mf50\n");       break;
                            case CB_MOVExTO9_MVS:    fprintf(f," mvs\n");        break;
                            case CB_MOVExTO9_RAW:    fprintf(f," raw\n");        break;
                            case CB_MOVExTO9_RAW_0:  fprintf(f," raw0\n");        break;
                            case CB_MOVExTO9_ISO:    fprintf(f," iso\n");        break;
                            case CB_MOVExTO9_MF40:   fprintf(f," mf40\n");       break;
                            default:                 fprintf(f," %d\n", cb_move_xto9_mode); break;
                        }
                    } else if ( strcasecmp (config_table[i].name, "move-spaces-to-displaynumeric") == 0 ) {
                        switch (cb_move_spaces_to_displaynumeric) {
                            case CB_MOVEspTO9_ERROR: fprintf(f," error\n");      break; 
                            case CB_MOVEspTO9_ZERO:  fprintf(f," zero\n");       break;
                            case CB_MOVEspTO9_SPACE: fprintf(f," space\n");      break;
                            default:                 fprintf(f," %d\n", cb_move_spaces_to_displaynumeric); break;
                        }
                    } else if ( strcasecmp (config_table[i].name, "move-spaces-to-comp3") == 0 ) {
                        switch (cb_move_spaces_to_comp3) {
                            case CB_MOVEspTO9_ERROR: fprintf(f," error\n");      break; 
                            case CB_MOVEspTO9_ZERO:  fprintf(f," zero\n");       break;
                            case CB_MOVEspTO9_SPACE: fprintf(f," space\n");      break;
                            default:                 fprintf(f," %d\n", cb_move_spaces_to_comp3); break;
                        }
                    } else if ( strcasecmp (config_table[i].name, "move-high-low-to-displaynumeric") == 0 ) {
                        switch (cb_move_high_low_to_displaynumeric) {
                            case CB_MOVEhlTO9_ERROR: fprintf(f," error\n");      break; 
                            case CB_MOVEhlTO9_ZERO:  fprintf(f," zero\n");       break;
                            case CB_MOVEhlTO9_VALUE: fprintf(f," value\n");      break;
                            default:                 fprintf(f," %d\n", cb_move_high_low_to_displaynumeric); break;
                        }
                    }
                    break;
                case CB_BOOLEAN:
                case CB_YES:
                    if (*((int*)(config_table[i].var))) {
                        fprintf(f," yes\n");
                    } else {
                        fprintf(f," no\n");
                    }
                    break;
                case CB_INT:
                    fprintf (f," %d\n", *((int*)(config_table[i].var)));
                    break;
                case CB_STRING:
                    fprintf (f," %s\n", *(char**)(config_table[i].var));
                    break;
                case CB_SUPPORT:
                    switch (*((int*)(config_table[i].var))) {
                        case CB_OK:         fprintf (f," Ok\n"); break;
                        case CB_WARNING:    fprintf (f," Warning\n"); break;
                        case CB_ARCHAIC:    fprintf (f," Archaic\n"); break;
                        case CB_OBSOLETE:   fprintf (f," Obsolete\n"); break;
                        case CB_SKIP:       fprintf (f," Skip\n"); break;
                        case CB_IGNORE:     fprintf (f," Ignore\n"); break;
                        case CB_ERROR:      fprintf (f," Error\n"); break;
                        case CB_UNCONFORMABLE:fprintf (f," Unconformable\n"); break;
                    }
                    break;
            }
        }
    }
    nr = norestab;
    while (nr) {
        if (listing) {
            fprintf(f,"%s* ", prefix);
        } 
        fprintf(f,"not-reserved:%s\n", nr->noresword);        
        nr = nr->next;
    }
    sl = staticlinktab;
    while (sl) {
        if (sl->static_link) {
            if (listing) {
                fprintf(f,"%s* ", prefix);
            } 
            fprintf(f,"static-link:%s\n", sl->staticlink_symb);
        }
        if (sl->external_link) {
            if (listing) {
                fprintf(f,"%s* ", prefix);
            }
            if (sl->external_link > 0) {
                fprintf(f,"external-link:%s\n", sl->staticlink_symb);
            } else {
                fprintf(f,"internal-link:%s\n", sl->staticlink_symb);
            }
        }
        sl = sl->next;
    }
    sm= cb_file_status_map;
    while (sm) {
        if (sm->org_map) {
            if (listing) {
                fprintf(f,"%s* ", prefix);
            } 
            fprintf(f,"fstatus-map:%s\n", sm->org_map);        
        }
        sm = sm->next;
    }
    sm= cb_crt_status_map;
    while (sm) {
        if (sm->org_map) {
            if (listing) {
                fprintf(f,"%s* ", prefix);
            } 
            fprintf(f,"crtstatus-map:%s\n", sm->org_map);        
        }
        sm = sm->next;
    }
    lc = cb_constants;
    while (lc) {
        if (lc->is_userconst && !lc->is_makesyn && lc->key && lc->value) {
            if (listing) {
                fprintf(f,"%s* ", prefix);
            } 
            fprintf(f,"constant:\"%s=%s\" \n", lc->key , lc->value);        
        }
        lc = lc->next;
    }
    lc = cb_constants;
    while (lc) {
        if (lc->is_userconst && lc->is_makesyn && lc->key && lc->value) {
            if (listing) {
                fprintf(f,"%s* ", prefix);
            } 
            fprintf(f,"makesyn:\"%s=%s\" \n", lc->key , lc->value);        
        }
        lc = lc->next;
    }

    return 0;
}

#include "stringutils.c"

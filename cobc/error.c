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
#include <stdarg.h>

#include <libcob.h>
#include "tree.h"
#include "cobc.h"

static void
print_error (char *file, int line, const char *prefix, const char *fmt, va_list ap)
{
    static struct cb_label *last_section = NULL;
    static struct cb_label *last_paragraph = NULL;
    char * name;

    file = file ? file : (char*)cb_lex_source_file.file_name;
    line = line ? line : cb_lex_source_file.line;

    /* print the paragraph or section name */
    if (current_section != last_section || current_paragraph != last_paragraph) {
        if (current_paragraph &&
            strcmp ((const char *)(current_paragraph->name), "MAIN PARAGRAPH")) {
            name = (char*)current_paragraph->orig_name;
            fprintf (stderr, _("%s: In paragraph '%s':\n"),
                     file, name);
        } else {
            /* Cobol-IT */
            if (current_section && 
                strcmp ((const char *)(current_section->name), "MAIN SECTION")) {
                name = (char*)current_section->orig_name;
                fprintf (stderr, _("%s: In section '%s':\n"),
                         file, name);
            }
        }
        last_section = current_section;
        last_paragraph = current_paragraph;
    }

    /* print the error */
    fprintf (stderr, "%s:%d: %s", file? file : "cobc", line, prefix);
    vfprintf (stderr, fmt, ap);
    fputs ("\n", stderr);
}

char *
check_filler_name (char *name)
{
    if (!memcmp (name, CB_PREFIX_FILLER, CB_PREFIX_FILLER_LEN)) {
        name = (char *)"FILLER";
    }
    return name;
}

int cb_disable_all_warning = 0;
void
cb_warning (const char *fmt, ...)
{
    va_list ap;

    if (!cb_disable_all_warning) {
        va_start (ap, fmt);
        if (current_statement) {
            print_error ((char *)current_statement->common.source_file, current_statement->common.source_line, 
                         "Warning: ", fmt, ap);
        } else {
            print_error (NULL, 0, "Warning: ", fmt, ap);
        }
        va_end (ap);

        warningcount++;
    }
}

void
cb_error (const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    if (current_statement) {
        print_error ((char *)current_statement->common.source_file, current_statement->common.source_line, 
                     "Error: ", fmt, ap);
    } else {
        print_error (NULL, 0, "Error: ", fmt, ap);
    }
    va_end (ap);

    errorcount++;

}

static void
cb_warning_x_va (cb_tree x, const char *fmt, va_list ap)
{  
    void * src  = NULL;
    int    line = 0;
    if (!x) {
        x = CB_TREE(current_statement);
    }
    if (x) {
        src  = x->source_file;
        line = x->source_line;
    }
    if (!cb_disable_all_warning) {
        print_error ((char *)src, line, "Warning: ", fmt, ap);
        warningcount++;
    }
}

void
cb_warning_x (cb_tree x, const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    cb_warning_x_va (x, fmt, ap);
    va_end (ap);
}

void
cb_warning_suggestion (cb_tree x, const char *fmt, ...)
{
    va_list ap;

    if (!cb_disable_all_warning && cb_warn_suggestion) {
        va_start (ap, fmt);
        print_error ((char *)(x->source_file), x->source_line, "Warning: ", fmt, ap);
        va_end (ap);

        warningcount++;
    }
}

void
cb_warning_information (cb_tree x, const char *fmt, ...)
{
    va_list ap;

    if (!cb_disable_all_warning && cb_warn_information) {
        va_start (ap, fmt);
        print_error ((char *)(x->source_file), x->source_line, "Warning: ", fmt, ap);
        va_end (ap);

        warningcount++;
    }
}

static void
cb_error_x_va (cb_tree x, const char *fmt, va_list ap)
{
    void * src  = NULL;
    int    line = 0;
    if (!x) {
        x = CB_TREE(current_statement);
    }
    if (x) {
        src  = x->source_file;
        line = x->source_line;
    }
    print_error ((char *)src, line, "Error: ", fmt, ap);
    errorcount++;
}

void
cb_error_x (cb_tree x, const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    cb_error_x_va (x, fmt, ap);
    va_end (ap);

    errorcount++;
}

int
cb_check_feature_x (cb_tree x, int tag, const char *fmt, ...)
{
    va_list ap;
    int res = 0;

    va_start (ap, fmt);
    switch ((enum cb_support) tag) {
        case CB_IGNORE:
        case CB_OK:
            res= 1; break;
        case CB_WARNING:
            cb_warning_x_va(x,fmt,ap);
            res= 1; 
            break;
        case CB_ARCHAIC:
        case CB_OBSOLETE:
        case CB_SKIP:
        case CB_ERROR:
        case CB_UNCONFORMABLE:
            cb_error_x_va(x,fmt,ap);
            break;
    }
    va_end (ap);
    return res;
}

int
cb_verify (enum cb_support tag, const char *feature)
{
    switch (tag) {
        case CB_OK:
            return 1;
        case CB_WARNING:
            return 1;
        case CB_ARCHAIC:
            if (cb_warn_archaic) {
                cb_warning (_("%s is archaic in %s"), feature, cb_config_name);
            }
            return 1;
        case CB_OBSOLETE:
            if (cb_warn_obsolete) {
                cb_warning (_("%s is obsolete in %s"), feature, cb_config_name);
            }
            return 1;
        case CB_SKIP:
            return 0;
        case CB_IGNORE:
            cb_warning (_("%s ignored"), feature);
            return 0;
        case CB_ERROR:
            return 0;
        case CB_UNCONFORMABLE:
            cb_error (_("%s does not conform to %s"), feature, cb_config_name);
            return 0;
    }
    return 0;
}

void
redefinition_error (cb_tree x)
{
    
    struct cb_word  *w;

    w = CB_REFERENCE (x)->word;
    /*
    cb_error_x (x, _("Redefinition of '%s'"), w->name);
    cb_error_x (CB_VALUE (w->items), _("-> '%s' previously defined here"), w->name); 
    */ 
    cb_error_x (x, _("Redefinition of '%s'"), cb_name(x));
    cb_error_x (CB_VALUE (w->items), _("-> '%s' previously defined here"), cb_name(x)); 
}

void
redefinition_warning (cb_tree x, cb_tree y)
{
    struct cb_word  *w;

    w = CB_REFERENCE (x)->word;
    cb_warning_x (x, _("Redefinition of '%s'"), cb_name(x));
    if (y) {
        cb_warning_x (y, _("-> '%s' previously defined here"), cb_name(x));
    } else {
        cb_warning_x (CB_VALUE (w->items), _("-> '%s' previously defined here"), cb_name(x));
    }
}

void
undefined_error (cb_tree x)
{
    struct cb_reference *r;
    char            buff[CB_MAX_CNAME];

    r = CB_REFERENCE (x);
    sprintf (buff, "'%s'", cb_name (x));
    /*
    for (c = r->chain; c; c = CB_REFERENCE (c)->chain) {
        strcat (buff, " in '");
        strcat (buff, cb_name (c));
        strcat (buff, "'");
    } 
    */
    if (r->cb_ref_optinal) {
        cb_warning_x (x, _("%s undefined"), buff);
    } else {
        cb_error_x (x, _("%s undefined"), buff);
    } 
}

void
typedef_error (cb_tree x, int typerequired)
{
    struct cb_reference *r;
    cb_tree         c;
    char            buff[CB_MAX_CNAME];

    r = CB_REFERENCE (x);
    sprintf (buff, "'%s'", CB_NAME (x));
    for (c = r->chain; c; c = CB_REFERENCE (c)->chain) {
        strcat (buff, " in '");
        strcat (buff, CB_NAME (c));
        strcat (buff, "'");
    }
    if ( typerequired ) {
        cb_error_x (x, _("%s is not a typedef"), buff);
    } else {
        cb_error_x (x, _("%s is typedef"), buff);
    }
}

void
ambiguous_error (cb_tree x)
{
    struct cb_word  *w;
    struct cb_field *p;
    struct cb_label *l2;
    cb_tree     l;
    cb_tree     y;
    char        buff[CB_MAX_CNAME];
    if (!x) {
        return;
    }
    w = CB_REFERENCE (x)->word;
    if (w->error == 0) {
        /* display error on the first time */
        /*
        sprintf (buff, "'%s'", CB_NAME (x));
        for (l = CB_REFERENCE (x)->chain; l; l = CB_REFERENCE (l)->chain) {
            strcat (buff, " in '");
            strcat (buff, CB_NAME (l));
            strcat (buff, "'");
        } 
        */ 
        sprintf (buff, "'%s'", cb_name (x));
        cb_error_x (x, _("%s ambiguous; need qualification"), buff);
        w->error = 1;

        /* display all fields with the same name */
        for (l = w->items; l; l = CB_CHAIN (l)) {
            y = CB_VALUE (l);
            if (CB_FIELD_P (y) && 
                CB_FIELD(y) -> flag_is_typedef) {
                continue;
            }
            sprintf (buff, "-> '%s' ", cb_name(y));
            switch (CB_TREE_TAG (y)) {
                case CB_TAG_FIELD:
                    for (p = CB_FIELD (y)->parent; p; p = p->parent) {
                        strcat (buff, "in '");
                        strcat (buff, p->name);
                        strcat (buff, "' ");
                    }
                    break;
                case CB_TAG_LABEL:
                    l2 = CB_LABEL (y);
                    if (l2->section) {
                        strcat (buff, "in '");
                        strcat (buff, cb_name((CB_TREE(l2->section))));
                        strcat (buff, "' ");
                    }
                    break;
                default:
                    break;
            }
            
            strcat (buff, _("defined here"));
            cb_error_x (y, buff);
        }
    }
}

void
group_error (cb_tree x, const char *clause)
{
    cb_error_x (x, _("Group item '%s' cannot have %s clause"), check_filler_name (cb_name (x)), clause);
}

void
level_redundant_error (cb_tree x, const char *clause)
{
    cb_error_x (x, _("Level %02d item '%s' cannot have %s clause"),
                cb_field (x)->level, check_filler_name (cb_name (x)), clause);
}

void
level_require_error (cb_tree x, const char *clause)
{
    cb_error_x (x, _("Level %02d item '%s' requires %s clause"),
                cb_field (x)->level, check_filler_name (cb_name (x)), clause);
}

void
level_except_error (cb_tree x, const char *clause)
{
    cb_error_x (x, _("Level %02d item '%s' cannot have other than %s clause"),
                cb_field (x)->level, check_filler_name (cb_name (x)), clause);
}

void 
enterprise_error (const char *msg)
{
    cb_error(_("COBOL-IT Enterprise extension : %s "), msg);
    fprintf (stderr, "  -> Enterprise extensions are not available with Community \n");
    fprintf (stderr, "  -> Please visit http://www.cobol-it.com or  \n");
    fprintf (stderr, "  ->        contact contact@cobol-it.com for more informations.\n");
    exit(-1);
}



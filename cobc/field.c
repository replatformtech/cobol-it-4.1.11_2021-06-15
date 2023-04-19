/*
 * Copyright (C) 2001-2007 Keisuke Nishida
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
#include <libcob.h>

#include "cobc.h"
#include "tree.h"

/* Global variables */

size_t          cb_needs_01 = 0;

/* Local variables */

static struct cb_field  *last_real_field = NULL;
static const int pic_digits[] = { 2, 4, 7, 9, 12, 14, 16, 18};

int
cb_get_level (cb_tree x) {
    const char  *p;
    const char  *name;
    int     level = 0;

    if ( CB_LITERAL_P(x) ) {
        name = (char*)CB_LITERAL(x)->data;
    } else {
        name = CB_NAME (x);
    }
    /* get level */
    for ( p = name; *p; p++ ) {
        if ( !isdigit (*p) ) {
            goto level_error;
        }
        level = level * 10 + (*p - '0');
    }

    /* check level */
    switch ( level ) {
        case 66:
        case 77:
        case 78:
        case 88:
            break;
        default:
            if ( level < 1 || level > 49 ) {
                goto level_error;
            }
            break;
    }

    return level;

    level_error:
    cb_error_x (x, _("Invalid level number '%s'"), name);
    return 0;
}


cb_tree
cb_build_field_tree (cb_tree level, cb_tree name,
                     struct cb_field *last_field,
                     enum cb_storage storage, struct cb_file *fn) {
    struct cb_reference *r;
    struct cb_field     *f;
    struct cb_field     *p;
    struct cb_field     *field_fill;
    cb_tree             dummy_fill;
    cb_tree             l;
    cb_tree             x;
    int                 lv;
    int                 last_field_level = 1;

    if ( level == cb_error_node || name == cb_error_node ) {
        return cb_error_node;
    }

    /* check the level number */
    lv = cb_get_level (level);
    if ( !lv ) {
        return cb_error_node;
    }
    if (lv == 1 || lv == 77) {
        if (level->source_column > 4) {
            cb_check_feature_x (level, cb_syntax_ibm5_2, "LEVEL 01/77 must be at COLUMN 8 to 11");
        }
    }
    /* build the field */
    r = CB_REFERENCE (name);
    f = CB_FIELD (cb_build_field (name));
    f->flag_sign_leading = current_program->flag_sign_leading ;
    f->flag_sign_separate= current_program->flag_sign_separate;
    f->storage = storage;
    last_real_field = last_field;
    if ( lv == 78 ) {
        f->level = 01;
        f->flag_item_78 = 1;
        return CB_TREE (f);
    } else {
        f->level = lv;
    }
    if ( f->level == 01 && storage == CB_STORAGE_FILE ) {
        if ( fn->external ) {
            f->flag_external = 1;
            has_external = 1;
        } else if ( fn->global ) {
            f->flag_is_global = 1;
        }
    }
    if ( last_field ) {
        if ( last_field->level == 77 && f->level != 01 &&
             f->level != 77 && f->level != 66 && f->level != 88 ) {
            cb_error_x (name, _("Level number must begin with 01 or 77"));
            return cb_error_node;
        }
    }

    /* checks for redefinition */
    if ( cb_warn_redefinition || cb_redefine_identifier != CB_OK) {
        if ( r->word->count > 1 ) {
            if ( f->level == 01 || f->level == 77 ) {
                if (cb_redefine_identifier == CB_ERROR) {
                    redefinition_error (name);
                } else {
                    redefinition_warning (name, NULL);
                }
            } else {
                for ( l = r->word->items; l; l = CB_CHAIN (l) ) {
                    x = CB_VALUE (l);
                    if ( !CB_FIELD_P (x)
                         || CB_FIELD (x)->level == 01
                         || CB_FIELD (x)->level == 77
                         || (f->level == last_field->level
                             && CB_FIELD (x)->parent == last_field->parent) ) {
                        if (cb_redefine_identifier == CB_ERROR) {
                            redefinition_error (name);
                        } else {
                            redefinition_warning (name, x);
                        }
                        break;
                    }
                }
            }
        }
    }

    if ( last_field && last_field->level == 88 ) {
        last_field = last_field->parent;
    }
    if ( last_field  ) {
        last_field_level = last_field->level;
        if (last_field_level == 77)
            last_field_level = 1;
    }
    /* link the field into the tree */
    if ( f->level == 01 || f->level == 77 ) {
        /* top level */
        cb_needs_01 = 0;
        if ( last_field ) {
            cb_field_add (cb_field_founder (last_field), f);
            /*
            p = cb_field_founder (last_field);
            p->sister = f;
            f->brother = p; 
            */ 
        }
    } else if ( !last_field || cb_needs_01 ) {
        /* invalid top level */
        cb_error_x (name, _("Level number must begin with 01 or 77"));
        return cb_error_node;
    } else if ( f->level == 66 ) {
        /* level 66 */
        f->parent = cb_field_founder (last_field);
        for ( p = f->parent->children; p && p->sister; p = p->sister ) ;
        if ( p ) {
            p->sister = f;
        }
    } else if ( f->level == 88 ) {
        /* level 88 */
        struct cb_field     *f88;

        f->parent = last_field;
        if (last_field->level88_children == NULL) {
            last_field->level88_children = f;
        } else {
            f88 = last_field->level88_children;
            while (f88->sister) {
                f88 = f88->sister;
            }
            f88->sister = f;
        }
    } else if ( f->level > last_field_level ) {
        /* lower level */
        last_field->children = f;
        f->parent = last_field;
    } else if ( f->level == last_field_level ) {
        /* same level */
        same_level:
        last_field->sister = f;
        f->parent = last_field->parent;
    } else {
        /* upper level */
        for ( p = last_field->parent; p; p = p->parent ) {
            if ( p->level == f->level ) {
                last_field = p;
                goto same_level;
            }
            if ( cb_relax_level_hierarchy && p->level < f->level ) {
                break;
            }
        }
        if ( p && cb_relax_level_hierarchy ) {
            dummy_fill = cb_build_filler ();
            field_fill = CB_FIELD (cb_build_field (dummy_fill));
            //cb_warning_x (name, _("No previous data item of level %02d"), f->level);
            field_fill->level = f->level;
            field_fill->storage = storage;
            field_fill->children = p->children;
            field_fill->parent = p;
            for ( p = p->children; p != NULL; p = p->sister ) {
                p->parent = field_fill;
            }
            field_fill->parent->children = field_fill;
            field_fill->sister = f;
            f->parent = field_fill->parent;
            last_field = field_fill;
        } else {
            cb_error_x (name, _("No previous data item of level %02d"), f->level);
            return cb_error_node;
        }
    }

    /* inherit parent's properties */
    if ( f->parent ) {
        f->usage = f->parent->usage;
        f->indexes = f->parent->indexes;
        f->flag_sign_leading = f->parent->flag_sign_leading;
        f->flag_sign_separate = f->parent->flag_sign_separate;
        f->flag_is_global = f->parent->flag_is_global;
        f->flag_synchronized = f->parent->flag_synchronized;
    }
    return CB_TREE (f);
}

struct cb_field *
cb_resolve_redefines (struct cb_field *field, cb_tree redefines) {
    struct cb_field     *f;
    struct cb_reference *r;
    const char      *name;
    cb_tree         x;

    r = CB_REFERENCE (redefines);
    name = CB_NAME (redefines);
    x = CB_TREE (field);

    /* check qualification */
    if ( r->chain ) {
        cb_error_x (x, _("'%s' cannot be qualified here"), name);
        return NULL;
    }

    /* check subscripts */
    if ( r->subs ) {
        cb_error_x (x, _("'%s' cannot be subscripted here"), name);
        return NULL;
    }

    /* resolve the name in the current group (if any) */
    if ( field->parent && field->parent->children ) {
        for ( f = field->parent->children; f; f = f->sister ) {
            if ( strcasecmp (f->name, name) == 0 ) {
                break;
            }
        }
        if ( f == NULL ) {
            cb_error_x (x, _("'%s' undefined in '%s'"), name, field->parent->name);
            return NULL;
        }
    } else {
        if ( cb_ref (redefines) == cb_error_node ) {
            return NULL;
        }
        f = cb_field (redefines);
    }

    /* check level number */
    if ( f->level != field->level ) {
        /*CIT*/
        if ( !((f->level == 77 && field->level==1) || (f->level == 1  && field->level==77)) ) {
            cb_error_x (x, _("Level number of REDEFINES entries must be identical (%d, %d)"), f->level, field->level);
            return NULL;
        }
    }
    if ( f->level == 66 || f->level == 88 ) {
        cb_error_x (x, _("Level number of REDEFINES entry cannot be 66 or 88"));
        return NULL;
    }

    if ( !cb_indirect_redefines && f->redefines ) {
        cb_error_x (x, _("'%s' not the original definition"), f->name);
        return NULL;
    }

    /* return the original definition */
    while ( f->redefines ) {
        f = f->redefines;
    }
    if ( f->usage == CB_USAGE_BIT && !f->children) {
        cb_error_x (x, _("'%s' is a bit field and may not be redefined"), f->name);
        return NULL;
    }

    f->flag_redefined = 1;
    if ( f->flag_usage_optimized ) {
        f->flag_usage_optimized = 0;
        f->usage = f->original_usage;
        cb_validate_field(f);
    }
    return f;
}

static int
validate_field_1 (struct cb_field *f) {
    cb_tree         x;
    cb_tree         l;
    char            *name;
    struct cb_field *p;
    char            *pp;
    unsigned char   *pstr;
    int             vorint;
    int             need_picture;
    char            pic[16];

    x = CB_TREE (f);
    name = cb_name (x);
    if ( f->flag_any_length ) {
        if ( f->storage != CB_STORAGE_LINKAGE ) {
            cb_error_x (x, _("'%s' ANY LENGTH only allowed in LINKAGE"), name);
            return -1;
        }
        if ( f->level != 01 ) {
            cb_error_x (x, _("'%s' ANY LENGTH must be 01 level"), name);
            return -1;
        }
        if ( f->flag_item_based || f->flag_external ) {
            cb_error_x (x, _("'%s' ANY LENGTH can not be BASED/EXTERNAL"), name);
            return -1;
        }
        if ( f->flag_occurs || f->occurs_depending ||
             f->children || f->values || f->flag_blank_zero ) {
            cb_error_x (x, _("'%s' ANY LENGTH has invalid definition"), name);
            return -1;
        }
        if ( !f->pic ) {
            cb_error_x (x, _("'%s' ANY LENGTH must have a PICTURE"), name);
            return -1;
        }
        if ( f->pic->size != 1 || f->usage != CB_USAGE_DISPLAY ) {
            cb_error_x (x, _("'%s' ANY LENGTH has invalid definition"), name);
            return -1;
        }
        f->count++;
        return 0;
    }

    if ( f->level == 77 ) {
        if ( f->storage != CB_STORAGE_WORKING &&
             f->storage != CB_STORAGE_LOCAL &&
             f->storage != CB_STORAGE_LINKAGE ) {
            cb_error_x (x, _("'%s' 77 level not allowed here"), name);
        }
    }
    if ( f->flag_external ) {
        if ( f->level != 01 && f->level != 77 ) {
            cb_error_x (x, _("'%s' EXTERNAL must be specified at 01/77 level"), name);
        }
        if ( f->storage != CB_STORAGE_WORKING &&
             f->storage != CB_STORAGE_FILE ) {
            cb_error_x (x, _("'%s' EXTERNAL can only be specified in WORKING-STORAGE section"),
                        name);
        }
        if ( f->flag_item_based ) {
            cb_error_x (x, _("'%s' EXTERNAL and BASED are mutually exclusive"), name);
        }
        if ( f->redefines ) {
            cb_error_x (x, _("'%s' EXTERNAL not allowed with REDEFINES"), name);
        }
    }
    if ( f->flag_item_based ) {
        if ( f->storage != CB_STORAGE_WORKING &&
             f->storage != CB_STORAGE_LOCAL &&
             f->storage != CB_STORAGE_LINKAGE ) {
            cb_error_x (x, _("'%s' BASED not allowed here"), name);
        }
        if ( f->redefines ) {
            cb_error_x (x, _("'%s' BASED not allowed with REDEFINES"), name);
        }
        if ( f->level != 01 && f->level != 77 ) {
            cb_error_x (x, _("'%s' BASED only allowed at the 01 and 77 levels"), name);
        }
    }
    if ( f->level == 66 ) {
        if ( !f->redefines ) {
            level_require_error (x, "RENAMES");
            return -1;
        }
        if ( f->flag_occurs ) {
            level_except_error (x, "RENAMES");
        }
        return 0;
    }

    /* validate OCCURS */
    if ( f->flag_occurs ) {
        if ( (!cb_verify (cb_top_level_occurs_clause, "01/77 OCCURS")
              && (f->level == 01 || f->level == 77))
             || (f->level == 66 || f->level == 88) ) {
            level_redundant_error (x, "OCCURS");
        }
        for ( l = f->index_list; l; l = CB_CHAIN (l) ) {
            cb_field (CB_VALUE (l))->flag_is_global = f->flag_is_global;
        }
    }

    /* validate OCCURS DEPENDING */
    if ( f->occurs_depending ) {
        if ( cb_flag_odo_slide ) {
            /* the data item that contains a OCCURS DEPENDING clause shall not
               be subordinate to a data item that has the OCCURS clause */
            for ( p = f->parent; p; p = p->parent ) {
                if ( p->flag_occurs ) {
                    cb_error_x (CB_TREE (p),
                                _("'%s' cannot have the OCCURS clause due to '%s'"),
                                check_filler_name ((char *)p->name),
                                check_filler_name (name));
                    break;
                }
            }
            /* the data item that contains a OCCURS DEPENDING clause must be
               the last data item in the group */
            for ( p = f; p->parent; p = p->parent ) {
                for ( ; p->sister; p = p->sister ) {
                    if ( p->sister == cb_field (f->occurs_depending) ) {
                        cb_error_x (x,
                                    _("'%s' ODO field item invalid here"),
                                    p->sister->name);
                    }
                    /*
                    if ( !p->sister->redefines ) {
                        if ( !cb_complex_odo ) {
                            cb_error_x (x,
                                        _("'%s' cannot have OCCURS DEPENDING"),
                                        check_filler_name (name));
                            break;
                        }
                    } 
                    */
                }
            }
        }
        /* If the field is GLOBAL, then the ODO must also be GLOBAL */
        if ( !cb_field (f->occurs_depending) ) {
            cb_error_x (x, _("ODO item must be a field"));
        } else {
            if ( f->flag_is_global ) {
                if ( !cb_field (f->occurs_depending)->flag_is_global ) {
                    cb_error_x (x, _("'%s' ODO item must have GLOBAL attribute"),
                                cb_field (f->occurs_depending)->name);
                }
                if ( f->storage != cb_field (f->occurs_depending)->storage ) {
                    cb_error_x (x, _("GLOBAL '%s' ODO item is not in the same section as OCCURS"),
                                cb_field (f->occurs_depending)->name);
                }
            }
            /* adjust min/max boundaries of the ON field */
            if (cb_field(f->occurs_depending)->is_odo_idx) {
                /* compare with previous values */
                if (f->occurs_min > cb_field(f->occurs_depending)->odo_idx_min) {
                   cb_field(f->occurs_depending)->odo_idx_min = f->occurs_min;
                }
                if (f->occurs_max < cb_field(f->occurs_depending)->odo_idx_max) {
                   cb_field(f->occurs_depending)->odo_idx_max = f->occurs_max;
                }
            } else {
                /* boundaries have not yet been set */
                cb_field(f->occurs_depending)->is_odo_idx = 1;
                cb_field(f->occurs_depending)->odo_idx_min = f->occurs_min;
                cb_field(f->occurs_depending)->odo_idx_max = f->occurs_max;
            }
        }
    }

    /* validate REDEFINES */
    if ( f->redefines ) {
        /* check OCCURS */
        if ( f->redefines->flag_occurs ) {
            cb_warning_suggestion ( x, _("The original definition '%s' should not have OCCURS"),
                                    f->redefines->name);
        }

        /* check definition */
        for ( p = f->redefines->sister; p && p != f; p = p->sister ) {
            if ( !p->redefines ) {
                cb_error_x (x, _("REDEFINES must follow the original definition"));
                break;
            }
        }

        /* check variable occurrence */

        /*        
        if ( f->occurs_depending || cb_field_variable_size (f) ) {
            cb_error_x (x, _("'%s' cannot be variable length"), f->name);
        } 
        */ 

        if (cb_flag_odo_slide && cb_field_variable_size (f->redefines) ) {
            cb_error_x (x,
                        _("The original definition '%s' cannot be variable length"),
                        f->redefines->name);
        }
    }

    if (f->screen_control) {
        if (CB_TREE_CLASS(f->screen_control) != CB_CLASS_ALPHANUMERIC) {
            cb_error_x (x, _("CONTROL field must be alphanumeric"));
        }
    }
    if (f->screen_input_size) {
        if (CB_TREE_CLASS(f->screen_input_size) != CB_CLASS_NUMERIC) {
            cb_error_x (x, _("CONTROL field must be numeric"));
        }
    }

    if ( f->children ) {
        /* group item */

        if ( f->pic ) {
            group_error (x, "PICTURE");
        }
        if ( f->flag_justified ) {
            group_error (x, "JUSTIFIED RIGHT");
        }
        if ( f->flag_synchronized && !cb_verify(cb_synchronized_clause_on_group,  "SYNC ON GROUP") ) {
            return -1;
        }
        if ( f->flag_blank_zero ) {
            group_error (x, "BLANK WHEN ZERO");
        }

        for ( f = f->children; f; f = f->sister ) {
            if ( validate_field_1 (f) != 0 ) {
                return -1;
            }
        }
    } else {
        /* elementary item */
        if (    f->usage == CB_USAGE_POINTER
                || f->usage == CB_USAGE_PROGRAM_POINTER) {
            f->flag_is_pointer = 1;
        }

        /* validate PICTURE */
        need_picture = 1;
        if ( f->usage == CB_USAGE_INDEX
             || f->usage == CB_USAGE_LENGTH
             || f->usage == CB_USAGE_OBJECT
             || f->usage == CB_USAGE_POINTER
             || f->usage == CB_USAGE_PROGRAM_POINTER
             || f->usage == CB_USAGE_FLOAT
             || f->usage == CB_USAGE_DOUBLE
             || f->usage == CB_USAGE_SIGNED_CHAR
             || f->usage == CB_USAGE_BYTE_ARRAY
             || f->usage == CB_USAGE_SIGNED_SHORT
             || f->usage == CB_USAGE_SIGNED_INT
             || f->usage == CB_USAGE_SIGNED_LONG
             || f->usage == CB_USAGE_UNSIGNED_CHAR
             || f->usage == CB_USAGE_UNSIGNED_SHORT
             || f->usage == CB_USAGE_UNSIGNED_INT
             || f->usage == CB_USAGE_UNSIGNED_LONG
             || f->usage == CB_USAGE_PROGRAM ) {
            need_picture = 0;
        }
        if ( f->pic == NULL && need_picture != 0 ) {
            if ( f->storage == CB_STORAGE_SCREEN ) {
/* RXW
                if (f->values &&
                    CB_LITERAL(CB_VALUE(f->values))->size) {
*/
                if ( f->values && CB_LITERAL_P(CB_VALUE(f->values))) {
                    sprintf (pic, "X(%d)", (int)CB_LITERAL(CB_VALUE(f->values))->size);
                } else {
                    sprintf (pic, "X(1)");
                }
                f->pic = CB_PICTURE (cb_build_picture (pic));
            } else if ( f->flag_item_78 && f->values &&
                        CB_VALUE(f->values) != cb_error_node ) {
                f->count++;
                if ( CB_NUMERIC_LITERAL_P(CB_VALUE(f->values)) ) {
                    memset (pic, 0, sizeof (pic));
                    pp = pic;
                    if ( CB_LITERAL(CB_VALUE(f->values))->sign ) {
                        *pp++ = 'S';
                    }
                    vorint = CB_LITERAL(CB_VALUE(f->values))->size -
                             CB_LITERAL(CB_VALUE(f->values))->scale;
                    if ( vorint ) {
                        pp += sprintf (pp, "9(%d)", vorint);
                    }
                    if ( CB_LITERAL(CB_VALUE(f->values))->scale ) {
                        sprintf (pp, "V9(%d)",
                                 CB_LITERAL(CB_VALUE(f->values))->scale);
                    }
                    if ( CB_LITERAL(CB_VALUE(f->values))->size < 10 ) {
                        f->usage = CB_USAGE_COMP_5;
                    } else {
                        f->usage = CB_USAGE_DISPLAY;
                    }
                    f->pic = CB_PICTURE (cb_build_picture (pic));
                    f->pic->category = CB_CATEGORY_NUMERIC;
                } else {
                    sprintf (pic, "X(%d)", (int)CB_LITERAL(CB_VALUE(f->values))->size);
                    f->pic = CB_PICTURE (cb_build_picture (pic));
                    f->pic->category = CB_CATEGORY_ALPHANUMERIC;
                    f->usage = CB_USAGE_DISPLAY;
                }
            } else {
                if ( f->flag_item_78 ) {
                    cb_error_x (x, _("Value required for constant item '%s'"), name);
                } else {
                    cb_error_x (x, _("PICTURE clause required for '%s'"), name);
                    /* just avoid crash later in compile*/
                    f->pic = CB_PICTURE (cb_build_picture ("X"));
                    f->pic->category = CB_CATEGORY_ALPHANUMERIC;
                    f->usage = CB_USAGE_DISPLAY;
                }
                return -1;
            }
        }
        if ( f->pic != NULL && need_picture == 0 ) {
            cb_error_x (x, _("'%s' cannot have PICTURE clause"), name);
        }

        /* validate USAGE */
        switch ( f->usage ) {
            case CB_USAGE_SIGNED_CHAR:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("S99"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_SIGNED_SHORT:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("S9(4)"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_SIGNED_INT:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("S9(9)"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_SIGNED_LONG:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("S9(18)"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_UNSIGNED_CHAR:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("99"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_UNSIGNED_SHORT:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("9(4)"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_UNSIGNED_INT:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("9(9)"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_UNSIGNED_LONG:
                f->usage = CB_USAGE_COMP_5;
                f->pic = CB_PICTURE (cb_build_picture ("9(18)"));
                f->flag_real_binary = 1;
                f->flag_binary_pack = 1;
                break;
            case CB_USAGE_BYTE_ARRAY:
                /* internal usage only*/
                sprintf (pic, "X(%d)", f->size);
                f->pic = CB_PICTURE (cb_build_picture (pic));
                f->pic->category = CB_CATEGORY_ALPHABETIC;
                f->usage = CB_USAGE_DISPLAY;
                break;
            case CB_USAGE_COMP_6:
            case CB_USAGE_PACKED:
                if ( f->pic->category != CB_CATEGORY_NUMERIC ) {
                    cb_error_x (x, _("'%s' PICTURE clause not compatible with USAGE"), name);
                }
                break;
            case CB_USAGE_BINARY:
                if ( (f->pic->category != CB_CATEGORY_NUMERIC) && 
                     (f->pic->category != CB_CATEGORY_BIT ) ) {
                    cb_error_x (x, _("'%s' PICTURE clause not compatible with USAGE"), name);
                }
                break;
            case CB_USAGE_COMP_5:
            case CB_USAGE_COMP_X:
                if ( f->pic ) {
                    if ( (f->pic->category != CB_CATEGORY_NUMERIC) &&
                         (f->pic->category != CB_CATEGORY_BIT )    &&
                         (f->pic->category != CB_CATEGORY_ALPHANUMERIC) ) {
                        cb_error_x (x, _("'%s' PICTURE clause not compatible with USAGE"), name);
                    }
                }
                break;
            default:
                break;
        }

        /* Try optimize level 77*/
        if ( cb_flag_77_optimize ) {
            if ( f->level == 77           &&
                 (f->storage == CB_STORAGE_WORKING || f->storage == CB_STORAGE_LOCAL) &&
                 !f->redefines            &&
                 !f->flag_redefined       &&
                 !f->flag_external        &&    
                 !f->flag_blank_zero      &&  
                 !f->flag_justified       &&  
                 !f->flag_sign_leading    &&  
                 !f->flag_sign_separate   &&  
                 (f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_DISPLAY) &&
                 f->occurs_max == 1       &&
                 f->pic                   && 
                 f->pic->scale == 0       &&
                 f->pic->category == CB_CATEGORY_NUMERIC && 
                 cb_fits_long_long(x) ) {
                f->flag_usage_optimized = 1;
                f->original_usage = f->usage;
                f->usage = CB_USAGE_COMP_5;
            }
        }

        /* validate SIGN */

        if ( f->usage == CB_USAGE_COMP_6 ) {
            if ( f->usage == CB_USAGE_COMP_6 && f->pic->have_sign ) {
                if ( cb_flag_signed_comp6_2_comp3 && cb_flag_ignore_comp6_sign ) {
                    cb_error_x (x, _("signed-comp6-as-comp3 and accept-but-ignore-comp6-signed configuration flags are mutually exclusive."));
                }
                if ( cb_flag_ignore_comp6_sign ) {
                    f->pic->have_sign = 0;
                } else if ( cb_flag_signed_comp6_2_comp3 ) {
                    f->usage = CB_USAGE_PACKED;
                    cb_warning_information(x, _("'%s' COMP-6 with signed picture converted to COMP-3"), name);
                } else {
                    cb_error_x (x, _("'%s' COMP-6 may not have sign"), name);
                }
            }
        }

        /* validate JUSTIFIED RIGHT */
        if ( f->flag_justified ) {
            switch ( f->pic->category ) {
                case CB_CATEGORY_ALPHABETIC:
                case CB_CATEGORY_ALPHANUMERIC:
                    break;
                default:
                    cb_error_x (x, _("'%s' cannot have JUSTIFIED RIGHT"), name);
                    break;
            }
        }

        /* validate SYNCHRONIZED */

        /* validate BLANK ZERO */
        if ( f->flag_blank_zero ) {
            switch ( f->pic->category ) {
                case CB_CATEGORY_NUMERIC:
                    /* reconstruct the picture string */
                    if ( f->pic->scale > 0 ) {
                        f->pic->str = cobc_malloc (20);
                        pstr = (unsigned char *)(f->pic->str);
                        *pstr++ = '9';
                        vorint = f->pic->digits - f->pic->scale;
                        memcpy (pstr, (unsigned char *)&vorint, sizeof(int));
                        pstr += sizeof(int);
                        *pstr++ = 'V';
                        vorint = 1;
                        memcpy (pstr, (unsigned char *)&vorint, sizeof(int));
                        pstr += sizeof(int);
                        *pstr++ = '9';
                        vorint = f->pic->scale;
                        memcpy (pstr, (unsigned char *)&vorint, sizeof(int));
                        /* ??? f->pic->size++; */
                    } else {
                        f->pic->str = cobc_malloc (8);
                        pstr = (unsigned char *)(f->pic->str);
                        *pstr++ = '9';
                        vorint = f->pic->digits;
                        memcpy (pstr, (unsigned char *)&vorint, sizeof(int));
                    }
                    f->pic->category = CB_CATEGORY_NUMERIC_EDITED;
                    break;
                case CB_CATEGORY_NUMERIC_EDITED:
                    break;
                default:
                    cb_error_x (x, _("'%s' cannot have BLANK WHEN ZERO"), name);
                    break;
            }
        }

        /* validate VALUE */
        if ( f->values ) {
            if ( CB_PAIR_P (CB_VALUE (f->values)) || CB_CHAIN (f->values) ) {
                cb_error_x (x, _("Only level 88 item may have multiple values"));
            }

            /* ISO+IEC+1989-2002: 13.16.42.2-10 */
            for ( p = f; p; p = p->parent ) {
                if ( p->redefines ) {
                    /* COBOL-IT*/
                    /* hack for MF compatibility */
                    if ( cb_relaxed_syntax_check ) {
                        cb_warning_suggestion (x, _("Entries under REDEFINES should not have VALUE clause"));
                    } else {
                        cb_error_x (x, _("Entries under REDEFINES cannot have VALUE clause"));
                    }
                }
                /*
                if ( p->flag_external ) {
                    cb_warning_information (x, _("VALUE clause ignored for EXTERNAL items"));
                } 
                */
            }
        }
    }

    return 0;
}

static void
setup_parameters (struct cb_field *f) {
    int flag_local;
    int action_code;
    char    pic[8];

    /* determine the class */
    if ( f->children ) {
        action_code = f->set_action_code;
        /* group field */
        flag_local = f->flag_local;
        for ( f = f->children; f; f = f->sister ) {
            f->flag_local = flag_local;
            f->set_action_code = action_code;
            setup_parameters (f);
        }
    } else {
        /* regular field */
        switch ( f->usage ) {
            case CB_USAGE_BINARY:
#ifndef WORDS_BIGENDIAN
                if ( cb_flag_binary_byteorder_big_endian 
                     || ((cb_binary_byteorder == CB_BYTEORDER_BIG_ENDIAN) 
                     && !cb_flag_binary_byteorder_native) ) {
                    f->flag_binary_swap = 1;
                }
#endif
                break;

            case CB_USAGE_INDEX:
                f->pic = CB_PICTURE (cb_build_picture ("S9(9)"));
                break;

            case CB_USAGE_LENGTH:
                f->pic = CB_PICTURE (cb_build_picture ("9(9)"));
                break;

            case CB_USAGE_POINTER:
            case CB_USAGE_PROGRAM_POINTER:
                f->pic = CB_PICTURE (cb_build_picture ("9(10)"));
                break;
            case CB_USAGE_FLOAT:
                f->pic = CB_PICTURE (cb_build_picture ("S9(7)V9(7)"));
                break;
            case CB_USAGE_DOUBLE:
                f->pic = CB_PICTURE (cb_build_picture ("S9(9)V9(9)"));
                break;

            case CB_USAGE_COMP_5:
            case CB_USAGE_COMP_X:
                if ( f->pic->category == CB_CATEGORY_ALPHANUMERIC ) {
                    if ( f->pic->size > 8 ) {
                        sprintf (pic, "9(36)");
                    } else {
                        sprintf (pic, "9(%d)", pic_digits[f->pic->size - 1]);
                    }
                    f->flag_compx_notrunc=1;
                    f->pic = CB_PICTURE (cb_build_picture (pic));
                }
                if (cb_flag_compute_ibm) {
                    f->flag_compx_notrunc=1;
                }
#ifndef WORDS_BIGENDIAN
                if ( f->usage == CB_USAGE_COMP_X ) {
                    if ( cb_flag_binary_byteorder_big_endian 
                     || ((cb_binary_byteorder == CB_BYTEORDER_BIG_ENDIAN) 
                     && !cb_flag_binary_byteorder_native) ) {
                        f->flag_binary_swap = 1;
                    }
                } else {
                    if ( cb_comp5_byteorder == CB_BYTEORDER_BIG_ENDIAN ) {
                        f->flag_binary_swap = 1;
                    }
                }
#endif
                break;

            default:
                break;
        }
    }
}

static int 
get_sync_align_size(int field_size)
{
    int align_size = 1;
    switch (field_size) {
        case 0:
        case 1: align_size = 1; break;                                
        case 2: align_size = 2; break;
        case 3:
        case 4: align_size = 4; break;
        default: 
            if (cb_synchronized_double_word) {
                align_size = 8 ;
            } else {
                align_size = 4; 
            }
            break;
    }
    return align_size;

}

static int 
compute_sync_align_size (struct cb_field *f) {
    int align_size = 1;
    struct cb_field *c = f;
    while (c) {
        if (!c->redefines && c->flag_synchronized
            && cb_verify (cb_synchronized_clause, "SYNC")) {
            if (c->children && cb_verify (cb_synchronized_group_align_size, "SYNC ON GROUP SIZE")) {
                align_size =get_sync_align_size(c->size);
            } else {
                switch ( c->usage ) {
                    case CB_USAGE_BINARY:
                    case CB_USAGE_COMP_5:
                    case CB_USAGE_COMP_X:
                    case CB_USAGE_FLOAT:
                    case CB_USAGE_DOUBLE:
                        align_size =get_sync_align_size(c->size);
                        break;
                    case CB_USAGE_INDEX:
                    case CB_USAGE_LENGTH:
                        align_size =get_sync_align_size(sizeof (int));
                        break;
                    case CB_USAGE_OBJECT:
                    case CB_USAGE_POINTER:
                    case CB_USAGE_PROGRAM_POINTER:
                    case CB_USAGE_PROGRAM:
                        align_size =get_sync_align_size(sizeof (void *));
                        break;
                    default:
                        break;
                }
            }
            /*printf("allign %d byte %d\n",align_size, c->flag_synchronized);*/
        }
        c = c->children;
    }
    return align_size;
}

static int 
compute_sync_padding (struct cb_field *f) {
    int align_size = 1;
    int pad = 0;
    align_size = compute_sync_align_size(f);
    if ( align_size > 1 ) {
        /*printf("allign %d byte\n",align_size);*/
        if ( f->offset % align_size != 0 ) {
            int newpad = align_size - (f->offset % align_size);
            if (newpad > pad ) {
                pad = newpad;
            }
            /*c->offset += pad;*/
        }
    }
    return pad;
}

static int 
compute_child_sync_max_align_size (struct cb_field *f)
{
    struct cb_field *c;
    int maxsz = 0;
    int sz;
    for ( c = f->children; c; c = c->sister ) {
        if (!c->redefines) {
            if (c->children) {
                sz =compute_child_sync_max_align_size(c);
                if (sz > maxsz)
                    maxsz=sz;
            }
            if (c->flag_synchronized) {
                sz = compute_sync_align_size(c);
                if (sz > maxsz)
                    maxsz=sz;
            }
        }
    }
    return maxsz;
}

static int
compute_size (struct cb_field *f) {
    struct cb_field *c;
    int     size;
    enum cb_binary_size field_binary_size = cb_binary_size;

    if ( f->level == 66 ) {
        /* rename */
        if ( f->rename_thru ) {
            f->size = f->rename_thru->offset + f->rename_thru->size -
                      f->redefines->offset;
        } else {
            f->size = f->redefines->size;
        }
        return f->size;
    }

    if ( f->children ) {
        /* groups */
        f->size = 0;
        for ( c = f->children; c; c = c->sister ) {
            if ( c->redefines ) {
                c->offset = c->redefines->offset;
                compute_size (c);
                /* increase the size if redefinition is larger */
                if ( c->level != 66 &&
                     (c->size * c->occurs_max) >
                     (c->redefines->size * c->redefines->occurs_max) ) {
                    if ( cb_larger_redefines_ok && !(c->occurs_depending || cb_field_variable_size(c))) {

                        cb_warning_information (CB_TREE (c),
                                                _("Size of '%s' larger than size of '%s'"),
                                                c->name, c->redefines->name);
                        /* f->size +=
                            (c->size * c->occurs_max) - (c->redefines->size * c->redefines->occurs_max);                               
                        */

                        if (c->redefines->redefine_size == 0) {
                            c->redefines->redefine_size = (c->redefines->size * c->redefines->occurs_max);
                        }
                        if (c->redefines->redefine_size < (c->size * c->occurs_max)) {
                            f->size +=
                            (c->size * c->occurs_max) - c->redefines->redefine_size;                               

                            c->redefines->redefine_size =  (c->size * c->occurs_max);
                        }

                    } else {
                        if ( (c->occurs_depending || cb_field_variable_size(c)) ) {
                            cb_error_x (CB_TREE (c),
                                        _("Size of '%s' is varying and larger than size of '%s'"),
                                        c->name, c->redefines->name);
                        } else {
                            cb_error_x (CB_TREE (c),
                                        _("Size of '%s' larger than size of '%s'"),
                                        c->name, c->redefines->name);
                        }
                    }
                }
            } else {
                int     sz;
                int     pad = 0;      
                int     propagate = 0;          
                int     szelem;
                int     child_align = 1;
                c->offset = f->offset + f->size;
                szelem = compute_size(c);
                pad = compute_sync_padding(c);
                if (pad) {
                    c->offset += pad;
                    szelem = compute_size(c);
                }
                if (cb_verify (cb_synchronized_clause, "SYNC") && 
                    cb_synchronized_propagate_to_occurs) {
                    if (c->occurs_max > 1 && c->children) {
                        child_align = compute_child_sync_max_align_size(c);
                        if (child_align > 1) {
                            propagate = 1;
                        }
                    }
                }
                if ((c->flag_synchronized || propagate) && 
                    cb_verify (cb_synchronized_clause, "SYNC") && 
                    (cb_verify (cb_synchronized_occurs_align_size, "SYNC ON OCCURS" )  || propagate)) {
                    if (c->occurs_max > 1 && c->children) {
                        int padelem = 0;
                        int alsz = child_align; /*get_sync_align_size(szelem);*/
                        if (cb_synchronized_propagate_to_occurs_goup_size) {
                            /* old bug default off*/
                            alsz = get_sync_align_size(szelem);
                        }
                        if (szelem % alsz) {
                            padelem= alsz - (szelem % alsz);
                            /*printf("%s : szel=%d pel=%d \n", f->name, szelem, padelem);*/
                            c->size += padelem;
                            szelem = c->size;
                        }
                    }
                }
                sz = szelem * c->occurs_max;
                f->size += sz + pad;
                /*printf("%s : fo=%d fs= %d  %s co=%d cs= %d\n", f->name, f->offset, f->size, c->name, c->offset, c->size);*/
            }
        }
    } else {
        /* elementary item */
        switch ( f->usage ) {
            case CB_USAGE_COMP_X:
                if ( f->pic->category == CB_CATEGORY_ALPHANUMERIC ) {
                    break;
                }
                if ( f->pic->category == CB_CATEGORY_BIT ) {
                    f->flag_real_binary=1;
                    f->size = f->pic->size / 8;
                    if ( f->pic->size % 8 ) {
                        f->size ++;
                    }
                } else {
                    size = f->pic->size;
                    f->size = ((size <= 2) ? 1 : (size <= 4) ? 2 :
                               (size <= 7) ? 3 : (size <= 9) ? 4 :
                               (size <= 12) ? 5 : (size <= 14) ? 6 :
                               (size <= 16) ? 7 : (size <= 18) ? 8 : 16);
                }
                break;
            case CB_USAGE_BINARY:
            case CB_USAGE_COMP_5:
                if ( f->pic->category == CB_CATEGORY_BIT ) {
                    f->flag_real_binary=1;
                    f->size = f->pic->size / 8;
                    if ( f->pic->size % 8 ) {
                        f->size ++;
                    }
                    if ( f->size > 8 ) {
                        cb_error_x (CB_TREE (f),
                                    _("'%s' BIT field cannot be larger than 64 bits"),
                                    f->name);
                    }
                } else {
                    size = f->pic->size;
                    if ( size > 18 ) {
                        f->flag_binary_swap = 0;
                        cb_error_x (CB_TREE (f),
                                    _("'%s' binary field cannot be larger than 18 digits"),
                                    f->name);
                    }

                    if ( f->flag_binary_pack ) {
                        field_binary_size = CB_BINARY_SIZE_1__8;
                    }
                    switch ( field_binary_size ) {
                        case CB_BINARY_SIZE_2_4_8:
                            if ( f->flag_real_binary && size <= 2 ) {
                                f->size = 1;
                            } else {
                                f->size = ((size <= 4) ? 2 :
                                           (size <= 9) ? 4 : (size <= 18) ? 8 : 16);
                            }
                            break;
                        case CB_BINARY_SIZE_1_2_4_8:
                            f->size = ((size <= 2) ? 1 :
                                       (size <= 4) ? 2 :
                                       (size <= 9) ? 4 : (size <= 18) ? 8 : 16);
                            break;
                        case CB_BINARY_SIZE_1__8:
                            if ( f->pic->have_sign ) {
                                f->size = ((size <= 2) ? 1 : (size <= 4) ? 2 :
                                           (size <= 6) ? 3 : (size <= 9) ? 4 :
                                           (size <= 11) ? 5 : (size <= 14) ? 6 :
                                           (size <= 16) ? 7 : (size <= 18) ? 8 : 16);
                            } else {
                                f->size = ((size <= 2) ? 1 : (size <= 4) ? 2 :
                                           (size <= 7) ? 3 : (size <= 9) ? 4 :
                                           (size <= 12) ? 5 : (size <= 14) ? 6 :
                                           (size <= 16) ? 7 : (size <= 18) ? 8 : 16);
                            }
                            break;
                    }
                }
                break;
            case CB_USAGE_DISPLAY:
                f->size = f->pic->size;
                if ( f->pic->category == CB_CATEGORY_NUMERIC
                     && f->pic->have_sign && f->flag_sign_separate ) {
                    f->size++;
                }
                break;
            case CB_USAGE_NATIONAL:
                f->size = f->pic->size;
                if ( f->pic->category != CB_CATEGORY_NATIONAL ) {
                    cb_error_x (CB_TREE (f), _("'%s' USAGE NATIONAL Only allow PICTURE N "), 
                                f->name);
                }
                break;

            case CB_USAGE_PACKED:
                f->size = f->pic->size / 2 + 1;
                break;
            case CB_USAGE_COMP_6:
                f->size = (f->pic->size+1) / 2;
                break;
            case CB_USAGE_INDEX:
            case CB_USAGE_LENGTH:
                f->size = sizeof (int);
                break;
            case CB_USAGE_FLOAT:
                f->size = sizeof (float);
                break;
            case CB_USAGE_DOUBLE:
                f->size = sizeof (double);
                break;
            case CB_USAGE_OBJECT:
            case CB_USAGE_POINTER:
            case CB_USAGE_PROGRAM_POINTER:
            case CB_USAGE_PROGRAM:
                f->size = sizeof (void *);
                break;
            default:
                ABORT ();
        }
    }

    /* the size of redefining field should not be larger than
       the size of redefined field unless the redefined field
       is level 01 and non-external */
    if ( f->redefines && (f->redefines->flag_external || f->redefines->level == 1) 
         && (f->size * f->occurs_max > f->redefines->size * f->redefines->occurs_max) ) {
        if ( cb_larger_redefines_ok ) {
            cb_warning_information (CB_TREE (f), _("Size of '%s' larger than size of '%s'"),
                                    f->name, f->redefines->name);
        } else {
            cb_error_x (CB_TREE (f), _("Size of '%s' larger than size of '%s'"),
                        f->name, f->redefines->name);
        }
    }

    return f->size;
}

static int
validate_field_value (struct cb_field *f) {
    if ( f->values ) {
        cb_validate_move (CB_VALUE (f->values), CB_TREE (f), 1);
    }

    if ( f->children ) {
        for ( f = f->children; f; f = f->sister ) {
            validate_field_value (f);
        }
    }

    return 0;
}


static void
cb_validate_proccess_like (struct cb_field *f) {
    if ( f->like ) {
        char buff[20];
        struct cb_field *like = f->like;
        f->like = NULL;
        cb_validate_field(like);
        f->usage = CB_USAGE_DISPLAY;
        sprintf(buff, "X(%d)", like->size);
        f->pic = CB_PICTURE (cb_build_picture (buff));
        if (f->children) {
            cb_error_x (CB_TREE (f), _("LIKE field '%s' may not have subfield when -fas400-like is used"),   f->name);
        }
    } else {
        if ( f->children ) {
            for ( f = f->children; f; f = f->sister ) {
                cb_validate_proccess_like (f);
            }
        }
    }

}

void
cb_validate_field (struct cb_field *f) {
    struct cb_field     *c;

    if (f->flag_is_verified) {
        return;
    }
    cb_validate_proccess_like(f);
    /*Do not validate Typedef, Instance will be validated*/
    if ( f->flag_is_typedef ) {
        if ( f->redefines && ((f->level == 1) || (f->level == 77)) ) {
            cb_error_x (CB_TREE (f), _("TYPEDEF '%s' may not REDEFINES '%s'"),   f->name, f->redefines->name);
        }
        setup_parameters (f);
        compute_size (f);
        return;
    }
    if ( validate_field_1 (f) != 0 ) {
        f->flag_invalid = 1;
        return;
    }
    /* RXW - Remove */
    if ( f->flag_item_78 ) {
        f->flag_is_verified = 1;
        return;
    }

    /* setup parameters */
    if ( f->storage == CB_STORAGE_LOCAL ||
         f->storage == CB_STORAGE_LINKAGE ||
         f->flag_item_based ) {
        f->flag_local = 1;
    }
    if ( f->storage == CB_STORAGE_LINKAGE || f->flag_item_based ) {
        f->flag_base = 1;
    }
    setup_parameters (f);

    /* compute size */
    compute_size (f);
    if ( f->occurs_max == 0 ) {
        cb_error_x (CB_TREE (f), _("INTERNAL OCCURS MAX=0 '%s'"),
                    f->name);
    }
    if ( f->size == 0 ) {
        cb_error_x (CB_TREE (f), _("INTERNAL SIZE=0 '%s'"),
                    f->name);
    }
    if ( !f->redefines ) {
        f->memory_size = f->size * f->occurs_max;
    } else {
        if ( f->redefines->memory_size < f->size * f->occurs_max ) {
            f->redefines->memory_size = f->size * f->occurs_max;
        }
    }

    validate_field_value (f);
    if ( f->flag_is_global ) {
        f->count++;
        for ( c = f->children; c; c = c->sister ) {
            c->flag_is_global = 1;
            c->count++;
        }
    }
    f->flag_is_verified = 1;
}

static int 
cb_validate_bit_field(struct cb_field *f){
    if ( f->redefines ) {
        cb_error_x (CB_TREE (f), _("Bit field '%s' may not redefine '%s'"),
                    f->name, f->redefines->name);
        return 0;
    }
    if ( !f->pic ) {
        f->pic = CB_PICTURE (cb_build_picture ("1"));
    }
    if ( f->pic->category != CB_CATEGORY_BIT ) {
        cb_error_x (CB_TREE (f), _("Invalid PICTURE for Bit field '%s'"),
                    f->name);
        return 0;
    }
    /* setup parameters */
    if ( f->storage == CB_STORAGE_LOCAL ||
         f->storage == CB_STORAGE_LINKAGE ||
         f->flag_item_based ) {
        f->flag_local = 1;
    }
    if ( f->storage == CB_STORAGE_LINKAGE || f->flag_item_based ) {
        f->flag_base = 1;
    }
    validate_field_value (f);
    f->flag_is_verified = 1;
    return 1;
}


static struct cb_field *  
cb_extract_bit_field_1 (struct cb_field *f) {
    cb_tree dummy_fill;
    struct cb_field *field_fill;
    struct cb_field *fb = NULL;
    int cnt = 0;
    int i, bits;

    if ( cb_validate_bit_field(f) ) {
        dummy_fill = cb_build_bitfield_filler ();
        field_fill = CB_FIELD (cb_build_field (dummy_fill));
        //cb_warning_x (name, _("No previous data item of level %02d"), f->level);
        field_fill->level = f->level;
        field_fill->flag_is_typedef         = f->flag_is_typedef;
        field_fill->flag_is_typedef_global  = f->flag_is_typedef_global;
        field_fill->storage = f->storage;
        field_fill->bitfield_children = f;
        if ( f->brother ) {
            f->brother->sister = field_fill;
            f->brother = NULL;
        }
        if ( f->parent ) {
            field_fill->parent = f->parent;
            if ( f->parent->children == f ) {
                f->parent->children = field_fill;
            }
        }
        /*
        if ( f->flag_occurs ) {
            f->parent = field_fill; 
            f->flag_is_verified = 1;
            f->pic->scale = 0;
            f->flag_binary_swap = !cb_flag_bitfield_first_is_lsb;                     
            cnt = f->pic->digits * f->occurs_max;
            fb = f; 
            f = f->sister; 
        } else 
        */
        {
            for ( ; f ;  f = f->sister ) {
                if ( ( f->usage == CB_USAGE_BIT && !f->children) && 
                     cb_validate_bit_field(f)  ) {
                    f->parent = field_fill; 
                    f->flag_is_verified = 1;
                    f->bitoffset = cnt;
                    /*f->pic->scale = cnt;*/
                    if ( !cb_flag_bitfield_first_is_lsb ) {
                        f->flag_binary_swap = 1;
                    }
                    if ( f->flag_occurs ) {
                        bits = f->pic->digits * f->occurs_max;
                    } else {
                        bits = f->pic->digits;
                    }
                    cnt += bits;
                    f->size = (bits /8 ) + ((bits % 8) ? 1 : 0);
                    fb = f;   
                    /*                    
                    if ( (cnt % 8) == 0 ) {
                        f = f->sister ;
                        break;
                    } 
                    */ 
                    if ( f->sister && f->sister->flag_synchronized ) {
                        f = f->sister ; 
                        break;
                    }
                } else {
                    break;
                }
            }
        }
        if ( fb ) {
            fb->sister = NULL;
        }
        field_fill->sister = f;
        field_fill->usage=CB_USAGE_BYTE_ARRAY;
        i = cnt /8;
        if ( cnt%8 ) {
            i++;
        }
        /*
        field_fill->occurs_max=i;
        field_fill->occurs_min=i; 
        */
        field_fill->occurs_max=1;
        field_fill->occurs_min=1; 
        field_fill->size=i;
        cb_validate_field(field_fill);

        f = field_fill;
        /* 
        sprintf(buff,"X(%d)", i );
        field_fill->pic = CB_PICTURE (cb_build_picture (buff));
        */
    }
    return f;
}

static struct cb_field *
cb_extract_bit_field(struct cb_field *f) {
    struct cb_field *brother = NULL;
    struct cb_field *ret = f;

    if ( f->children ) {
        for ( f=f->children; f; brother = f, f = f->sister ) {
            f->brother = brother;
            f = cb_extract_bit_field(f);
        }
    } else if ( f->usage == CB_USAGE_BIT && !f->children) {
        ret = cb_extract_bit_field_1(f);
    }
    return ret;
}

static void 
cb_adjust_bit_field(struct cb_field *f) {

    for ( ; f;  f = f->sister ) {
        if ( f->children ) {
            cb_adjust_bit_field(f->children);
        } else if ( f->bitfield_children ) {
            struct cb_field *fb = NULL;

            for ( fb = f->bitfield_children; fb; fb = fb->sister ) {
                if ( !(fb->pic) ) {
                    fprintf (stderr, "Missing PIC for bitfield\n");
                    ABORT ();
                }
                fb->offset     = fb->parent->offset + (fb->bitoffset / 8);
                fb->pic->scale = fb->bitoffset % 8;
                fb->flag_item_external = fb->flag_external;
                /*fb->size   = fb->parent->size * fb->parent->occurs_max;*/
            }
        }
    }
}
static void 
cb_field_tree_mark_typedef (struct cb_field *f) {
    struct cb_field     *p = f;
    f->storage = CB_STORAGE_TYPEDEF;
    f->flag_is_typedef = 1;
    for (p=p->children ;p ; p=p->sister ) {
        cb_field_tree_mark_typedef(p);
    }
    for (p=f->bitfield_children ;p ; p=p->sister ) {
        cb_field_tree_mark_typedef(p);
    }

}

void
cb_validate_section_fields (struct cb_field **pf) {

    struct cb_field *f = *pf;

    while ( pf && f ) {
        if ( f->flag_is_typedef ) {
            *pf = f->sister;
            f->sister = typedef_storage;
            f = cb_extract_bit_field (f);
            cb_validate_field (f);
            cb_adjust_bit_field (f);
            typedef_storage = f;
            cb_field_tree_mark_typedef(f);
        } else {
            if ( !f->flag_is_verified ) {
                f = cb_extract_bit_field (f);
                cb_validate_field (f);
                cb_adjust_bit_field (f);
                *pf = f;
            }
            pf = &(f->sister);
        }
        f = *pf;
    }
}

void
cb_validate_88_item (struct cb_field *f) {
    cb_tree x;

    x = CB_TREE (f);
    if ( !f->values ) {
        level_require_error (x, "VALUE");
    }

    if ( f->pic || f->flag_occurs ) {
        level_except_error (x, "VALUE");
    }
    f->flag_dynamic_data = 1;
}

struct cb_field *
cb_validate_78_item (struct cb_field *f) {
    cb_tree x;

    x = CB_TREE (f);
    if ( !f->values ) {
        level_require_error (x, "VALUE");
    }

    if ( f->pic || f->flag_occurs ) {
        level_except_error (x, "VALUE");
    }
    cb_add_78 (f);
    return last_real_field;
}

void
cb_clear_real_field (void) {
    last_real_field = NULL;
}

cb_tree
cb_get_optimized_field_shadow (struct cb_field *f) {

    cb_tree             l;
    struct cb_field *   p;
    char                buff [100];


    if ( !f->flag_usage_optimized || 
         f->occurs_max != 1 ) {
        cb_error_x (CB_TREE (f),
                    _("Internal compiler error (optimized_field_shadow)"));
    }
    sprintf (buff, "SHADOW$%s", f->name);
    l = cb_build_reference (buff);
    for ( p=current_program->working_storage; p; p=p->sister ) {
        if ( strcmp(p->name, buff) == 0 ) {
            CB_REFERENCE(l)->value = CB_TREE(p);
            return l;
        }
    }
    p = CB_FIELD (cb_build_field (l));
    p->usage = f->original_usage;
    p->pic   = f->pic;
    p->flag_ebcdic_charset = f->flag_ebcdic_charset;
    cb_validate_field (p);
    p->count ++;
    p->flag_no_init = 1;
    current_program->working_storage = cb_field_add (current_program->working_storage, p);
    return l;
}

void
cb_increment_indexes (struct cb_field *f) {

    for (;f ; f = f->sister) {
        f->indexes++;
        if (f->children) {
            cb_increment_indexes(f->children);
        }
    }
}


static struct cb_field *
dup_screen_field (struct cb_field *f) {
    struct cb_field *n;

    n = cobc_malloc(sizeof(struct cb_field));
    *n = *f; 
    n->id = cb_field_id++;
    if (n->screen_from != n->screen_to) {
        n->screen_from = cb_dup_reference(f->screen_from);
        n->screen_to = cb_dup_reference(f->screen_to);
    } else {
        n->screen_from = cb_dup_reference(f->screen_from);
        n->screen_to = n->screen_from;
    }
    return n;
}

static struct cb_field *
dup_screen_field_tree (struct cb_field *f) {
    struct cb_field *n;

    n = dup_screen_field(f);
    if (f->children) {
        n->children = dup_screen_field_tree(f->children);
    }
    if (f->sister) {
        n->sister = dup_screen_field_tree(f->sister);
    }
    return n;
}

void 
cb_expand_screen_field (struct cb_field *f, cb_tree subs) {
    int i;
    int occurs ;
    struct cb_field *last ;
    struct cb_reference *ref;
    for (;f ; f = f->sister) {
        last = NULL;
        if ( f->occurs_max > 1 ) {
            occurs = f->occurs_max;
            f->occurs_max = 1;
            for ( i = 2 ; i <= occurs; i++ ) {
                struct cb_field *n;
                n = dup_screen_field(f);
                if (f->children) {
                    n->children = dup_screen_field_tree(f->children);
                }
                f->sister = n;
                if ( last == NULL ) {
                    last = n;
                }
            }
            for ( i = 1 ; f && i <= occurs; i++ ) {
                cb_tree l = subs;
                for ( ; l; l=CB_CHAIN(l)) {
                    f->screen_subs = cb_list_add(f->screen_subs, CB_VALUE(l));
                }
                f->screen_subs = cb_list_insert_first(f->screen_subs, cb_int(i));
                if (f->screen_from && CB_REFERENCE_P(f->screen_from)) {
                    ref = CB_REFERENCE(f->screen_from);
                    if (ref->subs != f->screen_subs) {
                        ref->subs = cb_list_append(ref->subs, f->screen_subs);;
                    }
                }
                if (f->screen_to && CB_REFERENCE_P(f->screen_to) && (f->screen_from != f->screen_to)) {
                    ref = CB_REFERENCE(f->screen_to);
                    if (ref->subs != f->screen_subs) {
                        ref->subs = cb_list_append(ref->subs, f->screen_subs);;
                    }
                }

                if (f->children) {
                    cb_expand_screen_field(f->children, f->screen_subs);
                }
                f = f->sister;
            }
            f = last;
        } else {
            f->screen_subs = subs;
            if (f->screen_from && CB_REFERENCE_P(f->screen_from)) {
                ref = CB_REFERENCE(f->screen_from);
                if (ref->subs != f->screen_subs) {
                    ref->subs = cb_list_append(ref->subs, f->screen_subs);;
                }
            }
            if (f->screen_to && CB_REFERENCE_P(f->screen_to) && (f->screen_from != f->screen_to)) {
                ref = CB_REFERENCE(f->screen_to);
                if (ref->subs != f->screen_subs) {
                    ref->subs = cb_list_append(ref->subs, f->screen_subs);;
                }
            }
            if (f->children) {
                cb_expand_screen_field(f->children, f->screen_subs);
            }
        }          
    }
}


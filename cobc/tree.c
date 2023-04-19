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
#include <math.h>

#include "cobc.h"
#include "tree.h"
#include "enterprise/a2e.h"

#define PIC_ALPHABETIC          0x01
#define PIC_NUMERIC             0x02
#define PIC_NATIONAL            0x04
#define PIC_EDITED              0x08
#define PIC_ALPHANUMERIC        (PIC_ALPHABETIC | PIC_NUMERIC)
#define PIC_ALPHABETIC_EDITED   (PIC_ALPHABETIC | PIC_EDITED)
#define PIC_ALPHANUMERIC_EDITED (PIC_ALPHANUMERIC | PIC_EDITED)
#define PIC_NUMERIC_EDITED      (PIC_NUMERIC | PIC_EDITED)
#define PIC_NATIONAL_EDITED     (PIC_NATIONAL | PIC_EDITED)
#define PIC_BIT                 0x10

static const enum cb_class category_to_class_table[] = {
    CB_CLASS_UNKNOWN,       /* CB_CATEGORY_UNKNOWN */
    CB_CLASS_ALPHABETIC,    /* CB_CATEGORY_ALPHABETIC */
    CB_CLASS_ALPHANUMERIC,  /* CB_CATEGORY_ALPHANUMERIC */
    CB_CLASS_ALPHANUMERIC,  /* CB_CATEGORY_ALPHANUMERIC_EDITED */
    CB_CLASS_BOOLEAN,       /* CB_CATEGORY_BOOLEAN */
    CB_CLASS_INDEX,         /* CB_CATEGORY_INDEX */
    CB_CLASS_NATIONAL,      /* CB_CATEGORY_NATIONAL */
    CB_CLASS_NATIONAL,      /* CB_CATEGORY_NATIONAL_EDITED */
    CB_CLASS_NUMERIC,       /* CB_CATEGORY_NUMERIC */
    CB_CLASS_ALPHANUMERIC,  /* CB_CATEGORY_NUMERIC_EDITED */
    CB_CLASS_OBJECT,        /* CB_CATEGORY_OBJECT_REFERENCE */
    CB_CLASS_POINTER,       /* CB_CATEGORY_DATA_POINTER */
    CB_CLASS_POINTER,       /* CB_CATEGORY_PROGRAM_POINTER */
    CB_CLASS_NUMERIC,       /* CB_CATEGORY_BIT */
};

static struct int_node {
    struct int_node *next;
    cb_tree         node;
    int             n;
} *int_node_table = NULL;

static char *treenamebuff = NULL;

/*
 * Constants
 */

cb_tree cb_any;
cb_tree cb_positive;
cb_tree cb_not_positive;
cb_tree cb_negative;
cb_tree cb_not_negative;
cb_tree cb_numeric;
cb_tree cb_not_numeric;
cb_tree cb_alphabetic;
cb_tree cb_not_alphabetic;
cb_tree cb_alphabetic_lower;
cb_tree cb_not_alphabetic_lower;
cb_tree cb_alphabetic_upper;
cb_tree cb_not_alphabetic_upper;
cb_tree cb_true;
cb_tree cb_false;
cb_tree cb_null;
cb_tree cb_zero;
cb_tree cb_one;
cb_tree cb_space;
cb_tree cb_low;
cb_tree cb_high;
cb_tree cb_norm_low;
cb_tree cb_norm_high;
cb_tree cb_quote;
char    cb_quote_char = '"';
cb_tree cb_int0;
cb_tree cb_int1;
cb_tree cb_int2;
cb_tree cb_int3;
cb_tree cb_int4;
cb_tree cb_int5;
cb_tree cb_i[8];
cb_tree cb_error_node;
cb_tree cb_xml_code;
cb_tree cb_dirsep;

cb_tree cb_intr_whencomp;
cb_tree cb_intr_pi;
cb_tree cb_intr_e;

cb_tree cb_standard_error_handler;
cb_tree cb_save_context_handler;
cb_tree cb_save_context_handler_ref;

size_t  gen_screen_ptr = 0;
 
struct index_optimizer *index_optimizer_cache = NULL;
struct index_optimizer_stack  *io_stack =NULL;

/* Local functions */

char *
cb_to_cname (const char *s, char *copy) {
    unsigned char   *p;
    static char copy_buffer[COB_MEDIUM_BUFF];

    if ( !copy ) {
        copy = copy_buffer;
        strcpy(copy, s);
    }
    for ( p = (unsigned char *)copy; *p; p++ ) {
        *p = (*p == '-') ? '_' : toupper (*p);
    }
    return copy;
}

static size_t
hash (const unsigned char *s) {
    size_t val = 0;

    while ( *s ) {
        val += toupper (*s++);
        if ( *s ) {
            val += (toupper (*s++) << 8);
            if ( *s ) {
                val += (toupper (*s++) << 16);
                if ( *s ) {
                    val += (toupper (*s++) << 24);
                }
            }
        }
    }
    return val % CB_WORD_HASH_SIZE;
}

static struct cb_word *
new_word (const char *name, size_t val) {
    struct cb_word  *p;
    char *c;
    /* create new word */
    p = cobc_malloc (sizeof (struct cb_word));
    p->name = strdup (name);

    /* insert it into the table */
    if ( current_program ) {
        p->next = current_program->word_table[val];
        current_program->word_table[val] = p;
    }
    if ( cb_max_ident_length > 0 && (strlen(name) > cb_max_ident_length)) {
        c = strchr(name, '_');
        if ( !c ) {
            c = strchr(name, '$');
        }
        if ( !c || strlen(c+1) > cb_max_ident_length) {
            cb_error (("%s identifier is %d characters length, Limit is set to %d"), c ? c+1 : name, (int)strlen(c ? c+1 : name), cb_max_ident_length);
        }

    }

    return p;
}

static struct cb_word *
lookup_word (const char *name) {
    struct cb_word  *p;
    size_t          val;

    val = hash ((const unsigned char *)name);
    /* find the existing word */
    if ( current_program ) {
        for ( p = current_program->word_table[val]; p; p = p->next ) {
            if ( strcasecmp (p->name, name) == 0 ) {
                return p;
            }
        }
    }
    return new_word(name, val);
}

static void
file_error (cb_tree name, const char *clause) {
    cb_error_x (name, _("%s clause is required for file '%s'"), clause,
                CB_NAME (name));
}

/*
 * Tree
 */

static void *
make_tree (int tag, enum cb_category category, size_t size) {
    cb_tree x;

    x = cobc_malloc (size);
    x->tag = tag;
    x->category = category;
    x->dst = NULL;
    x->src = NULL;
    return x;
}

/*CIT*/
cb_tree
make_constant (enum cb_category category, const char *val) {
    struct cb_const *p;

    p = make_tree (CB_TAG_CONST, category, sizeof (struct cb_const));
    p->val = val;
    p->num_type = CB_CONST_STD;
    return CB_TREE (p);
}

/*CIT*/
cb_tree
make_constant_label (const char *name) {
    struct cb_label *p;

    p = CB_LABEL (cb_build_label (cb_build_reference (name), NULL));
    p->need_begin = 1;
    return CB_TREE (p);
}

cb_tree
make_temp_label (void) {
    struct cb_label *p;
    char buff [100];

    sprintf (buff, "lbltmp_%d", cb_id);
    p = CB_LABEL (cb_build_label (cb_build_reference (buff), NULL));
    p->need_begin = 1;
    return CB_TREE (p);
}

static int
cb_name_1 (char *s, cb_tree x) {
    char                    *orig;
    struct cb_funcall       *cbip;
    struct cb_binary_op     *cbop;
    struct cb_reference     *p;
    struct cb_intrinsic *cbit;
    cb_tree                 l;
    int                     i;

    orig = s;
    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_CONST:
            if ( x == cb_any ) {
                strcpy (s, "ANY");
            } else if ( x == cb_true ) {
                strcpy (s, "TRUE");
            } else if ( x == cb_false ) {
                strcpy (s, "FALSE");
            } else if ( x == cb_null ) {
                strcpy (s, "NULL");
            } else if ( x == cb_zero ) {
                strcpy (s, "ZERO");
            } else if ( x == cb_space ) {
                strcpy (s, "SPACE");
            } else if ( x == cb_low || x == cb_norm_low ) {
                strcpy (s, "LOW-VALUE");
            } else if ( x == cb_high || x == cb_norm_high ) {
                strcpy (s, "HIGH-VALUE");
            } else if ( x == cb_quote ) {
                strcpy (s, "QUOTE");
            } else if ( x == cb_dirsep ) {
                strcpy (s, "DIR-SEPARATOR");
            } else if ( x == cb_error_node ) {
                strcpy (s, "Internal error node");
            } else {
                strcpy (s, "#<unknown constant>");
            }
            break;

        case CB_TAG_LITERAL:
            if ( CB_TREE_CLASS (x) == CB_CLASS_NUMERIC ) {
                strcpy (s, (char *)CB_LITERAL (x)->data);
            } else {
                sprintf (s, "\"%s\"", CB_LITERAL (x)->data);
            }
            break;

        case CB_TAG_FIELD:
            strcpy (s, CB_FIELD (x)->name);
            break;

        case CB_TAG_REFERENCE:
            p = CB_REFERENCE (x);
            if (strncmp(CB_NAME(x), LABEL_PREFIX, strlen(LABEL_PREFIX))==0) {
                s += sprintf (s, "%s", (CB_NAME(x))+strlen(LABEL_PREFIX));
            } else {
                s += sprintf (s, "%s", p->word->name);
                if ( p->subs ) {
                    l = p->subs = cb_list_reverse (p->subs);
                    s += sprintf (s, " (");
                    for ( ; l; l = CB_CHAIN (l) ) {
                        s += cb_name_1 (s, CB_VALUE (l));
                        s += sprintf (s, CB_CHAIN (l) ? ", " : ")");
                    }
                    p->subs = cb_list_reverse (p->subs);
                }
                if ( p->offset ) {
                    s += sprintf (s, " (");
                    s += cb_name_1 (s, p->offset);
                    s += sprintf (s, ":");
                    if ( p->length ) {
                        s += cb_name_1 (s, p->length);
                    }
                    strcpy (s, ")");
                }
            }

            if ( p->chain ) {
                s += sprintf (s, " in ");
                s += cb_name_1 (s, p->chain);
            }
            break;

        case CB_TAG_LABEL:
            sprintf (s, "%s", CB_LABEL (x)->orig_name);
            break;

        case CB_TAG_ALPHABET_NAME:
            sprintf (s, "%s", CB_ALPHABET_NAME (x)->name);
            break;

        case CB_TAG_CLASS_NAME:
            sprintf (s, "%s", CB_CLASS_NAME (x)->name);
            break;

        case CB_TAG_LOCALE_NAME:
            sprintf (s, "%s", CB_LOCALE_NAME (x)->name);
            break;

        case CB_TAG_BINARY_OP:
            cbop = CB_BINARY_OP (x);
            if ( cbop->op == '@' ) {
                s += sprintf (s, "(");
                s += cb_name_1 (s, cbop->x);
                s += sprintf (s, ")");
            } else if ( cbop->op == '!' ) {
                s += sprintf (s, "!");
                s += cb_name_1 (s, cbop->x);
            } else if ( cbop->op == 'v' ) {
                s += cb_name_1 (s, cbop->x);
            } else if ( cbop->op == 'n' ) {
                s += sprintf (s, "not ");
                s += cb_name_1 (s, cbop->x);
            } else {
                s += sprintf (s, "(");
                s += cb_name_1 (s, cbop->x);
                s += sprintf (s, " %c ", cbop->op);
                s += cb_name_1 (s, cbop->y);
                strcpy (s, ")");
            }
            break;

        case CB_TAG_FUNCALL:
            cbip = CB_FUNCALL (x);
            s += sprintf (s, "%s", cbip->name);
            for ( i = 0; i < cbip->argc; i++ ) {
                s += sprintf (s, (i == 0) ? "(" : ", ");
                s += cb_name_1 (s, cbip->argv[i]);
            }
            s += sprintf (s, ")");
            break;

        case CB_TAG_INTRINSIC:
            cbit = CB_INTRINSIC (x);
            sprintf (s, "FUNCTION %s", cbit->intr_tab->name);
            break;
        default:
            sprintf (s, "#<unknown %d %p>", CB_TREE_TAG (x), x);
    }

    return strlen (orig);
}

static cb_tree
make_intrinsic (cb_tree name, struct cb_intrinsic_table *cbp, cb_tree args,
                cb_tree field, cb_tree refmod) {
    struct cb_intrinsic *x;

/* Leave in, we may need this
    cb_tree         l;
    for (l = args; l; l = CB_CHAIN(l)) {
        switch (CB_TREE_TAG (CB_VALUE(l))) {
        case CB_TAG_CONST:
        case CB_TAG_INTEGER:
        case CB_TAG_LITERAL:
        case CB_TAG_DECIMAL:
        case CB_TAG_FIELD:
        case CB_TAG_REFERENCE:
        case CB_TAG_INTRINSIC:
            break;
        default:
            cb_error (_("FUNCTION %s has invalid/not supported arguments - Tag %d"),
                cbp->name, CB_TREE_TAG(l));
            return cb_error_node;

        }
    }
*/
    x = make_tree (CB_TAG_INTRINSIC, cbp->category, sizeof (struct cb_intrinsic));
    x->name = name;
    x->args = args;
    x->intr_tab = cbp;
    x->intr_field = field;
    if ( refmod ) {
        x->offset = CB_PAIR_X (refmod);
        x->length = CB_PAIR_Y (refmod);
    }
    return CB_TREE (x);
}

static cb_tree
global_check (struct cb_reference *r, cb_tree items, size_t *ambiguous, int type_only) {
    cb_tree         candidate = NULL;
    struct cb_field     *p;
    cb_tree         v=NULL;
    cb_tree         c=NULL;

    for ( ; items; items = CB_CHAIN (items) ) {
        /* find a candidate value by resolving qualification */
        v = CB_VALUE (items);
        c = r->chain;
        if ( CB_SYSTEM_NAME_P(v) ) {
            if ( candidate == NULL ) {
                /* keep the first candidate */
                candidate = v;
            } else {
                /* multiple candidates and possibly ambiguous */
                *ambiguous = 1;
            }
            continue;
        }
        if ( CB_FIELD_P (v) ) {
            if ( ( type_only && !CB_FIELD (v)->flag_is_typedef ) ||
                 ( !type_only && CB_FIELD (v)->flag_is_typedef ) ) {
                v = NULL;
                c = NULL;
                continue;
            }
            if ( ( type_only &&  !CB_FIELD (v)->flag_is_typedef_global ) ||
                 ( !type_only && !CB_FIELD (v)->flag_is_global ) ) {
                continue;
            }
            /* in case the value is a field, it might be qualified
               by its parent names and a file name */
            if ( CB_FIELD (v)->flag_indexed_by ) {
                p = CB_FIELD (v)->index_qual;
            } else {
                p = CB_FIELD (v)->parent;
            }
            /* resolve by parents */
            for ( ; p; p = p->parent ) {
                if ( c && strcasecmp (CB_NAME (c), p->name) == 0 ) {
                    c = CB_REFERENCE (c)->chain;
                }
            }

            /* resolve by file */
            if ( c && CB_REFERENCE (c)->chain == NULL ) {
                if ( CB_REFERENCE (c)->word->count == 1 && CB_FILE_P (cb_ref (c))
                     && (CB_FILE (cb_ref (c)) == cb_field_founder (CB_FIELD (v))->file) ) {
                    c = CB_REFERENCE (c)->chain;
                }
            }
        }
        /* a well qualified value is a good candidate */
        if ( c == NULL ) {
            if ( candidate == NULL ) {
                /* keep the first candidate */
                candidate = v;
            } else {
                /* multiple candidates and possibly ambiguous */
                *ambiguous = 1;
            }
        }
    }
    return candidate;
}


/* Global functions */

cb_tree 
add_to_target_ref_list(cb_tree x, cb_tree r) {
    return add_to_target_ref_list_decimal(x,r,0);
}

cb_tree 
add_to_target_ref_list_decimal(cb_tree x, cb_tree r, int skip) {
    if ( CB_REF_OR_FIELD_P(r) ) {
        r->skip_for_decimal = skip;
        x->target_ref_list = cb_cons (r, x->target_ref_list);
    }
    return x;
}

int
is_base_alligned (cb_tree x) {
    struct cb_field     *f;
    struct cb_reference *r;

    if (CB_CAST_P (x) ) {
        struct cb_cast *p = CB_CAST (x);
        x = p->val;
    }
    r = CB_REFERENCE (x);
    f = CB_FIELD (r->value);

    if ( f->flag_item_78 ) {
        fprintf (stderr, "Unexpected CONSTANT item\n");
        ABORT ();
    }
    while ( f->redefines ) {
        f = f->redefines;
    }
    if ( cb_field_variable_address (f) && !r->cb_all_occurs) {
        return 0;
    } else if ( f->offset > 0 ) {
        if (f->size <=4) {
            return((f->offset % 4) == 0);
        } else {
            return((f->offset % 8) == 0);
        }
    }
    return 1;
}

int  is_same_field(cb_tree x, cb_tree y)
{
    int result = 0;
    struct cb_field *fx, *fy;
    struct cb_literal *lx= NULL, *ly=NULL;
    if (!x || !y) {
        return 0;
    }
    if (CB_REF_OR_FIELD_P(x) && CB_REF_OR_FIELD_P(y)) {

        fx = cb_field(x);
        fy = cb_field(y);
        if (cb_field_variable_address (fx) || cb_field_variable_address (fy)) {
            result = 0;
        } else {
            struct cb_reference *rx,*ry;
            rx = CB_REFERENCE (x);
            ry = CB_REFERENCE (y);
            /* Subscripts */
            if (fx->id == fy->id) {
                if ( rx->subs && ry->subs) {
                    result = same_index_from_table_occurs(x,y);
                } else if ( fx->usage == CB_USAGE_BIT && fy->usage == CB_USAGE_BIT ) {
                    result = (fx->pic->size == fy->pic->size && fx->pic->scale == fy->pic->scale);
                } else
                    result = 1;
            }
        }
    } else if (CB_LITERAL_P(x) && CB_LITERAL_P(y)) {
        lx = CB_LITERAL(x);
        ly = CB_LITERAL(y);
        result = (lx && ly && (lx->size == ly->size) && (lx->scale == ly->scale)
                  && (lx->sign == ly->sign)
                  && (strcmp((const char*)lx->data,(const char*)ly->data) == 0 ));
    } else
        return 0;
    return result;
}

int is_identical_or_ovelap_fields(cb_tree x, cb_tree y){
    int result;
    struct cb_field *fx, *fy;
    struct cb_literal *lx= NULL, *ly=NULL;
    if (!x || !y) {
        return 0;
    }
    if (CB_REF_OR_FIELD_P(x) && CB_REF_OR_FIELD_P(y)) {

        fx = cb_field(x);
        fy = cb_field(y);
        if (cb_field_variable_address (fx) || cb_field_variable_address (fy)) {
            result = 0;
        } else {
            struct cb_reference *rx,*ry;
            result = (derived_from_same_base_root(fx, fy) && is_overlapped(fx, fy));
            if (!result) {
                rx = CB_REFERENCE (x);
                ry = CB_REFERENCE (y);
                if (rx->subs && ry->subs) {
                    result = fx->id == fy->id && same_index_from_table_occurs(x,y);
                } else if ( fx->usage == CB_USAGE_BIT && fy->usage == CB_USAGE_BIT ) {
                    result = (fx->id == fy->id && fx->pic->size == fy->pic->size && fx->pic->scale == fy->pic->scale);
                } else {
                    result = (fx->id == fy->id && fx->offset == fy->offset && fx->size == fy->size);
                }
            }

        }
    } else if (CB_LITERAL_P(x) && CB_LITERAL_P(y)) {
        lx = CB_LITERAL(x);
        ly = CB_LITERAL(y);
        result = (lx && ly && (lx->size == ly->size) && (lx->scale == ly->scale)
                  && (lx->sign == ly->sign)
                  && (strcmp((const char*)lx->data,(const char*)ly->data) == 0 ));
    } else
        return 0;
    return result;
}
int derived_from_same_base_root(struct cb_field *f1, struct cb_field *f2){
    struct cb_field *f01, *f02;
    if (f1 == NULL || f2 == NULL) {
        return 0;
    }
    f01 = cb_field_founder (f1);
    f02 = cb_field_founder(f2);
    while ( f01->redefines ) {
        f01 = f01->redefines;
    }
    while ( f02->redefines ) {
        f02 = f02->redefines;
    }
    return(f01->id == f02->id);
}
static int
compare_index (cb_tree x, cb_tree y) {
    enum cb_tag xtag, ytag;
    xtag = CB_TREE_TAG (x);
    ytag = CB_TREE_TAG (y);
    if (xtag == CB_TAG_INTEGER && ytag == CB_TAG_INTEGER)
        return CB_INTEGER (x)->val == CB_INTEGER (y)->val;
    else if (CB_NUMERIC_LITERAL_P(x) && CB_NUMERIC_LITERAL_P(y))
        return cb_get_int (x) == cb_get_int (y);
    else {
        return 0; /*same_index_from_table_occurs(x, y);*/
    }
}
int same_index_from_table_occurs(cb_tree x, cb_tree y){
    struct cb_field *fx, *fy;
    struct cb_reference *rx,*ry;
    cb_tree         lsubx,lsuby;
    int result =0;
    if (!CB_REF_OR_FIELD_P(x) || !CB_REF_OR_FIELD_P(y))
        return 0;
    rx = CB_REFERENCE (x);
    fx = CB_FIELD (rx->value);
    ry = CB_REFERENCE (y);
    fy = CB_FIELD (ry->value);
    result = derived_from_same_base_root(fx,fy);

    /* Subscripts */
    if (result && rx->subs && ry->subs) {
        lsubx = rx->subs;
        lsuby = ry->subs;
        for ( ; fx && lsubx; fx = fx->parent ) {
            if ( fx->flag_occurs && fy->flag_occurs) {
                if ( fx->usage != CB_USAGE_BIT && fy->usage != CB_USAGE_BIT) {
                    if (CB_VALUE(lsubx) != cb_int1  && CB_VALUE(lsuby) != cb_int1) {
                        result = result && (fx->size == fy->size);

                        result = result && compare_index (CB_VALUE (lsubx), CB_VALUE (lsuby));
                    }
                }
                lsubx = CB_CHAIN (lsubx);
                lsuby = CB_CHAIN (lsuby);
                if (!lsuby) break;
            }
            fy = fy->parent;
            if (!fy)
                break;
        }
    }
    return result;
}

static struct cb_field * 
find_parent_occurs (struct cb_field *f1) {
    struct cb_field *res=f1;
    struct cb_field *f = f1;
    do {
        if (f->flag_occurs) {
            res = f;
        }
        f = f->parent;
    } while (f);
    return res;
}

int is_overlapped(struct cb_field *f1, struct cb_field *f2){

    if (f1 == NULL || f2 == NULL) {
        return 1;
    }
    f1 = find_parent_occurs(f1);
    f2 = find_parent_occurs(f2);
    return!( ( (f1->offset + (f1->size * f1->occurs_max)) <= f2->offset)  ||  ( (f2->offset + (f2->size * f2->occurs_max))  <= f1->offset)) ;
}
cb_tree 
add_list_to_target_ref_list(cb_tree x, cb_tree l) {
    if ( CB_LIST_P(l) ) {
        while ( l ) {
            x->target_ref_list = cb_cons (CB_VALUE(l), x->target_ref_list);
            l = CB_CHAIN(l);
        }
    }
    return x;
}

struct cb_literal *
build_literal (enum cb_category category, const unsigned char *data, size_t size) {
    struct cb_literal *p;

    p = make_tree (CB_TAG_LITERAL, category, sizeof (struct cb_literal));
    p->data = cobc_malloc ((size_t) (size + 2)); // +1 (final 0), +1 we some time need to add a char + - $ etc et the end
    p->size = size;
    memcpy (p->data, data, (size_t) size);
    /* RXW - malloc zeroes
    p->data[size] = 0;
    */
    return p;
}

char *
cb_name (cb_tree x) {
    if ( !treenamebuff ) {
        treenamebuff = cobc_malloc (COB_NORMAL_BUFF);
    }
    cb_name_1 (treenamebuff, x);
    return treenamebuff;
}

enum cb_class
cb_tree_class (cb_tree x) {

    return category_to_class_table[CB_TREE_CATEGORY (x)];
}

enum cb_category
cb_tree_category (cb_tree x) {
    struct cb_cast          *p;
    struct cb_reference     *r;
    struct cb_field         *f;

    if ( x == cb_error_node ) {
        return 0;
    }
    if ( x->category != CB_CATEGORY_UNKNOWN ) {
        return x->category;
    }

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_CAST:
            p = CB_CAST (x);
            switch ( p->type ) {
                case CB_CAST_ADDRESS:
                case CB_CAST_ADDR_OF_ADDR:
                    x->category = CB_CATEGORY_DATA_POINTER;
                    break;
                case CB_CAST_PROGRAM_POINTER:
                    x->category = CB_CATEGORY_PROGRAM_POINTER;
                    break;
                case CB_CAST_LENGTH:
                    x->category = CB_CATEGORY_NUMERIC;
                    break;
                default:
                    fprintf (stderr, "cobc:0: Unexpected cast type -> %d\n", p->type);
                    ABORT ();
            }
            break;
        case CB_TAG_REFERENCE:
            r = CB_REFERENCE (x);
            if ( r->offset ) {
                if ( cb_tree_category(r->value) == CB_CATEGORY_NATIONAL ) {
                    x->category = cb_tree_category (r->value);
                } else {
                    x->category = CB_CATEGORY_ALPHANUMERIC;
                }
            } else {
                x->category = cb_tree_category (r->value);
            }
            break;
        case CB_TAG_FIELD:
            f = CB_FIELD (x);
            if ( f->children ) {
                x->category = CB_CATEGORY_ALPHANUMERIC;
            } else if ( f->usage == CB_USAGE_POINTER && f->level != 88 ) {
                x->category = CB_CATEGORY_DATA_POINTER;
            } else if ( f->usage == CB_USAGE_PROGRAM_POINTER && f->level != 88 ) {
                x->category = CB_CATEGORY_PROGRAM_POINTER;
            } else if ( f->usage == CB_USAGE_NATIONAL ) {
                x->category = CB_CATEGORY_NATIONAL;
            } else if ( f->usage == CB_USAGE_UNSIGNED_CHAR ) {
                x->category = CB_CATEGORY_NUMERIC;
            } else {
                switch ( f->level ) {
                    case 66:
                        if ( f->rename_thru ) {
                            x->category = CB_CATEGORY_ALPHANUMERIC;
                        } else {
                            x->category = cb_tree_category (CB_TREE (f->redefines));
                        }
                        break;
                    case 88:
                        x->category = CB_CATEGORY_BOOLEAN;
                        break;
                    default:
                        if ( f->pic && f->pic->category == CB_CATEGORY_BIT ) {
                            switch ( f->usage ) {
                                case CB_USAGE_DISPLAY:
                                case CB_USAGE_BINARY:
                                case CB_USAGE_COMP_5:
                                case CB_USAGE_COMP_X:
                                case CB_USAGE_INDEX:
                                case CB_USAGE_LENGTH:
                                case CB_USAGE_BIT:
                                    x->category = CB_CATEGORY_NUMERIC;
                                    break;
                                default:
                                    break;
                            }
                        } else if ( f->pic )
                            x->category = f->pic->category;
                        break;
                }
            }
            break;
        case CB_TAG_ALPHABET_NAME:
        case CB_TAG_LOCALE_NAME:
            x->category = CB_CATEGORY_ALPHANUMERIC;
            break;
        case CB_TAG_BINARY_OP:
            x->category = CB_CATEGORY_BOOLEAN;
            break;
        case CB_TAG_OBJECT_LIST:
            x->category = CB_CATEGORY_ALPHANUMERIC;
            break;
        case CB_TAG_CONST:
            if ( CB_CONST(x)->num_type == CB_CONST_FLDADDR ) {
                x->category = CB_CATEGORY_UNKNOWN;
                return x->category;
            }
            /*fall through*/
        default:
            fprintf (stderr, "cobc:0: Unknown tree tag %d Category %d\n", CB_TREE_TAG (x), x->category);
            ABORT ();
    }

    return x->category;
}

int
cb_tree_type (cb_tree x) {
    struct cb_field *f;

    f = cb_field (x);
    if ( !f ) {
        return 0;
    }
    if ( f->children ) {
        return COB_TYPE_GROUP;
    }

    switch ( CB_TREE_CATEGORY (x) ) {
        case CB_CATEGORY_ALPHABETIC:
        case CB_CATEGORY_ALPHANUMERIC:
            return COB_TYPE_ALPHANUMERIC;
        case CB_CATEGORY_ALPHANUMERIC_EDITED:
            return COB_TYPE_ALPHANUMERIC_EDITED;
        case CB_CATEGORY_BIT:
            switch ( f->usage ) {
                case CB_USAGE_DISPLAY:
                    return COB_TYPE_ALPHANUMERIC_BITS;
                case CB_USAGE_BINARY:
                case CB_USAGE_COMP_5:
                case CB_USAGE_COMP_X:
                case CB_USAGE_INDEX:
                case CB_USAGE_LENGTH:
                    return COB_TYPE_NUMERIC_BITS;
                default:
                    fprintf (stderr, "cobc:0: Unexpected Bits field usage %s-> %d\n", f->name, f->usage);
                    ABORT ();
            }
            enterprise_error("PIC 1(n)" );
            break;
        case CB_CATEGORY_NUMERIC:
            if ( f->pic && f->pic->category == CB_CATEGORY_BIT ) {
                switch ( f->usage ) {
                    case CB_USAGE_DISPLAY:
                        return COB_TYPE_ALPHANUMERIC_BITS;
                    case CB_USAGE_BINARY:
                    case CB_USAGE_COMP_5:
                    case CB_USAGE_COMP_X:
                    case CB_USAGE_INDEX:
                    case CB_USAGE_LENGTH:
                        return COB_TYPE_NUMERIC_BITS;
                    case CB_USAGE_BIT:
                        return COB_TYPE_BITS;
                        break;
                    default:
                        fprintf (stderr, "cobc:0: Unexpected Bits field usage %s-> %d\n", f->name, f->usage);
                        ABORT ();
                }
                enterprise_error("PIC 1(n)" );
            }
            switch ( f->usage ) {
                case CB_USAGE_DISPLAY:
                    return COB_TYPE_NUMERIC_DISPLAY;
                case CB_USAGE_BINARY:
                case CB_USAGE_COMP_5:
                case CB_USAGE_COMP_X:
                case CB_USAGE_INDEX:
                case CB_USAGE_LENGTH:
                    return COB_TYPE_NUMERIC_BINARY;
                case CB_USAGE_FLOAT:
                    return COB_TYPE_NUMERIC_FLOAT;
                case CB_USAGE_DOUBLE:
                    return COB_TYPE_NUMERIC_DOUBLE;
                case CB_USAGE_PACKED:
                case CB_USAGE_COMP_6:
                    return COB_TYPE_NUMERIC_PACKED;
                default:
                    fprintf (stderr, "cobc:0: Unexpected numeric usage %s-> %d\n", f->name, f->usage);
                    ABORT ();
            }
        case CB_CATEGORY_NUMERIC_EDITED:
            return COB_TYPE_NUMERIC_EDITED;
        case CB_CATEGORY_OBJECT_REFERENCE:
        case CB_CATEGORY_DATA_POINTER:
        case CB_CATEGORY_PROGRAM_POINTER:
            return COB_TYPE_NUMERIC_BINARY;
        case CB_CATEGORY_NATIONAL:
            return COB_TYPE_NATIONAL;
        case CB_CATEGORY_NATIONAL_EDITED :
            return COB_TYPE_NATIONAL_EDITED;

        default:
            fprintf (stderr, "cobc:0: Unexpected category -> %d\n", CB_TREE_CATEGORY (x));
            ABORT ();
    }
/* NOT REACHED */
    return 0;
}

int
cb_fits_one_positive_digit (cb_tree x) {
    struct cb_literal       *l;
    struct cb_field         *f;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LITERAL:
            l = CB_LITERAL (x);
            if ( (l->scale == 0) && (l->size <= 1) && (l->sign > 0)) {
                if (cb_get_int(x) < 0) {
                    return 0;
                }
                if ( !isdigit(l->data[0]) ) {
                    return 0;
                }
                return 1;
            }
            return 0;
        case CB_TAG_FIELD:
            f = CB_FIELD (x);
            switch ( f->usage ) {
                case CB_USAGE_DISPLAY:
                    if ( f->size <= 1 ) {
                        if ( !f->pic || (f->pic->scale == 0 && !f->pic->have_sign)) {
                            return 1;
                        }
                    }
                    return 0;
                case CB_USAGE_PACKED:
                case CB_USAGE_COMP_6:
                    if ( f->pic->scale == 0 && f->pic->digits <= 1 && !f->pic->have_sign) {
                        return 1;
                    }
                    return 0;
                default:
                    return 0;
            }
        case CB_TAG_REFERENCE:
            return cb_fits_one_positive_digit (CB_REFERENCE (x)->value);
        default:
            return 0;
    }
}

int
cb_fits_int (cb_tree x) {
    struct cb_literal       *l;
    struct cb_field         *f;
    int                      i;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LITERAL:
            l = CB_LITERAL (x);
            if ( l->scale <= 0 && l->size < 10 ) {
                for ( i = 0; i < l->size ; i++ ) {
                    if ( !isdigit(l->data[i]) ) {
                        return 0;
                    }
                }
                return 1;
            }
            return 0;
        case CB_TAG_FIELD:
            f = CB_FIELD (x);
            switch ( f->usage ) {
                case CB_USAGE_INDEX:
                case CB_USAGE_LENGTH:
                case CB_USAGE_BIT:
                    return 1;
                case CB_USAGE_BINARY:
                case CB_USAGE_COMP_5:
                case CB_USAGE_COMP_X:
                    if ( f->pic->scale <= 0 && f->size <= (int)sizeof (int) ) {
                        return 1;
                    }
                    return 0;
                case CB_USAGE_DISPLAY:
                    if ( f->size < 10 ) {
                        if ( !f->pic || f->pic->scale <= 0 ) {
                            return 1;
                        }
                    }
                    return 0;
                case CB_USAGE_PACKED:
                case CB_USAGE_COMP_6:
                    if ( f->pic->scale <= 0 && f->pic->digits < 10 ) {
                        return 1;
                    }
                    return 0;
                default:
                    return 0;
            }
        case CB_TAG_REFERENCE:
            return cb_fits_int (CB_REFERENCE (x)->value);
        default:
            return 0;
    }
}

int
cb_fits_long_long (cb_tree x) {
    struct cb_literal       *l;
    struct cb_field         *f;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LITERAL:
            l = CB_LITERAL (x);
            if ( l->scale <= 0 && l->size < 19 ) {
                return 1;
            }
            return 0;
        case CB_TAG_FIELD:
            f = CB_FIELD (x);
            switch ( f->usage ) {
                case CB_USAGE_INDEX:
                case CB_USAGE_LENGTH:
                    return 1;
                case CB_USAGE_BINARY:
                case CB_USAGE_COMP_5:
                case CB_USAGE_COMP_X:
                    if ( f->pic->scale <= 0 && f->size <= (int)sizeof (long long) ) {
                        return 1;
                    }
                    return 0;
                case CB_USAGE_DISPLAY:
                    if ( f->pic->scale <= 0 && f->size < 19 ) {
                        return 1;
                    }
                    return 0;
                default:
                    return 0;
            }
        case CB_TAG_REFERENCE:
            return cb_fits_long_long (CB_REFERENCE (x)->value);
        default:
            return 0;
    }
}

int
cb_is_double (cb_tree x) {
    struct cb_literal       *l;
    struct cb_field         *f;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LITERAL:
            l = CB_LITERAL (x);
            if (CB_TREE_CLASS (x) == CB_CLASS_NUMERIC && 
                l->size < 19) {  /*use get_long_long to when reading a double */
                return 1;
            }
            return 0;
        case CB_TAG_FIELD:
            f = CB_FIELD (x);
            switch ( f->usage ) {
                case CB_USAGE_FLOAT:
                case CB_USAGE_DOUBLE:
                    return 1;
                default:
                    return 0;
            }
        case CB_TAG_REFERENCE:
            return cb_is_double (CB_REFERENCE (x)->value);
        default:
            return 0;
    }
}

int
cb_get_int (cb_tree x) {
    struct cb_literal       *l;
    size_t                  i;
    int                     val = 0;

    l = CB_LITERAL (x);
    for ( i = 0; i < l->size; i++ ) {
        if ( l->data[i] != '0' ) {
            break;
        }
    }

/* RXWRXW
        if (l->size - i >= 10) {
                ABORT ();
        }
*/

    for ( ; i < l->size; i++ ) {
        val = val * 10 + l->data[i] - '0';
    }
    if ( l->sign < 0 ) {
        val = -val;
    }
    return val;
}

double
cb_get_double (cb_tree x) {
    struct cb_literal       *l;
    double                  val;
    int i;

    l = CB_LITERAL (x);
    val = (double)cb_get_long_long(x);
    if ( l->scale > 0 ) {
        for ( i=0 ; i < (l->scale) ; i++ ) {
            val = val / (double) 10.0;
        }

    }if ( l->scale < 0 ) {
        for ( i=0 ; i < -(l->scale) ; i++ ) {
            val = val * (double) 10.0;
        }
    }

    return val;
}

long long
cb_get_long_long (cb_tree x) {
    struct cb_literal       *l;
    size_t                  i;
    long long               val = 0;

    l = CB_LITERAL (x);
    for ( i = 0; i < l->size; i++ ) {
        if ( l->data[i] != '0' ) {
            break;
        }
    }

    if ( l->size - i >= 19 ) {
        ABORT ();
    }

    for ( ; i < l->size; i++ ) {
        val = val * 10 + l->data[i] - '0';
    }
    if ( l->sign < 0 ) {
        val = -val;
    }
    return val;
}

void
cb_init_constants (void) {
    char    *s;
    int     i;
    char buff[5];

    buff[0] = cb_quote_char;
    buff[1] = 0;
    cb_error_node = make_constant (CB_CATEGORY_UNKNOWN, NULL);
    cb_any = make_constant (CB_CATEGORY_UNKNOWN, NULL);
    cb_positive = make_constant(CB_CATEGORY_UNKNOWN, "positive");
    cb_not_positive = make_constant(CB_CATEGORY_UNKNOWN, "not positive");
    cb_negative = make_constant(CB_CATEGORY_UNKNOWN, "negative");
    cb_not_negative = make_constant(CB_CATEGORY_UNKNOWN, "not negative");
    cb_numeric = make_constant(CB_CATEGORY_UNKNOWN, "numeric");
    cb_not_numeric = make_constant(CB_CATEGORY_UNKNOWN, "not numeric");
    cb_alphabetic = make_constant(CB_CATEGORY_UNKNOWN, "alphabetic");
    cb_not_alphabetic = make_constant(CB_CATEGORY_UNKNOWN, "not alphabetic");
    cb_alphabetic_lower = make_constant(CB_CATEGORY_UNKNOWN, "alphabetic lower");
    cb_not_alphabetic_lower = make_constant(CB_CATEGORY_UNKNOWN, "not alphabetic lower");
    cb_alphabetic_upper = make_constant(CB_CATEGORY_UNKNOWN, "alphabetic upper");
    cb_not_alphabetic_upper = make_constant(CB_CATEGORY_UNKNOWN, "not alphabetic upper");
    cb_true = make_constant (CB_CATEGORY_BOOLEAN, "1");
    cb_false = make_constant (CB_CATEGORY_BOOLEAN, "0");
    cb_null = make_constant (CB_CATEGORY_DATA_POINTER, "0");
    cb_zero = make_constant (CB_CATEGORY_NUMERIC, "&cob_zero");
    cb_one = make_constant (CB_CATEGORY_NUMERIC, "&cob_one");
    cb_space = make_constant (CB_CATEGORY_ALPHANUMERIC, "&cob_space");
    cb_low = make_constant (CB_CATEGORY_ALPHANUMERIC, "&cob_low");
    cb_norm_low = cb_low;
    cb_high = make_constant (CB_CATEGORY_ALPHANUMERIC, "&cob_high");
    cb_norm_high = cb_high;
    /* cb_quote = make_constant (CB_CATEGORY_ALPHANUMERIC, "&cob_quote");*/
    cb_quote = cb_build_alphanumeric_literal((unsigned char*)buff, 1, 0);
#ifdef _MSC_VER
    cb_dirsep = cb_build_alphanumeric_literal((unsigned char*)"\\", 1, 0);
#else
    cb_dirsep = cb_build_alphanumeric_literal((unsigned char*)"/", 1, 0);
#endif

    CB_LITERAL(cb_quote)->all = 1;
    cb_int0 = cb_int (0);
    cb_int1 = cb_int (1);
    cb_int2 = cb_int (2);
    cb_int3 = cb_int (3);
    cb_int4 = cb_int (4);
    cb_int5 = cb_int (5);
    for ( i = 1; i < 8; i++ ) {
        s = cobc_malloc (4);
        sprintf (s, "i%d", i);
        cb_i[i] = make_constant (CB_CATEGORY_NUMERIC, s);
    }
    cb_standard_error_handler   = make_constant_label ("Default Error Handler");
    cb_save_context_handler_ref = cb_build_reference ("Save Context Handler");
    cb_save_context_handler     = cb_build_label (cb_save_context_handler_ref, NULL);
    CB_LABEL (cb_save_context_handler)->need_begin = 1;
}

/*
 * Field List
 */

cb_tree
cb_build_object_list (cb_tree name, cb_tree list, enum cb_object_list_type type)
{
    struct cb_object_list *p;

    p = make_tree (CB_TAG_OBJECT_LIST, CB_CATEGORY_UNKNOWN, sizeof (struct cb_object_list));
    p->type  = type;
    p->olist = list;
    p->name = cb_define (name, CB_TREE (p));
    p->cname = cb_to_cname (p->name, strdup(p->name));
    return CB_TREE (p);
}

/*
 * Entry definition
 */

cb_tree
cb_build_entry (cb_tree label, cb_tree using_list, cb_tree returning, int call_conv)
{
    struct cb_entry *p;

    p = make_tree (CB_TAG_ENTRY, CB_CATEGORY_UNKNOWN, sizeof (struct cb_entry));
    p->label      =label      ;
    p->using_list =using_list ;
    p->returning  =returning  ;
    p->call_conv  =call_conv  ;
    return CB_TREE (p);
}

/*
 * List
 */

cb_tree
cb_build_list (cb_tree purpose, cb_tree value, cb_tree rest) {
    struct cb_list *p;

    p = make_tree (CB_TAG_LIST, CB_CATEGORY_UNKNOWN, sizeof (struct cb_list));
    p->purpose = purpose;
    p->value = value;
    p->chain = rest;
    return CB_TREE (p);
}

cb_tree
cb_list_append (cb_tree l1, cb_tree l2) {
    cb_tree l;

    if ( l1 == NULL || l1 == cb_error_node) {
        return l2;
    } else {
        l = l1;
        while ( CB_CHAIN (l) ) {
            l = CB_CHAIN (l);
        }
        if (l2 != cb_error_node) {
            CB_CHAIN (l) = l2;
        }
        return l1;
    }
}

cb_tree
cb_list_remove(cb_tree l0, cb_tree x) {
    cb_tree l;
    cb_tree prev = NULL;
    if (l0 == cb_error_node || x == cb_error_node) {
        return l0;
    }

    for (l = l0; l; l = CB_CHAIN(l)) {
        if (CB_VALUE(l) == x) {
            if (prev == NULL) {
                return CB_CHAIN(l);
            } else {
                CB_CHAIN(prev) = CB_CHAIN(l);
                return l0;
            }
        }
        prev = l;
    }
    return l0;
}

cb_tree
cb_list_append_contents (cb_tree l1, cb_tree l2)
{
    cb_tree l;

    for (l = l2; l ; l=CB_CHAIN(l)) {
        l1 = cb_list_add(l1, CB_VALUE(l));
    }
    return l1;
}

cb_tree
cb_list_add (cb_tree l, cb_tree x) {
    if ( x ) {
        return cb_list_append (l, cb_list_init (x));
    } else {
        return l;
    }
}

cb_tree
cb_list_insert_first (cb_tree l, cb_tree x) {
    return cb_list_append (cb_list_init (x), l);
}

cb_tree
cb_list_insert_after (cb_tree l, cb_tree x) {
    cb_tree sl;
    cb_tree l2;

    l2 = cb_list_init (x);
    sl = CB_CHAIN (l);
    CB_CHAIN (l) = l2;
    CB_CHAIN (l2) = sl;
    return l2;
}

cb_tree
cb_list_reverse (cb_tree l) {
    cb_tree next;
    cb_tree last = NULL;

    for ( ; l; l = next ) {
        next = CB_CHAIN (l);
        CB_CHAIN (l) = last;
        last = l;
    }
    return last;
}

int
cb_list_length (cb_tree l) {
    int n = 0;

    for ( ; l; l = CB_CHAIN (l) ) {
        n++;
    }
    return n;
}

void
cb_list_map (cb_tree (*func) (cb_tree x), cb_tree l) {
    for ( ; l; l = CB_CHAIN (l) ) {
        CB_VALUE (l) = func (CB_VALUE (l));
    }
}

cb_tree 
cb_list_join (cb_tree lx, cb_tree ly){
    cb_tree l =  NULL;

    for ( ; lx || ly; ) {
        l = cb_list_add(l,cb_build_pair(lx ? CB_VALUE(lx) : NULL, ly ? CB_VALUE(ly) : NULL));
        if ( lx ) {
            lx = CB_CHAIN (lx);
        }
        if ( ly ) {
            ly = CB_CHAIN (ly);
        }
    }
    return l;
}

/*
 * Program
 */

struct cb_program *
cb_build_program (struct cb_program *last_program, int nest_level) {
    struct cb_program *p;

    cb_reset_78 ();
    cb_reset_in_procedure ();
    cb_clear_real_field ();
    p = cobc_malloc (sizeof (struct cb_program));
    p->next_program = last_program;
    p->nested_level = nest_level;
    p->decimal_point = '.';
    p->currency_symbol = '$';
    p->numeric_separator = ',';
    p->flag_sign_leading = cb_flag_sign_leading; 
    p->flag_sign_separate= cb_flag_sign_separate;
    if ( nest_level ) {
        p->global_file_list = last_program->global_file_list;
        p->collating_sequence = last_program->collating_sequence;
        p->function_spec_list = last_program->function_spec_list;
        p->class_spec_list = last_program->class_spec_list;
        p->interface_spec_list = last_program->interface_spec_list;
        p->program_spec_list = last_program->program_spec_list;
        p->property_spec_list = last_program->property_spec_list;
        p->alphabet_name_list = last_program->alphabet_name_list;
        p->class_name_list = last_program->class_name_list;
        p->locale_list = last_program->locale_list;
        p->symbolic_list = last_program->symbolic_list;
        p->decimal_point = last_program->decimal_point;
        p->numeric_separator = last_program->numeric_separator;
        p->currency_symbol = last_program->currency_symbol;
        p->cb_return_code = last_program->cb_return_code;
        p->cb_context_filename = last_program->cb_context_filename;
        p->cb_context_code = last_program->cb_context_code;
        p->flag_sign_leading = last_program->flag_sign_leading  ;
        p->flag_sign_separate= last_program->flag_sign_separate ;
    } else {
        functions_are_all = cb_flag_functions_all;
    }
    return p;
}

/*
 * Integer
 */

cb_tree
cb_int (int n) {
    struct cb_integer       *x;
    struct int_node         *p;

    for ( p = int_node_table; p; p = p->next ) {
        if ( p->n == n ) {
            return p->node;
        }
    }

    x = make_tree (CB_TAG_INTEGER, CB_CATEGORY_NUMERIC, sizeof (struct cb_integer));
    x->val = n;

    p = cobc_malloc (sizeof (struct int_node));
    p->n = n;
    p->node = CB_TREE (x);
    p->next = int_node_table;
    int_node_table = p;
    return p->node;
}

/*
 * String
 */

cb_tree
cb_build_string (const unsigned char *data, size_t size) {
    struct cb_string *p;

    p = make_tree (CB_TAG_STRING, CB_CATEGORY_ALPHANUMERIC, sizeof (struct cb_string));
    p->size = size;
    p->data = data;
    return CB_TREE (p);
}

/*
 * Alphabet-name
 */

cb_tree
cb_build_alphabet_name (cb_tree name, enum cb_alphabet_name_type type) {
    struct cb_alphabet_name *p;

    p = make_tree (CB_TAG_ALPHABET_NAME, CB_CATEGORY_UNKNOWN, sizeof (struct cb_alphabet_name));
    p->name = cb_define (name, CB_TREE (p));
    p->cname = cb_to_cname (p->name, strdup(p->name));
    p->type = type;
    return CB_TREE (p);
}

/*
 * Class-name
 */

cb_tree
cb_build_class_name (cb_tree name, cb_tree list) {
    struct cb_class_name    *p;
    char            buff[COB_MINI_BUFF];

    p = make_tree (CB_TAG_CLASS_NAME, CB_CATEGORY_BOOLEAN, sizeof (struct cb_class_name));
    p->name = cb_define (name, CB_TREE (p));
    snprintf (buff, COB_MINI_MAX, "is_%s", cb_to_cname (p->name, NULL));
    p->cname = strdup (buff);
    p->list = list;
    return CB_TREE (p);
}

/*
 * Locale-name
 */

cb_tree
cb_build_locale_name (cb_tree name, cb_tree list) {
    struct cb_class_name    *p;

    p = make_tree (CB_TAG_LOCALE_NAME, CB_CATEGORY_UNKNOWN, sizeof (struct cb_locale_name));
    p->name = cb_define (name, CB_TREE (p));
    p->cname = cb_to_cname (p->name, strdup(p->name));
    p->list = list;
    return CB_TREE (p);
}

/*
 * System-name
 */

cb_tree
cb_build_system_name (enum cb_system_name_category category, int token) {
    struct cb_system_name *p;

    p = make_tree (CB_TAG_SYSTEM_NAME, CB_CATEGORY_UNKNOWN, sizeof (struct cb_system_name));
    p->category = category;
    p->token = token;
    return CB_TREE (p);
}

/*
 * Literal
 */

cb_tree
cb_build_numeric_literal (int sign, const unsigned char *data, int scale, const char *image) {
    struct cb_literal *p;

    p = build_literal (CB_CATEGORY_NUMERIC, data, strlen ((char *)data));
    p->sign = (char)sign;
    p->scale = (char)scale;
    if (image) {
        p->image = (unsigned char*)strdup((char*)image);
    }
    return CB_TREE (p);
}

cb_tree
cb_build_alphanumeric_literal (const unsigned char *data, size_t size, int national) {
    if ( national ) {
        return CB_TREE (build_literal (CB_CATEGORY_NATIONAL, data, size));
    } else {
        return CB_TREE (build_literal (CB_CATEGORY_ALPHANUMERIC, data, size));
    }
}

cb_tree
cb_concat_literals (cb_tree x1, cb_tree x2) {
    unsigned char           *buff;
    cb_tree                 x;
    unsigned char           *data1;
    char                    buffquote1[5];
    unsigned char           *data2;
    char                    buffquote2[5];
    size_t                  size1;
    size_t                  size2;

    if ( x1 == cb_error_node || x2 == cb_error_node ) {
        return cb_error_node;
    }
    if ( CB_LITERAL_P (x1) ) {
        data1 = CB_LITERAL (x1)->data;
        size1 = CB_LITERAL (x1)->size;
    } else if ( CB_CONST_P (x1) ) {
        size1 = 1;
        if ( x1 == cb_space ) {
            data1 = (unsigned char *)" ";
        } else if ( x1 == cb_zero ) {
            data1 = (unsigned char *)"0";
        } else if ( x1 == cb_quote ) {
            buffquote1[0] = cb_quote_char;
            buffquote1[1] = 0;
            data1 = (unsigned char *)buffquote1;
        } else if ( x1 == cb_dirsep ) {
#ifdef _MSC_VER
            data1 = (unsigned char *)"\\";
#else
            data1 = (unsigned char *)"/";
#endif
        } else if ( x1 == cb_norm_low ) {
            data1 = (unsigned char *)"\0";
        } else if ( x1 == cb_norm_high ) {
            data1 = (unsigned char *)"\255";
        } else if ( x1 == cb_null ) {
            data1 = (unsigned char *)"\0";
        } else {
            return cb_error_node;
        }
    } else {
        return cb_error_node;
    }
    if ( CB_LITERAL_P (x2) ) {
        data2 = CB_LITERAL (x2)->data;
        size2 = CB_LITERAL (x2)->size;
    } else if ( CB_CONST_P (x2) ) {
        size2 = 1;
        if ( x2 == cb_space ) {
            data2 = (unsigned char *)" ";
        } else if ( x2 == cb_zero ) {
            data2 = (unsigned char *)"0";
        } else if ( x2 == cb_quote ) {
            buffquote2[0] = cb_quote_char;
            buffquote2[1] = 0;
            data2 = (unsigned char *)buffquote2;
        } else if ( x2 == cb_norm_low ) {
            data2 = (unsigned char *)"\0";
        } else if ( x2 == cb_norm_high ) {
            data2 = (unsigned char *)"\255";
        } else if ( x2 == cb_null ) {
            data2 = (unsigned char *)"\0";
        } else {
            return cb_error_node;
        }
    } else {
        return cb_error_node;
    }
    buff = cobc_malloc (size1 + size2 + 3);
    memcpy (buff, data1, size1);
    memcpy (buff + size1, data2, size2);
    x = cb_build_alphanumeric_literal (buff, size1 + size2, 
                                       CB_TREE_CLASS(x1) == CB_CLASS_NATIONAL || CB_TREE_CLASS(x2) == CB_CLASS_NATIONAL);
    free (buff);
    return x;
}


cb_tree         
cb_a2e_literal (cb_tree x)
{
    if (CB_LITERAL_P(x)  ) {
        struct cb_literal *l = CB_LITERAL(x);
        if (!l->encoded) {
            if ( cb_source_codepage && cb_codepage && cb_source_codepage != cb_codepage) {
                char * target = cobc_malloc (l->size *4);
                int tsz = l->size *4;
                l->size = enterprise_sourcecp_to_runtimecp((char*)l->data,target, l->size, tsz);
                l->data = (unsigned char*)target;
                l->encoded = 1; 
            } else {
                if (cb_flag_ebcdic_charset) {
                    STRING_A2E(l->data, l->size);
                    l->encoded = 1;
                }
            }        
        }
    }
    return x;
}

/*
 * Decimal
 */

cb_tree
cb_build_decimal (int id) {
    struct cb_decimal *p;

    p = make_tree (CB_TAG_DECIMAL, CB_CATEGORY_NUMERIC, sizeof (struct cb_decimal));
    p->id = id;
    p->intermediate = 0;
    /*CIT*/
    p->binary = none_DBT;
    return CB_TREE (p);
}

/*
 * Picture
 */

cb_tree
cb_build_picture (const char *str) {
    struct cb_picture   *pic;
    const char          *p;
    size_t              idx = 0;
    size_t              buffcnt = 0;
    size_t              at_beginning;
    size_t              at_end;
    size_t              p_char_seen;
    size_t              s_char_seen;
    int                 category = 0;
    int                 size = 0;
    int                 allocated = 0;
    int                 digits = 0;
    int                 scale = 0;
    int                 c_count = 0;
    int                 s_count = 0;
    int                 v_count = 0;
    int                 i;
    int                 n;
    unsigned char       c;
    unsigned char       lastonechar = 0;
    unsigned char       lasttwochar = 0;
    unsigned char       buff[COB_SMALL_BUFF];

    pic = make_tree (CB_TAG_PICTURE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_picture));
    if ( strlen (str) > 50 ) {
        goto error;
    }
    memset (buff, 0, sizeof (buff));
    p_char_seen = 0;
    s_char_seen = 0;
    for ( p = str; *p; p++ ) {
        n = 1;
        c = *p;
        repeat:
        /* count the number of repeated chars */
        while ( p[1] == c ) {
            p++, n++;
        }

        /* add parenthesized numbers */
        if ( p[1] == '(' ) {
            i = 0;
            p += 2;
            for ( ; *p == '0'; p++ ) {
                ;
            }
            for ( ; *p != ')'; p++ ) {
                if ( !isdigit (*p) ) {
                    goto error;
                } else {
                    allocated++;
                    if ( allocated > 9 ) {
                        goto error;
                    }
                    i = i * 10 + (*p - '0');
                }
            }
            if ( i == 0 ) {
                goto error;
            }
            n += i - 1;
            goto repeat;
        }

        /* check grammar and category */
        /* FIXME: need more error check */
        switch ( c ) {
            case 'A':
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                category |= PIC_ALPHABETIC;
                break;

            case 'X':
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                category |= PIC_ALPHANUMERIC;
                break;

            case '1':
                category |= PIC_BIT;
                digits += n;
                break;

            case '9':
                category |= PIC_NUMERIC;
                digits += n;
                if ( v_count ) {
                    scale += n;
                }
                break;

            case 'N':
            case 'G':
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                category |= PIC_NATIONAL;
                break;

            case 'S':
                category |= PIC_NUMERIC;
                if ( category & PIC_ALPHABETIC ) {
                    goto error;
                }
                s_count += n;
                if ( s_count > 1 || idx != 0 ) {
                    goto error;
                }
                s_char_seen = 1;
                continue;

            case ',':
            case '.':
                category |= PIC_NUMERIC_EDITED;
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                if ( c != current_program->decimal_point ) {
                    break;
                }
                /* fall through */
            case 'V':
                category |= PIC_NUMERIC;
                if ( category & PIC_ALPHABETIC ) {
                    goto error;
                }
                v_count += n;
                if ( v_count > 1 ) {
                    goto error;
                }
                break;

            case 'P':
                category |= PIC_NUMERIC;
                if ( category & PIC_ALPHABETIC ) {
                    goto error;
                }
                if ( p_char_seen ) {
                    goto error;
                }
                at_beginning = 0;
                at_end = 0;
                switch ( buffcnt ) {
                    case 0:
                        /* P..... */
                        at_beginning = 1;
                        break;
                    case 1:
                        /* VP.... */
                        /* SP.... */
                        if ( lastonechar == 'V' || lastonechar == 'S' ) {
                            at_beginning = 1;
                        }
                        break;
                    case 2:
                        /* SVP... */
                        if ( lasttwochar == 'S' && lastonechar == 'V' ) {
                            at_beginning = 1;
                        }
                        break;
                }
                if ( p[1] == 0 || (p[1] == 'V' && p[2] == 0) ) {
                    /* .....P */
                    /* ....PV */
                    at_end = 1;
                }
                if ( !at_beginning && !at_end ) {
                    goto error;
                }
                p_char_seen = 1;
                if ( at_beginning ) {
                    v_count++;      /* implicit V */
                }
                digits += n;
                if ( v_count ) {
                    scale += n;
                } else {
                    scale -= n;
                }
                break;

            case '0':
            case 'B':
            case '/':
                category |= PIC_EDITED;
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                break;

            case '*':
            case 'Z':
                category |= PIC_NUMERIC_EDITED;
                if ( category & PIC_ALPHABETIC ) {
                    goto error;
                }
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                digits += n;
                if ( v_count ) {
                    scale += n;
                }
                break;

            case '+':
            case '-':
                category |= PIC_NUMERIC_EDITED;
                if ( category & PIC_ALPHABETIC ) {
                    goto error;
                }
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                if ( s_count == 0 ) {
                    digits += n - 1;
                } else {
                    digits += n ;
                }
                /* FIXME: need more check */
                if ( v_count && s_count ) {
                    scale += n;
                }
                s_count++;
                break;

            case 'C':
                category |= PIC_NUMERIC_EDITED;
                if ( !(p[1] == 'R' && p[2] == 0) ) {
                    goto error;
                }
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                p++;
                s_count++;
                break;

            case 'D':
                category |= PIC_NUMERIC_EDITED;
                if ( !(p[1] == 'B' && p[2] == 0) ) {
                    goto error;
                }
                if ( s_char_seen || p_char_seen ) {
                    goto error;
                }
                p++;
                s_count++;
                break;

            default:
                if ( c == current_program->currency_symbol ) {
                    category |= PIC_NUMERIC_EDITED;
                    if ( c_count == 0 ) {
                        digits += n - 1;
                    } else {
                        digits += n ;
                    }
                    c_count++;
                    /* FIXME: need more check */
                    if ( v_count ) {
                        scale += n;
                    }
                    break;
                }

                goto error;
        }

        if ( (category & PIC_BIT) && (category != PIC_BIT) ) {
            goto error;
        }

        /* calculate size */
        if ( c != 'V' && c != 'P' ) {
            size += n;
        }
        if ( c == 'C' || c == 'D' || c == 'N' || c == 'G' ) {
            size += n;
        }

        /* store in the buffer */
        buff[idx++] = c;
        lasttwochar = lastonechar;
        lastonechar = c;
        memcpy (&buff[idx], (unsigned char *)&n, sizeof(int));
        idx += sizeof(int);
        ++buffcnt;
    }
    buff[idx] = 0;

    if ( size == 0 && v_count ) {
        goto error;
    }
    /* set picture */
    pic->orig = strdup (str);
    pic->size = size;
    pic->digits = (unsigned char)digits;
    pic->scale = (signed char)scale;
    pic->have_sign = (unsigned char)s_count;

    /* set picture category */
    switch ( category ) {
        case PIC_ALPHABETIC:
            pic->category = CB_CATEGORY_ALPHABETIC;
            break;
        case PIC_BIT:
            pic->category = CB_CATEGORY_BIT;
            if ( digits > COB_MAX_NUMBER_BIT ) {
                cb_error (_("Bits field cannot be larger than %d digits"), COB_MAX_NUMBER_BIT);
            }
            break;
        case PIC_NUMERIC:
            pic->category = CB_CATEGORY_NUMERIC;
            if ( digits > COB_MAX_NUMBER_DIGIT ) {
                cb_error (_("Numeric field cannot be larger than %d digits"), COB_MAX_NUMBER_DIGIT);
            }
            break;
        case PIC_ALPHANUMERIC:
            pic->category = CB_CATEGORY_ALPHANUMERIC;
            break;
        case PIC_NATIONAL:
            pic->category = CB_CATEGORY_NATIONAL;
            break;
        case PIC_NUMERIC_EDITED:
            pic->str = cobc_malloc (idx + 1);
            memcpy (pic->str, buff, idx);
            pic->category = CB_CATEGORY_NUMERIC_EDITED;
            pic->lenstr = idx;
            break;
        case PIC_EDITED:
        case PIC_ALPHABETIC_EDITED:
        case PIC_ALPHANUMERIC_EDITED:
            pic->str = cobc_malloc (idx + 1);
            memcpy (pic->str, buff, idx);
            pic->category = CB_CATEGORY_ALPHANUMERIC_EDITED;
            pic->lenstr = idx;
            break;
        case PIC_NATIONAL_EDITED:
            pic->str = cobc_malloc (idx + 1);
            memcpy (pic->str, buff, idx);
            pic->category = CB_CATEGORY_NATIONAL_EDITED;
            pic->lenstr = idx;
            break;
        default:
            goto error;
    }
    goto end;

    error:
    cb_error (_("Invalid picture string - '%s'"), str);

    end:
    return CB_TREE (pic);
}

/*
 * Field
 */

cb_tree
cb_build_field (cb_tree name) {
    struct cb_field *p;

    p = make_tree (CB_TAG_FIELD, CB_CATEGORY_UNKNOWN, sizeof (struct cb_field));
    p->id = cb_field_id++;
    p->name = cb_define (name, CB_TREE (p));
    p->ename = NULL;
    p->usage = CB_USAGE_DISPLAY;
    p->storage = CB_STORAGE_WORKING;
    p->flag_ebcdic_charset = cb_flag_ebcdic_charset;
    p->occurs_max = 1;
    return CB_TREE (p);
}

void 
cb_mark_as_non_skipable(cb_tree x)
{
    if (x && CB_REF_OR_FIELD_P(x)) {
        struct cb_field *f = cb_field(x);
        if (f) {
            f->flag_disable_usused_skip = 1;
        } else {
            cb_error_x(x,"Not a valid field");
        }
    }
}

cb_tree
cb_build_implicit_field (cb_tree name, int len, int external) {
    cb_tree x;
    char    pic[32];

    x = cb_build_field (name);
    memset (pic, 0, sizeof(pic));
    sprintf (pic, "X(%d)", len);
    CB_FIELD (x)->pic = CB_PICTURE (cb_build_picture (pic));
    CB_FIELD (x)->flag_external = external;
    CB_FIELD (x)->level = 1;
    cb_mark_as_non_skipable(x);
    cb_validate_field (CB_FIELD (x));
    return x;
}

cb_tree
cb_build_constant (cb_tree name, cb_tree value) {
    cb_tree x;

    x = cb_build_field (name);
    x->category = cb_tree_category (value);
    CB_FIELD (x)->storage = CB_STORAGE_CONSTANT;
    CB_FIELD (x)->values = cb_list_init (value);
    return x;
}

struct cb_field *
cb_field (cb_tree x) {
    if ( CB_REFERENCE_P (x) ) {
        cb_tree r = cb_ref (x);
        if ( CB_FIELD_P(r) ) {
            return CB_FIELD (r);
        }
    } else {
        if ( CB_FIELD_P(x) ) {
            return CB_FIELD (x);
        }
    }
    return NULL;
}


struct cb_field *
cb_field_add (struct cb_field *f, struct cb_field *p) {
    struct cb_field *t;

    if ( cb_flag_debug_parser ) {
        for ( t = p; t ; t = t->sister ) {
            printf("[DEBUG PARSER] add field %s\n", t->name);
        }
    }
    if ( f == NULL ) {
        return p;
    }
    for ( t = f; t->sister; t = t->sister ) {
        ;
    }
    t->sister = p;
    return f;
}

int
cb_field_size (cb_tree x) {
    struct cb_reference     *r;
    struct cb_field         *f;

    if (!x) {
        return 0;
    }
    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LITERAL:
            return CB_LITERAL (x)->size;
        case CB_TAG_FIELD:
            return CB_FIELD (x)->size;
        case CB_TAG_REFERENCE:
            r = CB_REFERENCE (x);
            f = CB_FIELD (r->value);

            if ( r->length ) {
                if ( CB_LITERAL_P (r->length) ) {
                    if ( ( f->pic && (f->pic->category == CB_CATEGORY_NATIONAL) )
                         /* f->usage == CB_USAGE_NATIONAL */ ) {
                        return 2 * cb_get_int (r->length);
                    }
                    return cb_get_int (r->length);
                } else {
                    return -1;
                }
            } else if ( r->offset ) {
                if ( CB_LITERAL_P (r->offset) ) {
                    if ( ( f->pic && (f->pic->category == CB_CATEGORY_NATIONAL) )
                         /* f->usage == CB_USAGE_NATIONAL */ ) {
                        return f->size - (cb_get_int (r->offset)*2) + 1;
                    }
                    return f->size - cb_get_int (r->offset) + 1;
                } else {
                    return -1;
                }
            } else {
                return f->size;
            }
        default:
            fprintf (stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG (x));
            ABORT ();
    }
/* NOT REACHED */
    return 0;
}

struct cb_field *
cb_field_founder (struct cb_field *f) {
    while ( f->parent ) {
        f = f->parent;
    }
    return f;
}

struct cb_field *
cb_field_variable_size (struct cb_field *f) {
    struct cb_field *p;
    struct cb_field *cf;


    if ( f->cache.cb_field_variable_size_set ) {
        return f->cache.cb_field_variable_size;
    }

    if ( f->usage == CB_USAGE_BIT && !f->children ) {
        return cb_field_variable_size(f->parent);
    }

    cf = f;
    cf->cache.cb_field_variable_size_set = 1;
    cf->cache.cb_field_variable_size = NULL;
    for ( f = f->children; f; f = f->sister ) {
        if ( !f->redefines ) {
            if ( f->occurs_depending ) {
                if ( cb_flag_odo_slide || (f->sister == NULL) ) {
                    cf->cache.cb_field_variable_size = f;
                    cf->flag_no_optimizable = 1;
                    return f;
                }
            } else if ( (p = cb_field_variable_size (f)) != NULL ) {
                if ( cb_flag_odo_slide || (f->sister == NULL) ) {
                    cf->cache.cb_field_variable_size = p;
                    cf->flag_no_optimizable = 1;
                    return p;
                }
            }
        }
    }
    return NULL;
}

struct cb_field *
cb_field_variable_address (struct cb_field *f) {
    struct cb_field *p;
    struct cb_field *cf;


    if ( !cb_flag_odo_slide ) {
        return NULL;
    }
    if ( f->cache.cb_field_variable_address_set ) {
        return f->cache.cb_field_variable_address;
    }

    if ( f->usage == CB_USAGE_BIT && !f->children ) {
        return cb_field_variable_address(f->parent);
    }

    cf = f;
    cf->cache.cb_field_variable_address_set = 1;
    cf->cache.cb_field_variable_address = NULL;
    for ( p = f->parent; p; f = f->parent, p = f->parent ) {
        for ( p = p->children; p != f; p = p->sister ) {
            if ( !p->redefines ) {
                if ( p->occurs_depending || cb_field_variable_size (p) ) {
                    cf->cache.cb_field_variable_address = p;
                    cf->flag_no_optimizable = 1;
                    return p;
                }
            }
        }
    }
    return NULL;
}

static void 
cb_update_field_brother (struct cb_field *p)
{
    struct cb_field *last = NULL;

    for ( last=NULL; p; last = p, p = p->sister ) {
        p->brother = last;
        if ( p->children ) {
            cb_update_field_brother(p->children);
        }
    }
}

void
cb_set_field_variable_address_cache (struct cb_field *f) {
    struct cb_field *p = NULL;

    if ( f ) {
        if ( f->parent ) {
            p=f->parent->cache.cb_field_variable_address;
        }
        for ( ; f; f = f->sister ) {
            if ( !f->parent ) {
                /*At Level 01 reset*/
                p = NULL;
                cb_update_field_brother(f->children);
            }
            f->cache.cb_field_variable_address_set = 1;
            f->cache.cb_field_variable_address = p;
            if ( !f->redefines ) {
                if ( cb_flag_odo_slide && (f->occurs_depending || cb_field_variable_size (f)) ) {
                    p = f;
                }
            }
            if ( f->children ) {
                cb_set_field_variable_address_cache(f->children);
            }
        }
    }

}

/* Return 1 if P is subordinate to F */

int
cb_field_subordinate (struct cb_field *p, struct cb_field *f) {
    for ( p = p->parent; p; p = p->parent ) {
        if ( p == f ) {
            return 1;
        }
    }
    return 0;
}

/*
 * File
 */

struct cb_file *
build_file (cb_tree name) {
    struct cb_file *p;
    cb_tree fn;
    char buffer [COB_SMALL_BUFF];

    fn = cb_build_file_reference(CB_NAME(name));
    fn->source_file = name->source_file;
    fn->source_line = name->source_line;

    p = make_tree (CB_TAG_FILE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_file));
    p->name = cb_define (fn, CB_TREE (p));
    p->oname = strdup(CB_NAME (name));
    p->cname = cb_to_cname (p->name, strdup(p->name));

    if (cb_flag_mem_info) {
        sprintf (buffer, "%s_status", p->name);
        p->debug_file_status = CB_FIELD(cb_ref (cb_build_dummy_linkage (cb_build_reference (buffer), 2)));
        p->debug_file_status->flag_hide_from_debug = 1;
    }

    /*CIT*/
    if ( cb_flag_sequential_line )
        p->organization = COB_ORG_LINE_SEQUENTIAL;
    else
        p->organization = COB_ORG_SEQUENTIAL;
    if ( cb_flag_autolock ) {
        p->lock_mode = COB_LOCK_AUTOMATIC;
    } else if ( cb_flag_manuallock ) {
        p->lock_mode = COB_LOCK_MANUAL;
    } else if ( cb_flag_exclusivelock ) {
        p->lock_mode = COB_LOCK_EXCLUSIVE;
    }
    p->access_mode = COB_ACCESS_SEQUENTIAL;
    p->handler = CB_LABEL (cb_standard_error_handler);
    p->handler_prog = current_program;
    return p;
}

void
validate_file (struct cb_file *f, cb_tree name) {
    /* check RECORD/RELATIVE KEY clause */
    switch ( f->organization ) {
        case COB_ORG_INDEXED:
            if ( f->key == NULL ) {
                file_error (name, "RECORD KEY");
            }
            break;
        case COB_ORG_RELATIVE:
            if ( f->key == NULL && f->access_mode != COB_ACCESS_SEQUENTIAL ) {
                file_error (name, "RELATIVE KEY");
            }
            break;
    }
    if ((f->sharing == cb_int(COB_SHARE_WITH_ALL)) && 
        ((f->lock_mode & COB_LOCK_MASK) == 0)) {
        if ( cb_flag_share_all_lock_auto ) {
            f->lock_mode = COB_LOCK_AUTOMATIC;
        }
        if ( cb_flag_share_all_lock_manual ) {
            f->lock_mode = COB_LOCK_MANUAL;
        }
    }
}

static int field_minum_size (struct cb_field *f)
{
    int s = 0;
    struct cb_field *p;
    if ( !f->redefines ) {
        if ( f->children ) {
            for ( p = f->children; p; p = p->sister ) {
                s = s + field_minum_size(p);
            }
            return s * (f->occurs_min ? f->occurs_min : 1);
        } else {
            return f->size * (f->occurs_min ? f->occurs_min : 1);
        }
    }
    return 0;
}

static void
file_set_recmode (struct cb_file *f) {
    if ( 1 ) {
        if ( cb_flag_recmode_f && f->rec_mode == CB_REC_MODE_DEFAULT) { 
            f->rec_mode = CB_REC_MODE_FIX;
        }
        if ( f->rec_mode == CB_REC_MODE_DEFAULT ) {
            if ( cb_flag_recmode_v ) {
                f->rec_mode = CB_REC_MODE_VARIABLE;
            } else {
                if ( f->organization == COB_ORG_LINE_SEQUENTIAL ) {
                    f->rec_mode = CB_REC_MODE_VARIABLE;
                }
                if ( cb_flag_recmode_osvs && (f->record_max != f->record_min ) ) {
                    f->rec_mode = CB_REC_MODE_VARIABLE;
                }
            }
        }
    } else {
        if ( f->record_max != f->record_min ) {
            f->rec_mode = CB_REC_MODE_VARIABLE;
        }
    }
}

void
finalize_file (struct cb_file *f, struct cb_field *records) {
    struct cb_field *p;
    struct cb_field *v;
    cb_tree         l;
    cb_tree         x;
    char        buff[COB_MINI_BUFF];
    int         alloc_max=0;

    if ( f->special ) {
        f->organization = COB_ORG_LINE_SEQUENTIAL;
    }
    if ( f->line_advancing_clause && 
         f->organization == COB_ORG_SEQUENTIAL ) {
        f->organization = COB_ORG_LINE_SEQUENTIAL;
        f->is_printer = 1;
    }
    if ( f->fileid_assign && !f->assign ) {
        f->assign = cb_build_alphanumeric_literal ((unsigned char *)f->oname,
                                                   strlen (f->oname), 0);
    }
    if ( f->is_printer && f->organization == COB_ORG_LINE_SEQUENTIAL ) {
        f->rec_mode     = CB_REC_MODE_VARIABLE;
        f->ls_mfmode    = 0;
    }
    if ( f->is_printer && f->organization == COB_ORG_SEQUENTIAL ) {
        f->rec_mode     = CB_REC_MODE_FIX;
        f->ls_mfmode    = 0;
    }

    if (!records && f->organization != COB_ORG_SORT) {
        cb_error_x(CB_TREE(f), _("FD missing"));
    }

    /* check the record size if it is limited */
    for (p = records; p; p = p->sister) {
        cb_mark_as_non_skipable(CB_TREE(p));              
        if ( f->record_min > 0 && !f->record_contain_clause) {
            if ( p->size < f->record_min ) {
                if ( cb_flag_mf_compat_parser ) {
                    cb_warning_x (CB_TREE(p), _("Record size too small '%s'"), p->name);
                } else {
                    cb_error_x (CB_TREE(p), _("Record size too small '%s'"), p->name);
                }
            }
        }
        if ( f->record_max > 0 ) {
            if ( p->size > f->record_max ) {
                if ( cb_flag_mf_compat_parser ) {
                    cb_warning_x (CB_TREE(p), _("Record size too large '%s' (%d)"),
                                  p->name, p->size);
                } else {
                    cb_error_x (CB_TREE(p), _("Record size too large '%s' (%d)"),
                                p->name, p->size);
                }
            }
        }
        if (p->size > alloc_max) {
            alloc_max = p->size;
        }
    }

    /* compute the record size */
    if ( f->record_min == 0 ) {
        if ( records  && f->rec_mode != CB_REC_MODE_VARIABLE ) {
            f->record_min = records->size;
        } else {
            f->record_min = 0;
        }
    }
    if (!f->record_contain_clause) {
        for ( p = records; p; p = p->sister ) {
            v = cb_field_variable_size (p);
            if ( v ) {
                int s = field_minum_size(p);
                if ( s < f->record_min ) {
                    f->record_min = s;
                }
                if ( s > f->record_max ) {
                    f->record_max = s;
                }
                if (s > alloc_max) {
                    alloc_max = s;
                }
            }
            /*
            if ( v && v->offset + v->size * v->occurs_min < f->record_min ) {
                f->record_min = v->offset + v->size * v->occurs_min;
            } 
            */
            if ( p->size < f->record_min ) {
                f->record_min = p->size;
            }
            if ( p->size > f->record_max ) {
                f->record_max = p->size;
            }
        }
    }

    if ( f->same_clause ) {
        for ( l = current_program->file_list; l; l = CB_CHAIN (l) ) {
            if ( CB_FILE (CB_VALUE (l))->same_clause == f->same_clause ) {
                if ( CB_FILE (CB_VALUE (l))->finalized ) {
                    if ( f->record_max > CB_FILE (CB_VALUE (l))->record->memory_size ) {
                        CB_FILE (CB_VALUE (l))->record->memory_size =
                        f->record_max;
                    }
                    f->record = CB_FILE (CB_VALUE (l))->record;
                    for ( p = records; p; p = p->sister ) {
                        p->file = f;
                        p->redefines = f->record;
                    }
                    for ( p = f->record->sister; p; p = p->sister ) {
                        if ( !p->sister ) {
                            p->sister = records;
                            break;
                        }
                    }
                    f->finalized = 1;
                    cb_set_field_variable_address_cache(f->record);
                    file_set_recmode(f);
                    return;
                }
            }
        }
    }
    /* create record */
    snprintf (buff, COB_MINI_MAX, "%s_record", f->name);
    if ( f->record_max == 0 ) {
        f->record_max = 32;
        f->record_min = 32;
    }
    if ( f->organization == COB_ORG_LINE_SEQUENTIAL ) {
        f->record_min = 0;
    }
    if (f->record_max > alloc_max) {
        alloc_max = f->record_max;
    }
    if (cb_flag_read_into_copy) {
        cb_tree x;
        cb_tree y;
        char pic[32];
        char buff2[COB_MINI_BUFF];

        x = cb_build_field (cb_build_reference(buff));
        CB_FIELD (x)->flag_external = f->external;
        CB_FIELD (x)->level = 1;
        cb_mark_as_non_skipable(x);
        f->record = CB_FIELD (x);

        snprintf (buff2, COB_MINI_MAX, "%s_record2", f->name);
        y = cb_build_field (cb_build_reference(buff2));
        memset (pic, 0, sizeof(pic));
        sprintf (pic, "X(%d)", alloc_max);
        CB_FIELD (y)->pic = CB_PICTURE (cb_build_picture (pic));
        CB_FIELD (y)->level = 2;
        cb_mark_as_non_skipable(y);

        CB_FIELD (x)->children = CB_FIELD(y);
        cb_validate_field (CB_FIELD (x));
        cb_validate_field (CB_FIELD (y));
    }
    else 
        f->record = CB_FIELD(cb_build_implicit_field(cb_build_reference(buff),
        alloc_max, f->external));
    f->record->sister = records;
    f->record->count++;
    if ( f->external ) {
        has_external = 1;
        f->record->flag_external = 1;
    }

    for ( p = records; p; p = p->sister ) {
        p->file = f;
        p->redefines = f->record;
    }
    cb_set_field_variable_address_cache(f->record);
    f->finalized = 1;
    if ( f->linage ) {
        snprintf (buff, COB_MINI_MAX, "LC_%s", f->name);
        x = cb_build_field (cb_build_reference (buff));
        CB_FIELD (x)->pic = CB_PICTURE (cb_build_picture ("9(9)"));
        CB_FIELD (x)->usage = CB_USAGE_COMP_5;
        CB_FIELD (x)->values = cb_list_init (cb_zero);
        CB_FIELD (x)->flag_external = f->external;
        CB_FIELD (x)->count++;
        cb_validate_field (CB_FIELD (x));
        f->linage_ctr = cb_build_field_reference (CB_FIELD (x), NULL);
        current_program->working_storage =
        cb_field_add (current_program->working_storage, CB_FIELD (x));
    }
    cb_mark_as_non_skipable(f->key);   
    cb_mark_as_non_skipable(f->file_status);       
    cb_mark_as_non_skipable(f->record_depending);  
    cb_mark_as_non_skipable(f->linage);                 /* LINAGE */
    cb_mark_as_non_skipable(f->linage_ctr);             /* LINAGE COUNTER */
    cb_mark_as_non_skipable(f->latfoot);                /* LINAGE FOOTING */
    cb_mark_as_non_skipable(f->lattop);                 /* LINAGE TOP */
    cb_mark_as_non_skipable(f->latbot);                 /* LINAGE BOTTOM */

    file_set_recmode(f);
    if ( cb_flag_fcd_reg ) {
        x = cb_build_field (cb_build_reference ("FH--FCD"));
        CB_FIELD (x)->usage = CB_USAGE_POINTER;
        CB_FIELD (x)->level=1;
        CB_FIELD (x)->count++;
        CB_FIELD (x)->flag_is_fcd_reg= 1;
        CB_FIELD (x)->file = f;
        CB_FIELD (x)->flag_external = f->external;
        cb_validate_field (CB_FIELD (x));
        f->fcd_reg = cb_build_field_reference (CB_FIELD (x), cb_build_reference(f->name));
        cb_mark_as_non_skipable(f->fcd_reg);
    }
}

static cb_tree 
parce_sysfile_name (char *filename,  int *sizeMin, int *sizeMax, int *organization) {
    char *p;
    char *name;
    int  nospec =0;
    char *cc = strdup(filename);

    *sizeMin = 0;
    *sizeMax = 132;
    *organization = COB_ORG_LINE_SEQUENTIAL;
    name = strtok(filename, ",:");

    p = strtok(NULL, ",:");
    if ( p ) {
        if ( (*p == 'S') || (*p == 's') ) {
            *organization = COB_ORG_SEQUENTIAL;
            *sizeMin = *sizeMax = 132;
        } else if ( (*p == 'L') || (*p == 'l') ) {
            *organization = COB_ORG_LINE_SEQUENTIAL;
        } else if ( isdigit(*p) ) {
            nospec = 1;
        } else {
            cb_error(_("Invalid SYSTEM file format %s"), cc);
        }
        if ( !nospec ) {
            p = strtok(NULL, ",:");
        }
        if ( p ) {
            *sizeMax = *sizeMin = atoi(p);
            p = strtok(NULL, ",:");
            if ( p ) {
                *sizeMax = atoi(p);
            } else {
                if ( *organization == COB_ORG_LINE_SEQUENTIAL ) {
                    *sizeMin = 0;
                }
            }
        }
    }
    free(cc);
    if (!name || strlen(name) == 0 || (strlen(name) == 1 && name[0] == '.')) {
        return NULL;
    }
    return cb_build_alphanumeric_literal((unsigned char*)name, strlen(name),0);
}


struct cb_file *
build_sysfile (struct cb_program *prog, char *filename, const char *file, int input) {
    struct cb_file   *  f;
    char                buff[COB_MINI_BUFF];
    f= build_file (cb_build_file_reference(file));
    f->assign = parce_sysfile_name(filename,&f->record_min, &f->record_max, &f->organization);
    /* create record */
    if (f->assign == NULL) {
        f->special = input ? 1 : 2;
    }
    snprintf (buff, COB_MINI_MAX, "%s_record", f->name);
    f->record = CB_FIELD (cb_build_implicit_field (cb_build_reference (buff),
                                                   f->record_max, f->external));
    f->rec_mode = f->record_max == f->record_min ? 0 : CB_REC_MODE_VARIABLE ;
    f->finalized = 1;
    f->auto_close = 1;
    prog->file_list = cb_cons (CB_TREE (f), prog->file_list);
    return f;
}


void
finalize_section_files (cb_tree file_rec_list) {
    struct cb_file  *f;
    struct cb_field *r;
    cb_tree l;
    cb_tree p;

    for ( l = file_rec_list ; l ; l = CB_CHAIN(l) ) {
        p = CB_VALUE(l);
        if ( p ) {
            f = CB_FILE(CB_PAIR_X(p));
            r = CB_FIELD(CB_PAIR_Y(p));
            cb_validate_section_fields(&r);
            finalize_file(f,r);
        }
    }
}
/*
 * Reference
 */

cb_tree
cb_build_file_reference (const char *name) {
    struct cb_reference *p;
    char tmp[COB_SMALL_BUFF];

    sprintf (tmp, "%s%s", FILE_PREFIX, name);
    p = make_tree (CB_TAG_REFERENCE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_reference));
    p->word = lookup_word (tmp);
    return CB_TREE (p);
}


cb_tree
cb_build_label_reference (cb_tree word) {
    struct cb_reference *p;
    char tmp[COB_SMALL_BUFF];
    if ( cb_flag_mf_compat_parser ) {
        sprintf (tmp,  "%s%s", LABEL_PREFIX, CB_NAME(word));
        p = make_tree (CB_TAG_REFERENCE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_reference));
        p->word = lookup_word (tmp);
        p->common.source_file = word->source_file;
        p->common.source_line = word->source_line;
        return CB_TREE (p);
    } else {
        return word;
    }
}

cb_tree
cb_build_reference (const char *name) {
    struct cb_reference *p;

    p = make_tree (CB_TAG_REFERENCE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_reference));
    p->word = lookup_word (name);
    return CB_TREE (p);
}

cb_tree
cb_build_filler (void) {
    static int      id = 1;
    cb_tree         x;
    char            name[32];

    sprintf (name, "%s%d", CB_PREFIX_FILLER, id++);
    x = cb_build_reference (name);
    x->source_line = cb_lex_source_file.line;
    return x;
}

cb_tree
cb_build_bitfield_filler (void) {
    static int      id = 1;
    cb_tree         x;
    char            name[32];

    sprintf (name, "%s%d", CB_PREFIX_BITFIELD_FILLER, id++);
    x = cb_build_reference (name);
    x->source_line = cb_lex_source_file.line;
    return x;
}

cb_tree
cb_build_field_reference (struct cb_field *f, cb_tree ref) {
    cb_tree         x;
    struct cb_word  *word;

    x = cb_build_reference (f->name);
    word = CB_REFERENCE (x)->word;
    if ( ref ) {
        memcpy (x, ref, sizeof (struct cb_reference));
    }
    x->category = CB_CATEGORY_UNKNOWN;
    CB_REFERENCE (x)->word = word;
    CB_REFERENCE (x)->value = CB_TREE (f);
    return x;
}

const char *
cb_define (cb_tree name, cb_tree val) {
    struct cb_word *w;

    w = CB_REFERENCE (name)->word;
    w->items = cb_list_add (w->items, val);
    w->count++;
    val->source_file = name->source_file;
    val->source_line = name->source_line;
    CB_REFERENCE (name)->value = val;
    return w->name;
}

void
cb_define_system_name (const char *name) {
    cb_tree x;

    x = cb_build_reference (name);
    if ( CB_REFERENCE (x)->word->count == 0 ) {
        cb_define (x, lookup_system_name (name));
    }
}

static cb_tree cb_ref2 (cb_tree x, int dumperror);
static cb_tree
cb_ref1 (cb_tree x , int dump_error, int typedef_only) {
    struct cb_reference     *r = NULL;
    struct cb_field         *p;
    struct cb_label         *s;
    cb_tree                 candidate = NULL;
    cb_tree                 items;
    cb_tree                 cb1;
    cb_tree                 cb2;
    cb_tree                 v;
    cb_tree                 c;
    struct cb_program       *prog;
    struct cb_word          *w;
    size_t                  val;
    size_t                  ambiguous = 0;

    if ( !CB_REFERENCE_P (x) ) goto error;

    r = CB_REFERENCE (x);
    /* if this reference has already been resolved (and the value
       has been cached), then just return the value */
    if ( r->value ) {
        return r->value;
    }
    /* resolve the value */

    items = r->word->items;
    for ( ; items; items = CB_CHAIN (items) ) {
        /* find a candidate value by resolving qualification */
        v = CB_VALUE (items);
        c = r->chain;
        switch ( CB_TREE_TAG (v) ) {
            case CB_TAG_FIELD:
                if ( CB_FIELD (v)->flag_is_typedef && !typedef_only ) {
                    v = NULL;
                    c = NULL;
                    continue;
                }
                /* in case the value is a field, it might be qualified
                   by its parent names and a file name */
                if ( CB_FIELD (v)->flag_indexed_by ) {
                    p = CB_FIELD (v)->index_qual;
                } else {
                    p = CB_FIELD (v)->parent;
                }
                /* resolve by parents */
                for ( ; p && c; p = p->parent ) {
                    if ( c && strcasecmp (CB_NAME (c), p->name) == 0 ) {
                        c = CB_REFERENCE (c)->chain;
                    }
                }
                /* if the qualification is exact .. this is our candidate*/
                if ( (p == NULL) && (c == NULL) ) {
                    candidate = v;
                    goto end;
                }

                /* resolve by file */
                if ( c && CB_REFERENCE (c)->chain == NULL ) {
                    if ( /*CB_REFERENCE (c)->word->count == 1 && */CB_FILE_P (cb_ref2 (c, 0)) ) {
                        if ( (CB_FILE (cb_ref (c)) == cb_field_founder (CB_FIELD (v))->file) ) {
                            c = CB_REFERENCE (c)->chain;
                        }
                    }
                }

                break;
            case CB_TAG_LABEL:
                /* in case the value is a label, it might be qualified
                   by its section name */
                s = CB_LABEL (v)->section;

                /* unqualified paragraph name referenced within the section
                   is resolved without ambiguity check if not duplicated */
                if ( c == NULL && r->offset && s == CB_LABEL (r->offset) ) {
                    for ( cb1 = CB_CHAIN (items); cb1; cb1 = CB_CHAIN (cb1) ) {
                        cb2 = CB_VALUE (cb1);
                        if ( s == CB_LABEL (cb2)->section ) {
                            if ( dump_error ) {
                                ambiguous_error (x);
                            }
                            goto error;
                        }
                    }
                    candidate = v;
                    goto end;
                }
                /* resolve by section name */
                if ( c && s && strcasecmp (CB_NAME (c), (char *)s->name) == 0 ) {
                    c = CB_REFERENCE (c)->chain;
                }

                break;
            default:
                /* other values cannot be qualified */
                break;
        }

        /* a well qualified value is a good candidate */
        if ( c == NULL ) {
            if ( candidate == NULL ) {
                /* keep the first candidate */
                candidate = v;
            } else {
                /* multiple candidates and possibly ambiguous */
                ambiguous = 1;
                /* continue search because the reference might not
                   be ambiguous and exit loop by "goto end" later */
            }
        }
    }

    /* there is no candidate */
    if ( candidate == NULL ) {
        if ( current_program->nested_level > 0 ) {
            /* Nested program - check parents for GLOBAL candidate */
            ambiguous = 0;
            val = hash ((const unsigned char *)r->word->name);
            prog = current_program->next_program;
            for ( ; prog; prog = prog->next_program ) {
                if ( prog->nested_level >= current_program->nested_level ) {
                    continue;
                }
                for ( w = prog->word_table[val]; w; w = w->next ) {
                    if ( strcasecmp (r->word->name, w->name) == 0 ) {
                        candidate = global_check (r, w->items, &ambiguous, 0);
                        if ( candidate ) {
                            if ( ambiguous ) {
                                if ( dump_error ) {
                                    ambiguous_error (x);
                                }
                                goto error;
                            }
                            if ( CB_FILE_P(candidate) ) {
                                current_program->gen_file_error = 1;
                            }
                            goto end;
                        }
                    }
                }
                if ( prog->nested_level == 0 ) {
                    break;
                }
            }
        }
        /* check for types */
        /* Nested program - check parents for GLOBAL candidate */
        ambiguous = 0;
        val = hash ((const unsigned char *)r->word->name);
        prog = current_program->next_program;
        for ( ; prog; prog = prog->next_program ) {
            for ( w = prog->word_table[val]; w; w = w->next ) {
                if ( strcasecmp (r->word->name, w->name) == 0 ) {
                    candidate = global_check (r, w->items, &ambiguous, 1);
                    if ( candidate ) {
                        if ( ambiguous ) {
                            if ( dump_error ) {
                                ambiguous_error (x);
                            }
                            goto error;
                        }
                        goto end;
                    }
                }
            }
        }
        prog = external_program_list;
        for ( ; prog; prog = prog->next_program ) {
            for ( w = prog->word_table[val]; w; w = w->next ) {
                if ( strcasecmp (r->word->name, w->name) == 0 ) {
                    candidate = global_check (r, w->items, &ambiguous, 1);
                    if ( candidate ) {
                        if ( ambiguous ) {
                            if ( dump_error ) {
                                ambiguous_error (x);
                            }
                            goto error;
                        }
                        goto end;
                    }
                }
            }
        }
        if ( dump_error ) {
            undefined_error (x);
        }
        goto error;
    }

    /* the reference is ambiguous */
    if ( ambiguous ) {
        if ( dump_error ) {
            ambiguous_error (x);
        }
        goto error;
    }

    end:
    if ( CB_FIELD_P (candidate) ) {
        struct cb_field *f =CB_FIELD (candidate); 
        if ( typedef_only ) {
            if ( !f->flag_is_typedef ) {
                if ( dump_error ) {
                    typedef_error (x, 1);
                }
                goto error;
            }
        } else {
            if ( f->flag_is_typedef ) {
                if ( dump_error ) {
                    typedef_error (x, 0);
                }
                goto error;
            }
            f->count++;
            if ( f->flag_invalid ) {
                goto error;
            }
        }
    }

    r->value = candidate;
    return r->value;

    error:
    if ( dump_error && r ) {
        r->value = cb_error_node;
    }
    return cb_error_node;
}

static cb_tree
cb_ref2 (cb_tree x, int dumperror) {
    cb_tree c;
    cb_tree r;
    if ( !CB_REFERENCE_P(x) ) {
        return cb_error_node;
    }
    r = cb_ref1 (x, 0, 0);
    if ( r !=  cb_error_node ) {
        CB_REFERENCE (x)->value = r;
        return r;
    }
    c = cb_build_file_reference(CB_NAME(x));
    r = cb_ref1 (c, 0, 0);
    if ( CB_FILE_P (r) ) {
        CB_REFERENCE (x)->value = r;
        return r;
    }
    return cb_ref1 (x, dumperror, 0);
}

cb_tree
cb_ref (cb_tree x) {
    return cb_ref2(x, 1);
}

cb_tree
cb_dup_reference (cb_tree  x) {
    cb_tree  n = x;
    if (x && CB_REFERENCE_P(x)) {
        struct cb_reference *r = CB_REFERENCE(x);
        n = cobc_malloc(sizeof(struct cb_reference));
        memcpy (n, r, sizeof(struct cb_reference));
        if (r->subs) {
            cb_tree l = r->subs;
            CB_REFERENCE(n)->subs = NULL;
            for ( ; l; l=CB_CHAIN(l)) {
                CB_REFERENCE(n)->subs = cb_list_add(CB_REFERENCE(n)->subs, CB_VALUE(l));
            }
        }
    }
    return n;
}

cb_tree
cb_ref_or_type (cb_tree x) {
    cb_tree r;
    if ( !CB_REFERENCE_P(x) ) {
        return cb_error_node;
    }
    r = cb_ref1 (x, 0, 1);
    if ( r !=  cb_error_node ) {
        return r;
    }
    return cb_ref2(x, 1);
}

cb_tree
cb_global_ref (cb_tree x) {
    struct cb_reference     *r;
    struct cb_program       *prog;
    struct cb_word          *w;
    size_t                  ambiguous=0, val;
    cb_tree                 candidate = NULL;

    r = CB_REFERENCE (x);
    val = hash ((const unsigned char *)r->word->name);
    prog = current_program->next_program;
    for ( ; prog; prog = prog->next_program ) {
        for ( w = prog->word_table[val]; w; w = w->next ) {
            if ( strcasecmp (r->word->name, w->name) == 0 ) {
                candidate = global_check (r, w->items, &ambiguous, 1);
                if ( candidate ) {
                    if ( ambiguous ) {
                        cb_error_x (candidate, _("ambiguous; need qualification"));
                        goto error;
                    }
                    goto end;
                }
            }
        }
    }
    end:
    return candidate;
    error:
    return NULL;
}

struct cb_field *
cb_type (cb_tree x) {
    if ( CB_REFERENCE_P (x) ) {
        cb_tree r = cb_ref1 (x, 1, 1);
        if ( CB_FIELD_P(r) ) {
            return CB_FIELD (r);
        }
    } else {
        if ( CB_FIELD_P(x) ) {
            if ( !CB_FIELD (x)->flag_is_typedef ) {
                typedef_error (x, 1);
            } else {
                return CB_FIELD (x);
            }
        }
    }
    return NULL;
}

/*
 * Expression
 */

cb_tree
cb_build_cond_binary_op (cb_tree x, int op, cb_tree y, enum cb_category category) {
    struct cb_binary_op     *p;
    p = make_tree (CB_TAG_BINARY_OP, category, sizeof (struct cb_binary_op));
    p->op = op;
    p->x = x;
    p->y = y;
    return CB_TREE (p);
}

cb_tree
cb_build_binary_op (cb_tree x, int op, cb_tree y) {
    enum cb_category        category = CB_CATEGORY_UNKNOWN;

    switch ( op ) {
        case 's':
            if ( CB_TREE_CLASS (x) == CB_CLASS_POINTER ) {
                category = CB_CATEGORY_DATA_POINTER;
                break;
            }
            x = cb_check_numeric_value (x);
            if ( x == cb_error_node ) {
                return cb_error_node;
            }
            category = CB_CATEGORY_NUMERIC;
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%': /*CIT*/
        case '^':
        case 'a':
        case 'o':
        case 'y':
        case 'n':
            /* arithmetic operators */
            if ( CB_TREE_CLASS (x) == CB_CLASS_POINTER ||
                 CB_TREE_CLASS (y) == CB_CLASS_POINTER ) {
                category = CB_CATEGORY_DATA_POINTER;
                break;
            }
            x = cb_check_numeric_value (x);
            y = cb_check_numeric_value (y);
            if ( x == cb_error_node || y == cb_error_node ) {
                return cb_error_node;
            }
            category = CB_CATEGORY_NUMERIC;
            break;
        case '=':
        case '~':
        case '<':
        case '>':
        case '[':
        case ']':
        case 'v':
            /* relational operators */
            category = CB_CATEGORY_BOOLEAN;
            break;

        case '!':
        case '&':
        case '|':
            /* logical operators */
            if ( CB_TREE_CLASS (x) != CB_CLASS_BOOLEAN ) {
                x = cb_build_cond(x);
                if ( x == cb_error_node ) {
                    cb_error (_("Invalid expression: not boolean left side"));
                    return cb_error_node;
                }
            }
            if ( y && CB_TREE_CLASS (y) != CB_CLASS_BOOLEAN ) {
                y = cb_build_cond(y);
                if ( y == cb_error_node ) {
                    cb_error (_("Invalid expression: not boolean left side"));
                    return cb_error_node;
                }
            }
            category = CB_CATEGORY_BOOLEAN;
            break;

        case '@':
            /* parentheses */
            category = CB_TREE_CATEGORY (x);
            break;

        default:
            fprintf (stderr, "cobc:0: Unexpected operator -> %c\n", op);
            ABORT ();
    }
    return cb_build_cond_binary_op(x,op,y,category);
}

cb_tree
cb_build_binary_list (cb_tree l, int op) {
    cb_tree e;

    e = CB_VALUE (l);
    for ( l = CB_CHAIN (l); l; l = CB_CHAIN (l) ) {
        e = cb_build_binary_op (e, op, CB_VALUE (l));
    }
    return e;
}

/*
 * Function call
 */

cb_tree
cb_build_funcall (const char *name, int argc, cb_tree a1, cb_tree a2, cb_tree a3,
                  cb_tree a4, cb_tree a5, cb_tree a6, cb_tree a7,
                  cb_tree a8, cb_tree a9, cb_tree a10) {
    struct cb_funcall *p;

    p = make_tree (CB_TAG_FUNCALL, CB_CATEGORY_BOOLEAN, sizeof (struct cb_funcall));
    p->name = name;
    p->argc = argc;
    p->varcnt = 0;
    p->screenptr = gen_screen_ptr;
    p->argv[0] = a1;
    p->argv[1] = a2;
    p->argv[2] = a3;
    p->argv[3] = a4;
    p->argv[4] = a5;
    p->argv[5] = a6;
    p->argv[6] = a7;
    p->argv[7] = a8;
    p->argv[8] = a9;
    p->argv[9] = a10;
    return CB_TREE (p);
}

/*
 * Type cast
 */

cb_tree
cb_build_cast (enum cb_cast_type type, cb_tree val) {
    struct cb_cast          *p;
    enum cb_category        category;

    if ( type == CB_CAST_INTEGER ) {
        category = CB_CATEGORY_NUMERIC;
    } else {
        category = CB_CATEGORY_UNKNOWN;
    }
    p = make_tree (CB_TAG_CAST, category, sizeof (struct cb_cast));
    p->type = type;
    p->val = val;
    return CB_TREE (p);
}

/*
 * Label
 */

cb_tree
cb_build_label (cb_tree name, struct cb_label *section) {
    struct cb_label *p;

    p = make_tree (CB_TAG_LABEL, CB_CATEGORY_UNKNOWN, sizeof (struct cb_label));
    p->id = cb_id++;
    p->name = (const unsigned char *)cb_define (name, CB_TREE (p));
    p->orig_name = p->name;
    if ( cb_flag_mf_compat_parser &&
         strncmp((char*)p->name, LABEL_PREFIX, strlen(LABEL_PREFIX)) ==0 ) {
        p->orig_name += strlen(LABEL_PREFIX);
    }

    p->section = section;
    p->common.wipe_index_buffer = 1;
    delete_list();
    return CB_TREE (p);
}

cb_tree 
cb_build_paragraph (cb_tree name, int exitword) {
    cb_tree label;
    cb_tree res = NULL;

    non_const_word = 0;
    check_unreached = 0;
    res = cb_build_section_name(name, 1);
    /* if ($1 == cb_error_node) */
    if (res == cb_error_node) {
        return res;
    }
    if (name->source_column > 1)
        cb_check_feature_x(name, cb_syntax_ibm5_2, "PARAGRAPH name must be at COLUMN 8");

    /* Exit the last paragraph */
    if (current_paragraph) {
        if (current_paragraph->exit_label) {
            emit_statement(current_paragraph->exit_label);
        }
        emit_statement(cb_build_perform_exit(current_paragraph));
    }

    /* Begin a new paragraph */
    if (!current_section) {
        label = cb_build_reference("MAIN SECTION");
        current_section = CB_LABEL(cb_build_label(label, NULL));
        current_section->is_section = 1;
        emit_statement(CB_TREE(current_section));
    }
    current_paragraph = CB_LABEL(cb_build_label(res, current_section));
    if (current_section) {
        current_section->children =
            cb_cons(CB_TREE(current_paragraph), current_section->children);
    }
    emit_statement(CB_TREE(current_paragraph));
    current_program->all_label_list = cb_cons(CB_TREE(current_paragraph), current_program->all_label_list);
    return res;
}

/*
 * Assign
 */

cb_tree
cb_build_assign (cb_tree var, cb_tree val) {
    struct cb_assign *p;

    p = make_tree (CB_TAG_ASSIGN, CB_CATEGORY_UNKNOWN, sizeof (struct cb_assign));
    p->var = var;
    p->val = val;
    return CB_TREE (p);
}

/*
 * INITIALIZE
 */

cb_tree
cb_build_initialize (cb_tree var, cb_tree val, cb_tree rep, cb_tree def, 
                     int flag_statement, int flagdefault, int flag_fillFiller,
                     int care_varying) {
    struct cb_initialize *p;

    p = make_tree (CB_TAG_INITIALIZE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_initialize));
    p->var = var;
    p->val = val;
    p->rep = rep;
    p->def = def;
    p->flag_statement = flag_statement;
    p->flag_fillFiller = cb_flag_initalize_filler || flag_fillFiller;
    /*CIT*/
    p->flag_filldefaultbyte = flagdefault;
    p->flag_care_varing = care_varying;

    return CB_TREE (p);
}

/*
 * FIXVALUE
 */

cb_tree
cb_build_fixvalue (cb_tree var, int flag) {
    struct cb_fixvalue *p;

    p = make_tree (CB_TAG_FIXVALUE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_fixvalue));
    p->var = var;
    p->flag_spzero = flag;
    return CB_TREE (p);
}

/*
 * SEARCH
 */

cb_tree
cb_build_search (int flag_all, cb_tree table, cb_tree var, cb_tree end_stmt, cb_tree whens) {
    struct cb_search *p;

    p = make_tree (CB_TAG_SEARCH, CB_CATEGORY_UNKNOWN, sizeof (struct cb_search));
    p->flag_all = flag_all;
    p->table = table;
    p->var = var;
    p->end_stmt = end_stmt;
    p->whens = whens;
    return CB_TREE (p);
}

/*
 * CALL
 */

cb_tree
cb_build_call (cb_tree name, cb_tree args, cb_tree stmt1, cb_tree stmt2, cb_tree returning, 
               int is_system_call, int call_convention, cb_tree stmt3) {
    struct cb_call *p;

    p = make_tree (CB_TAG_CALL, CB_CATEGORY_UNKNOWN, sizeof (struct cb_call));
    p->name = name;
    p->args = args;
    p->stmt1 = stmt1;
    p->stmt2 = stmt2;
    p->stmt3 = stmt3;
    p->returning = returning;
    p->is_system = is_system_call;
    p->call_convention = call_convention;
    p->common.wipe_index_buffer = 1;
    delete_list();
    return CB_TREE (p);
}

/*
 * GO TO
 */

cb_tree
cb_build_goto (cb_tree target, cb_tree depending, cb_tree return_field) {
    struct cb_goto *p;

    p = make_tree (CB_TAG_GOTO, CB_CATEGORY_UNKNOWN, sizeof (struct cb_goto));
    p->target = target;
    p->depending = depending;
    p->return_field = return_field;
    return CB_TREE (p);
}

/*
 * IF
 */

cb_tree
cb_build_if (cb_tree test, cb_tree stmt1, cb_tree stmt2) {
    struct cb_if *p;

    p = make_tree (CB_TAG_IF, CB_CATEGORY_UNKNOWN, sizeof (struct cb_if));
    p->test = test;
    p->stmt1 = stmt1;
    p->stmt2 = stmt2;
    return CB_TREE (p);
}

/*
 * INLINE
 */

cb_tree
cb_build_inline (const char * line) {
    struct cb_inline *p;

    p = make_tree (CB_TAG_INLINE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_inline));
    p->line = strdup(line);
    return CB_TREE (p);
}

/*
 * PERFORM
 */

cb_tree
cb_build_perform (int type) {
    struct cb_perform *p;

    p = make_tree (CB_TAG_PERFORM, CB_CATEGORY_UNKNOWN, sizeof (struct cb_perform));
    p->type = type;
    p->common.wipe_index_buffer = 1;
    delete_list();
    return CB_TREE (p);
}

cb_tree
cb_build_perform_varying (cb_tree name, cb_tree from, cb_tree by, cb_tree until) {
    struct cb_perform_varying *p;

    p = make_tree (CB_TAG_PERFORM_VARYING, CB_CATEGORY_UNKNOWN, sizeof (struct cb_perform_varying));
    p->name = name;
    p->from = from;
    p->step = name ? cb_build_add (name, by, cb_high) : NULL;
    p->until = until;
    return CB_TREE (p);
}

cb_tree
cb_build_perform_varying_step (cb_tree name, cb_tree from, cb_tree step, cb_tree until) {
    struct cb_perform_varying *p;

    p = make_tree (CB_TAG_PERFORM_VARYING, CB_CATEGORY_UNKNOWN, sizeof (struct cb_perform_varying));
    p->name = name;
    p->from = from;
    p->step = step;
    p->until = until;
    return CB_TREE (p);
}

/*CIT*/
/*
 * PRAGMA pseudo statement
 */

cb_tree
cb_build_pragma (void) {
    return(make_tree (CB_TAG_PRAGMA, CB_CATEGORY_UNKNOWN, sizeof (struct cb_pragma)));
}

/*
 * Statement
 */

struct cb_statement *
cb_build_statement (const char *name) {
    struct cb_statement *p;

    p = make_tree (CB_TAG_STATEMENT, CB_CATEGORY_UNKNOWN, sizeof (struct cb_statement));
    p->name = name;
    return p;
}

/*
 * CONTINUE
 */

cb_tree
cb_build_continue (void) {
    struct cb_continue *p;

    p = make_tree (CB_TAG_CONTINUE, CB_CATEGORY_UNKNOWN, sizeof (struct cb_continue));
    return CB_TREE (p);
}

/*
 * READY
 */

cb_tree
cb_build_ready (void) {
    struct cb_ready *p;

    p = make_tree (CB_TAG_READY, CB_CATEGORY_UNKNOWN, sizeof (struct cb_ready));
    return CB_TREE (p);
}

/*
 * RESET
 */

cb_tree
cb_build_reset (void) {
    struct cb_ready *p;

    p = make_tree (CB_TAG_RESET, CB_CATEGORY_UNKNOWN, sizeof (struct cb_reset));
    return CB_TREE (p);
}

/*
 * FUNCTION
 */

cb_tree
cb_build_any_intrinsic (cb_tree args) {
    struct cb_intrinsic_table       *cbp;

    cbp = lookup_intrinsic ("LENGTH", 0);
    return make_intrinsic (NULL, cbp, args, NULL, NULL);
}

cb_tree
cb_build_cstring_intrinsic (cb_tree x) {
    struct cb_intrinsic_table       *cbp;

    cbp = lookup_intrinsic ("CSTRING", 0);
    return make_intrinsic (NULL, cbp, cb_list_init( x), NULL, NULL);
}

cb_tree
cb_build_intrinsic (cb_tree name, cb_tree args, cb_tree refmod) {
    struct cb_intrinsic_table       *cbp;
    cb_tree                         x;
    cb_tree                         l;
    cb_tree                         n;
    int                             numargs;
    char                            a[100];

    numargs = cb_list_length (args);

    cbp = lookup_intrinsic (CB_NAME (name), 0);
    if ( cbp ) {
        if ( (cbp->args != -1 && numargs != cbp->args) ||
             (cbp->args == -1 && cbp->intr_enum != CB_INTR_RANDOM && numargs < 1) ) {
            cb_error_x (name, _("FUNCTION %s has wrong number of arguments"), cbp->name);
            return cb_error_node;
        }
        if ( refmod ) {
            if ( !cbp->refmod ) {
                cb_error_x (name, _("FUNCTION %s can not have reference modification"), cbp->name);
                return cb_error_node;
            }
            if ( CB_LITERAL_P(CB_PAIR_X(refmod)) &&
                 cb_get_int (CB_PAIR_X(refmod))< 1 ) {
                cb_error_x (name, _("FUNCTION %s has invalid reference modification"), cbp->name);
                return cb_error_node;
            }
            if ( CB_PAIR_Y(refmod) && CB_LITERAL_P(CB_PAIR_Y(refmod)) &&
                 cb_get_int (CB_PAIR_Y(refmod))< 1 ) {
                cb_error_x (name, _("FUNCTION %s has invalid reference modification"), cbp->name);
                return cb_error_node;
            }
        }
        /* cb_tree      x; */
        switch ( cbp->intr_enum ) {
            case CB_INTR_LENGTH:
            case CB_INTR_BYTE_LENGTH:
                x = CB_VALUE (args);
                if ( CB_INTRINSIC_P (x) ) {
                    return make_intrinsic (name, cbp, args, NULL, NULL);
                } else if ( CB_REFERENCE_P (x) && CB_REFERENCE(x)->length && 
                            CB_REFERENCE_P (CB_REFERENCE(x)->length) ) {
                    return make_intrinsic (name, cbp, args, NULL, NULL);
                } else if ( (CB_FIELD_P (x) || CB_REFERENCE_P (x)) &&
                            (cb_field(x)->flag_any_length || cb_field(x)->usage == CB_USAGE_NATIONAL)) {
                    return make_intrinsic (name, cbp, args, NULL, NULL);
                } else {
                    return cb_build_length (CB_VALUE (args));
                }

            case CB_INTR_WHEN_COMPILED:
                if ( refmod ) {
                    return make_intrinsic (name, cbp,
                                           cb_list_init (cb_intr_whencomp), NULL, refmod);
                } else {
                    return cb_intr_whencomp;
                }
            case CB_INTR_PI:
                return cb_intr_pi;
            case CB_INTR_E:
                return cb_intr_e;

            case CB_INTR_LOWER_CASE:
            case CB_INTR_UPPER_CASE:
            case CB_INTR_REVERSE:
/* RXW Why did I do this ? - still do not know
                        if (CB_INTRINSIC_P (CB_VALUE (args))) {
                                return make_intrinsic (name, cbp, args, cb_int0);
                        } else {
                                return make_intrinsic (name, cbp, args,
                                                       cb_build_length (CB_VALUE (args)));
                        }
RXW */

            case CB_INTR_ABS:
            case CB_INTR_ACOS:
            case CB_INTR_ANNUITY:
            case CB_INTR_ASIN:
            case CB_INTR_ATAN:
            case CB_INTR_CHAR:
            case CB_INTR_COMBINED_DATETIME:
            case CB_INTR_CSTRING:
            case CB_INTR_COS:
            case CB_INTR_CURRENT_DATE:
            case CB_INTR_DATE_OF_INTEGER:
            case CB_INTR_DAY_OF_INTEGER:
            case CB_INTR_EXCEPTION_FILE:
            case CB_INTR_EXCEPTION_LOCATION:
            case CB_INTR_EXCEPTION_STATUS:
            case CB_INTR_EXCEPTION_STATEMENT:
            case CB_INTR_EXP:
            case CB_INTR_EXP10:
            case CB_INTR_FACTORIAL:
            case CB_INTR_FRACTION_PART:
            case CB_INTR_INTEGER:
            case CB_INTR_INTEGER_OF_DATE:
            case CB_INTR_INTEGER_OF_DAY:
            case CB_INTR_INTEGER_PART:
            case CB_INTR_LOCALE_DATE:
            case CB_INTR_LOCALE_TIME:
            case CB_INTR_LOCALE_TIME_FROM_SECS:
            case CB_INTR_LOG:
            case CB_INTR_LOG10:
            case CB_INTR_MOD:
            case CB_INTR_NUMVAL:
            case CB_INTR_NUMVAL_C:
            case CB_INTR_ORD:
            case CB_INTR_REM:
            case CB_INTR_SECONDS_FROM_FORMATTED_TIME:
            case CB_INTR_SECONDS_PAST_MIDNIGHT:
            case CB_INTR_SIGN:
            case CB_INTR_SIN:
            case CB_INTR_SQRT:
            case CB_INTR_STORED_CHAR_LENGTH:
            case CB_INTR_TAN:
            case CB_INTR_TEST_DATE_YYYYMMDD:
            case CB_INTR_TEST_DAY_YYYYDDD:
            case CB_INTR_TRIM:
            case CB_INTR_TRIML:
            case CB_INTR_TRIMR:
                return make_intrinsic (name, cbp, args, NULL, refmod);

            case CB_INTR_CONCATENATE:
                return make_intrinsic (name, cbp, args, cb_int1, refmod);
            case CB_INTR_DATE_TO_YYYYMMDD:
            case CB_INTR_DAY_TO_YYYYDDD:
            case CB_INTR_MAX:
            case CB_INTR_MEAN:
            case CB_INTR_MEDIAN:
            case CB_INTR_MIDRANGE:
            case CB_INTR_MIN:
            case CB_INTR_ORD_MAX:
            case CB_INTR_ORD_MIN:
            case CB_INTR_PRESENT_VALUE:
            case CB_INTR_RANDOM:
            case CB_INTR_RANGE:
            case CB_INTR_STANDARD_DEVIATION:
            case CB_INTR_SUM:
            case CB_INTR_VARIANCE:
            case CB_INTR_YEAR_TO_YYYY:
                return make_intrinsic (name, cbp, args, cb_int1, NULL);                 
            case CB_INTR_TO_UTF8:
            case CB_INTR_FROM_UTF8:
            case CB_INTR_NATIONAL_OF:
            case CB_INTR_DISPLAY_OF:
                return make_intrinsic (name, cbp, args, cb_int1, refmod);
                break;
            case CB_INTR_SUBSTITUTE:
            case CB_INTR_SUBSTITUTE_CASE:
                if ( numargs < 3 || (numargs % 2) == 0 ) {
                    cb_error_x (name, _("FUNCTION %s has wrong number of arguments"), cbp->name);
                    return cb_error_node;
                }
                return make_intrinsic (name, cbp, args, cb_int1, refmod);
            case CB_INTR_USER_DEFINE:
                for ( l = args; l; l = CB_CHAIN (l) ) {
                    CB_PURPOSE(l) = cb_int(CB_CALL_BY_REFERENCE);
                }
                sprintf (a, "pfld%d", current_program->field_pointer_count++);
                if ( current_program->field_pointer_max < current_program->field_pointer_count )
                    current_program->field_pointer_max = current_program->field_pointer_count;
                l = make_constant(CB_CATEGORY_UNKNOWN, strdup(a)) ;
                n = cb_build_alphanumeric_literal((unsigned char*)cbp->intr_routine, strlen(cbp->intr_routine), 0);
                CB_CONST(l)->num_type = CB_CONST_FLDADDR;
                current_statement->before_body = cb_list_add(current_statement->before_body, 
                                                             cb_build_call(n, args,
                                                                           NULL, NULL,l, 0,CB_CALL_RETURN_FIELD_ADDR,NULL));
                if ( cbp->args < 0 ) {
                    cbp->args = cb_list_length(args);
                }
                return l;
                break;
            default:
                break;
        }
    }
    cb_error_x (name, _("FUNCTION %s not implemented"), CB_NAME (name));
    return cb_error_node;
}

/*
TARGET
*/
void update_wipe_flag_and_target(cb_tree dst, cb_tree src){
   if (cb_flag_index_optimize || cb_flag_decimal_optimize) {
      dst->target_ref_list = cb_list_append_contents(dst->target_ref_list, src->target_ref_list);
      if (src->wipe_index_buffer) {
        dst->wipe_index_buffer = src->wipe_index_buffer;
      }
   }
}


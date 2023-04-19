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

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <libcob.h>
#include <tarstamp.h>

#include "cobc/tree.h"
#include "cobc/cobc.h"


extern void enterprise_optimizer_display_info (void);
extern void enterprise_optimizer_reset_info (int flags);
extern cb_tree enterprise_optimizer_mark_linear_statement (struct cb_program *current_program, cb_tree l) ;
extern cb_tree enterprise_pipe_optimizer  (struct cb_program *current_program, cb_tree l) ;
extern int enterprise_optimizer_enable_location (cb_tree x);


static int cb_optimizer_marked_zone = 0;
static int cb_optimizer_marked_statement = 0;
static int loc_flag_context_reload_enable = 0;
static struct cb_program  *cb_optimizer_current_program;
/* treshold */
#define LIN_STATEMENT_MARK 5

static int
is_openrand_global (cb_tree x)
{
    struct cb_field  *f;
    if (x != NULL) {
        if (CB_REF_OR_FIELD_P(x)) {
            if (CB_REFERENCE_P(x)) {
                cb_tree ref = cb_ref(x);
                cb_tree lsubs;
                struct cb_reference *r = CB_REFERENCE(x);
                if (CB_FILE_P(ref) || CB_ALPHABET_NAME_P(ref)) {
                    return 0;
                }
                if (!CB_FIELD_P(ref)) {
                    return 1;
                }
                if (!is_openrand_global(r->offset)) {
                    return 0;
                }
                if (!is_openrand_global(r->length)) {
                    return 0;
                }
                for (lsubs= r->subs; lsubs; lsubs = CB_CHAIN(lsubs)) {
                    if (!is_openrand_global(CB_VALUE(lsubs))) {
                        return 0;
                    }
                }
            }
            f=cb_field(x);
            if (!is_openrand_global(f->occurs_depending)) {
                return 0;
            }
            if (f->parent) {
                return is_openrand_global(CB_TREE(f->parent));
            }

            if (f->flag_is_pdiv_parm || f->flag_local_alloced || f->flag_local || f->flag_is_fcd_reg) {
                return 0;
            }
            if (f->storage != CB_STORAGE_WORKING && f->storage != CB_STORAGE_CONSTANT) {
                return 0;
            }
        } else if (CB_CAST_P(x)) {
            return is_openrand_global(CB_CAST(x)->val);
        } else if (CB_BINARY_OP_P(x)) {
            return is_openrand_global(CB_BINARY_OP(x)->x) && is_openrand_global(CB_BINARY_OP(x)->y);
        } else if (CB_INTRINSIC_P(x)) {
            struct cb_intrinsic *it = CB_INTRINSIC(x);
            return is_openrand_global(it->args) && is_openrand_global(it->intr_field) && 
            is_openrand_global(it->length) && is_openrand_global(it->offset);
        } else if (CB_LIST_P(x)) {
            cb_tree li = x;
            for (;li; li=CB_CHAIN(li)) {
                if (!is_openrand_global(CB_VALUE(li))) {
                    return 0;
                }
            }
        } else if (CB_FUNCALL_P(x)) {
            int i;
            /*if (CB_FUNCALL(x)->varcnt) {
                return 0;
            }*/
            for ( i = 0; i < CB_FUNCALL(x)->argc; i++ ) {
                if (! is_openrand_global(CB_FUNCALL(x)->argv[i]))
                    return 0;
            }
        } else if (CB_DECIMAL_P(x) && CB_DECIMAL (x)->binary) {
            return 0;
        } else if (CB_CONST_P(x) && CB_CONST(x)->num_type != CB_CONST_STD) {
            return 0;

        }
    }
    return 1;
}

static int
is_statement_linear (cb_tree x, int * included_step) {
    cb_tree             g, l;
    struct cb_statement *p;
    struct cb_label     *lp;
    /*struct cb_if        *ip;*/
    struct cb_call      *cl;
    int i;

    if ( x == NULL ) {
        return 1;
    }

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_STATEMENT:
            p = CB_STATEMENT (x);
            /* Output source location as a comment */
            if ( p->file )
                return 0;
            if ( p->null_check && ! is_statement_linear (p->null_check, included_step) )
                return 0;
            if ( p->before_body && ! is_statement_linear (p->before_body, included_step) )
                return 0;
            if ( p->body && ! is_statement_linear (p->body, included_step) )
                return 0;
            if ( p->after_body && ! is_statement_linear (p->after_body, included_step) )
                return 0;
            if ( p->handler1 && ! is_statement_linear (p->handler1, included_step) )
                return 0;
            if ( p->handler2 && ! is_statement_linear (p->handler2, included_step) )
                return 0;
            if ( p->handler3 && ! is_statement_linear (p->handler3, included_step) )
                return 0;
            break;
        case CB_TAG_LABEL:
            lp = CB_LABEL (x);
            if (!cb_optimizer_current_program) {
                return 0;
            }
            g = find_alter_goto(cb_optimizer_current_program, (char*)lp->name);
            if ( g && CB_GOTO_P(g) ) {
                return 0;
            }
            if ( lp->need_begin )
                return 0;
            break;
        case CB_TAG_PRAGMA:
        case CB_TAG_CONTINUE:
        case CB_TAG_READY:
        case CB_TAG_RESET:
            break;
        case CB_TAG_FUNCALL:
            /*if (CB_FUNCALL(x)->varcnt) {
                return 0;
            }*/
            for ( i = 0; i < CB_FUNCALL(x)->argc; i++ ) {
                if (! is_openrand_global(CB_FUNCALL(x)->argv[i]))
                    return 0;
            }
            break;
        case CB_TAG_ASSIGN:
            if (!is_openrand_global(CB_ASSIGN(x)->var) )
                return 0;
            if (!is_openrand_global(CB_ASSIGN(x)->val) )
                return 0;
            break;
        case CB_TAG_INITIALIZE:
            if (!is_openrand_global(CB_INITIALIZE(x)->var) )
                return 0;
            if (!is_openrand_global(CB_INITIALIZE(x)->val) )
                return 0;
            break;
        case CB_TAG_FIXVALUE:
            if (!is_openrand_global(CB_FIXVALUE(x)->var) )
                return 0;
            break;
        case CB_TAG_SEARCH:
            /* First instance search always false*/
            /* todo */
            return 0;
        case CB_TAG_CALL:
            if ( loc_flag_context_reload_enable ) {
                return 0;
            }
            cl = CB_CALL(x);
            if ( cl->stmt1 && ! is_statement_linear (cl->stmt1, included_step) )
                return 0;
            if ( cl->stmt2 && ! is_statement_linear (cl->stmt2, included_step) )
                return 0;
            if ( cl->stmt3 && ! is_statement_linear (cl->stmt3, included_step) )
                return 0;
            if (!is_openrand_global (cl->name)) {
                return 0;
            }
            for ( l = cl->args; l; l = CB_CHAIN (l)) {
                if ( !is_openrand_global (CB_VALUE (l)))
                    return 0;
            }
            (*included_step)+=3; 
            break;
        case CB_TAG_INLINE:
        case CB_TAG_PERFORM:
        case CB_TAG_GOTO:
            return 0;
            break;
        case CB_TAG_IF:
            {
                struct cb_if *ip = CB_IF (x);
                if ( ip->stmt1 && ! is_statement_linear (ip->stmt1 , included_step))
                    return 0;
                if ( ip->stmt2 && ! is_statement_linear (ip->stmt2 , included_step))
                    return 0;            
                if (!is_openrand_global(ip->test) )
                    return 0;
            }
            break;
        case CB_TAG_LIST:
            for ( ; x; x = CB_CHAIN (x) ) {
                if ( ! is_statement_linear (CB_VALUE (x), included_step) )
                    return 0;
            }
            break;
        default:
            cb_error_x (x,"Unexpected tree tag (is_statement_linear) %d\n", CB_TREE_TAG (x));
            ABORT ();
    }
    (*included_step)++; 
    return 1;
}

static int
count_linear_statement (cb_tree x , int * included_step) {
    int cnt = 0;

    if ( x == NULL ) {
        return 0;
    }
    if ( ! CB_LIST_P(x) ) {
        cb_error_x (x,"Unexpected tree tag (count_linear_statement) %d\n", CB_TREE_TAG (x));
        ABORT ();
    }
    for ( ; x; x = CB_CHAIN (x) ) {
        if ( is_statement_linear (CB_VALUE(x), included_step) )
            cnt ++;
        else
            break;

    } 
    return cnt;
}

static cb_tree 
skip_n_statement (cb_tree x, int n) {

    if ( ! CB_LIST_P(x) ) {
        cb_error_x (x, "Unexpected tree tag (skip_n_statement) %d\n", CB_TREE_TAG (x));
        ABORT ();
    }
    for ( ; (n > 0) && x; n--, x = CB_CHAIN (x) ) ;
    return x;
}

static cb_tree
enterprise_optmizer_mark_linear_statement_zone (cb_tree l, int n) {
    static int zone_id = 0;
    cb_tree x;
    cb_tree ln;

    if ( !l || ! CB_LIST_P(l) ) {
        return l;
    }
    cb_optimizer_marked_zone++;
    cb_optimizer_marked_statement +=n;
    ln = skip_n_statement (l, n-1);
    zone_id ++;
    x = cb_build_pragma();
    x->source_file = l->source_file;
    x->source_line = l->source_line;
    CB_PRAGMA(x)->code_gen_tag = PRAGMA_CODEGEN_EXTRACT_BEGIN; 
    CB_PRAGMA(x)->code_gen_opt = zone_id;
    x->wipe_index_buffer = 1;
    l = cb_cons (x, l);

    x = cb_build_pragma();
    CB_PRAGMA(x)->code_gen_tag = PRAGMA_CODEGEN_EXTRACT_END; 
    CB_PRAGMA(x)->code_gen_opt = zone_id;
    x->wipe_index_buffer = 1;
    if ( ln ) {
        cb_list_insert_after(ln, x);
        x->source_file = ln->source_file;
        x->source_line = ln->source_line;
    } else {
        x->source_file = l->source_file;
        x->source_line = l->source_line;
        cb_list_add(l, x);
    }

    return l;
}

void 
enterprise_optimizer_display_info (void)
{
    fprintf (stderr, "cobc:0: Optimizer mark %d statements in %d zones\n", 
             cb_optimizer_marked_statement,cb_optimizer_marked_zone);
}

void 
enterprise_optimizer_reset_info (int flags)
{
    cb_optimizer_marked_zone = 0;
    cb_optimizer_marked_statement = 0;
    loc_flag_context_reload_enable = flags; 
}

cb_tree
enterprise_optimizer_mark_linear_statement (struct cb_program *current_program,  cb_tree l) {
    int c=0;
    struct cb_call      *cl;
    cb_tree ret;
    cb_tree x;
    struct cb_statement *p;
    struct cb_if        *ip;
    int step= 0;

    cb_optimizer_current_program = current_program;
    if ( !l || ! CB_LIST_P(l) ) {
        return l;
    }
    c = count_linear_statement(l, &step);
    ret = l;
    if ( c> 0 && step > LIN_STATEMENT_MARK ) {
        ret = enterprise_optmizer_mark_linear_statement_zone(l,c);
        l = skip_n_statement (ret, c+2);
    }
    for ( ;l; l = CB_CHAIN(l) ) {
        step = 0;
        c = count_linear_statement(CB_CHAIN(l), &step);
        if ( c> 0 && step > LIN_STATEMENT_MARK ) {
            CB_CHAIN(l) = enterprise_optmizer_mark_linear_statement_zone(CB_CHAIN(l),c);
            l = skip_n_statement (l, c+2);     
        } else if ( c ) {
            l = skip_n_statement (l, c);     
        }
        if (l) {
            x = CB_VALUE(l);
            switch ( CB_TREE_TAG (x) ) {
                case CB_TAG_STATEMENT:
                    p = CB_STATEMENT (x);
                    /* Output source location as a comment */

                    p->null_check = enterprise_optimizer_mark_linear_statement (current_program, p->null_check);
                    p->before_body = enterprise_optimizer_mark_linear_statement (current_program, p->before_body) ;
                    p->body = enterprise_optimizer_mark_linear_statement (current_program, p->body) ;
                    p->after_body = enterprise_optimizer_mark_linear_statement (current_program, p->after_body) ;
                    p->handler1 = enterprise_optimizer_mark_linear_statement (current_program, p->handler1) ;
                    p->handler2 = enterprise_optimizer_mark_linear_statement (current_program, p->handler2) ;
                    p->handler3 = enterprise_optimizer_mark_linear_statement (current_program, p->handler3) ;
                    break;
                case CB_TAG_SEARCH:
                    /* First instance search always false*/
                    /* todo */
                    break;
                case CB_TAG_IF:
                    ip = CB_IF (x);
                    ip->stmt1 = enterprise_optimizer_mark_linear_statement (current_program, ip->stmt1) ;
                    ip->stmt2 = enterprise_optimizer_mark_linear_statement (current_program, ip->stmt2) ;
                    break;
                case CB_TAG_LIST:
                    CB_VALUE(l) = enterprise_optimizer_mark_linear_statement (current_program, x);
                    break;
                case CB_TAG_CALL:
                    cl = CB_CALL(x);
                    cl->stmt1 = enterprise_optimizer_mark_linear_statement (current_program, cl->stmt1);
                    cl->stmt2 = enterprise_optimizer_mark_linear_statement (current_program, cl->stmt2);
                    cl->stmt3 = enterprise_optimizer_mark_linear_statement (current_program, cl->stmt3);
                    break;
                default:
                    break;
            }
        }
    }
    return ret;
}

/*
static int
is_memset(cb_tree x , cb_tree *var, cb_tree *val) {
    if (x && CB_FUNCALL_P(x)) {
        struct cb_funcall   *p;
        p = CB_FUNCALL (CB_VALUE(res));
        if ( p->name[0] == '$' &&  p->name[1] == 'M') {
            if (val) {
                *val = p->argv[1];
            }
            if (var) {
                *var p->argv[0];
            }
            return 1;
        }

    }
    return 0;
}
static int
is_mergable(cb_tree x, cb_tree base) {
    struct cb_statement *p;

    cb_tree res = NULL;
    cb_tree varx = NULL;
    cb_tree varbase = NULL;
    cb_tree valx = NULL;
    cb_tree valbase = NULL;
    if ( !x || ! CB_LIST_P(x) ) {
        return 0;
    }
    if ( !base || ! CB_LIST_P(base) ) {
        return 0;
    }
    if (is_memset(CB_VALUE(x, &varx, &valx)) && is_memset(CB_VALUE(base, &varbase, &valbase))){
        if (valx == valbase) {
            struct cb_field *fx = cb_field(varx);
            struct cb_field *fbase = cb_field(varbase);
            if (fx && fbase && !fx->flag_no_optimizable && !fbase->flag_no_optimizable) {
                if (cb_field_founder(fx) && cb_field_founder(fx) == cb_field_founder(fbase)) {
                    if (fbase-) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

static int
merge_fct(cb_tree x, cb_tree base) {
    struct cb_statement *p;

    cb_tree res = NULL;
    cb_tree varx = NULL;
    cb_tree varbase = NULL;
    cb_tree valx = NULL;
    cb_tree valbase = NULL;
    if ( !x || ! CB_LIST_P(x) ) {
        return 0;
    }
    if ( !base || ! CB_LIST_P(base) ) {
        return 0;
    }
    if (x && CB_FUNCALL_P(x)) {
        struct cb_funcall   *p;
        p = CB_FUNCALL (CB_VALUE(res));
        if ( p->name[0] == '$' &&  p->name[1] == 'M') {
            if (val) {
                *val = p->argv[1];
            }
            if (var) {
                *var p->argv[0];
            }
            return 1;
        }

    }
    if (is_memset(CB_VALUE(x, &varx, &valx)) && is_memset(CB_VALUE(base, &varbase, &valbase))){
        if (valx == valbase) {
            struct cb_field *fx = cb_field(varx);
            struct cb_field *fbase = cb_field(varbase);
            if (fx && fbase) {
                if (cb_field_founder(fx) && cb_field_founder(fx) == cb_field_founder(fbase)) {
                    if (fbase->offset + fbase->size == fx->offset) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

cb_tree
enterprise_pipe_optimizer ( cb_tree l) {
    int c=0;
    struct cb_call      *cl;
    cb_tree ret;
    cb_tree base = NULL;
    cb_tree next;
    cb_tree x;
    struct cb_statement *p;
    struct cb_if        *ip;

    if ( !l || ! CB_LIST_P(l) ) {
        return l;
    }
    for ( ;l; l = CB_CHAIN(l) ) {
        x = CB_VALUE(l);
        switch ( CB_TREE_TAG (x) ) {
            case CB_TAG_STATEMENT:
                p = CB_STATEMENT (x);

                if (!p->null_check && p->before_body && !p->after_body && 
                    !p->handler1   && !p->handler2   && !p->handler3 && p->body ) {
                    if (is_mergable(p->body, base)) {

                    }
                    next = is_memset(p->body);
                    if (next) {
                        if (!base) {
                            base = next;
                        } else {
                            if (CB_CHAIN(next)) {
                            }
                        }
                    } else {
                        base = NULL;
                    }
                }
                break;
            default:
                base = NULL;
                break;
        }
    }
    return l;

}
cb_tree
enterprise_tree_simplify (struct cb_program *current_program,  cb_tree l) {
    int c=0;
    struct cb_call      *cl;
    cb_tree ret;
    cb_tree base;
    cb_tree next;
    cb_tree x;
    struct cb_statement *p;
    struct cb_if        *ip;

    cb_optimizer_current_program = current_program;
    if ( !l || ! CB_LIST_P(l) ) {
        return l;
    }
    for ( ;l; l = CB_CHAIN(l) ) {
        x = CB_VALUE(l);
        switch ( CB_TREE_TAG (x) ) {
            case CB_TAG_STATEMENT:
                p = CB_STATEMENT (x);

                if (!p->null_check && p->before_body && !p->after_body && 
                    !p->handler_id && !p->file &&
                    !p->handler1   && !p->handler2   && !p->handler3 && p->body ) {

                }
                break;
            default:
                break;
        }
    }
    return ret;
}

*/
int 
enterprise_optimizer_enable_location (cb_tree x)
{
    struct cb_statement *p;
    struct cb_label     *lp;

    if ( x == NULL ) {
        return 0;
    }

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_STATEMENT:
            p = CB_STATEMENT (x);
            /* Output source location as a comment */
            if ( p->file )
                return 1;
            return enterprise_optimizer_enable_location (p->body);
            break;
        case CB_TAG_LABEL:
            lp = CB_LABEL (x);
            if ( lp->need_begin )
                return 1;
            break;
        case CB_TAG_SEARCH:
            /* First instance search always false*/
            /* todo */
            return 0;
            break;
        case CB_TAG_IF:
        case CB_TAG_PERFORM:
        case CB_TAG_CALL:
            return 1;
        case CB_TAG_LIST:
            return enterprise_optimizer_enable_location (CB_VALUE(x));
            break;
        default:
            return 0;
    }
    return 0;
}




int dummy_enterprise (void);
int dummy_enterprise (void)
{
    /* for Stupid AIX linker*/
    return 0;
}

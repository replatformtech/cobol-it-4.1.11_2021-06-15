/*
 * Copyright (C) 2002-2007 Keisuke Nishida
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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <libcob.h>
#include <tarstamp.h>

#ifdef DEBUG
#define DEBUG_BASE 1
#endif
#include "cobc.h"
#include "tree.h"
#include "enterprise/a2e.h"
/*#include "call.def"*/
#include "enterprise/debugdb.h"

#define COB_MAX_SUBSCRIPTS      16

#define INITIALIZE_NONE         0
#define INITIALIZE_ONE          1
#define INITIALIZE_DEFAULT      2
#define INITIALIZE_COMPOUND     3
#define INITIALIZE_EXTERNAL     4
#define INITIALIZE_UNIFORM      5
#define INITIALIZE_ONE_VALUE    6

static int              inside_check = 0;
static int              inside_stack[64];
static int              param_id = 0;
static int              stack_id = 0;
static int              save_stack_id;
static int              num_cob_fields = 0;
static int              num_cob_bitsarray_fields = 0;
static int              loop_counter = 0;
static int              progid = 0;
static int              last_line = 0;
static unsigned char *last_source = NULL;
static int              needs_exit_prog = 0;
static int              need_double = 0;
static int              gen_ebcdic = 0;
static int              gen_ebcdic_ascii = 0;
static int              gen_full_ebcdic = 0;
static int              gen_native = 0;
static int              gen_custom = 0;
static int              field_iteration = 0;
static int              screenptr = 0;
/*CIT*/
static int              cur_location_idx = 0;
int                     cur_profiling_idx = 0;
static int              cur_source_line = 0;
int                     gen_unifunc_cnt = 0;
static unsigned char    *cur_source_file = NULL;
static const char       *debug_prefix_base = NULL;
static int              suspend_debug_location = 0;
static int              initialize_cache_cnt = 0;

static int              i_counters[COB_MAX_SUBSCRIPTS];

static int              output_indent_level = 0;
static int              output_extracting_Code_level = 0;
static FILE             *output_target[10];
static int              output_target_idx = 0;
static const char       *excp_current_source_name = NULL;
static const char       *excp_current_program_id = NULL;
static const char       *excp_current_section = NULL;
static const char       *excp_current_paragraph = NULL;
struct cb_program       *current_codegen_prog = NULL;
int cb_generating_initialize = 0;


static struct index_optimizer *zombie = NULL;

static int indexCpt;

static int lastIndex = -1;
#define MAX_INDEX_BUFFER    10
static int output_register_only = 0;
static int enter_index_fct_counters = 0;
/*CIT*/

static void
push_output_target(FILE *f) {
    output_target_idx++;
    output_target[output_target_idx] = f;
}

static void
pop_output_target() {
    if (output_target_idx > 0) {
        output_target_idx--;
    }
}

static struct string_list {
    struct string_list      *next;
    unsigned char *data;
    int                     size;
    int                     id;
} *src_file_cache = NULL;

static struct label_list {
    struct label_list   *next;
    int                 id;
    int                 call_num;
} *label_cache = NULL;

/*CIT*/
static struct thru_label_list {
    struct thru_label_list  *next;
    int                     id;
    struct label_list       *label;
} *thru_label_cache = NULL;

struct attr_list  *attr_cache = NULL;

static struct literal_list {
    struct literal_list *next;
    struct cb_literal   *literal;
    cb_tree             x;
    int         id;
} *literal_cache = NULL;

static struct field_list {
    struct field_list   *next;
    struct cb_field     *f;
    cb_tree             x;
    const char          *curr_prog;
} *field_cache = NULL;

static struct call_list {
    struct call_list    *next;
    const char          *callname;
} *call_cache = NULL;

static struct base_list {
    struct base_list    *next;
    struct cb_field     *f;
    const char          *curr_prog;
} *base_cache = NULL;

static struct local_list {
    struct local_list   *next;
    struct cb_field     *f;
} *local_cache = NULL;

struct sort_list {
    struct sort_list    *next;
};

struct system_table {
    const char          *syst_name;
    const char          *syst_call;
};


#define COB_NAME_RTD(suff) "rtd_" #suff

static const struct system_table    system_tab[] = {
#undef  COB_SYSTEM_GEN
#define COB_SYSTEM_GEN(x, y, z) { x, COB_NAME_RTD(z) },
#include "libcob/system.def"
    { NULL, NULL }
};

/* Globals */
int has_external = 0;


static void output_stmt1(cb_tree x, cb_tree x_refcheck);
static void output_stmt(cb_tree x);
static void index_optimize_wipe_buffer(void);
static void output_integer(cb_tree x);
static void output_index_optimization(cb_tree x, int display);
static void add_new_to_index_buffer(cb_tree x);
static struct index_optimizer* add_to_head_index_buffer(cb_tree x, struct cb_field *f, struct index_optimizer **n, index_optimizer_type typeI,
                                                        int index, enum decimal_binary_type  binary_type);
static void output_index(cb_tree x, int national);
static void output_func_1(const char *name, cb_tree x);
static void output_param_1(cb_tree x, int id, int staticfield, int protect_null, int no_local_storage);
static void output_param(cb_tree x, int id);
static void output_param_static(cb_tree x, int id);
static void add_to_fieldcache(cb_tree x, struct cb_field *f);
static void output_func_name(const char *name, int argc);
static void output_occurs(struct cb_field *p);
static void output_funcall(cb_tree x);
static void output_initial_values(struct cb_field *p, int external);
static void output_cond(cb_tree x, int save_flag, int prev_op);
static void output_exit_program_deallocate(struct cb_program *prog);
static void output_param(cb_tree x, int id);
static void output_size_runtime(cb_tree x);
static void output_file_initialization(struct cb_file *f, int external_use_assign);


/*CIT*/
static struct thru_label_list*
lookup_thru_label_list(int id) {
    struct thru_label_list *tl;

    tl = thru_label_cache;
    while (tl) {
        if (tl->id == id) {
            return (tl);
        }
        tl = tl->next;
    }
    tl = cobc_malloc(sizeof(struct thru_label_list));
    tl->id = id;
    tl->next = thru_label_cache;
    thru_label_cache = tl;
    return (tl);
}

/*CIT*/
int
lookup_string(const unsigned char *data, int size) {
    struct string_list *p;
    int idx = 0;
    p = src_file_cache;
    while (p) {
        if ((size == p->size) &&
            ((size == 0) ||
             (memcmp((char *)p->data, (char *)data, size) == 0))) {
            return (p->id);
        }
        idx++;
        p = p->next;
    }
    p = src_file_cache;
    src_file_cache =  cobc_malloc(sizeof(struct string_list));
    if (size == 0) {
        src_file_cache->data = cobc_malloc(1);
    } else {
        src_file_cache->data = cobc_malloc(size + 1);
        memcpy(src_file_cache->data, data, size);
    }
    src_file_cache->size = size;
    src_file_cache->id = idx;
    src_file_cache->next = p;
    return (src_file_cache->id);
}

static char CStringBuffer[256];
static char *
lookup_Cstring(const unsigned char *Cstring) {
    int i = lookup_string(Cstring, strlen((char*)Cstring));
    sprintf(CStringBuffer, "%s%d", CB_PREFIX_STR, i);
    return CStringBuffer;
}

static void make_zombie_index(struct index_optimizer *elt) {
    elt->next = zombie;
    zombie = elt;
}
static void delete_element_from_cache(struct index_optimizer *elt) {
    if (elt->type == IndexIOT) {
        make_zombie_index(elt);
    } else {
        cob_free(elt);
    }
}
static void
lookup_call(const char *p) {
    struct call_list *clp;

    for (clp = call_cache; clp; clp = clp->next) {
        if (strcmp(p, clp->callname) == 0) {
            return;
        }
    }
    clp = cobc_malloc(sizeof(struct call_list));
    clp->callname = strdup(p);
    clp->next = call_cache;
    call_cache = clp;
}

static struct attr_list*
attr_list_reverse(struct attr_list *p) {
    struct attr_list    *next;
    struct attr_list    *last = NULL;

    for (; p; p = next) {
        next = p->next;
        p->next = last;
        last = p;
    }
    return (last);
}

static struct literal_list*
literal_list_reverse(struct literal_list *p) {
    struct literal_list *next;
    struct literal_list *last = NULL;

    for (; p; p = next) {
        next = p->next;
        p->next = last;
        last = p;
    }
    return (last);
}

static struct local_list*
local_list_reverse(struct local_list *p) {
    struct local_list   *next;
    struct local_list   *last = NULL;

    for (; p; p = next) {
        next = p->next;
        p->next = last;
        last = p;
    }
    return (last);
}

static struct label_list*
label_list_reverse(struct label_list *p) {
    struct label_list    *next;
    struct label_list    *last = NULL;

    for (; p; p = next) {
        next = p->next;
        p->next = last;
        last = p;
    }
    return (last);
}

static void mark_modified_element_decimal(struct index_optimizer *elt, struct index_optimizer_stack *io_stack) {
    struct index_optimizer *lst = NULL;
    struct index_optimizer *tmp;
    if (io_stack) {
        lst = io_stack->list;
    }
    if (lst == NULL) {
        return;
    }
    tmp = lst;
    while (tmp) {
        if (tmp->type == DecimalIOT) {
            if (is_identical_or_ovelap_fields(tmp->x, elt->x) || (tmp->index == elt->index)) {
                tmp->modified = 1;
            }
        }
        tmp = tmp->next;
    }
}

static void mark_modified_element_index(struct index_optimizer *elt, struct index_optimizer *lst) {
    struct index_optimizer *tmp = lst;
    if (lst == NULL) {
        return;
    }
    while (tmp) {
        if (tmp->type == IndexIOT) {
            if ((tmp->index == elt->index) || is_identical_or_ovelap_fields(tmp->x, elt->x)) {
                tmp->modified = 1;
            }
        }
        tmp = tmp->next;
    }
}

static int insert_decimal(cb_tree x, int index, enum decimal_binary_type  binary_type) {
    /*update index_optimize_cache list and mark modified stack list*/
    struct index_optimizer     *current, *prev, *tmp;
    int cpt = 0;
    struct cb_field     *f = NULL;
    prev = current = index_optimizer_cache;
    while (current) {
        if (current->type == DecimalIOT &&
            is_same_field(current->x, x) && current->binary_type == binary_type) {

            if (current->index != index) {
                mark_modified_element_decimal(current, io_stack);
                current->index = index;
                cpt++;
                /* break;*/
            } else { /*put found elt to head list*/
                if (current != index_optimizer_cache) {
                    prev->next = current->next;
                    current->next = index_optimizer_cache;
                    index_optimizer_cache = current;
                }
                return 1;
            }
        }
        prev = current;
        current = current->next;
    }
    prev = current = index_optimizer_cache;
    while (current) {
        if (current->type == DecimalIOT && current->index == index) {
            if (!is_same_field(current->x, x) || current->binary_type != binary_type) {
                mark_modified_element_decimal(current, io_stack);
                if (cpt > 0 || !x) {
                    /*remove*/
                    if (current == index_optimizer_cache) {
                        index_optimizer_cache = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    delete_element_from_cache(current);
                } else {
                    current->f = cb_field(x);
                    current->x = x;
                }
                cpt++;
                break;
            }
        }
        prev = current;
        current = current->next;
    }
    if (x && cpt == 0 && index >= 0) { /*insert new elt to cache list*/
        if (CB_REF_OR_FIELD_P(x)) f = cb_field(x);
        tmp = (cobc_malloc(sizeof(struct index_optimizer)));

        index_optimizer_cache = add_to_head_index_buffer(x, f, &tmp, DecimalIOT, index, binary_type);

    }
    return 0; /*no found*/
}

static int set_up_decimal_list(cb_tree func_x) {

    struct object_attached             *src;
    struct object_attached             *dst;

    cb_tree x1;
    int found = 0;

    if (!cb_flag_decimal_optimize) {
        return found;
    }

    src = func_x->src;
    dst = func_x->dst;
    if (src && dst) {
        /*set_field*/
        if (src->type == FieldType && dst->type == DecimalType) {
            return insert_decimal(src->val.x, dst->val.index, dst->val.binary_type);
        }
        /*get_field*/
        else if (src->type == DecimalType && dst->type == FieldType) {
            x1 = dst->val.x;
            /* Do not cache field writen ... the value  may be modify by get field */
            if (!x1->modified) {
                return insert_decimal(x1, -1, src->val.binary_type);
            }

        }
    } else if (dst && dst->type == DecimalType) {
        return insert_decimal(NULL, dst->val.index, dst->val.binary_type);

    }
    return found;
}

static void cp_io_list(struct index_optimizer **target, struct index_optimizer *src) {
    struct index_optimizer *p;
    struct index_optimizer *prev = NULL;

    while (src) {
        p = cobc_malloc(sizeof(struct index_optimizer));
        if (!*target) {
            *target = p;
        }
        if (prev) {
            prev->next = p;
        }
        *p = *src;
        prev = p;
        src = src->next;
    }
    if (prev) {
        prev->next = NULL;
    }
}

static void free_io_list(struct index_optimizer **src) {
    struct index_optimizer *p;

    while (*src) {
        p = (*src)->next;
        cob_free(*src)
            * src = p;
    }
}
static void initialize_modified_flag(struct index_optimizer *lst) {
    while (lst) {
        lst->modified = 0;
        lst = lst->next;
    }
}

static void mark_modified_all_list(struct index_optimizer *lst) {
    while (lst) {
        lst->modified = 1;
        lst = lst->next;
    }
}

static void push_io_to_stack(void) {
    struct index_optimizer_stack *tmp = cobc_malloc(sizeof(struct index_optimizer_stack));
    if (io_stack == NULL) {
        /*at the beginning of stack*/
        initialize_modified_flag(index_optimizer_cache);
    }
#if DEBUG
    output("/*push io*/");
#endif
    tmp->next = io_stack;
    io_stack = tmp;
    cp_io_list(&io_stack->list, index_optimizer_cache);
    io_stack->cpt = indexCpt;
}

static void update_stack_with_modified_flag_index(struct index_optimizer *dst, struct index_optimizer *src) {
    struct index_optimizer *tmp;
    if (dst == NULL) {
        return;
    }
    while (src) {

        if (src->modified) {
            tmp = dst;
            while (tmp) {
                if (/*is_identical_fields(tmp->x, src->x) && */tmp->index == src->index && tmp->type == src->type) {
                    tmp->modified = 1;
                    break;
                }
                tmp = tmp->next;
            }
        }
        src = src->next;
    }
}

static void remove_modified_register(struct index_optimizer **lst) {
    struct index_optimizer *prev,*curr,*tmp;
    curr = *lst; prev = *lst;
    while (curr) {
        if (curr->modified) {
            if (curr == *lst) {
                *lst = curr->next;
                prev = *lst;
            } else {
                prev->next = curr->next;
            }
            tmp = curr;
            curr = curr->next;
            delete_element_from_cache(tmp);

        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}
static void pop_stack_to_io(void) {
    struct index_optimizer_stack *tmp;

#if DEBUG
    output("/*pop io*/");
#endif

    free_io_list(&index_optimizer_cache);
    cp_io_list(&index_optimizer_cache, io_stack->list);
    if (lastIndex < indexCpt) {
        lastIndex = indexCpt;
    }
    indexCpt = io_stack->cpt;
    free_io_list(&(io_stack->list));
    tmp = io_stack;
    io_stack = tmp->next;
    if (io_stack) {
        update_stack_with_modified_flag_index(io_stack->list, index_optimizer_cache);
    }
    /*and the end of stack, remove modified elements*/
    remove_modified_register(&index_optimizer_cache);
    if (index_optimizer_cache == NULL) {
        indexCpt = 0;
    }
    cob_free(tmp);
}

static void copy_stack_to_io(void) {

#if DEBUG
    output("/*copy io*/");
#endif
    free_io_list(&index_optimizer_cache);
    cp_io_list(&index_optimizer_cache, io_stack->list);
    if (lastIndex < indexCpt) {
        lastIndex = indexCpt;
    }
    indexCpt = io_stack->cpt;
}

/*
 * Output routines
 */

static FILE*
current_output(void) {
    return output_target[output_target_idx];
}


void
output(const char *fmt, ...) {
    va_list ap;

    if (output_target[output_target_idx]) {
        va_start(ap, fmt);
        vfprintf(output_target[output_target_idx], fmt, ap);
        va_end(ap);
    }
}

static void
output_newline(void) {
    if (output_target[output_target_idx]) {
        fputs("\n", output_target[output_target_idx]);
    }
}

static void
output_prefix(void) {
    int i;

    if (output_target[output_target_idx]) {
        for (i = 0; i < output_indent_level; i++) {
            fputc(' ', output_target[output_target_idx]);
        }
    }
}

void
output_line(const char *fmt, ...) {
    va_list ap;

    if (output_target[output_target_idx]) {
        output_prefix();
        va_start(ap, fmt);
        vfprintf(output_target[output_target_idx], fmt, ap);
        va_end(ap);
        fputc('\n', output_target[output_target_idx]);
    }
}

static void
output_trace(int newline, int cnt , const char *fmt, ...) {
    va_list ap;
    char buffer[2048];

    if (output_target[output_target_idx]) {
        output_prefix();
        va_start(ap, fmt);
        fputs("if (!rtd->cob_disable_trace)",output_target[output_target_idx]);
        sprintf (buffer, "  cob_display(rtd,%d,%d, %d,", cb_flag_trace_sysout ? COB_DEVICE_SYSOUT : COB_DEVICE_SYSERR, newline, cnt);
        fputs(buffer, output_target[output_target_idx]);
        vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
        va_end(ap);
        fputs(buffer, output_target[output_target_idx]);
        fputs(");\n", output_target[output_target_idx]);
    }
}

void
output_indent(const char *str) {
    const char  *p;
    int         level = 2;

    for (p = str; *p == ' '; p++) {
        level++;
    }

    if (*p == '}' && strcmp(str, "})") != 0) {
        output_indent_level -= level;
    }

    output_line(str);

    if (*p == '{' && strcmp(str, ")}") != 0) {
        output_indent_level += level;
    }
}

static void
output_string(const unsigned char *s, int size) {
    int i;
    int c;
    int printable = 1;

    for (i = 0; i < size; i++) {
        if ((s[i] < 0x20) || (s[i] >= 0x7f)) {
            printable = 0;
            break;
        }
    }

    output("\"");
    for (i = 0; i < size; i++) {
        c = s[i];
        if (printable) {
            if (c == '\"' || c == '\\' || c == '\?') {
                output("\\%c", c);
            } else {
                output("%c", c);
            }
        } else {
            output("\\%03o", c);
        }
    }
    output("\"");
}

void
output_storage(const char *fmt, ...) {
    va_list ap;

    if (cb_storage_file) {
        va_start(ap, fmt);
        vfprintf(cb_storage_file, fmt, ap);
        va_end(ap);
    }
}

void
output_debug(const char *fmt, ...) {
    va_list ap;
    if (cb_debug_file) {
        va_start(ap, fmt);
        vfprintf(cb_debug_file, fmt, ap);
        va_end(ap);
    }
}

void
output_global(const char *fmt, ...) {
    va_list ap;
    if (cb_global_file) {
        va_start(ap, fmt);
        vfprintf(cb_global_file, fmt, ap);
        va_end(ap);
    }
}

void
output_local(const char *fmt, ...) {
    va_list ap;

    if (current_codegen_prog->local_storage_file) {
        va_start(ap, fmt);
        vfprintf(current_codegen_prog->local_storage_file, fmt, ap);
        va_end(ap);
    }
}

void
output_local_register(const char *fmt, ...) {
    va_list ap;

    if (current_codegen_prog->local_register_file) {
        va_start(ap, fmt);
        vfprintf(current_codegen_prog->local_register_file, fmt, ap);
        va_end(ap);
    }
}

void
output_local_function(const char *fmt, ...) {
    va_list ap;

    if (cb_local_function_file) {
        va_start(ap, fmt);
        vfprintf(cb_local_function_file, fmt, ap);
        va_end(ap);
    }
}

void
output_static(const char *fmt, ...) {
    va_list ap;

    if (current_codegen_prog->static_storage_file) {
        va_start(ap, fmt);
        vfprintf(current_codegen_prog->static_storage_file, fmt, ap);
        va_end(ap);
    }
}

/*
 * Field
 */
static void
output_field_size(struct cb_field  *f, enum cb_operand_type type, int fullsize) {
    struct cb_field     *p;
    struct cb_field     *q;
    if (fullsize) {
        p = f->occurs_depending ? f : cb_field_variable_size(f);
    } else {
        p = cb_field_variable_size(f);
    }
    q = f;

again:
    if (p) {
        if (type == CB_SENDING_OPERAND || !cb_field_subordinate(cb_field(p->occurs_depending), q)) {
            if (p->offset - q->offset > 0) {
                output("%d + ", p->offset - q->offset);
            }
            if (p->size != 1) {
                output("%d * ", p->size);
            }
            output_occurs(p);
            q = p;
        } else {
            output("%d", q->size);
        }
    } else {
        if (fullsize /*&& type == CB_SENDING_OPERAND*/) {
            output("%d", q->size * q->occurs_max);
        } else {
            output("%d", q->size);
        }
    }

    for (; q != f; q = q->parent) {
        while (q->sister && q->sister->redefines) {
            q = q->sister;
        }
        if (q->sister && !q->sister->redefines) {
            q = q->sister;
            p = q->occurs_depending && (cb_flag_odo_slide || (q->sister == NULL)) ? q : cb_field_variable_size(q);
            output(" + ");
#if DEBUG_BASE
            output(" /*%s*/", q->name);
#endif

            goto again;
        }
    }

}
static void
output_base_root(struct cb_field     *f) {
    /*CIT THis is a very bad hack for debuger*/
    /* output ("%s%s", CB_PREFIX_BASE, name);*/
    if (debug_prefix_base && (f->storage == CB_STORAGE_LINKAGE) &&  !f->flag_unused_linkage) {
        output("%s%d", debug_prefix_base, f->id);
    } else {
        output("%s%d", CB_PREFIX_BASE, f->id);
    }

}


static void
output_base_allocation(struct cb_field *f01) {
    struct base_list    *bl;
    if (!f01->flag_base) {
        /*CIT if ( !f01->flag_external ) */
        {
            if (f01->flag_external || !f01->flag_local || f01->flag_is_global) {
                bl = cobc_malloc(sizeof(struct base_list));
                bl->f = f01;
                bl->curr_prog = excp_current_program_id;
                bl->next = base_cache;
                base_cache = bl;
            } else {
                if (current_codegen_prog->flag_global_use) {
                    output_local("unsigned char\t\t*%s%d = NULL;",
                                 CB_PREFIX_BASE, f01->id);
                    output_local("\t/* %s */\n", f01->name);
                    output_static("unsigned char\t*save_%s%d;\n",
                                  CB_PREFIX_BASE, f01->id);
                } else {
                    output_local("unsigned char\t*%s%d = NULL;",
                                 CB_PREFIX_BASE, f01->id);
                    output_local("\t/* %s */\n", f01->name);
                }
            }
        }

        f01->flag_base = 1;
    }

}
static void
output_base(cb_tree x, int root_only) {
    struct cb_field     *f;
    struct cb_reference *r;
    struct cb_field     *f01;
    struct cb_field     *p;

    r = CB_REFERENCE(x);
    f = CB_FIELD(r->value);
    f01 = cb_field_founder(f);

    if (f->flag_item_78) {
        fprintf(stderr, "Unexpected CONSTANT item\n");
        ABORT();
    }

    if (f01->flag_is_fcd_reg) {
        if (f01->file == NULL) {
            fprintf(stderr, "FH--FCD request and file == NULL\n");
            ABORT();
        }

        output("cob_get_extfh_fcd_ptr(rtd,%s%s)", CB_PREFIX_FILE, f01->file->cname);

        return;
    }
    while (f01->redefines) {
        f01 = f01->redefines;
    }
#if DEBUG_BASE
    output(" /*BB*/");
#endif
    output_base_allocation(f01);
    output_base_root(f01);
    if (!root_only) {

        while (f->redefines) {
            f = f->redefines;
        }
        if (cb_field_variable_address(f) && !r->cb_all_occurs) {

            /*
            for ( p = f->parent; p; f = f->parent, p = f->parent ) {
                for ( p = p->children; p && p != f; p = p->sister ) {
                    if ( !p->redefines ) 
                      {
                        v = p->occurs_depending ? p : cb_field_variable_size (p);
                        if ( v ) {
                            #if DEBUG_BASE
                            output (" / *Bv* /");
                            #endif
                            output (" + %d + ", v->offset - p->offset);
                            if ( v->size != 1 ) {
                                output ("%d * ", v->size);
                            }
                            output_integer (v->occurs_depending);
                        } else {
                            #if DEBUG_BASE
                            output (" / *Bnv* /");
                            #endif
                            output (" + %d", p->size * p->occurs_max);
                        }
                    }
                }
             */
#if DEBUG_BASE
            output(" /*BB1 %s */", f->name);
#endif
            for (p = f; f; f = f->parent, p = f) {
                while (p && p->redefines) {
                    p = p->redefines;
                }
                for (p = p->brother; p; p = p->brother) {
                    if (!p->redefines) {
                        output(" + ");
                        output_field_size(p, CB_SENDING_OPERAND, 1);
                    }
                }
            }
        } else if (f->offset > 0) {
            output(" + %d", f->offset);
        }
    }
#if DEBUG_BASE
    output(" /*BE*/");
#endif
}

static void
output_rtncode_size(void) {
    switch (cb_rtncode_size) {
    case 2:
        output("short"); break;
    case 4:
        output("int");   break;
    case 8:
        output("long long");  break;
    default:
        output("int"); break;
    }
}

/*CIT*/
static void
output_debug_callback(const int debug_action_code) {
    if (cb_flag_debuginfo && !suspend_debug_location) {
        output_line("if (cob_debug_callback) cob_debug_callback(rtd, %d);", debug_action_code);
    }
}

/*CIT*/
static void
output_meminfo_location(cb_tree x) {
    if (x && x->source_line && x->source_file) {
        cur_source_file = x->source_file;
        cur_source_line = x->source_line;
    }
    if (!cb_flag_source_location && cb_flag_mem_info &&
        cur_source_file && cur_source_line) {
        output_prefix();
        output("rtd->current_module->cur_line = %d;\n", cur_source_line);
        output_prefix();
        output("rtd->current_module->cur_source_file = %s;\n", lookup_Cstring(cur_source_file));
    }
}
static void
output_reg_name(cb_tree x) {
    char buffer[200];
    switch (CB_DECIMAL(x)->binary) {
    case none_DBT:
        output("&(COB_MDS %s%s.d%d)", CB_PREFIX_FIELD, current_codegen_prog->program_id,  CB_DECIMAL(x)->id);
        break;
    case int_DBT:
    case uint_DBT:
    case IEEE_float_DBT:
    case IEEE_double_DBT:
        output(binary_reg_name(buffer, CB_DECIMAL(x)->binary, CB_DECIMAL(x)->id));
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected decimal type %d\n", CB_DECIMAL(x)->binary);
        ABORT();
        break;
    }

}

static void
output_data_1(cb_tree x, int protect_null) {
    struct cb_literal   *l;
    struct cb_reference *r;
    struct cb_field     *f;
    cb_tree             lsub;
    struct cb_cast      *cp;
    cb_tree l1;
    int sav_stack_id;

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_LITERAL:
        l = CB_LITERAL_ENCODED(x);
        if (CB_TREE_CLASS(x) == CB_CLASS_NUMERIC) {
            int s = 0;
            output("(unsigned char *)");
            if (l->sign) {
                /* build Literal add 2 byte at 0 for this purpose */
                l->data[l->size] = (l->sign < 0) ? CHAR_MINUS : CHAR_PLUS;
                s = 1;
            }
            output_string(l->data, l->size + s);
        } else {
            output("(unsigned char *) %s%d", CB_PREFIX_STR, lookup_string(l->data, (int)l->size));
        }
        break;
    case CB_TAG_REFERENCE:
        r = CB_REFERENCE(x);
        if (r->check) {
            if (cb_flag_gen_gcc) {
                output_indent(" ({");
            } else {
                inside_stack[inside_check] = 0;
                ++inside_check;
                output(" (\n");
            }
            for (l1 = r->check; l1; l1 = CB_CHAIN(l1)) {
                sav_stack_id = stack_id;
                output_stmt(CB_VALUE(l1));
                stack_id = sav_stack_id;
                if (!cb_flag_gen_gcc) {
                    output(",\n");
                }
            }
        }
        if (CB_FILE_P(cb_ref(x))) {
            //           output("cob_get_extfh_fcd_ptr(rtd,%s%s)", CB_PREFIX_FILE, CB_FILE(cb_ref(x))->cname);
            output_param(cb_ref(x), -1);
        } else {
            f = CB_FIELD(r->value);

            if (protect_null) {
                /* Base address */
                output(" ( ");
                output_base(x, 1);
                output(" ? ");
            }
            /* Base address */
            output_base(x, 0);

            /* Subscripts */
            if (r->subs) {
                lsub = r->subs;
                for (; f /*CIT*/ && lsub; f = f->parent) {
                    if (f->flag_occurs) {
                        if (f->usage != CB_USAGE_BIT) {
                            if (CB_VALUE(lsub) != cb_int1) {
                                output(" + ");
#if DEBUG_BASE
                                output(" /*Bs*/");
#endif
                                if (f->size != 1) {
                                    output("%d * ", f->size);
                                }
                                if (CB_EXCEPTION_ENABLE(COB_EC_BOUND_SUBSCRIPT) && !cb_disable_runtime_check &&
                                    (f->storage != CB_STORAGE_LINKAGE || cb_flag_check_linkage_bound)) {
                                    output("(");

                                    if (f->occurs_depending) {
                                        output_funcall(cb_build_funcall_6("cob_check_odo_1",
                                                                          cb_build_cast_integer(CB_VALUE(lsub)),
                                                                          cb_build_cast_integer(f->occurs_depending),
                                                                          cb_int(f->occurs_min),
                                                                          cb_int(f->occurs_max),
                                                                          cb_build_string0((ucharptr)(cb_field(f->occurs_depending)->name)),
                                                                          cb_build_string0((ucharptr)r->word->name)));
                                    } else {
                                        output_funcall(cb_build_funcall_4("cob_check_subscript_1",
                                                                          cb_build_cast_integer(CB_VALUE(lsub)),
                                                                          cb_int1,
                                                                          cb_int(f->occurs_max),
                                                                          cb_build_string0((ucharptr)r->word->name)));
                                    }
                                    output(" - 1)");
                                } else {
                                    output_index(CB_VALUE(lsub), 0);
                                }
                            }
                        }
                        lsub = CB_CHAIN(lsub);
                    }
                }
            }

            /* Offset */
            if (r->offset) {
                output(" + ");
#if DEBUG_BASE
                output(" /*Bo*/ ");
#endif
                output_index(r->offset, CB_TREE_CATEGORY(x) == CB_CATEGORY_NATIONAL);
                /*f && f->pic && ( f->pic->category == CB_CATEGORY_NATIONAL */
                /*f->usage == CB_USAGE_NATIONAL*/
            }
            if (protect_null) {
                /* Base address */
                output(" : (void*)0 )");
            }
        }
        if (r->check) {
            if (cb_flag_gen_gcc) {
                output("; })");
            } else {
                --inside_check;
                output(" )");
            }
        }
        break;
    case CB_TAG_CAST:
        cp = CB_CAST(x);
        switch (cp->type) {
        case CB_CAST_INTEGER:
        case CB_CAST_DOUBLE:
        case CB_CAST_ADDRESS:
            output_data_1(cp->val, 0);
            break;
        case CB_CAST_ADDR_OF_ADDR:
            output("&");
            output_data_1(cp->val, 0);
            break;
        case CB_CAST_LENGTH:
            output_size_runtime(cp->val);
            break;
        case CB_CAST_PROGRAM_POINTER:
            output_param(cp->val, 0);
            break;
        }
        break;
    case CB_TAG_INTRINSIC:
        //output("module->cob_procedure_parameters[%d]->data", field_iteration);
        output_param(x, 0);
        output("->data");
        break;
    case CB_TAG_CONST:
        if (x == cb_null) {
            output("NULL");
            return;
        }
        output("%s", CB_CONST(x)->val);
        break;
    case CB_TAG_DECIMAL:
        output_reg_name(x);
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
}

static void
output_data(cb_tree x) {
    output_data_1(x, 0);
}

static void
output_size_1(cb_tree x, int FullSize) {
    struct cb_literal   *l;
    struct cb_reference *r;
    struct cb_field     *f;

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_CONST:
        output("1");
        break;
    case CB_TAG_LITERAL:
        l = CB_LITERAL(x);
        output("%d", (int)(l->size + ((l->sign != 0) ? 1 : 0)));
        break;

    case CB_TAG_REFERENCE:
        r = CB_REFERENCE(x);
        f = CB_FIELD(r->value);
        if (r->length) {
            if (f->pic && (f->pic->category == CB_CATEGORY_NATIONAL)) {
                /*if ( f->usage == CB_USAGE_NATIONAL ) {*/
                output("(2 * ");
                output_integer(r->length);
                output(")");
            } else {
                output_integer(r->length);
            }
        } else if (r->offset) {
            output("%d - ", f->size);
            /* output_index (r->offset, f->usage == CB_USAGE_NATIONAL); */
            output_index(r->offset, (f->pic && (f->pic->category == CB_CATEGORY_NATIONAL)));
        } else {
            if (r->subs) {
                FullSize = 0;
            }
            output_field_size(f, r->type, FullSize);
        }
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
}

static void
output_size_runtime(cb_tree x) {
    output_size_1(x, 1);
}

static void
output_size_storage(cb_tree x) {
    output_size_1(x, 0);
}

static int
lookup_attr(int type, int digits, int scale, int flags, unsigned char *pic, int lenstr) {
    struct attr_list *l;

    /* Search attribute cache */
    for (l = attr_cache; l; l = l->next) {
        if (type == l->type
            && digits == l->digits
            && scale == l->scale && flags == l->flags
            && ((pic == l->pic) || (pic && l->pic && lenstr == l->lenstr
                                    && memcmp((char *)pic, (char *)(l->pic), (size_t)lenstr) == 0))) {
            return (l->id);
        }
    }

    /* Output new attribute */

    /* Cache it */
    l = cobc_malloc(sizeof(struct attr_list));
    l->id = cb_attr_id;
    l->type = type;
    l->digits = digits;
    l->scale = scale;
    l->flags = flags;
    l->pic = pic;
    l->lenstr = lenstr;
    l->next = attr_cache;
    attr_cache = l;

    return (cb_attr_id++);
}

int cb_build_attr(cb_tree x, int is_bitarray) {
    struct cb_literal   *l;
    struct cb_reference *r;
    struct cb_field     *f;
    int         id;
    int         type;
    int         flags;

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_LITERAL:
        l = CB_LITERAL(x);
        if (CB_TREE_CLASS(x) == CB_CLASS_NUMERIC) {
            flags = 0;
            if (l->sign != 0) {
                flags = COB_FLAG_HAVE_SIGN | COB_FLAG_SIGN_SEPARATE;
            }
            id = lookup_attr(COB_TYPE_NUMERIC_DISPLAY,
                             (int)l->size, l->scale, flags, NULL, 0);
        } else if (CB_TREE_CLASS(x) == CB_CLASS_NATIONAL) {
            id = lookup_attr(COB_TYPE_NATIONAL, 0, 0, 0, NULL, 0);
        } else {
            if (l->all) {
                id = lookup_attr(COB_TYPE_ALPHANUMERIC_ALL, 0, 0, 0, NULL, 0);
            } else {
                id = lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
            }
        }
        break;
    case CB_TAG_REFERENCE:
        type = cb_tree_type(x);
        r = CB_REFERENCE(x);
        f = CB_FIELD(r->value);
        flags = 0;
        if (r->offset) {
            switch (type) {
            case COB_TYPE_NATIONAL :
            case COB_TYPE_NATIONAL_EDITED:
                id = lookup_attr(COB_TYPE_NATIONAL, 0, 0, 0, NULL, 0);
                break;
            default:
                id = lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
                break;
            }
        } else {
            switch (type) {
            case COB_TYPE_GROUP:
            case COB_TYPE_ALPHANUMERIC:
                if (f->flag_justified) {
                    id = lookup_attr(type, 0, 0, COB_FLAG_JUSTIFIED, NULL, 0);
                } else {
                    id = lookup_attr(type, 0, 0, 0, NULL, 0);
                }
                break;
            default:
                if (f->pic->have_sign) {
                    flags |= COB_FLAG_HAVE_SIGN;
                    if (f->flag_sign_separate) {
                        flags |= COB_FLAG_SIGN_SEPARATE;
                    }
                    if (f->flag_sign_leading) {
                        flags |= COB_FLAG_SIGN_LEADING;
                    }
                } else { /* COMP-6 */
                    if (f->usage == CB_USAGE_COMP_6) {
                        flags |= COB_FLAG_PACKED_SIGN_MISSING;
                    }
                }
                if (f->usage != CB_USAGE_DISPLAY) {
                    if (f->flag_compx_notrunc) {
                        flags |= COB_FLAG_BINARY_NOTRUNC;
                    }
                }
                if (f->flag_blank_zero) {
                    flags |= COB_FLAG_BLANK_ZERO;
                }
                if (f->flag_justified) {
                    flags |= COB_FLAG_JUSTIFIED;
                }
                if (f->flag_binary_swap) {
                    flags |= COB_FLAG_BINARY_SWAP;
                }
                if (f->flag_real_binary) {
                    flags |= COB_FLAG_REAL_BINARY;
                }
                if (f->flag_is_pointer) {
                    flags |= COB_FLAG_IS_POINTER;
                }
                if (type == COB_TYPE_BITS && is_bitarray) {
                    type = COB_TYPE_BITS_ARRAY;
                }
                id = lookup_attr(type, f->pic->digits, f->pic->scale,
                                 flags, (ucharptr)f->pic->str, f->pic->lenstr);
                break;
            }
        }
        break;
    case CB_TAG_ALPHABET_NAME:
        id = lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
    return id;
}

struct attr_list*
cb_get_attr(int id) {
    struct attr_list *l;

    /* Search attribute cache */
    for (l = attr_cache; l; l = l->next) {
        if (id == l->id) return (l);
    }
    return NULL;
}

static void
output_attr(cb_tree x, int is_bitarray) {
    output("&%s%d", CB_PREFIX_ATTR, cb_build_attr(x, is_bitarray));
}

static void
output_field(cb_tree x) {
    if (x) {
        output("{");
        output_size_storage(x);
        output(", ");
        output_data(x);
        output(", ");
        output_attr(x, 0);
        output("}");
    } else {
        output("{0, NULL, NULL}");
    }
}

/*
 * Literal
 */

static int
lookup_literal(cb_tree x) {

    struct cb_literal   *literal;
    struct literal_list *l;

    literal = CB_LITERAL(x);
    /* Search literal cache */
    for (l = literal_cache; l; l = l->next) {
        if (CB_TREE_CLASS(literal) == CB_TREE_CLASS(l->literal)
            && literal->size == l->literal->size
            && literal->all == l->literal->all
            && literal->sign == l->literal->sign
            && literal->scale == l->literal->scale
            && literal->encoded == l->literal->encoded
            && memcmp(literal->data, l->literal->data, literal->size) == 0) {
            return (l->id);
        }
    }

    /* Output new literal */
    push_output_target(NULL);
    output_field(x);
    pop_output_target();

    /* Cache it */
    l = cobc_malloc(sizeof(struct literal_list));
    l->id = cb_literal_id;
    l->literal = literal;
    l->x = x;
    l->next = literal_cache;
    literal_cache = l;

    return (cb_literal_id++);
}

/*
 * Integer
 */
static char*
map_bin_oper(char oper) {
    static char r[5];
    switch (oper) {
    case 'a':
        return(char *)" & ";
    case 'o':
        return(char *)" | ";
    case 'y':
        return(char *)" ^ ";
    case '&':
        return(char *)" && ";
    case '|':
        return(char *)" || ";
    default:
        r[0] = ' ';
        r[1] = oper;
        r[2] = ' ';
        r[3] = 0;
        return r;
    }
}

static void
output_integer_1(cb_tree x) {
    if (CB_TREE_TAG(x) == CB_TAG_REFERENCE) {
        struct cb_field *f = cb_field(x);
        if (f->usage == CB_USAGE_PROGRAM_POINTER) {
#ifdef  COB_NON_ALIGNED
            output("(cob_get_pointer (rtd, ");
            output_data(x);
            output("))");
#else
            output("(*(unsigned char **) (");
            output_data(x);
            output("))");
#endif
            return;
        }
    }
    output_integer(x);
}

static struct index_optimizer*
add_to_head_index_buffer(cb_tree x, struct cb_field *f, struct index_optimizer **n, index_optimizer_type typeI,
                         int index, enum decimal_binary_type  binary_type) {
    struct index_optimizer *nextIndex;
    nextIndex = *n;
    *n = (*n)->next;
    nextIndex->f = f;
    nextIndex->type = typeI;
    nextIndex->binary_type = binary_type;
    nextIndex->x = x;
    nextIndex->nbr_in_one_funcall = 0;
    if (index != 0) {
        nextIndex->index = index;
    }
    nextIndex->next = index_optimizer_cache;
    if (typeI == IndexIOT && io_stack) {
        mark_modified_element_index(nextIndex, io_stack->list);
    }
    return nextIndex;
}

void delete_list(void) {
    decimal_optimizer *current;
    if (!cb_flag_index_optimize && !cb_flag_decimal_optimize) {
        return;
    }
    /*when we have to wipe list 
      have to keep field with decimals*/

    if (current_program) {
        while (current_program->decimal_list) {
            current = current_program->decimal_list;
            current_program->decimal_list = current->next;
            cob_free(current);
        }
        if (current_program->decimal_index > current_program->decimal_index_max) {
            current_program->decimal_index_max = current_program->decimal_index;
        }
        current_program->decimal_index = 0;

        while (current_program->zombie) {
            current = current_program->zombie;
            current_program->zombie = current->next;
            cob_free(current);
        }
    }
}

static void initialize_index_buffer() {


    if (lastIndex < indexCpt) {
        lastIndex = indexCpt;
    }
    indexCpt = 0;
}

static void add_new_to_index_buffer(cb_tree x) {
    struct cb_field *f;
    struct index_optimizer *prevOldestIndex, *oldestIndex, *prev, *current;
    if (CB_REF_OR_FIELD_P(x)) f = cb_field(x);
    if (indexCpt < MAX_INDEX_BUFFER) {
        struct index_optimizer *tmp = (cobc_malloc(sizeof(struct index_optimizer)));
        index_optimizer_cache = add_to_head_index_buffer(x, f, &tmp, IndexIOT, indexCpt++, none_DBT);
    } else {
        if (zombie) {
            index_optimizer_cache = add_to_head_index_buffer(x, f, &zombie, IndexIOT, 0, none_DBT);

        } else {
            /*pick a victime, the last one in the list*/
            oldestIndex = NULL;
            prevOldestIndex = NULL;
            prev = current = index_optimizer_cache;
            while (current) {
                if (current->type == IndexIOT) {
                    oldestIndex = current;
                    prevOldestIndex = prev;
                }
                prev = current;
                current = current->next;
            }
            if (oldestIndex) {
                if (prevOldestIndex) {
                    prevOldestIndex->next = oldestIndex->next;
                }
                index_optimizer_cache = add_to_head_index_buffer(x, f, &oldestIndex, IndexIOT, 0, none_DBT);
            } else {
                struct index_optimizer *tmp = (cobc_malloc(sizeof(struct index_optimizer)));
                index_optimizer_cache = add_to_head_index_buffer(x, f, &tmp, IndexIOT, indexCpt++, none_DBT);
            }

        }
    }
}

static void output_get_numdisp_1(cb_tree x) {
    struct cb_field *f;
    f = cb_field(x);
    if (!cb_flag_old_disp_to_int) {
        output("cob_get_numdisp_2 (rtd, ");
    } else {
        output("cob_get_numdisp_1 (rtd, ");
    }
    output_data(x);
    output(", %d)", f->size - f->pic->scale);
}
static void output_get_packed_int(cb_tree x) {
    output_func_1("-cob_get_packed_int", x);
}
static void output_index_optimization(cb_tree x, int display) {

    struct index_optimizer *cpt;
    struct index_optimizer *prev;

    unsigned int mNew = 1;

    prev = cpt = index_optimizer_cache;
    if (cpt) {
        do {
            if (cpt->type == IndexIOT) {
                if (is_same_field(x, cpt->x)) {
                    cpt->nbr_in_one_funcall++;
                    if (cpt->nbr_in_one_funcall > 1) {
                        (display == 1) ? output_get_numdisp_1(x) :  output_get_packed_int(x);
                    } else output("r%d", cpt->index);
                    mNew = 0;
                    if (cpt != index_optimizer_cache) {
                        prev->next = cpt->next;
                        cpt->next = index_optimizer_cache;
                        index_optimizer_cache = cpt;
                    }

                    break;
                }
            }
            prev = cpt;
            cpt = cpt->next;

        }while (cpt);

    }
    if (mNew) {
        if (output_register_only && enter_index_fct_counters == 0) {
            pop_output_target();
        }
        add_new_to_index_buffer(x);
        index_optimizer_cache->nbr_in_one_funcall++;

        output("( ");
        output("r%d = ", index_optimizer_cache->index);
        enter_index_fct_counters++;
        (display == 1) ? output_get_numdisp_1(x) :  output_get_packed_int(x);
        enter_index_fct_counters--;
        output(") ");
        if (output_register_only && enter_index_fct_counters == 0) {
            output(";\n");
            push_output_target(NULL);
        }
    }

}
static void
output_integer(cb_tree x) {
    struct cb_binary_op *p;
    struct cb_cast      *cp;
    struct cb_field     *f;

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_CONST:
        if (x == cb_zero) {
            output("0");
        } else if (x == cb_one) {
            output("1");
        } else if (x == cb_space) {
            output("%d", CHAR_SP);
        } else if (x == cb_dirsep) {
            output("%d", *(CB_LITERAL(cb_dirsep)->data));
        } else if (x == cb_quote) {
            output("(unsigned int)(cb_quote->val[0])");
        } else if (x == cb_null) {
            output("((unsigned char *)0)");
        } else {
            output("%s", CB_CONST(x)->val);
        }
        break;
    case CB_TAG_INTEGER:
        output("%d", CB_INTEGER(x)->val);
#ifdef DEBUG
        output(" /*0x%X*/", CB_INTEGER(x)->val);
#endif
        break;
    case CB_TAG_FUNCALL:
        if (!output_register_only && set_up_decimal_list(x)) {
            break;
        }
        output("((int) ");
        output_funcall(x);
        output(")");
        break;
    case CB_TAG_LITERAL:
        output("%d", cb_get_int(x));
#ifdef DEBUG
        output(" /*0x%X*/", cb_get_int(x));
#endif
        break;
    case CB_TAG_BINARY_OP:
        p = CB_BINARY_OP(x);
        switch (p->op) {
        case 'v':
        case 'x':
            output_integer(p->x);
            break;
        case 'n':
            output("~");
            if (p->y) {
                output("~");
            }
            output_integer(p->x);
            if (p->y) {
                output(")&(");
                output_integer(p->y);
            }
            output(")");
            break;
        case '^':
            output("(int) pow (");
            output_integer(p->x);
            output(", ");
            output_integer(p->y);
            output(")");
            break;
        default:
            output("(");
            if (need_double) {
                output("(double)");
            }
            output_integer_1(p->x);
            output(map_bin_oper(p->op));
            if (need_double) {
                output("(double)");
            }
            output_integer_1(p->y);
            output(")");
            break;
        }
        break;
    case CB_TAG_CAST:
        cp = CB_CAST(x);
        switch (cp->type) {
        case CB_CAST_ADDRESS:
            output("(");
            output_data(cp->val);
            output(")");
            break;
        case CB_CAST_PROGRAM_POINTER:
            output_func_1("cob_call_resolve_2", x);
            break;
        case CB_CAST_LENGTH:
            output_size_runtime(cp->val);
            break;
        default:
            fprintf(stderr, "cobc:0: Unexpected cast type %d\n", cp->type);
            ABORT();
        }
        break;
    case CB_TAG_REFERENCE:
        f = cb_field(x);
        switch (f->usage) {
        case CB_USAGE_INDEX:
        case CB_USAGE_LENGTH:
#ifdef  COB_NON_ALIGNED
            if (!is_base_alligned(x)) {
                output_func_1("cob_get_int", x);
            } else
#endif
            {
                if (cb_flag_gen_gcc && cb_flag_gcc_O_bug) {
                    output("(*(int volatile *) (");
                } else {
                    output("(*(int *) (");
                }
                output_data(x);
                output("))");
            }
            return;

        case CB_USAGE_POINTER:
#ifdef  COB_NON_ALIGNED
            output("(cob_get_pointer (rtd, ");
            output_data(x);
            output("))");
#else
            output("(*(unsigned char **) (");
            output_data(x);
            output("))");
#endif
            return;

        case CB_USAGE_PROGRAM_POINTER:
#ifdef  COB_NON_ALIGNED
            output("(cob_get_prog_pointer (rtd, ");
            output_data(x);
            output("))");
#else
            output("(*(void **) (");
            output_data(x);
            output("))");
#endif
            return;

        case CB_USAGE_DISPLAY:
            if (f->pic && f->pic->scale >= 0
                && f->size - f->pic->scale > 0
                && f->size - f->pic->scale <= 9
                && f->pic->have_sign == 0) {
                if (cb_flag_index_optimize) {
                    output_index_optimization(x, 1);
                } else {
                    output_get_numdisp_1(x);
                }
                return;
            }
            break;

        case CB_USAGE_PACKED:
            if (f->pic->scale == 0 && f->pic->digits < 10) {
                if (cb_flag_index_optimize) {
                    output_index_optimization(x, 0);
                } else output_get_packed_int(x);
                return;
            }
            break;

        case CB_USAGE_BINARY:
        case CB_USAGE_COMP_5:
        case CB_USAGE_COMP_X:
            if (f->size == 1) {
                output("(*(");
                if (!f->pic->have_sign) {
                    output("unsigned ");
                } else {
                    output("signed ");
                }
                output("char *) (");
                output_data(x);
                output("))");
                return;
            }
#ifdef  COB_NON_ALIGNED
            if (f->storage != CB_STORAGE_LINKAGE && f->indexes == 0 && (
#ifdef  COB_SHORT_BORK
                                                                        (f->size == 2 && (f->offset % 4 == 0)) ||
#else
                                                                        (f->size == 2 && (f->offset % 2 == 0)) ||
#endif
                                                                        (f->size == 4 && (f->offset % 4 == 0)) ||
                                                                        (f->size == 8 && (f->offset % 8 == 0)))) {
#else
            if (f->size == 2 || f->size == 4 || f->size == 8) {
#endif
                if (f->flag_binary_swap) {
                    output("((");
                    if (!f->pic->have_sign) {
                        output("unsigned ");
                    }
                    switch (f->size) {
                    case 2:
                        output("short)COB_BSWAP_16(");
                        break;
                    case 4:
                        output("int)COB_BSWAP_32(");
                        break;
                    case 8:
                        output("long long)COB_BSWAP_64(");
                        break;
                    }
                    output("*(");
                    switch (f->size) {
                    case 2:
                        output("short *)(");
                        break;
                    case 4:
                        output("int *)(");
                        break;
                    case 8:
                        output("long long *)(");
                        break;
                    }
                    output_data(x);
                    output(")))");
                    return;
                } else {
                    output("(*(");
                    if (!f->pic->have_sign) {
                        output("unsigned ");
                    }
                    switch (f->size) {
                    case 2:
                        output("short *)(");
                        break;
                    case 4:
                        output("int *)(");
                        break;
                    case 8:
                        output("long long *)(");
                        break;
                    }
                    output_data(x);
                    output("))");
                    return;
                }
            }
            if (f->pic->have_sign == 0) {
                if (f->size > 4) {
                    output("(unsigned long long)");
                } else {
                    output("(unsigned int)");
                }
            }
            break;

        default:
            break;
        }

        if (f->size > 4) {
            output_func_1("cob_get_long_long", x);
        } else {
            output_func_1("cob_get_int", x);
        }
        break;
    case CB_TAG_INTRINSIC:
        output_func_name("cob_get_int", 1);
        output_param(x, -1);
        output(")");
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
}

static void
output_double(cb_tree x) {
    struct cb_binary_op *p;
    struct cb_cast      *cp;
    struct cb_field     *f;
    struct cb_literal   *l;
    char   buff[30];

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_CONST:
        if (x == cb_zero) {
            output("0");
        } else if (x == cb_null) {
            output("(unsigned char *)NULL");
        } else {
            output("%s", CB_CONST(x)->val);
        }
        break;
    case CB_TAG_INTEGER:
        output("%d", CB_INTEGER(x)->val);
        break;
    case CB_TAG_LITERAL:
        l = CB_LITERAL(x);
        /*
        {
            long long v = cb_get_long_long(x);
            printf("val %lld %20.20f  %20.20f ", v,(double)v, (double)v / (double)1000000.0);
            printf(" %20.20f ", cb_get_double (x));
            printf(" %20.20g ", cb_get_double (x));
            sprintf (buff, "%%%d.%df", l->size, l->scale);
            printf(buff, (double)cb_get_double (x));
            printf("\n");

        } 
        */
        sprintf(buff, "%%%d.%df", (int)(l->size), (int)(l->scale));
        output(buff, (double)cb_get_double(x));
        break;
    case CB_TAG_BINARY_OP:
        p = CB_BINARY_OP(x);
        if (p->op == '^') {
            output(" pow (");
            output_double(p->x);
            output(", ");
            output_double(p->y);
            output(")");
        } else {
            output("(");
            output_double(p->x);
            output(" %c ", p->op);
            output_double(p->y);
            output(")");
        }
        break;
    case CB_TAG_CAST:
        cp = CB_CAST(x);
        switch (cp->type) {
        case CB_CAST_ADDRESS:
            output("(");
            output_data(cp->val);
            output(")");
            break;
        case CB_CAST_PROGRAM_POINTER:
            output_func_1("cob_call_resolve_2", x);
            break;
        default:
            fprintf(stderr, "cobc:0: Unexpected cast type %d\n", cp->type);
            ABORT();
        }
        break;
    case CB_TAG_REFERENCE:
        f = cb_field(x);
#ifdef  COB_NON_ALIGNED
        if (!is_base_alligned(x)) {
            output_func_1("cob_get_double", x);
        } else
#endif
        {
            switch (f->usage) {
            case CB_USAGE_DOUBLE:
                output("(*(double*) (");
                output_data(x);
                output("))");
                return;
            case CB_USAGE_FLOAT:
                output("(*(float *) (");
                output_data(x);
                output("))");
                return;
            case CB_USAGE_COMP_5:
            case CB_USAGE_BINARY:
                if (cb_flag_fp_optimize && f->pic && f->pic->scale == 0 && f->pic->have_sign) {
                    output("((double) (");
                    output_integer(x);
                    output("))");
                    return;
                }
            default:
                output_func_1("cob_get_double", x);
                break;
            }
        }
        break;
    case CB_TAG_INTRINSIC:
        output_func_name("cob_get_double", 1);
        output_param(x, -1);
        output(")");
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
}

static void
output_index(cb_tree x, int national) {
    int mul = national ? 2 : 1;
    switch (CB_TREE_TAG(x)) {
    case CB_TAG_INTEGER:
        output("%d", (CB_INTEGER(x)->val - 1) * mul);
        break;
    case CB_TAG_LITERAL:
        output("%d", (cb_get_int(x) - 1) * mul);
        break;
    default:
        if (mul > 1) {
            output("( %d * (int)(", mul);
        } else {
            output("(int)(");
        }
        output_integer(x);
        output(" - 1)");
        if (mul > 1) {
            output(" )");
        }
        break;
    }
}

/*
 * Test
 */
static void
output_test(cb_tree x) {
    struct cb_reference *r;
    if (CB_REFERENCE_P(x)) {
        r = CB_REFERENCE(x);
        if (r->check) {
            output_stmt(r->check);
        }

    }
}

/*
 * Parameter
 */


void add_to_fieldcache(cb_tree x, struct cb_field *f) {
    struct field_list   *fl;

    if (!f->flag_field) {
        push_output_target(NULL);
        output_field(x);
        fl = cobc_malloc(sizeof(struct field_list));
        fl->x = x;
        fl->f = f;
        fl->curr_prog = excp_current_program_id;
        fl->next = field_cache;
        field_cache = fl;
        f->flag_field = 1;
        pop_output_target();
    }

}

static void
output_param_static(cb_tree x, int id) {
    output_param_1(x, id, 1, 0, 0);
}

static void
output_param_debug(cb_tree x, int id) {
    output_param_1(x, id, 0, 1, 1);
}

static void
output_param(cb_tree x, int id) {
    output_param_1(x, id, 0, 0, 0);
}
static int
has_local_strorage_ref(struct cb_field *field) {
    struct cb_field *f = field;
    if (f->children) {
        for (f = f->children; f; f = f->sister) {
            if (has_local_strorage_ref(f)) {
                return 1;
            }
        }
    }
    f = field;
    if (f->occurs_depending && cb_field(f->occurs_depending)->storage == CB_STORAGE_LOCAL) {
        return 1;
    }
    return 0;
}

static void
output_param_1(cb_tree x, int id, int staticfield, int protect_null, int no_local_storage) {
    struct cb_reference *r;
    struct cb_field     *f;
    struct cb_field     *pechk;
    struct cb_cast      *cp;
    struct cb_binary_op *bp;
    struct cb_intrinsic *ip;
    struct cb_object_list *ol;
    struct cb_alphabet_name *abp;
    struct cb_alphabet_name *rbp;
    cb_tree         l;
    int             n;
    int             extrefs;
    int             sav_stack_id;
    char            fname[12];
    char            buff[30];
    cb_tree save_check;

    param_id = id;

    if (x == NULL) {
        output("NULL");
        return;
    }

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_CONST:
        output("%s", CB_CONST(x)->val);
        break;
    case CB_TAG_INTEGER:
        output_integer(x);
        break;
    case CB_TAG_STRING:
        output_string(CB_STRING(x)->data, (int)CB_STRING(x)->size);
        break;
    case CB_TAG_LOCALE_NAME:
        output_param(CB_LOCALE_NAME(x)->list, id);
        break;
    case CB_TAG_OBJECT_LIST:
        ol = CB_OBJECT_LIST(x);
        switch (ol->type) {
        case CB_OBJECT_LIST_FIELD:
            output("((cob_field *)&%s%s)", CB_PREFIX_FIELDLIST, ol->cname);
            break;
        default:
            fprintf(stderr, "cobc:0: Invalid object list type %d\n", ol->type);
            ABORT();
            break;
        }
        break;
    case CB_TAG_ALPHABET_NAME:
        abp = CB_ALPHABET_NAME(x);
        switch (abp->type) {
        case CB_ALPHABET_STANDARD_1:
        case CB_ALPHABET_STANDARD_2:
            if (cb_flag_ebcdic_charset) {
                gen_ebcdic_ascii = 1;
                output("cob_ebcdic_ascii");
            } else {
                gen_native = 1;
                output("NULL");
            }
            break;
        case CB_ALPHABET_NATIVE:
            gen_native = 1;
            output("NULL");
            break;
        case CB_ALPHABET_EBCDIC:
            if (cb_flag_ebcdic_charset) {
                gen_native = 1;
                output("NULL");
            } else {
                gen_ebcdic = 1;
                output("cob_a2e");
            }
            break;
        case CB_ALPHABET_CUSTOM:
            gen_custom = 1;
            output("%s%s", CB_PREFIX_SEQUENCE, abp->cname);
            break;
        }
        break;
    case CB_TAG_CAST:
        cp = CB_CAST(x);
        switch (cp->type) {
        case CB_CAST_INTEGER:
            output_integer(cp->val);
            break;
        case CB_CAST_DOUBLE:
            output_double(cp->val);
            break;
        case CB_CAST_ADDRESS:
            output_data(cp->val);
            break;
        case CB_CAST_ADDR_OF_ADDR:
            output("&");
            output_data(cp->val);
            break;
        case CB_CAST_LENGTH:
            output_size_runtime(cp->val);
            break;
        case CB_CAST_PROGRAM_POINTER:
            output_param(cp->val, id);
            break;
        }
        break;
    case CB_TAG_DECIMAL:
        output_reg_name(x);
        break;
    case CB_TAG_FILE:
        output("%s%s", CB_PREFIX_FILE, CB_FILE(x)->cname);
        break;
    case CB_TAG_LITERAL:
        output("&%s%d", CB_PREFIX_CONST, lookup_literal(x));
        break;
    case CB_TAG_FIELD:
        /* TODO: remove me */
        output_param(cb_build_field_reference(CB_FIELD(x), NULL), id);
        break;
    case CB_TAG_REFERENCE:
        r = CB_REFERENCE(x);
        extrefs = 0;
        if (r->check) {
            if (cb_flag_gen_gcc) {
                output_indent(" ({");
            } else {
                inside_stack[inside_check] = 0;
                ++inside_check;
                output(" (\n");
            }
            for (l = r->check; l; l = CB_CHAIN(l)) {
                sav_stack_id = stack_id;
                output_stmt(CB_VALUE(l));
                stack_id = sav_stack_id;
                if (!cb_flag_gen_gcc) {
                    output(",\n");
                }
            }
        }

        if (CB_FILE_P(r->value)) {
            output("%s%s", CB_PREFIX_FILE, CB_FILE(r->value)->cname);
            if (r->check) {
                if (cb_flag_gen_gcc) {
                    output("; })");
                } else {
                    --inside_check;
                    output(" )");
                }
            }
            break;
        }
        if (CB_ALPHABET_NAME_P(r->value)) {
            rbp = CB_ALPHABET_NAME(r->value);
            switch (rbp->type) {
            case CB_ALPHABET_STANDARD_1:
            case CB_ALPHABET_STANDARD_2:
                if (cb_flag_ebcdic_charset) {
                    gen_ebcdic_ascii = 1;
                    output("&f_ebcdic_ascii");
                } else {
                    gen_native = 1;
                    output("&f_native");
                }
                break;
            case CB_ALPHABET_NATIVE:
                gen_native = 1;
                output("&f_native");
                break;
            case CB_ALPHABET_EBCDIC:
                if (cb_flag_ebcdic_charset) {
                    gen_native = 1;
                    output("&f_native");
                } else {
                    gen_full_ebcdic = 1;
                    output("&f_ebcdic");
                }
                break;
            case CB_ALPHABET_CUSTOM:
                gen_custom = 1;
                output("&f_%s", rbp->cname);
                break;
            }
            if (r->check) {
                if (cb_flag_gen_gcc) {
                    output("; })");
                } else {
                    --inside_check;
                    output(" )");
                }
            }
            break;
        }
        if (CB_OBJECT_LIST_P(r->value)) {
            output_param(r->value, id);
            break;
        }
        f = CB_FIELD(r->value);
        if (f->redefines && f->redefines->flag_external) {
            extrefs = 1;
            f->flag_item_external = 1;
            f->flag_external = 1;
        }
        if (f->redefines && f->redefines->flag_item_based) {
            f->flag_local = 1;
        }
        for (pechk = f->parent; pechk; pechk = pechk->parent) {
            if (pechk->flag_external) {
                extrefs = 1;
                f->flag_item_external = 1;
                break;
            }
            if (pechk->redefines && pechk->redefines->flag_external) {
                extrefs = 1;
                f->flag_item_external = 1;
                f->flag_external = 1;
                break;
            }
            if (pechk->flag_item_based) {
                f->flag_local = 1;
                break;
            }
            if (pechk->redefines && pechk->redefines->flag_item_based) {
                f->flag_local = 1;
                break;
            }
        }
        if (f->flag_external) {
            f->flag_item_external = 1;
        }
        if (!r->subs && !r->offset && f->count > 0
            && !cb_field_variable_size(f) &&
            !cb_field_variable_address(f)) {
            add_to_fieldcache(x, f);
            if (f->flag_local && !f->flag_dynamic_data) {
                if (f->flag_any_length && f->flag_anylen_done) {
                    output("&%s%d", CB_PREFIX_FIELD, f->id);
                } else {
                    output("(%s%d.data = ", CB_PREFIX_FIELD, f->id);
                    save_check = r->check;
                    r->check = NULL;
                    output_data_1(x, protect_null);
                    r->check = save_check;
                    output(", &%s%d)", CB_PREFIX_FIELD, f->id);
                    if (f->flag_any_length) {
                        f->flag_anylen_done = 1;
                    }
                }
            } else {
                if (screenptr && f->storage == CB_STORAGE_SCREEN) {
                    output("&s_%d", f->id);
                } else {
                    output("&%s%d", CB_PREFIX_FIELD, f->id);
                }
            }
        } else {
            if (f->usage == CB_USAGE_BIT && f->flag_occurs && r->subs) {
                if (stack_id >= num_cob_bitsarray_fields) {
                    num_cob_bitsarray_fields = stack_id + 1;
                }
                sprintf(fname, "fba%d", stack_id++);
                if (!cb_flag_gen_gcc) {
                    if (inside_check != 0) {
                        if (inside_stack[inside_check - 1] != 0) {
                            inside_stack[inside_check - 1] = 0;
                            output(",\n");
                        }
                    }
                }
                output("(%s.size = ", fname);
                if (no_local_storage && has_local_strorage_ref(f)) {
                    output("%d", f->size * f->occurs_max);
                } else {
                    output_size_runtime(x);
                }
                output(", %s.data = ", fname);
                save_check = r->check;
                r->check = NULL;
                output_data_1(x, protect_null);
                r->check = save_check;
                output(", %s.attr = ", fname);
                output_attr(x, 1);
                output(", %s.index = ", fname);
                output_index(CB_VALUE(r->subs), 0);
                output(", (cob_field*)&%s)", fname);
            } else {
                if (staticfield) {
                    struct cb_field *nf;
                    snprintf(buff, 30, "staticfield_%d", cb_field_id);
                    nf = (struct cb_field *)cb_build_field(cb_build_reference(buff));
                    sprintf(fname, "%s%d", CB_PREFIX_FIELD, nf->id);
                    add_to_fieldcache(NULL, nf);
                } else {
                    if (stack_id >= num_cob_fields) {
                        num_cob_fields = stack_id + 1;
                    }

                    sprintf(fname, "f%d", stack_id++);
                }
                if (!cb_flag_gen_gcc) {
                    if (inside_check != 0) {
                        if (inside_stack[inside_check - 1] != 0) {
                            inside_stack[inside_check - 1] = 0;
                            output(",\n");
                        }
                    }
                }
                output("(%s.size = ", fname);
                if (no_local_storage && has_local_strorage_ref(f)) {
                    output("%d", f->size * f->occurs_max);
                } else {
                    output_size_runtime(x);
                }
                output(", %s.data = ", fname);
                save_check = r->check;
                r->check = NULL;
                output_data_1(x, protect_null);
                r->check = save_check;
                output(", %s.attr = ", fname);
                output_attr(x, 0);
                output(", &%s)", fname);
            }
        }

        if (r->check) {
            if (cb_flag_gen_gcc) {
                output("; })");
            } else {
                --inside_check;
                output(" )");
            }
        }
        break;
    case CB_TAG_BINARY_OP:
        bp = CB_BINARY_OP(x);
        output("cob_intr_binop (rtd, ");
        output_param(bp->x, id);
        output(", ");
        output("%d", bp->op);
        output(", ");
        output_param(bp->y, id);
        output(")");
        break;
    case CB_TAG_INTRINSIC:
        n = 0;
        ip = CB_INTRINSIC(x);
        output("%s (rtd", ip->intr_tab->intr_routine);
        if (ip->intr_tab->refmod || ip->intr_field || ip->args) {
            output(", ");
        }
        if (ip->intr_tab->refmod) {
            if (ip->offset) {
                output_integer(ip->offset);
                output(", ");
            } else {
                output("0, ");
            }
            if (ip->length) {
                output_integer(ip->length);
            } else {
                output("0");
            }
            if (ip->intr_field || ip->args) {
                output(", ");
            }
        }
        if (ip->intr_field) {
            if (ip->intr_field == cb_int0) {
                output("NULL");
            } else if (ip->intr_field == cb_int1) {
                for (l = ip->args; l; l = CB_CHAIN(l)) {
                    n++;
                }
                output("%d", n);
            } else {
                output_param(ip->intr_field, id);
            }
            if (ip->args) {
                output(", ");
            }
        }
        for (l = ip->args; l; l = CB_CHAIN(l)) {
            output_param(CB_VALUE(l), id);
            id++;
            param_id++;
            if (CB_CHAIN(l)) {
                output(", ");
            }
        }
        output(")");
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
}

/*
 * Function call
 */

static void
output_func_name(const char *name, int argc) {
    if (name[0] == '-') {
        name++;
        output("%s (", name);
    } else {
        output("%s (rtd", name);
        if (argc) {
            output(", ");
        }
    }
}

static void reset_nbr_parameter() {
    index_optimizer *current;
    for (current = index_optimizer_cache; current; current = current->next) {
        current->nbr_in_one_funcall = 0;
    }
}

static void
output_base_C_type(cb_tree x) {
    struct cb_field     *f;
    struct cb_cast      *cp;

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_REFERENCE:
        f = cb_field(x);
        switch (f->usage) {
        case CB_USAGE_BINARY:
        case CB_USAGE_COMP_5:
        case CB_USAGE_COMP_X:
        case CB_USAGE_INDEX:
        case CB_USAGE_LENGTH:
            if (!f->pic->have_sign) {
                output("unsigned ");
            } else {
                output("signed ");
            }
            switch (f->size) {
            case 0:
            case 1:
                output("char "); return;
            case 2:
                output("short "); return;
            case 3:
            case 4:
                output("int "); return;
            default:
                output("long long "); return;
            }
        case CB_USAGE_DOUBLE:
            output("double "); return;
        case CB_USAGE_FLOAT:
            output("double "); return;
        case CB_USAGE_POINTER:
            output("void* "); return;
        default:
            fprintf(stderr, "cobc:0: output_base_C_type unexpected field type  %d\n", f->usage);
            ABORT();
        }
    case CB_TAG_CAST:
        cp = CB_CAST(x);
        switch (cp->type) {
        case CB_CAST_INTEGER:
        case CB_CAST_DOUBLE:
        case CB_CAST_ADDRESS:
            output_base_C_type(cp->val); return;
        default:
            fprintf(stderr, "cobc:0: output_base_C_type unexpected cast %d\n", cp->type);
            ABORT();
        }
        break;
        break;
    default:
        fprintf(stderr, "cobc:0: output_base_C_type unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
}

static void
output_funcall(cb_tree x) {
    struct cb_funcall   *p;
    cb_tree             l;
    int                 i;

    p = CB_FUNCALL(x);
    if (p->not_on_exception) {
        output("if (!rtd->cob_exception_code) ");
    }
    if (p->name[0] == '$') {
        switch (p->name[1]) {
        case 'A':
            /* Set of one register */
#ifdef  COB_NON_ALIGNED
            if (!is_base_alligned(p->argv[1])) {
                output("memcpy(");
                output("&");
                output_param(p->argv[0], 1);
                output(", ");
                output_data(p->argv[1]);
                output(", sizeof(");
                output_param(p->argv[0], 1);
                output("))");
            } else
#endif
            {
                output_param(p->argv[0], 1);
                output(" = *(");
                output_data(p->argv[1]);
                output(")");
            }
            break;
        case 'C':
            /* memcopy FLOAT reg into  field */
#ifdef  COB_NON_ALIGNED
            if (!is_base_alligned(p->argv[0])) {
                output("{");
                output_base_C_type(p->argv[0]);
                output(" rm =");
                output_param(p->argv[1], 1);
                output(";  memcpy(");
                output_data(p->argv[0]);
                output(", &rm, sizeof(rm));}");
            } else
#endif
            {
                /* Set of one character */
                output_param(p->argv[0], 1);
                output(" = ");
                output_param(p->argv[1], 1);
            }
            break;
        case 'E':
            /* Set of one character */
            output("*(");
            output_data(p->argv[0]);
            output(") = ");
            output_param(p->argv[1], 1);
            break;
        case 'F':
            /* Move of one character */
            output("*(");
            output_data(p->argv[0]);
            output(") = *(");
            output_data(p->argv[1]);
            output(")");
            break;
        case 'G':
            /* Test of one character */
            output("(int)(*(");
            output_data(p->argv[0]);
            if (p->argv[1] == cb_space) {
                output(") - %d)", (int)CHAR_SP);
            } else if (p->argv[1] == cb_zero) {
                output(") - %d)", (int)CHAR_0);
            } else if (p->argv[1] == cb_low) {
                output("))");
            } else if (p->argv[1] == cb_high) {
                output(") - 255)");
            } else if (CB_LITERAL_P(p->argv[1])) {
                output(") - %d)", *(CB_LITERAL_ENCODED(p->argv[1])->data));
            } else {
                output(") - *(");
                output_data(p->argv[1]);
                output("))");
            }
            break;
        case 'M':
            /* memset of field */
            if (cb_field_size(p->argv[0]) == 1) {
                /* Set of one character */
                output("*(");
                output_data(p->argv[0]);
                output(") = ");
                output_param(p->argv[1], 1);
            } else {
                output("memset(");
                output_data(p->argv[0]);
                output(", ");
                output_param(p->argv[1], 1);
                output(", ");
                output_size_runtime(p->argv[0]);
                output(")");
            }
            break;
        case 'P':
            /* Set Linkage data pointer  */
            output_data(p->argv[0]);
            output(" = (");
            output_param(p->argv[0], 1);
            output(")->data");
            break;


        default:
            ABORT();
        }
        reset_nbr_parameter();
        return;
    }
    screenptr = p->screenptr;
    output_func_name(p->name, p->argc);
    reset_nbr_parameter();
    for (i = 0; i < p->argc; i++) {
        if (p->varcnt && i + 1 == p->argc) {
            output("%d, ", p->varcnt);
            for (l = p->argv[i]; l; l = CB_CHAIN(l)) {

                output_param(CB_VALUE(l), i);
                i++;
                if (CB_CHAIN(l)) {
                    output(", ");
                }
            }
        } else {
            output_param(p->argv[i], i);
            if (i + 1 < p->argc) {
                output(", ");
            }
        }
    }
    /*end of function,reset*/
    reset_nbr_parameter();
    output(")");
    screenptr = 0;
}

static void
output_func_1(const char *name, cb_tree x) {
    output_func_name(name, 1);
    output_param(x, param_id);
    output(")");
}

/*
 * Condition
 */
/*
static int 
same_oper (cb_tree x, int op) {
    struct cb_binary_op *p;
    int ret = 0;
    if ( CB_TREE_TAG (x) == CB_TAG_BINARY_OP) {
        p = CB_BINARY_OP (x);
        ret = ( p->op == op );
    }
    return ret;
}
*/
static void
output_cond(cb_tree x, int save_flag, int prev_op) {
    struct cb_binary_op *p;

    switch (CB_TREE_TAG(x)) {
    case CB_TAG_CONST:
        if (x == cb_true) {
            output("1");
        } else if (x == cb_false) {
            output("0");
        } else {
            ABORT();
        }
        break;
    case CB_TAG_REFERENCE:
        output_integer(x);
        break;
    case CB_TAG_BINARY_OP:
        p = CB_BINARY_OP(x);
        switch (p->op) {
        case '!':
            output("!");
            output_cond(p->x, save_flag, p->op);
            break;
        case 'n':
            if (prev_op !=  p->op) {
                output("(");
            }
            output("~(");
            output_cond(p->x, save_flag, p->op);
            if (p->y) {
                output(" & ");
                output_integer(p->y);
            }
            output(")");
            if (prev_op !=  p->op) {
                output(")");
            }
            break;

        case '&':
        case '|':
        case 'a':
        case 'o':
        case 'y':
            if (prev_op !=  p->op) {
                output("(");
            }
            output_cond(p->x, save_flag, p->op);
            /*output (p->op == '&' ? " && " : " || ");*/
            output(map_bin_oper(p->op));
            output_cond(p->y, save_flag, p->op);
            if (prev_op !=  p->op) {
                output(")");
            }
            break;

        case '=':
        case '<':
        case '[':
        case '>':
        case ']':
        case '~':
            if (p->rawcondition) {
                output("(");
                output_param(p->x, 1);
                switch (p->op) {
                case '=':
                    output(" == ");
                    break;
                case '<':
                    output(" < ");
                    break;
                case '[':
                    output(" <= ");
                    break;
                case '>':
                    output(" > ");
                    break;
                case ']':
                    output(" >= ");
                    break;
                case '~':
                    output(" != ");
                    break;
                }
                output_param(p->y, 1);
                output(")");
            } else {
                output("((int)");
                output_cond(p->x, save_flag, p->op);
                switch (p->op) {
                case '=':
                    output(" == 0");
                    break;
                case '<':
                    output(" <  0");
                    break;
                case '[':
                    output(" <= 0");
                    break;
                case '>':
                    output(" >  0");
                    break;
                case ']':
                    output(" >= 0");
                    break;
                case '~':
                    output(" != 0");
                    break;
                }
                output(")");
            }
            break;

        default:
            output_integer(x);
            break;
        }
        break;
    case CB_TAG_FUNCALL:
        if (!output_register_only && set_up_decimal_list(x)) {
            break;
        }
        if (save_flag) {
            output("(ret = ");
        }
        output_funcall(x);
        if (save_flag) {
            output(")");
        }
        break;
    case CB_TAG_LIST:
        if (save_flag) {
            output("(ret = ");
        }
        if (cb_flag_gen_gcc) {
            output_indent("({");
        } else {
            inside_stack[inside_check] = 0;
            ++inside_check;
            output("(\n");
        }
        for (; x; x = CB_CHAIN(x)) {
            cb_tree v = CB_VALUE(x);
            output_stmt(v);
            update_wipe_flag_and_target(x, v);
        }
        if (cb_flag_gen_gcc) {

            output_indent("})");
        } else {
            --inside_check;
            output(")");
        }
        if (save_flag) {
            output(")");
        }
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
}

/*
 * MOVE
 */

static void
output_move(cb_tree src, cb_tree dst) {
    /* suppress warnings */
    suppress_warn = 1;
    output_stmt(cb_build_move(src, dst));
    suppress_warn = 0;
}

/*
 * INITIALIZE
 */

static int
initialize_type(struct cb_initialize *p, struct cb_field *f, int topfield) {
    cb_tree     l;
    int         type;

    if (f->flag_item_78) {
        fprintf(stderr, "Unexpected CONSTANT item\n");
        ABORT();
    }

    if (f->flag_chained) {
        return (INITIALIZE_ONE);
    }

    /*if ( f->flag_external ) {
        return(INITIALIZE_EXTERNAL);
    }*/

    if (f->redefines && (!topfield || !p->flag_statement)) {
        return (INITIALIZE_NONE);
    }

    if (p->val && f->values) {
        return (INITIALIZE_ONE_VALUE);
    }


    if (p->flag_statement && !f->children && !p->flag_fillFiller) {
        if ((strncmp(CB_PREFIX_FILLER, f->name, CB_PREFIX_FILLER_LEN) == 0) ||
            (strlen(f->name) > 4 && f->name[4] == '$')) {
            return (INITIALIZE_NONE);
        }
    }

    if (CB_REFERENCE_P(p->var) && CB_REFERENCE(p->var)->offset && f->children) {
        return (INITIALIZE_UNIFORM);
    }
    if (f->children) {
        type = initialize_type(p, f->children, 0);
        if (type == INITIALIZE_ONE || type == INITIALIZE_ONE_VALUE) {
            return (INITIALIZE_COMPOUND);
        }
        for (f = f->children->sister; f; f = f->sister) {
            if (type != initialize_type(p, f, 0)) {
                return (INITIALIZE_COMPOUND);
            }
        }
        return (type);
    } else if (f->bitfield_children) {
        type = initialize_type(p, f->bitfield_children, 0);
        if (type == INITIALIZE_ONE_VALUE) {
            return (INITIALIZE_COMPOUND);
        }
        for (f = f->bitfield_children->sister; f; f = f->sister) {
            if (type != initialize_type(p, f, 0)) {
                return (INITIALIZE_COMPOUND);
            }
        }
        return (type);
    } else {
        for (l = p->rep; l; l = CB_CHAIN(l)) {
            if ((int)CB_PURPOSE_INT(l) == (int)CB_TREE_CATEGORY(f)) {
                return (INITIALIZE_ONE);
            }
        }
    }
    if (p->def) {
        if (!topfield && !cb_flag_initialize_pointer) {
            if (CB_TREE_CLASS(f) == CB_CLASS_POINTER || f->usage == CB_USAGE_INDEX) {
                return (INITIALIZE_NONE);
            }
        }
        if (!cb_flag_fp_optimize) {
            if (f->usage == CB_USAGE_FLOAT || f->usage == CB_USAGE_DOUBLE) {
                return (INITIALIZE_ONE);
            }
        }

        switch (cb_tree_type(CB_TREE(f))) {
        case COB_TYPE_BITS       :
        case COB_TYPE_BITS_ARRAY :
        case COB_TYPE_NUMERIC_BITS:
            return (INITIALIZE_ONE);
        default:
            break;
        }
        switch (CB_TREE_CATEGORY(f)) {
        case CB_CATEGORY_ALPHANUMERIC_EDITED:
        case CB_CATEGORY_NATIONAL:
        case CB_CATEGORY_NATIONAL_EDITED:
            return (INITIALIZE_ONE);
        case CB_CATEGORY_NUMERIC_EDITED:
            if (!(cb_flag_displaynumeric_mf50 && cb_flag_displaynumericedited_mf50) ||
                p->flag_statement) {
                return (INITIALIZE_ONE);
            }
            /* fail over intentional */
        case CB_CATEGORY_NUMERIC:
            switch (cb_tree_type(CB_TREE(f))) {
            case COB_TYPE_NUMERIC_PACKED:
            case COB_TYPE_BITS       :
            case COB_TYPE_BITS_ARRAY :
            case COB_TYPE_NUMERIC_BITS:
                return (INITIALIZE_ONE);
            default:
                break;
            }
            if (f->pic && f->pic->have_sign && f->flag_sign_separate) {
                return (INITIALIZE_ONE);
            } else if (cb_flag_sign_ebcdic && f->pic && f->pic->have_sign && (cb_tree_type(CB_TREE(f)) == COB_TYPE_NUMERIC_DISPLAY)) {
                return (INITIALIZE_ONE);
            } else {
                return (INITIALIZE_DEFAULT);
            }

        default:
            return (INITIALIZE_DEFAULT);
        }
    }

    return (INITIALIZE_NONE);
}

static int
initialize_uniform_char(struct cb_initialize *p, struct cb_field *f) {
    int c;
    struct cb_field *thechildren = f->children ? f->children : f->bitfield_children;

    if (thechildren) {
        c = initialize_uniform_char(p, thechildren);
        for (f = thechildren->sister; f; f = f->sister) {
            if (!f->redefines) {
                if (c != initialize_uniform_char(p, f)) {
                    return (-1);
                }
            }
        }
        return (c);
    } else {
        if (p->flag_filldefaultbyte && cb_use_defaultbyte) {
            return (cb_defaultbyte);
        } else {
            switch (cb_tree_type(CB_TREE(f))) {
            case COB_TYPE_NUMERIC_BINARY:
                return (0);
            case COB_TYPE_NUMERIC_FLOAT:
            case COB_TYPE_NUMERIC_DOUBLE:
                if (cb_flag_fp_optimize) {
                    return (0);
                } else {
                    return -1;
                }
            case COB_TYPE_NUMERIC_BITS:
            case COB_TYPE_BITS:
            case COB_TYPE_BITS_ARRAY:
                return (-1);
            case COB_TYPE_NUMERIC_EDITED:
                if (f->usage != CB_USAGE_DISPLAY) {
                    return -1;
                }
                if (!(cb_flag_displaynumeric_mf50 && cb_flag_displaynumericedited_mf50)) {
                    return -1;
                }
                /* intetional pass over case !!!*/
            case COB_TYPE_NUMERIC_DISPLAY:
                if (cb_flag_displaynumeric_mf50 &&  p->flag_filldefaultbyte && !f->pic->have_sign) {
                    return (CHAR_SP);
                } else {
                    if ((f->pic && f->pic->have_sign) && (f->flag_sign_separate || cb_flag_sign_ebcdic)) {
                        return (-1);
                    }
                }
                return (CHAR_0);
            case COB_TYPE_ALPHANUMERIC_BITS:
                return (CHAR_0);
            case COB_TYPE_ALPHANUMERIC:
                return (CHAR_SP);
            default:
                return (-1);
            }
        }
    }
    return (-1);
}

static int
need_runtime_size(cb_tree x) {
    return (CB_REFERENCE_P(x) && (CB_REFERENCE(x)->length || CB_REFERENCE(x)->offset || CB_REFERENCE(x)->all));
}

static void
output_initialize_figurative(cb_tree x, struct cb_field *f, const int value) {
    output_prefix();
    if (f->flag_occurs  && CB_REFERENCE(x)->cb_all_occurs) {
        CB_REFERENCE(x)->subs =
            cb_cons(cb_int1, CB_REFERENCE(x)->subs);
    }
    if (f->size == 1 && (!CB_REFERENCE(x)->cb_all_occurs || f->occurs_max <= 1)) {
        output("*(unsigned char *)(");
        output_data(x);
        output(") = %d;\n", value);
    } else {
        output("memset (");
        output_data(x);
        if (/*CB_REFERENCE_P(x) && CB_REFERENCE(x)->length */need_runtime_size(x)) {
            output(", %d, ", value);
            output_size_runtime(x);
            output(");\n");
        } else if (f->flag_occurs  && CB_REFERENCE(x)->cb_all_occurs) {
            output(", %d, %d);\n", value, f->size * f->occurs_max);
        } else {
            output(", %d, %d);\n", value, f->size);
        }
    }
    if (f->flag_occurs && CB_REFERENCE(x)->cb_all_occurs) {
        /* Close loop */
        CB_REFERENCE(x)->subs = CB_CHAIN(CB_REFERENCE(x)->subs);
    }
}

static void
output_initialize_last_byte(cb_tree x, struct cb_field *f, const int value) {
    output_prefix();
    output("*(unsigned char *)(");
    output_data(x);
    output("+ %d -1) = %d;\n", f->size,  value);
}

static void
output_initialize_begin_occurs_loop(cb_tree x, struct cb_field *f) {
    int i0 = -1;
    if (f->flag_occurs  && CB_REFERENCE(x)->cb_all_occurs) {
        /* Begin occurs loop */
        i0 = f->indexes;
        i_counters[i0] = 1;
        output_line("for (i%d = 1; i%d <= %d; i%d++)",
                    i0, i0, f->occurs_max, i0);
        output_indent("{");
        CB_REFERENCE(x)->subs =
            cb_cons(cb_i[i0], CB_REFERENCE(x)->subs);
    }
}

static void
output_initialize_end_occurs_loop(cb_tree x, struct cb_field *f) {
    if (f->flag_occurs && CB_REFERENCE(x)->cb_all_occurs) {
        /* Close loop */
        CB_REFERENCE(x)->subs = CB_CHAIN(CB_REFERENCE(x)->subs);
        output_indent("}");
    }
}

static void
output_initialize_literal(cb_tree x, struct cb_field *f, struct cb_literal *l) {
    size_t  i;
    size_t  n;

    if (l->size == 1) {
        output_prefix();
        output("memset (");
        output_data(x);
        if (/*CB_REFERENCE_P(x) && CB_REFERENCE(x)->length */need_runtime_size(x)) {
            output(", %d, ", l->data[0]);
            output_size_runtime(x);
            output(");\n");
        } else {
            output(", %d, %d);\n", l->data[0], f->size);
        }
        return;
    }
    if (l->size >= f->size) {
        output_prefix();
        output("memcpy (");
        output_data(x);
        output(", ");
        output_string(l->data, f->size);
        output(", %d);\n", f->size);
        return;
    }
    i = f->size / l->size;
    i_counters[0] = 1;
    output_line("for (i0 = 0; i0 < %u; i0++)", (unsigned int)i);
    output_indent("{");
    output_prefix();
    output("memcpy (");
    output_data(x);
    output(" + (i0 * %u), ", (unsigned int)l->size);
    output_string(l->data, l->size);
    output(", %u);\n", (unsigned int)l->size);
    output_indent("}");
    n = f->size % l->size;
    if (n) {
        output_prefix();
        output("memcpy (");
        output_data(x);
        output(" + (i0 * %u), ", (unsigned int)l->size);
        output_string(l->data, n);
        output(", %u);\n", (unsigned int)n);
    }
}

static void
output_initialize_fp(cb_tree x, struct cb_field *f) {
    output_prefix();
    if (cb_flag_fp_optimize) {
        output("memset (");
        output_data(x);
        output(", 0, ");
        if (f->usage == CB_USAGE_FLOAT) {
            output("sizeof(float);\n");
        } else {
            output("sizeof(double);\n");
        }
    } else {
        if (f->usage == CB_USAGE_FLOAT) {
            output("{float temp = 0.0;");
        } else {
            output("{double temp = 0.0;");
        }
        output(" memcpy (");
        output_data(x);
        output(", (char *)&temp, sizeof(temp));}\n");
    }
}

static void
output_initialize_external(cb_tree x, struct cb_field *f) {
    unsigned char   *p;
    char            name[COB_MINI_BUFF];

    if (f->ename) {
        output_prefix();
        output_data(x);
        output(" = cob_external_addr_wstatus (rtd, \"%s\", %d, &(%s%d_init));\n", f->ename, f->size * f->occurs_max, CB_PREFIX_BASE, f->id);
    } else {
        if (f->file && (f->file->record != f)) {
            /* for file , EXTERNAL is always the dummy global record*/
            output_initialize_external(x, f->file->record);
        } else {
            output_prefix();
            output_data(x);
            strcpy(name, f->name);
            for (p = (unsigned char *)name; *p; p++) {
                if (islower(*p)) {
                    *p = toupper(*p);
                }
            }
            output(" = cob_external_addr_wstatus (rtd, \"%s\", %d, &(%s%d_init));\n", name, f->size * f->occurs_max, CB_PREFIX_BASE, f->id);
        }
    }
}

static void
output_initialize_uniform(cb_tree x, int c, int size) {
    output_prefix();
    if (size == 1) {
        output("*(unsigned char *)(");
        output_data(x);
        output(") = %d;\n", c);
    } else {
        output("memset (");
        output_data(x);
        if (need_runtime_size(x)) {
            output(", %d, ", c);
            output_size_runtime(x);
            output(");\n");
        } else {
            output(", %d, %d);\n", c, size);
        }
    }
}

static void
output_initialize_one(struct cb_initialize *p, cb_tree x, int init_default_byte_value) {
    struct cb_field     *f;
    cb_tree             value;
    cb_tree             lrp;
    struct cb_literal   *l;
    int                 i;
    int                 n;
    int                 buffchar;

    static char     *buff = NULL;
    static int      lastsize = 0;

    f = cb_field(x);

    /* CHAINING */
    if (f->flag_chained) {
        output_prefix();
        output("cob_chain_setup (rtd, ");
        output_data(x);
        output(", %d, %d);\n", f->param_num, f->size);
        return;
    }
    /* Initialize by value */
    if (p->val && f->values) {
        value = CB_VALUE(f->values);
        if (value == cb_low) {
            if (init_default_byte_value || (cb_defaultbyte != 0)) {
                output_initialize_figurative(x, f, 0);
            }
        } else if (value == cb_high) {
            if (init_default_byte_value || (cb_defaultbyte != 255)) {
                output_initialize_figurative(x, f, 255);
            }
        } else if (CB_TREE_CATEGORY(x) == CB_CATEGORY_NATIONAL) {
            output_initialize_begin_occurs_loop(x, f);
            output_move(value, x);
            output_initialize_end_occurs_loop(x, f);
        } else if (value == cb_space) {
            /* Fixme: This is to avoid an error when a
               numeric-edited item has VALUE SPACE because
               cob_build_move doubly checks the value.
               We should instead check the value only once.  */
            if (init_default_byte_value || (cb_defaultbyte != CHAR_SP)) {
                output_initialize_figurative(x, f, CHAR_SP);
            }
        } else if (value == cb_dirsep) {
            if (init_default_byte_value || (cb_defaultbyte != *(CB_LITERAL(cb_dirsep)->data))) {
                output_initialize_figurative(x, f, *(CB_LITERAL(cb_dirsep)->data)); /* '/'); */
            }
        } else if (value == cb_quote) {
            if (init_default_byte_value || (cb_defaultbyte != '"')) {
                output_initialize_figurative(x, f, A2E(cb_quote_char)); /* '"'); */
            }
        } else if (value == cb_zero && f->usage == CB_USAGE_DISPLAY &&
                   ((!cb_flag_sign_ebcdic && !(f->pic && f->pic->have_sign && f->flag_sign_separate)) ||
                    (cb_flag_sign_ebcdic && !(f->pic && f->pic->have_sign)))) {
            if (init_default_byte_value || (cb_defaultbyte != CHAR_0)) {
                output_initialize_figurative(x, f, CHAR_0);
            }
        } else if (value == cb_null && f->usage == CB_USAGE_DISPLAY &&
                   !(f->pic && f->pic->have_sign) && !(f->flag_sign_separate)) {
            if (init_default_byte_value || (cb_defaultbyte != 0)) {
                output_initialize_figurative(x, f, 0);
            }
        } else {
            output_initialize_begin_occurs_loop(x, f);
            if (CB_LITERAL_P(value) && CB_TREE_CATEGORY(value) == CB_CATEGORY_NATIONAL) {
                output_move(value, x);
            } else if (CB_LITERAL_P(value) && CB_LITERAL(value)->all) {

                /* ALL literal */
                output_initialize_literal(x, f, CB_LITERAL_ENCODED(value));
            } else if (CB_CONST_P(value)
                       || CB_TREE_CLASS(value) == CB_CLASS_NUMERIC) {
                /* Figurative literal, numeric literal */
                if (init_default_byte_value || cb_defaultbyte != 0) {
                    output_move(value, x);
                } else {
                    if (CB_LITERAL_P(value) && cb_get_int(value) == 0) {
                        /*do nothing */

                    } else {
                        output_move(value, x);
                    }
                }

            } else {
                /* Alphanumeric literal */
                /* We do not use output_move here because
                   we do not want to have the value be edited. */
                l = CB_LITERAL_ENCODED(value);
                if (!buff) {
                    if (f->size <= COB_SMALL_BUFF) {
                        buff = cobc_malloc(COB_SMALL_BUFF);
                        lastsize = COB_SMALL_BUFF;
                    } else {
                        buff = cobc_malloc((size_t)f->size);
                        lastsize = f->size;
                    }
                } else {
                    if (f->size > lastsize) {
                        free(buff);
                        buff = cobc_malloc((size_t)f->size);
                        lastsize = f->size;
                    }
                }
                if ((int)l->size >= (int)f->size) {
                    memcpy(buff, l->data, (size_t)f->size);
                } else {
                    memcpy(buff, l->data, l->size);
                    memset(buff + l->size, CHAR_SP, f->size - l->size);
                }
                output_prefix();
                buffchar = *buff;
                if (f->size == 1) {
                    if (init_default_byte_value || (cb_defaultbyte != buffchar)) {
                        output("*(unsigned char *) (");
                        output_data(x);
                        output(") = %d;\n", *(unsigned char *)buff);
                    }
                } else {
                    for (i = 0; i < f->size; i++) {
                        if (*(buff + i) != buffchar) {
                            break;
                        }
                    }
                    if (i == f->size) {
                        if (init_default_byte_value || (cb_defaultbyte != buffchar)) {
                            output("memset (");
                            output_data(x);
                            output(", %d, %d);\n", buffchar, f->size);
                        }
                    } else {
                        if (f->size >= 8) {
                            buffchar = *(buff + f->size - 1);
                            n = 0;
                            for (i = f->size - 1; i >= 0; i--, n++) {
                                if (*(buff + i) != buffchar) {
                                    break;
                                }
                            }
                            if (n > 2) {
                                output("memcpy (");
                                output_data(x);
                                output(", ");
                                output_string((ucharptr)buff,
                                              f->size - n);
                                output(", %d);\n", f->size - n);
                                output_prefix();
                                output("memset (");
                                output_data(x);
                                output(" + %d, %d, %d);\n",
                                       f->size - n, buffchar, n);
                                output_initialize_end_occurs_loop(x, f);
                                return;
                            }
                        }
                        output("memcpy (");
                        output_data(x);
                        output(", ");
                        output_string((ucharptr)buff, f->size);
                        output(", %d);\n", f->size);
                    }
                }
            }
            output_initialize_end_occurs_loop(x, f);
        }
        return;
    }

    output_initialize_begin_occurs_loop(x, f);
    /* Initialize replacing */
    if (!f->children) {
        for (lrp = p->rep; lrp; lrp = CB_CHAIN(lrp)) {
            if ((int)CB_PURPOSE_INT(lrp) == (int)CB_TREE_CATEGORY(x)) {
                output_move(CB_VALUE(lrp), x);
                output_initialize_end_occurs_loop(x, f);
                return;
            }
        }
    }

    /* Initialize by default */
    if (p->def) {
        if (f->bitfield_children) {
            if (init_default_byte_value || (cb_defaultbyte != 0)) {
                output_initialize_uniform(x, 0, f->size * f->occurs_max);
            }
        } else if (p->flag_filldefaultbyte && cb_use_defaultbyte) {
            if (init_default_byte_value) {
                output_initialize_uniform(x, cb_defaultbyte, f->size);
            }
        } else {
            if (f->usage == CB_USAGE_FLOAT || f->usage == CB_USAGE_DOUBLE) {
                output_initialize_fp(x, f);
                output_initialize_end_occurs_loop(x, f);
                return;
            }
            switch (CB_TREE_CATEGORY(x)) {
            case CB_CATEGORY_DATA_POINTER:
            case CB_CATEGORY_PROGRAM_POINTER:
                output_move(cb_null, x);
                break;
            case CB_CATEGORY_NUMERIC:
            case CB_CATEGORY_NUMERIC_EDITED:
                if (p->flag_filldefaultbyte &&
                    cb_flag_displaynumeric_mf50 &&
                    f->usage == CB_USAGE_DISPLAY &&
                    !f->pic->have_sign) {
                    if (init_default_byte_value || (cb_defaultbyte != CHAR_SP)) {
                        output_move(cb_space, x);
                    }
                } else {
                    if (init_default_byte_value || (cb_defaultbyte != 0)) {
                        output_move(cb_zero, x);
                    } else if (f->usage == CB_USAGE_PACKED) {
                        if (f->occurs_max == 1) {
                            if (f->pic->have_sign) {
                                output_initialize_last_byte(x, f, 0x0C);
                            } else {
                                output_initialize_last_byte(x, f, 0x0F);
                            }
                        } else {
                            output_move(cb_zero, x);
                        }
                    }
                }
                break;
            case CB_CATEGORY_ALPHANUMERIC_EDITED:
            case CB_CATEGORY_NATIONAL:
            case CB_CATEGORY_NATIONAL_EDITED:
                if (init_default_byte_value || (cb_defaultbyte != CHAR_SP)) {
                    output_move(cb_space, x);
                }
                break;
            default:
                fprintf(stderr, "cobc:0: Unexpected tree category %d\n", CB_TREE_CATEGORY(x));
                ABORT();
            }
        }
    }

    output_initialize_end_occurs_loop(x, f);
}

static int
count_numeric_binary_field(struct cb_field *f) {
    int res = 0;
    //printf("fld %s .", f->name);
    if (f->children) {
        for (f = f->children; f; f = f->sister) {
            res += count_numeric_binary_field(f);
        }
    } else {
        switch (f->usage) {
        case CB_USAGE_INDEX:
        case CB_USAGE_LENGTH:
        case CB_USAGE_BINARY:
        case CB_USAGE_COMP_5:
        case CB_USAGE_COMP_X:
        case CB_USAGE_COMP_6:
            res += f->size * f->occurs_max;
            break;
        default:
            /*do nothing*/
            break;
        }
    }

    return res;
}
static int
exist_filler_pointer(struct cb_field *f) {
    int res = 0;
    struct cb_field *thechildren = f->children ? f->children : f->bitfield_children;
    if (thechildren) {
        for (f = thechildren; f && !res; f = f->sister) {
            res = exist_filler_pointer(f);
        }
    } else {
        if (strncmp(CB_PREFIX_FILLER, f->name, CB_PREFIX_FILLER_LEN) == 0 || f->usage == CB_USAGE_POINTER) {
            res = 1;
        }
    }
    return res;
}
static void
output_initialize_compound(struct cb_initialize *p, cb_tree x, int init_default_byte_value) {
    struct  cb_field *ff, *thechildren;
    struct  cb_field *f;
    struct  cb_field *last_field;
    cb_tree c;
    int     type;
    int     last_char;
    int     i;
    long long ll;
    size_t  size;

    ff = cb_field(x);
    output_initialize_begin_occurs_loop(x, ff);
    thechildren = ff->children ? ff->children : ff->bitfield_children;

    /*if ( cb_flag_binary_optimize && !(ff->parent) ) { */
    if (!exist_filler_pointer(ff)) {
        if (cb_flag_binary_optimize && init_default_byte_value && cb_defaultbyte == 0) {
            /*Level 1 field*/
            i =  count_numeric_binary_field(ff);
            if (i > 0) {
                ll = i;
                ll = ((long long)ff->size * (long long)100) / ll;
            }
            if (i > 30) {
                c = cb_build_field_reference(ff, x);
                output_initialize_uniform(c, cb_defaultbyte, ff->size);
                init_default_byte_value = 0;
                //printf("%s has %d num field \n", ff->name, i);
            } else {
                init_default_byte_value = 1;
            }
        }

    }
    for (f = thechildren; f; f = f->sister) {
        stack_id = 0;
        type = initialize_type(p, f, 0);
        c = cb_build_field_reference(f, x);
        CB_REFERENCE(c)->cb_all_occurs = 1 /* p->flag_care_varing ? 0 : 1*/;

        switch (type) {
        case INITIALIZE_NONE:
            break;
        case INITIALIZE_DEFAULT:
            {
                last_field = f;
                last_char = initialize_uniform_char(p, f);

                if (last_char != -1) {
                    if (f->flag_occurs) {
                        CB_REFERENCE(c)->subs =
                            cb_cons(cb_int1, CB_REFERENCE(c)->subs);
                    }

                    for (; f->sister; f = f->sister) {
                        if (!f->sister->redefines) {
                            if (initialize_type(p, f->sister, 0) != INITIALIZE_DEFAULT
                                || initialize_uniform_char(p, f->sister) != last_char) {
                                break;
                            }
                        }
                    }

                    if (f->sister) {
                        size = f->sister->offset - last_field->offset;
                    } else {
                        size = ff->offset + ff->size - last_field->offset;
                    }
                    if (init_default_byte_value || (last_char != cb_defaultbyte)) {
                        output_initialize_uniform(c, last_char, (int)size);
                    }
                    break;
                }
                /* Fall through */
            }
        default:
            if (type == INITIALIZE_ONE || type == INITIALIZE_ONE_VALUE) {
                /*output_initialize_begin_occurs_loop(x,f);*/
                output_initialize_one(p, c, init_default_byte_value);
                /*output_initialize_end_occurs_loop(x,f);*/
            } else {
                output_initialize_compound(p, c, init_default_byte_value);
            }

        }
    }
    output_initialize_end_occurs_loop(x, ff);
}

static void
output_initialize(struct cb_initialize *p) {
    struct cb_field *f;
    int             c;
    int             occurs = 1;
    int             cache = -1;
    int             save = cb_disable_runtime_check;

    cb_disable_runtime_check = 1;

    if (CB_REFERENCE_P(p->var) && CB_FILE_P(cb_ref(p->var))) {
        struct cb_file *file  = CB_FILE(cb_ref(p->var));
        if (file->external) {
            output_file_initialization(file, 1);
        } else {
            cb_error_x(CB_TREE(p), "Only EXTERNAL File may be INITIALIZE \n");
        }
        cb_disable_runtime_check = save;
        return;
    }
    f = cb_field(p->var);
    if (cb_flag_initialize_optimize && p->flag_statement && f->children && f->size > 10) {
        cache = initialize_cache_cnt++;
    }
    cb_disable_runtime_check = 1;
    if (cache >= 0) {
        output_prefix();
        output("if (initcache[%d])\n", cache);
        output_indent("{");
        output_prefix();
        output("memcpy (");
        output_data(p->var);
        output(", initcache[%d], %d);\n", cache, f->size);
        output_indent("}");
        output_prefix();
        output("else\n");
        output_indent("{");
        output_prefix();
        output("initcache[%d] = cob_malloc_cbl_allocation(rtd, %d, 1, 1);\n", cache, f->size);
    }

    if (CB_REFERENCE_P(p->var) && CB_REFERENCE(p->var)->cb_all_occurs) {
        occurs = f->occurs_max;
    }
    switch (initialize_type(p, f, 1)) {
    case INITIALIZE_NONE:
        break;
    case INITIALIZE_ONE:
    case INITIALIZE_ONE_VALUE:
        output_initialize_one(p, p->var, 1);
        break;
        /*
        case INITIALIZE_EXTERNAL:
            if ( p->flag_statement ) {
                output_initialize_compound(p,p->var,1);
            } else {
                output_initialize_external (p->var, f);
            }
            break;
        */
    case INITIALIZE_DEFAULT:
        c = initialize_uniform_char(p, f);
        if (c != -1) {
            output_initialize_uniform(p->var, c, f->size * occurs);
        } else {
            struct cb_field *thechildren = f->children ? f->children : f->bitfield_children;
            if (thechildren) {
                output_initialize_compound(p, p->var, 1);
            } else {
                output_initialize_one(p, p->var, 1);
            }
        }
        break;
    case INITIALIZE_COMPOUND:
        output_initialize_compound(p, p->var, 1);
        break;
    case INITIALIZE_UNIFORM:
        if (p->flag_filldefaultbyte && cb_use_defaultbyte) c = cb_defaultbyte;
        else c = CHAR_SP;
        output_initialize_uniform(p->var, c, f->size * occurs);
        break;
    }
    if (cache >= 0) {
        output_prefix();
        output("memcpy (initcache[%d], ", cache);
        output_data(p->var);
        output(", %d);\n", f->size);
        output_indent("}");
    }
    cb_disable_runtime_check = save;
}

/*
 *  Fix Value
 */

static int
need_fixvalue(struct cb_field *f) {

    if (!f->redefines) {
        if (!f->children) {
            return (f->usage == CB_USAGE_DISPLAY) && f->pic &&
                   (f->pic->category ==  CB_CATEGORY_NUMERIC);
        } else {
            for (f = f->children; f; f = f->sister) {
                if (need_fixvalue(f)) {
                    return (1);
                }
            }
        }
    }
    return (0);
}

static void
output_fixvalue_one(cb_tree x) {
    struct cb_field     *f;
    f = cb_field(x);

    switch (CB_TREE_CATEGORY(x)) {
    case CB_CATEGORY_NUMERIC:
        if (f->usage == CB_USAGE_DISPLAY) {
            output_prefix();
            output("cb_fix_display_value (rtd, ");
            output_param(x, 0);
            output(");\n");
        }
        break;
    default:
        break;
    }


}

static void
output_fixvalue_compound(cb_tree x) {
    struct cb_field *ff;
    struct cb_field *f;
    cb_tree     c;
    int     i;

    ff = cb_field(x);
    for (f = ff->children; f; f = f->sister) {
        if (!f->redefines && need_fixvalue(f)) {
            c = cb_build_field_reference(f, x);
            if (f->flag_occurs) {
                /* Begin occurs loop */
                i = f->indexes;
                i_counters[i] = 1;
                output_line("for (i%d = 1; i%d <= %d; i%d++)",
                            i, i, f->occurs_max, i);
                output_indent("{");
                CB_REFERENCE(c)->subs =
                    cb_cons(cb_i[i], CB_REFERENCE(c)->subs);
            }
            if (!f->children) {
                output_fixvalue_one(c);
            } else {
                output_fixvalue_compound(c);
            }

            if (f->flag_occurs) {
                /* Close loop */
                CB_REFERENCE(c)->subs = CB_CHAIN(CB_REFERENCE(c)->subs);
                output_indent("}");
            }
        }


    }
}

static void
output_fixvalue(struct cb_fixvalue *p) {
    struct cb_field *f;

    if (!p->flag_spzero) {
        return;
    }
    f = cb_field(p->var);
    if (f->children) {
        output_fixvalue_compound(p->var);
    } else {
        output_fixvalue_one(p->var);
    }
}

/*
 * SEARCH
 */

static void
output_occurs(struct cb_field *p) {
    if (p->occurs_depending) {
        if (cb_flag_validate_odo && !cb_disable_runtime_check) {
            output("cob_validate_range(%d, %d, ", p->occurs_min, p->occurs_max);
        }
        output_integer(p->occurs_depending);
        if (cb_flag_validate_odo && !cb_disable_runtime_check) {
            output(") ");
        }
    } else {
        output("%d", p->occurs_max);
    }
}

static void
output_search_whens(cb_tree table, cb_tree var, cb_tree stmt, cb_tree whens, cb_tree x_refcheck) {
    cb_tree     l;
    struct cb_field *p;
    cb_tree     idx = NULL;

    p = cb_field(table);
    /* Determine the index to use */
    if (var) {
        for (l = p->index_list; l; l = CB_CHAIN(l)) {
            if (cb_ref(CB_VALUE(l)) == cb_ref(var)) {
                idx = var;
            }
        }
    }
    if (!idx) {
        idx = CB_VALUE(p->index_list);
    }

    /* Start loop */
    output_line("while (1)");
    index_optimize_wipe_buffer();
    push_io_to_stack();
    output_indent("{");

    if (x_refcheck) output_stmt(x_refcheck);
    /* End test */
    output_prefix();
    output("if (");
    output_integer(idx);
    output(" > ");
    output_occurs(p);
    output(")\n");
    output_indent("{");
    if (stmt) {
        output_stmt(stmt);
        copy_stack_to_io();
    }
    output_line("break;");
    output_indent("}");

    /* WHEN test */
    /*CIT*/
    for (l = whens; l; l = CB_IF(l)->stmt2) {
        CB_IF(l)->stmt1 = cb_list_add(CB_IF(l)->stmt1, cb_build_inline("break;"));
    }
    output_stmt(whens);
    copy_stack_to_io();
    /*CIT*/
    /*output_line ("else");*/
    output_indent("{");
    output_prefix();
    output_integer(idx);
    output("++;\n");
    if (var && var != idx) {
        output_move(idx, var);
    }
    output_line("continue;");
    output_indent("}");
    /*CIT*/
    /*output_line ("break;");*/
    output_indent("}");
    pop_stack_to_io();
}

static void
output_search_all(cb_tree table, cb_tree stmt, cb_tree cond, cb_tree when, cb_tree x_refcheck) {
    struct cb_field *p;
    cb_tree     idx;

    p = cb_field(table);
    idx = CB_VALUE(p->index_list);
    /* Header */
    output_indent("{");
    output_line("int ret;");
    output_line("int head = %d - 1;", p->occurs_min < 1 ? 1 : p->occurs_min);
    output_prefix();
    output("int tail = ");
    output_occurs(p);
    output(" + 1;\n");
    output_prefix();
    if (p->occurs_depending) {
        output("int dep = (");
        output_integer(p->occurs_depending);
        output("<= 0 );\n");
    }


    output(";\n");

    /* Start loop */
    output_line("while (1)");
    index_optimize_wipe_buffer();
    push_io_to_stack();
    output_indent("{");
    if (x_refcheck) output_stmt(x_refcheck);

    /* End test */
    if (p->occurs_depending) output_line("if ((head >= tail - 1) || dep )");
    else output_line("if (head >= tail - 1)");

    output_indent("{");
    if (stmt) {
        output_stmt(stmt);
        copy_stack_to_io();
    }
    output_line("break;");
    output_indent("}");

    /* Next index */
    output_prefix();
    output_integer(idx);
    output(" = (head + tail) / 2;\n");

    /* WHEN test */
    output_prefix();
    output("if (");
    output_cond(cond, 1, 0);
    output(")\n");
    if (CB_TREE_TAG(when) != CB_TAG_IF) {
        output_indent("{");
    }
    output_indent_level += 2;
    output_stmt(when);
    copy_stack_to_io();
    output_indent_level -= 2;
    if (CB_TREE_TAG(when) != CB_TAG_IF) {
        output_indent("}");
    }
    output_line("else");
    output_indent("{");
    output_line("if (ret < 0)");
    output_prefix();
    output("  head = ");
    output_integer(idx);
    output(";\n");
    output_line("else");
    output_prefix();
    output("  tail = ");
    output_integer(idx);
    output(";\n");
    output_line("continue;");
    output_indent("}");
    output_line("break;");
    output_indent("}");
    output_indent("}");
    pop_stack_to_io();
}

static void
output_search(struct cb_search *p, cb_tree x_refcheck) {
    if (p->flag_all) {
        output_search_all(p->table, p->end_stmt,
                          CB_IF(p->whens)->test, CB_IF(p->whens)->stmt1, x_refcheck);
    } else {
        output_search_whens(p->table, p->var, p->end_stmt, p->whens, x_refcheck);
    }
}

/*
 * CALL
 */
static void
output_call_param_size(cb_tree x, cb_tree l, int as_pointer) {
    int sizes;
    int force_size = 0;
    struct cb_field *f = NULL;

    if (l) {
        sizes = CB_SIZES_INT(l);
    } else {
        sizes = CB_SIZE_AUTO;
    }
    if (x) {
        f = cb_field(x);
        if (as_pointer && CB_REFERENCE_P(x) && CB_REFERENCE(x)->length && CB_LITERAL_P(CB_REFERENCE(x)->length)) {
            sizes = CB_SIZE_AUTO;
            force_size = cb_get_int(CB_REFERENCE(x)->length);
        }
    }
    if (sizes == CB_SIZE_AUTO && f) {
        if (f->pic && f->pic->have_sign) {
            output("(unsigned ");
        } else {
            output("(");
        }
        if (!force_size) {
            if (f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6 ||
                (f->usage == CB_USAGE_DISPLAY && CB_TREE_CLASS(f) == CB_CLASS_NUMERIC)) {
                sizes = f->pic->digits - f->pic->scale;
            } else {
                sizes = f->size;
            }
        } else {
            sizes = force_size;
        }
        switch (sizes) {
        case 0:
            sizes = CB_SIZE_4;
            break;
        case 1:
            sizes = CB_SIZE_1;
            break;
        case 2:
            sizes = CB_SIZE_2;
            break;
        case 3:
            sizes = CB_SIZE_4;
            break;
        case 4:
            sizes = CB_SIZE_4;
            break;
        case 5:
            sizes = CB_SIZE_8;
            break;
        case 6:
            sizes = CB_SIZE_8;
            break;
        case 7:
            sizes = CB_SIZE_8;
            break;
        default:
            sizes = CB_SIZE_8;
            break;
        }
    } else {
        if (CB_SIZES_INT_UNSIGNED(l)) {
            output("(unsigned ");
        } else {
            output("(");
        }
    }
    switch (sizes) {
    case CB_SIZE_1:
        output("char");
        break;
    case CB_SIZE_2:
        output("short");
        break;
    case CB_SIZE_4:
        output("int");
        break;
    case CB_SIZE_8:
        output("long long");
        break;
    default:
        output("int");
        break;
    }
    if (as_pointer) {
        output("*");
    }
    output(")");
}

static int
is_entry_name(const char *c) {
    cb_tree             l;
    for (l = current_codegen_prog->entry_list; l; l = CB_CHAIN(l)) {
        struct cb_entry *e = CB_ENTRY(CB_VALUE(l));
        if (strcasecmp((char *)(CB_LABEL(e->label)->name), c) == 0) {
            return 1;
        }
    }
    return 0;
}

static void
output_call(struct cb_call *p) {
    cb_tree             x;
    cb_tree             l;
    cb_tree             func_x;
    struct cb_literal   *lp;
    char                *callp;
    struct cb_field     *f;
    char                *system_call = NULL;
    struct system_table *psyst;
    size_t              n;
    size_t              parmnum;
    int                 retptr;
    int                 dynamic_link = 1;
    int                 need_param = !cb_flag_all_external_link || cb_sticky_linkage;
    int                 local_0_prototype = 0;
    char                call_conv[20];
    char               *call_litteral_program_name = NULL;
    char               *call_encoded_program_name = NULL;
    struct staticlink *slp;

    call_conv[0] = 0;
    retptr = p->call_convention & CB_CALL_RETURN_MASK;

    /* System routine entry points */
    if (p->is_system) {
        lp = CB_LITERAL(p->name);
        if (strstr("C$DEBUG",  (const char *)lp->data) && !cb_flag_debuginfo) {
            /* Ignore all C$DEBUG call if not producing debug */
            return;
        }
        psyst = (struct system_table *)&system_tab[0];
        for (; psyst->syst_name; psyst++) {
            if (!strcasecmp((const char *)lp->data,
                            (const char *)psyst->syst_name)) {
                system_call = (char *)psyst->syst_call;
                dynamic_link = 0;
                need_param = 1;
                break;
            }
        }
    }

    cob_enterprise_output_call_header();
    if ((cb_flag_static_call || (p->call_convention & CB_CALL_STATIC)) && CB_LITERAL_P(p->name)) {
        dynamic_link = 0;
    }
    if (CB_LITERAL_P(p->name)) {
        call_litteral_program_name = (char *)(CB_LITERAL(p->name)->data);
        for (l = current_codegen_prog->entry_list; l; l = CB_CHAIN(l)) {
            struct cb_entry *e = CB_ENTRY(CB_VALUE(l));
            if (strcasecmp((char *)(CB_LABEL(e->label)->name), call_litteral_program_name) == 0) {
                /* IF Local entry avoid extra NULL when call*/
                local_0_prototype = 1;
            }
        }
        call_encoded_program_name  =  cb_encode_program_id(call_litteral_program_name);
        slp = cobc_find_static_symb(call_encoded_program_name);
        if (slp) {
            if (slp->static_link) {
                dynamic_link = 0;
            }
            if (slp->external_link > 0) {
                /*need_param = !cb_flag_binary_optimize;*/
                need_param = 0;
            } else if (slp->external_link < 0) {
                /*need_param = !cb_flag_binary_optimize;*/
                need_param = 1;
            }

            call_litteral_program_name = slp->staticlink_symb;
            free(call_encoded_program_name);
            call_encoded_program_name = strdup(call_litteral_program_name);
        }
    }

    if (p->call_convention & CB_CALL_STDCALL) {
        strcpy(call_conv, "_stdcall");
    }
    /*CIT*/
    if (cb_flag_profiling) {
        cob_enterprise_output_profiling_tick();
    }

/*CIT*/
    if (need_param) {
        output_debug_callback(COB_DBCALLBACK_PERFORM_ENTER);
    }

    /* local variables */
    output_indent("{");
#ifdef  COB_NON_ALIGNED
    if ((dynamic_link && (retptr == CB_CALL_RETURN_PTR)) ||
        (retptr == CB_CALL_RETURN_INTO)) {
        output_line("void *temptr;");
    }
#else
    if (retptr == CB_CALL_RETURN_INTO) {
        output_line("void *temptr;");
    }
#endif
    if (CB_REFERENCE_P(p->name)
        && CB_FIELD_P(CB_REFERENCE(p->name)->value)
        && CB_FIELD(CB_REFERENCE(p->name)->value)->usage == CB_USAGE_PROGRAM_POINTER) {
        dynamic_link = 0;
    }

    /* Setup arguments */
    for (l = p->args, n = 1; l; l = CB_CHAIN(l), n++) {
        x = CB_VALUE(l);
        switch (CB_CALL_BY(CB_PURPOSE_INT(l))) {
        case CB_CALL_BY_REFERENCE:
        case CB_CALL_BY_DEFAULT:
        case CB_CALL_BY_DESCRIPTOR:
            if (CB_NUMERIC_LITERAL_P(x) || CB_BINARY_OP_P(x)) {
                output_line("union {");
                output_line("\tunsigned char data[8];");
                output_line("\tlong long     datall;");
                output_line("\tint           dataint;");
                output_line("} content_%d;", (int)n);
            } else if (CB_CAST_P(x)) {
                output_line("void *ptr_%d;", (int)n);
            }
            break;
        case CB_CALL_BY_CONTENT:
            if (CB_CAST_P(x)) {
                output_line("void *ptr_%d;", (int)n);
            } else if (CB_TREE_TAG(x) != CB_TAG_INTRINSIC &&
                       x != cb_null && !(CB_CAST_P(x))) {
                output_line("union {");
                output("\tunsigned char data[");
                if (CB_NUMERIC_LITERAL_P(x) ||
                    CB_BINARY_OP_P(x) || CB_CAST_P(x)) {
                    output("8");
                } else {
                    if (CB_REF_OR_FIELD_P(x)) {
                        output("%d", (int)cb_field(x)->size);
                    } else {
                        output_size_runtime(x);
                    }
                }
                output("+1];\n");
                output_line("\tlong long     datall;");
                output_line("\tint           dataint;");
                output_line("} content_%d = {0};", (int)n);
            }
            break;
        }
    }
    output("\n");
    if (p->returning) {
        output_line("rtd->return_field = NULL;");
    }

    for (l = p->args, n = 1; l; l = CB_CHAIN(l), n++) {
        x = CB_VALUE(l);
        switch (CB_CALL_BY(CB_PURPOSE_INT(l))) {
        case CB_CALL_BY_DEFAULT:
        case CB_CALL_BY_REFERENCE:
            if (CB_NUMERIC_LITERAL_P(x)) {
                output_prefix();
                if (cb_fits_int(x)) {
                    output("content_%d.dataint = ", (int)n);
                    if (cb_flag_call_using_comp5_as_comp) {
#ifndef WORDS_BIGENDIAN
                        output("(int) COB_BSWAP_32(%d)", cb_get_int(x));
#else
                        output("%d", cb_get_int(x));
#endif
                    } else output("%d", cb_get_int(x));
                } else {
                    output("content_%d.datall = ", (int)n);
                    if (cb_flag_call_using_comp5_as_comp) {
#ifndef WORDS_BIGENDIAN
                        output("(long long) COB_BSWAP_64(%lldLL)", cb_get_long_long(x));
#else
                        output("%lldLL", cb_get_long_long(x));
#endif
                    } else output("%lldLL", cb_get_long_long(x));
                }
                output(";\n");
            } else if (CB_BINARY_OP_P(x)) {
                output_prefix();
                output("content_%d.dataint = ", (int)n);
                output_integer(x);
                output(";\n");
            } else if (CB_CAST_P(x)) {
                output_prefix();
                output("ptr_%d = ", (int)n);
                output_integer(x);
                output(";\n");
            }
            break;
        case CB_CALL_BY_CONTENT:
            if (CB_CAST_P(x)) {
                output_prefix();
                output("ptr_%d = ", (int)n);
                output_integer(x);
                output(";\n");
            } else if (CB_TREE_TAG(x) != CB_TAG_INTRINSIC) {
                if (CB_NUMERIC_LITERAL_P(x)) {
                    output_prefix();
                    if (cb_fits_int(x)) {
                        output("content_%d.dataint = ", (int)n);
                        output("%d", cb_get_int(x));
                    } else {
                        output("content_%d.datall = ", (int)n);
                        output("%lldLL", cb_get_long_long(x));
                    }
                    output(";\n");
                } else if (CB_REF_OR_FIELD_P(x) &&
                           CB_TREE_CATEGORY(x) == CB_CATEGORY_NUMERIC &&
                           cb_field(x)->usage == CB_USAGE_LENGTH) {
                    output_prefix();
                    output("content_%d.dataint = ", (int)n);
                    output_integer(x);
                    output(";\n");
                } else if (x != cb_null && !(CB_CAST_P(x))) {
                    output_prefix();
                    output("memcpy (content_%d.data, ", (int)n);
                    output_data(x);
                    output(", ");
                    output_size_runtime(x);
                    output(");\n");
                }
            }
            break;
        case CB_CALL_BY_DESCRIPTOR:
            output_line("call_by_descriptor_struct descriptor_%d = {0};", (int)n);
            if (!CB_INTRINSIC_P(x)) {
                if (CB_NUMERIC_LITERAL_P(x)) {
                    output_prefix();
                    if (cb_fits_int(x)) {
                        output("content_%d.dataint = ", (int)n);
                        output("%d", cb_get_int(x));
                        output(";\n");
                        output_line("descriptor_%d.dsc_w_length = sizeof(content_%d.dataint);", (int)n, (int)n);
                    } else {
                        output("content_%d.datall = ", (int)n);
                        output("%lldLL", cb_get_long_long(x));
                        output(";\n");
                        output_line("descriptor_%d.dsc_w_length = sizeof(content_%d.datall);", (int)n, (int)n);
                    }
                    output_line("descriptor_%d.dsc_a_pointer = &content_%d;", (int)n, (int)n);
                } else if (CB_CAST_P(x) ||
                           (CB_REF_OR_FIELD_P(x) &&
                            CB_TREE_CATEGORY(x) == CB_CATEGORY_NUMERIC &&
                            cb_field(x)->usage == CB_USAGE_LENGTH)) {
                    output_prefix();
                    if (cb_fits_int(x)) {
                        output("content_%d.dataint = ", (int)n);
                        output_integer(x);
                        output(";\n");
                        output_line("descriptor_%d.dsc_w_length = sizeof(content_%d.dataint);", (int)n, (int)n);
                    } else {
                        output("content_%d.datall = ", (int)n);
                        output_integer(x);
                        output(";\n");
                        output_line("descriptor_%d.dsc_w_length = sizeof(content_%d.datall);", (int)n, (int)n);
                    }
                    output_line("descriptor_%d.dsc_a_pointer = &content_%d;", (int)n, (int)n);
                } else if (x != cb_null) {
                    output("\tdescriptor_%d.dsc_w_length = ", (int)n);
                    output_size_runtime(x);
                    output(";\n");
                    output("\tdescriptor_%d.dsc_a_pointer = ", (int)n);
                    output_data(x);
                    output(";\n");
                }
            }
            break;
        }
    }

    /* Function name */
    n = 0;
    for (l = p->args; l; l = CB_CHAIN(l), n++) {
        x = CB_VALUE(l);
        /* ! CB_TAG_INTRINSIC always store intrisic call result into cob_procedure_parameters */
        if ((!cb_flag_all_external_call && need_param) || (CB_TREE_TAG(x) == CB_TAG_INTRINSIC)) {
            field_iteration = (int)n;
            output_prefix();
            output("module->cob_procedure_parameters[%d] = ", (int)n);
            switch (CB_TREE_TAG(x)) {
            case CB_TAG_LITERAL:
            case CB_TAG_FIELD:
            case CB_TAG_INTRINSIC:
                output_param(x, -1);
                break;
            case CB_TAG_REFERENCE:
                switch (CB_TREE_TAG(CB_REFERENCE(x)->value)) {
                case CB_TAG_LITERAL:
                case CB_TAG_FIELD:
                case CB_TAG_INTRINSIC:
                    output_param(x, -1);
                    break;
                default:
                    output("NULL");
                    break;
                }
                break;
            default:
                output("NULL");
                break;
            }
            output(";\n");
        }
    }
    /*
    if ( need_param ) {
        if ( !cb_flag_binary_optimize ) {
            for ( parmnum = n; parmnum < n + 4; parmnum++ ) {
                output_line ("module->cob_procedure_parameters[%d] = NULL;", (int)parmnum);
            }
        }
    } 
    */
    if (need_param) {
        output_prefix();
        output("rtd->cob_call_params = %d;\n", (int)n);
    }
    parmnum = n;
    output_prefix();
    if (!dynamic_link) {
        if (CB_REFERENCE_P(p->name) &&
            CB_FIELD_P(CB_REFERENCE(p->name)->value) &&
            CB_FIELD(CB_REFERENCE(p->name)->value)->usage == CB_USAGE_PROGRAM_POINTER) {
            output("cob_unifunc.func_void = ");
            output_integer(p->name);
            output(";\n");
            output_prefix();
            switch (retptr) {
            case CB_CALL_RETURN_FIELD_ADDR:
            case CB_CALL_RETURN_INT:
                output_integer(current_codegen_prog->cb_return_code);
                output(" = cob_unifunc.funcint%s", call_conv);
                break;
            case CB_CALL_RETURN_VOID:
                output("cob_unifunc.funcint%s", call_conv);
                break;
            case CB_CALL_RETURN_PTR:
#ifdef  COB_NON_ALIGNED
                output("temptr");
#else
                output_integer(p->returning);
#endif
                output(" = cob_unifunc.funcptr%s", call_conv);
                break;
            case CB_CALL_RETURN_ADDROF:
                output_data(p->returning);
                output(" = cob_unifunc.funcptr%s", call_conv);
                break;
            case CB_CALL_RETURN_INTO:
                output("temptr = cob_unifunc.funcptr%s", call_conv);
                break;
            }
        } else {
            /* Static link */
            switch (retptr) {
            case CB_CALL_RETURN_FIELD_ADDR:
            case CB_CALL_RETURN_INT:
                output_integer(current_codegen_prog->cb_return_code);
                output(" = ");
                break;
            case CB_CALL_RETURN_VOID:
                break;
            case CB_CALL_RETURN_PTR:
#ifdef  COB_NON_ALIGNED
                output("temptr = ");
#else
                output_integer(p->returning);
                output(" = ");
#endif
                break;
            case CB_CALL_RETURN_ADDROF:
                output_data(p->returning);
                output(" = ");
                break;
            case CB_CALL_RETURN_INTO:
                output("temptr = ");
                break;
            }
            if (retptr) {
                output("(void *)");
            }
            if (system_call) {
                output("%s", system_call);
            } else {
                output("%s", call_encoded_program_name);
                /*CIT*/
                if (p->call_convention & CB_CALL_STDCALL) {
                    if (retptr) {
                        output_storage("extern void* __stdcall ");
                    } else {
                        output_storage("extern int __stdcall ");
                    }
                    output_storage("%s ();\n", call_encoded_program_name);
                    /*  cb_encode_program_id ((char *)(CB_LITERAL (p->name)->data)));*/


                }
            }
        }
    } else {
        /* Dynamic link */
        if (CB_LITERAL_P(p->name)) {

            /*CIT*/
            if (cb_flag_call_optimize) {
                callp = call_encoded_program_name;
                lookup_call(callp);
                output("if (unlikely(call_%s.func_void == NULL)) {\n", callp);
                output_prefix();
                output("  call_%s.func_void = ", callp);
            } else {
                callp = NULL;
                output("cob_unifunc.func_void = ");
            }
            if (!p->stmt1) {
                output("cob_resolve_1 (rtd, (const char *)\"%s\");\n",
                       call_litteral_program_name);
            } else {
                output("cob_resolve (rtd, (const char *)\"%s\");\n",
                       call_litteral_program_name);
            }
            output_prefix();
            if (cb_flag_call_optimize) {
                output("}\n");
            }
        } else {
            callp = NULL;
            if (!p->stmt1) {
                func_x = cb_build_funcall_1(
                    "cob_call_resolve_1", p->name);
            } else {
                func_x = cb_build_funcall_1(
                    "cob_call_resolve", p->name);
            }
            if (!(!output_register_only && set_up_decimal_list(func_x))) {
                output("cob_unifunc.func_void = ");
                output_funcall(func_x);
                output(";\n");
            }
        }
        if (p->stmt1) {
            push_io_to_stack();
            if (callp) {
                output_line("if (unlikely(call_%s.func_void == NULL))", callp);
            } else {
                output_line("if (unlikely(cob_unifunc.func_void == NULL))");
            }
            output_indent("{");
            output_indent_level += 2;
            output_stmt(p->stmt1);
            output_indent_level -= 2;
            output_indent("}");
            output_line("else");
            output_indent("{");
            pop_stack_to_io();
        }

        output_prefix();
        switch (retptr) {
        case CB_CALL_RETURN_FIELD_ADDR:
        case CB_CALL_RETURN_INT:
            output_integer(current_codegen_prog->cb_return_code);
            output(" = ");
            break;
        case CB_CALL_RETURN_VOID:
            break;
        case CB_CALL_RETURN_PTR:
#ifdef  COB_NON_ALIGNED
            output("temptr = ");
#else
            output_integer(p->returning);
            output(" = ");
#endif
            break;
        case CB_CALL_RETURN_ADDROF:
            output_data(p->returning);
            output(" = ");
            break;
        case CB_CALL_RETURN_INTO:
            output("temptr = ");
            break;
        }
        if (retptr && retptr != CB_CALL_RETURN_FIELD_ADDR) {
            if (callp) {
                output(" call_%s.funcptr%s", callp, call_conv);
            } else {
                output(" cob_unifunc.funcptr%s", call_conv);
            }
        } else {
            if (callp) {
                output(" call_%s.funcint%s", callp, call_conv);
            } else {
                output(" cob_unifunc.funcint%s", call_conv);
            }
        }
    }

    /* Arguments */
    output(" (");
    if (system_call) {
        output("rtd");
        if (p->args) {
            output(", ");
        }
    }
    for (l = p->args, n = 1; l; l = CB_CHAIN(l), n++) {
        field_iteration = (int)n - 1;
        x = CB_VALUE(l);
        switch (CB_CALL_BY(CB_PURPOSE_INT(l))) {
        case CB_CALL_BY_DEFAULT:
        case CB_CALL_BY_REFERENCE:
            if (CB_NUMERIC_LITERAL_P(x) || CB_BINARY_OP_P(x)) {
                output("content_%d.data", (int)n);
            } else if (CB_REFERENCE_P(x) && CB_FILE_P(cb_ref(x))) {
                output("cob_get_extfh_fcd_ptr(rtd,%s%s)", CB_PREFIX_FILE, CB_FILE(cb_ref(x))->cname);
            } else if (CB_CAST_P(x)) {
                output("&ptr_%d", (int)n);
            } else {
                output_data(x);
            }
            break;
        case CB_CALL_BY_CONTENT:
            if (CB_TREE_TAG(x) != CB_TAG_INTRINSIC && x != cb_null) {
                if (CB_CAST_P(x)) {
                    output("&ptr_%d", (int)n);
                } else {
                    output("content_%d.data", (int)n);
                }
            } else {
                output_data(x);
            }
            break;
        case CB_CALL_BY_DESCRIPTOR:
            if (CB_TREE_TAG(x) != CB_TAG_INTRINSIC && x != cb_null) {
                if (CB_CAST_P(x)) {
                    output("&ptr_%d", (int)n);
                } else {
                    output("&descriptor_%d", (int)n);
                }
            } else {
                if (CB_TREE_TAG(x) == CB_TAG_INTRINSIC) {
                    output("( descriptor_%d.dsc_w_length= module->cob_procedure_parameters[%d]->size ", n, field_iteration);
                    output(", descriptor_%d.dsc_a_pointer= module->cob_procedure_parameters[%d]->data, ", n, field_iteration);
                    output("&descriptor_%d)", (int)n);
                } else {
                    output_data(x);
                }
            }
            break;
        case CB_CALL_BY_VALUE:
            if (CB_TREE_TAG(x) != CB_TAG_INTRINSIC) {
                switch (CB_TREE_TAG(x)) {
                case CB_TAG_CONST:
                case CB_TAG_CAST:
                    output_integer(x);
                    break;
                case CB_TAG_LITERAL:
                    if (CB_TREE_CLASS(x) == CB_CLASS_NUMERIC) {
                        output_call_param_size(NULL, l, 0);
                        output("%d", cb_get_int(x));
                    } else {
                        output("%d", CB_LITERAL(x)->data[0]);
                    }
                    break;
                default:
/* RXWRXW
                    if (CB_TREE_CLASS (x) == CB_CLASS_NUMERIC) {
                        output_integer (x);
                    } else {
                        output ("*(");
                        output_data (x);
                        output (")");
                    }
*/
                    f = cb_field(x);
                    switch (f->usage) {
                    case CB_USAGE_BINARY:
                    case CB_USAGE_COMP_5:
                    case CB_USAGE_COMP_X:
                        /* RXWRXW */
                    case CB_USAGE_PACKED:
                    case CB_USAGE_COMP_6:
                        /*CIT*/
                        output_call_param_size(x, l, 0);
                        output("(");
                        output_integer(x);
                        output(")");
                        break;

                    case CB_USAGE_DISPLAY:
                        /*CIT*/
                        if (cb_flag_mf_by_value
                            && !(CB_PURPOSE_INT(l) & CB_CALL_BY_PROTOTYPED)) {
                            if ((CB_PURPOSE_INT(l) & CB_CALL_BY_PROTOTYPED) &&
                                CB_SIZES_INT(l) != CB_SIZE_AUTO) {
                                output_call_param_size(x, l, 0);
                            }
                            output("(*(");
                            output_call_param_size(x, NULL, 1);
                            output("(");
                            output_data(x);
                            output(")");
                            output("))");
                        } else {
                            output_call_param_size(x, l, 0);
                            output("(");
                            output_integer(x);
                            output(")");
                        }
                        break;
                    case CB_USAGE_INDEX:
                    case CB_USAGE_LENGTH:
                    case CB_USAGE_POINTER:
                    case CB_USAGE_PROGRAM_POINTER:
                        output_integer(x);
                        break;
                    default:
                        output("*(");
                        output_data(x);
                        output(")");
                        break;
                    }
                    break;
                }
            } else {
                output_data(x);
            }
            break;
        }
        if (CB_CHAIN(l)) {
            output(", ");
        }
    }
    /*CIT*/
    if (!system_call && !(p->call_convention & CB_CALL_STDCALL) &&
        !local_0_prototype) {
        if (cb_sticky_linkage || cb_flag_null_param) {
            for (n = 0; n < 2; n++) {
                if (n != 0 || parmnum != 0) {
                    output(", ");
                }
                output("NULL");
            }
        }
    }
    output(");\n");
    if (p->returning) {
        switch (retptr) {
        case CB_CALL_RETURN_INT:
            /* suppress warnings */
            suppress_warn = 1;
            output_line("if (rtd->return_field) {");
            output_prefix();
            output("cob_move (rtd, rtd->return_field, ");
            output_param(p->returning, 0);
            output(");\n");
            output_line("} else {");
            push_io_to_stack();
            output_stmt(cb_build_move(current_codegen_prog->cb_return_code,
                                      p->returning));
            pop_stack_to_io();
            output_line("}");
            suppress_warn = 0;
            break;
        case CB_CALL_RETURN_VOID:
            break;
        case CB_CALL_RETURN_PTR:
#ifdef  COB_NON_ALIGNED
            output_prefix();
            output("memcpy (");
            output_data(p->returning);
            output(", &temptr, %d);\n", (int)(sizeof(void *)));
#endif
            break;
        case CB_CALL_RETURN_ADDROF:
            /*nothing to do*/
            break;
        case CB_CALL_RETURN_INTO:
            output_line("if (rtd->return_field) {");
            output_prefix();
            output("cob_move (rtd, rtd->return_field, ");
            output_param(p->returning, 0);
            output(");\n");
            output_line("} else {");
            output_prefix();
            output("memcpy (");
            output_data(p->returning);
            output(", temptr, ");
            output_size_runtime(p->returning);
            output(");\n");
            output_line("}");

            break;
        case CB_CALL_RETURN_FIELD_ADDR:
            if (p->returning) {
                output_line("if (rtd->return_field) {");
                output_prefix();
                output_data(p->returning);
                output(" = rtd->return_field;\n");
                output_line("} else {");
                output_prefix();
                output_data(p->returning);
                output(" = ");
                output_param(current_codegen_prog->cb_return_code, 0);
                output_line(";\n}");
            }

            break;
        }
    }
    output_line("rtd->return_field = NULL;");

    if (p->stmt3) {
        output_stmt(p->stmt3);
    }
    if (p->stmt2) {
        output_stmt(p->stmt2);
    }
    if (dynamic_link && p->stmt1) {
        output_indent("}");
    }
    output_indent("}");

    /*CIT*/
    if (need_param) {
        output_line("rtd->current_module = module;");
        output_debug_callback(COB_DBCALLBACK_PERFORM_EXIT);
    }
    for (l = p->args, n = 1; l; l = CB_CHAIN(l), n++) {
        x = CB_VALUE(l);
        if ((CB_CALL_BY(CB_PURPOSE_INT(l)) == CB_CALL_BY_REFERENCE || CB_CALL_BY(CB_PURPOSE_INT(l)) == CB_CALL_BY_DEFAULT) &&
            (CB_REFERENCE_P(x) && CB_FILE_P(cb_ref(x))) &&
            CB_FILE(cb_ref(x))->file_status) {
            output_prefix();
            output("cob_update_return_status_to_field(rtd,%s%s,", CB_PREFIX_FILE, CB_FILE(cb_ref(x))->cname);
            output_param(CB_FILE(cb_ref(x))->file_status, 0);
            output_line(");");
        }
    }
    cob_enterprise_output_call_footer();
    if (call_encoded_program_name) {
        free(call_encoded_program_name);
    }
}

/*
 * GO TO
 */

static void
output_goto_1(cb_tree x) {
    output_line("goto %s%d;", CB_PREFIX_LABEL, CB_LABEL(cb_ref(x))->id);
}

static void
output_goto(struct cb_goto *p) {
    cb_tree l;
    int i = 1;

    /*CIT*/
    if (cb_flag_profiling) {
        cob_enterprise_output_profiling_tick();
    }
    if (p->depending) {
        output_prefix();
        output("switch (");
        output_param(cb_build_cast_integer(p->depending), 0);
        output(")\n");
        output_indent("{");
        for (l = p->target; l; l = CB_CHAIN(l)) {
            output_indent_level -= 2;
            output_line("case %d:", i++);
            output_indent_level += 2;
            output_goto_1(CB_VALUE(l));
        }
        output_indent("}");
    } else if (p->target == NULL) {
        needs_exit_prog = 1;
        if (cb_flag_implicit_init) {
            if (p->return_field) {
                output_prefix();
                output("rtd->return_field = ");
                output_param(p->return_field, 0);
                output_line(";");
            }
            output_line("goto exit_program;");
        } else {
            output_line("if (module->next) {");
            if (p->return_field) {
                output_prefix();
                output("  rtd->return_field = ");
                output_param(p->return_field, 0);
                output_line(";");
            }
            output_line("  goto exit_program;");
            output_line("}");


        }
    } else if (p->target == cb_int1) {
        needs_exit_prog = 1;
        if (p->return_field) {
            output_prefix();
            output("rtd->return_field = ");
            output_param(p->return_field, 0);
            output_line(";");
        }
        output_line("goto exit_program;");
    } else {
        output_goto_1(p->target);
    }
}


static void
output_alter_entry(struct cb_label *lb) {
    cb_tree g;

    g = find_alter_goto(current_codegen_prog, (const char *)lb->name);
    if (g && CB_GOTO_P(g)) {
        output_goto(CB_GOTO(g));
    }
}

/*
 * PERFORM
 */

void
output_perform_label(struct cb_label *lb, struct cb_label *le) {
    struct label_list       *l;
    struct thru_label_list  *tl;
    if (output_register_only) return;

    output_line("frame_ptr->perform_through = %d;", le->id);
    if (!cb_flag_goto_gcc) {
        l = cobc_malloc(sizeof(struct label_list));
        l->id = cb_id;
        /*
        if ( label_cache == NULL ) {
            l->call_num = 0;
        } else {
            l->call_num = label_cache->call_num + 1;
        } 
        */
        l->call_num = cb_call_num++;
        output_line("frame_ptr->return_address = %d;", l->call_num);
        output_line("goto %s%d;", CB_PREFIX_LABEL, lb->id);
        if (cb_flag_optreturn) {
            tl = lookup_thru_label_list(le->id);
            l->next = tl->label;
            tl->label = l;
        } else {
            l->next = label_cache;
            label_cache = l;
        }
    } else {
        output_line("frame_ptr->return_address = &&%s%d;",
                    CB_PREFIX_LABEL, cb_id);
        output_line("goto %s%d;", CB_PREFIX_LABEL, lb->id);
    }


}

static void
output_perform_call(struct cb_label *lb, struct cb_label *le) {

    if (lb == le) {
        output_line("/* PERFORM %s */", lb->name);
    } else {
        output_line("/* PERFORM %s THRU %s */", lb->name, le->name);
    }
    output_line("frame_ptr++;");
    if (cb_flag_stack_check) {
        output_line("if (unlikely(frame_ptr == frame_overflow))");
        output_line("    cob_fatal_error (rtd, COB_FERROR_STACK);");
    }
    if (cb_flag_debuginfo && cur_source_file) {
        output_line("module->debug_stack_frame_curpos++;");
        output_line("module->debug_stack_frame_curpos->line_nr = %d;", cur_source_line);
        output_line("module->debug_stack_frame_curpos->source_file = %s%d;", CB_PREFIX_STR,
                    lookup_string(cur_source_file, strlen((char *)cur_source_file)));
    }

    /*CIT*/
    if (cb_flag_profiling) {
        cob_enterprise_output_profiling_tick();
    }
    output_debug_callback(COB_DBCALLBACK_PERFORM_ENTER);

    output_perform_label(lb, le);
    output_line("%s%d:", CB_PREFIX_LABEL, cb_id);
    cb_id++;
    output_line("frame_ptr--;");
    if (cb_flag_debuginfo) {
        output_line("module->debug_stack_frame_curpos--;");
    }
    output_debug_callback(COB_DBCALLBACK_PERFORM_EXIT);
}

static void
output_perform_exit(struct cb_label *l) {
    if (output_register_only) return;
    if (l->is_global) {
        output_newline();
        output_line("if (entry == %d) ", l->id);
        output_indent("{");
        if (cb_flag_traceall) {
            output_line("cob_reset_trace (rtd);");
        }
        output_exit_program_deallocate(current_codegen_prog);
        output_line("return 0;");
        output_indent("}");
    }
    output_newline();
    output_line("if (frame_ptr->perform_through == %d)", l->id);
    if (!cb_flag_goto_gcc) {
        if (cb_flag_optreturn) {
            output_line("  RETURN_%s%d;", CB_PREFIX_LABEL, l->id);
            /* just to be sure macro exist*/
            lookup_thru_label_list(l->id);
        } else {
            output_line("  goto P_switch;");
        }
    } else {
        output_line("  goto *frame_ptr->return_address;");
    }
    if (cb_perform_osvs) {
        output_line("if (frame_ptr > frame_stack) {");
        output_line("  for (temp_index = frame_ptr - 1; temp_index->perform_through; temp_index--) {");
        output_line("    if (temp_index->perform_through == %d) {", l->id);
        if (cb_flag_debuginfo) {
            output_line("      module->debug_stack_frame_curpos -= (frame_ptr - temp_index);");
        }
        output_line("      frame_ptr = temp_index;");
        if (!cb_flag_goto_gcc) {
            if (cb_flag_optreturn) {
                output_line("    RETURN_%s%d;", CB_PREFIX_LABEL, l->id);
            } else {
                output_line("    goto P_switch;");
            }
        } else {
            output_line("      goto *frame_ptr->return_address;");
        }
        output_line("    }");
        output_line("  }");
        output_line("}");
    }
}

static void
output_perform_once(struct cb_perform *p) {
    if (p->body && CB_PAIR_P(p->body)) {
        output_perform_call(CB_LABEL(cb_ref(CB_PAIR_X(p->body))),
                            CB_LABEL(cb_ref(CB_PAIR_Y(p->body))));
    } else {
        output_stmt(p->body);
    }
    if (p->cycle_label) {
        output_stmt(cb_ref(p->cycle_label));
    }
}

static void
output_perform_until(struct cb_perform *p, cb_tree l, cb_tree x_refcheck) {
    struct cb_perform_varying   *v;
    cb_tree             next;

    if (l == NULL) {
        /* Perform body at the end */
        if (x_refcheck) output_stmt(x_refcheck);
        output_perform_once(p);
        return;
    }

    v = CB_PERFORM_VARYING(CB_VALUE(l));
    next = CB_CHAIN(l);

    output_line("while (1) /* PERFORM UNTIL*/");
    index_optimize_wipe_buffer();
    output_indent("{");

    if (next && CB_PERFORM_VARYING(CB_VALUE(next))->name) {
        output_move(CB_PERFORM_VARYING(CB_VALUE(next))->from,
                    CB_PERFORM_VARYING(CB_VALUE(next))->name);
    }

    if (p->test == CB_AFTER) {
        output_perform_until(p, next, NULL);
    }

    push_io_to_stack();
    if (x_refcheck) output_stmt(x_refcheck);
    output_prefix();
    output("if (");
    output_cond(v->until, 0, 0);
    output(")\n");
    output_line("  break;");

    if (p->test == CB_BEFORE) {
        output_perform_until(p, next, NULL);
    }

    if (v->step) {
        output_stmt(v->step);
    }
    pop_stack_to_io();
    output_indent("}");
}

static void
output_perform(struct cb_perform *p, cb_tree x_refcheck) {
    struct cb_perform_varying *v;

    switch (p->type) {
    case CB_PERFORM_EXIT:
        if (x_refcheck) output_stmt(x_refcheck);
        if (cb_flag_profiling) {
            cob_enterprise_output_profiling_tick();
        }
        if (CB_LABEL(p->data)->need_return) {
            output_perform_exit(CB_LABEL(p->data));
        }
        break;
    case CB_PERFORM_ONCE:
        if (x_refcheck) output_stmt(x_refcheck);
        output_perform_once(p);
        break;
    case CB_PERFORM_TIMES:
        output_prefix();
        output("for (l_cntr.n%d = ", loop_counter);
        output_param(cb_build_cast_integer(p->data), 0);
        output("; l_cntr.n%d > 0; l_cntr.n%d--)\n", loop_counter, loop_counter);
        loop_counter++;
        output_indent("{");
        if (x_refcheck) output_stmt(x_refcheck);
        output_perform_once(p);
        output_indent("}");
        break;
    case CB_PERFORM_UNTIL:
        v = CB_PERFORM_VARYING(CB_VALUE(p->varying));
        if (v->name && v->from) {
            output_move(v->from, v->name);
        }
        output_perform_until(p, p->varying, x_refcheck);
        break;
    case CB_PERFORM_FOREVER:
        output_prefix();
        output("while (1)\n");
        index_optimize_wipe_buffer();
        output_indent("{");
        if (x_refcheck) output_stmt(x_refcheck);
        output_perform_once(p);
        output_indent("}");
        break;
    }
    if (p->exit_label) {
        output_stmt(cb_ref(p->exit_label));
    }
}

static void
output_file_error(struct cb_file *pfile) {
    struct cb_file      *fl;
    cb_tree         l;

    for (l =  current_codegen_prog->local_file_list; l; l = CB_CHAIN(l)) {
        fl = CB_FILE(CB_VALUE(l));
        if (!strcmp(pfile->name, fl->name)) {
            output_perform_call(fl->handler,
                                fl->handler);
            return;
        }
    }
    for (l =  current_codegen_prog->global_file_list; l; l = CB_CHAIN(l)) {
        fl = CB_FILE(CB_VALUE(l));
        if (!strcmp(pfile->name, fl->name)) {
            if (fl->handler_prog == current_codegen_prog) {
                output_perform_call(fl->handler,
                                    fl->handler);
            } else {
                if (cb_flag_traceall) {
                    output_line("cob_reset_trace (rtd);");
                }
                output_line("%s (%d);", fl->handler_prog->main_entry_name, fl->handler->id);
                if (cb_flag_traceall) {
                    output_line("cob_ready_trace (rtd);");
                }
            }
            return;
        }
    }
    output_perform_call(pfile->handler, pfile->handler);
}

static unsigned char*
basename(const unsigned char *name) {
    const unsigned char *base = name;

    while (*name) {
        if (*name == '/' || *name == '\\') {
            base = name + 1;
        }
        name++;
    }
    return(unsigned char *)base;
}

/* 
 * Output Source Location
 */
static void init_output_location(char *source) {
    if (cb_flag_source_location) {
        unsigned char *base = basename((unsigned char *)source);
        output_debug("\t{1, %s, NULL, 0, 0},\n ", lookup_Cstring(base));
        cur_location_idx++;
    }
}

static void
output_location(cb_tree x, const unsigned char *statement,
                const unsigned char *section, const unsigned char *paragraph) {
    cur_source_line = x->source_line;
    cur_source_file = x->source_file;
    if (x->source_line) {
        if (cb_flag_source_location && !suspend_debug_location) {
            output_prefix();
            output("cob_set_location (rtd, %d, %s, %d,", cb_flag_debuginfo ? cur_location_idx++ : 0,
                   lookup_Cstring(x->source_file), x->source_line);
            if (statement) {
                output("%s);\n", lookup_Cstring((unsigned char *)statement));
            } else {
                output("NULL);\n");
            }
            if (cb_flag_debuginfo || paragraph || section) {
                if (cb_debugdb) {
                    char *label = NULL;
                    int sec = 0;
                    if (paragraph || section) {
                        label = (char *)(paragraph ? paragraph : section);
                        sec = paragraph ? 0 : 1;
                    }
                    debugdb_add_line_rec(cb_debugdb, cb_debugdb_module_id, cur_location_idx - 1,
                                         (char *)x->source_file, x->source_line, label, sec);
                } else {
                    output_debug("\t{%d,", x->source_line);
                    if (x->source_file) {
                        unsigned char *base = basename(x->source_file);
                        output_debug("%s,", lookup_Cstring(base));
                    } else {
                        output_debug("NULL,");
                    }

                    if (paragraph || section) {
                        output_debug("\"%s\", 0, %d},\n ", paragraph ? paragraph : section,
                                     paragraph ? 0 : 1);
                    } else {
                        output_debug("NULL, 0, 0},\n ");
                    }
                }
            }
        }
    }
}

static void
output_pragma(struct cb_pragma    *pa) {
    static FILE *save_target = NULL;
    static int save_output_indent_level;
    static int current_codegen_opt = 0;

    struct cb_literal *li;
    if (pa->pragma && CB_LITERAL_P(pa->pragma)) {
        li = CB_LITERAL(pa->pragma);
        if ((strcasecmp((char *)li->data, "DUMP") == 0) &&
            CB_LITERAL_P(pa->option)) {
            li = CB_LITERAL(pa->option);
            if (cb_flag_profiling) {
                output_line("cob_profiling_dump_all_to_file(rtd,\"%s\");", li->data);
            }
        } else if ((strcasecmp((char *)li->data, "PROFILING") == 0) &&
                   CB_LITERAL_P(pa->option)) {
            li = CB_LITERAL(pa->option);
            if (cb_flag_profiling) {
                cob_enterprise_output_profiling_tick();
                cur_profiling_idx++;
                cob_enterprise_output_profiling("\t{\"%s\", 0, 0, 0, 0, 0},\n", li->data);
                output_line("%s_%s[%d].entry_cnt ++;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
            }

        } else if ((strcasecmp((char *)li->data, "INLINE") == 0) &&
                   CB_LITERAL_P(pa->option)) {
            li = CB_LITERAL(pa->option);
            output_line("%s", li->data);


        } else if ((strcasecmp((char *)li->data, "DEBUGOFF") == 0)) {
            output_location(CB_TREE(pa), (unsigned char *)"DEBUGOFF", NULL, NULL);
            suspend_debug_location = 1;
        } else if ((strcasecmp((char *)li->data, "DEBUGON") == 0)) {
            suspend_debug_location = 0;
        }
    } else if (pa->code_gen_tag) {
        output_line("/* PRAGMA OPTIMIZER code_gen_tag=%d code_gen_option=%d (l %d) */",
                    pa->code_gen_tag, pa->code_gen_opt, pa->common.source_line);
        if (pa->code_gen_tag == PRAGMA_CODEGEN_EXTRACT_BEGIN) {
            if (save_target) {
                cb_error_x(CB_TREE(pa), "OPTIMIZER_tag = CODEGEN_BEGIN & save != NULL \n");
                /*ABORT ();*/
            }
            output_line("local_fct%d (rtd, COB_MDS_REF, module); ", pa->code_gen_opt);
            push_output_target(cb_local_function_file);
            save_target = output_target[output_target_idx];
            save_output_indent_level = output_indent_level;
            output_indent_level = 0;
            output_extracting_Code_level = 1;
            current_codegen_opt = pa->code_gen_opt;
            output_line(" ");
            output_line("/*-------------------------------------*/");
            output_line("static void");
            output_line("local_fct%d (COB_RTD, mds_t *mds, cob_module *module) ", pa->code_gen_opt);
            output_indent("{");
            output_line("#include \"%s\"", current_codegen_prog->local_register_name);

            output_line(" ");
        } else if (pa->code_gen_tag == PRAGMA_CODEGEN_EXTRACT_END) {
            if (!save_target) {
                cb_error_x(CB_TREE(pa), "OPTIMIZER_tag = CODEGEN_END & save == NULL \n");
                /*ABORT ();*/
            }
            if (current_codegen_opt != pa->code_gen_opt) {
                cb_error_x(CB_TREE(pa), "OPTIMIZER_opt not match %d  != %d  \n", current_codegen_opt, pa->code_gen_opt);
                /*ABORT ();*/
            }
            output_indent("}");
            pop_output_target();
            save_target = NULL;
            output_indent_level = save_output_indent_level;
            output_extracting_Code_level = 0;
        }
    }
}

/*
 * Output statement
 */

static void
output_ferror_stmt(struct cb_statement *p, int code) {
    output_line("if (unlikely(rtd->cob_exception_code != 0))");
    output_indent("{");
    push_io_to_stack();
    if (p->handler1) {
        if ((code & 0x00ff) == 0) {
            output_line("if ((rtd->cob_exception_code & 0xff00) == 0x%04x)",
                        code);
        } else {
            output_line("if (rtd->cob_exception_code == 0x%04x)", code);
        }
        output_indent("{");
        output_stmt(p->handler1);
        copy_stack_to_io();
        output_indent("}");
        output_line("else");
        output_indent("{");
    }
    if (CB_LIST_P(p->file)) {
        cb_tree l;
        for (l = p->file; l; l = CB_CHAIN(l)) {
            cb_tree v = CB_VALUE(l);
            output("if (((cob_file_extfh*)(rtd->cob_error_file))->extfh_ptr == ");
            output("cob_get_extfh_fcd_ptr(rtd,%s%s)", CB_PREFIX_FILE, CB_FILE(cb_ref(v))->cname);
            output_line(")");
            output_indent("{");
            output_file_error(CB_FILE(cb_ref(v)));
            output_indent("}");
        }
    } else if (CB_FILE_P(p->file)) {
        output_file_error(CB_FILE(p->file));
    }
    output_indent("}");
    if (p->handler1) {
        output_indent("}");
    }
    if (p->handler2 || p->handler3) {
        output_line("else");
        output_indent("{");
        if (p->handler3) {
            output_stmt(p->handler3);
            copy_stack_to_io();
        }
        if (p->handler2) {
            output_stmt(p->handler2);
            copy_stack_to_io();
        }
        output_indent("}");
    }
    pop_stack_to_io();
}

static int
is_only_pragma(cb_tree x) {
    switch (CB_TREE_TAG(x)) {
    case CB_TAG_STATEMENT:
        if (CB_STATEMENT(x) && CB_STATEMENT(x)->body) {
            return is_only_pragma(CB_STATEMENT(x)->body);
        }
        break;
    case CB_TAG_PRAGMA:
        return 1;
    case CB_TAG_LIST:
        if (!CB_CHAIN(x)) { /* Only 1 elem*/
            return (is_only_pragma(CB_VALUE(x)));
        }
        break;
    default:
        return 0;
    }
    return 0;
}




static void index_optimize_wipe_buffer(void) {
    struct index_optimizer *current, *prev;;
    prev = current = index_optimizer_cache;
    while (current) {

        if (current == index_optimizer_cache) {
            index_optimizer_cache = current->next;
            cob_free(current);
            prev = current = index_optimizer_cache;
        } else {
            prev->next = current->next;
            cob_free(current);
            current = prev->next;
        }
    }
    while (zombie) {
        current = zombie;
        zombie = current->next;
        cob_free(current);
    }
    if (io_stack) {
        mark_modified_all_list(io_stack->list);
    }
    initialize_index_buffer();
}

static int find_and_remove_from_index_buffer(cb_tree x) {

    struct index_optimizer *current;
    struct index_optimizer *prev;
    int skip;
    int found = 0;

    prev = current = index_optimizer_cache;
    skip = x->skip_for_decimal;
    while (current) {
        found = is_identical_or_ovelap_fields(x, current->x);
        if (found) {
            if (skip && current->type == DecimalIOT) {
                prev = current;
                current = current->next;
                continue;
            } else {
                if (current->type == IndexIOT && io_stack) mark_modified_element_index(current, io_stack->list);
                else if (current->type == DecimalIOT) {
                    mark_modified_element_decimal(current, io_stack);
                }
            }

            /*remove index from buffer*/
            if (current == index_optimizer_cache) {
                index_optimizer_cache = index_optimizer_cache->next;

                if (current->type == IndexIOT) {
                    delete_element_from_cache(current);
                } else if (!skip) {
                    delete_element_from_cache(current);
                }
                prev = current = index_optimizer_cache;

            } else {
                if (prev != NULL) {
                    prev->next = current->next;
                    if (current->type == IndexIOT) {
                        delete_element_from_cache(current);
                    } else {
                        if (!skip) {
                            delete_element_from_cache(current);
                        }
                    }
                    current = prev->next;

                }
            }
        } else {
            prev = current;
            current = current->next;
        }
    }
    return found;
}


static void check_target_ref_list(cb_tree x) {

    if (x ==  NULL) {
        return;
    }
    if (cb_flag_index_optimize || cb_flag_decimal_optimize) {
        cb_tree l;
        int found;
        /* check wipe_index_buffer*/
        if (x->wipe_index_buffer) {
            index_optimize_wipe_buffer();
        } else {
            /* parcourir la list des target_ref_list*/
            for (l = x->target_ref_list; l; l = CB_CHAIN(l)) {
                /* pour chaque element verifier qu'elle
                   n'overlap pas un des inice de cache*/
                found = find_and_remove_from_index_buffer(CB_VALUE(l));
            }
        }
    }
}


static char * 
CString_param (char *Res, char * Cstring, ...)
{
    va_list ap;
    char buffer[2048];
    cb_tree x;
    va_start(ap, Cstring);
    vsnprintf(buffer, sizeof(buffer)-1, Cstring, ap);
    va_end(ap);

    x = cb_build_alphanumeric_literal (buffer, strlen(buffer), 0);
    sprintf(Res, "&%s%d", CB_PREFIX_CONST, lookup_literal(x));
    return Res;
}

static int
is_single_statement(cb_tree x) {
    switch (CB_TREE_TAG(x)) {
    case CB_TAG_CALL:
    case CB_TAG_IF:
    case CB_TAG_FUNCALL:
    case CB_TAG_ASSIGN:
        return 1;
    default:
        return 0;

    }
}

#define OUTPUT_REGISTER_ONLY(cmd) if (!output_register_only && (cb_flag_index_optimize || cb_flag_decimal_optimize)) { \
                       int save = output_register_only;\
                       push_output_target(NULL);\
                       output_register_only = 1;\
                       cmd ;\
                       output_register_only = save;\
                       pop_output_target();\
                    }

static void
output_stmt1(cb_tree x, cb_tree x_refcheck) {
    static int cb_list_level = 0;
    struct cb_statement *p;
    struct cb_label     *lp;
    struct cb_assign    *ap;
    struct cb_if        *ip;
    struct cb_field     *f;
#ifdef  COB_NON_ALIGNED
    struct cb_cast      *cp;
#endif
    int                 code;
    int                 idend = 0;
    int                 idelse = 0;
    int                 ent_flag = 0;
    cb_tree             tmp;

    if (x == NULL) {
        output_line(";");
        return;
    }
    if (!cb_flag_gen_gcc) {
        if (inside_check != 0) {
            if (inside_stack[inside_check - 1] != 0) {
                inside_stack[inside_check - 1] = 0;
                output(",\n");
            }
        }
    }
    if (x->source_file) {
        cur_source_line = x->source_line;
        cur_source_file = x->source_file;
    }
    switch (CB_TREE_TAG(x)) {
    case CB_TAG_STATEMENT:
        stack_id = 0;
        p = CB_STATEMENT(x);

        /* Output source location as a comment */
        if (p->name && (cb_flag_mem_info || cb_flag_source_location)) {
            output_line("/* %s:%d: %s */",
                        x->source_file, x->source_line, p->name);
        }
        if (cb_flag_gen_gcc && cb_flag_cobol_lines) {
            output_line("#line %d \"%s\" ", x->source_line, x->source_file);
        }
        ent_flag = enterprise_optimizer_enable_location(p->body);
        if (is_only_pragma(x)) {
            output_stmt(p->body);
        } else {
            /* Output source location as a code */
            if (!cb_flag_binary_optimize || ent_flag ||
                cb_flag_runtimecheck || cb_flag_debuginfo) {
                if (x->source_file &&
                    (x->source_file != last_source ||
                     last_line != x->source_line) &&
                    !suspend_debug_location) {
                    if (cb_flag_source_location &&
                        ((cb_flag_debuginfo) || (x->source_file != last_source))) {
                        output_location(x, (const unsigned char *)p->name, NULL, NULL);
                    } else if (cb_flag_source_location) {
                        if (output_extracting_Code_level) {
                            output_line("module->cur_line = %d;", x->source_line);
                        } else {
                            output_line("loc_module.cur_line = %d;", x->source_line);
                        }
                    } else if (p->body) {
                        if (CB_LABEL_P(p->body) ||
                            (CB_LIST_P(p->body) && CB_LABEL_P(CB_VALUE(p->body)))) {
                            if (output_extracting_Code_level) {
                                output_line("module->cur_line = %d;", x->source_line);
                            } else {
                                output_line("loc_module.cur_line = %d;", x->source_line);
                            }
                        }
                    }
                    last_line = x->source_line;
                    last_source = x->source_file;
                }
            }
            if (p->handler1 || p->handler2 || (p->file && CB_EXCEPTION_ENABLE(COB_EC_I_O)) ||
                (cb_flag_trap_unhandled_exception && (p->handler_id) && CB_EXCEPTION_ENABLE(p->handler_id))) {

                output_line("rtd->cob_exception_code = 0;");
                //output_line ("rtd->exception_handler_defined = 1;");
            }

            if (p->null_check) {
                output_stmt(p->null_check);
            }
            if (p->before_body) {
                output_line("cob_save_exception_code = rtd->cob_exception_code;");
                output_stmt(p->before_body);
                output_line("rtd->cob_exception_code = cob_save_exception_code;");
            }
            if (p->body) {
                output_ccmap(last_source, last_line, 'S');
                output_stmt1(p->body, p->ref_check);
            }
            if (p->after_body) {
                output_line("cob_save_exception_code = rtd->cob_exception_code;");
                output_stmt(p->after_body);
                output_line("rtd->cob_exception_code = cob_save_exception_code;");
            }

            if (p->handler1 || p->handler2 || (p->file && CB_EXCEPTION_ENABLE(COB_EC_I_O)) ||
                ((cb_flag_trap_unhandled_exception) &&
                 (p->handler_id) && CB_EXCEPTION_ENABLE(p->handler_id))) {
                //output_line ("rtd->exception_handler_defined = 0;");
                code = CB_EXCEPTION_CODE(p->handler_id);
                if (p->file) {
                    output_ferror_stmt(p, code);
                } else {
                    if (p->handler1 || p->handler2) {
                        push_io_to_stack();
                        if (p->handler1) {
                            if ((code & 0x00ff) == 0) {
                                output_line("if (unlikely((rtd->cob_exception_code & 0xff00) == 0x%04x))",
                                            code);
                            } else {
                                output_line("if (unlikely(rtd->cob_exception_code == 0x%04x))", code);
                            }
                            output_indent("{");
                            output_ccmap(last_source, last_line, 'E');
                            output_stmt(p->handler1);

                            copy_stack_to_io();


                            output_indent("}");
                            if (p->handler2) {
                                output_line("else");
                            }
                        }
                        if (p->handler2) {
                            if (p->handler1 == NULL) {
                                output_line("if (!rtd->cob_exception_code)");
                            }
                            output_indent("{");
                            output_ccmap(last_source, last_line, 'E');
                            output_stmt(p->handler2);
                            copy_stack_to_io();
                            output_indent("}");
                        }
                        pop_stack_to_io();
                    } else {
                        if ((code & 0x00ff) == 0) {
                            output_line("if (unlikely((rtd->cob_exception_code & 0xff00) == 0x%04x))",
                                        code);
                        } else {
                            output_line("if (unlikely(rtd->cob_exception_code == 0x%04x))", code);
                        }
                        output_line(" cob_fatal_error (rtd, COB_FERROR_UNCATCH_EXP);");
                    }
                }
            }
        }
        if (p->final) {
            output_line("cob_save_exception_code = rtd->cob_exception_code;");
            output_stmt(p->final);
            output_line("rtd->cob_exception_code = cob_save_exception_code;");
        }
        if (p->body) update_wipe_flag_and_target(x, p->body);
        if (p->before_body) update_wipe_flag_and_target(x, p->before_body);
        if (p->after_body) update_wipe_flag_and_target(x, p->after_body);
        if (p->handler1) update_wipe_flag_and_target(x, p->handler1);
        if (p->handler2) update_wipe_flag_and_target(x, p->handler2);
        if (p->handler3) update_wipe_flag_and_target(x, p->handler3);
        if (p->final) update_wipe_flag_and_target(x, p->final);
        break;
    case CB_TAG_LABEL:
        if (x_refcheck) output_stmt(x_refcheck);

        stack_id = 0;
        lp = CB_LABEL(x);
        output_newline();
        if (lp->is_section) {
            if (strcmp((const char *)(lp->name), "MAIN SECTION")) {
                output_line("/* %s SECTION */", lp->orig_name);
            } else {
                output_line("/* %s */", lp->orig_name);
            }
            excp_current_section = (const char *)lp->orig_name;
            excp_current_paragraph = NULL;
        } else {
            if (lp->is_entry) {
                output_line("/* Entry %s */", lp->orig_name);
            } else {
                output_line("/* %s */", lp->orig_name);
            }
            excp_current_paragraph = (const char *)lp->orig_name;
            if (!lp->need_begin) {
                output_newline();
            }
        }
        if (lp->need_begin) {
            output_newline();
            output_line("%s%d:;", CB_PREFIX_LABEL, lp->id);
            output_meminfo_location(x);
        }
        if (cb_flag_trace || cb_flag_trace_ts || cb_flag_simple_trace || cb_flag_ready_trace) {
            char Buffer1[20];
            char Buffer2[20];

            if (cb_flag_trace || cb_flag_trace_ts || cb_flag_ready_trace) {
                if (cb_flag_ready_trace) {
                    output_line("if (cob_get_section_trace(rtd))");
                    output_indent("{");
                }
                if (cb_flag_trace_ts) {
                    output_trace(1, 3,  "%s,cob_intr_current_date(rtd, 0, 0), %s", 
                                 CString_param(Buffer1, "["), CString_param(Buffer2,"]") );
                }
                if (lp->is_section) {
                    if (strcmp((const char *)(lp->name), "MAIN SECTION")) {
                        output_trace(1, 1, CString_param(Buffer1, (char*)"PROGRAM-ID: %s: %s SECTION", excp_current_source_name, lp->orig_name));
                    } else {
                        output_trace(1, 1, CString_param(Buffer1, (char*)"PROGRAM-ID: %s: %s", excp_current_source_name, lp->orig_name));
                    }
                } else if (lp->is_entry) {
                    output_trace(1, 1, CString_param(Buffer1, (char*)"PROGRAM-ID: %s: ENTRY %s", excp_current_source_name, lp->orig_name));
                } else {
                    output_trace(1, 1, CString_param(Buffer1, (char*)"PROGRAM-ID: %s: %s", excp_current_source_name, lp->orig_name));
                }
                if (cb_flag_ready_trace) {
                    output_indent("}");
                }
            } else if (strncmp("L$", (const char *)lp->name, 2) != 0) {
                output_trace(1, 1, CString_param(Buffer1, (char*)lp->orig_name));
            }
        }
        if (lp->is_exception) {
            output_debug_callback(COB_DBCALLBACK_ENTER_EXCEPTION);
        }
        if (lp->is_section) {
            output_location(x, NULL, lp->orig_name, NULL);
        } else {
            output_location(x, NULL,  NULL, lp->orig_name);
        }
        last_line = x->source_line;
        last_source = x->source_file;
        output_ccmap(cur_source_file, cur_source_line, 'L');

        if (cb_flag_profiling && (strncmp("L$", (const char *)lp->name, 2) != 0)) {
            cur_profiling_idx++;
            if (excp_current_section) {
                cob_enterprise_output_profiling("\t{\"%s: %s.%s\", 0, 0, 0, 0, 0},\n", excp_current_program_id, excp_current_section, lp->name);
            } else {
                cob_enterprise_output_profiling("\t{\"%s: %s\", 0, 0, 0, 0, 0},\n", excp_current_program_id, lp->name);
            }
            output_line("%s_%s[%d].entry_cnt ++;", current_codegen_prog->program_id, PROFILING_SUFFIX, cur_profiling_idx);
        }
        output_alter_entry(lp);
        break;
    case CB_TAG_PRAGMA:
        if (x_refcheck) output_stmt(x_refcheck);
        output_pragma(CB_PRAGMA(x));
        break;
    case CB_TAG_INLINE:
        if (x_refcheck) output_stmt(x_refcheck);
        output_line(CB_INLINE(x)->line);
        break;
    case CB_TAG_FUNCALL:
        if (x_refcheck) output_stmt(x_refcheck);
        {
            if (!output_register_only && set_up_decimal_list(x)) {
#ifdef DEBUG
                output("/*removed %s */",  CB_FUNCALL(x)->name);
#endif
                break;
            }
            OUTPUT_REGISTER_ONLY(output_funcall(x));

            output_prefix();
            output_funcall(x);
            if (cb_flag_gen_gcc) {
                output(";\n");
            } else {
                if (inside_check == 0) {
                    output(";\n");
                } else {
                    inside_stack[inside_check - 1] = 1;
                }
            }
        }
        break;
    case CB_TAG_ASSIGN:
        if (x_refcheck) output_stmt(x_refcheck);
        set_up_decimal_list(x);
        ap = CB_ASSIGN(x);
        output_test(ap->var);
        output_test(ap->val);
#ifdef  COB_NON_ALIGNED
        /* Nonaligned */
        if (CB_TREE_CLASS(ap->var) == CB_CLASS_POINTER
            || CB_TREE_CLASS(ap->val) == CB_CLASS_POINTER) {
            /* Pointer assignment */
            output_indent("{");
            output_line("void *temp_ptr;");

            /* temp_ptr = source address; */
            output_prefix();
            if (ap->val == cb_null || ap->val == cb_zero) {
                /* MOVE NULL ... */
                output("temp_ptr = 0;\n");
            } else if (CB_TREE_TAG(ap->val) == CB_TAG_CAST) {
                /* MOVE ADDRESS OF val ... */
                cp = CB_CAST(ap->val);
                output("temp_ptr = ");
                switch (cp->type) {
                case CB_CAST_ADDRESS:
                    output_data(cp->val);
                    break;
                case CB_CAST_PROGRAM_POINTER:
                    output_func_1("cob_call_resolve_2", ap->val);
                    break;
                default:
                    fprintf(stderr, "cobc:0: Unexpected cast type %d\n", cp->type);
                    ABORT();
                }
                output(";\n");
            } else {
                /* MOVE val ... */
                output("memcpy(&temp_ptr, ");
                output_data(ap->val);
                output(", sizeof(temp_ptr));\n");
            }

            /* destination address = temp_ptr; */
            output_prefix();
            if (CB_TREE_TAG(ap->var) == CB_TAG_CAST) {
                /* SET ADDRESS OF var ... */
                cp = CB_CAST(ap->var);
                if (cp->type != CB_CAST_ADDRESS) {
                    fprintf(stderr, "cobc:0: Unexpected tree type %d\n", cp->type);
                    ABORT();
                }
                output_data(cp->val);
                output(" = temp_ptr;\n");
            } else {
                /* MOVE ... TO var */
                output("memcpy(");
                output_data(ap->var);
                output(", &temp_ptr, sizeof(temp_ptr));\n");
            }

            output_indent("}");
        } else {
            /* Numeric assignment */
            output_prefix();
            output_integer(ap->var);
            output(" = ");
            output_integer(ap->val);
            if (cb_flag_gen_gcc) {
                output(";\n");
            } else {
                if (inside_check == 0) {
                    output(";\n");
                } else {
                    inside_stack[inside_check - 1] = 1;
                }
            }
        }
#else   /* Nonaligned */
        output_prefix();
        output_integer(ap->var);
        output(" = ");
        output_integer(ap->val);
        if (cb_flag_gen_gcc) {
            output(";\n");
        } else {
            if (inside_check == 0) {
                output(";\n");
            } else {
                inside_stack[inside_check - 1] = 1;
            }
        }
#endif  /* Nonaligned */
        if (ap->common.need_cache_reload && CB_TREE_CLASS(ap->var) == CB_CLASS_POINTER &&
            CB_CAST_P(ap->var) && CB_CAST(ap->var)->type == CB_CAST_ADDRESS) {
            f = cb_field(CB_CAST(ap->var)->val);
            if (f && cb_flag_mem_info && f->storage == CB_STORAGE_LINKAGE) {
                output_line("(COB_MDS %s%d) = %s%d;", CB_PREFIX_LINKAGE, f->id, CB_PREFIX_BASE, f->id);
                output_line("cob_runtime_debugger_clear_field_cache(rtd);");
            }

        }
        break;
    case CB_TAG_INITIALIZE:
        if (x_refcheck) output_stmt(x_refcheck);
        cb_generating_initialize = 1;
        output_initialize(CB_INITIALIZE(x));
        cb_generating_initialize = 0;
        break;
    case CB_TAG_FIXVALUE:
        if (x_refcheck) output_stmt(x_refcheck);
        output_fixvalue(CB_FIXVALUE(x));
        break;
    case CB_TAG_SEARCH:
        {
            struct cb_search *p = CB_SEARCH(x);
            output_search(p, x_refcheck);
            if (p->whens) update_wipe_flag_and_target(x, p->whens);
            if (p->end_stmt) update_wipe_flag_and_target(x, p->end_stmt);
        }


        break;
    case CB_TAG_CALL:
        if (x_refcheck) output_stmt(x_refcheck);
        OUTPUT_REGISTER_ONLY(output_call(CB_CALL(x)));
        output_call(CB_CALL(x));
        break;
    case CB_TAG_GOTO:
        if (x_refcheck) output_stmt(x_refcheck);
        output_goto(CB_GOTO(x));
        break;
    case CB_TAG_IF:
        if (x_refcheck) output_stmt(x_refcheck);
        cb_if_level++;
        ip = CB_IF(x);
        if (cb_if_level > 10) {
            ip->unnest = 1;
        }
        OUTPUT_REGISTER_ONLY(output_cond(ip->test, 0, 0));
        output_prefix();
        if (ip->unnest) {
            output("if (!(");
            push_io_to_stack();
            output_cond(ip->test, 0, 0);
            output("))\n");
            idelse = cb_id++;
            idend  = cb_id++;
            output_line("goto %s%d;", CB_PREFIX_LABEL, idelse);

            if (ip->stmt1) {
                output_ccmap(cur_source_file, cur_source_line, 'C');
                output_stmt(ip->stmt1);
                copy_stack_to_io();
            }
            if (ip->stmt2) {
                output_line("goto %s%d;", CB_PREFIX_LABEL, idend);
            }
            output_line("%s%d:;", CB_PREFIX_LABEL, idelse);
            if (ip->stmt2) {
                output_ccmap(cur_source_file, cur_source_line, 'C');
                //              push_io_to_stack();
                output_stmt(ip->stmt2);
                copy_stack_to_io();
                output_line("%s%d:;", CB_PREFIX_LABEL, idend);
            }
            pop_stack_to_io();


        } else {
            output("if (");
            push_io_to_stack();
            output_cond(ip->test, 0, 0);
            output(")\n");

            if (ip->stmt1) {
                output_line("{");
                output_indent_level += 2;
                output_ccmap(cur_source_file, cur_source_line, 'C');
                output_stmt(ip->stmt1);
                copy_stack_to_io();
                output_indent_level -= 2;
                output_line("}");
            } else {
                output_line("  /* nothing */;");
            }
            if (ip->stmt2) {
                output_line("else");
                output_line("{");
                output_indent_level += 2;
                output_ccmap(cur_source_file, cur_source_line, 'C');
                output_stmt(ip->stmt2);
                copy_stack_to_io();
                output_indent_level -= 2;
                output_line("}");
            }
            pop_stack_to_io();
        }
        cb_if_level--;
//      if (ip->test)  update_wipe_flag_and_target(x, ip->test);
//      if (ip->stmt1) update_wipe_flag_and_target(x, ip->stmt1);
//      if (ip->stmt2) update_wipe_flag_and_target(x, ip->stmt2);

        break;
    case CB_TAG_PERFORM:
        output_perform(CB_PERFORM(x), x_refcheck);
        output_meminfo_location(NULL);
        break;
    case CB_TAG_CONTINUE:
        if (x_refcheck) output_stmt(x_refcheck);
        output_prefix();
        output(";\n");
        break;
    case CB_TAG_READY:
        if (x_refcheck) output_stmt(x_refcheck);
        if (cb_flag_ready_trace) {
            output_line("cob_ready_section_trace(rtd);");
        }
        break;
    case CB_TAG_RESET:
        if (x_refcheck) output_stmt(x_refcheck);
        if (cb_flag_ready_trace) {
            output_line("cob_reset_section_trace(rtd);");
        }
        break;
    case CB_TAG_LIST:
        save_stack_id = stack_id;
        if ((cb_list_level == 0) && cb_flag_gcc_bug) {
            output_indent("{");
        }
        cb_list_level++;
        tmp = x;
        for (; tmp; tmp = CB_CHAIN(tmp)) {
            cb_tree v = CB_VALUE(tmp);
            output_stmt1(v, x_refcheck);
            x_refcheck = NULL;
            update_wipe_flag_and_target(x, v);
        }
        cb_list_level--;
        if ((cb_list_level == 0) && cb_flag_gcc_bug) {
            output_indent("}");
        }
        if (stack_id) {
            stack_id = save_stack_id;
        }
        break;
    default:
        fprintf(stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG(x));
        ABORT();
    }
    check_target_ref_list(x);
}

static void
output_stmt(cb_tree x) {
    output_stmt1(x, NULL);
}



/*
 * File definition
 */

static int
output_file_allocation(struct cb_program *prog,  struct cb_file *f) {

    struct cb_alt_key *l;
    int  nkeys = 0;

    if (f->organization == COB_ORG_RELATIVE
        || f->organization == COB_ORG_INDEXED) {
        nkeys = 1;
        for (l = f->alt_key_list; l; l = l->next) {
            nkeys++;
        }
    }
    if (f->global) {
        output_storage("/* Global FCD file %s */\n", f->name);
    } else {
        output_local("/* FCD File %s */\n", f->name);
    }
    /* Output RELATIVE/RECORD KEY's */
    if (f->global) {
        push_output_target(cb_global_file);
        if (f->external) {
            output("#define %s%s (COB_MDS i%s%s)\n",
                   CB_PREFIX_FILE, f->cname,
                   CB_PREFIX_FILE, f->cname);
        } else {

            output("#define %s%s (&(COB_MDS i%s%s))\n",
                   CB_PREFIX_FILE, f->cname,
                   CB_PREFIX_FILE, f->cname);
            if (f->linage) {
                output("#define %s%s_linage (&(COB_MDS i%s%s_linage))\n",
                       CB_PREFIX_FILE, f->cname,
                       CB_PREFIX_FILE, f->cname);
            }
        }

    } else {
        push_output_target(current_codegen_prog->static_storage_file);
        output_local("#undef %s%s \n", CB_PREFIX_FILE, f->cname);
        if (f->external) {
            output_local("#define %s%s (COB_MDS %s%s.i%s%s)\n",
                         CB_PREFIX_FILE, f->cname,
                         CB_PREFIX_STATIC, prog->program_id,
                         CB_PREFIX_FILE, f->cname);
        } else {

            output_local("#define %s%s (&(COB_MDS %s%s.i%s%s))\n",
                         CB_PREFIX_FILE, f->cname,
                         CB_PREFIX_STATIC, prog->program_id,
                         CB_PREFIX_FILE, f->cname);
            if (f->linage) {
                output_local("#define %s%s_linage (&(COB_MDS %s%s.i%s%s_linage))\n",
                             CB_PREFIX_FILE, f->cname,
                             CB_PREFIX_STATIC, prog->program_id,
                             CB_PREFIX_FILE, f->cname);
            }
        }
    }
    /*key size in total*/
    if (nkeys) {
        int s = sizeof(mf_extfh_global_info) + (nkeys * sizeof(mf_extfh_key_def)) +
            (nkeys * sizeof(mf_extfh_comp_def) * COB_MAX_KEY_PART);
        output_local("cit_uint8\t\t%s%s_key[%d];\n", CB_PREFIX_FILE, f->cname, s);
    }
    /*Declaration in c.s.h*/
    if (f->external) {
        output("cob_file_extfh\t\t*i%s%s;\n",
               CB_PREFIX_FILE, f->cname);

    } else {
        output("cob_file_extfh\t\ti%s%s;\n",
               CB_PREFIX_FILE, f->cname);
        if (f->linage) {
            output("struct linage_struct\t\ti%s%s_linage;\n",
                   CB_PREFIX_FILE, f->cname);
        }

    }

    pop_output_target();

    if (f->linage) {
        return (1);
    }
    return (0);
}

/*
static void 
output_file_build_field_map (struct cb_file *f, struct cb_field *l, int *idx)
{
    struct cb_field *c;

    if (l->children) {
        for (c = l->children; c; c= c->sister) {
            if (!c->flag_item_78 && !c->redefines) {
                output_file_build_field_map (f, c, idx);
            }
        }
    } else {
        switch (l->usage) {
            case CB_USAGE_DISPLAY:
                for (i=0; i< l->size; i++; (*idx)++) {
                    map_set (f->record_map, *idx);
                }
                break;
            default:
                map_set (f->record_map, *idx);
                (*idx)+= l->size;
                break;

        }
    }



}


static void 
output_file_record_map (struct cb_file *f)
{
    struct cb_field *l;
    int idx = 0;

    f->record_map =  cob_malloc((f->record->size /8) +1 );
    output_file_build_field_map (f, f->record_map, &idx)
}
*/
//static void
//output_key_field_list_initialization(struct cb_file *f, cb_tree x) {
//
//    cb_tree l;
//    int sz = 0;
//    int ct = 0;
//    int i;
//    struct cb_object_list *o;
//
//    o = CB_OBJECT_LIST(x);
//    l = o->olist;
//    for (; l; l = CB_CHAIN(l)) {
//        if (CB_REF_OR_FIELD_P(CB_VALUE(l))) {
//            struct cb_field *f = cb_field(CB_VALUE(l));
//            ct++;
//            sz += f->size;
//        }
//    }
//    output_line("%s%s.size = %d;",
//                CB_PREFIX_FIELDLIST, o->cname, sz);
//    output_line("%s%s.data = b%s%s;",
//                CB_PREFIX_FIELDLIST, o->cname,
//                CB_PREFIX_FIELDLIST, o->cname);
//    output_line("%s%s.attr = &a_%s%s;",
//                CB_PREFIX_FIELDLIST, o->cname,
//                CB_PREFIX_FIELDLIST, o->cname);
//    output_line("%s%s.attr->type = COB_TYPE_FIELD_LIST;",
//                CB_PREFIX_FIELDLIST, o->cname);
//    output_line("%s%s.attr->fcount = %d;",
//                CB_PREFIX_FIELDLIST, o->cname, ct);
//    output_line("%s%s.attr->part = t_%s%s;",
//                CB_PREFIX_FIELDLIST, o->cname,
//                CB_PREFIX_FIELDLIST, o->cname);
//
//    i = 0;
//    l = o->olist;
//    for (; l; l = CB_CHAIN(l)) {
//        if (CB_REF_OR_FIELD_P(CB_VALUE(l))) {
//            struct cb_field *fl = cb_field(CB_VALUE(l));
//            output_line("%s%s.attr->part[%d].start = %d;",
//                        CB_PREFIX_FIELDLIST, o->cname, i, fl->offset);
//            output_line("%s%s.attr->part[%d].leng = %d;",
//                        CB_PREFIX_FIELDLIST, o->cname, i, fl->size);
//            output_prefix();
//            output("%s%s.attr->part[%d].field = ",
//                   CB_PREFIX_FIELDLIST, o->cname, i);
//            output_param(CB_VALUE(l), -1);
//            output(";\n");
//            i++;
//        }
//    }
//}

static int
is_extenal(struct cb_field       *f) {
    if (f->parent) {
        return is_extenal(f->parent);
    } else if (f->flag_external) {
        return 1;
    } else if (f->redefines && f->redefines->flag_external) {
        return 1;
    }
    return 0;
}

static void
output_initextern_one(cb_tree x, struct cb_field *f) {
    if (!f->flag_init_gen &&
        (f->flag_item_external  || (cb_flag_mem_info && is_extenal(f)))) {
        if (x) {
            output_prefix();
            output("%s%d.data = ", CB_PREFIX_FIELD, f->id);
            output_data(x);
            output(";\n");
        }
        f->flag_init_gen = 1;
    }
}

static void
output_initextern(struct cb_program *prog) {
    struct field_list       *k;
    if (has_external) {
        output_newline();
        output_line("/* EXTERNAL data initialization */");
        output_line("L_initextern: ;");
        for (k = field_cache; k; k = k->next) {
            output_initextern_one(k->x, k->f);
        }
        output_line("\tgoto LRET_initextern;");
    }
}

static int get_comp_count_fcd(cb_tree x) {
    int comp_count = 0;
    /*finding comp count*/
    if (CB_OBJECT_LIST_P(x) && (CB_OBJECT_LIST(x)->type == CB_OBJECT_LIST_FIELD)) {
        struct cb_object_list *o;
        cb_tree l;
        o = CB_OBJECT_LIST(x);
        l = o->olist;
        for (; l; l = CB_CHAIN(l)) {
            if (CB_REF_OR_FIELD_P(CB_VALUE(l))) {
                comp_count++;
            }
        }

    } else {
        comp_count = 1;
    }
    return comp_count;
}
static void output_key_component_fcd(cb_tree key, int nkeys, int *ci, char *fname) {
//    char   fname[1024];
    int sz = 0;
//    sprintf (fname, "%s%s", CB_PREFIX_FILE, f->cname);
    if (CB_OBJECT_LIST_P(key) && (CB_OBJECT_LIST(key)->type == CB_OBJECT_LIST_FIELD)) {
        struct cb_object_list *o;
        cb_tree l;
        o = CB_OBJECT_LIST(key);
        l = o->olist;
        for (; l; l = CB_CHAIN(l)) {
            if (CB_REF_OR_FIELD_P(CB_VALUE(l))) {
                struct cb_field *fl = cb_field(CB_VALUE(l));
                output_prefix();
                output("cob_define_key_component_fcd(fcd_ptr, %d, %d, %d, ",
                       nkeys, *ci, fl->offset);
                output_param(CB_TREE(fl), -1);
                output(");\n");
                sz += fl->size;
                (*ci)++;
            }
        }

    } else {
        output_prefix();
        output("cob_define_key_component_fcd(fcd_ptr, %d, %d, %d, ",
               nkeys, *ci, cb_field(key)->offset);
        output_param(key, -1);
        output(");\n");
        (*ci)++;
    }

}

static void
output_file_initialization(struct cb_file *f, int external_use_assign) {
    char   fname[1024];
    int             nkeys = 0;
    int             val = 0;
    int fcd_len, xdd_len, key_len;
    struct cb_alt_key   *l;

    int comp_offset = 0;
    int s;
    int ci;
    int flag;
    int compression_flag = 0;
    int id_key = 0;
    struct cb_field     *fb;

    sprintf(fname, "%s%s", CB_PREFIX_FILE, f->cname);
    if (f->organization == COB_ORG_RELATIVE
        || f->organization == COB_ORG_INDEXED) {
        nkeys = 1;
        for (l = f->alt_key_list; l; l = l->next) {
            nkeys++;
        }
    }
    fcd_len = sizeof(mf_extfh_FCD);
    xdd_len = (f->organization == COB_ORG_INDEXED && cb_flag_use_ctree) ?  sizeof(ctree_extfh_FCD_extention) : 0;
    key_len = sizeof(mf_extfh_global_info) + (nkeys * sizeof(mf_extfh_key_def)) +
        (nkeys * sizeof(mf_extfh_comp_def) * COB_MAX_KEY_PART);

    if (f->external) {
        if (external_use_assign && f->assign) {
            output("%s = (cob_file_extfh *)cob_external_addr_from_fld (rtd,", fname);
            output_param(f->assign, -1);
            output_line(", sizeof(cob_file_extfh));");
        } else {
            output_line("%s = (cob_file_extfh *)cob_external_addr (rtd, \"%s\", sizeof(cob_file_extfh));",
                        fname, f->cname);
        }
        output_line("if (rtd->cob_initial_external)");
        output_indent("{");

        if (f->linage) {
            output_line("%s->linorkeyptr = cob_external_addr (rtd, \"%s_linage\", sizeof(struct linage_struct));", fname, fname);
        }
    } else {
        if (f->linage) {
            output_line("%s->linorkeyptr = %s_linage;", fname, fname);
        }
    }
    output_line("%s->select_name = (const char *)\"%s\";", fname, f->oname);
//  if (f->external && !f->file_status) {
//      output_line("%s->file_status = cob_external_addr (rtd, \"%s_status\", 4);",
//                  fname, fname);
//  } else {
//      output_line("%s->file_status = %s->default_status;", fname, fname);
//      output_line("memset (%s->default_status, %d, 2);", fname, (int)CHAR_0);
//  }

    /*to decide if keep it in mini cob_file, can do it at typeck level*/
    output_prefix();
    output("%s->record_size = ", fname);
    if (f->record_depending) {
        output_param(f->record_depending, -1);
    } else {
        output("NULL");
    }
    output(";\n");
    /**/
    output_line("%s->file = NULL;", fname);

    output_line("%s->flag_optional = %d;",   fname, f->optional & COB_FILE_OPTIONAL_MASK);
    output_line("%s->optional_in_source = %d;",   fname, f->optional & COB_FILE_OPTIONAL_IN_SOURCE ? 1 : 0);
    output_line("%s->last_open_mode = %d;",  fname, COB_OPEN_CLOSED);



    output_line("%s->flag_select_features = 0x%x;", fname,
                ((f->file_status ? COB_SELECT_FILE_STATUS : 0) |
                 (f->linage ? COB_SELECT_LINAGE : 0)));

    output_line("%s->flag_needs_top = 0;",   fname);
    output_line("%s->file_version = %d;",    fname, COB_FILE_VERSION);


    /*FCD INITIALIZATION*/
    output_line("%s->extfh_ptr = &(%s->FCD);", fname, fname);
    output_line("fcd_ptr = (mf_extfh_FCD *) %s->extfh_ptr;", fname);

    /*DEVICE FLAG*/
    if (f->special) {
        if (f->special == 1) {
            output_line("CP_FLD1(fcd_ptr->device_flag, FCD_DEVICE_STDIN);");
        } else {
            output_line("CP_FLD1(fcd_ptr->device_flag, FCD_DEVICE_STDOUT);");
        }
    }

    if (f->debug_file_status) {
        output_prefix();
        output("%s%d = ", CB_PREFIX_BASE, f->debug_file_status->id);
        output("fcd_ptr->user_file_status;\n");
    }
    output_line("CP_FLD2(fcd_ptr->fcd_len, %d);", (fcd_len + xdd_len));

#ifdef COB_PTR_64BITS
    output_line("CP_FLD1(fcd_ptr->version, 1);");
#else
    output_line("CP_FLD1(fcd_ptr->version, 0);");
#endif
    /*ASSIGN NAME*/
    output_prefix();
    output("cob_file_set_fcd_file_name (rtd, %s, ", fname);
    if (f->special) {
        output("NULL");
    } else {
        output_param(f->assign, -1);
    }
    output(");\n");

    output_line("CP_FLDP(fcd_ptr->key_def_block_ptr, 0);");
    /*RECORD*/
    if (f->record) {
        output_prefix();
        output("CP_FLDP(fcd_ptr->rec_data_ptr");
        output(", (");
        output_param(CB_TREE(f->record), -1);
        output(")->data);\n");

        output_prefix();
        output("CP_FLD2_4(fcd_ptr->cur_rec_length");
        output(", (");
        output_param(CB_TREE(f->record), -1);
        output(")->size);\n");
    }


    /*MAX REC LENGTH*/
    output_line("CP_FLD2_4(fcd_ptr->max_rec_length, %d);", f->record_max);
    /*MIN REC LENGTH*/
    output_line("CP_FLD2_4(fcd_ptr->min_rec_length, %d);", f->record_min);

    /*  KEY */

    output_prefix();
    output("%s->relative_rec_number = ",  fname);
    if (f->organization == COB_ORG_RELATIVE && f->key) {
        output_param(f->key, -1);
    } else {
        output("NULL");
    }
    output(";\n");

    if (f->organization == COB_ORG_INDEXED && nkeys) {
        output_line("cob_define_global_info_fcd(rtd, fcd_ptr, %d, %s%s_key);", nkeys, CB_PREFIX_FILE, f->cname);
        s = sizeof(mf_extfh_global_info) + (nkeys * sizeof(mf_extfh_key_def));
        ci = 0;
        /*PRIMARY KEY*/
        if (f->key) {

            comp_offset = s + (ci * sizeof(mf_extfh_comp_def));
            flag =  (f->key_duplicates ? 1 : 0) + (f->prim_key_compress << 8);
            if ((COB_FILE_VERSION > 0) && (flag & 0xF0)) {
                compression_flag =   (flag & 0xF0) >> 8;
            }

            output_line("cob_define_key_def_fcd(fcd_ptr, %d,  %d, %d, %d, %d);",
                        id_key, get_comp_count_fcd(f->key), comp_offset, (flag & 0x0F ? 1 << 6 : 0), compression_flag);

            output_key_component_fcd(f->key, nkeys, &ci, fname);
        }
        /*ALTERNATE KEYS*/
        id_key = 1;
        for (l = f->alt_key_list; l; l = l->next) {
            comp_offset = s + (ci * sizeof(mf_extfh_comp_def));
            flag = (l->duplicates ? 1 : 0) + (l->key_compress << 8);
            if ((COB_FILE_VERSION > 0) && (flag & 0xF0)) {
                compression_flag =   (flag & 0xF0) >> 8;
            }
            output_line("cob_define_key_def_fcd(fcd_ptr, %d,  %d, %d, %d, %d);",
                        id_key, get_comp_count_fcd(l->key), comp_offset, (flag & 0x0F ? 1 << 6 : 0), compression_flag);

            output_key_component_fcd(l->key, nkeys, &ci, fname);
            id_key++;
        }

    }

    /*LINAGE*/
    if (f->linage) {
        output_line("lingptr = (struct linage_struct *)(%s->linorkeyptr);", fname);
        output_prefix();
        output("lingptr->linage = ");
        output_param(f->linage, -1);
        output(";\n");
        output_prefix();
        output("lingptr->linage_ctr = ");
        output_param(f->linage_ctr, -1);
        output(";\n");
        if (f->latfoot) {
            output_prefix();
            output("lingptr->latfoot = ");
            output_param(f->latfoot, -1);
            output(";\n");
        } else {
            output_line("lingptr->latfoot = NULL;");
        }
        if (f->lattop) {
            output_prefix();
            output("lingptr->lattop = ");
            output_param(f->lattop, -1);
            output(";\n");
        } else {
            output_line("lingptr->lattop = NULL;");
        }
        if (f->latbot) {
            output_prefix();
            output("lingptr->latbot = ");
            output_param(f->latbot, -1);
            output(";\n");
        } else {
            output_line("lingptr->latbot = NULL;");
        }
        output_line("lingptr->lin_lines = 0;");
        output_line("lingptr->lin_foot = 0;");
        output_line("lingptr->lin_top = 0;");
        output_line("lingptr->lin_bot = 0;");
    }
    /*ORGANISATION*/
    switch (f->organization) {
    case COB_ORG_LINE_SEQUENTIAL:
        output_line("CP_FLD1(fcd_ptr->organization, FCD_ORG_LINE_SEQUENTIAL);"); break;
    case COB_ORG_SEQUENTIAL     :
        output_line("CP_FLD1(fcd_ptr->organization, FCD_ORG_SEQUENTIAL);"); break;
    case COB_ORG_INDEXED        :
        output_line("CP_FLD1(fcd_ptr->organization, FCD_ORG_INDEXED);"); break;
    case COB_ORG_RELATIVE       :
        output_line("CP_FLD1(fcd_ptr->organization, FCD_ORG_RELATIVE);"); break;
    default                     :
        output_line("CP_FLD1(fcd_ptr->organization, FCD_ORG_SEQUENTIAL);"); break;
    }
    /*ACCESS MODE*/
    switch (f->access_mode) {
    case COB_ACCESS_SEQUENTIAL:
        output_line("CP_FLD1(fcd_ptr->access_mode, FCD_ACCESS_SEQUENTIAL);"); break;
    case COB_ACCESS_RANDOM    :
        output_line("CP_FLD1(fcd_ptr->access_mode, FCD_ACCESS_RANDOM);"); break;
    case COB_ACCESS_DYNAMIC   :
        output_line("CP_FLD1(fcd_ptr->access_mode, FCD_ACCESS_DYNAMIC);"); break;
    default                   :
        output_line("CP_FLD1(fcd_ptr->access_mode, FCD_ACCESS_SEQUENTIAL);"); break;
    }
    /*LOCK MODE*/
    switch (f->lock_mode) {
    case COB_LOCK_EXCLUSIVE   :
        output_line("CP_FLD1(fcd_ptr->lock_mode, FCD_LOCK_EXCLUSIVE);"); break;
    case COB_LOCK_MANUAL      :
        output_line("CP_FLD1(fcd_ptr->lock_mode, FCD_LOCK_MANUAL);"); break;
    case COB_LOCK_AUTOMATIC   :
        output_line("CP_FLD1(fcd_ptr->lock_mode, FCD_LOCK_AUTOMATIC);"); break;
    case COB_LOCK_MULTIPLE    :
        output_line("CP_FLD1(fcd_ptr->lock_mode, FCD_LOCK_MULTIPLE);"); break;
    }


    /*OPEN MODE*/
    output_line("CP_FLD1(fcd_ptr->open_mode, FCD_OPEN_CLOSED);");

    if (f->with_rollback) {
        output_line("SETB_ON_FLD1(fcd_ptr->FCD_FILE_SHARE_FLAG_FLAG_WITH_ROLLBACK);");
    }
    if (f->auto_close) {
        output_line("%s->auto_close = %d;", fname, f->auto_close);
    }

    if ((f->external_assign && cb_external_mapping) || cb_filename_mapping) {
        output_line("SETB_ON_FLD1(fcd_ptr->FCD_OTHER_FLAGS_BIT_SELECT_EXTERNAL);");
    }

    /*CIT*/
    if (cb_flag_line_seq_dos) val |= 1 << 3;
//  if (cb_flag_line_seq_unix)
//      val |= 1 << 4;
    if (f->is_printer) output_line("SETB_ON_FLD1(fcd_ptr->FCD_OTHER_FLAGS_BIT_LineAdv);");
    if (f->organization == COB_ORG_LINE_SEQUENTIAL) {
        if (f->is_printer && cb_flag_printer_need_crlf) val |= 1 << 3;
        if (f->ls_mfmode) val |= 1 << 1;
        if (f->ls_notrunc) output_line("SETB_OFF_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_MODE_TRUNC);");
        else if (!f->ls_notrunc) output_line("SETB_ON_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_MODE_TRUNC);");
        if (f->ls_utf16) output_line("SETB_ON_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_UTF16);");
        else output_line("SETB_OFF_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_UTF16);");
        if (cb_flag_utf16_le) output_line("SETB_ON_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_UTF16_LE);");
        else output_line("SETB_OFF_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_UTF16_LE);");
        if (f->ls_mfmode) val |= f->ls_expandtab ? 0 : 1 << 4;
        if (f->rec_mode != CB_REC_MODE_VARIABLE) {
            val |= 1 << 5;
        }
    }

    val |= val << 7; /* status COBOL 85*/
    if (cb_flag_emulate_vms) output_line("SETB_ON_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_EMULATE_VMS);");
    else output_line("SETB_OFF_FLD1(fcd_ptr->FCD_ADV_FLAGS_BIT_EMULATE_VMS);");

    output_line("CP_FLD1(fcd_ptr->status_type, %d);", val);
    output_line("CP_FLD1(fcd_ptr->recording_mode, %d);", (f->rec_mode == CB_REC_MODE_VARIABLE  ? 1 : 0));
    if (cb_flag_mainframe_var_rec) {
        output_line("SETB_ON_FLD1(fcd_ptr->FCD_FLAG_1_BIT_MAINFRAME_FILES);");
    }

/*CobolIT*/
    if (f->organization == COB_ORG_INDEXED) {
        if (cb_isam_extfh) {
            output_line("%s->extfh_func = %s;", fname, cb_isam_extfh);
        } else if (cb_flag_all_extfh && cb_extfh_name) {
            output_line("%s->extfh_func = %s;", fname, cb_extfh_name);
        }
        if (cb_flag_use_ctree) {
            output_prefix();
            output("%s->ctree_xdd = \"", fname);
            cob_enterprise_output_xdd_file(current_output(), f);
            output("\";\n");
            output_line("%s->ctree = 1;", fname);
            output_line("extfh_cob_alloc_file_xdd(rtd, fcd_ptr, %s->ctree_xdd);",
                        fname);
            output_line("%s->extfh_name = \"CTEXTFH\";", fname);
        } else if (cb_isam_extfh || cb_flag_all_extfh) {
/*
            output_line("%s->extfh_ptr = extfh_cob_alloc_file(rtd);", fname);
*/
        } else {
/*
            output_line("%s->extfh_ptr = extfh_cob_alloc_file_auto(rtd, %d);", fname,
                        f->organization == COB_ORG_INDEXED ? 1 : 0);                          
*/
        }
    } else if ((cb_flag_all_extfh) && (f->organization != COB_ORG_SORT)) {
        output_line("%s->extfh_func = %s;", fname, cb_extfh_name);
    } else if (cb_flat_extfh && (f->organization != COB_ORG_INDEXED)) {
        output_line("%s->extfh_func = %s;", fname, cb_flat_extfh);
    } 
    if (f->organization == COB_ORG_SORT && cb_extsm_name && cb_flag_extsm)
    {
       output_line("%s->extsm_name = \"%s\";", fname, cb_extsm_name);

    }

    if (f->data_compress) {
        output_line("CP_FLD1(fcd_ptr->data_compression, %d);", f->data_compress);
    }
    if (cb_flag_initialize_fd && f->record && f->record->sister) {
        cb_tree x = cb_build_field_reference(f->record->sister, NULL);
        CB_REFERENCE(x)->cb_all_occurs = 1;
        output_stmt(cb_build_initialize(x, cb_true, NULL, cb_true, 1, 1, 1, 0));
    }
    if (f->external) {
        output_indent("}");
    }

    if (f->fcd_reg) {
        fb = cb_field(f->fcd_reg);
        if (fb) {
            fb->flag_item_external = 1;
            add_to_fieldcache(f->fcd_reg, fb);
            output_initextern_one(f->fcd_reg, fb);
        }
    }


}

/*
 * Screen definition
 */

static void
output_screen_definition(struct cb_program *prog, struct cb_field *p) {
    int type;

    if (p->sister) {
        output_screen_definition(prog, p->sister);
    }
    if (p->children) {
        output_screen_definition(prog, p->children);
    }

    type = (p->children ? COB_SCREEN_TYPE_GROUP :
            p->values ? COB_SCREEN_TYPE_VALUE :
            (p->size > 0) ? COB_SCREEN_TYPE_FIELD : COB_SCREEN_TYPE_ATTRIBUTE);
    output_line("  cob_screen is_%d;", p->id);
    output_line("  #define s_%d  (COB_MDS %s%s.is_%d)", p->id, CB_PREFIX_SCREEN, prog->program_id, p->id);

    push_output_target(cb_local_function_file);
    output("/* %d */\n",  p->common.source_line);
    if (p->sister) {
        output("s_%d.next = &s_%d; /* %d */\n", p->id,  p->sister->id, p->sister->common.source_line);
    }
    if (type == COB_SCREEN_TYPE_GROUP) {
        output("s_%d.child=  &s_%d; /*%d */\n", p->id, p->children->id, p->children->common.source_line);
    }
    if (type == COB_SCREEN_TYPE_FIELD) {
        cb_tree r;
        p->count++;
        output("s_%d.field= ", p->id);
        r = cb_build_field_reference(p, NULL);
        output_param_static(r, -1);
        output(";\n");
    }
    if (type == COB_SCREEN_TYPE_VALUE) {
        output("s_%d.value= ", p->id);
        output_param_static(CB_VALUE(p->values), p->id);
        output(";\n");
    }
    if (p->screen_line) {
        output("s_%d.line= ", p->id);
        output_param_static(p->screen_line, 0);
        output(";\n");
    }
    if (p->screen_column) {
        output("s_%d.column= ", p->id);
        output_param_static(p->screen_column, 0);
        output(";\n");
    }
    if (p->screen_foreg) {
        output("s_%d.foreg_field= ", p->id);
        output_param_static(p->screen_foreg, 0);
        output(";\n");
    }
    if (p->screen_backg) {
        output("s_%d.backg_field= ", p->id);
        output_param_static(p->screen_backg, 0);
        output(";\n");
    }
    if (type) {
        output("s_%d.type= %d;\n", p->id, type);
    }
    /*
    if ( p->occurs_min ) {
        output ("s_%d.occurs= %d;\n", p->id, p->occurs_min);
    } 
    */
    if (p->screen_flag) {
        output("s_%d.attr= %d;\n",      p->id, p->screen_flag);
        output("s_%d.attr_base= %d;\n", p->id, p->screen_flag);
    }
    if (p->screen_prompt) {
        output("s_%d.prompt= %d;\n", p->id, p->screen_prompt);
    }
    if (p->screen_control) {
        output("s_%d.control= ", p->id);
        output_param_static(p->screen_control, 0);
        output(";\n");
    }
    if (p->screen_input_size) {
        output("s_%d.input_size= ", p->id);
        output_param_static(p->screen_input_size, 0);
        output(";\n");
    }
    pop_output_target();


}

/*
 * Alphabet-name
 */

static int
literal_value(cb_tree x) {
    if (x == cb_space) {
        return (CHAR_SP);
    } else if (x == cb_zero) {
        return (CHAR_0);
    } else if (x == cb_quote) {
        return (A2E(cb_quote_char)); /* '"');*/
    } else if (x == cb_dirsep) {
        return (*(CB_LITERAL(cb_dirsep)->data)); /* '/');*/
    } else if (x == cb_norm_low) {
        return (0);
    } else if (x == cb_norm_high) {
        return (255);
    } else if (x == cb_null) {
        return (0);
    } else if (CB_TREE_CLASS(x) == CB_CLASS_NUMERIC) {
        return (cb_get_int(x) - 1);
    } else {
        return (CB_LITERAL_ENCODED(x)->data[0]);
    }
}

static void
output_alphabet_name_definition(struct cb_alphabet_name *p) {
    cb_tree     l;
    cb_tree     ls;
    cb_tree     x;
    unsigned char   *data;
    int     i;
    int     n = 0;
    int     size;
    int     upper;
    int     lower;
    int     table[256];

    /* Reset to -1 */
    for (i = 0; i < 256; i++) {
        table[i] = -1;
    }

    for (l = p->custom_list; l; l = CB_CHAIN(l)) {
        x = CB_VALUE(l);
        if (CB_PAIR_P(x)) {
            /* X THRU Y */
            lower = literal_value(CB_PAIR_X(x));
            upper = literal_value(CB_PAIR_Y(x));
            if (lower <= upper) {
                for (i = lower; i <= upper; i++) {
                    table[i] = n++;
                }
            } else {
                for (i = upper; i >= lower; i--) {
                    table[i] = n++;
                }
            }
        } else if (CB_LIST_P(x)) {
            /* X ALSO Y ... */
            for (ls = x; ls; ls = CB_CHAIN(ls)) {
                table[literal_value(CB_VALUE(ls))] = n;
            }
            n++;
        } else {
            /* Literal */
            if (CB_TREE_CLASS(x) == CB_CLASS_NUMERIC) {
                table[literal_value(x)] = n++;
            } else if (CB_LITERAL_P(x)) {
                size = (int)CB_LITERAL(x)->size;
                data = CB_LITERAL_ENCODED(x)->data;
                for (i = 0; i < size; i++) {
                    table[data[i]] = n++;
                }
            } else {
                table[literal_value(x)] = n++;
            }
        }
    }

    /* Fill the rest of characters */
    for (i = 0; i < 256; i++) {
        if (table[i] == -1) {
            table[i] = n++;
        }
    }

    /* Output the table */
    output_local("static const unsigned char %s%s[256] = {\n", CB_PREFIX_SEQUENCE, p->cname);
    for (i = 0; i < 256; i++) {
        if (i == 255) {
            output_local(" %d", table[i]);
        } else {
            output_local(" %d,", table[i]);
        }
        if (i % 16 == 15) {
            output_local("\n");
        }
    }
    output_local("};\n");
    i = lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
    output_local("static cob_field f_%s = { 256, (unsigned char *)%s%s, &%s%d };\n",
                 p->cname, CB_PREFIX_SEQUENCE, p->cname, CB_PREFIX_ATTR, i);
    output_local("\n");
}

/*
 * Class definition
 */

static void
output_class_name_definition(struct cb_class_name *p) {
    cb_tree     l;
    cb_tree     x;
    unsigned char   *data;
    size_t      i;
    size_t      size;
    int     lower;
    int     upper;

    output_line("static int");
    output_line("%s (COB_RTD, cob_field *f)", p->cname);
    output_indent("{");
    output_line("int i;");
    output_line("for (i = 0; i < f->size; i++)");
    output_prefix();
    output("  if (!(    ");
    for (l = p->list; l; l = CB_CHAIN(l)) {
        x = CB_VALUE(l);
        if (CB_PAIR_P(x)) {
            lower = literal_value(CB_PAIR_X(x));
            upper = literal_value(CB_PAIR_Y(x));
            if (!lower) {
                output("f->data[i] <= %d", upper);
            } else {
                output("(%d <= f->data[i] && f->data[i] <= %d)", lower, upper);
            }
        } else {
            if (CB_TREE_CLASS(x) == CB_CLASS_NUMERIC) {
                output("f->data[i] == %d", literal_value(x));
            } else if (x == cb_space) {
                output("f->data[i] == %d", CHAR_SP);
            } else if (x == cb_zero) {
                output("f->data[i] == %d", CHAR_0);
            } else if (x == cb_quote) {
                output("f->data[i] == %d", A2E(cb_quote_char));  /* '"'); */
            } else if (x == cb_dirsep) {
                output("f->data[i] == %d", *(CB_LITERAL(cb_dirsep)->data));  /* '"'); */
            } else if ((x == cb_null) || (x == cb_norm_low)) {
                output("f->data[i] == 0");
            } else if (x == cb_norm_high) {
                output("f->data[i] == 0xFF");
            } else {
                size = CB_LITERAL(x)->size;
                data = CB_LITERAL_ENCODED(x)->data;
                for (i = 0; i < size; i++) {
                    output("f->data[i] == %d", data[i]);
                    if (i + 1 < size) {
                        output(" || ");
                    }
                }
            }
        }
        if (CB_CHAIN(l)) {
            output("\n");
            output_prefix();
            output("             || ");
        }
    }
    output(" ))\n");
    output_line("    return 0;");
    output_line("return 1;");
    output_indent("}");
    output_newline();
}

static void
output_initial_values(struct cb_field *p, int external) {
    cb_tree x;
    cb_tree def;
    int stat = 0;

    def = cb_auto_initialize ? cb_true : NULL;
    for (; p; p = p->sister) {
        stat = 0;
        if (!p->flag_external && !cb_flag_keep_unused_var && !p->flag_disable_usused_skip &&
            (p->level == 1 || p->level == 77) &&
            (!p->flag_base)) {
            continue;
        }
        if (p->redefines) {
            continue;
        }
        if (external && !p->flag_external) {
            continue;
        }
        x = cb_build_field_reference(p, NULL);
        CB_REFERENCE(x)->cb_all_occurs = 1;
        if (external && p->flag_external) {
            output_initialize_external(x, p);
            continue;
        }

        if (p->flag_item_based) {
            continue;
        }
        /* For special registers */
        if (p->flag_no_init && !p->count) {
            continue;
        }
        if (!external && p->flag_external) {
            output_prefix();
            output("if ( %s%d_init ) ", CB_PREFIX_BASE, p->id);
            output_indent("{");
            stat = 1;
        }
        stack_id = 0;
        output_stmt(cb_build_initialize(x, cb_true, NULL, def, stat, 1, 1, 0));
        if (!external && p->flag_external) {
            output_indent("}");
        }
    }
}

static void
output_goto_initextern(void) {
    if (has_external) {
        output_line("goto L_initextern;");
        output_line("LRET_initextern: ;");
    }
}

static void
output_sysfile_open(struct cb_file *file, int filenr) {
    output_prefix();
    output("cob_redirect_sysfile(rtd,");
    if (file) {
        output_param(CB_TREE(file), 0);
    } else {
        output("NULL");
    }
    output(", %d);\n", filenr);
}

static void
output_initialize_sequence(struct cb_program *prog) {
    cb_tree          l;
    struct cb_field *f;
    int save = cb_disable_runtime_check;

    if (prog->cb_has_report_section) {
        output_line("cob_c_check_rw();");
    }

    cb_disable_runtime_check = 1;
    for (l = prog->file_list; l; l = CB_CHAIN(l)) {
        f = CB_FILE(CB_VALUE(l))->record;
        if (f &&  f->flag_external && !f->redefines) {
            output_initialize_external(cb_build_field_reference(f, NULL), f);
        }
    }
    output_initial_values(prog->working_storage, 1);
    output_goto_initextern();

    output_initial_values(prog->working_storage, 0);
    if (prog->file_list) {
        output_newline();
        for (l = prog->file_list; l; l = CB_CHAIN(l)) {
            output_file_initialization(CB_FILE(CB_VALUE(l)), 0);
        }
        output_newline();
    }
    cb_disable_runtime_check = save;
}

static void
output_initialize_sequence_proc(struct cb_program *prog) {
    output_newline();
    output_line("/* Working storage data initialization */");
    output_line("L_initworking: ;");
    output_initialize_sequence(prog);
    output_line("goto LRET_initworking;");
}

static void
output_initialize_sequence_call(struct cb_program *prog) {
    output_newline();
    output_line("/* Call Working storage data initialization */");
    output_line("goto L_initworking;");
    output_line("LRET_initworking: ;");
}

/*CIT*/
static void
output_perform_opt_return(struct cb_program *prog) {
    /* resolve all labels */
    struct thru_label_list      *l;
    struct label_list           *r;
    struct label_list           *pl;

    for (l = thru_label_cache; l; l = l->next) {
        output_local("\n");
        output_local("#undef RETURN_%s%d \n", CB_PREFIX_LABEL, l->id);
        output_local("#define RETURN_%s%d {\\\n", CB_PREFIX_LABEL, l->id);
        if (l->label) {
            r = l->label;
            if (r->next) {
                output_local(" switch (frame_ptr->return_address) {\\\n");
                for (pl = r; pl; pl = pl->next) {
                    output_local(" case %d: \\\n", pl->call_num);
                    output_local("   goto %s%d ;\\\n", CB_PREFIX_LABEL, pl->id);
                }
                output_local(" }\\\n");
                output_local(" cob_fatal_error (rtd, COB_FERROR_CODEGEN);\\\n");
            } else {
                output_local(" if (frame_ptr->return_address == %d) {\\\n", r->call_num);
                output_local("   goto %s%d ;\\\n", CB_PREFIX_LABEL, r->id);
                output_local(" } else { \\\n");
                output_local("   cob_fatal_error (rtd, COB_FERROR_CODEGEN);\\\n");
                output_local(" }\\\n");
            }


        } else {
            /* cb_error("cobc:0: Internal Error output_perform_opt_return id = %d", l->le->id);*/
            output_local("   cob_fatal_error (rtd, COB_FERROR_CODEGEN);\\\n");
        }
        output_local(" }\n");
        output_local("\n");
    }
}

static void
output_get_rtd() {
    output_line("COB_RTD = cob_get_rtd();");
}

static void
output_rtd_init(int need_module) {
    output_line("/* runtime & module data */");
    cob_enterprise_output_rtd_init_header();
    if (need_module) {
        output_line("cob_module loc_module;");
        output_line("cob_module *module= &loc_module;");
    }
}

unsigned
get_runtime_flags(void) {
    unsigned res = 0;

    if (cb_flag_mf_spzero) {
        res |= COB_FLAG_RT_MF_SPZERO;
    }
    if (cb_flag_round_fp || cb_flag_compute_ibm) {
        res |= COB_FLAG_RT_ROUND_FP;
    }
    if (cb_flag_displaynumeric_mf50) {
        res |= COB_FLAG_RT_DISPLAY_MF50;
    }
    if (cb_flag_displaynumeric_vms) {
        res |= COB_FLAG_RT_DISPLAY_VMS;
    }
    if (cb_flag_0_length_trim) {

        res |= COB_FLAG_RT_ZERO_LEN_TRIM;
    }
    if (cb_flag_hostnumcompare_1) {
        res |= COB_FLAG_RT_MF_HOSTNUMCOMPARE_1;
    }
    if (cb_flag_fast_operation) {
        res |= COB_FLAG_RT_OPTIMIZE_OPERATION;
    }
    switch (cb_move_xto9_mode) {
    case CB_MOVExTO9_MF50:
        res |= COB_FLAG_RT_DISPLAY_MF50; break;
    case CB_MOVExTO9_ISO:
        res |= COB_FLAG_RT_MOVE_XTO9_ISO; break;
    case CB_MOVExTO9_MVS:
        res |= COB_FLAG_RT_MOVE_XTO9_MVS; break;
    case CB_MOVExTO9_RAW:
        res |= COB_FLAG_RT_MOVE_XTO9_RAW; break;
    case CB_MOVExTO9_RAW_0:
        res |= COB_FLAG_RT_MOVE_XTO9_RAW_0; break;
    case CB_MOVExTO9_MF40:
        res |= COB_FLAG_RT_MOVE_XTO9_MF40; break;
    default:
        break;
    }
    if (cb_flag_carealia_sign) {
        res |= COB_FLAG_RT_CAREALIA_SIGN;
    }
    if (cb_flag_compute_ibm) {
        res |= COB_FLAG_RT_COMPUTE_IBM;
    }
    if (cb_flag_ibm_display) {
        res |= COB_FLAG_RT_DISPLAY_IBM;
    }
    if (cb_flag_ibm_mode) {
        res |= COB_UNSTRING_MOVE;
        res |= COB_FLAG_RT_COMPUTE_IBM;
    }

    if (cb_flag_unstring_use_move) {
        res |= COB_UNSTRING_MOVE;
    }
    if (cb_flag_strict_compare_low) {
        res |= COB_FLAG_RT_STRICT_COMPARE_LOW;
    }
    return res;
}

unsigned
get_runtime_call_flags(void) {
    unsigned res = 0;

    if (cb_flag_load_priority || cb_flag_thread_safe) {
        res |= COB_LOAD_PRIORITY;
    }
    if (cb_flag_full_cancel) {
        res |= COB_FULL_CANCEL;
    }
    if (cb_flag_obj_cit) {
        res |= COB_FLAG_DOT_CIT_FIRST;
    }
    return res;
}

static void
output_file_all_allocation(struct cb_program *prog) {
    cb_tree     l;
    int         i;

    if (cb_sysin_redirect) {
        cb_sysin_file  = build_sysfile(prog, cb_sysin_redirect, "SYSIN", 1);
    }
    if (cb_sysout_redirect) {
        cb_sysout_file = build_sysfile(prog, cb_sysout_redirect, "SYSOUT", 0);
    }
    if (cb_syserr_redirect) {
        cb_syserr_file = build_sysfile(prog, cb_syserr_redirect, "SYSERR", 0);
    }
    if (cb_sysprint_redirect) {
        cb_sysprint_file = build_sysfile(prog, cb_sysprint_redirect, "SYSPRINT", 0);
    }
    if (prog->file_list) {
        i = 0;
        for (l = prog->file_list; l; l = CB_CHAIN(l)) {
            i += output_file_allocation(prog, CB_FILE(CB_VALUE(l)));
        }
        if (i) {
            output_local("\nstatic struct linage_struct *lingptr;\n");
        }
        output_local("\nstatic mf_extfh_FCD * fcd_ptr;\n");
    }
}


static int
hide_from_debug(struct cb_field *f) {
    if (f->flag_hide_from_debug) {
        return 1;
    }
    if (f->flag_hide_from_list && (f->count == 0)) return 1;
    return 0;
}

static char*
output_debug_str(char *str) {
    static int idx = 0;
    static char *sarray[20];

    if (str) {
        if (!sarray[idx]) {
            sarray[idx] = malloc(1024);
        }
        sprintf(sarray[idx], "%s", lookup_Cstring((unsigned char *)str));
        str = sarray[idx];
        idx++;
        if (idx >= 20) {
            idx = 0;
        }
    } else str = (char *)"NULL";
    return str;
}

typedef struct debug_meminfo_rec {
    int         totalchildcnt;
    long long   moduleid;
    struct {
        int         groupid;
        int         fieldid;
        char       *name;
        int         occurs;
        char       *redefine_name;
        int         childid;
        char       *source;
        int         line;
        char       *fullname;
        int         childcnt;
    } rec;
} debug_meminfo_rec;

static void
output_debug_meminfo_rec(debug_meminfo_rec *r, int mem_only_rec) {
    char child[COB_SMALL_BUFF];
    if (r->rec.childid) {
        sprintf(child, "%s_%lld_%d", DEBUG_INFO_SUFFIX, r->moduleid,  r->rec.childid);
    } else {
        strcpy(child, "NULL");
    }
    if (!mem_only_rec && cb_debugdb) {
        if (r->rec.fieldid) {
            debugdb_add_field_rec(cb_debugdb, r->totalchildcnt, r->moduleid, r->rec.fieldid, r->rec.name, r->rec.occurs,
                                  r->rec.redefine_name, r->rec.groupid, r->rec.childid,
                                  r->rec.fullname ? r->rec.fullname : r->rec.name, r->rec.childcnt);
        }
    } else {
        output_debug("\t{ %d, %s, %d, %s, %s, %s, %d}",
                     r->rec.fieldid, output_debug_str(r->rec.name), r->rec.occurs, output_debug_str(r->rec.redefine_name),
                     child, output_debug_str(r->rec.source), r->rec.line);
        if (r->rec.fieldid != 0) {
            output_debug(",\n");
        } else {
            output_debug("};\n");
        }
    }
    r->totalchildcnt++;
}
static int
count_sisters(struct cb_field *f) {
    int sister = 0;
    for (; f; f = f->sister) {
        sister++;
    }
    return sister;
}

static void
output_one_field_debug(debug_meminfo_rec *r, int enable_local, int groupid, struct cb_field *f, int force_hiden, char *prefix) {
    struct cb_field *p;
    char fullname[1024];
    char buff[1024];

    if (!f->flag_external && !cb_flag_keep_unused_var && !f->flag_disable_usused_skip && (f->level == 1 || f->level == 77) && !f->flag_base) {
        return;
    }
    if ((!f->flag_local || enable_local) && (!hide_from_debug(f) || force_hiden)/*&& !f->flag_item_external */) {
        if (f->redefines) r->rec.redefine_name = (char *)f->redefines->name;
        else r->rec.redefine_name = NULL;

        r->rec.fieldid  = f->id;
        if (f->children) {
            r->rec.childid = f->children->debugdb_idx;
            r->rec.childcnt = count_sisters(f->children);
        } else if (f->bitfield_children) {
            r->rec.childid = f->bitfield_children->debugdb_idx;
            r->rec.childcnt = count_sisters(f->bitfield_children);
        } else if (f->level88_children) {
            r->rec.childid = f->level88_children->debugdb_idx;
            r->rec.childcnt = count_sisters(f->level88_children);
        } else {
            r->rec.childid = 0;
            r->rec.childcnt = 0;
            if (f->flag_dynamic_data) {
                r->rec.fieldid = -r->rec.fieldid;
            }
        }
        p = f->parent;
        strcpy(fullname, f->name);
        while (p) {
            sprintf(buff, "%s%c%s", p->name, '.', fullname);
            strcpy(fullname, buff);
            p = p->parent;
        }
        if (prefix) {
            sprintf(buff, "%s%c%s", prefix,  '.', fullname);
            strcpy(fullname, buff);
        }
        r->rec.fullname = fullname;
        r->rec.name     = (char *)f->name;
        r->rec.groupid  = groupid;
        r->rec.occurs   = f->occurs_max;
        r->rec.source   = (char *)f->common.source_file;
        r->rec.line     = f->common.source_line;
        f->debugdb_idx  = r->totalchildcnt;
        f->debugdb_groupid = groupid;

        output_debug_meminfo_rec(r, 0);
    }


}
static void
output_field_debug(debug_meminfo_rec *r, struct cb_field *field, int enable_local, struct cb_field *additional_field, char *prefix) {
    struct cb_field *f;
    int groupid = 0;

    if (field && field->flag_debug_dumped) {
        return;
    }
    f = field;
    if (!f->flag_external && !cb_flag_keep_unused_var && !f->flag_disable_usused_skip && (f->level == 1 || f->level == 77) && !f->flag_base) {
        return;
    }
    for (; f; f = f->sister) {
        if (f->children) {
            output_field_debug(r, f->children, enable_local, NULL, prefix);
        }
        if (f->level88_children) {
            output_field_debug(r, f->level88_children, enable_local, NULL, prefix);
        }
        if (f->bitfield_children) {
            output_field_debug(r, f->bitfield_children, enable_local, NULL, prefix);
        }
    }
    f = field;
    if (f && ((!f->flag_local || enable_local)/*&& !f->flag_item_external */)) {
        groupid = r->totalchildcnt;
        if (f->debugdb_groupid == 0) {
            f->debugdb_groupid = groupid;
        }
        f->flag_debug_dumped = 1;
        if (!cb_debugdb) {
            output_debug("\n/* fields info for debug */\n");
            output_debug("static struct cob_field_debug_info %s_%lld_%d[]\t= {\n", DEBUG_INFO_SUFFIX, r->moduleid, groupid);
        }
        for (; f; f = f->sister) {
            output_one_field_debug(r, enable_local, groupid, f, 0, prefix);
        }
        if (additional_field) {
            output_one_field_debug(r, 1, groupid, additional_field, 1, prefix);
        }
        if (!cb_debugdb) {
            memset(&r->rec, 0, sizeof(r->rec));
            output_debug_meminfo_rec(r, 1);
        } else {
            r->totalchildcnt++;
        }
    }


}

static void
output_field_proc_debug(struct cb_field *field, int enable_local, int outputing) {
    struct cb_field *f;
    cb_tree c;

    if (field && field->flag_debug_cased) {
        return;
    }
    f = field;
    for (; f; f = f->sister) {
        stack_id = 0;
        if (!f->flag_external && !cb_flag_keep_unused_var && !f->flag_disable_usused_skip && (f->level == 1 || f->level == 77) && !f->flag_base) {
            continue;
        }
        if (f->children) {
            output_field_proc_debug(f->children, enable_local, outputing);
        }
        if (f->level88_children) {
            output_field_proc_debug(f->level88_children, enable_local, outputing);
        }
        if (f->bitfield_children) {
            output_field_proc_debug(f->bitfield_children, enable_local, outputing);
        }
    }
    f = field;
    push_output_target(cb_debug_file);

    for (; f; f = f->sister) {
        f->flag_debug_cased = outputing;
        if (!f->flag_external && !cb_flag_keep_unused_var && !f->flag_disable_usused_skip && (f->level == 1 || f->level == 77) && !f->flag_base) {
            continue;
        }

        if ((!f->flag_local || enable_local) && !f->flag_item_78) {
            stack_id = 0;
            output("\t\tcase %d : ", f->flag_dynamic_data ? -f->id : f->id);
            output_debug(" f = ");
            if (f->level == 88) {
                cb_tree c1, c3;
                c1 = cb_build_field_reference(f, NULL);
                c3 = cb_build_cond(cb_build_cond_88(c1));

                output_debug(" cob_debug_true_false(rtd, ");
                output_cond(c3, 0, 0);
                output_debug("); break;\n");
            } else {
                c = cb_build_field_reference(f, NULL);
                if (f->flag_occurs) {
                    /* Begin occurs loop */
                    CB_REFERENCE(c)->subs =
                        cb_cons(cb_int1, CB_REFERENCE(c)->subs);
                }
                output_param_debug(c, -1);
                output_debug("; break;\n");
            }
        }
    }
    pop_output_target();
}


static void
mark_all_field_in_cache(struct cb_field *f, int enable_local) {
    FILE                *savetarget;

    savetarget = cb_debug_file;
    cb_debug_file = NULL;
    /* For debug mark all existing field attr*/
    output_field_proc_debug(f, enable_local, 0);
    cb_debug_file = savetarget;
}
static void
init_debugdb_transaction(struct cb_program *prog) {
    static int dummy_modid = 1;
    if (cb_debugdb) {
        debugdb_start_transaction(cb_debugdb);
        cb_debugdb_module_id = debugdb_add_module(cb_debugdb, prog->program_id, cb_main_source_file, cb_build_stamp);
    } else {
        cb_debugdb_module_id = dummy_modid++;
    }
}

static void
close_debugdb_transaction(void) {
    debugdb_commit_transaction(cb_debugdb);
}

static struct cb_field*
find_first_debug_idx(struct cb_field *f) {
    while (f && f->debugdb_groupid == 0) {
        f = f->sister;
    }
    return f;
}
static int
output_debug_meminfo(struct cb_program *prog) {
    cb_tree l;
    char buffer[40];
    debug_meminfo_rec r = { 0 };
    struct cb_field *f;
    int save_flag_index_optimize;
    /* Dump debug info */
    if (cb_flag_mem_info && cb_debug_file) {
        save_flag_index_optimize = cb_flag_index_optimize;
        cb_flag_index_optimize = 0;
        memset(&r, 0, sizeof(r));
        r.totalchildcnt = 1;
        r.moduleid = cb_debugdb_module_id;

        /* For debug dump a table of visible symbol*/
        mark_all_field_in_cache(prog->working_storage, 0);
        output_field_debug(&r, prog->working_storage, 0, NULL, (char *)"WORKING-STORAGE");
        for (l = prog->file_list; l; l = CB_CHAIN(l)) {
            if (CB_FILE(CB_VALUE(l))->record) {
                mark_all_field_in_cache(CB_FILE(CB_VALUE(l))->record, 0);
                output_field_debug(&r, CB_FILE(CB_VALUE(l))->record, 0, CB_FILE(CB_VALUE(l))->debug_file_status, (char *)CB_FILE(CB_VALUE(l))->name);
            }
        }

        mark_all_field_in_cache(prog->linkage_storage, 1);
        output_field_debug(&r, prog->linkage_storage, 1, NULL, (char *)"LINKAGE");

        output_debug("\n/* fields info for debug All storage */\n");
        output_debug("static struct cob_field_debug_info %s_%s_all_field[]\t= {\n", DEBUG_INFO_SUFFIX, prog->program_id);
        if (cb_debugdb) {
            memset(&(r.rec), 0, sizeof(r.rec));
            r.rec.fieldid = -3;
            r.rec.name = (char *)prog->program_id;
            r.rec.redefine_name = cb_build_stamp;
            r.rec.source = cb_main_source_file;
            output_debug_meminfo_rec(&r, 1);
        }
        f = find_first_debug_idx(prog->working_storage);
        if (f) {
            memset(&(r.rec), 0, sizeof(r.rec));
            r.rec.fieldid = -1;
            r.rec.name = (char *)"WORKING-STORAGE";
            r.rec.childid = f->debugdb_groupid;
            r.rec.childcnt = count_sisters(f);
            output_debug_meminfo_rec(&r, 0);
        }
        for (l = prog->file_list; l; l = CB_CHAIN(l)) {
            if (CB_FILE(CB_VALUE(l))->record) {
                memset(&(r.rec), 0, sizeof(r.rec));
                r.rec.fieldid = -1;
                r.rec.name = (char *)CB_FILE(CB_VALUE(l))->name;
                r.rec.childid = CB_FILE(CB_VALUE(l))->record->debugdb_groupid;
                r.rec.childcnt = count_sisters(CB_FILE(CB_VALUE(l))->record);
                output_debug_meminfo_rec(&r, 0);
            }
        }
        f = find_first_debug_idx(prog->linkage_storage);
        if (f) {
            r.rec.fieldid = -2;
            r.rec.name = (char *)"LINKAGE";
            r.rec.childid = f->debugdb_groupid;
            r.rec.childcnt = count_sisters(f);
            output_debug_meminfo_rec(&r, 0);
        }
        push_output_target(cb_debug_file);
        memset(&(r.rec), 0, sizeof(r.rec));
        output_debug_meminfo_rec(&r, 1);
        output_debug("\n\nstatic cob_field * %s_get_field (void* storage, int id) \n", prog->program_id);
        output_debug("{\n");
        output_debug("\tstatic cob_field\tf0;\n");
        output_debug("\tstatic cob_bitsarray_field\tfba0;\n");
        output_debug("\tcob_field *f=NULL;\n");
        output_debug("\tmds_t *mds=storage;\n");
        output_get_rtd();
        output_debug("\tswitch(id)\n");
        output_debug("\t{\n");
        pop_output_target();
        sprintf(buffer, "COB_MDS %s", CB_PREFIX_LINKAGE);
        debug_prefix_base = buffer;
        output_field_proc_debug(prog->working_storage,  0, 1);
        if (prog->file_list) {
            output_debug("\n/* file record */\n");
            for (l = prog->file_list; l; l = CB_CHAIN(l)) {
                if (CB_FILE(CB_VALUE(l))->record) {
                    output_field_proc_debug(CB_FILE(CB_VALUE(l))->record, 0, 1);
                }
            }
        }
        if (prog->linkage_storage) {
            output_debug("\n/* linkage saved pointers */\n");
            output_field_proc_debug(prog->linkage_storage,  1, 1);
            output_debug("\n");
        }
        debug_prefix_base = NULL;
        output_debug("\t};\n");
        output_debug("\treturn f;\n");
        output_debug("}\n");
        if (save_flag_index_optimize) {
            cb_flag_index_optimize = save_flag_index_optimize;
        }
    }
    return r.totalchildcnt;
}

/** 
 * 
 * 
 * @param prog
 * @param k
 */
static
void output_field_init_fct(struct cb_program *prog) {
    struct field_list *k;
    push_output_target(cb_local_function_file);
    output_line("/* Init field code */");
    output_line("static void %s_init_fields (COB_RTD, mds_t *mds) ", prog->program_id);
    output_indent("{");

    /* Field data initialization */
    if (cb_flag_thread_safe) {
        for (k = field_cache; k; k = k->next) {
            if (k->x) {
                output("%s%d.size = ", CB_PREFIX_FIELD, k->f->id);
                output_size_storage(k->x);
                output(";\n");
                if (!k->f->flag_local && !k->f->flag_item_external) {
                    output("%s%d.data = ", CB_PREFIX_FIELD, k->f->id);
                    output_data(k->x);
                    output(";\n");
                }
                output("%s%d.attr = ", CB_PREFIX_FIELD, k->f->id);
                output_attr(k->x, 0);
                output(";\n");
            }
        }
    }
    if (prog->screen_storage) output_line("init_screens(rtd, mds);");
    output_indent("}");
    pop_output_target();


}

static void
output_exit_program_deallocate(struct cb_program *prog) {
    if (prog->local_storage) {
        output_line("/* Deallocate LOCAL storage */");
        output_line("if (local_storage) {");
        output_line("\tcob_free_cbl_allocation (rtd, local_storage);");
        output_line("\tlocal_storage = NULL;");
        output_line("}");
        output_newline();
    }
    output_line("/* Pop module stack */");
    /*CIT*/
    /*output_line ("rtd->current_module = rtd->current_module->next;");*/
    output_newline();
    output_line("cob_module_leave(rtd, module);");
}

static void
output_internal_function(struct cb_program *prog, cb_tree parameter_list) {
    cb_tree                 l;
    cb_tree                 l2;
    struct cb_field         *f;
    struct cb_field         *ff;
    struct local_list       *locptr;
    struct cb_file          *fl;
    struct handler_struct   *hstr;
    struct label_list       *pl;
    int                     i;
    int                     n;
    int                     parmnum = 0;
    int                     seen = 0;
    int                     anyseen;
    int                     totdbfield;
    int                     j;
    init_debugdb_transaction(prog);
    /* Module Structure init*/
    output_line("static void %s_module_init (mds_t *loc_cob_mds,  cob_module *module)", prog->program_id);
    output_indent("{");
    output_line("memset (module, 0, sizeof(cob_module));");
    output_line("module->cob_procedure_parameters = (loc_cob_mds-> %s%s.cob_user_parameters);", CB_PREFIX_STATIC, prog->program_id);
    output_line("module->display_sign             = %d;", cb_display_sign_ebcdic);
    output_line("module->decimal_point            = (unsigned char)%d;", (unsigned)(A2E(prog->decimal_point)));
    output_line("module->currency_symbol          = (unsigned char)%d;", (unsigned)(A2E(prog->currency_symbol)));
    output_line("module->numeric_separator        = (unsigned char)%d;", (unsigned)(A2E(prog->numeric_separator)));
    output_line("module->flag_filename_mapping    = %d;", cb_filename_mapping);
    output_line("module->flag_binary_truncate     = %d;", cb_binary_truncate);
    output_line("module->flag_pretty_display      = %d;", (cb_pretty_display ? COB_NUMERIC_PRETTY_DISPLAY : 0) |
                (cb_flag_raw_pic9_display ? COB_NUMERIC_RAW_DISPLAY : 0));

    output_line("module->module_name              = \"%s\";", prog->program_id);
    output_line("module->source_file              = COB_SOURCE_FILE;");
    output_line("module->build_stamp              = COB_BUILD_STAMP;");
    output_line("module->cur_source_file          = COB_SOURCE_FILE;");
    if (cb_codepage) {
        output_line("module->default_cp              = \"%s\";", cb_codepage);
    }
    if (cb_flag_mem_info) {
        output_line("module->fields_debug_info       = %s_%s_all_field;", DEBUG_INFO_SUFFIX, prog->program_id);
        output_line("module->get_cob_field           = %s_get_field;", prog->program_id);
    }
    if (cb_debugdb && cb_debugdb_name) {
        output_line("module->debugdb_name            = \"%s\";", cb_debugdb_name);
    }
    output_line("module->data_storage             = loc_cob_mds;");
    output_line("module->cbl_allocated_list_ptr   = &(loc_cob_mds->module_allocated_list);");
    if (cb_flag_source_location && !cb_debugdb) {
        output_line("module->lines_debug_info         = %s_lines_debug;", prog->program_id);
    }
    output_line("module->runtime_flags               = 0x%x;", get_runtime_flags());
    output_line("module->module_version              = 4;");
    output_line("module->lines_debug_info_version    = %d;", LINES_DEBUG_INFO_VERSION);
    cob_enterprise_output_internal_function_init(prog);
    output_line("module->ebcdic_charset              = %d;", cb_flag_ebcdic_charset);

    if (cb_ccmap_file) {
        output_line("module->ccmapdata              = &%s_ccmap_data;",  prog->program_id);
        output_line("module->ccmapdata_size         = sizeof(%s_ccmap_data);",  prog->program_id);
    }
    output_line("module->utf16_le                   = %d;", cb_flag_utf16_le ? 1 : 0);
    output_line("module->no_cbl_error_proc          = %d;", cb_flag_no_cbl_error_proc ? 1 : 0);
    output_line("module->display_dos                = %d;", cb_flag_display_dos ? 1 : 0);
    output_line("module->emulate_vms                = %d;", cb_flag_emulate_vms ? 1 : 0);
    output_line("module->xparse_event               = %d;", cb_flag_xparse_event ? 1 : 0);
    output_line("module->numval_validate            = %d;", cb_flag_numval_validate ? 1 : 0);
/* END INIT cob_module */
    output_indent("}");

/* Program function */
    output("static ");
    output_rtncode_size();
    output(" %s (const int entry", prog->main_entry_name);
    if ((!cb_sticky_linkage && !prog->gen_params_protection) || prog->flag_chained) {
        if (!prog->flag_chained) {
            for (l = parameter_list; l; l = CB_CHAIN(l)) {
                output(", unsigned char *%s%d",
                       CB_PREFIX_BASE, cb_field(CB_VALUE(l))->id);
                parmnum++;
            }
        }
        output(")\n");
        output_indent("{");
    } else {
        /*intermedaite local var */
        for (l = parameter_list; l; l = CB_CHAIN(l)) {
            output(", unsigned char *p_%s%d",
                   CB_PREFIX_BASE, cb_field(CB_VALUE(l))->id);
            parmnum++;
        }
        output(")\n");
        output_indent("{");
        for (l = parameter_list; l; l = CB_CHAIN(l)) {
            output_line("unsigned char *%s%d = p_%s%d;",
                        CB_PREFIX_BASE, cb_field(CB_VALUE(l))->id, CB_PREFIX_BASE, cb_field(CB_VALUE(l))->id);
        }
    }

/* Local variables */
    output_line("/* Local variables */");
    output_line("#include \"%s\"", prog->local_storage_name);
    output_line("#include \"%s\"", prog->local_register_name);
    output_newline();

    output_line("FLD_VAR");

/* Alphabet-names */
    if (prog->alphabet_name_list) {
        output_local("/* Alphabet names */\n");
        for (l = prog->alphabet_name_list; l; l = CB_CHAIN(l)) {
            output_alphabet_name_definition(CB_ALPHABET_NAME(CB_VALUE(l)));
        }
        output_local("\n");
    }

    if (prog->decimal_index_max) {
        output_local("/* Decimal structures */\n");
        output_local("struct {\n");

        for (i = 0; i < prog->decimal_index_max; i++) {
            /* output_local ("cob_decimal d%d;\n", i); */
            /*CIT*/
            output_local("cob_reg_type r%d;\n", i);
        }
        output_local("} reg;\n");
    }
    if (prog->field_pointer_max) {
        output_local("/* field pointer */\n");
        for (i = 0; i < prog->field_pointer_max; i++) {
            output_local("cob_field *pfld%d;\n", i);
        }
    }
    output_local_register("union cob_call_union cob_unifunc;\n");
    output_local_register("int cob_save_exception_code;\n");

    output_static("cob_field *cob_user_parameters[COB_MAX_FIELD_PARAMS];\n");
/*CIT*/
/*runtime & module data */
    output_rtd_init(1);

    output_newline();

/* External items */
    if ((prog->gen_params_protection || cb_sticky_linkage || cb_flag_mem_info) && prog->linkage_storage) {
        output_global("\n/* Sticky linkage save pointers */\n");
        for (f = prog->linkage_storage; f; f = f->sister) {
            output_global("unsigned char\t*%s%d;\n", CB_PREFIX_LINKAGE, f->id);
            output_global("unsigned char\tstore_%s%d[%d];\n", CB_PREFIX_LINKAGE, f->id, sizeof(long long));

        }
        output_global("\n");
    }

/* Files */
    output_file_all_allocation(prog);

/*loop*/
    if (prog->loop_counter) {
        output_local("\n/* Loop counters */\n");
        output_local("struct local_count_s {\n");
        output_local("  int n0;\n");
        for (i = 1; i < prog->loop_counter; i++) {
            output_local("  int n%d;\n", i);
        }
        output_local("} l_cntr;\n");
        output_local("\n");
    }

/* BASED working-storage */
    i = 0;
    for (f = prog->working_storage; f; f = f->sister) {
        if (f->flag_item_based) {
            if (!i) {
                i = 1;
                output_static("/* BASED WORKING-STORAGE SECTION */\n");
            }
            output_static("unsigned char *i%s%d; /* %s */\n",
                          CB_PREFIX_BASE, f->id, f->name);
            output_static("#define %s%d (COB_MDS %s%s.i%s%d)",
                          CB_PREFIX_BASE, f->id,
                          CB_PREFIX_STATIC, prog->program_id,
                          CB_PREFIX_BASE, f->id);
        }
    }
    if (i) {
        output_static("\n");
    }

/* BASED local-storage */
    i = 0;
    for (f = prog->local_storage; f; f = f->sister) {
        if (f->flag_item_based) {
            if (!i) {
                i = 1;
                output_local("/* BASED LOCAL-STORAGE */\n");
            }
            output_local("unsigned char\t\t*%s%d = NULL; /* %s */\n",
                         CB_PREFIX_BASE, f->id, f->name);
            if (prog->flag_global_use) {
                output_static("unsigned char\t*save_%s%d;\n",
                              CB_PREFIX_BASE, f->id, f->name);
            }
        }
    }
    if (i) {
        output_local("\n");
    }

/* Dangling linkage section items */
    seen = 0;
    for (f = prog->linkage_storage; f; f = f->sister) {
        f->flag_unused_linkage = 0;
        for (l = parameter_list; l; l = CB_CHAIN(l)) {
            if (f == cb_field(CB_VALUE(l))) {
                break;
            }
        }
        if (l == NULL || prog->gen_params_protection) {
            if (!seen) {
                seen = 1;
                output_global("\n/* LINKAGE SECTION (Items not referenced by USING clause or used when param == NULL) */\n");
            }
            if ((l == NULL && cb_flag_alloc_linkage) ||
                (l == NULL && prog->returning && cb_field(prog->returning) == f) ||
                (l != NULL && prog->gen_params_protection)) {
                output_global("union { long long i ; unsigned char c[%d] ;} u%s%d; \n",
                              f->memory_size <= 0 ? 1 : f->memory_size,
                              CB_PREFIX_BASE, f->id);
                f->flag_linkage_allocated = 1;
            }

            if (l == NULL) {

                output_global(" /* %s */ \n", f->name);
                output_global("unsigned char\t*i%s%d;  \n",
                              CB_PREFIX_BASE, f->id, f->name);
                output_global("#define %s%d (COB_MDS i%s%d)\n",
                              CB_PREFIX_BASE, f->id,
                              CB_PREFIX_BASE, f->id);
                f->flag_unused_linkage = 1;
            }
        }
    }
    if (seen) {
        output_global("\n");
    }

/* Screens */
    push_output_target(cb_storage_file);
    output("\n/* Screens */\n\n");
    output("typedef struct {\n");
    output("  int initialized;\n");

    if (prog->screen_storage) {
        //output_target[output_target_idx] = current_prog->local_storage_file;
        output_local_function("/* Init screens code */\n");
        output_local_function("static void\n");
        output_local_function("init_screens (COB_RTD, mds_t *mds) \n");
        output_local_function("{ \n");
        output_local_function("if (!(COB_MDS %s%s.initialized) ) {\n", CB_PREFIX_SCREEN, prog->program_id);
        output_screen_definition(prog, prog->screen_storage);
        output_local_function("  COB_MDS %s%s.initialized = 1;\n", CB_PREFIX_SCREEN, prog->program_id);
        output_local_function("  }\n");
        output_local_function("}\n\n");
        output_newline();
    }
    output("} %s%s_t;\n\n", CB_PREFIX_SCREEN, prog->program_id);
    output_newline();
    pop_output_target();

    output_target[output_target_idx] = yyout;

    output_local("\n/* Define perform frame stack */\n\n");
    if (cb_perform_osvs) {
        output_local("struct cob_frame *temp_index;\n");
    }
    if (cb_flag_stack_check) {
        output_local("struct cob_frame *frame_overflow;\n");
    }
    output_local("struct cob_frame *frame_ptr;\n");
    if (cb_flag_debuginfo) {
        output_local("struct cob_stack_debug_info cob_debug_stack_frame[COB_STACK_SIZE];\n");
    }
    output_local("struct cob_frame frame_stack[COB_STACK_SIZE];\n\n");

    i = 0;
    anyseen = 0;
    for (l = parameter_list; l; l = CB_CHAIN(l), i++) {
        f = cb_field(CB_VALUE(l));
        if (f->flag_any_length) {
            if (!anyseen) {
                anyseen = 1;
                output_local("/* ANY LENGTH fields */\n");
            }
            output_local("cob_field\t\t*anylen_%d;\n", i);
            if (prog->flag_global_use) {
                output_static("cob_field\t*save_anylen_%d;\n", i);
            }
        }
    }
    if (anyseen) {
        output_local("\n");
    }
    if (prog->flag_global_use && parameter_list) {
        output_static("/* Parameter save */\n");
        for (l = parameter_list; l; l = CB_CHAIN(l)) {
            f = cb_field(CB_VALUE(l));
            output_static("unsigned char\t*save_%s%d;\n",
                          CB_PREFIX_BASE, f->id);
        }
        output_static("\n");
    }

/*START OF Code*/

    output_line("/* Start of function code */");
    output_line("%s_module_init (COB_MDS_REF, module);", prog->program_id);
    if (prog->crt_status && cb_field(prog->crt_status)->count) {
        output_prefix();
        output("module->crt_status = ");
        output_param(cb_ref(prog->crt_status), -1);
        output(";\n");
    }
    if (prog->cursor_pos) {
        output_prefix();
        output("module->cursor_pos = ");
        output_param(cb_ref(prog->cursor_pos), -1);
        output(";\n");
    }
    if (cb_flag_profiling) {
        output_line("cob_profiling_register(rtd,module->profiling_info);");
    }

    if (cb_flag_map_screen_exceptions) {
        output_line("rtd->cob_extended_status = 1;");
    }
    if (cb_flag_map_screen_raw_keys) {
        output_line("rtd->cob_screen_key_raw = 1;");
    }
    if (prog->collating_sequence) {
        output_prefix();
        output("module->collating_sequence = ");
        output_param(cb_ref(prog->collating_sequence), -1);
        output(";\n");
    }
    if (!cb_flag_thread_safe && !cb_flag_C_data_preinit) {
        output_line("if (!cob_mds_initialized) {");
        output_line("  memset (&cob_mds, 0, sizeof(mds_t));");
        output_line("  cob_mds_initialized = 1;");
        output_line("  }");
    }

    output_newline();
    output_line("/* CANCEL callback handling */");
    output_line("if (unlikely(entry < 0)) {");
    output_line("  if (! (COB_MDS %s%s.initialized)) {", CB_PREFIX_FIELD, prog->program_id);
    output_line("      return 0;");
    output_line("  }");
    for (l = prog->file_list; l; l = CB_CHAIN(l)) {
        fl = CB_FILE(CB_VALUE(l));
        if (fl->organization != COB_ORG_SORT && !fl->external) {
            output_line("  cob_close_extfh (rtd, %s%s, 0, NULL);",
                        CB_PREFIX_FILE, fl->cname);
        }
    }
    if (prog->decimal_index_max) {
        for (i = 0; i < prog->decimal_index_max; i++) {
            /*CIT*/
            output_line("  cob_decimal_clear (&(COB_MDS %s%s.d%d));",  CB_PREFIX_FIELD, prog->program_id, i);
        }
    }
    if (cb_flag_profiling) {
        output_line("cob_profiling_unregister(rtd,module->profiling_info);");
    }
    output_line("  cob_free_module_allocated (rtd, module);");
    if (cb_flag_initialize_optimize) {
        output_line("  memset (initcache, 0, sizeof(initcache));\n");
    }
    output_line("  COB_MDS %s%s.initialized = 0;", CB_PREFIX_FIELD, prog->program_id);
    if (!prog->flag_is_part_of_nested && cb_flag_thread_safe) {
        if (!cb_free_thread_safe_data) {
            output_line("  if (rtd->call_flag & COB_FULL_CANCEL)");
        }
        output_line("    cob_free_module_storage (rtd, mds_key, sizeof(mds_t));");
    }
    output_line("  return 0;");
    output_line("}");
    output_newline();
    for (i = 0, l = parameter_list; i < parmnum; l = CB_CHAIN(l), i++) {
        f = cb_field(CB_VALUE(l));
        if (cb_sticky_linkage) {
            output_line("if (%s%d != NULL) {", CB_PREFIX_BASE, f->id);
            output_line("  (COB_MDS %s%d) = %s%d;", CB_PREFIX_LINKAGE, f->id, CB_PREFIX_BASE, f->id);
            output_line("  }");
            output_line("else {");
            output_line("  %s%d = (COB_MDS %s%d);", CB_PREFIX_BASE, f->id, CB_PREFIX_LINKAGE, f->id);
            output_line("  }");
        } else if (cb_flag_mem_info) {
            output_line("(COB_MDS %s%d) = %s%d;", CB_PREFIX_LINKAGE, f->id, CB_PREFIX_BASE, f->id);
        }
    }
    for (f = prog->linkage_storage; f; f = f->sister) {
        if (f->flag_linkage_allocated) {
            output_line("if (%s%d == NULL) {", CB_PREFIX_BASE, f->id);
            output_line("  %s%d = (unsigned char*)&(COB_MDS u%s%d);",
                        CB_PREFIX_BASE, f->id,
                        CB_PREFIX_BASE, f->id);
            output_line("  }");
        }
    }

    output_line("/* Initialize frame stack */");
/* Context reload process*/
    if (cb_flag_context_reload_enable) {
        output_line("memset (frame_stack, 0, sizeof(frame_stack));");
        if (cb_flag_debuginfo) {
            output_line("memset (cob_debug_stack_frame, 0, sizeof(cob_debug_stack_frame));");
        }
    }
    output_line("frame_ptr = &frame_stack[0];");
    if (cb_flag_debuginfo) {
        output_line("module->debug_stack_frame_start = &cob_debug_stack_frame[0];");
        output_line("module->debug_stack_frame_curpos = module->debug_stack_frame_start;");
    }
    output_line("frame_ptr->perform_through = 0;");
    if (cb_flag_stack_check) {
        output_line("frame_overflow = &frame_stack[COB_STACK_SIZE - 1];");
    }
    output_newline();

/*CIT*/
    if (cb_flag_debuginfo) {
        output_line("if (unlikely(COB_MDS %s%s.initialized == 0)) {", CB_PREFIX_FIELD, prog->program_id);
        output_line("  cob_runtime_debugger_set_module_name(module->module_name);");
        output_line("  cob_runtime_debugger_init();\n}");
    }
    output_line("cob_module_enter(rtd, module);");

/* initialization */
    output_line("if (unlikely(COB_MDS %s%s.initialized == 0))", CB_PREFIX_FIELD, prog->program_id);
    output_indent("{");
    output_line("if (!rtd->cob_initialized) {");
    if (cb_flag_implicit_init) {
        output_line("  cob_init (rtd, 0,NULL);");
    } else {
        output_line("  cob_fatal_error (rtd, COB_FERROR_INITIALIZED);");
    }
    output_line("}");
    output_line("cob_check_version_1 (rtd, COB_SOURCE_FILE, COB_PACKAGE_VERSION, COB_PATCH_LEVEL, %d);", cb_flag_ebcdic_charset);
    output_line("if (COB_MDS initialized != 0711) {");
    output_line("  init_fields(rtd, COB_MDS_REF);");
    output_line("  COB_MDS initialized = 0711;");
    output_line("}");

    if (get_runtime_call_flags()) {
        output_line("rtd->call_flag |= 0x%x;", get_runtime_call_flags());
    }

    for (l = prog->entry_list; l; l = CB_CHAIN(l)) {
        struct cb_entry *e = CB_ENTRY(CB_VALUE(l));
        if (CB_LABEL(e->label)->is_module_entry && !e->no_entry_function) {
            char *p;
            char *entry_name = (char *)(CB_LABEL(e->label)->name);

            output_line("cob_set_cancel (rtd, (const char *)\"%s\", (void *)%s, (void *)%s);",
                        entry_name, entry_name,  prog->main_entry_name);
            p = strdup((char *)entry_name);
            cob_strupper(p);
            if (strcmp((char *)entry_name, p) != 0) {
                output_line("cob_set_cancel (rtd, (const char *)\"%s\", (void *)%s, (void *)%s);",
                            p, p,  prog->main_entry_name);
            }
            free(p);
        }
    }
    if (cb_flag_static_call == 2) {
        output_line("goto L_init_%s;", prog->program_id);
        output_line("LRET_init_%s: ;", prog->program_id);
    }
    if (prog->decimal_index_max) {
        output_line("/* Initialize decimal numbers */");
        output_line("memset (&reg, 0, sizeof(reg));");
        for (i = 0; i < prog->decimal_index_max; i++) {
            output_line("cob_decimal_init (&(COB_MDS %s%s.d%d));", CB_PREFIX_FIELD, prog->program_id, i);
        }
        output_newline();
    }
    if (!prog->flag_initial) {
        output_initialize_sequence_call(prog);
        /*output_initialize_sequence(prog);*/
    }

    output_line("COB_MDS %s%s.initialized = 1;", CB_PREFIX_FIELD, prog->program_id);
    if (prog->flag_chained) {
        output("    } else {\n");
        output_line("  cob_fatal_error (rtd, COB_FERROR_CHAINING);");
        output_indent("}");
    } else {
        output_indent("}");
    }
    output_newline();
    if (!prog->flag_initial) {
        output_sysfile_open(cb_sysin_file,  0);
        output_sysfile_open(cb_sysout_file, 1);
        output_sysfile_open(cb_syserr_file, 2);
        output_sysfile_open(cb_sysprint_file, 3);
    }

/* Set up LOCAL-STORAGE cache */
    if (prog->local_storage) {
        for (f = prog->local_storage; f; f = f->sister) {
            ff = cb_field_founder(f);
            while (ff->redefines) {
                ff = ff->redefines;
            }
            if (ff->flag_item_based || ff->flag_local_alloced) {
                continue;
            }
            if (ff->flag_item_78) {
                fprintf(stderr, "Unexpected CONSTANT item\n");
                ABORT();
            }
            ff->flag_local_alloced = 1;
            locptr = cobc_malloc(sizeof(struct local_list));
            locptr->f = ff;
            locptr->next = local_cache;
            local_cache = locptr;
        }
        local_cache = local_list_reverse(local_cache);
    }

/* Global entry dispatch */
    if (prog->global_list) {
        output_line("/* Global entry dispatch */");
        output_newline();
        for (l = prog->global_list; l; l = CB_CHAIN(l)) {
            output_line("if (unlikely(entry == %d)) {",
                        CB_LABEL(CB_VALUE(l))->id);
            if (cb_flag_traceall) {
                output_line("\tcob_ready_trace (rtd);");
            }
            for (locptr = local_cache; locptr; locptr = locptr->next) {
                output_line("\t%s%d = (COB_MDS %s%s).save_%s%d;",
                            CB_PREFIX_BASE, locptr->f->id,
                            CB_PREFIX_STATIC, prog->program_id,
                            CB_PREFIX_BASE, locptr->f->id);
            }
            i = 0;
            for (l2 = parameter_list; l2; l2 = CB_CHAIN(l), i++) {
                f = cb_field(CB_VALUE(l2));
                output_line("\t%s%d = (COB_MDS %s%s).save_%s%d;",
                            CB_PREFIX_BASE, f->id,
                            CB_PREFIX_STATIC, prog->program_id,
                            CB_PREFIX_BASE, f->id);
                if (f->flag_any_length) {
                    output_line("\tanylen_%d = (COB_MDS %s%s).save_anylen_%d;", i, CB_PREFIX_STATIC, prog->program_id, i);
                }
            }
            output_line("\tgoto %s%d;",
                        CB_PREFIX_LABEL,
                        CB_LABEL(CB_VALUE(l))->id);
            output_line("}");
        }
        output_newline();
    }


    if (prog->flag_initial) {
        output_initialize_sequence_call(prog);
        /*output_initialize_sequence(prog);*/
        output_sysfile_open(cb_sysin_file,  0);
        output_sysfile_open(cb_sysout_file, 1);
        output_sysfile_open(cb_syserr_file, 2);
        output_sysfile_open(cb_sysprint_file, 3);
    }

    if (prog->local_storage) {
        int localsize = 0;
        output_local("unsigned char *local_storage = NULL;\n");
        if (local_cache) {
            output_line("/* Allocate LOCAL storage */");
        }
        localsize = (cb_local_storage_guard / 8) * 8;
        for (locptr = local_cache; locptr; locptr = locptr->next) {

            locptr->f->local_offset = localsize;
            localsize +=  locptr->f->memory_size <= 0 ? 8 : ((locptr->f->memory_size / 8) + 1) * 8;
        }
        localsize += cb_local_storage_guard;
        output_line("local_storage = cob_malloc_cbl_allocation (rtd, %d, 1, 1);", localsize);
        for (locptr = local_cache; locptr; locptr = locptr->next) {
            output_base_allocation(locptr->f);
            output_line("%s%d = local_storage + %d;", CB_PREFIX_BASE, locptr->f->id, locptr->f->local_offset);
            if (current_codegen_prog->flag_global_use) {
                output_line("(COB_MDS %s%s).save_%s%d = %s%d;",
                            CB_PREFIX_STATIC, prog->program_id,
                            CB_PREFIX_BASE, locptr->f->id,
                            CB_PREFIX_BASE, locptr->f->id);
            }
        }
        output_newline();
        output_line("/* Initialialize LOCAL storage */");
        output_initial_values(prog->local_storage, 0);
        output_newline();
    }

    if (cb_field(current_codegen_prog->cb_call_params)->count) {
        output_line("/* Initialize number of call params */");
        output("  ");
        output_integer(current_codegen_prog->cb_call_params);
        output_line(" = rtd->cob_call_params;");
    }
    output_line("rtd->cob_save_call_params = rtd->cob_call_params;");
    output_newline();
    if (cb_flag_traceall) {
        output_line("cob_ready_trace (rtd);");
        output_newline();
    }

    i = 0;
    if (anyseen) {
        output_line("/* Initialize ANY LENGTH parameters */");
    }
    for (l = parameter_list; l; l = CB_CHAIN(l), i++) {
        f = cb_field(CB_VALUE(l));
        if (f->flag_any_length) {
            output("  anylen_%d = ", i);
            output_param(CB_VALUE(l), i);
            output(";\n");
            if (prog->flag_global_use) {
                output_line("(COB_MDS %s%s).save_anylen_%d = anylen_%d;",
                            CB_PREFIX_STATIC, prog->program_id, i, i);
            }
            output_line("if (rtd->cob_call_params > %d && %s%d%s)",
                        i, "module->next->cob_procedure_parameters[",
                        i, "]");
            output_line("  anylen_%d->size = %s%d%s;", i,
                        "module->next->cob_procedure_parameters[",
                        i, "]->size");
        }
    }
    if (anyseen) {
        output_newline();
    }
    if (prog->flag_global_use && parameter_list) {
        output_line("/* Parameter save */");
        for (l = parameter_list; l; l = CB_CHAIN(l)) {
            f = cb_field(CB_VALUE(l));
            output_line("(COB_MDS %s%s).save_%s%d = %s%d;",
                        CB_PREFIX_STATIC, prog->program_id,
                        CB_PREFIX_BASE, f->id,
                        CB_PREFIX_BASE, f->id);
        }
        output_newline();
    }

/* Context reload process*/
    if (cb_flag_context_reload_enable) {
        needs_exit_prog = 1;
        output_line("/* Context reload  */");
        output_line("if (rtd->cob_context_info.cob_context_mode & COB_CONTEXT_RELOAD)");
        output_indent("{");
        output_goto_1(cb_save_context_handler_ref);
        output_indent("}");
    }

    output_line("rtd->return_field = NULL;");
/* Entry dispatch */
    output_line("/* Entry dispatch */");
    if (cb_list_length(prog->entry_list) > 1) {
        output_newline();
        output_line("switch (entry)");
        output_line("  {");
        for (i = 0, l = prog->entry_list; l; l = CB_CHAIN(l)) {
            struct cb_entry *e = CB_ENTRY(CB_VALUE(l));
            if (!e->no_entry_function) {
                output_line("  case %d:", i);
                output_line("    goto %s%d;",
                            CB_PREFIX_LABEL, CB_LABEL(e->label)->id);
            }
            i++;
        }
        output_line("  }");
        output_line("/* This should never be reached */");
        output_line("cob_fatal_error (rtd, COB_FERROR_CHAINING);");
        output_newline();
    } else {
        l = prog->entry_list;
        output_line("goto %s%d;", CB_PREFIX_LABEL, CB_LABEL(CB_ENTRY(CB_VALUE(l))->label)->id);
        output_newline();
    }

/* PROCEDURE DIVISION */
    prog->flag_init_outputed = 1;
/* first run optimize */
    if (cb_optimize_size) {
        prog->exec_list = enterprise_optimizer_mark_linear_statement(prog, prog->exec_list);
    }
/* then output the code*/
//   remove_duplicate_decimal_list();
    output_line("/* PROCEDURE DIVISION */");
    for (l = prog->exec_list; l; l = CB_CHAIN(l)) {
        output_stmt(CB_VALUE(l));
    }
    output_newline();
    output_line("/* Program exit */");
    output_newline();

    if (needs_exit_prog) {
        output_line("exit_program:");
        output_newline();
    }
    output_exit_program_deallocate(prog);
    if (prog->local_storage) {
        local_cache = local_list_reverse(local_cache);
    }
    if (cb_flag_traceall) {
        output_line("cob_reset_trace (rtd);");
        output_newline();
    }
    output_line("/* Program return */");
    cob_enterprise_output_rtd_exit_footer();
    if (current_codegen_prog->returning) {
        output_prefix();
        output("rtd->return_field = ");
        output_param(current_codegen_prog->returning, 0);
        output(";\n");
    } else {
        /*output_param(current_codegen_prog->cb_return_code, 0);*/
        /* output ("NULL"); */
    }
    output_prefix();
    output("return ");
    output_integer(current_codegen_prog->cb_return_code);
    output(";\n");
/*INDEX BUFFER*/
    if (cb_flag_index_optimize) {
        for (j = 0; j <= lastIndex; j++) {
            output_local_register("int\t\tr%d;\n", j);
        }
    }
/* Error handlers */
    if (prog->file_list || prog->gen_file_error) {
        output_newline();
        seen = 0;
        for (i = COB_OPEN_INPUT; i <= COB_OPEN_EXTEND; i++) {
            if (prog->global_handler[i].handler_label) {
                seen = 1;
                break;
            }
        }
        output_stmt(cb_standard_error_handler);
        output_newline();
        if (seen) {
            output_line("switch (((cob_file_extfh*)(rtd->cob_error_file))->last_open_mode)");
            output_indent("{");
            for (i = COB_OPEN_INPUT; i <= COB_OPEN_EXTEND; i++) {
                hstr = &prog->global_handler[i];
                if (hstr->handler_label) {
                    output_line("case %d:", i);
                    output_indent("{");
                    if (prog == hstr->handler_prog) {
                        output_perform_call(hstr->handler_label,
                                            hstr->handler_label);
                        /*CIT*/
                        output_line("rtd->current_module = module;");
                    } else {
                        if (cb_flag_traceall) {
                            output_line("cob_reset_trace (rtd);");
                        }
                        output_prefix();
                        output("%s (%d",
                               hstr->handler_prog->main_entry_name,
                               hstr->handler_label->id);
                        parmnum = cb_list_length(hstr->handler_prog->parameter_list);
                        for (n = 0; n < parmnum; n++) {
                            output(", NULL");
                        }
                        output(");\n");
                        if (cb_flag_traceall) {
                            output_line("cob_ready_trace (rtd);");
                        }
                    }
                    output_line("break;");
                    output_indent("}");
                }
            }
            output_line("default:");
            output_indent("{");
        }
        if (!cb_flag_vms_error_handler) {
            output_line("if (!(((cob_file_extfh*)(rtd->cob_error_file))->flag_select_features & COB_SELECT_FILE_STATUS)) ");
        }
        output_line("{");
        output_line("  cob_default_error_handle (rtd);");
        output_line("  cob_stop_abend (rtd, 1);");
        output_line("}");
        if (seen) {
            output_line("break;");
            output_indent("}");
            output_indent("}");
        }
        output_perform_exit(CB_LABEL(cb_standard_error_handler));
        output_newline();
        output_line("cob_fatal_error (rtd, COB_FERROR_CODEGEN);");
        output_newline();
    }
    if (!cb_flag_goto_gcc) {
        if (cb_flag_optreturn) {
            output_local("/* Frame stack return table */\n");
            output_perform_opt_return(prog);
        } else {
            output_newline();
            output_line("/* Frame stack jump table */");
            output_line("P_switch:");
            if (label_cache) {
                /*CIT*/
                label_cache = label_list_reverse(label_cache);
                output_line(" switch (frame_ptr->return_address) {");
                for (pl = label_cache; pl; pl = pl->next) {
                    output_line(" case %d:", pl->call_num);
                    output_line("   goto %s%d;", CB_PREFIX_LABEL, pl->id);
                }
                output_line(" }");
            }
            output_line(" cob_fatal_error (rtd, COB_FERROR_CODEGEN);");
            output_newline();
        }
    }


    output_newline();
    if (cb_flag_context_reload_enable) {
        struct base_list *blp;

        output_stmt(cb_save_context_handler);
        output_newline();
        output_indent("{");
        output_line("int frame_idx;");
        output_line("int father;");
        output_line("cob_open_context_file (rtd, \"%s %s %s\");", excp_current_source_name, __DATE__, __TIME__);
        output_line("switch (rtd->cob_context_info.cob_context_mode & 0x0F) {");
        /* Add context to basecache */
        push_output_target(NULL);
        output_integer(prog->cb_context_code);
        pop_output_target();

        output_line("  case COB_CONTEXT_RELOAD:");
        for (blp = base_cache; blp; blp = blp->next) {
            if (strcmp(blp->curr_prog, excp_current_program_id) == 0) {
                output_line("      cob_read_context_data (rtd, %s%d, %d);", CB_PREFIX_BASE, blp->f->id, blp->f->size);


            }
        }
        for (locptr = local_cache; locptr; locptr = locptr->next) {
            output_line("      cob_read_context_data (rtd, %s%d, %d);", CB_PREFIX_BASE, locptr->f->id, locptr->f->size);
        }
        output_line("      cob_read_context_data (rtd, frame_stack,sizeof(frame_stack));");
        output_line("      cob_read_context_data (rtd, &frame_idx,sizeof(frame_idx));");
        output_line("      cob_read_context_data (rtd, &father,sizeof(father));");
        if (prog->loop_counter) {
            output_line("      cob_read_context_data (rtd, &l_cntr, sizeof(l_cntr));");
        }
        output_line("      cob_close_context_file(rtd);");
        output_prefix();
        output("      ");
        output_integer(prog->cb_context_code);
        output_line(" = rtd->cob_context_info.cob_context_mode & COB_CONTEXT_CONTINUE ? 0 : 1;");
        output_line("      rtd->cob_context_info.cob_context_mode =  father ? rtd->cob_context_info.cob_context_mode : COB_CONTEXT_NONE;");

        output_line("      frame_ptr = &frame_stack[frame_idx];");
        if (cb_flag_debuginfo) {
            output_line("      module->debug_stack_frame_curpos = &cob_debug_stack_frame[frame_idx];");
        }
        output_line("      break;");
        output_line("  case COB_CONTEXT_SAVE:");
        output_prefix();
        output("      ");
        output_integer(prog->cb_context_code);
        output_line(" = rtd->cob_context_info.cob_context_mode & COB_CONTEXT_CONTINUE ? 0 : 1;");
        output_line("      father = rtd->cob_context_info.cob_next_context_father;");
        output_line("      frame_idx = (frame_ptr - frame_stack);");
        for (blp = base_cache; blp; blp = blp->next) {
            if (strcmp(blp->curr_prog, excp_current_program_id) == 0) {
                output_line("      cob_write_context_data (rtd, %s%d, %d);", CB_PREFIX_BASE, blp->f->id, blp->f->size);


            }
        }
        for (locptr = local_cache; locptr; locptr = locptr->next) {
            output_line("      cob_write_context_data (rtd, %s%d, %d);", CB_PREFIX_BASE, locptr->f->id, locptr->f->size);
        }
        output_line("      cob_write_context_data (rtd, frame_stack,sizeof(frame_stack));");
        output_line("      cob_write_context_data (rtd, &frame_idx,sizeof(frame_idx));");
        output_line("      cob_write_context_data (rtd, &father,sizeof(father));");
        if (prog->loop_counter) {
            output_line("      cob_write_context_data (rtd, &l_cntr, sizeof(l_cntr));");
        }
        output_line("      cob_close_context_file(rtd);");
        output_line("      rtd->cob_context_info.cob_next_context_father = rtd->current_module->next ? 1 : 0;");
        output_line("      if (!rtd->cob_context_info.cob_next_context_father)  { ");
        output_line("          if (rtd->cob_context_info.cob_context_mode & COB_CONTEXT_CONTINUE) {");
        output_line("              rtd->cob_context_info.cob_context_mode = COB_CONTEXT_RELOAD | COB_CONTEXT_CONTINUE;");
        output_line("              break; ");
        output_line("          } ");
        output_line("          rtd->cob_context_info.cob_context_mode = COB_CONTEXT_NONE;");
        output_line("      } ");
        output_line("      goto exit_program;");
        output_line("      break;");
        output_line("  default:");
        output_line("      cob_fatal_error (rtd, COB_FERROR_CODEGEN);");
        output_line("      break;");
        output_line("} ");

        output_indent("}");
        output_perform_exit(CB_LABEL(cb_save_context_handler));
        output_newline();
        output_line("cob_fatal_error (rtd, COB_FERROR_CODEGEN);");
        output_newline();
    }

    if (cb_flag_source_location && !cb_debugdb) {
        output_debug("\t{0, NULL, NULL, 0, 0} };\n");
    }

    totdbfield = output_debug_meminfo(prog);
    prog->flag_init_outputed = 0;
    output_initialize_sequence_proc(prog);
    output_initextern(prog);
    prog->flag_init_outputed = 1;
    output_field_init_fct(prog);

/* end internal function*/
    output_indent("}");
    output_newline();
    if (cb_debugdb) {
        debugdb_update_module(cb_debugdb, cb_debugdb_module_id, totdbfield, cur_location_idx);
    }
    close_debugdb_transaction();
}

#define ENTRY_PARAM_NONE    0
#define ENTRY_PARAM_BASE    1
#define ENTRY_PARAM_READ_STORE   2
#define ENTRY_PARAM_STORE_ARRAY   3
static void
output_entry_function_param(cb_tree l, int gencode, int name_only, int base_param_type, const char *base_prefix) {
    struct cb_field     *f;
    char *prefix = (char *)"";

    switch (base_param_type) {
    case ENTRY_PARAM_STORE_ARRAY:
    case ENTRY_PARAM_BASE:
        prefix     = (char *)CB_PREFIX_BASE_PARAM; break;
    case ENTRY_PARAM_READ_STORE:
        prefix     = (char *)"COB_MDS cob_parm_"; base_prefix = ""; break;
    }
    f = cb_field(CB_VALUE(l));
    switch (CB_CALL_BY(CB_PURPOSE_INT(l))) {
    case CB_CALL_BY_VALUE:
        if (base_param_type <= ENTRY_PARAM_BASE) {
            if ((f->usage == CB_USAGE_POINTER ||
                 f->usage == CB_USAGE_PROGRAM_POINTER)) {
                if (!name_only) {
                    output("unsigned char *");
                }
                if (gencode) {
                    output("%sptr_%d", prefix,  f->id);
                }
                break;
            }
            if (CB_TREE_CLASS(CB_VALUE(l)) == CB_CLASS_NUMERIC) {
                if (!name_only) {
                    if (CB_SIZES(l) & CB_SIZE_UNSIGNED) {
                        output("unsigned ");
                    }
                    switch (CB_SIZES_INT(l)) {
                    case CB_SIZE_1:
                        output("char");
                        break;
                    case CB_SIZE_2:
                        output("short");
                        break;
                    case CB_SIZE_4:
                        output("int");
                        break;
                    case CB_SIZE_8:
                        output("long long");
                        break;
                    }
                }
                if (gencode) {
                    output(" %si_%d", prefix, f->id);
                }
                break;
            }
        } else {
            switch (base_param_type) {
            case ENTRY_PARAM_READ_STORE:
                prefix     = (char *)"COB_MDS cob_parm_"; base_prefix = ""; break;
            case ENTRY_PARAM_STORE_ARRAY:
                prefix     = (char *)"COB_MDS store_cob_parm_"; base_prefix = ""; break;
            }

        }
        /* Fall through */
    case CB_CALL_BY_REFERENCE:
    case CB_CALL_BY_DEFAULT:
    case CB_CALL_BY_CONTENT:
    case CB_CALL_BY_DESCRIPTOR:
        if (!name_only) {
            output("unsigned char *");
        }
        if (gencode) {
            output(" %s%s%d", prefix, base_prefix, f->id);
        }
        break;
    }
}


static void
output_entry_function_saveparam(struct cb_entry *entry) {
    cb_tree             using_list;
    cb_tree             l;
    struct cb_field     *f;
    int i;
    int need_copy = 0;

    using_list = entry->using_list;
    for (i = 0, l = using_list; l; l = CB_CHAIN(l), i++) {
        f = cb_field(CB_VALUE(l));
        output("unsigned char * %s%i = ", CB_PREFIX_BASE, f->id);
        output_entry_function_param(l, 1, 1, ENTRY_PARAM_STORE_ARRAY, CB_PREFIX_BASE);
        output(";\n");
        if (CB_CALL_BY(CB_PURPOSE_INT(l)) == CB_CALL_BY_VALUE) {
            need_copy = 1;
        }
    }
    if (need_copy) {
        using_list = cb_list_reverse(using_list);
        output_line("switch (rtd->cob_call_params) {");
        for (i--, l = using_list; l; l = CB_CHAIN(l), i--) {
            if (CB_CALL_BY(CB_PURPOSE_INT(l)) == CB_CALL_BY_VALUE) {
                output("   case %d: (*(", i);
                output_entry_function_param(l, 0, 0, ENTRY_PARAM_NONE, CB_PREFIX_BASE);
                output(" *)(");
                output_entry_function_param(l, 1, 1, ENTRY_PARAM_STORE_ARRAY, CB_PREFIX_LINKAGE);
                output(")) = ");
                output_entry_function_param(l, 1, 1, ENTRY_PARAM_BASE, CB_PREFIX_BASE);
                output(";\n");
            }
        }
        output("}\n");
        cb_list_reverse(using_list);
    }
}

static void
output_entry_function1(struct cb_program *prog, struct cb_entry *entry,
                       cb_tree parameter_list, const int gencode,
                       const unsigned char  *entry_name) {

    cb_tree             using_list;
    cb_tree             l, l1, l2;
    struct cb_label     *label = NULL;
    struct cb_field     *f;
    int                 parmnum, i;
    int                 is_sticky = (cb_flag_safe_linkage || cb_sticky_linkage == CB_STICKY_VARIABLE);

    using_list = entry->using_list;
    l = entry->label;
    if (l && CB_LABEL_P(l)) {
        label = CB_LABEL(l);
    }
#ifdef  _MSC_VER
    if (!gencode) {
        output("__declspec(dllexport) ");
    }
#endif
    output_rtncode_size();

    if (label && (label->entry_call_convesion & CB_CALL_STDCALL)) {
        output(" __stdcall");
    }
    output(" %s (", entry_name);
    if (prog->flag_chained) {
        using_list = NULL;
        parameter_list = NULL;
    }
    if (!gencode && !using_list) {
        output("void);\n");
        return;
    }
    parmnum = 0;
    for (l = using_list; l; l = CB_CHAIN(l), parmnum++) {
        output_entry_function_param(l, gencode, 0, is_sticky ? ENTRY_PARAM_BASE : ENTRY_PARAM_NONE, CB_PREFIX_BASE);
        if (CB_CHAIN(l)) {
            output(", ");
        }
    }
    if (gencode) {
        output(")\n");
    } else {
        output(");\n");
        return;
    }
    output("{\n");

    /*CIT*/
    if (is_sticky && parmnum) {
        output_rtd_init(0);
        output_entry_function_saveparam(entry);
        output_line("if (rtd->cob_call_params < %d) {", parmnum);
        output_line("  switch (rtd->cob_call_params) {");
        for (i = 0, l = using_list; l; l = CB_CHAIN(l), i++) {
            f = cb_field(CB_VALUE(l));
            output_line("  case %d:", i);
            output("       %s%d = ", CB_PREFIX_BASE, f->id);
            if (cb_sticky_linkage == CB_STICKY_VARIABLE) {
                output_entry_function_param(l, 1, 1, ENTRY_PARAM_READ_STORE, CB_PREFIX_LINKAGE);
                output(";\n");
            } else {
                output("NULL;\n");
            }
        }
        output_line("  }");
        output_line("}");
    }
    /*
    for ( l1 = parameter_list; l1; l1 = CB_CHAIN (l1) ) {
        for ( l2 = using_list; l2; l2 = CB_CHAIN (l2) ) {
            if ( strcasecmp (cb_field (CB_VALUE (l1))->name,
                             cb_field (CB_VALUE (l2))->name) == 0 ) {
                f = cb_field (CB_VALUE (l2));
                if ( CB_PURPOSE_INT (l2) == CB_CALL_BY_VALUE &&
                     (f->usage == CB_USAGE_POINTER ||
                      f->usage == CB_USAGE_PROGRAM_POINTER) ) {
                    output ("  *ptr_%d = %s%d;\n",
                            f->id, CB_PREFIX_BASE, f->id);
                }
            }
        }
    }*/
    output("  return %s (%d", prog->main_entry_name, progid);
    for (l1 = parameter_list; l1; l1 = CB_CHAIN(l1)) {
        for (l2 = using_list; l2; l2 = CB_CHAIN(l2)) {
            if (strcasecmp(cb_field(CB_VALUE(l1))->name,
                           cb_field(CB_VALUE(l2))->name) == 0) {
                f = cb_field(CB_VALUE(l2));
                switch (CB_CALL_BY(CB_PURPOSE_INT(l2))) {
                case CB_CALL_BY_VALUE:
                    if (!is_sticky) {
                        if (f->usage == CB_USAGE_POINTER ||
                            f->usage == CB_USAGE_PROGRAM_POINTER) {
                            output(", (unsigned char *)&ptr_%d", f->id);
                            break;
                        } else if (CB_TREE_CLASS(CB_VALUE(l2)) == CB_CLASS_NUMERIC) {
                            output(", (unsigned char *)&i_%d", f->id);
                            break;
                        }
                    }
                    /* Fall through */
                case CB_CALL_BY_REFERENCE:
                case CB_CALL_BY_DEFAULT:
                case CB_CALL_BY_CONTENT:
                case CB_CALL_BY_DESCRIPTOR:
                    output(", %s%d", CB_PREFIX_BASE, f->id);
                    break;
                }
                break;
            }
        }
        if (l2 == NULL) {
            output(", NULL");
        }
    }
    output(");\n");
    output("}\n\n");
}


static void
output_entry_function(struct cb_program *prog,
                      struct cb_entry *entry,
                      cb_tree parameter_list, const int gencode) {

    const unsigned char  *entry_name;
    char  *p;
    if (!entry->no_entry_function) {
        entry_name = CB_LABEL(entry->label)->name;
        output_entry_function1(prog, entry, parameter_list, gencode, entry_name);
        p = strdup((char *)entry_name);
        cob_strupper(p);
        if (strcmp((char *)entry_name, p) != 0) {
            output_entry_function1(prog, entry, parameter_list, gencode, (unsigned char *)p);
        }
        free(p);
    }
    if (gencode) {
        progid++;
    }
}

/*CIT*/
/*
static void
output_main_function (struct cb_program *prog)
{
    cb_main_generated = 1;
    output_line ("int");
    output_line ("main (int argc, char **argv)");
    output_indent ("{");
    output_line ("cob_init (argc, argv);");
    output_line ("cob_stop_run (%s ());", prog->program_id);
    output_indent ("}\n");
}

*/

void output_source_header(FILE *f, char *locbuff, char *source_file) {
    int i;
    if (f) {
        fprintf(f, "/* Generated by            cobc %s.%d */\n",
                PACKAGE_VERSION, PATCH_LEVEL);
        fprintf(f, "/* Generated from          %s */\n", source_file);
        if (locbuff) {
            fprintf(f, "/* Generated at            %s */\n", locbuff);
        }
        fprintf(f, "/* Cobol-IT build date     %s */\n", cb_build_stamp);
        fprintf(f, "/* Compile command         ");
        for (i = 0; i < cb_saveargc; i++) {
            fprintf(f, "%s ", cb_saveargv[i]);
        }
        fprintf(f, "*/\n\n");
    }
}

static int field_cache_cmp(void *mp1, void *mp2) {
    struct field_list   *fl1;
    struct field_list   *fl2;
    int         ret;

    fl1 = (struct field_list *)mp1;
    fl2 = (struct field_list *)mp2;
    ret = strcasecmp(fl1->curr_prog, fl2->curr_prog);
    if (ret) {
        return (ret);
    }
    return (fl1->f->id - fl2->f->id);
}

static int base_cache_cmp(void *mp1, void *mp2) {
    struct base_list    *fl1;
    struct base_list    *fl2;

    fl1 = (struct base_list *)mp1;
    fl2 = (struct base_list *)mp2;
    return (fl1->f->id - fl2->f->id);
}

/* Sort a structure linked list in place */
/* Assumed that "next" is first item in structure */
static void*
list_cache_sort(void *inlist, int (*cmpfunc)(void *mp1, void *mp2)) {
    struct sort_list    *p;
    struct sort_list    *q;
    struct sort_list    *e;
    struct sort_list    *tail;
    struct sort_list    *list;
    int         insize;
    int         nmerges;
    int         psize;
    int         qsize;
    int         i;

    if (!inlist) {
        return (NULL);
    }
    list = (struct sort_list *)inlist;
    insize = 1;
    while (1) {
        p = list;
        list = NULL;
        tail = NULL;
        nmerges = 0;
        while (p) {
            nmerges++;
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
                q = q->next;
                if (!q) {
                    break;
                }
            }
            qsize = insize;
            while (psize > 0 || (qsize > 0 && q)) {
                if (psize == 0) {
                    e = q;
                    q = q->next;
                    qsize--;
                } else if (qsize == 0 || !q) {
                    e = p;
                    p = p->next;
                    psize--;
                } else if ((*cmpfunc)(p, q) <= 0) {
                    e = p;
                    p = p->next;
                    psize--;
                } else {
                    e = q;
                    q = q->next;
                    qsize--;
                }
                if (tail) {
                    tail->next = e;
                } else {
                    list = e;
                }
                tail = e;
            }
            p = q;
        }
        tail->next = NULL;
        if (nmerges <= 1) {
            return(void *)list;
        }
        insize *= 2;
    }
}

/*
static void
output_userdefined_function (void) {
    struct cb_intrinsic_table *c;
    int i;
    cb_tree l = NULL;
    char a[100];

    push_output_target(cb_local_function_file);
    c = userdefined_functions;
    while ( c ) {
        if ( c->args >= 0 ) {
            output ("\nstatic cob_field * \n%s (COB_RTD", c->intr_routine);
            for ( i = 0; i < c->args; i++ ) {
                output (", cob_field *fl%d", i);
                sprintf (a,"fl%d->data", i);
                l = cb_list_append(l,cb_build_list(cb_int(CB_CALL_BY_REFERENCE), make_constant(CB_CATEGORY_UNKNOWN, a), NULL));
            }
            output (") {\n");
            output ("  rtd->return_field = NULL;\n");
            output_call(CB_CALL(cb_build_call(cb_build_alphanumeric_literal((unsigned char*)c->intr_routine, strlen(c->intr_routine), 0), l,
                                      NULL, NULL,NULL, 0,CB_CALL_RETURN_FIELD_ADDR,NULL)));
            output ("  return rtd->return_field;\n");
            output ("}\n");
        }
        c = c->next;
    }
    pop_output_target();
}
*/
static int
loc_strcmp(const char *a, const char *b) {
    if (a && b) {
        return strcmp(a, b);
    }
    return 0;
}

static void
generate_code(struct cb_program *prog, int nested) {
    int                 i;
    cb_tree             l;
    cb_tree             parameter_list;
    struct cb_program   *cp;
    cb_tree             l1;
    cb_tree             l2;
    time_t              loctime;
    char                locbuff[48];

    suspend_debug_location = 0;
    current_codegen_prog = prog;
    param_id = 0;
    stack_id = 0;
    num_cob_fields = 0;
    num_cob_bitsarray_fields = 0;
    progid = 0;
    loop_counter = 0;
    output_indent_level = 0;
    last_line = 0;
    needs_exit_prog = 0;
    gen_custom = 0;
    gen_unifunc_cnt = 0;
    label_cache = NULL;
    local_cache = NULL;
    thru_label_cache = NULL;
    cb_call_num = 0;
    parameter_list = NULL;
    excp_current_source_name = prog->orig_source_name;
    excp_current_program_id = prog->program_id;
    excp_current_section = NULL;
    excp_current_paragraph = NULL;
    memset((char *)i_counters, 0, sizeof(i_counters));

    output_target[output_target_idx] = yyout;

    if (cb_flag_index_optimize) {
        /*initialize*/
        initialize_index_buffer();
    }
    if (!nested) {
        src_file_cache = NULL;
        gen_ebcdic = 0;
        gen_ebcdic_ascii = 0;
        gen_full_ebcdic = 0;
        gen_native = 0;
        attr_cache = NULL;
        base_cache = NULL;
        literal_cache = NULL;
        field_cache = NULL;

        loctime = time(NULL);
        strftime(locbuff, sizeof(locbuff) - 1, "%b %d %Y %Z %H:%M.%S00",
                 localtime(&loctime));
        output_source_header(output_target[output_target_idx], locbuff, cb_main_source_file);
        output_source_header(cb_storage_file, locbuff, cb_main_source_file);

        for (cp = prog; cp; cp = cp->next_program) {
            output_source_header(cp->local_storage_file, locbuff, cb_main_source_file);
            output_source_header(cp->local_register_file, locbuff, cb_main_source_file);
            output_source_header(cp->static_storage_file, locbuff, cb_main_source_file);
            cb_set_field_variable_address_cache(cp->working_storage);
            cb_set_field_variable_address_cache(cp->local_storage);
            cb_set_field_variable_address_cache(cp->linkage_storage);
            cb_set_field_variable_address_cache(cp->screen_storage);
        }

        output_storage("/* Frame stack declaration */\n");
        output_storage("struct cob_frame {\n");
        output_storage("\tint\tperform_through;\n");
        if (!cb_flag_goto_gcc) {
            output_storage("\tint\treturn_address;\n");
        } else {
            output_storage("\tvoid\t*return_address;\n");
        }
        output_storage("};\n\n");
        output_storage("/* Union for CALL statement */\n");
        output_storage("union cob_call_union {\n");
        output_storage("\tvoid *    (*funcptr)();\n");
        output_storage("\tint       (*funcint)();\n");
        output_storage("\tlong long (*funcll)();\n");
        output_storage("\tshort     (*funcshort)();\n");
        output_storage("\tvoid       *func_void;\n");
#ifdef _WIN32
        output_storage("\tvoid *(__stdcall *funcptr_stdcall)();\n");
        output_storage("\tint  (__stdcall *funcint_stdcall)();\n");

#endif
        output_storage("};\n");

        output("#define  __USE_STRING_INLINES 1\n");
#ifdef  _XOPEN_SOURCE_EXTENDED
        output("#ifndef        _XOPEN_SOURCE_EXTENDED\n");
        output("#define  _XOPEN_SOURCE_EXTENDED 1\n");
        output("#endif\n");
#endif
        output("#include <stdio.h>\n");
        output("#include <stdlib.h>\n");
        output("#include <string.h>\n");
        output("#include <math.h>\n");
#ifdef  WORDS_BIGENDIAN
        output("#define WORDS_BIGENDIAN 1\n");
#endif
#ifdef  HAVE_BUILTIN_EXPECT
        output("#define HAVE_BUILTIN_EXPECT\n");
#endif
        if (optimize_flag) {
            output("#define COB_LOCAL_INLINE\n");
        }
        output("#include <libcob.h>\n\n");

        output("#define COB_SOURCE_FILE        ");
        output_string((unsigned char *)cb_main_source_file, (int)strlen(cb_main_source_file));
        output("\n");
        output("#define COB_BUILD_STAMP        \"%s\"\n", cb_build_stamp);
        output("#define COB_PACKAGE_VERSION    \"%s\"\n", PACKAGE_VERSION);
        output("#define COB_PATCH_LEVEL        %d\n\n", PATCH_LEVEL);
        output("/* Global variables */\n");

        output_storage("/* Generated from %s by cobc %s.%d (Build date - %s %s) */\n\n",
                       cb_main_source_file, PACKAGE_VERSION, PATCH_LEVEL,
                       __DATE__, __TIME__);

        output_debug("/* Generated from %s by cobc %s.%d (Build date - %s %s) */\n\n",
                     cb_main_source_file, PACKAGE_VERSION, PATCH_LEVEL,
                     __DATE__, __TIME__);


        output_newline();
        if (cb_flag_profiling) {
            cob_enterprise_output_profiling("/* Generated from %s by cobc %s.%d (Build date - %s %s) */\n\n",
                                            cb_main_source_file, PACKAGE_VERSION, PATCH_LEVEL,
                                            __DATE__, __TIME__);
        }
        output_newline();
        if (cb_flag_profiling) {
            output("/* Profiling storage */\n");
            output("#include \"%s\"  \n", cb_profiling_file_name);
        }
        output("/* Source variables */\n");
        output("#include \"%s\"\n\n", cb_storage_file_name);

        output("#include \"%s\"  \n", cb_debug_file_name);
        if (cb_ccmap_file) {
            output("#include \"%s\"  \n", cb_ccmap_file_name);
        }
        output_newline();

        for (cp = prog; cp; cp = cp->next_program) {
            if (cp->gen_ptrmanip) {
                output("static void\n");
                output("cob_pointer_manip (COB_RTD, cob_field *f1, cob_field *f2, size_t addsub)\n");
                output("{\n");
                output("        unsigned char   *tmptr;\n");
                output("        memcpy (&tmptr, f1->data, sizeof(void *));\n");
                output("        if (addsub) {\n");
                output("                tmptr -= cob_get_int (rtd, f2);\n");
                output("        } else {\n");
                output("                tmptr += cob_get_int (rtd, f2);\n");
                output("        }\n");
                output("        memcpy (f1->data, &tmptr, sizeof(void *));\n");
                output("}\n\n");
                break;
            }
        }
        output("#include \"%s\"\n", cb_local_function_file_name);

        output_storage("/* PROGRAM-ID : %s */\n\n", prog->orig_source_name);
        output_debug("/* PROGRAM-ID : %s */\n\n", prog->orig_source_name);
        if (cb_flag_profiling) {
            cob_enterprise_output_profiling("/* PROGRAM-ID : %s */\n\n", prog->orig_source_name);
        }
        output("/* Function prototypes */\n\n");
        for (cp = prog; cp; cp = cp->next_program) {
            /* Build parameter list */
            for (l = cp->entry_list; l; l = CB_CHAIN(l)) {
                struct cb_entry *e = CB_ENTRY(CB_VALUE(l));
                for (l1 = e->using_list; l1; l1 = CB_CHAIN(l1)) {
                    for (l2 = cp->parameter_list; l2; l2 = CB_CHAIN(l2)) {
                        if (strcasecmp(cb_field(CB_VALUE(l1))->name,
                                       cb_field(CB_VALUE(l2))->name) == 0) {
                            break;
                        }
                    }
                    if (l2 == NULL) {
                        cp->parameter_list = cb_list_add(cp->parameter_list, CB_VALUE(l1));
                    }
                }
            }
            /* CIT 
            if (cp->gen_main) {
                output ("int %s ();\n", cp->program_id);
            } else {
                for (l = cp->entry_list; l; l = CB_CHAIN (l)) {
                    output_entry_function (cp, l, cp->parameter_list, 0);
                }
            }*/
            for (l = cp->entry_list; l; l = CB_CHAIN(l)) {
                struct cb_entry *e = CB_ENTRY(CB_VALUE(l));
                output_entry_function(cp, e, cp->parameter_list, 0);
            }
            output("static ");
            output_rtncode_size();
            output(" %s (const int", cp->main_entry_name);
            if (!cp->flag_chained) {
                for (l = cp->parameter_list; l; l = CB_CHAIN(l)) {
                    output(", unsigned char *");
                }
            }
            output(");\n");
        }
        output("\n");
        /* EXTFH EXTSM Prototype*/
        if (cb_extfh_name && !is_entry_name(cb_extfh_name)) {
            output("/* EXTFH function prototypes */\n");
            output("COB_DLL_EXPIMP int %s (void* op, void* FCD);\n\n", cb_extfh_name);
        }
        if (cb_isam_extfh && !is_entry_name(cb_isam_extfh) &&
            loc_strcmp(cb_isam_extfh, cb_extfh_name)) {
            output("/* EXTFH function prototypes */\n");
            output("COB_DLL_EXPIMP int %s (void* op, void* FCD);\n\n", cb_isam_extfh);
        }
        if (cb_flat_extfh && !is_entry_name(cb_flat_extfh) &&
            loc_strcmp(cb_flat_extfh, cb_extfh_name) &&
            loc_strcmp(cb_flat_extfh, cb_isam_extfh)) {
            output("/* EXTFH function prototypes */\n");
            output("COB_DLL_EXPIMP int %s (void* op, void* FCD);\n\n", cb_flat_extfh);
        }
//      if (cb_flag_extsm && !is_entry_name(cb_extsm_name)) {
//          output("/* EXTSM function prototypes */\n");
//          output("COB_DLL_EXPIMP int %s (void* op, void* FCD);\n\n", cb_extsm_name);
//      }


    }

    /* Class-names */
    if (!prog->nested_level && prog->class_name_list) {
        output("/* Class names */\n");
        for (l = prog->class_name_list; l; l = CB_CHAIN(l)) {
            output_class_name_definition(CB_CLASS_NAME(CB_VALUE(l)));
        }
    }
    prog->flag_is_part_of_nested = nested || prog->next_program;

    /* Functions */
    if (!nested) {
        output("/* Functions */\n\n");
    }
    for (l = prog->entry_list; l; l = CB_CHAIN(l)) {
        struct cb_entry *e = CB_ENTRY(CB_VALUE(l));
        output_entry_function(prog, e, prog->parameter_list, 1);
    }

    /* Cobol-IT */
    cur_location_idx = 0;
    cur_profiling_idx = 0;
    if (cb_flag_source_location) {
        if (!cb_debugdb) {
            output_debug("static cob_lines_debug_info %s_lines_debug[] = {\n", prog->program_id);
            init_output_location(prog->orig_source_name);
        } else {
            cur_location_idx++;
        }
    }
    if (cb_flag_profiling) {
        cob_enterprise_output_profiling("static struct cob_paragraph_profiling_info %s_%s[] = {\n", prog->program_id, PROFILING_SUFFIX);
        cob_enterprise_output_profiling("\t{NULL, 0, 0, 0}, \n");
    }
    output_ccmap_init(prog);
    output_internal_function(prog, prog->parameter_list);

    /*
    if ( cb_flag_source_location ) {
        output_debug ("\t{0, NULL, NULL, 0, 0} };\n"); 
    } 
    */
    if (cb_flag_profiling) {
        cob_enterprise_output_profiling("\t{NULL, 0, 0, 0} };\n");
    }
    output_ccmap_finish(prog);
    if (!prog->next_program) {
        output("/* End functions */\n\n");
    }

    if (gen_native || gen_full_ebcdic || gen_ebcdic_ascii || prog->alphabet_name_list) {
        (void)lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
    }

    output_target[output_target_idx] = cb_storage_file;

    /* Program local stuff */

    for (i = 0; i < COB_MAX_SUBSCRIPTS; i++) {
        if (i_counters[i]) {
            output_local_register("int\t\ti%d;\n", i);
        }
    }

    if (num_cob_bitsarray_fields) {
        output_local_register("\n/* Local cob_bitsarray_field items */\n");
        for (i = 0; i < num_cob_bitsarray_fields; i++) {
            output_local_register("cob_bitsarray_field\tfba%d;\n", i);
        }
        output_local_register("\n");
    }
    if (num_cob_fields) {
        output_local_register("\n/* Local cob_field items */\n");
        for (i = 0; i < num_cob_fields; i++) {
            output_local_register("cob_field\tf%d;\n", i);
        }
        output_local_register("\n");
    }


    /*output_debug_meminfo(prog);*/

    /* Skip to next nested program */
    if (prog->next_program) {
        generate_code(prog->next_program, 1);
    }
    if (!nested) {
        output_local_function("/* Init field code */\n");
        output_local_function("static void\n");
        output_local_function("init_fields (COB_RTD, mds_t *mds) \n");
        output_local_function("{\n");
        for (cp = prog; cp; cp = cp->next_program) {
            output_local_function("  %s_init_fields (rtd, mds); \n", cp->program_id);
        }
        output_local_function("}\n");
        /*output_userdefined_function ();*/
    }
}


static void
generate_storage_array(struct base_list *blp, const char *prefix_mds, int enable_static) {
    /* 
    const char          *attribute;
    const char          *store_type;
    const char          *indirect;
    int                  sz;
    int                  att =0;
 
    #ifdef HAVE_ATTRIBUTE_ALIGNED
    att =1;
    #ifdef HAVE_ATTRIBUTE_ALIGNED8
    att =2;
    #endif
    #endif
    attribute = "__attribute__((aligned))";
    store_type = "unsigned char";
    indirect = "";
    sz = blp->f->memory_size;
    if (enable_static && ((att) || cb_flag_gen_gcc) ) {
        if ( cb_flag_align_8 ) {
            if ( (att > 1) || cb_flag_gen_gcc ) {
                attribute = "__attribute__((aligned(8)))";
            } else {
                sz = blp->f->memory_size / sizeof(long long);
                if ( blp->f->memory_size % sizeof(long long) ) {
                    sz++;
                }
                store_type = "static long long";
                attribute = "";
                indirect = "il";
            }
        }
    } else {
        attribute = "";
        if ( cb_flag_align_8 ) {
            sz = blp->f->memory_size / sizeof(long long);
            if ( blp->f->memory_size % sizeof(long long) ) {
                sz++;
            }
            store_type = "long long";
            indirect = "il";
        } else {
            sz = blp->f->memory_size / sizeof(int);
            if ( blp->f->memory_size % sizeof(int) ) {
                sz++;
            }
            store_type = "int";
            indirect = "i";
        }
    }
    output_storage ("        %s %s %s%s%d[%d] %s %s ; ", 
                    enable_static ? "static" : "", 
                    store_type, indirect,
                    CB_PREFIX_BASE, blp->f->id, sz, attribute, 
                    enable_static ? "= {0}" : "" );
    output_storage ("\t/ * %s [%d] * /\n", blp->f->name, blp->f->memory_size);
    if (indirect[0] != 0) {
        output_storage ("        #define %s%d ((unsigned char*)(%s %s%s%d ))\n",
                        CB_PREFIX_BASE, blp->f->id, 
                        prefix_mds, indirect,
                        CB_PREFIX_BASE, blp->f->id);
    }
    */
    int                  sz;
    const char          *store_type;

    sz = blp->f->memory_size <= 0 ? 1 : blp->f->memory_size;
    if (cb_flag_align_8) {
        store_type = "long long";
    } else {
        store_type = "int";
    }
    output_storage("        %s union { %s i ; unsigned char c[%d] ;} u%s%d %s; ",
                   enable_static ? "static" : "",
                   store_type, sz + 1, CB_PREFIX_BASE, blp->f->id,
                   enable_static ? "= {0}" : "");

    output_storage("\t/* %s */\n", blp->f->name);
    output_storage("        #define %s%d (%s u%s%d.c)\n",
                   CB_PREFIX_BASE, blp->f->id,
                   prefix_mds,
                   CB_PREFIX_BASE, blp->f->id);
}

static void
output_constant(void) {
    struct literal_list *m;

    output("/* Constants */\n");
    for (m = literal_cache; m; m = m->next) {
        output("static cob_field %s%d\t= ", CB_PREFIX_CONST, m->id);
        output_field(m->x);
        output(";\n");
    }
    output("\n");
}

static void
generate_storage(struct cb_program *prog) {
    int                 i;
    struct attr_list    *j;
    struct field_list   *k;
    struct call_list    *clp;
    struct base_list    *blp;
    unsigned char       *s;
    struct cb_program   *cp;

    /*Module data Storage macro*/
    if (cb_flag_thread_safe) {
        output_storage("#define COB_MDS mds->\n");
        output_storage("#define COB_MDS_REF mds\n");
    } else {
        output_storage("#define COB_MDS cob_mds.\n");
        output_storage("#define COB_MDS_REF &cob_mds\n");
    }

    /* Finalize the storage file */

    output_target[output_target_idx] = cb_storage_file;
    if (call_cache) {
        output_storage("\n/* Optimized Call pointers */\n");
        for (clp = call_cache; clp; clp = clp->next) {
            output_storage("static union cob_call_union\tcall_%s = { NULL };\n", clp->callname);
        }
        output_storage("\n");
    }

    if (attr_cache) {
        output_storage("\n/* Attributes */\n\n");
        attr_cache = attr_list_reverse(attr_cache);
        for (j = attr_cache; j; j = j->next) {
            output_storage("static const cob_field_attr %s%d\t= ",
                           CB_PREFIX_ATTR, j->id);
            output_storage("{0x%02x, %3d, %2d, 0x%02x, ", j->type, j->digits,
                           j->scale, j->flags);
            if (j->pic) {
                output_storage("\"");
                for (s = j->pic; *s; s += 5) {
                    output_storage("%c\\%03o\\%03o\\%03o\\%03o",
                                   s[0], s[1], s[2], s[3], s[4]);
                }
                output_storage("\"");
            } else {
                output_storage("NULL");
            }
            output_storage("};\n");
        }
    }
    if (cb_file_status_map) {
        struct cb_file_status_map_list *l;
        int cnt = 0;
        output_storage("\n/* File status map */\n\n");
        l = cb_file_status_map;
        while (l) {
            l = l->next;
            cnt++;
        }
        l = cb_file_status_map;
        output_storage("static const cob_file_status_map_item file_status_map_items[] = {{%d,%d}",
                       l->cit_status, l->custome_status);
        l = l->next;
        while (l) {
            output_storage(", {%u,%u}", l->cit_status, l->custome_status);
            l = l->next;
        }
        output_storage("};\n");
        output_storage("static const cob_file_status_map file_status_map = { %d, file_status_map_items};\n\n",
                       cnt);
    }
    if (cb_crt_status_map) {
        struct cb_file_status_map_list *l;
        int cnt = 0;
        output_storage("\n/* Crt status map */\n\n");
        l = cb_crt_status_map;
        while (l) {
            l = l->next;
            cnt++;
        }
        l = cb_crt_status_map;
        output_storage("static const cob_crt_status_map_item crt_status_map_items[] = {{%u,0x%x}",
                       l->cit_status, l->custome_status);
        l = l->next;
        while (l) {
            output_storage(", {%u,0x%x}", l->cit_status, l->custome_status);
            l = l->next;
        }
        output_storage("};\n");
        output_storage("static const cob_crt_status_map crt_status_map = { %d, crt_status_map_items};\n\n",
                       cnt);
    }

    base_cache = list_cache_sort(base_cache, &base_cache_cmp);
    field_cache = list_cache_sort(field_cache, &field_cache_cmp);

    output_storage("typedef struct {\n");
    output_storage("    int initialized;");
    output_storage("    void * module_allocated_list;");
    for (cp = prog; cp; cp = cp->next_program) {
        output_storage("\n/* PROGRAM-ID : %s */\n", cp->program_id);
        output_storage("\n    /* Storage */\n");
        output_storage("    struct {\n");
        output_storage("        unsigned int unused;\n");
        if (base_cache) {
            for (blp = base_cache; blp; blp = blp->next) {
                if (blp->curr_prog == cp->program_id) {
                    if (blp->f->flag_external) {
                        /*CIT*/
                        output_storage("        unsigned char *i%s%d;", CB_PREFIX_BASE, blp->f->id);
                        output_storage("\t/* external %s */\n", blp->f->name);
                        output_storage("        #define %s%d (COB_MDS %s%s.i%s%d)\n",
                                       CB_PREFIX_BASE, blp->f->id,
                                       CB_PREFIX_BASE, cp->program_id,
                                       CB_PREFIX_BASE, blp->f->id);
                        output_storage("        int           i%s%d_init;\n", CB_PREFIX_BASE, blp->f->id);
                        output_storage("        #define %s%d_init (COB_MDS %s%s.i%s%d_init)\n",
                                       CB_PREFIX_BASE, blp->f->id,
                                       CB_PREFIX_BASE, cp->program_id,
                                       CB_PREFIX_BASE, blp->f->id);
                    } else {
                        char prefix_mds[255];
                        sprintf(prefix_mds, "COB_MDS %s%s.", CB_PREFIX_BASE, cp->program_id);
                        generate_storage_array(blp, prefix_mds, 0);
                    }
                }
            }
        }
        output_storage("\n    }%s%s ;\n", CB_PREFIX_BASE, cp->program_id);
        output_storage("\n    /* End of storage */\n\n");
        output_storage("    /* Fields */\n");
        output_storage("    struct {\n");
        output_storage("        int     initialized;\n");
        for (i = 0; i < cp->decimal_index_max; i++) {
            output_storage("        cob_decimal d%d;\n", i);
        }
        if (cb_flag_thread_safe && field_cache) {
            for (k = field_cache; k; k = k->next) {
                if (k->curr_prog == cp->program_id) {
                    output("        cob_field i%s%d", CB_PREFIX_FIELD, k->f->id);
                    output(";\t/* %s */\n", k->f->name);
                    output_storage("        #define %s%d (COB_MDS %s%s.i%s%d)\n",
                                   CB_PREFIX_FIELD, k->f->id,
                                   CB_PREFIX_FIELD, cp->program_id,
                                   CB_PREFIX_FIELD, k->f->id);
                }
            }
        }
        output_storage("\n    }%s%s ;\n", CB_PREFIX_FIELD, cp->program_id);
        output_storage("\n    /* End of fields */\n");
        output_storage("    /* Local Static */\n");
        output_storage("    struct {\n");
        output_storage("        #include \"%s\"\n", cp->static_storage_name);
        output_storage("    }%s%s ;\n", CB_PREFIX_STATIC, cp->program_id);
        output_storage("    %s%s_t %s%s;\n", CB_PREFIX_SCREEN, cp->program_id, CB_PREFIX_SCREEN, cp->program_id);
    }
    output_storage("    #include \"%s\"\n", cb_global_file_name);
    output_storage("\n} mds_t ;\n");

    /*Module initialize cache*/
    if (cb_flag_initialize_optimize) {
        output_storage("static void * initcache[%d] = {NULL};\n", initialize_cache_cnt + 1);
    }

    /*Module data Storage key*/
    if (cb_flag_thread_safe) {
        output_storage("static const char mds_key[]= \"%s %s %s \";\n", prog->program_id, __DATE__, __TIME__);
    } else {
        if (cb_flag_C_data_preinit) {
            output_storage("static mds_t cob_mds = {0};\n");
            output_storage("static int cob_mds_initialized=1;");
        } else {
            output_storage("static mds_t cob_mds;\n");
            output_storage("static int cob_mds_initialized=0;");
        }
        for (cp = prog; cp; cp = cp->next_program) {
            if (field_cache) {
                output_storage("\n/* PROGRAM-ID : %s */\n", cp->program_id);
                output_storage("\n/* Fields static (no thread safe)*/\n");
                for (k = field_cache; k; k = k->next) {
                    if (k->curr_prog == cp->program_id) {
                        if (k->x) {
                            output("static cob_field %s%d\t= {", CB_PREFIX_FIELD, k->f->id);
                            output_size_storage(k->x);
                            output(", ");
                            if (!k->f->flag_local && !k->f->flag_item_external) {
                                output_data(k->x);
                            } else {
                                output("NULL");
                            }
                            output(", ");
                            output_attr(k->x, 0);
                            output("};\t/* %s */\n", k->f->name);
                        } else {
                            output("static cob_field %s%d\t;", CB_PREFIX_FIELD, k->f->id);
                            output("\t/* %s */\n", k->f->name);
                        }
                    }
                }
            }
        }
    }


    if (literal_cache) {
        /*First time To NULL to create String cache*/
        push_output_target(NULL);
        literal_cache = literal_list_reverse(literal_cache);
        output_constant();
        pop_output_target();
    }


    if (gen_ebcdic) {
        output_storage("/* EBCDIC translate table */\n");
        output("static const unsigned char\tcob_a2e[256] = {\n");
        if (alt_ebcdic) {
            output("\t0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,\n");
            output("\t0x16, 0x05, 0x25, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\n");
            output("\t0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,\n");
            output("\t0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,\n");
            output("\t0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,\n");
            output("\t0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,\n");
            output("\t0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,\n");
            output("\t0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,\n");
            output("\t0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,\n");
            output("\t0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,\n");
            output("\t0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,\n");
            output("\t0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,\n");
            output("\t0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,\n");
            output("\t0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,\n");
            output("\t0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,\n");
            output("\t0xA7, 0xA8, 0xA9, 0xC0, 0x6A, 0xD0, 0xA1, 0x07,\n");
            output("\t0x68, 0xDC, 0x51, 0x42, 0x43, 0x44, 0x47, 0x48,\n");
            output("\t0x52, 0x53, 0x54, 0x57, 0x56, 0x58, 0x63, 0x67,\n");
            output("\t0x71, 0x9C, 0x9E, 0xCB, 0xCC, 0xCD, 0xDB, 0xDD,\n");
            output("\t0xDF, 0xEC, 0xFC, 0xB0, 0xB1, 0xB2, 0x3E, 0xB4,\n");
            output("\t0x45, 0x55, 0xCE, 0xDE, 0x49, 0x69, 0x9A, 0x9B,\n");
            output("\t0xAB, 0x9F, 0xBA, 0xB8, 0xB7, 0xAA, 0x8A, 0x8B,\n");
            output("\t0xB6, 0xB5, 0x62, 0x4F, 0x64, 0x65, 0x66, 0x20,\n");
            output("\t0x21, 0x22, 0x70, 0x23, 0x72, 0x73, 0x74, 0xBE,\n");
            output("\t0x76, 0x77, 0x78, 0x80, 0x24, 0x15, 0x8C, 0x8D,\n");
            output("\t0x8E, 0x41, 0x06, 0x17, 0x28, 0x29, 0x9D, 0x2A,\n");
            output("\t0x2B, 0x2C, 0x09, 0x0A, 0xAC, 0x4A, 0xAE, 0xAF,\n");
            output("\t0x1B, 0x30, 0x31, 0xFA, 0x1A, 0x33, 0x34, 0x35,\n");
            output("\t0x36, 0x59, 0x08, 0x38, 0xBC, 0x39, 0xA0, 0xBF,\n");
            output("\t0xCA, 0x3A, 0xFE, 0x3B, 0x04, 0xCF, 0xDA, 0x14,\n");
            output("\t0xE1, 0x8F, 0x46, 0x75, 0xFD, 0xEB, 0xEE, 0xED,\n");
            output("\t0x90, 0xEF, 0xB3, 0xFB, 0xB9, 0xEA, 0xBB, 0xFF\n");
        } else {
            /* MF */
            output("\t0x00, 0x01, 0x02, 0x03, 0x1D, 0x19, 0x1A, 0x1B,\n");
            output("\t0x0F, 0x04, 0x16, 0x06, 0x07, 0x08, 0x09, 0x0A,\n");
            output("\t0x0B, 0x0C, 0x0D, 0x0E, 0x1E, 0x1F, 0x1C, 0x17,\n");
            output("\t0x10, 0x11, 0x20, 0x18, 0x12, 0x13, 0x14, 0x15,\n");
            output("\t0x21, 0x27, 0x3A, 0x36, 0x28, 0x30, 0x26, 0x38,\n");
            output("\t0x24, 0x2A, 0x29, 0x25, 0x2F, 0x2C, 0x22, 0x2D,\n");
            output("\t0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,\n");
            output("\t0x7B, 0x7C, 0x35, 0x2B, 0x23, 0x39, 0x32, 0x33,\n");
            output("\t0x37, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D,\n");
            output("\t0x5E, 0x5F, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,\n");
            output("\t0x67, 0x68, 0x69, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,\n");
            output("\t0x70, 0x71, 0x72, 0x7D, 0x6A, 0x7E, 0x7F, 0x31,\n");
            output("\t0x34, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41,\n");
            output("\t0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,\n");
            output("\t0x4A, 0x4B, 0x4C, 0x4E, 0x4F, 0x50, 0x51, 0x52,\n");
            output("\t0x53, 0x54, 0x55, 0x56, 0x2E, 0x60, 0x4D, 0x05,\n");
            output("\t0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,\n");
            output("\t0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,\n");
            output("\t0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,\n");
            output("\t0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,\n");
            output("\t0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,\n");
            output("\t0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,\n");
            output("\t0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,\n");
            output("\t0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,\n");
            output("\t0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,\n");
            output("\t0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,\n");
            output("\t0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,\n");
            output("\t0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,\n");
            output("\t0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,\n");
            output("\t0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,\n");
            output("\t0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,\n");
            output("\t0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF\n");
        }
        output("};\n");
        output_storage("\n");
    }
    if (gen_full_ebcdic) {
        output("static const unsigned char\tcob_ebcdic[256] = {\n");
        output("\t0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,\n");
        output("\t0x16, 0x05, 0x25, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\n");
        output("\t0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,\n");
        output("\t0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,\n");
        output("\t0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,\n");
        output("\t0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,\n");
        output("\t0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,\n");
        output("\t0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,\n");
        output("\t0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,\n");
        output("\t0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,\n");
        output("\t0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,\n");
        output("\t0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,\n");
        output("\t0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,\n");
        output("\t0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,\n");
        output("\t0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,\n");
        output("\t0xA7, 0xA8, 0xA9, 0xC0, 0x6A, 0xD0, 0xA1, 0x07,\n");
        output("\t0x68, 0xDC, 0x51, 0x42, 0x43, 0x44, 0x47, 0x48,\n");
        output("\t0x52, 0x53, 0x54, 0x57, 0x56, 0x58, 0x63, 0x67,\n");
        output("\t0x71, 0x9C, 0x9E, 0xCB, 0xCC, 0xCD, 0xDB, 0xDD,\n");
        output("\t0xDF, 0xEC, 0xFC, 0xB0, 0xB1, 0xB2, 0x3E, 0xB4,\n");
        output("\t0x45, 0x55, 0xCE, 0xDE, 0x49, 0x69, 0x9A, 0x9B,\n");
        output("\t0xAB, 0x9F, 0xBA, 0xB8, 0xB7, 0xAA, 0x8A, 0x8B,\n");
        output("\t0xB6, 0xB5, 0x62, 0x4F, 0x64, 0x65, 0x66, 0x20,\n");
        output("\t0x21, 0x22, 0x70, 0x23, 0x72, 0x73, 0x74, 0xBE,\n");
        output("\t0x76, 0x77, 0x78, 0x80, 0x24, 0x15, 0x8C, 0x8D,\n");
        output("\t0x8E, 0x41, 0x06, 0x17, 0x28, 0x29, 0x9D, 0x2A,\n");
        output("\t0x2B, 0x2C, 0x09, 0x0A, 0xAC, 0x4A, 0xAE, 0xAF,\n");
        output("\t0x1B, 0x30, 0x31, 0xFA, 0x1A, 0x33, 0x34, 0x35,\n");
        output("\t0x36, 0x59, 0x08, 0x38, 0xBC, 0x39, 0xA0, 0xBF,\n");
        output("\t0xCA, 0x3A, 0xFE, 0x3B, 0x04, 0xCF, 0xDA, 0x14,\n");
        output("\t0xE1, 0x8F, 0x46, 0x75, 0xFD, 0xEB, 0xEE, 0xED,\n");
        output("\t0x90, 0xEF, 0xB3, 0xFB, 0xB9, 0xEA, 0xBB, 0xFF\n");
        output("};\n");
        i = lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
        output
            ("static cob_field f_ebcdic = { 256, (unsigned char *)cob_ebcdic, &%s%d };\n",
             CB_PREFIX_ATTR, i);
        output_storage("\n");
    }
    if (gen_ebcdic_ascii) {
        output("static const unsigned char\tcob_ebcdic_ascii[256] = {\n");
        output("\t0x00, 0x01, 0x02, 0x03, 0xEC, 0x09, 0xCA, 0x7F,\n");
        output("\t0xE2, 0xD2, 0xD3, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\n");
        output("\t0x10, 0x11, 0x12, 0x13, 0xEF, 0xC5, 0x08, 0xCB,\n");
        output("\t0x18, 0x19, 0xDC, 0xD8, 0x1C, 0x1D, 0x1E, 0x1F,\n");
        output("\t0xB7, 0xB8, 0xB9, 0xBB, 0xC4, 0x0A, 0x17, 0x1B,\n");
        output("\t0xCC, 0xCD, 0xCF, 0xD0, 0xD1, 0x05, 0x06, 0x07,\n");
        output("\t0xD9, 0xDA, 0x16, 0xDD, 0xDE, 0xDF, 0xE0, 0x04,\n");
        output("\t0xE3, 0xE5, 0xE9, 0xEB, 0x14, 0x15, 0x9E, 0x1A,\n");
        output("\t0x20, 0xC9, 0x83, 0x84, 0x85, 0xA0, 0xF2, 0x86,\n");
        output("\t0x87, 0xA4, 0xD5, 0x2E, 0x3C, 0x28, 0x2B, 0xB3,\n");
        output("\t0x26, 0x82, 0x88, 0x89, 0x8A, 0xA1, 0x8C, 0x8B,\n");
        output("\t0x8D, 0xE1, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,\n");
        output("\t0x2D, 0x2F, 0xB2, 0x8E, 0xB4, 0xB5, 0xB6, 0x8F,\n");
        output("\t0x80, 0xA5, 0x7C, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,\n");
        output("\t0xBA, 0x90, 0xBC, 0xBD, 0xBE, 0xF3, 0xC0, 0xC1,\n");
        output("\t0xC2, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,\n");
        output("\t0xC3, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,\n");
        output("\t0x68, 0x69, 0xAE, 0xAF, 0xC6, 0xC7, 0xC8, 0xF1,\n");
        output("\t0xF8, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,\n");
        output("\t0x71, 0x72, 0xA6, 0xA7, 0x91, 0xCE, 0x92, 0xA9,\n");
        output("\t0xE6, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,\n");
        output("\t0x79, 0x7A, 0xAD, 0xA8, 0xD4, 0x5B, 0xD6, 0xD7,\n");
        output("\t0x9B, 0x9C, 0x9D, 0xFA, 0x9F, 0xB1, 0xB0, 0xAC,\n");
        output("\t0xAB, 0xFC, 0xAA, 0xFE, 0xE4, 0x5D, 0xBF, 0xE7,\n");
        output("\t0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,\n");
        output("\t0x48, 0x49, 0xE8, 0x93, 0x94, 0x95, 0xA2, 0xED,\n");
        output("\t0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,\n");
        output("\t0x51, 0x52, 0xEE, 0x96, 0x81, 0x97, 0xA3, 0x98,\n");
        output("\t0x5C, 0xF0, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,\n");
        output("\t0x59, 0x5A, 0xFD, 0xF5, 0x99, 0xF7, 0xF6, 0xF9,\n");
        output("\t0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,\n");
        output("\t0x38, 0x39, 0xDB, 0xFB, 0x9A, 0xF4, 0xEA, 0xFF\n");
        output("};\n");
        i = lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
        output
            ("static cob_field f_ebcdic_ascii = { 256, (unsigned char *)cob_ebcdic_ascii, &%s%d };\n",
             CB_PREFIX_ATTR, i);
        output_storage("\n");
    }
    if (gen_native) {
        output("static const unsigned char\tcob_native[256] = {\n");
        output("\t0, 1, 2, 3, 4, 5, 6, 7,\n");
        output("\t8, 9, 10, 11, 12, 13, 14, 15,\n");
        output("\t16, 17, 18, 19, 20, 21, 22, 23,\n");
        output("\t24, 25, 26, 27, 28, 29, 30, 31,\n");
        output("\t32, 33, 34, 35, 36, 37, 38, 39,\n");
        output("\t40, 41, 42, 43, 44, 45, 46, 47,\n");
        output("\t48, 49, 50, 51, 52, 53, 54, 55,\n");
        output("\t56, 57, 58, 59, 60, 61, 62, 63,\n");
        output("\t64, 65, 66, 67, 68, 69, 70, 71,\n");
        output("\t72, 73, 74, 75, 76, 77, 78, 79,\n");
        output("\t80, 81, 82, 83, 84, 85, 86, 87,\n");
        output("\t88, 89, 90, 91, 92, 93, 94, 95,\n");
        output("\t96, 97, 98, 99, 100, 101, 102, 103,\n");
        output("\t104, 105, 106, 107, 108, 109, 110, 111,\n");
        output("\t112, 113, 114, 115, 116, 117, 118, 119,\n");
        output("\t120, 121, 122, 123, 124, 125, 126, 127,\n");
        output("\t128, 129, 130, 131, 132, 133, 134, 135,\n");
        output("\t136, 137, 138, 139, 140, 141, 142, 143,\n");
        output("\t144, 145, 146, 147, 148, 149, 150, 151,\n");
        output("\t152, 153, 154, 155, 156, 157, 158, 159,\n");
        output("\t160, 161, 162, 163, 164, 165, 166, 167,\n");
        output("\t168, 169, 170, 171, 172, 173, 174, 175,\n");
        output("\t176, 177, 178, 179, 180, 181, 182, 183,\n");
        output("\t184, 185, 186, 187, 188, 189, 190, 191,\n");
        output("\t192, 193, 194, 195, 196, 197, 198, 199,\n");
        output("\t200, 201, 202, 203, 204, 205, 206, 207,\n");
        output("\t208, 209, 210, 211, 212, 213, 214, 215,\n");
        output("\t216, 217, 218, 219, 220, 221, 222, 223,\n");
        output("\t224, 225, 226, 227, 228, 229, 230, 231,\n");
        output("\t232, 233, 234, 235, 236, 237, 238, 239,\n");
        output("\t240, 241, 242, 243, 244, 245, 246, 247,\n");
        output("\t248, 249, 250, 251, 252, 253, 254, 255\n");
        output("};\n");
        i = lookup_attr(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL, 0);
        output
            ("static cob_field f_native = { 256, (unsigned char *)cob_native, &%s%d };\n",
             CB_PREFIX_ATTR, i);
        output_storage("\n");
    }
    /*CIT*/
    if (src_file_cache) {
        struct string_list *p;
        p = src_file_cache;
        output("/* Constant Strings */\n");
        while (p) {
            output("static const unsigned char %s%d[]= ", CB_PREFIX_STR, p->id);
            output_string(p->data, p->size);
            output(";\n");
            p = p->next;
        }
    }
    if (literal_cache) {
        /* Now the actual storage*/
        output_constant();
    }


}

void
codegen(struct cb_program *prog) {
    initialize_cache_cnt = 0;
    generate_code(prog, 0);
    generate_storage(prog);
    if (cb_flag_please_include_main) {
        output_main_symbol(yyout, (char *)prog->program_id, prog->flag_chained ? 0 :
                           cb_list_length(CB_ENTRY(CB_VALUE(prog->entry_list))->using_list));

        cb_flag_please_include_main = 0;
    }



}

static void
output_main_symbol_1(FILE *file, char *callname, int paramcnt) {
    if (paramcnt >= 2) {
#ifdef  WORDS_BIGENDIAN
        fprintf(file, "    cob_stop_run (rtd, %s (argc,argv", callname);
#else
        fprintf(file, "    cob_stop_run (rtd, %s (COB_BSWAP_32_CONSTANT(argc),argv", callname);
#endif
        paramcnt -= 2;
    } else if (paramcnt == 1) {
        fprintf(file, "    cob_stop_run (rtd, %s (cob_linkage_command_line(rtd)", callname);
        paramcnt--;
    } else {
        fprintf(file, "    cob_stop_run (rtd, %s (", callname);
    }
    while (paramcnt--) {
        fprintf(file, ", NULL");
    }
    fprintf(file, "));\n");
}

void
output_main_symbol(FILE *file, char *callname, int paramcnt) {
    fprintf(file, "int\n");
    fprintf(file, "main (int argc, char **argv)\n");
    fprintf(file, "{\n");
    fprintf(file, "    /* runtime data */");
    /*fprintf(file, "    COB_RTD = cob_get_rtd_main(%d);", cb_flag_thread_safe);*/
    fprintf(file, "    COB_RTD = cob_get_rtd();");
    fprintf(file, "    cob_init (rtd, argc, argv);\n");
    output_main_symbol_1(file, callname, paramcnt);
    fprintf(file, "}\n");
#ifdef  _MSC_VER
    fprintf(file, "int __stdcall WinMain (void*h, void*ph, const char *c, int wc)\n");
    fprintf(file, "{\n");
    fprintf(file, "    int argc; char **argv;");
    fprintf(file, "    /* runtime data */");
    /*fprintf(file, "    COB_RTD = cob_get_rtd_main(%d);", cb_flag_thread_safe);*/
    fprintf(file, "    COB_RTD = cob_get_rtd();");
    fprintf(file, "    cob_init (rtd, 0, NULL);\n");
    fprintf(file, "    cob_set_command_line (rtd, \"%s\", c);\n", callname);
    fprintf(file, "    argc = rtd->cob_argc;");
    fprintf(file, "    argv = rtd->cob_argv;");
    output_main_symbol_1(file, callname, paramcnt);
    fprintf(file, "}\n");
#endif
    cb_main_generated = 1;;
}



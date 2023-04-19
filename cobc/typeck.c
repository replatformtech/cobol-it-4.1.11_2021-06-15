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
#include <time.h>
#include <assert.h>
#ifdef HAVE_SYS_TIME_H
    #include <sys/time.h>
#endif
#ifdef  _WIN32
    #define WINDOWS_LEAN_AND_MEAN
    #include <windows.h>
#endif 

#ifdef  HAVE_LOCALE_H
    #include <locale.h>
#endif 

#include <libcob.h>

#include "cobc.h"
#include "tree.h"
#include "coblocal.h"

static const unsigned long long bitmask [] = {
    0x0000000000000000LL,
    0x0000000000000001LL,
    0x0000000000000003LL,
    0x0000000000000007LL,
    0x000000000000000fLL,
    0x000000000000001fLL,
    0x000000000000003fLL,
    0x000000000000007fLL,
    0x00000000000000ffLL,
    0x00000000000001ffLL,
    0x00000000000003ffLL,
    0x00000000000007ffLL,
    0x0000000000000fffLL,
    0x0000000000001fffLL,
    0x0000000000003fffLL,
    0x0000000000007fffLL,
    0x000000000000ffffLL,
    0x000000000001ffffLL,
    0x000000000003ffffLL,
    0x000000000007ffffLL,
    0x00000000000fffffLL,
    0x00000000001fffffLL,
    0x00000000003fffffLL,
    0x00000000007fffffLL,
    0x0000000000ffffffLL,
    0x0000000001ffffffLL,
    0x0000000003ffffffLL,
    0x0000000007ffffffLL,
    0x000000000fffffffLL,
    0x000000001fffffffLL,
    0x000000003fffffffLL,
    0x000000007fffffffLL,
    0x00000000ffffffffLL,
    0x00000001ffffffffLL,
    0x00000003ffffffffLL,
    0x00000007ffffffffLL,
    0x0000000fffffffffLL,
    0x0000001fffffffffLL,
    0x0000003fffffffffLL,
    0x0000007fffffffffLL,
    0x000000ffffffffffLL,
    0x000001ffffffffffLL,
    0x000003ffffffffffLL,
    0x000007ffffffffffLL,
    0x00000fffffffffffLL,
    0x00001fffffffffffLL,
    0x00003fffffffffffLL,
    0x00007fffffffffffLL,
    0x0000ffffffffffffLL,
    0x0001ffffffffffffLL,
    0x0003ffffffffffffLL,
    0x0007ffffffffffffLL,
    0x000fffffffffffffLL,
    0x001fffffffffffffLL,
    0x003fffffffffffffLL,
    0x007fffffffffffffLL,
    0x00ffffffffffffffLL,
    0x01ffffffffffffffLL,
    0x03ffffffffffffffLL,
    0x07ffffffffffffffLL,
    0x0fffffffffffffffLL,
    0x1fffffffffffffffLL,
    0x3fffffffffffffffLL,
    0x7fffffffffffffffLL,
    0xffffffffffffffffLL
};

struct system_table {
    const char      *syst_name;
    const int       syst_params;
};

struct expr_node {
    /* The token of this node.
     *  'x'                          - values (cb_tree)
     *  '+', '-', '*', '/', '^'      - arithmetic operators
     *  '=', '~', '<', '>', '[', ']' - relational operators
     *  '!', '&', '|'                - logical operators
     *  '(', ')'                     - parentheses
     */
    int     token;
    /* The value itself if this node is a value */
    cb_tree     value;
};

#define START_STACK_SIZE        32
#define TOKEN(offset)           (expr_stack[expr_index + offset].token)
#define VALUE(offset)           (expr_stack[expr_index + offset].value)

#define dpush(x)                decimal_stack = cb_cons (x, decimal_stack)

#define cb_emit(x) \
        current_statement->body = cb_list_add (current_statement->body, x)
#define cb_emit_list(l) \
        current_statement->body = cb_list_append (current_statement->body, l)
#define cb_validate_field_call(x)  \
        if (!x->flag_is_verified) { cb_validate_field (x); }
#define turn_on_wipe_index_buffer(x) \
        if(x)                     {   x->wipe_index_buffer = 1;     }

/* Global variables */

size_t          sending_id = 0;
size_t          suppress_warn = 0;

/* Local variables */
static int                     decimal_index_limit = 4;
static cb_tree      decimal_stack = NULL;
static int          saved_decimal=-1;
/*CIT*/
static int          allow_binary_div_optimize = 0;


static const char   *inspect_func;
static cb_tree      inspect_data;

static int          expr_op;                /* last operator */
static cb_tree      expr_lh;                /* last left hand */

static int          expr_index;             /* stack index */
static int          expr_stack_size;        /* stack max size */
static struct       expr_node *expr_stack;  /* expr node stack */

static char         expr_prio[256];

static const struct system_table    system_tab[] = {
#undef  COB_SYSTEM_GEN
#define COB_SYSTEM_GEN(x, y, z) { x, y },
#include "libcob/system.def"
    { NULL, 0}
};

static const char   * const bin_set_funcs[] = {
    NULL,
    "-cob_setswp_u16_binary",
    "-cob_setswp_u24_binary",
    "-cob_setswp_u32_binary",
    "-cob_setswp_u40_binary",
    "-cob_setswp_u48_binary",
    "-cob_setswp_u56_binary",
    "-cob_setswp_u64_binary",
    NULL,
    "-cob_setswp_s16_binary",
    "-cob_setswp_s24_binary",
    "-cob_setswp_s32_binary",
    "-cob_setswp_s40_binary",
    "-cob_setswp_s48_binary",
    "-cob_setswp_s56_binary",
    "-cob_setswp_s64_binary"
};

static const char   * const bin_compare_funcs[] = {
    "-cob_cmp_u8_binary",
    "-cob_cmp_u16_binary",
    "-cob_cmp_u24_binary",
    "-cob_cmp_u32_binary",
    "-cob_cmp_u40_binary",
    "-cob_cmp_u48_binary",
    "-cob_cmp_u56_binary",
    "-cob_cmp_u64_binary",
    "-cob_cmp_s8_binary",
    "-cob_cmp_s16_binary",
    "-cob_cmp_s24_binary",
    "-cob_cmp_s32_binary",
    "-cob_cmp_s40_binary",
    "-cob_cmp_s48_binary",
    "-cob_cmp_s56_binary",
    "-cob_cmp_s64_binary",
    "-cob_cmp_u8_binary",
    "-cob_cmpswp_u16_binary",
    "-cob_cmpswp_u24_binary",
    "-cob_cmpswp_u32_binary",
    "-cob_cmpswp_u40_binary",
    "-cob_cmpswp_u48_binary",
    "-cob_cmpswp_u56_binary",
    "-cob_cmpswp_u64_binary",
    "-cob_cmp_s8_binary",
    "-cob_cmpswp_s16_binary",
    "-cob_cmpswp_s24_binary",
    "-cob_cmpswp_s32_binary",
    "-cob_cmpswp_s40_binary",
    "-cob_cmpswp_s48_binary",
    "-cob_cmpswp_s56_binary",
    "-cob_cmpswp_s64_binary"
};

static const char   * const bin_add_funcs[] = {
    "-cob_add_u8_binary",
    "-cob_add_u16_binary",
    "-cob_add_u24_binary",
    "-cob_add_u32_binary",
    "-cob_add_u40_binary",
    "-cob_add_u48_binary",
    "-cob_add_u56_binary",
    "-cob_add_u64_binary",
    "-cob_add_s8_binary",
    "-cob_add_s16_binary",
    "-cob_add_s24_binary",
    "-cob_add_s32_binary",
    "-cob_add_s40_binary",
    "-cob_add_s48_binary",
    "-cob_add_s56_binary",
    "-cob_add_s64_binary",
    "-cob_add_u8_binary",
    "-cob_addswp_u16_binary",
    "-cob_addswp_u24_binary",
    "-cob_addswp_u32_binary",
    "-cob_addswp_u40_binary",
    "-cob_addswp_u48_binary",
    "-cob_addswp_u56_binary",
    "-cob_addswp_u64_binary",
    "-cob_add_s8_binary",
    "-cob_addswp_s16_binary",
    "-cob_addswp_s24_binary",
    "-cob_addswp_s32_binary",
    "-cob_addswp_s40_binary",
    "-cob_addswp_s48_binary",
    "-cob_addswp_s56_binary",
    "-cob_addswp_s64_binary"
};

static const char   * const bin_sub_funcs[] = {
    "-cob_sub_u8_binary",
    "-cob_sub_u16_binary",
    "-cob_sub_u24_binary",
    "-cob_sub_u32_binary",
    "-cob_sub_u40_binary",
    "-cob_sub_u48_binary",
    "-cob_sub_u56_binary",
    "-cob_sub_u64_binary",
    "-cob_sub_s8_binary",
    "-cob_sub_s16_binary",
    "-cob_sub_s24_binary",
    "-cob_sub_s32_binary",
    "-cob_sub_s40_binary",
    "-cob_sub_s48_binary",
    "-cob_sub_s56_binary",
    "-cob_sub_s64_binary",
    "-cob_sub_u8_binary",
    "-cob_subswp_u16_binary",
    "-cob_subswp_u24_binary",
    "-cob_subswp_u32_binary",
    "-cob_subswp_u40_binary",
    "-cob_subswp_u48_binary",
    "-cob_subswp_u56_binary",
    "-cob_subswp_u64_binary",
    "-cob_sub_s8_binary",
    "-cob_subswp_s16_binary",
    "-cob_subswp_s24_binary",
    "-cob_subswp_s32_binary",
    "-cob_subswp_s40_binary",
    "-cob_subswp_s48_binary",
    "-cob_subswp_s56_binary",
    "-cob_subswp_s64_binary"
};

/*CIT*/
static const char   * const bin_mul_funcs[] = {
    "-cob_mul_u8_binary",
    "-cob_mul_u16_binary",
    "-cob_mul_u24_binary",
    "-cob_mul_u32_binary",
    "-cob_mul_u40_binary",
    "-cob_mul_u48_binary",
    "-cob_mul_u56_binary",
    "-cob_mul_u64_binary",
    "-cob_mul_s8_binary",
    "-cob_mul_s16_binary",
    "-cob_mul_s24_binary",
    "-cob_mul_s32_binary",
    "-cob_mul_s40_binary",
    "-cob_mul_s48_binary",
    "-cob_mul_s56_binary",
    "-cob_mul_s64_binary",
    "-cob_mul_u8_binary",
    "-cob_mulswp_u16_binary",
    "-cob_mulswp_u24_binary",
    "-cob_mulswp_u32_binary",
    "-cob_mulswp_u40_binary",
    "-cob_mulswp_u48_binary",
    "-cob_mulswp_u56_binary",
    "-cob_mulswp_u64_binary",
    "-cob_mul_s8_binary",
    "-cob_mulswp_s16_binary",
    "-cob_mulswp_s24_binary",
    "-cob_mulswp_s32_binary",
    "-cob_mulswp_s40_binary",
    "-cob_mulswp_s48_binary",
    "-cob_mulswp_s56_binary",
    "-cob_mulswp_s64_binary"
};

static const char   * const bin_div_funcs[] = {
    "-cob_div_u8_binary",
    "-cob_div_u16_binary",
    "-cob_div_u24_binary",
    "-cob_div_u32_binary",
    "-cob_div_u40_binary",
    "-cob_div_u48_binary",
    "-cob_div_u56_binary",
    "-cob_div_u64_binary",
    "-cob_div_s8_binary",
    "-cob_div_s16_binary",
    "-cob_div_s24_binary",
    "-cob_div_s32_binary",
    "-cob_div_s40_binary",
    "-cob_div_s48_binary",
    "-cob_div_s56_binary",
    "-cob_div_s64_binary",
    "-cob_div_u8_binary",
    "-cob_divswp_u16_binary",
    "-cob_divswp_u24_binary",
    "-cob_divswp_u32_binary",
    "-cob_divswp_u40_binary",
    "-cob_divswp_u48_binary",
    "-cob_divswp_u56_binary",
    "-cob_divswp_u64_binary",
    "-cob_div_s8_binary",
    "-cob_divswp_s16_binary",
    "-cob_divswp_s24_binary",
    "-cob_divswp_s32_binary",
    "-cob_divswp_s40_binary",
    "-cob_divswp_s48_binary",
    "-cob_divswp_s56_binary",
    "-cob_divswp_s64_binary"
};

static const char   * const align_bin_compare_funcs[] = {
    "-cob_cmp_u8_binary",
    "-cob_cmp_align_u16_binary",
    "-cob_cmp_u24_binary",
    "-cob_cmp_align_u32_binary",
    "-cob_cmp_u40_binary",
    "-cob_cmp_u48_binary",
    "-cob_cmp_u56_binary",
    "-cob_cmp_align_u64_binary",
    "-cob_cmp_s8_binary",
    "-cob_cmp_align_s16_binary",
    "-cob_cmp_s24_binary",
    "-cob_cmp_align_s32_binary",
    "-cob_cmp_s40_binary",
    "-cob_cmp_s48_binary",
    "-cob_cmp_s56_binary",
    "-cob_cmp_align_s64_binary",
    "-cob_cmp_u8_binary",
    "-cob_cmpswp_align_u16_binary",
    "-cob_cmpswp_u24_binary",
    "-cob_cmpswp_align_u32_binary",
    "-cob_cmpswp_u40_binary",
    "-cob_cmpswp_u48_binary",
    "-cob_cmpswp_u56_binary",
    "-cob_cmpswp_align_u64_binary",
    "-cob_cmp_s8_binary",
    "-cob_cmpswp_align_s16_binary",
    "-cob_cmpswp_s24_binary",
    "-cob_cmpswp_align_s32_binary",
    "-cob_cmpswp_s40_binary",
    "-cob_cmpswp_s48_binary",
    "-cob_cmpswp_s56_binary",
    "-cob_cmpswp_align_s64_binary"
};

static const char   * const align_bin_add_funcs[] = {
    "-cob_add_u8_binary",
    "-cob_add_align_u16_binary",
    "-cob_add_u24_binary",
    "-cob_add_align_u32_binary",
    "-cob_add_u40_binary",
    "-cob_add_u48_binary",
    "-cob_add_u56_binary",
    "-cob_add_align_u64_binary",
    "-cob_add_s8_binary",
    "-cob_add_align_s16_binary",
    "-cob_add_s24_binary",
    "-cob_add_align_s32_binary",
    "-cob_add_s40_binary",
    "-cob_add_s48_binary",
    "-cob_add_s56_binary",
    "-cob_add_align_s64_binary",
    "-cob_add_u8_binary",
    "-cob_addswp_u16_binary",
    "-cob_addswp_u24_binary",
    "-cob_addswp_u32_binary",
    "-cob_addswp_u40_binary",
    "-cob_addswp_u48_binary",
    "-cob_addswp_u56_binary",
    "-cob_addswp_u64_binary",
    "-cob_add_s8_binary",
    "-cob_addswp_s16_binary",
    "-cob_addswp_s24_binary",
    "-cob_addswp_s32_binary",
    "-cob_addswp_s40_binary",
    "-cob_addswp_s48_binary",
    "-cob_addswp_s56_binary",
    "-cob_addswp_s64_binary"
};

static const char   * const align_bin_sub_funcs[] = {
    "-cob_sub_u8_binary",
    "-cob_sub_align_u16_binary",
    "-cob_sub_u24_binary",
    "-cob_sub_align_u32_binary",
    "-cob_sub_u40_binary",
    "-cob_sub_u48_binary",
    "-cob_sub_u56_binary",
    "-cob_sub_align_u64_binary",
    "-cob_sub_s8_binary",
    "-cob_sub_align_s16_binary",
    "-cob_sub_s24_binary",
    "-cob_sub_align_s32_binary",
    "-cob_sub_s40_binary",
    "-cob_sub_s48_binary",
    "-cob_sub_s56_binary",
    "-cob_sub_align_s64_binary",
    "-cob_sub_u8_binary",
    "-cob_subswp_u16_binary",
    "-cob_subswp_u24_binary",
    "-cob_subswp_u32_binary",
    "-cob_subswp_u40_binary",
    "-cob_subswp_u48_binary",
    "-cob_subswp_u56_binary",
    "-cob_subswp_u64_binary",
    "-cob_sub_s8_binary",
    "-cob_subswp_s16_binary",
    "-cob_subswp_s24_binary",
    "-cob_subswp_s32_binary",
    "-cob_subswp_s40_binary",
    "-cob_subswp_s48_binary",
    "-cob_subswp_s56_binary",
    "-cob_subswp_s64_binary"
};

/*CIT*/
static const char   * const align_bin_mul_funcs[] = {
    "-cob_mul_u8_binary",
    "-cob_mul_align_u16_binary",
    "-cob_mul_u24_binary",
    "-cob_mul_align_u32_binary",
    "-cob_mul_u40_binary",
    "-cob_mul_u48_binary",
    "-cob_mul_u56_binary",
    "-cob_mul_align_u64_binary",
    "-cob_mul_s8_binary",
    "-cob_mul_align_s16_binary",
    "-cob_mul_s24_binary",
    "-cob_mul_align_s32_binary",
    "-cob_mul_s40_binary",
    "-cob_mul_s48_binary",
    "-cob_mul_s56_binary",
    "-cob_mul_align_s64_binary",
    "-cob_mul_u8_binary",
    "-cob_mulswp_u16_binary",
    "-cob_mulswp_u24_binary",
    "-cob_mulswp_u32_binary",
    "-cob_mulswp_u40_binary",
    "-cob_mulswp_u48_binary",
    "-cob_mulswp_u56_binary",
    "-cob_mulswp_u64_binary",
    "-cob_mul_s8_binary",
    "-cob_mulswp_s16_binary",
    "-cob_mulswp_s24_binary",
    "-cob_mulswp_s32_binary",
    "-cob_mulswp_s40_binary",
    "-cob_mulswp_s48_binary",
    "-cob_mulswp_s56_binary",
    "-cob_mulswp_s64_binary"
};   

static const char   * const align_bin_div_funcs[] = {
    "-cob_div_u8_binary",
    "-cob_div_align_u16_binary",
    "-cob_div_u24_binary",
    "-cob_div_align_u32_binary",
    "-cob_div_u40_binary",
    "-cob_div_u48_binary",
    "-cob_div_u56_binary",
    "-cob_div_align_u64_binary",
    "-cob_div_s8_binary",
    "-cob_div_align_s16_binary",
    "-cob_div_s24_binary",
    "-cob_div_align_s32_binary",
    "-cob_div_s40_binary",
    "-cob_div_s48_binary",
    "-cob_div_s56_binary",
    "-cob_div_align_s64_binary",
    "-cob_div_u8_binary",
    "-cob_divswp_u16_binary",
    "-cob_divswp_u24_binary",
    "-cob_divswp_u32_binary",
    "-cob_divswp_u40_binary",
    "-cob_divswp_u48_binary",
    "-cob_divswp_u56_binary",
    "-cob_divswp_u64_binary",
    "-cob_div_s8_binary",
    "-cob_divswp_s16_binary",
    "-cob_divswp_s24_binary",
    "-cob_divswp_s32_binary",
    "-cob_divswp_s40_binary",
    "-cob_divswp_s48_binary",
    "-cob_divswp_s56_binary",
    "-cob_divswp_s64_binary"
};

static const char *  cob_exp10LL_str[19] = {
    "1LL",
    "10LL",
    "100LL",
    "1000LL",
    "10000LL",
    "100000LL",
    "1000000LL",
    "10000000LL",
    "100000000LL",
    "1000000000LL",
    "10000000000LL",
    "100000000000LL",
    "1000000000000LL",
    "10000000000000LL",
    "100000000000000LL",
    "1000000000000000LL",
    "10000000000000000LL",
    "100000000000000000LL",
    "1000000000000000000LL"
};

static const int    cb_exp10[10] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000
};

static cb_tree cb_expr_shift_class (const char *name);
static void cb_expr_shift_sign (const int op);

static size_t
cb_validate_one (cb_tree x) {
    cb_tree     y;

    if ( x == cb_error_node ) {
        return 1;
    }
    if ( !x ) {
        return 0;
    }
    if ( CB_REFERENCE_P (x) ) {
        y = cb_ref (x);
        if ( y == cb_error_node ) {
            return 1;
        }
        if ( CB_FIELD_P (y) && CB_FIELD (y)->level == 88 ) {
            cb_error_x (x, _("Invalid use of 88 level item"));
            return 1;
        }
    }
    return 0;
}

static size_t
cb_validate_list (cb_tree l) {
    for ( ; l; l = CB_CHAIN (l) ) {
        if ( cb_validate_one (CB_VALUE (l)) ) {
            return 1;
        }
    }
    return 0;
}

static int  
cb_count_in_procedure_param_list(cb_tree l, cb_tree x) {
    int cnt = 0;
    char a[COB_NORMAL_BUFF];
    char b[COB_NORMAL_BUFF];

    strncpy(a, cb_name(x), sizeof(a));
    for ( ; l; l = CB_CHAIN (l) ) {
        cb_tree v = CB_VALUE(l);
        strncpy(b, cb_name(v), sizeof(b));
        if ( strcmp(a,b) == 0 ) {
            cnt++;
        }
    }
    return cnt;
}

#define ACTION_FUNCTION_SETTING_NAME "cob_setting_action"
void 
cb_check_setting_action (cb_tree dst, int assign_action)
{
    struct cb_field *f;
    f = cb_field(dst);
    if ( f && f->set_action_code ) {
        cb_tree p = current_statement->after_body;
        cb_tree x;
        cb_tree c = cb_int(f->set_action_code);

        for ( ; p; p=CB_CHAIN(p) ) {
            x = CB_VALUE(p);
            if ( x && 
                 CB_FUNCALL_P(p) && 
                 (strcmp(CB_FUNCALL(p)->name,  ACTION_FUNCTION_SETTING_NAME) == 0) &&
                 c == CB_FUNCALL(p)->argv[0] ) {
                return;
            }
        }
        current_statement->after_body  = cb_list_add (current_statement->after_body,  
                                                      cb_build_funcall_1(ACTION_FUNCTION_SETTING_NAME, c));
    }

    if (assign_action && f && CB_EXCEPTION_ENABLE (COB_EC_DATA_INCOMPATIBLE) && !cb_disable_runtime_check ) {
        if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC && (f->usage == CB_USAGE_DISPLAY || f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6 )) {
            current_statement->after_body  = cb_list_add (current_statement->after_body
                                                          , (cb_build_funcall_2 ("cob_check_numeric", dst, cb_build_string0 ((ucharptr)(f->name)))));
        }
    }
    if (cb_flag_validate_dep_on && assign_action && f && f->is_odo_idx && CB_EXCEPTION_ENABLE (COB_EC_BOUND_ODO) && !cb_disable_runtime_check ) {
        if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC && (f->usage == CB_USAGE_DISPLAY || f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6 )) {
            current_statement->after_body = cb_list_add ( current_statement->after_body
                                                        , (cb_build_funcall_4 ("cob_check_odo", cb_build_cast_integer(dst)
                                                        , cb_int (f->odo_idx_min)
                                                        , cb_int (f->odo_idx_max)
                                                        , cb_build_string0 ((ucharptr)(f->name)))));
        }
    }
}

cb_tree 
cb_validate_procedure_param_list(cb_tree l) {
    cb_tree ll = l;
    for ( ; l; l = CB_CHAIN (l) ) {
        cb_tree v = CB_VALUE(l);
        if ( cb_count_in_procedure_param_list(ll, v) > 1 ) {
            cb_error_x(v, "PROCEDURE parameters '%s' specified more than once ", cb_name(v));
            break;
        }
    }
    return ll;
}

static cb_tree
cb_check_group_name (cb_tree x) {
    cb_tree     y;

    if ( x == cb_error_node ) {
        return cb_error_node;
    }

    if ( CB_REFERENCE_P (x) ) {
        y = cb_ref (x);
        if ( y == cb_error_node ) {
            return cb_error_node;
        }
        if ( CB_FIELD_P (y) && CB_FIELD (y)->children != NULL &&
             CB_REFERENCE (x)->offset == NULL ) {
            return x;
        }
    }

    cb_error_x (x, _("'%s' is not group name"), cb_name (x));
    return cb_error_node;
}

static cb_tree
cb_check_numeric_name (cb_tree x) {
    if ( x == cb_error_node ) {
        return cb_error_node;
    }

    if ( CB_REFERENCE_P (x)
         && CB_FIELD_P (cb_ref (x))
         && CB_TREE_CATEGORY (x) == CB_CATEGORY_NUMERIC ) {
        return x;
    }

    cb_error_x (x, _("'%s' is not a numeric name"), cb_name (x));
    return cb_error_node;
}

static cb_tree
cb_check_numeric_edited_name (cb_tree x) {
    if ( x == cb_error_node ) {
        return cb_error_node;
    }

    if ( CB_REFERENCE_P (x)
         && CB_FIELD_P (cb_ref (x))
         && (CB_TREE_CATEGORY (x) == CB_CATEGORY_NUMERIC
             || CB_TREE_CATEGORY (x) == CB_CATEGORY_NUMERIC_EDITED) ) {
        return x;
    }

    cb_error_x (x, _("'%s' is not numeric or numeric-edited name"), cb_name (x));
    return cb_error_node;
}

cb_tree
cb_check_numeric_value (cb_tree x) {
    if ( x == cb_error_node ) {
        return cb_error_node;
    }

    if ( CB_TREE_CATEGORY (x) == CB_CATEGORY_NUMERIC ) {
        return x;
    }

    if ( CB_INTRINSIC_P(x) && CB_INTRINSIC(x)->intr_tab->intr_enum == CB_INTR_USER_DEFINE ) {
        cb_warning_x (x, _("User function '%s' may not be a numeric value"), cb_name (x));
        return x;
    }

    if ( CB_CONST_P(x) && CB_CONST(x)->num_type == CB_CONST_FLDADDR ) {
        return x;
    }

    if ( CB_BINARY_OP_P(x) && CB_BINARY_OP(x)->op == 'v' ) {
        return x;
    }

    cb_error_x (x, _("'%s' is not a numeric value"), cb_name (x));
    return cb_error_node;
}

static cb_tree
cb_check_integer_value (cb_tree x) {
    struct cb_literal   *l;
    struct cb_field     *f;
    cb_tree         y;

    if ( x == cb_error_node ) {
        return cb_error_node;
    }

    if ( CB_TREE_CATEGORY (x) != CB_CATEGORY_NUMERIC ) {
        goto invalid;
    }

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_CONST:
            if ( x != cb_zero ) {
                goto invalid;
            }
            return x;
        case CB_TAG_LITERAL:
            l = CB_LITERAL (x);
            if ( l->sign < 0 || l->scale > 0 ) {
                goto invliteral;
            }
            return x;
        case CB_TAG_REFERENCE:
            y = cb_ref (x);
            if ( y == cb_error_node ) {
                return cb_error_node;
            }
            f = CB_FIELD (y);
            if ( f->pic->scale > 0 ) {
                goto invalid;
            }
            return x;
        case CB_TAG_BINARY_OP:
            /* TODO: need to check */
            return x;
        case CB_TAG_INTRINSIC:
            /* TODO: need to check */
            return x;
        default:
            invalid:
            cb_error_x (x, _("'%s' is not an integer value"), cb_name (x));
            return cb_error_node;
    }
    invliteral:
    cb_error_x (x, _("A positive numeric integer is required here"));
    return cb_error_node;
}

void
cb_build_registers (void) {
#if !defined(__linux__) && !defined(__CYGWIN__) && defined(HAVE_TIMEZONE)
    long    contz;
#endif
    struct cb_constant_list *l;
    time_t  t;
    char    buff[48];
    enum cb_usage usage =  CB_USAGE_INDEX;
    int s;
    /*struct cb_constant_list *l;*/

    /* RETURN-CODE */
    if ( !current_program->nested_level ) {
        l = cb_constants;
        while (l) {
            if (l->is_userconst && !l->is_makesyn && l->key && l->value) {
                update_level_78(l->key, l->value);
            }
            l = l->next;
        }

        switch ( cb_rtncode_size ) {
            case 2: usage = CB_USAGE_SIGNED_SHORT; break;
            case 4: usage = CB_USAGE_SIGNED_INT;   break;
            case 8: usage = CB_USAGE_SIGNED_LONG;  break;
            default: usage =  CB_USAGE_INDEX; break;
        } 
        current_program->cb_return_code =
        cb_build_index (cb_build_reference ("RETURN-CODE"),
                        cb_zero, 0, NULL, 0, usage);
        cb_field (current_program->cb_return_code)->flag_is_global = 1;
        /* TALLY */
        if ( cb_flag_tally_register ) {
            /* 01 TALLY GLOBAL PICTURE 9(9) USAGE COMP-5 VALUE ZERO. */
            /* TALLY/EXAMINE  not standard/supported */
            current_program->cb_tally =
            cb_build_index (cb_build_reference ("TALLY"),
                            cb_zero, 0, NULL, 0, CB_USAGE_INDEX);
            cb_field (current_program->cb_tally)->flag_is_global = 1;
        }
    }

    /* SORT-RETURN */
    current_program->cb_sort_return =
    cb_build_index (cb_build_reference ("SORT-RETURN"), cb_zero, 0, NULL, 0, CB_USAGE_INDEX);
    cb_field (current_program->cb_sort_return)->flag_no_init = 1;

    /* XML-CODE */
    current_program->cb_xml_code =
    cb_build_index (cb_build_reference ("XML-CODE"), cb_zero, 0, NULL, 0, CB_USAGE_INDEX);
    cb_field (current_program->cb_xml_code)->flag_no_init = 1;
    cb_field (current_program->cb_xml_code)->flag_hide_from_list = 1;

    current_program->cb_xml_text =
    cb_build_dummy_linkage (cb_build_reference ("XML-TEXT"), 1);
    cb_field (current_program->cb_xml_text)->flag_no_init = 1;
    cb_field (current_program->cb_xml_text)->flag_dynamic_data = 1;

    current_program->cb_xml_ntext =
    cb_build_dummy_national_linkage (cb_build_reference ("XML-NTEXT"), 1);
    cb_field (current_program->cb_xml_ntext)->flag_no_init = 1;
    cb_field (current_program->cb_xml_ntext)->flag_dynamic_data = 1;

    current_program->cb_xml_event =
    cb_build_dummy_linkage (cb_build_reference ("XML-EVENT"), 1);
    cb_field (current_program->cb_xml_event)->flag_no_init = 1;
    cb_field (current_program->cb_xml_event)->flag_dynamic_data = 1;

    /* NUMBER-OF-CALL-PARAMETERS */
    current_program->cb_call_params =
    cb_build_index (cb_build_reference ("NUMBER-OF-CALL-PARAMETERS"), cb_zero, 0, NULL, 0, CB_USAGE_INDEX);
    cb_field (current_program->cb_call_params)->flag_no_init = 1;


    t = time (NULL);

    if ( !current_program->nested_level && cb_flag_context_reload_enable ) {
        /* CHECKPOINT-CODE */
        current_program->cb_context_code =
        cb_build_index (cb_build_reference ("CHECKPOINT-CODE"),
                        cb_zero, 0, NULL, 0, CB_USAGE_INDEX);
        cb_field (current_program->cb_context_code)->flag_is_global = 1;

        /* CHECKPOINT-FILENAME */
        current_program->cb_context_filename =
        cb_build_display (cb_build_reference ("CHECKPOINT-FILENAME"), cb_null, 256);
        cb_field (current_program->cb_context_filename)->flag_no_init = 1;
        cb_field (current_program->cb_context_filename)->flag_is_global = 1;
    }

    /* WHEN-COMPILED */
    memset (buff, 0, sizeof (buff));
    if (cb_when_compiled_register_format) {
        s = sizeof(buff)-1;
        strftime (buff, s, cb_when_compiled_register_format, localtime (&t));
    } else {
        strftime (buff, 17, "%m/%d/%y%H.%M.%S", localtime (&t));
        s = 17;
    }
    cb_build_constant (cb_build_reference ("WHEN-COMPILED"),
                       cb_build_alphanumeric_literal ((ucharptr)buff, s-1, 0));


    /*user defined  constant */
    /*
    l = cb_constants;
    while (l ) {
        if ( l->key && l->value && !l->is_makesyn && l->is_userconst ) {
            cb_build_constant (cb_build_reference (l->key),
                               cb_build_alphanumeric_literal ((ucharptr)l->value, strlen(l->value), 0));

        }
        l = l->next;
    } 
    */ 
    /* FUNCTION WHEN-COMPILED */
    memset (buff, 0, sizeof (buff));
#if defined(__linux__) || defined(__CYGWIN__)
    strftime (buff, 22, "%Y%m%d%H%M%S00%z", localtime (&t));
#elif defined(HAVE_TIMEZONE)
    strftime (buff, 17, "%Y%m%d%H%M%S00", localtime (&t));
    if ( timezone <= 0 ) {
        contz = -timezone;
        buff[16] = '+';
    } else {
        contz = timezone;
        buff[16] = '-';
    }
    sprintf (&buff[17], "%2.2ld%2.2ld", contz / 3600, contz % 60);
#else
    strftime (buff, 22, "%Y%m%d%H%M%S0000000", localtime (&t));
#endif
    cb_intr_whencomp = cb_build_alphanumeric_literal ((ucharptr)buff, 21, 0);

    /* FUNCTION PI */
    memset (buff, 0, sizeof (buff));
    strcpy (buff, "31415926535897932384626433832795029");
    cb_intr_pi = cb_build_numeric_literal (0, (ucharptr)buff, 34, NULL);

    /* FUNCTION E */
    memset (buff, 0, sizeof (buff));
    strcpy (buff, "27182818284590452353602874713526625");
    cb_intr_e = cb_build_numeric_literal (0, (ucharptr)buff, 34, NULL);
}

char *
cb_encode_program_id (const char *name) {
    unsigned char       *p;
    const unsigned char *s;
    unsigned char       buff[COB_SMALL_BUFF];

    p = buff;
    s = (const unsigned char *)name;
    /* encode the initial digit */
    if ( isdigit (*s) ) {
        p += sprintf ((char *)p, "_%02X", *s++);
    }
    /* encode invalid letters */
    for ( ; *s; s++ ) {
        if ( isalnum (*s) || *s == '_' ) {
            *p++ = *s;
        } else if ( *s == '-' ) {
            *p++ = '_';
        } else {
            p += sprintf ((char *)p, "_%02X", *s);
        }
    }
    *p = 0;
    return strdup ((char *)buff);
}

const char *
cb_build_program_id (cb_tree name, cb_tree alt_name) {
    const char  *s;

/* This needs some more thought, should we generate an entry
    point per program source name ?
    if (alt_name) {
        s = (char *)CB_LITERAL (alt_name)->data;
    } else if (CB_LITERAL_P (name)) {
        s = (char *)CB_LITERAL (name)->data;
    } else {
        s = (char *)CB_NAME (name);
    }

    if (!cb_flag_main && strcmp (s, source_name)) {
        cb_warning (_("Source name '%s' differs from PROGRAM-ID '%s'"),
                source_name, s);
        current_program->source_name = strdup (source_name);
    }
 End comment out */

    if ( alt_name ) {
        current_program->orig_source_name = strdup ((char *)CB_LITERAL (alt_name)->data);
        s = (char *)CB_LITERAL (alt_name)->data;
    } else if ( CB_LITERAL_P (name) ) {
        current_program->orig_source_name = strdup ((char *)CB_LITERAL (name)->data);
        s = cb_encode_program_id ((char *)CB_LITERAL (name)->data);
    } else {
        current_program->orig_source_name = strdup (CB_NAME (name));
        s = cb_encode_program_id (CB_NAME (name));
    }
    if ( cobc_check_valid_name (current_program->orig_source_name) ) {
        cb_error (_("PROGRAM-ID '%s' invalid, may be a reserved C language word or function name."), current_program->orig_source_name);
    }
    return s;
}

void
cb_define_switch_name (cb_tree name, cb_tree sname, cb_tree flag, cb_tree ref) {
    cb_tree switch_id;
    cb_tree value;

    if ( name == cb_error_node ) {
        return;
    }
    if ( sname == cb_error_node ) {
        return;
    }
    if ( CB_SYSTEM_NAME (sname)->category != CB_SWITCH_NAME ) {
        cb_error_x (ref, _("Switch-name is expected '%s'"), CB_NAME (ref));
    } else {
        switch_id = cb_int (CB_SYSTEM_NAME (sname)->token);
        value = cb_build_funcall_1 ("cob_get_switch", switch_id);
        if ( flag == cb_int0 ) {
            value = cb_build_negation (value);
        }
        cb_build_constant (name, value);
    }
}

cb_tree
cb_build_section_name (cb_tree name, int sect_or_para) {
    cb_tree x;

    if ( name == cb_error_node ) {
        return cb_error_node;
    }
    name = cb_build_label_reference(name);
    if ( CB_REFERENCE (name)->word->count > 0 ) {
        x = CB_VALUE (CB_REFERENCE (name)->word->items);
        /* Used as a non-label name or used as a section name.
           Duplicate paragraphs are allowed if not referenced;
           Checked in typeck.c */
        if ( !CB_LABEL_P (x) || sect_or_para == 0
             || (sect_or_para && CB_LABEL_P (x) && CB_LABEL (x)->is_section) ) {
            if ( cb_flag_mf_compat_parser ) {
                redefinition_warning(name, NULL);
            } else {
                redefinition_error (name);
                return cb_error_node;
            }
        }
    }

    return name;
}

cb_tree
cb_build_assignment_name (struct cb_file *cfile, cb_tree name) {
    const char  *s;
    const char  *p;

    if ( name == cb_error_node ) {
        return cb_error_node;
    }

    switch ( CB_TREE_TAG (name) ) {
        case CB_TAG_LITERAL:
            if ( strcmp ((char *)(CB_LITERAL(name)->data), "$#@DUMMY@#$") == 0 ) {
                cfile->special = 2;
            }
            return name;

        case CB_TAG_REFERENCE:
            s = CB_REFERENCE (name)->word->name;
            if ( strcasecmp (s, "KEYBOARD") == 0 ) {
                s = "#DUMMY#";
                cfile->special = 1;
                return cb_build_alphanumeric_literal ((ucharptr)s, strlen (s), 0);
            }
            if ( cfile->dynanic_assign ) {
                current_program->reference_list =
                cb_list_add (current_program->reference_list, name);
                return name;
            }
            switch ( cb_assign_clause ) {
                
                case CB_ASSIGN_EXTERNAL:
                case CB_ASSIGN_DYNAMIC:
                case CB_ASSIGN_MF:
                    if ( cfile->external_assign ) {
                        p = strrchr (s, '-');
                        if ( p ) {
                            s = p + 1;
                        }
                        return cb_build_alphanumeric_literal ((ucharptr)s, strlen (s), 0);
                    }
                    current_program->reference_list =
                    cb_list_add (current_program->reference_list, name);
                    return name;

                case CB_ASSIGN_IBM:
                    /* check organization */
                    /* skip the device label if exists */
                    if ( (p = strchr (s, '-')) != NULL ) {
                        if (p - s <= 3) {
                            s = p + 1;
                        }
                    }
                    /* check organization again */
                    if ( (p = strchr (s, '-')) != NULL ) {
                        if (p - s <= 2) {
                            s = p + 1;
                        }
                    }
                    /* convert the name into literal */
                    return cb_build_alphanumeric_literal ((ucharptr)s, strlen (s), 0);

            }

        default:
            return cb_error_node;
    }
}

cb_tree
cb_build_index (cb_tree x, cb_tree values, int indexed_by, struct cb_field *qual, int local_storage, enum cb_usage usage) {
    struct cb_field *f;

    f = CB_FIELD (cb_build_field (x));
    f->usage = usage /*CB_USAGE_INDEX*/;
    if ( local_storage ) {
        f->storage = CB_STORAGE_LOCAL;
    }
    cb_validate_field (f);
    if ( values ) {
        f->values = cb_list_init (values);
    }
    if ( qual ) {
        f->index_qual = qual;
    }
    f->flag_indexed_by = indexed_by;
    if ( local_storage ) {
        current_program->local_storage = cb_field_add (current_program->local_storage, f);
    } else {
        current_program->working_storage = cb_field_add (current_program->working_storage, f);
    }
    return x;
}

cb_tree
cb_build_display (cb_tree x, cb_tree values, int size) {
    struct cb_field *f;
    char buff[20];

    f = CB_FIELD (cb_build_field (x));
    f->usage = CB_USAGE_DISPLAY;
    sprintf(buff, "X(%d)", size);
    f->pic = CB_PICTURE (cb_build_picture (buff));
    f->values = cb_list_init (values);
    f->size = size;
    cb_validate_field (f);
    current_program->working_storage = cb_field_add (current_program->working_storage, f);
    return x;
}

cb_tree
cb_build_dummy_linkage (cb_tree x, int size) {
    struct cb_field *f;
    char buff[20];

    f = CB_FIELD (cb_build_field (x));
    f->level = 1;
    f->usage = CB_USAGE_DISPLAY;
    f->storage = CB_STORAGE_LINKAGE;
    sprintf(buff, "X(%d)", size);
    f->pic = CB_PICTURE (cb_build_picture (buff));
    f->size = size;
    f->flag_hide_from_list = 1;
    cb_validate_field (f);
    current_program->linkage_storage = cb_field_add (current_program->linkage_storage, f);
    return x;
}

cb_tree
cb_build_dummy_national_linkage (cb_tree x, int size) {
    struct cb_field *f;
    char buff[20];

    f = CB_FIELD (cb_build_field (x));
    f->level = 1;
    f->usage = CB_USAGE_NATIONAL;
    f->storage = CB_STORAGE_LINKAGE;
    sprintf(buff, "N(%d)", size);
    f->pic = CB_PICTURE (cb_build_picture (buff));
    f->size = size*2;
    f->flag_hide_from_list = 1;
    cb_validate_field (f);
    current_program->linkage_storage = cb_field_add (current_program->linkage_storage, f);
    return x;
}

cb_tree
cb_build_identifier (cb_tree x) {
    struct cb_reference *r;
    struct cb_field     *f;
    struct cb_field     *p;
    struct cb_literal   *lit;
    const char          *name;
    cb_tree             v;
    cb_tree             e1;
    cb_tree             l;
    cb_tree             sub;
    int                 offset;
    int                 length;
    int                 n;
    char                buff[64]; /*CIT*/

    if ( x == cb_error_node ) {
        return cb_error_node;
    }

    r = CB_REFERENCE (x);
    name = r->word->name;

    /* resolve reference */
    v = cb_ref (x);
    if ( v == cb_error_node ) {
        return cb_error_node;
    }

    /* check if it is a data name */
    if ( !CB_FIELD_P (v) ) {
        if ( r->subs ) {
            cb_error_x (x, _("'%s' cannot be subscripted (not a field)"), name);
            return cb_error_node;
        }
        if ( r->offset ) {
            cb_error_x (x, _("'%s' cannot be reference modified"), name);
            return cb_error_node;
        }
        return x;
    }
    f = CB_FIELD (v);

    /* BASED check */
    if ( CB_EXCEPTION_ENABLE (COB_EC_BOUND_PTR) ) {
        for ( p = f; p->parent; p = p->parent ) {
            ;
        }
        if ( current_statement ) {
            if ( p->flag_item_based ||
                 (f->storage == CB_STORAGE_LINKAGE &&
                  !p->flag_is_pdiv_parm) ) {
                e1 = cb_build_funcall_2 (
                                           "cob_check_based",
                                           cb_build_address (cb_build_field_reference (p, NULL)),
                                           cb_build_string0 ((ucharptr)name));
                current_statement->null_check = cb_list_add (current_statement->null_check, e1);
            }
        }
    }

    /*CIT*/
    /* Let correct Parsing error of ID(1,1) when DECIMAL IS COMMA  */
    if ( current_program->decimal_point == ',' && r->subs ) {
        for ( l = r->subs; l; l = CB_CHAIN (l) ) {
            sub = CB_VALUE (l);
            if ( sub == cb_error_node ) {
                continue;
            }
            if ( CB_LITERAL_P (sub) ) {
                cb_tree   lita;
                cb_tree   litb;

                lit = CB_LITERAL (sub);
                /*printf("lit sub scale %d '%s' ", lit->scale, lit->data);*/
                if ( lit->scale > 0 ) {
                    /* indice 1.1 (1,1) if not possible must be 2 different indice*/
                    if ( lit->scale < lit->size ) {
                        litb = cb_build_numeric_literal(lit->sign, &lit->data[lit->size-lit->scale], 0, NULL);
                        lit->data[lit->size-lit->scale] =  0;
                        lita = cb_build_numeric_literal(lit->sign, lit->data, 0, NULL);
                        CB_VALUE (l) = litb;
                        l = cb_list_insert_after(l,lita);
                    } else {
                        lit->scale = 0;
                    }
                }
            }

        }
    }

    /* check the number of subscripts */
    if ( !r->all && cb_list_length (r->subs) != f->indexes ) {
        if ( (cb_list_length (r->subs) < f->indexes) &&  cb_flag_incomplete_occurs ) {
            r->all = 1;
        } else {
            switch ( f->indexes ) {
                case 0:
                    cb_error_x (x, _("'%s' cannot be subscripted (no occurs)"), name);
                    return cb_error_node;
                case 1:
                    cb_error_x (x, _("'%s' requires 1 subscript"), name);
                    return cb_error_node;
                default:
                    cb_error_x (x, _("'%s' requires %d subscripts"), name, f->indexes);
                    return cb_error_node;
            }
        }
    }

    /* subscript check */
    if ( !r->all && r->subs ) {
        l = r->subs;
        for ( p = f; p; p = p->parent ) {
            if ( p->flag_occurs ) {
                if ( CB_VALUE (l) == cb_int0 ) {
                    /* 'ALL' given as subscript*/
                    cb_error_x (x, _("'%s' : ALL as subscript not supported"), name);
                    return cb_error_node;
                }
                sub = cb_check_integer_value (CB_VALUE (l));

                l = CB_CHAIN (l);

                if ( sub == cb_error_node ) {
                    continue;
                }

                /* compile-time check */
                if ( CB_LITERAL_P (sub) ) {
                    n = cb_get_int (sub);
                    if ( n < 1 || n > p->occurs_max ) {
                        cb_error_x (x, _("Subscript of '%s' out of bounds: %d"),
                                    name, n);
                    }
                }
                
                /* run-time check 
                if ( CB_EXCEPTION_ENABLE (COB_EC_BOUND_SUBSCRIPT) && !cb_disable_runtime_check &&
                     (f->storage != CB_STORAGE_LINKAGE || cb_flag_check_linkage_bound)) {
                    if ( p->occurs_depending ) {
                        e1 = cb_build_funcall_4 ("cob_check_odo",
                                                 cb_build_cast_integer (p->occurs_depending),
                                                 cb_int (p->occurs_min),
                                                 cb_int (p->occurs_max),
                                                 cb_build_string0
                                                 ((ucharptr)(cb_field (p->occurs_depending)->name)));
                        e2 = cb_build_funcall_4 ("cob_check_subscript",
                                                 cb_build_cast_integer (sub),
                                                 cb_int1,
                                                 cb_build_cast_integer (p->occurs_depending),
                                                 cb_build_string0 ((ucharptr)name));
                        r->check = cb_list_add (r->check, e1);
                        r->check = cb_list_add (r->check, e2);
                    } else {
                        if ( !CB_LITERAL_P (sub) ) {
                            e1 = cb_build_funcall_4 ("cob_check_subscript",
                                                     cb_build_cast_integer (sub),
                                                     cb_int1,
                                                     cb_int (p->occurs_max),
                                                     cb_build_string0 ((ucharptr)name));
                            r->check = cb_list_add (r->check, e1);
                        }
                    }
                } 
                */ 
            }
        }
    }

    /* reference modification check */
    if ( r->offset ) {
        int checksize = f->size;
        if ( f->usage == CB_USAGE_NATIONAL ) {
            checksize = checksize / 2;
        }

        /* compile-time check */
        if ( CB_LITERAL_P (r->offset) ) {
            offset = cb_get_int (r->offset);
            if ( (offset < 1 || offset > checksize) ) {
                if ( !cb_relax_bounds_check ) {
                    cb_error_x (x, _("Offset of '%s' out of bounds: %d"), name, offset);
                } else if ((f->storage != CB_STORAGE_LINKAGE) || 
                           cb_flag_check_linkage_bound) {
                    cb_warning_information (x, _("Offset of '%s' out of bounds: %d"), name, offset);
                }
            } else if ( r->length && CB_LITERAL_P (r->length) ) {
                length = cb_get_int (r->length);
                if ( length < 1 || length > checksize - offset + 1  ) {
                    /*CIT*/
                    if ( (length < 1 ) || !cb_relax_bounds_check ) {
                        cb_error_x (x, _("Length of '%s' out of bounds: %d"),
                                    name, length);
                    } else if ((f->storage != CB_STORAGE_LINKAGE) || 
                               cb_flag_check_linkage_bound) {
                        cb_warning_information (x, _("Length of '%s' out of bounds: %d"),
                                                name, length);
                        sprintf (buff, "%d",  (checksize - offset + 1) );
                        r->length = cb_build_numeric_literal(0,(unsigned char*)buff,0, NULL);             
                    }
                }
            }
        }

        /* run-time check */
        if ( CB_EXCEPTION_ENABLE (COB_EC_BOUND_REF_MOD) && !cb_disable_runtime_check) {
            if ( !CB_LITERAL_P (r->offset)
                 || (r->length && !CB_LITERAL_P (r->length)) ) {
                if (f->flag_dynamic_data) {
                    e1 = cb_build_funcall_4("cob_check_ref_mod",
                                             cb_build_cast_integer (r->offset),
                                             r->length ? cb_build_cast_integer (r->length) :
                                             cb_int1, cb_build_cast_integer (cb_build_length((cb_tree)f)),
                                             cb_build_string0 ((ucharptr)f->name));
                    
                } else {
                    e1 = cb_build_funcall_4("cob_check_ref_mod",
                                             cb_build_cast_integer (r->offset),
                                             r->length ? cb_build_cast_integer (r->length) :
                                             cb_int1, cb_int (checksize),
                                             cb_build_string0 ((ucharptr)f->name));
                }
                if ( current_statement ) {
                   current_statement->ref_check = cb_list_add (current_statement->ref_check, e1);
                }
                else
                {
                   r->check = cb_list_add (r->check, e1);
                }
            }
        }
    }

    if ( f->storage == CB_STORAGE_CONSTANT ) {
        return CB_VALUE (f->values);
    }

    return x;
}

/*
static cb_tree
cb_build_length_1 (cb_tree x) {
    struct cb_reference     *r;
    struct cb_field         *f;
    struct cb_field         *fn;
    cb_tree                 e;
    cb_tree                 size;

    r = CB_REFERENCE(x);
    f = CB_FIELD (cb_ref (x));

    if ( cb_field_variable_size (f) == NULL ) {
        if ( r->all ) {
            return cb_build_cast_length (x);
        } else {
            return cb_int (cb_field_size (x));
        }
    } else {
        e = NULL;
        for ( f = f->children; f; f = f->sister ) {
            if ( !f->redefines ) {
                if ( f->flag_redefined ) {
                    fn = f->parent;
                    if ( fn ) {
                        fn = fn->children;
                    }
                    while (fn &&  (fn->redefines == f)) {

                    }
                } else {
                    if ( cb_flag_odo_slide || (f->sister==NULL) ) {
                        size = cb_build_length_1 (cb_build_field_reference (f, x));
                        if ( f->occurs_depending ) {
                            size = cb_build_binary_op (size, '*', f->occurs_depending);
                        } else if ( f->occurs_max > 1 ) {
                            size = cb_build_binary_op (size, '*', cb_int (f->occurs_max));
                        }
                    } else {
                        size = cb_int (cb_field_size (CB_TREE(f)));
                        if ( f->occurs_max > 1 ) {
                            size = cb_build_binary_op (size, '*', cb_int (f->occurs_max));
                        }
                    }
                }
                e = e ? cb_build_binary_op (e, '+', size) : size;
            }
        }
        return e;
    }
}
*/
cb_tree
cb_build_const_length (cb_tree x) {
    struct cb_field     *f;
    char                buff[64];

    if ( x == cb_error_node ) {
        return cb_error_node;
    }
    if ( CB_REFERENCE_P (x) && cb_ref (x) == cb_error_node ) {
        return cb_error_node;
    }

    memset (buff, 0, sizeof (buff));
    f = CB_FIELD (cb_ref (x));
    if ( f->flag_any_length ) {
        cb_error (_("ANY LENGTH item not allowed here"));
        return cb_error_node;
    }
    if ( f->level == 88 ) {
        cb_error (_("88 level item not allowed here"));
        return cb_error_node;
    }
    if ( f->redefines ) {
        /* rename / redefines */
        cb_validate_field_call (f->redefines) ;
        if ( f->rename_thru ) {
            cb_validate_field_call (f->rename_thru) ;
        }
        cb_validate_field_call (f) ;
        sprintf (buff, "%d", f->size);
    } else {
        cb_validate_field_call (f) ;
        sprintf (buff, "%d", f->memory_size);
    }
    return cb_build_numeric_literal (0, (ucharptr)buff, 0, NULL);
}

cb_tree
cb_build_length (cb_tree x) {
    struct cb_field     *f;
    struct cb_reference *r;
    struct cb_literal   *l;
    cb_tree         temp;
    char            buff[64];

    if ( x == cb_error_node ) {
        return cb_error_node;
    }
    if ( CB_REFERENCE_P (x) && cb_ref_or_type (x) == cb_error_node ) {
        return cb_error_node;
    }

    memset (buff, 0, sizeof (buff));
    if ( CB_LITERAL_P (x) ) {
        l = CB_LITERAL (x);
        sprintf (buff, "%d", (int)l->size);
        return cb_build_numeric_literal (0, (ucharptr)buff, 0, NULL);
    }
    if ( CB_REF_OR_FIELD_P (x) ) {
        if (CB_REFERENCE_P(x) ) {
            r = CB_REFERENCE(x);
            f = CB_FIELD (cb_ref_or_type (x));
        } else {
            r = NULL;
            f = CB_FIELD(x);
        }
        if (f->flag_any_length || f->flag_dynamic_data) {
            return cb_build_any_intrinsic (cb_list_init (x));
        }
        if ( (cb_field_variable_size (f) == NULL) && (!r || !r->all) ) {
            sprintf (buff, "%d", cb_field_size (x));
            return cb_build_numeric_literal (0, (ucharptr)buff, 0, NULL);
        }
        if ( f->flag_is_typedef ) {
            cb_error_x (x, _("LENGTH of variable size TYPEDEF not allowed"));
            return cb_error_node;
        }
    }
    if ( CB_INTRINSIC_P (x) ) {
        return cb_build_any_intrinsic (cb_list_init (x));
    }
    /*
    temp = cb_build_index (cb_build_filler (), NULL, 0, NULL, 0);
    CB_FIELD (cb_ref (temp))->usage = CB_USAGE_LENGTH;
    CB_FIELD (cb_ref (temp))->count++;
    cb_emit (cb_build_assign (temp, cb_build_length_1 (x)));
    return temp; 
    */
    temp = cb_build_index (cb_build_filler (), NULL, 0, NULL, 0, CB_USAGE_LENGTH);
    CB_FIELD (cb_ref (temp))->count++;
    cb_emit (cb_build_assign (temp, cb_build_cast_length(x)));
    return temp; 
}

cb_tree
cb_build_address (cb_tree x) {
    if ( x == cb_error_node ||
         (CB_REFERENCE_P (x) && cb_ref (x) == cb_error_node) ) {
        return cb_error_node;
    }

    return cb_build_cast_address (x);
}

cb_tree
cb_build_ppointer (cb_tree x) {
    struct cb_field *f;

    if ( x == cb_error_node ||
         (CB_REFERENCE_P (x) && cb_ref (x) == cb_error_node) ) {
        return cb_error_node;
    }

    if ( CB_REFERENCE_P (x) ) {
        f = cb_field(cb_ref(x));
        f->count++;
    }
    return cb_build_cast_ppointer (x);
}

/* validate program */

static int
get_value (cb_tree x) {
    if ( x == cb_space ) {
        return ' ';
    } else if ( x == cb_zero ) {
        return '0';
    } else if ( x == cb_dirsep ) {
        return(*(CB_LITERAL(cb_dirsep)->data)); /* '/');*/ 
    } else if ( x == cb_quote ) {
        return cb_quote_char; /*'"';*/
    } else if ( x == cb_norm_low ) {
        return 0;
    } else if ( x == cb_norm_high ) {
        return 255;
    } else if ( x == cb_null ) {
        return 0;
    } else if ( CB_TREE_CLASS (x) == CB_CLASS_NUMERIC ) {
        return cb_get_int (x) - 1;
    } else {
        return CB_LITERAL (x)->data[0];
    }
}

void
cb_validate_program_environment (struct cb_program *prog) {
    cb_tree         x;
    cb_tree         y;
    cb_tree         l;
    cb_tree         ls;
    struct cb_alphabet_name *ap;
    unsigned char       *data;
    size_t          dupls;
    size_t          unvals;
    size_t          count;
    int         lower;
    int         upper;
    int         size;
    int         n;
    int         i;
    int         lastval;
    int         values[256];

    /* Check ALPHABET clauses */
    for ( l = current_program->alphabet_name_list; l; l = CB_CHAIN (l) ) {
        ap = CB_ALPHABET_NAME (CB_VALUE (l));
        if ( ap->type != CB_ALPHABET_CUSTOM ) {
            continue;
        }
        ap->low_val_char = 0;
        ap->high_val_char = 255;
        dupls = 0;
        unvals = 0;
        count = 0;
        lastval = 0;
        for ( n = 0; n < 256; n++ ) {
            values[n] = -1;
        }
        for ( y = ap->custom_list; y; y = CB_CHAIN (y) ) {
            if ( count > 255 ) {
                unvals = 1;
                break;
            }
            x = CB_VALUE (y);
            if ( CB_PAIR_P (x) ) {
                /* X THRU Y */
                lower = get_value (CB_PAIR_X (x));
                upper = get_value (CB_PAIR_Y (x));
                lastval = upper;
                if ( !count ) {
                    ap->low_val_char = lower;
                }
                if ( lower < 0 || lower > 255 ) {
                    unvals = 1;
                    continue;
                }
                if ( upper < 0 || upper > 255 ) {
                    unvals = 1;
                    continue;
                }
                if ( lower <= upper ) {
                    for ( i = lower; i <= upper; i++ ) {
                        if ( values[i] != -1 ) {
                            dupls = 1;
                        }
                        values[i] = i;
                        count++;
                    }
                } else {
                    for ( i = lower; i >= upper; i-- ) {
                        if ( values[i] != -1 ) {
                            dupls = 1;
                        }
                        values[i] = i;
                        count++;
                    }
                }
            } else if ( CB_LIST_P (x) ) {
                /* X ALSO Y ... */
                if ( !count ) {
                    ap->low_val_char = get_value (CB_VALUE (x));
                }
                for ( ls = x; ls; ls = CB_CHAIN (ls) ) {
                    n = get_value (CB_VALUE (ls));
                    if ( !CB_CHAIN (ls) ) {
                        lastval = n;
                    }
                    if ( n < 0 || n > 255 ) {
                        unvals = 1;
                        continue;
                    }
                    if ( values[n] != -1 ) {
                        dupls = 1;
                    }
                    values[n] = n;
                    count++;
                }
            } else {
                /* literal */
                if ( CB_TREE_CLASS (x) == CB_CLASS_NUMERIC ) {
                    n = get_value (x);
                    lastval = n;
                    if ( !count ) {
                        ap->low_val_char = n;
                    }
                    if ( n < 0 || n > 255 ) {
                        unvals = 1;
                        continue;
                    }
                    if ( values[n] != -1 ) {
                        dupls = 1;
                    }
                    values[n] = n;
                    count++;
                } else if ( CB_LITERAL_P (x) ) {
                    size = (int)CB_LITERAL (x)->size;
                    data = CB_LITERAL (x)->data;
                    if ( !count ) {
                        ap->low_val_char = data[0];
                    }
                    lastval = data[size - 1];
                    for ( i = 0; i < size; i++ ) {
                        n = data[i];
                        if ( values[n] != -1 ) {
                            dupls = 1;
                        }
                        values[n] = n;
                        count++;
                    }
                } else {
                    n = get_value (x);
                    lastval = n;
                    if ( !count ) {
                        ap->low_val_char = n;
                    }
                    if ( n < 0 || n > 255 ) {
                        unvals = 1;
                        continue;
                    }
                    if ( values[n] != -1 ) {
                        dupls = 1;
                    }
                    values[n] = n;
                    count++;
                }
            }
        }
        if ( dupls || unvals ) {
            if ( dupls ) {
                cb_error_x (l, _("Duplicate character values in alphabet '%s'"),
                            cb_name (CB_VALUE(l)));
            }
            if ( unvals ) {
                cb_error_x (l, _("Invalid character values in alphabet '%s'"),
                            cb_name (CB_VALUE(l)));
            }
            ap->low_val_char = 0;
            ap->high_val_char = 255;
            continue;
        }
        /* Calculate HIGH-VALUE */
        /* If all 256 values have been specified, HIGH-VALUE is the last one */
        /* Otherwise if HIGH-VALUE has been specified, find the highest */
        /* value that has not been used */
        if ( count == 256 ) {
            ap->high_val_char = lastval;
        } else if ( values[255] != -1 ) {
            for ( n = 254; n >= 0; n-- ) {
                if ( values[n] == -1 ) {
                    ap->high_val_char = n;
                    break;
                }
            }
        }
    }
    /* Rest HIGH/LOW-VALUES */
    cb_low = cb_norm_low;
    cb_high = cb_norm_high;
    /* resolve the program collating sequence */
    if ( !prog->collating_sequence ) {
        return;
    }
    x = cb_ref (prog->collating_sequence);
/* RXWRXW
    if (x == cb_error_node) {
        prog->collating_sequence = NULL;
        return;
    }
*/
    if ( !CB_ALPHABET_NAME_P (x) ) {
        cb_error_x (prog->collating_sequence, _("'%s' not alphabet name"),
                    cb_name (prog->collating_sequence));
        prog->collating_sequence = NULL;
        return;
    }
    if ( CB_ALPHABET_NAME (x)->type != CB_ALPHABET_CUSTOM ) {
        return;
    }
    if ( CB_ALPHABET_NAME (x)->low_val_char ) {
        cb_low = cb_build_alphanumeric_literal ((ucharptr)"\0", 1, 0);
        CB_LITERAL(cb_low)->data[0] = CB_ALPHABET_NAME (x)->low_val_char;
        CB_LITERAL(cb_low)->all = 1;
    }
    if ( CB_ALPHABET_NAME (x)->high_val_char != 255 ) {
        cb_high = cb_build_alphanumeric_literal ((ucharptr)"\0", 1, 0);
        CB_LITERAL(cb_high)->data[0] = CB_ALPHABET_NAME (x)->high_val_char;
        CB_LITERAL(cb_high)->all = 1;
    }
}

static void 
cb_propagate_settings (struct cb_field *f)
{
    int action_code;

    if ( f->children ) {
        action_code = f->set_action_code;
        for ( f = f->children; f; f = f->sister ) {
            f->set_action_code = action_code;
            cb_propagate_settings (f);
        }
    }
}

static void 
cb_make_external(cb_tree x, const char *basename1, const char*basename2)
{
    char buff[255];
    if ( x ) {
        struct cb_field *f = cb_field(x);
        if ( f  && !f->flag_external && f->storage != CB_STORAGE_LINKAGE ) {
            if ( f->parent ) {
                while ( f->parent ) {
                    f = f->parent;
                }
                if ( f  && !f->flag_external ) {
                    cb_error_x (x, _("'%s' part of an EXTERNAL file and stored in a non external structure"), CB_NAME (x));
                }
            } else {
                f->flag_external = 1;
                sprintf (buff, "FE_%s_%s", basename1, basename2);
                f->ename =  strdup(buff);
                cb_warning_x (x, _("'%s' declared implicitly EXTERNAL AS '%s' (-fno-file-auto-external to disable)"), CB_NAME (x), buff);
                cb_validate_field(f);
            }
        }
    }
}

void
cb_validate_program_data (struct cb_program *prog) {
    cb_tree     l;
    cb_tree     x;
    cb_tree     assign;
    struct cb_field *p;
    struct cb_file  *f;
    unsigned char   *c;

    for ( l = current_program->file_list; l; l = CB_CHAIN (l) ) {
        f = CB_FILE (CB_VALUE (l));
        if ( !f->finalized ) {
            finalize_file (f, NULL);
        }
    }
    /* build undeclared assignment name now */
    for ( l = current_program->file_list; l; l = CB_CHAIN (l) ) {
        f = CB_FILE (CB_VALUE (l));
        assign = f->assign;
        if ( !assign ) {
            continue;
        }
        if ( (cb_assign_clause == CB_ASSIGN_MF) || (f->dynanic_assign) ) {
            if ( CB_REFERENCE_P (assign) ) {
                for ( x = current_program->file_list; x; x = CB_CHAIN (x) ) {
                    if ( !strcmp (CB_FILE (CB_VALUE (x))->name,
                                  CB_REFERENCE (assign)->word->name) ) {
                        redefinition_error (assign);
                    }
                } 

                p = check_level_78 (CB_REFERENCE (assign)->word->name);
                if ( p ) {
                    c = (unsigned char *)CB_LITERAL(CB_VALUE(p->values))->data;
                    assign = CB_TREE (build_literal (CB_CATEGORY_ALPHANUMERIC, c, strlen ((char *)c)));
                    CB_FILE (CB_VALUE (l))->assign = assign;
                }
            }
            if ( CB_REFERENCE_P (assign) && CB_REFERENCE (assign)->word->count == 0 ) {
                if ( cb_warn_implicit_define ) {
                    cb_warning (_("'%s' will be implicitly defined"), CB_NAME (assign));
                }
                x = cb_build_implicit_field (assign, COB_SMALL_BUFF, f->external);
                p = current_program->working_storage;
                CB_FIELD (x)->count++;
                CB_FIELD (x)->values = cb_list_init(cb_build_alphanumeric_literal((const unsigned char*)CB_NAME(assign), strlen(CB_NAME(assign)), 0));
                if ( p ) {
                    while ( p->sister ) {
                        p = p->sister;
                    }
                    p->sister = CB_FIELD (x);
                } else {
                    current_program->working_storage = CB_FIELD (x);
                }
            }
            if ( CB_REFERENCE_P (assign) ) {
                x = cb_ref (assign);
                if ( CB_FIELD_P (x) && CB_FIELD (x)->level == 88 ) {
                    cb_error_x (assign, _("ASSIGN data item '%s' invalid"), CB_NAME (assign));
                }
                cb_mark_as_non_skipable(assign);      
            }
        }
    }

    if ( prog->cursor_pos ) {
        x = cb_ref (prog->cursor_pos);
        if ( x == cb_error_node ) {
            prog->cursor_pos = NULL;
        } else if ( CB_FIELD(x)->size != 6 && CB_FIELD(x)->size != 4 ) {
            cb_error_x (prog->cursor_pos, _("'%s' CURSOR is not 4 or 6 characters long"),
                        cb_name (prog->cursor_pos));
            prog->cursor_pos = NULL;
        } else {
            CB_FIELD(x)->set_action_code = COB_SAC_CLEAR_CURSOR_POS;
            cb_propagate_settings(CB_FIELD(x));
        }
    }
    if ( prog->crt_status ) {
        x = cb_ref (prog->crt_status);
        if ( x == cb_error_node ) {
            prog->crt_status = NULL;
        } else if ( CB_FIELD(x)->size != 4 ) {
            /*
            cb_error_x (prog->crt_status, _("'%s' CRT STATUS is not 4 characters long"),
                        cb_name (prog->crt_status));
             
            prog->crt_status = NULL; 
            */ 
            /* runtime now support crt status < 4 byte*/
            cb_warning_x (prog->crt_status, _("'%s' CRT STATUS is not 4 characters long"),
                          cb_name (prog->crt_status));
        }
    } else {
        l = cb_build_reference ("COB-CRT-STATUS");
        p = CB_FIELD (cb_build_field (l));
        p->usage = CB_USAGE_DISPLAY;
        p->pic = CB_PICTURE (cb_build_picture ("9(4)"));
        cb_validate_field (p);
        p->flag_no_init = 1;
        /* Do not initialize/bump ref count here
        p->values = cb_list_init (cb_zero);
        p->count++;
        */
        current_program->working_storage =
        cb_field_add (current_program->working_storage, p);
        prog->crt_status = l;
        /* RXWRXW - Maybe better
        prog->crt_status = cb_build_index (cb_build_reference ("COB-CRT-STATUS"), cb_zero, 0, NULL);
*/
    }

    /* resolve all references so far */
    for ( l = cb_list_reverse (prog->reference_list); l; l = CB_CHAIN (l) ) {
        cb_ref_or_type (CB_VALUE (l));
    }
    for ( l = current_program->file_list; l; l = CB_CHAIN (l) ) {
        f = CB_FILE (CB_VALUE (l));
        if ( f->record_depending && f->record_depending != cb_error_node ) {
            x = f->record_depending;
            if ( cb_ref (x) != cb_error_node ) {
/* RXW - This breaks old legacy programs
                if (CB_REF_OR_FIELD_P(x)) {
                    p = cb_field (x);
                    switch (p->storage) {
                    case CB_STORAGE_WORKING:
                    case CB_STORAGE_LOCAL:
                    case CB_STORAGE_LINKAGE:
                        break;
                    default:
                        cb_error (_("RECORD DEPENDING item must be in WORKING/LOCAL/LINKAGE section"));
                    }
                } else {
*/
                if ( !CB_REFERENCE_P(x) && !CB_FIELD_P(x) ) {
                    cb_error_x (x, _("Invalid RECORD DEPENDING item"));
                } else {
                    p = cb_field (x);
                    if (p->storage == CB_STORAGE_LOCAL) {
                        cb_error_x (x, _("RECORD DEPENDING item may not be in LOCAL-STORAGE SECTION"));
                    }

                }
            }
        }
    }
    for ( l = current_program->file_list; l; l = CB_CHAIN (l) ) {
        f = CB_FILE (CB_VALUE (l));
        if ( f->external && cb_flag_file_auto_external ) {
            cb_make_external(f->assign, f->name, "ASSIGN"); 
            /*cb_make_external(f->sharing,  f->name, "SHARING");*/
            if (f->organization ==COB_ORG_RELATIVE) {
                cb_make_external(f->key, f->name, "KEY");
            }
        }
    }
}

void
cb_validate_program_body (struct cb_program *prog) {
    /* resolve all labels */
    cb_tree l;
    cb_tree x;
    cb_tree v;

    for ( l = cb_list_reverse (prog->label_list); l; l = CB_CHAIN (l) ) {
        x = CB_VALUE (l);
        v = cb_ref (x);
        if ( CB_LABEL_P (v) ) {
            CB_LABEL (v)->need_begin = 1;
            if ( CB_REFERENCE (x)->length ) {
                CB_LABEL (v)->need_return = 1;
            }
        } else if ( v != cb_error_node ) {
            cb_error_x (x, _("'%s' not procedure name"), cb_name (x));
        }
    }

    prog->file_list = cb_list_reverse (prog->file_list);
    prog->exec_list = cb_list_reverse (prog->exec_list);
}

/*
 * Expressions
 */

static void
cb_expr_init (void) {
    static int initialized = 0;

    if ( initialized == 0 ) {
        /* init priority talble */
        expr_prio['x'] = 0;
        expr_prio['v'] = 0;
        expr_prio['n'] = 1;
        expr_prio['^'] = 1;
        expr_prio['*'] = 2;
        expr_prio['a'] = 2;
        expr_prio['/'] = 2;
        expr_prio['%'] = 2;     /*CIT*/
        expr_prio['o'] = 3;
        expr_prio['y'] = 3;
        expr_prio['+'] = 3;
        expr_prio['-'] = 3;
        expr_prio['='] = 4;
        expr_prio['~'] = 4;
        expr_prio['<'] = 4;
        expr_prio['>'] = 4;
        expr_prio['['] = 4;
        expr_prio[']'] = 4;
        expr_prio['!'] = 5;
        expr_prio['s'] = 5;
        expr_prio['&'] = 6;
        expr_prio['|'] = 7;
        expr_prio[')'] = 8;
        expr_prio['('] = 9;
        expr_prio[0] = 10;
        /* init stack */
        expr_stack_size = START_STACK_SIZE;
        expr_stack = cobc_malloc (sizeof (struct expr_node) * START_STACK_SIZE);
        expr_stack[0].token = 0;    /* dummy */
        expr_stack[1].token = 0;    /* dummy */
        expr_stack[2].token = 0;    /* dummy */
        initialized = 1;
    }

    expr_op = 0;
    expr_lh = NULL;
    expr_index = 3;
}

static int 
expr_is_boolean (cb_tree x){
    int type;

    if ( CB_TREE_CLASS (x) == CB_CLASS_BOOLEAN ) {
        return 1;
    }
    type = cb_tree_type(x);
    if ( type == COB_TYPE_BITS || 
         type == COB_TYPE_NUMERIC_BITS ||
         type == COB_TYPE_ALPHANUMERIC_BITS ) {
        return 1;
    }
    return 0;
}

static int
expr_reduce (int token) {
    /* Example:
     * index: -3  -2  -1   0
     * token: 'x' '*' 'x' '+' ...
     */

    int op;
    int i;

    while ( expr_prio[TOKEN (-2)] <= expr_prio[token] ) {
        /* Reduce the expression depending on the last operator */
        /*printf("r: '%c' '%c' '%c' '%c' '%c' %d\n", TOKEN (-4), TOKEN (-3), TOKEN (-2), TOKEN (-1),TOKEN (0), expr_index);*/
        op = TOKEN (-2);
        switch ( op ) {
            case 'x':
                return 0;

            case 'a':
            case 'o':
            case 'y':
            case '+':
            case '-':
            case '*':
            case '/':
            case '%': /*CIT*/
            case '^':
                /* Arithmetic operators: 'x' op 'x' */
                if ( TOKEN (-1) != 'x' || TOKEN (-3) != 'x' ) {
                    return -1;
                }
                TOKEN (-3) = 'x';
                VALUE (-3) = cb_build_binary_op (VALUE (-3), op, VALUE (-1));
                expr_index -= 2;
                break;

            case 'n':
                /* Bitwise Negation: 'n' 'x' */
                if ( TOKEN (-1) != 'x' ) {
                    return -1;
                }
                TOKEN (-2) = 'x';
                VALUE (-2) = cb_build_not_value (VALUE (-1));
                expr_index -= 1;
                break;
            case '!':
                /* Negation: '!' 'x' */
                if ( TOKEN (-1) != 'x' ) {
                    return -1;
                }
                /* 'x' '=' 'x' '|' '!' 'x' */
                if ( expr_lh ) {
                    /*if ( CB_TREE_CLASS (VALUE (-1)) != CB_CLASS_BOOLEAN ) {*/
                    if ( !expr_is_boolean(VALUE (-1)) ) {
                        VALUE (-1) = cb_build_binary_op (expr_lh, expr_op, VALUE (-1));
                    }
                }
                TOKEN (-2) = 'x';
                VALUE (-2) = cb_build_negation (VALUE (-1));
                expr_index -= 1;
                break;

            case '&':
            case '|':
                /* Logical AND/OR: 'x' op 'x' */
                if ( TOKEN (-1) != 'x' || TOKEN (-3) != 'x' ) {
                    return -1;
                }
                /* 'x' '=' 'x' '|' 'x' */
                if ( expr_lh ) {
                    /*if ( CB_TREE_CLASS (VALUE (-1)) != CB_CLASS_BOOLEAN ) {*/
                    if ( !expr_is_boolean(VALUE (-1)) ) {
                        VALUE (-1) = cb_build_binary_op (expr_lh, expr_op, VALUE (-1));
                    }
                    /*if ( CB_TREE_CLASS (VALUE (-3)) != CB_CLASS_BOOLEAN ) {*/
                    if ( !expr_is_boolean(VALUE (-3)) ) {
                        VALUE (-3) = cb_build_binary_op (expr_lh, expr_op, VALUE (-3));
                    }
                }
                /* warning for complex expressions without explicit parentheses
                   (i.e., "a OR b AND c" or "a AND b OR c") */
                if ( cb_warn_parentheses && op == '|' ) {
                    if ( (CB_BINARY_OP_P (VALUE (-3))
                          && CB_BINARY_OP (VALUE (-3))->op == '&')
                         || (CB_BINARY_OP_P (VALUE (-1))
                             && CB_BINARY_OP (VALUE (-1))->op == '&') ) {
                        cb_warning (_("Suggest parentheses around AND within OR"));
                    }
                }
                TOKEN (-3) = 'x';
                VALUE (-3) = cb_build_binary_op (VALUE (-3), op, VALUE (-1));
                expr_index -= 2;
                break;

            case '(':
            case ')':
                return 0;

            default:
                /* Relational operators */
                if ( TOKEN (-1) != 'x' ) {
                    return -1;
                }
                switch ( TOKEN (-3) ) {
                    case '(':
                        expr_index++;
                        for ( i = 0; i < 3; i++ ) {
                            TOKEN (-i) = TOKEN (-(i+1));
                            VALUE (-i) = VALUE (-(i+1));
                        }
                        TOKEN (-3) = 'x';
                        VALUE (-3) = expr_lh;
                        /*printf("(: '%c' '%c' '%c' '%c' '%c' %d\n", TOKEN (-4), TOKEN (-3), TOKEN (-2), TOKEN (-1),TOKEN (0), expr_index);*/
                        return expr_reduce(token);
                        break;
                        /* no break */
                    case 'x':
                        /* Simple condition: 'x' op 'x' */
                        if ( VALUE (-3) == cb_error_node || VALUE (-1) == cb_error_node ) {
                            VALUE (-3) = cb_error_node;
                        } else {
                            if ( VALUE (-3) ) {
                                expr_lh = VALUE (-3);
                            }
                            if ( expr_lh == NULL ) {
                                VALUE (-3) = cb_error_node;
                                return -1;
                            }
                            if ( CB_REF_OR_FIELD_P (expr_lh) ) {
                                if ( cb_field(expr_lh)->level == 88 ) {
                                    VALUE (-3) = cb_error_node;
                                    return -1;
                                }
                            }
                            if ( CB_REF_OR_FIELD_P (VALUE(-1)) ) {
                                if ( cb_field(VALUE(-1))->level == 88 ) {
                                    VALUE (-3) = cb_error_node;
                                    return -1;
                                }
                            }
                            expr_op = op;
                            TOKEN (-3) = 'x';
                            /*if ( CB_TREE_CLASS (VALUE (-1)) != CB_CLASS_BOOLEAN ) {*/
                            if ( !expr_is_boolean(VALUE (-1)) ) {
                                VALUE (-3) = cb_build_binary_op (expr_lh, op, VALUE (-1));
                            } else {
                                VALUE (-3) = VALUE (-1);
                            }
                        }
                        expr_index -= 2;
                        break;
                    case '&':
                    case '|':
                        /* Complex condition: 'x' '=' 'x' '|' op 'x' */
                        if ( VALUE (-1) == cb_error_node ) {
                            VALUE (-2) = cb_error_node;
                        } else {
                            expr_op = op;
                            TOKEN (-2) = 'x';
                            /*if ( CB_TREE_CLASS (VALUE (-1)) != CB_CLASS_BOOLEAN ) {*/
                            if ( !expr_is_boolean(VALUE (-1)) ) {
                                VALUE (-2) = cb_build_binary_op (expr_lh, op, VALUE (-1));
                            } else {
                                VALUE (-2) = VALUE (-1);
                            }
                        }
                        expr_index -= 1;
                        break;
                    default:
                        return -1;
                }
                break;
        }
    }

    /* handle special case "op OR x AND" */
    if ( token == '&' && TOKEN (-2) == '|' && 
         /* CB_TREE_CLASS (VALUE (-1)) != CB_CLASS_BOOLEAN  {*/
         !expr_is_boolean(VALUE (-1)) ) {
        TOKEN (-1) = 'x';
        VALUE (-1) = cb_build_binary_op (expr_lh, expr_op, VALUE (-1));
    }

    return 0;
}

static void
cb_expr_shift_sign (const int op) {
    int have_not = 0;

    if ( TOKEN (-1) == '!' ) {
        have_not = 1;
        expr_index--;
    }
    expr_reduce ('=');
    if ( TOKEN (-1) == 'x' ) {
        VALUE (-1) = cb_build_binary_op (VALUE (-1), op, cb_zero);
        /*VALUE (-1) = cb_build_sign_value(VALUE (-1)) ;*/
        if ( have_not ) {
            VALUE (-1) = cb_build_negation (VALUE (-1));
        }
    }
}

static cb_tree
cb_expr_shift_class (const char *name) {
    int have_not = 0;
    cb_tree r=NULL;

    if ( TOKEN (-1) == '!' ) {
        have_not = 1;
        expr_index--;
    }
    expr_reduce ('=');
    if ( TOKEN (-1) == 'x' ) {
        r = VALUE (-1);
        VALUE (-1) = cb_build_funcall_1 (name, r);
        if ( have_not ) {
            VALUE (-1) = cb_build_negation (VALUE (-1));
        }
    }
    return r;
}

static void
cb_expr_shift (int token, cb_tree value) {
    /*printf("s: '%c' '%c' '%c' '%c' '%c' %d\n", TOKEN (-4), TOKEN (-3), TOKEN (-2), TOKEN (-1),TOKEN (0), expr_index);*/
    switch ( token ) {
        case 'x':
            /* sign ZERO condition */

            if ( value == cb_zero ) {
                /* printf("c %c %c %c %d\n", TOKEN (-3), TOKEN (-2), TOKEN (-1), expr_index);*/
                if ( TOKEN (-1) == 'x' ) {
                    cb_expr_shift_sign ('=');
                    return;
                }
                if ( TOKEN (-1) == '!' && TOKEN (-2) == 'x' ) {
                    cb_expr_shift_sign ('=');
                    return;
                }
            }


            /* class condition */
            if ( CB_REFERENCE_P (value)
                 && CB_CLASS_NAME_P (cb_ref (value)) ) {
                cb_expr_shift_class (CB_CLASS_NAME (cb_ref (value))->cname);
                return;
            }

            /* unary sign */
            if ( (TOKEN (-1) == '+' || TOKEN (-1) == '-') && TOKEN (-2) != 'x' ) {
                if ( TOKEN (-1) == '-' ) {
                    if (CB_NUMERIC_LITERAL_P(value)) {
                        struct cb_literal *l = (struct cb_literal *)value;
                        if (l->sign) {
                            l->sign = 0 - l->sign;
                        } else
                            l->sign = -1;
                    } else {
                        /*value = cb_build_sign_value(value); */
                        value = cb_build_binary_op (cb_zero, '-', value);
                    }
                }
                expr_index -= 1;
            }
            break;

        case '(':
            /* 'x' op '(' --> '(' 'x' op */
            switch ( TOKEN (-1) ) {
                case '=':
                case '~':
                case '<':
                case '>':
                case '[':
                case ']':
                    expr_op = TOKEN (-1);
                    if ( TOKEN (-2) == 'x' ) {
                        expr_lh = VALUE (-2);
                    }
                    break;

            }
            /* 'x' & '(' --> '(' 'x' op */
            break;

        case ')':
            /* enclose by parentheses */
            expr_reduce (token);
            if ( TOKEN (-2) == '(' ) {
                value = cb_build_parenthesis (VALUE (-1));
                expr_index -= 2;
                cb_expr_shift ('x', value);
                return;
            }
            break;

        default:
            /* '<' '|' '=' --> '[' */
            /* '>' '|' '=' --> ']' */
            if ( token == '=' && TOKEN (-1) == '|' && (TOKEN (-2) == '<' || TOKEN (-2) == '>') ) {
                token = (TOKEN (-2) == '<') ? '[' : ']';
                expr_index -= 2;
            }

            /* '!' '=' --> '~', etc. */
            if ( TOKEN (-1) == '!' ) {
                switch ( token ) {
                    case '=':
                        token = '~';
                        expr_index--;
                        break;
                    case '~':
                        token = '=';
                        expr_index--;
                        break;
                    case '<':
                        token = ']';
                        expr_index--;
                        break;
                    case '>':
                        token = '[';
                        expr_index--;
                        break;
                    case '[':
                        token = '>';
                        expr_index--;
                        break;
                    case ']':
                        token = '<';
                        expr_index--;
                        break;
                }
            }
            break;
    }

    /* reduce */
    expr_reduce (token);

    /* allocate sufficient stack memory */
    if ( expr_index >= expr_stack_size ) {
        expr_stack_size *= 2;
        expr_stack = cobc_realloc (expr_stack, sizeof (struct expr_node) * expr_stack_size);
    }

    /* put on the stack */
    TOKEN (0) = token;
    VALUE (0) = value;
    expr_index++;
}

static void
expr_expand (cb_tree *x) {
    struct cb_binary_op *p;

    start:
    /* remove parenthesis */
    if ( CB_BINARY_OP_P (*x) ) {
        p = CB_BINARY_OP (*x);
        if ( p->op == '@' ) {
            *x = p->x;
            goto start;
        }
        if (p->x)
            expr_expand (&p->x);
        else
            cb_error (_("Invalid expression (code 004)"));
        if ( p->y ) {
            expr_expand (&p->y);
        }
    }
}

static cb_tree
cb_expr_finish (void) {
    expr_reduce (0);    /* reduce all */

    if ( expr_index != 4 ) {
        cb_error (_("Invalid expression (code 001)"));
        return cb_error_node;
    }

    if ( !expr_stack[3].value ) {
        cb_error (_("Invalid expression (code 002)"));
        return cb_error_node;
    }
    expr_expand (&expr_stack[3].value);
    if ( expr_stack[3].token != 'x' ) {
        cb_error (_("Invalid expression (code 003)"));
        return cb_error_node;
    }
    return expr_stack[3].value;
}

static cb_tree
cb_build_expr_local (cb_tree list) {
    cb_tree l;
    cb_tree x;
    int op;
    int i = 0;
    for ( l = list; l; l = CB_CHAIN (l) ) {
        op = CB_PURPOSE_INT (l);
        switch ( op ) {
            case '9': /* NUMERIC */
                x = cb_expr_shift_class ("cob_is_numeric");
                if (x && CB_TREE_CLASS(x) != CB_CLASS_NUMERIC) {
                    struct cb_field *f = cb_field(x);
                    if (f && f->children) {
                        cb_check_feature_x(x, cb_numeric_group, "IS NUMERIC not allowed on group");
                    }
                }
                break;
            case 'A': /* ALPHABETIC */
                cb_expr_shift_class ("cob_is_alpha");
                break;
            case 'L': /* ALPHABETIC_LOWER */
                cb_expr_shift_class ("cob_is_lower");
                break;
            case 'U': /* ALPHABETIC_UPPER */
                cb_expr_shift_class ("cob_is_upper");
                break;
            case 'S': /* SUCCESS */
                cb_expr_shift_class ("cob_is_success");
                break;
            case 'F': /* FAILURE */
                cb_expr_shift_class ("cob_is_failure");
                break;
            case 'P': /* POSITIVE */
                cb_expr_shift_sign ('>');
                break;
            case 'N': /* NEGATIVE */
                cb_expr_shift_sign ('<');
                break;
            case 'O': /* OMITTED */
                current_statement->null_check = NULL;
                cb_expr_shift_class ("cob_is_omitted");
                break;

            case 'x':
                if ( CB_VALUE (l) && CB_REFERENCE_P (CB_VALUE (l)) ) {
                    cb_tree x = CB_CHAIN (l);
                    struct cb_field *f = cb_field(CB_VALUE (l));

                    if ( x && f && f->level == 88 ) {
                        switch ( CB_PURPOSE_INT (x) ) {
                            case '&':
                            case '|':
                            case '(':
                            case ')':
                                break;
                            default:
                                cb_error_x (list, _("Invalid condition, field %s is a level 88"), f->name);
                                return cb_error_node;
                                break;
                        }
                    } else if (i == 0 && x && f->usage != CB_USAGE_BIT) {/*left most position*/
                        switch ( CB_PURPOSE_INT (x) ) {
                            case '&':
                            case '|':
                            case '(':
                            case ')':
                                cb_error_x (list, _("Invalid condition, field %s at left most must be a subject"), f->name);
                                return cb_error_node;
                                break;
                            default:
                                break;
                        }
                    }
                }
                cb_expr_shift (op, CB_VALUE (l));
                break;

            default:
                cb_expr_shift (op, CB_VALUE (l));
                break;
        }
        i++;
    }

    return cb_expr_finish ();
}

cb_tree
cb_build_expr (cb_tree list) {
    cb_expr_init ();
    return cb_build_expr_local(list);
}

cb_tree
cb_build_evaluate_expr (cb_tree list, cb_tree subject) {
    cb_expr_init ();
    expr_lh = subject;
    return cb_build_expr_local(list);

}

/*
 * Numerical operation
 */
/*CIT*/
static int 
build_store_option_int (cb_tree x, cb_tree round_opt) {
    int opt = 0;

    if ( round_opt == cb_int1 ) {
        opt |= COB_STORE_ROUND;
    }

    switch ( CB_FIELD (cb_ref (x))->usage ) {
        case CB_USAGE_DISPLAY:
        case CB_USAGE_PACKED:
        case CB_USAGE_COMP_6:
            if ( current_statement->handler1 || current_statement->handler2 ) {
                opt |= COB_STORE_KEEP_ON_OVERFLOW;
            }
            break;
        case CB_USAGE_COMP_5:
        case CB_USAGE_COMP_X:
            if (!cb_flag_compute_ibm) {
                if ( current_statement->handler1 ) {
                    opt |= COB_STORE_KEEP_ON_OVERFLOW;
                }
                break;
            }
        default:
            if ( !cb_binary_truncate ) {
                if ( current_statement->handler1 ) {
                    opt |= COB_STORE_KEEP_ON_OVERFLOW;
                }
                break;
            }

/* RXW Check - It seems as though we have NEVER implemented TRUNC,
   Code has always been wrong. Hmm. The following statement would
   activate what was intended but ...
   What should we do here?
        if (current_statement->handler1) {
*/
            /*CIT was current_statement->handler_id allwais true*/
            if ( current_statement->handler1 || current_statement->handler2 ) {
                opt |= COB_STORE_KEEP_ON_OVERFLOW;
            } else if ( cb_binary_truncate ) {
                opt |= COB_STORE_TRUNC_ON_OVERFLOW;
            }
            break;
    }

    return opt;
}

/*CIT*/
static cb_tree
build_store_option (cb_tree x, cb_tree round_opt) {
    return cb_int(build_store_option_int (x, round_opt));
}

static decimal_optimizer * remove_from_list(decimal_optimizer **prev, decimal_optimizer **current, decimal_optimizer *head){
    int debut = 0;
    if ((*current) == head) {
        (*prev) = head = (*current)->next;
        debut = 1;
    } else {
        (*prev)->next = (*current)->next;
    }
    (*current)->next = current_program->zombie;
    current_program->zombie = (*current);
    (*current) = (debut)? (*prev) : (*prev)->next;
    return head;
}
static decimal_optimizer * creat_new(cb_tree x, decimal_optimizer * td){
    int index = -1;
    struct cb_field *f = NULL;
    struct cb_literal *l = NULL;
    struct cb_decimal *d = NULL;
    if (x) {
        if (CB_REF_OR_FIELD_P(x)) {
            f = cb_field(x);
        } else
            l = CB_LITERAL(x);
    }

    if (td == NULL) {
        td = cobc_malloc(sizeof(decimal_optimizer));

        if (index != -1) {
            td->d = CB_DECIMAL(cb_build_decimal(index));
        } else {
            td->d = CB_DECIMAL(cb_build_decimal (current_program->decimal_index));
            current_program->decimal_index++;
        }
    } else {
        /*copy decimal*/
        d = CB_DECIMAL(cb_build_decimal(td->d->id));
        //       cob_free(td->d);
        td->d = d;
    }
    /*reset*/
/*    td->d->binary = none_DBT;
    td->d->intermediate = 0;*/
    if (x == NULL) {
        td->d->intermediate = 1;
    }
    td->f = f;
    td->l = l;
    td->x = x;
    td->next = current_program->decimal_list;

    return td;
}

static void update_decimal_list_after_assign(cb_tree x, cb_tree d){
    decimal_optimizer *current, *prev;
    cb_tree y;

    if (!cb_flag_decimal_optimize || x == NULL) {
        return;
    }
    prev = current = current_program->decimal_list;
    while (current) {
        y = current->x;
        /*Looking for the old couple x,d then delete*/
        if (is_identical_or_ovelap_fields(x,y)  && (!(current->d->id == CB_DECIMAL(d)->id) || !(current->d->binary == CB_DECIMAL(d)->binary))) {
            current_program->decimal_list = remove_from_list(&prev, &current, current_program->decimal_list);

        }
        /*update the result couple*/
        else {
            if (current->d->id == CB_DECIMAL(d)->id) {

                current->f = cb_field(x);
                current->x = x;
                current->l = NULL;
                current->d->intermediate = 0;
                if (x->modified) {
                    current_program->decimal_list = remove_from_list(&prev, &current, current_program->decimal_list);

                    continue;
                }
            }
            prev = current;
            current = current->next;
        }

    }

}
static void clear_list(){
    decimal_optimizer *current, *prev;

    /*Clear the list as much as we can 
      Remove unused decimals
      Put them into zombie for further use
    */

    prev = current = current_program->decimal_list;
    while (current) {

        if (current->d->intermediate) {
            current_program->decimal_list = remove_from_list(&prev, &current, current_program->decimal_list);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

static decimal_optimizer * add_new_to_list(cb_tree x, enum decimal_binary_type binary){
    decimal_optimizer * r;

    if (current_program->decimal_index < decimal_index_limit) {
        r = creat_new(x,NULL);
    } else {
        if (current_program->zombie) {
            decimal_optimizer *tmp = current_program->zombie;
            current_program->zombie = current_program->zombie->next;
            r = creat_new(x, tmp);

        } else {
            decimal_index_limit ++;
            r = creat_new(x, NULL);
        }
    }
    r->d->binary=binary;
    return r;
}


static cb_tree
decimal_alloc_ref (cb_tree x, enum decimal_binary_type binary) {
    cb_tree d = NULL;
    decimal_optimizer *current, *prev;
    decimal_optimizer *decimal_list = current_program->decimal_list;
    unsigned int mNew = 1;
    /*for intermidate results case*/
    if (x != NULL) {
        prev = current = decimal_list;
        for (; current; current=current->next) {
            if (is_same_field(x,current->x) && current->d->binary == binary) {
                struct cb_decimal *d_target =  current->d;
                /*put object found in the head of list*/
                if (current != decimal_list) {
                    prev->next = current->next;
                    current->next = decimal_list;
                    decimal_list = current;
                }
                mNew = 0;

                d = CB_TREE(d_target);
                break;
            }
            prev = current;

        }
    }


    if (mNew) {
        decimal_list = add_new_to_list(x, binary);
        d = CB_TREE(decimal_list->d);
    }

    current_program->decimal_list = decimal_list;
    return d;
}
static cb_tree
decimal_alloc (cb_tree x, int opt, enum decimal_binary_type binary) {
    cb_tree d;
    if (opt && (cb_flag_decimal_optimize && (!x || (x && (CB_REF_OR_FIELD_P(x) || CB_LITERAL_P(x)))))) {
        d = decimal_alloc_ref(x, binary);
    } else {
        if (current_program->decimal_index >= decimal_index_limit) {
            decimal_index_limit++;
        }
        d = cb_build_decimal (current_program->decimal_index);
        CB_DECIMAL(d)->binary=binary;
        current_program->decimal_index++;
        if (current_program->decimal_index == saved_decimal) {
            current_program->decimal_index ++;
        }
        if ( current_program->decimal_index > current_program->decimal_index_max ) {
            current_program->decimal_index_max = current_program->decimal_index;
        }
    }
    return d;
}

/*CIT*/


static cb_tree
decimal_binary_to_decimal ( cb_tree d) {
    static cb_tree x;

    if (d == NULL) {
        return d;
    }
    if ( CB_DECIMAL(d)->binary == none_DBT ) {
        return d;
    }
    x = cb_build_decimal (CB_DECIMAL(d)->id);
    switch ( CB_DECIMAL(d)->binary ) {
        case int_DBT:
            dpush (cb_build_funcall_2 ("-cob_decimal_set_sll", x, d));
            break;
        case uint_DBT:
            dpush (cb_build_funcall_2 ("-cob_decimal_set_ull", x, d));
            break;
        case IEEE_float_DBT: 
            dpush (cb_build_funcall_2 ("-cob_decimal_set_float", x, d));
            break;
        case IEEE_double_DBT:
            dpush (cb_build_funcall_2 ("-cob_decimal_set_double", x, d));
            break;
        default:
            fprintf (stderr, "cobc:0: decimal_binary_to_decimal Unexpected decimal type %d\n", CB_DECIMAL (d)->binary);
            ABORT ();
            break;

    }
    return x;
}

/*CIT*/ 
static int 
decimal_is_IEEE_float (cb_tree x)
{
    int res = 0;
    if (x && ((CB_DECIMAL(x)->binary == IEEE_float_DBT) || (CB_DECIMAL(x)->binary == IEEE_double_DBT) )) {
        res = 1;
    }
    return res;
}


static  cb_tree
decimal_compute (const int op, cb_tree x, cb_tree y, int maxscale) {
    const char *func = NULL;
    const char *func_o = NULL;
    const char *funcint_o = NULL;
    const char *funcuint_o = NULL;
    const char *funcbin_o  = NULL;
    const char *funcbin  = NULL;
    int use_maxscale = 0;
    const char *funcint  = NULL;
    const char *funcuint = NULL;
    cb_tree r = NULL;
    switch ( op ) {
        case 'a':
            func     = "cob_decimal_and";
            funcbin  = "-cob_binary_and";
            funcbin_o  = "-cob_binary_and_1";
            break;
        case 'o':
            func     = "cob_decimal_or";
            funcbin  = "-cob_binary_or";
            funcbin_o  = "-cob_binary_or_1";
            break;
        case 'y':
            func     = "cob_decimal_xor";
            funcbin  = "-cob_binary_xor";
            funcbin_o  = "-cob_binary_xor_1";
            break;
        case 'n':
            func     = "cob_decimal_not";
            funcbin  = "-cob_binary_not";
            funcbin_o  = "-cob_binary_not_1";
            break;
        case 's':
            func     = "cob_decimal_sign";
            func     = "cob_decimal_sign_1";
            funcbin  = "-cob_binary_sign";
            funcbin_o  = "-cob_binary_sign_1";
            break;
        case '+':
            func     = "cob_decimal_add";
            func_o   = "cob_decimal_add_1";
            funcbin  = "-cob_binary_add";
            funcbin_o  = "-cob_binary_add_1";
            funcint  = "cob_decimal_add_sll";
            funcint_o= "cob_decimal_add_sll_1";
            funcuint = "cob_decimal_add_ull";
            funcuint_o = "cob_decimal_add_ull_1";
            break;
        case '-':
            func     = "cob_decimal_sub";
            func_o  = "cob_decimal_sub_1";
            funcbin  = "-cob_binary_sub";
            funcbin_o  = "-cob_binary_sub_1";
            funcint  = "cob_decimal_sub_sll";
            funcint_o  = "cob_decimal_sub_sll_1";
            funcuint = "cob_decimal_sub_ull";
            funcuint_o = "cob_decimal_sub_ull_1";
            break;
        case '*':
            func     = "-cob_decimal_mul";
            func_o     = "-cob_decimal_mul_1";
            funcbin  = "-cob_binary_mul";
            funcbin_o  = "-cob_binary_mul_1";
            funcint  = "cob_decimal_mul_sll";
            funcint_o  = "cob_decimal_mul_sll_1";
            funcuint = "cob_decimal_mul_ull";
            funcuint_o = "cob_decimal_mul_ull_1";
            break;
        case '/':
            /*CIT*/

            if (cb_flag_compute_ibm && maxscale >= 0) {
                use_maxscale = 1;
                func = "cob_decimal_div_ibm";
                func_o = "cob_decimal_div_ibm_1";
            } else {
                func = "cob_decimal_div";
                func_o = "cob_decimal_div_1";
            } 
            if (!cb_flag_compute_ibm || maxscale == 0 ||
                (decimal_is_IEEE_float(x) && decimal_is_IEEE_float(y))) {
                if ( allow_binary_div_optimize || 
                     decimal_is_IEEE_float(x) ) {
                    if ( cb_binary_div_check ) {
                        funcbin = "-cob_binary_div_c";
                        funcbin_o = "-cob_binary_div_c_1";
                    } else {
                        funcbin = "-cob_binary_div";
                        funcbin_o = "-cob_binary_div_1";
                    }
                } else {
                    funcbin  = NULL;
                }
            }

            //funcint  = NULL;
            //funcuint = "cob_decimal_div_ull";
            break;
        case '%':
            func = "cob_decimal_mod";
            func_o = "cob_decimal_mod_1";
            if ( allow_binary_div_optimize || 
                 decimal_is_IEEE_float(x) ) {
                if ( cb_binary_div_check ) {
                    funcbin = "-cob_binary_mod_c";
                    funcbin_o = "-cob_binary_mod_c_1";
                } else {
                    funcbin = "-cob_binary_mod";
                    funcbin_o = "-cob_binary_mod_1";
                }
            } else {
                funcbin = NULL;
            }
            //funcint  = NULL;
            //funcuint = "cob_decimal_mod_uint";
            break;
        case '^':
            func     = "-cob_decimal_pow";
            func_o   = "-cob_decimal_pow_1";
            if ( decimal_is_IEEE_float(x) ) {
                funcbin = "-cob_binary_pow";
                funcbin_o = "-cob_binary_pow_1";
            }
            //funcint  = NULL;
            //funcuint = "cob_decimal_pow_uint";
            break;
        default:
            fprintf (stderr, "cobc:0: Unexpected operation %d\n", op);
            ABORT ();
    }
    /*CIT*/
    if ( (CB_DECIMAL(x)->binary && y && CB_DECIMAL(y)->binary ) && (funcbin) ) {
        use_maxscale = 0;
        if (cb_flag_decimal_optimize && !CB_DECIMAL(x)->intermediate && funcbin_o) {
            func = funcbin_o;
        } else
            func = funcbin;
    } else {
        /*
        if (cb_flag_fast_math && y && (CB_DECIMAL(y)->binary == int_DBT) && funcint) {
            if (cb_flag_decimal_optimize && !CB_DECIMAL(x)->intermediate && funcint_o) {
                func = funcint_o;
            } else
                func =funcint;
        } else if (cb_flag_fast_math && (CB_DECIMAL(y)->binary == uint_DBT) && funcuint) {
            if (cb_flag_decimal_optimize && !CB_DECIMAL(x)->intermediate && funcuint_o) {
                func = funcuint_o;
            } else
                func =funcuint;
        } else 
        */ 
        {
            x = decimal_binary_to_decimal(x);
            y = decimal_binary_to_decimal(y);
            if (cb_flag_decimal_optimize && !CB_DECIMAL(x)->intermediate && func_o) {
                func = func_o;
            }
        }
    }
    if (func == func_o || func == funcint_o || func == funcuint_o || func == funcbin_o ) {
        r = decimal_alloc(NULL, 1, CB_DECIMAL(x)->binary);
        if (use_maxscale) {
            dpush (cb_build_funcall_4 (func, r, x, y, cb_int(maxscale)));
        } else {
            dpush (cb_build_funcall_3 (func, r, x, y));
        }
        if (maxscale >= 0 && cb_flag_compute_ibm && !CB_DECIMAL(r)->binary && cb_flag_compute_ibm_trunc) {
            dpush (cb_build_funcall_2 ("cob_decimal_trunc", r, cb_int(maxscale)));
        }
        return r;
    } else {
        if (use_maxscale) {
            dpush (cb_build_funcall_3 (func, x, y, cb_int(maxscale)));
        } else {
            dpush (cb_build_funcall_2 (func, x, y));
        }
        if (maxscale >= 0 && cb_flag_compute_ibm && !CB_DECIMAL(x)->binary && cb_flag_compute_ibm_trunc) {
            dpush (cb_build_funcall_2 ("cob_decimal_trunc", x, cb_int(maxscale)));
        }
        return x;
    }

}

static int 
decimal_may_binary_optize (void) {
    return(    cb_flag_binary_optimize 
               && !current_statement->handler1 
               && !current_statement->handler2);
}

static enum decimal_binary_type
get_expected_binary_type_ref_field (cb_tree x) {
    struct cb_binary_op *p;
    struct cb_field     *f;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_REFERENCE:
            f = cb_field (x);
            switch ( f->usage ) {
                case CB_USAGE_FLOAT:  
                    return IEEE_float_DBT;
                    break;               
                case CB_USAGE_DOUBLE:                
                    return IEEE_double_DBT;
                    break;               
                case CB_USAGE_SIGNED_CHAR:           
                case CB_USAGE_SIGNED_SHORT:          
                case CB_USAGE_SIGNED_INT:            
                case CB_USAGE_SIGNED_LONG:           
                    return int_DBT;
                    break;               
                case CB_USAGE_INDEX:                 
                case CB_USAGE_LENGTH:                
                case CB_USAGE_UNSIGNED_CHAR:         
                case CB_USAGE_UNSIGNED_SHORT:        
                case CB_USAGE_UNSIGNED_INT:          
                case CB_USAGE_UNSIGNED_LONG:         
                case CB_USAGE_BIT:
                    return uint_DBT;
                    break;               
                case CB_USAGE_COMP_5:
                    if ( f->flag_real_binary ) {
                        if ( f->pic->have_sign ) {
                            return int_DBT;
                        } else {
                            return uint_DBT;
                        }
                    }
                    break;
                default:
                    return none_DBT;
                    break;
            }
            break;
        case CB_TAG_BINARY_OP:
            p = CB_BINARY_OP (x);
            return get_expected_binary_type_ref_field(p->x);
            break;
        default:
            return none_DBT;
            break;
    }
    return none_DBT;
}

typedef struct {
    int maxscale;
    int forcegmp;
} assign_info;  


static int
get_maxscale (cb_tree x, int flag_compute_ibm, int dmax) {
    struct cb_binary_op *p;
    struct cb_field     *f;
    struct cb_literal   *l;
    int maxsc = 0;
    cb_tree             li; 
    int res = -1;
    int mx, my;

    if (!flag_compute_ibm) {
        return -1;
    }
    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LITERAL:
            l = CB_LITERAL (x);
            res = l->scale;
            break;
        case CB_TAG_LIST:
            for ( li = x; li; li = CB_CHAIN (li) ) {
                mx = get_maxscale(CB_VALUE(li),flag_compute_ibm, dmax);
                maxsc=max(maxsc, mx);
            }
            res = maxsc;
            break;
        case CB_TAG_REFERENCE:
            f = cb_field (x);
            switch ( f->usage ) {
                case CB_USAGE_FLOAT:  
                case CB_USAGE_DOUBLE:                
                    return -1;
                    break;               
                case CB_USAGE_SIGNED_CHAR:           
                case CB_USAGE_SIGNED_SHORT:          
                case CB_USAGE_SIGNED_INT:            
                case CB_USAGE_SIGNED_LONG:           
                case CB_USAGE_INDEX:                 
                case CB_USAGE_LENGTH:                
                case CB_USAGE_UNSIGNED_CHAR:         
                case CB_USAGE_UNSIGNED_SHORT:        
                case CB_USAGE_UNSIGNED_INT:          
                case CB_USAGE_UNSIGNED_LONG:         
                case CB_USAGE_BIT:
                    res = 0;
                    break;               
                default:
                    if (f->pic) {
                        res = f->pic->scale;
                    }
                    break;
            }
            break;
        case CB_TAG_BINARY_OP:
            p = CB_BINARY_OP (x);
            if (! p->y) {
                res = max(dmax, get_maxscale(p->x, flag_compute_ibm, dmax));
            } else {
                switch (p->op) {
                    case '/':
                        if (dmax < 0 ) {
                            my = -1;
                            res = get_maxscale(p->x, flag_compute_ibm, dmax);
                            if ((CB_TREE_TAG (p->y) != CB_TAG_REFERENCE)  && (CB_TREE_TAG (p->y) != CB_TAG_LITERAL)) {
                                my = get_maxscale(p->y, flag_compute_ibm, dmax);
                            }
                            res = max(res, my);
                        } else {
                        //  res = (get_maxscale(p->y , flag_compute_ibm, dmax) - get_maxscale(p->x , flag_compute_ibm, dmax));
                            res = (get_maxscale(p->x , flag_compute_ibm, dmax) - get_maxscale(p->y , flag_compute_ibm, dmax));
                            res = max(res, dmax);
                        }
                        break;                
                    case '*':
                        if (dmax < 0) {
                            mx = get_maxscale(p->x,flag_compute_ibm, dmax);
                            my = get_maxscale(p->y,flag_compute_ibm, dmax);
                            res = max(mx, my);
                        } else {
                            res = get_maxscale(p->x,flag_compute_ibm, dmax) + get_maxscale(p->y,flag_compute_ibm, dmax);
                        }
                        break;
                    case '^':
                        res = get_maxscale(p->x,flag_compute_ibm, dmax);
                        break;
                    default:
                        mx = get_maxscale(p->x,flag_compute_ibm, dmax);
                        my = get_maxscale(p->y,flag_compute_ibm, dmax);
                        res = max(mx, my);
                        break;
                }
            }
            break;
        default:
            return -1;
            break;
    }
    return res;
}

static int
is_dev_or_exp (cb_tree x) {
    struct cb_binary_op *p;
    cb_tree             li; 
    int res = 0;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LIST:
            for ( li = x; !res && li; li = CB_CHAIN (li) ) {
                if (is_dev_or_exp(CB_VALUE(li)))
                    res = 1;
            }
            break;
        case CB_TAG_BINARY_OP:
            p = CB_BINARY_OP (x);
            switch (p->op) {
                case '^':
                case '/':
                    res = 1;
                    break;                
                default:
                    if (p->y) {
                        res = is_dev_or_exp(p->x) || is_dev_or_exp(p->y);
                    } else {
                        res = is_dev_or_exp(p->x);
                    }
                    break;
            }
            break;
        default:
            break;
    }
    return res;
}

static int
is_float_operand (cb_tree x, int maxscale) {
    struct cb_binary_op *p;
    struct cb_field     *f;
    cb_tree             li; 
    int res = 0;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_LIST:
            for ( li = x; li; li = CB_CHAIN (li) ) {
                if (is_float_operand(CB_VALUE(li), maxscale))
                    return 1;
            }
            break;
        case CB_TAG_REFERENCE:
            f = cb_field (x);
            switch ( f->usage ) {
                case CB_USAGE_FLOAT:  
                case CB_USAGE_DOUBLE:                
                    return 1;
                    break;               
                default:
                    break;
            }
            break;
        case CB_TAG_BINARY_OP:
            p = CB_BINARY_OP (x);
            switch (p->op) {
                case '^':
                    res = is_float_operand(p->x, maxscale) || is_float_operand(p->y, maxscale);
                    res = res || (get_maxscale(p->y, 1, -1) != 0);
                    res = res || (maxscale > 0 && is_dev_or_exp(p->y));
                    break;
                default:
                    if (p->y) {
                        res = is_float_operand(p->x, maxscale) || is_float_operand(p->y, maxscale);
                    } else {
                        res = is_float_operand(p->x, maxscale);
                    }
                    break;
            }
            break;
        default:
            break;
    }
    return res;
}


cb_tree
cb_get_bits_size (cb_tree x) {
    struct cb_binary_op *p;
    struct cb_field     *f;

    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_REFERENCE:
            f = cb_field (x);
            if ( f->pic && f->pic->category == CB_CATEGORY_BIT ) {
                struct cb_reference *r= CB_REFERENCE(x);
                if ( r->offset ) {
                    if ( r->length ) {
                        return r->length;
                    } else {
                        return cb_int(1);
                    }
                }
                return cb_int(f->pic->digits);
            } else {
                return cb_int(min(f->size, 64));
            }
            break;
        case CB_TAG_BINARY_OP:
            p = CB_BINARY_OP (x);
            return cb_get_bits_size(p->x);
            break;
        default:
            return cb_int(64);
            break;
    }
    return cb_int(64);
}
static struct object_attached * object_attached_set_up(union_type type, union_val v) {
    struct object_attached *p = cobc_malloc(sizeof(struct object_attached));
    p->type = type;
    p->val = v;
    return p;
}

static cb_tree add_source_and_target(cb_tree assign_x, cb_tree x, struct cb_decimal *decimal, int field_to_dec) {
    union_val val= {0};

    if (!cb_flag_decimal_optimize) {
        return assign_x;
    }

    if (decimal) {
        val.index = decimal->id;
        val.binary_type = decimal->binary;
    }
    if (field_to_dec) {
        /*field to decimal*/
        assign_x->dst = object_attached_set_up(DecimalType,val);
        val.x = x;
        if (x) {
            assign_x->src = object_attached_set_up(FieldType, val);     
        } else {
            assign_x->src = NULL;
        }
    } else {
        /*decimal to field*/
        assign_x->src = object_attached_set_up(DecimalType, val);
        val.x = x;
        if (x) {
            assign_x->dst = object_attached_set_up(FieldType, val);     
        } else {
            assign_x->dst = NULL;
        }
    }
    return assign_x;
}

static cb_tree 
decimal_expand (cb_tree d, cb_tree x, enum decimal_binary_type expected_binary_type , int dmax,  int readonly, int has_float) {
    struct cb_literal   *l;
    struct cb_field     *f;
    struct cb_binary_op *p;
    cb_tree   func_x;
    cb_tree   t;
    int       mx;

    if ( x == NULL ) {
        return d;
    }
    switch ( CB_TREE_TAG (x) ) {
        case CB_TAG_CONST:

            if ( x == cb_zero ) {
                /*CIT*/
                if ( !decimal_may_binary_optize() && !has_float) {
                    d = decimal_alloc(x,0, none_DBT);
                    func_x = cb_build_funcall_2 ("-cob_decimal_set_int", d, cb_int0); 
                } else {
                    d = decimal_alloc(x,0,has_float ? IEEE_double_DBT: int_DBT);
                    /* ACO01 d = cb_build_decimal (CB_DECIMAL(d)->id);
                    CB_DECIMAL(d)->binary=int_DBT; 
                    */ 
                    func_x =cb_build_funcall_2 ("-cob_binary_set_int", d, cb_int0);               
                }
                dpush (add_source_and_target(func_x,NULL,CB_DECIMAL(d),1));               
                current_program->gen_decset = 1;
            } else if ( CB_CONST(x)->num_type == CB_CONST_FLDADDR ) {
                d = decimal_alloc(x,0, none_DBT);
                func_x = (cb_build_funcall_2 ("cob_decimal_set_field", d, x));
                dpush (add_source_and_target(func_x,NULL,CB_DECIMAL(d),1));               
            } else {
                fprintf (stderr, "cobc:0: Unexpected constant expansion\n");
                ABORT ();
            }
            break;
        case CB_TAG_INTEGER:
            d = decimal_alloc(x,0,has_float ? IEEE_double_DBT: int_DBT);
            /* ACO01 d = cb_build_decimal (CB_DECIMAL(d)->id);
            CB_DECIMAL(d)->binary=uint_DBT; 
            */ 
            func_x = (cb_build_funcall_2 ("-cob_binary_set_uint", d, x));               
            dpush (add_source_and_target(func_x,NULL,CB_DECIMAL(d),1));               
            current_program->gen_decset = 1;
            break;
        case CB_TAG_LITERAL:
            /* set d, N */

            l = CB_LITERAL (x);
            /*printf("expand lit %s, exp %d\n", l->data,  expected_binary_type);*/
            if ( l->size < 10 && l->scale == 0 && (expected_binary_type == none_DBT) ) {
                /*CIT*/
                if ( !decimal_may_binary_optize() ) {
                    d = decimal_alloc(x,1, none_DBT);
                    func_x = cb_build_funcall_2 ("-cob_decimal_set_sll", d, cb_build_cast_integer (x));
                    dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1)); 

                } else {
                    d = decimal_alloc(x,1,has_float ? IEEE_double_DBT: int_DBT);
                    func_x = cb_build_funcall_2 ("-cob_binary_set_int", d, cb_build_cast_integer (x));
                    dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));               
                }
                current_program->gen_decset = 1;
            } else {
                switch ( expected_binary_type ) {
                    case IEEE_double_DBT:
                        if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("-cob_decimal_set_double", d, cb_build_cast_double (x));
                        } else {
                            d = decimal_alloc(x,1, expected_binary_type);
                            func_x = cb_build_funcall_2 ("-cob_binary_set_double", d, cb_build_cast_double (x));
                        }
                        break;
                    case IEEE_float_DBT:
                        if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("-cob_decimal_set_float", d, cb_build_cast_double (x));
                        } else {
                            d = decimal_alloc(x,1, expected_binary_type);
                            func_x = cb_build_funcall_2 ("-cob_binary_set_float", d, cb_build_cast_double (x));
                        }
                        break;
                    case int_DBT:
                        if (cb_fits_int(x)) {
                            if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                                d = decimal_alloc(x,1, none_DBT);
                                func_x = cb_build_funcall_2 ("-cob_decimal_set_sll", d, cb_build_cast_integer (x));
                            } else {
                                d = decimal_alloc(x,1,has_float ? IEEE_double_DBT: expected_binary_type);
                                func_x = cb_build_funcall_2 ("-cob_binary_set_int", d, cb_build_cast_integer (x));
                            }
                        } else {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("cob_decimal_set_field", d, x);
                        }
                        break;
                    case uint_DBT:
                        if (cb_fits_int(x)) {
                            if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                                d = decimal_alloc(x,1, none_DBT);
                                func_x = cb_build_funcall_2 ("-cob_decimal_set_uint", d, cb_build_cast_integer (x));
                            } else {
                                d = decimal_alloc(x,1,has_float ? IEEE_double_DBT: expected_binary_type);
                                func_x = cb_build_funcall_2 ("-cob_binary_set_uint", d, cb_build_cast_integer (x));
                            }
                        } else {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("cob_decimal_set_field", d, x);
                        }
                        break;
                    case none_DBT:
                    default:
                        /*CIT*/
                        d = decimal_alloc(x,1, none_DBT);
                        func_x = cb_build_funcall_2 ("cob_decimal_set_field", d, x);
                        break;
                }
                dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));
            }
            break;
        case CB_TAG_REFERENCE:

            /* set d, X */
            f = cb_field (x);
            /*printf("expand fld %s, exp %d\n", f->name,  expected_binary_type);*/
            /* check numeric */
            if ( CB_EXCEPTION_ENABLE (COB_EC_DATA_INCOMPATIBLE) && !cb_disable_runtime_check) {
                if ( f->usage == CB_USAGE_DISPLAY || f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6 ) {
                    dpush (cb_build_funcall_2 ("cob_check_numeric",
                                               x, cb_build_string0 ((ucharptr)(f->name))));
                }
            }


            if ( cb_fits_int (x) && (expected_binary_type == none_DBT) ) {
                if ( !decimal_may_binary_optize()  ) {
                    d = decimal_alloc(x,1, none_DBT);
                    if ( ((f->usage == CB_USAGE_DISPLAY) || (f->usage == CB_USAGE_PACKED)) ) {
                        func_x = cb_build_funcall_2 ("cob_decimal_set_field", d, x);
                    } else {
                        func_x = cb_build_funcall_2 ("-cob_decimal_set_sll", d, cb_build_cast_integer (x)); 
                    }
                    dpush(add_source_and_target(func_x,x,CB_DECIMAL(d),1));
                } else {
                    d = decimal_alloc(x,1, has_float ? IEEE_double_DBT: int_DBT);
                    func_x = cb_build_funcall_2 ("-cob_binary_set_int", d, cb_build_cast_integer (x));
                    dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));               
                }
                current_program->gen_decset = 1;
            } else {
                switch ( expected_binary_type ) {
                    case IEEE_double_DBT:
                        if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("-cob_decimal_set_double", d, cb_build_cast_double (x));
                        } else {
                            d = decimal_alloc(x,1, expected_binary_type);
                            func_x = cb_build_funcall_2 ("-cob_binary_set_double", d, cb_build_cast_double (x));
                        }
                        dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));          
                        break;
                    case IEEE_float_DBT:
                        if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("-cob_decimal_set_float", d, cb_build_cast_double (x));
                        } else {
                            d = decimal_alloc(x,1, expected_binary_type);
                            func_x = cb_build_funcall_2 ("-cob_binary_set_float", d, cb_build_cast_double (x));
                        }
                        dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));            
                        break;
                    case int_DBT:
                        if (cb_fits_int(x)) {
                            if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                                d = decimal_alloc(x,1, none_DBT);
                                func_x = cb_build_funcall_2 ("-cob_decimal_set_int", d, cb_build_cast_integer (x));
                            } else {
                                d = decimal_alloc(x,1,has_float ? IEEE_double_DBT: expected_binary_type);
                                func_x = cb_build_funcall_2 ("-cob_binary_set_int", d, cb_build_cast_integer (x));
                            }
                        } else {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("cob_decimal_set_field", d, x);
                        }
                        dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));             
                        break;
                    case uint_DBT:
                        if (cb_fits_int(x)) {
                            if (cb_flag_binary_optimize_strict && !decimal_may_binary_optize()) {
                                d = decimal_alloc(x,1, none_DBT);
                                func_x = cb_build_funcall_2 ("-cob_decimal_set_uint", d, cb_build_cast_integer (x));
                            } else {
                                d = decimal_alloc(x,1,has_float ? IEEE_double_DBT: expected_binary_type);
                                func_x = cb_build_funcall_2 ("-cob_binary_set_uint", d, cb_build_cast_integer (x));
                            }
                        } else {
                            d = decimal_alloc(x,1, none_DBT);
                            func_x = cb_build_funcall_2 ("cob_decimal_set_field", d, x);
                        }
                        dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));             
                        break;
                    case none_DBT:
                    default:
                        expected_binary_type = get_expected_binary_type_ref_field (x);
                        if ( expected_binary_type == none_DBT ) {
                            /*CIT*/
                            d = decimal_alloc(x,1, none_DBT);
                            /* ACO01 d = cb_build_decimal(CB_DECIMAL(d)->id);
                            CB_DECIMAL(d)->binary = none_DBT; 
                            */ 
                            func_x = cb_build_funcall_2 ("cob_decimal_set_field", d, x);
                            dpush (add_source_and_target(func_x,x,CB_DECIMAL(d),1));
                        } else {
                            d = decimal_alloc(x,1, expected_binary_type);
                            d = decimal_expand(d,x,expected_binary_type, dmax,  readonly, has_float);
                        }
                        break;
                }
            }
            break;
        case CB_TAG_BINARY_OP:
            /* set d, X
             * set t, Y
             * OP d, t */
            p = CB_BINARY_OP (x);
            /*printf("expand bop %c, exp %d\n", p->op,  expected_binary_type);*/
            dmax = max(dmax, get_maxscale(x, dmax < 0 ? 0 : 1, -1));
            mx = max(dmax, get_maxscale(x, dmax < 0 ? 0 : 1, dmax));
            if ( p->op == 'n' ) {
                d = decimal_expand (d, p->x, expected_binary_type, dmax,  0, has_float);
                t = decimal_expand (NULL, cb_get_bits_size(x), expected_binary_type, dmax,  1, has_float);
            } else {
                d = decimal_expand (d, p->x, expected_binary_type, dmax,  0, has_float);
                t = decimal_expand (NULL, p->y, expected_binary_type, dmax,  1, has_float);
            }

            d = decimal_compute (p->op, d, t, mx);
            break;
        case CB_TAG_INTRINSIC:
            d = decimal_alloc(x, 0, none_DBT);
            func_x =cb_build_funcall_2 ("cob_decimal_set_field", d, x);
            dpush (add_source_and_target(func_x,NULL,CB_DECIMAL(d),1));
            break;
        default:
            fprintf (stderr, "cobc:0: Unexpected tree tag %d\n", CB_TREE_TAG (x));
            ABORT ();
    }
    return d;
}

static cb_tree update_source_and_target(cb_tree assign_x, cb_tree x, cb_tree d){
    cb_tree l;
    switch (CB_TREE_TAG(assign_x)) {
        case CB_TAG_LIST:
            for (l = assign_x; l; l = CB_CHAIN(l)) {
                CB_VALUE(l) = update_source_and_target(CB_VALUE(l),x,d);
            }
            break;
        case CB_TAG_ASSIGN:
            assign_x = add_source_and_target(assign_x,x,CB_DECIMAL(d),0);
            break;
        case CB_TAG_FUNCALL:
            assign_x = add_source_and_target(assign_x,x,CB_DECIMAL(d),0);
            break;
        default: 

            break;
    }
    return assign_x;
}

char *
binary_reg_name(char * buffer, enum decimal_binary_type  binary, int id)
{
    switch (binary) {
        case int_DBT:
            sprintf (buffer, "reg.r%d.si", id);
            break;
        case uint_DBT:
            sprintf (buffer, "reg.r%d.ui", id);
            break;
        case IEEE_float_DBT: 
            sprintf (buffer, "reg.r%d.sf", id);
            break;
        case IEEE_double_DBT:
            sprintf (buffer, "reg.r%d.sd", id);
            break;
        case none_DBT: 
        default:
            fprintf (stderr, "cobc:0: binary_reg_name Unexpected decimal type %d\n", binary);
            ABORT ();
            break;
    }
    return buffer;
}

static void
decimal_assign (cb_tree x, cb_tree d, cb_tree round_opt) {
    char                buffer[200];
    cb_tree             src;
    struct cb_field     *f;
    enum cb_constant_num_type t;
    cb_tree             assign_x = NULL;
    int                 store_opt;

    store_opt = build_store_option_int (x, round_opt);

    if ( CB_DECIMAL(d)->binary ) {
        /*CIT*/
        /*Very bad hack */
        //dpush (cb_build_funcall_2 ("cob_set_int", x, d));
        /*
        if ( CB_DECIMAL(d)->binary ==  1 ) {
            sprintf (buffer, "si%d", CB_DECIMAL(d)->id);
            src = make_constant(CB_CATEGORY_NUMERIC, strdup(buffer));
            CB_CONST(src)->num_type = CB_CONST_SLL;
        } else {
            sprintf (buffer, "ui%d", CB_DECIMAL(d)->id);
            src = make_constant(CB_CATEGORY_NUMERIC, strdup(buffer));
            CB_CONST(src)->num_type = CB_CONST_ULL;
        } 
        */ 

        f = cb_field (x);

        switch ( CB_DECIMAL (d)->binary ) {
            case int_DBT:
                binary_reg_name (buffer, int_DBT, CB_DECIMAL(d)->id);
                t = CB_CONST_SLL;
                break;
            case uint_DBT:
                binary_reg_name (buffer, uint_DBT, CB_DECIMAL(d)->id);
                t = CB_CONST_ULL;
                break;
            case IEEE_float_DBT: 
                binary_reg_name (buffer, IEEE_float_DBT, CB_DECIMAL(d)->id);
                t = CB_CONST_FLOAT;
                break;
            case IEEE_double_DBT:
                binary_reg_name (buffer, IEEE_double_DBT, CB_DECIMAL(d)->id);
                t = CB_CONST_DOUBLE;
                break;
            case none_DBT: 
            default:
                fprintf (stderr, "cobc:0: decimal_assign Unexpected decimal type %d\n", CB_DECIMAL (x)->binary);
                ABORT ();
                break;
        }
        if (t>=0) {
            src = make_constant(CB_CATEGORY_NUMERIC, strdup(buffer));
            CB_CONST(src)->num_type = t;
        }
        if ((cb_flag_round_fp || cb_flag_compute_ibm) &&
            (store_opt & COB_STORE_ROUND) && (decimal_is_IEEE_float(d)) &&
            ((f->usage != CB_USAGE_DOUBLE) && (f->usage != CB_USAGE_FLOAT) )) {
            d = decimal_binary_to_decimal(d);
            assign_x = cb_build_funcall_3 ("cob_decimal_get_field", d, x, cb_int(store_opt | COB_STORE_ROUND_FP));    
        } else {
            if ((cb_flag_fp_optimize) && (decimal_is_IEEE_float(d)) &&
                ((f->usage == CB_USAGE_DOUBLE) || (f->usage == CB_USAGE_FLOAT) )) {
                assign_x = cb_build_funcall_2 ("$C", cb_build_cast(CB_CAST_DOUBLE, x), d);    
                current_statement->handler_id= 0;
                /*ICI*/
            } else if ( f->size <= 8 && (f->pic->scale <= 0) && (f->pic->digits <= 18) &&
                        (f->usage == CB_USAGE_BINARY || f->usage == CB_USAGE_COMP_5 ||
                         f->usage == CB_USAGE_COMP_X) ) {
                if ( store_opt & COB_STORE_TRUNC_ON_OVERFLOW ) {
                    switch ( CB_DECIMAL (d)->binary ) {
                        case IEEE_float_DBT: 
                        case IEEE_double_DBT:
                            d = decimal_binary_to_decimal(d);
                            assign_x = cb_build_funcall_3 ("cob_decimal_get_field", d, x, build_store_option (x, round_opt));    
                            break;
                        default:
                            if (!f->flag_compx_notrunc) {
                                dpush (cb_build_funcall_2 ("-cob_binary_mod", d, 
                                                           make_constant(CB_CATEGORY_NUMERIC, cob_exp10LL_str[f->pic->digits] )));
                            }
                            if (!f->pic->have_sign) {
                                dpush (cb_build_funcall_1 ("-cob_binary_abs_1", d));
                            }
                    }
                }
            }
        }

        if (assign_x == NULL) {
            assign_x = cb_build_move (src, x);
        }
        if (cb_flag_decimal_optimize) {
            dpush (update_source_and_target(assign_x,x,d)); 
            update_decimal_list_after_assign(x,d);  
        } else {
            dpush (assign_x); 
        }
    } else {
        //if (store_opt & COB_STORE_ROUND) {
        //    x->modified = 1;
        //}
        assign_x = cb_build_funcall_3 ("cob_decimal_get_field", d, x, build_store_option (x, round_opt));
        if (cb_flag_decimal_optimize) {
            dpush (update_source_and_target(assign_x,x,d));    
            update_decimal_list_after_assign(x,d);
        } else {
            dpush (assign_x); 
        }
    }
}

static cb_tree
build_decimal_assign (cb_tree vars, int op, cb_tree val) {
    cb_tree l;
    cb_tree t;
    cb_tree s1 = NULL;
    cb_tree d = NULL;
    int dmax=-1;
    int s;
    int mx;
    int has_float = 0;
    enum decimal_binary_type forced_type = none_DBT;
    enum decimal_binary_type used_forced_type = IEEE_double_DBT;
    enum decimal_binary_type tmp_forced_type = none_DBT;

    for ( l = vars; l; l = CB_CHAIN (l) ) {
        /* set d, VAL */
        s = get_maxscale(CB_VALUE (l), cb_flag_compute_ibm, -1);
        if (CB_PURPOSE (l) == cb_int1 && s >= 0) {
            s ++;
        }
        if (s > dmax) {
            dmax = s;
        }
    }
    mx = get_maxscale(val, cb_flag_compute_ibm, -1);
    dmax = max (dmax, mx);
    has_float = is_float_operand(val, dmax);
    for ( l = vars; l&& !has_float; l = CB_CHAIN (l) ) {
        has_float = is_float_operand(CB_VALUE(l), dmax);
    }
    if (cb_flag_round_fp || cb_flag_compute_ibm) {
        if (has_float) {
            forced_type = IEEE_double_DBT;
        } 
    }

    if (forced_type != none_DBT) {
        used_forced_type = forced_type;
    } else {
        for ( l = vars; l; l = CB_CHAIN (l) ) {
            tmp_forced_type = get_expected_binary_type_ref_field(CB_VALUE (l));
            if (tmp_forced_type < used_forced_type) {
                used_forced_type = tmp_forced_type;
            }                                
        }
    }
    if ( op == 0 ) {
        /* set d, VAL */
        d = decimal_expand (NULL, val, used_forced_type, dmax,  1, has_float);
        /* set VAR, d */
        for ( l = vars; l; l = CB_CHAIN (l) ) {
            decimal_assign (CB_VALUE (l), d, forced_type != none_DBT ? cb_int1 : CB_PURPOSE (l));
            clear_list();
            s1 = cb_list_add (s1, cb_list_reverse (decimal_stack));
            s1 = add_to_target_ref_list_decimal(s1, CB_VALUE (l),1);
            decimal_stack = NULL;
        }
    } else {
        /* set t, VAR
         * OP t, d
         * set VAR, t
         */
        d = decimal_expand (NULL, val, used_forced_type, dmax,  1, has_float);
        for ( l = vars; l; l = CB_CHAIN (l) ) {
            if (d != NULL && CB_CHAIN (l)) {
                CB_DECIMAL(d)->intermediate = 0;
                saved_decimal = CB_DECIMAL(d)->id;
            }
            t = decimal_expand (NULL, CB_VALUE (l), CB_DECIMAL(d)->binary, dmax,  0, has_float);
            //assert (CB_DECIMAL(d)->id != CB_DECIMAL(t)->id);
            t = decimal_compute (op, t, d, dmax);
            decimal_assign (CB_VALUE (l), t, forced_type != none_DBT ? cb_int1 : CB_PURPOSE (l));

            clear_list();

            s1 = cb_list_add (s1, cb_list_reverse (decimal_stack));
            s1 = add_to_target_ref_list_decimal(s1, CB_VALUE (l),1);
            decimal_stack = NULL;
        }
    }
    if (!cb_flag_decimal_optimize) {
        current_program->decimal_index = 0;
    }
    return s1;
}

static int
cb_opimizable_operation (char op) {
    if ( op == '+' || op == '-' ) {
        return 1;
    }
    if ( cb_flag_binary_optimize && ((op == '*') || (op == '/')) ) {
        return 1;
    }
    return 0;
}

cb_tree  
cb_check_optimized_field_1(cb_tree x) {
    cb_tree                     r;
    struct cb_field            *f;

    if ( (CB_REFERENCE_P (x) && CB_FIELD_P(CB_REFERENCE(x)->value))
         || CB_FIELD_P (x) ) {
        f= cb_field(x);
        if ( f->flag_usage_optimized && f->usage != f->original_usage ) {
            if ( current_statement->handler1 || current_statement->handler2 ) {
                r = cb_get_optimized_field_shadow(f);
                current_statement->before_body = cb_list_add (current_statement->before_body, cb_build_move(x, r));
                current_statement->after_body  = cb_list_add (current_statement->after_body,  cb_build_move(r, x));
                x = r;
            }
        }
    }
    if ( CB_REFERENCE_P (x) && CB_REFERENCE(x)->subs ) {
        cb_check_optimized_field (CB_REFERENCE(x)->subs, 0);
    }
    return x;
}

static cb_tree  
cb_check_optimized_field_2(cb_tree x) {
    if ( CB_PAIR_P(x) ) {
        cb_tree  a = CB_PAIR_X(x);
        cb_tree  b = CB_PAIR_Y(x);
        if ( CB_INTEGER_P(a) && (a != cb_int1) ) {
            CB_PAIR_Y(x) = cb_check_optimized_field_1(b);
        }
    }
    return x;
}

void 
cb_check_optimized_field(cb_tree vars, int round_only) {
    cb_list_map (cb_check_optimized_field_2, vars);
    if ( !round_only ) {
        cb_list_map (cb_check_optimized_field_1, vars);    
    }
}

void
cb_emit_arithmetic (cb_tree vars, int op, cb_tree val, int allow_div_optimize) {
    cb_tree         l;
    struct cb_field *f;

    val = cb_check_numeric_value (val);
    if ( op ) {
        cb_list_map (cb_check_numeric_name, vars);
    } else {
        cb_list_map (cb_check_numeric_edited_name, vars);
    }

    if ( cb_validate_one (val) ) {
        return;
    }
    if ( cb_validate_list (vars) ) {
        return;
    }
    cb_check_setting_action(val, 0);

    /*CIT*/
    if ( allow_div_optimize ) {
        if (get_maxscale(vars, cb_flag_compute_ibm, -1) == 0 && get_maxscale(val, cb_flag_compute_ibm, -1) == 0) {
            allow_binary_div_optimize = 1;
        }
    } else {
        allow_binary_div_optimize = 0;
    }

    if ( !CB_BINARY_OP_P (val) ) {
        if ( cb_opimizable_operation(op) ) {
            if ( CB_EXCEPTION_ENABLE (COB_EC_DATA_INCOMPATIBLE) && !cb_disable_runtime_check &&
                 (CB_REF_OR_FIELD_P (val)) ) {
                f = cb_field (val);
                if ( f->usage == CB_USAGE_DISPLAY ||
                     f->usage == CB_USAGE_COMP_6  ||
                     f->usage == CB_USAGE_PACKED ) {
                    cb_emit (cb_build_funcall_2 ("cob_check_numeric",
                                                 val,
                                                 cb_build_string0 ((ucharptr)(f->name))));
                }
            }
            for ( l = vars; l; l = CB_CHAIN (l) ) {
                if ( CB_EXCEPTION_ENABLE (COB_EC_DATA_INCOMPATIBLE) && !cb_disable_runtime_check &&
                     (CB_REF_OR_FIELD_P (CB_VALUE(l))) ) {
                    f = cb_field (CB_VALUE(l));
                    if ( f->usage == CB_USAGE_DISPLAY ||
                         f->usage == CB_USAGE_COMP_6  ||
                         f->usage == CB_USAGE_PACKED ) {
                        cb_emit (cb_build_funcall_2 ("cob_check_numeric",
                                                     CB_VALUE(l),
                                                     cb_build_string0 ((ucharptr)(f->name))));
                    }
                }
                if ( op == '+' ) {
                    CB_VALUE (l) = cb_build_add (CB_VALUE (l), val, CB_PURPOSE (l));
                } else if ( op == '-' ) {
                    CB_VALUE (l) = cb_build_sub (CB_VALUE (l), val, CB_PURPOSE (l));
                } else if ( op == '*' ) {
                    CB_VALUE (l) = cb_build_mul (CB_VALUE (l), val, CB_PURPOSE (l));
                } else if ( op == '/' ) {
                    CB_VALUE (l) = cb_build_div (CB_VALUE (l), val, CB_PURPOSE (l));
                } else {
                    fprintf (stderr, "cobc:0: Unexpected optimizable operation %c\n", op);
                    ABORT ();                
                }
            }
            cb_emit_list (vars);
            return;
        }
    }

    cb_emit (build_decimal_assign (vars, op, val));
}

/*
 * Condition
 */
cb_tree
cb_build_cond_88 (cb_tree x) {
    struct cb_field *f;
    cb_tree     l;
    cb_tree     t;
    cb_tree     c1 = NULL;
    cb_tree     c2;

    f = cb_field (x);
    /* refer to parent's data storage */
    x = cb_build_field_reference (f->parent, x);
    f->parent->count++;

    /* build condition */
    for ( l = f->values; l; l = CB_CHAIN (l) ) {
        t = CB_VALUE (l);
        if ( CB_PAIR_P (t) ) {
            /* VALUE THRU VALUE */
            c2 = cb_build_binary_op (cb_build_binary_op (CB_PAIR_X (t), '[', x),
                                     '&', cb_build_binary_op (x, '[', CB_PAIR_Y (t)));
        } else {
            /* VALUE */
            c2 = cb_build_binary_op (x, '=', t);
        }
        if ( c1 == NULL ) {
            c1 = c2;
        } else {
            c1 = cb_build_binary_op (c1, '|', c2);
        }
    }
    return c1;
}

static cb_tree
cb_build_optim_cond_int (struct cb_binary_op *p) {
    struct cb_field *fx = NULL;
    struct cb_field *fy = NULL;
    const char  *s;
    size_t      n;
    if ( CB_REF_OR_FIELD_P (p->x) ) {
        fx = cb_field (p->x);
    }
    if ( CB_REF_OR_FIELD_P (p->y) ) {
        fy = cb_field (p->y);
    }
    if ( !cb_flag_binary_optimize ) {
        if ( fy ) {
            if ( !fy->pic->have_sign && (fy->usage == CB_USAGE_BINARY ||
                                         fy->usage == CB_USAGE_COMP_5 ||
                                         fy->usage == CB_USAGE_COMP_X) ) {
                return cb_build_funcall_2 ("cob_cmp_uint", p->x, cb_build_cast_integer (p->y));
            }
        }
    }
    if ( fx ) {
        if (fx->usage == CB_USAGE_DOUBLE || fx->usage == CB_USAGE_FLOAT) {

            if (cb_flag_cmp_inline) {
                p = CB_BINARY_OP(cb_build_binary_op(cb_build_cast_double(p->x), p->op, cb_build_cast_integer (p->y) ));
                p->rawcondition = 1;
                return CB_TREE(p);
            } else if ( cb_flag_fp_optimize ) 
            
            {
                return cb_build_funcall_2 ("-cob_cmp_double_int",
                                           cb_build_cast_double(p->x),
                                           cb_build_cast_integer (p->y));
            }

        }
        if ( !fx->pic->scale && fx->usage == CB_USAGE_PACKED ) {
            if ( fx->pic->digits < 10 ) {
                return cb_build_funcall_2 ("-cob_cmp_packed_int",
                                           p->x,
                                           cb_build_cast_integer (p->y));
            } else {
                return cb_build_funcall_2 ("cob_cmp_packed",
                                           p->x,
                                           cb_build_cast_integer (p->y));
            }
        }
        if ( !fx->pic->scale && fx->usage == CB_USAGE_DISPLAY &&
             !fx->flag_sign_leading && !fx->flag_sign_separate ) {
            if ( cb_flag_displaynumeric_mf50 || cb_move_xto9_mode == CB_MOVExTO9_MF50 ) {
                if (fy && !fy->pic->scale && fy->usage == CB_USAGE_DISPLAY &&
                    !fy->flag_sign_leading && !fy->flag_sign_separate &&
                    !fx->pic->have_sign && !fy->pic->have_sign) {
                    return cb_build_funcall_4 ("cob_cmp_display_mf50",
                                               cb_build_cast_address (p->x),
                                               cb_int (fx->size),
                                               cb_build_cast_address (p->y),
                                               cb_int (fy->size));

                }
            }
            if ( cb_fits_int (p->x) ) {
                if ( !fx->pic->have_sign ) {
                    return cb_build_funcall_3 ("cob_cmp_numdisp",
                                               cb_build_cast_address (p->x),
                                               cb_int (fx->size),
                                               cb_build_cast_integer (p->y));
                } else {
                    return cb_build_funcall_3 ("cob_cmp_sign_numdisp",
                                               cb_build_cast_address (p->x),
                                               cb_int (fx->size),
                                               cb_build_cast_integer (p->y));
                }
            } else if ( cb_fits_long_long (p->x) ) {
                if ( !fx->pic->have_sign ) {
                    return cb_build_funcall_3 ("cob_cmp_long_numdisp",
                                               cb_build_cast_address (p->x),
                                               cb_int (fx->size),
                                               cb_build_cast_integer (p->y));
                } else {
                    return cb_build_funcall_3 ("cob_cmp_long_sign_numdisp",
                                               cb_build_cast_address (p->x),
                                               cb_int (fx->size),
                                               cb_build_cast_integer (p->y));
                }
            }
        }
        if ( !fx->pic->scale && (fx->usage == CB_USAGE_BINARY ||
                                 fx->usage == CB_USAGE_COMP_5 ||
                                 fx->usage == CB_USAGE_INDEX ||
                                 fx->usage == CB_USAGE_COMP_X ) ) {

            if (cb_flag_cmp_inline) {
                p = CB_BINARY_OP(cb_build_binary_op(cb_build_cast_integer(p->x), p->op, cb_build_cast_integer (p->y)));
                p->rawcondition = 1;
                return CB_TREE(p);
            }
            n = (fx->size - 1) + (8 * (fx->pic->have_sign ? 1 : 0)) +
                (16 * (fx->flag_binary_swap ? 1 : 0));
#if     defined(COB_NON_ALIGNED) && !defined(_MSC_VER)
            switch ( fx->size ) {
                case 2:
#ifdef      COB_SHORT_BORK
                    s = bin_compare_funcs[n];
                    break;
#endif
                case 4:
                case 8:
                    if ( fx->storage != CB_STORAGE_LINKAGE &&
                         fx->indexes == 0 && (fx->offset % fx->size) == 0 ) {
                        s = align_bin_compare_funcs[n];
                    } else {
                        s = bin_compare_funcs[n];
                    }
                    break;
                default:
                    s = bin_compare_funcs[n];
                    break;
            }
#else
            s = bin_compare_funcs[n];
#endif
            if ( s ) {
                return cb_build_funcall_2 (s,
                                           cb_build_cast_address (p->x),
                                           cb_build_cast_integer (p->y));
            }
        }
    }

    if ( CB_REFERENCE_P (p->y) || CB_FIELD_P (p->y) ) {
        fy = cb_field (p->y);
        if ( !fy->pic->have_sign && (fy->usage == CB_USAGE_BINARY ||
                                     fy->usage == CB_USAGE_COMP_5 ||
                                     fy->usage == CB_USAGE_COMP_X) ) {
            return cb_build_funcall_2 ("cob_cmp_uint", p->x, cb_build_cast_integer (p->y));
        }
    }

    return cb_build_funcall_2 ("cob_cmp_int", p->x, cb_build_cast_integer (p->y));
}

static int
cb_chk_num_cond (cb_tree x, cb_tree y) {
    struct cb_field     *fx;
    struct cb_field     *fy;
    struct cb_literal   *lx;
    struct cb_literal   *ly;
    int xsign, ysign, xscale, yscale, xsize, ysize;

    if ( !CB_REFERENCE_P (x) && !CB_FIELD_P (x) && !CB_LITERAL_P(x)) {
        return 0;
    }
    if ( !CB_REFERENCE_P (y) && !CB_FIELD_P (y)  && !CB_LITERAL_P(y)) {
        return 0;
    }
    if ( CB_TREE_CATEGORY (x) != CB_CATEGORY_NUMERIC ) {
        return 0;
    }
    if ( CB_TREE_CATEGORY (y) != CB_CATEGORY_NUMERIC ) {
        return 0;
    }
    if ( CB_TREE_CLASS (x) != CB_CLASS_NUMERIC ) {
        return 0;
    }
    if ( CB_TREE_CLASS (y) != CB_CLASS_NUMERIC ) {
        return 0;
    }
    if (!CB_LITERAL_P(x)) {
        fx = cb_field (x);
        if ( fx->usage != CB_USAGE_DISPLAY ) {
            return 0;
        }
        xsign  = fx->pic->have_sign;
        xscale = fx->pic->scale;
        xsize  = fx->size;
    } else {
        lx = CB_LITERAL(x);
        xsign  = lx->sign;
        xscale = lx->scale;
        xsize  = lx->size;
        if (lx->all) {
            return 0;
        }
    }
    if (!CB_LITERAL_P(y)) {
        fy = cb_field (y);
        if ( fy->usage != CB_USAGE_DISPLAY ) {
            return 0;
        }
        ysign  = fy->pic->have_sign;
        yscale = fy->pic->scale;
        ysize  = fy->size;
    } else {
        ly = CB_LITERAL(y);
        ysign  = ly->sign;
        yscale = ly->scale;
        ysize  = ly->size;
        if (ly->all) {
            return 0;
        }
    }
    if ( xsign || ysign ) {
        return 0;
    }
    if ( xsize != ysize ) {
        return 0;
    }
    if ( xscale != yscale ) {
        return 0;
    }
    /*CIT*/
    if (!cb_flag_raw_compare) {
        return 0;
    }
    if ( cb_flag_mf_spzero || cb_flag_hostnumcompare_1) {
        return 0;
    }
    if ( cb_flag_displaynumeric_mf50 || cb_move_xto9_mode == CB_MOVExTO9_MF50 ) {
        return 0;
    }
    return 1;
}

/*CIT*/
static int
cb_chk_spzero_cond (cb_tree x, cb_tree y) {
    struct cb_field     *fx;
    struct cb_field     *fy;

    if ( CB_TREE_CATEGORY (x) != CB_CATEGORY_NUMERIC ) {
        return 0;
    }
    if ( CB_TREE_CATEGORY (y) != CB_CATEGORY_NUMERIC ) {
        return 0;
    }
    fx = cb_field (x);
    fy = cb_field (y);
    if ( fx->usage != CB_USAGE_DISPLAY ) {
        return 0;
    }
    if ( fy->usage != CB_USAGE_DISPLAY ) {
        return 0;
    }
    if (!cb_flag_raw_compare) {
        return 0;
    }
    return cb_flag_mf_spzero;
}

static int
cb_chk_alpha_cond (cb_tree x) {
    if ( current_program->alphabet_name_list ) {
        return 0;
    }
    if ( CB_LITERAL_P (x) ) {
        return 1;
    }
    if ( !CB_REFERENCE_P (x) && !CB_FIELD_P (x) ) {
        return 0;
    }
    if ( CB_TREE_CATEGORY (x) != CB_CATEGORY_ALPHANUMERIC &&
         CB_TREE_CATEGORY (x) != CB_CATEGORY_ALPHABETIC ) {
        return 0;
    }
    if ( cb_field_variable_size (cb_field (x)) ) {
        return 0;
    }
    if ( cb_field_size (x) < 0 ) {
        return 0;
    }
    return 1;
}

static cb_tree
cb_expand_literal (cb_tree src, int size)
{
    unsigned char       *buff;
    struct cb_literal   *l;
    int i;
    int j;
    l = CB_LITERAL_ENCODED(src);
    buff = cobc_malloc (size);
    memset (buff,  CHAR_SP, size);
    j = 0;
    do {
        for ( i = 0; i < l->size && j < size; i++ , j++) {
            buff[j] = l->data[i];
        }
    } while ( l->all && j < size );
    return cb_build_string(buff, size);

}


cb_tree
cb_build_cond (cb_tree x) {
    int                 size1;
    int                 size2;
    struct cb_field     *f;
    struct cb_binary_op *p;
    cb_tree             d1;
    cb_tree             d2;

    if (x) {
        switch ( CB_TREE_TAG (x) ) {
            case CB_TAG_CONST:
            case CB_TAG_FUNCALL:
                return x;
            case CB_TAG_REFERENCE:
                if ( !CB_FIELD_P (cb_ref (x)) ) {
                    return cb_build_cond (cb_ref (x));
                }

                f = cb_field (x);

                /* level 88 condition */
                if ( f->level == 88 ) {
                    /* We need to build a 88 condition at every occurrence
                       instead of once at the beginning because a 88 item
                       may be subscripted (i.e., it is not a constant tree). */
                    return cb_build_cond (cb_build_cond_88 (x));
                }

                /* Usage Bit condition */
                if ( f->usage == CB_USAGE_BIT ) {
                    return cb_build_value(x);
                }
                cb_error_x (x, _("Invalid expression in condition field %s is not a level 88"), f->name);
                return cb_error_node;
            case CB_TAG_BINARY_OP:
                p = CB_BINARY_OP (x);
                switch ( p->op ) {
                    case 'v':
                        return cb_build_value(x);
                        break;
                    case 'n':
                        if (!p->x) {
                            cb_error_x (x, _("Invalid condition expression"));
                            return cb_error_node;
                        }
                        return cb_build_not_value(cb_build_cond(p->x));
                        break;
                    case 'a':
                    case 'o':
                    case 'y':
                        if (!p->x) {
                            cb_error_x (x, _("Invalid condition expression"));
                            return cb_error_node;
                        }
                        /* cb_build_negation (cb_build_binary_op(x, '=', cb_zero));*/
                        return cb_build_binary_op (cb_build_cond (p->x), p->op, cb_build_cond (p->y));
                        break;
                    case '!':
                        if (!p->x) {
                            cb_error_x (x, _("Invalid condition expression"));
                            return cb_error_node;
                        }
                        return cb_build_negation (cb_build_cond (p->x));
                    case '&':
                    case '|':
                        if (!p->x) {
                            cb_error_x (x, _("Invalid condition expression"));
                            return cb_error_node;
                        }
                        return cb_build_binary_op (cb_build_cond (p->x), p->op, cb_build_cond (p->y));
                    default:
                        if (!p->x) {
                            cb_error_x (x, _("Invalid condition expression"));
                            return cb_error_node;
                        }
                        if ( CB_INDEX_P (p->x) || CB_INDEX_P (p->y)
                             || CB_TREE_CLASS (p->x) == CB_CLASS_POINTER
                             || CB_TREE_CLASS (p->y) == CB_CLASS_POINTER ) {
                            x = cb_build_binary_op (p->x, '-', p->y);
                        } else if ( CB_BINARY_OP_P (p->x) || CB_BINARY_OP_P (p->y) ) {
                            int mx = get_maxscale(p->x, cb_flag_compute_ibm, -1);
                            int my = get_maxscale(p->y, cb_flag_compute_ibm, -1);
                            int maxscale = max (mx, my);
                            enum decimal_binary_type forced_type =  none_DBT;
                            /*CIT*/
                            int has_float = is_float_operand(p->x, maxscale) || is_float_operand(p->y, maxscale) ;
                            if ((cb_flag_round_fp || cb_flag_compute_ibm ) &&
                                (has_float)) {
                                forced_type = IEEE_double_DBT;
                            }

                            d1 = decimal_expand (NULL, p->x, forced_type == none_DBT ? get_expected_binary_type_ref_field(p->x) : forced_type, maxscale,  1, has_float);
                            d2 = decimal_expand (NULL, p->y, forced_type == none_DBT ? get_expected_binary_type_ref_field(p->y) : forced_type, maxscale,  1, has_float);
                            if ( CB_DECIMAL(d1)->binary && CB_DECIMAL(d2)->binary ) {
                                dpush (cb_build_funcall_2 ("-cob_binary_cmp", d1, d2));
                            } else {
                                d1 = decimal_binary_to_decimal(d1);
                                d2 = decimal_binary_to_decimal(d2);
                                dpush (cb_build_funcall_2 ("cob_decimal_cmp", d1, d2));
                            }
                            clear_list();
                            if (!cb_flag_decimal_optimize) {
                                current_program->decimal_index = 0;
                            }
                            x = cb_list_reverse (decimal_stack);
                            decimal_stack = NULL;
                        } else {
                            if ( cb_chk_num_cond (p->x, p->y) ) {
                                size1 = cb_field_size (p->x);
                                if (size1 == 1) {
                                    x = cb_build_funcall_2 ("$G", p->x, p->y);
                                } else {
                                    x = cb_build_funcall_3 ("-memcmp",
                                                            cb_build_cast_address (p->x),
                                                            cb_build_cast_address (p->y),
                                                            cb_int (size1));
                                }
                                break;
                            }
                            if ( CB_TREE_CLASS (p->x) == CB_CLASS_NUMERIC
                                 && CB_TREE_CLASS (p->y) == CB_CLASS_NUMERIC
                                 && cb_fits_int (p->y) ) {
                                x = cb_build_optim_cond_int (p);
                                break;
                            }
                            if ( CB_TREE_CLASS (p->x) == CB_CLASS_NUMERIC &&
                                 cb_is_double(p->x) &&
                                 CB_TREE_CLASS (p->y) == CB_CLASS_NUMERIC &&
                                 cb_is_double(p->y)) {
                                if (cb_flag_cmp_inline) {
                                    p = CB_BINARY_OP(cb_build_binary_op(cb_build_cast_double(p->x), p->op, cb_build_cast_double (p->y) ));
                                    p->rawcondition = 1;
                                    x= CB_TREE(p);
                                } else if ( cb_flag_fp_optimize ) {
                                    x= cb_build_funcall_2 ("-cob_cmp_double_double",
                                                           cb_build_cast_double(p->x),
                                                           cb_build_cast_double (p->y));
                                } else {
                                    x = cb_build_funcall_2 ("cob_numeric_cmp", p->x, p->y);
                                }
                                break;
                            }

                            /* field comparison */
                            if ( (CB_REF_OR_FIELD_P (p->x))
                                 && (CB_TREE_CATEGORY (p->x) == CB_CATEGORY_ALPHANUMERIC ||
                                     CB_TREE_CATEGORY (p->x) == CB_CATEGORY_ALPHABETIC)
                                 && (cb_field_size (p->x) == 1)
                                 && (!current_program->alphabet_name_list)
                                 && (p->y == cb_space || p->y == cb_low ||
                                     p->y == cb_high || p->y == cb_zero) ) {
                                x = cb_build_funcall_2 ("$G", p->x, p->y);
                                break;
                            }
                            if ( cb_chk_alpha_cond (p->x) && cb_chk_alpha_cond (p->y) ) {
                                size1 = cb_field_size (p->x);
                                size2 = cb_field_size (p->y);
                            } else {
                                size1 = 0;
                                size2 = 0;
                            }
                            if (cb_flag_cmp_optimize && size1 == 1 && size2 == 1 ) {
                                x = cb_build_funcall_2 ("$G", p->x, p->y);
                            } else if (cb_flag_cmp_optimize &&  size1 != 0 && size1 == size2 && !cb_chk_spzero_cond(p->x, p->y) ) {
                                x = cb_build_funcall_3 ("-memcmp",
                                                        cb_build_cast_address (p->x),
                                                        cb_build_cast_address (p->y),
                                                        cb_int (size1));
                            } else if (cb_flag_cmp_optimize &&  size1 != 0 && size1 > size2 && CB_LITERAL_P (p->y)  && size1 <= cb_max_literal_expand) {
                                x = cb_build_funcall_3 ("-memcmp",
                                                        cb_build_cast_address (p->x),
                                                        cb_expand_literal (p->y, size1),
                                                        cb_int (size1));
                            } else if (cb_flag_cmp_optimize &&  size2 != 0 && size2 > size1 && CB_LITERAL_P (p->x) && size2 <= cb_max_literal_expand ) {
                                x = cb_build_funcall_3 ("-memcmp",
                                                        cb_expand_literal (p->x, size2),
                                                        cb_build_cast_address (p->y),
                                                        cb_int (size2));
                            } else {
                                if ( CB_TREE_CLASS (p->x) == CB_CLASS_NUMERIC && p->y == cb_zero ) {
                                    x = cb_build_optim_cond_int (p);
                                } else if (CB_TREE_CLASS (p->x) == CB_CLASS_NUMERIC && CB_TREE_CLASS (p->y) == CB_CLASS_NUMERIC) {
                                    x = cb_build_funcall_2 ("cob_numeric_cmp", p->x, p->y);
                                } else {
                                    x = cb_build_funcall_2 ("cob_cmp", p->x, p->y);
                                }
                            }
                        }
                }
                if (CB_BINARY_OP_P(x) && CB_BINARY_OP(x)->rawcondition) {
                    return x;
                } else {
                    return cb_build_binary_op (x, p->op, p->y);
                }
            default:
                cb_error_x (x, _("Invalid condition expression"));
                return cb_error_node;
        }
        /* NOT REACHED */
        return x;
    } else {
        cb_error (_("null condition expression"));
        return cb_error_node;
    }
}

/*
 * ADD/SUBTRACT CORRESPONDING
 */
static int 
litteral_scale_may_match(cb_tree v, cb_tree n)
{
    struct cb_field *f;
    int res =0;

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        if (f->pic  && cb_fits_int(n) ) {
            if (f->pic->scale > 0 && f->pic->scale < 10  && CB_LITERAL_P(n)) {
                res = 1; 
            } else {
                res = !f->pic->scale;
            }
        }
    }
    return res;
}

static cb_tree 
adjust_litteral_scale(cb_tree v, cb_tree n) {
    struct cb_field *f;

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        if (f->pic->scale > 0 && f->pic->scale < 10 && cb_fits_int(n) && CB_LITERAL_P(n)) {
            int v = cb_get_int(n);
            v *= cb_exp10[f->pic->scale];
            n = cb_int(v);
            CB_INTEGER(n)->scale = f->pic->scale;
        }
    }
    return n;
}
static cb_tree
cb_build_optim_add (cb_tree v, cb_tree n) {
    size_t      z;
    const char  *s;
    struct cb_field *f;

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        if ( litteral_scale_may_match(v,n) && (f->usage == CB_USAGE_BINARY ||
                                               f->usage == CB_USAGE_COMP_5 ||
                                               f->usage == CB_USAGE_COMP_X) ) {
            n = adjust_litteral_scale(v,n);
            z = (f->size - 1) + (8 * (f->pic->have_sign ? 1 : 0)) +
                (16 * (f->flag_binary_swap ? 1 : 0));
#if     defined(COB_NON_ALIGNED) && !defined(_MSC_VER)
            switch ( f->size ) {
                case 2:
#ifdef      COB_SHORT_BORK
                    s = bin_add_funcs[z];
                    break;
#endif
                case 4:
                case 8:
                    if ( f->storage != CB_STORAGE_LINKAGE &&
                         f->indexes == 0 && (f->offset % f->size) == 0 ) {
                        s = align_bin_add_funcs[z];
                    } else {
                        s = bin_add_funcs[z];
                    }
                    break;
                default:
                    s = bin_add_funcs[z];
                    break;
            }
#else
            if ( f->usage == CB_USAGE_COMP_5 && !f->flag_binary_swap ) {
                switch ( f->size ) {
                    case 1:
                    case 2:
                    case 4:
                    case 8:
                        return cb_build_assign (v, cb_build_binary_op (v, '+', n));
                }
            }
            s = bin_add_funcs[z];
#endif
            if ( s ) {
                return cb_build_funcall_2 (s,
                                           cb_build_cast_address (v),
                                           cb_build_cast_integer (n));
            }
        } else if ( !f->pic->scale && f->usage == CB_USAGE_PACKED &&
                    f->pic->digits < 10 ) {
            return cb_build_funcall_2 ("cob_add_packed_int",
                                       v, cb_build_cast_integer (n));
        } else if ( (f->usage == CB_USAGE_DISPLAY) && 
                    !(f->pic->have_sign) && (f->pic->scale == 0) &&
                    cb_fits_one_positive_digit(n)) {
            return cb_build_funcall_2 ("cob_unsign_display_fast_add_small_int",
                                       v, cb_build_cast_integer (n));
        } else if ( (f->usage == CB_USAGE_DISPLAY) && 
                    !(f->pic->have_sign && (f->pic->scale > 0)) ) {
            return cb_build_funcall_2 ("cob_display_add_int",
                                       v, cb_build_cast_integer (n));
        }

    }
    return cb_build_funcall_2 ("cob_add_int", v, cb_build_cast_integer (n));
}

static cb_tree
cb_build_optim_sub (cb_tree v, cb_tree n) {
    size_t      z;
    const char  *s;
    struct cb_field *f;

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        if ( litteral_scale_may_match(v,n) && (f->usage == CB_USAGE_BINARY ||
                                               f->usage == CB_USAGE_COMP_5 ||
                                               f->usage == CB_USAGE_COMP_X) ) {
            n = adjust_litteral_scale(v,n);
            z = (f->size - 1) + (8 * (f->pic->have_sign ? 1 : 0)) +
                (16 * (f->flag_binary_swap ? 1 : 0));
#if     defined(COB_NON_ALIGNED) && !defined(_MSC_VER)
            switch ( f->size ) {
                case 2:
#ifdef      COB_SHORT_BORK
                    s = bin_sub_funcs[z];
                    break;
#endif
                case 4:
                case 8:
                    if ( f->storage != CB_STORAGE_LINKAGE &&
                         f->indexes == 0 && (f->offset % f->size) == 0 ) {
                        s = align_bin_sub_funcs[z];
                    } else {
                        s = bin_sub_funcs[z];
                    }
                    break;
                default:
                    s = bin_sub_funcs[z];
                    break;
            }
#else
            if ( f->usage == CB_USAGE_COMP_5 && !f->flag_binary_swap) {
                switch ( f->size ) {
                    case 1:
                    case 2:
                    case 4:
                    case 8:
                        return cb_build_assign (v, cb_build_binary_op (v, '-', n));
                }
            }
            s = bin_sub_funcs[z];
#endif
            if ( s ) {
                return cb_build_funcall_2 (s,
                                           cb_build_cast_address (v),
                                           cb_build_cast_integer (n));
            }
        }

    }
    return cb_build_funcall_2 ("cob_sub_int", v, cb_build_cast_integer (n));
}

static cb_tree
cb_build_optim_mul (cb_tree v, cb_tree n) {
    size_t      z;
    const char  *s;
    struct cb_field *f;

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        if ( cb_fits_int(n) && (f->usage == CB_USAGE_BINARY ||
                                f->usage == CB_USAGE_COMP_5 ||
                                f->usage == CB_USAGE_COMP_X) ) {
            z = (f->size - 1) + (8 * (f->pic->have_sign ? 1 : 0)) +
                (16 * (f->flag_binary_swap ? 1 : 0));
#if     defined(COB_NON_ALIGNED) && !defined(_MSC_VER)
            switch ( f->size ) {
                case 2:
#ifdef      COB_SHORT_BORK
                    s = bin_mul_funcs[z];
                    break;
#endif
                case 4:
                case 8:
                    if ( f->storage != CB_STORAGE_LINKAGE &&
                         f->indexes == 0 && (f->offset % f->size) == 0 ) {
                        s = align_bin_mul_funcs[z];
                    } else {
                        s = bin_mul_funcs[z];
                    }
                    break;
                default:
                    s = bin_mul_funcs[z];
                    break;
            }
#else
            if ( f->usage == CB_USAGE_COMP_5 && !f->flag_binary_swap ) {
                switch ( f->size ) {
                    case 1:
                    case 2:
                    case 4:
                    case 8:
                        return cb_build_assign (v, cb_build_binary_op (v, '*', n));
                }
            }
            s = bin_mul_funcs[z];
#endif
            if ( s ) {
                return cb_build_funcall_2 (s,
                                           cb_build_cast_address (v),
                                           cb_build_cast_integer (n));
            }
        }
    }
    return cb_build_funcall_3 ("cob_mul", v, n, cb_int0);
}

static cb_tree
cb_build_optim_div (cb_tree v, cb_tree n) {
    size_t      z;
    const char  *s;
    struct cb_field *f;

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        if ( cb_fits_int(n) && (f->usage == CB_USAGE_BINARY ||
                                f->usage == CB_USAGE_COMP_5 ||
                                f->usage == CB_USAGE_COMP_X) ) {
            z = (f->size - 1) + (8 * (f->pic->have_sign ? 1 : 0)) +
                (16 * (f->flag_binary_swap ? 1 : 0));
#if     defined(COB_NON_ALIGNED) && !defined(_MSC_VER)
            switch ( f->size ) {
                case 2:
#ifdef      COB_SHORT_BORK
                    s = bin_div_funcs[z];
                    break;
#endif
                case 4:
                case 8:
                    if ( f->storage != CB_STORAGE_LINKAGE &&
                         f->indexes == 0 && (f->offset % f->size) == 0 ) {
                        s = align_bin_div_funcs[z];
                    } else {
                        s = bin_div_funcs[z];
                    }
                    break;
                default:
                    s = bin_div_funcs[z];
                    break;
            }
#else
            if ( f->usage == CB_USAGE_COMP_5 && !f->flag_binary_swap) {
                switch ( f->size ) {
                    case 1:
                    case 2:
                    case 4:
                    case 8:
                        return cb_build_assign (v, cb_build_binary_op (v, '/', n));
                }
            }
            s = bin_div_funcs[z];
#endif
            if ( s ) {
                return cb_build_funcall_2 (s,
                                           cb_build_cast_address (v),
                                           cb_build_cast_integer (n));
            }
        }
    }
    return cb_build_funcall_3 ("cob_div", v, n, cb_int0);
}

cb_tree
cb_build_add (cb_tree v, cb_tree n, cb_tree round_opt) {
    cb_tree         opt;
    struct cb_field *f;
    cb_tree         res = NULL;


#ifdef  COB_NON_ALIGNED
    if ( CB_INDEX_P (v) ) {
        return cb_build_move (cb_build_binary_op (v, '+', n), v);
    } else {
        if ( CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
            current_program->gen_ptrmanip = 1;
            res = cb_build_funcall_3 ("cob_pointer_manip", v, n, cb_int0);
        }
#else
    if ( CB_INDEX_P (v) || CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
        return cb_build_move (cb_build_binary_op (v, '+', n), v);
    } else {

#endif

        cb_check_setting_action(v, 0);

        if ( CB_REF_OR_FIELD_P (v) ) {
            f = cb_field (v);
            f->count++;
        }
        if ( CB_REF_OR_FIELD_P (n) ) {
            f = cb_field (n);
            f->count++;
        }
        if ( round_opt == cb_high ) {
            if ( cb_fits_int (n) ) {
                res = cb_build_optim_add (v, n);
            } else {
                res = cb_build_funcall_3 ("cob_add", v, n, cb_int0);
            }
        } else {
            opt = build_store_option (v, round_opt);
            if ( opt == cb_int0 && cb_fits_int (n) ) {
                res = cb_build_optim_add (v, n);
            } else {
                res = cb_build_funcall_3 ("cob_add", v, n, opt);    
            }
        }
    }

    return add_to_target_ref_list(res, v);
}

cb_tree
cb_build_sub (cb_tree v, cb_tree n, cb_tree round_opt) {
    cb_tree     opt;
    struct cb_field *f;
    cb_tree        res = NULL;

#ifdef  COB_NON_ALIGNED
    if ( CB_INDEX_P (v) ) {
        return cb_build_move (cb_build_binary_op (v, '-', n), v);
    }
    if ( CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
        current_program->gen_ptrmanip = 1;
        return add_to_target_ref_list(cb_build_funcall_3 ("cob_pointer_manip", v, n, cb_int1), v);
    }
#else
    if ( CB_INDEX_P (v) || CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
        return cb_build_move (cb_build_binary_op (v, '-', n), v);
    }
#endif

    cb_check_setting_action(v, 0);

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        f->count++;
    }
    if ( CB_REF_OR_FIELD_P (n) ) {
        f = cb_field (n);
        f->count++;
    }
    opt = build_store_option (v, round_opt);
    if ( opt == cb_int0 && cb_fits_int (n) ) {
        res = cb_build_optim_sub (v, n);
    } else
        res = cb_build_funcall_3 ("cob_sub", v, n, opt);

    return add_to_target_ref_list(res, v);
}

cb_tree
cb_build_mul (cb_tree v, cb_tree n, cb_tree round_opt) {
    cb_tree         opt;
    cb_tree        res = NULL;
    struct cb_field *f;

#ifdef  COB_NON_ALIGNED
    if ( CB_INDEX_P (v) ) {
        return cb_build_move (cb_build_binary_op (v, '*', n), v);
    }
    if ( CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
        current_program->gen_ptrmanip = 1;
        return add_to_target_ref_list(cb_build_funcall_3 ("cob_pointer_manip", v, n, cb_int0), v);
    }
#else
    if ( CB_INDEX_P (v) || CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
        return cb_build_move (cb_build_binary_op (v, '*', n), v);
    }
#endif

    cb_check_setting_action(v, 0);

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        f->count++;
    }
    if ( CB_REF_OR_FIELD_P (n) ) {
        f = cb_field (n);
        f->count++;
    }
    if ( round_opt == cb_high ) {
        if ( cb_fits_int (n) ) {
            res = cb_build_optim_mul (v, n);
        } else {
            res = cb_build_funcall_3 ("cob_mul", v, n, cb_int0);
        }
    } else {
        opt = build_store_option (v, round_opt);
        if ( opt == cb_int0 && cb_fits_int (n) ) {
            res = cb_build_optim_mul (v, n);
        } else {
            res = cb_build_funcall_3 ("cob_mul", v, n, opt);
        }
    }
    return add_to_target_ref_list(res,v);
}

cb_tree
cb_build_div (cb_tree v, cb_tree n, cb_tree round_opt) {
    cb_tree         opt;
    struct cb_field *f;
    cb_tree        res = NULL;

#ifdef  COB_NON_ALIGNED
    if ( CB_INDEX_P (v) ) {
        return cb_build_move (cb_build_binary_op (v, '/', n), v);
    }
    if ( CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
        current_program->gen_ptrmanip = 1;
        return add_to_target_ref_list(cb_build_funcall_3 ("cob_pointer_manip", v, n, cb_int0),v);
    }
#else
    if ( CB_INDEX_P (v) || CB_TREE_CLASS (v) == CB_CLASS_POINTER ) {
        return cb_build_move (cb_build_binary_op (v, '/', n), v);
    }
#endif
    cb_check_setting_action(v, 0);

    if ( CB_REF_OR_FIELD_P (v) ) {
        f = cb_field (v);
        f->count++;
    }
    if ( CB_REF_OR_FIELD_P (n) ) {
        f = cb_field (n);
        f->count++;
    }
    if ( round_opt == cb_high ) {
        if ( cb_fits_int (n) ) {
            res = cb_build_optim_div (v, n);
        } else {
            res = cb_build_funcall_3 ("cob_div", v, n, cb_int0);
        }
    } else {
        opt = build_store_option (v, round_opt);
        if ( opt == cb_int0 && cb_fits_int (n) ) {
            res = cb_build_optim_div (v, n);
        } else
            res = cb_build_funcall_3 ("cob_div", v, n, opt);
    }
    return add_to_target_ref_list(res, v);
}


static void
emit_corresponding (cb_tree (*func) (cb_tree f1, cb_tree f2, cb_tree f3),
                    cb_tree x1, cb_tree x2, cb_tree opt) {
    struct cb_field *f1, *f2;
    cb_tree     t1;
    cb_tree     t2;

    for ( f1 = cb_field (x1)->children; f1; f1 = f1->sister ) {
        if ( !f1->redefines && !f1->flag_occurs ) {
            for ( f2 = cb_field (x2)->children; f2; f2 = f2->sister ) {
                if ( !f2->redefines && !f2->flag_occurs ) {
                    if ( strcmp (f1->name, f2->name) == 0 ) {
                        t1 = cb_build_field_reference (f1, x1);
                        t2 = cb_build_field_reference (f2, x2);
                        if ( f1->children && f2->children ) {
                            emit_corresponding (func, t1, t2, opt);
                        } else {
                            cb_emit (func (t1, t2, opt));
                        }
                    }
                }
            }
        }
    }
}

void
cb_emit_corresponding (cb_tree (*func) (cb_tree f1, cb_tree f2, cb_tree f3),
                       cb_tree x1, cb_tree x2, cb_tree opt) {
    x1 = cb_check_group_name (x1);
    x2 = cb_check_group_name (x2);

    if ( cb_validate_one (x1) ) {
        return;
    }
    if ( cb_validate_one (x2) ) {
        return;
    }

    emit_corresponding (func, x1, x2, opt);
}

/*
static int 
need_emit_fixvalue (cb_tree x1, cb_tree x2)
{
    struct cb_literal   *l;
    int i;
    if ( (!CB_CONST_P(x1) &&  (CB_TREE_CATEGORY(x1) == CB_CATEGORY_ALPHANUMERIC || 
                               CB_TREE_CATEGORY(x1) == CB_CATEGORY_ALPHANUMERIC_EDITED)) 
       ) {
        if ( CB_LITERAL_P(x1) ) {
            l = CB_LITERAL(x1);
            for ( i = 0;  i < l->size; i++ ) {
                if ( l->data[i] == ' ' ) {
                    return 1;
                }
            }
            return 0;
        }
        return 1;
    } else if ( x1 == cb_space ) {
        return 1;
    }
    x2 = x2;
    return 0;
}
*/
static void
emit_move_corresponding (cb_tree x1, cb_tree x2) {
    struct cb_field *f1, *f2;
    cb_tree     t1;
    cb_tree     t2;

    for ( f1 = cb_field (x1)->children; f1; f1 = f1->sister ) {
        if ( !f1->redefines && !f1->flag_occurs ) {
            for ( f2 = cb_field (x2)->children; f2; f2 = f2->sister ) {
                if ( !f2->redefines && !f2->flag_occurs ) {
                    if ( strcmp (f1->name, f2->name) == 0 ) {
                        t1 = cb_build_field_reference (f1, x1);
                        t2 = cb_build_field_reference (f2, x2);
                        if ( f1->children && f2->children ) {
                            emit_move_corresponding (t1, t2);
                        } else {
                            cb_emit (cb_build_move (t1, t2));
                            /*CIT*/
                            /*
                            if ( cb_flag_spzero && need_emit_fixvalue(t1,t2)) {
                                cb_emit_fixvalue(t2, 1);
                            } 
                            */ 
                        }
                    }
                }
            }
        }
    }
}

void
cb_emit_move_corresponding (cb_tree x1, cb_tree x2) {
    cb_tree     l;
    cb_tree     v;

    x1 = cb_check_group_name (x1);
    if ( cb_validate_one (x1) ) {
        return;
    }
    for ( l = x2; l; l = CB_CHAIN(l) ) {
        v = CB_VALUE(l);
        v = cb_check_group_name (v);
        if ( cb_validate_one (v) ) {
            return;
        }
        emit_move_corresponding (x1, v);
    }
}

static void
output_screen_from (struct cb_field *p, const size_t sisters) {
    int type;

    if ( sisters && p->sister ) {
        output_screen_from (p->sister, 1);
    }
    if ( p->children ) {
        output_screen_from (p->children, 1);
    }

    type = (p->children ? COB_SCREEN_TYPE_GROUP :
            p->values ? COB_SCREEN_TYPE_VALUE :
            (p->size > 0) ? COB_SCREEN_TYPE_FIELD : COB_SCREEN_TYPE_ATTRIBUTE);
    if ( type == COB_SCREEN_TYPE_FIELD && p->screen_from ) {
        cb_emit (cb_build_funcall_2 ("cob_move", p->screen_from, CB_TREE (p)));
    }
}

static void
output_screen_to (struct cb_field *p, const size_t sisters) {
    int type;

    if ( sisters && p->sister ) {
        output_screen_to (p->sister, 1);
    }
    if ( p->children ) {
        output_screen_to (p->children, 1);
    }

    type = (p->children ? COB_SCREEN_TYPE_GROUP :
            p->values ? COB_SCREEN_TYPE_VALUE :
            (p->size > 0) ? COB_SCREEN_TYPE_FIELD : COB_SCREEN_TYPE_ATTRIBUTE);
    if ( type == COB_SCREEN_TYPE_FIELD && p->screen_to ) {
        cb_emit (cb_build_funcall_2 ("cob_move", CB_TREE (p), p->screen_to));
    }
}

/*
 * ACCEPT statement
 */

void
cb_emit_accept (cb_tree var, cb_tree line, cb_tree column, cb_tree fgc, cb_tree bgc,
                cb_tree scroll, int dispattrs, char dispprompt, 
                cb_tree excp_field, cb_tree timeout) {

    if ( cb_validate_one (var) ) {
        return;
    }
    if ( cb_validate_one (line) ) {
        return;
    }
    if ( cb_validate_one (column) ) {
        return;
    }
    if ( cb_validate_one (fgc) ) {
        return;
    }
    if ( cb_validate_one (bgc) ) {
        return;
    }
    if ( cb_validate_one (scroll) ) {
        return;
    }
    if ( cb_validate_one (timeout) ) {
        return;
    }
    if (timeout) {
        cb_emit (cb_build_funcall_1 ("cob_accept_set_timeout",timeout));
    }
    if ( current_program->flag_screen ) {
        /* Bump ref count to force CRT STATUS field generation */
        if ( current_program->crt_status != NULL ) {
            cb_field (current_program->crt_status)->count++;
        }
        if ( (CB_REF_OR_FIELD_P (var)) &&
             CB_FIELD (cb_ref (var))->storage == CB_STORAGE_SCREEN ) {
            output_screen_from (CB_FIELD (cb_ref (var)), 0);
            gen_screen_ptr = 1;
            if ( line || column ) {
                if ( line && column == cb_low ) {
                    cb_emit (cb_build_funcall_4 ("cob_screen_accept_1",
                                                 var, line, NULL, cb_int(dispattrs)));
                } else {
                    cb_emit (cb_build_funcall_4 ("cob_screen_accept_1",
                                                 var, line, column, cb_int(dispattrs)));
                }
            } else {
                cb_emit (cb_build_funcall_4 ("cob_screen_accept_1",
                                             var, NULL, NULL, cb_int(dispattrs)));
            }
            gen_screen_ptr = 0;
            output_screen_to (CB_FIELD (cb_ref (var)), 0);
        } else {
            if ( line || column || fgc || bgc ) {
                if ( !line && !column ) {
                    cb_emit (cb_build_funcall_9 ("cob_field_accept_with_status",
                                                 var, NULL, NULL, fgc, bgc,
                                                 scroll, cb_int (dispattrs),cb_int (dispprompt), 
                                                 excp_field));
                } else if ( line && column == cb_low ) {
                    cb_emit (cb_build_funcall_9 ("cob_field_accept_with_status",
                                                 var, line, NULL, fgc, bgc,
                                                 scroll, cb_int (dispattrs),cb_int (dispprompt),
                                                 excp_field));
                } else {
                    cb_emit (cb_build_funcall_9 ("cob_field_accept_with_status",
                                                 var, line, column, fgc, bgc,
                                                 scroll, cb_int (dispattrs),cb_int (dispprompt),
                                                 excp_field));
                }
            } else {
                cb_emit (cb_build_funcall_9 ("cob_field_accept_with_status",
                                             var, NULL, NULL, fgc, bgc,
                                             scroll, cb_int (dispattrs),cb_int (dispprompt), excp_field));
            }
        }
    } else if ( line || column || fgc || bgc || scroll ) {
        /* Bump ref count to force CRT STATUS field generation */
        if ( current_program->crt_status != NULL ) {
            cb_field (current_program->crt_status)->count++;
        }
        if ( !line && !column ) {
            cb_emit (cb_build_funcall_9 ("cob_field_accept_with_status",
                                         var, NULL, NULL, fgc, bgc, scroll,
                                         cb_int (dispattrs),cb_int (dispprompt), excp_field));
        } else if ( line && column == cb_low ) {
            cb_emit (cb_build_funcall_9 ("cob_field_accept_with_status",
                                         var, line, NULL, fgc, bgc, scroll,
                                         cb_int (dispattrs),cb_int (dispprompt), excp_field));
        } else {
            cb_emit (cb_build_funcall_9 ("cob_field_accept_with_status",
                                         var, line, column, fgc, bgc, scroll,
                                         cb_int (dispattrs),cb_int (dispprompt), excp_field));
        }
    } else {
        cb_emit (cb_build_funcall_2 ("cob_accept", cb_int(COB_DEVICE_SYSIN), var));
    }
    cb_emit_wipe_cache();
}

void             
cb_emit_accept_escape (cb_tree var)
{
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_screen_accept_escape", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_line_or_col (cb_tree var, const int l_or_c) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_2 ("cob_screen_line_col", var, cb_int (l_or_c)));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_date (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_date", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_date_yyyymmdd (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_date_yyyymmdd", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_day (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_day", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_day_yyyyddd (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_day_yyyyddd", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_day_of_week (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_day_of_week", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_time (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_time", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_command_line (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_command_line", var));
    cb_emit_wipe_cache();
}

void
cb_emit_get_environment (cb_tree envvar, cb_tree envval) {
    if ( cb_validate_one (envvar) ) {
        return;
    }
    if ( cb_validate_one (envval) ) {
        return;
    }
    cb_emit (cb_build_funcall_2 ("cob_get_environment", envvar, envval));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_environment (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_environment", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_arg_number (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_arg_number", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_arg_value (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_accept_arg_value", var));
    cb_emit_wipe_cache();
}

void
cb_emit_accept_mnemonic (cb_tree var, cb_tree mnemonic) {
    int dev;
    if ( cb_validate_one (var) ) {
        return;
    }
    dev = CB_SYSTEM_NAME (cb_ref (mnemonic))->token;
    switch ( dev ) {
        case COB_DEVICE_CONSOLE:
        case COB_DEVICE_SYSIN:
            if ( cb_flag_console_equal_sysfile ) {
                dev = COB_DEVICE_SYSIN;
            }
            cb_emit (cb_build_funcall_2 ("cob_accept", cb_int(dev), var));
            cb_emit_wipe_cache();
            break;
        default:
            cb_error_x (mnemonic, _("Invalid input stream '%s'"),
                        cb_name (mnemonic));
            break;
    }
}

void
cb_emit_accept_name (cb_tree var, cb_tree name) {
    cb_tree sys;
    int dev;

    if ( cb_validate_one (var) ) {
        return;
    }
    if ( CB_REFERENCE (name)->word->count == 0 ) {
        sys = lookup_system_name (CB_NAME (name));

        if ( sys != cb_error_node ) {
            dev = CB_SYSTEM_NAME (sys)->token;
            switch ( dev ) {
                case COB_DEVICE_CONSOLE:
                case COB_DEVICE_SYSIN:
                    if ( cb_flag_console_equal_sysfile ) {
                        dev = COB_DEVICE_SYSIN;
                    }
                    cb_warning_information (name, _("'%s' undefined in SPECIAL-NAMES"), CB_NAME (name));
                    cb_emit (cb_build_funcall_2 ("cob_accept", cb_int(dev), var));
                    cb_emit_wipe_cache();
                    return;
                default:
                    break;
            }
        }
    }

    cb_error_x (name, _("'%s' undefined in SPECIAL-NAMES"), CB_NAME (name));
    cb_emit_wipe_cache();
}

/*
 * ALLOCATE statement
 */

void
cb_emit_allocate (cb_tree target1, cb_tree target2, cb_tree size, cb_tree initialize) {
    cb_tree x;
    char    buff[32];

    if ( cb_validate_one (target1) ) {
        return;
    }
    if ( cb_validate_one (target2) ) {
        return;
    }
    if ( cb_validate_one (size) ) {
        return;
    }
    if ( target1 ) {
        if ( !(CB_REFERENCE_P(target1) &&
               cb_field(target1)->flag_item_based) ) {
            cb_error_x (CB_TREE(current_statement),
                        _("Target of ALLOCATE is not a BASED item"));
        }
    }
    if ( target2 ) {
        if ( !(CB_REFERENCE_P(target2) &&
               CB_TREE_CLASS (target2) == CB_CLASS_POINTER) ) {
            cb_error_x (CB_TREE(current_statement),
                        _("Target of RETURNING is not a data pointer"));
        }
    }
    if ( size ) {
        if ( CB_TREE_CLASS (size) != CB_CLASS_NUMERIC ) {
            cb_error_x (CB_TREE(current_statement),
                        _("The CHARACTERS field of ALLOCATE must be numeric"));
        }
    }
    if ( target1 ) {
        sprintf (buff, "%d", cb_field (target1)->memory_size);
        x = cb_build_numeric_literal (0, (ucharptr)buff, 0, NULL);
        cb_emit (cb_build_funcall_3 ("cob_allocate",
                                     cb_build_cast_addr_of_addr(target1), target2, x));
    } else {
        cb_emit (cb_build_funcall_3 ("cob_allocate",
                                     NULL, target2, size));
    }
    if ( initialize && target1 ) {
        current_statement->handler2 =
        cb_build_initialize (target1, cb_true, NULL, cb_true, 0, 0, 1, 0);
        /*CIT
        current_statement->handler2 =
            cb_list_init(cb_build_initialize (target1, cb_true, NULL, cb_true, 0));
        current_statement->handler2 = 
            cb_list_add(current_statement->handler2, cb_build_fixvalue(target1, 1));
       */
    }
    cb_emit_wipe_cache();
}


/*
 * ALTER statement
 */
/*CIT*/
#define ALTER_IDX_PREFIX "ALTER IDX "
cb_tree 
find_alter_goto(struct cb_program *prog,const char *label) {
    cb_tree         l;
    cb_tree         g;
    char            buff[COB_MINI_BUFF];

    snprintf (buff, COB_MINI_MAX, "%s%s", ALTER_IDX_PREFIX, label);
    for ( l = prog->alter_goto_list; l; l = CB_CHAIN (l) ) {
        g = CB_VALUE (l);
        if ( g && (strcmp(CB_NAME(CB_GOTO(g)->depending), buff) == 0) ) {
            return  g;
        }
    }
    return NULL;
}

void
cb_emit_alter (cb_tree src, cb_tree target) {
    struct cb_goto  *g;
    char            buff[COB_MINI_BUFF];
    cb_tree         l;
    cb_tree         ref;
    int             found =0, idx=0 ;

    if ( !CB_REFERENCE_P(src) || !CB_REFERENCE_P(target) ) {
        cb_error_x (CB_TREE(current_statement),
                    _("ALTER apply only on labels"));        
        return;
    }

    snprintf (buff, COB_MINI_MAX, "%s%s", ALTER_IDX_PREFIX, CB_NAME(src));
    ref = cb_build_reference (buff);
    l = find_alter_goto(current_program, CB_NAME(src));
    if ( !l ) {
        cb_build_index (ref, cb_int0, 0, NULL, 1,CB_USAGE_INDEX);
        l = cb_build_goto(cb_list_init(target), ref, NULL);
        current_program->alter_goto_list = cb_list_add(current_program->alter_goto_list, l);
    }

    g = CB_GOTO(l);

    found =0;
    idx = 1;
    for ( l = g->target; l; l = CB_CHAIN (l) ) {
        if ( strcmp(CB_NAME(CB_VALUE(l)), CB_NAME(target)) == 0 ) {
            found = 1;
            break;
        }
        idx ++;
    }

    if ( !found ) {
        g->target = cb_list_add(g->target, target);
    }

    cb_emit(cb_build_move(cb_int(idx), g->depending));
}

/*
 * CALL statement
 */
/*CIT*/
int  
cb_merge_call_conv (cb_tree mf_call_conv, cb_tree call_convention) {
    int call_conv = 0, val = 0;
    int iconv =  0, ilink =  0;

    if ( call_convention ) {
        if ( !CB_PAIR_P(call_convention) || 
             !CB_INTEGER_P(CB_PAIR_X(call_convention)) || 
             !CB_INTEGER_P(CB_PAIR_Y(call_convention)) ) {
            cb_error (_("Invalid CALL ... WITH clause"));
        }
        ilink = CB_INTEGER(CB_PAIR_X(call_convention))->val;
        iconv = CB_INTEGER(CB_PAIR_Y(call_convention))->val;
#ifndef _WIN32
        if ( iconv != CB_CALL_CDECL ) {
            cb_error (_("CALL ... WITH STDCALL LINKAGE Calling convention only supported on Windows "));
            return 0;
        }
#endif
    }
    if ( mf_call_conv ) {
        if ( CB_LITERAL_P(mf_call_conv) ) {
            val = cb_get_int(mf_call_conv);
        } else if ( CB_INTEGER_P(mf_call_conv) ) {
            val = CB_INTEGER(mf_call_conv)->val;
        } else if ( (cb_ref (mf_call_conv) != cb_error_node) &&  
                    CB_SYSTEM_NAME_P(cb_ref (mf_call_conv)) &&
                    (CB_SYSTEM_NAME(cb_ref (mf_call_conv))->category == CB_CALL_CONVENTION_NAME) ) {
            val = CB_SYSTEM_NAME(cb_ref (mf_call_conv))->token;
        } else {
            cb_error (_("CALL-CONVENTION invalid"));
        }
        if ( (val & 16) || (val & 32) ) {
            cb_error (_("CALL-CONVENTION OS/2 Unsupported"));
        }
        if ( val & 8 ) {
            ilink |= CB_CALL_STATIC;
        }
        if ( val & 4 ) {
            iconv |= CB_CALL_RETURN_VOID;
        }
        if ( val & 64 || val & 2) {
            iconv |= CB_CALL_STDCALL;
        }
        
        if ( val & 1 ) {
            cb_error (_("CALL-CONVENTION 'Process parameters from left-to-right' Unsupported"));
        } 
        
    }
    call_conv = iconv | ilink;
    return call_conv;
}


static void
cb_emit_call_1 (cb_tree prog, int call_conv, cb_tree using_list, cb_tree returning,
                cb_tree on_exception, cb_tree not_on_exception, int is_chain) {
    cb_tree                     l;
    cb_tree                     x;
    cb_tree                     r;
    cb_tree                     a = NULL;
    struct cb_field            *f;
    const struct system_table   *psyst;
    int                         is_sys_call = 0;
    int                         returningopt=0;

    cob_enterprise_map_call_to_entry (prog, &call_conv, using_list, returning);
    if ( CB_INTRINSIC_P (prog) ) {
        if ( CB_INTRINSIC(prog)->intr_tab->category != CB_CATEGORY_ALPHANUMERIC ) {
            cb_error (_("Only alphanumeric FUNCTION types are allowed here"));
            return;
        }
    }
    if ( returning ) {
        if ( CB_PAIR_P(returning) ) {
            returningopt = CB_INTEGER(CB_PAIR_X(returning))->val;
            returning    = CB_PAIR_Y(returning);
        }
        if ( (call_conv & CB_CALL_RETURN_MASK) == CB_CALL_RETURN_VOID ) {
            call_conv &= ~CB_CALL_RETURN_MASK;
            call_conv |= returningopt;
        }
        switch ( returningopt ) {
            case CB_CALL_RETURN_VOID:
                cb_error (_("RETURNING not allowed with no return CALL Convension"));
                return;
                break;
            case CB_CALL_RETURN_INT:
            case CB_CALL_RETURN_PTR:
                if ( CB_TREE_CLASS(returning) != CB_CLASS_NUMERIC &&
                     CB_TREE_CLASS(returning) != CB_CLASS_POINTER && 
                     CB_TREE_CLASS(returning) != CB_CLASS_ALPHANUMERIC ) {
                    cb_error (_("Invalid RETURNING field"));
                    return;
                }
                if ( CB_TREE_CLASS(returning) == CB_CLASS_POINTER ) {
                    returningopt = CB_CALL_RETURN_PTR;
                }
                break;
            case CB_CALL_RETURN_ADDROF:
                if ( (CB_REFERENCE_P (returning) && CB_FIELD_P(CB_REFERENCE(returning)->value))
                     || CB_FIELD_P (returning) ) {
                    f= cb_field(returning);
                    if ( !f->flag_item_based && f->storage != CB_STORAGE_LINKAGE) {
                        cb_error (_("RETURNING ADDRESS OF \"field\" Must refer a BASED/LINKAGE field"));
                        return;
                    }
                } else {
                    cb_error (_("Invalid RETURNING field"));
                    return;
                }
                break;
            case CB_CALL_RETURN_INTO:
                if ( CB_TREE_CLASS(returning) == CB_CLASS_NUMERIC ||
                     CB_TREE_CLASS(returning) == CB_CLASS_POINTER ) {
                    cb_error (_("Invalid RETURNING INTO field"));
                    return;                    
                }
                break;
            case CB_CALL_RETURN_FIELD_ADDR:
                cb_error (_("Invalid RETURNING FIELDADDR "));
                break;
        }
    }
    for ( l = using_list; l; l = CB_CHAIN (l) ) {
        x = CB_VALUE (l);
        if ( x == cb_error_node ) {
            continue;
        }
        if ( CB_CONST_P (x) && x != cb_null && 
             CB_CALL_BY(CB_PURPOSE_INT (l)) != CB_CALL_BY_VALUE ) {
            cb_error_x (x, _("Figurative constant invalid here"));
        }
        if ( (CB_REFERENCE_P (x) && CB_FIELD_P(CB_REFERENCE(x)->value))
             || CB_FIELD_P (x) ) {
            f= cb_field(x);
            if ( f->level == 88 ) {
                cb_error_x (x, _("'%s' Not a data name"), CB_NAME (x));
                return;
            }
            if ( cb_warn_call_params &&
                 (CB_CALL_BY(CB_PURPOSE_INT (l)) == CB_CALL_BY_REFERENCE || 
                  CB_CALL_BY(CB_PURPOSE_INT (l)) == CB_CALL_BY_DEFAULT) ) {
                if ( f->level != 01 &&
                     f->level != 77 ) {
                    cb_warning_x (x, _("'%s' is not 01 or 77 level item"), CB_NAME (x));
                }
            }
            if ( CB_CALL_BY(CB_PURPOSE_INT (l)) != CB_CALL_BY_VALUE && 
                 f->flag_usage_optimized && 
                 f->usage != f->original_usage ) {
                r = cb_get_optimized_field_shadow(f);
                CB_VALUE (l) = r;
                current_statement->before_body = cb_list_add(current_statement->before_body, cb_build_move(x, r));
                a = cb_list_add(a, cb_build_move(r, x));
            }
        }
    }

    if ( CB_LITERAL_P(prog) ) {
        for ( psyst = (const struct system_table *)&system_tab[0]; psyst->syst_name; psyst++ ) {
            if ( !strcasecmp((const char *)CB_LITERAL(prog)->data,
                             (const char *)psyst->syst_name) ) {
                if ( psyst->syst_params > cb_list_length (using_list) ) {
                    cb_error (_("Wrong number of CALL parameters for '%s'"),
                              (char *)psyst->syst_name);
                    return;
                }
                is_sys_call = 1;
                break;
            }
        }
    }
    if ( !CB_LITERAL_P(prog) && (call_conv & CB_CALL_STATIC) ) {
        cb_error (_("CALL ... WITH STATIC LINKAGE only valid with literal proram name"));
        return;
    }
    if ( CB_LITERAL_P(prog) && !is_sys_call) {
        if (cb_flag_call_uppercase) {
            cob_strupper((char*)(CB_LITERAL(prog)->data));
        }else if (cb_flag_call_lowercase) {
            cob_strlower((char*)(CB_LITERAL(prog)->data));
        }
    }
    
    x = cb_build_call (prog, using_list, on_exception, not_on_exception,
                       returning, is_sys_call, call_conv | returningopt, a);
    CB_CALL(x)->is_chain = is_chain;
    cb_emit (x);
}

void
cb_emit_call (cb_tree prog, int call_conv, cb_tree using_list, cb_tree returning,
              cb_tree on_exception, cb_tree not_on_exception) {
    cb_emit_call_1(prog,call_conv,using_list,returning,on_exception,not_on_exception,0);
    cb_emit_wipe_cache();
}

void
cb_emit_chain (cb_tree prog, int call_conv, cb_tree using_list, cb_tree returning,
               cb_tree on_exception, cb_tree not_on_exception) {
    cb_emit_call_1(prog,call_conv,using_list,returning,on_exception,not_on_exception,1);
    cb_emit_wipe_cache();
}
/*
 * CANCEL statement
 */

void
cb_emit_cancel (cb_tree prog) {
    if ( cb_validate_one (prog) ) {
        return;
    }
    /*CIT*/
    if ( cb_flag_call_optimize ) {
        cb_error_x (CB_TREE (current_statement),
                    _("-fcall-opt not allowed when CANCEL is used"));
    }
    cb_emit (cb_build_funcall_1 ("cob_field_cancel", prog));
    cb_emit_wipe_cache();
}

/*
 * CLOSE statement
 */

void
cb_emit_close (cb_tree file, cb_tree opt) {
    if ( file == cb_error_node ) {
        return;
    }
    file = cb_ref (file);
    if ( file == cb_error_node ) {
        return;
    }
    current_statement->file = file;
    if ( CB_FILE (file)->organization == COB_ORG_SORT ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Operation not allowed on SORT files"));
    }
    cb_emit (cb_build_funcall_3 ("cob_close_extfh", file, opt,
                                 CB_FILE(file)->file_status));
    cb_emit_wipe_cache();
}

/*
 * COMMIT statement
 */

void
cb_emit_commit (void) {
    cb_emit (cb_build_funcall_0 ("cob_commit_extfh"));
    cb_emit_wipe_cache();
}

/*
 * CONTINUE statement
 */

void
cb_emit_continue (void) {
    cb_emit (cb_build_continue ());
}

/*
 * DELETE statement
 */

void
cb_emit_delete (cb_tree file) {
    if ( file == cb_error_node ) {
        return;
    }
    file = cb_ref (file);
    if ( file == cb_error_node ) {
        return;
    }
    current_statement->file = file;
    if ( CB_FILE (file)->organization == COB_ORG_SORT ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Operation not allowed on SORT files"));
    }
    cb_emit (cb_build_funcall_2 ("cob_delete_extfh", file, CB_FILE(file)->file_status));
    cb_emit_wipe_cache();
}

void
cb_emit_delete_file (cb_tree file) {
    cb_tree x;
    if ( file == cb_error_node ) {
        return;
    }
    x = cb_ref (file);
    if ( x == cb_error_node || !CB_FILE_P (x) ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Operation not allowed on %s"), CB_NAME(file));
        return;
    }
    current_statement->file = x;
    if ( CB_FILE (x)->organization == COB_ORG_SORT ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Operation not allowed on SORT files"));
    }
    cb_emit (cb_build_funcall_2 ("cob_delete_file_extfh", x, CB_FILE(x)->file_status));
    cb_emit_wipe_cache();
}

/*
 * DISPLAY statement
 */

void
cb_emit_env_name (cb_tree value) {
    if ( cb_validate_one (value) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_display_environment", value));
}

void
cb_emit_env_value (cb_tree value) {
    if ( cb_validate_one (value) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_display_env_value", value));
}

void
cb_emit_arg_number (cb_tree value) {
    if ( cb_validate_one (value) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_display_arg_number", value));
}

void
cb_emit_command_line (cb_tree value) {
    if ( cb_validate_one (value) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_display_command_line", value));
}

void
cb_emit_display (cb_tree values, cb_tree upon, cb_tree no_adv, cb_tree line, cb_tree column,
                 cb_tree fgc, cb_tree bgc, cb_tree scroll, int dispattrs) {
    cb_tree l;
    cb_tree x;
    cb_tree p;
    cb_tree temp;

    if ( cb_validate_list (values) ) {
        return;
    }
    if ( cb_validate_one (line) ) {
        return;
    }
    if ( cb_validate_one (column) ) {
        return;
    }
    if ( cb_validate_one (fgc) ) {
        return;
    }
    if ( cb_validate_one (bgc) ) {
        return;
    }
    if ( cb_validate_one (scroll) ) {
        return;
    }
    for ( l = values; l; l = CB_CHAIN (l) ) {
        x = CB_VALUE (l);
        if ( x == cb_error_node ) {
            return;
        }

        switch ( CB_TREE_TAG (x) ) {
            case CB_TAG_LITERAL:
            case CB_TAG_INTRINSIC:
            case CB_TAG_CONST:
            case CB_TAG_STRING:
            case CB_TAG_INTEGER:
                break;
            case CB_TAG_REFERENCE:
                if ( !CB_FIELD_P(CB_REFERENCE(x)->value) ) {
                    cb_error_x (x, _("'%s' is an invalid type for DISPLAY operand"), cb_name (x));
                    return;
                }
                break;
            case CB_TAG_CAST:
                switch ( CB_CAST(x)->type ) {
                    case CB_CAST_ADDRESS:
                    case CB_CAST_ADDR_OF_ADDR:
                    case CB_CAST_PROGRAM_POINTER:
                        temp = cb_build_index (cb_build_filler (), NULL, 0, NULL, 0, CB_USAGE_POINTER);
                        CB_FIELD (cb_ref (temp))->count++;
                        cb_emit (cb_build_assign (temp, x));
                        CB_VALUE (l) = temp;
                        break;
                    default:
                        cb_error_x (x, _("Invalid type for DISPLAY operand"));
                        return;
                }
                break;
            default:
                cb_error_x (x, _("Invalid type for DISPLAY operand"));
                return;
        }
    }
    if ( upon == cb_error_node ) {
        return;
    }
    if (!values) {
        values = cb_list_init(NULL);
    }
    x = CB_VALUE (values);

    if (x &&  (CB_REF_OR_FIELD_P (x)) &&
        CB_FIELD (cb_ref (x))->storage == CB_STORAGE_SCREEN ) {
        output_screen_from (CB_FIELD (cb_ref (x)), 0);
        gen_screen_ptr = 1;
        if ( line || column ) {
            if ( line && column == cb_low ) {
                p = cb_build_funcall_3 ("cob_screen_display", x,
                                        line, NULL);
            } else {
                if ( line == NULL ) {
                    line = cb_one;
                }
                if ( column == NULL ) {
                    column = cb_one;
                }
                p = cb_build_funcall_3 ("cob_screen_display", x,
                                        line, column);

            }

        } else {
            p = cb_build_funcall_3 ("cob_screen_display", x,
                                    NULL, NULL);
        }
        cb_emit (p);
        gen_screen_ptr = 0;
    } else if ( line || column || fgc || bgc || scroll || dispattrs ) {
        if ( line && column == cb_low ) {
            p = cb_build_funcall_7 ("cob_field_display",
                                    CB_VALUE (values), line, NULL, fgc, bgc,
                                    scroll, cb_int (dispattrs));
        } else if ( line || column ) {
            if ( line == NULL ) {
                line = cb_null;
            }
            if ( column == NULL ) {
                column = cb_null;
            }
            p = cb_build_funcall_7 ("cob_field_display",
                                    CB_VALUE (values), line, column, fgc, bgc,
                                    scroll, cb_int (dispattrs));
        } else {
            p = cb_build_funcall_7 ("cob_field_display",
                                    CB_VALUE (values), NULL, NULL, fgc, bgc,
                                    scroll, cb_int (dispattrs));
        }
        cb_emit (p);
        if (values) {
            for ( l = CB_CHAIN (values); l; l = CB_CHAIN (l) ) {
                x = CB_VALUE (l);
                if ( x ) {
                    cb_emit (cb_build_funcall_7 ("cob_field_display",
                                                 x, NULL, NULL, fgc, bgc,
                                                 scroll, cb_int (dispattrs)));
                }
            }
        }
    } else {
        /* DISPLAY x ... [UPON device-name] */
        p = cb_build_funcall_3 ("cob_display", upon, no_adv, values);
        CB_FUNCALL(p)->varcnt = cb_list_length (values);
        cb_emit (p);
        for ( l = values; l; l = CB_CHAIN (l) ) {
            x = CB_VALUE (l);
            if ( CB_FIELD_P (x) ) {
                CB_FIELD (cb_ref (x))->count++;
            }
        }
    }
}

cb_tree
cb_build_display_upon (cb_tree x) {
    int dev; 

    if ( x == cb_error_node ) {
        return cb_error_node;
    }

    dev = CB_SYSTEM_NAME (cb_ref (x))->token;
    switch ( dev ) {
        case COB_DEVICE_CONSOLE:
        case COB_DEVICE_SYSOUT:
        case COB_DEVICE_SYSERR:
            if ( cb_flag_console_equal_sysfile && (dev == COB_DEVICE_CONSOLE) ) {
                dev = COB_DEVICE_SYSOUT;
            }
            return cb_int(dev);
        default:
            cb_error_x (x, _("Invalid output stream"));
            return cb_error_node;
    }
}

cb_tree
cb_build_display_upon_direct (cb_tree x) {
    const char  *name;
    cb_tree     sys;
    int         dev;

    if ( x == cb_error_node ) {
        return cb_error_node;
    }
    name = CB_NAME (x);
    if ( CB_REFERENCE (x)->word->count == 0 ) {
        sys = lookup_system_name (CB_NAME (x));
        if ( sys != cb_error_node ) {
            dev = CB_SYSTEM_NAME (sys)->token;
            switch ( dev ) {
                case COB_DEVICE_CONSOLE:
                case COB_DEVICE_SYSOUT:
                case COB_DEVICE_SYSERR:
                    if ( cb_flag_console_equal_sysfile && (dev == COB_DEVICE_CONSOLE) ) {
                        dev = COB_DEVICE_SYSOUT;
                    }
                    cb_warning_information (x, _("'%s' undefined in SPECIAL-NAMES"), name);
                    return cb_int(dev);
                default:
                    break;
            }
        }
    }

    cb_error_x (x, _("'%s' undefined in SPECIAL-NAMES"), name);
    return cb_error_node;
}

cb_tree
cb_build_exhibit_one(cb_tree x, int options)
{
    cb_tree l = NULL;
    if ( options & 1 ) {
        /*NAMED*/
        if ( CB_REF_OR_FIELD_P(x) ) {
            char *s = cb_name(x);
            l = cb_list_add(l, cb_build_alphanumeric_literal((unsigned char*)s, strlen(s), 0));
            l = cb_list_add(l, cb_build_alphanumeric_literal((unsigned char*)" = ", 3, 0));
        }
    }
    l = cb_list_add(l, x);
    return l;
}

void 
cb_emit_exhibit (cb_tree values, cb_tree upon, cb_tree no_adv, int options)
{
    cb_tree l;
    cb_tree x;
    cb_tree p;
    cb_tree temp;

    if ( cb_validate_list (values) ) {
        return;
    }
    for ( l = values; l; l = CB_CHAIN (l) ) {
        x = CB_VALUE (l);
        if ( x == cb_error_node ) {
            return;
        }

        switch ( CB_TREE_TAG (x) ) {
            case CB_TAG_LITERAL:
            case CB_TAG_INTRINSIC:
            case CB_TAG_CONST:
            case CB_TAG_STRING:
            case CB_TAG_INTEGER:
                break;
            case CB_TAG_REFERENCE:
                if ( !CB_FIELD_P(CB_REFERENCE(x)->value) ) {
                    cb_error_x (x, _("'%s' is an invalid type for EXHIBIT operand"), cb_name (x));
                    return;
                }
                break;
            case CB_TAG_CAST:
                switch ( CB_CAST(x)->type ) {
                    case CB_CAST_ADDRESS:
                    case CB_CAST_ADDR_OF_ADDR:
                    case CB_CAST_PROGRAM_POINTER:
                        temp = cb_build_index (cb_build_filler (), NULL, 0, NULL, 0, CB_USAGE_POINTER);
                        CB_FIELD (cb_ref (temp))->count++;
                        cb_emit (cb_build_assign (temp, x));
                        CB_VALUE (l) = temp;
                        break;
                    default:
                        cb_error_x (x, _("Invalid type for EXHIBIT operand"));
                        return;
                }
                break;
            default:
                cb_error_x (x, _("Invalid type for EXHIBIT operand"));
                return;
        }
    }
    if ( upon == cb_error_node ) {
        return;
    }
    if ( options & 2 ) {
        /* CHANGED*/
        cb_warning (_("EXHIBIT CHANGED not implemented"));
    }
    p = cb_build_funcall_3 ("cob_display", upon, no_adv, values);
    CB_FUNCALL(p)->varcnt = cb_list_length (values);
    cb_emit (p);
    for ( l = values; l; l = CB_CHAIN (l) ) {
        x = CB_VALUE (l);
        if ( CB_FIELD_P (x) ) {
            CB_FIELD (cb_ref (x))->count++;
        }
    }

}

/*
 * DIVIDE statement
 */
/*
int static
cb_equal_field (cb_tree x, cb_tree y)
{
    if ( CB_REF_OR_FIELD_P(x) && CB_REF_OR_FIELD_P(y) && 
         (cb_field(x)->id == cb_field(y)->id)) {
        return 1;
    }
    return 0;
}
*/
void
cb_emit_divide (cb_tree dividend, cb_tree divisor, cb_tree quotient, cb_tree remainder) {
    if ( cb_validate_one (dividend) ) {
        return;
    }
    if ( cb_validate_one (divisor) ) {
        return;
    }
    CB_VALUE (quotient) = cb_check_numeric_edited_name (CB_VALUE (quotient));
    CB_VALUE (remainder) = cb_check_numeric_edited_name (CB_VALUE (remainder));

    if ( cb_validate_one (CB_VALUE (quotient)) ) {
        return;
    }
    if ( cb_validate_one (CB_VALUE (remainder)) ) {
        return;
    }

    /*CIT*/
    /*
    if ( decimal_may_binary_optize() 
         
         && is_all_field_not_scaled(quotient)
         && is_all_field_not_scaled(dividend)
         && is_all_field_not_scaled(divisor)
         && !cb_equal_field(quotient, dividend)) {
        cb_emit_arithmetic (quotient, 0, cb_build_binary_op (dividend, '/', divisor), 1);
        cb_emit_arithmetic (remainder, 0, cb_build_binary_op (dividend, '%', divisor), allow_binary_div_optimize);
    } else { 
    */
    cb_emit (add_to_target_ref_list (cb_build_funcall_4 ("cob_div_quotient", dividend, divisor,
                                 CB_VALUE (quotient),
                                 build_store_option (CB_VALUE (quotient), CB_PURPOSE (quotient))), CB_VALUE (quotient)));
    cb_emit (add_to_target_ref_list (cb_build_funcall_2 ("cob_div_remainder", CB_VALUE (remainder),
                                 build_store_option (CB_VALUE (remainder), cb_int0)),CB_VALUE (remainder))) ;
    /*
    } 
    */ 
}

/*
 * EVALUATE statement
 */

static cb_tree
evaluate_test (cb_tree s, cb_tree o) {
    int flag;
    cb_tree x, y;
    cb_tree t;

    /* ANY is always true */
    if ( o == cb_any ) {
        return cb_true;
    }

    /* object TRUE or FALSE */
    if ( o == cb_true ) {
        return s;
    }
    if ( o == cb_false ) {
        return cb_build_negation (s);
    }

    if (o == cb_positive) {
        return cb_build_binary_op (s, '>', cb_build_numeric_literal(0, (ucharptr)"0", 0, NULL));
    } else if (o == cb_not_positive) {
        return cb_build_binary_op (s, '[', cb_build_numeric_literal(0, (ucharptr)"0", 0, NULL));
    } else if (o == cb_negative) {
        return cb_build_binary_op (s, '<', cb_build_numeric_literal(0, (ucharptr)"0", 0, NULL));
    } else if (o == cb_not_negative) {
        return cb_build_binary_op (s, ']', cb_build_numeric_literal(0, (ucharptr)"0", 0, NULL));
    } else if (o == cb_numeric) {
        return cb_build_funcall_1("cob_is_numeric", s);
    } else if (o == cb_not_numeric) {
        return cb_build_negation(cb_build_funcall_1("cob_is_numeric", s));
    } else if (o == cb_alphabetic) {
        return cb_build_funcall_1("cob_is_alpha", s);
    } else if (o == cb_not_alphabetic) {
        return cb_build_negation(cb_build_funcall_1("cob_is_alpha", s));
    } else if (o == cb_alphabetic_lower) {
        return cb_build_funcall_1("cob_is_lower", s);
    } else if (o == cb_not_alphabetic_lower) {
        return cb_build_negation(cb_build_funcall_1("cob_is_lower", s));
    } else if (o == cb_alphabetic_upper) {
        return cb_build_funcall_1("cob_is_upper", s);
    } else if (o == cb_not_alphabetic_upper) {
        return cb_build_negation(cb_build_funcall_1("cob_is_upper", s));
    }

    flag = CB_PURPOSE_INT(o);
    x = CB_PAIR_X (CB_VALUE (o));
    y = CB_PAIR_Y (CB_VALUE (o));


    /* subject TRUE or FALSE */
    if (s == cb_true || s == cb_false) {
        switch (flag) {
            case '&':
            case '|':
                x = evaluate_test ( cb_true, x);
                if (y) {
                    x = evaluate_test ( cb_true, y);
                    x = cb_build_binary_op (x, flag,
                                            evaluate_test ( cb_true, y));
                }
                break;
        }

        if ( s == cb_true ) {
            return flag ? cb_build_negation (x) : x;
        }
        if ( s == cb_false ) {
            return flag ? x : cb_build_negation (x);
        }
    }

    /* x THRU y */
    if ( y ) {
        switch (flag) {
            case '&':
            case '|':
                return cb_build_binary_op (evaluate_test ( s, x),
                                           flag,
                                           evaluate_test ( s, y));
                break;
            default:
                t = cb_build_binary_op (cb_build_binary_op (x, '[', s),
                                        '&',
                                        cb_build_binary_op (s, '[', y));

                return flag ? cb_build_negation (t) : t;
        }
    }

    if ( CB_REFERENCE_P(x) && CB_FIELD_P(CB_REFERENCE(x)->value) &&
         CB_FIELD(CB_REFERENCE(x)->value)->level == 88 ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Invalid use of 88 level in WHEN expression"));
        return NULL;
    }
    /* regular comparison */
    /*CIT
    if ( flag ) {
        return cb_build_binary_op (s, '~', x);
    } else if ( flag ) {
        return cb_build_binary_op (s, '=', x);
    }*/
    if ( flag == 1 ) {
        return cb_build_binary_op (s, '~', x);
    } else if ( flag == 0 ) {
        return cb_build_binary_op (s, '=', x);
    } else {
        return cb_build_binary_op (s, flag, x);
    }
}

static cb_tree
build_evaluate (cb_tree subject_list, cb_tree case_list) {
    cb_tree c1 = NULL;
    cb_tree c2;
    cb_tree c3;
    cb_tree subjs;
    cb_tree whens;
    cb_tree objs;
    cb_tree stmt;

    if ( case_list == NULL ) {
        return NULL;
    }

    whens = CB_VALUE (case_list);
    stmt = CB_VALUE (whens);
    whens = CB_CHAIN (whens);

    /* for each WHEN sequence */
    for ( ; whens; whens = CB_CHAIN (whens) ) {
        c2 = NULL;
        /* single WHEN test */
        for ( subjs = subject_list, objs = CB_VALUE (whens);
            subjs && objs; subjs = CB_CHAIN (subjs), objs = CB_CHAIN (objs) ) {
            c3 = evaluate_test (CB_VALUE (subjs), CB_VALUE (objs));
            if ( c3 == NULL ) {
                return NULL;
            }

            if ( c2 == NULL ) {
                c2 = c3;
            } else {
                c2 = cb_build_binary_op (c2, '&', c3);
            }
        }
        if ( subjs || objs ) {
            cb_error (_("Wrong number of WHEN parameters"));
        }
        /* connect multiple WHEN's */
        if ( c1 == NULL ) {
            c1 = c2;
        } else {
            c1 = cb_build_binary_op (c1, '|', c2);
        }
    }

    if ( c1 == NULL ) {
        return stmt;
    } else {

        return cb_build_if (cb_build_cond (c1), stmt,
                            build_evaluate (subject_list, CB_CHAIN (case_list)));
    }
}

int 
cb_is_other_caselist(cb_tree case_list)
{
    cb_tree whens;
    cb_tree objs;

    if ( case_list == NULL ) {
        return 0;
    }
    for ( whens = case_list; whens; whens = CB_CHAIN (whens) ) {
        for ( objs = CB_VALUE (whens); objs; objs = CB_CHAIN (objs) ) {
            if ( CB_VALUE (objs) == NULL ) {
                return 1;
            }
        }
    }
    return 0;
}

void
cb_emit_evaluate (cb_tree subject_list, cb_tree case_list) {
    cb_emit (build_evaluate (subject_list, case_list));
}

/*
 * FREE statement
 */

void
cb_emit_free (cb_tree vars) {
    cb_tree     l;
    struct cb_field *f;
    int     i;

    if ( cb_validate_list (vars) ) {
        return;
    }
    for ( l = vars, i = 1; l; l = CB_CHAIN (l), i++ ) {
        if ( CB_TREE_CLASS (CB_VALUE (l)) == CB_CLASS_POINTER ) {
            if ( CB_CAST_P (CB_VALUE (l)) ) {
                f = cb_field (CB_CAST (CB_VALUE(l))->val);
                if ( !f->flag_item_based ) {
                    cb_error_x (CB_TREE (current_statement),
                                _("Target %d of FREE, a data address identifier, must address a BASED data item"), i);
                }
                cb_emit (cb_build_funcall_2 ("cob_free_alloc",
                                             cb_build_cast_address (CB_VALUE (l)), NULL));
            } else {
                cb_emit (cb_build_funcall_2 ("cob_free_alloc",
                                             NULL, cb_build_cast_address (CB_VALUE (l))));
            }
        } else if ( CB_REF_OR_FIELD_P (CB_VALUE (l)) ) {
            f = cb_field (CB_VALUE (l));
            if ( !f->flag_item_based ) {
                cb_error_x (CB_TREE (current_statement),
                            _("Target %d of FREE, a data address identifier, must address a BASED data item"), i);
            }
            cb_emit (cb_build_funcall_2 ("cob_free_alloc",
                                         cb_build_cast_addr_of_addr (CB_VALUE (l)), NULL));
        } else {
            cb_error_x (CB_TREE (current_statement),
                        _("Target %d of FREE must be a data pointer"), i);
        }
    }
}

/*
 * GO TO statement
 */

void
cb_emit_goto (cb_tree target, cb_tree depending) {
    if ( target == cb_error_node ) {
        return;
    }
    if ( depending ) {
        /* GO TO procedure-name ... DEPENDING ON identifier */
        cb_emit (cb_build_goto (target, depending, NULL));
    } else {
        /* GO TO procedure-name */
        if ( target == NULL ) {
            cb_verify (cb_goto_statement_without_name, "GO TO without procedure-name");
        } else if ( CB_CHAIN (target) ) {
            cb_error (_("GO TO with multiple procedure-names"));
        } else {
            cb_emit (cb_build_goto (CB_VALUE (target), NULL, NULL));
        }
    }
    cb_emit_wipe_cache();
}

void
cb_emit_exit (size_t goback, cb_tree return_field) {
    if ( return_field ) {
        cb_emit_move (return_field, cb_list_init(current_program->cb_return_code));
        if ( CB_TREE_CATEGORY (return_field) == CB_CATEGORY_NUMERIC && 
             cb_fits_int(return_field) ) {
            return_field= NULL;
        }
    }

    if ( goback ) {
        cb_emit (cb_build_goto (cb_int1, NULL, return_field));
    } else {
        cb_emit (cb_build_goto (NULL, NULL, return_field));
    }
}

/*
 * IF statement
 */

void
cb_emit_if (cb_tree cond, cb_tree stmt1, cb_tree stmt2) {
    cb_emit (cb_build_if (cond, stmt1, stmt2));
}

/*
 * INITIALIZE statement
 */

cb_tree
cb_build_initialize_vars (cb_tree vars, cb_tree fillinit, cb_tree value, cb_tree replacing, cb_tree def, int filldefault) {
    cb_tree l;
    int fill_init = 0;
    cb_tree sl= NULL;

    if ( cb_validate_list (vars) ) {
        return sl;
    }
    if ( value == NULL && replacing == NULL ) {
        def = cb_true;
    }
    if ( fillinit == cb_true ) {
        fill_init = 1;
    }
    for ( l = vars; l; l = CB_CHAIN (l) ) {
        sl = cb_list_add (sl, cb_build_initialize (CB_VALUE (l), value, replacing, def, 1, filldefault, fill_init, 1));
        /*CIT 
        cb_emit (cb_build_fixvalue(CB_VALUE (l), 1));*/
    }
    return sl;
}

void
cb_emit_initialize_vars (cb_tree vars, cb_tree fillinit, cb_tree value, cb_tree replacing, cb_tree def) {
    cb_emit(cb_build_initialize_vars(vars,fillinit,value,replacing,def, 0));
}
/*
 * FIXValue  
 */

void
cb_emit_fixvalue (cb_tree var, int flag) {
    if ( !flag ) {
        return;
    }
    cb_emit (cb_build_fixvalue (var, flag));
}

/*
 * INSPECT statement
 */

void
cb_emit_inspect (cb_tree var, cb_tree body, cb_tree replacing, int replconv) {
    switch ( CB_TREE_TAG(var) ) {
        case CB_TAG_REFERENCE:
            break;
        case CB_TAG_INTRINSIC:
            switch ( CB_TREE_CATEGORY(var) ) {
                case CB_CATEGORY_ALPHABETIC:
                case CB_CATEGORY_ALPHANUMERIC:
                case CB_CATEGORY_NATIONAL:
                    break;
                default:
                    cb_error (_("Invalid target for INSPECT"));
                    return;
            }
            break;
        case CB_TAG_LITERAL:
            break;
        default:
            cb_error (_("Invalid target for REPLACING/CONVERTING"));
            return;
    }
    if ( replconv && sending_id ) {
        cb_error (_("Invalid target for REPLACING/CONVERTING"));
    }
    cb_emit (cb_build_funcall_2 ("cob_inspect_init", var, replacing));
    cb_emit_list (body);
    cb_emit (cb_build_funcall_0 ("cob_inspect_finish"));
}

void
cb_init_tarrying (void) {
    inspect_func = NULL;
    inspect_data = NULL;
}

cb_tree
cb_build_tarrying_data (cb_tree x) {
    inspect_data = x;
    return NULL;
}

cb_tree
cb_build_tarrying_characters (cb_tree l) {
    if ( inspect_data == NULL ) {
        cb_error (_("Data name expected before CHARACTERS"));
    }
    inspect_func = NULL;
    return cb_list_add (l, cb_build_funcall_1 ("cob_inspect_characters", inspect_data));
}

cb_tree
cb_build_tarrying_all (void) {
    if ( inspect_data == NULL ) {
        cb_error (_("Data name expected before ALL"));
    }
    inspect_func = "cob_inspect_all";
    return NULL;
}

cb_tree
cb_build_tarrying_leading (void) {
    if ( inspect_data == NULL ) {
        cb_error (_("Data name expected before LEADING"));
    }
    inspect_func = "cob_inspect_leading";
    return NULL;
}

cb_tree
cb_build_tarrying_trailing (void) {
    if ( inspect_data == NULL ) {
        cb_error (_("Data name expected before TRAILING"));
    }
    inspect_func = "cob_inspect_trailing";
    return NULL;
}

cb_tree
cb_build_tarrying_value (cb_tree x, cb_tree l) {
    if ( inspect_func == NULL ) {
        cb_error_x (x, _("ALL, LEADING or TRAILING expected before '%s'"), cb_name (x));
    }
    return cb_list_add (l, cb_build_funcall_2 (inspect_func, inspect_data, x));
}

cb_tree
cb_build_replacing_characters (cb_tree x, cb_tree l) {
    return cb_list_add (l, cb_build_funcall_1 ("cob_inspect_characters", x));
}

cb_tree
cb_build_replacing_all (cb_tree x, cb_tree y, cb_tree l) {
    return cb_list_add (l, cb_build_funcall_2 ("cob_inspect_all", y, x));
}

cb_tree
cb_build_replacing_leading (cb_tree x, cb_tree y, cb_tree l) {
    return cb_list_add (l, cb_build_funcall_2 ("cob_inspect_leading", y, x));
}

cb_tree
cb_build_replacing_first (cb_tree x, cb_tree y, cb_tree l) {
    return cb_list_add (l, cb_build_funcall_2 ("cob_inspect_first", y, x));
}

cb_tree
cb_build_replacing_trailing (cb_tree x, cb_tree y, cb_tree l) {
    return cb_list_add (l, cb_build_funcall_2 ("cob_inspect_trailing", y, x));
}

cb_tree
cb_build_converting (cb_tree x, cb_tree y, cb_tree l) {
    return cb_list_add (l, cb_build_funcall_2 ("cob_inspect_converting", x, y));
}

cb_tree
cb_build_inspect_region_start (void) {
    return cb_list_init (cb_build_funcall_0 ("cob_inspect_start"));
}

cb_tree
cb_build_inspect_region (cb_tree l, cb_tree pos, cb_tree x) {
    if ( pos == CB_BEFORE ) {
        return cb_list_add (l, cb_build_funcall_1 ("cob_inspect_before", x));
    } else {
        return cb_list_add (l, cb_build_funcall_1 ("cob_inspect_after", x));
    }
}

/*
 * MOVE statement
 */

static void
warning_destination (cb_tree x) {
    struct cb_reference *r;
    struct cb_field     *f;
    cb_tree         loc;

    r = CB_REFERENCE (x);
    f = CB_FIELD (r->value);
    loc = CB_TREE (f);

    if ( r->offset ) {
        return;
    }

    if ( !strcmp (f->name, "RETURN-CODE") ||
         !strcmp (f->name, "SORT-RETURN") ||
         !strcmp (f->name, "NUMBER-OF-CALL-PARAMETERS") ) {
        cb_warning (_("-> Internal register '%s' defined as BINARY-LONG"), f->name);
    } else if ( f->pic ) {
        cb_warning_x (loc, _("-> '%s' defined here as PIC %s"), f->name, f->pic->orig);
    } else {
        cb_warning_x (loc, _("-> '%s' defined here as a group of length %d"), f->name, f->size);
    }
}

static int
move_error (cb_tree src, cb_tree dst, const size_t value_flag, const int flag,
            const int src_flag, const char *msg) {
    cb_tree loc;

    if ( suppress_warn ) {
        return 0;
    }
    loc = src->source_line ? src : dst;
    if ( value_flag ) {
        /* VALUE clause */
        cb_warning_x (loc, msg);
    } else {
        /* MOVE statement */
        if ( flag ) {
            cb_warning_x (loc, msg);
            if ( src_flag ) {
                warning_destination (src);
            }
            warning_destination (dst);
        }
    }

    return 0;
}

/* count the number of free places in an alphanumeric edited field */
static int
count_pic_alphanumeric_edited (struct cb_field *field) {
    int     count;
    int     repeat;
    unsigned char   *p;

    count = 0;
    for ( p = (unsigned char *)(field->pic->str); *p; p += 5 ) {
        if ( *p == '9' || *p == 'A' || *p == 'X' ) {
            memcpy ((unsigned char *)&repeat, p + 1, sizeof(int));
            count += repeat;
        }
    }
    return count;
}

int
cb_validate_move (cb_tree src, cb_tree dst, size_t is_value) {
    struct cb_field     *f;
    struct cb_literal   *l;
    unsigned char       *p;
    cb_tree         loc;
    long long       val;
    size_t          i;
    size_t          is_numeric_edited = 0;
    int         src_scale_mod;
    int         dst_scale_mod;
    int         dst_size_mod;
    int         size;
    int         most_significant;
    int         least_significant;

    loc = src->source_line ? src : dst;
    if ( CB_REFERENCE_P(dst) ) {
        if ( CB_ALPHABET_NAME_P(CB_REFERENCE(dst)->value) ) {
            goto invalid;
        }
        if ( CB_FILE_P(CB_REFERENCE(dst)->value) ) {
            goto invalid;
        }
        if ( CB_OBJECT_LIST_P(CB_REFERENCE(dst)->value) ) {
            if ( CB_TREE_CLASS (src) == CB_CLASS_ALPHANUMERIC ||
                 CB_TREE_CLASS (src) == CB_CLASS_ALPHABETIC   ||
                 CB_TREE_CLASS (src) == CB_CLASS_NUMERIC      || 
                 CB_TREE_CLASS (src) == CB_CLASS_NATIONAL ) {
                return 0;
            } else {
                goto invalid;
            }
        }
    }
    if ( CB_REFERENCE_P(src) && CB_OBJECT_LIST_P(CB_REFERENCE(src)->value) ) {
        if ( CB_TREE_CLASS (dst) == CB_CLASS_ALPHANUMERIC ||
             CB_TREE_CLASS (dst) == CB_CLASS_ALPHABETIC   ||
             CB_TREE_CLASS (dst) == CB_CLASS_NUMERIC      || 
             CB_TREE_CLASS (dst) == CB_CLASS_NATIONAL ) {
            return 0;
        } else {
            goto invalid;
        }
    }
    if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_BOOLEAN ) {
        cb_error_x (loc, _("Invalid destination for MOVE"));
        return -1;
    }

    if ( CB_TREE_CLASS (dst) == CB_CLASS_POINTER ) {
        if ( CB_TREE_CLASS (src) == CB_CLASS_POINTER ) {
            return 0;
        } else {
            goto invalid;
        }
    }

    if ( CB_TREE_TAG (dst) == CB_TAG_LITERAL ) {
        cb_error_x (loc, _("Invalid destination for MOVE"));
        return -1;

    }

    f = cb_field (dst);
    switch ( CB_TREE_TAG (src) ) {
        case CB_TAG_CAST:
            goto invalid;
            break;
        case CB_TAG_CONST:
            if ( src == cb_space ) {
                if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC
                     || (CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC_EDITED && !is_value) ) {
                    if ( f->usage == CB_USAGE_DISPLAY ) {
                        if ( (cb_move_spaces_to_displaynumeric ==  CB_MOVEspTO9_ERROR) ) {
                            goto invalid;
                        }
                    } else if ( f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6 ) {
                        if ( (cb_move_spaces_to_comp3 ==  CB_MOVEspTO9_ERROR) ) {
                            goto invalid;
                        }
                    } else {
                        if ( cb_move_xto9_mode != CB_MOVExTO9_MF50 ) {
                            goto invalid;
                        }
                    }

                }
            } else if ( src == cb_zero ) {
                if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_ALPHABETIC ) {
                    goto invalid;
                }
            } else if ((src == cb_low || src == cb_high)) {
                if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC
                     || (CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC_EDITED) ) {
                    if (f->usage == CB_USAGE_DISPLAY) {
                        if (cb_move_high_low_to_displaynumeric == CB_MOVEhlTO9_ERROR) {
                            goto invalid;
                        }
                    }
                }
            }

            break;
        case CB_TAG_LITERAL:
            /* TODO: ALL literal */
            if ( f->usage != CB_USAGE_BIT ) {
                l = CB_LITERAL (src);
                if ( CB_TREE_CLASS (src) == CB_CLASS_NUMERIC ) {
                    /* Numeric literal */
                    if ( l->all ) {
                        goto invalid;
                    }
                    most_significant = -999;
                    least_significant = 999;

                    /* compute the most significant figure place */
                    for ( i = 0; i < l->size; i++ ) {
                        if ( l->data[i] != '0' ) {
                            break;
                        }
                    }
                    if ( i != l->size ) {
                        most_significant = (int) (l->size - l->scale - i - 1);
                    }

                    /* compute the least significant figure place */
                    for ( i = 0; i < l->size; i++ ) {
                        if ( l->data[l->size - i - 1] != '0' ) {
                            break;
                        }
                    }
                    if ( i != l->size ) {
                        least_significant = (int) (-l->scale + i);
                    }

                    /* value check */
                    switch ( CB_TREE_CATEGORY (dst) ) {
                        case CB_CATEGORY_ALPHANUMERIC:
                        case CB_CATEGORY_ALPHANUMERIC_EDITED:
                            if ( is_value ) {
                                goto expect_alphanumeric;
                            }

                            if ( l->scale == 0 ) {
                                goto expect_alphanumeric;
                            } else {
                                if ( cb_move_noninteger_to_alphanumeric == CB_ERROR ) {
                                    goto invalid;
                                }
                            }
                        case CB_CATEGORY_NUMERIC:
                            if ( f->pic->scale < 0 ) {
                                /* check for PIC 9(n)P(m) */
                                if ( least_significant < -f->pic->scale ) {
                                    goto value_mismatch;
                                }
                            } else if ( f->pic->scale > f->pic->size ) {
                                /* check for PIC P(n)9(m) */
                                if ( most_significant >= f->pic->size - f->pic->scale ) {
                                    goto value_mismatch;
                                }
                            }
                            break;
                        case CB_CATEGORY_NUMERIC_EDITED:
                            if ( is_value ) {
                                goto expect_alphanumeric;
                            }

                            /* TODO */
                            break;
                        default:
                            if ( is_value ) {
                                goto expect_alphanumeric;
                            }
                            goto invalid;
                    }

                    /* sign check */
                    if ( l->sign != 0 && !f->pic->have_sign ) {
                        if ( is_value ) {
                            cb_error_x (loc, _("Data item not signed"));
                            return -1;
                        }
                        if ( cb_warn_constant ) {
                            cb_warning_x (loc, _("Ignoring negative sign"));
                        }
                    }

                    /* size check */
                    if ( f->flag_real_binary || 
                         ((f->usage == CB_USAGE_COMP_5 ||
                           f->usage == CB_USAGE_COMP_X ||
                           f->usage == CB_USAGE_BINARY) &&
                          f->pic->scale == 0) ) {
                        p = l->data;
                        for ( i = 0; i < l->size; i++ ) {
                            if ( l->data[i] != '0' ) {
                                p = &l->data[i];
                                break;
                            }
                        }
                        i = l->size - i;
                        switch ( f->size ) {
                            case 1:
                                if ( i > 18 ) {
                                    goto numlit_overflow;
                                }
                                val = cb_get_long_long (src);
                                if ( f->pic->have_sign ) {
                                    if ( val < -128LL ||
                                         val > 127LL ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( val > 255LL ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                            case 2:
                                if ( i > 18 ) {
                                    goto numlit_overflow;
                                }
                                val = cb_get_long_long (src);
                                if ( f->pic->have_sign ) {
                                    if ( val < -32768LL ||
                                         val > 32767LL ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( val > 65535LL ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                            case 3:
                                if ( i > 18 ) {
                                    goto numlit_overflow;
                                }
                                val = cb_get_long_long (src);
                                if ( f->pic->have_sign ) {
                                    if ( val < -8388608LL ||
                                         val > 8388607LL ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( val > 16777215LL ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                            case 4:
                                if ( i > 18 ) {
                                    goto numlit_overflow;
                                }
                                val = cb_get_long_long (src);
                                if ( f->pic->have_sign ) {
                                    if ( val < -2147483648LL ||
                                         val > 2147483647LL ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( val > 4294967295LL ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                            case 5:
                                if ( i > 18 ) {
                                    goto numlit_overflow;
                                }
                                val = cb_get_long_long (src);
                                if ( f->pic->have_sign ) {
                                    if ( val < -549755813888LL ||
                                         val > 549755813887LL ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( val > 1099511627775LL ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                            case 6:
                                if ( i > 18 ) {
                                    goto numlit_overflow;
                                }
                                val = cb_get_long_long (src);
                                if ( f->pic->have_sign ) {
                                    if ( val < -140737488355328LL ||
                                         val > 140737488355327LL ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( val > 281474976710655LL ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                            case 7:
                                if ( i > 18 ) {
                                    goto numlit_overflow;
                                }
                                val = cb_get_long_long (src);
                                if ( f->pic->have_sign ) {
                                    if ( val < -36028797018963968LL ||
                                         val > 36028797018963967LL ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( val > 72057594037927935LL ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                            default:
                                if ( f->pic->have_sign ) {
                                    if ( i < 19 ) {
                                        break;
                                    }
                                    if ( i > 19 ) {
                                        goto numlit_overflow;
                                    }
                                    if ( memcmp (p, "9223372036854775807", 19) > 0 ) {
                                        goto numlit_overflow;
                                    }
                                } else {
                                    if ( i < 20 ) {
                                        break;
                                    }
                                    if ( i > 20 ) {
                                        goto numlit_overflow;
                                    }
                                    if ( memcmp (p, "18446744073709551615", 20) > 0 ) {
                                        goto numlit_overflow;
                                    }
                                }
                                break;
                        }
                        return 0;
                    }
                    if ( least_significant < -f->pic->scale ) {
                        goto size_overflow;
                    }
                    if ( f->pic->scale > 0 ) {
                        size = f->pic->digits - f->pic->scale;
                    } else {
                        size = f->pic->digits;
                    }
                    if ( most_significant >= size ) {
                        goto size_overflow;
                    }
                } else {
                    /* Alphanumeric literal */

                    /* value check */
                    switch ( CB_TREE_CATEGORY (dst) ) {
                        case CB_CATEGORY_ALPHABETIC:
                            for ( i = 0; i < l->size; i++ ) {
                                if ( !isalpha (l->data[i]) && !isspace (l->data[i]) ) {
                                    goto value_mismatch;
                                }
                            }
                            break;
                        case CB_CATEGORY_NUMERIC:
                            goto expect_numeric;
                        case CB_CATEGORY_NUMERIC_EDITED:
                            if ( !is_value ) {
                                goto expect_numeric;
                            }

                            /* TODO: validate the value */
                            break;
                        default:
                            break;
                    }

                    /* size check */
                    size = cb_field_size (dst);
                    if ( size >= 0 && (int)l->size > size ) {
                        goto size_overflow;
                    }
                }
            }
            break;
        case CB_TAG_FIELD:
        case CB_TAG_REFERENCE:
            if ( CB_REFERENCE_P(src) &&
                 CB_ALPHABET_NAME_P(CB_REFERENCE(src)->value) ) {
                break;
            }
            if ( CB_REFERENCE_P(src) &&
                 CB_FILE_P(CB_REFERENCE(src)->value) ) {
                goto invalid;
            }
            size = cb_field_size (src);
            if ( size < 0 ) {
                size = cb_field(src)->size;
            }
            /* non-elementary move */
            if ( cb_field (src)->children || cb_field (dst)->children ) {
                if ( size > cb_field(dst)->size ) {
                    goto size_overflow_1;
                }
                break;
            }

            /* elementary move */
            switch ( CB_TREE_CATEGORY (src) ) {
                case CB_CATEGORY_NATIONAL:
                    switch ( CB_TREE_CATEGORY (dst) ) {
                        case CB_CATEGORY_NATIONAL:
                            if ( size > cb_field(dst)->size ) {
                                goto size_overflow_1;
                            }
                            break;
                        case CB_CATEGORY_NUMERIC:
                        case CB_CATEGORY_NUMERIC_EDITED:
                            if ( size/2 > cb_field(dst)->pic->digits ) {
                                goto size_overflow_2;
                            }
                            break;
                        case CB_CATEGORY_ALPHANUMERIC_EDITED:
                            if ( size/2 >
                                 count_pic_alphanumeric_edited (cb_field(dst)) ) {
                                goto size_overflow_1;
                            }
                            break;
                        default:
                            if ( size/2 > cb_field(dst)->size ) {
                                goto size_overflow_1;
                            }
                            break;
                    }
                    break;

                case CB_CATEGORY_ALPHANUMERIC:
                    switch ( CB_TREE_CATEGORY (dst) ) {
                        case CB_CATEGORY_NATIONAL:
                            if ( size > cb_field(dst)->size/2 ) {
                                goto size_overflow_1;
                            }
                            break;
                        case CB_CATEGORY_NUMERIC:
                        case CB_CATEGORY_NUMERIC_EDITED:
                            if (!cb_verify(cb_compile_move_xto9, "MOVE PIC X TO PIC 9")) {
                                cb_error (_("MOVE PIC X TO PIC 9 is not accepted by configuration"));
                                return -1;
                            }
                            if ( size > cb_field(dst)->pic->digits ) {
                                goto size_overflow_2;
                            }
                            break;
                        case CB_CATEGORY_ALPHANUMERIC_EDITED:
                            if ( size >
                                 count_pic_alphanumeric_edited (cb_field(dst)) ) {
                                goto size_overflow_1;
                            }
                            break;
                        default:
                            if ( size > cb_field(dst)->size ) {
                                goto size_overflow_1;
                            }
                            break;
                    }
                    break;
                case CB_CATEGORY_ALPHABETIC:
                case CB_CATEGORY_ALPHANUMERIC_EDITED:
                    switch ( CB_TREE_CATEGORY (dst) ) {
                        case CB_CATEGORY_NUMERIC:
                        case CB_CATEGORY_NUMERIC_EDITED:
                            goto invalid;
                        case CB_CATEGORY_ALPHANUMERIC_EDITED:
                            if ( size >
                                 count_pic_alphanumeric_edited(cb_field(dst)) ) {
                                goto size_overflow_1;
                            }
                            break;
                        default:
                            if ( size > cb_field(dst)->size ) {
                                goto size_overflow_1;
                            }
                            break;
                    }
                    break;
                case CB_CATEGORY_NUMERIC:
                case CB_CATEGORY_NUMERIC_EDITED:
                    switch ( CB_TREE_CATEGORY (dst) ) {
                        case CB_CATEGORY_ALPHABETIC:
                            goto invalid;
                        case CB_CATEGORY_ALPHANUMERIC_EDITED:
                            is_numeric_edited = 1;
                            /* Drop through */
                        case CB_CATEGORY_ALPHANUMERIC:
                            if ( is_numeric_edited ) {
                                dst_size_mod = count_pic_alphanumeric_edited (cb_field(dst));
                            } else {
                                dst_size_mod = cb_field(dst)->size;
                            }
                            if ( CB_TREE_CATEGORY (src) == CB_CATEGORY_NUMERIC
                                 && cb_field (src)->pic->scale > 0 ) {
                                if ( cb_move_noninteger_to_alphanumeric == CB_ERROR ) {
                                    goto invalid;
                                }
                                cb_warning_information (loc, _("Move non-integer to alphanumeric"));
                                break;
                            }
                            if ( CB_TREE_CATEGORY (src) == CB_CATEGORY_NUMERIC
                                 && cb_field(src)->pic->digits > dst_size_mod ) {
                                goto size_overflow_2;
                            }
                            if ( CB_TREE_CATEGORY (src) == CB_CATEGORY_NUMERIC_EDITED
                                 && cb_field(src)->size > dst_size_mod ) {
                                goto size_overflow_1;
                            }
                            break;
                        default:
                            src_scale_mod = cb_field(src)->pic->scale < 0 ?
                                            0 : cb_field(src)->pic->scale;
                            dst_scale_mod = cb_field(dst)->pic->scale < 0 ?
                                            0 : cb_field(dst)->pic->scale;
                            if ( cb_field(src)->pic->digits - src_scale_mod > 
                                 cb_field(dst)->pic->digits - dst_scale_mod  ||
                                 src_scale_mod > dst_scale_mod ) {
                                goto size_overflow_2;
                            }
                            break;
                    }
                    break;
                default:
                    cb_error_x (loc, _("Invalid source for MOVE"));
                    return -1;
            }
            break;
        case CB_TAG_INTEGER:
        case CB_TAG_BINARY_OP:
        case CB_TAG_INTRINSIC:
            /* TODO: check this */
            break;
        default:
            fprintf (stderr, "cobc:0: Invalid tree tag %d\n", CB_TREE_TAG (src));
            ABORT ();
    }
    return 0;

    invalid:
    if ( is_value ) {
        cb_error_x (loc, _("Invalid VALUE clause"));
    } else {
        cb_error_x (loc, _("Invalid MOVE statement"));
    }
    return -1;

    numlit_overflow:
    if ( is_value ) {
        cb_error_x (loc, _("Invalid VALUE clause - literal exceeds data size"));
        return -1;
    }
    if ( cb_warn_constant ) {
        cb_warning_x (loc, _("Numeric literal exceeds data size"));
    }
    return 0;

    expect_numeric:
    return move_error (src, dst, is_value, cb_warn_strict_typing, 0,
                       _("Numeric value is expected"));

    expect_alphanumeric:
    return move_error (src, dst, is_value, cb_warn_strict_typing, 0,
                       _("Alphanumeric value is expected"));

    value_mismatch:
    return move_error (src, dst, is_value, cb_warn_constant, 0,
                       _("Value does not fit the picture string"));

    size_overflow:
    return move_error (src, dst, is_value, cb_warn_constant, 0,
                       _("Value size exceeds data size"));

    size_overflow_1:
    if (CB_REFERENCE_P(src) && CB_REFERENCE(src)->length && 
        !cb_check_feature_x (src, cb_syntax_ibm5_2, "Sending field lenght (o,l) larger than receiving field")) {
        return 0;
    }
    return move_error(src, dst, is_value, cb_warn_truncate, 1,
                       _("Sending field larger than receiving field"));

    size_overflow_2:
    return move_error (src, dst, is_value, cb_warn_truncate, 1,
                       _("Some digits may be truncated"));
}

static cb_tree
cb_build_memset (cb_tree x, int c) {
    cb_tree res;
    cb_tree l;
    /*
    int size = cb_field_size (x);

    if ( size == 1 ) {
        res = cb_build_funcall_2 ("$E", x, cb_int (c));
    } else {
        res = cb_build_funcall_3 ("-memset", 
                                  cb_build_cast_address (x),
                                  cb_int (c), cb_build_cast_length (x));
    }
    */
    res = cb_build_funcall_2 ("$M" , x, cb_int (c & 0xFF));
    if ( CB_REFERENCE_P(x) &&  CB_REFERENCE(x)->check ) {
        l = cb_list_init (CB_REFERENCE(x)->check);
        return cb_list_add(l,res);
    } else {
        return res;
    }
}

cb_tree
cb_build_move_memcopy (cb_tree src, cb_tree dst) {
    cb_tree res;
    cb_tree l = NULL;
    int size = cb_field_size (dst);

    if ( CB_REFERENCE_P(src) &&  CB_REFERENCE(src)->check ) {
        l = cb_list_init (CB_REFERENCE(src)->check);
    }
    if ( CB_REFERENCE_P(dst) &&  CB_REFERENCE(dst)->check ) {
        l = cb_list_add (l, CB_REFERENCE(dst)->check);
    }

    if ( size == 1 ) {
        res = cb_build_funcall_2 ("$F", dst, src);
    } else {
#if 0 /*HAVE_MEMMOVE*/
        res = cb_build_funcall_3 ("-memmove",
                                  cb_build_cast_address (dst),
                                  cb_build_cast_address (src), cb_build_cast_length (dst));
#else
        res = cb_build_funcall_3 ("-memcpy",
                                  cb_build_cast_address (dst),
                                  cb_build_cast_address (src), cb_build_cast_length (dst));
#endif
    }
    if ( l ) {
        return cb_list_add(l,res);
    } else {
        return res;
    }
}

static cb_tree
cb_build_move_image (cob_field* src, cb_tree dst) {
    int id;
    struct attr_list * l;
    static cob_module module;

    memset (&module, 0, sizeof(module));
    module.ebcdic_charset = cb_flag_ebcdic_charset;
    cob_module_enter(rtd, &module);
    module.display_sign             = cb_display_sign_ebcdic;
    module.flag_filename_mapping    = cb_filename_mapping;
    module.flag_binary_truncate     = cb_binary_truncate;
    module.flag_pretty_display      = (cb_pretty_display ? COB_NUMERIC_PRETTY_DISPLAY : 0) |
                                      (cb_flag_raw_pic9_display ? COB_NUMERIC_RAW_DISPLAY : 0);
    module.decimal_point            = A2E(current_program->decimal_point);
    module.currency_symbol          = A2E(current_program->currency_symbol);
    module.numeric_separator        = A2E(current_program->numeric_separator);
    if ( cb_codepage ) {
        module.default_cp           = cb_codepage;
    }
    module.runtime_flags            = get_runtime_flags();
    module.module_version           = 2;

    id = cb_build_attr(dst, 0);
    l = cb_get_attr(id);
    if ( l ) {
        cob_field       temp;
        cob_field_attr  attr;
        cb_tree x;
        COB_ATTR_INIT (l->type, l->digits, l->scale,
                       l->flags, l->pic);
        temp.size = cb_field_size(dst);
        temp.data = cobc_malloc(temp.size);
        temp.attr = &attr;
        cob_move (rtd, src, &temp);                
        x = CB_TREE (build_literal (CB_CATEGORY_ALPHANUMERIC, temp.data, temp.size));
        CB_LITERAL(x)->encoded = 1;
        return cb_build_funcall_3 ("-memcpy",
                                   cb_build_cast_address (dst),
                                   cb_build_cast_address (x),
                                   cb_build_cast_length (dst));
    } else {
        fprintf (stderr, "cobc:0: Internal error ' %d\n", CB_TREE_TAG (src));
        ABORT ();
    }
    return cb_error_node;
}

/*
static int 
cb_is_usage_display (cb_tree x)
{
    struct cb_field *f;
    f = cb_field (x);
    if (f) {
        return f->usage == CB_USAGE_DISPLAY;
    }
    return 0;
}
*/ 
static int cb_tree_type_fixed (cb_tree x)
{
   int src_type = cb_tree_type(x);
   if (CB_TREE_TAG(x) == CB_TAG_REFERENCE && CB_REFERENCE(x)->offset)
   {
      switch (src_type) {
        case COB_TYPE_NATIONAL :
        case COB_TYPE_NATIONAL_EDITED:
            src_type=COB_TYPE_NATIONAL;
            break;
        default:
            src_type = COB_TYPE_ALPHANUMERIC;
            break;
        }
   }
   return src_type;
}

static cb_tree
cb_build_move_call (cb_tree src, cb_tree dst) {
    cob_field       source_field = {0};
    cob_field_attr  source_attr = {0};
    unsigned char   source_data = 0;
    int             source_set =0;
    int             dst_type = cb_tree_type_fixed(dst);
    const char            *fct;
    struct cb_field *src_f;
    struct cb_field *dst_f;
    int     src_size =  0;
    int     dst_size =  0;

    src_f = cb_field (src);
    if (src_f) {
        src_size = cb_field_size (src);
    }
    dst_f = cb_field (dst);
    if (dst_f) {
        dst_size = cb_field_size (dst);
    }


    if ( cb_flag_optimize_move && 
         (dst_type != COB_TYPE_BITS) && 
         (dst_type != COB_TYPE_BITS_ARRAY) && 
         (dst_type != COB_TYPE_NUMERIC_BITS) ) {
        if ( src == cb_zero ) {
            source_attr.type = COB_TYPE_ALPHANUMERIC_ALL;
            source_field.attr = &source_attr;
            source_field.data = &source_data;
            source_field.size = 1;
            source_data = CHAR_0;  
            source_set=1;
        }
        if ( source_set && !cb_flag_syntax_only) {
            struct cb_field *f = cb_field(dst);
            if ( f && !cb_field_variable_size (f) && 
                 cb_field_size(dst) < 512 ) {
                return cb_build_move_image(&source_field, dst);
            }
        }
    }
    fct = "cob_move";
    if ( cb_flag_optimize_move_call ) {
       int src_type = cb_tree_type_fixed(src);
       if (src_size != 0 && dst_size != 0 && src_f != dst_f)
       {
            switch (src_type) {
                case COB_TYPE_GROUP:        fct ="cob_move_alphanum_to_alphanum"; break;
                case COB_TYPE_FIELD_LIST:   fct ="cob_move_field_list"; break;
                case COB_TYPE_NUMERIC_DISPLAY:
                    switch ( dst_type ) {
                        case COB_TYPE_GROUP:            fct ="cob_move_alphanum_to_alphanum"; break;
                        case COB_TYPE_FIELD_LIST:       fct ="cob_move_field_list"; break;
                        case COB_TYPE_NUMERIC_FLOAT:    
                        case COB_TYPE_NUMERIC_DOUBLE:   
                            if (cb_flag_fp_optimize) fct ="cob_move_display_to_fp_opt"; else fct ="cob_move_display_to_fp";break;
                        case COB_TYPE_NUMERIC_DISPLAY:  fct ="cob_move_numeric_display_to_numeric_display"; break;
                        case COB_TYPE_NUMERIC_PACKED:   fct ="cob_move_display_to_packed"; break;
                        case COB_TYPE_NUMERIC_BINARY:
                        case COB_TYPE_NUMERIC_BITS:     fct ="cob_move_display_to_binary"; break;
                        case COB_TYPE_NUMERIC_EDITED:   fct ="cob_move_display_to_edited"; break;
                        case COB_TYPE_ALPHANUMERIC_BITS:  fct ="cob_enterprise_move_display_to_abit"; break;
                        case COB_TYPE_BITS:
                        case COB_TYPE_BITS_ARRAY:       fct ="cob_enterprise_move_display_to_bits"; break;
                        default: break;
                    }
                    break;
                case COB_TYPE_NUMERIC_PACKED:
                    switch ( dst_type ) {
                        case COB_TYPE_FIELD_LIST:       fct ="cob_move_field_list"; break;
                        case COB_TYPE_GROUP:            fct ="cob_move_alphanum_to_alphanum"; break;
                        case COB_TYPE_NUMERIC_DISPLAY:  fct ="cob_move_packed_to_display"; break;
                        case COB_TYPE_NUMERIC_PACKED:   fct ="cob_move_packed_to_packed"; break;
                        case COB_TYPE_NUMERIC_BINARY:
                        case COB_TYPE_NUMERIC_BITS:     fct ="cob_move_packed_to_binary"; break;
                        case COB_TYPE_NUMERIC_FLOAT:
                        case COB_TYPE_NUMERIC_DOUBLE:   fct ="cob_move_packed_to_fp"; break;
                        default: break;
                    }
                    break;
                case COB_TYPE_NUMERIC_BINARY:
                case COB_TYPE_NUMERIC_BITS:
                    switch ( dst_type ) {
                        case COB_TYPE_FIELD_LIST:       fct ="cob_move_field_list"; break;
                        case COB_TYPE_GROUP:            fct ="cob_move_alphanum_to_alphanum"; break;
                        case COB_TYPE_NUMERIC_DISPLAY:  fct ="cob_move_binary_to_display"; break;
                        case COB_TYPE_NUMERIC_BINARY:
                        case COB_TYPE_NUMERIC_BITS:     fct ="cob_move_binary_to_binary"; break;
                        case COB_TYPE_NUMERIC_PACKED:   fct ="cob_move_binary_to_packed"; break;
                        case COB_TYPE_NUMERIC_DOUBLE:
                        case COB_TYPE_NUMERIC_FLOAT:    fct ="cob_move_binary_to_fp"; break;
                        default: break;
                    }
                    break;
                case COB_TYPE_NUMERIC_EDITED:
                    switch ( dst_type ) {
                        case COB_TYPE_FIELD_LIST:       fct ="cob_move_field_list"; break;
                        case COB_TYPE_GROUP:            fct ="cob_move_alphanum_to_alphanum"; break;
                        case COB_TYPE_NUMERIC_DISPLAY:  fct ="cob_move_edited_to_display"; break;
                        case COB_TYPE_ALPHANUMERIC_EDITED:  fct="cob_move_alphanum_to_edited"; break;
                        default:                        break;
                    }
                    break;
                case COB_TYPE_NUMERIC_FLOAT:
                case COB_TYPE_NUMERIC_DOUBLE:
                    switch ( dst_type ) {
                        case COB_TYPE_FIELD_LIST:   fct="cob_move_field_list"; break;
                        case COB_TYPE_GROUP:        fct="cob_move_alphanum_to_alphanum"; break;
                        case COB_TYPE_NUMERIC_FLOAT:
                        case COB_TYPE_NUMERIC_DOUBLE:   fct="cob_move_fp_to_fp"; break;
                        case COB_TYPE_NUMERIC_DISPLAY:  
                            if (cb_flag_fp_optimize )    fct="cob_move_fp_to_display_opt" ; else  fct="cob_move_fp_to_display"  ; 
                            break;
                        case COB_TYPE_NUMERIC_BINARY:
                        case COB_TYPE_NUMERIC_BITS:     fct="cob_move_fp_to_binary"; break;
                        default:  break;
                    }
                    break;
                case COB_TYPE_ALPHANUMERIC_BITS:
                case COB_TYPE_BITS:
                case COB_TYPE_BITS_ARRAY:
                    switch ( dst_type ) {
                        case COB_TYPE_FIELD_LIST:   fct="cob_move_field_list"; break;
                        case COB_TYPE_GROUP:        fct="cob_move_alphanum_to_alphanum"; break;
                        default:  break;
                    }
                    break;
                case COB_TYPE_NATIONAL:
                case COB_TYPE_NATIONAL_EDITED:
                    switch ( dst_type ) {
                        case COB_TYPE_FIELD_LIST:   fct="cob_move_field_list"; break;
                        case COB_TYPE_GROUP:    fct="cob_move_alphanum_to_alphanum"; break;
                        case COB_TYPE_NATIONAL:
                        case COB_TYPE_NATIONAL_EDITED:  fct="cob_move_national_to_national"; break;
                        case COB_TYPE_ALPHANUMERIC: fct="cob_move_national_to_alphanum"; break;
                        default:  break;
                    }
                    break;
                default:
                    switch ( dst_type ) {
                        case COB_TYPE_FIELD_LIST:   fct="cob_move_field_list"; break;
                        case COB_TYPE_ALPHANUMERIC: 
                        case COB_TYPE_GROUP:    fct="cob_move_alphanum_to_alphanum"; break;
                        case COB_TYPE_ALPHANUMERIC_EDITED:  fct="cob_move_alphanum_to_edited"; break;
                        case COB_TYPE_NATIONAL:
                        case COB_TYPE_NATIONAL_EDITED:  fct="cob_move_alphanum_to_national"; break;
                        default: break;
                    }
            }
        }
    }
    return cb_build_funcall_2 (fct, src, dst);
}

static cb_tree
cb_build_move_num_zero (cb_tree x) {
    struct cb_field *f;

    f = cb_field (x);
    switch ( f->usage ) {
        case CB_USAGE_BINARY:
        case CB_USAGE_COMP_5:
        case CB_USAGE_COMP_X:
            if ( f->flag_binary_swap ) {
                return cb_build_memset (x, 0);
            }
            switch ( f->size ) {
#ifdef  COB_NON_ALIGNED
                case 1:
                    return cb_build_assign (x, cb_int0);
                case 2:
#ifdef      COB_SHORT_BORK
                    if ( f->storage != CB_STORAGE_LINKAGE && f->indexes == 0 &&
                         (is_base_alligned(x)) ) {
                        return cb_build_assign (x, cb_int0);
                    }
                    break;
#endif
                case 4:
                case 8:
                    if ( f->storage != CB_STORAGE_LINKAGE && f->indexes == 0 &&
                         (is_base_alligned(x)) ) {
                        return cb_build_assign (x, cb_int0);
                    }
                    break;
#else
                case 1:
                case 2:
                case 4:
                case 8:
                    return cb_build_assign (x, cb_int0);
#endif
            }
            return cb_build_memset (x, 0);
        case CB_USAGE_DISPLAY:
            if ( cb_flag_sign_ebcdic && (f->pic && f->pic->have_sign ) ) {
                return cb_build_move_call (cb_zero, x);
            } else if ( !(f->pic && f->pic->have_sign ) && !(f->flag_sign_separate) ) {
                if ( cb_flag_displaynumeric_mf50 || cb_move_xto9_mode == CB_MOVExTO9_MF50 ) {
                    return cb_build_move_call (cb_zero, x);
                } else {
                    return cb_build_memset (x, CHAR_0);
                }
            } else {
                return cb_build_move_call (cb_zero, x);
            }
        case CB_USAGE_PACKED:
        case CB_USAGE_COMP_6:
            return cb_build_funcall_1 ("-cob_set_packed_zero", x);
        default:
            return cb_build_move_call (cb_zero, x);
    }
}

static cb_tree
cb_build_move_space (cb_tree x) {
    switch ( CB_TREE_CATEGORY (x) ) {
        case CB_CATEGORY_ALPHANUMERIC:
        case CB_CATEGORY_ALPHABETIC:
            return cb_build_memset (x, CHAR_SP);
        case CB_CATEGORY_NATIONAL:
            return cb_build_funcall_2 ("cob_fill_national", x, cb_int(CHAR_SP));
        case CB_CATEGORY_NUMERIC:
            if (cb_flag_fast_figurative_move ) {
                return cb_build_memset (x, CHAR_SP);
            }
            /*No Break*/
        default:
            return cb_build_move_call (cb_space, x);
    }
}

static cb_tree
cb_build_move_zero (cb_tree x) {
    switch ( CB_TREE_CATEGORY (x) ) {
        case CB_CATEGORY_NUMERIC:
            if ( cb_field (x)->flag_blank_zero ) {
                return cb_build_move_space (x);
            } else {
                return cb_build_move_num_zero (x);
            }
        case CB_CATEGORY_ALPHABETIC:
        case CB_CATEGORY_ALPHANUMERIC:
            return cb_build_memset (x, CHAR_0);
        case CB_CATEGORY_NATIONAL:
            return cb_build_funcall_2 ("cob_fill_national", x, cb_int(CHAR_0));
        default:
            return cb_build_move_call (cb_zero, x);
    }
}

static cb_tree
cb_build_move_high (cb_tree x) {
    switch ( CB_TREE_CATEGORY (x) ) {
        case CB_CATEGORY_ALPHABETIC:
        case CB_CATEGORY_ALPHANUMERIC:
            if ( cb_high == cb_norm_high ) {
                return cb_build_memset (x, 255);
            } else {
                return cb_build_move_call (cb_high, x);
            }
        case CB_CATEGORY_NUMERIC:
            if (cb_flag_fast_figurative_move ) {
                return cb_build_memset (x, 255);
            }
            /*No Break*/
        default:
            return cb_build_move_call (cb_high, x);
    }
}

static cb_tree
cb_build_move_low (cb_tree x) {
    switch ( CB_TREE_CATEGORY (x) ) {
        case CB_CATEGORY_ALPHABETIC:
        case CB_CATEGORY_ALPHANUMERIC:
            if ( cb_low == cb_norm_low ) {
                return cb_build_memset (x, 0);
            } else {
                return cb_build_move_call (cb_low, x);
            }
        case CB_CATEGORY_NUMERIC:
            if ( cb_low == cb_norm_low  && cb_flag_fast_figurative_move) {
                return cb_build_memset (x, 0);
            }
            /*No Break*/
        default:
            return cb_build_move_call (cb_low, x);
    }
}

static cb_tree
cb_build_move_quote (cb_tree x) {
    switch ( CB_TREE_CATEGORY (x) ) {
        case CB_CATEGORY_ALPHABETIC:
        case CB_CATEGORY_ALPHANUMERIC:
            return cb_build_memset (x, A2E(cb_quote_char)); /* '"'); */
        case CB_CATEGORY_NUMERIC:
            if (cb_flag_fast_figurative_move) {
                return cb_build_memset (x, A2E(cb_quote_char));
            }
            /*No Break*/
        default:
            return cb_build_move_call (cb_quote, x);
    }
}

static int
is_numeric_only (cb_tree src) {
    struct cb_literal   *l = CB_LITERAL(src);
    int i = 0;
    unsigned char * s = l->data;

    if ( isdigit (s[i]) || (s[i] == '+') || (s[i] == '-')) {
        i++;
        for ( ; i < strlen((char*)s); ++i ) {
            if ( !isdigit (s[i]) ) {
                return(0);
            }
        }
        return 1;
    } else
        return 0;
}   

static cb_tree
cb_build_move_literal (cb_tree src, cb_tree dst) {
    struct cb_literal   *l;
    struct cb_field     *f;
    cb_tree              lst = NULL;
    unsigned char       *buff;
    unsigned char       *p;
    enum cb_category    cat;
    int         i;
    int         diff;
    int         val;
    int         n;
    unsigned char       bbyte;

    l = CB_LITERAL_ENCODED (src);
    f = cb_field (dst);
    cat = CB_TREE_CATEGORY (dst);
    if ( CB_REFERENCE_P(dst) &&  CB_REFERENCE(dst)->check ) {
        lst = cb_list_init (CB_REFERENCE(dst)->check);
    }

    if (cat == CB_CATEGORY_NUMERIC && !is_numeric_only(src)) {
        return cb_build_move_call (src, dst);
    }
    if ( l->all ) {
        if ( cat == CB_CATEGORY_NUMERIC || cat == CB_CATEGORY_NUMERIC_EDITED ) {
            return cb_build_move_call (src, dst);
        }
        if (cat == CB_CATEGORY_ALPHANUMERIC_EDITED && cb_move_all_edited)
        {
           return cb_build_funcall_2 ("cob_move_all_char_to_edited", src, dst);
        }
        if (l->size == 1)
        {
            return cb_list_add(lst, cb_build_funcall_3 ("-memset",
                                                        cb_build_cast_address (dst),
                                                        cb_int (l->data[0]), cb_build_cast_length (dst)));
        }
        bbyte = l->data[0];
        for ( i = 0; i < (int)l->size; i++ ) {
            if ( bbyte != l->data[i] ) {
                break;
            }
            bbyte = l->data[i];
        }
        if ( i == (int)l->size ) {
            return cb_list_add(lst, cb_build_funcall_3 ("-memset",
                                                        cb_build_cast_address (dst),
                                                        cb_int (l->data[0]), cb_build_cast_length (dst)));
        }
        if ( f->size > 128 ) {
            return cb_build_move_call (src, dst);
        }
        buff = cobc_malloc ((size_t)f->size);
        for ( i = 0; i < f->size; i++ ) {
            buff[i] = l->data[i % l->size];
        }
        return cb_list_add(lst, cb_build_funcall_3 ("-memcpy",
                                                    cb_build_cast_address (dst),
                                                    cb_build_string (buff, f->size), cb_build_cast_length (dst)));
    } else if ( (cat == CB_CATEGORY_NUMERIC
                 && f->usage == CB_USAGE_DISPLAY
                 && f->pic->scale == l->scale && !f->flag_sign_leading && !f->flag_sign_separate)
                || ((cat == CB_CATEGORY_ALPHABETIC || cat == CB_CATEGORY_ALPHANUMERIC)
                    && f->size < (int) (l->size + 16) && !cb_field_variable_size (f) &&
                    (f->storage != CB_STORAGE_LINKAGE || cb_flag_check_linkage_bound)) ) {
        buff = cobc_malloc ((size_t)f->size);
        diff = (int) (f->size - l->size);
        if ( cat == CB_CATEGORY_NUMERIC ) {
            if ( diff <= 0 ) {
                memcpy (buff, l->data - diff, (size_t)f->size);
            } else {
                /*
                if ( cb_flag_displaynumeric_mf50 || cb_move_xto9_mode == CB_MOVExTO9_MF50 ) {
                    memset (buff, CHAR_SP, (size_t)diff);
                } else {
                    memset (buff, CHAR_0, (size_t)diff);
                } 
                */ 
                memset (buff, CHAR_0, (size_t)diff);
                memcpy (buff + diff, l->data, (size_t)l->size);
            }
            if ( f->pic->have_sign ) {
                p = &buff[f->size - 1];
                if ( cb_display_sign_ebcdic ) {
                    cob_put_sign_ebcdic (rtd, p, l->sign);
                } else if ( l->sign < 0 ) {
                    if ( cb_flag_ebcdic_charset )
                        cob_put_sign_ascii (rtd, p, l->sign);
                    else if (cb_flag_carealia_sign)
                        PUT_SIGN_CAREALIA (*p);
                    else
                        PUT_SIGN_ASCII (*p);
                    /* else
                        *p += 0x40;
                    */
                }
            }
        } else {
            if ( f->flag_justified ) {
                if ( diff <= 0 ) {
                    memcpy (buff, l->data - diff, (size_t)f->size);
                } else {
                    memset (buff, CHAR_SP, (size_t)diff);
                    memcpy (buff + diff, l->data, (size_t)l->size);
                }
            } else {
                if ( diff <= 0 ) {
                    memcpy (buff, l->data, (size_t)f->size);
                } else {
                    memcpy (buff, l->data, (size_t)l->size);
                    memset (buff + l->size, CHAR_SP, (size_t)diff);
                }
            }
        }
        bbyte = *buff;
        if ( f->size == 1 ) {
            free (buff);
            return cb_list_add(lst, cb_build_funcall_2 ("$E", dst, cb_int (bbyte)));
        }
        for ( i = 0; i < f->size; i++ ) {
            if ( bbyte != buff[i] ) {
                break;
            }
        }
        if ( i == f->size ) {
            free (buff);
            return cb_list_add(lst, cb_build_funcall_3 ("-memset",
                                                        cb_build_cast_address (dst),
                                                        cb_int (bbyte), cb_build_cast_length (dst)));
        }
        return cb_list_add(lst, cb_build_funcall_3 ("-memcpy",
                                                    cb_build_cast_address (dst),
                                                    cb_build_string (buff, f->size),
                                                    cb_build_cast_length (dst)));
    } else if ( cb_fits_int (src) && f->size <= 8 &&
                (f->usage == CB_USAGE_BINARY || f->usage == CB_USAGE_COMP_5 ||
                 f->usage == CB_USAGE_COMP_X)) {
        val = cb_get_int (src);
        n = f->pic->scale - l->scale;
        if ( (l->size + n) > 9 ) {
            return cb_build_move_call (src, dst);
        }
        for ( ; n > 0; n-- ) {
            val *= 10;
        }
        for ( ; n < 0; n++ ) {
            val /= 10;
        }
        if ( val == 0 ) {
            return cb_build_move_num_zero (dst);
        }
        if (cb_binary_truncate && !f->flag_compx_notrunc) {
            n = f->pic->digits;
            if (n> 9) {
                return cb_build_move_call (src, dst);
            }
            val %= cb_exp10[n];
        }

        if ( f->size == 1 ) {
            return cb_build_assign (dst, cb_int (val));
        }
        if ( f->flag_binary_swap ) {
            i = (f->size - 1) + (8 * (f->pic->have_sign ? 1 : 0));
            return cb_list_add(lst, cb_build_funcall_2 (bin_set_funcs[i],
                                                        cb_build_cast_address (dst),
                                                        cb_int (val)));
        }
        switch ( f->size ) {
            case 2:
#ifdef  COB_SHORT_BORK
                if ( f->storage != CB_STORAGE_LINKAGE && f->indexes == 0 &&
                     (is_base_alligned(dst)) ) {
                    return cb_build_assign (dst, cb_int (val));
                }
                break;
#endif
            case 4:
            case 8:
#ifdef  COB_NON_ALIGNED
                if ( f->storage != CB_STORAGE_LINKAGE && f->indexes == 0 &&
                     (is_base_alligned(dst)) ) {
                    return cb_build_assign (dst, cb_int (val));
                }
                break;
#else
                return cb_build_assign (dst, cb_int (val));
#endif
        }
        return cb_build_move_call (src, dst);
    } else if ( cb_fits_int (src) && 
                (f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6) ) {
        if ( f->pic->scale < 0 ) {
            return cb_build_move_call (src, dst);
        }
        val = cb_get_int (src);
        n = f->pic->scale - l->scale;
        if ( (l->size + n) > 9 ) {
            return cb_build_move_call (src, dst);
        }
        for ( ; n > 0; n-- ) {
            val *= 10;
        }
        for ( ; n < 0; n++ ) {
            val /= 10;
        }
        if ( val == 0 ) {
            return cb_build_move_num_zero (dst);
        }
        return cb_build_funcall_2 ("-cob_set_packed_int", dst, cb_int (val));
    } else if ( cb_flag_fp_optimize &&  cb_is_double (src) &&
                (f->usage == CB_USAGE_DOUBLE || f->usage == CB_USAGE_FLOAT) ) {
        return cb_build_funcall_2 ("$C", cb_build_cast_double (dst), cb_build_cast_double (src));    
    } else {
        return cb_build_move_call (src, dst);
    }
}

/*CIT*/
static cb_tree
cb_build_move_constant (cb_tree src, cb_tree dst) {
    struct cb_field     *f;
    enum cb_category    cat;
    cb_tree             lst = NULL;
    int         i;

    f = cb_field (dst);
    cat = CB_TREE_CATEGORY (dst);
    if ( CB_REFERENCE_P(dst) &&  CB_REFERENCE(dst)->check ) {
        lst = cb_list_init (CB_REFERENCE(dst)->check);
    }

    if ( f->size <= 8 && f->pic->scale == 0 &&
         (f->usage == CB_USAGE_BINARY || f->usage == CB_USAGE_COMP_5 ||
          f->usage == CB_USAGE_COMP_X) ) {
        if ( f->size == 1 ) {
            return cb_build_assign (dst, src);
        }
        if ( f->flag_binary_swap ) {
            i = (f->size - 1) + (8 * (f->pic->have_sign ? 1 : 0));
            return cb_list_add(lst, cb_build_funcall_2 (bin_set_funcs[i],
                                                        cb_build_cast_address (dst),
                                                        src));
        }
        switch ( f->size ) {
            case 2:
#ifdef  COB_SHORT_BORK
                if ( f->storage != CB_STORAGE_LINKAGE && f->indexes == 0 &&
                     (is_base_alligned(dst)) ) {
                    return cb_build_assign (dst, src);
                }
                break;
#endif
            case 4:
            case 8:
#ifdef  COB_NON_ALIGNED
                if ( f->storage != CB_STORAGE_LINKAGE && f->indexes == 0 &&
                     (is_base_alligned(dst)) ) {
                    return cb_build_assign (dst, src);
                }
                break;
#else
                return cb_build_assign (dst, src);
#endif
        }
        if ( CB_CONST_P(src) ) {
            if ( CB_CONST(src)->num_type == CB_CONST_SLL ) {
                return cb_build_funcall_2 ("cob_set_sll", dst, src);
            } else if ( CB_CONST(src)->num_type == CB_CONST_ULL ) {
                return cb_build_funcall_2 ("cob_set_ull", dst, src);
            }
        }
        return cb_build_funcall_2 ("cob_set_int", dst, src);
    } else if ( cb_fits_int (src) && 
                (f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6) ) {
        return cb_build_funcall_2 ("-cob_set_packed_int", dst, src);
    } else {
        if ( CB_CONST_P(src) ) {
            switch ( CB_CONST(src)->num_type ) {
                case CB_CONST_SLL:
                    return cb_build_funcall_2 ("cob_set_sll", dst, src);
                    break;
                case CB_CONST_ULL:
                    return cb_build_funcall_2 ("cob_set_ull", dst, src);
                    break;
                case CB_CONST_FLOAT:
                    return cb_build_funcall_2 ("cob_set_float", dst, src);
                    break;
                case CB_CONST_DOUBLE:
                    return cb_build_funcall_2 ("cob_set_double", dst, src);
                    break;
                default:
                    return cb_build_funcall_2 ("cob_set_int", dst, src);
                    break;
            }
        }
        return cb_build_funcall_2 ("cob_set_int", dst, src);
    }
}

static int
cb_usage_is_compatible (enum cb_usage usage1, enum cb_usage usage2) {
    if ( usage1 == CB_USAGE_BIT && usage2 == CB_USAGE_BIT ) {
        /*Bits field are supposed not compatible (not copy by memcpy*/
        return 0;

    }
    if ( usage1 == usage2 ) {
        return 1;
    }
    if ( ((usage1 == CB_USAGE_BINARY) || (usage1 == CB_USAGE_COMP_5) || (usage1 == CB_USAGE_COMP_X))  &&
         ((usage2 == CB_USAGE_BINARY) || (usage2 == CB_USAGE_COMP_5) || (usage2 == CB_USAGE_COMP_X)) ) {
        return 1;
    }
    return 0;
}

static cb_tree
cb_build_move_field (cb_tree src, cb_tree dst) {
    struct cb_field *src_f;
    struct cb_field *dst_f;
    int     src_size;
    int     dst_size;

    src_f = cb_field (src);
    src_size = cb_field_size (src);
    dst_f = cb_field (dst);
    dst_size = cb_field_size (dst);
    if ( cb_flag_optimize_move && (src_f->id == dst_f->id) ) {
        if ( CB_REFERENCE_P (src) && CB_REFERENCE_P (dst) ) {
            struct cb_reference *rsrc = CB_REFERENCE (src);
            struct cb_reference *rdst = CB_REFERENCE (dst);

            if ( (rsrc->subs == rdst->subs) &&
                 (rsrc->cb_all_occurs == rdst->cb_all_occurs) &&
                 (rsrc->length == rdst->length) &&
                 (rsrc->offset == rdst->offset) ) {
                return cb_build_continue();
            }

        }
    }

    if ( cb_flag_optimize_move && !src_f->flag_any_length && !dst_f->flag_any_length && 
         (src_size > 0 && dst_size > 0 && src_size >= dst_size)
         && (!cb_field_variable_size (src_f) && !cb_field_variable_size (dst_f)) ) {
        switch ( CB_TREE_CATEGORY (src) ) {
            case CB_CATEGORY_ALPHABETIC:
                if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_ALPHABETIC
                     || CB_TREE_CATEGORY (dst) == CB_CATEGORY_ALPHANUMERIC ) {
                    if ( dst_f->flag_justified == 0 ) {
                        return cb_build_move_memcopy (src, dst);
                    }
                }
                break;
            case CB_CATEGORY_ALPHANUMERIC:
                if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_ALPHANUMERIC ) {
                    if ( dst_f->flag_justified == 0 ) {
                        return cb_build_move_memcopy (src, dst);
                    }
                }
                break;
            case CB_CATEGORY_NUMERIC:
                if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC
                     && cb_usage_is_compatible(src_f->usage, dst_f->usage) /*CIT*/
                     && src_f->pic->size == dst_f->pic->size
                     && src_f->pic->digits == dst_f->pic->digits
                     && src_f->pic->scale == dst_f->pic->scale
                     && src_f->pic->have_sign == dst_f->pic->have_sign
                     && src_f->flag_binary_swap == dst_f->flag_binary_swap
                     && src_f->flag_sign_leading == dst_f->flag_sign_leading
                     && src_f->flag_sign_separate == dst_f->flag_sign_separate ) {
                    if ( CB_EXCEPTION_ENABLE (COB_EC_DATA_INCOMPATIBLE) && !cb_disable_runtime_check && dst_f->usage == CB_USAGE_DISPLAY) {
                        goto do_move_call;
                    } else {
                        return cb_build_move_memcopy (src, dst);
                    }
                } else if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_ALPHANUMERIC
                            && src_f->usage == CB_USAGE_DISPLAY
                            && src_f->pic->have_sign == 0
                            && !dst_f->flag_justified
                            && !src_f->flag_sign_leading
                            && !src_f->flag_sign_separate ) {
                    return cb_build_move_memcopy (src, dst);
                }
                break;
            default:
                break;
        }
    }

    /*
    if (cb_flag_optimize_move) {
        if ((dst_f->usage == CB_USAGE_DISPLAY)  &&
            (src_f->usage == CB_USAGE_DOUBLE || src_f->usage == CB_USAGE_FLOAT)) {
            return cb_build_funcall_2 ("cob_display_set_double", dst, cb_build_cast_double(src));
        }
    } 
    */ 

    do_move_call:

    return cb_build_move_call (src, dst);
}

static int
cb_is_group (cb_tree x) {
    struct cb_field *f = cb_field(x);
    if ( f && f->children ) {
        return 1;
    }
    return 0;
}

static cb_tree cb_build_move1 (cb_tree src, cb_tree dst, int validate );

static cb_tree
cb_build_move_group_separated (cb_tree src, cb_tree dst) {
    struct cb_field *f;
    cb_tree          l=NULL;

    f = cb_field(dst)->children;
    for ( ; f; f = f->sister ) {
        l = cb_list_add(l,cb_build_move1(src,cb_build_field_reference(f,dst), 0));
    }
    return l; 
}

static cb_tree
cb_build_move1 (cb_tree src, cb_tree dst, int validate ) {
    /*
    struct cb_field *f;
    struct cb_field *p; 
    */
    cb_tree res = cb_error_node;

    if ( src == cb_error_node || dst == cb_error_node ) {
        return cb_error_node;
    }

    if ( validate && (cb_validate_move (src, dst, 0) < 0) ) {
        return cb_error_node;
    }

    if ( CB_REFERENCE_P (src) ) {
        CB_REFERENCE (src)->type = CB_SENDING_OPERAND;
    }
    if ( CB_REFERENCE_P (dst) ) {
        CB_REFERENCE (dst)->type = CB_RECEIVING_OPERAND;
        if (!cb_generating_initialize) {
            cb_check_setting_action(dst, 1);
        }
    }

    if ( CB_TREE_CLASS (dst) == CB_CLASS_POINTER ) {
        res = cb_build_assign (dst, src);
        res->need_cache_reload = 1;
        goto cb_build_move1_end;
    }

    if ( CB_REFERENCE_P (src) && CB_OBJECT_LIST_P(CB_REFERENCE(src)->value) ) {
        res = cb_build_move_call (src, dst);
        goto cb_build_move1_end;
    }
    if ( CB_REFERENCE_P (dst) && CB_OBJECT_LIST_P(CB_REFERENCE(dst)->value) ) {
        res = cb_build_move_call (src, dst);
        goto cb_build_move1_end;
    }
    if ( CB_REFERENCE_P (src) && CB_ALPHABET_NAME_P(CB_REFERENCE(src)->value) ) {
        res = cb_build_move_call (src, dst);
        goto cb_build_move1_end;
    }
    if ( CB_INDEX_P (dst) ) {
#ifdef  COB_NON_ALIGNED
        if (!is_base_alligned(dst)) {
            res = cb_build_funcall_2 ("cob_set_int", dst, cb_build_cast_integer (src));
            goto cb_build_move1_end;
        }
#endif             
        if ( src == cb_null ) {
            res = cb_build_assign (dst, cb_zero);
            goto cb_build_move1_end;
        }
        res = cb_build_assign (dst, src);
        goto cb_build_move1_end;
    }

    if ( CB_INDEX_P (src) ) {
        res = cb_build_funcall_2 ("cob_set_int", dst, cb_build_cast_integer (src));
        goto cb_build_move1_end;
    }

    if ( CB_INTRINSIC_P (src) || CB_INTRINSIC_P (dst) ) {
        res = cb_build_move_call (src, dst);
        goto cb_build_move1_end;
    }

    /*
    f = cb_field (dst);
    if ( CB_EXCEPTION_ENABLE (COB_EC_BOUND_SUBSCRIPT) ) {
        for ( p = f; p; p = p->parent ) {
            if ( p->flag_occurs ) {
                if ( CB_CONST_P (src)  ) {
                    return cb_build_move_constant(src, dst);
                }
                return cb_build_move_call (src, dst);
            }
        }
        if ( CB_REF_OR_FIELD_P (src) ) {
            for ( p = cb_field (src); p; p = p->parent ) {
                if ( p->flag_occurs ) {
                    return cb_build_move_call (src, dst);
                }
            }
        }
    } 
    */
    if ( cb_flag_move_to_group_separated && 
         (src == cb_zero || src == cb_space ) &&
         cb_is_group(dst) ) {
        res = cb_build_move_group_separated (src, dst);
        goto cb_build_move1_end;
    }
    if ( src == cb_space ) {
        struct cb_field *f = cb_field (dst);

        if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC
             || (CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC_EDITED) ) {

            if (f && (f->usage == CB_USAGE_PACKED || f->usage == CB_USAGE_COMP_6)) {
                if (cb_move_spaces_to_comp3 != CB_MOVEspTO9_SPACE)
                    src = cb_zero;
            } else if (cb_move_spaces_to_displaynumeric != CB_MOVEspTO9_SPACE)
                src = cb_zero;

        }
    }

    if ( src == cb_low || src == cb_high ) {
        if ( CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC                
             || (CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC_EDITED) ) {
            struct cb_field *f = cb_field (dst);                          
            if ( f->usage == CB_USAGE_DISPLAY) {
                if ( cb_move_high_low_to_displaynumeric == CB_MOVEhlTO9_ZERO) {
                    src = cb_zero;                                        
                }
            }
        }
    }

    if ( cb_flag_optimize_move ) {
        if ( CB_LITERAL_P(src) && 
             CB_TREE_CATEGORY (src) == CB_CATEGORY_NUMERIC &&
             CB_TREE_CATEGORY (dst) == CB_CATEGORY_NUMERIC &&
             cb_get_int(src) == 0 ) {
            res = cb_build_move_zero (dst);
            goto cb_build_move1_end;
        }
    }

    /* output optimal code */
    if ( src == cb_zero ) {
        res = cb_build_move_zero (dst);
        goto cb_build_move1_end;
    } else if ( src == cb_space ) {
        res = cb_build_move_space (dst);
        goto cb_build_move1_end;
    } else if ( src == cb_high ) {
        res = cb_build_move_high (dst);
        goto cb_build_move1_end;
    } else if ( src == cb_low ) {
        res = cb_build_move_low (dst);
        goto cb_build_move1_end;
    } else if ( src == cb_quote ) {
        res = cb_build_move_quote (dst);
        goto cb_build_move1_end;
    } else if (cb_flag_optimize_move &&  CB_LITERAL_P (src) ) {
        res = cb_build_move_literal (src, dst);
        goto cb_build_move1_end;
    } else if ( CB_CONST_P (src) ) { /*CIT*/
        res = cb_build_move_constant(src, dst);
        goto cb_build_move1_end;
    }
    res = cb_build_move_field (src, dst);
    goto cb_build_move1_end;

    cb_build_move1_end:
    return add_to_target_ref_list(res, dst);
}

cb_tree
cb_build_move (cb_tree src, cb_tree dst) {
    return cb_build_move1(src,dst,1);
}


void
cb_emit_move_one (cb_tree src, cb_tree dst) {
    cb_tree res;

    if ( cb_validate_one (src) ) {
        return;
    }
    if ( cb_validate_one (dst) ) {
        return;
    }

    res = cb_build_move (src, dst);
    cb_emit (res);
}

void
cb_emit_move (cb_tree src, cb_tree dsts) {
    cb_tree l;
    cb_tree res;

    if ( cb_validate_one (src) ) {
        return;
    }
    if ( cb_validate_list (dsts) ) {
        return;
    }

    for ( l = dsts; l; l = CB_CHAIN (l) ) {
        res = cb_build_move (src, CB_VALUE (l));
        cb_emit (res);
        /*CIT*/
        /*
        if ( cb_flag_spzero && (res != cb_error_node) && need_emit_fixvalue(src, CB_VALUE (l))) {
            cb_emit_fixvalue(CB_VALUE (l), 1);
        } 
        */ 

    }
}

/*
 * OPEN statement
 */

/*CobolIT*/
void
cb_emit_open (cb_tree file, cb_tree mode, cb_tree sharing, cb_tree option) {
    if ( file == cb_error_node ) {
        return;
    }
    file = cb_ref (file);
    if ( file == cb_error_node ) {
        return;
    }
    current_statement->file = file;

    if ( CB_FILE (file)->organization == COB_ORG_SORT ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Operation not allowed on SORT files"));
    }
    if ( mode == cb_int (COB_OPEN_INPUT_REVERSED) ) {
        if ( CB_FILE (file)->organization != COB_ORG_SEQUENTIAL ) {
            cb_error_x (CB_TREE (current_statement),
                        _("REVERSED allowed on SEQUENTIAL files only"));
        }
        if ( CB_FILE (file)->rec_mode == CB_REC_MODE_VARIABLE ) {
            cb_error_x (CB_TREE (current_statement),
                        _("REVERSED not allowed on variable record size files"));
        }
    }
    if ( sharing == NULL ) {
        sharing = CB_FILE (file)->sharing ? CB_FILE (file)->sharing : cb_int(COB_SHARE_NONE);
    }

    /*CobolIT*/
    if ( option == NULL ) {
        option = cb_int0;
    }
    /* READ ONLY */
    if ( sharing == cb_int0 && 
         CB_INTEGER (mode)->val != COB_OPEN_INPUT &&
         CB_INTEGER (mode)->val != COB_OPEN_INPUT_REVERSED &&  
         CB_FILE (file)->lock_mode != COB_LOCK_MANUAL ) {
        sharing = cb_int1;
    }
    cb_emit(cb_build_funcall_2("cob_file_set_fcd_file_name", file, CB_FILE(file)->assign));
    cb_emit (cb_build_funcall_5 ("cob_open_extfh", file, mode,
                                 sharing, CB_FILE(file)->file_status, option));
    cb_emit_wipe_cache();
}

/*
 * PERFORM statement
 */

void
cb_emit_perform (cb_tree perform, cb_tree body) {
    if ( perform == cb_error_node ) {
        return;
    }
    CB_PERFORM (perform)->body = body;
    cb_emit (perform);
    cb_emit_wipe_cache();

}

cb_tree
cb_build_perform_once (cb_tree body) {
    cb_tree x;

    if ( body == cb_error_node ) {
        return cb_error_node;
    }
    x = cb_build_perform (CB_PERFORM_ONCE);
    CB_PERFORM (x)->body = body;
    return x;
}

cb_tree
cb_build_perform_times (cb_tree times) {
    cb_tree x;

    if ( cb_check_integer_value (times) == cb_error_node ) {
        return cb_error_node;
    }

    x = cb_build_perform (CB_PERFORM_TIMES);
    CB_PERFORM (x)->data = times;
    current_program->loop_counter++;
    return x;
}

cb_tree
cb_build_perform_until (cb_tree condition, cb_tree varying) {
    cb_tree x;

    x = cb_build_perform (CB_PERFORM_UNTIL);
    CB_PERFORM (x)->test = condition;
    CB_PERFORM (x)->varying = varying;
    return x;
}

cb_tree
cb_build_perform_forever (cb_tree body) {
    cb_tree x;

    if ( body == cb_error_node ) {
        return cb_error_node;
    }
    x = cb_build_perform (CB_PERFORM_FOREVER);
    CB_PERFORM (x)->body = body;
    return x;
}

cb_tree
cb_build_perform_exit (struct cb_label * label) {
    cb_tree x;

    x = cb_build_perform (CB_PERFORM_EXIT);
    CB_PERFORM (x)->data = CB_TREE (label);
    return x;
}

/*
 * PRAGMA pseudo statement
 */
/*CIT*/
void
cb_emit_pragma (cb_tree pragma, cb_tree option) {
    cb_tree x;

    x = cb_build_pragma ();
    CB_PRAGMA(x)->pragma = pragma;
    CB_PRAGMA(x)->option = option;
    x->source_file = CB_TREE (current_statement)->source_file ;
    x->source_line = CB_TREE (current_statement)->source_line ;
    cb_emit(x);
}

/*
 * READ statement
 */

void
cb_emit_read (cb_tree ref, cb_tree next, cb_tree into, cb_tree key, cb_tree lock_opts) {
    int read_opts = 0;
    cb_tree file;
    cb_tree rec;
    cb_tree res;
    if ( lock_opts == cb_int1 ) {
        read_opts = COB_READ_LOCK;
    } else if ( lock_opts == cb_int2 ) {
        read_opts = COB_READ_NO_LOCK;
    } else if ( lock_opts == cb_int3 ) {
        read_opts = COB_READ_IGNORE_LOCK;
    } else if ( lock_opts == cb_int4 ) {
        read_opts = COB_READ_WAIT_LOCK;
    }
    if ( ref == cb_error_node ) {
        return;
    }
    file = cb_ref (ref);
    if ( file == cb_error_node ) {
        return;
    }
    rec = cb_build_field_reference (CB_FILE (file)->record, ref);
    if ( CB_FILE (file)->organization == COB_ORG_SORT ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Operation not allowed on SORT files"));
    }

    /* following hack is to be conform to MF Syntax
       if AT END is specified on a Indexed file THEN READ is always READ NEXT*/
    if ( cb_flag_read_at_end_mf && 
         current_statement->handler_id == COB_EC_I_O_AT_END && 
         CB_FILE (file)->organization == COB_ORG_INDEXED &&
         next == cb_int0 ) {
        next = cb_int1;
    }
    if ( next == cb_int1 || next == cb_int2 ||
         CB_FILE (file)->access_mode == COB_ACCESS_SEQUENTIAL ) {
        /* READ NEXT/PREVIOUS */
        if ( next == cb_int2 ) {
            if ( (CB_FILE (file)->organization != COB_ORG_INDEXED) &&
                 (CB_FILE (file)->organization != COB_ORG_RELATIVE) ) { /*CIT*/
                cb_error_x (CB_TREE (current_statement),
                            _("READ PREVIOUS only allowed for INDEXED/RELATIVE SEQUENTIAL files"));
            }
            read_opts |= COB_READ_PREVIOUS;
        } else {
            read_opts |= COB_READ_NEXT;
        }
        if ( key ) {
            cb_warning (_("KEY ignored with sequential READ"));
        }
        res = cb_build_funcall_4 ("cob_read_extfh", file, cb_int0, 
                                  CB_FILE(file)->file_status,
                                  cb_int (read_opts));

    } else {
        /* READ */
        res = cb_build_funcall_4 ("cob_read_extfh",
                                  file, key ? key : CB_FILE (file)->key,
                                  CB_FILE(file)->file_status, cb_int (read_opts));
    }
    turn_on_wipe_index_buffer(res)
    cb_emit (res);
    if ( into ) {
        current_statement->handler3 = cb_build_move (rec, into);
    }
    current_statement->file = file;
}

/*
 * READY statement
 */

void
cb_emit_ready (void) {
    cb_emit (cb_build_ready ());
}

/*
 * RESET statement
 */

void
cb_emit_reset (void) {
    cb_emit (cb_build_reset ());
}

/*
 * REWRITE statement
 */

void
cb_emit_rewrite (cb_tree record, cb_tree from, cb_tree lockopt) {
    cb_tree file;
    int opts = 0;
    cb_tree res;
    if ( record == cb_error_node || cb_ref (record) == cb_error_node ) {
        return;
    }
    if ( !CB_REF_OR_FIELD_P (cb_ref (record)) ) {
        cb_error_x (CB_TREE (current_statement),
                    _("REWRITE requires a record name as subject"));
        return;
    }
    if ( cb_field (record)->storage != CB_STORAGE_FILE ) {
        cb_error_x (CB_TREE (current_statement),
                    _("REWRITE subject does not refer to a record name"));
        return;
    }
    file = CB_TREE (CB_FIELD (cb_ref (record))->file);
    current_statement->file = file;
    if ( CB_FILE (file)->organization == COB_ORG_SORT ) {
        cb_error_x (CB_TREE (current_statement),
                    _("Operation not allowed on SORT files"));
    } else if ( current_statement->handler_id == COB_EC_I_O_INVALID_KEY &&
                (CB_FILE(file)->organization != COB_ORG_RELATIVE &&
                 CB_FILE(file)->organization != COB_ORG_INDEXED) && 
                (!cb_verify (cb_invalid_with_file_type, "KEY clause with wrong file type"))
              ) {
        cb_error_x (CB_TREE(current_statement),
                    _("INVALID KEY clause invalid with this file type"));
    } else if ( (CB_FILE (file)->lock_mode & COB_LOCK_AUTOMATIC) && lockopt ) {
        cb_error_x (CB_TREE (current_statement),
                    _("LOCK clause invalid with file LOCK AUTOMATIC"));
    } else if ( lockopt == cb_int1 ) {
        opts = COB_WRITE_LOCK;
    }
    if ( from ) {
        cb_emit (cb_build_move (from, record));
    }
    res = cb_build_funcall_4 ("cob_rewrite_extfh", file, record,
                              cb_int (opts), CB_FILE(file)->file_status);
    turn_on_wipe_index_buffer(res);
    cb_emit (res);
}

/*
 * RELEASE statement
 */

void
cb_emit_release (cb_tree record, cb_tree from) {
    struct cb_field *f;
    cb_tree     file;

    if ( record == cb_error_node ) {
        return;
    }
    if ( from == cb_error_node ) {
        return;
    }
    if ( cb_ref (record) == cb_error_node ) {
        return;
    }
    if ( !CB_REF_OR_FIELD_P (cb_ref (record)) ) {
        cb_error_x (CB_TREE (current_statement),
                    _("RELEASE requires a record name as subject"));
        return;
    }
    if ( cb_field (record)->storage != CB_STORAGE_FILE ) {
        cb_error_x (CB_TREE (current_statement),
                    _("RELEASE subject does not refer to a record name"));
        return;
    }
    f = CB_FIELD (cb_ref (record));
    file = CB_TREE (f->file);
    if ( CB_FILE (file)->organization != COB_ORG_SORT ) {
        cb_error_x (CB_TREE (current_statement),
                    _("RELEASE not allowed on this record item"));
        return;
    }
    current_statement->file = file;
    if ( from ) {
        cb_emit (cb_build_move (from, record));
    }
    /*CIT*/
//    cb_emit (cb_build_funcall_2 ((cb_flag_extsm ? "extsm_release_1" : "cob_file_release_1"), file, record));
    cb_emit (cb_build_funcall_2 (("extsm_release_1_extfh"), file, record));
    cb_emit_wipe_cache();
}

/*
 * RETURN statement
 */

void
cb_emit_return (cb_tree ref, cb_tree into) {
    cb_tree file;
    cb_tree rec;

    if ( ref == cb_error_node ) {
        return;
    }
    if ( into == cb_error_node ) {
        return;
    }
    file = cb_ref (ref);
    if ( file == cb_error_node ) {
        return;
    }
    rec = cb_build_field_reference (CB_FILE (file)->record, ref);
    /*CIT*/
//    cb_emit (cb_build_funcall_1 ((cb_flag_extsm ? "extsm_return" : "cob_file_return"), file));
    cb_emit (cb_build_funcall_1 (("extsm_return_extfh"), file));
    if ( into ) {
        current_statement->handler3 = cb_build_move (rec, into);
    }
    current_statement->file = file;
    cb_emit_wipe_cache();
}

/*
 * ROLLBACK statement
 */

void
cb_emit_rollback (void) {
    cb_emit (cb_build_funcall_0 ("cob_rollback_extfh"));
    cb_emit_wipe_cache();
}

/*
 * SEARCH statement
 */

static void
search_set_keys (struct cb_field *f, cb_tree x) {
    struct cb_binary_op *p;
    int         i;

    if ( CB_REFERENCE_P (x) ) {
        x = cb_build_cond_88 (x);
    }

    p = CB_BINARY_OP (x);
    switch ( p->op ) {
        case '&':
            search_set_keys (f, p->x);
            search_set_keys (f, p->y);
            break;
        case '=':
            for ( i = 0; i < f->nkeys; i++ ) {
                if ( cb_field (p->x) == cb_field (f->keys[i].key) ) {
                    f->keys[i].ref = p->x;
                    f->keys[i].val = p->y;
                    break;
                }
            }
            if ( i == f->nkeys ) {
                /*CIT*/
                for ( i = 0; i < f->nkeys; i++ ) {
                    if ( cb_field (p->y) == cb_field (f->keys[i].key) ) {
                        f->keys[i].ref = p->y;
                        f->keys[i].val = p->x;
                        break;
                    }
                }
                if ( i == f->nkeys ) {
                    cb_error_x (x, _("Undeclared key '%s'"), cb_field (p->x)->name);
                }
            }
            break;
        default:
            cb_error_x (x, _("Invalid SEARCH ALL condition"));
            break;
    }
}

static cb_tree
cb_build_search_all (cb_tree table, cb_tree cond) {
    cb_tree     c1 = NULL;
    cb_tree     c2;
    struct cb_field *f;
    int     i;

    f = cb_field (table);
    /* set keys */
    for ( i = 0; i < f->nkeys; i++ ) {
        f->keys[i].ref = NULL;
    }
    search_set_keys (f, cond);

    /* build condition */
    for ( i = 0; i < f->nkeys; i++ ) {
        if ( f->keys[i].ref ) {
            if ( f->keys[i].dir == COB_ASCENDING ) {
                c2 = cb_build_binary_op (f->keys[i].ref, '=', f->keys[i].val);
            } else {
                c2 = cb_build_binary_op (f->keys[i].val, '=', f->keys[i].ref);
            }
            if ( c1 == NULL ) {
                c1 = c2;
            } else {
                c1 = cb_build_binary_op (c1, '&', c2);
            }
        }
    }

    return cb_build_cond (c1);
}

void
cb_emit_search (cb_tree table, cb_tree varying, cb_tree at_end, cb_tree whens) {
    if ( cb_validate_one (table) ) {
        return;
    }
    if ( cb_validate_one (varying) ) {
        return;
    }
    if ( table == cb_error_node ) {
        return;
    }
    cb_emit (cb_build_search (0, table, varying, at_end, whens));
    cb_emit_wipe_cache();
}

void
cb_emit_search_all (cb_tree table, cb_tree at_end, cb_tree when, cb_tree stmts) {
    if ( cb_validate_one (table) ) {
        return;
    }
    if ( table == cb_error_node ) {
        return;
    }
    cb_emit (cb_build_search (1, table, NULL, at_end,
                              cb_build_if (cb_build_search_all (table, when), stmts, NULL)));
    cb_emit_wipe_cache();
}

/*
 * SET statement
 */

void
cb_emit_setenv (cb_tree x, cb_tree y) {
    cb_emit (cb_build_funcall_2 ("cob_set_environment", x, y));
}

void
cb_emit_set_to (cb_tree vars, cb_tree x) {
    cb_tree     l;
    cb_tree     v;
    struct cb_cast  *p;
#if 0
    enum cb_class class = CB_CLASS_UNKNOWN;
#endif

    if ( cb_validate_one (x) ) {
        return;
    }
    if ( cb_validate_list (vars) ) {
        return;
    }

#if 0
    /* determine the class of targets */
    for ( l = vars; l; l = CB_CHAIN (l) ) {
        if ( CB_TREE_CLASS (CB_VALUE (l)) != CB_CLASS_UNKNOWN ) {
            if ( class == CB_CLASS_UNKNOWN ) {
                class = CB_TREE_CLASS (CB_VALUE (l));
            } else if ( class != CB_TREE_CLASS (CB_VALUE (l)) ) {
                break;
            }
        }
    }
    if ( l || (class != CB_CLASS_INDEX && class != CB_CLASS_POINTER) ) {
        cb_error_x (CB_TREE (current_statement),
                    _("The targets of SET must be either indexes or pointers"));
        return;
    }
#endif

    if ( CB_CAST_P (x) ) {
        p = CB_CAST (x);
        if ( p->type == CB_CAST_PROGRAM_POINTER ) {
            for ( l = vars; l; l = CB_CHAIN (l) ) {
                v = CB_VALUE (l);
                if ( !CB_REFERENCE_P (v) ) {
                    cb_error_x (CB_TREE (current_statement),
                                _("SET targets must be PROGRAM-POINTER"));
                    CB_VALUE (l) = cb_error_node;
                } else if ( CB_FIELD(cb_ref(v))->usage != CB_USAGE_PROGRAM_POINTER ) {
                    cb_error_x (CB_TREE (current_statement),
                                _("SET targets must be PROGRAM-POINTER"));
                    CB_VALUE (l) = cb_error_node;
                }
            }
        }
    }
    /* validate the targets */
    for ( l = vars; l; l = CB_CHAIN (l) ) {
        v = CB_VALUE (l);
        if ( CB_CAST_P (v) ) {
            p = CB_CAST (v);
            if ( p->type == CB_CAST_ADDRESS) {
                cb_tree r= cb_ref (p->val);
                if (CB_FIELD_P (r) 
                    && !CB_FIELD (r)->flag_item_based
                    && CB_FIELD (r)->storage != CB_STORAGE_LINKAGE ) {
                    cb_error_x (p->val, _("The address of '%s' cannot be changed"),
                                cb_name (p->val));
                    CB_VALUE (l) = cb_error_node;
                }
                if (CB_FILE_P(r) && !CB_FILE(r)->external) {
                    cb_error_x (p->val, _("The address of '%s' cannot be changed if not external"),
                                cb_name (p->val));
                    CB_VALUE (l) = cb_error_node;
                }
            }
        }
    }
    if ( cb_validate_list (vars) ) {
        return;
    }

    for ( l = vars; l; l = CB_CHAIN (l) ) {
        cb_emit (cb_build_move (x, CB_VALUE (l)));
    }
}

void
cb_emit_set_up_down (cb_tree l, cb_tree flag, cb_tree x) {
    if ( cb_validate_one (x) ) {
        return;
    }
    if ( cb_validate_list (l) ) {
        return;
    }
    for ( ; l; l = CB_CHAIN (l) ) {
        if ( flag == cb_int0 ) {
            cb_emit (cb_build_add (CB_VALUE (l), x, cb_int0));
        } else {
            cb_emit (cb_build_sub (CB_VALUE (l), x, cb_int0));
        }
    }
}

void
cb_emit_set_on_off (cb_tree l, cb_tree flag) {
    struct cb_system_name *s;

    if ( cb_validate_list (l) ) {
        return;
    }
    for ( ; l; l = CB_CHAIN (l) ) {
        s = CB_SYSTEM_NAME (cb_ref (CB_VALUE (l)));
        cb_emit (cb_build_funcall_2 ("cob_set_switch", cb_int (s->token), flag));
    }
}

void
cb_emit_set_true (cb_tree l) {
    cb_tree     x;
    struct cb_field *f;
    cb_tree     ref;
    cb_tree     val;

    for ( ; l; l = CB_CHAIN (l) ) {
        x = CB_VALUE (l);
        if ( x == cb_error_node ) {
            return;
        }
        if ( !(CB_REFERENCE_P (x) && CB_FIELD_P(CB_REFERENCE(x)->value))
             && !CB_FIELD_P (x) ) {
            cb_error_x (x, _("Invalid SET statement"));
            return;
        }
        f = cb_field (x);
        if ( f->level != 88 ) {
            cb_error_x (x, _("Invalid SET statement (Not Level 88 actual is %d)"), f->level );
            return;
        }
        ref = cb_build_field_reference (f->parent, x);
        val = CB_VALUE (f->values);
        if ( CB_PAIR_P (val) ) {
            val = CB_PAIR_X (val);
        }
        cb_emit (cb_build_move (val, ref));
    }
}

void
cb_emit_set_false (cb_tree l) {
    cb_tree     x;
    struct cb_field *f;
    cb_tree     ref;
    cb_tree     val;

    for ( ; l; l = CB_CHAIN (l) ) {
        x = CB_VALUE (l);
        if ( x == cb_error_node ) {
            return;
        }
        if ( !(CB_REFERENCE_P (x) && CB_FIELD_P(CB_REFERENCE(x)->value))
             && !CB_FIELD_P (x) ) {
            cb_error_x (x, _("Invalid SET statement"));
            return;
        }
        f = cb_field (x);
        if ( f->level != 88 ) {
            cb_error_x (x, _("Invalid SET statement"));
            return;
        }
        if ( !f->false_88 ) {
            cb_error_x (x, _("Field does not have FALSE clause"));
            return;
        }
        ref = cb_build_field_reference (f->parent, x);
        val = CB_VALUE (f->false_88);
        if ( CB_PAIR_P (val) ) {
            val = CB_PAIR_X (val);
        }
        cb_emit (cb_build_move (val, ref));
    }
}

/*
 * SORT statement
 */

static void
cb_emit_noe(cb_tree x) {
    if (CB_FUNCALL_P(x)) {
        struct cb_funcall *f = CB_FUNCALL(x);
        f->not_on_exception =1;
    }
    cb_emit(x);
}

void
cb_emit_sort_init (cb_tree name, cb_tree keys, cb_tree col) {
    cb_tree         l;
    struct cb_field *f;
    int             i;
    int             id_comp = 0;
    int             nkeys;
    if ( cb_validate_list (keys) ) {
        return;
    }
    for ( l = keys; l; l = CB_CHAIN (l) ) {
        if ( CB_VALUE (l) == NULL ) {
            CB_VALUE (l) = name;
        }
        cb_ref (CB_VALUE (l));
    }

    if ( CB_FILE_P (cb_ref (name)) ) {

        if ( CB_FILE (cb_ref (name))->organization != COB_ORG_SORT ) {
            cb_error_x (name, _("Invalid SORT filename"));
        }
        /*COBOL-IT*/
        nkeys = cb_list_length (keys);
//        cb_emit (cb_build_funcall_5 ((cb_flag_extsm ?"extsm_sort_init": "cob_file_sort_init"),
        cb_emit (cb_build_funcall_5 (("extsm_sort_init_extfh"),
                                     cb_ref (name),
                                     cb_int (nkeys), col,
                                     cb_build_cast_address(current_program->cb_sort_return),
                                     CB_FILE(cb_ref (name))->file_status));
        if(nkeys) {
            for ( l = keys; l; l = CB_CHAIN (l) ) {
                cb_emit (cb_build_funcall_5("cob_define_key_component_sort",
                             cb_ref (name), cb_int(id_comp), CB_PURPOSE (l), CB_VALUE (l), cb_int(cb_field(CB_VALUE(l))->offset)));
                id_comp++;

            }
        }

    } else {
        f = CB_FIELD (cb_ref (name));
        if ( keys == NULL ) {
            if ( f->nkeys && f->keys ) {
                keys = cb_build_pair(cb_int(f->keys[0].dir), cb_ref(f->keys[0].key));
                for ( i=1; i < f->nkeys; i++ ) {
                    keys = cb_list_append(keys, cb_build_pair(cb_int(f->keys[i].dir), cb_ref(f->keys[i].key)));
                }
            } else {
                cb_error_x (name, _("Table sort without keys not implemented yet"));
            }
        }
        cb_emit (cb_build_funcall_2 ("cob_table_sort_init", cb_int (cb_list_length (keys)), col));
        for ( l = keys; l; l = CB_CHAIN (l) ) {
            cb_emit (cb_build_funcall_3 ("cob_table_sort_init_key",
                                         CB_PURPOSE (l),
                                         CB_VALUE (l),
                                         cb_int(cb_field(CB_VALUE(l))->offset - cb_field(name)->offset)));
        }
        cb_emit (cb_build_funcall_2 ("cob_table_sort", name,
                                     (f->occurs_depending
                                      ? cb_build_cast_integer (f->occurs_depending)
                                      : cb_int (f->occurs_max))));
    }
    cb_emit_wipe_cache();
}

void
cb_emit_sort_using (cb_tree file, cb_tree l) {
    if ( cb_validate_list (l) ) {
        return;
    }
    current_statement->file = cb_list_append_contents(current_statement->file, l) ;
    for ( ; l; l = CB_CHAIN (l) ) {
        if ( CB_FILE (cb_ref(CB_VALUE(l)))->organization == COB_ORG_SORT ) {
            cb_error (_("Invalid SORT USING parameter"));
        }
        /*COBOL-IT*/
        cb_emit_noe (cb_build_funcall_2 (("extsm_sort_using_extfh"),
                                     cb_ref (file), cb_ref (CB_VALUE (l))));
    }
}

/*COBOL-IT*/
void
cb_emit_sort_input (cb_tree file, cb_tree proc) {
//  if ( cb_flag_extsm ) {
        current_statement->on_hold = cb_list_init(cb_build_perform_once(proc));
        current_statement->on_hold = cb_list_add (current_statement->on_hold, 
                                                  cb_build_funcall_1 ("extsm_end_input_extfh", cb_ref (file)));
//  } else {
//      cb_emit_noe (cb_build_perform_once (proc));
//  }
}

void
cb_emit_sort_giving (cb_tree file, cb_tree l,int input_proc_used) {
    cb_tree     p;
    int     listlen;

    if ( cb_validate_list (l) ) {
        return;
    }
    for ( p = l; p; p = CB_CHAIN (p) ) {
        if ( CB_FILE (cb_ref(CB_VALUE(p)))->organization == COB_ORG_SORT ) {
            cb_error (_("Invalid SORT GIVING parameter"));
        }
    }
    current_statement->file = cb_list_append_contents(current_statement->file, l) ;
    listlen = cb_list_length (l);
    /*COBOL-IT*/
//    p = cb_build_funcall_2 ((cb_flag_extsm ? "extsm_sort_giving": "cob_file_sort_giving"), cb_ref (file), l);
    p = cb_build_funcall_2 (("extsm_sort_giving_extfh"), cb_ref (file), l);
    CB_FUNCALL(p)->varcnt = listlen;
    if (input_proc_used) {
        cb_emit(p);
    } else {
        cb_emit_noe(p);
    }
//    if ( cb_flag_extsm ) {
        cb_emit_noe (cb_build_funcall_1 ("extsm_process_extfh", cb_ref (file)));
        if ( current_statement->on_hold ) {
            cb_emit_noe(current_statement->on_hold);
        }
//    }
}

/*COBOL-IT*/
void
cb_emit_sort_output (cb_tree file, cb_tree proc) {
//    if ( cb_flag_extsm ) {
        cb_emit_noe (cb_build_funcall_1 ("extsm_process_extfh",  cb_ref (file)));
        if ( current_statement->on_hold ) {
            cb_emit_noe(current_statement->on_hold);
        }
//    }
    cb_emit_noe (cb_build_perform_once (proc));
//    if ( cb_flag_extsm ) {
        cb_emit_noe (cb_build_funcall_1 ("extsm_end_output_extfh", cb_ref (file)));
//    }
}

void
cb_emit_sort_finish (cb_tree file) {
    if ( CB_FILE_P (cb_ref (file)) ) {
        /*COBOL-IT*/
//        current_statement->final = cb_build_funcall_1((cb_flag_extsm ? "extsm_sort_close" : "cob_file_sort_close"), cb_ref(file));
        current_statement->final = cb_build_funcall_1(("extsm_sort_close_extfh"), cb_ref(file));
    }
    cb_emit_wipe_cache();
}

/*
 * START statement
 */

static int 
get_key_length_from_obj_list(cb_tree key) { 
    int sz = 0;
    if (CB_OBJECT_LIST_P(key) && (CB_OBJECT_LIST(key)->type == CB_OBJECT_LIST_FIELD)) {
        struct cb_object_list *o;
        cb_tree l;
        o = CB_OBJECT_LIST(key);
        l = o->olist;
        for (; l; l = CB_CHAIN(l)) {
            if (CB_REF_OR_FIELD_P(CB_VALUE(l))) {
                struct cb_field *fl = cb_field(CB_VALUE(l));
                sz += fl->size;
            }
        }
    } 
    return sz;
}

void
cb_emit_start (cb_tree file, cb_tree op, cb_tree key) {
    if ( cb_validate_one (key) ) {
        return;
    }
    if ( file != cb_error_node ) {
        cb_tree f = cb_ref(file);
        if (CB_FILE (f)->organization == COB_ORG_INDEXED) {
            cb_tree k;
            int id_key = -1;
            int len = -2;

            current_statement->file = f;

            if (key) {
                k = cb_ref(key);
                if (CB_FIELD_P(k)) {
                    if (   CB_REFERENCE_P(CB_FILE(f)->key) 
                          && derived_from_same_base_root(CB_FIELD(k), CB_FIELD(CB_REFERENCE(CB_FILE(f)->key)->value))
                          && CB_FIELD(k)->offset == CB_FIELD(CB_REFERENCE(CB_FILE(f)->key)->value)->offset )
                    {
                        id_key = 0;
                        if (CB_FIELD(k)->size <= CB_FIELD(CB_REFERENCE(CB_FILE(f)->key)->value)->size) {
                            len = CB_FIELD(k)->size;
                        } else {
                            cb_error (_("Length of START field exceeds key length"));
                        }
                    } else {
                        struct cb_alt_key *l;
                        int i = 1;
                        for (l = CB_FILE(f)->alt_key_list; l; l = l->next) {
                            cb_tree r = cb_ref(l->key);
                            if ((r && CB_FIELD_P(r))) {
                                if ( derived_from_same_base_root(CB_FIELD(k), CB_FIELD(r))
                                        && CB_FIELD(k)->offset == CB_FIELD(r)->offset
                                ) {
                                    struct cb_field *f = cb_field(r);
                                    id_key = i;
                                    if (CB_FIELD(k)->size <= f->size) {
                                        len = CB_FIELD(k)->size;
                                    } else {
                                        cb_error (_("Length of START field exceeds length of key field"));
                                    }
                                    break;
                                } 
                            }
                            i++;
                        }
                        if (id_key < 0) {
                            cb_error(_("Invalid START field"));
                        }
                    }
                } else if (CB_OBJECT_LIST_P(k)) {
                    if (k == CB_FILE(f)->key) {
                        id_key = 0;
                        len = get_key_length_from_obj_list(k);
                    } else {
                        struct cb_alt_key *l;
                        int i = 1;
                        for (l = CB_FILE(f)->alt_key_list; l; l = l->next) {
                            if (k == l->key) {
                                id_key = i;
                                len = get_key_length_from_obj_list(k);
                                break;
                            }
                            i++;
                        }
                    }
                }
            } else {
                id_key = 0;
                if (CB_REFERENCE_P(CB_FILE(f)->key)) {
                    len = CB_FIELD(CB_REFERENCE(CB_FILE(f)->key)->value)->size;
                } else if (CB_OBJECT_LIST_P(CB_FILE(f)->key)) {
                    len = get_key_length_from_obj_list(CB_FILE(f)->key);
                }
            }
            cb_emit (cb_build_funcall_5 ("cob_start_extfh_indexed", cb_ref (file), op,
                                     cb_int(id_key),
                                     cb_int(len),  
                                     CB_FILE(cb_ref(file))->file_status));
        } else {
            if ( file != cb_error_node ) {
                current_statement->file = cb_ref (file);
                cb_emit (cb_build_funcall_4 ("cob_start_extfh_relative", cb_ref (file), op,
                                             key ? key : CB_FILE (cb_ref (file))->key,
                                             CB_FILE(cb_ref(file))->file_status));
            }
        }
    }
    cb_emit_wipe_cache();
}

/*
 * STOP statement
 */

void
cb_emit_stop_run (cb_tree x) {
    cb_emit (cb_build_funcall_1 ("cob_stop_run", cb_build_cast_integer (x)));
    cb_emit_wipe_cache();
}

/*
 * STOP ABEND statement
 */
/*COBOL-IT*/
void
cb_emit_stop_abend (cb_tree x) {
    cb_emit (cb_build_funcall_1 ("cob_stop_abend", cb_build_cast_integer (x)));
    cb_emit_wipe_cache();
}
/*
 * STRING statement
 */

void
cb_emit_string (cb_tree items, cb_tree into, cb_tree pointer) {
    cb_tree start;
    cb_tree l;
    cb_tree end;
    cb_tree dlm;

    if ( cb_validate_one (into) ) {
        return;
    }
    if ( cb_validate_one (pointer) ) {
        return;
    }
    start = items;
    cb_emit (cb_build_funcall_2 ("cob_string_init", into, pointer));
    while ( start ) {

        /* find DELIMITED item */
        for ( end = start; end; end = CB_CHAIN (end) ) {
            if ( CB_PAIR_P (CB_VALUE (end)) ) {
                break;
            }
        }

        /* cob_string_delimited */
        dlm = end ? CB_PAIR_X (CB_VALUE (end)) : cb_int0;
        cb_emit (cb_build_funcall_1 ("cob_string_delimited", dlm));

        /* cob_string_append */
        for ( l = start; l != end; l = CB_CHAIN (l) ) {
            cb_emit (cb_build_funcall_1 ("cob_string_append", CB_VALUE (l)));
        }

        start = end ? CB_CHAIN (end) : NULL;
    }
    cb_emit (cb_build_funcall_0 ("cob_string_finish"));
    cb_emit_wipe_cache();
}

/*
 * UNLOCK statement
 */

void
cb_emit_unlock (cb_tree ref) {
    cb_tree file;

    if ( ref != cb_error_node ) {
        file = cb_ref (ref);
        cb_emit (cb_build_funcall_2 ("cob_unlock_file_extfh",
                                     file, CB_FILE(file)->file_status));
        current_statement->file = file;
    }
}

/*
 * UNSTRING statement
 */

void
cb_emit_unstring (cb_tree name, cb_tree delimited, cb_tree into, cb_tree pointer, cb_tree tallying) {
    if ( cb_validate_one (name) ) {
        return;
    }
    if ( cb_validate_one (tallying) ) {
        return;
    }
    if ( cb_validate_list (delimited) ) {
        return;
    }
    if ( cb_validate_list (into) ) {
        return;
    }
    cb_emit (cb_build_funcall_3 ("cob_unstring_init", name, pointer,
                                 cb_int(cb_list_length(delimited))));
    cb_emit_list (delimited);
    cb_emit_list (into);
    if ( tallying ) {
        cb_emit (cb_build_funcall_1 ("cob_unstring_tallying", tallying));
    }
    cb_emit (cb_build_funcall_0 ("cob_unstring_finish"));
    cb_emit_wipe_cache();
}

cb_tree
cb_build_unstring_delimited (cb_tree all, cb_tree value) {
    if ( cb_validate_one (value) ) {
        return cb_error_node;
    }
    return cb_build_funcall_2 ("cob_unstring_delimited", value, all);
}

cb_tree
cb_build_unstring_into (cb_tree name, cb_tree delimiter, cb_tree count) {
    if ( cb_validate_one (name) ) {
        return cb_error_node;
    }
    if ( delimiter == NULL ) {
        delimiter = cb_int0;
    }
    if ( count == NULL ) {
        count = cb_int0;
    }
    return cb_build_funcall_3 ("cob_unstring_into", name, delimiter, count);
}

/*
 * WRITE statement
 */

void
cb_emit_write (cb_tree record, cb_tree from, cb_tree opt, cb_tree lockopt) {
    cb_tree     file;
    int     val;
    cb_tree res;

    if ( record != cb_error_node && cb_ref (record) != cb_error_node ) {
        if ( !CB_REF_OR_FIELD_P (cb_ref (record)) ) {
            cb_error_x (CB_TREE (current_statement),
                        _("WRITE requires a record name as subject"));
            return;
        }
        if ( cb_field (record)->storage != CB_STORAGE_FILE ) {
            cb_error_x (CB_TREE (current_statement),
                        _("WRITE subject does not refer to a record name"));
            return;
        }
        file = CB_TREE (CB_FIELD (cb_ref (record))->file);
        current_statement->file = file;
        if ( CB_FILE (file)->organization == COB_ORG_SORT ) {
            cb_error_x (CB_TREE (current_statement),
                        _("Operation not allowed on SORT files"));
        } else if ( current_statement->handler_id == COB_EC_I_O_INVALID_KEY &&
                    (CB_FILE(file)->organization != COB_ORG_RELATIVE &&
                     CB_FILE(file)->organization != COB_ORG_INDEXED) && 
                    (!cb_verify (cb_invalid_with_file_type, "KEY clause with wrong file type")) ) {
            cb_error_x (CB_TREE(current_statement),
                        _("INVALID KEY clause invalid with this file type"));
        } else if ( lockopt ) {
            if ( (CB_FILE (file)->lock_mode & COB_LOCK_AUTOMATIC) ) {
                cb_error_x (CB_TREE (current_statement),
                            _("LOCK clause invalid with file LOCK AUTOMATIC"));
            } else if ( opt != cb_int0 ) {
                cb_error_x (CB_TREE (current_statement),
                            _("LOCK clause invalid here"));
            } else if ( lockopt == cb_int1 ) {
                opt = cb_int (COB_WRITE_LOCK);
            }
        }
        if ( from ) {
            /*need cb_dup_reference to preserve CB_SENDING_OPERAND status of RECORD*/
            cb_emit (cb_build_move (from, cb_dup_reference(record)));
        }
        if ( CB_FILE (file)->organization == COB_ORG_LINE_SEQUENTIAL &&
             opt == cb_int0 ) {
            /*CIT
            if ( cb_flag_all_extfh ) {
                opt = cb_int (COB_WRITE_LINES | 1);
            } else {
                opt = cb_int (COB_WRITE_BEFORE | COB_WRITE_LINES | 1);
            } 
            */ 
            /* opt = cb_int (COB_WRITE_BEFORE | COB_WRITE_LINES | 1); */

        }
        /* RXW - This is horrible */
        if ( current_statement->handler_id == COB_EC_I_O_EOP &&
             current_statement->handler1 ) {
            if ( CB_CAST_P(opt) ) {
                val = CB_INTEGER(CB_BINARY_OP(CB_CAST(opt)->val)->x)->val;
                val |= COB_WRITE_EOP;
                CB_BINARY_OP(CB_CAST(opt)->val)->x = cb_int (val);
            } else {
                val = CB_INTEGER(opt)->val;
                val |= COB_WRITE_EOP;
                opt = cb_int (val);
            }
        }

        res = cb_build_funcall_4 ("cob_write_extfh", file, record, opt,
                                  CB_FILE(file)->file_status);
        turn_on_wipe_index_buffer(res);
        cb_emit (res);
    }
}

cb_tree
cb_build_write_advancing_lines (cb_tree pos, cb_tree lines) {
    cb_tree e;
    int opt;

    opt = (pos == CB_BEFORE) ? COB_WRITE_BEFORE : COB_WRITE_AFTER;
    e = cb_build_binary_op (cb_int (opt | COB_WRITE_LINES), '+', lines);
    return cb_build_cast_integer (e);
}

cb_tree
cb_build_write_advancing_mnemonic (cb_tree pos, cb_tree mnemonic) {
    int opt;
    int token;

    token = CB_SYSTEM_NAME (cb_ref (mnemonic))->token;
    switch ( token ) {
        case CB_FEATURE_FORMFEED:
            opt = (pos == CB_BEFORE) ? COB_WRITE_BEFORE : COB_WRITE_AFTER;
            return cb_int (opt | COB_WRITE_PAGE);
        case CB_FEATURE_CSP:
            opt = (pos == CB_BEFORE) ? COB_WRITE_BEFORE : COB_WRITE_AFTER;
            return cb_int (opt);
        case CB_FEATURE_C01:
        case CB_FEATURE_C02:
        case CB_FEATURE_C03:
        case CB_FEATURE_C04:
        case CB_FEATURE_C05:
        case CB_FEATURE_C06:
        case CB_FEATURE_C07:
        case CB_FEATURE_C08:
        case CB_FEATURE_C09:
        case CB_FEATURE_C10:
        case CB_FEATURE_C11:
        case CB_FEATURE_C12:
            opt = (pos == CB_BEFORE) ? COB_WRITE_BEFORE : COB_WRITE_AFTER;
            return cb_int (opt | COB_WRITE_CHANNEL | token);
        default:
            cb_error_x (mnemonic, _("Invalid mnemonic name"));
            return cb_error_node;
    }
}

cb_tree
cb_build_write_advancing_page (cb_tree pos) {
    int opt = (pos == CB_BEFORE) ? COB_WRITE_BEFORE : COB_WRITE_AFTER;

    return cb_int (opt | COB_WRITE_PAGE);
}


void 
cb_emit_set_context_filename (cb_tree var) {
    if ( cb_validate_one (var) ) {
        return;
    }
    cb_emit (cb_build_funcall_1 ("cob_set_context_appli_prefix_field", var));
}

void 
cb_emit_init_context_save (cb_tree exit_flag) {
    cb_emit (cb_build_funcall_1 ("cob_init_context_save", exit_flag));
}

void 
cb_emit_xml_parse (cb_tree ident, cb_tree proc, cb_tree encoding, cb_tree returning_national) {
    cb_tree x;
    cb_tree l;
    cb_tree condition;

    cb_emit(cb_build_move(cb_zero, current_program->cb_xml_code));
    cb_emit (cb_build_funcall_6 ("cob_enterprise_xml_parse_init_1", ident, current_program->cb_xml_code, 
                                 current_program->cb_xml_text, 
                                 returning_national ? current_program->cb_xml_ntext: cb_null,
                                 current_program->cb_xml_event, encoding));
    cb_emit (cb_build_funcall_1 ("$P" , current_program->cb_xml_event));
    cb_emit (cb_build_funcall_1 ("$P" , current_program->cb_xml_text));
    if (returning_national) {
        cb_emit(cb_build_funcall_1("$P", current_program->cb_xml_ntext));
    }

    x = cb_build_perform (CB_PERFORM_UNTIL);
    CB_PERFORM (x)->test = CB_BEFORE;
    CB_PERFORM (x)->body = proc;

    condition = cb_build_cond(cb_build_binary_op (current_program->cb_xml_code, '~', cb_zero));
    l = cb_list_init(cb_build_funcall_4("cob_enterprise_xml_parse_next", current_program->cb_xml_code,
                                        current_program->cb_xml_text,
                                        returning_national ? current_program->cb_xml_ntext : cb_null,
                                        current_program->cb_xml_event));
    l = cb_list_add(l, cb_build_funcall_1("$P", current_program->cb_xml_event));
    l = cb_list_add(l, cb_build_funcall_1("$P", current_program->cb_xml_text));
    if (returning_national) {
        l = cb_list_add(l, cb_build_funcall_1("$P", current_program->cb_xml_ntext));
    }
    CB_PERFORM(x)->varying = cb_list_init(cb_build_perform_varying_step(NULL, NULL, l, condition));
    cb_emit(x);
    cb_emit (cb_build_funcall_0 ("cob_enterprise_xml_parse_close"));
    cb_field (current_program->cb_xml_code)->count++;
    cb_field (current_program->cb_xml_text)->count++;
    cb_field (current_program->cb_xml_event)->count++;
    if (returning_national) {
        cb_field(current_program->cb_xml_ntext)->count++;
    }
    cb_emit_wipe_cache();
}


static cb_tree 
cb_build_xml_generate (cb_tree ident, cb_tree from, cb_tree cnt, cb_tree xml_code) {
    cb_tree x;
    struct cb_reference *r;
    struct cb_field *f;
    struct cb_field *c;
    struct cb_reference *a;
    cb_tree res = NULL;
    cb_tree l = NULL;
    cb_tree ll = NULL;


    if (!CB_REFERENCE_P(from) ) {
        cb_error_x(from, "must be a reference");
        return NULL;
    }
    x = from;
    r = CB_REFERENCE(x);
    f = cb_field(x);
    f->count++;
    if ( f->redefines )
        return NULL;
    if (!memcmp (f->name, CB_PREFIX_FILLER, CB_PREFIX_FILLER_LEN))
        return NULL;
    if ( f->usage ==  CB_USAGE_POINTER || f->usage == CB_USAGE_PROGRAM || f->usage == CB_USAGE_PROGRAM_POINTER) {
        return NULL;
    }
    if ( f->flag_occurs && r->all) {
        cb_tree temp = cb_build_index (cb_build_filler (), NULL, 0, NULL, 0, CB_USAGE_INDEX);

        CB_FIELD (cb_ref (temp))->count++;
        a = CB_REFERENCE(cb_dup_reference(x));
        a->all = 0;
        a->subs = cb_list_insert_first(a->subs, temp);
        l = cb_list_add(l,  cb_build_xml_generate(ident,CB_TREE(a),cnt, xml_code));
        l = cb_list_add(l,  cb_build_add(temp, cb_int1, cb_int0));

        res = cb_list_add(res, cb_build_move(cb_int1, temp));
        if (f->occurs_depending) {
            ll = cb_build_perform_times(f->occurs_depending);
        } else {
            ll = cb_build_perform_times(cb_int(f->occurs_max));
        }
        CB_PERFORM (ll)->body = l;
        res = cb_list_add(res, ll);
    } else {
        res = cb_list_add(res,  cb_build_funcall_5 ("cob_enterprise_xml_generate_name", ident, 
                                                    cb_build_alphanumeric_literal((unsigned char*)f->name, strlen(f->name), 0 ) , cnt, xml_code,cb_false));
        if (f->children) {
            c = f->children;
            while ( c ) {
                l = cb_build_field_reference(c, x);
                if ( c->flag_occurs ) {
                    CB_REFERENCE(l)->all = 1;
                }
                res = cb_list_add(res,  cb_build_xml_generate(ident,l,cnt, xml_code));            
                c->count++;
                c = c->sister;
            }
        } else {
            res = cb_list_add(res,  cb_build_funcall_4 ("cob_enterprise_xml_generate_data", ident, x , cnt, xml_code));            
        }
        res = cb_list_add(res,  cb_build_funcall_5 ("cob_enterprise_xml_generate_name", ident, 
                                                    cb_build_alphanumeric_literal((unsigned char*)f->name,strlen(f->name), 0 ) , cnt, xml_code, cb_true));
    }
    return res;
}

void 
cb_emit_xml_generate (cb_tree ident, cb_tree from, cb_tree cnt) {
    cb_tree temp;
    temp = cb_build_index (cb_build_filler (), NULL, 0, NULL, 0, CB_USAGE_INDEX);
    CB_FIELD (cb_ref (temp))->count++;
    cb_emit(cb_build_move(cb_zero, current_program->cb_xml_code));
    cb_emit(cb_build_move(cb_zero, temp));
    cb_emit(cb_build_move(cb_space, ident));
    cb_field (current_program->cb_xml_code)->count++;

    cb_emit(cb_build_xml_generate(ident,from,temp, current_program->cb_xml_code));
    if (cnt) {
        cb_emit_move(temp, cb_list_init(cnt));
    }
    cb_emit_wipe_cache();
}

void 
cb_emit_wipe_cache (void)
{
    cb_tree p = cb_build_pragma();
    p->wipe_index_buffer = 1;
    delete_list();
    cb_emit(p);
}

void
cb_emit_target_cache (cb_tree x)
{
    cb_tree p = cb_build_pragma();
    add_to_target_ref_list(p,x);
    cb_emit(p);
}


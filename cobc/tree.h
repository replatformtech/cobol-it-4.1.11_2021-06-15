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

#ifndef CB_TREE_H
#define CB_TREE_H

#define CB_BEFORE               cb_int0
#define CB_AFTER                cb_int1

#define COB_MAX_SUBSCRIPTS      16

#define CB_PREFIX_ATTR          "a_"    /* field attribute (cob_field_attr) */
#define CB_PREFIX_BASE          "b_"    /* base address (unsigned char *) */
#define CB_PREFIX_CONST         "c_"    /* constant or literal (cob_field) */
#define CB_PREFIX_DECIMAL       "d_"    /* decimal number (cob_decimal) */
#define CB_PREFIX_FIELD         "f_"    /* field (cob_field) */
#define CB_PREFIX_FILE          "h_"    /* file (cob_file_extfh) */
#define CB_PREFIX_KEYS          "k_"    /* file keys (struct cob_file_key []) */
#define CB_PREFIX_LABEL         "l_"    /* label */
#define CB_PREFIX_SEQUENCE      "s_"    /* collating sequence */
#define CB_PREFIX_BASE_PARAM    "p_"    /* base address for a param (unsigned char *) */
#define CB_PREFIX_FIELDLIST     "fl_"   /* Field List */
/*CIT*/
#define CB_PREFIX_LINKAGE       "cob_parm_"
#define CB_PREFIX_STR           "cob_str_"
#define CB_PREFIX_RTD           "rtd"
#define CB_PREFIX_STATIC        "sta_"
#define CB_PREFIX_SCREEN        "scr_"
#define CB_PREFIX_FILLER        "FILLER$"
#define CB_PREFIX_BITFIELD_FILLER "BITFIELD$"
#define CB_PREFIX_FILLER_LEN    7
#define CB_PREFIX_BITFIELD_FILLER_LEN    9
#define LABEL_PREFIX "label_"
#define FILE_PREFIX  "file_"
#define PROFILING_SUFFIX "prof_info"

#define CB_PROGRAM_TYPE     0
#define CB_FUNCTION_TYPE    1

enum cb_tag {
    /* primitives */
    CB_TAG_CONST,           /* 0 constant value */
    CB_TAG_INTEGER,         /* 1 integer constant */
    CB_TAG_STRING,          /* 2 string constant */
    CB_TAG_ALPHABET_NAME,   /* 3 alphabet-name */
    CB_TAG_CLASS_NAME,      /* 4 class-name */
    CB_TAG_LOCALE_NAME,     /* 5 locale-name */
    CB_TAG_SYSTEM_NAME,     /* 6 system-name */
    CB_TAG_LITERAL,         /* 7 numeric/alphanumeric literal */
    CB_TAG_DECIMAL,         /* 8 decimal number */
    CB_TAG_FIELD,           /* 9 user-defined variable */
    CB_TAG_FILE,            /* 10 file description */
    /* expressions */
    CB_TAG_REFERENCE,       /* 11 reference to a field, file, or label */
    CB_TAG_BINARY_OP,       /* 12 binary operation */
    CB_TAG_FUNCALL,         /* 13 run-time function call */
    CB_TAG_CAST,            /* 14 type cast */
    CB_TAG_INTRINSIC,       /* 15 intrinsic function */
    /* statements */
    CB_TAG_LABEL,           /* 16 label statement */
    CB_TAG_ASSIGN,          /* 17 assignment statement */
    CB_TAG_INITIALIZE,      /* 18 INITIALIZE statement */
    CB_TAG_SEARCH,          /* 19 SEARCH statement */
    CB_TAG_CALL,            /* 20 CALL statement */
    CB_TAG_GOTO,            /* 21 GO TO statement */
    CB_TAG_IF,              /* 22 IF statement */
    CB_TAG_PERFORM,         /* 23 PERFORM statement */
    CB_TAG_STATEMENT,       /* 24 general statement */
    CB_TAG_CONTINUE,        /* 25 CONTINUE statement */
    CB_TAG_READY,           /* 26 READY statement */
    CB_TAG_RESET,           /* 27 RESET statement */
    /* miscellaneous */
    CB_TAG_PERFORM_VARYING, /* 28 PERFORM VARYING parameter */
    CB_TAG_PICTURE,         /* 29 PICTURE clause */
    CB_TAG_LIST,            /* 30 list */
    CB_TAG_PRAGMA,          /* 31 PRAGMA */ /*CIT*/
    CB_TAG_FIXVALUE,        /* 32 Fix Space to Zero CIT*/
    CB_TAG_INLINE,          /* 33 Inline C  CIT*/
    CB_TAG_OBJECT_LIST,     /* 34 List of Object ref*/
    CB_TAG_ENTRY            /* 35 Entry definition*/
};

enum cb_alphabet_name_type {
    CB_ALPHABET_NATIVE,
    CB_ALPHABET_STANDARD_1,
    CB_ALPHABET_STANDARD_2,
    CB_ALPHABET_EBCDIC,
    CB_ALPHABET_CUSTOM
};

enum cb_system_name_category {
    CB_CALL_CONVENTION_NAME,
    CB_CODE_NAME,
    CB_COMPUTER_NAME,
    CB_DEVICE_NAME,
    CB_ENTRY_CONVENTION_NAME,
    CB_EXTERNAL_LOCALE_NAME,
    CB_FEATURE_NAME,
    CB_LIBRARY_NAME,
    CB_SWITCH_NAME,
    CB_TEXT_NAME
};

enum cb_feature_name {
    CB_FEATURE_FORMFEED,
    CB_FEATURE_C01,
    CB_FEATURE_C02,
    CB_FEATURE_C03,
    CB_FEATURE_C04,
    CB_FEATURE_C05,
    CB_FEATURE_C06,
    CB_FEATURE_C07,
    CB_FEATURE_C08,
    CB_FEATURE_C09,
    CB_FEATURE_C10,
    CB_FEATURE_C11,
    CB_FEATURE_C12,
    CB_FEATURE_CSP
};

enum cb_switch_name {
    CB_SWITCH_1,
    CB_SWITCH_2,
    CB_SWITCH_3,
    CB_SWITCH_4,
    CB_SWITCH_5,
    CB_SWITCH_6,
    CB_SWITCH_7,
    CB_SWITCH_8,
    CB_SWITCH_9,
    CB_SWITCH_10,
    CB_SWITCH_11,
    CB_SWITCH_12,
    CB_SWITCH_13,
    CB_SWITCH_14,
    CB_SWITCH_15,
    CB_SWITCH_16,
    CB_SWITCH_17,
    CB_SWITCH_18,
    CB_SWITCH_19,
    CB_SWITCH_20,
    CB_SWITCH_21,
    CB_SWITCH_22,
    CB_SWITCH_23,
    CB_SWITCH_24,
    CB_SWITCH_25,
    CB_SWITCH_26,
    CB_SWITCH_27,
    CB_SWITCH_28,
    CB_SWITCH_29,
    CB_SWITCH_30,
    CB_SWITCH_31,
    CB_SWITCH_32,
    CB_SWITCH_33,
    CB_SWITCH_34,
    CB_SWITCH_35,
    CB_SWITCH_36,
    CB_SWITCH_37
};

enum cb_class {
    CB_CLASS_UNKNOWN,               /* 0 */
    CB_CLASS_ALPHABETIC,            /* 1 */
    CB_CLASS_ALPHANUMERIC,          /* 2 */
    CB_CLASS_BOOLEAN,               /* 3 */
    CB_CLASS_INDEX,                 /* 4 */
    CB_CLASS_NATIONAL,              /* 5 */
    CB_CLASS_NUMERIC,               /* 6 */
    CB_CLASS_OBJECT,                /* 7 */
    CB_CLASS_POINTER                /* 8 */
};

enum cb_category {
    CB_CATEGORY_UNKNOWN,                    /* 0 */
    CB_CATEGORY_ALPHABETIC,                 /* 1 */
    CB_CATEGORY_ALPHANUMERIC,               /* 2 */
    CB_CATEGORY_ALPHANUMERIC_EDITED,        /* 3 */
    CB_CATEGORY_BOOLEAN,                    /* 4 */
    CB_CATEGORY_INDEX,                      /* 5 */
    CB_CATEGORY_NATIONAL,                   /* 6 */
    CB_CATEGORY_NATIONAL_EDITED,            /* 7 */
    CB_CATEGORY_NUMERIC,                    /* 8 */
    CB_CATEGORY_NUMERIC_EDITED,             /* 9 */
    CB_CATEGORY_OBJECT_REFERENCE,           /* 10 */
    CB_CATEGORY_DATA_POINTER,               /* 11 */
    CB_CATEGORY_PROGRAM_POINTER,            /* 12 */
    CB_CATEGORY_BIT                         /* 13 */
};

enum cb_storage {
    CB_STORAGE_CONSTANT,            /* Constants */
    CB_STORAGE_FILE,                /* FILE SECTION */
    CB_STORAGE_WORKING,             /* WORKING-STORAGE SECTION */
    CB_STORAGE_LOCAL,               /* LOCAL-STORAGE SECTION */
    CB_STORAGE_LINKAGE,             /* LINKAGE SECTION */
    CB_STORAGE_SCREEN,              /* SCREEN SECTION */
    CB_STORAGE_REPORT,              /* REPORT SECTION */
    CB_STORAGE_COMMUNICATION,       /* COMMUNICATION SECTION */
    CB_STORAGE_TYPEDEF              /* is TYPEDEF */
};

enum cb_usage {
    CB_USAGE_BINARY,                /* 0 */
    CB_USAGE_BIT,                   /* 1 */
    CB_USAGE_COMP_5,                /* 2 */
    CB_USAGE_COMP_X,                /* 3 */
    CB_USAGE_DISPLAY,               /* 4 */
    CB_USAGE_FLOAT,                 /* 5 */
    CB_USAGE_DOUBLE,                /* 6 */
    CB_USAGE_INDEX,                 /* 7 */
    CB_USAGE_NATIONAL,              /* 8 */
    CB_USAGE_OBJECT,                /* 9 */
    CB_USAGE_PACKED,                /* 10 */
    CB_USAGE_POINTER,               /* 11 */
    CB_USAGE_PROGRAM,               /* 12 */
    CB_USAGE_LENGTH,                /* 13 */
    CB_USAGE_PROGRAM_POINTER,       /* 14 */
    CB_USAGE_UNSIGNED_CHAR,         /* 15 */
    CB_USAGE_SIGNED_CHAR,           /* 16 */
    CB_USAGE_UNSIGNED_SHORT,        /* 17 */
    CB_USAGE_SIGNED_SHORT,          /* 18 */
    CB_USAGE_UNSIGNED_INT,          /* 19 */
    CB_USAGE_SIGNED_INT,            /* 20 */
    CB_USAGE_UNSIGNED_LONG,         /* 21 */
    CB_USAGE_SIGNED_LONG,           /* 22 */
    CB_USAGE_COMP_6,                /* 23 */
    CB_USAGE_BYTE_ARRAY             /* 24 */
};

enum cb_operand_type {
    CB_SENDING_OPERAND,
    CB_RECEIVING_OPERAND
};

/*CIT*/
enum cb_constant_num_type {
    CB_CONST_STD,
    CB_CONST_SLL,
    CB_CONST_ULL,
    CB_CONST_FLOAT,
    CB_CONST_DOUBLE,
    CB_CONST_FLDADDR
};

enum cb_rec_mode_type {
    CB_REC_MODE_DEFAULT,
    CB_REC_MODE_VARIABLE,
    CB_REC_MODE_FIX
};

enum cb_object_list_type {
    CB_OBJECT_LIST_FIELD = 1
};

enum decimal_binary_type {
    none_DBT  = 0,
    int_DBT   = 1,
    uint_DBT  = 2,
    IEEE_float_DBT,
    IEEE_double_DBT
} ;

/*
 * Tree
 */

struct cb_tree_common {
    enum cb_tag             tag;
    enum cb_category        category;
    unsigned char           *source_file;
    int                     source_line;
    int                     source_column;
    int                     wipe_index_buffer;
    struct cb_tree_common  *target_ref_list;
    int                     skip_for_decimal:1;
    int                     modified:1;
    int                     generated:1;
    int                     need_cache_reload:1;
    struct object_attached                 *dst;
    struct object_attached                 *src;
};

typedef struct cb_tree_common   *cb_tree;

#define CB_TREE(x)              ((struct cb_tree_common *) (x))
#define CB_TREE_TAG(x)          (CB_TREE (x)->tag)
#define CB_TREE_CLASS(x)        cb_tree_class (CB_TREE (x))
#define CB_TREE_CATEGORY(x)     cb_tree_category (CB_TREE (x))

#ifdef  __GNUC__
    #define CB_TREE_CAST(tg,ty,x)                                           \
  ({                                                                    \
    cb_tree _x = (x);                                                   \
    if (!_x || CB_TREE_TAG (_x) != tg)                                  \
      {                                                                 \
        cobc_tree_cast_error (_x, __FILE__, __LINE__, tg);              \
      }                                                                 \
    ((ty *) (_x));                                                      \
  })
#else
    #define CB_TREE_CAST(tg,ty,x)   ((ty *) (x))
#endif

extern cb_tree          add_to_target_ref_list(cb_tree x, cb_tree r);
extern cb_tree          add_to_target_ref_list_decimal(cb_tree x, cb_tree r, int skip);
extern cb_tree          add_list_to_target_ref_list(cb_tree x, cb_tree l);

extern char             *cb_name (cb_tree x);
extern enum cb_class    cb_tree_class (cb_tree x);
extern enum cb_category cb_tree_category (cb_tree x);
extern int              cb_tree_type (cb_tree x);
extern int              cb_fits_one_positive_digit (cb_tree x) ;
extern int              cb_fits_int (cb_tree x);
extern int              cb_fits_long_long (cb_tree x);
extern int              cb_get_int (cb_tree x);
extern long long        cb_get_long_long (cb_tree x);
extern double           cb_get_double (cb_tree x) ;
extern int              cb_is_double (cb_tree x) ;
/*INDEX OPTIMIZE*/
extern struct index_optimizer *index_optimizer_cache;
extern struct index_optimizer_stack  *io_stack;


typedef struct decimal_optimizer{
    struct cb_field         *f;
    struct cb_decimal       *d;
    struct decimal_optimizer   *next;
    struct cb_literal       *l;
    cb_tree                x;

}decimal_optimizer;

typedef enum { IndexIOT =0, DecimalIOT = 1 } index_optimizer_type;
typedef struct index_optimizer {
    index_optimizer_type type;
    enum decimal_binary_type  binary_type;
    int index;
    struct cb_field *f;
    struct index_optimizer *next;
    cb_tree                 x;
    int modified;                       /*register has been modified in if else branch*/
    int nbr_in_one_funcall;
} index_optimizer;

typedef struct index_optimizer_stack{
    struct index_optimizer_stack *next;
    struct index_optimizer *list;
    int cpt;
} index_optimizer_stack;
typedef enum { DecimalType, FieldType } union_type;
typedef struct {
    int index;
    enum decimal_binary_type  binary_type;
    cb_tree x;
} union_val;

struct object_attached{
    union_type type;
    union_val val;
};


/*
 * Constants
 */

extern cb_tree  cb_any;
extern cb_tree  cb_positive;
extern cb_tree  cb_not_positive;
extern cb_tree  cb_negative;
extern cb_tree  cb_not_negative;
extern cb_tree  cb_numeric;
extern cb_tree  cb_not_numeric;
extern cb_tree  cb_alphabetic;
extern cb_tree  cb_not_alphabetic;
extern cb_tree  cb_alphabetic_lower;
extern cb_tree  cb_not_alphabetic_lower;
extern cb_tree  cb_alphabetic_upper;
extern cb_tree  cb_not_alphabetic_upper;
extern cb_tree  cb_true;
extern cb_tree  cb_false;
extern cb_tree  cb_null;
extern cb_tree  cb_zero;
extern cb_tree  cb_one;
extern cb_tree  cb_space;
extern cb_tree  cb_low;
extern cb_tree  cb_high;
extern cb_tree  cb_norm_low;
extern cb_tree  cb_norm_high;
extern cb_tree  cb_quote;
extern char     cb_quote_char;
extern cb_tree  cb_int0;
extern cb_tree  cb_int1;
extern cb_tree  cb_int2;
extern cb_tree  cb_int3;
extern cb_tree  cb_int4;
extern cb_tree  cb_int5;
extern cb_tree  cb_i[8];
extern cb_tree  cb_error_node;
extern cb_tree  cb_dirsep;

extern cb_tree  cb_intr_whencomp;
extern cb_tree  cb_intr_pi;
extern cb_tree  cb_intr_e;

extern cb_tree  cb_standard_error_handler;
extern cb_tree  cb_save_context_handler;
extern cb_tree  cb_save_context_handler_ref;

extern size_t   gen_screen_ptr;

struct cb_const {
    struct cb_tree_common       common;
    const char                  *val;
    enum cb_constant_num_type   num_type; /*CIT*/
};

#define CB_CONST(x)     (CB_TREE_CAST (CB_TAG_CONST, struct cb_const, x))
#define CB_CONST_P(x)   (CB_TREE_TAG (x) == CB_TAG_CONST)

extern void             cb_init_constants (void);
/*CIT*/
extern cb_tree          make_constant (enum cb_category category, const char *val);
extern cb_tree          make_constant_label (const char *name);
extern cb_tree          make_temp_label (void);



/*
 * Integer
 */

struct cb_integer {
    struct cb_tree_common   common;
    int                     val;
    int                     scale;
};

#define CB_INTEGER(x)   (CB_TREE_CAST (CB_TAG_INTEGER, struct cb_integer, x))
#define CB_INTEGER_P(x) (CB_TREE_TAG (x) == CB_TAG_INTEGER)

extern cb_tree          cb_int (int n);


/*
 * String
 */

struct cb_string {
    struct cb_tree_common   common;
    size_t                  size;
    const unsigned char     *data;
};

#define CB_STRING(x)    (CB_TREE_CAST (CB_TAG_STRING, struct cb_string, x))
#define CB_STRING_P(x)  (CB_TREE_TAG (x) == CB_TAG_STRING)

#define cb_build_string0(str) cb_build_string (str, strlen ((char *)str))

extern cb_tree          cb_build_string (const unsigned char *data, size_t size);


/*
 * Alphabet-name
 */

struct cb_alphabet_name {
    struct cb_tree_common           common;
    const char                      *name;
    char                            *cname;
    cb_tree                         custom_list;
    enum cb_alphabet_name_type      type;
    int                             low_val_char;
    int                             high_val_char;
};

#define CB_ALPHABET_NAME(x)     (CB_TREE_CAST (CB_TAG_ALPHABET_NAME, struct cb_alphabet_name, x))
#define CB_ALPHABET_NAME_P(x)   (CB_TREE_TAG (x) == CB_TAG_ALPHABET_NAME)

extern cb_tree          cb_build_alphabet_name (cb_tree name, enum cb_alphabet_name_type type);


/*
 * Class-name
 */

struct cb_class_name {
    struct cb_tree_common   common;
    const char              *name;
    char                    *cname;
    cb_tree                 list;
};

#define CB_CLASS_NAME(x)        (CB_TREE_CAST (CB_TAG_CLASS_NAME, struct cb_class_name, x))
#define CB_CLASS_NAME_P(x)      (CB_TREE_TAG (x) == CB_TAG_CLASS_NAME)

extern cb_tree          cb_build_class_name (cb_tree name, cb_tree list);


/*
 * Locale name
 */

struct cb_locale_name {
    struct cb_tree_common   common;
    const char              *name;
    char                    *cname;
    cb_tree                 list;
};

#define CB_LOCALE_NAME(x)       (CB_TREE_CAST (CB_TAG_LOCALE_NAME, struct cb_locale_name, x))
#define CB_LOCALE_NAME_P(x)     (CB_TREE_TAG (x) == CB_TAG_LOCALE_NAME)

extern cb_tree          cb_build_locale_name (cb_tree name, cb_tree list);

/*
 * System-name
 */

struct cb_system_name {
    struct cb_tree_common           common;
    enum cb_system_name_category    category;
    int                             token;
};

#define CB_SYSTEM_NAME(x)       (CB_TREE_CAST (CB_TAG_SYSTEM_NAME, struct cb_system_name, x))
#define CB_SYSTEM_NAME_P(x)     (CB_TREE_TAG (x) == CB_TAG_SYSTEM_NAME)

extern cb_tree          cb_build_system_name (enum cb_system_name_category category, int token);


/*
 * Literal
 */

struct cb_literal {
    struct cb_tree_common   common;
    size_t                  size;
    unsigned char           *data;
    unsigned char           *image;
    unsigned char           all;
    signed char             sign;   /* unsigned: 0 negative: -1 positive: 1 */
    signed char             scale;
    unsigned char           encoded;        /* coded as hexa or  A2E executed */
    unsigned char           scaled; 
};

#define CB_LITERAL(x)   (CB_TREE_CAST (CB_TAG_LITERAL, struct cb_literal, x))
#define CB_LITERAL_P(x) (CB_TREE_TAG (x) == CB_TAG_LITERAL)
#define CB_NUMERIC_LITERAL_P(x) \
  (CB_LITERAL_P (x) && CB_TREE_CATEGORY (x) == CB_CATEGORY_NUMERIC)
#define CB_HEXA_LITERAL_P(x) \
  (CB_LITERAL_P (x) && CB_LITERAL_P (x)->encoded)

#define CB_ALPHANUMERIC_LITERAL_P(x) \
  (CB_LITERAL_P (x) && CB_TREE_CATEGORY (x) == CB_CATEGORY_ALPHANUMERIC)

#define CB_LITERAL_ENCODED(x)   (CB_TREE_CAST (CB_TAG_LITERAL, struct cb_literal, cb_a2e_literal(x)))

extern cb_tree         cb_build_numeric_literal (int sign, const unsigned char *data, int scale, const char *image);
extern cb_tree         cb_build_alphanumeric_literal (const unsigned char *data, size_t size, int national);
extern cb_tree         cb_build_national_literal (const unsigned char *data, size_t size);
extern cb_tree         cb_concat_literals (cb_tree x1, cb_tree x2);
extern cb_tree         cb_a2e_literal (cb_tree x1);


/*
 * Decimal
 */

struct cb_decimal {
    struct cb_tree_common       common;
    int                         id;
    enum decimal_binary_type    binary;  /*CIT*/
    int                         intermediate;
};

#define CB_DECIMAL(x)   (CB_TREE_CAST (CB_TAG_DECIMAL, struct cb_decimal, x))
#define CB_DECIMAL_P(x) (CB_TREE_TAG (x) == CB_TAG_DECIMAL)

extern cb_tree          cb_build_decimal (int id);


/*
 * Picture
 */

struct cb_picture {
    struct cb_tree_common   common;
    int                     size;           /* byte size */
    int                     lenstr;         /* length of picture string */
    char                    *orig;          /* original picture string */
    char                    *str;           /* packed picture string */
    enum cb_category        category;       /* field category */
    unsigned char           digits;         /* the number of digit places */
    signed char             scale;          /* 1/10^scale */
    unsigned char           have_sign;      /* have 'S' */
    unsigned char           spare;          /* spare */
};

#define CB_PICTURE(x)   (CB_TREE_CAST (CB_TAG_PICTURE, struct cb_picture, x))
#define CB_PICTURE_P(x) (CB_TREE_TAG (x) == CB_TAG_PICTURE)

extern cb_tree          cb_build_picture (const char *str);


/*
 * Field
 */

struct cb_field {
    struct cb_tree_common   common;
    int                     id;             /* field id */
    int                     storage_id;     /* storage id */
    const char              *name;          /* the original name */
    const char              *ename;         /* the externalized name */
    int                     size;           /* field size */
    int                     redefine_size;  /* max field size of redefining field */
    int                     memory_size;    /* memory size (Level 01 Only)*/
    int                     offset;         /* byte offset from top (01 field) */
    int                     level;          /* level number */
    int                     occurs_min;     /* OCCURS <max> */
    int                     occurs_max;     /* or OCCURS <min> TO <max> */
    int                     indexes;        /* number of parents who have OCCURS */
    int                     count;          /* reference count */
    cb_tree                 occurs_depending;       /* OCCURS ... DEPENDING ON */
    enum cb_storage         storage;
    enum cb_usage           usage;          /* USAGE */
    enum cb_usage           original_usage; /* USAGE before optimizing*/

    cb_tree                 values;         /* VALUE */
    cb_tree                 false_88;       /* 88 FALSE clause */
    cb_tree                 index_list;     /* INDEXED BY */
    struct cb_field         *parent;        /* upper level field (NULL for 01 fields) */
    struct cb_field         *children;      /* top of lower level fields */
    struct cb_field         *level88_children;/* top of lower level 88 fields */
    struct cb_field         *bitfield_children;  /* BIT field of this field  */
    unsigned int            bitoffset;      /* Bitfield offset*/
    struct cb_field         *sister;        /* fields in the same level (left)*/
    struct cb_field         *brother;       /* fields in the same level (right)*/
    struct cb_field         *redefines;     /* REDEFINES */
    struct cb_field         *like;          /* LIKE */
    struct cb_field         *shadow;        /* EXHIBIT CHANGED store */
    struct cb_field         *rename_thru;   /* RENAMES THRU */
    struct cb_field         *index_qual;    /* INDEXED BY qualifier */
    struct cb_file          *file;          /* file name associated in FD section */
    struct cb_key {
        int             dir;                /* ASCENDING or DESCENDING */
        cb_tree         key;                /* KEY */
        cb_tree         ref;                /* reference used in SEARCH ALL */
        cb_tree         val;                /* value to be compared in SEARCH ALL */
    } *keys;
    int                     nkeys;          /* the number of keys */
    int                     param_num;      /* CHAINING param number */
    struct cb_picture       *pic;           /* PICTURE */
    /* screen parameters */
    cb_tree                 screen_line;
    cb_tree                 screen_column;
    cb_tree                 screen_from;
    cb_tree                 screen_to;
    cb_tree                 screen_foreg;
    cb_tree                 screen_backg;
    int                     screen_flag;    /* flags used in SCREEN SECTION */
    int                     screen_prompt;  
    cb_tree                 screen_control; 
    cb_tree                 screen_input_size;
    /* flags */
    unsigned int flag_external      : 1;    /* EXTERNAL */
    unsigned int flag_blank_zero    : 1;    /* BLANK WHEN ZERO */
    unsigned int flag_justified     : 1;    /* JUSTIFIED RIGHT */
    unsigned int flag_sign_leading  : 1;    /* SIGN IS LEADING */
    unsigned int flag_sign_separate : 1;    /* SIGN IS SEPARATE */
    unsigned int flag_synchronized  : 1;    /* SYNCHRONIZED */
    unsigned int flag_occurs        : 1;    /* OCCURS */
    unsigned int flag_invalid       : 1;    /* is broken */
    unsigned int flag_binary_swap   : 1;    /* binary byteswap */
    unsigned int flag_local         : 1;    /* has local scope */
    unsigned int flag_base          : 1;    /* has memory allocation */
    unsigned int flag_field         : 1;    /* has been internally cached */
    unsigned int flag_item_external : 1;    /* is EXTERNAL */
    unsigned int flag_chained       : 1;    /* CHAINING item */
    unsigned int flag_real_binary   : 1;    /* is BINARY-CHAR/SHORT/LONG/DOUBLE */
    unsigned int flag_item_based    : 1;    /* is BASED */
    unsigned int flag_item_78       : 1;    /* is 78 level */
    unsigned int flag_any_length    : 1;    /* is ANY LENGTH */
    unsigned int flag_anylen_done   : 1;    /* ANY LENGTH is set up */
    unsigned int flag_indexed_by    : 1;    /* INDEXED BY item */
    unsigned int flag_is_pointer    : 1;    /* is POINTER */
    unsigned int flag_is_verified   : 1;    /* has been verified */
    unsigned int flag_is_global     : 1;    /* is GLOBAL */
    unsigned int flag_is_c_long     : 1;    /* is BINARY-C-LONG */
    unsigned int flag_is_pdiv_parm  : 1;    /* is PROC DIV USING */
    unsigned int flag_local_alloced : 1;    /* LOCAL storage is allocated */
    unsigned int flag_no_init       : 1;    /* no initialize unless used */
    unsigned int flag_unused_linkage: 1;    /* Unused likage level 01 */
    unsigned int flag_linkage_allocated: 1; /* linkage level 01 allocated */
    unsigned int flag_usage_optimized: 1;   /* usage has been optimized */
    unsigned int flag_redefined     : 1;    /* field is redefined taget */
    unsigned int flag_binary_pack   : 1;    /* binary pack byte (COMP-4) */
    unsigned int flag_debug_dumped  : 1;    /* for debug field generation */
    unsigned int flag_debug_cased   : 1;    /* for debug field generation */
    unsigned int flag_is_typedef    : 1;    /* is TYPEDEF */
    unsigned int flag_is_fcd_reg    : 1;    /* is FH--FCD */
    unsigned int flag_is_typedef_global:1;
    unsigned int flag_init_gen      : 1;        
    unsigned int flag_no_optimizable : 1;        
    unsigned int flag_usage_set      : 1;        
    unsigned int flag_hide_from_debug: 1;        
    unsigned int flag_dynamic_data   : 1;   /* Is internal use register */
    unsigned int flag_hide_from_list : 1;   /* hide from listing */
    unsigned int flag_ebcdic_charset : 1;
    unsigned int flag_compx_notrunc  : 1;
    unsigned int flag_disable_usused_skip : 1;   /* do not skip when check unused */
    struct {
        int                     cb_field_variable_size_set;   
        struct cb_field         *cb_field_variable_size;   
        int                     cb_field_variable_address_set;
        struct cb_field         *cb_field_variable_address;
    } cache;
    int                         set_action_code; /* action code is set*/
    int                         typedef_id;      /* field id in typedef*/
    cb_tree                     screen_subs;
    int                         debugdb_idx;
    int                         debugdb_groupid;
    int                         local_offset;    /* offset in local storage*/
    int                     is_odo_idx;     /* controls occurrences of other fields */
    int                     odo_idx_min;    /* minimum value */
    int                     odo_idx_max;    /* maximum value */
};

#define CB_FIELD(x)             (CB_TREE_CAST (CB_TAG_FIELD, struct cb_field, x))
#define CB_FIELD_P(x)           (CB_TREE_TAG (x) == CB_TAG_FIELD)

extern cb_tree          cb_build_field (cb_tree name);
extern cb_tree          cb_build_implicit_field (cb_tree name, int len, int external);
extern cb_tree          cb_build_constant (cb_tree name, cb_tree value);
extern void             cb_mark_as_non_skipable(cb_tree x);

extern struct cb_field  *cb_type (cb_tree x) ;
extern struct cb_field  *cb_field (cb_tree x);
extern struct cb_field  *cb_field_add (struct cb_field *f, struct cb_field *p);
extern int              cb_field_size (cb_tree x);
extern struct cb_field  *cb_field_founder (struct cb_field *f);
extern struct cb_field  *cb_field_variable_size (struct cb_field *f);
extern struct cb_field  *cb_field_variable_address (struct cb_field *f);
extern int              cb_field_subordinate (struct cb_field *p, struct cb_field *f);
extern void             cb_set_field_variable_address_cache (struct cb_field *f) ;

#define CB_REF_OR_FIELD_P(x)    ((CB_FIELD_P (x) || CB_REFERENCE_P (x)))

/* Index */

#define CB_INDEX_P(x)           ((CB_FIELD_P (x) || CB_REFERENCE_P (x)) \
   && cb_field (x)->usage == CB_USAGE_INDEX)


/*
 * Label
 */

struct cb_label {
    struct cb_tree_common   common;
    const unsigned char     *name;
    struct cb_label         *section;
    cb_tree                 exit_label;
    cb_tree                 exit_label_ref;
    cb_tree                 children;
    const unsigned char     *orig_name;
    int                     id;
    int                     is_section;
    int                     is_entry;
    int                     is_module_entry;
    unsigned char           need_begin;
    unsigned char           need_return;
    unsigned char           is_global;
    unsigned char           filler;
    unsigned char           is_exception;
    unsigned int            entry_call_convesion;
};

#define CB_LABEL(x)             (CB_TREE_CAST (CB_TAG_LABEL, struct cb_label, x))
#define CB_LABEL_P(x)           (CB_TREE_TAG (x) == CB_TAG_LABEL)

extern cb_tree          cb_build_label (cb_tree name, struct cb_label *section);
extern cb_tree          cb_build_paragraph (cb_tree name, int exitword);

struct handler_struct {
    struct cb_label     *handler_label;
    struct cb_program   *handler_prog;
};

/*
 * File
 */

struct cb_alt_key {
    struct cb_alt_key       *next;
    cb_tree                 key;
    int                     duplicates;
    char                    key_compress;
    int                     offset;
};

struct cb_file {
    struct cb_tree_common   common;
    const char              *oname;                 /* The original name */
    const char              *name;                  /* The tree name 'file_'name*/
    char                    *cname;                 /* The name used in C */
    /* SELECT */
    cb_tree                 assign;                 /* ASSIGN */
    cb_tree                 file_status;            /* FILE STATUS */
    cb_tree                 sharing;                /* SHARING */
    cb_tree                 key;                    /* RELATIVE/RECORD KEY */
    int                     key_duplicates;         /* PRIMARY KEY WITH DUPS */
    struct cb_alt_key       *alt_key_list;          /* ALTERNATE RECORD KEY */
    /* FD/SD */
    struct cb_field         *record;                /* Record descriptor */
    cb_tree                 record_depending;       /* RECORD DEPENDING */
    cb_tree                 linage;                 /* LINAGE */
    cb_tree                 linage_ctr;             /* LINAGE COUNTER */
    cb_tree                 latfoot;                /* LINAGE FOOTING */
    cb_tree                 lattop;                 /* LINAGE TOP */
    cb_tree                 latbot;                 /* LINAGE BOTTOM */
    struct cb_label         *handler;               /* Error handler */
    struct cb_program       *handler_prog;          /* Prog where defined */
    int                     record_min;             /* RECORD CONTAINS */
    int                     record_max;             /* RECORD CONTAINS */
    int                     optional;               /* OPTIONAL */
    int                     organization;           /* ORGANIZATION */
    int                     access_mode;            /* ACCESS MODE */
    int                     lock_mode;              /* LOCK MODE */
    int                     same_clause;            /* SAME clause */
    int                     finalized;              /* Is finalized */
    int                     external;               /* Is EXTERNAL */
    int                     special;                /* Special file */
    int                     external_assign;        /* ASSIGN EXTERNAL */
    int                     dynanic_assign;         /* ASSIGN DYNAMIC */
    int                     fileid_assign;          /* ASSIGN DISK */
    int                     global;                 /* Is GLOBAL */

    /*CIT*/
    enum cb_rec_mode_type   rec_mode;               /* RECORDING MODE is .. (Source)*/
    int                     line_advancing_clause;  /* LINE ADVANCING */
    char                    data_compress;
    char                    prim_key_compress;
    char                    ls_utf16;
    char                    ls_mfmode;
    char                    ls_notrunc;
    char                    ls_expandtab;
    char                    auto_close;
    char                    is_printer;             /* IS PRINTER*/
    char                    with_rollback;
    char                    record_contain_clause;
    cb_tree                 fcd_reg;                /* FH--FCD pseudo field */
    struct cb_field        *debug_file_status;      /* Status if no FILE STATUS */
};

#define CB_FILE(x)      (CB_TREE_CAST (CB_TAG_FILE, struct cb_file, x))
#define CB_FILE_P(x)    (CB_TREE_TAG (x) == CB_TAG_FILE)

extern struct cb_file   *build_file (cb_tree name);
extern void             validate_file (struct cb_file *f, cb_tree name);
extern void             finalize_file (struct cb_file *f, struct cb_field *records);
extern void             finalize_section_files (cb_tree file_rec_list) ;
extern struct cb_file   *build_sysfile (struct cb_program *prog, char *filename, const char *file, int input);

extern int              is_base_alligned (cb_tree x);
extern int              is_identical_or_ovelap_fields(cb_tree x, cb_tree y);
extern int              is_same_field(cb_tree x, cb_tree y);
extern int              derived_from_same_base_root(struct cb_field *f1, struct cb_field *f2);
extern int              is_overlapped(struct cb_field *f1, struct cb_field *f2);
extern int              same_index_from_table_occurs(cb_tree x, cb_tree y);
extern void             delete_list(void);
/*
 * Reference
 */

#define CB_WORD_HASH_SIZE       2011 /*was 133*/

struct cb_word {
    struct cb_word  *next;      /* next word with the same hash value */
    const char      *name;      /* word name */
    cb_tree         items;      /* objects associated with this word */
    int             count;      /* number of words with the same name */
    int             error;      /* set to 1 if error displayed */
};

struct cb_reference {
    struct cb_tree_common   common;
    struct cb_word          *word;
    enum cb_operand_type    type;
    cb_tree                 value;          /* item referred by this reference */
    cb_tree                 subs;           /* the list of subscripts */
    cb_tree                 offset;         /* 1st operand of reference modification */
    cb_tree                 length;         /* 2nd operand of reference modification */
    cb_tree                 check;
    cb_tree                 chain;          /* next qualified name */
    int                     all;
    int                     cb_ref_optinal:1; /*this reference is optional (only warning generated if not found*/
    int                     cb_all_occurs:1; /*Referece this with all varying occurs as maximum*/
};

#define CB_REFERENCE(x)         (CB_TREE_CAST (CB_TAG_REFERENCE, struct cb_reference, x))
#define CB_REFERENCE_P(x)       (CB_TREE_TAG (x) == CB_TAG_REFERENCE)

#define CB_NAME(x)              (CB_REFERENCE (x)->word->name)

extern cb_tree          cb_build_filler (void);
extern cb_tree          cb_build_bitfield_filler (void) ;
extern cb_tree          cb_build_reference (const char *name);
extern cb_tree          cb_build_file_reference (const char *name) ;
extern cb_tree          cb_build_label_reference (cb_tree word) ;
extern cb_tree          cb_build_field_reference (struct cb_field *f, cb_tree ref);
extern const char       *cb_define (cb_tree name, cb_tree val);
extern void             cb_define_system_name (const char *name);
extern cb_tree          cb_dup_reference (cb_tree  x);
extern cb_tree          cb_ref (cb_tree x);
extern cb_tree          cb_ref_or_type (cb_tree x);
extern cb_tree          cb_global_ref (cb_tree x);

/*
 * Binary operation
 */

/*
  '+'   x + y
  '-'   x - y
  '*'   x * y
  '/'   x / y
  '^'   x ** y
  '='   x = y
  '>'   x > y
  '<'   x < y
  '['   x <= y
  ']'   x >= y
  '~'   x != y
  '!'   not x
  '&'   x and y
  '|'   x or y
  '@'   ( x )
*/

struct cb_binary_op {
    struct cb_tree_common   common;
    int                     op;
    cb_tree                 x;
    cb_tree                 y;
    int                     rawcondition:1;
};

#define CB_BINARY_OP(x)         (CB_TREE_CAST (CB_TAG_BINARY_OP, struct cb_binary_op, x))
#define CB_BINARY_OP_P(x)       (CB_TREE_TAG (x) == CB_TAG_BINARY_OP)

#define cb_build_parenthesis(x) cb_build_binary_op (x, '@', NULL)
#define cb_build_negation(x)    cb_build_binary_op (x, '!', NULL)
#define cb_build_value(x)       cb_build_binary_op (x, 'v', NULL)
#define cb_build_not_value(x)   cb_build_binary_op (x, 'n', cb_get_bits_size(x))
#define cb_build_sign_value(x)   cb_build_binary_op (x, 's', NULL)

extern cb_tree          cb_get_bits_size (cb_tree x);
extern cb_tree          cb_build_binary_op (cb_tree x, int op, cb_tree y);
extern cb_tree          cb_build_cond_binary_op (cb_tree x, int op, cb_tree y,enum cb_category category);
extern cb_tree          cb_build_binary_list (cb_tree l, int op);


/*
 * Function call
 */

struct cb_funcall {
    struct cb_tree_common   common;
    const char              *name;
    cb_tree                 argv[10];
    int                     argc;
    int                     varcnt;
    size_t                  screenptr;
    int                     not_on_exception;
};

#define CB_FUNCALL(x)           (CB_TREE_CAST (CB_TAG_FUNCALL, struct cb_funcall, x))
#define CB_FUNCALL_P(x)         (CB_TREE_TAG (x) == CB_TAG_FUNCALL)

extern cb_tree          cb_build_funcall (const char *name, int argc, cb_tree a1, cb_tree a2,
                                          cb_tree a3, cb_tree a4, cb_tree a5, cb_tree a6,
                                          cb_tree a7, cb_tree a8, cb_tree a9, cb_tree a10);

#define cb_build_funcall_0(f)               \
    cb_build_funcall(f, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define cb_build_funcall_1(f,a1)            \
    cb_build_funcall(f, 1, a1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define cb_build_funcall_2(f,a1,a2)         \
    cb_build_funcall(f, 2, a1, a2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define cb_build_funcall_3(f,a1,a2,a3)          \
    cb_build_funcall(f, 3, a1, a2, a3, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define cb_build_funcall_4(f,a1,a2,a3,a4)       \
    cb_build_funcall(f, 4, a1, a2, a3, a4, NULL, NULL, NULL, NULL, NULL, NULL)
#define cb_build_funcall_5(f,a1,a2,a3,a4,a5)        \
    cb_build_funcall(f, 5, a1, a2, a3, a4, a5, NULL, NULL, NULL, NULL, NULL)
#define cb_build_funcall_6(f,a1,a2,a3,a4,a5,a6)     \
    cb_build_funcall(f, 6, a1, a2, a3, a4, a5, a6, NULL, NULL, NULL, NULL)
#define cb_build_funcall_7(f,a1,a2,a3,a4,a5,a6,a7)  \
    cb_build_funcall(f, 7, a1, a2, a3, a4, a5, a6, a7, NULL, NULL, NULL)
#define cb_build_funcall_8(f,a1,a2,a3,a4,a5,a6,a7,a8)  \
    cb_build_funcall(f, 8, a1, a2, a3, a4, a5, a6, a7, a8, NULL, NULL)
#define cb_build_funcall_9(f,a1,a2,a3,a4,a5,a6,a7,a8,a9)  \
    cb_build_funcall(f, 9, a1, a2, a3, a4, a5, a6, a7, a8, a9, NULL)
#define cb_build_funcall_10(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)  \
    cb_build_funcall(f, 10, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)


/*
 * Type cast
 */

enum cb_cast_type {
    CB_CAST_INTEGER,
    CB_CAST_DOUBLE, 
    CB_CAST_ADDRESS,
    CB_CAST_ADDR_OF_ADDR,
    CB_CAST_LENGTH,
    CB_CAST_PROGRAM_POINTER
};

struct cb_cast {
    struct cb_tree_common   common;
    enum cb_cast_type       type;
    cb_tree                 val;
};

#define CB_CAST(x)      (CB_TREE_CAST (CB_TAG_CAST, struct cb_cast, x))
#define CB_CAST_P(x)    (CB_TREE_TAG (x) == CB_TAG_CAST)

extern cb_tree          cb_build_cast (enum cb_cast_type type, cb_tree val);

#define cb_build_cast_integer(x)        cb_build_cast (CB_CAST_INTEGER, x)
#define cb_build_cast_double(x)         cb_build_cast (CB_CAST_DOUBLE, x)
#define cb_build_cast_address(x)        cb_build_cast (CB_CAST_ADDRESS, x)
#define cb_build_cast_addr_of_addr(x)   cb_build_cast (CB_CAST_ADDR_OF_ADDR, x)
#define cb_build_cast_length(x)         cb_build_cast (CB_CAST_LENGTH, x)
#define cb_build_cast_ppointer(x)       cb_build_cast (CB_CAST_PROGRAM_POINTER, x)


/*
 * Assign
 */

struct cb_assign {
    struct cb_tree_common   common;
    cb_tree                 var;
    cb_tree                 val;
};

#define CB_ASSIGN(x)            (CB_TREE_CAST (CB_TAG_ASSIGN, struct cb_assign, x))
#define CB_ASSIGN_P(x)          (CB_TREE_TAG (x) == CB_TAG_ASSIGN)

extern cb_tree          cb_build_assign (cb_tree var, cb_tree val);


/*
 * Intrinsic FUNCTION
 */

enum cb_intr_enum {
    CB_INTR_ABS = 1,
    CB_INTR_ACOS,
    CB_INTR_ANNUITY,
    CB_INTR_ASIN,
    CB_INTR_ATAN,
    CB_INTR_BOOLEAN_OF_INTEGER,
    CB_INTR_BYTE_LENGTH,
    CB_INTR_CHAR,
    CB_INTR_CHAR_NATIONAL,
    CB_INTR_COMBINED_DATETIME,
    CB_INTR_CONCATENATE,
    CB_INTR_COS,
    CB_INTR_CURRENT_DATE,
    CB_INTR_CSTRING,    
    CB_INTR_DATE_OF_INTEGER,
    CB_INTR_DATE_TO_YYYYMMDD,
    CB_INTR_DAY_OF_INTEGER,
    CB_INTR_DAY_TO_YYYYDDD,
    CB_INTR_DISPLAY_OF,
    CB_INTR_E,
    CB_INTR_EXCEPTION_FILE,
    CB_INTR_EXCEPTION_FILE_N,
    CB_INTR_EXCEPTION_LOCATION,
    CB_INTR_EXCEPTION_LOCATION_N,
    CB_INTR_EXCEPTION_STATEMENT,
    CB_INTR_EXCEPTION_STATUS,
    CB_INTR_EXP,
    CB_INTR_EXP10,
    CB_INTR_FACTORIAL,
    CB_INTR_FRACTION_PART,
    CB_INTR_HIGHEST_ALGEBRAIC,
    CB_INTR_INTEGER,
    CB_INTR_INTEGER_OF_BOOLEAN,
    CB_INTR_INTEGER_OF_DATE,
    CB_INTR_INTEGER_OF_DAY,
    CB_INTR_INTEGER_PART,
    CB_INTR_LENGTH,
    CB_INTR_LOCALE_COMPARE,
    CB_INTR_LOCALE_DATE,
    CB_INTR_LOCALE_TIME,
    CB_INTR_LOCALE_TIME_FROM_SECS,
    CB_INTR_LOG,
    CB_INTR_LOG10,
    CB_INTR_LOWER_CASE,
    CB_INTR_LOWEST_ALGEBRAIC,
    CB_INTR_MAX,
    CB_INTR_MEAN,
    CB_INTR_MEDIAN,
    CB_INTR_MIDRANGE,
    CB_INTR_MIN,
    CB_INTR_MOD,
    CB_INTR_NATIONAL_OF,
    CB_INTR_NUMVAL,
    CB_INTR_NUMVAL_C,
    CB_INTR_NUMVAL_F,
    CB_INTR_ORD,
    CB_INTR_ORD_MAX,
    CB_INTR_ORD_MIN,
    CB_INTR_PI,
    CB_INTR_PRESENT_VALUE,
    CB_INTR_RANDOM,
    CB_INTR_RANGE,
    CB_INTR_REM,
    CB_INTR_REVERSE,
    CB_INTR_SECONDS_FROM_FORMATTED_TIME,
    CB_INTR_SECONDS_PAST_MIDNIGHT,
    CB_INTR_SIGN,
    CB_INTR_SIN,
    CB_INTR_SQRT,
    CB_INTR_STANDARD_COMPARE,
    CB_INTR_STANDARD_DEVIATION,
    CB_INTR_STORED_CHAR_LENGTH,
    CB_INTR_SUBSTITUTE,
    CB_INTR_SUBSTITUTE_CASE,
    CB_INTR_SUM,
    CB_INTR_TAN,
    CB_INTR_TEST_DATE_YYYYMMDD,
    CB_INTR_TEST_DAY_YYYYDDD,
    CB_INTR_TEST_NUMVAL,
    CB_INTR_TEST_NUMVAL_C,
    CB_INTR_TEST_NUMVAL_F,
    CB_INTR_TRIM,
    CB_INTR_UPPER_CASE,
    CB_INTR_VARIANCE,
    CB_INTR_WHEN_COMPILED,
    CB_INTR_YEAR_TO_YYYY,
    CB_INTR_TRIMR,
    CB_INTR_TRIML,
    CB_INTR_TO_UTF8,
    CB_INTR_FROM_UTF8,
    CB_INTR_USER_DEFINE
};

struct cb_intrinsic_table {
    const char              *name;          /* FUNCTION NAME */
    int                     args;           /* 0-n, negative = variable */
    int                     implemented;    /* Have we implemented it? */
    enum cb_intr_enum       intr_enum;      /* Enum intrinsic */
    const char              *intr_routine;  /* Routine name */
    enum cb_category        category;       /* Category */
    unsigned int            refmod;         /* Can be refmodded */
    struct cb_intrinsic_table *next;        /* For user List*/
};

struct cb_intrinsic {
    struct cb_tree_common           common;
    cb_tree                         name;
    cb_tree                         args;
    cb_tree                         intr_field;     /* Field to use */
    struct cb_intrinsic_table       *intr_tab;
    cb_tree                         offset;
    cb_tree                         length;
};

#define CB_INTRINSIC(x)         (CB_TREE_CAST (CB_TAG_INTRINSIC, struct cb_intrinsic, x))
#define CB_INTRINSIC_P(x)       (CB_TREE_TAG (x) == CB_TAG_INTRINSIC)

extern struct cb_intrinsic_table    *lookup_intrinsic (const char *name,
                                                       const int checkres);
extern cb_tree              cb_build_intrinsic (cb_tree name,
                                                cb_tree args,
                                                cb_tree refmod);
extern cb_tree          cb_build_any_intrinsic (cb_tree args);
extern cb_tree          cb_build_cstring_intrinsic (cb_tree x);


/*
 * INITIALIZE
 */

struct cb_initialize {
    struct cb_tree_common   common;
    cb_tree                 var;
    cb_tree                 val;
    cb_tree                 rep;
    cb_tree                 def;
    int                     flag_statement;
    int                     flag_filldefaultbyte;
    int                     flag_fillFiller;
    int                     flag_care_varing;
};

#define CB_INITIALIZE(x)        (CB_TREE_CAST (CB_TAG_INITIALIZE, struct cb_initialize, x))
#define CB_INITIALIZE_P(x)      (CB_TREE_TAG (x) == CB_TAG_INITIALIZE)

extern cb_tree          cb_build_initialize (cb_tree var, cb_tree val, cb_tree rep, cb_tree def, 
                                             int flag_statement, int flagdefault, int flag_fillFiller, 
                                             int care_varying);

/*
 * FIXVALUE
 */

struct cb_fixvalue {
    struct cb_tree_common   common;
    cb_tree                 var;
    int                     flag_spzero;
};

#define CB_FIXVALUE(x)        (CB_TREE_CAST (CB_TAG_FIXVALUE, struct cb_fixvalue, x))
#define CB_FIXVALUE_P(x)      (CB_TREE_TAG (x) == CB_TAG_FIXVALUE)

extern cb_tree          cb_build_fixvalue (cb_tree var, int flag);

/*
 * SEARCH
 */

struct cb_search {
    struct cb_tree_common   common;
    int                     flag_all;
    cb_tree                 table;
    cb_tree                 var;
    cb_tree                 end_stmt;
    cb_tree                 whens;
};

#define CB_SEARCH(x)            (CB_TREE_CAST (CB_TAG_SEARCH, struct cb_search, x))
#define CB_SEARCH_P(x)          (CB_TREE_TAG (x) == CB_TAG_SEARCH)

extern cb_tree          cb_build_search (int flag_all, cb_tree table, cb_tree var, cb_tree end_stmt, cb_tree whens);


/*
 * CALL
 */

#define CB_CALL_BY_DEFAULT      0x00
#define CB_CALL_BY_REFERENCE    0x01
#define CB_CALL_BY_CONTENT      0x02
#define CB_CALL_BY_VALUE        0x03
#define CB_CALL_BY_DESCRIPTOR   0x04
#define CB_CALL_BY_MASK         0x0F
#define CB_CALL_BY_PROTOTYPED  0x10
#define CB_CALL_BY(x)  (x & CB_CALL_BY_MASK)

#define CB_CALL_DYNAMIC        0x0000 
#define CB_CALL_STATIC         0x0001 
#define CB_CALL_CDECL          0x0000
#define CB_CALL_STDCALL        0x0010
#define CB_CALL_RETURN_INT     0x0000
#define CB_CALL_RETURN_PTR     0x0100
#define CB_CALL_RETURN_ADDROF  0x0200
#define CB_CALL_RETURN_INTO    0x0300
#define CB_CALL_RETURN_FIELD_ADDR   0x0400
#define CB_CALL_RETURN_VOID    0x0500
#define CB_CALL_RETURN_MASK    0x0F00

struct cb_call {
    struct cb_tree_common   common;
    cb_tree                 name;
    cb_tree                 args;
    cb_tree                 stmt1;
    cb_tree                 stmt2;
    cb_tree                 stmt3;
    cb_tree                 returning;
    int                     is_system;
    int                     call_convention;
    int                     is_chain;
};

#define CB_CALL(x)              (CB_TREE_CAST (CB_TAG_CALL, struct cb_call, x))
#define CB_CALL_P(x)            (CB_TREE_TAG (x) == CB_TAG_CALL)

extern cb_tree          cb_build_call (cb_tree name, cb_tree args, cb_tree stmt1, cb_tree stmt2,
                                       cb_tree returning, int is_system_call, int call_convention,
                                       cb_tree stmt3);


/*
 * GO TO statement
 */

struct cb_goto {
    struct cb_tree_common   common;
    cb_tree                 target;
    cb_tree                 depending;
    cb_tree                 return_field;
};

#define CB_GOTO(x)              (CB_TREE_CAST (CB_TAG_GOTO, struct cb_goto, x))
#define CB_GOTO_P(x)            (CB_TREE_TAG (x) == CB_TAG_GOTO)

extern cb_tree          cb_build_goto (cb_tree target, cb_tree depending, cb_tree return_field);


/*
 * IF
 */

struct cb_if {
    struct cb_tree_common   common;
    cb_tree                 test;
    cb_tree                 stmt1;
    cb_tree                 stmt2;
    int                     unnest;
};

#define CB_IF(x)                (CB_TREE_CAST (CB_TAG_IF, struct cb_if, x))
#define CB_IF_P(x)              (CB_TREE_TAG (x) == CB_TAG_IF)

extern cb_tree          cb_build_if (cb_tree test, cb_tree stmt1, cb_tree stmt2);

/*
 * INLINE
 */

struct cb_inline {
    struct cb_tree_common   common;
    char *                  line;
};

#define CB_INLINE(x)                (CB_TREE_CAST (CB_TAG_INLINE, struct cb_inline, x))
#define CB_INLINE_P(x)              (CB_TREE_TAG (x) == CB_TAG_INLINE)

extern cb_tree          cb_build_inline (const char* line);


/*
 * PERFORM
 */

enum cb_perform_type {
    CB_PERFORM_EXIT,
    CB_PERFORM_ONCE,
    CB_PERFORM_TIMES,
    CB_PERFORM_UNTIL,
    CB_PERFORM_FOREVER
};

struct cb_perform_varying {
    struct cb_tree_common   common;
    cb_tree                 name;
    cb_tree                 from;
    cb_tree                 step;
    cb_tree                 until;
};

struct cb_perform {
    struct cb_tree_common   common;
    enum cb_perform_type    type;
    cb_tree                 test;
    cb_tree                 body;
    cb_tree                 data;
    cb_tree                 varying;
    cb_tree                 exit_label;
    cb_tree                 cycle_label;
};

#define CB_PERFORM_VARYING(x)   (CB_TREE_CAST (CB_TAG_PERFORM_VARYING, struct cb_perform_varying, x))

#define CB_PERFORM(x)           (CB_TREE_CAST (CB_TAG_PERFORM, struct cb_perform, x))
#define CB_PERFORM_P(x)         (CB_TREE_TAG (x) == CB_TAG_PERFORM)

extern cb_tree          cb_build_perform (int type);
extern cb_tree          cb_build_perform_varying (cb_tree name, cb_tree from, cb_tree step, cb_tree until);
extern cb_tree          cb_build_perform_varying_step (cb_tree name, cb_tree from, cb_tree step, cb_tree until);

/*
 * PRAGMA pseudo statement
 */
#define  PRAGMA_CODEGEN_EXTRACT_BEGIN 0x01
#define  PRAGMA_CODEGEN_EXTRACT_END   0x02
/*CIT*/
struct cb_pragma {
    struct cb_tree_common   common;
    cb_tree                 pragma;
    cb_tree                 option;
    int                     code_gen_tag;
    int                     code_gen_opt;
};
#define CB_PRAGMA(x)           (CB_TREE_CAST (CB_TAG_PRAGMA, struct cb_pragma, x))
#define CB_PRAGMA_P(x)         (CB_TREE_TAG (x) == CB_TAG_PRAGMA)
extern cb_tree                  cb_build_pragma (void);


/*
 * Statement
 */

struct cb_statement {
    struct cb_tree_common   common;
    const char              *name;
    cb_tree                 body;
    cb_tree                 before_body;
    cb_tree                 after_body;
    cb_tree                 file;
    cb_tree                 handler1;
    cb_tree                 handler2;
    cb_tree                 handler3;
    cb_tree                 null_check;
    cb_tree                 ref_check;
    cb_tree                 on_hold;                /*CIT parsing time temporary*/
    cb_tree                 call_using_save;        /*CIT parsing time temporary*/
    cb_tree                 call_returning_save;    /*CIT parsing time temporary*/
    cb_tree                 final;
    int                     handler_id;
    int                     need_terminator;
};

#define CB_STATEMENT(x)         (CB_TREE_CAST (CB_TAG_STATEMENT, struct cb_statement, x))
#define CB_STATEMENT_P(x)       (CB_TREE_TAG (x) == CB_TAG_STATEMENT)

extern struct cb_statement *cb_build_statement (const char *name);


/*
 * CONTINUE
 */

struct cb_continue {
    struct cb_tree_common   common;
};

#define CB_CONTINUE(x)          (CB_TREE_CAST (CB_TAG_CONTINUE, struct cb_continue, x))
#define CB_CONTINUE_P(x)        (CB_TREE_TAG (x) == CB_TAG_CONTINUE)

extern cb_tree          cb_build_continue (void);


/*
 * READY
 */

struct cb_ready {
    struct cb_tree_common   common;
};

#define CB_READY(x)          (CB_TREE_CAST (CB_TAG_READY, struct cb_ready, x))
#define CB_READY_P(x)        (CB_TREE_TAG (x) == CB_TAG_READY)

extern cb_tree          cb_build_ready (void);


/*
 * RESET
 */

struct cb_reset {
    struct cb_tree_common   common;
};

#define CB_RESET(x)          (CB_TREE_CAST (CB_TAG_RESET, struct cb_reset, x))
#define CB_RESET_P(x)        (CB_TREE_TAG (x) == CB_TAG_RESET)

extern cb_tree          cb_build_reset (void);


/*
 * List
 */

struct cb_list {
    struct cb_tree_common   common;
    cb_tree                 purpose;
    cb_tree                 value;
    cb_tree                 chain;
    int                     sizes;
};

#define CB_LIST(x)      (CB_TREE_CAST (CB_TAG_LIST, struct cb_list, x))
#define CB_LIST_P(x)    (CB_TREE_TAG (x) == CB_TAG_LIST)

#define CB_PURPOSE(x)   (CB_LIST (x)->purpose)
#define CB_VALUE(x)     (CB_LIST (x)->value)
#define CB_CHAIN(x)     (CB_LIST (x)->chain)
#define CB_SIZES(x)     (CB_LIST (x)->sizes)

#define CB_PURPOSE_INT(x) (CB_INTEGER (CB_PURPOSE (x))->val)

#define CB_SIZE_AUTO            0x00
#define CB_SIZE_1               0x01
#define CB_SIZE_2               0x02
#define CB_SIZE_4               0x03
#define CB_SIZE_8               0x04
#define CB_SIZE_UNSIGNED        0x08
#define CB_SIZE_DELIMITED       0x10

#define CB_SIZES_INT(x)                 ((CB_LIST (x)->sizes) & 0x07)
#define CB_SIZES_INT_UNSIGNED(x)        ((CB_LIST (x)->sizes) & CB_SIZE_UNSIGNED)

extern cb_tree          cb_build_list (cb_tree purpose, cb_tree value, cb_tree rest);
extern cb_tree          cb_list_add (cb_tree l, cb_tree x);
extern cb_tree          cb_list_insert_first (cb_tree l, cb_tree x);
extern cb_tree          cb_list_insert_after (cb_tree l, cb_tree x);
extern cb_tree          cb_list_append (cb_tree l1, cb_tree l2);
extern cb_tree          cb_list_remove(cb_tree l0, cb_tree x);
extern cb_tree          cb_list_reverse (cb_tree l);
extern int              cb_list_length (cb_tree l);
extern cb_tree          cb_list_append_contents (cb_tree l1, cb_tree l2);

#define cb_list_init(x)         cb_build_list (NULL, x, NULL)
#define cb_cons(x,l)            cb_build_list (NULL, x, l)
#define emit_statement(x) \
  current_program->exec_list = cb_cons (x, current_program->exec_list)

/* Pair */

#define CB_PAIR_P(x)            (CB_LIST_P (x) && CB_PAIR_X (x))
#define CB_PAIR_X(x)            CB_PURPOSE (x)
#define CB_PAIR_Y(x)            CB_VALUE (x)

#define cb_build_pair(x,y)      cb_build_list (x, y, NULL)

/* Object list */


struct cb_object_list {
    struct cb_tree_common   common;
    const char              *name;
    char                    *cname;

    cb_tree                 olist;
    enum cb_object_list_type type;
};
#define CB_OBJECT_LIST(x)       (CB_TREE_CAST (CB_TAG_OBJECT_LIST, struct cb_object_list, x))
#define CB_OBJECT_LIST_P(x)     (CB_TREE_TAG (x) == CB_TAG_OBJECT_LIST)
extern cb_tree                  cb_build_object_list (cb_tree name, cb_tree list, enum cb_object_list_type type);

/* Entry Definitionlist */

struct cb_entry {
    struct cb_tree_common   common;
    cb_tree                 label;
    cb_tree                 using_list;
    cb_tree                 returning;
    int                     call_conv;
    int                     no_entry_function;
};
#define CB_ENTRY(x)       (CB_TREE_CAST (CB_TAG_ENTRY, struct cb_entry, x))
#define CB_ENTRY_P(x)     (CB_TREE_TAG (x) == CB_TAG_ENTRY)
extern cb_tree            cb_build_entry (cb_tree label, cb_tree using_list, cb_tree returning, int call_conv);

/*
 * Program
 */

struct cb_program {
    /* program variables */
    struct cb_program       *next_program;          /* Nested */
    char                    *source_file;
    int                     source_line;
    int                     source_column;
    const char              *program_id;
    char                    *orig_source_name;
    char                    *main_entry_name;
    FILE                    *local_storage_file;
    char                    *local_storage_name;
    FILE                    *local_register_file;
    char                    *local_register_name;
    FILE                    *static_storage_file;
    char                    *static_storage_name;
    cb_tree                 entry_list;
    cb_tree                 file_list;
    cb_tree                 exec_list;
    cb_tree                 label_list;         /*only used in perform label list*/
    cb_tree                 all_label_list;     /*all declared label list*/
    cb_tree                 reference_list;
    cb_tree                 alphabet_name_list;
    cb_tree                 class_name_list;
    cb_tree                 parameter_list;
    cb_tree                 locale_list;
    cb_tree                 symbolic_list;
    cb_tree                 global_list;
    cb_tree                 cb_return_code;
    cb_tree                 cb_sort_return;
    cb_tree                 cb_xml_code;
    cb_tree                 cb_xml_text;
    cb_tree                 cb_xml_ntext;
    cb_tree                 cb_xml_event;
    cb_tree                 cb_call_params;
    cb_tree                 cb_context_filename;
    cb_tree                 cb_context_code;
    cb_tree                 class_spec_list;
    cb_tree                 cb_tally;
    cb_tree                 interface_spec_list;
    cb_tree                 function_spec_list;
    cb_tree                 program_spec_list;
    cb_tree                 property_spec_list;
    cb_tree                 file_rec_list;
    struct cb_field         *working_storage;
    struct cb_field         *local_storage;
    struct cb_field         *linkage_storage;
    struct cb_field         *screen_storage;
    cb_tree                 local_file_list;
    cb_tree                 global_file_list;
    struct handler_struct   global_handler[5];
    cb_tree                 collating_sequence;
    cb_tree                 cursor_pos;
    cb_tree                 crt_status;
    cb_tree                 returning;              /* RETURNING */
    struct cb_word          *word_table[CB_WORD_HASH_SIZE];
    /* internal variables */
    int                     loop_counter;
    int                     decimal_index;
    int                     decimal_index_max;
    unsigned char           decimal_point;          /* '.' or ',' */
    unsigned char           currency_symbol;        /* '$' or user-specified */
    unsigned char           numeric_separator;      /* ',' or '.' */
    unsigned char           nested_level;           /* Nested program level */
    unsigned char           flag_main;              /* Gen main function */
    unsigned char           flag_common;            /* COMMON PROGRAM */
    unsigned char           flag_initial;           /* INITIAL PROGRAM */
    unsigned char           flag_recursive;         /* RECURSIVE PROGRAM */
    unsigned char           flag_screen;            /* have SCREEN SECTION */
    unsigned char           flag_validated;         /* End program validate */
    unsigned char           flag_chained;           /* PROCEDURE CHAINING */
    unsigned char           flag_global_use;        /* USE GLOBAL */
    unsigned char           flag_is_external;       /* is_external */
    unsigned char           gen_decset;             /* Gen decimal_set_int */
    unsigned char           gen_udecset;            /* Gen decimal_set_uint */
    unsigned char           gen_ptrmanip;           /* Gen cob_pointer_manip */
    unsigned char           gen_file_error;         /* Gen error routine */
    unsigned char           prog_type;              /* Program type */
    unsigned char           spare[3];               /* Spare */
    /*CIT*/
    unsigned char           gen_params_protection;  /* Gen params protection agains NULL */
    unsigned char           flag_is_part_of_nested;
    cb_tree                 alter_goto_list;
    int                     field_pointer_count;
    int                     field_pointer_max;
    unsigned char           flag_sign_leading;
    unsigned char           flag_sign_separate;
    unsigned char           flag_init_outputed;
    long long               debug_moduleid;
    char *                  build_timestamp;
    /*INDEX OPTIMIZE*/
    decimal_optimizer          *decimal_list;
    decimal_optimizer          *zombie;
    int                     cb_has_report_section;

};

extern struct cb_program    *cb_build_program (struct cb_program *last_program,
                                               int nest_level);

/* parser.y */
extern int  non_const_word;
extern size_t   check_unreached;

/* reserved.c */
extern struct cb_intrinsic_table *userdefined_functions;
extern cb_tree          lookup_system_name (const char *name);
extern int              lookup_reserved_word (const char *name);
extern int              is_reserved_word_excluded (const char *name);
extern void             cb_list_reserved (void);
extern void             cb_list_intrinsics (void);
extern void             cb_list_mnemonics (void);
extern void             cb_init_reserved (void);
extern void             cb_list_map (cb_tree (*func) (cb_tree x), cb_tree l);
extern cb_tree          cb_list_join (cb_tree lx, cb_tree ly);
/* error.c */
extern int              cb_disable_all_warning;  /*CIT*/
#ifdef __GNUC__
extern void             cb_warning_x (cb_tree x, const char *fmt, ...)
__attribute__ ((__format__ (__printf__, 2, 3)));
extern void             cb_error_x (cb_tree x, const char *fmt, ...)
__attribute__ ((__format__ (__printf__, 2, 3)));
#else
extern void              cb_warning_x (cb_tree x, const char *fmt, ...);
extern void              cb_error_x (cb_tree x, const char *fmt, ...);
#endif
extern int               cb_check_feature_x (cb_tree x, int tag, const char *fmt, ...);

extern void             cb_warning_suggestion (cb_tree x, const char *fmt, ...);
extern void             cb_warning_information (cb_tree x, const char *fmt, ...);
extern char             *check_filler_name (char *name);
extern void             redefinition_error (cb_tree x);
extern void             redefinition_warning (cb_tree x, cb_tree y);
extern void             undefined_error (cb_tree x);
extern void             ambiguous_error (cb_tree x);
extern void             group_error (cb_tree x, const char *clause);
extern void             level_redundant_error (cb_tree x, const char *clause);
extern void             level_require_error (cb_tree x, const char *clause);
extern void             level_except_error (cb_tree x, const char *clause);
extern void             enterprise_error (const char *msg);
extern void             typedef_error (cb_tree x, int typerequired);

struct cb_literal       *build_literal (enum cb_category category,
                                        const unsigned char *data, size_t size);

/* field.c */
extern size_t           cb_needs_01;
extern int              cb_get_level (cb_tree x);
extern cb_tree          cb_build_field_tree (cb_tree level, cb_tree name,
                                             struct cb_field *last_field,
                                             enum cb_storage storage, struct cb_file *fn);

extern struct cb_field  *cb_resolve_redefines (struct cb_field *field,
                                               cb_tree redefines);
extern void             cb_validate_field (struct cb_field *p);
extern void             cb_validate_section_fields (struct cb_field **f);
extern void             cb_validate_88_item (struct cb_field *p);
extern struct cb_field  *cb_validate_78_item (struct cb_field *p);
extern void             cb_clear_real_field (void);

/* typeck.c */
extern cb_tree          cb_check_numeric_value (cb_tree x);

extern void             cb_build_registers (void);
extern char             *cb_encode_program_id (const char *name);
extern const char       *cb_build_program_id (cb_tree name, cb_tree alt_name);
extern void             cb_define_switch_name (cb_tree name, cb_tree sname,
                                               cb_tree flag, cb_tree ref);
extern cb_tree          cb_build_section_name (cb_tree name, int sect_or_para);
extern cb_tree          cb_build_assignment_name (struct cb_file *curfile,
                                                  cb_tree name);
extern cb_tree          cb_build_index (cb_tree name, cb_tree values,
                                        int indexed_by, struct cb_field *qual, int local_storage, enum cb_usage usage);
extern cb_tree          cb_build_display (cb_tree x, cb_tree values, int size);
extern cb_tree          cb_build_identifier (cb_tree x);
extern cb_tree          cb_build_length (cb_tree x);
extern cb_tree          cb_build_const_length (cb_tree x);
extern cb_tree          cb_build_address (cb_tree x);
extern cb_tree          cb_build_ppointer (cb_tree x);
extern cb_tree          cb_build_dummy_linkage (cb_tree x, int size);
extern cb_tree          cb_build_dummy_national_linkage(cb_tree x, int size);

extern void             cb_validate_program_environment (struct cb_program *prog);
extern void             cb_validate_program_data (struct cb_program *prog);
extern void             cb_validate_program_body (struct cb_program *prog);

extern cb_tree          cb_build_expr (cb_tree list);
extern cb_tree          cb_build_evaluate_expr (cb_tree list, cb_tree subject);
extern cb_tree          cb_build_cond (cb_tree x);
extern cb_tree          cb_build_cond_88 (cb_tree x);

/*CIT*/
extern cb_tree          cb_get_optimized_field_shadow (struct cb_field *f);
extern cb_tree          cb_check_optimized_field_1(cb_tree x);
extern void             cb_check_optimized_field(cb_tree vars, int rounded_only);
extern void             cb_emit_arithmetic (cb_tree vars, int op, cb_tree val, int allow_div_optimize);
extern cb_tree          cb_build_add (cb_tree v, cb_tree n, cb_tree round_opt);
extern cb_tree          cb_build_sub (cb_tree v, cb_tree n, cb_tree round_opt);
extern void             cb_emit_corresponding (
                                              cb_tree (*func) (cb_tree f1, cb_tree f2, cb_tree f3),
                                              cb_tree x1, cb_tree x2, cb_tree opt);
extern void             cb_emit_move_corresponding (cb_tree x1, cb_tree x2);
extern cb_tree          cb_validate_procedure_param_list(cb_tree l);
extern void             cb_check_setting_action (cb_tree dst, int assign_action);
extern void             cb_increment_indexes (struct cb_field *f);
extern void             cb_expand_screen_field (struct cb_field *f, cb_tree subs) ;

/*CIT*/
extern cb_tree          cb_build_mul (cb_tree v, cb_tree n, cb_tree round_opt);
extern cb_tree          cb_build_div (cb_tree v, cb_tree n, cb_tree round_opt);

extern void             cb_emit_accept (cb_tree var, cb_tree line, cb_tree column, cb_tree fgc,
                                        cb_tree bgc, cb_tree scroll, int dispattrs, char dispprompt, 
                                        cb_tree excp_field , cb_tree timeout);
extern void             cb_emit_accept_line_or_col (cb_tree var, const int l_or_c);
extern void             cb_emit_accept_date (cb_tree var);
extern void             cb_emit_accept_date_yyyymmdd (cb_tree var);
extern void             cb_emit_accept_day (cb_tree var);
extern void             cb_emit_accept_day_yyyyddd (cb_tree var);
extern void             cb_emit_accept_day_of_week (cb_tree var);
extern void             cb_emit_accept_time (cb_tree var);
extern void             cb_emit_accept_command_line (cb_tree var);
extern void             cb_emit_get_environment (cb_tree envvar, cb_tree envval);
extern void             cb_emit_accept_environment (cb_tree var);
extern void             cb_emit_accept_mnemonic (cb_tree var, cb_tree mnemonic);
extern void             cb_emit_accept_name (cb_tree var, cb_tree name);
extern void             cb_emit_accept_arg_number (cb_tree var);
extern void             cb_emit_accept_arg_value (cb_tree var);
extern void             cb_emit_accept_escape (cb_tree var);

extern void             cb_emit_allocate (cb_tree target1, cb_tree target2,
                                          cb_tree size, cb_tree initialize);
extern void             cb_emit_alter (cb_tree src, cb_tree target);
extern void             cb_emit_free (cb_tree vars);
extern cb_tree          find_alter_goto(struct cb_program *prog, const char *label);

/*CIT*/
extern int              cb_merge_call_conv (cb_tree mf_call_conv, cb_tree call_convention);

extern void             cb_emit_call (cb_tree prog, int call_convention, cb_tree using_list, cb_tree returning,
                                      cb_tree on_exception, cb_tree not_on_exception);

extern void             cb_emit_chain (cb_tree prog, int call_convention, cb_tree using_list, cb_tree returning,
                                      cb_tree on_exception, cb_tree not_on_exception);

extern void             cb_emit_cancel (cb_tree prog);

extern void             cb_emit_close (cb_tree file, cb_tree opt);

extern void             cb_emit_commit (void);

extern void             cb_emit_continue (void);
extern void             cb_emit_ready (void);
extern void             cb_emit_reset (void);

extern void             cb_emit_delete (cb_tree file);
extern void             cb_emit_delete_file (cb_tree file);

extern void             cb_emit_display (cb_tree values, cb_tree upon,
                                         cb_tree no_adv, cb_tree line, cb_tree column,
                                         cb_tree fgc, cb_tree bgc,
                                         cb_tree scroll, int dispattrs);
extern void             cb_emit_exhibit (cb_tree values, cb_tree upon, cb_tree no_adv, int options);
extern cb_tree          cb_build_exhibit_one (cb_tree x, int option);
extern cb_tree          cb_build_display_upon (cb_tree x);
extern cb_tree          cb_build_display_upon_direct (cb_tree x);
extern void             cb_emit_env_name (cb_tree value);
extern void             cb_emit_env_value (cb_tree value);
extern void             cb_emit_arg_number (cb_tree value);
extern void             cb_emit_command_line (cb_tree value);

extern void             cb_emit_divide (cb_tree dividend, cb_tree divisor,
                                        cb_tree quotient, cb_tree remainder);

extern int              cb_is_other_caselist(cb_tree case_list);
extern void             cb_emit_evaluate (cb_tree subject_list, cb_tree case_list);

extern void             cb_emit_goto (cb_tree target, cb_tree depending);
extern void             cb_emit_exit (size_t goback, cb_tree return_field);

extern void             cb_emit_if (cb_tree cond, cb_tree stmt1, cb_tree stmt2);

extern void             cb_emit_initialize_vars (cb_tree vars, cb_tree fillinit,
                                            cb_tree value, cb_tree replacing, cb_tree def);
extern void             cb_emit_fixvalue (cb_tree vars, int flag);

extern void             cb_emit_inspect (cb_tree var, cb_tree body,
                                         cb_tree replacing, int replconv);
extern void             cb_init_tarrying (void);
extern cb_tree          cb_build_initialize_vars (cb_tree vars, cb_tree fillinit,
                                            cb_tree value, cb_tree replacing, cb_tree def, int filldefault);
extern cb_tree          cb_build_tarrying_data (cb_tree x);
extern cb_tree          cb_build_tarrying_characters (cb_tree l);
extern cb_tree          cb_build_tarrying_all (void);
extern cb_tree          cb_build_tarrying_leading (void);
extern cb_tree          cb_build_tarrying_trailing (void);
extern cb_tree          cb_build_tarrying_value (cb_tree x, cb_tree l);
extern cb_tree          cb_build_replacing_characters (cb_tree x, cb_tree l);
extern cb_tree          cb_build_replacing_all (cb_tree x, cb_tree y, cb_tree l);
extern cb_tree          cb_build_replacing_leading (cb_tree x, cb_tree y, cb_tree l);
extern cb_tree          cb_build_replacing_first (cb_tree x, cb_tree y, cb_tree l);
extern cb_tree          cb_build_replacing_trailing (cb_tree x, cb_tree y, cb_tree l);
extern cb_tree          cb_build_converting (cb_tree x, cb_tree y, cb_tree l);
extern cb_tree          cb_build_inspect_region_start (void);
extern cb_tree          cb_build_inspect_region (cb_tree l, cb_tree pos, cb_tree x);

extern int              cb_validate_move (cb_tree src, cb_tree dst, size_t is_value);
extern cb_tree          cb_build_move (cb_tree src, cb_tree dst);
extern void             cb_emit_move (cb_tree src, cb_tree dsts);
extern void             cb_emit_move_one (cb_tree src, cb_tree dst);
extern char *           cb_to_cname (const char *s, char * copy);
extern cb_tree          cb_build_move_memcopy (cb_tree src, cb_tree dst);

/*CIT*/
extern void             cb_emit_open (cb_tree file, cb_tree mode, cb_tree sharing, cb_tree option);

extern void             cb_emit_perform (cb_tree perform, cb_tree body);
extern cb_tree          cb_build_perform_once (cb_tree body);
extern cb_tree          cb_build_perform_times (cb_tree count);
extern cb_tree          cb_build_perform_until (cb_tree condition, cb_tree varying);
extern cb_tree          cb_build_perform_forever (cb_tree body);
extern cb_tree          cb_build_perform_exit (struct cb_label *label);

/*CIT*/
extern void             cb_emit_pragma (cb_tree pragma, cb_tree option);

extern void             cb_emit_read (cb_tree ref, cb_tree next, cb_tree into,
                                      cb_tree key, cb_tree lock_opts);

extern void             cb_emit_rewrite (cb_tree record, cb_tree from,
                                         cb_tree lockopt);

extern void             cb_emit_release (cb_tree ref, cb_tree from);
extern void             cb_emit_return (cb_tree ref, cb_tree into);

extern void             cb_emit_rollback (void);

extern void             cb_emit_search (cb_tree table, cb_tree varying,
                                        cb_tree at_end, cb_tree whens);
extern void             cb_emit_search_all (cb_tree table, cb_tree at_end,
                                            cb_tree when, cb_tree stmts);

extern void             cb_emit_setenv (cb_tree x, cb_tree y);
extern void             cb_emit_set_to (cb_tree l, cb_tree x);
extern void             cb_emit_set_up_down (cb_tree l, cb_tree flag, cb_tree x);
extern void             cb_emit_set_on_off (cb_tree l, cb_tree flag);
extern void             cb_emit_set_true (cb_tree l);
extern void             cb_emit_set_false (cb_tree l);

extern void             cb_emit_sort_init (cb_tree name, cb_tree keys, cb_tree col);
extern void             cb_emit_sort_using (cb_tree file, cb_tree l);
/*CIT*/
extern void             cb_emit_sort_input (cb_tree file, cb_tree proc);
extern void             cb_emit_sort_giving (cb_tree file, cb_tree l, int input_proc_used);
extern void             cb_emit_sort_output (cb_tree file, cb_tree proc);
extern void             cb_emit_sort_finish (cb_tree file);

extern void             cb_emit_start (cb_tree file, cb_tree op, cb_tree key);

extern void             cb_emit_stop_run (cb_tree x);
/*CIT*/
extern void             cb_emit_stop_abend (cb_tree x);

extern void             cb_emit_string (cb_tree items, cb_tree into, cb_tree pointer);

extern void             cb_emit_unlock (cb_tree ref);

extern void             cb_emit_unstring (cb_tree name, cb_tree delimited,
                                          cb_tree into, cb_tree pointer, cb_tree tallying);
extern cb_tree          cb_build_unstring_delimited (cb_tree all, cb_tree value);
extern cb_tree          cb_build_unstring_into (cb_tree name, cb_tree delimiter,
                                                cb_tree count);

extern void             cb_emit_write (cb_tree record, cb_tree from, cb_tree opt,
                                       cb_tree lockopt);
extern cb_tree          cb_build_write_advancing_lines (cb_tree pos, cb_tree lines);
extern cb_tree          cb_build_write_advancing_mnemonic (cb_tree pos,
                                                           cb_tree mnemonic);
extern cb_tree          cb_build_write_advancing_page (cb_tree pos);

extern void             cobc_tree_cast_error (cb_tree x, const char *filen,
                                              const int linenum, const int tagnum);
extern void             cb_emit_set_context_filename (cb_tree var);
extern void             cb_emit_init_context_save (cb_tree exit_flag);
extern void             cb_emit_xml_parse (cb_tree ident, cb_tree proc, cb_tree encoding, cb_tree returning_national);
extern void             cb_emit_xml_generate (cb_tree ident, cb_tree proc, cb_tree cnt);

extern void             cb_emit_wipe_cache (void);
extern void             cb_emit_target_cache (cb_tree x);
extern void             update_wipe_flag_and_target(cb_tree dst, cb_tree src);
extern char *           binary_reg_name(char * buffer, enum decimal_binary_type  binary, int id);

/* codegen.c */
extern void             codegen (struct cb_program *prog);
extern void             display_optimize_info (void);
extern void             reset_optimize_info (void);
extern void             output_main_symbol (FILE *file, char *callname, int paramcnt) ;
/*CIT*/
void                    output_source_header (FILE *f, char *locbuff, char* source_file);

/* scanner.l */
extern void             cb_set_in_procedure (void);
extern void             cb_reset_in_procedure (void);
extern struct cb_level_78 *cb_add_78 (struct cb_field *f);
extern void             cb_reset_78 (void);
extern struct cb_field  *check_level_78 (const char *name);
extern void              update_level_78 (const char *name, const char * value);
extern size_t            cb_force_pid_literal;

typedef struct cb_lex_source_file_info_t
{
    unsigned char * file_name;
    int             file_name_hash;
    int             line;
    int             column;
    struct cb_lex_source_file_info_t *next;
} cb_lex_source_file_info;

#endif /* CB_TREE_H */

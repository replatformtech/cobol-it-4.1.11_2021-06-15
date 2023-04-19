
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 31 "parser.y"

#include "cobc.h"

#include <stdlib.h>
#include <string.h>
#include <libcob.h>

#include <libcob.h>
#include "tree.h"
#include <enterprise/a2e.h>

#define YYSTYPE                 cb_tree
#define yyerror                 cb_error
#define YYDEBUG                 1
#define YYERROR_VERBOSE         1

#define PENDING(x)              cb_warning (_("'%s' not implemented"), x)

#define emit_statement(x) \
  current_program->exec_list = cb_cons (x, current_program->exec_list)

#define push_expr(type, node) \
  current_expr = cb_build_list (cb_int (type), node, current_expr)

#define TERM_NONE               0
#define TERM_ACCEPT             1
#define TERM_ADD                2
#define TERM_CALL               3
#define TERM_COMPUTE            4
#define TERM_DELETE             5
#define TERM_DISPLAY            6
#define TERM_DIVIDE             7
#define TERM_EVALUATE           8
#define TERM_IF                 9
#define TERM_MULTIPLY           10
#define TERM_PERFORM            11
#define TERM_READ               12
#define TERM_RECEIVE            13
#define TERM_RETURN             14
#define TERM_REWRITE            15
#define TERM_SEARCH             16
#define TERM_START              17
#define TERM_STRING             18
#define TERM_SUBTRACT           19
#define TERM_UNSTRING           20
#define TERM_WRITE              21
#define TERM_CHAIN              22
#define TERM_EXHIBIT            23
#define TERM_XML                24
#define TERM_MAX                25

/* Global variables */

struct cb_program       *external_program_list= NULL;
struct cb_program       *current_program = NULL;
struct cb_statement     *current_statement = NULL;
struct cb_label         *current_section = NULL;
struct cb_label         *current_paragraph = NULL;
size_t                  functions_are_all = 0;
size_t                  check_unreached = 0;
int                     non_const_word = 0;
struct cb_field         *typedef_storage = NULL;

/* Local variables */

static struct cb_statement      *main_statement;

static cb_tree                  current_expr;
static struct cb_field          *current_field = NULL;
static struct cb_field          *description_field;
static struct cb_file           *current_file;

static enum cb_storage          current_storage;

static int                      call_mode;
static int                      size_mode;
static int                      rounded_used;

static cb_tree                  perform_stack = NULL;
static cb_tree                  qualifier = NULL;

static cb_tree                  display_fgc;
static cb_tree                  display_bgc;
static cb_tree                  display_line;
static cb_tree                  display_column;
static cb_tree                  display_upon;
static cb_tree                  display_advance;
static cb_tree                  display_x_list;
static cb_tree                  scroll;
static cb_tree                  save_tree_1;
static cb_tree                  save_tree_2;
static cb_tree                  dummy_tree;
static cb_tree                  acc_exception_field;
static cb_tree                  acc_timeout;
static cb_tree                  start_cond;
static int                      sort_input_proc;

/*saving variables*/
static cb_tree                  save_display_fgc;
static cb_tree                  save_display_bgc;
static cb_tree                  save_display_line;
static cb_tree                  save_display_column;
static cb_tree                  save_scroll;
static int                      save_dispattrs = 0;
static int                      save_dispprompt = 0;
/**/
static cb_tree                  current_file_type;
static size_t                   in_declaratives = 0;
static size_t                   current_linage = 0;
static size_t                   prog_end = 0;
static size_t                   use_global_ind = 0;
static size_t                   samearea = 1;
static size_t                   organized_seen = 0;
static size_t                   inspect_keyword = 0;
static int                      next_label_id = 0;
static int                      eval_level = 0;
static int                      eval_inc = 0;
static int                      eval_inc2 = 0;
static int                      depth = 0;
static int                      dispattrs = 0;
static int                      dispprompt = 0;
static struct cb_file           *linage_file;
static cb_tree                  next_label_list = NULL;
static char                     *stack_progid[32];
static int                      term_array[TERM_MAX];
static int                      eval_check[64][64];
static int                      exhibit_option;
/*static cb_tree                  eval_subject[64][64];*/

/* Static functions */

static void
BEGIN_STATEMENT_1 (const char *name, const size_t term, int checkpos)
{
    if (cb_warn_unreachable && check_unreached) {
        cb_warning (_("Unreachable statement"));
    }
    current_statement = cb_build_statement ((char *)name);
    CB_TREE (current_statement)->source_file = (unsigned char *)cb_lex_source_file.file_name;
    CB_TREE (current_statement)->source_line = cb_lex_source_file.line;
    CB_TREE (current_statement)->source_column = cb_lex_source_file.column; 
    emit_statement (CB_TREE (current_statement));
    if (term) {
        term_array[term]++;
    }
    main_statement = current_statement;
    current_program->field_pointer_count = 0;
    if (checkpos && CB_TREE (current_statement)->source_column <= 4) {
      cb_check_feature_x ( CB_TREE (current_statement), cb_syntax_ibm5_2, "Statement start at %d must start after column 12", CB_TREE (current_statement)->source_column + 7);
    }
}

#define BEGIN_STATEMENT(name,term) BEGIN_STATEMENT_1(name,term,1)

static void
BEGIN_IMPLICIT_STATEMENT (void)
{
    current_statement = cb_build_statement (NULL);
    main_statement->body = cb_list_add (main_statement->body,
                        CB_TREE (current_statement));
}

static void
emit_entry (const char *name, const int encode, cb_tree using_list, int call_conv, int module_entry)
{
    cb_tree     l;
    cb_tree     label;
    cb_tree     x;
    cb_tree     y;
    struct cb_field *f;
    int         parmnum;
    char        buff[256];
    struct cb_program *p;

    sprintf (buff, "E$%s", name);
    label = cb_build_label (cb_build_reference (buff), NULL);
    if (encode) {
        CB_LABEL (label)->name = (unsigned char *)(cb_encode_program_id (name));
        CB_LABEL (label)->orig_name = (unsigned char *)name;
    } else {
        CB_LABEL (label)->name = (unsigned char *)name;
        CB_LABEL (label)->orig_name = (unsigned char *)current_program->orig_source_name;
    }
    CB_LABEL (label)->need_begin = 1;
    CB_LABEL (label)->is_entry = 1;
    CB_LABEL (label)->is_module_entry = module_entry;
    CB_LABEL (label)->entry_call_convesion = call_conv;
    emit_statement (label);

    parmnum = 1;
    for (l = using_list; l; l = CB_CHAIN (l)) {
        x = CB_VALUE (l);
        if (x != cb_error_node && cb_ref (x) != cb_error_node) {
            f = CB_FIELD (cb_ref (x));
            if (f->level != 01 && f->level != 77) {
                cb_error_x (x, _("'%s' not level 01 or 77"), cb_name (x));
            }
            if (!current_program->flag_chained) {
                if (f->storage != CB_STORAGE_LINKAGE) {
                        /*COBOL-IT*/
                        if ( !cb_relaxed_syntax_check || 
                             !cb_check_feature_x (x, cb_syntax_ibm5_2, "USING field must be in LINKAGE")) {
                               cb_error_x (x, _("'%s' is not in LINKAGE SECTION"), cb_name (x));
                        } else {
                               /*COBOL-IT*/
                               /*Hack for MF to allow now linage section */
                               if (f->flag_external ){
                                        cb_error_x (x, _("PROCEDURE USING parameter '%s' is EXTERNAL"), cb_name (x));
                               } else {
                                        cb_warning_information (x, _("'%s' is not in LINKAGE SECTION"), cb_name (x));
                                        /*
                                        f->storage = CB_STORAGE_LINKAGE;
                                        f->flag_local = 1;
                                        f->flag_base = 1;
                                        */
                                        y = cb_build_filler ();
                                        cb_build_dummy_linkage(y,f->size);
                                        f = CB_FIELD (cb_ref (y));
                                        CB_VALUE (l) = y;
                                        emit_statement(cb_build_move_memcopy(y, x));
                               }
                        }
                }
                if (f->flag_item_based || f->flag_external) {
                    cb_error_x (x, _("'%s' can not be BASED/EXTERNAL"), cb_name (x));
                }
                f->flag_is_pdiv_parm = 1;
            } else {
                if (f->storage != CB_STORAGE_WORKING) {
                    cb_error_x (x, _("'%s' is not in WORKING-STORAGE SECTION"), cb_name (x));
                }
                f->flag_chained = 1;
                f->param_num = parmnum;
                parmnum++;
            }
            if (f->redefines) {
                if (CB_PURPOSE(l) != cb_int (CB_CALL_BY_REFERENCE) &&
                    CB_PURPOSE(l) != cb_int (CB_CALL_BY_DEFAULT) &&
                    CB_PURPOSE(l) != cb_int (CB_CALL_BY_DESCRIPTOR)) {
                    cb_error_x (x, _("'%s' REDEFINES field not allowed here"), cb_name (x));
                } else {
                    while(f->redefines) {
                        f = f->redefines;
                    }
                    CB_VALUE (l) = cb_build_field_reference (f, NULL);
                }
            } 
        }
    }

    /* Check dangling LINKAGE items */
    if (cb_warn_linkage) {
        for (f = current_program->linkage_storage; f; f = f->sister) {
            for (l = using_list; l; l = CB_CHAIN (l)) {
                x = CB_VALUE (l);
                if (x != cb_error_node && cb_ref (x) != cb_error_node) {
                    if (f == CB_FIELD (cb_ref (x))) {
                        break;
                    }
                }
            }
            if (!l && !f->redefines) {
                cb_warning (_("LINKAGE item '%s' is not a PROCEDURE USING parameter"), f->name);
            }
        }
    }

    /* check for duplicated entry*/
    for (p=current_program; p; p = p->next_program) {
        for (l = p->entry_list; l; l = CB_CHAIN (l)) {
            struct cb_entry *e = CB_ENTRY(CB_VALUE(l)); 
            if (strcasecmp ((const char *)name, (const char *)(CB_LABEL(e->label)->name)) == 0) {               
                if (current_statement) {
                    /* DO we have ENTRY with the same name than module file and != PRODID*/
                    if (source_demangle_name &&
                        (strcasecmp(p->program_id, source_demangle_name) != 0)) {
                       /* yes then change the "faque entry build from the module name to this entry*/
                       e->no_entry_function = 1;
                    } else {
                       cb_error_x (CB_TREE (current_statement), _("ENTRY '%s' duplicated"), name);
                    }
                } else {
                    cb_error (_("ENTRY '%s' duplicated or use -fno-module-name-entry"), name);
                }
            }
        }
    }

    current_program->entry_list = cb_list_add (current_program->entry_list,
                                       cb_build_entry(label,using_list,NULL, call_conv));
}

static void
terminator_warning (const size_t termid)
{
    check_unreached = 0;
    if (cb_warn_terminator && term_array[termid]) {
        cb_warning_x (CB_TREE (current_statement),
            _("%s statement not terminated by END-%s"),
            current_statement->name, current_statement->name);
    }
    if (term_array[termid]) {
        term_array[termid]--;
    }
}

/*
static void
terminator_error (void)
{
    check_unreached = 0;
    cb_error_x (CB_TREE (current_statement),
            _("%s statement not terminated by END-%s"),
            current_statement->name, current_statement->name);
}
*/
static void
terminator_clear (const size_t termid)
{
    check_unreached = 0;
    if (term_array[termid]) {
        term_array[termid]--;
    }
}

static int
literal_value (cb_tree x)
{
    if (x == cb_space) {
        return CHAR_SP;
    } else if (x == cb_zero) {
        return CHAR_0;
    } else if (x == cb_quote) {
        return cb_quote_char ; /* '"'; */
    } else if (x == cb_dirsep) {
        return *(CB_LITERAL(cb_dirsep)->data) ; /* '/'; */
        
    } else if (x == cb_null) {
        return 0;
    } else if (CB_TREE_CLASS (x) == CB_CLASS_NUMERIC) {
        return cb_get_int (x);
    } else {
        return CB_LITERAL (x)->data[0];
    }
}

static void
setup_use_file (struct cb_file *fileptr)
{
    struct cb_file  *newptr;

    if (fileptr->organization == COB_ORG_SORT) {
        cb_error (_("USE statement invalid for SORT file"));
    }
    if (fileptr->global) {
        newptr = cobc_malloc (sizeof(struct cb_file));
        *newptr = *fileptr;
        newptr->handler = current_section;
        newptr->handler_prog = current_program;
        if (!use_global_ind) {
            current_program->local_file_list =
                cb_list_add (current_program->local_file_list,
                         CB_TREE (newptr));
        } else {
            current_program->global_file_list =
                cb_list_add (current_program->global_file_list,
                         CB_TREE (newptr));
        }
    } else {
        fileptr->handler = current_section;
    }
}



/* Line 189 of yacc.c  */
#line 449 "parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_EOF = 0,
     ABEND = 258,
     ACCEPT = 259,
     ACCESS = 260,
     ADD = 261,
     ADDRESS = 262,
     ADVANCING = 263,
     AFTER = 264,
     ALL = 265,
     ALLOCATE = 266,
     ALPHABET = 267,
     ALPHABETIC = 268,
     ALPHABETIC_LOWER = 269,
     ALPHABETIC_UPPER = 270,
     ALPHANUMERIC = 271,
     ALPHANUMERIC_EDITED = 272,
     ALSO = 273,
     ALTER = 274,
     ALTERNATE = 275,
     AND = 276,
     ANY = 277,
     APPLY = 278,
     ARE = 279,
     AREA = 280,
     ARGUMENT_NUMBER = 281,
     ARGUMENT_VALUE = 282,
     AS = 283,
     ASCENDING = 284,
     ASSIGN = 285,
     AT = 286,
     AUTO = 287,
     AUTOMATIC = 288,
     BACKGROUND_COLOR = 289,
     BASED = 290,
     BEFORE = 291,
     BELL = 292,
     B_AND = 293,
     B_NOT = 294,
     B_OR = 295,
     B_XOR = 296,
     BINARY = 297,
     BINARY_C_LONG = 298,
     BINARY_CHAR = 299,
     BINARY_DOUBLE = 300,
     BINARY_LONG = 301,
     BINARY_SHORT = 302,
     BIT = 303,
     BLANK = 304,
     BLANK_LINE = 305,
     BLANK_SCREEN = 306,
     BLINK = 307,
     BLOCK = 308,
     BOTTOM = 309,
     BY = 310,
     BYTE_LENGTH = 311,
     CALL = 312,
     CANCEL = 313,
     CDECL = 314,
     CENTURY_DATE = 315,
     CENTURY_DAY = 316,
     CH = 317,
     CHAIN = 318,
     CHAINING = 319,
     CHANGED = 320,
     CHARACTER = 321,
     CHARACTERS = 322,
     CHECKPOINT = 323,
     CLASS = 324,
     CLOSE = 325,
     CODE = 326,
     CODE_SET = 327,
     COLLATING = 328,
     COL = 329,
     COLS = 330,
     COLUMN = 331,
     COLUMNS = 332,
     COMMA = 333,
     COMMAND_LINE = 334,
     COMMA_DELIM = 335,
     COMMIT = 336,
     COMMON = 337,
     COMP = 338,
     COMPUTE = 339,
     COMP_1 = 340,
     COMP_2 = 341,
     COMP_3 = 342,
     COMP_4 = 343,
     COMP_5 = 344,
     COMP_6 = 345,
     COMP_X = 346,
     CONCATENATE_FUNC = 347,
     CONFIGURATION = 348,
     CONSTANT = 349,
     CONTAINS = 350,
     CONTENT = 351,
     CONTINUE = 352,
     CONTROL = 353,
     CONTROLS = 354,
     CONVERT = 355,
     CONVERTING = 356,
     CORE_INDEX = 357,
     CORRESPONDING = 358,
     COUNT = 359,
     CRT = 360,
     CURRENCY = 361,
     CURRENT_DATE_FUNC = 362,
     CURSOR = 363,
     CYCLE = 364,
     DATA = 365,
     DATE = 366,
     DAY = 367,
     DAY_OF_WEEK = 368,
     DE = 369,
     DEBUGGING = 370,
     DECIMAL_POINT = 371,
     DECLARATIVES = 372,
     DEFAULT = 373,
     DELETE = 374,
     DELIMITED = 375,
     DELIMITER = 376,
     DEPENDING = 377,
     DESCENDING = 378,
     DESCRIPTOR = 379,
     DETAIL = 380,
     DIR_SEPARATOR = 381,
     DISK = 382,
     DISPLAY = 383,
     DISPLAY_1 = 384,
     DISPLAY_OF_FUNC = 385,
     DIVIDE = 386,
     DIVISION = 387,
     DOTWORD = 388,
     DOWN = 389,
     DUPLICATES = 390,
     DYNAMIC = 391,
     ECHO = 392,
     EBCDIC = 393,
     ELSE = 394,
     EMPTY_CHECK = 395,
     ENCODING = 396,
     END = 397,
     END_ACCEPT = 398,
     END_ADD = 399,
     END_CALL = 400,
     END_COMPUTE = 401,
     END_CHAIN = 402,
     END_DELETE = 403,
     END_DISPLAY = 404,
     END_DIVIDE = 405,
     END_EVALUATE = 406,
     END_EXHIBIT = 407,
     END_FUNCTION = 408,
     END_IF = 409,
     END_MULTIPLY = 410,
     END_PERFORM = 411,
     END_PROGRAM = 412,
     END_READ = 413,
     END_RETURN = 414,
     END_REWRITE = 415,
     END_SEARCH = 416,
     END_START = 417,
     END_STRING = 418,
     END_SUBTRACT = 419,
     END_UNSTRING = 420,
     END_WRITE = 421,
     END_XML = 422,
     ENTRY = 423,
     ENVIRONMENT = 424,
     ENVIRONMENT_NAME = 425,
     ENVIRONMENT_VALUE = 426,
     EOL = 427,
     EOP = 428,
     EOS = 429,
     EQUAL = 430,
     EQUALS = 431,
     ERASE = 432,
     ERROR = 433,
     ESCAPE = 434,
     EVALUATE = 435,
     EVENT_STATUS = 436,
     EXCEPTION = 437,
     EXCLUSIVE = 438,
     EXIT = 439,
     EXHIBIT = 440,
     EXTEND = 441,
     EXTERNAL = 442,
     FAILURE = 443,
     FD = 444,
     FILE_CONTROL = 445,
     FILE_ID = 446,
     FILLER = 447,
     FINAL = 448,
     FIRST = 449,
     FOOTING = 450,
     FOR = 451,
     FOREGROUND_COLOR = 452,
     FOREVER = 453,
     FREE = 454,
     FROM = 455,
     FULL = 456,
     FUNCTION = 457,
     FUNCTION_ID = 458,
     FUNCTION_NAME = 459,
     GE = 460,
     GENERATE = 461,
     GIVING = 462,
     GLOBAL = 463,
     GO = 464,
     GOBACK = 465,
     GREATER = 466,
     GROUP = 467,
     HEADING = 468,
     HIGHLIGHT = 469,
     HIGH_VALUE = 470,
     IDENTIFICATION = 471,
     IF = 472,
     IGNORE = 473,
     IGNORING = 474,
     IN = 475,
     INDEX = 476,
     INDEXED = 477,
     INDICATE = 478,
     INITIALIZE = 479,
     INITIALIZED = 480,
     INITIATE = 481,
     INPUT = 482,
     INPUT_OUTPUT = 483,
     INSPECT = 484,
     INTO = 485,
     INTRINSIC = 486,
     INVALID = 487,
     INVALID_KEY = 488,
     IS = 489,
     I_O = 490,
     I_O_CONTROL = 491,
     JUSTIFIED = 492,
     KEPT = 493,
     KEY = 494,
     LABEL = 495,
     LAST = 496,
     LE = 497,
     LEADING = 498,
     LEFT = 499,
     LENGTH = 500,
     LENGTH_CHECK = 501,
     LESS = 502,
     LIKE = 503,
     LIMIT = 504,
     LIMITS = 505,
     LINAGE = 506,
     LINAGE_COUNTER = 507,
     LINE = 508,
     LINES = 509,
     LINKAGE = 510,
     LITERAL = 511,
     LOCALE = 512,
     LOCALE_DT_FUNC = 513,
     LOCAL_STORAGE = 514,
     LOCK = 515,
     LOWER_CASE_FUNC = 516,
     LOWLIGHT = 517,
     LOW_VALUE = 518,
     MANUAL = 519,
     MEMORY = 520,
     MERGE = 521,
     MINUS = 522,
     MNEMONIC_NAME = 523,
     MODE = 524,
     MOVE = 525,
     MULTIPLE = 526,
     MULTIPLY = 527,
     NAMED = 528,
     NATIONAL = 529,
     NATIONAL_EDITED = 530,
     NATIONAL_OF_FUNC = 531,
     NATIVE = 532,
     NE = 533,
     NEGATIVE = 534,
     NEXT = 535,
     NO = 536,
     NOT = 537,
     NOT_END = 538,
     NOT_EOP = 539,
     NOT_EXCEPTION = 540,
     NOT_INVALID_KEY = 541,
     NOT_OVERFLOW = 542,
     NOT_SIZE_ERROR = 543,
     NO_ADVANCING = 544,
     NUMBER = 545,
     NUMBERS = 546,
     NUMERIC = 547,
     NUMERIC_EDITED = 548,
     NUMVALC_FUNC = 549,
     OBJECT_COMPUTER = 550,
     OCCURS = 551,
     OF = 552,
     OFF = 553,
     OMITTED = 554,
     ON = 555,
     ONLY = 556,
     OPEN = 557,
     OPTIONAL = 558,
     OR = 559,
     ORDER = 560,
     ORGANIZATION = 561,
     OTHER = 562,
     OUTPUT = 563,
     COB_OVERFLOW = 564,
     OVERLINE = 565,
     PACKED_DECIMAL = 566,
     PADDING = 567,
     PAGE = 568,
     PARAGRAPH = 569,
     PARSE = 570,
     PERFORM = 571,
     PICTURE = 572,
     PLUS = 573,
     POINTER = 574,
     POSITION = 575,
     POSITIVE = 576,
     PRESENT = 577,
     PRAGMA = 578,
     PREVIOUS = 579,
     PRINTER = 580,
     PRINTING = 581,
     PROCEDURE = 582,
     PROCEDURES = 583,
     PROCEED = 584,
     PROGRAM = 585,
     PROGRAM_ID = 586,
     PROGRAM_NAME = 587,
     PROGRAM_POINTER = 588,
     PROMPT = 589,
     PROCESSING = 590,
     QUOTE = 591,
     RANDOM = 592,
     RD = 593,
     READ = 594,
     READY = 595,
     RECORD = 596,
     RECORDING = 597,
     RECORDS = 598,
     RECORD_OVERFLOW = 599,
     RECURSIVE = 600,
     REDEFINES = 601,
     REEL = 602,
     REFERENCE = 603,
     RELATIVE = 604,
     RELEASE = 605,
     REMAINDER = 606,
     REMOVAL = 607,
     RENAMES = 608,
     REORG_CRITERIA = 609,
     REPLACING = 610,
     REPORT = 611,
     REPORTING = 612,
     REPORTS = 613,
     REPOSITORY = 614,
     REQUIRED = 615,
     RESERVE = 616,
     RESET = 617,
     RETURN = 618,
     RETURNING = 619,
     REVERSED = 620,
     REVERSE_FUNC = 621,
     REVERSE_VIDEO = 622,
     REWIND = 623,
     REWRITE = 624,
     RIGHT = 625,
     ROLLBACK = 626,
     ROUNDED = 627,
     RUN = 628,
     SAME = 629,
     SCREEN = 630,
     SCREEN_CONTROL = 631,
     SCROLL = 632,
     SD = 633,
     SEARCH = 634,
     SECTION = 635,
     SECURE = 636,
     SEGMENT_LIMIT = 637,
     SELECT = 638,
     SEMI_COLON = 639,
     SENTENCE = 640,
     SEPARATE = 641,
     SEQUENCE = 642,
     SEQUENTIAL = 643,
     SET = 644,
     SHARING = 645,
     SIGN = 646,
     SIGNED = 647,
     SIGNED_INT = 648,
     SIGNED_LONG = 649,
     SIGNED_SHORT = 650,
     SIZE = 651,
     SIZE_ERROR = 652,
     SORT = 653,
     SORT_MERGE = 654,
     SOURCE = 655,
     SOURCE_COMPUTER = 656,
     SPACE = 657,
     SPECIAL_NAMES = 658,
     STANDARD = 659,
     STANDARD_1 = 660,
     STANDARD_2 = 661,
     START = 662,
     STATIC = 663,
     STATUS = 664,
     STDCALL = 665,
     STOP = 666,
     STRING = 667,
     SUBSTITUTE_FUNC = 668,
     SUBSTITUTE_CASE_FUNC = 669,
     SUBTRACT = 670,
     SUCCESS = 671,
     SUM = 672,
     SUPPRESS = 673,
     SYMBOLIC = 674,
     SYNCHRONIZED = 675,
     TAB = 676,
     TALLYING = 677,
     TAPE = 678,
     TERMINATE = 679,
     TEST = 680,
     THAN = 681,
     THEN = 682,
     THRU = 683,
     TIME = 684,
     TIMEOUT = 685,
     TIMES = 686,
     TO = 687,
     TOK_FALSE = 688,
     TOK_FILE = 689,
     TOK_INITIAL = 690,
     TOK_NULL = 691,
     TOK_TRUE = 692,
     TOP = 693,
     TRAILING = 694,
     TRACE = 695,
     TRANSFORM = 696,
     TRIM_FUNCTION = 697,
     TRIML_FUNCTION = 698,
     TRIMR_FUNCTION = 699,
     TYPE = 700,
     TYPEDEF = 701,
     UNDERLINE = 702,
     UNIT = 703,
     UNLOCK = 704,
     UNSIGNED = 705,
     UNSIGNED_INT = 706,
     UNSIGNED_LONG = 707,
     UNSIGNED_SHORT = 708,
     UNSTRING = 709,
     UNTIL = 710,
     UP = 711,
     UPDATE = 712,
     UPON = 713,
     UPON_ARGUMENT_NUMBER = 714,
     UPON_COMMAND_LINE = 715,
     UPON_ENVIRONMENT_NAME = 716,
     UPON_ENVIRONMENT_VALUE = 717,
     UPPER_CASE_FUNC = 718,
     USAGE = 719,
     USE = 720,
     USING = 721,
     VALUE = 722,
     VARYING = 723,
     WAIT = 724,
     WHEN = 725,
     WHEN_COMPILED_FUNC = 726,
     WITH = 727,
     WORD = 728,
     WORDS = 729,
     WORKING_STORAGE = 730,
     WRITE = 731,
     WRITE_ONLY = 732,
     XML = 733,
     YYYYDDD = 734,
     YYYYMMDD = 735,
     ZERO = 736,
     UNARY_SIGN = 737
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 974 "parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   8829

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  496
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  784
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1798
/* YYNRULES -- Number of states.  */
#define YYNSTATES  2618

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   737

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   495,     2,
     490,   491,   484,   482,     2,   483,   488,   485,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   494,     2,
     493,   489,   492,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   487,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   486
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    12,    13,    14,    24,
      25,    27,    30,    31,    32,    37,    38,    43,    44,    45,
      54,    55,    62,    63,    65,    66,    68,    71,    73,    75,
      77,    78,    81,    82,    86,    88,    91,    93,    95,    97,
      99,   100,   103,   107,   109,   111,   113,   116,   117,   122,
     124,   126,   129,   131,   133,   135,   137,   141,   142,   145,
     149,   152,   156,   158,   160,   163,   167,   168,   171,   175,
     178,   180,   183,   185,   187,   189,   195,   197,   199,   204,
     208,   212,   213,   216,   218,   221,   225,   229,   231,   234,
     238,   239,   242,   244,   247,   249,   251,   253,   255,   257,
     259,   261,   263,   265,   267,   269,   271,   275,   279,   282,
     284,   287,   291,   292,   298,   303,   304,   310,   312,   314,
     315,   321,   323,   325,   327,   329,   331,   333,   336,   338,
     342,   343,   348,   350,   354,   356,   358,   360,   362,   364,
     366,   368,   370,   372,   374,   376,   378,   380,   382,   386,
     388,   391,   395,   397,   400,   402,   405,   410,   412,   415,
     417,   421,   426,   431,   435,   439,   444,   448,   452,   454,
     457,   461,   463,   465,   467,   470,   473,   475,   476,   483,
     484,   487,   489,   491,   493,   495,   497,   499,   501,   503,
     505,   507,   509,   511,   513,   515,   521,   526,   531,   532,
     534,   536,   538,   539,   541,   543,   545,   547,   549,   554,
     556,   558,   560,   567,   571,   577,   578,   580,   582,   587,
     590,   593,   595,   596,   601,   607,   610,   612,   614,   617,
     621,   623,   625,   629,   632,   634,   637,   642,   647,   653,
     655,   659,   664,   669,   673,   676,   680,   683,   686,   689,
     691,   694,   697,   699,   702,   704,   707,   709,   711,   713,
     719,   720,   722,   724,   726,   732,   734,   737,   740,   741,
     744,   749,   751,   753,   755,   759,   760,   761,   766,   770,
     771,   774,   776,   778,   780,   782,   784,   786,   789,   793,
     794,   798,   801,   803,   805,   806,   811,   812,   815,   818,
     821,   823,   825,   827,   829,   831,   833,   835,   837,   839,
     845,   846,   848,   850,   855,   862,   872,   873,   877,   878,
     881,   882,   885,   889,   891,   893,   894,   896,   902,   908,
     910,   912,   914,   916,   917,   922,   928,   929,   932,   934,
     936,   938,   945,   951,   957,   962,   966,   970,   974,   975,
     982,   983,   986,   987,   990,   994,   995,   996,  1003,  1006,
    1008,  1010,  1011,  1013,  1015,  1017,  1018,  1021,  1023,  1027,
    1031,  1038,  1039,  1042,  1044,  1046,  1048,  1050,  1052,  1054,
    1056,  1058,  1060,  1062,  1064,  1066,  1068,  1070,  1072,  1075,
    1078,  1082,  1083,  1086,  1089,  1091,  1093,  1097,  1101,  1104,
    1106,  1108,  1110,  1112,  1114,  1116,  1118,  1120,  1122,  1124,
    1126,  1128,  1130,  1132,  1134,  1136,  1138,  1140,  1142,  1144,
    1146,  1149,  1152,  1154,  1157,  1160,  1162,  1165,  1168,  1170,
    1173,  1176,  1178,  1181,  1184,  1186,  1188,  1190,  1194,  1198,
    1204,  1205,  1207,  1209,  1212,  1214,  1216,  1218,  1219,  1222,
    1226,  1228,  1233,  1235,  1238,  1240,  1242,  1246,  1248,  1251,
    1253,  1256,  1259,  1260,  1262,  1264,  1268,  1270,  1272,  1273,
    1282,  1283,  1292,  1294,  1297,  1299,  1303,  1305,  1306,  1310,
    1313,  1318,  1321,  1322,  1328,  1329,  1335,  1336,  1337,  1343,
    1344,  1346,  1348,  1351,  1357,  1358,  1361,  1364,  1368,  1370,
    1372,  1376,  1380,  1383,  1384,  1386,  1387,  1389,  1391,  1393,
    1395,  1397,  1399,  1401,  1403,  1405,  1408,  1409,  1412,  1418,
    1419,  1422,  1430,  1434,  1439,  1447,  1452,  1454,  1459,  1462,
    1465,  1466,  1469,  1474,  1475,  1478,  1480,  1482,  1484,  1486,
    1488,  1490,  1492,  1494,  1496,  1498,  1500,  1502,  1504,  1506,
    1508,  1510,  1514,  1517,  1520,  1523,  1525,  1528,  1531,  1534,
    1539,  1544,  1547,  1549,  1552,  1555,  1557,  1559,  1562,  1566,
    1568,  1572,  1579,  1582,  1586,  1589,  1591,  1594,  1597,  1599,
    1602,  1603,  1605,  1607,  1612,  1615,  1616,  1618,  1620,  1621,
    1622,  1623,  1630,  1631,  1633,  1635,  1638,  1640,  1641,  1647,
    1648,  1651,  1653,  1655,  1657,  1660,  1662,  1665,  1668,  1670,
    1672,  1674,  1676,  1678,  1680,  1682,  1684,  1686,  1688,  1690,
    1692,  1697,  1703,  1709,  1713,  1717,  1719,  1721,  1723,  1725,
    1727,  1729,  1731,  1734,  1737,  1740,  1744,  1748,  1749,  1751,
    1753,  1755,  1757,  1758,  1760,  1762,  1764,  1766,  1770,  1771,
    1772,  1773,  1784,  1785,  1786,  1790,  1791,  1795,  1797,  1800,
    1805,  1806,  1809,  1812,  1815,  1816,  1818,  1819,  1823,  1827,
    1832,  1836,  1837,  1839,  1840,  1843,  1844,  1845,  1853,  1854,
    1857,  1859,  1861,  1863,  1865,  1868,  1871,  1873,  1878,  1880,
    1883,  1885,  1887,  1890,  1891,  1893,  1894,  1895,  1899,  1900,
    1902,  1903,  1906,  1909,  1911,  1913,  1915,  1917,  1919,  1921,
    1923,  1925,  1927,  1929,  1931,  1933,  1935,  1937,  1939,  1941,
    1943,  1945,  1947,  1949,  1951,  1953,  1955,  1957,  1959,  1961,
    1963,  1965,  1967,  1969,  1971,  1973,  1975,  1977,  1979,  1981,
    1983,  1985,  1987,  1989,  1991,  1993,  1995,  1997,  1999,  2001,
    2003,  2005,  2007,  2009,  2011,  2013,  2015,  2017,  2019,  2022,
    2023,  2028,  2032,  2037,  2041,  2045,  2049,  2054,  2058,  2062,
    2067,  2071,  2075,  2079,  2083,  2088,  2094,  2098,  2103,  2107,
    2111,  2114,  2117,  2120,  2125,  2130,  2135,  2136,  2138,  2140,
    2143,  2145,  2148,  2151,  2154,  2158,  2161,  2165,  2169,  2172,
    2175,  2178,  2181,  2184,  2187,  2190,  2195,  2200,  2205,  2210,
    2213,  2216,  2219,  2222,  2225,  2228,  2231,  2237,  2241,  2242,
    2244,  2245,  2250,  2255,  2261,  2268,  2269,  2272,  2273,  2275,
    2276,  2280,  2284,  2290,  2291,  2294,  2295,  2299,  2300,  2306,
    2307,  2310,  2311,  2321,  2322,  2332,  2333,  2336,  2338,  2340,
    2341,  2343,  2344,  2349,  2353,  2356,  2357,  2359,  2361,  2362,
    2364,  2365,  2367,  2368,  2372,  2374,  2377,  2380,  2386,  2387,
    2390,  2393,  2396,  2399,  2402,  2406,  2411,  2413,  2415,  2416,
    2417,  2421,  2422,  2423,  2427,  2428,  2430,  2431,  2433,  2434,
    2438,  2439,  2442,  2443,  2447,  2448,  2449,  2455,  2456,  2458,
    2461,  2462,  2464,  2467,  2470,  2471,  2474,  2475,  2479,  2480,
    2484,  2485,  2487,  2491,  2495,  2498,  2500,  2502,  2503,  2508,
    2513,  2514,  2516,  2518,  2520,  2522,  2524,  2525,  2532,  2533,
    2539,  2540,  2542,  2543,  2548,  2552,  2556,  2560,  2564,  2567,
    2569,  2572,  2574,  2576,  2579,  2582,  2585,  2588,  2591,  2594,
    2597,  2601,  2604,  2608,  2612,  2615,  2618,  2621,  2624,  2627,
    2632,  2637,  2642,  2647,  2650,  2653,  2654,  2656,  2657,  2662,
    2667,  2674,  2681,  2690,  2699,  2700,  2702,  2703,  2710,  2711,
    2713,  2714,  2718,  2720,  2723,  2726,  2732,  2737,  2738,  2741,
    2742,  2748,  2750,  2754,  2756,  2758,  2760,  2762,  2764,  2767,
    2768,  2772,  2773,  2775,  2778,  2782,  2784,  2786,  2788,  2792,
    2795,  2797,  2799,  2801,  2802,  2805,  2806,  2808,  2809,  2813,
    2814,  2815,  2819,  2821,  2824,  2826,  2828,  2829,  2832,  2833,
    2834,  2842,  2844,  2847,  2849,  2852,  2853,  2855,  2857,  2859,
    2862,  2863,  2866,  2869,  2872,  2875,  2878,  2879,  2881,  2882,
    2886,  2887,  2891,  2892,  2898,  2899,  2903,  2904,  2908,  2909,
    2912,  2913,  2917,  2918,  2923,  2925,  2927,  2930,  2934,  2939,
    2943,  2946,  2947,  2949,  2950,  2958,  2959,  2962,  2963,  2967,
    2971,  2972,  2975,  2977,  2980,  2985,  2987,  2989,  2991,  2993,
    2995,  2997,  2999,  3000,  3002,  3003,  3007,  3008,  3013,  3015,
    3017,  3019,  3021,  3024,  3026,  3028,  3030,  3031,  3035,  3037,
    3040,  3043,  3046,  3048,  3050,  3052,  3055,  3058,  3060,  3063,
    3068,  3071,  3072,  3074,  3076,  3078,  3080,  3085,  3092,  3093,
    3098,  3099,  3101,  3102,  3106,  3107,  3111,  3115,  3120,  3121,
    3126,  3131,  3138,  3139,  3141,  3142,  3146,  3147,  3154,  3156,
    3158,  3160,  3162,  3163,  3167,  3168,  3172,  3175,  3176,  3178,
    3179,  3183,  3186,  3187,  3192,  3195,  3196,  3198,  3200,  3204,
    3205,  3207,  3210,  3214,  3218,  3219,  3223,  3225,  3227,  3229,
    3233,  3241,  3242,  3247,  3249,  3252,  3253,  3255,  3256,  3268,
    3269,  3272,  3273,  3276,  3279,  3283,  3287,  3290,  3294,  3295,
    3298,  3301,  3305,  3309,  3312,  3316,  3317,  3321,  3322,  3324,
    3326,  3327,  3329,  3332,  3335,  3336,  3341,  3342,  3350,  3351,
    3353,  3354,  3362,  3363,  3366,  3370,  3371,  3373,  3375,  3376,
    3381,  3386,  3387,  3395,  3396,  3399,  3400,  3401,  3406,  3408,
    3411,  3412,  3417,  3418,  3420,  3421,  3425,  3427,  3429,  3431,
    3433,  3435,  3440,  3445,  3449,  3454,  3456,  3458,  3460,  3463,
    3467,  3469,  3472,  3476,  3480,  3481,  3485,  3486,  3494,  3495,
    3501,  3502,  3505,  3506,  3509,  3510,  3514,  3515,  3518,  3523,
    3524,  3527,  3532,  3533,  3534,  3543,  3544,  3549,  3552,  3555,
    3558,  3561,  3564,  3565,  3567,  3568,  3572,  3575,  3577,  3580,
    3581,  3583,  3586,  3589,  3590,  3599,  3601,  3604,  3606,  3610,
    3614,  3615,  3619,  3620,  3622,  3623,  3628,  3633,  3640,  3647,
    3648,  3650,  3653,  3654,  3656,  3657,  3661,  3662,  3670,  3671,
    3676,  3677,  3679,  3681,  3682,  3692,  3693,  3697,  3699,  3703,
    3706,  3709,  3712,  3716,  3717,  3721,  3722,  3726,  3727,  3731,
    3732,  3734,  3736,  3738,  3740,  3749,  3750,  3752,  3754,  3756,
    3758,  3760,  3762,  3763,  3765,  3766,  3768,  3770,  3772,  3774,
    3776,  3778,  3780,  3781,  3783,  3789,  3791,  3794,  3800,  3801,
    3810,  3811,  3814,  3815,  3820,  3824,  3828,  3830,  3832,  3833,
    3835,  3837,  3838,  3840,  3843,  3846,  3849,  3850,  3851,  3856,
    3857,  3858,  3862,  3863,  3865,  3868,  3869,  3870,  3874,  3875,
    3876,  3880,  3883,  3884,  3885,  3889,  3890,  3891,  3895,  3897,
    3899,  3902,  3903,  3907,  3908,  3912,  3914,  3916,  3919,  3920,
    3924,  3925,  3929,  3930,  3932,  3934,  3936,  3939,  3940,  3944,
    3945,  3949,  3950,  3956,  3965,  3971,  3973,  3975,  3978,  3979,
    3983,  3984,  3986,  3987,  3990,  3991,  3994,  3995,  3999,  4001,
    4003,  4004,  4007,  4009,  4012,  4014,  4016,  4018,  4020,  4022,
    4024,  4026,  4028,  4030,  4032,  4034,  4036,  4038,  4040,  4042,
    4044,  4046,  4048,  4051,  4054,  4057,  4060,  4063,  4066,  4068,
    4070,  4072,  4074,  4076,  4078,  4080,  4082,  4084,  4086,  4088,
    4090,  4092,  4094,  4097,  4099,  4101,  4104,  4106,  4109,  4111,
    4117,  4119,  4125,  4127,  4131,  4133,  4135,  4137,  4141,  4142,
    4144,  4146,  4148,  4152,  4156,  4160,  4164,  4167,  4170,  4174,
    4178,  4182,  4186,  4190,  4193,  4195,  4199,  4201,  4204,  4207,
    4209,  4211,  4213,  4216,  4218,  4220,  4223,  4225,  4226,  4229,
    4231,  4233,  4235,  4239,  4241,  4243,  4246,  4248,  4249,  4251,
    4253,  4255,  4257,  4258,  4260,  4262,  4265,  4269,  4271,  4275,
    4277,  4280,  4282,  4286,  4290,  4294,  4299,  4303,  4305,  4307,
    4309,  4311,  4315,  4319,  4323,  4325,  4327,  4329,  4331,  4333,
    4335,  4337,  4339,  4341,  4343,  4345,  4347,  4349,  4350,  4352,
    4354,  4356,  4358,  4359,  4361,  4363,  4365,  4367,  4369,  4371,
    4373,  4374,  4376,  4378,  4380,  4383,  4386,  4390,  4392,  4396,
    4398,  4402,  4405,  4409,  4414,  4420,  4422,  4424,  4426,  4429,
    4431,  4435,  4437,  4439,  4441,  4443,  4445,  4447,  4449,  4451,
    4454,  4457,  4463,  4469,  4475,  4481,  4487,  4493,  4499,  4505,
    4511,  4516,  4522,  4528,  4534,  4537,  4538,  4543,  4549,  4550,
    4554,  4555,  4557,  4559,  4563,  4567,  4569,  4573,  4575,  4579,
    4580,  4581,  4583,  4584,  4586,  4587,  4589,  4590,  4592,  4594,
    4595,  4597,  4598,  4601,  4603,  4604,  4606,  4607,  4610,  4612,
    4614,  4617,  4620,  4623,  4626,  4629,  4631,  4634,  4636,  4637,
    4639,  4640,  4642,  4643,  4645,  4646,  4648,  4649,  4651,  4652,
    4654,  4655,  4657,  4658,  4660,  4661,  4663,  4664,  4666,  4667,
    4669,  4670,  4672,  4673,  4675,  4676,  4678,  4679,  4681,  4682,
    4684,  4685,  4687,  4688,  4690,  4691,  4693,  4694,  4696,  4698,
    4699,  4701,  4702,  4704,  4705,  4707,  4709,  4710,  4712,  4713,
    4715,  4716,  4718,  4719,  4721,  4722,  4724,  4725,  4727,  4728,
    4730,  4733,  4734,  4736,  4737,  4739,  4740,  4742,  4743,  4745,
    4746,  4748,  4750,  4751,  4753,  4754,  4756,  4757,  4759,  4760,
    4762,  4765,  4766,  4768,  4769,  4771,  4772,  4774,  4775,  4777,
    4778,  4780,  4781,  4783,  4784,  4786,  4787,  4789,  4790
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     497,     0,    -1,    -1,   498,   499,    -1,   500,    -1,   499,
     500,    -1,    -1,    -1,   507,   518,   501,   623,   502,   770,
     503,   504,   995,    -1,    -1,   500,    -1,   503,   500,    -1,
      -1,    -1,   157,   505,  1264,   488,    -1,    -1,   153,   506,
    1264,   488,    -1,    -1,    -1,   331,   511,   508,   513,   514,
     509,   515,   511,    -1,    -1,   203,   511,   510,   513,   514,
     511,    -1,    -1,   488,    -1,    -1,   488,    -1,   512,   488,
      -1,   332,    -1,   256,    -1,   133,    -1,    -1,    28,   256,
      -1,    -1,  1250,   516,  1263,    -1,    82,    -1,    82,   517,
      -1,   187,    -1,   517,    -1,   435,    -1,   345,    -1,    -1,
     518,   519,    -1,   169,   132,   488,    -1,   520,    -1,   577,
      -1,   521,    -1,   520,   521,    -1,    -1,    93,   380,   488,
     522,    -1,   523,    -1,   524,    -1,   523,   524,    -1,   525,
      -1,   530,    -1,   543,    -1,   538,    -1,   401,   488,   526,
      -1,    -1,   528,   488,    -1,   528,   527,   488,    -1,   527,
     488,    -1,  1279,   115,   269,    -1,   529,    -1,   473,    -1,
     529,   473,    -1,   295,   488,   531,    -1,    -1,   528,   488,
      -1,   528,   532,   488,    -1,   532,   488,    -1,   533,    -1,
     532,   533,    -1,   534,    -1,   536,    -1,   537,    -1,   265,
     396,  1250,  1207,   535,    -1,    67,    -1,   474,    -1,  1263,
    1231,  1250,  1181,    -1,   382,  1250,  1207,    -1,   359,   488,
     539,    -1,    -1,   540,   488,    -1,   541,    -1,   540,   541,
      -1,   202,   542,  1249,    -1,   202,    10,   231,    -1,   473,
      -1,   542,   473,    -1,   403,   488,   544,    -1,    -1,   545,
     488,    -1,   546,    -1,   545,   546,    -1,   549,    -1,   553,
      -1,   562,    -1,   570,    -1,   567,    -1,   571,    -1,   572,
      -1,   573,    -1,   574,    -1,   575,    -1,   576,    -1,   547,
      -1,   548,   243,  1227,    -1,   548,   439,  1227,    -1,   292,
    1270,    -1,   391,    -1,   391,   234,    -1,   473,  1250,   105,
      -1,    -1,   473,  1250,  1185,   550,   551,    -1,   473,   256,
    1250,  1184,    -1,    -1,   551,   552,  1272,  1250,  1184,    -1,
     300,    -1,   298,    -1,    -1,    12,  1184,   554,  1250,   555,
      -1,   277,    -1,   405,    -1,   406,    -1,   138,    -1,   556,
      -1,   557,    -1,   556,   557,    -1,   560,    -1,   560,   428,
     560,    -1,    -1,   560,    18,   558,   559,    -1,   561,    -1,
     559,    18,   561,    -1,   256,    -1,   402,    -1,   481,    -1,
     336,    -1,   126,    -1,   215,    -1,   263,    -1,   256,    -1,
     402,    -1,   481,    -1,   336,    -1,   126,    -1,   215,    -1,
     263,    -1,   419,  1241,   563,    -1,   564,    -1,   564,   563,
      -1,   565,  1251,   566,    -1,  1184,    -1,   565,  1184,    -1,
    1207,    -1,   566,  1207,    -1,    69,  1184,  1250,   568,    -1,
     569,    -1,   568,   569,    -1,  1211,    -1,  1211,   428,  1211,
      -1,   257,  1184,  1250,  1181,    -1,   106,  1269,  1250,   256,
      -1,   116,  1250,    78,    -1,   108,  1250,  1181,    -1,   105,
     409,  1250,  1181,    -1,   376,  1250,  1181,    -1,   181,  1250,
    1181,    -1,   578,    -1,   577,   578,    -1,   228,   380,   488,
      -1,   579,    -1,   610,    -1,   580,    -1,   579,   580,    -1,
     190,   488,    -1,   581,    -1,    -1,   383,  1225,  1184,   582,
     583,   488,    -1,    -1,   583,   584,    -1,   585,    -1,   589,
      -1,   591,    -1,   592,    -1,   593,    -1,   595,    -1,   599,
      -1,   600,    -1,   602,    -1,   603,    -1,   604,    -1,   606,
      -1,   607,    -1,   608,    -1,    30,  1277,   587,   586,   588,
      -1,    30,  1277,   587,   127,    -1,    30,  1277,   587,   325,
      -1,    -1,   127,    -1,   325,    -1,   599,    -1,    -1,   187,
      -1,   136,    -1,   256,    -1,   128,    -1,  1204,    -1,     5,
    1256,  1250,   590,    -1,   388,    -1,   136,    -1,   337,    -1,
      20,  1265,  1252,  1250,   605,  1221,    -1,  1231,  1250,   473,
      -1,   594,   409,  1250,  1181,  1182,    -1,    -1,   434,    -1,
     398,    -1,   260,  1256,  1250,   596,    -1,   264,   597,    -1,
      33,   597,    -1,   183,    -1,    -1,   472,   260,   300,   598,
      -1,   472,   260,   300,   271,   598,    -1,   472,   371,    -1,
     341,    -1,   343,    -1,   253,     8,    -1,   306,  1250,   601,
      -1,   601,    -1,   222,    -1,   341,  1238,   388,    -1,  1238,
     388,    -1,   349,    -1,   253,   388,    -1,   312,  1240,  1250,
    1183,    -1,   341,   121,  1250,   405,    -1,   341,  1252,  1250,
     605,  1221,    -1,  1181,    -1,  1184,   489,  1180,    -1,  1184,
     400,  1250,  1180,    -1,   349,  1252,  1250,  1181,    -1,   361,
    1207,  1235,    -1,   361,   281,    -1,   390,  1279,   609,    -1,
      10,  1262,    -1,   281,  1262,    -1,   339,   301,    -1,   611,
      -1,   610,   611,    -1,   236,   488,    -1,   612,    -1,   613,
     488,    -1,   614,    -1,   613,   614,    -1,   615,    -1,   617,
      -1,   621,    -1,   374,   616,  1235,  1246,  1172,    -1,    -1,
     341,    -1,   398,    -1,   399,    -1,   271,  1245,  1273,  1242,
     618,    -1,   619,    -1,   618,   619,    -1,  1173,   620,    -1,
      -1,   320,  1207,    -1,    23,   622,  1259,  1172,    -1,   477,
      -1,   102,    -1,   344,    -1,   354,  1277,  1201,    -1,    -1,
      -1,   626,   624,   716,   758,    -1,   110,   132,   511,    -1,
      -1,   626,   627,    -1,   625,    -1,   628,    -1,   660,    -1,
     712,    -1,   714,    -1,   629,    -1,   628,   629,    -1,   434,
     380,   488,    -1,    -1,   632,   630,   631,    -1,   633,   662,
      -1,   189,    -1,   378,    -1,    -1,  1173,   634,   635,   488,
      -1,    -1,   635,   636,    -1,  1250,   187,    -1,  1250,   208,
      -1,   637,    -1,   639,    -1,   643,    -1,   646,    -1,   649,
      -1,   651,    -1,   657,    -1,   658,    -1,   659,    -1,    53,
    1242,  1207,   642,   638,    -1,    -1,    67,    -1,   343,    -1,
     341,  1242,  1207,  1241,    -1,   341,  1242,  1207,   432,  1207,
    1241,    -1,   341,  1250,   468,  1248,  1271,   641,   642,  1241,
     640,    -1,    -1,   122,  1259,  1181,    -1,    -1,  1247,  1207,
      -1,    -1,   432,  1207,    -1,   240,   645,   644,    -1,   404,
      -1,   299,    -1,    -1,  1229,    -1,   467,   297,   473,  1250,
     648,    -1,   467,   297,   647,  1250,   648,    -1,   191,    -1,
     216,    -1,   256,    -1,  1204,    -1,    -1,   110,   650,  1229,
    1180,    -1,   251,  1250,  1183,  1255,   652,    -1,    -1,   652,
     653,    -1,   654,    -1,   655,    -1,   656,    -1,  1279,   195,
    1255,  1237,  1183,  1255,    -1,  1255,  1237,   438,  1183,  1255,
      -1,  1255,  1237,    54,  1183,  1255,    -1,   342,  1256,  1250,
     473,    -1,    72,  1250,   473,    -1,   356,  1250,   757,    -1,
     358,  1234,   757,    -1,    -1,   475,   380,   511,   661,   662,
     511,    -1,    -1,   663,   664,    -1,    -1,  1219,   665,    -1,
     664,  1219,   665,    -1,    -1,    -1,   668,   669,   666,   674,
     667,   512,    -1,   673,   512,    -1,   473,    -1,   256,    -1,
      -1,   192,    -1,   473,    -1,   473,    -1,    -1,  1250,   208,
      -1,  1209,    -1,   245,  1258,  1202,    -1,    56,  1258,  1202,
      -1,   668,   670,    94,   671,  1236,   672,    -1,    -1,   674,
     675,    -1,   677,    -1,   678,    -1,   680,    -1,   681,    -1,
     682,    -1,   684,    -1,   685,    -1,   698,    -1,   699,    -1,
     701,    -1,   702,    -1,   705,    -1,   710,    -1,   711,    -1,
     676,    -1,  1250,   446,    -1,   346,  1202,    -1,  1250,   187,
     679,    -1,    -1,    28,   256,    -1,  1250,   208,    -1,   317,
      -1,   683,    -1,   464,  1250,   683,    -1,   464,  1250,   473,
      -1,   248,  1201,    -1,    42,    -1,    48,    -1,    83,    -1,
      85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,
      90,    -1,    91,    -1,   128,    -1,   221,    -1,   311,    -1,
     319,    -1,   333,    -1,   395,    -1,   393,    -1,   394,    -1,
     453,    -1,   451,    -1,   452,    -1,    44,   392,    -1,    44,
     450,    -1,    44,    -1,    47,   392,    -1,    47,   450,    -1,
      47,    -1,    46,   392,    -1,    46,   450,    -1,    46,    -1,
      45,   392,    -1,    45,   450,    -1,    45,    -1,    43,   392,
      -1,    43,   450,    -1,    43,    -1,   274,    -1,   129,    -1,
    1270,   243,  1227,    -1,  1270,   439,  1227,    -1,   296,  1207,
     689,  1276,   686,    -1,    -1,   687,    -1,   688,    -1,   687,
     688,    -1,   690,    -1,   691,    -1,   695,    -1,    -1,   432,
    1207,    -1,   122,  1259,  1181,    -1,   693,    -1,   694,  1252,
    1250,  1180,    -1,   692,    -1,   693,   692,    -1,    29,    -1,
     123,    -1,   222,  1239,   696,    -1,   697,    -1,   696,   697,
      -1,   473,    -1,   237,  1267,    -1,   420,   700,    -1,    -1,
     244,    -1,   370,    -1,    49,  1278,   481,    -1,    35,    -1,
     707,    -1,    -1,   467,  1251,   707,   704,  1278,  1268,  1277,
     709,    -1,    -1,   467,  1251,   707,   706,  1278,  1268,  1277,
     709,    -1,   708,    -1,   707,   708,    -1,  1209,    -1,  1209,
     428,  1209,    -1,  1201,    -1,    -1,   433,  1250,  1209,    -1,
     353,  1204,    -1,   353,  1204,   428,  1204,    -1,    22,   245,
      -1,    -1,   259,   380,   488,   713,   662,    -1,    -1,   255,
     380,   488,   715,   662,    -1,    -1,    -1,   356,   380,   488,
     717,   718,    -1,    -1,   719,    -1,   720,    -1,   719,   720,
      -1,   338,   757,   721,   488,   736,    -1,    -1,   721,   722,
      -1,  1250,   208,    -1,    71,  1250,  1196,    -1,   723,    -1,
     732,    -1,    98,  1250,   724,    -1,    99,  1234,   724,    -1,
     725,   731,    -1,    -1,   193,    -1,    -1,   249,    -1,   250,
      -1,   253,    -1,   254,    -1,    75,    -1,    77,    -1,   114,
      -1,   125,    -1,    62,    -1,    98,   213,    -1,    -1,   731,
    1201,    -1,   313,   726,  1251,   735,   733,    -1,    -1,   733,
     734,    -1,   213,  1250,  1207,   194,   729,  1250,  1207,    -1,
     213,  1250,  1207,    -1,   241,   730,  1250,  1207,    -1,   241,
     729,  1250,  1207,   195,  1250,  1207,    -1,   241,   729,  1250,
    1207,    -1,  1207,    -1,  1207,   727,  1207,   728,    -1,  1207,
     727,    -1,  1207,   728,    -1,    -1,   736,   737,    -1,   668,
     669,   738,   488,    -1,    -1,   738,   739,    -1,   740,    -1,
     742,    -1,   749,    -1,   681,    -1,   682,    -1,   684,    -1,
     698,    -1,   743,    -1,   701,    -1,   754,    -1,   744,    -1,
     705,    -1,   747,    -1,   755,    -1,   685,    -1,   748,    -1,
     445,  1250,   741,    -1,   356,   213,    -1,   313,   213,    -1,
      98,   213,    -1,   125,    -1,    98,   195,    -1,   313,   195,
      -1,   356,   195,    -1,   280,   212,  1250,  1207,    -1,    76,
    1257,  1250,  1207,    -1,    76,   291,    -1,    77,    -1,    74,
     290,    -1,    74,   291,    -1,    75,    -1,   745,    -1,   744,
     745,    -1,   417,  1258,   746,    -1,  1181,    -1,   322,   470,
    1151,    -1,   468,  1201,   200,  1189,    55,  1189,    -1,   750,
     751,    -1,   253,   753,  1251,    -1,   254,  1234,    -1,   752,
      -1,   751,   752,    -1,   318,  1207,    -1,  1207,    -1,   280,
     313,    -1,    -1,   290,    -1,   291,    -1,   400,  1250,  1201,
    1226,    -1,   212,   756,    -1,    -1,   223,    -1,   473,    -1,
      -1,    -1,    -1,   375,   380,   488,   759,   760,   761,    -1,
      -1,   762,    -1,   763,    -1,   762,   763,    -1,   673,    -1,
      -1,   668,   669,   764,   765,   488,    -1,    -1,   765,   766,
      -1,    50,    -1,    51,    -1,    37,    -1,   281,    37,    -1,
      52,    -1,   177,   172,    -1,   177,   174,    -1,   214,    -1,
     262,    -1,   367,    -1,   447,    -1,   310,    -1,    32,    -1,
     421,    -1,   381,    -1,   360,    -1,   201,    -1,   463,    -1,
     261,    -1,   334,  1240,  1250,  1200,    -1,   253,  1257,  1250,
     767,  1197,    -1,    76,  1257,  1250,   768,  1197,    -1,   197,
    1250,  1198,    -1,    34,  1250,  1198,    -1,   682,    -1,   701,
      -1,   698,    -1,   684,    -1,   703,    -1,   681,    -1,   769,
      -1,   466,  1201,    -1,   200,  1190,    -1,   432,  1201,    -1,
      98,  1250,  1201,    -1,   396,  1250,  1198,    -1,    -1,   318,
      -1,   482,    -1,   267,    -1,   483,    -1,    -1,   318,    -1,
     482,    -1,   267,    -1,   483,    -1,   296,  1207,  1276,    -1,
      -1,    -1,    -1,   327,   132,   829,   773,   782,   488,   771,
     783,   772,   785,    -1,    -1,    -1,   466,   774,   776,    -1,
      -1,    64,   775,   776,    -1,   777,    -1,   776,   777,    -1,
     778,   780,   781,   473,    -1,    -1,  1239,   348,    -1,  1239,
     467,    -1,  1239,   124,    -1,    -1,   120,    -1,    -1,   396,
    1250,    32,    -1,   396,  1250,   118,    -1,   450,   396,  1250,
    1207,    -1,   396,  1250,  1207,    -1,    -1,   303,    -1,    -1,
     364,  1204,    -1,    -1,    -1,   117,   488,   784,   785,   142,
     117,   488,    -1,    -1,   785,   786,    -1,   787,    -1,   789,
      -1,   992,    -1,   790,    -1,   797,   488,    -1,     1,   488,
      -1,   488,    -1,   791,   380,   792,   488,    -1,   473,    -1,
     788,   488,    -1,   473,    -1,   473,    -1,   788,   473,    -1,
      -1,   256,    -1,    -1,    -1,   794,   795,   797,    -1,    -1,
     793,    -1,    -1,   798,   799,    -1,   797,   799,    -1,   800,
      -1,   810,    -1,   815,    -1,   819,    -1,   823,    -1,   847,
      -1,   825,    -1,   850,    -1,   858,    -1,   868,    -1,   863,
      -1,   869,    -1,   870,    -1,   874,    -1,   880,    -1,   884,
      -1,   892,    -1,   907,    -1,   912,    -1,   921,    -1,   923,
      -1,   925,    -1,   928,    -1,   931,    -1,   937,    -1,   946,
      -1,   948,    -1,   965,    -1,   967,    -1,   970,    -1,   974,
      -1,   981,    -1,   992,    -1,   996,    -1,  1004,    -1,  1006,
      -1,  1005,    -1,  1008,    -1,  1011,    -1,  1015,    -1,  1016,
      -1,  1027,    -1,  1038,    -1,  1048,    -1,  1054,    -1,  1058,
      -1,  1064,    -1,  1068,    -1,  1070,    -1,  1072,    -1,  1074,
      -1,  1077,    -1,  1088,    -1,  1101,    -1,  1142,    -1,   280,
     385,    -1,    -1,     4,   801,   802,   809,    -1,  1201,   806,
    1108,    -1,  1201,   200,   179,   239,    -1,  1201,   200,   254,
      -1,  1201,   200,    77,    -1,  1201,   200,   111,    -1,  1201,
     200,   111,   480,    -1,  1201,   200,    60,    -1,  1201,   200,
     112,    -1,  1201,   200,   112,   479,    -1,  1201,   200,    61,
      -1,  1201,   200,   113,    -1,  1201,   200,   429,    -1,  1201,
     200,    79,    -1,  1201,   200,   171,  1108,    -1,  1201,   200,
     169,  1193,  1108,    -1,  1201,   200,    26,    -1,  1201,   200,
      27,  1108,    -1,  1201,   200,  1175,    -1,  1201,   200,   473,
      -1,  1237,   804,    -1,  1237,   805,    -1,    31,  1193,    -1,
     253,  1257,  1190,  1243,    -1,    76,  1257,  1190,  1243,    -1,
     320,  1257,  1190,  1243,    -1,    -1,   807,    -1,   808,    -1,
     807,   808,    -1,   803,    -1,  1279,   100,    -1,  1279,   137,
      -1,  1279,    37,    -1,  1279,   281,    37,    -1,  1279,    52,
      -1,  1279,   177,   172,    -1,  1279,   177,   174,    -1,  1279,
     214,    -1,  1279,   262,    -1,  1279,   367,    -1,  1279,   447,
      -1,  1279,   310,    -1,  1279,   463,    -1,  1279,   261,    -1,
    1279,   197,  1250,  1198,    -1,  1279,    34,  1250,  1198,    -1,
    1279,   377,   456,  1150,    -1,  1279,   377,   134,  1150,    -1,
    1279,    32,    -1,  1279,   421,    -1,  1279,   201,    -1,  1279,
     360,    -1,  1279,   381,    -1,  1279,   457,    -1,  1279,   334,
      -1,  1279,   334,    66,  1250,   256,    -1,   430,  1092,  1198,
      -1,    -1,   143,    -1,    -1,     6,   811,   812,   814,    -1,
    1188,   432,  1168,  1116,    -1,  1188,   813,   207,  1168,  1116,
      -1,   103,  1201,   432,  1201,  1226,  1116,    -1,    -1,   432,
    1189,    -1,    -1,   144,    -1,    -1,    11,   816,   817,    -1,
     473,  1222,   818,    -1,  1152,    67,  1222,   364,  1187,    -1,
      -1,   364,  1187,    -1,    -1,    19,   820,   821,    -1,    -1,
     821,  1177,   432,   822,  1177,    -1,    -1,   329,   432,    -1,
      -1,    57,   824,   829,  1196,   830,   827,   841,   843,   845,
      -1,    -1,    63,   826,   829,  1196,   830,   833,   841,   843,
     846,    -1,    -1,   827,   828,    -1,   834,    -1,   839,    -1,
      -1,  1175,    -1,    -1,   472,   832,   831,  1253,    -1,   408,
     831,  1253,    -1,   410,  1253,    -1,    -1,    59,    -1,   410,
      -1,    -1,   408,    -1,    -1,   834,    -1,    -1,   466,   835,
     836,    -1,   837,    -1,   836,   837,    -1,   838,   299,    -1,
     838,   780,  1189,   780,   779,    -1,    -1,  1239,   348,    -1,
    1239,    96,    -1,  1239,   124,    -1,  1239,   467,    -1,   840,
    1201,    -1,   840,   230,  1201,    -1,   840,     7,  1258,  1201,
      -1,   364,    -1,   207,    -1,    -1,    -1,  1095,   842,   793,
      -1,    -1,    -1,  1096,   844,   793,    -1,    -1,   145,    -1,
      -1,   147,    -1,    -1,    58,   848,   849,    -1,    -1,   849,
    1196,    -1,    -1,    68,   851,   852,    -1,    -1,    -1,  1195,
     853,   855,   854,   857,    -1,    -1,    97,    -1,   184,   856,
      -1,    -1,  1196,    -1,   364,  1196,    -1,   472,  1196,    -1,
      -1,   207,  1201,    -1,    -1,    70,   859,   860,    -1,    -1,
     860,  1173,   861,    -1,    -1,   862,    -1,   862,  1246,   352,
      -1,  1279,   281,   368,    -1,  1279,   260,    -1,   347,    -1,
     448,    -1,    -1,    84,   864,   865,   866,    -1,  1168,   867,
    1152,  1116,    -1,    -1,   146,    -1,   489,    -1,   175,    -1,
      81,    -1,    97,    -1,    -1,   119,   871,  1173,  1265,  1136,
     873,    -1,    -1,   119,   434,   872,  1173,   873,    -1,    -1,
     148,    -1,    -1,   128,   875,   876,   879,    -1,  1189,   461,
    1109,    -1,  1189,   462,  1109,    -1,  1189,   459,  1109,    -1,
    1189,   460,  1109,    -1,   877,  1109,    -1,   878,    -1,   877,
     878,    -1,  1189,    -1,   803,    -1,   458,  1175,    -1,   458,
     473,    -1,   458,   325,    -1,   458,   105,    -1,  1279,   289,
      -1,  1279,   100,    -1,  1279,    37,    -1,  1279,   281,    37,
      -1,  1279,    52,    -1,  1279,   177,   172,    -1,  1279,   177,
     174,    -1,  1279,   214,    -1,  1279,   262,    -1,  1279,   367,
      -1,  1279,   447,    -1,  1279,   310,    -1,  1279,   197,  1250,
    1198,    -1,  1279,    34,  1250,  1198,    -1,  1279,   377,   456,
    1150,    -1,  1279,   377,   134,  1150,    -1,  1279,    50,    -1,
    1279,    51,    -1,    -1,   149,    -1,    -1,   131,   881,   882,
     883,    -1,  1189,   230,  1168,  1116,    -1,  1189,   230,  1189,
     207,  1168,  1116,    -1,  1189,    55,  1189,   207,  1168,  1116,
      -1,  1189,   230,  1189,   207,  1169,   351,  1169,  1116,    -1,
    1189,    55,  1189,   207,  1169,   351,  1169,  1116,    -1,    -1,
     150,    -1,    -1,   168,   885,   256,   829,   886,   891,    -1,
      -1,   887,    -1,    -1,   466,   888,   889,    -1,   890,    -1,
     889,   890,    -1,   838,   299,    -1,   838,   780,  1204,   780,
     779,    -1,   838,   780,    22,   780,    -1,    -1,   364,  1204,
      -1,    -1,   180,   893,   894,   896,   906,    -1,   895,    -1,
     894,    18,   895,    -1,  1152,    -1,   437,    -1,   433,    -1,
     897,    -1,   898,    -1,   897,   898,    -1,    -1,   901,   899,
     900,    -1,    -1,   793,    -1,   470,   902,    -1,   901,   470,
     902,    -1,   307,    -1,   903,    -1,   904,    -1,   903,    18,
     904,    -1,  1153,   905,    -1,    22,    -1,   437,    -1,   433,
      -1,    -1,   428,  1152,    -1,    -1,   151,    -1,    -1,   184,
     908,   909,    -1,    -1,    -1,   330,   910,   911,    -1,   316,
      -1,   316,   109,    -1,   380,    -1,   314,    -1,    -1,  1266,
    1189,    -1,    -1,    -1,   185,   913,   916,   915,   918,   914,
     920,    -1,  1189,    -1,   915,  1189,    -1,   917,    -1,   916,
     917,    -1,    -1,   273,    -1,    65,    -1,   919,    -1,   918,
     919,    -1,    -1,   458,  1175,    -1,   458,   473,    -1,   458,
     325,    -1,   458,   105,    -1,  1279,   289,    -1,    -1,   152,
      -1,    -1,   199,   922,  1186,    -1,    -1,   206,   924,  1201,
      -1,    -1,   209,  1277,   926,  1176,   927,    -1,    -1,   122,
    1259,  1196,    -1,    -1,   210,   929,   930,    -1,    -1,  1266,
    1189,    -1,    -1,   217,   932,   933,    -1,    -1,  1151,  1275,
     934,   935,    -1,     1,    -1,   154,    -1,   793,   936,    -1,
     793,   139,   154,    -1,   793,   139,   793,   936,    -1,   139,
     793,   936,    -1,   139,   154,    -1,    -1,   154,    -1,    -1,
     224,   938,  1186,   939,   940,   941,   945,    -1,    -1,  1279,
     192,    -1,    -1,    10,  1277,   467,    -1,   944,  1277,   467,
      -1,    -1,   355,   942,    -1,   943,    -1,   942,   943,    -1,
     944,  1244,    55,  1189,    -1,    13,    -1,    16,    -1,   292,
      -1,    17,    -1,   293,    -1,   274,    -1,   275,    -1,    -1,
     118,    -1,    -1,   226,   947,   731,    -1,    -1,   229,   949,
     950,   951,    -1,  1201,    -1,  1209,    -1,  1212,    -1,   952,
      -1,   951,   952,    -1,   953,    -1,   957,    -1,   962,    -1,
      -1,   422,   954,   955,    -1,   956,    -1,   955,   956,    -1,
    1193,   196,    -1,    67,   963,    -1,    10,    -1,   243,    -1,
     439,    -1,  1193,   963,    -1,   355,   958,    -1,   959,    -1,
     958,   959,    -1,    67,    55,  1193,   963,    -1,   960,   961,
      -1,    -1,    10,    -1,   243,    -1,   194,    -1,   439,    -1,
    1193,    55,  1194,   963,    -1,   101,  1233,  1193,   432,  1194,
     963,    -1,    -1,   963,  1105,   964,  1189,    -1,    -1,   435,
      -1,    -1,   266,   966,  1040,    -1,    -1,   270,   968,   969,
      -1,  1189,   432,  1186,    -1,   103,  1189,   432,  1186,    -1,
      -1,   272,   971,   972,   973,    -1,  1189,    55,  1168,  1116,
      -1,  1189,    55,  1189,   207,  1168,  1116,    -1,    -1,   155,
      -1,    -1,   302,   975,   976,    -1,    -1,   976,   977,   978,
    1172,   980,   979,    -1,   227,    -1,   308,    -1,   235,    -1,
     186,    -1,    -1,   390,  1279,   609,    -1,    -1,  1279,   281,
     368,    -1,  1279,   260,    -1,    -1,   365,    -1,    -1,   316,
     982,   983,    -1,   986,   987,    -1,    -1,   987,   984,   793,
     985,    -1,   987,   985,    -1,    -1,   156,    -1,  1177,    -1,
    1177,   428,  1177,    -1,    -1,   198,    -1,  1190,   431,    -1,
     988,   455,   989,    -1,   988,   468,   990,    -1,    -1,  1279,
     425,  1105,    -1,  1151,    -1,   184,    -1,   991,    -1,   990,
       9,   991,    -1,  1201,   200,  1189,    55,  1189,   455,  1151,
      -1,    -1,   323,   993,   256,  1200,    -1,   992,    -1,   994,
     992,    -1,    -1,   994,    -1,    -1,   339,   997,  1173,  1223,
    1265,  1000,   998,   999,  1001,  1002,  1003,    -1,    -1,   230,
    1201,    -1,    -1,   219,   260,    -1,  1279,   260,    -1,  1279,
     281,   260,    -1,  1279,   218,   260,    -1,  1279,   469,    -1,
    1279,   238,   260,    -1,    -1,   219,   260,    -1,   472,   260,
      -1,   472,   281,   260,    -1,   472,   218,   260,    -1,   472,
     469,    -1,   472,   238,   260,    -1,    -1,   239,  1250,  1201,
      -1,    -1,  1126,    -1,  1137,    -1,    -1,   158,    -1,   340,
     440,    -1,   362,   440,    -1,    -1,   350,  1007,  1170,  1103,
      -1,    -1,   363,  1009,  1173,  1265,   998,  1126,  1010,    -1,
      -1,   159,    -1,    -1,   369,  1012,  1170,  1103,  1013,  1136,
    1014,    -1,    -1,  1279,   260,    -1,  1279,   281,   260,    -1,
      -1,   160,    -1,   371,    -1,    -1,   379,  1017,  1018,  1026,
      -1,  1171,  1020,  1021,  1023,    -1,    -1,    10,  1171,  1021,
     470,  1152,  1019,   796,    -1,    -1,   468,  1201,    -1,    -1,
      -1,  1237,   142,  1022,   793,    -1,  1024,    -1,  1024,  1023,
      -1,    -1,   470,  1151,  1025,   796,    -1,    -1,   161,    -1,
      -1,   389,  1028,  1029,    -1,  1030,    -1,  1031,    -1,  1032,
      -1,  1034,    -1,  1036,    -1,   169,  1193,   432,  1193,    -1,
    1186,   432,   168,  1192,    -1,  1186,   432,  1189,    -1,  1186,
    1033,    55,  1189,    -1,   456,    -1,   134,    -1,  1035,    -1,
    1034,  1035,    -1,  1174,   432,   552,    -1,  1037,    -1,  1036,
    1037,    -1,  1186,   432,   437,    -1,  1186,   432,   433,    -1,
      -1,   398,  1039,  1040,    -1,    -1,  1204,  1042,  1044,  1045,
    1041,  1046,  1047,    -1,    -1,  1042,  1259,   694,  1252,  1043,
      -1,    -1,  1043,  1204,    -1,    -1,  1230,  1261,    -1,    -1,
    1231,  1250,  1181,    -1,    -1,   466,  1172,    -1,   227,   327,
    1250,   986,    -1,    -1,   207,  1172,    -1,   308,   327,  1250,
     986,    -1,    -1,    -1,   407,  1049,  1173,   999,  1050,  1051,
    1136,  1053,    -1,    -1,   239,  1250,  1052,  1189,    -1,  1224,
    1157,    -1,  1224,  1158,    -1,  1224,  1159,    -1,  1224,  1160,
      -1,  1224,  1161,    -1,    -1,   162,    -1,    -1,   411,  1055,
    1056,    -1,   373,  1057,    -1,   256,    -1,     3,  1057,    -1,
      -1,  1189,    -1,   364,  1189,    -1,   207,  1189,    -1,    -1,
     412,  1059,  1060,   230,  1201,  1062,  1121,  1063,    -1,  1061,
      -1,  1060,  1061,    -1,  1189,    -1,   120,  1239,   396,    -1,
     120,  1239,  1189,    -1,    -1,  1279,   319,  1201,    -1,    -1,
     163,    -1,    -1,   415,  1065,  1066,  1067,    -1,  1188,   200,
    1168,  1116,    -1,  1188,   200,  1189,   207,  1168,  1116,    -1,
     103,  1201,   200,  1201,  1226,  1116,    -1,    -1,   164,    -1,
     418,  1069,    -1,    -1,   326,    -1,    -1,   424,  1071,   731,
      -1,    -1,   441,  1073,  1201,   200,  1193,   432,  1194,    -1,
      -1,   449,  1075,  1173,  1076,    -1,    -1,   341,    -1,   343,
      -1,    -1,   454,  1078,  1196,  1079,  1082,  1062,  1086,  1121,
    1087,    -1,    -1,   120,  1239,  1080,    -1,  1081,    -1,  1080,
    1260,  1081,    -1,  1220,  1193,    -1,   230,  1083,    -1,  1082,
    1083,    -1,  1201,  1084,  1085,    -1,    -1,   121,  1248,  1201,
      -1,    -1,   104,  1248,  1201,    -1,    -1,   422,  1248,  1201,
      -1,    -1,   165,    -1,  1089,    -1,  1098,    -1,  1100,    -1,
     465,  1090,  1092,  1093,  1094,  1097,  1259,  1091,    -1,    -1,
     208,    -1,  1172,    -1,   227,    -1,   308,    -1,   235,    -1,
     186,    -1,    -1,     9,    -1,    -1,   404,    -1,   182,    -1,
     178,    -1,   182,    -1,   309,    -1,   285,    -1,   287,    -1,
      -1,   327,    -1,   465,  1246,   115,  1259,  1099,    -1,  1177,
      -1,    10,   328,    -1,   465,  1090,    36,   357,  1201,    -1,
      -1,   476,  1102,  1170,  1103,  1013,  1104,  1106,  1107,    -1,
      -1,   200,  1190,    -1,    -1,  1105,  1232,  1198,  1254,    -1,
    1105,  1232,  1175,    -1,  1105,  1232,   313,    -1,    36,    -1,
       9,    -1,    -1,  1131,    -1,  1137,    -1,    -1,   166,    -1,
    1111,  1113,    -1,  1111,  1113,    -1,  1111,  1113,    -1,    -1,
      -1,   182,  1112,  1115,   793,    -1,    -1,    -1,   285,  1114,
     793,    -1,    -1,  1201,    -1,  1117,  1119,    -1,    -1,    -1,
     397,  1118,   793,    -1,    -1,    -1,   288,  1120,   793,    -1,
    1122,  1124,    -1,    -1,    -1,   309,  1123,   793,    -1,    -1,
      -1,   287,  1125,   793,    -1,  1127,    -1,  1129,    -1,  1127,
    1129,    -1,    -1,   142,  1128,   793,    -1,    -1,   283,  1130,
     793,    -1,  1132,    -1,  1134,    -1,  1132,  1134,    -1,    -1,
     173,  1133,   793,    -1,    -1,   284,  1135,   793,    -1,    -1,
    1137,    -1,  1138,    -1,  1140,    -1,  1138,  1140,    -1,    -1,
     233,  1139,   793,    -1,    -1,   286,  1141,   793,    -1,    -1,
     478,  1143,  1144,  1110,  1147,    -1,   315,  1196,  1279,  1149,
    1148,  1145,  1250,   986,    -1,   206,  1201,   200,  1196,  1146,
      -1,   335,    -1,   327,    -1,   335,   327,    -1,    -1,   104,
    1248,  1201,    -1,    -1,   167,    -1,    -1,   840,   274,    -1,
      -1,   141,  1196,    -1,    -1,  1239,  1198,  1254,    -1,  1152,
      -1,  1153,    -1,    -1,  1154,  1155,    -1,  1156,    -1,  1155,
    1156,    -1,  1189,    -1,   490,    -1,   491,    -1,   482,    -1,
     483,    -1,   484,    -1,   485,    -1,   487,    -1,    38,    -1,
      40,    -1,    41,    -1,    39,    -1,  1157,    -1,  1158,    -1,
    1159,    -1,   205,    -1,   242,    -1,   278,    -1,   282,  1157,
      -1,   282,  1158,    -1,   282,  1159,    -1,   282,   205,    -1,
     282,   242,    -1,   282,   278,    -1,   282,    -1,    21,    -1,
     304,    -1,   299,    -1,   292,    -1,    13,    -1,    14,    -1,
      15,    -1,   321,    -1,   279,    -1,   416,    -1,   188,    -1,
     234,    -1,   489,    -1,   175,  1277,    -1,   176,    -1,   492,
      -1,   211,  1274,    -1,   493,    -1,   247,  1274,    -1,   205,
      -1,   211,  1274,   304,   175,  1277,    -1,   242,    -1,   247,
    1274,   304,   175,  1277,    -1,  1163,    -1,  1162,  1165,  1163,
      -1,  1166,    -1,    10,    -1,  1166,    -1,  1164,  1165,  1166,
      -1,    -1,    80,    -1,   384,    -1,  1190,    -1,  1166,   482,
    1166,    -1,  1166,   483,  1166,    -1,  1166,   484,  1166,    -1,
    1166,   485,  1166,    -1,   482,  1166,    -1,   483,  1166,    -1,
    1166,   487,  1166,    -1,   490,  1166,   491,    -1,  1166,    38,
    1166,    -1,  1166,    40,  1166,    -1,  1166,    41,  1166,    -1,
      39,  1166,    -1,   252,    -1,   252,  1228,   473,    -1,  1169,
      -1,  1168,  1169,    -1,  1189,  1226,    -1,  1204,    -1,  1204,
      -1,  1173,    -1,  1172,  1173,    -1,   473,    -1,  1175,    -1,
    1174,  1175,    -1,   268,    -1,    -1,  1176,  1177,    -1,  1178,
      -1,  1203,    -1,  1179,    -1,  1179,  1228,  1179,    -1,   256,
      -1,  1181,    -1,  1180,  1181,    -1,  1204,    -1,    -1,  1181,
      -1,  1181,    -1,   256,    -1,   473,    -1,    -1,  1184,    -1,
    1187,    -1,  1186,  1187,    -1,  1186,  1277,  1187,    -1,  1201,
      -1,     7,  1258,  1202,    -1,  1189,    -1,  1188,  1189,    -1,
    1201,    -1,   245,  1258,  1202,    -1,   245,  1258,  1210,    -1,
     245,  1258,  1212,    -1,     7,  1258,  1191,  1192,    -1,     7,
    1258,  1202,    -1,  1209,    -1,  1212,    -1,  1167,    -1,  1201,
      -1,   245,  1258,  1202,    -1,   245,  1258,  1210,    -1,   245,
    1258,  1212,    -1,  1210,    -1,  1212,    -1,  1167,    -1,   330,
      -1,   168,    -1,  1202,    -1,   256,    -1,  1201,    -1,  1210,
      -1,  1212,    -1,  1201,    -1,  1209,    -1,  1212,    -1,    -1,
    1196,    -1,  1201,    -1,  1211,    -1,  1212,    -1,    -1,  1199,
      -1,  1201,    -1,  1207,    -1,   481,    -1,  1201,    -1,  1208,
      -1,   481,    -1,    -1,   256,    -1,  1202,    -1,  1204,    -1,
    1204,  1205,    -1,  1204,  1206,    -1,  1204,  1205,  1206,    -1,
     473,    -1,   473,  1228,  1203,    -1,   473,    -1,   473,  1228,
    1204,    -1,  1204,   133,    -1,   490,  1162,   491,    -1,   490,
    1166,   494,   491,    -1,   490,  1166,   494,  1166,   491,    -1,
     256,    -1,   256,    -1,  1210,    -1,    10,  1211,    -1,  1211,
      -1,  1210,   495,  1211,    -1,   256,    -1,   402,    -1,   481,
      -1,   336,    -1,   126,    -1,   215,    -1,   263,    -1,   436,
      -1,   107,  1213,    -1,   471,  1213,    -1,   463,   490,  1166,
     491,  1213,    -1,   261,   490,  1166,   491,  1213,    -1,   366,
     490,  1166,   491,  1213,    -1,    92,   490,  1164,   491,  1213,
      -1,   413,   490,  1164,   491,  1213,    -1,   414,   490,  1164,
     491,  1213,    -1,   442,   490,  1216,   491,  1213,    -1,   443,
     490,  1166,   491,  1213,    -1,   444,   490,  1166,   491,  1213,
      -1,   294,   490,  1217,   491,    -1,   258,   490,  1218,   491,
    1213,    -1,   276,   490,  1164,   491,  1213,    -1,   130,   490,
    1164,   491,  1213,    -1,   204,  1214,    -1,    -1,   490,  1166,
     494,   491,    -1,   490,  1166,   494,  1166,   491,    -1,    -1,
     490,  1215,   491,    -1,    -1,  1164,    -1,  1166,    -1,  1166,
    1165,   243,    -1,  1166,  1165,   439,    -1,  1166,    -1,  1166,
    1165,  1166,    -1,  1166,    -1,  1166,  1165,  1181,    -1,    -1,
      -1,    10,    -1,    -1,  1230,    -1,    -1,   225,    -1,    -1,
     280,    -1,   324,    -1,    -1,   282,    -1,    -1,   282,   303,
      -1,   303,    -1,    -1,   372,    -1,    -1,   386,  1240,    -1,
     220,    -1,   297,    -1,   341,  1250,    -1,   341,  1234,    -1,
     343,  1234,    -1,   343,  1250,    -1,   472,   135,    -1,   135,
      -1,    73,   387,    -1,   387,    -1,    -1,     8,    -1,    -1,
      10,    -1,    -1,    24,    -1,    -1,    25,    -1,    -1,    28,
      -1,    -1,    31,    -1,    -1,    42,    -1,    -1,    55,    -1,
      -1,    66,    -1,    -1,    67,    -1,    -1,    95,    -1,    -1,
      78,    -1,    -1,   110,    -1,    -1,   434,    -1,    -1,   196,
      -1,    -1,   200,    -1,    -1,   220,    -1,    -1,   231,    -1,
      -1,   234,    -1,    -1,   234,    -1,    24,    -1,    -1,   239,
      -1,    -1,   255,    -1,    -1,   253,    -1,   254,    -1,    -1,
     254,    -1,    -1,   269,    -1,    -1,   290,    -1,    -1,   297,
      -1,    -1,   300,    -1,    -1,   304,    -1,    -1,   305,    -1,
     220,   305,    -1,    -1,   307,    -1,    -1,   330,    -1,    -1,
     513,    -1,    -1,   341,    -1,    -1,   364,    -1,   207,    -1,
      -1,   370,    -1,    -1,   389,    -1,    -1,   391,    -1,    -1,
     391,    -1,   391,   234,    -1,    -1,   396,    -1,    -1,   409,
      -1,    -1,   423,    -1,    -1,   426,    -1,    -1,   427,    -1,
      -1,   431,    -1,    -1,   432,    -1,    -1,   470,    -1,    -1,
     472,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   900,   900,   900,   947,   948,   953,   954,   952,   967,
     968,   969,   972,   974,   973,   997,   996,  1028,  1032,  1027,
    1091,  1090,  1140,  1141,  1144,  1145,  1146,  1150,  1151,  1152,
    1160,  1161,  1164,  1165,  1169,  1176,  1183,  1187,  1191,  1195,
    1206,  1207,  1210,  1214,  1215,  1223,  1224,  1227,  1227,  1235,
    1245,  1246,  1250,  1251,  1252,  1253,  1262,  1265,  1266,  1267,
    1268,  1272,  1279,  1283,  1284,  1292,  1295,  1296,  1297,  1298,
    1302,  1303,  1307,  1308,  1309,  1313,  1320,  1321,  1325,  1332,
    1344,  1347,  1348,  1352,  1353,  1357,  1361,  1368,  1369,  1379,
    1382,  1383,  1387,  1388,  1392,  1393,  1394,  1395,  1396,  1397,
    1398,  1399,  1400,  1401,  1402,  1403,  1410,  1415,  1422,  1422,
    1422,  1425,  1436,  1435,  1462,  1474,  1475,  1484,  1485,  1493,
    1492,  1504,  1505,  1506,  1507,  1508,  1516,  1517,  1522,  1523,
    1525,  1524,  1536,  1537,  1541,  1542,  1543,  1544,  1545,  1546,
    1547,  1551,  1552,  1553,  1554,  1555,  1556,  1557,  1564,  1571,
    1578,  1588,  1600,  1601,  1605,  1606,  1613,  1622,  1623,  1627,
    1628,  1642,  1657,  1724,  1735,  1742,  1749,  1755,  1762,  1763,
    1766,  1770,  1771,  1780,  1781,  1784,  1788,  1794,  1793,  1824,
    1825,  1829,  1831,  1832,  1833,  1834,  1836,  1837,  1838,  1839,
    1840,  1841,  1842,  1843,  1844,  1851,  1855,  1859,  1868,  1869,
    1870,  1875,  1879,  1883,  1887,  1894,  1895,  1902,  1909,  1913,
    1914,  1915,  1922,  1948,  1958,  1967,  1969,  1970,  1976,  1980,
    1981,  1982,  1985,  1986,  1987,  1991,  2003,  2004,  2009,  2015,
    2016,  2020,  2030,  2040,  2052,  2064,  2079,  2089,  2096,  2105,
    2106,  2109,  2117,  2124,  2125,  2132,  2136,  2137,  2138,  2147,
    2148,  2152,  2156,  2160,  2164,  2165,  2169,  2170,  2171,  2177,
    2202,  2203,  2204,  2205,  2211,  2218,  2219,  2223,  2226,  2227,
    2233,  2240,  2241,  2242,  2243,  2250,  2253,  2252,  2265,  2268,
    2269,  2273,  2275,  2277,  2279,  2281,  2290,  2291,  2294,  2301,
    2300,  2336,  2350,  2351,  2361,  2360,  2379,  2380,  2384,  2391,
    2398,  2399,  2400,  2401,  2402,  2403,  2404,  2405,  2406,  2413,
    2417,  2417,  2417,  2452,  2469,  2497,  2529,  2530,  2537,  2538,
    2542,  2543,  2550,  2557,  2558,  2561,  2562,  2568,  2572,  2586,
    2587,  2591,  2592,  2599,  2598,  2611,  2629,  2630,  2634,  2635,
    2636,  2640,  2647,  2654,  2664,  2699,  2716,  2720,  2732,  2732,
    2746,  2746,  2769,  2770,  2771,  2777,  2788,  2776,  2820,  2824,
    2825,  2830,  2835,  2841,  2850,  2858,  2859,  2867,  2868,  2869,
    2873,  2893,  2897,  2906,  2907,  2908,  2909,  2910,  2911,  2912,
    2913,  2914,  2915,  2916,  2917,  2918,  2919,  2920,  2925,  2935,
    2957,  2981,  2982,  2994,  3019,  3036,  3037,  3038,  3048,  3068,
    3069,  3070,  3071,  3072,  3073,  3074,  3076,  3077,  3078,  3079,
    3080,  3081,  3082,  3088,  3094,  3095,  3096,  3097,  3098,  3099,
    3100,  3101,  3102,  3103,  3104,  3105,  3106,  3107,  3108,  3109,
    3110,  3111,  3112,  3121,  3130,  3139,  3145,  3156,  3161,  3172,
    3191,  3192,  3196,  3197,  3201,  3202,  3203,  3208,  3209,  3213,
    3220,  3244,  3259,  3261,  3266,  3267,  3271,  3278,  3279,  3284,
    3294,  3301,  3304,  3305,  3306,  3313,  3320,  3345,  3347,  3347,
    3352,  3352,  3357,  3358,  3362,  3363,  3364,  3371,  3372,  3385,
    3397,  3417,  3433,  3432,  3454,  3454,  3468,  3470,  3469,  3481,
    3482,  3486,  3487,  3496,  3503,  3506,  3510,  3514,  3515,  3516,
    3523,  3524,  3528,  3531,  3531,  3532,  3532,  3532,  3533,  3533,
    3534,  3534,  3535,  3535,  3536,  3536,  3538,  3539,  3545,  3549,
    3550,  3554,  3555,  3556,  3557,  3558,  3562,  3563,  3564,  3565,
    3568,  3569,  3573,  3577,  3578,  3582,  3583,  3584,  3585,  3586,
    3587,  3588,  3589,  3590,  3591,  3592,  3593,  3594,  3595,  3596,
    3597,  3601,  3605,  3606,  3607,  3608,  3609,  3610,  3611,  3615,
    3619,  3620,  3621,  3622,  3623,  3624,  3628,  3629,  3633,  3637,
    3642,  3646,  3650,  3654,  3655,  3659,  3660,  3664,  3665,  3666,
    3669,  3669,  3669,  3672,  3676,  3679,  3679,  3682,  3689,  3690,
    3691,  3690,  3709,  3710,  3714,  3715,  3720,  3722,  3721,  3764,
    3765,  3769,  3770,  3771,  3772,  3773,  3774,  3775,  3776,  3777,
    3778,  3779,  3780,  3781,  3782,  3783,  3784,  3785,  3787,  3788,
    3789,  3813,  3832,  3851,  3855,  3859,  3860,  3861,  3862,  3863,
    3864,  3865,  3866,  3873,  3877,  3883,  3888,  3897,  3901,  3905,
    3909,  3913,  3921,  3924,  3928,  3932,  3936,  3944,  3957,  3959,
    3969,  3958,  4012,  4014,  4013,  4023,  4022,  4035,  4036,  4041,
    4048,  4050,  4054,  4062,  4070,  4073,  4083,  4085,  4093,  4101,
    4130,  4161,  4163,  4173,  4178,  4189,  4190,  4190,  4217,  4218,
    4223,  4224,  4225,  4226,  4227,  4246,  4250,  4262,  4295,  4334,
    4338,  4351,  4352,  4355,  4356,  4365,  4369,  4365,  4382,  4383,
    4387,  4387,  4405,  4409,  4410,  4411,  4412,  4413,  4414,  4415,
    4416,  4417,  4418,  4419,  4420,  4421,  4422,  4423,  4424,  4425,
    4426,  4427,  4428,  4429,  4430,  4431,  4432,  4433,  4434,  4435,
    4436,  4437,  4438,  4439,  4440,  4441,  4442,  4443,  4444,  4445,
    4446,  4447,  4448,  4449,  4450,  4451,  4452,  4453,  4454,  4455,
    4456,  4457,  4458,  4459,  4460,  4461,  4462,  4463,  4464,  4487,
    4486,  4508,  4512,  4516,  4520,  4524,  4528,  4532,  4536,  4540,
    4544,  4548,  4552,  4556,  4560,  4564,  4568,  4572,  4576,  4580,
    4588,  4589,  4590,  4595,  4600,  4601,  4605,  4606,  4610,  4611,
    4615,  4616,  4617,  4618,  4619,  4620,  4621,  4622,  4623,  4624,
    4625,  4626,  4627,  4628,  4629,  4630,  4634,  4638,  4642,  4647,
    4648,  4649,  4650,  4651,  4652,  4653,  4654,  4664,  4668,  4669,
    4678,  4678,  4684,  4699,  4713,  4729,  4730,  4734,  4735,  4744,
    4744,  4749,  4753,  4760,  4761,  4771,  4770,  4778,  4779,  4786,
    4786,  4794,  4794,  4811,  4811,  4826,  4827,  4830,  4831,  4837,
    4843,  4848,  4851,  4855,  4859,  4866,  4867,  4868,  4873,  4874,
    4878,  4879,  4884,  4883,  4896,  4897,  4902,  4909,  4916,  4918,
    4922,  4930,  4938,  4956,  4957,  4958,  4962,  4963,  4968,  4972,
    4971,  4983,  4987,  4986,  4997,  4998,  5002,  5003,  5012,  5012,
    5016,  5017,  5029,  5029,  5035,  5043,  5034,  5056,  5057,  5061,
    5069,  5070,  5071,  5072,  5076,  5077,  5085,  5085,  5089,  5090,
    5101,  5102,  5103,  5104,  5105,  5108,  5108,  5116,  5116,  5122,
    5146,  5147,  5150,  5150,  5157,  5170,  5183,  5183,  5191,  5191,
    5202,  5203,  5213,  5212,  5232,  5236,  5240,  5244,  5248,  5268,
    5269,  5274,  5275,  5276,  5277,  5278,  5279,  5280,  5281,  5282,
    5283,  5284,  5285,  5286,  5287,  5288,  5289,  5290,  5291,  5292,
    5296,  5300,  5304,  5309,  5310,  5314,  5315,  5324,  5324,  5332,
    5348,  5365,  5382,  5395,  5411,  5412,  5421,  5421,  5437,  5438,
    5443,  5442,  5452,  5453,  5458,  5465,  5481,  5492,  5493,  5518,
    5517,  5536,  5537,  5542,  5552,  5558,  5576,  5583,  5584,  5590,
    5589,  5610,  5613,  5618,  5619,  5625,  5626,  5630,  5631,  5636,
    5729,  5730,  5731,  5735,  5736,  5740,  5741,  5750,  5750,  5755,
    5757,  5756,  5764,  5781,  5798,  5816,  5837,  5838,  5850,  5857,
    5849,  5864,  5865,  5869,  5870,  5873,  5874,  5875,  5879,  5880,
    5883,  5884,  5885,  5886,  5887,  5888,  5892,  5893,  5901,  5901,
    5914,  5914,  5927,  5927,  5936,  5940,  5953,  5953,  5958,  5962,
    5974,  5974,  5980,  5979,  5987,  5991,  5996,  6001,  6008,  6015,
    6022,  6031,  6032,  6040,  6040,  6048,  6049,  6053,  6054,  6055,
    6059,  6060,  6065,  6066,  6071,  6075,  6076,  6077,  6078,  6079,
    6080,  6081,  6085,  6086,  6095,  6095,  6108,  6107,  6117,  6118,
    6119,  6123,  6124,  6128,  6129,  6130,  6136,  6136,  6141,  6142,
    6146,  6147,  6148,  6149,  6150,  6151,  6157,  6161,  6162,  6166,
    6171,  6175,  6176,  6177,  6178,  6179,  6183,  6209,  6218,  6219,
    6223,  6223,  6231,  6231,  6241,  6241,  6246,  6250,  6262,  6262,
    6268,  6283,  6303,  6304,  6313,  6313,  6318,  6319,  6337,  6338,
    6339,  6340,  6344,  6345,  6350,  6351,  6352,  6356,  6357,  6365,
    6365,  6370,  6375,  6374,  6384,  6392,  6393,  6397,  6402,  6411,
    6414,  6418,  6422,  6433,  6440,  6441,  6445,  6446,  6450,  6451,
    6456,  6470,  6469,  6489,  6490,  6492,  6492,  6499,  6499,  6524,
    6525,  6529,  6530,  6534,  6538,  6542,  6546,  6550,  6557,  6558,
    6562,  6566,  6570,  6574,  6578,  6585,  6586,  6589,  6590,  6591,
    6595,  6596,  6605,  6617,  6630,  6630,  6645,  6645,  6656,  6657,
    6666,  6666,  6682,  6683,  6687,  6694,  6695,  6704,  6717,  6717,
    6723,  6728,  6727,  6738,  6739,  6743,  6745,  6744,  6755,  6756,
    6761,  6760,  6772,  6773,  6782,  6782,  6787,  6788,  6789,  6790,
    6791,  6797,  6806,  6810,  6819,  6826,  6827,  6833,  6834,  6838,
    6847,  6848,  6852,  6856,  6868,  6868,  6874,  6873,  6896,  6899,
    6915,  6916,  6919,  6920,  6924,  6925,  6930,  6935,  6943,  6956,
    6961,  6969,  6985,  6986,  6985,  7009,  7010,  7014,  7015,  7016,
    7017,  7018,  7022,  7023,  7032,  7032,  7037,  7042,  7047,  7056,
    7057,  7058,  7059,  7068,  7068,  7077,  7078,  7082,  7083,  7084,
    7088,  7089,  7093,  7094,  7103,  7103,  7109,  7123,  7139,  7155,
    7156,  7165,  7172,  7173,  7181,  7181,  7194,  7194,  7210,  7210,
    7219,  7221,  7222,  7231,  7231,  7241,  7242,  7247,  7248,  7253,
    7260,  7261,  7266,  7273,  7274,  7278,  7279,  7283,  7284,  7288,
    7289,  7298,  7299,  7300,  7304,  7329,  7332,  7340,  7350,  7355,
    7360,  7365,  7372,  7373,  7376,  7377,  7381,  7381,  7385,  7385,
    7389,  7389,  7392,  7393,  7397,  7404,  7405,  7409,  7421,  7421,
    7436,  7437,  7442,  7445,  7449,  7453,  7460,  7461,  7464,  7465,
    7466,  7470,  7471,  7484,  7492,  7500,  7507,  7509,  7508,  7533,
    7535,  7534,  7558,  7559,  7569,  7576,  7578,  7577,  7587,  7589,
    7588,  7604,  7610,  7612,  7611,  7621,  7623,  7622,  7638,  7643,
    7648,  7658,  7657,  7669,  7668,  7684,  7689,  7694,  7704,  7703,
    7715,  7714,  7729,  7730,  7734,  7739,  7744,  7754,  7753,  7765,
    7764,  7779,  7779,  7787,  7791,  7799,  7800,  7801,  7805,  7806,
    7810,  7811,  7815,  7816,  7820,  7821,  7830,  7833,  7845,  7852,
    7859,  7859,  7869,  7871,  7875,  7877,  7878,  7880,  7881,  7882,
    7883,  7884,  7885,  7886,  7887,  7888,  7890,  7891,  7892,  7893,
    7894,  7895,  7896,  7897,  7898,  7899,  7900,  7901,  7903,  7904,
    7905,  7907,  7908,  7909,  7910,  7911,  7914,  7915,  7916,  7917,
    7918,  7921,  7921,  7921,  7922,  7922,  7923,  7923,  7924,  7924,
    7925,  7925,  7930,  7931,  7935,  7936,  7940,  7941,  7944,  7945,
    7946,  7950,  7951,  7952,  7953,  7954,  7955,  7956,  7957,  7958,
    7959,  7960,  7961,  7962,  7973,  7985,  8000,  8001,  8006,  8012,
    8018,  8038,  8042,  8058,  8080,  8081,  8086,  8092,  8093,  8098,
    8107,  8108,  8109,  8113,  8128,  8129,  8133,  8143,  8144,  8148,
    8149,  8155,  8166,  8167,  8179,  8180,  8181,  8185,  8186,  8190,
    8191,  8195,  8196,  8197,  8198,  8199,  8200,  8201,  8202,  8203,
    8207,  8208,  8209,  8210,  8211,  8212,  8213,  8217,  8218,  8222,
    8223,  8227,  8228,  8229,  8233,  8234,  8235,  8246,  8247,  8251,
    8253,  8254,  8258,  8259,  8263,  8264,  8265,  8269,  8270,  8271,
    8275,  8276,  8290,  8294,  8295,  8296,  8297,  8301,  8302,  8306,
    8307,  8308,  8312,  8321,  8325,  8337,  8349,  8360,  8361,  8371,
    8372,  8377,  8378,  8379,  8380,  8381,  8382,  8383,  8384,  8392,
    8396,  8400,  8404,  8408,  8412,  8416,  8420,  8424,  8428,  8432,
    8436,  8440,  8444,  8448,  8452,  8459,  8460,  8461,  8465,  8466,
    8470,  8471,  8476,  8483,  8490,  8500,  8507,  8517,  8524,  8538,
    8548,  8549,  8553,  8554,  8558,  8559,  8563,  8564,  8565,  8569,
    8570,  8574,  8575,  8576,  8580,  8581,  8585,  8586,  8593,  8593,
    8594,  8594,  8594,  8594,  8595,  8595,  8596,  8596,  8598,  8598,
    8599,  8599,  8600,  8600,  8601,  8601,  8602,  8602,  8603,  8603,
    8604,  8604,  8605,  8605,  8606,  8606,  8607,  8607,  8608,  8608,
    8609,  8609,  8610,  8610,  8611,  8611,  8612,  8612,  8613,  8613,
    8614,  8614,  8615,  8615,  8616,  8616,  8617,  8617,  8617,  8618,
    8618,  8619,  8619,  8620,  8620,  8620,  8621,  8621,  8622,  8622,
    8623,  8623,  8624,  8624,  8625,  8625,  8626,  8626,  8627,  8627,
    8627,  8628,  8628,  8629,  8629,  8630,  8630,  8631,  8631,  8632,
    8632,  8632,  8633,  8633,  8634,  8634,  8635,  8635,  8636,  8636,
    8636,  8637,  8637,  8638,  8638,  8639,  8639,  8640,  8640,  8641,
    8641,  8642,  8642,  8643,  8643,  8645,  8645,  8646,  8646
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "ABEND", "ACCEPT", "ACCESS",
  "ADD", "ADDRESS", "ADVANCING", "AFTER", "ALL", "ALLOCATE", "ALPHABET",
  "ALPHABETIC", "\"ALPHABETIC-LOWER\"", "\"ALPHABETIC-UPPER\"",
  "ALPHANUMERIC", "\"ALPHANUMERIC-EDITED\"", "ALSO", "ALTER", "ALTERNATE",
  "AND", "ANY", "APPLY", "ARE", "AREA", "\"ARGUMENT-NUMBER\"",
  "\"ARGUMENT-VALUE\"", "AS", "ASCENDING", "ASSIGN", "AT", "AUTO",
  "AUTOMATIC", "\"BACKGROUND-COLOR\"", "BASED", "BEFORE", "BELL",
  "\"B-AND\"", "\"B-NOT\"", "\"B-OR\"", "\"B-XOR\"", "BINARY",
  "\"BINARY-C-LONG\"", "\"BINARY-CHAR\"", "\"BINARY-DOUBLE\"",
  "\"BINARY-LONG\"", "\"BINARY-SHORT\"", "BIT", "BLANK", "\"BLANK-LINE\"",
  "\"BLANK-SCREEN\"", "BLINK", "BLOCK", "BOTTOM", "BY", "\"BYTE-LENGTH\"",
  "CALL", "CANCEL", "CDECL", "\"CENTURY-DATE\"", "\"CENTURY-DAY\"", "CH",
  "CHAIN", "CHAINING", "CHANGED", "CHARACTER", "CHARACTERS", "CHECKPOINT",
  "CLASS", "CLOSE", "CODE", "\"CODE-SET\"", "COLLATING", "COL", "COLS",
  "COLUMN", "COLUMNS", "COMMA", "\"COMMAND-LINE\"", "\"comma delimiter\"",
  "COMMIT", "COMMON", "COMP", "COMPUTE", "\"COMP-1\"", "\"COMP-2\"",
  "\"COMP-3\"", "\"COMP-4\"", "\"COMP-5\"", "\"COMP-6\"", "\"COMP-X\"",
  "\"FUNCTION CONCATENATE\"", "CONFIGURATION", "CONSTANT", "CONTAINS",
  "CONTENT", "CONTINUE", "CONTROL", "CONTROLS", "CONVERT", "CONVERTING",
  "\"CORE-INDEX\"", "CORRESPONDING", "COUNT", "CRT", "CURRENCY",
  "\"FUNCTION CURRENT-DATE\"", "CURSOR", "CYCLE", "DATA", "DATE", "DAY",
  "\"DAY-OF-WEEK\"", "DE", "DEBUGGING", "\"DECIMAL-POINT\"",
  "DECLARATIVES", "DEFAULT", "DELETE", "DELIMITED", "DELIMITER",
  "DEPENDING", "DESCENDING", "DESCRIPTOR", "DETAIL", "\"DIR-SEPARATOR\"",
  "DISK", "DISPLAY", "\"DISPLAY-1\"", "\"FUNCTION DISPLAY-OF\"", "DIVIDE",
  "DIVISION", "\".Identifier\"", "DOWN", "DUPLICATES", "DYNAMIC", "ECHO",
  "EBCDIC", "ELSE", "\"EMPTY-CHECK\"", "ENCODING", "END", "\"END-ACCEPT\"",
  "\"END-ADD\"", "\"END-CALL\"", "\"END-COMPUTE\"", "\"END-CHAIN\"",
  "\"END-DELETE\"", "\"END-DISPLAY\"", "\"END-DIVIDE\"",
  "\"END-EVALUATE\"", "\"END-EXHIBIT\"", "\"END FUNCTION\"", "\"END-IF\"",
  "\"END-MULTIPLY\"", "\"END-PERFORM\"", "\"END PROGRAM\"", "\"END-READ\"",
  "\"END-RETURN\"", "\"END-REWRITE\"", "\"END-SEARCH\"", "\"END-START\"",
  "\"END-STRING\"", "\"END-SUBTRACT\"", "\"END-UNSTRING\"",
  "\"END-WRITE\"", "\"END-XML\"", "ENTRY", "ENVIRONMENT",
  "\"ENVIRONMENT-NAME\"", "\"ENVIRONMENT-VALUE\"", "EOL", "EOP", "EOS",
  "EQUAL", "EQUALS", "ERASE", "ERROR", "ESCAPE", "EVALUATE",
  "\"EVENT-STATUS\"", "EXCEPTION", "EXCLUSIVE", "EXIT", "EXHIBIT",
  "EXTEND", "EXTERNAL", "FAILURE", "FD", "\"FILE-CONTROL\"", "\"FILE-ID\"",
  "FILLER", "FINAL", "FIRST", "FOOTING", "FOR", "\"FOREGROUND-COLOR\"",
  "FOREVER", "FREE", "FROM", "FULL", "FUNCTION", "\"FUNCTION-ID\"",
  "\"FUNCTION\"", "GE", "GENERATE", "GIVING", "GLOBAL", "GO", "GOBACK",
  "GREATER", "GROUP", "HEADING", "HIGHLIGHT", "\"HIGH-VALUE\"",
  "IDENTIFICATION", "IF", "IGNORE", "IGNORING", "IN", "INDEX", "INDEXED",
  "INDICATE", "INITIALIZE", "INITIALIZED", "INITIATE", "INPUT",
  "\"INPUT-OUTPUT\"", "INSPECT", "INTO", "INTRINSIC", "INVALID",
  "\"INVALID KEY\"", "IS", "\"I-O\"", "\"I-O-CONTROL\"", "JUSTIFIED",
  "KEPT", "KEY", "LABEL", "LAST", "LE", "LEADING", "LEFT", "LENGTH",
  "\"LENGTH-CHECK\"", "LESS", "LIKE", "LIMIT", "LIMITS", "LINAGE",
  "\"LINAGE-COUNTER\"", "LINE", "LINES", "LINKAGE", "\"Literal\"",
  "LOCALE", "\"FUNCTION LOCALE\"", "\"LOCAL-STORAGE\"", "LOCK",
  "\"FUNCTION LOWER-CASE\"", "LOWLIGHT", "\"LOW-VALUE\"", "MANUAL",
  "MEMORY", "MERGE", "MINUS", "\"MNEMONIC NAME\"", "MODE", "MOVE",
  "MULTIPLE", "MULTIPLY", "NAMED", "NATIONAL", "\"NATIONAL-EDITED\"",
  "\"FUNCTION NATIONAL-OF\"", "NATIVE", "NE", "NEGATIVE", "NEXT", "NO",
  "NOT", "\"NOT END\"", "\"NOT EOP\"", "\"NOT EXCEPTION\"",
  "\"NOT INVALID KEY\"", "\"NOT OVERFLOW\"", "\"NOT SIZE ERROR\"",
  "\"NO ADVANCING\"", "NUMBER", "NUMBERS", "NUMERIC", "\"NUMERIC-EDITED\"",
  "\"FUNCTION NUMVALC\"", "\"OBJECT-COMPUTER\"", "OCCURS", "OF", "OFF",
  "OMITTED", "ON", "ONLY", "OPEN", "OPTIONAL", "OR", "ORDER",
  "ORGANIZATION", "OTHER", "OUTPUT", "COB_OVERFLOW", "OVERLINE",
  "\"PACKED-DECIMAL\"", "PADDING", "PAGE", "PARAGRAPH", "PARSE", "PERFORM",
  "PICTURE", "PLUS", "POINTER", "POSITION", "POSITIVE", "PRESENT",
  "PRAGMA", "PREVIOUS", "PRINTER", "PRINTING", "PROCEDURE", "PROCEDURES",
  "PROCEED", "PROGRAM", "\"PROGRAM-ID\"", "\"Program name\"",
  "\"PROGRAM-POINTER\"", "PROMPT", "PROCESSING", "QUOTE", "RANDOM", "RD",
  "READ", "READY", "RECORD", "RECORDING", "RECORDS", "\"RECORD-OVERFLOW\"",
  "RECURSIVE", "REDEFINES", "REEL", "REFERENCE", "RELATIVE", "RELEASE",
  "REMAINDER", "REMOVAL", "RENAMES", "\"REORG-CRITERIA\"", "REPLACING",
  "REPORT", "REPORTING", "REPORTS", "REPOSITORY", "REQUIRED", "RESERVE",
  "RESET", "RETURN", "RETURNING", "REVERSED", "\"FUNCTION REVERSE\"",
  "\"REVERSE-VIDEO\"", "REWIND", "REWRITE", "RIGHT", "ROLLBACK", "ROUNDED",
  "RUN", "SAME", "SCREEN", "\"SCREEN-CONTROL\"", "SCROLL", "SD", "SEARCH",
  "SECTION", "SECURE", "\"SEGMENT-LIMIT\"", "SELECT", "\"semi-colon\"",
  "SENTENCE", "SEPARATE", "SEQUENCE", "SEQUENTIAL", "SET", "SHARING",
  "SIGN", "SIGNED", "\"SIGNED-INT\"", "\"SIGNED-LONG\"",
  "\"SIGNED-SHORT\"", "SIZE", "\"SIZE ERROR\"", "SORT", "\"SORT-MERGE\"",
  "SOURCE", "\"SOURCE-COMPUTER\"", "SPACE", "\"SPECIAL-NAMES\"",
  "STANDARD", "\"STANDARD-1\"", "\"STANDARD-2\"", "START", "STATIC",
  "STATUS", "STDCALL", "STOP", "STRING", "\"FUNCTION SUBSTITUTE\"",
  "\"FUNCTION SUBSTITUTE-CASE\"", "SUBTRACT", "SUCCESS", "SUM", "SUPPRESS",
  "SYMBOLIC", "SYNCHRONIZED", "TAB", "TALLYING", "TAPE", "TERMINATE",
  "TEST", "THAN", "THEN", "THRU", "TIME", "TIMEOUT", "TIMES", "TO",
  "\"FALSE\"", "\"FILE\"", "\"INITIAL\"", "\"NULL\"", "\"TRUE\"", "TOP",
  "TRAILING", "TRACE", "TRANSFORM", "\"FUNCTION TRIM\"",
  "\"FUNCTION TRIML\"", "\"FUNCTION TRIMR\"", "TYPE", "TYPEDEF",
  "UNDERLINE", "UNIT", "UNLOCK", "UNSIGNED", "\"UNSIGNED-INT\"",
  "\"UNSIGNED-LONG\"", "\"UNSIGNED-SHORT\"", "UNSTRING", "UNTIL", "UP",
  "UPDATE", "UPON", "\"UPON ARGUMENT-NUMBER\"", "\"UPON COMMAND-LINE\"",
  "\"UPON ENVIRONMENT-NAME\"", "\"UPON ENVIRONMENT-VALUE\"",
  "\"FUNCTION UPPER-CASE\"", "USAGE", "USE", "USING", "VALUE", "VARYING",
  "WAIT", "WHEN", "\"FUNCTION WHEN-COMPILED\"", "WITH", "\"Identifier\"",
  "WORDS", "\"WORKING-STORAGE\"", "WRITE", "\"WRITE-ONLY\"", "XML",
  "YYYYDDD", "YYYYMMDD", "ZERO", "'+'", "'-'", "'*'", "'/'", "UNARY_SIGN",
  "'^'", "'.'", "'='", "'('", "')'", "'>'", "'<'", "':'", "'&'", "$accept",
  "start", "$@1", "nested_list", "program_definition", "$@2", "$@3",
  "nested_prog", "end_program", "$@4", "$@5", "identification_division",
  "$@6", "$@7", "$@8", "_dot", "_dot_list", "program_name", "as_literal",
  "program_type", "program_type_clause", "_init_or_recurs",
  "environment_division", "environment_division_item",
  "configuration_section", "configuration_section_item", "$@9",
  "configuration_list", "configuration_paragraph",
  "source_computer_paragraph", "source_computer_entry",
  "with_debugging_mode", "computer_name", "word_list",
  "object_computer_paragraph", "object_computer_entry",
  "object_clauses_list", "object_clauses", "object_computer_memory",
  "object_char_or_word", "object_computer_sequence",
  "object_computer_segment", "repository_paragraph", "opt_repository",
  "repository_list", "repository_name", "repository_literal_list",
  "special_names_paragraph", "opt_special_names", "special_name_list",
  "special_name", "numemric_sign_clause", "numeric_sign_is",
  "mnemonic_name_clause", "$@10", "special_name_mnemonic_on_off",
  "on_or_off", "alphabet_name_clause", "$@11", "alphabet_definition",
  "alphabet_literal_list", "alphabet_literal", "@12",
  "alphabet_also_sequence", "alphabet_lits", "alphabet_also_literal",
  "symbolic_characters_clause", "symbolic_characters_list_list",
  "symbolic_characters_list", "char_list", "integer_list",
  "class_name_clause", "class_item_list", "class_item", "locale_clause",
  "currency_sign_clause", "decimal_point_clause", "cursor_clause",
  "crt_status_clause", "screen_control", "event_status",
  "input_output_section", "input_output_section_item",
  "file_control_paragraph", "file_control_paragraph_item",
  "file_control_entry", "$@13", "select_clause_sequence", "select_clause",
  "assign_clause", "_device", "_ext_clause", "assignment_name",
  "access_mode_clause", "access_mode", "alternative_record_key_clause",
  "collating_sequence_clause", "file_status_clause", "file_or_sort",
  "lock_mode_clause", "lock_mode", "lock_with", "lock_records",
  "line_advancing_clause", "organization_clause", "organization",
  "padding_character_clause", "record_delimiter_clause",
  "record_key_clause", "opt_splitk", "relative_key_clause",
  "reserve_clause", "sharing_clause", "sharing_option",
  "i_o_control_paragraph", "i_o_control_paragraph_item", "opt_i_o_control",
  "i_o_control_list", "i_o_control_clause", "same_clause", "same_option",
  "multiple_file_tape_clause", "multiple_file_list", "multiple_file",
  "multiple_file_position", "apply_clause", "apply_clause_option",
  "data_division", "$@14", "data_division_entry", "data_division_list",
  "data_division_list_item", "file_section", "file_section_item", "$@15",
  "file_description_sequence_without_type", "file_type",
  "file_description_entry", "@16", "file_description_clause_sequence",
  "file_description_clause", "block_contains_clause",
  "_records_or_characters", "record_clause", "record_depending",
  "opt_from_integer", "opt_to_integer", "label_records_clause",
  "label_option", "label_records", "value_of_clause", "file_id",
  "valueof_name", "data_records_clause", "$@17", "linage_clause",
  "linage_sequence", "linage_lines", "linage_footing", "linage_top",
  "linage_bottom", "recording_mode_clause", "code_set_clause",
  "report_clause", "working_storage_section", "$@18",
  "record_description_list", "$@19", "record_description_list_2",
  "data_description", "$@20", "$@21", "level_number", "entry_name",
  "const_name", "const_global", "lit_or_length", "constant_entry",
  "data_description_clause_sequence", "data_description_clause",
  "typdef_clause", "redefines_clause", "external_clause", "as_extname",
  "global_clause", "picture_clause", "usage_clause", "usage",
  "sign_clause", "occurs_clause", "occurs_option_list_1",
  "occurs_option_list", "occurs_option", "occurs_to_integer",
  "occurs_depending", "occurs_keys", "occurs_key_list_item",
  "occurs_key_list", "ascending_or_descending", "occurs_indexed",
  "occurs_index_list", "occurs_index", "justified_clause",
  "synchronized_clause", "left_or_right", "blank_clause", "based_clause",
  "screen_value_clause", "$@22", "value_clause", "$@23", "value_item_list",
  "value_item", "false_is", "renames_clause", "any_length_clause",
  "local_storage_section", "$@24", "linkage_section", "$@25",
  "report_section", "$@26", "opt_report_description_list",
  "report_description_list", "report_description_entry",
  "report_description_options", "report_description_option",
  "control_clause", "control_field_list", "_final", "_limit", "_line",
  "_column", "_detail", "_control_heading", "identifier_list",
  "page_limit_clause", "page_limit_options", "page_limit_option",
  "page_line_column", "report_group_description_list",
  "report_group_description_entry", "report_group_options",
  "report_group_option", "type_clause", "type_option", "next_group_clause",
  "column_clause", "sum_clause_list", "sum_clause", "ref_id_exp",
  "present_when_condition", "varying_clause", "line_clause",
  "line_keyword_clause", "report_line_integer_list", "line_or_plus",
  "_numbers", "source_clause", "group_indicate_clause", "_indicate",
  "report_name", "screen_section", "$@27", "$@28",
  "opt_screen_description_list", "screen_description_list",
  "screen_description", "$@29", "screen_options", "screen_option",
  "screen_line_plus_minus", "screen_col_plus_minus",
  "screen_occurs_clause", "procedure_division", "$@30", "$@31",
  "procedure_using_chaining", "$@32", "$@33", "procedure_param_list",
  "procedure_param", "procedure_type", "delimited_optional",
  "size_optional", "procedure_optional", "procedure_returning",
  "procedure_declaratives", "$@34", "procedure_list", "procedure",
  "section_header", "paragraph_name", "paragraph_header",
  "invalid_statement", "section_name", "opt_segment", "statement_list",
  "@35", "@36", "_statement_list", "statements", "$@37", "statement",
  "accept_statement", "$@38", "accept_body", "opt_at_line_column",
  "line_number", "column_number", "opt_accp_attr", "accp_attrs",
  "accp_attr", "end_accept", "add_statement", "$@39", "add_body", "add_to",
  "end_add", "allocate_statement", "$@40", "allocate_body",
  "allocate_returning", "alter_statement", "$@41", "alter_options",
  "_proceed_to", "call_statement", "$@42", "chain_statement", "$@43",
  "call_using_returning_list", "call_using_returning", "call_mfconv",
  "call_with", "call_conv", "call_static", "call_using", "call_using_core",
  "$@44", "call_param_list", "call_param", "call_type",
  "call_returning_core", "RETURNING_GIVING", "call_on_exception", "$@45",
  "call_not_on_exception", "$@46", "end_call", "end_chain",
  "cancel_statement", "$@47", "cancel_list", "checkpoint_statement",
  "$@48", "checkpoint_body", "$@49", "$@50", "checkpoint_options",
  "checkpoint_returning", "checkpoint_giving", "close_statement", "$@51",
  "close_list", "close_option", "reel_or_unit", "compute_statement",
  "$@52", "compute_body", "end_compute", "comp_equal", "commit_statement",
  "continue_statement", "delete_statement", "$@53", "@54", "end_delete",
  "display_statement", "$@55", "display_body", "disp_attrs", "disp_attr",
  "end_display", "divide_statement", "$@56", "divide_body", "end_divide",
  "entry_statement", "$@57", "entry_using", "entry_using_core", "$@58",
  "entry_param_list", "entry_param", "entry_returning",
  "evaluate_statement", "$@59", "evaluate_subject_list",
  "evaluate_subject", "evaluate_condition_list", "evaluate_case_list",
  "evaluate_case", "$@60", "evaluate_statement_list", "evaluate_when_list",
  "evaluate_object_list_or_other", "evaluate_object_list",
  "evaluate_object", "opt_evaluate_thru_expr", "end_evaluate",
  "exit_statement", "$@61", "exit_body", "$@62", "exit_program_returning",
  "exhibit_statement", "$@63", "$@64", "exhibit_x_list", "exhibit_options",
  "exhibit_opt", "exhibit_upon_adv", "exhibit_upon", "end_exhibit",
  "free_statement", "$@65", "generate_statement", "$@66", "goto_statement",
  "$@67", "goto_depending", "goback_statement", "$@68", "goback_option",
  "if_statement", "$@69", "if_core", "$@70", "if_else_sentence", "end_if",
  "initialize_statement", "$@71", "initialize_filler", "initialize_value",
  "initialize_replacing", "initialize_replacing_list",
  "initialize_replacing_item", "initialize_category", "initialize_default",
  "initiate_statement", "$@72", "inspect_statement", "$@73",
  "send_identifier", "inspect_list", "inspect_item", "inspect_tallying",
  "$@74", "tallying_list", "tallying_item", "inspect_replacing",
  "replacing_list", "replacing_item", "rep_keyword", "replacing_region",
  "inspect_converting", "inspect_region", "_initial", "merge_statement",
  "$@75", "move_statement", "$@76", "move_body", "multiply_statement",
  "$@77", "multiply_body", "end_multiply", "open_statement", "$@78",
  "open_list", "open_mode", "open_sharing", "open_option", "open_reversed",
  "perform_statement", "$@79", "perform_body", "$@80", "end_perform",
  "perform_procedure", "perform_option", "perform_test",
  "perform_condition", "perform_varying_list", "perform_varying",
  "pragma_statement", "$@81", "pragma_statement_list", "_pragma_statement",
  "read_statement", "$@82", "read_into", "_with_lock", "with_lock",
  "read_key", "read_handler", "end_read", "ready_statement",
  "reset_statement", "release_statement", "$@83", "return_statement",
  "$@84", "end_return", "rewrite_statement", "$@85", "write_lock",
  "end_rewrite", "rollback_statement", "search_statement", "$@86",
  "search_body", "$@87", "search_varying", "search_at_end", "$@88",
  "search_whens", "search_when", "$@89", "end_search", "set_statement",
  "$@90", "set_body", "set_environment", "set_to", "set_up_down",
  "up_or_down", "set_to_on_off_sequence", "set_to_on_off",
  "set_to_true_false_sequence", "set_to_true_false", "sort_statement",
  "$@91", "sort_body", "$@92", "sort_key_list", "opt_key_list",
  "sort_duplicates", "sort_collating", "sort_input", "sort_output",
  "start_statement", "$@93", "@94", "start_key", "start_op", "end_start",
  "stop_statement", "$@95", "stop_option", "stop_returning",
  "string_statement", "$@96", "string_item_list", "string_item",
  "opt_with_pointer", "end_string", "subtract_statement", "$@97",
  "subtract_body", "end_subtract", "suppress_statement", "_printing",
  "terminate_statement", "$@98", "transform_statement", "$@99",
  "unlock_statement", "$@100", "opt_record", "unstring_statement", "$@101",
  "unstring_delimited", "unstring_delimited_list",
  "unstring_delimited_item", "unstring_into", "unstring_into_item",
  "unstring_into_delimiter", "unstring_into_count", "unstring_tallying",
  "end_unstring", "use_statement", "use_exception", "use_global",
  "use_exception_target", "_after", "_standard", "exception_or_error",
  "exception_or_overflow", "not_exception_or_overflow", "_procedure",
  "use_debugging", "use_debugging_target", "use_reporting",
  "write_statement", "$@102", "write_from", "write_option",
  "before_or_after", "write_handler", "end_write", "on_accp_exception",
  "on_disp_exception", "on_xml_exception", "opt_on_exception", "$@103",
  "opt_not_on_exception", "$@104", "opt_on_exception_identifier",
  "on_size_error", "opt_on_size_error", "$@105", "opt_not_on_size_error",
  "$@106", "on_overflow", "opt_on_overflow", "$@107",
  "opt_not_on_overflow", "$@108", "at_end", "at_end_sentence", "$@109",
  "not_at_end_sentence", "$@110", "at_eop", "at_eop_sentence", "$@111",
  "not_at_eop_sentence", "$@112", "opt_invalid_key", "invalid_key",
  "invalid_key_sentence", "$@113", "not_invalid_key_sentence", "$@114",
  "xml_statement", "$@115", "xml_body", "xml_processing_procedure",
  "opt_count_in", "end_xml", "xml_returning", "xml_encoding",
  "_opt_scroll_lines", "condition", "expr", "partial_expr", "$@116",
  "expr_tokens", "expr_token", "eq", "gt", "lt", "ge", "le",
  "subref_exp_list", "subref_exp", "exp_list", "e_sep", "exp",
  "linage_counter", "arithmetic_x_list", "arithmetic_x", "record_name",
  "table_name", "file_name_list", "file_name", "mnemonic_name_list",
  "mnemonic_name", "procedure_name_list", "procedure_name", "label",
  "integer_label", "reference_list", "reference", "opt_reference",
  "reference_or_literal", "undefined_word", "_undefined_word",
  "target_x_list", "target_x", "x_list", "x", "arith_x", "prog_or_entry",
  "alnum_or_id", "simple_value", "simple_all_value", "null_or_id_or_lit",
  "id_or_lit_or_func", "_num_id_or_lit", "num_id_or_lit_positive",
  "num_id_or_lit", "null_or_lit", "identifier", "identifier_1",
  "qualified_label_word", "qualified_word", "subref", "refmod",
  "integer_positive", "integer", "literal", "basic_literal", "basic_value",
  "function", "func_refmod", "func_args", "list_func_args", "trim_args",
  "numvalc_args", "locale_dt_args", "not_const_word", "flag_all",
  "flag_duplicates", "flag_initialized", "flag_next", "flag_not",
  "flag_optional", "flag_rounded", "flag_separate", "in_of", "records",
  "with_dups", "coll_sequence", "_advancing", "_all", "_are", "_area",
  "_as", "_at", "_binary", "_by", "_character", "_characters", "_contains",
  "_comma", "_data", "_file", "_for", "_from", "_in", "_intrinsic", "_is",
  "_is_are", "_key", "_linkage", "_line_or_lines", "_lines", "_mode",
  "_number", "_of", "_on", "_or", "_in_order", "_other", "_program",
  "_program_name", "_record", "_returning", "_right", "_set", "_sign",
  "_sign_is", "_size", "_status", "_tape", "_than", "_then", "_times",
  "_to", "_when", "_with", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
     605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
     615,   616,   617,   618,   619,   620,   621,   622,   623,   624,
     625,   626,   627,   628,   629,   630,   631,   632,   633,   634,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   647,   648,   649,   650,   651,   652,   653,   654,
     655,   656,   657,   658,   659,   660,   661,   662,   663,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,   677,   678,   679,   680,   681,   682,   683,   684,
     685,   686,   687,   688,   689,   690,   691,   692,   693,   694,
     695,   696,   697,   698,   699,   700,   701,   702,   703,   704,
     705,   706,   707,   708,   709,   710,   711,   712,   713,   714,
     715,   716,   717,   718,   719,   720,   721,   722,   723,   724,
     725,   726,   727,   728,   729,   730,   731,   732,   733,   734,
     735,   736,    43,    45,    42,    47,   737,    94,    46,    61,
      40,    41,    62,    60,    58,    38
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   496,   498,   497,   499,   499,   501,   502,   500,   503,
     503,   503,   504,   505,   504,   506,   504,   508,   509,   507,
     510,   507,   511,   511,   512,   512,   512,   513,   513,   513,
     514,   514,   515,   515,   516,   516,   516,   516,   517,   517,
     518,   518,   519,   519,   519,   520,   520,   522,   521,   521,
     523,   523,   524,   524,   524,   524,   525,   526,   526,   526,
     526,   527,   528,   529,   529,   530,   531,   531,   531,   531,
     532,   532,   533,   533,   533,   534,   535,   535,   536,   537,
     538,   539,   539,   540,   540,   541,   541,   542,   542,   543,
     544,   544,   545,   545,   546,   546,   546,   546,   546,   546,
     546,   546,   546,   546,   546,   546,   547,   547,   548,   548,
     548,   549,   550,   549,   549,   551,   551,   552,   552,   554,
     553,   555,   555,   555,   555,   555,   556,   556,   557,   557,
     558,   557,   559,   559,   560,   560,   560,   560,   560,   560,
     560,   561,   561,   561,   561,   561,   561,   561,   562,   563,
     563,   564,   565,   565,   566,   566,   567,   568,   568,   569,
     569,   570,   571,   572,   573,   574,   575,   576,   577,   577,
     578,   578,   578,   579,   579,   580,   580,   582,   581,   583,
     583,   584,   584,   584,   584,   584,   584,   584,   584,   584,
     584,   584,   584,   584,   584,   585,   585,   585,   586,   586,
     586,   586,   587,   587,   587,   588,   588,   588,   589,   590,
     590,   590,   591,   592,   593,   594,   594,   594,   595,   596,
     596,   596,   597,   597,   597,   597,   598,   598,   599,   600,
     600,   601,   601,   601,   601,   601,   602,   603,   604,   605,
     605,   605,   606,   607,   607,   608,   609,   609,   609,   610,
     610,   611,   611,   612,   613,   613,   614,   614,   614,   615,
     616,   616,   616,   616,   617,   618,   618,   619,   620,   620,
     621,   622,   622,   622,   622,   623,   624,   623,   625,   626,
     626,   627,   627,   627,   627,   627,   628,   628,   629,   630,
     629,   631,   632,   632,   634,   633,   635,   635,   636,   636,
     636,   636,   636,   636,   636,   636,   636,   636,   636,   637,
     638,   638,   638,   639,   639,   639,   640,   640,   641,   641,
     642,   642,   643,   644,   644,   645,   645,   646,   646,   647,
     647,   648,   648,   650,   649,   651,   652,   652,   653,   653,
     653,   654,   655,   656,   657,   658,   659,   659,   661,   660,
     663,   662,   664,   664,   664,   666,   667,   665,   665,   668,
     668,   669,   669,   669,   670,   671,   671,   672,   672,   672,
     673,   674,   674,   675,   675,   675,   675,   675,   675,   675,
     675,   675,   675,   675,   675,   675,   675,   675,   676,   677,
     678,   679,   679,   680,   681,   682,   682,   682,   682,   683,
     683,   683,   683,   683,   683,   683,   683,   683,   683,   683,
     683,   683,   683,   683,   683,   683,   683,   683,   683,   683,
     683,   683,   683,   683,   683,   683,   683,   683,   683,   683,
     683,   683,   683,   683,   683,   683,   683,   684,   684,   685,
     686,   686,   687,   687,   688,   688,   688,   689,   689,   690,
     691,   692,   693,   693,   694,   694,   695,   696,   696,   697,
     698,   699,   700,   700,   700,   701,   702,   703,   704,   703,
     706,   705,   707,   707,   708,   708,   708,   709,   709,   710,
     710,   711,   713,   712,   715,   714,   716,   717,   716,   718,
     718,   719,   719,   720,   721,   721,   722,   722,   722,   722,
     723,   723,   724,   725,   725,   726,   726,   726,   727,   727,
     728,   728,   729,   729,   730,   730,   731,   731,   732,   733,
     733,   734,   734,   734,   734,   734,   735,   735,   735,   735,
     736,   736,   737,   738,   738,   739,   739,   739,   739,   739,
     739,   739,   739,   739,   739,   739,   739,   739,   739,   739,
     739,   740,   741,   741,   741,   741,   741,   741,   741,   742,
     743,   743,   743,   743,   743,   743,   744,   744,   745,   746,
     747,   748,   749,   750,   750,   751,   751,   752,   752,   752,
     753,   753,   753,   754,   755,   756,   756,   757,   758,   759,
     760,   758,   761,   761,   762,   762,   763,   764,   763,   765,
     765,   766,   766,   766,   766,   766,   766,   766,   766,   766,
     766,   766,   766,   766,   766,   766,   766,   766,   766,   766,
     766,   766,   766,   766,   766,   766,   766,   766,   766,   766,
     766,   766,   766,   766,   766,   766,   766,   767,   767,   767,
     767,   767,   768,   768,   768,   768,   768,   769,   770,   771,
     772,   770,   773,   774,   773,   775,   773,   776,   776,   777,
     778,   778,   778,   778,   779,   779,   780,   780,   780,   780,
     780,   781,   781,   782,   782,   783,   784,   783,   785,   785,
     786,   786,   786,   786,   786,   786,   786,   787,   788,   789,
     790,   791,   791,   792,   792,   794,   795,   793,   796,   796,
     798,   797,   797,   799,   799,   799,   799,   799,   799,   799,
     799,   799,   799,   799,   799,   799,   799,   799,   799,   799,
     799,   799,   799,   799,   799,   799,   799,   799,   799,   799,
     799,   799,   799,   799,   799,   799,   799,   799,   799,   799,
     799,   799,   799,   799,   799,   799,   799,   799,   799,   799,
     799,   799,   799,   799,   799,   799,   799,   799,   799,   801,
     800,   802,   802,   802,   802,   802,   802,   802,   802,   802,
     802,   802,   802,   802,   802,   802,   802,   802,   802,   802,
     803,   803,   803,   804,   805,   805,   806,   806,   807,   807,
     808,   808,   808,   808,   808,   808,   808,   808,   808,   808,
     808,   808,   808,   808,   808,   808,   808,   808,   808,   808,
     808,   808,   808,   808,   808,   808,   808,   808,   809,   809,
     811,   810,   812,   812,   812,   813,   813,   814,   814,   816,
     815,   817,   817,   818,   818,   820,   819,   821,   821,   822,
     822,   824,   823,   826,   825,   827,   827,   828,   828,   829,
     829,   830,   830,   830,   830,   831,   831,   831,   832,   832,
     833,   833,   835,   834,   836,   836,   837,   837,   838,   838,
     838,   838,   838,   839,   839,   839,   840,   840,   841,   842,
     841,   843,   844,   843,   845,   845,   846,   846,   848,   847,
     849,   849,   851,   850,   853,   854,   852,   855,   855,   855,
     856,   856,   856,   856,   857,   857,   859,   858,   860,   860,
     861,   861,   861,   861,   861,   862,   862,   864,   863,   865,
     866,   866,   867,   867,   868,   869,   871,   870,   872,   870,
     873,   873,   875,   874,   876,   876,   876,   876,   876,   877,
     877,   878,   878,   878,   878,   878,   878,   878,   878,   878,
     878,   878,   878,   878,   878,   878,   878,   878,   878,   878,
     878,   878,   878,   878,   878,   879,   879,   881,   880,   882,
     882,   882,   882,   882,   883,   883,   885,   884,   886,   886,
     888,   887,   889,   889,   890,   890,   890,   891,   891,   893,
     892,   894,   894,   895,   895,   895,   896,   897,   897,   899,
     898,   900,   900,   901,   901,   902,   902,   903,   903,   904,
     904,   904,   904,   905,   905,   906,   906,   908,   907,   909,
     910,   909,   909,   909,   909,   909,   911,   911,   913,   914,
     912,   915,   915,   916,   916,   917,   917,   917,   918,   918,
     919,   919,   919,   919,   919,   919,   920,   920,   922,   921,
     924,   923,   926,   925,   927,   927,   929,   928,   930,   930,
     932,   931,   934,   933,   933,   935,   935,   935,   935,   935,
     935,   936,   936,   938,   937,   939,   939,   940,   940,   940,
     941,   941,   942,   942,   943,   944,   944,   944,   944,   944,
     944,   944,   945,   945,   947,   946,   949,   948,   950,   950,
     950,   951,   951,   952,   952,   952,   954,   953,   955,   955,
     956,   956,   956,   956,   956,   956,   957,   958,   958,   959,
     959,   960,   960,   960,   960,   960,   961,   962,   963,   963,
     964,   964,   966,   965,   968,   967,   969,   969,   971,   970,
     972,   972,   973,   973,   975,   974,   976,   976,   977,   977,
     977,   977,   978,   978,   979,   979,   979,   980,   980,   982,
     981,   983,   984,   983,   983,   985,   985,   986,   986,   987,
     987,   987,   987,   987,   988,   988,   989,   989,   990,   990,
     991,   993,   992,   994,   994,   995,   995,   997,   996,   998,
     998,   999,   999,   999,   999,   999,   999,   999,  1000,  1000,
    1000,  1000,  1000,  1000,  1000,  1001,  1001,  1002,  1002,  1002,
    1003,  1003,  1004,  1005,  1007,  1006,  1009,  1008,  1010,  1010,
    1012,  1011,  1013,  1013,  1013,  1014,  1014,  1015,  1017,  1016,
    1018,  1019,  1018,  1020,  1020,  1021,  1022,  1021,  1023,  1023,
    1025,  1024,  1026,  1026,  1028,  1027,  1029,  1029,  1029,  1029,
    1029,  1030,  1031,  1031,  1032,  1033,  1033,  1034,  1034,  1035,
    1036,  1036,  1037,  1037,  1039,  1038,  1041,  1040,  1042,  1042,
    1043,  1043,  1044,  1044,  1045,  1045,  1046,  1046,  1046,  1047,
    1047,  1047,  1049,  1050,  1048,  1051,  1051,  1052,  1052,  1052,
    1052,  1052,  1053,  1053,  1055,  1054,  1056,  1056,  1056,  1057,
    1057,  1057,  1057,  1059,  1058,  1060,  1060,  1061,  1061,  1061,
    1062,  1062,  1063,  1063,  1065,  1064,  1066,  1066,  1066,  1067,
    1067,  1068,  1069,  1069,  1071,  1070,  1073,  1072,  1075,  1074,
    1076,  1076,  1076,  1078,  1077,  1079,  1079,  1080,  1080,  1081,
    1082,  1082,  1083,  1084,  1084,  1085,  1085,  1086,  1086,  1087,
    1087,  1088,  1088,  1088,  1089,  1090,  1090,  1091,  1091,  1091,
    1091,  1091,  1092,  1092,  1093,  1093,  1094,  1094,  1095,  1095,
    1096,  1096,  1097,  1097,  1098,  1099,  1099,  1100,  1102,  1101,
    1103,  1103,  1104,  1104,  1104,  1104,  1105,  1105,  1106,  1106,
    1106,  1107,  1107,  1108,  1109,  1110,  1111,  1112,  1111,  1113,
    1114,  1113,  1115,  1115,  1116,  1117,  1118,  1117,  1119,  1120,
    1119,  1121,  1122,  1123,  1122,  1124,  1125,  1124,  1126,  1126,
    1126,  1128,  1127,  1130,  1129,  1131,  1131,  1131,  1133,  1132,
    1135,  1134,  1136,  1136,  1137,  1137,  1137,  1139,  1138,  1141,
    1140,  1143,  1142,  1144,  1144,  1145,  1145,  1145,  1146,  1146,
    1147,  1147,  1148,  1148,  1149,  1149,  1150,  1150,  1151,  1152,
    1154,  1153,  1155,  1155,  1156,  1156,  1156,  1156,  1156,  1156,
    1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,
    1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,
    1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,  1156,
    1156,  1157,  1157,  1157,  1158,  1158,  1159,  1159,  1160,  1160,
    1161,  1161,  1162,  1162,  1163,  1163,  1164,  1164,  1165,  1165,
    1165,  1166,  1166,  1166,  1166,  1166,  1166,  1166,  1166,  1166,
    1166,  1166,  1166,  1166,  1167,  1167,  1168,  1168,  1169,  1170,
    1171,  1172,  1172,  1173,  1174,  1174,  1175,  1176,  1176,  1177,
    1178,  1178,  1178,  1179,  1180,  1180,  1181,  1182,  1182,  1183,
    1183,  1184,  1185,  1185,  1186,  1186,  1186,  1187,  1187,  1188,
    1188,  1189,  1189,  1189,  1189,  1189,  1189,  1189,  1189,  1189,
    1190,  1190,  1190,  1190,  1190,  1190,  1190,  1191,  1191,  1192,
    1192,  1193,  1193,  1193,  1194,  1194,  1194,  1195,  1195,  1196,
    1196,  1196,  1197,  1197,  1198,  1198,  1198,  1199,  1199,  1199,
    1200,  1200,  1201,  1202,  1202,  1202,  1202,  1203,  1203,  1204,
    1204,  1204,  1205,  1206,  1206,  1207,  1208,  1209,  1209,  1210,
    1210,  1211,  1211,  1211,  1211,  1211,  1211,  1211,  1211,  1212,
    1212,  1212,  1212,  1212,  1212,  1212,  1212,  1212,  1212,  1212,
    1212,  1212,  1212,  1212,  1212,  1213,  1213,  1213,  1214,  1214,
    1215,  1215,  1216,  1216,  1216,  1217,  1217,  1218,  1218,  1219,
    1220,  1220,  1221,  1221,  1222,  1222,  1223,  1223,  1223,  1224,
    1224,  1225,  1225,  1225,  1226,  1226,  1227,  1227,  1228,  1228,
    1229,  1229,  1229,  1229,  1230,  1230,  1231,  1231,  1232,  1232,
    1233,  1233,  1234,  1234,  1235,  1235,  1236,  1236,  1237,  1237,
    1238,  1238,  1239,  1239,  1240,  1240,  1241,  1241,  1242,  1242,
    1243,  1243,  1244,  1244,  1245,  1245,  1246,  1246,  1247,  1247,
    1248,  1248,  1249,  1249,  1250,  1250,  1251,  1251,  1251,  1252,
    1252,  1253,  1253,  1254,  1254,  1254,  1255,  1255,  1256,  1256,
    1257,  1257,  1258,  1258,  1259,  1259,  1260,  1260,  1261,  1261,
    1261,  1262,  1262,  1263,  1263,  1264,  1264,  1265,  1265,  1266,
    1266,  1266,  1267,  1267,  1268,  1268,  1269,  1269,  1270,  1270,
    1270,  1271,  1271,  1272,  1272,  1273,  1273,  1274,  1274,  1275,
    1275,  1276,  1276,  1277,  1277,  1278,  1278,  1279,  1279
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     0,     0,     9,     0,
       1,     2,     0,     0,     4,     0,     4,     0,     0,     8,
       0,     6,     0,     1,     0,     1,     2,     1,     1,     1,
       0,     2,     0,     3,     1,     2,     1,     1,     1,     1,
       0,     2,     3,     1,     1,     1,     2,     0,     4,     1,
       1,     2,     1,     1,     1,     1,     3,     0,     2,     3,
       2,     3,     1,     1,     2,     3,     0,     2,     3,     2,
       1,     2,     1,     1,     1,     5,     1,     1,     4,     3,
       3,     0,     2,     1,     2,     3,     3,     1,     2,     3,
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     2,     1,
       2,     3,     0,     5,     4,     0,     5,     1,     1,     0,
       5,     1,     1,     1,     1,     1,     1,     2,     1,     3,
       0,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       2,     3,     1,     2,     1,     2,     4,     1,     2,     1,
       3,     4,     4,     3,     3,     4,     3,     3,     1,     2,
       3,     1,     1,     1,     2,     2,     1,     0,     6,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     5,     4,     4,     0,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     4,     1,
       1,     1,     6,     3,     5,     0,     1,     1,     4,     2,
       2,     1,     0,     4,     5,     2,     1,     1,     2,     3,
       1,     1,     3,     2,     1,     2,     4,     4,     5,     1,
       3,     4,     4,     3,     2,     3,     2,     2,     2,     1,
       2,     2,     1,     2,     1,     2,     1,     1,     1,     5,
       0,     1,     1,     1,     5,     1,     2,     2,     0,     2,
       4,     1,     1,     1,     3,     0,     0,     4,     3,     0,
       2,     1,     1,     1,     1,     1,     1,     2,     3,     0,
       3,     2,     1,     1,     0,     4,     0,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       0,     1,     1,     4,     6,     9,     0,     3,     0,     2,
       0,     2,     3,     1,     1,     0,     1,     5,     5,     1,
       1,     1,     1,     0,     4,     5,     0,     2,     1,     1,
       1,     6,     5,     5,     4,     3,     3,     3,     0,     6,
       0,     2,     0,     2,     3,     0,     0,     6,     2,     1,
       1,     0,     1,     1,     1,     0,     2,     1,     3,     3,
       6,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       3,     0,     2,     2,     1,     1,     3,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     2,     2,     1,     2,     2,     1,     2,
       2,     1,     2,     2,     1,     1,     1,     3,     3,     5,
       0,     1,     1,     2,     1,     1,     1,     0,     2,     3,
       1,     4,     1,     2,     1,     1,     3,     1,     2,     1,
       2,     2,     0,     1,     1,     3,     1,     1,     0,     8,
       0,     8,     1,     2,     1,     3,     1,     0,     3,     2,
       4,     2,     0,     5,     0,     5,     0,     0,     5,     0,
       1,     1,     2,     5,     0,     2,     2,     3,     1,     1,
       3,     3,     2,     0,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     0,     2,     5,     0,
       2,     7,     3,     4,     7,     4,     1,     4,     2,     2,
       0,     2,     4,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     2,     2,     1,     2,     2,     2,     4,
       4,     2,     1,     2,     2,     1,     1,     2,     3,     1,
       3,     6,     2,     3,     2,     1,     2,     2,     1,     2,
       0,     1,     1,     4,     2,     0,     1,     1,     0,     0,
       0,     6,     0,     1,     1,     2,     1,     0,     5,     0,
       2,     1,     1,     1,     2,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     5,     5,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     3,     3,     0,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     3,     0,     0,
       0,    10,     0,     0,     3,     0,     3,     1,     2,     4,
       0,     2,     2,     2,     0,     1,     0,     3,     3,     4,
       3,     0,     1,     0,     2,     0,     0,     7,     0,     2,
       1,     1,     1,     1,     2,     2,     1,     4,     1,     2,
       1,     1,     2,     0,     1,     0,     0,     3,     0,     1,
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     0,
       4,     3,     4,     3,     3,     3,     4,     3,     3,     4,
       3,     3,     3,     3,     4,     5,     3,     4,     3,     3,
       2,     2,     2,     4,     4,     4,     0,     1,     1,     2,
       1,     2,     2,     2,     3,     2,     3,     3,     2,     2,
       2,     2,     2,     2,     2,     4,     4,     4,     4,     2,
       2,     2,     2,     2,     2,     2,     5,     3,     0,     1,
       0,     4,     4,     5,     6,     0,     2,     0,     1,     0,
       3,     3,     5,     0,     2,     0,     3,     0,     5,     0,
       2,     0,     9,     0,     9,     0,     2,     1,     1,     0,
       1,     0,     4,     3,     2,     0,     1,     1,     0,     1,
       0,     1,     0,     3,     1,     2,     2,     5,     0,     2,
       2,     2,     2,     2,     3,     4,     1,     1,     0,     0,
       3,     0,     0,     3,     0,     1,     0,     1,     0,     3,
       0,     2,     0,     3,     0,     0,     5,     0,     1,     2,
       0,     1,     2,     2,     0,     2,     0,     3,     0,     3,
       0,     1,     3,     3,     2,     1,     1,     0,     4,     4,
       0,     1,     1,     1,     1,     1,     0,     6,     0,     5,
       0,     1,     0,     4,     3,     3,     3,     3,     2,     1,
       2,     1,     1,     2,     2,     2,     2,     2,     2,     2,
       3,     2,     3,     3,     2,     2,     2,     2,     2,     4,
       4,     4,     4,     2,     2,     0,     1,     0,     4,     4,
       6,     6,     8,     8,     0,     1,     0,     6,     0,     1,
       0,     3,     1,     2,     2,     5,     4,     0,     2,     0,
       5,     1,     3,     1,     1,     1,     1,     1,     2,     0,
       3,     0,     1,     2,     3,     1,     1,     1,     3,     2,
       1,     1,     1,     0,     2,     0,     1,     0,     3,     0,
       0,     3,     1,     2,     1,     1,     0,     2,     0,     0,
       7,     1,     2,     1,     2,     0,     1,     1,     1,     2,
       0,     2,     2,     2,     2,     2,     0,     1,     0,     3,
       0,     3,     0,     5,     0,     3,     0,     3,     0,     2,
       0,     3,     0,     4,     1,     1,     2,     3,     4,     3,
       2,     0,     1,     0,     7,     0,     2,     0,     3,     3,
       0,     2,     1,     2,     4,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     0,     3,     0,     4,     1,     1,
       1,     1,     2,     1,     1,     1,     0,     3,     1,     2,
       2,     2,     1,     1,     1,     2,     2,     1,     2,     4,
       2,     0,     1,     1,     1,     1,     4,     6,     0,     4,
       0,     1,     0,     3,     0,     3,     3,     4,     0,     4,
       4,     6,     0,     1,     0,     3,     0,     6,     1,     1,
       1,     1,     0,     3,     0,     3,     2,     0,     1,     0,
       3,     2,     0,     4,     2,     0,     1,     1,     3,     0,
       1,     2,     3,     3,     0,     3,     1,     1,     1,     3,
       7,     0,     4,     1,     2,     0,     1,     0,    11,     0,
       2,     0,     2,     2,     3,     3,     2,     3,     0,     2,
       2,     3,     3,     2,     3,     0,     3,     0,     1,     1,
       0,     1,     2,     2,     0,     4,     0,     7,     0,     1,
       0,     7,     0,     2,     3,     0,     1,     1,     0,     4,
       4,     0,     7,     0,     2,     0,     0,     4,     1,     2,
       0,     4,     0,     1,     0,     3,     1,     1,     1,     1,
       1,     4,     4,     3,     4,     1,     1,     1,     2,     3,
       1,     2,     3,     3,     0,     3,     0,     7,     0,     5,
       0,     2,     0,     2,     0,     3,     0,     2,     4,     0,
       2,     4,     0,     0,     8,     0,     4,     2,     2,     2,
       2,     2,     0,     1,     0,     3,     2,     1,     2,     0,
       1,     2,     2,     0,     8,     1,     2,     1,     3,     3,
       0,     3,     0,     1,     0,     4,     4,     6,     6,     0,
       1,     2,     0,     1,     0,     3,     0,     7,     0,     4,
       0,     1,     1,     0,     9,     0,     3,     1,     3,     2,
       2,     2,     3,     0,     3,     0,     3,     0,     3,     0,
       1,     1,     1,     1,     8,     0,     1,     1,     1,     1,
       1,     1,     0,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     5,     1,     2,     5,     0,     8,
       0,     2,     0,     4,     3,     3,     1,     1,     0,     1,
       1,     0,     1,     2,     2,     2,     0,     0,     4,     0,
       0,     3,     0,     1,     2,     0,     0,     3,     0,     0,
       3,     2,     0,     0,     3,     0,     0,     3,     1,     1,
       2,     0,     3,     0,     3,     1,     1,     2,     0,     3,
       0,     3,     0,     1,     1,     1,     2,     0,     3,     0,
       3,     0,     5,     8,     5,     1,     1,     2,     0,     3,
       0,     1,     0,     2,     0,     2,     0,     3,     1,     1,
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     2,     1,     2,     1,     5,
       1,     5,     1,     3,     1,     1,     1,     3,     0,     1,
       1,     1,     3,     3,     3,     3,     2,     2,     3,     3,
       3,     3,     3,     2,     1,     3,     1,     2,     2,     1,
       1,     1,     2,     1,     1,     2,     1,     0,     2,     1,
       1,     1,     3,     1,     1,     2,     1,     0,     1,     1,
       1,     1,     0,     1,     1,     2,     3,     1,     3,     1,
       2,     1,     3,     3,     3,     4,     3,     1,     1,     1,
       1,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     1,     1,     2,     2,     3,     1,     3,     1,
       3,     2,     3,     4,     5,     1,     1,     1,     2,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     5,     5,     5,     5,     5,     5,     5,     5,     5,
       4,     5,     5,     5,     2,     0,     4,     5,     0,     3,
       0,     1,     1,     3,     3,     1,     3,     1,     3,     0,
       0,     1,     0,     1,     0,     1,     0,     1,     1,     0,
       1,     0,     2,     1,     0,     1,     0,     2,     1,     1,
       2,     2,     2,     2,     2,     1,     2,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1,     1,     0,
       1,     0,     1,     0,     1,     1,     0,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       2,     0,     1,     0,     1,     0,     1,     0,     1,     0,
       1,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       2,     0,     1,     0,     1,     0,     1,     0,     1,     0,
       1,     0,     1,     0,     1,     0,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,    22,    22,     3,     4,    40,    23,
      20,    17,     5,     6,     0,     0,     0,     0,     0,     0,
       0,     0,  1724,     0,     0,   260,  1681,     0,     0,   279,
      41,    43,    45,    49,    50,    52,    53,    55,    54,    44,
     168,   171,   173,   176,   172,   249,   252,     0,   254,   256,
     257,   258,    29,    28,    27,    30,    30,   272,   273,  1793,
     271,  1754,     0,     0,   175,     0,   251,  1725,  1785,    66,
      81,   261,   262,   263,  1704,     0,  1683,     0,    57,    90,
       7,   276,    46,    51,   169,   174,   250,   253,   255,     0,
      22,    18,  1794,     0,  1755,     0,    47,    42,   170,  1786,
    1718,     0,  1764,  1734,    63,  1763,    62,    65,  1763,    70,
      72,    73,    74,     0,     0,    80,     0,    83,  1705,  1726,
    1682,  1561,   177,  1798,    56,     0,  1797,     0,     0,     0,
       0,  1776,  1734,  1734,  1734,     0,  1778,  1734,   109,  1716,
    1734,    89,     0,    92,   105,     0,    94,    95,    96,    98,
      97,    99,   100,   101,   102,   103,   104,   648,     0,   292,
       0,     0,   293,     0,     0,   486,   281,   280,   282,   286,
     289,   283,   284,   285,    31,    21,    32,  1619,   274,  1612,
    1613,  1543,   270,  1541,    48,  1719,     0,  1734,  1735,     0,
      67,  1763,    64,    69,    71,     0,  1697,  1734,     0,    87,
    1732,    82,    84,  1727,     0,   179,    60,    58,     0,     0,
     119,  1734,  1734,  1777,  1734,     0,     0,     0,  1734,  1779,
     108,     0,   110,  1717,     0,  1734,  1562,    91,    93,  1686,
    1686,     0,     9,    22,     0,     0,     0,    22,     0,   588,
     287,     0,    22,     0,  1688,  1689,     0,  1621,     0,  1614,
    1615,  1542,   264,   265,   268,     0,  1625,    79,    68,  1696,
       0,    86,  1733,    88,    85,   259,   215,    59,    61,  1734,
       0,     0,     0,   164,  1556,   163,   167,     0,  1780,   166,
     148,   149,  1736,   152,     0,   111,  1563,   112,  1714,   106,
     107,   849,    10,    12,   278,   484,   482,   288,   348,     0,
       0,   277,   290,   350,   294,    19,    34,    36,    39,    38,
    1763,    37,  1620,  1515,     0,     0,  1655,  1635,     0,  1658,
    1636,  1752,  1534,  1631,     0,     0,  1637,     0,     0,  1634,
       0,  1632,     0,     0,  1638,     0,     0,     0,     0,  1655,
    1633,     0,     0,     0,  1518,  1512,  1514,  1586,  1521,  1580,
    1584,  1629,  1585,     0,  1616,   266,     0,   267,     0,    78,
    1748,  1767,  1793,  1711,   231,     0,  1748,  1734,  1714,  1739,
     234,     0,  1797,   217,   216,   178,   180,   181,   182,   183,
     184,   185,     0,   186,   187,   188,   230,   189,   190,   191,
     192,   193,   194,  1734,     0,     0,   156,   157,   159,   165,
     162,   161,   150,  1738,  1737,   153,     0,   114,   115,  1715,
    1687,  1546,   652,   850,    15,    13,    11,  1185,   350,   350,
     350,   487,     0,   291,   352,   296,    35,    33,  1533,     0,
       0,  1639,     0,  1660,  1654,  1753,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1640,
    1526,  1527,     0,  1519,  1520,  1622,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   269,    76,
      77,    75,  1749,  1734,  1768,  1739,   202,   228,   235,  1734,
    1710,  1734,  1734,  1740,     0,  1734,  1734,   244,  1704,     0,
    1734,     0,   233,   138,   124,   139,   134,   140,   121,   137,
     135,   122,   123,   136,   120,   125,   126,   128,   158,     0,
     151,   154,   113,   655,   653,   673,  1765,  1765,  1181,  1183,
    1186,     8,   485,   483,    22,   489,   589,   351,     0,  1734,
    1518,  1516,     0,  1518,  1518,     0,  1581,  1582,  1583,  1535,
    1518,     0,     0,  1518,  1518,     0,     0,  1518,  1518,  1518,
       0,     0,     0,     0,  1529,  1513,  1514,  1530,  1531,  1532,
    1522,  1523,  1524,  1525,  1528,  1623,     0,  1630,     0,  1734,
     204,   203,   198,     0,     0,  1710,   234,   229,     0,     0,
     232,     0,     0,   243,  1761,  1761,     0,   245,     0,   213,
     127,   130,     0,   160,   155,   118,   117,  1783,   660,   660,
       0,     0,  1766,     0,     0,     0,  1184,   349,     0,   488,
     490,   491,   590,     0,   360,   359,   353,   361,    24,  1718,
    1734,   333,   325,  1734,  1718,  1748,  1734,  1702,     0,   295,
     297,   300,   301,   302,   303,   304,   305,   306,   307,   308,
       0,  1655,     0,     0,  1655,  1659,     0,  1655,  1655,  1655,
       0,  1650,  1655,  1655,  1655,     0,  1655,  1655,  1655,  1655,
    1624,   210,   211,   209,   208,     0,   196,     0,   197,     0,
     201,   222,   221,   222,   218,  1560,  1559,   236,   237,  1619,
    1672,   239,     0,   242,  1762,   246,   247,   248,  1557,     0,
     129,  1784,  1734,  1713,   660,   657,   666,     0,   660,   674,
     649,    16,    14,  1610,   587,   494,   492,   592,   354,   362,
     363,   355,     0,    25,   358,     0,     0,     0,  1702,  1702,
       0,   326,     0,     0,     0,  1734,     0,  1703,     0,     0,
     298,   299,  1644,  1517,  1656,     0,  1653,  1668,  1651,  1642,
    1652,  1666,  1643,  1645,  1646,  1663,  1664,  1647,  1648,  1649,
    1641,  1672,   206,   205,   195,   207,     0,   220,   219,  1695,
       0,   238,  1673,  1734,     0,  1558,   214,   145,   146,   141,
     147,   144,   142,   143,   131,   132,     0,   658,  1734,     0,
     671,   663,   661,   662,   675,  1611,  1182,  1734,   361,   596,
     591,   593,   594,   371,   365,    26,   320,   345,     0,  1691,
    1690,  1692,  1693,   324,   323,   322,  1746,  1716,  1730,     0,
     346,   347,   329,   330,  1734,  1734,  1657,   212,     0,   225,
    1694,     0,   240,  1554,     0,   116,     0,  1734,   672,     0,
       0,   650,  1734,  1734,  1702,   505,   530,   495,   498,   499,
       0,   597,   595,   356,  1706,     0,     0,   310,   334,  1747,
     336,     0,   313,  1731,  1781,   344,     0,     0,     0,   241,
    1555,   133,   667,   668,   670,     0,   659,   676,   678,     0,
     503,   503,   506,   507,  1736,   493,   496,   599,     0,   466,
     399,   434,   422,   431,   428,   425,   400,  1795,   401,   402,
     403,   404,   405,   406,   407,   408,   409,   436,   410,  1772,
       0,   435,     0,   411,   394,   412,   413,     0,     0,   415,
     416,   414,   462,   418,   419,   417,  1734,  1736,    24,   372,
     387,   373,   374,   375,   376,   377,   395,   378,   379,   380,
     381,   382,   383,   384,   385,   386,     0,     0,  1707,     0,
     366,   321,   311,   312,   309,   335,  1716,  1782,   318,   331,
     327,   332,   328,     0,   226,   227,   223,   669,   678,     0,
     497,  1599,  1600,  1601,   504,   500,   516,   501,     0,   361,
     531,  1778,   481,   432,   433,   420,   421,   429,   430,   426,
     427,   423,   424,  1796,     0,  1773,   460,   398,   447,   389,
     479,   463,   464,   461,     0,     0,   357,   391,   393,   388,
    1686,  1686,     0,  1752,  1752,   370,   367,  1627,   337,   338,
     339,   340,  1708,     0,   314,  1729,   320,     0,   224,     0,
       0,   690,   686,   679,   680,     0,   681,   683,     0,     0,
       0,   682,   502,   519,   526,   363,   533,   613,  1734,   603,
     601,   602,   605,  1750,  1734,     0,  1734,     0,   617,   608,
    1750,   619,   609,     0,     0,   612,  1714,   616,   610,   615,
    1734,   614,     0,   611,   618,     0,  1736,   598,   630,   625,
     628,   627,   626,   629,   467,   472,   600,   631,   476,   474,
     465,     0,  1791,     0,   397,   396,   470,     0,   390,   437,
     438,  1628,     0,     0,  1709,     0,  1746,  1716,   319,     0,
     685,   692,   689,   693,   759,   820,   829,   835,   841,   888,
     843,   892,   906,   924,   917,   925,   926,   932,   967,   976,
     989,  1017,  1028,  1048,  1050,  1793,  1056,  1060,  1073,  1094,
    1096,  1132,  1134,  1138,     0,  1144,  1159,  1187,     0,  1214,
       0,  1216,  1220,  1227,  1228,  1244,  1264,  1282,  1294,  1303,
    1314,  1322,  1324,  1326,  1328,  1333,  1355,  1378,  1441,   684,
     702,   703,   704,   705,   706,   707,   709,   708,   710,   711,
     713,   712,   714,   715,   716,   717,   718,   719,   720,   721,
     722,   723,   724,   725,   726,   727,   728,   729,   730,   731,
     732,   733,   734,   735,   736,   737,   739,   738,   740,   741,
     742,   743,   744,   745,   746,   747,   748,   749,   750,   751,
     752,   753,   754,   755,  1351,  1352,  1353,   756,   757,   701,
     517,   518,   510,   511,   508,   509,   528,   529,  1778,     0,
    1751,  1734,     0,   606,   607,     0,   633,  1734,   604,  1791,
    1734,     0,   634,   632,     0,   473,     0,   448,  1792,   440,
     480,  1795,   392,   369,   368,     0,     0,  1708,   316,     0,
     694,     0,     0,     0,  1460,   837,   849,   890,   849,  1597,
     908,     0,   928,     0,  1797,     0,     0,  1460,  1019,  1035,
       0,     0,  1052,  1058,     0,     0,   516,     0,     0,     0,
       0,   758,  1146,  1169,     0,  1212,     0,  1213,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1323,  1321,   516,
       0,     0,     0,  1356,  1362,     0,     0,     0,  1734,     0,
     520,     0,     0,   565,  1750,   562,   585,   580,  1702,     0,
       0,  1734,  1752,  1734,     0,   532,   538,   539,   540,   549,
     541,   543,   546,   534,   535,   536,   542,   545,   566,   547,
     550,   537,     0,   544,   548,  1606,   624,  1604,  1605,   642,
     635,   623,   637,   647,  1610,   636,   468,   475,   454,  1754,
     455,  1712,   439,   441,   442,   444,   445,   452,   450,  1739,
     446,  1774,  1746,  1746,     0,  1754,   315,   677,   687,   818,
     786,  1752,     0,  1752,   827,  1579,   825,  1569,  1571,  1577,
    1578,  1674,   830,     0,  1459,     0,   836,     0,   889,     0,
     893,   894,  1598,   907,   920,     0,  1536,  1684,     0,  1767,
    1709,     0,   942,   965,  1396,   939,   941,     0,     0,   974,
       0,   849,   995,   994,     0,   991,   993,  1025,  1022,  1020,
    1024,  1018,  1037,  1036,     0,  1033,  1752,  1049,  1564,  1567,
    1051,  1547,  1771,  1770,  1057,     0,  1064,  1061,  1789,  1458,
    1075,  1095,     0,  1098,  1099,  1100,  1133,  1268,     0,  1135,
       0,  1142,     0,  1145,  1170,  1553,  1617,  1160,  1169,  1162,
       0,  1167,  1549,  1551,     0,  1550,     0,  1676,  1380,  1539,
    1767,  1380,     0,  1242,  1233,  1540,     0,  1245,  1246,  1247,
    1248,  1249,  1257,  1250,  1260,     0,  1544,     0,  1265,  1191,
    1299,  1297,  1299,  1295,  1712,     0,  1305,  1307,     0,  1319,
       0,  1325,     0,  1330,  1335,  1363,     0,  1364,  1754,  1380,
       0,     0,  1396,     0,   514,     0,   512,   513,  1734,  1734,
     527,   563,   564,   561,  1734,   586,   584,   581,   582,  1736,
     574,  1734,  1460,     0,     0,     0,     0,   567,     0,     0,
     572,   575,   578,   645,   643,   644,   646,  1602,   640,   638,
     639,   641,  1602,   620,  1795,     0,     0,   443,   453,  1734,
    1775,  1793,   343,   342,  1746,     0,   819,   760,     0,  1362,
     790,  1396,   787,   788,     0,     0,     0,     0,   828,   821,
       0,     0,  1570,  1675,   833,  1674,  1493,  1494,  1495,  1489,
    1472,  1475,  1473,  1474,  1793,  1503,  1499,  1479,  1787,  1500,
    1480,  1787,  1481,  1497,  1488,  1492,  1491,  1490,  1496,  1498,
    1467,  1468,  1469,  1470,  1471,  1501,  1465,  1466,  1504,  1506,
    1461,  1462,  1476,  1477,  1478,  1464,  1553,  1617,     0,   851,
     891,   851,   897,   910,   921,   918,   923,   922,  1460,  1537,
    1685,  1538,   930,  1432,   782,  1591,  1592,  1593,   946,   945,
     944,   943,   966,   933,  1397,   940,   938,  1399,   941,  1396,
    1396,  1396,  1396,  1750,  1750,  1750,   780,   781,  1734,   949,
     963,   964,   951,   948,     0,  1734,   954,   955,     0,   947,
     958,   956,     0,   957,   975,   968,     0,     0,   978,  1460,
    1460,  1015,   996,   997,   999,  1023,  1026,  1040,  1034,  1031,
       0,  1565,     0,  1054,  1059,  1790,  1062,  1077,     0,  1700,
    1121,  1106,  1097,  1101,  1103,  1104,  1105,  1272,     0,     0,
    1143,  1139,     0,  1151,  1148,  1150,  1149,  1152,     0,  1161,
    1166,   695,  1164,  1460,     0,     0,     0,  1171,     0,  1677,
    1678,  1767,     0,  1215,  1189,  1222,  1235,  1243,  1229,     0,
    1235,     0,  1258,  1261,     0,     0,  1545,  1256,     0,  1255,
       0,     0,  1283,     0,     0,     0,  1298,  1300,  1296,     0,
       0,  1306,     0,  1320,  1315,     0,     0,  1331,  1332,  1329,
    1712,     0,     0,  1365,     0,     0,  1222,     0,  1797,  1450,
    1399,   522,   515,     0,     0,     0,   573,     0,   570,  1684,
     568,   569,     0,   555,     0,     0,   551,     0,   579,   577,
     576,  1626,  1609,   622,  1603,  1607,  1608,   621,  1774,   449,
     459,   456,   457,     0,   477,   341,   317,   776,  1396,   767,
     770,   764,   773,   765,   768,   771,     0,  1396,     0,   763,
     772,   779,   778,     0,   761,  1399,   789,   809,  1734,   793,
     795,   791,   792,     0,  1734,   811,   798,   804,   799,     0,
     802,   815,   812,   800,     0,   813,   810,   801,   814,   803,
    1588,  1587,     0,  1576,     0,  1572,  1573,  1574,  1405,  1684,
       0,     0,   831,     0,  1502,  1788,  1505,  1507,  1485,  1486,
    1487,  1482,  1483,  1484,  1463,     0,   839,   855,  1741,   858,
     845,   860,   898,   900,   895,   915,   916,   909,   911,     0,
    1405,   931,   929,  1437,  1439,   930,  1433,  1434,  1435,  1402,
    1400,  1394,   936,   937,   934,   935,     0,     0,     0,     0,
     952,   953,     0,   950,  1456,  1456,     0,  1405,  1684,   980,
     987,   979,   992,  1010,  1005,  1012,  1011,  1003,  1006,  1007,
    1013,  1016,   990,   998,  1460,   695,  1021,     0,     0,  1029,
    1038,  1032,     0,  1568,  1566,  1754,  1053,  1548,   695,  1793,
    1085,  1086,  1088,  1090,  1091,  1087,  1089,  1080,  1793,  1076,
    1701,     0,  1122,     0,  1124,  1123,  1125,  1116,  1117,     0,
       0,  1102,  1274,  1758,     0,     0,  1136,  1405,  1684,  1797,
       0,  1618,  1165,   696,  1177,  1172,  1176,  1173,  1178,     0,
    1168,  1552,  1387,  1386,  1175,  1198,  1381,     0,     0,  1432,
       0,     0,     0,  1234,     0,     0,  1794,  1259,     0,  1263,
    1262,  1253,     0,  1192,  1285,     0,     0,  1193,     0,  1196,
    1302,  1301,  1308,  1309,  1310,     0,  1405,  1684,     0,  1670,
       0,  1310,  1377,  1367,  1366,  1372,     0,  1374,  1375,  1382,
       0,  1454,  1451,  1442,  1395,     0,   525,   523,   560,   559,
     583,   556,   554,   557,   553,   558,   552,     0,  1793,   458,
     451,  1734,   471,   777,   766,   769,  1396,   774,   762,   817,
    1393,     0,   796,   797,     0,   794,  1734,  1456,  1456,  1590,
    1575,  1589,  1684,  1406,   822,  1408,  1405,   834,     0,     0,
       0,   856,   857,  1741,  1742,   854,   859,   855,   878,   862,
     878,   861,     0,     0,   899,   901,   904,     0,   914,     0,
     919,   695,   695,   927,  1436,   695,  1403,   695,  1720,  1720,
    1720,   960,   959,   962,     0,   961,     0,   969,     0,   868,
       0,   977,  1460,  1460,  1009,  1004,  1002,  1000,  1027,  1044,
    1043,  1042,  1041,  1046,  1039,  1045,     0,   695,  1065,  1071,
    1063,     0,     0,  1092,     0,     0,     0,  1118,  1120,     0,
    1112,  1128,  1113,  1114,  1107,  1108,  1128,  1266,  1734,     0,
    1759,  1273,  1739,  1137,  1140,     0,     0,  1157,  1163,   700,
       0,     0,     0,     0,  1189,  1190,  1421,  1423,  1218,  1418,
    1419,  1225,  1223,     0,  1460,  1236,  1460,  1230,  1238,  1251,
    1252,  1254,  1734,  1432,  1195,  1197,  1194,  1412,     0,  1684,
    1316,     0,     0,  1671,  1756,  1337,     0,  1340,  1343,  1347,
    1341,  1373,  1754,  1376,  1388,  1698,  1448,     0,  1452,  1734,
    1734,     0,   477,     0,   775,   806,   805,     0,   808,   807,
    1405,   695,  1409,  1404,   823,   832,   840,   838,   853,  1741,
    1368,   877,  1369,   876,   846,   847,   848,     0,   881,   879,
     868,   881,   902,   903,     0,   896,   912,   913,  1438,  1440,
    1398,  1401,  1721,   784,   783,   785,  1743,  1405,  1536,  1405,
    1536,   666,   981,   982,     0,   988,  1008,  1014,  1047,  1030,
    1055,  1070,  1071,   695,  1072,  1066,  1078,  1081,  1082,  1722,
    1093,  1074,  1079,     0,  1128,     0,  1111,  1109,  1110,  1115,
    1276,     0,  1760,  1270,  1405,  1153,  1158,  1154,   697,  1179,
       0,  1199,     0,     0,  1200,     0,  1203,  1191,   695,   695,
    1219,  1217,  1420,  1226,  1221,  1224,  1231,   695,  1240,  1239,
    1679,  1292,  1413,  1312,  1415,     0,  1405,  1405,  1327,  1594,
    1595,  1596,  1757,  1670,  1339,  1730,  1345,  1730,  1412,     0,
    1428,  1430,  1391,  1389,  1425,  1426,  1390,  1699,     0,  1730,
    1444,  1455,     0,     0,     0,     0,   571,   469,   478,   816,
     824,  1407,   695,   852,  1752,     0,   873,  1370,  1371,   884,
     882,   695,   863,   864,   666,   886,   905,  1744,  1745,  1457,
     971,     0,   970,     0,   984,     0,   983,   870,   871,   869,
     872,  1069,  1067,  1071,  1083,  1723,     0,  1128,  1119,  1128,
    1130,     0,     0,  1279,  1275,  1269,  1141,  1147,     0,     0,
    1202,  1204,  1201,  1205,  1422,  1424,   695,  1237,   695,  1680,
       0,     0,  1293,  1284,   695,  1313,  1304,  1416,  1411,  1311,
    1318,  1317,  1338,     0,  1730,  1342,     0,  1349,  1361,  1358,
    1360,  1359,  1354,  1357,   695,   695,  1392,  1379,  1427,  1385,
    1384,  1743,     0,  1453,  1446,  1445,  1734,   521,   524,  1410,
       0,   874,   885,   842,   695,   880,   865,   866,     0,   887,
     844,  1405,  1405,   666,   666,  1068,     0,  1127,  1126,  1131,
       0,  1734,  1277,     0,     0,  1267,  1271,  1156,     0,     0,
    1734,  1207,   699,  1232,  1241,  1286,  1508,  1787,  1510,  1787,
    1287,  1288,  1289,  1290,  1291,  1414,   695,  1344,     0,  1348,
    1350,  1334,  1429,  1431,  1383,  1449,  1447,     0,   875,   883,
     666,   973,   972,   986,   664,  1084,  1129,     0,  1280,  1734,
    1155,  1460,     0,  1210,  1208,  1209,  1505,  1507,  1417,  1346,
    1443,   664,   665,   985,  1278,     0,  1180,  1206,  1211,  1188,
       0,     0,   867,  1281,  1793,  1793,  1509,  1511
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,     7,    29,   157,   293,   417,   517,
     516,     8,    15,   176,    14,    10,   714,   602,    90,   242,
     310,   311,    13,    30,    31,    32,   184,    33,    34,    35,
     124,   125,   105,   106,    36,   107,   108,   109,   110,   471,
     111,   112,    37,   115,   116,   117,   200,    38,   141,   142,
     143,   144,   145,   146,   408,   512,   597,   147,   269,   504,
     505,   506,   689,   774,   507,   775,   148,   280,   281,   282,
     510,   149,   396,   397,   150,   151,   152,   153,   154,   155,
     156,    39,    40,    41,    42,    43,   205,   266,   376,   377,
     669,   572,   754,   378,   664,   379,   380,   381,   382,   383,
     674,   757,   956,   384,   385,   386,   387,   388,   389,   680,
     390,   391,   392,   587,    44,    45,    46,    47,    48,    49,
      74,    50,   252,   253,   357,    51,    61,    80,   165,   166,
      81,   167,   168,   169,   241,   302,   170,   303,   425,   529,
     630,   631,   944,   632,  1386,  1016,   847,   633,   805,   720,
     634,   815,   950,   635,   717,   636,   945,  1008,  1009,  1010,
    1011,   637,   638,   639,   171,   420,   423,   424,   527,   616,
     793,   918,   617,   711,   712,   844,  1005,   618,   843,   919,
     920,   921,   922,  1088,   923,   924,   925,   926,   927,   928,
    1372,  1373,  1374,  1082,  1375,  1376,  1377,  1378,  1379,  1380,
    1841,  1842,   929,   930,   993,   931,   932,  1073,  1574,   933,
    1251,  1074,  1075,  2102,   934,   935,   172,   419,   173,   418,
     239,   525,   609,   610,   611,   787,   837,   838,   965,   966,
     874,  1226,  1227,  1538,  1539,  1032,   839,  1221,  1320,  1033,
     875,   970,  1228,  1343,  1344,  1826,  1345,  1346,  1347,  1348,
    1820,  1349,  1350,  1351,  1352,  1560,  1561,  1549,  1353,  1354,
    1546,   705,   301,   612,   707,   790,   791,   792,   877,   971,
    1076,  1572,  1567,  1077,   232,   784,   868,   515,   599,   598,
     694,   695,   696,  2603,   780,   829,   601,   831,   958,   959,
    1023,  1024,  1025,  1026,  1027,  1028,  1261,  2552,  2023,  2219,
    2553,  1029,  1030,  1160,  1161,  1262,  1389,  1422,  1686,  1687,
    1591,  1592,  1593,  1587,  1162,  1263,  1394,  1601,  1599,  1163,
    1264,  1402,  1902,  1164,  1265,  1406,  2130,  1165,  1266,  1166,
    1268,  2138,  2294,   412,  1920,  2133,  2137,  2140,  2141,  2300,
    2432,  2433,  2321,  2296,  2297,  2298,  2431,  2429,  2524,  2523,
    2530,  1167,  1267,  1408,  1168,  1269,  1410,  1652,  2146,  1924,
    2144,  2305,  1169,  1270,  1413,  1927,  1928,  1170,  1271,  1414,
    1655,  1658,  1171,  1172,  1173,  1273,  1418,  1932,  1174,  1274,
    1423,  1424,  1425,  1673,  1175,  1275,  1429,  1705,  1176,  1276,
    1960,  1961,  2169,  2322,  2323,  2171,  1177,  1277,  1434,  1435,
    1711,  1712,  1713,  1975,  2177,  1714,  1967,  1968,  1969,  2174,
    1972,  1178,  1278,  1441,  1716,  1976,  1179,  1279,  2183,  1717,
    1444,  1445,  1979,  1980,  2329,  1180,  1280,  1181,  1281,  1182,
    1451,  1986,  1183,  1283,  1454,  1184,  1284,  1457,  1988,  2190,
    2335,  1185,  1285,  1727,  1997,  2193,  2337,  2338,  2339,  2341,
    1186,  1286,  1187,  1287,  1462,  1732,  1733,  1734,  2010,  2204,
    2205,  1735,  2007,  2008,  2009,  2198,  1736,  2346,  2540,  1188,
    1288,  1189,  1289,  1469,  1190,  1290,  1471,  1741,  1191,  1292,
    1473,  1747,  2020,  2467,  2357,  1192,  1293,  1477,  1751,  1752,
    1478,  1479,  1480,  2025,  2027,  2028,  1193,   605,   520,   521,
    1194,  1294,  2038,  1782,  2224,  2551,  2593,  2609,  1195,  1196,
    1197,  1296,  1198,  1298,  2371,  1199,  1299,  2039,  2374,  1200,
    1201,  1300,  1493,  2476,  1770,  2041,  2377,  2237,  2238,  2478,
    1768,  1202,  1301,  1497,  1498,  1499,  1500,  1780,  1501,  1502,
    1503,  1504,  1203,  1302,  1466,  2350,  1737,  2465,  2012,  2207,
    2463,  2545,  1204,  1303,  2054,  2243,  2480,  2483,  1205,  1304,
    1513,  1786,  1206,  1305,  1515,  1516,  2247,  2486,  1207,  1306,
    1519,  1794,  1208,  1308,  1209,  1309,  1210,  1310,  1211,  1311,
    1799,  1212,  1312,  1801,  2254,  2255,  2071,  2257,  2396,  2495,
    2398,  2571,  1213,  1214,  1314,  2502,  1527,  1804,  2075,  2299,
    2430,  2262,  1215,  2077,  1216,  1217,  1316,  1763,  2264,  2460,
    2402,  2507,  1864,  1676,  1809,  1677,  1939,  1941,  2157,  2155,
    2124,  2125,  2281,  2283,  2422,  2383,  2384,  2484,  2488,  2566,
    2228,  2229,  2368,  2230,  2369,  2403,  2404,  2504,  2405,  2505,
    1935,  1936,  1937,  2151,  1938,  2152,  1218,  1317,  1532,  2516,
    2410,  2083,  2413,  2268,  2163,  1458,  1459,  1404,  1405,  1640,
    1641,  1642,  1643,  1644,  2563,  2564,   344,   345,   530,   642,
     531,  1395,  1415,  1659,  1488,  1494,   182,   183,  1505,   413,
    1723,  1481,  1482,  1483,   822,   676,   766,   677,   283,   287,
    1447,  1448,  1396,  1417,   348,  1892,  2120,  2206,  2388,  1411,
     960,  1833,  1356,  1834,   786,  1398,   179,  1485,   180,   249,
     250,  1358,  1836,  1399,  1007,   351,  1400,   431,   434,   535,
     550,   545,   541,   528,  2256,   761,  1604,  1761,  2481,    77,
    1661,   289,   246,   721,   762,   197,  2408,  2001,   728,   119,
     939,  1427,   394,   697,   410,   224,   186,  2313,  2456,    68,
     204,  1017,   854,   264,   189,   406,   485,  2135,  2439,   850,
     473,  1231,   436,    95,  2393,  2211,   685,   113,   603,   475,
    1455,   986,  1581,   214,   937,   948,   692,   100,  1906,  1726,
    1249,  1722,   984,   127
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -2131
static const yytype_int16 yypact[] =
{
   -2131,   828,   -64, -2131,   354,   354,   -64, -2131, -2131, -2131,
   -2131, -2131, -2131,  1334,   923,   923,   128,   465,   716,   397,
     553,   613,   729,   714,   722,  1026,  1125,   787,   797,   901,
   -2131,   315, -2131,  1078, -2131, -2131, -2131, -2131, -2131,   751,
   -2131,   606, -2131, -2131,    73, -2131, -2131,    31, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,  1228,  1228, -2131, -2131,  1008,
   -2131,  1110,   965,   983, -2131,  1020, -2131, -2131,  1067,   379,
    1328, -2131, -2131, -2131,  1508,  1255, -2131,  1090,   -16,  1152,
   -2131,  1044, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1317,
     354, -2131, -2131,  1111, -2131,  1118, -2131, -2131, -2131, -2131,
    1528,  1198, -2131,  1369, -2131,   741,  1154, -2131,   864, -2131,
   -2131, -2131, -2131,    82,    55, -2131,    85, -2131, -2131,  1438,
   -2131, -2131, -2131, -2131, -2131,  1160,  1030,  1538,  1090,  1090,
    1248,  1273,  1369,  1369,  1369,  1090,  1295,  1369,  1453,  1621,
     949, -2131,  1091, -2131, -2131,   414, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1363,  1563, -2131,
    1314,  1319, -2131,  1320,  1323,  1341, -2131, -2131,   618, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,   461,   631, -2131, -2131,
     -31, -2131,  1118, -2131, -2131, -2131,  1118,  1369, -2131,  1446,
   -2131,   870, -2131, -2131, -2131,  1318, -2131,  1369,  1475, -2131,
     -96, -2131, -2131, -2131,  1118, -2131, -2131, -2131,  1219,  1440,
   -2131,  1369,  1369, -2131,  1369,  1111,  1634,  1111,  1369,  1480,
   -2131,  1111, -2131, -2131,  1090,  1369,   -27, -2131, -2131,  1329,
    1329,  1584,   -64,   354,  1230,  1231,  1233,   354,  1344,  1348,
   -2131,  1118,   354,   352, -2131, -2131,  1111, -2131,  2402,  1236,
   -2131, -2131,  1118, -2131,  1407,  1446, -2131, -2131, -2131, -2131,
    1111, -2131, -2131, -2131, -2131,  1118,  1072, -2131, -2131,  1369,
     705,  1111,  1472, -2131,  1597, -2131, -2131,  1111, -2131, -2131,
   -2131,  1090,    81, -2131,  1090, -2131, -2131, -2131,  1665, -2131,
   -2131,  1464, -2131,  1070, -2131, -2131, -2131, -2131, -2131,  1245,
    1354, -2131, -2131, -2131, -2131, -2131,   527, -2131, -2131, -2131,
    1409, -2131,  1597, -2131,  6910,  1253,  1254, -2131,  1256,  1257,
   -2131,  1445,   631, -2131,  1258,  1259, -2131,  1261,  1265, -2131,
    1268, -2131,  1270,  1271, -2131,  1272,  1275,  1277,  1279,  1254,
   -2131,  6910,  6910,  6910,     7, -2131,   104, -2131, -2131, -2131,
    1281, -2131, -2131,  6910, -2131, -2131,  1446, -2131,    43, -2131,
    1471,  1404,  1008, -2131, -2131,   116,  1471,  1369,  1665,   183,
     492,   -43,  1291, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131,  1357, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131,  1369,  1380,  1389,   705, -2131,  1342, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,  1446, -2131, -2131, -2131,
   -2131, -2131,    34, -2131, -2131, -2131, -2131,  1449, -2131, -2131,
   -2131, -2131,  1285, -2131, -2131, -2131, -2131, -2131,   230,  6910,
    6910, -2131,  6910,  6910, -2131, -2131,  7973,  1301,  6910,  6910,
    6910,  6910,  6910,  6910,  6910,  6910,  6910,  6910,  6910, -2131,
     109,   109,   391, -2131, -2131, -2131,  2402,  6910,  6910,  6910,
    6910,  6910,  6910,  6910,  6910,  6751,   705,   104, -2131, -2131,
   -2131, -2131, -2131,  1369, -2131,  1539,    72, -2131, -2131,  1369,
     727,  1369,  1369, -2131,  1391,  1369,  1369, -2131,  1508,   410,
    1369,  1304, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,   641, -2131,    32, -2131,   705,
    1446, -2131,   978, -2131, -2131,  1416,   923,   923, -2131, -2131,
    1449, -2131, -2131, -2131,   354,  1444, -2131,   -71,    76,  1234,
      12,   230,   146,    33,    99,  1292, -2131,  1281, -2131, -2131,
     124,  1296,   597,   112,   195,  1297,   780,   148,   201,   378,
    1298,   852,   906,  1010, -2131, -2131,   230,   230,   230,   230,
     267,   267,   109,   109,   109, -2131,  1029, -2131,   456,  1369,
   -2131, -2131,   750,   118,  1402,  1750, -2131, -2131,   440,  1388,
   -2131,  1324,  1111, -2131,  1491,  1491,  1498, -2131,  1111, -2131,
   -2131, -2131,   641, -2131, -2131, -2131, -2131,  1392,    42,    42,
    1111,  1315, -2131,  1325,  1326,  1544, -2131, -2131,  1331, -2131,
    1444, -2131, -2131,    76, -2131, -2131, -2131,   -40,  1327,  1528,
    1369, -2131,  1210,  1369,   -13,  1471,  1369,  1781,  1509, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
    1276,  1254,  6910,  6885,  1254, -2131,  1111,  1254,  1254,  1254,
    6910, -2131,  1254,  1254,  1254,   418,  1254,  1254,  1254,  1254,
   -2131, -2131, -2131, -2131, -2131,  1324,   -22,  1799,   232,   299,
   -2131,  1336, -2131,  1336, -2131, -2131, -2131, -2131, -2131,   515,
     -28, -2131,   804, -2131, -2131, -2131, -2131, -2131,  1111,  1064,
   -2131, -2131,  1369, -2131,   304, -2131,  -197,   205,   382,  1597,
   -2131, -2131, -2131,  1554, -2131, -2131, -2131,    76, -2131, -2131,
    1717, -2131,  1718, -2131,  1332,  1446,  1346,  1210,   598,   598,
     535, -2131,   440,  1446,  1349,  1369,  1331, -2131,  1331,   377,
   -2131, -2131, -2131,   230, -2131,  1055, -2131, -2131, -2131, -2131,
   -2131,   230, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131,   -28, -2131, -2131, -2131,  1597,   529, -2131, -2131, -2131,
    1681, -2131, -2131,  1369,  1111, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  1803, -2131,  1090, -2131,  1369,  1426,
    1520, -2131, -2131, -2131,  1708, -2131, -2131,    90,   -40, -2131,
   -2131,    76, -2131, -2131,  1027, -2131,  1394, -2131,  1111, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,  1573,    18,  1608,  1356,
   -2131, -2131, -2131, -2131,  1369,  1369, -2131, -2131,  1530, -2131,
   -2131,  1111,  1111, -2131,  1064, -2131,   781,  1369, -2131,  1359,
    1345, -2131,  1369,  1369,  1781,   995, -2131, -2131, -2131, -2131,
    1626, -2131, -2131,  7450,  1807,  1628,  1446,   291,  1111, -2131,
   -2131,  1446, -2131, -2131,  1441, -2131,   481,   481,  1038,  1111,
   -2131, -2131, -2131, -2131, -2131,  1446, -2131, -2131, -2131,  7973,
    1645,  1645, -2131, -2131,   757,    76, -2131, -2131,  1594, -2131,
   -2131,   816,   930,   946,   985,  1000, -2131,  1370, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1473,
    1111, -2131,  1446, -2131, -2131, -2131, -2131,  1111,  1111, -2131,
   -2131, -2131,   -72, -2131, -2131, -2131,  1369,   757,  1327, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,   -34,   498, -2131,    71,
   -2131, -2131, -2131, -2131, -2131,   235,  1621, -2131,   957, -2131,
   -2131,  1597, -2131,  1278, -2131, -2131, -2131, -2131, -2131,  2944,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1446,    -9,
   -2131,  4237, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  1360, -2131, -2131, -2131,  1410, -2131,
      -3, -2131, -2131, -2131,  1924,    65,  1332,  1816, -2131, -2131,
    1329,  1329,   705,  1445,  1445, -2131, -2131,  1281, -2131, -2131,
   -2131, -2131,  1814,  1651, -2131, -2131,  1394,  1446, -2131,  3432,
    1362,   981, -2131, -2131, -2131,  1003, -2131, -2131,  1468,  4032,
    4580, -2131,  1111, -2131,   857, -2131, -2131, -2131,  1369, -2131,
   -2131, -2131, -2131,  1559,  1369,  1452,  1369,  7886, -2131, -2131,
    1559, -2131, -2131,  1815,  1446, -2131,  1665, -2131, -2131, -2131,
    1369, -2131,  1111, -2131, -2131,  1111,   757, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,    65, -2131, -2131, -2131, -2131,  1430,
   -2131,  1446,  1428,  1111, -2131, -2131,    65,  1605, -2131, -2131,
   -2131, -2131,  1111,  1111, -2131,    36,  1573,  1621, -2131,  1746,
   -2131, -2131, -2131,  1609, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,  1432, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,  1008, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  1479, -2131, -2131, -2131,  1427, -2131,
    1429, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131,  1542, -2131, -2131, -2131, -2131,  1035, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131,  1057, -2131, -2131, -2131, -2131,  1446, -2131,  7359,   328,
   -2131,  1369,  1111, -2131, -2131,   328, -2131,  1369, -2131,  1428,
    1369,   328, -2131, -2131,    65, -2131,    79, -2131, -2131,  1062,
    1597,  1370, -2131, -2131, -2131,   440,   440,  1814,  1749,  1386,
   -2131,  1390,  1111,  5661,  1419, -2131,  1464, -2131,  1464,  7973,
   -2131,  6465, -2131,  1118,  4881,  6465,  1624,   868,  1015,    89,
      49,  1111, -2131,  5133,  2498,    49, -2131,  4451,  1111,  5726,
    6465, -2131, -2131,  7634,  1118, -2131,  1111, -2131,  1118,  1111,
      62,    63,  1111,  1118,   138,  5850,  5942, -2131, -2131, -2131,
    1111,  1118,  7973, -2131,   206,  1762,  1111,   818,  1369,  1434,
   -2131,  1562,  1287, -2131,  1307, -2131,  1670,  1316,  1781,  1682,
    1425,  1369,  1445,  1369,  1111, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1481, -2131, -2131,
   -2131, -2131,  1267, -2131, -2131, -2131, -2131, -2131, -2131,   768,
   -2131, -2131,   771, -2131,  1554, -2131,    65, -2131, -2131,  1110,
   -2131,  1841, -2131,  1062, -2131, -2131, -2131, -2131,  1107,  1539,
   -2131,  1521,  1573,  1573,   440,  1110, -2131, -2131, -2131,  1768,
    7651,  1445,  1111,  1445,  1769, -2131,  6046, -2131, -2131, -2131,
   -2131,  1687, -2131,  1847, -2131,  3712,   524,  7973,  7973,  7973,
   -2131, -2131, -2131,  1118,  1773,  3125, -2131,  1548,  1118,  1404,
    7973,   413, -2131,  1772,  4785, -2131,  1121,   739,  1312,  1777,
     708,  1464, -2131, -2131,    35, -2131, -2131, -2131,  1819, -2131,
   -2131, -2131, -2131, -2131,  5267, -2131,  1445,    93, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,  6465, -2131, -2131,  1502, -2131,
      48,  1111,   369, -2131, -2131, -2131, -2131,  1597,  6465, -2131,
    1500,  1776,  1878,   987, -2131,   890,   533, -2131,  7743,  8114,
    1086,  1506, -2131,   631,  1504, -2131,  1511,  -113,  1737,  1597,
    1404,  1737,  1111,  1779,  1470,  1597,  7973, -2131, -2131, -2131,
   -2131,  1464, -2131,    49, -2131,   -75, -2131,    84, -2131,  1269,
    5363, -2131,  5363, -2131,  1841,  5380, -2131, -2131,  1111,  1778,
    6175,  1111,  1741,  1322,  1823, -2131,  1587,  1543,  1110,  1737,
    1111,  7973,  1764,  1446, -2131,  1735, -2131, -2131,  1369,  1369,
   -2131, -2131, -2131, -2131,  1369, -2131, -2131, -2131, -2131,   757,
   -2131,  1369, -2131,  1111,  1111,   630,  1751, -2131,  1636,  1446,
    1267, -2131, -2131, -2131, -2131, -2131, -2131,   446, -2131, -2131,
   -2131, -2131,   446, -2131,  1370,  1111,  1477, -2131, -2131,  1369,
   -2131,  1008, -2131, -2131,  1573,  1111, -2131, -2131,  1263,  1943,
   -2131,  1764,  7663, -2131,  4789,    -5,  1522,  7973, -2131, -2131,
    6465,  1748, -2131, -2131,  1592,  1687, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  1008, -2131, -2131, -2131,  1531, -2131,
   -2131,  1531, -2131, -2131,  1433, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
    3712, -2131, -2131, -2131, -2131, -2131, -2131,   631,  1526,  1033,
   -2131,  1033,    80,   840, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131,  1812,   -26, -2131, -2131,  1281, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1676, -2131,  1764,
    1764,  1764,  1764,  1559,  1559,  1559, -2131, -2131,  1369, -2131,
   -2131, -2131, -2131, -2131,  1495,  1369, -2131, -2131,  1925, -2131,
   -2131, -2131,    78, -2131, -2131, -2131,  6465,  6465,  1497,   868,
     234,  1813,  1505, -2131,  1507, -2131,  5133,  5552, -2131, -2131,
    1111, -2131,    49,   170, -2131, -2131, -2131,  1343,  1782,  1955,
      53, -2131,   369, -2131, -2131, -2131, -2131,   142,  1549,    49,
   -2131, -2131,  6465, -2131, -2131, -2131, -2131,  1586,  1510, -2131,
   -2131, -2131, -2131,  1796,  1111,   524,  1726, -2131,  1103, -2131,
   -2131,  1404,  7886, -2131,  1754,   497,   172, -2131, -2131,  1111,
     172,  1553, -2131, -2131,    94,   978, -2131, -2131,  5015, -2131,
    1931,  1727, -2131,   827,  6465,  6465, -2131, -2131, -2131,  6262,
    1111, -2131,  1790, -2131, -2131,  6465,  7973, -2131, -2131, -2131,
    1841,  1761,  1111, -2131,  1172,    50,   497,  1792,  1291,  1826,
    1676,  1800, -2131,  1446,  1446,  1446, -2131,  1446, -2131,  1548,
   -2131, -2131,  1192, -2131,  1223,  1252, -2131,  6465, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1521, -2131,
   -2131,  1477, -2131,  1111,  1564, -2131, -2131, -2131,  1764, -2131,
   -2131, -2131, -2131,  1515,  1517, -2131,  7973,  1764,  1759, -2131,
   -2131, -2131, -2131,   328, -2131,  1676, -2131, -2131,  1369, -2131,
   -2131, -2131, -2131,  1499,  1369, -2131, -2131, -2131, -2131,  1962,
   -2131,  1934, -2131, -2131,   121, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131,   580, -2131,  1111, -2131,  1281, -2131,  6409,   -50,
    6465,    49, -2131,  1637, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,  1529,  1674,    44,  1763,  1596,
   -2131,  1551, -2131,  6767, -2131, -2131, -2131, -2131,   709,  1196,
    1623, -2131, -2131, -2131, -2131,  1812, -2131,  1719, -2131,  1111,
   -2131, -2131, -2131, -2131, -2131, -2131,  7886,  7886,  7886,   328,
   -2131, -2131,   328, -2131,  1841,  1841,  1817,  6409,   552, -2131,
    1657, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  2007, -2131,
    1598, -2131, -2131, -2131,   234,  8312, -2131,  6465,   508,   670,
   -2131, -2131,  1738, -2131, -2131,  1110, -2131, -2131,  1413,  1008,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1673,  1008, -2131,
   -2131,  7973, -2131,  1974, -2131, -2131, -2131,  6669, -2131,  7973,
    5959, -2131,    82,   604,  1107,    49,    93,  6409,   603,  1291,
    1118, -2131,  1874, -2131, -2131, -2131, -2131,  2022, -2131,  1832,
   -2131, -2131, -2131, -2131, -2131,   100, -2131,  1111,   653,   -26,
    1240,  1565,  1891, -2131,  1567,  7973,   969, -2131,   580, -2131,
   -2131, -2131,  6465, -2131,  1795,  1783,  1784, -2131,  1785, -2131,
   -2131, -2131, -2131, -2131,   344,  1111,  6409,   647,  1610,  2031,
    1111,   898, -2131, -2131, -2131,  1721,  1728, -2131, -2131,  1103,
    7973,  1905, -2131, -2131, -2131,  1006,  1854, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1999,  1008, -2131,
    1111,  1369, -2131, -2131, -2131, -2131,  1764, -2131, -2131, -2131,
   -2131,   328, -2131, -2131,   328, -2131,  1369,   168,   168, -2131,
   -2131, -2131,  1548, -2131, -2131,  1767,  6409, -2131,    49,  1625,
     524, -2131, -2131,  1763, -2131, -2131, -2131,    44,   612, -2131,
     917, -2131,  7973,  7973, -2131, -2131,  1851,  1707, -2131,  1692,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1983,  1983,
    1983, -2131, -2131, -2131,   328, -2131,  6465, -2131,  6465,   113,
    1111, -2131,    58, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131,  1910, -2131, -2131,  7973,  1909, -2131,  1418,
   -2131,  1599,  1294,  1946,  1600,  1638,  7973, -2131, -2131,  2013,
   -2131, -2131, -2131, -2131,  5959, -2131,  1873, -2131,  1369,  1766,
   -2131, -2131,  1539,    93, -2131,  6465,   410,   813, -2131, -2131,
    1111,  6465,  1818,   845,  1754, -2131, -2131, -2131,  1914,  1791,
   -2131,  1915, -2131,  1820, -2131, -2131, -2131, -2131,  1567, -2131,
   -2131, -2131,  1369,   -26, -2131, -2131, -2131,  1770,  1757,  1548,
   -2131,  6465,  4451, -2131,   -35, -2131,  7973, -2131,  1956,  1660,
   -2131, -2131,  1110, -2131,  1250,  2075,  1980,  7973,   491,  1369,
    1369,  6465,  1564,    79, -2131, -2131, -2131,  1833, -2131, -2131,
    1623, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1763,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,    69,  1387, -2131,
     113,  1387, -2131, -2131,  1111, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,  1361,  6409,  1739,  6409,
    1744,  -153,   701, -2131,   130,  1597, -2131, -2131, -2131, -2131,
   -2131, -2131,  1938,  1942, -2131, -2131, -2131,  1294, -2131,  1978,
   -2131, -2131, -2131,  4451, -2131,  4451,  1103, -2131, -2131,  1103,
     -57,  1111, -2131, -2131,  6409, -2131, -2131,   497,  4580, -2131,
    2042, -2131,  1840,  1842, -2131,  1845, -2131,  1269, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
    1821,  1939, -2131,  1944,  1822,  1111,  1623,  6409, -2131, -2131,
   -2131, -2131, -2131,  2031, -2131,  1608,  2002,  1608,  1770,   809,
   -2131, -2131,  1945, -2131,  1824, -2131, -2131, -2131,   935,  1608,
   -2131, -2131,  1836,   636,  1446,  1446, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  1445,  1111, -2131, -2131, -2131,  1971,
   -2131, -2131,  3613, -2131,   863,  1970, -2131, -2131, -2131, -2131,
   -2131,  6465, -2131,  6465, -2131,    37, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131,  1938, -2131, -2131,  2063, -2131,  1103, -2131,
    1684,  1793,  1118,   -17, -2131,  1111, -2131, -2131,  1244,  6465,
   -2131, -2131, -2131,  1883, -2131, -2131,  8341, -2131,  8341, -2131,
    6465,   683, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131,  1111,  1608, -2131,  1111,  1958, -2131, -2131,
   -2131, -2131, -2131,  1118, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131,  1361,  1111, -2131, -2131,  1797,  1369, -2131, -2131, -2131,
    1111, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  6465, -2131,
   -2131,  1623,  1623,  -197,   308, -2131,  6465,  1103,  1103, -2131,
    6465,  1369,  1118,  1118,  1798, -2131,  1597, -2131,  1758,  1672,
    1369,  1166, -2131, -2131, -2131, -2131, -2131,  1531, -2131,  1531,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,  1111, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,   524, -2131, -2131,
    -197, -2131, -2131, -2131,  2011, -2131, -2131,   524,  1118,  1369,
   -2131, -2131,  1111,  1975, -2131, -2131,  1828,  1830, -2131, -2131,
   -2131,  2011, -2131, -2131, -2131,   524, -2131, -2131, -2131, -2131,
    1960,  1961, -2131, -2131,  1008,  1008, -2131, -2131
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -2131, -2131, -2131, -2131,   152, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,    46,  1220,  1627,  2081, -2131,
   -2131,  1834, -2131, -2131, -2131,  2108, -2131, -2131,  2109, -2131,
   -2131,  2015,  2065, -2131, -2131, -2131,  2039,   735, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,  2030, -2131, -2131, -2131, -2131,
    2005, -2131, -2131, -2131, -2131, -2131,   375, -2131, -2131, -2131,
   -2131,  1647, -2131, -2131,  1561,  1330, -2131,  1875, -2131, -2131,
   -2131, -2131, -2131,  1765, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131,  2116, -2131,  2117, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131,  1486,  1209,  1591, -2131,  1685, -2131, -2131, -2131,  1503,
   -2131, -2131, -2131,   -49, -2131,  2125, -2131, -2131,  2123, -2131,
   -2131, -2131, -2131,  1919, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131,  2004, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,  1157, -2131, -2131, -2131,
   -2131, -2131,  1321, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,  1213, -2131, -2131,  1566,
   -2131, -2131,   271,   103, -2131, -2131, -2131,   115, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,  -854,  -681,  1180,  -628,   952,
   -2131, -2131,   808, -2131, -2131, -2131,   805, -2131,   173, -2131,
   -2131,   341,  -481, -2131, -2131,  -389, -2131, -2131, -2131,   960,
   -2131,  -632,  -829,   -83, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  1580, -2131, -2131, -2131,  1333, -2131,
   -2131, -2131,   871,   106, -2131,  -422, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,   846,
   -2131, -2131, -2131, -2131, -2131, -2131,   634, -2131, -2131, -2131,
   -2131,   950, -2131, -2131, -2131, -2131, -2131,  1408, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
    1601,   867, -2131,  -406, -1666, -2131, -2131, -2131, -2131,  1243,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -1658, -2131, -2131,
    -276,   -12, -2131,  1173, -2131, -2131, -2131, -1259, -2131, -2131,
   -2131, -2131,   614, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131,  -283,   557,    74, -2131, -2131,    75, -2131,
   -2131,  -223, -1226, -2131,   -58,    77, -2131,   -87, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,   280, -2131, -2131,
   -2131, -2131,   792, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  -104, -2131, -2131, -2131, -2131,   510,
   -2131, -2131,   509, -2131, -2131, -2131,   248, -2131,    51, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131,   782, -2131,   245, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -1648, -2131, -2131, -2131, -2131, -2131, -2131,  -112,   500, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,   496, -2131, -2131, -2131,
      26, -2131, -2131,   224, -2131, -2131, -2131, -1304, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,   210,
   -1157,   755, -2131, -2131, -2131,    14,  -383, -2131, -2131, -2131,
   -2131, -2131,    13,  -131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131,   432, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,   469, -2131,     2, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,   740,
   -2131,   742, -2131, -2131,   940, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131,   732, -2131, -2131, -2131,   731,   176, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131,  -144, -2131,   179, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131,   663, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131, -2131, -2131, -2131, -1310, -2131, -1496,
   -2131, -2131, -1149,   -69, -2131, -1453, -2131,  -649, -2131, -2131,
   -1842, -2131, -2131, -2131, -2131,  -143, -2131, -2131, -2131, -2131,
    -297, -2131, -2131,    27, -2131, -2131, -2131, -2131,  -146, -2131,
   -1291, -2130, -2131, -2131,   322, -2131, -2131, -2131, -2131, -2131,
   -2131, -2131, -2131, -2131,  -799, -1536, -1244, -1598, -2131, -2131,
     621, -1601, -1600, -1597, -2131, -2131, -2131,  1806,  1156,   854,
    2623,  1843, -1564, -1194, -1070,   772,  -202,  -179, -2131, -1295,
   -2131, -1342, -2131,   507,  -783,  -203, -2131,  -691,    -6, -2131,
   -1255, -1399,   959,   824, -1030, -2131,   218, -1385,  -662, -2131,
   -1096,   695, -1216, -2131,   905,   -92,  -428, -1589,   -19, -2131,
    2021,   -61, -2131,  -926,   569,  -227,  1443,  1002, -2131, -2131,
   -2131, -2131, -2131,  1747, -2131,  1527,   666, -2131, -2131, -2131,
   -1735,  -201,  -317,  1560,   543,  -262, -2131, -2131,  -652,  1808,
   -2131,  -984,   356, -1334,  -336,  -674,   925,  -475, -2131, -2131,
   -1130, -2131, -1279, -2131,   -93,  -822,  -361, -1183,  -217,  -896,
    -226,  -941,  -993, -1327, -2131, -2131,  1710,  1987,  1794, -1205,
     582, -2131,   471, -2131,  2174, -2131, -2131, -2131, -1599, -2131,
    1074,   -59, -1135,  -351
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1798
static const yytype_int16 yytable[] =
{
      93,   178,   265,   251,   393,   437,  1506,   254,   536,   486,
    1092,  1093,   273,  1006,   276,   848,  1818,  1236,   279,  1361,
    1403,   489,  1907,  1911,  1912,  1365,  1315,  1913,  1095,   290,
    1460,   806,   481,  1436,   519,  1664,  1898,  1576,   859,   215,
     216,   217,  1575,   398,   221,  1079,  1507,   226,  1721,  1012,
     591,    11,   968,  1709,    16,  1446,  1446,   359,  1585,  2533,
    2076,  1721,   304,  2002,  1648,   198,   799,   801,   399,  1079,
    1446,   122,  1492,   254,   401,  1002,  2424,  1416,   285,  1810,
    1963,  1002,   185,   243,  2090,   223,   251,   453,  2150,  1002,
    1255,  1446,   453,  2022,   255,   995,    16,   693,   513, -1797,
    1446,  1446,   247,  2131,   260,   403,  -199,   759,  1721,  1237,
     469,  1771,  1970,   453,   569,  2167,  1381,  1068,   270,   271,
    2003,   272,   210,   211,   477,   277,  1671,  1003,   257,   218,
     247,  1590,   284,   852,  2406,   262,   175,   606,  1865,     4,
     479,  1510,   457,  1957,   458,   459,  2444,   457,  1079,   458,
     459,   671,   709,   997,  1442,   195,   349,  -826,    12,  2021,
    1079,   832,   457,  1890,   458,   459, -1712,  1759,   693,   398,
    2461, -1754,   991,  1412,   998,  2214,   395,  1922,  2017,   453,
    1789,  1765,   871,   709,   457, -1669,   458,   459,   833,   834,
    2543,   317,   453,   411,   358, -1336,   274,   317,   274,   778,
    1257,  1805,   274,  1094,   453,   317,  1506,  1933,   570, -1712,
    1776,  1760,  1954,   256,  1663,  1525,  1524,  2026,  1777,  1806,
     286,   188,   349,   693,  2250,   363,  2447,   312,   453,  1491,
      57,  2066,  1496,   457,  -199,   458,   459, -1712,   487,   567,
   -1797,   274,  1526,   778,  1244,  1245,  1529,  2004,  1774,   349,
     349,   349,   274,   779,  2448,  2117,  1963,  1245,   274,   571,
    1934,   349,  2034,  1484,  1923, -1754, -1746,     5,   457,  2392,
     458,   459,  1014,  1384,   480,   453,   405,   759,   407,   294,
     320,   453,   593,   298,  2284,  1764,   320,   114,   305, -1746,
    1069,  2544,  1985,  1862,   320,   468,  2005,   779,   992,  2425,
     491,   672,    22,   476,   482,   457,  1646,   458,   459,    21,
     488,  1649,  1650,  1651, -1708,   404,  1004,  2176,  1079,  2222,
    1367,   323,  1660,  1984,   188,  1891,  2021,   323,   326,   781,
    2189,   411,   614,  1590,   326,   323,  2126,   349,   349,  1554,
     349,   349,   326,  1070,    22,   511,   349,   349,   349,   349,
     349,   349,   349,   349,   349,   349,   349,  1775,   942,   693,
    -200,  1464,  1443,  1086,   349,   349,   349,   349,   349,   349,
     349,   349,   349,   349,  1336,  1721,  1970,   263,   681,   683,
     568,  1987,   673,  1544,   292,   688,   573,  2280,   578,   579,
   -1712,   454,   581,   582,  1511,  1865,   454,   588,  1595,   725,
    1597,   329, -1669,   835,  1865,    25,  1416,   329,    17,  2462,
    2014,  2068,   999,  2030,  1930,   329,   457,   454,   458,   459,
     584,  2595,   483,  1258, -1712,  1083,  1646,   752, -1712,   457,
   -1797,   458,   459,   710,   306,  1808,   640,   693,  2420,  1838,
    1079,   247,    94,   737,   760,   416,   121,    25,  1484,   594,
     851,  -199, -1763,  1720,  2132, -1734,   123,   104,   453,   248,
     592, -1712,   681,  2078,  1035,  1436,  2069,   331,   177,   196,
    1729,  2106,    58,   331,  1256,  2440,   665,  2442,  2449,   989,
      92,   331,    59,   454,  2016,   765,  1582,  1583,  -200,   849,
    1071,  1965,  2006,  2308,  2309,  1966,   454,  2310,   455,  2311,
     514,   334,  2127,   641,   478,  1710, -1712,   334,   454, -1712,
     177,  1512,  2466,  1416,  2386,   334,  1778,   470,  1668,    87,
     123,   177,   177,  1647,   644,    92,  2046,   716,   199,  2332,
     722,   724,   454,   726,  1955,   177,   177,  1245,   177,   307,
    1779,  1964,   177, -1734,  2490,  2491,   340,  1337,  1416,   615,
     349,   349,   340,   782,   121,   753,  2035,   177,   349,   274,
     340,   823,   274,   274,  1382,  1383,   177,   177,   812,   274,
     607, -1710,  2223,   201,  1970,   682,  1031,  2118,   836,   454,
   -1712,   699,  1072,  2265,   256,   454,   460,   461,   462,   463,
   -1661,   464,   661,   813,  1013,   823,   464,  2450,   465,   776,
    1338,  1416,  2317,   649,  2319,    60,   460,   461,   462,   463,
      23,   464,  1366,  2179,   760, -1667,  2195,  1721,   823,   860,
    2164,  2164,   727,  2421,  2199,   800,   802,   274,   460,   461,
     462,   463,   809,   464,   943,   457,  1031,   458,   459,   653,
     643, -1712,   962,  1647,   101,   860,   274,  2109, -1734, -1712,
     755,  2354, -1712,  1865,   796,  2445,   860,   229,  2186,   682,
    2239,   745,   807, -1797,  1253,  1254, -1619,  1965,  -656,   274,
     821,  1966,   783, -1746,    24,  2453,  1550,   460,   461,   462,
     463,   411,   464,  2182,  2451,   826, -1665,  2387,  1845,  2581,
    2582,   585,   654,  1584,   840,   188,   675,   308,  2291,  2103,
    2378,   845,  1831,   274,   778,  -200,  1416,   123,  2107,   102,
    2474,  2475,   460,   461,   462,   463,    27,   464,    28,  2477,
    1240,   856,   857,  -868,  1730,   484, -1739,  1816,  1822,  2285,
   -1712,   483,  2036,  2161,   865,   244,  2162,   949,  1669,   869,
     870,  1000,  1946,  1947,  1948,   274,  -654,  1340,  2231,   586,
     936,   462,   463,   244,   464,  1823,   693, -1797,   779,  2168,
    2213,   103,   454,  1706,  2519,   864, -1763,   493,  2528,   363,
     825, -1712,   177,  2525,    16,  1091,   411,   961, -1797,   274,
    1646,   403,  2042,  2164,  2164,   941,  2042,   309,  2287,   818,
     946,  1731,  -656,   662,  2290,  2226,    19, -1712,  2147,  1089,
    1090,   177,   274,   274,   957,  2535, -1734,   159,   987,  1355,
    2215,  2344,   245,   862,  1721,  1683,   123,   350,   457,  2291,
     458,   459,   789,   994,  2209, -1712,  2565,  2145,     3,   274,
     245,   317,   188,  2180,   803,  2324,  2119,   951,   951,  1341,
     274,   988,     9,   194,   663,    62,  2572,  2573,    63, -1712,
     814,   244,   104,   230,  2251,  2293,   495,   746,  1614,  1615,
     460,   461,   462,   463,  1461,   464,  2579,  2583,  2584, -1662,
    -654,  2394,   308,   460,   461,   462,   463,   666,   464,  1078,
    2560,  2561,   554,   350,  2562,    64,  1670,  1521,  2556,   990,
     457,   841,   458,   459,  2557,  2275, -1734,   496,  2276,   863,
     819,  -275,  2349,  1078,   497,   203,   789,  1034,  2598,  2210,
     350,   350,   350,   177,  2601, -1561,  2158,  2159,  2160,   177,
     320,  2292,   350,  1428,  1660,  2558,   194,  1832,   245,  2327,
    2559,   484,  1222,    65,  1223,  2399,  2227,  1001,  1707,   804,
    1220,    19,  1486,  1824,   457,  1229,   458,   459,  2316,   364,
    2288,  1232,  2381,  1235,   177,   349,  1098,  2274,  2596, -1040,
    2597,   323,   309,  2514, -1619, -1739,  2324,  1241,   326,   123,
    1242,  2515,  2318,  1243,  2320,  1660,  2293,   499,   788,    20,
     574,  2181,  1078,  1407,  2266,  1409,  1825,    21,  2324,    26,
    2376,   404,  1684,  1239,  1078,  2498,   162,  1647,   350,   350,
    -868,   350,   350,   667, -1561,   537,   101,   350,   350,   350,
     350,   350,   350,   350,   350,   350,   350,   350,  1579,  1660,
    1247,  1416,    22, -1619,  1530,   350,   350,   350,   350,   350,
     350,   350,   350,   350,   350,  1563,  2499,   256,  1568,  1594,
    2458,   329,   962,   500,  2500,  2055,  2302,  2303,   457, -1712,
     458,   459,   163,   177,  -275,  2606,    52,  1416,  -275,  1685,
    2100, -1726,   788,  2362,  1250,  2056,  1282,   457,   575,   458,
     459,   102,  1036,  1428,  2434,   668,   576,   360,  2139,   460,
     461,   462,   463,  2363,   464,   962,  1564,  2057,   648,  1569,
    2330,  1368,   361,   457,  1419,   458,   459,  -868,  2324,  2290,
   -1797,    66,   362,   128,  -275,  2364,  2423,   331,  2058,  1728,
    1224,  1225,  2032,  2510,   363,  1487,  2493,  2501,  2496,  1490,
    1536, -1797,   503,   103,  1509,    25,  2365,  1486,  1978,   101,
    2512,  1537,  1523,  1531,    26,   101,  1368,  1357,  1359,  2033,
    1360,   334,   123,  1357,  1362,   195,   969,  1364,  1708,  1357,
   -1726,  -868,  1078,  2537,   158,  2538,  2165,  1015,  1783,  1748,
     129,  2084,  2527,    67,   128,  1321,  1756,  1893, -1712,  1895,
    1390,  2411,  1635,  1743,  -868,  1638,  1639,   961,  2356,    53,
     962,   962,   962,   188,  1369,  1370,   340,  1925,  1449,  1450,
     767,   256,  2511,  1449,   102,  1463,   130,   131,   456,   132,
     102,   349,    69,   411,   763,   225,  2434,   133,   973,  1449,
      70,   350,   350, -1728,  1744,  2568,  2110, -1797,  1522,   350,
     961,   129,  1745,   414,  -275,  1533,  2292,   415,  -275,   190,
    1370,   203,  -275,   159,  1653, -1734,   274,   274,  1553,  1662,
    1555,  1594,  1556,  1313,   872,   873,   103,  2531,  2509,  2532,
    1565,  1566,   103,  1570,  1571,    54,    89,   130,   131,   778,
     132,   188,   460,   461,   462,   463,   974,   464,   133,  1467,
    1318,   652,   134,     4,  1078,    78,   595,  1489,   596,   768,
    1489,  1495,   181,  1467,  1371,    79,   181,   619,  1926,  1847,
    1848,  1562,  1983,   764,   364,  1746,  2059,  1489,  1319,   160,
    1596,  1432,  1929,   161,   962,  1433,   620,  1990,  2226,   953,
    1991,  1992,   123,   779,  2366,   961,   961,   961,  2278,  2279,
     769, -1631,   975,  1849,  1850,   365,  2390,   770,  1665,  1437,
    1915,  1438,   366,   134,   460,   461,   462,   463,   977,   464,
    1851,   449,  1852,   657,   621,  1439,  1688,  2418,   135,  1689,
    2073,  1821,   193,  1989,  2074,  1449,  1990,    16,   258,  1991,
    1992,  -691,  1690,  1691,  1692,   274,  1982,    71,  1449,  1220,
     123,   177,  1839,    23,  1853,  1854,  1855,   979,   367,   954,
     976,   955,  1846,   136,   368, -1631,   349,  2091,   460,   461,
     462,   463,   981,   464,   646,  1440,   978,   658,   650,  1933,
     771,     5,  2049,   655,  1665,  2092,  2050,    75,   177,   135,
      94,  1449,  1693,   369,  2040,  1449,  1355,  2390,  2093,  2390,
    2600,   370,   162,  2400,    72,    73,  1792,    17,    76,  1220,
    2604,  2520,  1856,   371,  1857,   980,  2094,    24,  1807,   961,
      92,  1917,  1858,  1918,   136,  1813,  1814,  2095,  2613,  2227,
     982,  1815,  1934,    96,  -688,  2040,  2148,  2081,  1817,   196,
   -1710,  1819,   372,   730,  2121,  2096,   772,   137,   188,  -688,
     373,    97,  1811,  1495,   622,  1835,  1101,  2149,   163,    27,
    1835,    28,   138,  1933,   731,   623,  1843, -1797,  1781,  1694,
      99,  1102,   460,   461,   462,   463,  1534,   464,  1829,  1562,
    2232,   659,   123,    18,  2547,  1919,   374, -1797,    98,  1695,
     139,   460,   461,   462,   463,   493,   464,  1859,   207,   164,
     660,  2233,  1844,   256,    19,  2548,  1696,   494,   137, -1797,
     114,   411,  1535,   118,  2401,   274,  1934,   460,   461,   462,
     463,  1753,   464,   138,   715,   773,   816,  1558,  1536,   723,
   -1797,   718,  2187,   719,  1754,  1904,   274,  2333,   120,  1537,
     375,   777,    20,   121,   140,   777,   274,  2188,  1993,  1994,
      21,   139,  2334,   174,  1697,   624,   625,  1541,  1542,   227,
    1679,  1680,  1681,  1682,   177,  1559,  1995,  1996,   533,   534,
     626,   181,   627,  1698,   187,  1949,   543,  1230,  1543,   547,
     548,  1699,  1952,   188,   495,    22,  1547,  1548,  1614,  1615,
    1942,  1943,  1944,  1945,  2437,  2438,   350,  1993,  1994,   954,
    2121,   955,  1700,   185,  1233,   140,  1234,   192,  1982,    23,
    1449,   522,   523,   524,   203,  1995,  1996,  1222,  1908,  1223,
     823,    55,    56,   732,  1618,   496,   736,  1449,   206,   738,
     739,   740,   497,   209,   742,   743,   744,   212,   747,   748,
     749,   750,  2029,  1797,   213,  1798,   498,  1950,  2216,  1951,
     349,  2112,  2427,  2113,  2428,  1909,   810,  2043,   811,  1701,
    1621,  2457,  1449,  2459,  2314,  2315,   219,   222,   223,  1702,
     231,   352,  1860,    24,   234,   233,   962,   238,  2064,   235,
     236,   628,   256,   237,  1665,   259,   261,   267,    25,   268,
    2072,  1910,   275,  2248,   278,   288,   291,    26,   295,   296,
    2248,   297,   629,   300,   299,   499,   353,   356,   400,   312,
     247,   409,   411,   421,   422,    27,  1861,    28, -1797,   102,
     472,   123,   435,   429,   430,   474,   432,   433,   438,   439,
    2208,   440,  2086,  2087,  2088,   441,  2089,   352,   442,  1703,
     443,   444,   445,   123,  1665,   446,   490,   447,   492,   448,
     509,  1357,   518,   526,   539,  2111,   466,   589,   483,   580,
     600,  2114,   608,   645,   352,   352,   352,   647,   651,   656,
     478,   500,   363,   678,   501,   502,   352,   679,   684,   687,
     703,   691,  2122,   700,   704,   727,   729,   477,   756,  1449,
     785,  -364,   794,   701,   702,   713,   820,   808,  2217,   797,
     795,   824,   827,   828,   274,   830,   846,   849,   853,   855,
     858,   961,   866,   867,   876,   938,   940,   947,   964,   972,
     983,  1080,  1081,   985,  1087,  1094,  1096,  2156,  1103,  1230,
    1100,  2353,  1238,   962,   349,   349,   349,  1357,  1246,  1248,
    1357,  1252,   350,  1259,  1291,  1260,  1272,  1295,  1307,  1297,
     503,  1385,   352,   352,  1387,   352,   352,  1528,  1388,   538,
    1431,   352,   352,   352,   352,   352,   352,   352,   352,   352,
     352,   352,  1401,  1545,  1551,  1552,   693,   860,  1332,   352,
     352,   352,   352,   352,   352,   352,   352,   352,   352,  1665,
    1580,  1586,  1603,  1598,  1605,   962,   962,  1665,  1665,  1654,
    1660,  1672,  1635,  1449,  1449,  1638,  1639,  1704,  1715,  1725,
    2191,  1740,  1739,  1742,  1755,  1757,  1758,  1762,  1769,  2194,
    1767,  1796,  1793,  1800,  1802,  2225,  1674,  1803,  1812,  1828,
    1840,  1827,  1525,  1665,  1894,  1900,  1901,  1905,  1916,   962,
    1931,  1940,  1953,  1959,  1971,  2000,   880,   881,   882,   883,
     884,   885,   886,  2249,  1999,  1710,  2019,  1974,  2258,  2258,
    2024,  2015,  1646,  1476,  2037,  2045,  2052,  2053,   961,  1666,
    2065,  2070,  2080,  2082,  2085,  2104,  2105,  2101,  2108,  2115,
    2116,  2128,  1647,  2129,  2136,  1934,  2468,   888,  2273,   889,
     890,   891,   892,   893,   894,   895,  1783,  2139,  2134,  1357,
    2123,  2170,  1357,  2277,  2166,  2172,  2173,  2185,  2192,  2196,
    1750,  2220,  2221,  2235,  2242,  2234,  1449,  2236,   251,  2272,
     962,  2253,  2252,  2244,  2245,  2246,  2267,   350,  2261,  2270,
     961,   961,   896,   897,  2271,  2282,  2263,  2286,  2304,  2306,
    2307,  2312,  2328,  2331,  2340,  1666,  2336,  2342,  2345,  2348,
    2343,  2352,  1357,  2370,  2227,  2373,  2385,  2395,  2361,  2382,
    2375,   274,  2397,  2407,  2409,   352,   352,  1397,  2455,  2419,
    2441,   347,  2334,   352,   961,  2443,  2452,  2469,  1426,  1430,
    2470,  2482,  2471,  2479,  1665,  2472,  2494,  2485,  2401,  2487,
    2513,  2506,  1665,  1470,  1472,  2351,  2522,  2529,  2536,  2539,
    2541,  1449,  2550,  2570,  2576,  2589,  2590,  2591,  2029,  1517,
    1397,  2602,  2610,  2608,  2611,  2614,  2615,    91,   996,    82,
     426,   208,    83,   126,   191,   898,   202,   228,  2464,  2380,
    2047,  2325,   590,   690,   861,    84,   402,   347,    85,   758,
    2389,   508,  1018,   670,  1665,   577,  1896,  2355,   751,    86,
      88,   355,   240,  1097,  1085,   961,  2414,  2415,   952,   708,
    1339,  1577,  2099,  1578,   347,   347,   347,  2212,  1342,  2417,
     706,  2269,  1540,  1557,  1830,  2612,   347,  2503,   901,   842,
     698,  1019,  2554,  1219,   967,  2426,  1866,  2358,  1921,  2526,
    2412,  2289,  2436,  2295,  2435,  2153,  1675,  2301,  2446,  1962,
    1602,  1973,  2175,  2326,  2184,  2454,  1718,  1998,  2011,  1645,
    2347,  2197,  2218,  1749,  2359,   903,  2473,  2367,  2079,  2044,
    2379,  1772,  1508,   905,  1788,  1773,  1791,  2259,  1678,  2492,
    2260,  2389,  1863,  2389,  2594,  2497,  2372,   906,  2508,  2154,
    2542,  1914,   555,  2031,  1766,  1520,  2240,  1837,  1719,  1573,
     354,  1903,   347,   347,   613,   347,   347,   798,   817,  1724,
    2013,   347,   347,   347,   347,   347,   347,   347,   347,   347,
     347,   347,  1738,  2489,  2574,   686,   583,   427,  1977,   347,
     347,   347,   347,   347,   347,   347,   347,   347,   347,  2098,
     220,   604,   963,  1363,     0,     0,  1357,   909,   910,   911,
       0,     0,     0,     0,   251,     0,     0,     0,     0,     0,
       0,   350,   274,  2521,  1787,     0,  1787,     0,     0,  1517,
       0,  2588,     0,     0,  1602,     0,     0,     0,     0,     0,
       0,     0,     0,  2517,  2518,     0,     0,     0,     0,     0,
       0,     0,     0,   251,     0,  1666,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   913,   914,   915,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1084,     0,     0,
       0,  2567,     0,     0,  2569,     0,     0,     0,     0,   251,
       0,     0,   313,     0,     0,     0,     0,     0,     0,     0,
    2575,     0,     0,  2577,  1899,  1666,  2534,     0,  2578,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   314,     0,     0,     0,     0,  2546,     0,  2587,     0,
       0,     0,     0,     0,     0,     0,     0,  2592,     0,     0,
       0,     0,     0,     0,  1645,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  2599,     0,     0,     0,
       0,     0,     0,     0,     0,   347,   347,     0,     0,     0,
     352,     0,     0,   347,   315,     0,  2605,     0,     0,  1456,
    2607,     0,     0,     0,     0, -1460,     0,     0, -1460,   316,
       0, -1460, -1460, -1460,     0,   350,   350,   350,     0, -1460,
       0,     0,     0,     0,     0,     0,     0,     0,   317,     0,
    1956,  1958,   318,     0,     0,     0, -1460, -1460, -1460, -1460,
       0,  1981,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  2616,  2617,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  2018,     0,     0,     0,
    1666,     0,     0,     0,     0,     0,     0,     0,  1666,  1666,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   -1460,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  2051,     0,     0, -1460,   319,     0,  2060,  2061,
       0,     0,     0,  2063,  1666,     0,     0,   320,     0,  2067,
       0,     0,     0,     0, -1460,     0,     0,     0, -1460,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   321,     0,     0,
       0,  2097,     0,     0,   322,     0,     0,     0,   323,     0,
     324,     0,     0,   325,     0,   326,     0,     0,     0,     0,
       0,     0,     0, -1460, -1460,     0,     0,     0,   327,     0,
       0,     0,     0,     0,     0,     0, -1460,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   328,     0,     0,     0,
       0,     0, -1460, -1460,     0,     0,     0,     0,     0, -1460,
       0,     0,   963, -1460,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1465,     0, -1460,     0,     0,     0,   352,     0,   329,     0,
   -1460,     0,     0, -1460,     0, -1460,     0,     0,     0,     0,
   -1460,     0,     0,     0, -1460,   963, -1460,     0,     0, -1460,
       0, -1460,     0,     0,     0,  1666,     0,     0,   330,     0,
       0,     0,     0,  1666, -1460,     0, -1460, -1460,     0,     0,
   -1460,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   -1460,     0, -1460,     0,     0,     0,     0, -1460,     0,     0,
       0,  2178, -1460,     0,   331,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   332,   333,     0,     0, -1460,
       0,     0,     0,     0,     0,  1666,     0,     0,     0,     0,
       0,     0,     0,     0, -1460,     0,     0,     0,   334,     0,
       0,     0,     0,     0,   335,   336,   337,     0,     0,     0,
     963,   963,   963,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1667, -1460,   338,     0,     0,     0,     0,
       0,   346,     0,   339,     0,   177,  2241,     0,     0,     0,
       0,     0,     0,   340,   341,   342,     0,     0,     0,     0,
     347,     0,   343,     0,     0,     0,     0,     0,     0,     0,
   -1460,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0, -1460, -1460,     0, -1460,     0,     0,     0,     0,     0,
       0,   352,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0, -1460,     0,     0,   428,     0,  1667,
   -1460, -1460, -1460,     0,  -651,  1020,     0,     0,  -700,     0,
    -700,     0,     0,     0,     0,  -700,     0,     0,     0,     0,
       0, -1460,     0,  -700,   450,   451,   452,     0,     0, -1460,
       0, -1460,     0,     0,   963,     0,   467,     0,     0, -1460,
   -1460, -1460, -1460, -1460,     0, -1460,     0, -1460, -1460, -1460,
   -1460, -1460,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -700,  -700,     0,     0,     0,     0,  -700,     0,     0,
       0,     0,  -700,     0,  -700,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -700,     0,     0,  -700,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1897,  -700,     0,     0,     0,  2360,     0,     0,     0,     0,
       0,     0,     0,   532,     0,     0,     0,     0,     0,     0,
       0,   540,   542,  -700,   544,   546,     0,     0,   549,   551,
     552,   553,  -700,     0,     0,  -700,     0,     0,     0,   556,
     557,   558,   559,   560,   561,   562,   563,   564,   566,     0,
       0,     0,     0,     0,     0,  2416,     0,  -651,     0,     0,
       0,  -651,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -700,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -700,     0,     0,     0,  -700,  -700,
       0,     0,  1391,     0,     0,  1002,   347,     0,     0,     0,
       0,     0,     0,  -700,     0,     0,     0,  -651,     0,     0,
    -700,     0,     0,  -700,  -700,     0,     0,     0,     0,     0,
       0,  -700,     0,     0,     0,     0,     0,     0,  -700,     0,
    -700,     0,     0,  -700,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   352,     0,     0,     0,     0,
    -700,     0,     0,     0,  -700,     0,  -700,   315,     0,     0,
       0,     0,     0,     0,  -700,     0,     0,     0,     0,     0,
       0,     0,   316,     0,     0,     0,     0,     0,     0,  1667,
       0,     0,     0,     0,     0,     0,  -700,     0,     0,     0,
       0,   317,     0,     0,     0,   318,     0,     0,     0,     0,
    -700,     0,     0,     0,     0,   733,   735,   518,     0,     0,
       0,     0,     0,   741,     0,  -651,     0,     0,     0,     0,
       0,     0,     0,  -700,  -700,     0,     0,     0,     0,     0,
       0,     0,     0,  2549,  -700,     0,     0,     0,     0,  1667,
    1656,     0,     0,     0,  2555,     0,  -700,  -700,     0,     0,
       0,     0,     0,  -700,     0,  -700,     0,     0,     0,     0,
       0,   347,     0,  -700,     0,     0,     0,     0,     0,   319,
       0,     0,     0,  -700,     0,     0,     0,     0,     0,     0,
     320,     0,  -700,     0,     0,     0,     0,     0,     0,     0,
       0,  -700,  2580,     0,     0,  -700,  -700,     0,     0,  -700,
    2585,     0,  -700,     0,  2586,     0,   963,     0,  -700,     0,
    1393,     0,     0,     0,     0,     0,     0,   322,     0,     0,
       0,   323,     0,   324,     0,  -700,   325,     0,   326,   352,
     352,   352,     0,  -700,     0,     0,     0,     0,  -700,     0,
       0,   327,     0,     0,     0,     0,     0,     0,     0,  -700,
       0,     0,     0,     0,     0,     0,     0,  1021,     0,   328,
    -700,     0,  -700,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1022,  1020,     0,     0,  -700,     0,  -700,     0,
       0,     0,     0,  -700,  1667,     0,     0,     0,     0,     0,
       0,  -700,  1667,  1667,     0,     0,     0,     0,     0,     0,
       0,   329,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1667,  -700,
    -700,   330,     0,     0,     0,  -700,     0,     0,     0,     0,
    -700,     0,  -700,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -700,     0,     0,  -700,     0,     0,     0,
       0,     0,     0,   963,     0,     0,     0,   331,     0,  -700,
       0,     0,     0,     0,     0,     0,     0,     0,   332,   333,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -700,     0,     0,     0,     0,     0,     0,     0,     0,
    -700,   334,     0,  -700,     0,     0,     0,   335,   336,   337,
       0,     0,     0,     0,  1099,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   963,   963,     0,   338,     0,
       0,     0,     0,     0,     0,     0,   339,     0,   177,     0,
    -700,     0,     0,     0,     0,   347,   340,     0,     0,     0,
       0,     0,  -700,     0,  1657,     0,  -700,  -700,     0,     0,
    -868,     0,     0,  -868,     0,     0,     0,     0,     0,   963,
       0,  -700,     0,     0,     0,     0,     0,     0,  -700,  1667,
       0,  -700,  -700,     0,     0,     0,     0,  1667,     0,  -700,
       0,     0,     0,     0,     0,     0,  -700,     0,  -700,     0,
       0,  -700,     0,     0,     0,     0,     0,     0,   693,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  2391,     0,     0,  -700,  1667,
       0,     0,  -700,     0,  -700,  -868,     0,     0,     0, -1712,
     963,     0,  -700,     0,     0,     0,     0,     0,     0,  1391,
    -868,     0,  1002,     0,     0,  1606,  1607,  1608,     0,     0,
       0,     0,     0,  1609,  -700,     0,     0, -1712,     0,  -868,
       0,     0,     0,  -868,     0,     0,     0,     0,  -700,     0,
    1610,  1611,  1612,  1613,     0,   518,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -700,  -700,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -700,     0,     0,     0,  2391,     0,  2391,   347,
     347,   347,     0,     0,  -700,  -700,     0,     0,     0,     0,
       0,  -700,     0,  -700,   315,     0,     0,     0,     0,     0,
       0,  -700,     0,     0,     0,     0,     0,  -868,     0,   316,
       0,  -700,     0,     0,     0,     0,     0,     0,  -868,     0,
    -700,     0,     0,     0,     0,     0,     0,     0,   317,  -700,
       0,     0,   318,  -700,  -700,     0,     0,  -700,     0,     0,
    -700,     0,     0,     0,     0,     0,  -700,     0,  -868,     0,
       0,     0,     0,     0,     0,  -868,     0,     0,     0,  -868,
       0,  -868,     0,  -700,  -868,     0,  -868,     0,     0,     0,
       0,  -700,     0,     0,     0,     0,  -700,  1614,  1615,  -868,
       0,     0,     0,     0,     0,     0,     0,  -700,     0,     0,
    1616,     0,     0,     0,     0,  1021,     0,  -868,  -700,     0,
    -700,     0,  -868,     0,     0,     0,   319,  1617,     0,     0,
    1022,     0,     0,  1618,     0,     0,     0,   320,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1619,     0,     0,  -868,
       0,     0,     0,     0,  1620,     0,     0,  1393,     0,  1621,
       0, -1712,     0,     0,   322,     0,     0,     0,   323,     0,
     324,     0,     0,   325,     0,   326,     0,     0,     0,  -868,
       0,     0,     0,     0,     0,     0,     0,     0,   327,     0,
    1622,  1623,     0,     0,  1624,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1625,     0,   328,     0,     0,  -868,
       0,  1626,     0,     0,     0,  -868,  1627,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -868,  -868,     0,     0,
       0,     0,     0,  1628,     0,     0,  1104,     0,  1105,     0,
       0,     0,     0,  1106,     0,     0,     0,     0,   329,  -868,
       0,  1107,     0,     0,     0,  -868,  -868,  -868,     0,     0,
       0,     0,     0,  -868,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -868,     0,   330,     0,
   -1712,     0,     0,     0,  -868,     0,  -868,     0,     0,  1108,
    1109,     0,     0,     0,  -868,  1110,     0,     0,     0,     0,
    1111,     0,  1112,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1113,   331,     0,  1114,     0,     0,     0,
       0,     0,     0,     0,     0,   332,   333,     0,  1629,  1115,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   334,     0,
       0,  1116,     0,     0,   335,   336,   337,     0,     0,     0,
    1117,     0,     0,  1118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   338,     0,     0,     0,     0,
       0,     0,     0,   339,     0,   177,     0,     0,     0,     0,
       0,     0,     0,   340,  1630,  1631,  1632,  1633,     0,  1634,
    1119,  1635,  1636,  1637,  1638,  1639,     0,     0,     0,     0,
       0,     0,  1120,     0,     0,     0,  1121,  1122,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1123,     0,     0,     0,     0,     0,     0,  1124,     0,
       0,  1125,  1126,     0,     0,     0,     0,  1002,     0,  1127,
       0,     0,     0,     0,     0,     0,  1128,     0,  1129,     0,
       0,  1130,     0,     0,     0,     0,     0,     0,     0,  1037,
       0,  1038,     0,     0,  1039,     0,     0,     0,     0,   880,
     881,   882,   883,   884,   885,   886,   887,  1040,  1041,  1042,
       0,     0,     0,     0,     0,     0,     0,     0,  1131,     0,
       0,     0,  1132,     0,  1133,     0,     0,     0,     0,     0,
       0,     0,  1134,  1043,     0,     0,     0,     0,     0,     0,
     888,     0,   889,   890,   891,   892,   893,   894,   895,     0,
       0,     0,     0,     0,  1135,  1044,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1136,     0,
       0,     0,     0,     0,     0,   518,     0,     0,     0,     0,
       0,     0,     0,   317,     0,   896,   897,     0,     0,     0,
       0,  1137,  1138,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1140,  1141,     0,     0,     0,     0,
       0,  1142,     0,  1143,     0,     0,     0,     0,     0,     0,
       0,  1144,     0,     0,  1045,     0,     0,     0,     0,     0,
       0,  1145,     0,     0,     0,     0,     0,     0,     0,     0,
    1146,     0,     0,     0,  1046,     0,     0,  1047,  1048,  1147,
       0,     0,     0,  1148,  1149,     0,     0,  1150,     0,     0,
    1151,  1049,   320,     0,     0,     0,  1152,     0,   898,     0,
       0,  1002,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1153,   899,     0,     0,     0,     0,     0,
       0,  1154,     0,     0,     0,   900,  1155,     0,     0,     0,
    1050,     0,     0,   323,     0,     0,     0,  1156,  1051,  1052,
     326,     0,     0,     0,     0,     0,     0,     0,  1157,     0,
    1158,   901,     0,     0,     0,     0,     0,     0,  1053,     0,
    1159,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1054,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   315,     0,     0,     0,  1055,   903,     0,
       0,     0,     0,     0,   904,     0,   905,     0,   316,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     906,  1056,     0,   329,     0,     0,     0,   317,     0,     0,
       0,   318,     0,     0,  1104,     0,  1105,     0,     0,     0,
       0,  1106,     0,     0,     0,     0,     0,  1057,     0,  1107,
       0,     0,     0,     0,  1058,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1059,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   219,     0,
     909,   910,   911,  1060,     0,     0,     0,  1108,  1109,   331,
       0,     0,     0,  1110,     0,     0,     0,     0,  1111,     0,
    1112,     0,     0,     0,     0,   319,     0,     0,  1061,     0,
       0,  1113,     0,     0,  1114,     0,   320,     0,     0,  1062,
       0,     0,     0,   334,     0,     0,     0,  1115,     0,     0,
       0,     0,     0,     0,  1063,     0,     0,     0,   913,   914,
     915,     0,     0,     0,     0,     0,     0,     0,     0,  1116,
    1064,   916,     0,  1065,  1066,     0,     0,   323,  1117,   324,
     177,  1118,   325,     0,   326,     0,     0,     0,   340,     0,
       0,     0,     0,     0,     0,  1067,     0,   327,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   328,     0,     0,  1119,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1120,     0,     0,     0,  1121,  1122,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1123,
       0,     0,     0,     0,     0,     0,  1124,   329,     0,  1125,
    1126,     0,  1391,     0,     0,  1002,     0,  1127,     0,     0,
       0,     0,     0,     0,  1128,     0,  1129,     0,     0,  1130,
       0,     0,     0,     0,     0,     0,  1420,   330,     0, -1797,
       0,  1867, -1797,  1868,     0,     0,  1869,     0,     0,     0,
       0,     0,     0,     0,     0, -1797, -1797, -1797,     0,     0,
       0,  1870,     0,     0,     0,     0,  1131,     0,     0,     0,
    1132,     0,  1133,   331,     0,     0,     0,     0,     0,     0,
    1134, -1708,     0,     0,   332,   333,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   315,     0,     0,
       0,     0,  1135,     0,     0, -1797,     0,   334,  1391,  1871,
       0,  1002,   316,   335,   336,   337,  1136,     0,     0,     0,
       0,     0,     0,   518,     0,     0,     0,     0,     0,     0,
       0,   317,  1420,     0,   338,   318,     0,     0,     0,  1137,
    1138,     0,   339,     0,   177,     0,  1872,     0,     0,     0,
    1139,     0,   340,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1140,  1141,     0,     0,     0,     0,     0,  1142,
       0,  1143,     0,     0,     0,     0,     0, -1708,     0,  1144,
       0,     0, -1797,     0,     0,     0,  1873,  1674,     0,  1145,
       0,     0,     0,   315,     0,     0,     0,     0,  1146,     0,
       0,     0, -1797,     0,     0,     0,  1874,  1147,   316,   319,
    1875,  1148,  1149,     0,     0,  1150,     0,     0,  1151, -1797,
     320,     0,     0,  1876,  1152,     0,     0,   317,     0,     0,
       0,   318,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1153,  1391,     0,     0,  1002,     0,     0,     0,  1154,
    1393,     0,     0,     0,  1155,     0,     0,   322, -1708,     0,
       0,   323,     0,   324,     0,  1156,   325, -1797,   326,     0,
    1877,  1878,     0,     0,     0,     0,  1157,     0,  1158,     0,
       0,   327,     0,     0,     0,     0, -1797,     0,     0,     0,
    1879,     0,     0,     0, -1797,     0,     0,     0,     0,   328,
       0,     0,     0,     0,     0,   319,     0,     0,     0,     0,
       0,     0,     0,     0,     0, -1797,   320,     0,     0,  1880,
       0,     0,     0,     0,     0, -1708,     0,   315,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   329,   316,  1881,     0,     0,  1393,     0,     0,     0,
       0,     0,     0,   322, -1708,     0,     0,   323,     0,   324,
   -1769,   317,   325, -1769,   326,   318,     0,     0,     0,  1882,
       0,   330, -1797,     0,     0,     0,  1883,   327,     0,     0,
       0,     0, -1797,     0,     0,     0,  1884,     0,     0,     0,
    1885,     0,     0,     0,     0,   328,     0,     0,     0,     0,
       0,     0,     0,  2048,     0,     0,     0,   331,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   332,   333,
       0, -1708,     0,     0,     0,     0,     0,     0,     0,     0,
    1886,     0,     0,     0,     0,     0,     0,   329,     0,   319,
       0,   334,     0,     0,     0, -1769,     0,   335,   336,   337,
     320,     0, -1797,     0,     0,     0,  1887,     0,     0,     0,
   -1769,     0,     0,  1421,     0,     0,  1888,   330,   338,     0,
       0,     0,  1889,     0,     0,     0,   339,   123,   177, -1769,
    1393,     0,     0, -1769,     0,     0,   340,   322,     0,     0,
       0,   323,     0,   324,  1391,     0,   325,  1002,   326,     0,
       0,     0,     0,   331,     0,     0,     0,     0,     0,     0,
       0,   327,     0,     0,   332,   333,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   328,
       0,     0,     0,     0,     0,     0,     0,   334,     0,     0,
       0,     0,     0,   335,   336,   337,     0,     0,     0,     0,
       0,     0,  1442,     0,     0,     0,     0, -1769,     0,  1421,
    1452,     0,     0,     0,   338,     0,     0,     0, -1769,     0,
       0,   329,   339,   123,   177,     0,     0,     0,     0,   315,
       0,     0,   340,     0,     0,     0,     0,     0,     0,     0,
    1391,     0,     0,  1002,   316,     0,     0,     0, -1769,     0,
       0,   330,     0,     0,     0, -1769,     0,  1391,     0, -1769,
    1002, -1769,     0,   317, -1769,     0, -1769,   318,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0, -1769,
       0,     0,     0,     0,     0,     0,     0,   331,     0,     0,
       0,     0,     0,     0,     0,     0,     0, -1769,   332,   333,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  2049,     0,
       0,   334,  2050,     0,     0,   315,     0,   335,   336,   337,
       0,     0,     0,     0,     0,     0,     0,     0,     0, -1769,
     316,   319,   315,     0,     0,     0,     0,     0,   338,     0,
       0,     0,   320,     0,     0,     0,   339,   316,   177,   317,
       0,     0,     0,   318,     0,     0,   340,  1453,     0, -1769,
    1514,     0,     0,     0,     0,     0,   317,     0,     0,     0,
     318,     0,  1393,     0,     0,     0,     0,     0,     0,   322,
       0,     0,     0,   323,     0,   324,     0,     0,   325,     0,
     326,     0,     0,     0,     0, -1769,     0,     0,     0,     0,
    1443,     0,     0,   327,     0,     0, -1769, -1769,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1391,
       0,   328,  1002,     0,     0,     0,     0,   319,     0, -1769,
    1784,     0,     0,     0,     0, -1769, -1769, -1769,   320,     0,
       0,     0,     0,     0,   319,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   320, -1769,     0,     0,     0,
       0,     0,     0,   329, -1769,     0, -1769,     0,  1393,     0,
    1790,     0,     0,     0, -1769,   322,     0,     0,     0,   323,
       0,   324,     0,     0,   325,  1393,   326,     0,     0,     0,
       0,     0,   322,   330,     0,     0,   323,     0,   324,   327,
       0,   325,     0,   326,   315,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   327,   328,     0,   316,
       0,     0,     0,     0,     0,     0,     0,     0,  1391,   331,
       0,  1002,     0,     0,   328,     0,     0,     0,   317,     0,
     332,   333,   318,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   329,
       0,     0,     0,   334,     0,     0,     0,     0,     0,   335,
     336,   337,     0,     0,     0,     0,   329,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1785,     0,   330,
     338,     0,     0,  1391,     0,     0,  1002,     0,   339,     0,
     177,     0,     0,     0,     0,     0,   330,     0,   340,     0,
       0,     0,     0,   315,     0,     0,   319,     0,     0,     0,
       0,     0,     0,     0,  1392,   331,     0,   320,   316,     0,
       0,     0,     0,     0,     0,     0,   332,   333,     0,     0,
       0,     0,   331,     0,     0,     0,     0,   317,     0,     0,
       0,   318,     0,   332,   333,     0,     0,  1393,     0,   334,
       0,     0,     0,     0,   322,   335,   336,   337,   323,     0,
     324,     0,     0,   325,     0,   326,   334,     0,   315,     0,
       0,     0,   335,   336,   337,     0,   338,     0,   327,  1468,
       0,     0,     0,   316,   339,     0,   177,     0,     0,     0,
       0,     0,     0,   338,   340,     0,   328,     0,     0,     0,
       0,   339,   317,   177,     0,     0,   318,  1391,     0,     0,
    1002,   340,     0,     0,     0,   319,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   320,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   329,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1393,     0,     0,     0,
       0,     0,     0,   322,     0,     0,     0,   323,   330,   324,
       0,     0,   325,     0,   326,     0,     0,     0,     0,     0,
     319,     0,     0,     0,     0,     0,     0,   327,     0,     0,
       0,   320,   315,     0,     0,     0,     0,     0,     0,  1391,
       0,     0,  1002,     0,   331,   328,     0,   316,     0,     0,
       0,     0,     0,     0,     0,   332,   333,     0,     0,  2200,
    1514,  1393,     0,     0,     0,     0,   317,     0,   322,     0,
     318,     0,   323,     0,   324,     0,     0,   325,   334,   326,
       0,     0,     0,     0,   335,   336,   337,   329,     0,     0,
       0,     0,   327,     0,     0,     0,     0,     0,     0,     0,
    1978,     0,     0,     0,     0,   338,     0,     0,     0,     0,
     328,     0,     0,   339,   123,   177,  2201,   330,     0,     0,
       0,     0,     0,   340,   315,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1518,     0,     0,     0,   316,
       0,   315,     0,  1391,   319,     0,  1002,     0,     0,     0,
       0,     0,   329,   331,     0,   320,   316,     0,   317,     0,
       0,     0,   318,     0,   332,   333,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   317,     0,     0,     0,   318,
       0,     0,   330,     0,     0,  1393,     0,   334,     0,     0,
       0,     0,   322,   335,   336,   337,   323,     0,   324,     0,
       0,   325,     0,   326,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   338,     0,   327,     0,   331,     0,
       0,     0,   339,     0,   177,     0,     0,     0,   315,   332,
     333,     0,   340,     0,   328,     0,   319,     0,     0,     0,
       0,     0,     0,   316,     0,     0,     0,   320,     0,     0,
       0,     0,   334,   319,     0,     0,     0,     0,   335,   336,
     337,     0,   317,     0,   320,     0,   318,     0,     0,     0,
       0,     0,  1391,     0,     0,  1002,   329,  1393,     0,   338,
       0,     0,     0,     0,   322,     0,     0,   339,   323,   177,
     324,     0,  2202,   325,     0,   326,     0,   340,     0,     0,
       0,     0,     0,     0,     0,   323,   330,   324,   327,     0,
     325,     0,   326,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   327,   328,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     319,     0,   331,   328,     0,     0,     0,     0,     0,     0,
       0,   320,     0,   332,   333,     0,     0,   315,     0,  1391,
       0,     0,  1002,     0,     0,     0,     0,     0,   329,     0,
       0,     0,   316,     0,     0,     0,   334,     0,     0,     0,
       0,  1393,   335,   336,   337,   329,     0,     0,   322,     0,
       0,   317,   323,     0,   324,   318,     0,   325,   330,   326,
       0,     0,     0,   338,     0,     0,     0,     0,     0,     0,
       0,   339,   327,   177,     0,   330,     0,     0,     0,     0,
       0,   340,     0,     0,     0,     0,     0,     0,     0,     0,
     328,     0,     0,     0,   331,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   315,   332,   333,     0,     0,     0,
       0,   331,     0,     0,     0,     0,     0,     0,     0,   316,
       0,     0,   332,   333,     0,  1795,     0,     0,   334,   319,
       0,     0,   329,     0,   335,   336,   337,     0,   317,     0,
     320,     0,   318,     0,     0,   334,     0,     0,  2203,     0,
       0,   335,   336,   337,     0,   338,     0,     0,     0,     0,
       0,     0,   330,   339,     0,   177,  1391,     0,     0,  1002,
    1393,     0,   338,   340,     0,     0,     0,   322,     0,     0,
     339,   323,   177,   324,     0,     0,   325,     0,   326,     0,
     340,     0,     0,     0,     0,     0,     0,     0,   331,     0,
       0,   327,     0,     0,     0,     0,     0,     0,     0,   332,
     333,     0,     0,     0,     0,     0,   319,     0,     0,   328,
       0,     0,  1391,     0,     0,  1002,     0,   320,  1600,     0,
       0,     0,   334,     0,     0,     0,     0,     0,   335,   336,
     337,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   315,     0,     0,     0,     0,     0,  1393,     0,   338,
       0,   329,     0,     0,   322,     0,   316,   339,   323,   177,
     324,     0,     0,   325,     0,   326,     0,   340,     0,     0,
       0,     0,     0,     0,     0,   317,     0,     0,   327,   318,
       0,   330,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   328,   315,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   316,     0,     0,     0,     0,   331,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   332,   333,
       0,   317,     0,     0,     0,   318,     0,     0,   329,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   334,     0,   319,     0,     0,     0,   335,   336,   337,
       0,     0,     0,     0,   320,     0,     0,     0,   330,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   338,     0,
       0,     0,     0,     0,     0,     0,   339,     0,   177,     0,
       0,     0,     0,     0,  1393,     0,   340,     0,  2062,     0,
       0,   322,     0,     0,   331,   323,     0,   324,     0,   319,
     325,     0,   326,     0,     0,   332,   333,     0,     0,  2002,
     320,     0,     0,     0,     0,   327,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   334,     0,
       0,     0,     0,   328,   335,   336,   337,     0,     0,     0,
    1393,     0,     0,     0,     0,     0,     0,   322,     0,     0,
       0,   323,     0,   324,     0,   338,   325,     0,   326,     0,
       0,     0,     0,   339,     0,   177,  2003,     0,     0,     0,
       0,   327,     0,   340,     0,   329,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   328,
       0, -1121,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   330, -1121,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     314,     0,     0,     0,     0, -1121,     0,     0,     0, -1121,
       0,   329,     0,     0,     0,     0,  2123,     0,     0,     0,
       0,   331,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   332,   333,     0,     0,     0,     0,     0,     0,
       0,   330,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   315,     0,   334,     0,     0,     0,     0,
       0,   335,   336,   337,     0,     0,     0,     0,   316,   315,
       0,     0,     0,  2004,     0,     0,     0,   331,     0,     0,
       0,     0,   338, -1121,   316,     0,     0,   317,   332,   333,
     339,   318,   177,     0, -1121,     0,     0,     0,     0,     0,
     340,     0,     0,   317,     0,     0,     0,   318,     0,     0,
       0,   334,     0,     0,     0,     0,     0,   335,   336,   337,
       0,     0,  2005,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   314, -1121,     0, -1121,   338,     0,
   -1121,     0, -1121,     0,     0,     0,   339,     0,   177,     0,
       0,     0,     0,     0,     0, -1121,   340,     0,     0,   314,
       0,     0,     0,     0,     0,   319,     0,     0,     0,     0,
       0,     0,     0, -1121,     0,     0,   320,     0,     0,     0,
       0,   319,     0,     0,     0,     0,     0,   315,     0,     0,
       0,     0,   320,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   316,     0,     0,     0,   321,     0,     0,     0,
       0,     0,   315,   322,     0, -1121,     0,   323,     0,   324,
       0,   317,   325,     0,   326,   318,     0,   316,     0,     0,
       0,     0,     0,   323,     0,   324,     0,   327,   325,     0,
     326,     0,     0,     0,     0, -1121,   317,     0,     0,     0,
     318,     0,     0,   327,     0,   328,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   328,     0,     0,     0,     0,     0,     0,     0,     0,
       0, -1121,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0, -1121, -1121,     0,     0,     0,   329,     0,   319,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     320,     0,     0,   329,     0, -1121,     0,     0,  2006,     0,
       0, -1121, -1121, -1121,   319,     0,     0,   330,     0,     0,
       0,     0,     0,     0,     0,   320,     0,     0,     0,     0,
     321,  2142, -1121,   330,     0,     0,     0,   322,     0,     0,
   -1121,   323, -1121,   324,     0,     0,   325,     0,   326,     0,
   -1121,     0,     0,   331,     0,   321,     0,     0,     0,     0,
       0,   327,   322,     0,   332,   333,   323,     0,   324,   331,
       0,   325,     0,   326,     0,     0,     0,     0,     0,   328,
     332,   333,     0,     0,     0,     0,   327,   334,     0,     0,
       0,     0,     0,   335,   336,   337,     0,     0,     0,     0,
       0,     0,     0,   334,   328,     0,     0,     0,     0,   335,
     336,   337,     0,     0,   338,     0,     0,     0,     0,     0,
       0,   329,   339,     0,   177,     0,     0,     0,     0,     0,
     338,     0,   340,   341,   342,     0,     0,     0,   339,  2143,
     177,   343,   565,     0,     0,     0,   329,     0,   340,     0,
       0,   330,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   330,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   331,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   332,   333,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   331,     0,     0,     0,     0,     0,     0,     0,
       0,   334,     0,   332,   333,     0,     0,   335,   336,   337,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   334,     0,   338,     0,
       0,     0,   335,   336,   337,     0,   339,     0,   177,     0,
       0,     0,     0,     0,     0,     0,   340,   341,   342,     0,
       0,     0,     0,   338,     0,   343,   734,     0,     0,     0,
       0,   339,     0,   177,     0,     0,     0,     0,     0,     0,
       0,   340,   341,   342,     0,     0,     0,     0,     0,     0,
     343,   880,   881,   882,   883,   884,   885,   886,   887,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1322,  1323,  1324,  1325,     0,     0,     0,
       0,     0,   888,     0,   889,   890,   891,   892,   893,   894,
     895,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   878,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   879,     0,   896,   897,     0,
       0,     0,   880,   881,   882,   883,   884,   885,   886,   887,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   888,     0,   889,   890,   891,   892,   893,
     894,   895,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1326,     0,     0,     0,     0,     0,     0,   896,   897,
     898,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   899,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   900,     0,     0,
       0,     0,  1327,  1328,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   901,     0,     0,     0, -1734,     0,  1329,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   902,     0,     0, -1734,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     903,   898,     0,     0,     0,     0,   904,     0,   905,     0,
       0,  1330,  1420, -1797,   188, -1797,     0,   899, -1797,     0,
       0,     0,   906, -1778,  1420, -1797,     0, -1797,   900,     0,
   -1797,     0,     0, -1797,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0, -1797,     0,     0,     0,     0,
       0,     0,     0,     0,   901,     0,   315, -1708,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0, -1708,
       0,   316,     0,     0,     0,     0,   902,     0,     0,     0,
     219, -1797,   909,   910,   911,     0,     0,     0,     0,  1331,
     317,   903,     0, -1797,   318,     0,     0,   904,     0,   905,
       0,     0,     0,     0,     0,     0,  1332,     0,     0,     0,
       0,     0,     0,   906,     0,     0,     0,     0, -1797,     0,
       0,     0,     0,     0,     0,     0,   907,     0,     0,     0,
   -1797,     0,     0,   908,  1333,     0,     0,     0,     0,     0,
     913,   914,   915,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   916,     0,     0,   917,  1334, -1797,     0,
       0,     0,  1474,     0,     0,   315,     0,     0,   319,     0,
   -1797,   219,     0,   909,   910,   911,     0,  1335, -1797,   320,
     316,  1588, -1797,     0,     0,     0,     0,     0,     0,     0,
   -1797,     0,     0,     0, -1797, -1797,     0,     0,     0,   317,
     912,     0,     0,   318,     0,     0,     0, -1797,     0,   321,
       0,     0,     0,     0,     0,     0,   322,     0,     0, -1778,
    1475,     0,   324,     0,     0,   325, -1734,   326,     0,     0,
       0,   913,   914,   915, -1708,     0,     0,     0,     0,     0,
     327,     0, -1797, -1797,   916,     0, -1708,   917,     0,     0,
       0,     0,     0,     0, -1797, -1797,     0,     0,   328,     0,
       0,     0, -1797,     0,     0,     0,     0,     0,     0,     0,
       0,  1474,     0,     0, -1797,     0,     0,   319,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   320,     0,
       0, -1797,     0,     0,     0,     0,     0,     0,     0,     0,
     329, -1708,     0, -1797,     0,     0,     0,     0,   315,     0,
       0,     0,     0, -1708,     0, -1797,     0,     0,   321,     0,
       0,     0,     0,   316,     0,   322,     0, -1797,     0,   323,
     330,   324,     0,     0,   325,     0,   326,     0,     0,     0,
       0, -1797,   317,     0,     0,     0,   318,     0, -1797,   327,
       0,     0,     0, -1797,     0,     0,     0,     0, -1797,     0,
   -1797,     0, -1797,     0,     0,     0,   331,   328,     0,     0,
   -1797,     0,     0,     0, -1797,     0,     0,   332,   333,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0, -1797,
       0,     0,     0,     0,     0,   315,     0,     0,     0,     0,
     334,     0, -1797,     0,     0,     0,   335,   336,   337,   329,
     316,  1589,     0,     0, -1797,     0,     0,     0,     0, -1174,
     319,     0,     0,  1589,     0,     0,     0,   338, -1797,   317,
       0,   320, -1174,   318,     0,   339,   123,  1476, -1797,   330,
   -1797,     0,     0,     0, -1797,   340,     0,     0,     0,     0,
   -1797,     0,     0,   123,     0,     0, -1797,     0,     0,     0,
       0,   321,     0,     0,     0,   123,     0,     0,   322,     0,
       0,     0,   323,     0,   324,   331,     0,   325,     0,   326,
       0,     0,     0,     0,     0,     0,   332,   333,     0,     0,
       0,     0,   327,     0,     0,     0,     0,     0, -1797,     0,
       0,     0,     0,     0,     0,     0,     0,   319,     0,   334,
     328,     0,     0,     0,     0,   335,   336,   337,   320,     0,
       0,     0,     0,     0,     0,     0,     0,     0, -1174,     0,
       0,     0,     0,     0,     0,     0,   338,     0,     0,     0,
       0, -1174,     0,     0,   339,   123,   177,     0,     0,     0,
       0,     0,   329,     0,   340,     0,     0,     0,     0,   323,
       0,   324,     0,     0,   325,     0,   326,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   327,
       0,     0,   330, -1165,     0,     0,     0, -1165, -1165, -1165,
   -1165, -1165, -1165, -1165, -1165, -1165,     0,   328, -1165, -1165,
    1750,     0, -1165, -1165, -1165, -1165, -1165, -1165, -1165, -1165,
   -1165, -1165,     0,     0,     0,     0,     0,     0,   331,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   332,
     333,     0,     0,     0,     0,     0,     0,     0,     0,   329,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   334,     0,     0,     0,     0,     0,   335,   336,
     337,     0,     0,     0,     0,     0,     0,     0,     0,   330,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   338,
       0,     0,     0,     0,     0,     0,     0,   339,     0,   177,
       0,     0,     0,     0,     0,     0,     0,   340,     0,     0,
       0,     0,     0,     0,     0,   331,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   332,   333,     0,     0,
       0,     0,     0,     0,     0,     0,     0, -1165, -1165, -1165,
   -1165, -1165, -1165,     0,     0,     0,     0,     0,     0,   334,
       0,     0,     0,     0,     0,   335,   336,   337,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   338,     0,     0,     0,
       0,     0,     0,     0,   339,     0,   177,     0,     0,     0,
       0, -1001,     0,     0,   340, -1001, -1001, -1001, -1001, -1001,
   -1001, -1001, -1001, -1001,     0,     0, -1001, -1001, -1001,     0,
   -1001, -1001, -1001, -1001, -1001, -1001, -1001, -1001, -1001, -1001,
    -698,     0,     0,     0,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,     0,     0,  -698,  -698,  -698,     0,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0, -1165,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0, -1001, -1001, -1001, -1001, -1001,
   -1001,     0, -1165,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -698,  -698,  -698,  -698,  -698,  -698,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0, -1001,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   -1001,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -698,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -698
};

static const yytype_int16 yycheck[] =
{
      59,    93,   204,   182,   266,   322,  1301,   186,   436,   370,
    1003,  1004,   215,   939,   217,   798,  1552,  1047,   221,  1235,
    1264,   372,  1621,  1624,  1624,  1241,  1156,  1624,  1012,   230,
    1285,   722,   368,  1277,   417,  1420,  1600,  1371,   821,   132,
     133,   134,  1369,   270,   137,   971,  1301,   140,  1447,   945,
      18,     5,   874,    18,    23,     7,     7,   260,  1385,    22,
      10,  1460,   241,    10,  1406,    10,   718,   719,   271,   995,
       7,    77,    10,   252,   277,    10,     7,  1271,   105,  1532,
      22,    10,    95,   176,  1819,    67,   265,    80,  1930,    10,
      54,     7,    80,  1751,   187,   917,    23,    55,    64,   115,
       7,     7,   133,    59,   197,    24,   128,   135,  1507,  1050,
      67,  1496,  1710,    80,   475,  1957,  1251,   971,   211,   212,
      67,   214,   128,   129,     8,   218,  1421,    56,   189,   135,
     133,  1390,   225,   807,  2264,   231,    90,   520,  1591,   203,
     366,     3,    38,  1707,    40,    41,   299,    38,  1074,    40,
      41,    33,   192,   187,    65,    73,   248,   207,     6,  1748,
    1086,    71,    38,   168,    40,    41,   124,   280,    55,   396,
     227,    29,   244,  1269,   208,  2017,   269,    97,  1742,    80,
    1514,  1491,   834,   192,    38,   256,    40,    41,    98,    99,
     207,   126,    80,   268,   255,   230,   215,   126,   217,   396,
    1096,  1528,   221,    31,    80,   126,  1501,   233,   136,    96,
    1505,   324,   134,   256,  1419,     9,  1312,  1753,   134,  1529,
     226,   234,   314,    55,  2066,    42,    96,   246,    80,  1299,
     102,  1795,   169,    38,   256,    40,    41,   124,   281,   466,
     192,   260,    36,   396,  1066,  1074,  1316,   194,  1503,   341,
     342,   343,   271,   450,   124,   134,    22,  1086,   277,   187,
     286,   353,  1758,  1293,   184,   123,    31,   331,    38,   304,
      40,    41,   946,  1257,   367,    80,   282,   135,   284,   233,
     215,    80,   509,   237,  2126,  1490,   215,   202,   242,    54,
     971,   308,   122,  1588,   215,   356,   243,   450,   370,   230,
     393,   183,   271,   362,   121,    38,   256,    40,    41,   236,
     371,  1407,  1408,  1409,   142,   234,   245,  1975,  1244,   219,
    1246,   256,   372,  1722,   234,   330,  1915,   256,   263,   124,
    1988,   268,   256,  1592,   263,   256,  1900,   429,   430,  1332,
     432,   433,   263,   971,   271,   406,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   432,    67,    55,
     128,  1287,   273,   995,   456,   457,   458,   459,   460,   461,
     462,   463,   464,   465,  1228,  1774,  1974,   473,   581,   582,
     473,  1723,   264,  1324,   232,   588,   479,  2122,   481,   482,
     348,   384,   485,   486,   256,  1848,   384,   490,  1391,   625,
    1393,   336,   473,   313,  1857,   374,  1600,   336,    93,   466,
    1737,  1796,   446,  1755,  1658,   336,    38,   384,    40,    41,
      10,  2551,   239,  1097,   256,   428,   256,   128,   124,    38,
     195,    40,    41,   473,    82,  1531,   529,    55,  2280,  1574,
    1366,   133,   300,   646,   472,   293,   473,   374,  1478,   510,
     432,   473,    73,  1446,   410,   468,   472,   473,    80,   490,
     428,   348,   665,  1805,   473,  1709,  1800,   402,   473,   387,
     101,  1856,   344,   402,   438,  2317,   569,  2319,   348,   907,
     432,   402,   354,   384,  1739,   688,  1382,  1383,   256,   254,
     971,   433,   439,  2151,  2152,   437,   384,  2155,   491,  2157,
     466,   436,  1901,   491,   388,   470,   124,   436,   384,   467,
     473,   373,  2354,  1707,  2249,   436,   432,   474,   105,   488,
     472,   473,   473,   473,   491,   432,   432,   620,   473,  2187,
     623,   624,   384,   626,   456,   473,   473,  1366,   473,   187,
     456,   307,   473,    82,  2386,  2387,   481,  1228,  1742,   473,
     642,   643,   481,   348,   473,   256,  1761,   473,   650,   578,
     481,   764,   581,   582,  1255,  1256,   473,   473,   191,   588,
     524,   388,   472,   488,  2172,   581,   959,   456,   488,   384,
     467,   600,   971,  2079,   256,   384,   482,   483,   484,   485,
     491,   487,   136,   216,   945,   798,   487,   467,   494,   692,
    1228,  1795,  2166,   491,  2168,   477,   482,   483,   484,   485,
     295,   487,  1244,   105,   472,   491,  2001,  2016,   821,   822,
    1954,  1955,    24,  2281,  2009,   718,   719,   646,   482,   483,
     484,   485,   725,   487,   343,    38,  1019,    40,    41,   491,
     494,   473,   869,   473,   265,   848,   665,  1863,   187,   481,
     669,  2215,   348,  2106,   715,  2321,   859,   243,  1985,   665,
    2045,   243,   723,   319,  1092,  1093,   133,   433,   364,   688,
     763,   437,   467,   438,   359,  2333,  1328,   482,   483,   484,
     485,   268,   487,  1978,  2332,   778,   491,  2251,  1584,  2531,
    2532,   281,   491,  1384,   787,   234,   256,   345,   207,  1848,
    2236,   794,   256,   722,   396,   473,  1900,   472,  1857,   330,
    2368,  2369,   482,   483,   484,   485,   401,   487,   403,  2377,
    1056,   814,   815,    22,   355,   369,   234,  1549,    98,  2128,
     348,   239,  1762,  1949,   827,   220,  1952,   256,   325,   832,
     833,   243,  1683,  1684,  1685,   764,   364,  1228,  2039,   339,
     843,   484,   485,   220,   487,   125,    55,   260,   450,   207,
    2015,   382,   384,    55,  2422,   826,   387,   126,  2434,    42,
     776,   467,   473,  2431,    23,  1002,   268,   869,   281,   798,
     256,    24,  1766,  2117,  2118,   846,  1770,   435,  2130,   260,
     851,   422,   488,   337,   182,   142,   190,    96,  1928,  1000,
    1001,   473,   821,   822,   865,  2453,   345,   189,   900,   481,
     207,  2196,   297,    32,  2213,    76,   472,   248,    38,   207,
      40,    41,   707,   916,   220,   124,  2484,  1923,     0,   848,
     297,   126,   234,   325,   299,  2169,   256,   856,   857,  1228,
     859,   902,   488,   108,   388,   380,  2504,  2505,   132,   467,
     473,   220,   473,   439,   207,   364,   215,   439,   175,   176,
     482,   483,   484,   485,  1286,   487,  2524,  2533,  2534,   491,
     488,  2256,   345,   482,   483,   484,   485,   127,   487,   971,
    2481,  2481,   491,   314,  2481,   488,   473,  1309,   205,   908,
      38,   788,    40,    41,   211,  2111,   435,   256,  2114,   118,
     371,     0,  2206,   995,   263,   196,   791,   968,  2566,   305,
     341,   342,   343,   473,  2580,   400,  1946,  1947,  1948,   473,
     215,   309,   353,  1274,   372,   242,   191,   481,   297,  2173,
     247,   575,    75,   380,    77,  2262,   283,   439,   230,   404,
    1032,   190,  1293,   313,    38,  1038,    40,    41,  2164,   222,
    2133,  1044,  2243,  1046,   473,  1047,  1017,  2106,  2557,   289,
    2559,   256,   435,   327,   431,   473,  2300,  1060,   263,   472,
    1062,   335,  2166,  1065,  2168,   372,   364,   336,   707,   228,
     253,   473,  1074,  1266,  2080,  1268,   356,   236,  2322,   383,
    2234,   234,   253,  1054,  1086,   186,   378,   473,   429,   430,
     299,   432,   433,   253,   489,   436,   265,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,  1379,   372,
    1081,  2215,   271,   490,   206,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   267,   227,   256,   267,  1390,
    2344,   336,  1269,   402,   235,   218,  2142,  2143,    38,   348,
      40,    41,   434,   473,   153,  2591,   133,  2251,   157,   320,
    1843,   352,   791,   218,  1083,   238,  1125,    38,   341,    40,
      41,   330,   969,  1424,  2300,   325,   349,     5,   466,   482,
     483,   484,   485,   238,   487,  1312,   318,   260,   491,   318,
    2186,    29,    20,    38,  1273,    40,    41,   396,  2432,   182,
     260,   488,    30,    12,   203,   260,  2289,   402,   281,  1460,
     253,   254,     9,  2408,    42,  1294,  2395,   308,  2397,  1298,
     114,   281,   481,   382,  1303,   374,   281,  1478,   458,   265,
    2409,   125,  1311,   315,   383,   265,    29,  1229,  1231,    36,
    1232,   436,   472,  1235,  1237,    73,   875,  1240,  1431,  1241,
     115,   450,  1244,  2457,   110,  2459,  1955,   200,  1509,  1476,
      69,  1810,   299,   434,    12,  1226,  1483,  1595,   467,  1597,
    1262,  2267,   489,   186,   473,   492,   493,  1269,   365,   256,
    1407,  1408,  1409,   234,   122,   123,   481,   347,  1280,  1281,
     126,   256,  2408,  1285,   330,  1287,   105,   106,   344,   108,
     330,  1293,   488,   268,   400,   256,  2432,   116,   392,  1301,
     488,   642,   643,   256,   227,  2494,  1865,   319,  1310,   650,
    1312,    69,   235,   153,   323,  1318,   309,   157,   327,   488,
     123,   196,   331,   189,  1413,   208,  1255,  1256,  1331,  1418,
    1333,  1592,  1334,   208,   249,   250,   382,  2441,   313,  2443,
     482,   483,   382,   482,   483,   332,    28,   105,   106,   396,
     108,   234,   482,   483,   484,   485,   450,   487,   116,  1288,
     213,   491,   181,   203,  1366,   488,   298,  1296,   300,   215,
    1299,  1300,   473,  1302,   222,   488,   473,    53,   448,    26,
      27,  1352,  1720,   489,   222,   308,   469,  1316,   241,   255,
    1392,   433,  1653,   259,  1531,   437,    72,    13,   142,   271,
      16,    17,   472,   450,   469,  1407,  1408,  1409,  2117,  2118,
     256,   431,   392,    60,    61,   253,  2252,   263,  1420,   314,
    1647,   316,   260,   181,   482,   483,   484,   485,   392,   487,
      77,   339,    79,   491,   110,   330,    34,  2273,   257,    37,
     178,  1554,   488,    10,   182,  1447,    13,    23,   488,    16,
      17,   380,    50,    51,    52,  1384,  1717,   341,  1460,  1461,
     472,   473,  1575,   295,   111,   112,   113,   392,   306,   341,
     450,   343,  1585,   292,   312,   495,  1478,   195,   482,   483,
     484,   485,   392,   487,   540,   380,   450,   491,   544,   233,
     336,   331,   433,   549,  1496,   213,   437,   282,   473,   257,
     300,  1503,   100,   341,  1765,  1507,   481,  2343,   195,  2345,
    2577,   349,   378,   173,   398,   399,  1518,    93,   303,  1521,
    2587,  2424,   169,   361,   171,   450,   213,   359,  1530,  1531,
     432,   408,   179,   410,   292,  1538,  1539,   195,  2605,   283,
     450,  1544,   286,   488,   473,  1806,   260,  1808,  1551,   387,
     388,  1553,   390,   187,  1892,   213,   402,   376,   234,   488,
     398,   488,  1533,  1492,   240,  1567,   473,   281,   434,   401,
    1572,   403,   391,   233,   208,   251,  1579,   218,   219,   177,
     423,   488,   482,   483,   484,   485,    62,   487,  1559,  1560,
     260,   491,   472,   169,   260,   472,   434,   238,   488,   197,
     419,   482,   483,   484,   485,   126,   487,   254,   488,   475,
     491,   281,  1581,   256,   190,   281,   214,   138,   376,   260,
     202,   268,    98,    25,   284,  1554,   286,   482,   483,   484,
     485,   455,   487,   391,   619,   481,   491,   280,   114,   624,
     281,   341,   139,   343,   468,  1614,  1575,   139,   303,   125,
     488,   694,   228,   473,   473,   698,  1585,   154,   274,   275,
     236,   419,   154,   256,   262,   341,   342,   290,   291,   488,
     459,   460,   461,   462,   473,   318,   292,   293,   432,   433,
     356,   473,   358,   281,   396,  1688,   440,   290,   291,   443,
     444,   289,  1695,   234,   215,   271,   290,   291,   175,   176,
    1679,  1680,  1681,  1682,   253,   254,  1047,   274,   275,   341,
    2048,   343,   310,    95,   172,   473,   174,   473,  1979,   295,
    1722,   418,   419,   420,   196,   292,   293,    75,   205,    77,
    1843,    14,    15,   641,   211,   256,   644,  1739,   488,   647,
     648,   649,   263,   115,   652,   653,   654,   409,   656,   657,
     658,   659,  1754,   341,   391,   343,   277,   172,  2019,   174,
    1762,   172,   285,   174,   287,   242,   726,  1769,   728,   367,
     247,  2343,  1774,  2345,  2159,  2160,   391,   234,    67,   377,
     327,   248,   429,   359,   380,   132,  1923,   356,  1790,   380,
     380,   467,   256,   380,  1796,   387,   231,   488,   374,   269,
    1802,   278,    78,  2064,   234,   386,   132,   383,   488,   488,
    2071,   488,   488,   375,   380,   336,   490,   320,   256,  1748,
     133,    66,   268,   488,   380,   401,   473,   403,   469,   330,
     269,   472,   297,   490,   490,   341,   490,   490,   490,   490,
    2012,   490,  1813,  1814,  1815,   490,  1817,   314,   490,   447,
     490,   490,   490,   472,  1856,   490,   409,   490,   388,   490,
     428,  1863,   323,   488,   473,  1868,   495,   473,   239,   388,
     364,  1874,   338,   491,   341,   342,   343,   491,   491,   491,
     388,   402,    42,   405,   405,   406,   353,   473,   307,   301,
     256,   409,  1894,   488,   473,    24,   297,     8,   472,  1901,
     256,    94,    94,   488,   488,   488,   135,   468,  2020,   473,
     488,    18,   396,   303,  1843,   117,   432,   254,   220,   473,
     300,  1923,   473,   488,   208,    28,   208,   396,   193,   245,
     470,   481,   432,   370,    28,    31,   195,  1939,   380,   290,
     488,  2212,    37,  2080,  1946,  1947,  1948,  1949,   428,   431,
    1952,   256,  1293,   117,   385,   256,   434,   440,   326,   440,
     481,   122,   429,   430,   488,   432,   433,   115,   488,   436,
     256,   438,   439,   440,   441,   442,   443,   444,   445,   446,
     447,   448,   473,   223,   212,   470,    55,  2100,   417,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   465,  2001,
     389,   143,   225,   144,    67,  2142,  2143,  2009,  2010,   146,
     372,   149,   489,  2015,  2016,   492,   493,   150,   109,   427,
    1989,   155,   432,    55,   428,   431,   425,   200,   468,  1998,
     161,   200,   164,   120,   357,  2037,   182,   404,   213,   313,
     473,   200,     9,  2045,   432,   207,   364,   426,   432,  2186,
     148,   285,    37,   466,   151,    10,    42,    43,    44,    45,
      46,    47,    48,  2065,   192,   470,   390,   470,  2070,  2071,
     184,   432,   256,   473,   230,   432,    55,   260,  2080,  1420,
     200,   230,   200,   167,   194,   480,   479,   433,   239,    37,
      66,   364,   473,   329,   408,   286,  2357,    83,  2101,    85,
      86,    87,    88,    89,    90,    91,  2367,   466,   255,  2111,
     397,   364,  2114,  2116,   207,    18,   428,   289,   355,    55,
     156,     9,   200,   142,   239,   470,  2128,   470,  2217,  2098,
    2267,    10,   432,   260,   260,   260,   141,  1478,   327,   195,
    2142,  2143,   128,   129,    55,   288,   328,   432,   207,   352,
     368,    78,   152,   154,   118,  1496,   467,   467,    55,   196,
     432,   305,  2164,   159,   283,   160,   319,   121,   260,   309,
     260,  2100,   422,     8,   104,   642,   643,  1263,   110,   256,
     351,   248,   154,   650,  2186,   351,   154,    55,  1274,  1275,
     260,   162,   260,   282,  2196,   260,   104,   163,   284,   287,
     274,   166,  2204,  1289,  1290,  2208,   145,   147,    55,   435,
     327,  2213,   239,   165,   327,   327,   368,   455,  2220,  1305,
    1306,   120,   304,   158,   304,   175,   175,    56,   918,    31,
     306,   126,    33,    78,   105,   221,   116,   142,  2351,  2242,
    1775,  2170,   505,   592,   824,    39,   281,   314,    41,   673,
    2252,   396,   953,   572,  2256,   480,  1597,  2216,   665,    44,
      47,   252,   168,  1016,   994,  2267,  2269,  2270,   857,   613,
    1228,  1373,  1841,  1378,   341,   342,   343,  2014,  1228,  2272,
     610,  2085,  1321,  1347,  1560,  2601,   353,  2399,   274,   791,
     599,   958,  2478,  1030,   871,  2297,  1592,  2219,  1651,  2432,
    2268,  2137,  2304,  2138,  2301,  1935,  1424,  2140,  2322,  1709,
    1396,  1712,  1974,  2172,  1979,  2337,  1444,  1727,  1732,  1405,
    2204,  2007,  2022,  1478,  2220,   311,  2367,  2224,  1806,  1770,
    2238,  1501,  1302,   319,  1512,  1503,  1515,  2071,  1424,  2393,
    2071,  2343,  1589,  2345,  2551,  2398,  2229,   333,  2404,  1937,
    2462,  1640,   456,  1756,  1492,  1306,  2048,  1572,  1444,  1364,
     249,  1605,   429,   430,   527,   432,   433,   717,   751,  1455,
    1737,   438,   439,   440,   441,   442,   443,   444,   445,   446,
     447,   448,  1468,  2385,  2511,   585,   488,   310,  1716,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   465,  1838,
     136,   517,   869,  1239,    -1,    -1,  2408,   393,   394,   395,
      -1,    -1,    -1,    -1,  2503,    -1,    -1,    -1,    -1,    -1,
      -1,  1762,  2351,  2425,  1510,    -1,  1512,    -1,    -1,  1515,
      -1,  2543,    -1,    -1,  1520,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  2414,  2415,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  2542,    -1,  1796,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   451,   452,   453,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   473,    -1,    -1,
      -1,  2493,    -1,    -1,  2496,    -1,    -1,    -1,    -1,  2588,
      -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    2512,    -1,    -1,  2516,  1600,  1856,  2445,    -1,  2520,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    -1,    -1,    -1,    -1,  2465,    -1,  2541,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  2550,    -1,    -1,
      -1,    -1,    -1,    -1,  1640,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  2568,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   642,   643,    -1,    -1,    -1,
    1047,    -1,    -1,   650,    92,    -1,  2589,    -1,    -1,     1,
    2592,    -1,    -1,    -1,    -1,     7,    -1,    -1,    10,   107,
      -1,    13,    14,    15,    -1,  1946,  1947,  1948,    -1,    21,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,    -1,
    1706,  1707,   130,    -1,    -1,    -1,    38,    39,    40,    41,
      -1,  1717,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  2614,  2615,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1742,    -1,    -1,    -1,
    2001,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  2009,  2010,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1778,    -1,    -1,   107,   204,    -1,  1784,  1785,
      -1,    -1,    -1,  1789,  2045,    -1,    -1,   215,    -1,  1795,
      -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,   130,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   245,    -1,    -1,
      -1,  1827,    -1,    -1,   252,    -1,    -1,    -1,   256,    -1,
     258,    -1,    -1,   261,    -1,   263,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   175,   176,    -1,    -1,    -1,   276,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   188,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   294,    -1,    -1,    -1,
      -1,    -1,   204,   205,    -1,    -1,    -1,    -1,    -1,   211,
      -1,    -1,  1269,   215,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1287,    -1,   234,    -1,    -1,    -1,  1293,    -1,   336,    -1,
     242,    -1,    -1,   245,    -1,   247,    -1,    -1,    -1,    -1,
     252,    -1,    -1,    -1,   256,  1312,   258,    -1,    -1,   261,
      -1,   263,    -1,    -1,    -1,  2196,    -1,    -1,   366,    -1,
      -1,    -1,    -1,  2204,   276,    -1,   278,   279,    -1,    -1,
     282,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     292,    -1,   294,    -1,    -1,    -1,    -1,   299,    -1,    -1,
      -1,  1977,   304,    -1,   402,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   413,   414,    -1,    -1,   321,
      -1,    -1,    -1,    -1,    -1,  2256,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   336,    -1,    -1,    -1,   436,    -1,
      -1,    -1,    -1,    -1,   442,   443,   444,    -1,    -1,    -1,
    1407,  1408,  1409,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1420,   366,   463,    -1,    -1,    -1,    -1,
      -1,   248,    -1,   471,    -1,   473,  2052,    -1,    -1,    -1,
      -1,    -1,    -1,   481,   482,   483,    -1,    -1,    -1,    -1,
    1047,    -1,   490,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     402,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   413,   414,    -1,   416,    -1,    -1,    -1,    -1,    -1,
      -1,  1478,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   436,    -1,    -1,   314,    -1,  1496,
     442,   443,   444,    -1,     0,     1,    -1,    -1,     4,    -1,
       6,    -1,    -1,    -1,    -1,    11,    -1,    -1,    -1,    -1,
      -1,   463,    -1,    19,   341,   342,   343,    -1,    -1,   471,
      -1,   473,    -1,    -1,  1531,    -1,   353,    -1,    -1,   481,
     482,   483,   484,   485,    -1,   487,    -1,   489,   490,   491,
     492,   493,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    63,    -1,    -1,
      -1,    -1,    68,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1597,    97,    -1,    -1,    -1,  2221,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   430,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   438,   439,   119,   441,   442,    -1,    -1,   445,   446,
     447,   448,   128,    -1,    -1,   131,    -1,    -1,    -1,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   465,    -1,
      -1,    -1,    -1,    -1,    -1,  2271,    -1,   153,    -1,    -1,
      -1,   157,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   168,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   180,    -1,    -1,    -1,   184,   185,
      -1,    -1,     7,    -1,    -1,    10,  1293,    -1,    -1,    -1,
      -1,    -1,    -1,   199,    -1,    -1,    -1,   203,    -1,    -1,
     206,    -1,    -1,   209,   210,    -1,    -1,    -1,    -1,    -1,
      -1,   217,    -1,    -1,    -1,    -1,    -1,    -1,   224,    -1,
     226,    -1,    -1,   229,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1762,    -1,    -1,    -1,    -1,
     266,    -1,    -1,    -1,   270,    -1,   272,    92,    -1,    -1,
      -1,    -1,    -1,    -1,   280,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,    -1,    -1,    -1,    -1,  1796,
      -1,    -1,    -1,    -1,    -1,    -1,   302,    -1,    -1,    -1,
      -1,   126,    -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,
     316,    -1,    -1,    -1,    -1,   642,   643,   323,    -1,    -1,
      -1,    -1,    -1,   650,    -1,   331,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   339,   340,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  2469,   350,    -1,    -1,    -1,    -1,  1856,
     175,    -1,    -1,    -1,  2480,    -1,   362,   363,    -1,    -1,
      -1,    -1,    -1,   369,    -1,   371,    -1,    -1,    -1,    -1,
      -1,  1478,    -1,   379,    -1,    -1,    -1,    -1,    -1,   204,
      -1,    -1,    -1,   389,    -1,    -1,    -1,    -1,    -1,    -1,
     215,    -1,   398,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   407,  2528,    -1,    -1,   411,   412,    -1,    -1,   415,
    2536,    -1,   418,    -1,  2540,    -1,  1923,    -1,   424,    -1,
     245,    -1,    -1,    -1,    -1,    -1,    -1,   252,    -1,    -1,
      -1,   256,    -1,   258,    -1,   441,   261,    -1,   263,  1946,
    1947,  1948,    -1,   449,    -1,    -1,    -1,    -1,   454,    -1,
      -1,   276,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   465,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   473,    -1,   294,
     476,    -1,   478,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   488,     1,    -1,    -1,     4,    -1,     6,    -1,
      -1,    -1,    -1,    11,  2001,    -1,    -1,    -1,    -1,    -1,
      -1,    19,  2009,  2010,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   336,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  2045,    57,
      58,   366,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      68,    -1,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    81,    -1,    -1,    84,    -1,    -1,    -1,
      -1,    -1,    -1,  2080,    -1,    -1,    -1,   402,    -1,    97,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   413,   414,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   119,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     128,   436,    -1,   131,    -1,    -1,    -1,   442,   443,   444,
      -1,    -1,    -1,    -1,   142,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  2142,  2143,    -1,   463,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   471,    -1,   473,    -1,
     168,    -1,    -1,    -1,    -1,  1762,   481,    -1,    -1,    -1,
      -1,    -1,   180,    -1,   489,    -1,   184,   185,    -1,    -1,
       7,    -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,  2186,
      -1,   199,    -1,    -1,    -1,    -1,    -1,    -1,   206,  2196,
      -1,   209,   210,    -1,    -1,    -1,    -1,  2204,    -1,   217,
      -1,    -1,    -1,    -1,    -1,    -1,   224,    -1,   226,    -1,
      -1,   229,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  2252,    -1,    -1,   266,  2256,
      -1,    -1,   270,    -1,   272,    92,    -1,    -1,    -1,    96,
    2267,    -1,   280,    -1,    -1,    -1,    -1,    -1,    -1,     7,
     107,    -1,    10,    -1,    -1,    13,    14,    15,    -1,    -1,
      -1,    -1,    -1,    21,   302,    -1,    -1,   124,    -1,   126,
      -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,   316,    -1,
      38,    39,    40,    41,    -1,   323,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   339,   340,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   350,    -1,    -1,    -1,  2343,    -1,  2345,  1946,
    1947,  1948,    -1,    -1,   362,   363,    -1,    -1,    -1,    -1,
      -1,   369,    -1,   371,    92,    -1,    -1,    -1,    -1,    -1,
      -1,   379,    -1,    -1,    -1,    -1,    -1,   204,    -1,   107,
      -1,   389,    -1,    -1,    -1,    -1,    -1,    -1,   215,    -1,
     398,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,   407,
      -1,    -1,   130,   411,   412,    -1,    -1,   415,    -1,    -1,
     418,    -1,    -1,    -1,    -1,    -1,   424,    -1,   245,    -1,
      -1,    -1,    -1,    -1,    -1,   252,    -1,    -1,    -1,   256,
      -1,   258,    -1,   441,   261,    -1,   263,    -1,    -1,    -1,
      -1,   449,    -1,    -1,    -1,    -1,   454,   175,   176,   276,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   465,    -1,    -1,
     188,    -1,    -1,    -1,    -1,   473,    -1,   294,   476,    -1,
     478,    -1,   299,    -1,    -1,    -1,   204,   205,    -1,    -1,
     488,    -1,    -1,   211,    -1,    -1,    -1,   215,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   234,    -1,    -1,   336,
      -1,    -1,    -1,    -1,   242,    -1,    -1,   245,    -1,   247,
      -1,   348,    -1,    -1,   252,    -1,    -1,    -1,   256,    -1,
     258,    -1,    -1,   261,    -1,   263,    -1,    -1,    -1,   366,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   276,    -1,
     278,   279,    -1,    -1,   282,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   292,    -1,   294,    -1,    -1,   396,
      -1,   299,    -1,    -1,    -1,   402,   304,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   413,   414,    -1,    -1,
      -1,    -1,    -1,   321,    -1,    -1,     4,    -1,     6,    -1,
      -1,    -1,    -1,    11,    -1,    -1,    -1,    -1,   336,   436,
      -1,    19,    -1,    -1,    -1,   442,   443,   444,    -1,    -1,
      -1,    -1,    -1,   450,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   463,    -1,   366,    -1,
     467,    -1,    -1,    -1,   471,    -1,   473,    -1,    -1,    57,
      58,    -1,    -1,    -1,   481,    63,    -1,    -1,    -1,    -1,
      68,    -1,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    81,   402,    -1,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   413,   414,    -1,   416,    97,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   436,    -1,
      -1,   119,    -1,    -1,   442,   443,   444,    -1,    -1,    -1,
     128,    -1,    -1,   131,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   463,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   471,    -1,   473,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   481,   482,   483,   484,   485,    -1,   487,
     168,   489,   490,   491,   492,   493,    -1,    -1,    -1,    -1,
      -1,    -1,   180,    -1,    -1,    -1,   184,   185,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   199,    -1,    -1,    -1,    -1,    -1,    -1,   206,    -1,
      -1,   209,   210,    -1,    -1,    -1,    -1,    10,    -1,   217,
      -1,    -1,    -1,    -1,    -1,    -1,   224,    -1,   226,    -1,
      -1,   229,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   266,    -1,
      -1,    -1,   270,    -1,   272,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   280,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    -1,    85,    86,    87,    88,    89,    90,    91,    -1,
      -1,    -1,    -1,    -1,   302,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   316,    -1,
      -1,    -1,    -1,    -1,    -1,   323,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   126,    -1,   128,   129,    -1,    -1,    -1,
      -1,   339,   340,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   350,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   362,   363,    -1,    -1,    -1,    -1,
      -1,   369,    -1,   371,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   379,    -1,    -1,   177,    -1,    -1,    -1,    -1,    -1,
      -1,   389,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     398,    -1,    -1,    -1,   197,    -1,    -1,   200,   201,   407,
      -1,    -1,    -1,   411,   412,    -1,    -1,   415,    -1,    -1,
     418,   214,   215,    -1,    -1,    -1,   424,    -1,   221,    -1,
      -1,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   441,   237,    -1,    -1,    -1,    -1,    -1,
      -1,   449,    -1,    -1,    -1,   248,   454,    -1,    -1,    -1,
     253,    -1,    -1,   256,    -1,    -1,    -1,   465,   261,   262,
     263,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   476,    -1,
     478,   274,    -1,    -1,    -1,    -1,    -1,    -1,   281,    -1,
     488,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   296,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    92,    -1,    -1,    -1,   310,   311,    -1,
      -1,    -1,    -1,    -1,   317,    -1,   319,    -1,   107,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     333,   334,    -1,   336,    -1,    -1,    -1,   126,    -1,    -1,
      -1,   130,    -1,    -1,     4,    -1,     6,    -1,    -1,    -1,
      -1,    11,    -1,    -1,    -1,    -1,    -1,   360,    -1,    19,
      -1,    -1,    -1,    -1,   367,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   381,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   391,    -1,
     393,   394,   395,   396,    -1,    -1,    -1,    57,    58,   402,
      -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    68,    -1,
      70,    -1,    -1,    -1,    -1,   204,    -1,    -1,   421,    -1,
      -1,    81,    -1,    -1,    84,    -1,   215,    -1,    -1,   432,
      -1,    -1,    -1,   436,    -1,    -1,    -1,    97,    -1,    -1,
      -1,    -1,    -1,    -1,   447,    -1,    -1,    -1,   451,   452,
     453,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,
     463,   464,    -1,   466,   467,    -1,    -1,   256,   128,   258,
     473,   131,   261,    -1,   263,    -1,    -1,    -1,   481,    -1,
      -1,    -1,    -1,    -1,    -1,   488,    -1,   276,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   294,    -1,    -1,   168,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     180,    -1,    -1,    -1,   184,   185,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   199,
      -1,    -1,    -1,    -1,    -1,    -1,   206,   336,    -1,   209,
     210,    -1,     7,    -1,    -1,    10,    -1,   217,    -1,    -1,
      -1,    -1,    -1,    -1,   224,    -1,   226,    -1,    -1,   229,
      -1,    -1,    -1,    -1,    -1,    -1,    31,   366,    -1,    34,
      -1,    32,    37,    34,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    52,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    -1,   266,    -1,    -1,    -1,
     270,    -1,   272,   402,    -1,    -1,    -1,    -1,    -1,    -1,
     280,    76,    -1,    -1,   413,   414,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    92,    -1,    -1,
      -1,    -1,   302,    -1,    -1,   100,    -1,   436,     7,   100,
      -1,    10,   107,   442,   443,   444,   316,    -1,    -1,    -1,
      -1,    -1,    -1,   323,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   126,    31,    -1,   463,   130,    -1,    -1,    -1,   339,
     340,    -1,   471,    -1,   473,    -1,   137,    -1,    -1,    -1,
     350,    -1,   481,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   362,   363,    -1,    -1,    -1,    -1,    -1,   369,
      -1,   371,    -1,    -1,    -1,    -1,    -1,    76,    -1,   379,
      -1,    -1,   177,    -1,    -1,    -1,   177,   182,    -1,   389,
      -1,    -1,    -1,    92,    -1,    -1,    -1,    -1,   398,    -1,
      -1,    -1,   197,    -1,    -1,    -1,   197,   407,   107,   204,
     201,   411,   412,    -1,    -1,   415,    -1,    -1,   418,   214,
     215,    -1,    -1,   214,   424,    -1,    -1,   126,    -1,    -1,
      -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   441,     7,    -1,    -1,    10,    -1,    -1,    -1,   449,
     245,    -1,    -1,    -1,   454,    -1,    -1,   252,   253,    -1,
      -1,   256,    -1,   258,    -1,   465,   261,   262,   263,    -1,
     261,   262,    -1,    -1,    -1,    -1,   476,    -1,   478,    -1,
      -1,   276,    -1,    -1,    -1,    -1,   281,    -1,    -1,    -1,
     281,    -1,    -1,    -1,   289,    -1,    -1,    -1,    -1,   294,
      -1,    -1,    -1,    -1,    -1,   204,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   310,   215,    -1,    -1,   310,
      -1,    -1,    -1,    -1,    -1,   320,    -1,    92,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   336,   107,   334,    -1,    -1,   245,    -1,    -1,    -1,
      -1,    -1,    -1,   252,   253,    -1,    -1,   256,    -1,   258,
       7,   126,   261,    10,   263,   130,    -1,    -1,    -1,   360,
      -1,   366,   367,    -1,    -1,    -1,   367,   276,    -1,    -1,
      -1,    -1,   377,    -1,    -1,    -1,   377,    -1,    -1,    -1,
     381,    -1,    -1,    -1,    -1,   294,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   168,    -1,    -1,    -1,   402,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   413,   414,
      -1,   320,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     421,    -1,    -1,    -1,    -1,    -1,    -1,   336,    -1,   204,
      -1,   436,    -1,    -1,    -1,    92,    -1,   442,   443,   444,
     215,    -1,   447,    -1,    -1,    -1,   447,    -1,    -1,    -1,
     107,    -1,    -1,   458,    -1,    -1,   457,   366,   463,    -1,
      -1,    -1,   463,    -1,    -1,    -1,   471,   472,   473,   126,
     245,    -1,    -1,   130,    -1,    -1,   481,   252,    -1,    -1,
      -1,   256,    -1,   258,     7,    -1,   261,    10,   263,    -1,
      -1,    -1,    -1,   402,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   276,    -1,    -1,   413,   414,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   294,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   436,    -1,    -1,
      -1,    -1,    -1,   442,   443,   444,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    -1,    -1,    -1,    -1,   204,    -1,   458,
     207,    -1,    -1,    -1,   463,    -1,    -1,    -1,   215,    -1,
      -1,   336,   471,   472,   473,    -1,    -1,    -1,    -1,    92,
      -1,    -1,   481,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    10,   107,    -1,    -1,    -1,   245,    -1,
      -1,   366,    -1,    -1,    -1,   252,    -1,     7,    -1,   256,
      10,   258,    -1,   126,   261,    -1,   263,   130,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   276,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   402,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   294,   413,   414,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   433,    -1,
      -1,   436,   437,    -1,    -1,    92,    -1,   442,   443,   444,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   336,
     107,   204,    92,    -1,    -1,    -1,    -1,    -1,   463,    -1,
      -1,    -1,   215,    -1,    -1,    -1,   471,   107,   473,   126,
      -1,    -1,    -1,   130,    -1,    -1,   481,   364,    -1,   366,
     120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,
     130,    -1,   245,    -1,    -1,    -1,    -1,    -1,    -1,   252,
      -1,    -1,    -1,   256,    -1,   258,    -1,    -1,   261,    -1,
     263,    -1,    -1,    -1,    -1,   402,    -1,    -1,    -1,    -1,
     273,    -1,    -1,   276,    -1,    -1,   413,   414,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,
      -1,   294,    10,    -1,    -1,    -1,    -1,   204,    -1,   436,
     207,    -1,    -1,    -1,    -1,   442,   443,   444,   215,    -1,
      -1,    -1,    -1,    -1,   204,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   215,   463,    -1,    -1,    -1,
      -1,    -1,    -1,   336,   471,    -1,   473,    -1,   245,    -1,
     230,    -1,    -1,    -1,   481,   252,    -1,    -1,    -1,   256,
      -1,   258,    -1,    -1,   261,   245,   263,    -1,    -1,    -1,
      -1,    -1,   252,   366,    -1,    -1,   256,    -1,   258,   276,
      -1,   261,    -1,   263,    92,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   276,   294,    -1,   107,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,   402,
      -1,    10,    -1,    -1,   294,    -1,    -1,    -1,   126,    -1,
     413,   414,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   336,
      -1,    -1,    -1,   436,    -1,    -1,    -1,    -1,    -1,   442,
     443,   444,    -1,    -1,    -1,    -1,   336,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   364,    -1,   366,
     463,    -1,    -1,     7,    -1,    -1,    10,    -1,   471,    -1,
     473,    -1,    -1,    -1,    -1,    -1,   366,    -1,   481,    -1,
      -1,    -1,    -1,    92,    -1,    -1,   204,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   103,   402,    -1,   215,   107,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   413,   414,    -1,    -1,
      -1,    -1,   402,    -1,    -1,    -1,    -1,   126,    -1,    -1,
      -1,   130,    -1,   413,   414,    -1,    -1,   245,    -1,   436,
      -1,    -1,    -1,    -1,   252,   442,   443,   444,   256,    -1,
     258,    -1,    -1,   261,    -1,   263,   436,    -1,    92,    -1,
      -1,    -1,   442,   443,   444,    -1,   463,    -1,   276,   103,
      -1,    -1,    -1,   107,   471,    -1,   473,    -1,    -1,    -1,
      -1,    -1,    -1,   463,   481,    -1,   294,    -1,    -1,    -1,
      -1,   471,   126,   473,    -1,    -1,   130,     7,    -1,    -1,
      10,   481,    -1,    -1,    -1,   204,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   215,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   336,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   245,    -1,    -1,    -1,
      -1,    -1,    -1,   252,    -1,    -1,    -1,   256,   366,   258,
      -1,    -1,   261,    -1,   263,    -1,    -1,    -1,    -1,    -1,
     204,    -1,    -1,    -1,    -1,    -1,    -1,   276,    -1,    -1,
      -1,   215,    92,    -1,    -1,    -1,    -1,    -1,    -1,     7,
      -1,    -1,    10,    -1,   402,   294,    -1,   107,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   413,   414,    -1,    -1,    10,
     120,   245,    -1,    -1,    -1,    -1,   126,    -1,   252,    -1,
     130,    -1,   256,    -1,   258,    -1,    -1,   261,   436,   263,
      -1,    -1,    -1,    -1,   442,   443,   444,   336,    -1,    -1,
      -1,    -1,   276,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     458,    -1,    -1,    -1,    -1,   463,    -1,    -1,    -1,    -1,
     294,    -1,    -1,   471,   472,   473,    67,   366,    -1,    -1,
      -1,    -1,    -1,   481,    92,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   103,    -1,    -1,    -1,   107,
      -1,    92,    -1,     7,   204,    -1,    10,    -1,    -1,    -1,
      -1,    -1,   336,   402,    -1,   215,   107,    -1,   126,    -1,
      -1,    -1,   130,    -1,   413,   414,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,   130,
      -1,    -1,   366,    -1,    -1,   245,    -1,   436,    -1,    -1,
      -1,    -1,   252,   442,   443,   444,   256,    -1,   258,    -1,
      -1,   261,    -1,   263,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   463,    -1,   276,    -1,   402,    -1,
      -1,    -1,   471,    -1,   473,    -1,    -1,    -1,    92,   413,
     414,    -1,   481,    -1,   294,    -1,   204,    -1,    -1,    -1,
      -1,    -1,    -1,   107,    -1,    -1,    -1,   215,    -1,    -1,
      -1,    -1,   436,   204,    -1,    -1,    -1,    -1,   442,   443,
     444,    -1,   126,    -1,   215,    -1,   130,    -1,    -1,    -1,
      -1,    -1,     7,    -1,    -1,    10,   336,   245,    -1,   463,
      -1,    -1,    -1,    -1,   252,    -1,    -1,   471,   256,   473,
     258,    -1,   243,   261,    -1,   263,    -1,   481,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   256,   366,   258,   276,    -1,
     261,    -1,   263,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   276,   294,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     204,    -1,   402,   294,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   215,    -1,   413,   414,    -1,    -1,    92,    -1,     7,
      -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,   336,    -1,
      -1,    -1,   107,    -1,    -1,    -1,   436,    -1,    -1,    -1,
      -1,   245,   442,   443,   444,   336,    -1,    -1,   252,    -1,
      -1,   126,   256,    -1,   258,   130,    -1,   261,   366,   263,
      -1,    -1,    -1,   463,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   471,   276,   473,    -1,   366,    -1,    -1,    -1,    -1,
      -1,   481,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     294,    -1,    -1,    -1,   402,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    92,   413,   414,    -1,    -1,    -1,
      -1,   402,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
      -1,    -1,   413,   414,    -1,   200,    -1,    -1,   436,   204,
      -1,    -1,   336,    -1,   442,   443,   444,    -1,   126,    -1,
     215,    -1,   130,    -1,    -1,   436,    -1,    -1,   439,    -1,
      -1,   442,   443,   444,    -1,   463,    -1,    -1,    -1,    -1,
      -1,    -1,   366,   471,    -1,   473,     7,    -1,    -1,    10,
     245,    -1,   463,   481,    -1,    -1,    -1,   252,    -1,    -1,
     471,   256,   473,   258,    -1,    -1,   261,    -1,   263,    -1,
     481,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   402,    -1,
      -1,   276,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   413,
     414,    -1,    -1,    -1,    -1,    -1,   204,    -1,    -1,   294,
      -1,    -1,     7,    -1,    -1,    10,    -1,   215,   432,    -1,
      -1,    -1,   436,    -1,    -1,    -1,    -1,    -1,   442,   443,
     444,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    -1,    -1,    -1,    -1,    -1,   245,    -1,   463,
      -1,   336,    -1,    -1,   252,    -1,   107,   471,   256,   473,
     258,    -1,    -1,   261,    -1,   263,    -1,   481,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,   276,   130,
      -1,   366,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   294,    92,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,    -1,    -1,   402,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   413,   414,
      -1,   126,    -1,    -1,    -1,   130,    -1,    -1,   336,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   436,    -1,   204,    -1,    -1,    -1,   442,   443,   444,
      -1,    -1,    -1,    -1,   215,    -1,    -1,    -1,   366,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   463,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   471,    -1,   473,    -1,
      -1,    -1,    -1,    -1,   245,    -1,   481,    -1,   396,    -1,
      -1,   252,    -1,    -1,   402,   256,    -1,   258,    -1,   204,
     261,    -1,   263,    -1,    -1,   413,   414,    -1,    -1,    10,
     215,    -1,    -1,    -1,    -1,   276,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   436,    -1,
      -1,    -1,    -1,   294,   442,   443,   444,    -1,    -1,    -1,
     245,    -1,    -1,    -1,    -1,    -1,    -1,   252,    -1,    -1,
      -1,   256,    -1,   258,    -1,   463,   261,    -1,   263,    -1,
      -1,    -1,    -1,   471,    -1,   473,    67,    -1,    -1,    -1,
      -1,   276,    -1,   481,    -1,   336,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   294,
      -1,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   366,   107,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,   130,
      -1,   336,    -1,    -1,    -1,    -1,   397,    -1,    -1,    -1,
      -1,   402,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   413,   414,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   366,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    92,    -1,   436,    -1,    -1,    -1,    -1,
      -1,   442,   443,   444,    -1,    -1,    -1,    -1,   107,    92,
      -1,    -1,    -1,   194,    -1,    -1,    -1,   402,    -1,    -1,
      -1,    -1,   463,   204,   107,    -1,    -1,   126,   413,   414,
     471,   130,   473,    -1,   215,    -1,    -1,    -1,    -1,    -1,
     481,    -1,    -1,   126,    -1,    -1,    -1,   130,    -1,    -1,
      -1,   436,    -1,    -1,    -1,    -1,    -1,   442,   443,   444,
      -1,    -1,   243,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    39,   256,    -1,   258,   463,    -1,
     261,    -1,   263,    -1,    -1,    -1,   471,    -1,   473,    -1,
      -1,    -1,    -1,    -1,    -1,   276,   481,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,   204,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   294,    -1,    -1,   215,    -1,    -1,    -1,
      -1,   204,    -1,    -1,    -1,    -1,    -1,    92,    -1,    -1,
      -1,    -1,   215,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,    -1,   245,    -1,    -1,    -1,
      -1,    -1,    92,   252,    -1,   336,    -1,   256,    -1,   258,
      -1,   126,   261,    -1,   263,   130,    -1,   107,    -1,    -1,
      -1,    -1,    -1,   256,    -1,   258,    -1,   276,   261,    -1,
     263,    -1,    -1,    -1,    -1,   366,   126,    -1,    -1,    -1,
     130,    -1,    -1,   276,    -1,   294,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   294,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   402,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   413,   414,    -1,    -1,    -1,   336,    -1,   204,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     215,    -1,    -1,   336,    -1,   436,    -1,    -1,   439,    -1,
      -1,   442,   443,   444,   204,    -1,    -1,   366,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   215,    -1,    -1,    -1,    -1,
     245,   364,   463,   366,    -1,    -1,    -1,   252,    -1,    -1,
     471,   256,   473,   258,    -1,    -1,   261,    -1,   263,    -1,
     481,    -1,    -1,   402,    -1,   245,    -1,    -1,    -1,    -1,
      -1,   276,   252,    -1,   413,   414,   256,    -1,   258,   402,
      -1,   261,    -1,   263,    -1,    -1,    -1,    -1,    -1,   294,
     413,   414,    -1,    -1,    -1,    -1,   276,   436,    -1,    -1,
      -1,    -1,    -1,   442,   443,   444,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   436,   294,    -1,    -1,    -1,    -1,   442,
     443,   444,    -1,    -1,   463,    -1,    -1,    -1,    -1,    -1,
      -1,   336,   471,    -1,   473,    -1,    -1,    -1,    -1,    -1,
     463,    -1,   481,   482,   483,    -1,    -1,    -1,   471,   472,
     473,   490,   491,    -1,    -1,    -1,   336,    -1,   481,    -1,
      -1,   366,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   366,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   402,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   413,   414,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   402,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   436,    -1,   413,   414,    -1,    -1,   442,   443,   444,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   436,    -1,   463,    -1,
      -1,    -1,   442,   443,   444,    -1,   471,    -1,   473,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   481,   482,   483,    -1,
      -1,    -1,    -1,   463,    -1,   490,   491,    -1,    -1,    -1,
      -1,   471,    -1,   473,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   481,   482,   483,    -1,    -1,    -1,    -1,    -1,    -1,
     490,    42,    43,    44,    45,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    74,    75,    76,    77,    -1,    -1,    -1,
      -1,    -1,    83,    -1,    85,    86,    87,    88,    89,    90,
      91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    -1,   128,   129,    -1,
      -1,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    -1,    85,    86,    87,    88,    89,
      90,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   212,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,
     221,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   237,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   248,    -1,    -1,
      -1,    -1,   253,   254,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   274,    -1,    -1,    -1,   187,    -1,   280,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   296,    -1,    -1,   208,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     311,   221,    -1,    -1,    -1,    -1,   317,    -1,   319,    -1,
      -1,   322,    31,    32,   234,    34,    -1,   237,    37,    -1,
      -1,    -1,   333,   243,    31,    32,    -1,    34,   248,    -1,
      37,    -1,    -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   274,    -1,    92,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      -1,   107,    -1,    -1,    -1,    -1,   296,    -1,    -1,    -1,
     391,   100,   393,   394,   395,    -1,    -1,    -1,    -1,   400,
     126,   311,    -1,   100,   130,    -1,    -1,   317,    -1,   319,
      -1,    -1,    -1,    -1,    -1,    -1,   417,    -1,    -1,    -1,
      -1,    -1,    -1,   333,    -1,    -1,    -1,    -1,   137,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   346,    -1,    -1,    -1,
     137,    -1,    -1,   353,   445,    -1,    -1,    -1,    -1,    -1,
     451,   452,   453,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   464,    -1,    -1,   467,   468,   177,    -1,
      -1,    -1,   198,    -1,    -1,    92,    -1,    -1,   204,    -1,
     177,   391,    -1,   393,   394,   395,    -1,   488,   197,   215,
     107,   200,   201,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     197,    -1,    -1,    -1,   201,   214,    -1,    -1,    -1,   126,
     420,    -1,    -1,   130,    -1,    -1,    -1,   214,    -1,   245,
      -1,    -1,    -1,    -1,    -1,    -1,   252,    -1,    -1,   439,
     256,    -1,   258,    -1,    -1,   261,   446,   263,    -1,    -1,
      -1,   451,   452,   453,   253,    -1,    -1,    -1,    -1,    -1,
     276,    -1,   261,   262,   464,    -1,   253,   467,    -1,    -1,
      -1,    -1,    -1,    -1,   261,   262,    -1,    -1,   294,    -1,
      -1,    -1,   281,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   198,    -1,    -1,   281,    -1,    -1,   204,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   215,    -1,
      -1,   310,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     336,   320,    -1,   310,    -1,    -1,    -1,    -1,    92,    -1,
      -1,    -1,    -1,   320,    -1,   334,    -1,    -1,   245,    -1,
      -1,    -1,    -1,   107,    -1,   252,    -1,   334,    -1,   256,
     366,   258,    -1,    -1,   261,    -1,   263,    -1,    -1,    -1,
      -1,   360,   126,    -1,    -1,    -1,   130,    -1,   367,   276,
      -1,    -1,    -1,   360,    -1,    -1,    -1,    -1,   377,    -1,
     367,    -1,   381,    -1,    -1,    -1,   402,   294,    -1,    -1,
     377,    -1,    -1,    -1,   381,    -1,    -1,   413,   414,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   425,
      -1,    -1,    -1,    -1,    -1,    92,    -1,    -1,    -1,    -1,
     436,    -1,   421,    -1,    -1,    -1,   442,   443,   444,   336,
     107,   430,    -1,    -1,   421,    -1,    -1,    -1,    -1,   455,
     204,    -1,    -1,   430,    -1,    -1,    -1,   463,   447,   126,
      -1,   215,   468,   130,    -1,   471,   472,   473,   457,   366,
     447,    -1,    -1,    -1,   463,   481,    -1,    -1,    -1,    -1,
     457,    -1,    -1,   472,    -1,    -1,   463,    -1,    -1,    -1,
      -1,   245,    -1,    -1,    -1,   472,    -1,    -1,   252,    -1,
      -1,    -1,   256,    -1,   258,   402,    -1,   261,    -1,   263,
      -1,    -1,    -1,    -1,    -1,    -1,   413,   414,    -1,    -1,
      -1,    -1,   276,    -1,    -1,    -1,    -1,    -1,   425,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   204,    -1,   436,
     294,    -1,    -1,    -1,    -1,   442,   443,   444,   215,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   455,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   463,    -1,    -1,    -1,
      -1,   468,    -1,    -1,   471,   472,   473,    -1,    -1,    -1,
      -1,    -1,   336,    -1,   481,    -1,    -1,    -1,    -1,   256,
      -1,   258,    -1,    -1,   261,    -1,   263,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   276,
      -1,    -1,   366,   139,    -1,    -1,    -1,   143,   144,   145,
     146,   147,   148,   149,   150,   151,    -1,   294,   154,   155,
     156,    -1,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,    -1,    -1,    -1,    -1,    -1,    -1,   402,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   413,
     414,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   336,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   436,    -1,    -1,    -1,    -1,    -1,   442,   443,
     444,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   366,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   463,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   471,    -1,   473,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   481,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   402,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   413,   414,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   283,   284,   285,
     286,   287,   288,    -1,    -1,    -1,    -1,    -1,    -1,   436,
      -1,    -1,    -1,    -1,    -1,   442,   443,   444,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   463,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   471,    -1,   473,    -1,    -1,    -1,
      -1,   139,    -1,    -1,   481,   143,   144,   145,   146,   147,
     148,   149,   150,   151,    -1,    -1,   154,   155,   156,    -1,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     139,    -1,    -1,    -1,   143,   144,   145,   146,   147,   148,
     149,   150,   151,    -1,    -1,   154,   155,   156,    -1,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   470,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   283,   284,   285,   286,   287,
     288,    -1,   488,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   283,   284,   285,   286,   287,   288,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   470,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     488,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   470,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   488
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   497,   498,     0,   203,   331,   499,   500,   507,   488,
     511,   511,   500,   518,   510,   508,    23,    93,   169,   190,
     228,   236,   271,   295,   359,   374,   383,   401,   403,   501,
     519,   520,   521,   523,   524,   525,   530,   538,   543,   577,
     578,   579,   580,   581,   610,   611,   612,   613,   614,   615,
     617,   621,   133,   256,   332,   513,   513,   102,   344,   354,
     477,   622,   380,   132,   488,   380,   488,   434,  1245,   488,
     488,   341,   398,   399,   616,   282,   303,  1225,   488,   488,
     623,   626,   521,   524,   578,   580,   611,   488,   614,    28,
     514,   514,   432,  1277,   300,  1259,   488,   488,   488,   423,
    1273,   265,   330,   382,   473,   528,   529,   531,   532,   533,
     534,   536,   537,  1263,   202,   539,   540,   541,    25,  1235,
     303,   473,  1184,   472,   526,   527,   528,  1279,    12,    69,
     105,   106,   108,   116,   181,   257,   292,   376,   391,   419,
     473,   544,   545,   546,   547,   548,   549,   553,   562,   567,
     570,   571,   572,   573,   574,   575,   576,   502,   110,   189,
     255,   259,   378,   434,   475,   624,   625,   627,   628,   629,
     632,   660,   712,   714,   256,   511,   509,   473,  1201,  1202,
    1204,   473,  1172,  1173,   522,    95,  1242,   396,   234,  1250,
     488,   532,   473,   488,   533,    73,   387,  1231,    10,   473,
     542,   488,   541,   196,  1246,   582,   488,   488,   527,   115,
    1184,  1184,   409,   391,  1269,  1250,  1250,  1250,  1184,   391,
    1270,  1250,   234,    67,  1241,   256,  1250,   488,   546,   243,
     439,   327,   770,   132,   380,   380,   380,   380,   356,   716,
     629,   630,   515,  1250,   220,   297,  1228,   133,   490,  1205,
    1206,  1173,   618,   619,  1173,  1250,   256,  1207,   488,   387,
    1250,   231,   231,   473,  1249,  1172,   583,   488,   269,   554,
    1250,  1250,  1250,  1181,  1204,    78,  1181,  1250,   234,  1181,
     563,   564,   565,  1184,  1250,   105,  1184,  1185,   386,  1227,
    1227,   132,   500,   503,   511,   488,   488,   488,   511,   380,
     375,   758,   631,   633,  1173,   511,    82,   187,   345,   435,
     516,   517,  1204,    10,    39,    92,   107,   126,   130,   204,
     215,   245,   252,   256,   258,   261,   263,   276,   294,   336,
     366,   402,   413,   414,   436,   442,   443,   444,   463,   471,
     481,   482,   483,   490,  1162,  1163,  1166,  1167,  1190,  1201,
    1210,  1211,  1212,   490,  1206,   619,   320,   620,  1207,  1181,
       5,    20,    30,    42,   222,   253,   260,   306,   312,   341,
     349,   361,   390,   398,   434,   488,   584,   585,   589,   591,
     592,   593,   594,   595,   599,   600,   601,   602,   603,   604,
     606,   607,   608,  1231,  1238,  1250,   568,   569,  1211,  1181,
     256,  1181,   563,    24,   234,  1184,  1251,  1184,   550,    66,
    1240,   268,   829,  1175,   153,   157,   500,   504,   715,   713,
     661,   488,   380,   662,   663,   634,   517,  1263,  1166,   490,
     490,  1213,   490,   490,  1214,   297,  1258,  1228,   490,   490,
     490,   490,   490,   490,   490,   490,   490,   490,   490,  1213,
    1166,  1166,  1166,    80,   384,   491,  1165,    38,    40,    41,
     482,   483,   484,   485,   487,   494,   495,  1166,  1207,    67,
     474,   535,   269,  1256,   341,  1265,  1277,     8,   388,  1256,
    1250,  1240,   121,   239,  1238,  1252,  1252,   281,  1207,  1279,
     409,  1250,   388,   126,   138,   215,   256,   263,   277,   336,
     402,   405,   406,   481,   555,   556,   557,   560,   569,   428,
     566,  1207,   551,    64,   466,   773,   506,   505,   323,   992,
     994,   995,   662,   662,   662,   717,   488,   664,  1219,   635,
    1164,  1166,  1166,  1164,  1164,  1215,  1202,  1210,  1212,   473,
    1166,  1218,  1166,  1164,  1166,  1217,  1166,  1164,  1164,  1166,
    1216,  1166,  1166,  1166,   491,  1163,  1166,  1166,  1166,  1166,
    1166,  1166,  1166,  1166,  1166,   491,  1166,  1211,  1250,  1252,
     136,   187,   587,  1250,   253,   341,   349,   601,  1250,  1250,
     388,  1250,  1250,  1235,    10,   281,   339,   609,  1250,   473,
     557,    18,   428,  1211,  1207,   298,   300,   552,   775,   774,
     364,   782,   513,  1264,  1264,   993,   992,   511,   338,   718,
     719,   720,   759,  1219,   256,   473,   665,   668,   673,    53,
      72,   110,   240,   251,   341,   342,   356,   358,   467,   488,
     636,   637,   639,   643,   646,   649,   651,   657,   658,   659,
    1250,   491,  1165,   494,   491,   491,  1165,   491,   491,   491,
    1165,   491,   491,   491,   491,  1165,   491,   491,   491,   491,
     491,   136,   337,   388,   590,  1250,   127,   253,   325,   586,
     599,    33,   183,   264,   596,   256,  1181,  1183,   405,   473,
     605,  1181,  1184,  1181,   307,  1262,  1262,   301,  1181,   558,
     560,   409,  1272,    55,   776,   777,   778,  1239,   776,  1204,
     488,   488,   488,   256,   473,   757,   720,   760,   665,   192,
     473,   669,   670,   488,   512,  1242,  1250,   650,   341,   343,
     645,  1229,  1250,  1242,  1250,  1256,  1250,    24,  1234,   297,
     187,   208,  1213,  1166,   491,  1166,  1213,  1181,  1213,  1213,
    1213,  1166,  1213,  1213,  1213,   243,   439,  1213,  1213,  1213,
    1213,   605,   128,   256,   588,  1204,   472,   597,   597,   135,
     472,  1221,  1230,   400,   489,  1181,  1182,   126,   215,   256,
     263,   336,   402,   481,   559,   561,  1250,   777,   396,   450,
     780,   124,   348,   467,   771,   256,  1200,   721,   668,   673,
     761,   762,   763,   666,    94,   488,  1207,   473,  1229,  1234,
    1250,  1234,  1250,   299,   404,   644,  1183,  1207,   468,  1250,
     757,   757,   191,   216,   473,   647,   491,  1221,   260,   371,
     135,  1250,  1180,  1181,    18,  1184,  1250,   396,   303,   781,
     117,   783,    71,    98,    99,   313,   488,   722,   723,   732,
    1250,   669,   763,   674,   671,  1250,   432,   642,  1180,   254,
    1255,   432,  1241,   220,  1248,   473,  1250,  1250,   300,  1180,
    1181,   561,    32,   118,  1207,  1250,   473,   488,   772,  1250,
    1250,  1234,   249,   250,   726,   736,   208,   764,    22,    35,
      42,    43,    44,    45,    46,    47,    48,    49,    83,    85,
      86,    87,    88,    89,    90,    91,   128,   129,   221,   237,
     248,   274,   296,   311,   317,   319,   333,   346,   353,   393,
     394,   395,   420,   451,   452,   453,   464,   467,   667,   675,
     676,   677,   678,   680,   681,   682,   683,   684,   685,   698,
     699,   701,   702,   705,   710,   711,  1250,  1270,    28,  1236,
     208,  1207,    67,   343,   638,   652,  1207,   396,  1271,   256,
     648,  1204,   648,   271,   341,   343,   598,  1207,   784,   785,
    1196,  1201,  1211,  1212,   193,   724,   725,   724,  1251,   668,
     737,   765,   245,   392,   450,   392,   450,   392,   450,   392,
     450,   392,   450,   470,  1278,   370,  1267,  1201,  1207,  1202,
    1204,   244,   370,   700,  1250,  1251,   512,   187,   208,   446,
     243,   439,    10,    56,   245,   672,  1209,  1210,   653,   654,
     655,   656,  1255,  1279,  1241,   200,   641,  1247,   598,   785,
       1,   473,   488,   786,   787,   788,   789,   790,   791,   797,
     798,   992,   731,   735,  1207,   473,   669,    32,    34,    37,
      50,    51,    52,    76,    98,   177,   197,   200,   201,   214,
     253,   261,   262,   281,   296,   310,   334,   360,   367,   381,
     396,   421,   432,   447,   463,   466,   467,   488,   681,   682,
     684,   698,   701,   703,   707,   708,   766,   769,  1201,  1209,
     481,   432,   689,   428,   473,   683,   707,    28,   679,  1227,
    1227,  1211,  1258,  1258,    31,  1237,   195,   642,  1207,   142,
     488,   473,   488,   380,     4,     6,    11,    19,    57,    58,
      63,    68,    70,    81,    84,    97,   119,   128,   131,   168,
     180,   184,   185,   199,   206,   209,   210,   217,   224,   226,
     229,   266,   270,   272,   280,   302,   316,   339,   340,   350,
     362,   363,   369,   371,   379,   389,   398,   407,   411,   412,
     415,   418,   424,   441,   449,   454,   465,   476,   478,   488,
     799,   800,   810,   815,   819,   823,   825,   847,   850,   858,
     863,   868,   869,   870,   874,   880,   884,   892,   907,   912,
     921,   923,   925,   928,   931,   937,   946,   948,   965,   967,
     970,   974,   981,   992,   996,  1004,  1005,  1006,  1008,  1011,
    1015,  1016,  1027,  1038,  1048,  1054,  1058,  1064,  1068,  1070,
    1072,  1074,  1077,  1088,  1089,  1098,  1100,  1101,  1142,   799,
    1201,   733,    75,    77,   253,   254,   727,   728,   738,  1250,
     290,  1257,  1250,   172,   174,  1250,  1190,  1257,    37,  1207,
    1240,  1250,  1201,  1201,  1251,   708,   428,  1207,   431,  1276,
    1204,   706,   256,  1202,  1202,    54,   438,  1255,  1241,   117,
     256,   792,   801,   811,   816,   820,   824,   848,   826,   851,
     859,   864,   434,   871,   875,   881,   885,   893,   908,   913,
     922,   924,  1277,   929,   932,   938,   947,   949,   966,   968,
     971,   385,   975,   982,   997,   440,  1007,   440,  1009,  1012,
    1017,  1028,  1039,  1049,  1055,  1059,  1065,   326,  1069,  1071,
    1073,  1075,  1078,   208,  1090,  1246,  1102,  1143,   213,   241,
     734,  1207,    74,    75,    76,    77,   212,   253,   254,   280,
     322,   400,   417,   445,   468,   488,   681,   682,   684,   685,
     698,   701,   705,   739,   740,   742,   743,   744,   745,   747,
     748,   749,   750,   754,   755,   481,  1198,  1201,  1207,  1250,
    1201,  1198,  1250,  1276,  1250,  1198,   707,  1209,    29,   122,
     123,   222,   686,   687,   688,   690,   691,   692,   693,   694,
     695,  1278,  1183,  1183,  1237,   122,   640,   488,   488,   802,
    1201,     7,   103,   245,   812,  1167,  1188,  1189,  1201,  1209,
    1212,   473,   817,  1152,  1153,  1154,   821,   829,   849,   829,
     852,  1195,  1196,   860,   865,  1168,  1169,  1189,   872,  1173,
      31,   458,   803,   876,   877,   878,  1189,  1237,  1279,   882,
    1189,   256,   433,   437,   894,   895,  1152,   314,   316,   330,
     380,   909,    65,   273,   916,   917,     7,  1186,  1187,  1201,
    1201,   926,   207,   364,   930,  1266,     1,   933,  1151,  1152,
    1186,   731,   950,  1201,  1209,  1212,  1040,  1204,   103,   969,
    1189,   972,  1189,   976,   198,   256,   473,   983,   986,   987,
     988,  1177,  1178,  1179,  1190,  1203,  1279,  1173,  1170,  1204,
    1173,  1170,    10,  1018,  1171,  1204,   169,  1029,  1030,  1031,
    1032,  1034,  1035,  1036,  1037,  1174,  1175,  1186,  1040,  1173,
       3,   256,   373,  1056,   120,  1060,  1061,  1189,   103,  1066,
    1188,   731,  1201,  1173,  1196,     9,    36,  1092,   115,  1170,
     206,   315,  1144,  1250,    62,    98,   114,   125,   729,   730,
     728,   290,   291,   291,  1257,   223,   756,   290,   291,   753,
    1234,   212,   470,  1250,  1258,  1250,  1201,   745,   280,   318,
     751,   752,  1207,   267,   318,   482,   483,   768,   267,   318,
     482,   483,   767,  1200,   704,  1259,  1239,   688,   692,  1252,
     389,  1268,  1255,  1255,  1183,  1259,   143,   809,   200,   430,
     803,   806,   807,   808,  1279,  1258,  1201,  1258,   144,   814,
     432,   813,  1189,   225,  1222,    67,    13,    14,    15,    21,
      38,    39,    40,    41,   175,   176,   188,   205,   211,   234,
     242,   247,   278,   279,   282,   292,   299,   304,   321,   416,
     482,   483,   484,   485,   487,   489,   490,   491,   492,   493,
    1155,  1156,  1157,  1158,  1159,  1189,   256,   473,  1177,  1196,
    1196,  1196,   853,  1173,   146,   866,   175,   489,   867,  1169,
     372,  1226,  1173,  1265,  1193,  1201,  1210,  1212,   105,   325,
     473,  1175,   149,   879,   182,   878,  1109,  1111,  1189,   459,
     460,   461,   462,    76,   253,   320,   804,   805,    34,    37,
      50,    51,    52,   100,   177,   197,   214,   262,   281,   289,
     310,   367,   377,   447,   150,   883,    55,   230,   829,    18,
     470,   896,   897,   898,   901,   109,   910,   915,   917,  1189,
    1258,  1187,  1277,  1176,  1189,   427,  1275,   939,  1279,   101,
     355,   422,   951,   952,   953,   957,   962,  1042,  1189,   432,
     155,   973,    55,   186,   227,   235,   308,   977,  1228,   987,
     156,   984,   985,   455,   468,   428,  1228,   431,   425,   280,
     324,  1223,   200,  1103,  1265,  1103,  1171,   161,  1026,   468,
    1020,  1193,  1035,  1037,  1186,   432,  1175,   134,   432,   456,
    1033,   219,   999,  1279,   207,   364,  1057,  1189,  1057,  1239,
     230,  1061,  1201,   164,  1067,   200,   200,   341,   343,  1076,
     120,  1079,   357,   404,  1093,  1259,  1103,  1201,  1196,  1110,
    1111,  1207,   213,  1250,  1250,  1250,  1251,  1250,  1151,  1201,
     746,  1181,    98,   125,   313,   356,   741,   200,   313,  1207,
     752,   256,   481,  1197,  1199,  1201,  1208,  1197,  1278,  1181,
     473,   696,   697,  1250,  1277,  1255,  1181,    26,    27,    60,
      61,    77,    79,   111,   112,   113,   169,   171,   179,   254,
     429,   473,  1175,  1092,  1108,  1111,   808,    32,    34,    37,
      52,   100,   137,   177,   197,   201,   214,   261,   262,   281,
     310,   334,   360,   367,   377,   381,   421,   447,   457,   463,
     168,   330,  1191,  1202,   432,  1202,  1210,  1212,  1168,  1189,
     207,   364,   818,  1222,  1277,   426,  1274,  1274,   205,   242,
     278,  1157,  1158,  1159,  1156,  1228,   432,   408,   410,   472,
     830,   830,    97,   184,   855,   347,   448,   861,   862,  1279,
    1152,   148,   873,   233,   286,  1136,  1137,  1138,  1140,  1112,
     285,  1113,  1109,  1109,  1109,  1109,  1257,  1257,  1257,  1250,
     172,   174,  1250,    37,   134,   456,  1189,  1168,  1189,   466,
     886,   887,   895,    22,   307,   433,   437,   902,   903,   904,
    1153,   151,   906,   898,   470,   899,   911,  1266,   458,   918,
     919,  1189,  1279,  1202,  1187,   122,   927,  1177,   934,    10,
      13,    16,    17,   274,   275,   292,   293,   940,   944,   192,
      10,  1233,    10,    67,   194,   243,   439,   958,   959,   960,
     954,   952,  1044,  1230,  1259,   432,  1186,  1168,  1189,   390,
     978,  1203,   793,   794,   184,   989,  1151,   990,   991,  1201,
    1177,  1179,     9,    36,  1105,  1265,  1190,   230,   998,  1013,
    1279,  1021,  1237,  1201,  1021,   432,   432,   552,   168,   433,
     437,  1189,    55,   260,  1050,   218,   238,   260,   281,   469,
    1189,  1189,   396,  1189,  1201,   200,  1168,  1189,  1193,  1239,
     230,  1082,  1201,   178,   182,  1094,    10,  1099,  1177,  1013,
     200,  1279,   167,  1147,  1113,   194,  1207,  1207,  1207,  1207,
    1226,   195,   213,   195,   213,   195,   213,  1189,  1268,   697,
    1180,   433,   709,  1108,   480,   479,  1193,  1108,   239,  1198,
    1113,  1250,   172,   174,  1250,    37,    66,   134,   456,   256,
    1192,  1202,  1201,   397,  1116,  1117,  1168,  1187,   364,   329,
     822,    59,   410,   831,   255,  1253,   408,   832,   827,   466,
     833,   834,   364,   472,   856,  1196,   854,  1246,   260,   281,
    1116,  1139,  1141,   873,  1140,  1115,  1201,  1114,  1190,  1190,
    1190,  1198,  1198,  1150,  1239,  1150,   207,  1116,   207,   888,
     364,   891,    18,   428,   905,   902,   793,   900,  1189,   105,
     325,   473,  1175,   914,   919,   289,  1259,   139,   154,   793,
     935,  1277,   355,   941,  1277,  1193,    55,   959,   961,  1193,
      10,    67,   243,   439,   955,   956,  1193,  1045,  1231,   220,
     305,  1261,   694,  1186,  1116,   207,  1279,  1172,   985,   795,
       9,   200,   219,   472,  1000,  1201,   142,   283,  1126,  1127,
    1129,  1136,   260,   281,   470,   142,   470,  1023,  1024,  1193,
    1192,  1189,   239,  1051,   260,   260,   260,  1062,  1279,  1201,
    1116,   207,   432,    10,  1080,  1081,  1220,  1083,  1201,  1062,
    1083,   327,  1097,   328,  1104,  1105,  1196,   141,  1149,   729,
     195,    55,  1277,  1250,  1108,  1198,  1198,  1250,  1150,  1150,
    1226,  1118,   288,  1119,  1116,  1187,   432,  1177,  1253,   831,
     182,   207,   309,   364,   828,   834,   839,   840,   841,  1095,
     835,   841,  1196,  1196,   207,   857,   352,   368,   793,   793,
     793,   793,    78,  1243,  1243,  1243,  1198,  1168,  1169,  1168,
    1169,   838,   889,   890,  1239,  1204,   904,  1152,   152,   920,
    1196,   154,   793,   139,   154,   936,   467,   942,   943,   944,
     118,   945,   467,   432,  1193,    55,   963,   956,   196,   963,
    1041,  1250,   305,  1252,  1168,   609,   365,   980,   797,   991,
    1189,   260,   218,   238,   260,   281,   469,   998,  1128,  1130,
     159,  1010,  1129,   160,  1014,   260,  1152,  1022,  1151,  1023,
    1250,  1136,   309,  1121,  1122,   319,  1226,  1168,  1194,  1201,
    1209,  1212,   304,  1260,  1193,   121,  1084,   422,  1086,  1259,
     173,   284,  1106,  1131,  1132,  1134,  1137,     8,  1232,   104,
    1146,  1196,   840,  1148,  1250,  1250,  1189,   709,  1209,   256,
    1116,   793,  1120,  1253,     7,   230,  1201,   285,   287,   843,
    1096,   842,   836,   837,   838,   843,  1201,   253,   254,  1254,
    1116,   351,  1116,   351,   299,   780,   890,    96,   124,   348,
     467,   936,   154,   793,   943,   110,  1244,  1194,   963,  1194,
    1105,   227,   466,  1046,  1181,  1043,  1116,   979,  1279,    55,
     260,   260,   260,   999,   793,   793,  1019,   793,  1025,   282,
    1052,  1224,   162,  1053,  1123,   163,  1063,   287,  1124,  1201,
    1116,  1116,  1081,  1248,   104,  1085,  1248,  1121,   186,   227,
     235,   308,  1091,  1172,  1133,  1135,   166,  1107,  1134,   313,
    1175,  1198,  1248,   274,   327,   335,  1145,  1207,  1207,   793,
    1258,  1201,   145,   845,   844,   793,   837,   299,   780,   147,
     846,  1169,  1169,    22,  1204,   936,    55,   963,   963,   435,
     964,   327,  1172,   207,   308,  1047,  1204,   260,   281,  1189,
     239,  1001,   793,   796,   796,  1189,   205,   211,   242,   247,
    1157,  1158,  1159,  1160,  1161,   793,  1125,  1201,  1248,  1201,
     165,  1087,   793,   793,  1254,  1201,   327,  1250,  1201,   793,
    1189,  1116,  1116,   780,   780,  1189,  1189,  1250,  1172,   327,
     368,   455,  1250,  1002,  1126,  1137,  1274,  1274,   793,  1201,
     986,   780,   120,   779,   986,  1250,  1151,  1201,   158,  1003,
     304,   304,   779,   986,   175,   175,  1277,  1277
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 900 "parser.y"
    {
        perform_stack = NULL;
        current_statement = NULL;
        /*CIT*/
        current_paragraph = NULL;
        current_section = NULL;

        next_label_id = 0;
        current_linage = 0;
        current_storage = 0;
        eval_level = 0;
        eval_inc = 0;
        eval_inc2 = 0;
        prog_end = 0;
        depth = 0;
        inspect_keyword = 0;
        check_unreached = 0;
        samearea = 1;
        memset ((char *)eval_check, 0, sizeof(eval_check));
        memset ((char *)term_array, 0, sizeof(term_array));
        linage_file = NULL;
        next_label_list = NULL;
        current_program = cb_build_program (NULL, 0);
        external_program_list = NULL;
        cb_build_registers ();
        current_program->flag_main = cb_flag_main;
  ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 929 "parser.y"
    {
    if (!current_program->flag_validated) {
        current_program->flag_validated = 1;
                cb_validate_program_body (current_program);
        }
    if (depth > 1) {
                cb_error (_("Multiple PROGRAM-ID's without matching END PROGRAM"));
        }
        if (errorcount > 0) {
                YYABORT;
        }
        if (!current_program->entry_list) {
                emit_entry (current_program->program_id, 0, NULL, 0, 1);
        }
  ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 953 "parser.y"
    { cb_validate_program_environment (current_program); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 954 "parser.y"
    { cb_validate_program_data (current_program); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 959 "parser.y"
    {
    if (!current_program->flag_validated) {
        current_program->flag_validated = 1;
        cb_validate_program_body (current_program);
    }
  ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 974 "parser.y"
    {
    cb_force_pid_literal = 1;
  ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 978 "parser.y"
    {
    char                    *s;

    if ((yyvsp[(3) - (4)])) {
        if (CB_LITERAL_P ((yyvsp[(3) - (4)]))) {
                s = (char *)(CB_LITERAL ((yyvsp[(3) - (4)]))->data);
        } else {
                s = (char *)(CB_NAME ((yyvsp[(3) - (4)])));
        }
        if (depth) {
                depth--;
        }
        if (strcasecmp (stack_progid[depth], s)) {
            cb_error (_("END PROGRAM '%s' is different to PROGRAM-ID '%s'"),
                s, stack_progid[depth]);
        }
    }
  ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 997 "parser.y"
    {
    cb_force_pid_literal = 1;
  ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 1001 "parser.y"
    {
    char            *s;

    if ((yyvsp[(3) - (4)])) {
        if (CB_LITERAL_P ((yyvsp[(3) - (4)]))) {
            s = (char *)(CB_LITERAL ((yyvsp[(3) - (4)]))->data);
        } else {
            s = (char *)(CB_NAME ((yyvsp[(3) - (4)])));
        }
        if (depth) {
            depth--;
        }
        if (strcmp (stack_progid[depth], s)) {
            cb_error (_("END FUNCTION '%s' is different to FUNCTION-ID '%s'"),
                s, stack_progid[depth]);
        }
    }
  ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 1028 "parser.y"
    {
    cb_force_pid_literal = 1;
  ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 1032 "parser.y"
    {
        char LocBuffer[1024];
        current_section = NULL;
        current_paragraph = NULL;
        if (CB_LITERAL_P ((yyvsp[(4) - (5)]))) {
            stack_progid[depth] = (char *)(CB_LITERAL ((yyvsp[(4) - (5)]))->data);
            } else {
            stack_progid[depth] = (char *)(CB_NAME ((yyvsp[(4) - (5)])));
        }
        if (prog_end) {
            if (!current_program->flag_validated) {
                current_program->flag_validated = 1;
                cb_validate_program_body (current_program);
            }
            perform_stack = NULL;
            current_statement = NULL;
            next_label_id = 0;
            current_linage = 0;
            current_storage = 0;
            eval_level = 0;
            inspect_keyword = 0;
            check_unreached = 0;
            eval_inc = 0;
            eval_inc2 = 0;
            samearea = 1;
            memset ((char *)eval_check, 0, sizeof(eval_check));
            memset ((char *)term_array, 0, sizeof(term_array));
            linage_file = NULL;
            next_label_list = NULL;
            if (current_program && current_program->flag_is_external) {
                struct cb_program *n =current_program->next_program;
                current_program->next_program = external_program_list;
                external_program_list = current_program;
                current_program = cb_build_program (n, depth);
            } else {
                current_program = cb_build_program (current_program, depth);
            }
            cb_build_registers ();
        } else {
          prog_end = 1;
        }
    depth++;
    current_program->program_id = cb_build_program_id ((yyvsp[(4) - (5)]), (yyvsp[(5) - (5)]));
    if (source_demangle_name && (strcasecmp(source_demangle_name, current_program->program_id) != 0)) {                
      sprintf(LocBuffer, "%s_%s_", current_program->program_id, source_demangle_name);
    } else {
      sprintf(LocBuffer, "%s_", current_program->program_id);
    }
    cb_disable_runtime_check=0;
    current_program->main_entry_name = (char*)strdup(LocBuffer);
    if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] PROGRAM_ID %s\n", current_program->program_id );
    current_program->source_file =  strdup((char*)cb_lex_source_file.file_name);
    current_program->source_line = cb_lex_source_file.line;
  ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 1091 "parser.y"
    {
    cb_force_pid_literal = 1;
  ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 1095 "parser.y"
    {
    cb_error (_("FUNCTION-ID is not yet implemented"));
    current_section = NULL;
    current_paragraph = NULL;
    if (CB_LITERAL_P ((yyvsp[(4) - (6)]))) {
        stack_progid[depth] = (char *)(CB_LITERAL ((yyvsp[(4) - (6)]))->data);
    } else {
        stack_progid[depth] = (char *)(CB_NAME ((yyvsp[(4) - (6)])));
    }
    if (prog_end) {
        if (!current_program->flag_validated) {
            current_program->flag_validated = 1;
            cb_validate_program_body (current_program);
        }
        perform_stack = NULL;
        current_statement = NULL;
        next_label_id = 0;
        current_linage = 0;
        current_storage = 0;
        eval_level = 0;
        inspect_keyword = 0;
        check_unreached = 0;
        eval_inc = 0;
        eval_inc2 = 0;
        samearea = 1;
        memset ((char *)eval_check, 0, sizeof(eval_check));
        memset ((char *)term_array, 0, sizeof(term_array));
        linage_file = NULL;
        next_label_list = NULL;
        current_program = cb_build_program (current_program, depth);
        cb_build_registers ();
    } else {
        prog_end = 1;
    }
    depth++;
    cb_disable_runtime_check=0;
    current_program->program_id = cb_build_program_id ((yyvsp[(4) - (6)]), (yyvsp[(5) - (6)]));
    current_program->prog_type = CB_FUNCTION_TYPE;
    current_program->flag_recursive = 1;
    current_program->flag_initial = 1;
    current_program->source_file =  strdup((char*)cb_lex_source_file.file_name);
    current_program->source_line = cb_lex_source_file.line;
  ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 1151 "parser.y"
    {cb_force_pid_literal = 0;;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 1152 "parser.y"
    {
                   const char *p  = CB_REFERENCE (yylval)->word->name;
                   cb_force_pid_literal = 0;
                   (yyval) = cb_build_alphanumeric_literal ((const unsigned char *)p, strlen(p), 0);
                   ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 1160 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 1161 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 1170 "parser.y"
    {
    if (!current_program->nested_level) {
        cb_error (_("COMMON may only be used in a nested program"));
    }
    current_program->flag_common = 1;
  ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 1177 "parser.y"
    {
    if (!current_program->nested_level) {
        cb_error (_("COMMON may only be used in a nested program"));
    }
    current_program->flag_common = 1;
  ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 1184 "parser.y"
    {
    current_program->flag_is_external = 1;
  ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 1192 "parser.y"
    {
    current_program->flag_initial = 1;
  ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 1196 "parser.y"
    {
    current_program->flag_recursive = 1;
    current_program->flag_initial = 1;
  ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 1211 "parser.y"
    {
                                   if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] ENVIRONMENT DIVISION\n");
                           ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 1227 "parser.y"
    {
                                   if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] CONFIGURATION SECTION\n");
                            ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 1230 "parser.y"
    {
    if (current_program->nested_level) {
        cb_error (_("CONFIGURATION SECTION not allowed in nested programs"));
    }
  ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 1236 "parser.y"
    {
    if (current_program->nested_level) {
        cb_error (_("CONFIGURATION SECTION not allowed in nested programs"));
    }
  ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 1273 "parser.y"
    {
        cb_verify (cb_debugging_line, "DEBUGGING MODE");
  ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 1279 "parser.y"
    { ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 1283 "parser.y"
    { ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 1284 "parser.y"
    { ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 1314 "parser.y"
    {
        cb_verify (cb_memory_size_clause, "MEMORY SIZE");
  ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 1326 "parser.y"
    {
    current_program->collating_sequence = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 1333 "parser.y"
    {
    /* Ignore */
  ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 1358 "parser.y"
    {
    current_program->function_spec_list = (yyvsp[(2) - (3)]);
  ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 1362 "parser.y"
    {
    functions_are_all = 1;
  ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 1368 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 1370 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1411 "parser.y"
    {
        current_program->flag_sign_separate = CB_INTEGER ((yyvsp[(3) - (3)]))->val;
        current_program->flag_sign_leading  = 1;
  ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1416 "parser.y"
    {
        current_program->flag_sign_separate = CB_INTEGER ((yyvsp[(3) - (3)]))->val;
        current_program->flag_sign_leading  = 0;
  ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1426 "parser.y"
    {
    save_tree_1 = lookup_system_name (CB_NAME ((yyvsp[(1) - (3)])));
    if (save_tree_1 == cb_error_node) {
        cb_error_x ((yyvsp[(1) - (3)]), _("Unknown system-name '%s'"), CB_NAME ((yyvsp[(1) - (3)])));
    } else if (CB_SYSTEM_NAME(save_tree_1)->token != COB_DEVICE_CONSOLE) {
        cb_error_x (save_tree_1, _("Invalid CRT clause"));
    }
        /* current_program->flag_screen = 1; */
  ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1436 "parser.y"
    {
    save_tree_1 = lookup_system_name (CB_NAME ((yyvsp[(1) - (3)])));
    if (save_tree_1 == cb_error_node) {
        cb_error_x ((yyvsp[(1) - (3)]), _("Unknown system-name '%s'"), CB_NAME ((yyvsp[(1) - (3)])));
        save_tree_2 = cb_error_node;
    } else {
        if ((yyvsp[(3) - (3)])) {
            save_tree_2 = (yyvsp[(3) - (3)]);
            cb_define ((yyvsp[(3) - (3)]), save_tree_1);
        } else {
            save_tree_2 = (yyvsp[(1) - (3)]);
        }
    }
  ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1463 "parser.y"
    {
    save_tree_1 = lookup_system_name (CB_NAME ((yyvsp[(1) - (4)])));
    if (save_tree_1 == cb_error_node) {
        cb_error_x ((yyvsp[(1) - (4)]), _("Unknown system-name '%s'"), CB_NAME ((yyvsp[(1) - (4)])));
    } else if (CB_SYSTEM_NAME(save_tree_1)->category != CB_CALL_CONVENTION_NAME) {
        cb_error_x (save_tree_1, _("Invalid special-names clause"));
    }
    cb_define ((yyvsp[(4) - (4)]), cb_build_system_name(CB_SYSTEM_NAME(save_tree_1)->category, cb_get_int((yyvsp[(2) - (4)]))));
  ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 1477 "parser.y"
    {
        if ((save_tree_1 != cb_error_node) && (save_tree_2 != cb_error_node))
            cb_define_switch_name ((yyvsp[(5) - (5)]), save_tree_1, (yyvsp[(2) - (5)]), save_tree_2);
    ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 1484 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 1485 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1493 "parser.y"
    {
        save_tree_1 = (yyvsp[(2) - (2)]);
  ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1497 "parser.y"
    {
        current_program->alphabet_name_list =
        cb_list_add (current_program->alphabet_name_list, (yyvsp[(5) - (5)]));
  ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1504 "parser.y"
    { (yyval) = cb_build_alphabet_name (save_tree_1, CB_ALPHABET_NATIVE); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1505 "parser.y"
    { (yyval) = cb_build_alphabet_name (save_tree_1, CB_ALPHABET_STANDARD_1); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1506 "parser.y"
    { (yyval) = cb_build_alphabet_name (save_tree_1, CB_ALPHABET_STANDARD_2); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1507 "parser.y"
    { (yyval) = cb_build_alphabet_name (save_tree_1, CB_ALPHABET_EBCDIC); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1509 "parser.y"
    {
    (yyval) = cb_build_alphabet_name (save_tree_1, CB_ALPHABET_CUSTOM);
        CB_ALPHABET_NAME ((yyval))->custom_list = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1516 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1518 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1522 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1523 "parser.y"
    { (yyval) = cb_build_pair ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1525 "parser.y"
    {
    (yyval) = cb_list_init ((yyvsp[(1) - (2)]));
    save_tree_2 = (yyval);
  ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1530 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (4)]);
  ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1541 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1542 "parser.y"
    { (yyval) = cb_space; ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1543 "parser.y"
    { (yyval) = cb_zero; ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 1544 "parser.y"
    { (yyval) = cb_quote; ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 1545 "parser.y"
    { (yyval) = cb_dirsep; ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 1546 "parser.y"
    { (yyval) = cb_norm_high; ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1547 "parser.y"
    { (yyval) = cb_norm_low; ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 1551 "parser.y"
    { cb_list_add (save_tree_2, (yyvsp[(1) - (1)])); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 1552 "parser.y"
    { cb_list_add (save_tree_2, cb_space); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 1553 "parser.y"
    { cb_list_add (save_tree_2, cb_zero); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 1554 "parser.y"
    { cb_list_add (save_tree_2, cb_quote); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 1555 "parser.y"
    { cb_list_add (save_tree_2, cb_dirsep); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 1556 "parser.y"
    { cb_list_add (save_tree_2, cb_norm_high); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 1557 "parser.y"
    { cb_list_add (save_tree_2, cb_norm_low); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 1565 "parser.y"
    {
        /*PENDING ("SYMBOLIC CHARACTERS");*/
  ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1572 "parser.y"
    {
        if ((yyvsp[(1) - (1)])) {
            current_program-> symbolic_list =
                cb_list_append (current_program->symbolic_list, (yyvsp[(1) - (1)]));
        }
  ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1579 "parser.y"
    {
        if ((yyvsp[(1) - (2)])) {
            current_program-> symbolic_list =
                cb_list_append (current_program->symbolic_list, (yyvsp[(1) - (2)]));
        }
  ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1589 "parser.y"
    {
    if (cb_list_length ((yyvsp[(1) - (3)])) != cb_list_length ((yyvsp[(3) - (3)]))) {
        cb_error (_("Invalid SYMBOLIC clause"));
        (yyval) = NULL;
    } else {
        (yyval) = cb_list_join((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
    }
  ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1600 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 1601 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1605 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1606 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1614 "parser.y"
    {
        current_program->class_name_list =
                        cb_list_add (current_program->class_name_list,
                        cb_build_class_name ((yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])));
  ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 1622 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1623 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1627 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1629 "parser.y"
    {
        /* if (CB_LITERAL ($1)->data[0] < CB_LITERAL ($3)->data[0]) */
        if (literal_value ((yyvsp[(1) - (3)])) < literal_value ((yyvsp[(3) - (3)]))) {
                (yyval) = cb_build_pair ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
        } else {
                (yyval) = cb_build_pair ((yyvsp[(3) - (3)]), (yyvsp[(1) - (3)]));
        }
  ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 1643 "parser.y"
    {
        cb_tree l;

        l = cb_build_locale_name ((yyvsp[(2) - (4)]), (yyvsp[(4) - (4)]));
        if (l != cb_error_node) {
                current_program->locale_list =
                        cb_list_add (current_program->locale_list, l);
        }
  ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1658 "parser.y"
    {
        unsigned char *s = CB_LITERAL ((yyvsp[(4) - (4)]))->data;

        if (CB_LITERAL ((yyvsp[(4) - (4)]))->size != 1) {
                cb_error_x ((yyvsp[(4) - (4)]), _("Invalid currency sign '%s'"), s);
        }
        switch (*s) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'c':
        case 'C':
        case 'd':
        case 'D':
        case 'e':
        case 'E':
        case 'n':
        case 'N':
        case 'p':
        case 'P':
        case 'r':
        case 'R':
        case 's':
        case 'S':
        case 'v':
        case 'V':
        case 'x':
        case 'X':
        case 'z':
        case 'Z':
        case '+':
        case '-':
        case ',':
        case '.':
        case '*':
        case '/':
        case ';':
        case '(':
        case ')':
        case '=':
        case '"':
        case ' ':
                cb_error_x ((yyvsp[(4) - (4)]), _("Invalid currency sign '%s'"), s);
                break;
        default:
                break;
        }
        current_program->currency_symbol = s[0];
  ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1725 "parser.y"
    {
        current_program->decimal_point = ',';
        current_program->numeric_separator = '.';
  ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1735 "parser.y"
    { current_program->cursor_pos = (yyvsp[(3) - (3)]); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1742 "parser.y"
    { current_program->crt_status = (yyvsp[(4) - (4)]); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1749 "parser.y"
    {  PENDING ("SCREEN CONTROL"); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1755 "parser.y"
    {  PENDING ("EVENT STATUS"); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1767 "parser.y"
    {
                                   if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] INPUT_OUTPUT SECTION\n");
                           ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1785 "parser.y"
    {
                                   if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] FILE_CONTROL\n");
                           ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1794 "parser.y"
    {
        organized_seen = 0;
        if ((yyvsp[(3) - (3)]) == cb_error_node) {
                if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] SELECT ERROR\n");
                YYERROR;
        }

        /* build new file */
        current_file = build_file ((yyvsp[(3) - (3)]));
        if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] SELECT %s\n", current_file->cname);
        current_file->optional = CB_INTEGER ((yyvsp[(2) - (3)]))->val;
        current_file->data_compress = cb_data_compress;
        current_file->ls_utf16 = cb_ls_utf16;
        current_file->ls_mfmode = cb_flag_line_seq_mf;
        current_file->ls_notrunc = cb_flag_line_seq_notrunc;
        current_file->ls_expandtab = cb_flag_line_seq_expand_tab;
        if (cb_flag_share_all_default) current_file->sharing = cb_int(COB_SHARE_WITH_ALL);

        /* register the file */
        current_program->file_list =
                cb_cons (CB_TREE (current_file), current_program->file_list);
        
  ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1818 "parser.y"
    {
        if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] SELECT %s Validate\n", current_file->cname);
        validate_file (current_file, (yyvsp[(3) - (6)]));
  ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1830 "parser.y"
    {if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] SELECT %s assign \n", current_file->cname);;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1835 "parser.y"
    {if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] SELECT %s status \n", current_file->cname);;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1852 "parser.y"
    {
        current_file->assign = cb_build_assignment_name (current_file, (yyvsp[(5) - (5)]));
  ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1856 "parser.y"
    {
        current_file->fileid_assign = 1;
  ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1860 "parser.y"
    {
        current_file->organization = COB_ORG_LINE_SEQUENTIAL; 
        current_file->rec_mode     = CB_REC_MODE_VARIABLE;
        current_file->is_printer   = 1;
        current_file->fileid_assign = 1;
  ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1870 "parser.y"
    { 
                  current_file->is_printer   = 1;
                  current_file->organization = COB_ORG_LINE_SEQUENTIAL; 
                  current_file->rec_mode     = CB_REC_MODE_VARIABLE;
                ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1879 "parser.y"
    {
        current_file->external_assign = cb_assign_clause == CB_ASSIGN_EXTERNAL;
        current_file->dynanic_assign  = cb_assign_clause == CB_ASSIGN_DYNAMIC;
  ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1884 "parser.y"
    {
        current_file->external_assign = 1;
  ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1888 "parser.y"
    {
        current_file->dynanic_assign = 1;
  ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1896 "parser.y"
    {
        const char      *s;

        s = "$#@DUMMY@#$";
        (yyval) = cb_build_alphanumeric_literal ((unsigned char *)s, strlen(s), 0);
  ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1913 "parser.y"
    { current_file->access_mode = COB_ACCESS_SEQUENTIAL; ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1914 "parser.y"
    { current_file->access_mode = COB_ACCESS_DYNAMIC; ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 1915 "parser.y"
    { current_file->access_mode = COB_ACCESS_RANDOM; ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1923 "parser.y"
    {
        struct cb_alt_key *p;
        struct cb_alt_key *l;

        p = cobc_malloc (sizeof (struct cb_alt_key));
        p->key = (yyvsp[(5) - (6)]);
        p->duplicates = CB_INTEGER ((yyvsp[(6) - (6)]))->val;
        p->next = NULL;
        p->key_compress = cb_key_compress;

        /* add to the end of list */
        if (current_file->alt_key_list == NULL) {
                current_file->alt_key_list = p;
        } else {
                l = current_file->alt_key_list;
                for (; l->next; l = l->next);
                l->next = p;
        }
  ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1949 "parser.y"
    {
        PENDING ("COLLATING SEQUENCE");
  ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1959 "parser.y"
    {
    current_file->file_status = (yyvsp[(4) - (5)]);
    if ((yyvsp[(5) - (5)])) {
                PENDING ("2nd FILE STATUS");
        }
  ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1980 "parser.y"
    { current_file->lock_mode = COB_LOCK_MANUAL; ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1981 "parser.y"
    { current_file->lock_mode = COB_LOCK_AUTOMATIC; ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1982 "parser.y"
    { current_file->lock_mode = COB_LOCK_EXCLUSIVE; ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1988 "parser.y"
    {
    current_file->lock_mode |= COB_LOCK_MULTIPLE;
  ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1992 "parser.y"
    { 
      if (!cb_flag_ignore_with_rollback)
        current_file->with_rollback = 1;
      current_file->lock_mode |= COB_LOCK_MULTIPLE;
      
      if (current_file->organization != COB_ORG_INDEXED)
        PENDING ("WITH ROLLBACK"); 
    ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 2009 "parser.y"
    { current_file->line_advancing_clause = 1;;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 2021 "parser.y"
    {
        if (organized_seen) {
                cb_error (_("Invalid or duplicate ORGANIZED clause"));
        } else {
                current_file->organization = COB_ORG_INDEXED;
                organized_seen = 1;
        }
  ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 2031 "parser.y"
    {
        if (organized_seen) {
                cb_error (_("Invalid or duplicate ORGANIZED clause"));
        } else {
                current_file->organization = COB_ORG_SEQUENTIAL;
                organized_seen = 1;
        }
  ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 2041 "parser.y"
    {
        if (organized_seen) {
                cb_error (_("Invalid or duplicate ORGANIZED clause"));
        } else {
                if (cb_flag_sequential_line)
                     current_file->organization = COB_ORG_LINE_SEQUENTIAL;
                else
                     current_file->organization = COB_ORG_SEQUENTIAL;
                organized_seen = 1;
        }
  ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 2053 "parser.y"
    {
        if (organized_seen) {
                cb_error (_("Invalid or duplicate ORGANIZED clause"));
        } else {
            if(cb_flag_relative_mf)
                current_file->organization = COB_ORG_RELATIVE;
            else
                current_file->organization = COB_ORG_RELATIVE;
            organized_seen = 1;
        }
  ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 2065 "parser.y"
    {
        if (organized_seen) {
                cb_error (_("Invalid or duplicate ORGANIZED clause"));
        } else {
                current_file->organization = COB_ORG_LINE_SEQUENTIAL;
                organized_seen = 1;
        }
  ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 2080 "parser.y"
    {
        cb_verify (cb_padding_character_clause, "PADDING CHARACTER");
  ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 2089 "parser.y"
    { /* ignored */ ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 2097 "parser.y"
    {
        current_file->key = (yyvsp[(4) - (5)]);
        current_file->key_duplicates = CB_INTEGER ((yyvsp[(5) - (5)]))->val;
        current_file->prim_key_compress = cb_key_compress;
  ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 2105 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 2106 "parser.y"
    { 
                                                (yyval) = cb_build_object_list((yyvsp[(1) - (3)]) , (yyvsp[(3) - (3)]), CB_OBJECT_LIST_FIELD); 
                                             ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 2109 "parser.y"
    { 
                                                   (yyval) = cb_build_object_list((yyvsp[(1) - (4)]) , (yyvsp[(4) - (4)]), CB_OBJECT_LIST_FIELD); 
                                             ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 2117 "parser.y"
    { current_file->key = (yyvsp[(4) - (4)]); ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 2124 "parser.y"
    { /* ignored */ ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 2125 "parser.y"
    { /* ignored */ ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 2132 "parser.y"
    { current_file->sharing = (yyvsp[(3) - (3)]); ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 2136 "parser.y"
    { (yyval) = cb_int(COB_SHARE_WITH_ALL); ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 2137 "parser.y"
    { (yyval) = cb_int(COB_SHARE_WITH_NO_OTHER); ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 2138 "parser.y"
    { (yyval) = cb_int(COB_SHARE_WITH_READ); ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 2153 "parser.y"
    {
                                   if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] I_O_CONTROL\n");
                           ;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 2178 "parser.y"
    {
        cb_tree l;

        switch (CB_INTEGER ((yyvsp[(2) - (5)]))->val) {
        case 0:
                /* SAME AREA */
                break;
        case 1:
                /* SAME RECORD */
                for (l = (yyvsp[(5) - (5)]); l; l = CB_CHAIN (l)) {
                        if (CB_VALUE (l) != cb_error_node) {
                                CB_FILE (cb_ref (CB_VALUE (l)))->same_clause = samearea;
                        }
                }
                samearea++;
                break;
        case 2:
                /* SAME SORT-MERGE */
                break;
        }
  ;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 2202 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 2203 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 2204 "parser.y"
    { (yyval) = cb_int2; ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 2205 "parser.y"
    { (yyval) = cb_int2; ;}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 2212 "parser.y"
    {
        cb_verify (cb_multiple_file_tape_clause, "MULTIPLE FILE TAPE");
  ;}
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 2223 "parser.y"
    { ;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 2234 "parser.y"
    {
        cb_warning (_("APPLY Clause ignored"));
  ;}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 2240 "parser.y"
    {;}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 2241 "parser.y"
    {;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 2242 "parser.y"
    {;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 2243 "parser.y"
    {;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 2253 "parser.y"
    {
        if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] Finalize Data Division\n");
        finalize_section_files (current_program->file_rec_list);
        cb_validate_section_fields(&(current_program->working_storage));
        cb_validate_section_fields(&(current_program->local_storage));
        cb_validate_section_fields(&(current_program->linkage_storage));
   ;}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 2274 "parser.y"
    {if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] DATA DIVISION\n");;}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 2276 "parser.y"
    {if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] FILE SECTION\n");;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 2278 "parser.y"
    {if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] WORKING STORAGE\n");;}
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 2280 "parser.y"
    {if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] LOCAL STORAGE\n");  ;}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 2282 "parser.y"
    {if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] LINKAGE STORAGE\n");  ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 2295 "parser.y"
    { 
        if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] FILE SECTION\n");
        current_storage = CB_STORAGE_FILE; 
        ;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 2301 "parser.y"
    {
    /*
        if (cb_relaxed_syntax_check) {
                cb_warning (_("FILE SECTION header missing - assumed"));
        } else {
                cb_error (_("FILE SECTION header missing"));
        }
     */
     current_file_type = (yyvsp[(1) - (1)]);
     current_storage = CB_STORAGE_FILE;
  ;}
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 2338 "parser.y"
    {
        if ((yyvsp[(2) - (2)]) && (yyvsp[(2) - (2)]) != cb_error_node) {
                current_program->file_rec_list = cb_list_add(current_program->file_rec_list,
                                                             cb_build_pair(CB_TREE(current_file), (yyvsp[(2) - (2)])));
        } else {
                cb_error (_("RECORD description missing or invalid"));
        }
  ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 2350 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 2351 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 2361 "parser.y"
    {
        if ((yyvsp[(1) - (1)]) == cb_error_node) {
                YYERROR;
        }

        current_file = CB_FILE (cb_ref ((yyvsp[(1) - (1)])));
        if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER]  FILE NAME %s\n", current_file->cname);
        if (current_file_type == cb_int1) {
                current_file->organization = COB_ORG_SORT;
        }
  ;}
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 2373 "parser.y"
    {
    /* Shut up bison */
    dummy_tree = (yyvsp[(2) - (4)]);
  ;}
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 2385 "parser.y"
    {
    if (0 && current_file->global) {
        cb_error (_("File cannot have both EXTERNAL and GLOBAL clauses"));
    }
    current_file->external = 1;
  ;}
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 2392 "parser.y"
    {
    if (0 && current_file->external) {
        cb_error (_("File cannot have both EXTERNAL and GLOBAL clauses"));
    }
    current_file->global = 1;
  ;}
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 2414 "parser.y"
    { /* ignored */ ;}
    break;

  case 313:

/* Line 1455 of yacc.c  */
#line 2453 "parser.y"
    {
        if (cb_flag_ls_ignore_rec_size && current_file->organization == COB_ORG_LINE_SEQUENTIAL) {
                cb_warning (_("RECORD clause ignored for LINE SEQUENTIAL"));
        } else {
                current_file->record_contain_clause = cb_flag_record_contains_strict;
                current_file->record_max = cb_get_int ((yyvsp[(3) - (4)]));
                if (current_file->record_max < 1)  {
                        current_file->record_max = 1;
                        cb_error (_("RECORD clause invalid"));
                }
                if (current_file->record_contain_clause)
                    current_file->record_min = current_file->record_max;
                if (current_file->rec_mode == CB_REC_MODE_DEFAULT)
                        current_file->rec_mode = CB_REC_MODE_FIX;
        }
  ;}
    break;

  case 314:

/* Line 1455 of yacc.c  */
#line 2470 "parser.y"
    {
        int     error_ind = 0;

        if (cb_flag_ls_ignore_rec_size && current_file->organization == COB_ORG_LINE_SEQUENTIAL) {
                cb_warning (_("RECORD clause ignored for LINE SEQUENTIAL"));
        } else {
                current_file->record_contain_clause = cb_flag_record_contains_strict;
                current_file->record_min = cb_get_int ((yyvsp[(3) - (6)]));
                current_file->record_max = cb_get_int ((yyvsp[(5) - (6)]));
                if (current_file->record_min < 0)  {
                        current_file->record_min = 0;
                        error_ind = 1;
                }
                if (current_file->record_max < 1)  {
                        current_file->record_max = 1;
                        error_ind = 1;
                }
                if (current_file->record_max < current_file->record_min)  {
                        error_ind = 1;
                }
                if (error_ind) {
                        cb_error (_("RECORD clause invalid"));
                }
                if (cb_flag_recmode_osvs && (current_file->rec_mode == CB_REC_MODE_DEFAULT))
                        current_file->rec_mode = CB_REC_MODE_VARIABLE;
        }
  ;}
    break;

  case 315:

/* Line 1455 of yacc.c  */
#line 2499 "parser.y"
    {
        int     error_ind = 0;

        if ((yyvsp[(6) - (9)]) && (yyvsp[(7) - (9)]) ) current_file->record_contain_clause = cb_flag_record_contains_strict;
        current_file->record_min = (yyvsp[(6) - (9)]) ? cb_get_int ((yyvsp[(6) - (9)])) : 0;
        current_file->record_max = (yyvsp[(7) - (9)]) ? cb_get_int ((yyvsp[(7) - (9)])) : 0;
        if (cb_flag_record_depending_iso && current_file->record_depending && current_file->record_min == 0) {
            current_file->record_min = 1;
        }
        if ((yyvsp[(6) - (9)]) && current_file->record_min < 0)  {
                current_file->record_min = 0;
                error_ind = 1;
        }
        if ((yyvsp[(7) - (9)]) && current_file->record_max < 1)  {
                current_file->record_max = 1;
                error_ind = 1;
        }
        if (((yyvsp[(6) - (9)]) || (yyvsp[(7) - (9)])) && current_file->record_max > 0  && current_file->record_max < current_file->record_min)  {
                error_ind = 1;
        }
        if ((current_file->rec_mode == CB_REC_MODE_DEFAULT) && 
            ((current_file->record_min != current_file->record_max) || (current_file->record_max == 0))){
                current_file->rec_mode = CB_REC_MODE_VARIABLE;
        } 
        if (error_ind) {
                cb_error (_("RECORD clause invalid"));
        }
  ;}
    break;

  case 317:

/* Line 1455 of yacc.c  */
#line 2531 "parser.y"
    {
        current_file->record_depending = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 318:

/* Line 1455 of yacc.c  */
#line 2537 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 319:

/* Line 1455 of yacc.c  */
#line 2538 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 320:

/* Line 1455 of yacc.c  */
#line 2542 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 321:

/* Line 1455 of yacc.c  */
#line 2543 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 322:

/* Line 1455 of yacc.c  */
#line 2551 "parser.y"
    {
        cb_verify (cb_label_records_clause, "LABEL RECORDS");
  ;}
    break;

  case 327:

/* Line 1455 of yacc.c  */
#line 2569 "parser.y"
    {
        cb_verify (cb_value_of_clause, "VALUE OF");
  ;}
    break;

  case 328:

/* Line 1455 of yacc.c  */
#line 2573 "parser.y"
    {
        if (!current_file->assign || cb_value_id_priority) {
                current_file->external_assign = 0;
                if (current_file->assign 
                    && CB_TREE_TAG(current_file->assign) == CB_TAG_REFERENCE ) {
                    current_program->reference_list = cb_list_remove(current_program->reference_list, current_file->assign);
                }
                current_file->assign = cb_build_assignment_name (current_file, (yyvsp[(5) - (5)]));
        }
  ;}
    break;

  case 333:

/* Line 1455 of yacc.c  */
#line 2599 "parser.y"
    { cb_warn_undefine = 1;;}
    break;

  case 334:

/* Line 1455 of yacc.c  */
#line 2601 "parser.y"
    {
        cb_warn_undefine = 0;
        cb_verify (cb_data_records_clause, "DATA RECORDS");
  ;}
    break;

  case 335:

/* Line 1455 of yacc.c  */
#line 2613 "parser.y"
    {
        if (current_file->organization != COB_ORG_LINE_SEQUENTIAL && 
            current_file->organization != COB_ORG_SEQUENTIAL) {
                cb_error (_("LINAGE clause with wrong file type"));
                (yyval) = cb_error_node;
        } else {
                current_file->linage = (yyvsp[(3) - (5)]);                
                current_file->is_printer = 1;
                if (current_linage == 0) {
                        linage_file = current_file;
                }
                current_linage++;
        }
  ;}
    break;

  case 341:

/* Line 1455 of yacc.c  */
#line 2641 "parser.y"
    {
        current_file->latfoot = (yyvsp[(5) - (6)]);
  ;}
    break;

  case 342:

/* Line 1455 of yacc.c  */
#line 2648 "parser.y"
    {
        current_file->lattop = (yyvsp[(4) - (5)]);
  ;}
    break;

  case 343:

/* Line 1455 of yacc.c  */
#line 2655 "parser.y"
    {
        current_file->latbot = (yyvsp[(4) - (5)]);
  ;}
    break;

  case 344:

/* Line 1455 of yacc.c  */
#line 2665 "parser.y"
    { /*CIT*/ 
    if (current_file->rec_mode != CB_REC_MODE_DEFAULT) {
        cb_warning ( _("Multiple RECORDING clause ignored '%s'"), current_file->name);
    }
    if ((yyvsp[(4) - (4)]) != cb_error_node) {      
        if (CB_REFERENCE_P((yyvsp[(4) - (4)]))) {
            if ((strncasecmp (CB_NAME((yyvsp[(4) - (4)])), "V", 1) == 0) || (strncasecmp (CB_NAME((yyvsp[(4) - (4)])), "S", 1) == 0))
                if (current_file->organization == COB_ORG_LINE_SEQUENTIAL) {
                    if (cb_flag_line_seq_recording_mode) {
                        current_file->rec_mode = CB_REC_MODE_VARIABLE;  
                    }
                } else {
                    current_file->rec_mode = CB_REC_MODE_VARIABLE;
                }
            else if (strncasecmp (CB_NAME((yyvsp[(4) - (4)])), "F", 1) == 0) {
                if (current_file->organization == COB_ORG_LINE_SEQUENTIAL) {
                    if (cb_flag_line_seq_recording_mode) {
                        current_file->rec_mode = CB_REC_MODE_FIX;  
                    }
                } else {
                    current_file->rec_mode = CB_REC_MODE_FIX;  
                }
            }                              
            else if (strncasecmp (CB_NAME((yyvsp[(4) - (4)])), "U", 1) != 0)
                cb_warning ( _("Unsupported RECORDING mode '%s'"), CB_NAME((yyvsp[(4) - (4)])));
        }
    }
  ;}
    break;

  case 345:

/* Line 1455 of yacc.c  */
#line 2700 "parser.y"
    {
        if ((yyvsp[(3) - (3)]) != cb_error_node) {
                cb_tree x;

                x = cb_ref ((yyvsp[(3) - (3)]));
                if (!CB_ALPHABET_NAME_P (x)) {
                        cb_error_x ((yyvsp[(3) - (3)]), _("Alphabet-name is expected '%s'"), cb_name ((yyvsp[(3) - (3)])));
                } else if (CB_ALPHABET_NAME (x)->custom_list) {
                        PENDING ("CODE-SET");
                }
        }
  ;}
    break;

  case 346:

/* Line 1455 of yacc.c  */
#line 2717 "parser.y"
    {
    cb_warning (_("file descriptor REPORT IS"));
  ;}
    break;

  case 347:

/* Line 1455 of yacc.c  */
#line 2721 "parser.y"
    {
    cb_warning (_("file descriptor REPORTS ARE"));
  ;}
    break;

  case 348:

/* Line 1455 of yacc.c  */
#line 2732 "parser.y"
    { current_storage = CB_STORAGE_WORKING; 
                                  if ( cb_flag_debug_parser ) fprintf(stderr, "[DEBUG PARSER] WORKING-STORAGE\n");
                                ;}
    break;

  case 349:

/* Line 1455 of yacc.c  */
#line 2736 "parser.y"
    {
       
        if ((yyvsp[(5) - (6)])) {
                current_program->working_storage =
                        cb_field_add (current_program->working_storage, CB_FIELD ((yyvsp[(5) - (6)])));
        }
  ;}
    break;

  case 350:

/* Line 1455 of yacc.c  */
#line 2746 "parser.y"
    {
    /* cb_field_tree_check_typedef(current_field);*/
        current_field = NULL;
    description_field = NULL;
    cb_clear_real_field ();
  ;}
    break;

  case 351:

/* Line 1455 of yacc.c  */
#line 2753 "parser.y"
    {
    /*CIT*/
    /*Validation will be done later when all data section have been parsed*/
    (yyval) = CB_TREE (description_field);
  ;}
    break;

  case 355:

/* Line 1455 of yacc.c  */
#line 2777 "parser.y"
    {
        cb_tree x;

        x = cb_build_field_tree ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), current_field, current_storage, current_file);
        if (x == cb_error_node) {
                YYERROR;
        } else {
            current_field = CB_FIELD (x);
        }
  ;}
    break;

  case 356:

/* Line 1455 of yacc.c  */
#line 2788 "parser.y"
    {
    if (current_field->flag_is_typedef) {
        /* move it out of the storage*/
        if ((current_field->level != 1) && (current_field->level != 77 )) {
            cb_error (_("is TYPEDEF must be Level 01 or 77"));            
        }
        if (!description_field) {
            description_field = current_field;
        }
    } else {
        if (!qualifier && (current_field->level == 88 ||
            current_field->level == 77 || current_field->level == 66 ||
            current_field->flag_item_78)) {
            if (current_field->level == 77){
                    cb_warning (_("Item Level 77 without a data name"));                    
            } else {
                    cb_error (_("Item requires a data name"));    
            }
        }
        if (current_field->level == 88) {
            cb_validate_88_item (current_field);
        }
        if (current_field->flag_item_78) {
            /* Reset to last non-78 item */
            current_field = cb_validate_78_item (current_field);
        }
        if (!description_field) {
            description_field = current_field;
        }
    }
  ;}
    break;

  case 361:

/* Line 1455 of yacc.c  */
#line 2830 "parser.y"
    {
    (yyval) = cb_build_filler ();
    qualifier = NULL;
    non_const_word = 0;
  ;}
    break;

  case 362:

/* Line 1455 of yacc.c  */
#line 2836 "parser.y"
    {
    (yyval) = cb_build_filler ();
    qualifier = NULL;
    non_const_word = 0;
  ;}
    break;

  case 363:

/* Line 1455 of yacc.c  */
#line 2842 "parser.y"
    {
    (yyval) = (yyvsp[(1) - (1)]);
    qualifier = (yyvsp[(1) - (1)]);
    non_const_word = 0;
  ;}
    break;

  case 364:

/* Line 1455 of yacc.c  */
#line 2851 "parser.y"
    {
    (yyval) = (yyvsp[(1) - (1)]);
    qualifier = (yyvsp[(1) - (1)]);
    non_const_word = 0;
  ;}
    break;

  case 366:

/* Line 1455 of yacc.c  */
#line 2860 "parser.y"
    {
    current_field->flag_is_global = 1;
    cb_error (_("CONSTANT with GLOBAL clause is not yet supported"));
  ;}
    break;

  case 367:

/* Line 1455 of yacc.c  */
#line 2867 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 368:

/* Line 1455 of yacc.c  */
#line 2868 "parser.y"
    { (yyval) = cb_build_const_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 369:

/* Line 1455 of yacc.c  */
#line 2869 "parser.y"
    { (yyval) = cb_build_const_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 370:

/* Line 1455 of yacc.c  */
#line 2874 "parser.y"
    {
    cb_tree x;
    int level;

    level = cb_get_level ((yyvsp[(1) - (6)]));
    if (level && level != 1) {
        cb_error (_("CONSTANT item not at 01 level"));
    }
    x = cb_build_constant ((yyvsp[(2) - (6)]), (yyvsp[(6) - (6)]));
    CB_FIELD (x)->flag_item_78 = 1;
    CB_FIELD (x)->level = 1;
    cb_needs_01 = 1;
    /* Ignore return value */
    cb_validate_78_item (CB_FIELD (x));
  ;}
    break;

  case 371:

/* Line 1455 of yacc.c  */
#line 2893 "parser.y"
    {
    /* required to check redefines */
    (yyval) = NULL;
  ;}
    break;

  case 372:

/* Line 1455 of yacc.c  */
#line 2899 "parser.y"
    {
    /* required to check redefines */
    (yyval) = cb_true;
  ;}
    break;

  case 388:

/* Line 1455 of yacc.c  */
#line 2926 "parser.y"
    { 
            current_field->flag_is_typedef =1;
            if (cb_flag_global_typedef)
                current_field->flag_is_typedef_global = 1;
        ;}
    break;

  case 389:

/* Line 1455 of yacc.c  */
#line 2936 "parser.y"
    {
        if ((yyvsp[(0) - (2)]) != NULL) {
                /* hack for MF compatibility */
                if (cb_relaxed_syntax_check) {
                        cb_warning_suggestion ((yyvsp[(2) - (2)]), _("REDEFINES clause should follow entry-name"));
                } else {
                        cb_error_x ((yyvsp[(2) - (2)]), _("REDEFINES clause must follow entry-name"));
                }
        }

        current_field->redefines = cb_resolve_redefines (current_field, (yyvsp[(2) - (2)]));
        if (current_field->redefines == NULL) {
                YYERROR;
        }
  ;}
    break;

  case 390:

/* Line 1455 of yacc.c  */
#line 2958 "parser.y"
    {
   if (current_storage != CB_STORAGE_WORKING) {
      cb_error (_("EXTERNAL not allowed here"));
    } else if (current_field->level != 1 && current_field->level != 77) {
      cb_error (_("EXTERNAL only allowed at 01/77 level"));
    } else if (!qualifier) {
      cb_error (_("EXTERNAL requires a data name"));
    /* CIT
    } else if (current_field->flag_is_global) {
        cb_error (_("GLOBAL and EXTERNAL are mutually exclusive"));
    */
    } else if (current_field->flag_item_based) {
      cb_error (_("BASED and EXTERNAL are mutually exclusive"));
    } else if (current_field->redefines) {
      cb_error (_("EXTERNAL and REDEFINES are mutually exclusive"));
    } else {
      current_field->flag_external = 1;
      has_external = 1;
    }
  ;}
    break;

  case 391:

/* Line 1455 of yacc.c  */
#line 2981 "parser.y"
    { current_field->ename = NULL; ;}
    break;

  case 392:

/* Line 1455 of yacc.c  */
#line 2983 "parser.y"
    {
        struct cb_field *x;

        x = CB_FIELD(cb_build_field (cb_build_reference ((char *)(CB_LITERAL ((yyvsp[(2) - (2)]))->data))));
        current_field->ename = x->name;
 ;}
    break;

  case 393:

/* Line 1455 of yacc.c  */
#line 2995 "parser.y"
    {
    if (current_field->level != 1 && current_field->level != 77) {
        cb_error (_("GLOBAL only allowed at 01/77 level"));
    } else if (!qualifier) {
        cb_error (_("GLOBAL requires a data name"));
    /* CIT
    } else if (current_field->flag_external) {
        cb_error (_("GLOBAL and EXTERNAL are mutually exclusive"));
    */
    } else if (current_storage == CB_STORAGE_LOCAL) {
        if (cb_flag_ignore_global_in_local)
            cb_warning(_("GLOBAL in LOCAL STORAGE is ignored"));
        else
            cb_error (_("GLOBAL not allowed here"));
    } else {
        current_field->flag_is_global = 1;
    }
  ;}
    break;

  case 394:

/* Line 1455 of yacc.c  */
#line 3020 "parser.y"
    { 
        if ((yyvsp[(1) - (1)]) != cb_error_node) {
            current_field->pic = CB_PICTURE ((yyvsp[(1) - (1)]));
            if ((current_field->pic->category == CB_CATEGORY_NATIONAL || 
                 current_field->pic->category == CB_CATEGORY_NATIONAL_EDITED ) &&
                !current_field->flag_usage_set) {
                current_field->usage = CB_USAGE_NATIONAL;
            }
        }
    ;}
    break;

  case 397:

/* Line 1455 of yacc.c  */
#line 3039 "parser.y"
    {
        struct cb_field *f = cb_type((yyvsp[(3) - (3)]));
        if (f && f->flag_is_typedef) {
            cob_enterprise_map_field_from_type (current_field, f);
        } else {
            cb_error (_("Invalid USAGE") );
        }
        
    ;}
    break;

  case 398:

/* Line 1455 of yacc.c  */
#line 3049 "parser.y"
    {
        struct cb_field *f = cb_field((yyvsp[(2) - (2)]));
        if (f ) {
            if (cb_flag_as400_like) {
                if (current_field->flag_is_typedef) {
                    cb_error_x (CB_TREE (f), _("LIKE may not define a TYPEDEF '%s'"),   f->name);
                } else {
                    current_field->like = f;
                }
            } else {
                cob_enterprise_map_field_from_type (current_field, f);
            }
        } else {
            cb_error (_("Invalid USAGE") );
        }
    ;}
    break;

  case 399:

/* Line 1455 of yacc.c  */
#line 3068 "parser.y"
    { current_field->usage = CB_USAGE_BINARY; current_field->flag_usage_set = 1;;}
    break;

  case 400:

/* Line 1455 of yacc.c  */
#line 3069 "parser.y"
    { current_field->usage = CB_USAGE_BIT;    current_field->flag_usage_set = 1;;}
    break;

  case 401:

/* Line 1455 of yacc.c  */
#line 3070 "parser.y"
    { current_field->usage = CB_USAGE_BINARY; current_field->flag_usage_set = 1;;}
    break;

  case 402:

/* Line 1455 of yacc.c  */
#line 3071 "parser.y"
    { current_field->usage = CB_USAGE_FLOAT;  current_field->flag_usage_set = 1;;}
    break;

  case 403:

/* Line 1455 of yacc.c  */
#line 3072 "parser.y"
    { current_field->usage = CB_USAGE_DOUBLE; current_field->flag_usage_set = 1;;}
    break;

  case 404:

/* Line 1455 of yacc.c  */
#line 3073 "parser.y"
    { current_field->usage = CB_USAGE_PACKED; current_field->flag_usage_set = 1;;}
    break;

  case 405:

/* Line 1455 of yacc.c  */
#line 3074 "parser.y"
    { current_field->usage = CB_USAGE_BINARY; current_field->flag_usage_set = 1;
                                  current_field->flag_binary_pack =  (cb_flag_pack_comp_4 ? 1 : 0);;}
    break;

  case 406:

/* Line 1455 of yacc.c  */
#line 3076 "parser.y"
    { current_field->usage = CB_USAGE_COMP_5; current_field->flag_usage_set = 1;;}
    break;

  case 407:

/* Line 1455 of yacc.c  */
#line 3077 "parser.y"
    { current_field->usage = CB_USAGE_COMP_6; current_field->flag_usage_set = 1;;}
    break;

  case 408:

/* Line 1455 of yacc.c  */
#line 3078 "parser.y"
    { current_field->usage = CB_USAGE_COMP_X; current_field->flag_usage_set = 1;;}
    break;

  case 409:

/* Line 1455 of yacc.c  */
#line 3079 "parser.y"
    { current_field->usage = CB_USAGE_DISPLAY;current_field->flag_usage_set = 1;;}
    break;

  case 410:

/* Line 1455 of yacc.c  */
#line 3080 "parser.y"
    { current_field->usage = CB_USAGE_INDEX;  current_field->flag_usage_set = 1;;}
    break;

  case 411:

/* Line 1455 of yacc.c  */
#line 3081 "parser.y"
    { current_field->usage = CB_USAGE_PACKED; current_field->flag_usage_set = 1;;}
    break;

  case 412:

/* Line 1455 of yacc.c  */
#line 3083 "parser.y"
    {
        current_field->usage = CB_USAGE_POINTER;
        current_field->flag_is_pointer = 1;
        current_field->flag_usage_set = 1;
  ;}
    break;

  case 413:

/* Line 1455 of yacc.c  */
#line 3089 "parser.y"
    {
        current_field->usage = CB_USAGE_PROGRAM_POINTER;
        current_field->flag_is_pointer = 1;
        current_field->flag_usage_set = 1;
  ;}
    break;

  case 414:

/* Line 1455 of yacc.c  */
#line 3094 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_SHORT; current_field->flag_usage_set = 1;;}
    break;

  case 415:

/* Line 1455 of yacc.c  */
#line 3095 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_INT;   current_field->flag_usage_set = 1;;}
    break;

  case 416:

/* Line 1455 of yacc.c  */
#line 3096 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_LONG;  current_field->flag_usage_set = 1;;}
    break;

  case 417:

/* Line 1455 of yacc.c  */
#line 3097 "parser.y"
    { current_field->usage = CB_USAGE_UNSIGNED_SHORT; current_field->flag_usage_set = 1;;}
    break;

  case 418:

/* Line 1455 of yacc.c  */
#line 3098 "parser.y"
    { current_field->usage = CB_USAGE_UNSIGNED_INT;   current_field->flag_usage_set = 1;;}
    break;

  case 419:

/* Line 1455 of yacc.c  */
#line 3099 "parser.y"
    { current_field->usage = CB_USAGE_UNSIGNED_LONG;  current_field->flag_usage_set = 1;;}
    break;

  case 420:

/* Line 1455 of yacc.c  */
#line 3100 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_CHAR;    current_field->flag_usage_set = 1;;}
    break;

  case 421:

/* Line 1455 of yacc.c  */
#line 3101 "parser.y"
    { current_field->usage = CB_USAGE_UNSIGNED_CHAR;  current_field->flag_usage_set = 1;;}
    break;

  case 422:

/* Line 1455 of yacc.c  */
#line 3102 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_CHAR;    current_field->flag_usage_set = 1;;}
    break;

  case 423:

/* Line 1455 of yacc.c  */
#line 3103 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_SHORT;   current_field->flag_usage_set = 1;;}
    break;

  case 424:

/* Line 1455 of yacc.c  */
#line 3104 "parser.y"
    { current_field->usage = CB_USAGE_UNSIGNED_SHORT; current_field->flag_usage_set = 1;;}
    break;

  case 425:

/* Line 1455 of yacc.c  */
#line 3105 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_SHORT;   current_field->flag_usage_set = 1;;}
    break;

  case 426:

/* Line 1455 of yacc.c  */
#line 3106 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_INT;     current_field->flag_usage_set = 1;;}
    break;

  case 427:

/* Line 1455 of yacc.c  */
#line 3107 "parser.y"
    { current_field->usage = CB_USAGE_UNSIGNED_INT;   current_field->flag_usage_set = 1;;}
    break;

  case 428:

/* Line 1455 of yacc.c  */
#line 3108 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_INT;     current_field->flag_usage_set = 1;;}
    break;

  case 429:

/* Line 1455 of yacc.c  */
#line 3109 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_LONG;    current_field->flag_usage_set = 1;;}
    break;

  case 430:

/* Line 1455 of yacc.c  */
#line 3110 "parser.y"
    { current_field->usage = CB_USAGE_UNSIGNED_LONG;  current_field->flag_usage_set = 1;;}
    break;

  case 431:

/* Line 1455 of yacc.c  */
#line 3111 "parser.y"
    { current_field->usage = CB_USAGE_SIGNED_LONG;    current_field->flag_usage_set = 1;;}
    break;

  case 432:

/* Line 1455 of yacc.c  */
#line 3113 "parser.y"
    {
        current_field->flag_usage_set = 1;
        if (sizeof(long) == 4) {
                current_field->usage = CB_USAGE_SIGNED_INT;
        } else {
                current_field->usage = CB_USAGE_SIGNED_LONG;
        }
  ;}
    break;

  case 433:

/* Line 1455 of yacc.c  */
#line 3122 "parser.y"
    {
        current_field->flag_usage_set = 1;
        if (sizeof(long) == 4) {
                current_field->usage = CB_USAGE_UNSIGNED_INT;
        } else {
                current_field->usage = CB_USAGE_UNSIGNED_LONG;
        }
  ;}
    break;

  case 434:

/* Line 1455 of yacc.c  */
#line 3131 "parser.y"
    {
        current_field->flag_usage_set = 1;
        if (sizeof(long) == 4) {
                current_field->usage = CB_USAGE_SIGNED_INT;
        } else {
                current_field->usage = CB_USAGE_SIGNED_LONG;
        }
  ;}
    break;

  case 435:

/* Line 1455 of yacc.c  */
#line 3140 "parser.y"
    { 
    current_field->usage = CB_USAGE_NATIONAL;
    current_field->flag_usage_set = 1;

  ;}
    break;

  case 436:

/* Line 1455 of yacc.c  */
#line 3146 "parser.y"
    { 
    current_field->usage = CB_USAGE_NATIONAL;
    current_field->flag_usage_set = 1;
  ;}
    break;

  case 437:

/* Line 1455 of yacc.c  */
#line 3157 "parser.y"
    {
        current_field->flag_sign_separate = CB_INTEGER ((yyvsp[(3) - (3)]))->val;
        current_field->flag_sign_leading  = 1;
  ;}
    break;

  case 438:

/* Line 1455 of yacc.c  */
#line 3162 "parser.y"
    {
        current_field->flag_sign_separate = CB_INTEGER ((yyvsp[(3) - (3)]))->val;
        current_field->flag_sign_leading  = 0;
  ;}
    break;

  case 439:

/* Line 1455 of yacc.c  */
#line 3174 "parser.y"
    {
       if (current_field->occurs_depending && !((yyvsp[(3) - (5)]))) {
            cb_verify (cb_odo_without_to, "ODO without TO clause");
       }
       current_field->occurs_min = (yyvsp[(3) - (5)]) ? cb_get_int ((yyvsp[(2) - (5)])) : 1;
       current_field->occurs_max = (yyvsp[(3) - (5)]) ? cb_get_int ((yyvsp[(3) - (5)])) : cb_get_int ((yyvsp[(2) - (5)]));
       current_field->indexes++;
       if (current_field->children){
           cb_increment_indexes(current_field->children);
       }
       if (current_field->indexes > COB_MAX_SUBSCRIPTS) {
           cb_error (_("Maximum OCCURS depth exceeded"));
       }
       current_field->flag_occurs = 1;
  ;}
    break;

  case 447:

/* Line 1455 of yacc.c  */
#line 3208 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 448:

/* Line 1455 of yacc.c  */
#line 3209 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 449:

/* Line 1455 of yacc.c  */
#line 3214 "parser.y"
    {
        current_field->occurs_depending = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 450:

/* Line 1455 of yacc.c  */
#line 3221 "parser.y"
    {
        if ((yyvsp[(1) - (1)])) {
                cb_tree         l;
                struct cb_key   *keys;
                int             i;
                int             nkeys;

                l = (yyvsp[(1) - (1)]);
                nkeys = cb_list_length ((yyvsp[(1) - (1)]));
                keys = cobc_malloc (sizeof (struct cb_key) * nkeys);

                for (i = 0; i < nkeys; i++) {
                        keys[i].dir = CB_PURPOSE_INT (l);
                        keys[i].key = CB_VALUE (l);
                        l = CB_CHAIN (l);
                }
                current_field->keys = keys;
                current_field->nkeys = nkeys;
        }
  ;}
    break;

  case 451:

/* Line 1455 of yacc.c  */
#line 3245 "parser.y"
    {
        cb_tree l;

        for (l = (yyvsp[(4) - (4)]); l; l = CB_CHAIN (l)) {
                CB_PURPOSE (l) = (yyvsp[(1) - (4)]);
        if (qualifier && !CB_REFERENCE(CB_VALUE(l))->chain &&
                    strcasecmp(CB_NAME(CB_VALUE(l)), CB_NAME(qualifier))) {
                        CB_REFERENCE(CB_VALUE(l))->chain = qualifier;
                }
        }
        (yyval) = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 452:

/* Line 1455 of yacc.c  */
#line 3260 "parser.y"
    {         (yyval) = cb_list_append (NULL, (yyvsp[(1) - (1)]));;}
    break;

  case 453:

/* Line 1455 of yacc.c  */
#line 3262 "parser.y"
    {         (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]));;}
    break;

  case 454:

/* Line 1455 of yacc.c  */
#line 3266 "parser.y"
    { (yyval) = cb_int (COB_ASCENDING); ;}
    break;

  case 455:

/* Line 1455 of yacc.c  */
#line 3267 "parser.y"
    { (yyval) = cb_int (COB_DESCENDING); ;}
    break;

  case 456:

/* Line 1455 of yacc.c  */
#line 3272 "parser.y"
    {
        current_field->index_list = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 457:

/* Line 1455 of yacc.c  */
#line 3278 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 458:

/* Line 1455 of yacc.c  */
#line 3280 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 459:

/* Line 1455 of yacc.c  */
#line 3285 "parser.y"
    {
        (yyval) = cb_build_index ((yyvsp[(1) - (1)]), cb_int1, 1, current_field, 0,CB_USAGE_INDEX);
  ;}
    break;

  case 460:

/* Line 1455 of yacc.c  */
#line 3294 "parser.y"
    { current_field->flag_justified = 1; ;}
    break;

  case 461:

/* Line 1455 of yacc.c  */
#line 3301 "parser.y"
    { current_field->flag_synchronized = 1; ;}
    break;

  case 465:

/* Line 1455 of yacc.c  */
#line 3313 "parser.y"
    { current_field->flag_blank_zero = 1; ;}
    break;

  case 466:

/* Line 1455 of yacc.c  */
#line 3321 "parser.y"
    {
    if (current_storage != CB_STORAGE_WORKING &&
        current_storage != CB_STORAGE_LINKAGE &&
        current_storage != CB_STORAGE_LOCAL) {
        cb_error (_("BASED not allowed here"));
    } else if (current_field->level != 1 && current_field->level != 77) {
        cb_error (_("BASED only allowed at 01/77 level"));
    } else if (!qualifier) {
        cb_error (_("BASED requires a data name"));
    } else if (current_field->flag_external) {
        cb_error (_("BASED and EXTERNAL are mutually exclusive"));
    } else if (current_field->redefines) {
        cb_error (_("BASED and REDEFINES are mutually exclusive"));
    } else if (current_field->flag_any_length) {
        cb_error (_("BASED and ANY LENGTH are mutually exclusive"));
    } else {
        current_field->flag_item_based = 1;
    }
  ;}
    break;

  case 467:

/* Line 1455 of yacc.c  */
#line 3345 "parser.y"
    { current_field->values = (yyvsp[(1) - (1)]); ;}
    break;

  case 468:

/* Line 1455 of yacc.c  */
#line 3347 "parser.y"
    { current_field->values = (yyvsp[(3) - (3)]); ;}
    break;

  case 470:

/* Line 1455 of yacc.c  */
#line 3352 "parser.y"
    { current_field->values = (yyvsp[(3) - (3)]); ;}
    break;

  case 472:

/* Line 1455 of yacc.c  */
#line 3357 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 473:

/* Line 1455 of yacc.c  */
#line 3358 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 474:

/* Line 1455 of yacc.c  */
#line 3362 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 475:

/* Line 1455 of yacc.c  */
#line 3363 "parser.y"
    { (yyval) = cb_build_pair ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 476:

/* Line 1455 of yacc.c  */
#line 3364 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); 
                                  if (!CB_CONST_P((yyval))) {
                                      cb_error_x((yyval),"is not a constant");
                                  }
                                ;}
    break;

  case 478:

/* Line 1455 of yacc.c  */
#line 3373 "parser.y"
    {
        if (current_field->level != 88) {
                cb_error (_("FALSE clause only allowed for 88 level"));
        }
    current_field->false_88 = cb_list_init ((yyvsp[(3) - (3)]));
  ;}
    break;

  case 479:

/* Line 1455 of yacc.c  */
#line 3386 "parser.y"
    {
        if (cb_ref ((yyvsp[(2) - (2)])) != cb_error_node) {
                if (CB_FIELD (cb_ref ((yyvsp[(2) - (2)])))->level == 01 ||
                    CB_FIELD (cb_ref ((yyvsp[(2) - (2)])))->level > 50) {
                        cb_error (_("RENAMES may not reference a level 01 or > 50"));
                } else {
                        current_field->redefines = CB_FIELD (cb_ref ((yyvsp[(2) - (2)])));
                        current_field->pic = current_field->redefines->pic;
                }
        }
  ;}
    break;

  case 480:

/* Line 1455 of yacc.c  */
#line 3398 "parser.y"
    {
        if (cb_ref ((yyvsp[(2) - (4)])) != cb_error_node && cb_ref ((yyvsp[(4) - (4)])) != cb_error_node) {
                if (CB_FIELD (cb_ref ((yyvsp[(2) - (4)])))->level == 01 ||
                    CB_FIELD (cb_ref ((yyvsp[(2) - (4)])))->level > 50) {
                        cb_error (_("RENAMES may not reference a level 01 or > 50"));
                } else if (CB_FIELD (cb_ref ((yyvsp[(4) - (4)])))->level == 01 ||
                    CB_FIELD (cb_ref ((yyvsp[(4) - (4)])))->level > 50) {
                        cb_error (_("RENAMES may not reference a level 01 or > 50"));
                } else {
                        current_field->redefines = CB_FIELD (cb_ref ((yyvsp[(2) - (4)])));
                        current_field->rename_thru = CB_FIELD (cb_ref ((yyvsp[(4) - (4)])));
                }
        }
  ;}
    break;

  case 481:

/* Line 1455 of yacc.c  */
#line 3418 "parser.y"
    {
    if (current_field->flag_item_based) {
        cb_error (_("BASED and ANY LENGTH are mutually exclusive"));
    } else {
        current_field->flag_any_length = 1;
  }
  ;}
    break;

  case 482:

/* Line 1455 of yacc.c  */
#line 3433 "parser.y"
    {
    current_storage = CB_STORAGE_LOCAL;
    if (current_program->nested_level) {
        cb_error (_("LOCAL-STORAGE not allowed in nested programs"));
    }
  ;}
    break;

  case 483:

/* Line 1455 of yacc.c  */
#line 3440 "parser.y"
    {
        if ((yyvsp[(5) - (5)])) {
                current_program->local_storage = 
                    cb_field_add (current_program->local_storage, CB_FIELD ((yyvsp[(5) - (5)])));
        }
  ;}
    break;

  case 484:

/* Line 1455 of yacc.c  */
#line 3454 "parser.y"
    { current_storage = CB_STORAGE_LINKAGE; ;}
    break;

  case 485:

/* Line 1455 of yacc.c  */
#line 3456 "parser.y"
    {
        if ((yyvsp[(5) - (5)])) {
                current_program->linkage_storage = 
                    cb_field_add (current_program->linkage_storage, CB_FIELD ((yyvsp[(5) - (5)])));        
                }
  ;}
    break;

  case 487:

/* Line 1455 of yacc.c  */
#line 3470 "parser.y"
    {
    cb_error (_("REPORT SECTION need preprocessing ... use SPCRW2"));
    current_storage = CB_STORAGE_REPORT;
  ;}
    break;

  case 494:

/* Line 1455 of yacc.c  */
#line 3503 "parser.y"
    {
    cb_warning (_("Report description using defaults"));
  ;}
    break;

  case 496:

/* Line 1455 of yacc.c  */
#line 3511 "parser.y"
    {
    cb_error (_("GLOBAL is not allowed with RD"));
  ;}
    break;

  case 535:

/* Line 1455 of yacc.c  */
#line 3582 "parser.y"
    { cb_warning (_("looking for Report line TYPE")); ;}
    break;

  case 589:

/* Line 1455 of yacc.c  */
#line 3690 "parser.y"
    { current_storage = CB_STORAGE_SCREEN; ;}
    break;

  case 590:

/* Line 1455 of yacc.c  */
#line 3691 "parser.y"
    {
        current_field = NULL;
    description_field = NULL;
    cb_clear_real_field ();
  ;}
    break;

  case 591:

/* Line 1455 of yacc.c  */
#line 3697 "parser.y"
    {
        struct cb_field *p;

        cb_expand_screen_field (description_field, NULL);
        for (p = description_field; p; p = p->sister) {
            cb_validate_field (p);
            }
        current_program->screen_storage = description_field;
        current_program->flag_screen = 1;
        ;}
    break;

  case 597:

/* Line 1455 of yacc.c  */
#line 3722 "parser.y"
    {
        cb_tree x;

        x = cb_build_field_tree ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), current_field, current_storage, current_file);
        if (x == cb_error_node) {
                YYERROR;
        }

        current_field = CB_FIELD (x);
        if (current_field->parent) {
                int flg = current_field->parent->screen_flag;

                flg &= ~COB_SCREEN_BLANK_SCREEN;
                flg &= ~COB_SCREEN_BELL;
                current_field->screen_flag |= flg;
                current_field->screen_foreg = current_field->parent->screen_foreg;
                current_field->screen_backg = current_field->parent->screen_backg;
        } else {
            if (cb_flag_accept_with_auto)
                current_field->screen_flag |= COB_SCREEN_AUTO;
        }
  ;}
    break;

  case 598:

/* Line 1455 of yacc.c  */
#line 3745 "parser.y"
    {
    if (!qualifier && (current_field->level == 88 ||
        current_field->level == 77 || current_field->level == 66 ||
        current_field->flag_item_78)) {
        cb_error (_("Item requires a data name"));
    }
    if (current_field->level == 88) {
        cb_validate_88_item (current_field);
    }
    if (current_field->flag_item_78) {
        /* Reset to last non-78 item */
        current_field = cb_validate_78_item (current_field);
    }
    if (!description_field) {
        description_field = current_field;
    }
  ;}
    break;

  case 601:

/* Line 1455 of yacc.c  */
#line 3769 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_BLANK_LINE; ;}
    break;

  case 602:

/* Line 1455 of yacc.c  */
#line 3770 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_BLANK_SCREEN; ;}
    break;

  case 603:

/* Line 1455 of yacc.c  */
#line 3771 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_BELL; ;}
    break;

  case 604:

/* Line 1455 of yacc.c  */
#line 3772 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_NO_BELL; ;}
    break;

  case 605:

/* Line 1455 of yacc.c  */
#line 3773 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_BLINK; ;}
    break;

  case 606:

/* Line 1455 of yacc.c  */
#line 3774 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_ERASE_EOL; ;}
    break;

  case 607:

/* Line 1455 of yacc.c  */
#line 3775 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_ERASE_EOS; ;}
    break;

  case 608:

/* Line 1455 of yacc.c  */
#line 3776 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_HIGHLIGHT; ;}
    break;

  case 609:

/* Line 1455 of yacc.c  */
#line 3777 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_LOWLIGHT; ;}
    break;

  case 610:

/* Line 1455 of yacc.c  */
#line 3778 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_REVERSE; ;}
    break;

  case 611:

/* Line 1455 of yacc.c  */
#line 3779 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_UNDERLINE; ;}
    break;

  case 612:

/* Line 1455 of yacc.c  */
#line 3780 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_OVERLINE; ;}
    break;

  case 613:

/* Line 1455 of yacc.c  */
#line 3781 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_AUTO; ;}
    break;

  case 614:

/* Line 1455 of yacc.c  */
#line 3782 "parser.y"
    { current_field->screen_flag &= ~COB_SCREEN_AUTO; ;}
    break;

  case 615:

/* Line 1455 of yacc.c  */
#line 3783 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_SECURE; ;}
    break;

  case 616:

/* Line 1455 of yacc.c  */
#line 3784 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_REQUIRED; ;}
    break;

  case 617:

/* Line 1455 of yacc.c  */
#line 3785 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_FULL; ;}
    break;

  case 618:

/* Line 1455 of yacc.c  */
#line 3787 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_UPPERCASE; ;}
    break;

  case 619:

/* Line 1455 of yacc.c  */
#line 3788 "parser.y"
    { current_field->screen_flag |= COB_SCREEN_LOWERCASE; ;}
    break;

  case 620:

/* Line 1455 of yacc.c  */
#line 3790 "parser.y"
    {

        current_field->screen_flag |= COB_SCREEN_PROMPT; 
        if((yyvsp[(4) - (4)])){
           unsigned char *s = CB_LITERAL ((yyvsp[(4) - (4)]))->data;
           if(!(yyvsp[(2) - (4)]) && !(yyvsp[(3) - (4)])){
               cb_error_x ((yyvsp[(4) - (4)]), _("Invalid PROMPT '%s'"), s);
           }
           if (CB_LITERAL ((yyvsp[(4) - (4)]))->size != 1) {
                   cb_error_x ((yyvsp[(4) - (4)]), _("Invalid PROMPT CHARACTER '%s'"), s);
           }
           current_field->screen_prompt = *s;
        }
        else{
            if((yyvsp[(2) - (4)]))
               if((yyvsp[(3) - (4)]))
                  cb_error (_("Invalid PROMPT CHARACTER IS"));
               else
                  cb_error (_("Invalid PROMPT CHARACTER"));
            else if((yyvsp[(3) - (4)]))
               cb_error (_("Invalid PROMPT IS"));
        }
  ;}
    break;

  case 621:

/* Line 1455 of yacc.c  */
#line 3814 "parser.y"
    { 
      if ((yyvsp[(5) - (5)]) == NULL) {
        current_field->screen_flag |= COB_SCREEN_LINE_PLUS;
        current_field->screen_line = cb_one;
      } else {
        if((yyvsp[(4) - (5)]) == NULL && CB_LITERAL_P((yyvsp[(5) - (5)]))){
            struct cb_literal *p =CB_LITERAL((yyvsp[(5) - (5)]));
            if (p->image[0] == '+' ) {
              current_field->screen_flag |= COB_SCREEN_LINE_PLUS;
            }
            else if( p->image[0] == '-'){
               p->sign = 1;
              current_field->screen_flag |= COB_SCREEN_LINE_MINUS;
            }
        }
        current_field->screen_line = (yyvsp[(5) - (5)]);
      }
  ;}
    break;

  case 622:

/* Line 1455 of yacc.c  */
#line 3833 "parser.y"
    {
      if ((yyvsp[(5) - (5)]) == NULL){
        current_field->screen_flag |= COB_SCREEN_COLUMN_PLUS;
        current_field->screen_column = cb_one;
      } else {
        if((yyvsp[(4) - (5)]) == NULL && CB_LITERAL_P((yyvsp[(5) - (5)]))){
            struct cb_literal *p = CB_LITERAL((yyvsp[(5) - (5)]));
            if(p->image[0] == '+'){
               current_field->screen_flag |= COB_SCREEN_COLUMN_PLUS;
            }
            else if (p->image[0] == '-'){
               p->sign = 1;
              current_field->screen_flag |= COB_SCREEN_COLUMN_MINUS;         
            }
        }
        current_field->screen_column = (yyvsp[(5) - (5)]);
      }
  ;}
    break;

  case 623:

/* Line 1455 of yacc.c  */
#line 3852 "parser.y"
    {
        current_field->screen_foreg = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 624:

/* Line 1455 of yacc.c  */
#line 3856 "parser.y"
    {
        current_field->screen_backg = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 632:

/* Line 1455 of yacc.c  */
#line 3867 "parser.y"
    {
        current_field->screen_from = (yyvsp[(2) - (2)]);
        current_field->screen_to = (yyvsp[(2) - (2)]);
        current_field->screen_flag |= COB_SCREEN_PROMPT;
        current_field->screen_flag |= COB_SCREEN_INPUT;
  ;}
    break;

  case 633:

/* Line 1455 of yacc.c  */
#line 3874 "parser.y"
    {
        current_field->screen_from = (yyvsp[(2) - (2)]);
  ;}
    break;

  case 634:

/* Line 1455 of yacc.c  */
#line 3878 "parser.y"
    {
        current_field->screen_to = (yyvsp[(2) - (2)]);
        current_field->screen_flag |= COB_SCREEN_PROMPT;
        current_field->screen_flag |= COB_SCREEN_INPUT;
  ;}
    break;

  case 635:

/* Line 1455 of yacc.c  */
#line 3884 "parser.y"
    {
    current_field->screen_control = (yyvsp[(3) - (3)]);

  ;}
    break;

  case 636:

/* Line 1455 of yacc.c  */
#line 3889 "parser.y"
    {
    current_field->screen_input_size = (yyvsp[(3) - (3)]);

  ;}
    break;

  case 637:

/* Line 1455 of yacc.c  */
#line 3897 "parser.y"
    {
        /* Nothing */
      (yyval) = NULL;
  ;}
    break;

  case 638:

/* Line 1455 of yacc.c  */
#line 3902 "parser.y"
    {
    current_field->screen_flag |= COB_SCREEN_LINE_PLUS;
  ;}
    break;

  case 639:

/* Line 1455 of yacc.c  */
#line 3906 "parser.y"
    {
        current_field->screen_flag |= COB_SCREEN_LINE_PLUS;
  ;}
    break;

  case 640:

/* Line 1455 of yacc.c  */
#line 3910 "parser.y"
    {
        current_field->screen_flag |= COB_SCREEN_LINE_MINUS;
  ;}
    break;

  case 641:

/* Line 1455 of yacc.c  */
#line 3914 "parser.y"
    {
    current_field->screen_flag |= COB_SCREEN_LINE_MINUS;
  ;}
    break;

  case 642:

/* Line 1455 of yacc.c  */
#line 3921 "parser.y"
    {
      (yyval) = NULL;
  ;}
    break;

  case 643:

/* Line 1455 of yacc.c  */
#line 3925 "parser.y"
    {
    current_field->screen_flag |= COB_SCREEN_COLUMN_PLUS;
  ;}
    break;

  case 644:

/* Line 1455 of yacc.c  */
#line 3929 "parser.y"
    {
        current_field->screen_flag |= COB_SCREEN_COLUMN_PLUS;
  ;}
    break;

  case 645:

/* Line 1455 of yacc.c  */
#line 3933 "parser.y"
    {
    current_field->screen_flag |= COB_SCREEN_COLUMN_MINUS;
  ;}
    break;

  case 646:

/* Line 1455 of yacc.c  */
#line 3937 "parser.y"
    {
        current_field->screen_flag |= COB_SCREEN_COLUMN_MINUS;
  ;}
    break;

  case 647:

/* Line 1455 of yacc.c  */
#line 3945 "parser.y"
    {
        current_field->occurs_max = cb_get_int ((yyvsp[(2) - (3)]));
    current_field->occurs_min = current_field->occurs_max;
        current_field->indexes++;
        current_field->flag_occurs = 1;
  ;}
    break;

  case 649:

/* Line 1455 of yacc.c  */
#line 3959 "parser.y"
    {
        current_section = NULL;
        current_paragraph = NULL;
        cb_define_system_name ("CONSOLE");
        cb_define_system_name ("SYSIN");
        cb_define_system_name ("SYSOUT");
        cb_define_system_name ("SYSERR");
        cb_set_in_procedure ();
  ;}
    break;

  case 650:

/* Line 1455 of yacc.c  */
#line 3969 "parser.y"
    {       
         
        current_program->gen_params_protection=cb_flag_protect_parameters;
        if (current_program->flag_main && !current_program->flag_chained && (yyvsp[(4) - (8)])) {
                cb_warning (_("Executable program requested but PROCEDURE/ENTRY has USING clause"));
                current_program->gen_params_protection=1;
        }
        emit_entry (current_program->program_id, 0, (yyvsp[(4) - (8)]), cb_merge_call_conv((yyvsp[(3) - (8)]), NULL), 1); /* main entry point */
        if (cb_flag_module_name_entry) {
            if ((current_program->next_program == NULL) && source_demangle_name && 
                (strcasecmp(source_demangle_name, current_program->program_id) != 0)) {                
                    emit_entry (source_demangle_name, 0, (yyvsp[(4) - (8)]), cb_merge_call_conv((yyvsp[(3) - (8)]), NULL), 1); /* Entry point using object name*/
            }
        }
        if (cb_initcall_list) {
            struct cb_text_list *l;

            BEGIN_STATEMENT_1("INITCALL",0,0);
            for (l=cb_initcall_list; l; l = l->next) {
               int lt = strlen(l->text) ;
               cb_emit_call(cb_build_alphanumeric_literal((unsigned char*)l->text, lt,0), 0,
                            NULL, NULL, NULL, NULL);
            }
        }
  ;}
    break;

  case 651:

/* Line 1455 of yacc.c  */
#line 3995 "parser.y"
    {
        if (current_paragraph) {
                if (current_paragraph->exit_label) {
                        emit_statement (current_paragraph->exit_label);
                }
                emit_statement (cb_build_perform_exit (current_paragraph));
        }
        if (current_section) {
                if (current_section->exit_label) {
                        emit_statement (current_section->exit_label);
                }
                emit_statement (cb_build_perform_exit (current_section));
        }
  ;}
    break;

  case 652:

/* Line 1455 of yacc.c  */
#line 4012 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 653:

/* Line 1455 of yacc.c  */
#line 4014 "parser.y"
    {
    call_mode = CB_CALL_BY_REFERENCE;
    size_mode = CB_SIZE_4;
  ;}
    break;

  case 654:

/* Line 1455 of yacc.c  */
#line 4019 "parser.y"
    { 
      (yyval) = cb_validate_procedure_param_list((yyvsp[(3) - (3)]));
  ;}
    break;

  case 655:

/* Line 1455 of yacc.c  */
#line 4023 "parser.y"
    {
    call_mode = CB_CALL_BY_REFERENCE;
    current_program->flag_chained = 1;
  ;}
    break;

  case 656:

/* Line 1455 of yacc.c  */
#line 4028 "parser.y"
    { 
        (yyval) = (yyvsp[(3) - (3)]); 

  ;}
    break;

  case 657:

/* Line 1455 of yacc.c  */
#line 4035 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 658:

/* Line 1455 of yacc.c  */
#line 4037 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 659:

/* Line 1455 of yacc.c  */
#line 4042 "parser.y"
    {
    (yyval) = cb_build_pair (cb_int (call_mode), cb_build_identifier ((yyvsp[(4) - (4)])));
    CB_SIZES ((yyval)) = size_mode;
  ;}
    break;

  case 661:

/* Line 1455 of yacc.c  */
#line 4051 "parser.y"
    {
    call_mode = CB_CALL_BY_REFERENCE;
  ;}
    break;

  case 662:

/* Line 1455 of yacc.c  */
#line 4055 "parser.y"
    {
    if (current_program->flag_chained) {
        cb_error (_("BY VALUE not allowed in CHAINED program"));
    } else {
        call_mode = CB_CALL_BY_VALUE;
    }
  ;}
    break;

  case 663:

/* Line 1455 of yacc.c  */
#line 4063 "parser.y"
    {
    call_mode = CB_CALL_BY_DESCRIPTOR;
  ;}
    break;

  case 664:

/* Line 1455 of yacc.c  */
#line 4070 "parser.y"
    {
        size_mode &= ~CB_SIZE_DELIMITED;
  ;}
    break;

  case 665:

/* Line 1455 of yacc.c  */
#line 4074 "parser.y"
    {
    if (!current_program->flag_is_external) {
        cb_error (_("DELIMITED only availaible for external ENTRY definition"));
    } else {
        size_mode |= CB_SIZE_DELIMITED;
    }
  ;}
    break;

  case 667:

/* Line 1455 of yacc.c  */
#line 4086 "parser.y"
    {
    if (CB_CALL_BY(call_mode) != CB_CALL_BY_VALUE) {
        cb_error (_("SIZE only allowed for BY VALUE items"));
    } else {
        size_mode = CB_SIZE_AUTO;
    }
  ;}
    break;

  case 668:

/* Line 1455 of yacc.c  */
#line 4094 "parser.y"
    {
    if (CB_CALL_BY(call_mode) != CB_CALL_BY_VALUE) {
        cb_error (_("SIZE only allowed for BY VALUE items"));
    } else {
        size_mode = CB_SIZE_4;
    }
  ;}
    break;

  case 669:

/* Line 1455 of yacc.c  */
#line 4102 "parser.y"
    {
    unsigned char *s = CB_LITERAL ((yyvsp[(4) - (4)]))->data;

    if (CB_CALL_BY(call_mode) != CB_CALL_BY_VALUE) {
        cb_error (_("SIZE only allowed for BY VALUE items"));
    } else if (CB_LITERAL ((yyvsp[(4) - (4)]))->size != 1) {
        cb_error_x ((yyvsp[(4) - (4)]), _("Invalid value for SIZE"));
    } else {
        size_mode = CB_SIZE_UNSIGNED;
        switch (*s) {
        case '1':
            size_mode |= CB_SIZE_1;
            break;
        case '2':
            size_mode |= CB_SIZE_2;
            break;
        case '4':
            size_mode |= CB_SIZE_4;
            break;
        case '8':
            size_mode |= CB_SIZE_8;
            break;
        default:
            cb_error_x ((yyvsp[(4) - (4)]), _("Invalid value for SIZE"));
            break;
        }
    }
  ;}
    break;

  case 670:

/* Line 1455 of yacc.c  */
#line 4131 "parser.y"
    {
    unsigned char *s = CB_LITERAL ((yyvsp[(3) - (3)]))->data;

    if (CB_CALL_BY(call_mode) != CB_CALL_BY_VALUE) {
        cb_error (_("SIZE only allowed for BY VALUE items"));
    } else if (CB_LITERAL ((yyvsp[(3) - (3)]))->size != 1) {
        cb_error_x ((yyvsp[(3) - (3)]), _("Invalid value for SIZE"));
    } else {
        size_mode = 0;
        switch (*s) {
        case '1':
            size_mode = CB_SIZE_1;
            break;
        case '2':
            size_mode = CB_SIZE_2;
            break;
        case '4':
            size_mode = CB_SIZE_4;
            break;
        case '8':
            size_mode = CB_SIZE_8;
            break;
        default:
            cb_error_x ((yyvsp[(3) - (3)]), _("Invalid value for SIZE"));
            break;
        }
    }
  ;}
    break;

  case 672:

/* Line 1455 of yacc.c  */
#line 4164 "parser.y"
    {
    if (CB_CALL_BY(call_mode) != CB_CALL_BY_REFERENCE && CB_CALL_BY(call_mode) != CB_CALL_BY_DEFAULT) {
        cb_error (_("OPTIONAL only allowed for BY REFERENCE items"));
    }
  ;}
    break;

  case 673:

/* Line 1455 of yacc.c  */
#line 4173 "parser.y"
    {
    if (current_program->prog_type == CB_FUNCTION_TYPE) {
        cb_error (_("RETURNING clause is required for a FUNCTION"));
    }
  ;}
    break;

  case 674:

/* Line 1455 of yacc.c  */
#line 4179 "parser.y"
    {
    if (cb_ref ((yyvsp[(2) - (2)])) != cb_error_node) {
        current_program->returning = (yyvsp[(2) - (2)]);
        if (cb_field ((yyvsp[(2) - (2)]))->storage != CB_STORAGE_LINKAGE) {
            cb_error (_("RETURNING item is not defined in LINKAGE SECTION"));
        }
    }
  ;}
    break;

  case 676:

/* Line 1455 of yacc.c  */
#line 4190 "parser.y"
    { in_declaratives = 1; ;}
    break;

  case 677:

/* Line 1455 of yacc.c  */
#line 4193 "parser.y"
    {
        in_declaratives = 0;
        if (current_paragraph) {
                if (current_paragraph->exit_label) {
                        emit_statement (current_paragraph->exit_label);
                }
                emit_statement (cb_build_perform_exit (current_paragraph));
                current_paragraph = NULL;
        }
        if (current_section) {
                if (current_section->exit_label) {
                        emit_statement (current_section->exit_label);
                }
                emit_statement (cb_build_perform_exit (current_section));
                current_section = NULL;
        }
  ;}
    break;

  case 684:

/* Line 1455 of yacc.c  */
#line 4228 "parser.y"
    {
        if (next_label_list) {
                cb_tree label;
                char name[16];

                sprintf (name, "L$%d", next_label_id);
                label = cb_build_reference (name);
                emit_statement (cb_build_label (label, NULL));
                current_program->label_list =
                        cb_list_append (current_program->label_list, next_label_list);
                next_label_list = NULL;
                next_label_id++;
        }
        if ((yyvsp[(2) - (2)]) == cb_int1){

        /* check_unreached = 0; */
        }
  ;}
    break;

  case 685:

/* Line 1455 of yacc.c  */
#line 4247 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 686:

/* Line 1455 of yacc.c  */
#line 4251 "parser.y"
    {
    /* check_unreached = 0; */
  ;}
    break;

  case 687:

/* Line 1455 of yacc.c  */
#line 4263 "parser.y"
    {
    non_const_word = 0;
    check_unreached = 0;
        if ((yyvsp[(1) - (4)]) == cb_error_node) {
                YYERROR;
        }
         
        if ((yyvsp[(1) - (4)])->source_column > 4) cb_check_feature_x ((yyvsp[(1) - (4)]), cb_syntax_ibm5_2, "SECTION name must be at COLUMN 8 to 11");
        /* Exit the last section */
        if (current_paragraph) {
                if (current_paragraph->exit_label) {
                        emit_statement (current_paragraph->exit_label);
                }
                emit_statement (cb_build_perform_exit (current_paragraph));
        }
        if (current_section) {
                if (current_section->exit_label) {
                        emit_statement (current_section->exit_label);
                }
                emit_statement (cb_build_perform_exit (current_section));
        }

        /* Begin a new section */
        current_section = CB_LABEL (cb_build_label ((yyvsp[(1) - (4)]), NULL));
        current_section->is_section = 1;
        current_paragraph = NULL;
        emit_statement (CB_TREE (current_section));
        current_program->all_label_list = cb_cons(CB_TREE(current_section),current_program->all_label_list);
  ;}
    break;

  case 688:

/* Line 1455 of yacc.c  */
#line 4296 "parser.y"
    {
        cb_tree label;

        non_const_word = 0;
        check_unreached = 0;
        (yyval) = cb_build_section_name ((yyvsp[(1) - (1)]), 1);
        /* if ($1 == cb_error_node) */
        if ((yyval) == cb_error_node) {
                YYERROR;
        }
        if ((yyvsp[(1) - (1)])->source_column > 4) cb_check_feature_x ((yyvsp[(1) - (1)]), cb_syntax_ibm5_2, "PARAGRAPH name must be at COLUMN 8 to 11");

        /* Exit the last paragraph */
        if (current_paragraph) {
                if (current_paragraph->exit_label) {
                        emit_statement (current_paragraph->exit_label);
                }
                emit_statement (cb_build_perform_exit (current_paragraph));
        }

        /* Begin a new paragraph */
        if (!current_section) {
                label = cb_build_reference ("MAIN SECTION");
                current_section = CB_LABEL (cb_build_label (label, NULL));
                current_section->is_section = 1;
                emit_statement (CB_TREE (current_section));
        }
        current_paragraph = CB_LABEL (cb_build_label ((yyval), current_section));
        if (current_section) {
                current_section->children =
                        cb_cons (CB_TREE (current_paragraph), current_section->children);
        }
        emit_statement (CB_TREE (current_paragraph));
        current_program->all_label_list = cb_cons(CB_TREE (current_paragraph),current_program->all_label_list);
  ;}
    break;

  case 690:

/* Line 1455 of yacc.c  */
#line 4339 "parser.y"
    {
    non_const_word = 0;
    check_unreached = 0;
    (yyval) = cb_build_section_name ((yyvsp[(1) - (1)]), 0);
        if ((yyval) != cb_error_node) {
                cb_error_x ((yyvsp[(1) - (1)]), _("Unknown statement '%s'"), CB_NAME ((yyvsp[(1) - (1)])));
        }
        YYERROR;
  ;}
    break;

  case 691:

/* Line 1455 of yacc.c  */
#line 4351 "parser.y"
    { (yyval) = cb_build_section_name ((yyvsp[(1) - (1)]), 0); ;}
    break;

  case 692:

/* Line 1455 of yacc.c  */
#line 4352 "parser.y"
    { (yyval) = cb_build_section_name ((yyvsp[(2) - (2)]), 0); ;}
    break;

  case 694:

/* Line 1455 of yacc.c  */
#line 4356 "parser.y"
    { /* ignore */ ;}
    break;

  case 695:

/* Line 1455 of yacc.c  */
#line 4365 "parser.y"
    {
        (yyval) = current_program->exec_list;
        current_program->exec_list = NULL;
  ;}
    break;

  case 696:

/* Line 1455 of yacc.c  */
#line 4369 "parser.y"
    {
        (yyval) = CB_TREE (current_statement);
        current_statement = NULL;
  ;}
    break;

  case 697:

/* Line 1455 of yacc.c  */
#line 4374 "parser.y"
    {
        (yyval) = cb_list_reverse (current_program->exec_list);
        current_program->exec_list = (yyvsp[(1) - (3)]);
        current_statement = CB_STATEMENT ((yyvsp[(2) - (3)]));
  ;}
    break;

  case 698:

/* Line 1455 of yacc.c  */
#line 4382 "parser.y"
    { (yyval)=cb_list_init(cb_build_continue ()); ;}
    break;

  case 700:

/* Line 1455 of yacc.c  */
#line 4387 "parser.y"
    {
        cb_tree label;

        if (!current_section) {
                label = cb_build_reference ("MAIN SECTION");
                current_section = CB_LABEL (cb_build_label (label, NULL));
                current_section->is_section = 1;
                emit_statement (CB_TREE (current_section));
        }
        if (!current_paragraph) {
                label = cb_build_reference ("MAIN PARAGRAPH");
                current_paragraph = CB_LABEL (cb_build_label (label, NULL));
                emit_statement (CB_TREE (current_paragraph));
                current_section->children =
                        cb_cons (CB_TREE (current_paragraph), current_section->children);
        }
  ;}
    break;

  case 758:

/* Line 1455 of yacc.c  */
#line 4465 "parser.y"
    {
        if (cb_verify (cb_next_sentence_phrase, "NEXT SENTENCE")) {
                cb_tree label;
                char    name[16];

                BEGIN_STATEMENT ("NEXT SENTENCE", 0);
                sprintf (name, "L$%d", next_label_id);
                label = cb_build_reference (name);
                next_label_list = cb_list_add (next_label_list, label);
                emit_statement (cb_build_goto (label, NULL, NULL));
        }
    check_unreached = 0;
  ;}
    break;

  case 759:

/* Line 1455 of yacc.c  */
#line 4487 "parser.y"
    {
        BEGIN_STATEMENT ("ACCEPT", TERM_ACCEPT);
        dispattrs = 0;
        if (cb_flag_accept_with_update)
            dispattrs |= COB_SCREEN_UPDATE;
        if (cb_flag_accept_with_auto)
            dispattrs |= COB_SCREEN_AUTO;
        dispprompt = 0;
        display_fgc = NULL;
        display_bgc = NULL;
        display_line = NULL;
        display_column = NULL;
        scroll = NULL;
        acc_exception_field = NULL;
        acc_timeout = NULL;
  ;}
    break;

  case 761:

/* Line 1455 of yacc.c  */
#line 4509 "parser.y"
    {
    cb_emit_accept ((yyvsp[(1) - (3)]), display_line, display_column, display_fgc, display_bgc, scroll, dispattrs, dispprompt, acc_exception_field, acc_timeout);
  ;}
    break;

  case 762:

/* Line 1455 of yacc.c  */
#line 4513 "parser.y"
    {
    cb_emit_accept_escape((yyvsp[(1) - (4)]));
  ;}
    break;

  case 763:

/* Line 1455 of yacc.c  */
#line 4517 "parser.y"
    {
    cb_emit_accept_line_or_col ((yyvsp[(1) - (3)]), 0);
  ;}
    break;

  case 764:

/* Line 1455 of yacc.c  */
#line 4521 "parser.y"
    {
    cb_emit_accept_line_or_col ((yyvsp[(1) - (3)]), 1);
  ;}
    break;

  case 765:

/* Line 1455 of yacc.c  */
#line 4525 "parser.y"
    {
    cb_emit_accept_date ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 766:

/* Line 1455 of yacc.c  */
#line 4529 "parser.y"
    {
    cb_emit_accept_date_yyyymmdd ((yyvsp[(1) - (4)]));
  ;}
    break;

  case 767:

/* Line 1455 of yacc.c  */
#line 4533 "parser.y"
    {
    cb_emit_accept_date_yyyymmdd ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 768:

/* Line 1455 of yacc.c  */
#line 4537 "parser.y"
    {
    cb_emit_accept_day ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 769:

/* Line 1455 of yacc.c  */
#line 4541 "parser.y"
    {
    cb_emit_accept_day_yyyyddd ((yyvsp[(1) - (4)]));
  ;}
    break;

  case 770:

/* Line 1455 of yacc.c  */
#line 4545 "parser.y"
    {
    cb_emit_accept_day_yyyyddd ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 771:

/* Line 1455 of yacc.c  */
#line 4549 "parser.y"
    {
    cb_emit_accept_day_of_week ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 772:

/* Line 1455 of yacc.c  */
#line 4553 "parser.y"
    {
    cb_emit_accept_time ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 773:

/* Line 1455 of yacc.c  */
#line 4557 "parser.y"
    {
    cb_emit_accept_command_line ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 774:

/* Line 1455 of yacc.c  */
#line 4561 "parser.y"
    {
    cb_emit_accept_environment ((yyvsp[(1) - (4)]));
  ;}
    break;

  case 775:

/* Line 1455 of yacc.c  */
#line 4565 "parser.y"
    { 
        cb_emit_get_environment ((yyvsp[(4) - (5)]), (yyvsp[(1) - (5)]));
  ;}
    break;

  case 776:

/* Line 1455 of yacc.c  */
#line 4569 "parser.y"
    {
    cb_emit_accept_arg_number ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 777:

/* Line 1455 of yacc.c  */
#line 4573 "parser.y"
    {
    cb_emit_accept_arg_value ((yyvsp[(1) - (4)]));
  ;}
    break;

  case 778:

/* Line 1455 of yacc.c  */
#line 4577 "parser.y"
    {
    cb_emit_accept_mnemonic ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 779:

/* Line 1455 of yacc.c  */
#line 4581 "parser.y"
    {
    cb_emit_accept_name ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 780:

/* Line 1455 of yacc.c  */
#line 4588 "parser.y"
    { display_line = (yyvsp[(2) - (2)]); ;}
    break;

  case 781:

/* Line 1455 of yacc.c  */
#line 4589 "parser.y"
    { display_column = (yyvsp[(2) - (2)]); ;}
    break;

  case 782:

/* Line 1455 of yacc.c  */
#line 4590 "parser.y"
    { display_line = (yyvsp[(2) - (2)]); display_column = cb_low;;}
    break;

  case 783:

/* Line 1455 of yacc.c  */
#line 4595 "parser.y"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 784:

/* Line 1455 of yacc.c  */
#line 4600 "parser.y"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 785:

/* Line 1455 of yacc.c  */
#line 4601 "parser.y"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 791:

/* Line 1455 of yacc.c  */
#line 4616 "parser.y"
    { PENDING("CONVERT");;}
    break;

  case 792:

/* Line 1455 of yacc.c  */
#line 4617 "parser.y"
    { PENDING("ECHO") ;;}
    break;

  case 793:

/* Line 1455 of yacc.c  */
#line 4618 "parser.y"
    { dispattrs |= COB_SCREEN_BELL; ;}
    break;

  case 794:

/* Line 1455 of yacc.c  */
#line 4619 "parser.y"
    { dispattrs |= COB_SCREEN_NO_BELL; ;}
    break;

  case 795:

/* Line 1455 of yacc.c  */
#line 4620 "parser.y"
    { dispattrs |= COB_SCREEN_BLINK; ;}
    break;

  case 796:

/* Line 1455 of yacc.c  */
#line 4621 "parser.y"
    { dispattrs |= COB_SCREEN_ERASE_EOL; ;}
    break;

  case 797:

/* Line 1455 of yacc.c  */
#line 4622 "parser.y"
    { dispattrs |= COB_SCREEN_ERASE_EOS; ;}
    break;

  case 798:

/* Line 1455 of yacc.c  */
#line 4623 "parser.y"
    { dispattrs |= COB_SCREEN_HIGHLIGHT; ;}
    break;

  case 799:

/* Line 1455 of yacc.c  */
#line 4624 "parser.y"
    { dispattrs |= COB_SCREEN_LOWLIGHT; ;}
    break;

  case 800:

/* Line 1455 of yacc.c  */
#line 4625 "parser.y"
    { dispattrs |= COB_SCREEN_REVERSE; ;}
    break;

  case 801:

/* Line 1455 of yacc.c  */
#line 4626 "parser.y"
    { dispattrs |= COB_SCREEN_UNDERLINE; ;}
    break;

  case 802:

/* Line 1455 of yacc.c  */
#line 4627 "parser.y"
    { dispattrs |= COB_SCREEN_OVERLINE; ;}
    break;

  case 803:

/* Line 1455 of yacc.c  */
#line 4628 "parser.y"
    { dispattrs |= COB_SCREEN_UPPERCASE; ;}
    break;

  case 804:

/* Line 1455 of yacc.c  */
#line 4629 "parser.y"
    { dispattrs |= COB_SCREEN_LOWERCASE; ;}
    break;

  case 805:

/* Line 1455 of yacc.c  */
#line 4631 "parser.y"
    {
        display_fgc = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 806:

/* Line 1455 of yacc.c  */
#line 4635 "parser.y"
    {
        display_bgc = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 807:

/* Line 1455 of yacc.c  */
#line 4639 "parser.y"
    {
    scroll = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 808:

/* Line 1455 of yacc.c  */
#line 4643 "parser.y"
    {
    dispattrs |= COB_SCREEN_SCROLL_DOWN;
    scroll = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 809:

/* Line 1455 of yacc.c  */
#line 4647 "parser.y"
    { dispattrs |= COB_SCREEN_AUTO; ;}
    break;

  case 810:

/* Line 1455 of yacc.c  */
#line 4648 "parser.y"
    { dispattrs &= ~COB_SCREEN_AUTO; ;}
    break;

  case 811:

/* Line 1455 of yacc.c  */
#line 4649 "parser.y"
    { dispattrs |= COB_SCREEN_FULL; ;}
    break;

  case 812:

/* Line 1455 of yacc.c  */
#line 4650 "parser.y"
    { dispattrs |= COB_SCREEN_REQUIRED; ;}
    break;

  case 813:

/* Line 1455 of yacc.c  */
#line 4651 "parser.y"
    { dispattrs |= COB_SCREEN_SECURE; ;}
    break;

  case 814:

/* Line 1455 of yacc.c  */
#line 4652 "parser.y"
    { dispattrs |= COB_SCREEN_UPDATE; ;}
    break;

  case 815:

/* Line 1455 of yacc.c  */
#line 4653 "parser.y"
    { dispattrs |= COB_SCREEN_PROMPT; ;}
    break;

  case 816:

/* Line 1455 of yacc.c  */
#line 4655 "parser.y"
    {
        unsigned char *s = CB_LITERAL ((yyvsp[(5) - (5)]))->data;

        if (CB_LITERAL ((yyvsp[(5) - (5)]))->size != 1) {
                cb_error_x ((yyvsp[(5) - (5)]), _("Invalid PROMPT CHARACTER '%s'"), s);
        }
        dispattrs |= COB_SCREEN_PROMPT; 
        dispprompt = *s;
  ;}
    break;

  case 817:

/* Line 1455 of yacc.c  */
#line 4664 "parser.y"
    {acc_timeout = (yyvsp[(3) - (3)]);;}
    break;

  case 818:

/* Line 1455 of yacc.c  */
#line 4668 "parser.y"
    { terminator_warning (TERM_ACCEPT); ;}
    break;

  case 819:

/* Line 1455 of yacc.c  */
#line 4669 "parser.y"
    { terminator_clear (TERM_ACCEPT); ;}
    break;

  case 820:

/* Line 1455 of yacc.c  */
#line 4678 "parser.y"
    { BEGIN_STATEMENT ("ADD", TERM_ADD);rounded_used = 0; ;}
    break;

  case 822:

/* Line 1455 of yacc.c  */
#line 4685 "parser.y"
    {
   /*CIT*/          
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
     
        if (current_statement->handler1 || current_statement->handler2) {
            cb_check_optimized_field((yyvsp[(1) - (4)]), 0);
            cb_check_optimized_field((yyvsp[(3) - (4)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(3) - (4)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(3) - (4)]), '+', cb_build_binary_list ((yyvsp[(1) - (4)]), '+'), !rounded_used);
  ;}
    break;

  case 823:

/* Line 1455 of yacc.c  */
#line 4700 "parser.y"
    {
   /*CIT*/
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
        if (current_statement->handler1 || current_statement->handler2) {
            cb_check_optimized_field((yyvsp[(1) - (5)]), 0);
            cb_check_optimized_field((yyvsp[(4) - (5)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(4) - (5)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(4) - (5)]), 0, cb_build_binary_list ((yyvsp[(1) - (5)]), '+'), !rounded_used);
  ;}
    break;

  case 824:

/* Line 1455 of yacc.c  */
#line 4714 "parser.y"
    {
        void * a2, *a4;
        a2 = (yyvsp[(2) - (6)]);
        a4 = (yyvsp[(4) - (6)]);
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
        if (current_statement->handler1 || current_statement->handler2 || (yyvsp[(5) - (6)]) != cb_int0) {
            a2 = cb_check_optimized_field_1((yyvsp[(2) - (6)]));
            a4 = cb_check_optimized_field_1((yyvsp[(4) - (6)]));
        }
        cb_emit_corresponding (cb_build_add, a4, a2, (yyvsp[(5) - (6)]));
  ;}
    break;

  case 826:

/* Line 1455 of yacc.c  */
#line 4730 "parser.y"
    { cb_list_add ((yyvsp[(0) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 827:

/* Line 1455 of yacc.c  */
#line 4734 "parser.y"
    { terminator_warning (TERM_ADD); ;}
    break;

  case 828:

/* Line 1455 of yacc.c  */
#line 4735 "parser.y"
    { terminator_clear (TERM_ADD); ;}
    break;

  case 829:

/* Line 1455 of yacc.c  */
#line 4744 "parser.y"
    { BEGIN_STATEMENT ("ALLOCATE", 0); ;}
    break;

  case 831:

/* Line 1455 of yacc.c  */
#line 4750 "parser.y"
    {
        cb_emit_allocate ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), NULL, (yyvsp[(2) - (3)]));
  ;}
    break;

  case 832:

/* Line 1455 of yacc.c  */
#line 4754 "parser.y"
    {
        cb_emit_allocate (NULL, (yyvsp[(5) - (5)]), (yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]));
  ;}
    break;

  case 833:

/* Line 1455 of yacc.c  */
#line 4760 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 834:

/* Line 1455 of yacc.c  */
#line 4761 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 835:

/* Line 1455 of yacc.c  */
#line 4771 "parser.y"
    {
        //cb_error (_("ALTER statement is obsolete and unsupported"));
        { BEGIN_STATEMENT ("ALTER", 0); }
  ;}
    break;

  case 838:

/* Line 1455 of yacc.c  */
#line 4781 "parser.y"
    {
    cb_emit_alter((yyvsp[(2) - (5)]), (yyvsp[(5) - (5)]));
;}
    break;

  case 841:

/* Line 1455 of yacc.c  */
#line 4794 "parser.y"
    { 
                                  BEGIN_STATEMENT ("CALL", TERM_CALL); 
                                ;}
    break;

  case 842:

/* Line 1455 of yacc.c  */
#line 4804 "parser.y"
    {
        cb_emit_call ((yyvsp[(4) - (9)]), cb_merge_call_conv((yyvsp[(3) - (9)]), (yyvsp[(5) - (9)])), current_statement->call_using_save, 
                      current_statement->call_returning_save, (yyvsp[(7) - (9)]), (yyvsp[(8) - (9)]));
  ;}
    break;

  case 843:

/* Line 1455 of yacc.c  */
#line 4811 "parser.y"
    { BEGIN_STATEMENT ("CHAIN", TERM_CHAIN); ;}
    break;

  case 844:

/* Line 1455 of yacc.c  */
#line 4819 "parser.y"
    {
        
        cb_emit_chain ((yyvsp[(4) - (9)]), cb_merge_call_conv((yyvsp[(3) - (9)]), (yyvsp[(5) - (9)])), (yyvsp[(6) - (9)]), current_program->cb_return_code, (yyvsp[(7) - (9)]), (yyvsp[(8) - (9)]));
        cb_emit_stop_run (current_program->cb_return_code);
  ;}
    break;

  case 847:

/* Line 1455 of yacc.c  */
#line 4830 "parser.y"
    { current_statement->call_using_save = (yyvsp[(1) - (1)]); ;}
    break;

  case 848:

/* Line 1455 of yacc.c  */
#line 4831 "parser.y"
    { current_statement->call_returning_save = (yyvsp[(1) - (1)]); ;}
    break;

  case 849:

/* Line 1455 of yacc.c  */
#line 4837 "parser.y"
    {
    (yyval) = NULL; 
    if (cb_flag_defaultcall) {
        (yyval) = cb_int(cb_flag_defaultcall);
        }
    ;}
    break;

  case 851:

/* Line 1455 of yacc.c  */
#line 4848 "parser.y"
    { 
    (yyval) = NULL; 
  ;}
    break;

  case 852:

/* Line 1455 of yacc.c  */
#line 4852 "parser.y"
    {
      (yyval) = cb_build_pair ((yyvsp[(2) - (4)]), (yyvsp[(3) - (4)]));
  ;}
    break;

  case 853:

/* Line 1455 of yacc.c  */
#line 4856 "parser.y"
    {
      (yyval) = cb_build_pair (cb_int(CB_CALL_STATIC), (yyvsp[(2) - (3)]));
  ;}
    break;

  case 854:

/* Line 1455 of yacc.c  */
#line 4860 "parser.y"
    {
      (yyval) = cb_build_pair (cb_int(CB_CALL_DYNAMIC), cb_int(CB_CALL_STDCALL));
  ;}
    break;

  case 855:

/* Line 1455 of yacc.c  */
#line 4866 "parser.y"
    { (yyval) = cb_int(CB_CALL_CDECL); ;}
    break;

  case 856:

/* Line 1455 of yacc.c  */
#line 4867 "parser.y"
    { (yyval) = cb_int(CB_CALL_CDECL); ;}
    break;

  case 857:

/* Line 1455 of yacc.c  */
#line 4868 "parser.y"
    { (yyval) = cb_int(CB_CALL_STDCALL); ;}
    break;

  case 858:

/* Line 1455 of yacc.c  */
#line 4873 "parser.y"
    { (yyval) = cb_int(CB_CALL_DYNAMIC); ;}
    break;

  case 859:

/* Line 1455 of yacc.c  */
#line 4874 "parser.y"
    { (yyval) = cb_int(CB_CALL_STATIC); ;}
    break;

  case 860:

/* Line 1455 of yacc.c  */
#line 4878 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 861:

/* Line 1455 of yacc.c  */
#line 4879 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]) ; ;}
    break;

  case 862:

/* Line 1455 of yacc.c  */
#line 4884 "parser.y"
    {
    call_mode = CB_CALL_BY_DEFAULT;
    if (cb_flag_value_size_auto)
      size_mode = CB_SIZE_AUTO;
    else
      size_mode = CB_SIZE_4;
  ;}
    break;

  case 863:

/* Line 1455 of yacc.c  */
#line 4891 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 864:

/* Line 1455 of yacc.c  */
#line 4896 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 865:

/* Line 1455 of yacc.c  */
#line 4898 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 866:

/* Line 1455 of yacc.c  */
#line 4903 "parser.y"
    {
    if (CB_CALL_BY(call_mode) != CB_CALL_BY_REFERENCE && CB_CALL_BY(call_mode) != CB_CALL_BY_DEFAULT) {
                cb_error (_("OMITTED only allowed with BY REFERENCE"));
        }
    (yyval) = cb_build_pair (cb_int (call_mode), cb_null);
  ;}
    break;

  case 867:

/* Line 1455 of yacc.c  */
#line 4910 "parser.y"
    {
    (yyval) = cb_build_pair (cb_int (call_mode), (yyvsp[(3) - (5)]) );
    CB_SIZES ((yyval)) = size_mode;
  ;}
    break;

  case 869:

/* Line 1455 of yacc.c  */
#line 4919 "parser.y"
    {
    call_mode = CB_CALL_BY_REFERENCE;
  ;}
    break;

  case 870:

/* Line 1455 of yacc.c  */
#line 4923 "parser.y"
    {
    if (current_program->flag_chained) {
                cb_error (_("BY CONTENT not allowed in CHAINED program"));
        } else {
        call_mode = CB_CALL_BY_CONTENT;
    }
  ;}
    break;

  case 871:

/* Line 1455 of yacc.c  */
#line 4931 "parser.y"
    {
    if (current_program->flag_chained) {
                cb_error (_("BY DESCRIPTOR not allowed in CHAINED program"));
        } else {
        call_mode = CB_CALL_BY_DESCRIPTOR;
    }
  ;}
    break;

  case 872:

/* Line 1455 of yacc.c  */
#line 4939 "parser.y"
    {
    if (current_program->flag_chained) {
                cb_error (_("BY VALUE not allowed in CHAINED program"));
        } else {
        call_mode = CB_CALL_BY_VALUE;
    }
  ;}
    break;

  case 873:

/* Line 1455 of yacc.c  */
#line 4956 "parser.y"
    { (yyval) = cb_build_pair(cb_int0 , (yyvsp[(2) - (2)])); ;}
    break;

  case 874:

/* Line 1455 of yacc.c  */
#line 4957 "parser.y"
    { (yyval) = cb_build_pair(cb_int(CB_CALL_RETURN_INTO), (yyvsp[(3) - (3)])); ;}
    break;

  case 875:

/* Line 1455 of yacc.c  */
#line 4958 "parser.y"
    { (yyval) = cb_build_pair(cb_int(CB_CALL_RETURN_ADDROF), (yyvsp[(4) - (4)])); ;}
    break;

  case 878:

/* Line 1455 of yacc.c  */
#line 4968 "parser.y"
    {
    (yyval) = NULL;
  ;}
    break;

  case 879:

/* Line 1455 of yacc.c  */
#line 4972 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 880:

/* Line 1455 of yacc.c  */
#line 4976 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 881:

/* Line 1455 of yacc.c  */
#line 4983 "parser.y"
    {
    (yyval) = NULL;
  ;}
    break;

  case 882:

/* Line 1455 of yacc.c  */
#line 4987 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 883:

/* Line 1455 of yacc.c  */
#line 4991 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 884:

/* Line 1455 of yacc.c  */
#line 4997 "parser.y"
    { terminator_warning (TERM_CALL); ;}
    break;

  case 885:

/* Line 1455 of yacc.c  */
#line 4998 "parser.y"
    { terminator_clear (TERM_CALL); ;}
    break;

  case 886:

/* Line 1455 of yacc.c  */
#line 5002 "parser.y"
    { terminator_warning (TERM_CHAIN); ;}
    break;

  case 887:

/* Line 1455 of yacc.c  */
#line 5003 "parser.y"
    { terminator_clear (TERM_CHAIN); ;}
    break;

  case 888:

/* Line 1455 of yacc.c  */
#line 5012 "parser.y"
    { BEGIN_STATEMENT ("CANCEL", 0); ;}
    break;

  case 891:

/* Line 1455 of yacc.c  */
#line 5018 "parser.y"
    {
        cb_emit_cancel ((yyvsp[(2) - (2)]));
  ;}
    break;

  case 892:

/* Line 1455 of yacc.c  */
#line 5029 "parser.y"
    {                BEGIN_STATEMENT ("CHECKPOINT", 0); ;}
    break;

  case 894:

/* Line 1455 of yacc.c  */
#line 5035 "parser.y"
    { 
             if (!cb_flag_context_reload_enable) {
                 cb_error (_("CHECKPOINT is only valid with -fcheckpoint compilation flag"));
             }
             if ((yyvsp[(1) - (1)]))
                 cb_emit_set_context_filename ((yyvsp[(1) - (1)]));
      ;}
    break;

  case 895:

/* Line 1455 of yacc.c  */
#line 5043 "parser.y"
    {
            cb_emit_perform (cb_build_perform_once(NULL), 
                             cb_build_pair (cb_save_context_handler_ref, cb_save_context_handler_ref));
            cb_emit_move (cb_zero, cb_list_init(current_program->cb_return_code));
      ;}
    break;

  case 896:

/* Line 1455 of yacc.c  */
#line 5049 "parser.y"
    {
            if ((yyvsp[(5) - (5)])) {
                cb_emit_move (current_program->cb_context_code, cb_list_init((yyvsp[(5) - (5)])));
            }
      ;}
    break;

  case 898:

/* Line 1455 of yacc.c  */
#line 5058 "parser.y"
    {
      cb_emit_init_context_save (cb_zero);
;}
    break;

  case 899:

/* Line 1455 of yacc.c  */
#line 5062 "parser.y"
    {
      cb_emit_init_context_save (cb_one);
      cb_emit_move ((yyvsp[(2) - (2)]), cb_list_init(current_program->cb_return_code));
;}
    break;

  case 900:

/* Line 1455 of yacc.c  */
#line 5069 "parser.y"
    { (yyval) = cb_zero; ;}
    break;

  case 901:

/* Line 1455 of yacc.c  */
#line 5070 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 902:

/* Line 1455 of yacc.c  */
#line 5071 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 903:

/* Line 1455 of yacc.c  */
#line 5072 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 904:

/* Line 1455 of yacc.c  */
#line 5076 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 905:

/* Line 1455 of yacc.c  */
#line 5077 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 906:

/* Line 1455 of yacc.c  */
#line 5085 "parser.y"
    { BEGIN_STATEMENT ("CLOSE", 0); ;}
    break;

  case 909:

/* Line 1455 of yacc.c  */
#line 5092 "parser.y"
    {
        BEGIN_IMPLICIT_STATEMENT ();
    if ((yyvsp[(2) - (3)]) != cb_error_node) {
        cb_emit_close ((yyvsp[(2) - (3)]), (yyvsp[(3) - (3)]));
  }
  ;}
    break;

  case 910:

/* Line 1455 of yacc.c  */
#line 5101 "parser.y"
    { (yyval) = cb_int (COB_CLOSE_NORMAL); ;}
    break;

  case 911:

/* Line 1455 of yacc.c  */
#line 5102 "parser.y"
    { (yyval) = cb_int (COB_CLOSE_UNIT); ;}
    break;

  case 912:

/* Line 1455 of yacc.c  */
#line 5103 "parser.y"
    { (yyval) = cb_int (COB_CLOSE_UNIT_REMOVAL); ;}
    break;

  case 913:

/* Line 1455 of yacc.c  */
#line 5104 "parser.y"
    { (yyval) = cb_int (COB_CLOSE_NO_REWIND); ;}
    break;

  case 914:

/* Line 1455 of yacc.c  */
#line 5105 "parser.y"
    { (yyval) = cb_int (COB_CLOSE_LOCK); ;}
    break;

  case 917:

/* Line 1455 of yacc.c  */
#line 5116 "parser.y"
    { BEGIN_STATEMENT ("COMPUTE", TERM_COMPUTE); rounded_used = 0; ;}
    break;

  case 919:

/* Line 1455 of yacc.c  */
#line 5123 "parser.y"
    {
   /*CIT*/
        if (current_statement->handler1 || current_statement->handler2) {
            cb_check_optimized_field((yyvsp[(1) - (4)]), 0);
        }
        else 
        {
           if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_ZERO_DIVIDE)) {
               current_statement->handler_id = COB_EC_SIZE_ZERO_DIVIDE;
           } 
           if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW)) {
               if (current_statement->handler_id == 0)
                  current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
               else 
                  current_statement->handler_id = COB_EC_SIZE;
           }
           cb_check_optimized_field((yyvsp[(1) - (4)]), 1);
        }        
        cb_emit_arithmetic ((yyvsp[(1) - (4)]), 0, (yyvsp[(3) - (4)]), !rounded_used);
  ;}
    break;

  case 920:

/* Line 1455 of yacc.c  */
#line 5146 "parser.y"
    { terminator_warning (TERM_COMPUTE); ;}
    break;

  case 921:

/* Line 1455 of yacc.c  */
#line 5147 "parser.y"
    { terminator_clear (TERM_COMPUTE); ;}
    break;

  case 924:

/* Line 1455 of yacc.c  */
#line 5158 "parser.y"
    {
        BEGIN_STATEMENT ("COMMIT", 0);
        cb_emit_commit ();
  ;}
    break;

  case 925:

/* Line 1455 of yacc.c  */
#line 5171 "parser.y"
    {
        BEGIN_STATEMENT ("CONTINUE", 0);
        cb_emit_continue ();
  ;}
    break;

  case 926:

/* Line 1455 of yacc.c  */
#line 5183 "parser.y"
    { BEGIN_STATEMENT ("DELETE", TERM_DELETE); ;}
    break;

  case 927:

/* Line 1455 of yacc.c  */
#line 5186 "parser.y"
    {
    if ((yyvsp[(3) - (6)]) != cb_error_node) {
        cb_emit_delete ((yyvsp[(3) - (6)]));
  }
  ;}
    break;

  case 928:

/* Line 1455 of yacc.c  */
#line 5191 "parser.y"
    { BEGIN_STATEMENT ("DELETE", TERM_DELETE); ;}
    break;

  case 929:

/* Line 1455 of yacc.c  */
#line 5194 "parser.y"
    {
    if ((yyvsp[(3) - (5)]) != cb_error_node) {
        cb_emit_delete_file ((yyvsp[(4) - (5)]));
  }
  ;}
    break;

  case 930:

/* Line 1455 of yacc.c  */
#line 5202 "parser.y"
    { terminator_warning (TERM_DELETE); ;}
    break;

  case 931:

/* Line 1455 of yacc.c  */
#line 5203 "parser.y"
    { terminator_clear (TERM_DELETE); ;}
    break;

  case 932:

/* Line 1455 of yacc.c  */
#line 5213 "parser.y"
    {
        BEGIN_STATEMENT ("DISPLAY", TERM_DISPLAY);
        dispattrs = 0;
        dispprompt = 0;
        display_fgc = NULL;
        display_bgc = NULL;
        display_line = NULL;
        display_column = NULL;
        scroll = NULL;
        display_advance = cb_int1;
        display_x_list = NULL;
        display_upon = cb_int(COB_DEVICE_SYSOUT);
        acc_exception_field = NULL;
  ;}
    break;

  case 934:

/* Line 1455 of yacc.c  */
#line 5233 "parser.y"
    {
    cb_emit_env_name ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 935:

/* Line 1455 of yacc.c  */
#line 5237 "parser.y"
    {
    cb_emit_env_value ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 936:

/* Line 1455 of yacc.c  */
#line 5241 "parser.y"
    {
    cb_emit_arg_number ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 937:

/* Line 1455 of yacc.c  */
#line 5245 "parser.y"
    {
    cb_emit_command_line ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 938:

/* Line 1455 of yacc.c  */
#line 5249 "parser.y"
    {
    cb_emit_display (display_x_list, display_upon, display_advance, display_line, display_column, display_fgc, display_bgc, scroll, dispattrs);
  ;}
    break;

  case 941:

/* Line 1455 of yacc.c  */
#line 5274 "parser.y"
    { display_x_list = cb_list_add ( display_x_list, (yyvsp[(1) - (1)])); ;}
    break;

  case 943:

/* Line 1455 of yacc.c  */
#line 5276 "parser.y"
    { display_upon = cb_build_display_upon ((yyvsp[(2) - (2)])); ;}
    break;

  case 944:

/* Line 1455 of yacc.c  */
#line 5277 "parser.y"
    { display_upon = cb_build_display_upon_direct ((yyvsp[(2) - (2)])); ;}
    break;

  case 945:

/* Line 1455 of yacc.c  */
#line 5278 "parser.y"
    { display_upon = cb_int(COB_DEVICE_PRINTER); ;}
    break;

  case 946:

/* Line 1455 of yacc.c  */
#line 5279 "parser.y"
    { display_upon = cb_flag_console_equal_sysfile ? cb_int(COB_DEVICE_SYSOUT) : cb_int(COB_DEVICE_CONSOLE); ;}
    break;

  case 947:

/* Line 1455 of yacc.c  */
#line 5280 "parser.y"
    { display_advance = cb_int0; ;}
    break;

  case 948:

/* Line 1455 of yacc.c  */
#line 5281 "parser.y"
    { cb_warning (_("CONVERT ignored in DISPLAY/ACCEPT"));;}
    break;

  case 949:

/* Line 1455 of yacc.c  */
#line 5282 "parser.y"
    { dispattrs |= COB_SCREEN_BELL; ;}
    break;

  case 950:

/* Line 1455 of yacc.c  */
#line 5283 "parser.y"
    { dispattrs |= COB_SCREEN_NO_BELL; ;}
    break;

  case 951:

/* Line 1455 of yacc.c  */
#line 5284 "parser.y"
    { dispattrs |= COB_SCREEN_BLINK; ;}
    break;

  case 952:

/* Line 1455 of yacc.c  */
#line 5285 "parser.y"
    { dispattrs |= COB_SCREEN_ERASE_EOL; ;}
    break;

  case 953:

/* Line 1455 of yacc.c  */
#line 5286 "parser.y"
    { dispattrs |= COB_SCREEN_ERASE_EOS; ;}
    break;

  case 954:

/* Line 1455 of yacc.c  */
#line 5287 "parser.y"
    { dispattrs |= COB_SCREEN_HIGHLIGHT; ;}
    break;

  case 955:

/* Line 1455 of yacc.c  */
#line 5288 "parser.y"
    { dispattrs |= COB_SCREEN_LOWLIGHT; ;}
    break;

  case 956:

/* Line 1455 of yacc.c  */
#line 5289 "parser.y"
    { dispattrs |= COB_SCREEN_REVERSE; ;}
    break;

  case 957:

/* Line 1455 of yacc.c  */
#line 5290 "parser.y"
    { dispattrs |= COB_SCREEN_UNDERLINE; ;}
    break;

  case 958:

/* Line 1455 of yacc.c  */
#line 5291 "parser.y"
    { dispattrs |= COB_SCREEN_OVERLINE; ;}
    break;

  case 959:

/* Line 1455 of yacc.c  */
#line 5293 "parser.y"
    {
        display_fgc = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 960:

/* Line 1455 of yacc.c  */
#line 5297 "parser.y"
    {
        display_bgc = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 961:

/* Line 1455 of yacc.c  */
#line 5301 "parser.y"
    {
    scroll = (yyvsp[(3) - (4)]);
  ;}
    break;

  case 962:

/* Line 1455 of yacc.c  */
#line 5305 "parser.y"
    {
    dispattrs |= COB_SCREEN_SCROLL_DOWN;
    scroll = (yyvsp[(3) - (4)]);
  ;}
    break;

  case 963:

/* Line 1455 of yacc.c  */
#line 5309 "parser.y"
    { dispattrs |= COB_SCREEN_BLANK_LINE; ;}
    break;

  case 964:

/* Line 1455 of yacc.c  */
#line 5310 "parser.y"
    { dispattrs |= COB_SCREEN_BLANK_SCREEN; ;}
    break;

  case 965:

/* Line 1455 of yacc.c  */
#line 5314 "parser.y"
    { terminator_warning (TERM_DISPLAY); ;}
    break;

  case 966:

/* Line 1455 of yacc.c  */
#line 5315 "parser.y"
    { terminator_clear (TERM_DISPLAY); ;}
    break;

  case 967:

/* Line 1455 of yacc.c  */
#line 5324 "parser.y"
    { BEGIN_STATEMENT ("DIVIDE", TERM_DIVIDE); 
                                  rounded_used = 0;   
                                ;}
    break;

  case 969:

/* Line 1455 of yacc.c  */
#line 5333 "parser.y"
    {
   /*CIT*/
        void *a1 = (yyvsp[(1) - (4)]);
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_ZERO_DIVIDE)) {
            current_statement->handler_id = COB_EC_SIZE_ZERO_DIVIDE;
        }

        if (current_statement->handler1 || current_statement->handler2) {
            a1 = cb_check_optimized_field_1((yyvsp[(1) - (4)]));
            cb_check_optimized_field((yyvsp[(3) - (4)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(3) - (4)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(3) - (4)]), '/', a1, !rounded_used);
  ;}
    break;

  case 970:

/* Line 1455 of yacc.c  */
#line 5349 "parser.y"
    {
   /*CIT*/
        void * a1 = (yyvsp[(1) - (6)]);
        void * a3 = (yyvsp[(3) - (6)]);
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_ZERO_DIVIDE)) {
            current_statement->handler_id = COB_EC_SIZE_ZERO_DIVIDE;
        }
        if (current_statement->handler1 || current_statement->handler2) {
            a1 = cb_check_optimized_field_1((yyvsp[(1) - (6)]));
            a3 = cb_check_optimized_field_1((yyvsp[(3) - (6)]));
            cb_check_optimized_field((yyvsp[(5) - (6)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(5) - (6)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(5) - (6)]), 0, cb_build_binary_op (a3, '/', a1), !rounded_used);
  ;}
    break;

  case 971:

/* Line 1455 of yacc.c  */
#line 5366 "parser.y"
    {
        void * a1 = (yyvsp[(1) - (6)]);
        void * a3 = (yyvsp[(3) - (6)]);
   /*CIT*/
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_ZERO_DIVIDE)) {
            current_statement->handler_id = COB_EC_SIZE_ZERO_DIVIDE;
        }
        if (current_statement->handler1 || current_statement->handler2) {
            a1 = cb_check_optimized_field_1((yyvsp[(1) - (6)]));
            a3 = cb_check_optimized_field_1((yyvsp[(3) - (6)]));
            cb_check_optimized_field((yyvsp[(5) - (6)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(5) - (6)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(5) - (6)]), 0, cb_build_binary_op (a1, '/', a3), !rounded_used);
  ;}
    break;

  case 972:

/* Line 1455 of yacc.c  */
#line 5383 "parser.y"
    {
        void * a1;
        void * a3;
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_ZERO_DIVIDE)) {
            current_statement->handler_id = COB_EC_SIZE_ZERO_DIVIDE;
        }
        a1 = cb_check_optimized_field_1((yyvsp[(1) - (8)]));
        a3 = cb_check_optimized_field_1((yyvsp[(3) - (8)]));
        cb_check_optimized_field((yyvsp[(5) - (8)]), 0);
        cb_check_optimized_field((yyvsp[(7) - (8)]), 0);
        cb_emit_divide (a3, a1, (yyvsp[(5) - (8)]), (yyvsp[(7) - (8)]));
  ;}
    break;

  case 973:

/* Line 1455 of yacc.c  */
#line 5396 "parser.y"
    {
        void * a1;
        void * a3;
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_ZERO_DIVIDE)) {
            current_statement->handler_id = COB_EC_SIZE_ZERO_DIVIDE;
        }
        a1 = cb_check_optimized_field_1((yyvsp[(1) - (8)]));
        a3 = cb_check_optimized_field_1((yyvsp[(3) - (8)]));
        cb_check_optimized_field((yyvsp[(5) - (8)]), 0);
        cb_check_optimized_field((yyvsp[(7) - (8)]), 0);
        cb_emit_divide (a1, a3, (yyvsp[(5) - (8)]), (yyvsp[(7) - (8)]));
  ;}
    break;

  case 974:

/* Line 1455 of yacc.c  */
#line 5411 "parser.y"
    { terminator_warning (TERM_DIVIDE); ;}
    break;

  case 975:

/* Line 1455 of yacc.c  */
#line 5412 "parser.y"
    { terminator_clear (TERM_DIVIDE); ;}
    break;

  case 976:

/* Line 1455 of yacc.c  */
#line 5421 "parser.y"
    { BEGIN_STATEMENT ("ENTRY", 0); ;}
    break;

  case 977:

/* Line 1455 of yacc.c  */
#line 5423 "parser.y"
    {
    if (current_program->nested_level) {
        cb_error (_("ENTRY is invalid in nested program"));
    } else if (cb_verify (cb_entry_statement, "ENTRY")) {
        if (cobc_check_valid_name ((char *)(CB_LITERAL ((yyvsp[(3) - (6)]))->data))) {
                        cb_error (_("ENTRY '%s' invalid, may be a reserved C language word or function name."), (char *)(CB_LITERAL ((yyvsp[(3) - (6)]))->data));
                }
                emit_entry ((char *)(CB_LITERAL ((yyvsp[(3) - (6)]))->data), 1, (yyvsp[(5) - (6)]), cb_merge_call_conv((yyvsp[(4) - (6)]), NULL), 0);
        }
    check_unreached = 0;
  ;}
    break;

  case 978:

/* Line 1455 of yacc.c  */
#line 5437 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 979:

/* Line 1455 of yacc.c  */
#line 5438 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]) ; ;}
    break;

  case 980:

/* Line 1455 of yacc.c  */
#line 5443 "parser.y"
    {
    call_mode = CB_CALL_BY_REFERENCE;
    size_mode = CB_SIZE_4;
  ;}
    break;

  case 981:

/* Line 1455 of yacc.c  */
#line 5447 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 982:

/* Line 1455 of yacc.c  */
#line 5452 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 983:

/* Line 1455 of yacc.c  */
#line 5454 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 984:

/* Line 1455 of yacc.c  */
#line 5459 "parser.y"
    {
    if (CB_CALL_BY(call_mode) != CB_CALL_BY_REFERENCE) {
                cb_error (_("OMITTED only allowed with BY REFERENCE"));
        }
    (yyval) = cb_build_pair (cb_int (call_mode), cb_null);
  ;}
    break;

  case 985:

/* Line 1455 of yacc.c  */
#line 5466 "parser.y"
    {
    cb_tree x = NULL;
    if (current_program->flag_is_external) {
       struct cb_field *f = cb_type((yyvsp[(3) - (5)]));
       if (f && f->flag_is_typedef) {
            x = CB_TREE(f);
       } 
    } else {
        x = cb_build_identifier((yyvsp[(3) - (5)]));
    }
    if (!x)
        cb_error (_("Invalid entry parameter "));
    (yyval) = cb_build_pair (cb_int (call_mode), x );
    CB_SIZES ((yyval)) = size_mode;
  ;}
    break;

  case 986:

/* Line 1455 of yacc.c  */
#line 5482 "parser.y"
    {
    if (!current_program->flag_is_external) {
                cb_error (_("ANY only allowed with program-id is external declaration "));
    }
    (yyval) = cb_build_pair (cb_int (call_mode), cb_null );
    CB_SIZES ((yyval)) = size_mode;
  ;}
    break;

  case 987:

/* Line 1455 of yacc.c  */
#line 5492 "parser.y"
    { (yyval) = NULL;;}
    break;

  case 988:

/* Line 1455 of yacc.c  */
#line 5494 "parser.y"
    {
    cb_tree x = NULL;
    struct cb_field *f = cb_type((yyvsp[(2) - (2)]));
    if (f && f->flag_is_typedef) {
        if (current_program->flag_is_external) {
            if (f->size > sizeof(void*)) {
                cb_error (_("RETURNING size not supported"));
            } else {
                x = CB_TREE(f);
            }
        }
    } 
    if (!x)
        cb_error (_("Invalid RETURNING : ENTRY only support TYPE returning "));
    (yyval) = x;
  ;}
    break;

  case 989:

/* Line 1455 of yacc.c  */
#line 5518 "parser.y"
    {
        BEGIN_STATEMENT ("EVALUATE", TERM_EVALUATE);
        eval_level++;
        for (eval_inc = 0; eval_inc < 64; eval_inc++) {
                eval_check[eval_level][eval_inc] = 0;
        }
        eval_inc = 0;
        eval_inc2 = 0;
  ;}
    break;

  case 990:

/* Line 1455 of yacc.c  */
#line 5529 "parser.y"
    {
        cb_emit_evaluate ((yyvsp[(3) - (5)]), (yyvsp[(4) - (5)]));
        eval_level--;
  ;}
    break;

  case 991:

/* Line 1455 of yacc.c  */
#line 5536 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 992:

/* Line 1455 of yacc.c  */
#line 5538 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 993:

/* Line 1455 of yacc.c  */
#line 5543 "parser.y"
    {
        (yyval) = (yyvsp[(1) - (1)]);
        /*eval_subject[eval_level][eval_inc] = $1;*/
        if (CB_REFERENCE_P((yyvsp[(1) - (1)]))) {
                eval_check[eval_level][eval_inc++] = 0;
        } else {
                eval_check[eval_level][eval_inc++] = 1;
        }
  ;}
    break;

  case 994:

/* Line 1455 of yacc.c  */
#line 5553 "parser.y"
    {
        (yyval) = cb_true;
        /*eval_subject[eval_level][eval_inc] = cb_true;*/
        eval_check[eval_level][eval_inc++] = 2;
  ;}
    break;

  case 995:

/* Line 1455 of yacc.c  */
#line 5559 "parser.y"
    {
        (yyval) = cb_false;
        /*eval_subject[eval_level][eval_inc] = cb_false;*/
        eval_check[eval_level][eval_inc++] = 3;
  ;}
    break;

  case 996:

/* Line 1455 of yacc.c  */
#line 5577 "parser.y"
    {
    (yyval) = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 997:

/* Line 1455 of yacc.c  */
#line 5583 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 998:

/* Line 1455 of yacc.c  */
#line 5585 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 999:

/* Line 1455 of yacc.c  */
#line 5590 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1000:

/* Line 1455 of yacc.c  */
#line 5594 "parser.y"
    {
    if (cb_list_length((yyvsp[(3) - (3)])) == 0 || 
        (cb_list_length((yyvsp[(3) - (3)])) == 1 && CB_TREE((yyvsp[(3) - (3)]))->generated)) {
        cb_check_feature_x ((yyvsp[(1) - (3)]), cb_syntax_ibm5_2, "Empty WHEN/OTHER clause");
    }

    if (cb_is_other_caselist((yyvsp[(1) - (3)]))) {
        (yyval) = cb_cons ((yyvsp[(3) - (3)]), NULL);
    } else {
        (yyval) = cb_cons ((yyvsp[(3) - (3)]), (yyvsp[(1) - (3)]));
    }
    eval_inc2 = 0;
  ;}
    break;

  case 1001:

/* Line 1455 of yacc.c  */
#line 5610 "parser.y"
    { (yyval) = cb_list_init(cb_build_continue ());
      CB_TREE((yyval))->generated=1;
    ;}
    break;

  case 1002:

/* Line 1455 of yacc.c  */
#line 5614 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]);  ;}
    break;

  case 1003:

/* Line 1455 of yacc.c  */
#line 5618 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(2) - (2)])); eval_inc2 = 0;;}
    break;

  case 1004:

/* Line 1455 of yacc.c  */
#line 5620 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); eval_inc2 = 0;;}
    break;

  case 1005:

/* Line 1455 of yacc.c  */
#line 5625 "parser.y"
    { (yyval) = cb_list_init (NULL);;}
    break;

  case 1006:

/* Line 1455 of yacc.c  */
#line 5626 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]);;}
    break;

  case 1007:

/* Line 1455 of yacc.c  */
#line 5630 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)]));  ;}
    break;

  case 1008:

/* Line 1455 of yacc.c  */
#line 5632 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 1009:

/* Line 1455 of yacc.c  */
#line 5637 "parser.y"
    {
        cb_tree oper;
        cb_tree e1;
        cb_tree e2;
        int subject_condition = 0;

        oper = cb_int0;
        e1 = (yyvsp[(1) - (2)]);
        e2 = (yyvsp[(2) - (2)]);

        /* in case the first token is NOT */
        if (CB_PURPOSE_INT (e1) == '!') {
                if (eval_check[eval_level][eval_inc2] < 2) {
                        oper = cb_int1;
                        e1 = CB_CHAIN (e1);
                }
        }
    /*CIT*/
        /* class or sign condition on subject */
        if ((eval_check[eval_level][eval_inc2] < 2) && !CB_CHAIN(e1))
            switch (CB_PURPOSE_INT (e1)) {
                case 'P': 
                    e1 = (oper == cb_int0 ? cb_positive : cb_not_positive); 
                    subject_condition = 1;
                    break;
                case 'N': 
                    e1 = (oper == cb_int0 ? cb_negative : cb_not_negative); 
                    subject_condition = 1;
                    break;
                case '9': 
                    e1 = (oper == cb_int0 ? cb_numeric : cb_not_numeric); 
                    subject_condition = 1;
                    break;
                case 'A': 
                    e1 = (oper == cb_int0 ? cb_alphabetic : cb_not_alphabetic); 
                    subject_condition = 1;
                    break;
                case 'L': 
                    e1 = (oper == cb_int0 ? cb_alphabetic_lower : cb_not_alphabetic_lower); 
                    subject_condition = 1;
                    break;
                case 'U': 
                    e1 = (oper == cb_int0 ? cb_alphabetic_upper : cb_not_alphabetic_upper); 
                    subject_condition = 1;
            }
    

        if (subject_condition) {
            (yyval) = e1;
            eval_inc2++;
        }
        else {
            /* in case the first token is Condition */
            switch (CB_PURPOSE_INT (e1)) {
            case '<':
            case '>':
            case '[':
            case ']':
            case '=':
            case '~':
                    if (eval_check[eval_level][eval_inc2] < 2) {
                            if (oper == cb_int1) {
                              switch (CB_PURPOSE_INT (e1)) {
                              case '<': oper = cb_int (']'); break;
                              case '>': oper = cb_int ('['); break;
                              case '[': oper = cb_int ('>'); break;
                              case ']': oper = cb_int ('<'); break;
                              case '=': oper = cb_int ('~'); break;
                              case '~': oper = cb_int ('='); break;
                              }
                            } else {
                               oper = CB_PURPOSE (e1) ;
                            }
                            e1 = CB_CHAIN (e1);
                    }
            }
        

            /* build expr now */
            e1 = cb_build_expr (e1);

            if (e2 == NULL) {
                    /* WHEN expr */
                    eval_inc2++;
                    (yyval) = cb_build_pair (oper, cb_build_pair (e1, NULL));
            } else {
                    /* WHEN expr THRU expr */
                    (yyval) = cb_build_pair (oper, cb_build_pair (e1, e2));
                    eval_inc2++;
            }            
        }
  ;}
    break;

  case 1010:

/* Line 1455 of yacc.c  */
#line 5729 "parser.y"
    { (yyval) = cb_any; eval_inc2++; ;}
    break;

  case 1011:

/* Line 1455 of yacc.c  */
#line 5730 "parser.y"
    { (yyval) = cb_true; eval_inc2++; ;}
    break;

  case 1012:

/* Line 1455 of yacc.c  */
#line 5731 "parser.y"
    { (yyval) = cb_false; eval_inc2++; ;}
    break;

  case 1013:

/* Line 1455 of yacc.c  */
#line 5735 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1014:

/* Line 1455 of yacc.c  */
#line 5736 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1015:

/* Line 1455 of yacc.c  */
#line 5740 "parser.y"
    { terminator_warning (TERM_EVALUATE); ;}
    break;

  case 1016:

/* Line 1455 of yacc.c  */
#line 5741 "parser.y"
    { terminator_clear (TERM_EVALUATE); ;}
    break;

  case 1017:

/* Line 1455 of yacc.c  */
#line 5750 "parser.y"
    { BEGIN_STATEMENT ("EXIT", 0); ;}
    break;

  case 1019:

/* Line 1455 of yacc.c  */
#line 5755 "parser.y"
    { /* nothing */ ;}
    break;

  case 1020:

/* Line 1455 of yacc.c  */
#line 5757 "parser.y"
    {
        if (in_declaratives && use_global_ind) {
            cb_error (_("EXIT PROGRAM is not allowed within a USE GLOBAL procedure"));
        }
        check_unreached = 1;
    ;}
    break;

  case 1022:

/* Line 1455 of yacc.c  */
#line 5765 "parser.y"
    {
        struct cb_perform       *p;
        char                    name[64];

        if (!perform_stack) {
                cb_error (_("EXIT PERFORM is only valid with inline PERFORM"));
        } else {
                p = CB_PERFORM (CB_VALUE (perform_stack));
                if (!p->exit_label) {
                        sprintf (name, "EXIT PERFORM %d", cb_id);
                        p->exit_label = cb_build_reference (name);
                        CB_LABEL (cb_build_label (p->exit_label, NULL))->need_begin = 1;
                }
        cb_emit_goto (cb_list_init (p->exit_label), NULL);
    }
  ;}
    break;

  case 1023:

/* Line 1455 of yacc.c  */
#line 5782 "parser.y"
    {
        struct cb_perform       *p;
        char                    name[64];

        if (!perform_stack) {
                cb_error (_("EXIT PERFORM is only valid with inline PERFORM"));
        } else {
                p = CB_PERFORM (CB_VALUE (perform_stack));
                if (!p->cycle_label) {
                        sprintf (name, "EXIT PERFORM CYCLE %d", cb_id);
                        p->cycle_label = cb_build_reference (name);
                        CB_LABEL (cb_build_label (p->cycle_label, NULL))->need_begin = 1;
                }
        cb_emit_goto (cb_list_init (p->cycle_label), NULL);
    }
  ;}
    break;

  case 1024:

/* Line 1455 of yacc.c  */
#line 5799 "parser.y"
    {
        cb_tree plabel;
        char    name[64];

        if (!current_section) {
            cb_error (_("EXIT SECTION is only valid with an active SECTION"));
        } else {
            if (!current_section->exit_label) {
                sprintf (name, "EXIT SECTION %d", cb_id);
                plabel = cb_build_reference(name);
                current_section->exit_label = cb_build_label (plabel, NULL);
                current_section->exit_label_ref = plabel;
                CB_LABEL (current_section->exit_label)->need_begin = 1;
                }
            cb_emit_goto (cb_list_init (current_section->exit_label_ref), NULL);
        }
  ;}
    break;

  case 1025:

/* Line 1455 of yacc.c  */
#line 5817 "parser.y"
    {
        cb_tree plabel;
        char    name[64];

        if (!current_paragraph) {
                cb_error (_("EXIT PARAGRAPH is only valid with an active PARAGRAPH"));
        } else {
                if (!current_paragraph->exit_label) {
                        sprintf (name, "EXIT PARAGRAPH %d", cb_id);
                        plabel = cb_build_reference(name);
                        current_paragraph->exit_label = cb_build_label (plabel, NULL);
                        current_paragraph->exit_label_ref = plabel;
                        CB_LABEL (current_paragraph->exit_label)->need_begin = 1;
                }
        cb_emit_goto (cb_list_init (current_paragraph->exit_label_ref), NULL);
    }
  ;}
    break;

  case 1026:

/* Line 1455 of yacc.c  */
#line 5837 "parser.y"
    {    cb_emit_exit (cb_flag_exit_program_forced, NULL); ;}
    break;

  case 1027:

/* Line 1455 of yacc.c  */
#line 5839 "parser.y"
    {          
        cb_emit_exit (cb_flag_exit_program_forced, (yyvsp[(2) - (2)]));
    ;}
    break;

  case 1028:

/* Line 1455 of yacc.c  */
#line 5850 "parser.y"
    {
        BEGIN_STATEMENT ("EXHIBIT", TERM_EXHIBIT);
        display_advance = cb_int1;
        display_upon = cb_int(COB_DEVICE_SYSOUT);
        exhibit_option = 0;
  ;}
    break;

  case 1029:

/* Line 1455 of yacc.c  */
#line 5857 "parser.y"
    {
    cb_emit_exhibit ((yyvsp[(4) - (5)]), display_upon, display_advance, exhibit_option);
  ;}
    break;

  case 1031:

/* Line 1455 of yacc.c  */
#line 5864 "parser.y"
    { (yyval) = cb_list_append (NULL, cb_build_exhibit_one((yyvsp[(1) - (1)]), exhibit_option)); ;}
    break;

  case 1032:

/* Line 1455 of yacc.c  */
#line 5865 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), cb_build_exhibit_one((yyvsp[(2) - (2)]), exhibit_option));   ;}
    break;

  case 1036:

/* Line 1455 of yacc.c  */
#line 5874 "parser.y"
    { exhibit_option |= 1; ;}
    break;

  case 1037:

/* Line 1455 of yacc.c  */
#line 5875 "parser.y"
    { exhibit_option |= 2; ;}
    break;

  case 1041:

/* Line 1455 of yacc.c  */
#line 5884 "parser.y"
    { display_upon = cb_build_display_upon ((yyvsp[(2) - (2)])); ;}
    break;

  case 1042:

/* Line 1455 of yacc.c  */
#line 5885 "parser.y"
    { display_upon = cb_build_display_upon_direct ((yyvsp[(2) - (2)])); ;}
    break;

  case 1043:

/* Line 1455 of yacc.c  */
#line 5886 "parser.y"
    { display_upon = cb_int(COB_DEVICE_SYSOUT); ;}
    break;

  case 1044:

/* Line 1455 of yacc.c  */
#line 5887 "parser.y"
    { display_upon = cb_flag_console_equal_sysfile ? cb_int(COB_DEVICE_SYSOUT) : cb_int(COB_DEVICE_CONSOLE); ;}
    break;

  case 1045:

/* Line 1455 of yacc.c  */
#line 5888 "parser.y"
    { display_advance = cb_int0; ;}
    break;

  case 1046:

/* Line 1455 of yacc.c  */
#line 5892 "parser.y"
    { terminator_warning (TERM_EXHIBIT); ;}
    break;

  case 1047:

/* Line 1455 of yacc.c  */
#line 5893 "parser.y"
    { terminator_clear (TERM_EXHIBIT); ;}
    break;

  case 1048:

/* Line 1455 of yacc.c  */
#line 5901 "parser.y"
    { BEGIN_STATEMENT ("FREE", 0); ;}
    break;

  case 1049:

/* Line 1455 of yacc.c  */
#line 5903 "parser.y"
    {
    cb_emit_free ((yyvsp[(3) - (3)]));
  ;}
    break;

  case 1050:

/* Line 1455 of yacc.c  */
#line 5914 "parser.y"
    { BEGIN_STATEMENT ("GENERATE", 0); ;}
    break;

  case 1051:

/* Line 1455 of yacc.c  */
#line 5916 "parser.y"
    {
    PENDING("GENERATE");
  ;}
    break;

  case 1052:

/* Line 1455 of yacc.c  */
#line 5927 "parser.y"
    { BEGIN_STATEMENT ("GO TO", 0); ;}
    break;

  case 1053:

/* Line 1455 of yacc.c  */
#line 5929 "parser.y"
    {
        cb_emit_goto ((yyvsp[(4) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1054:

/* Line 1455 of yacc.c  */
#line 5936 "parser.y"
    {
    check_unreached = 1;
    (yyval) = NULL;
  ;}
    break;

  case 1055:

/* Line 1455 of yacc.c  */
#line 5941 "parser.y"
    {
    check_unreached = 0;
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1056:

/* Line 1455 of yacc.c  */
#line 5953 "parser.y"
    { BEGIN_STATEMENT ("GOBACK", 0); ;}
    break;

  case 1058:

/* Line 1455 of yacc.c  */
#line 5958 "parser.y"
    {
        check_unreached = 1;
        cb_emit_exit (1, NULL);
  ;}
    break;

  case 1059:

/* Line 1455 of yacc.c  */
#line 5963 "parser.y"
    {          
        cb_emit_exit (1, (yyvsp[(2) - (2)]));
    ;}
    break;

  case 1060:

/* Line 1455 of yacc.c  */
#line 5974 "parser.y"
    { BEGIN_STATEMENT ("IF", TERM_IF); ;}
    break;

  case 1062:

/* Line 1455 of yacc.c  */
#line 5980 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1063:

/* Line 1455 of yacc.c  */
#line 5984 "parser.y"
    {
    cb_emit_if ((yyvsp[(1) - (4)]), CB_PAIR_X((yyvsp[(4) - (4)])), CB_PAIR_Y((yyvsp[(4) - (4)])));
  ;}
    break;

  case 1065:

/* Line 1455 of yacc.c  */
#line 5992 "parser.y"
    {
    (yyval) = cb_build_pair (NULL, NULL);
    cb_check_feature_x (CB_TREE(current_statement), cb_syntax_ibm5_2, "Empty IF - END-IF");
  ;}
    break;

  case 1066:

/* Line 1455 of yacc.c  */
#line 5998 "parser.y"
    {
    (yyval) = cb_build_pair((yyvsp[(1) - (2)]),NULL);
  ;}
    break;

  case 1067:

/* Line 1455 of yacc.c  */
#line 6004 "parser.y"
    {
    (yyval) = cb_build_pair((yyvsp[(1) - (3)]), NULL);
    cb_check_feature_x (CB_TREE(current_statement), cb_syntax_ibm5_2, "Empty ELSE - END-IF");
  ;}
    break;

  case 1068:

/* Line 1455 of yacc.c  */
#line 6012 "parser.y"
    {
    (yyval) = cb_build_pair((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]));
  ;}
    break;

  case 1069:

/* Line 1455 of yacc.c  */
#line 6018 "parser.y"
    {
    (yyval) = cb_build_pair(NULL, (yyvsp[(2) - (3)]));
    cb_check_feature_x (CB_TREE(current_statement), cb_syntax_ibm5_2, "Empty IF - ELSE");
  ;}
    break;

  case 1070:

/* Line 1455 of yacc.c  */
#line 6024 "parser.y"
    {
    (yyval) = cb_build_pair (NULL, NULL);
    cb_check_feature_x (CB_TREE(current_statement), cb_syntax_ibm5_2, "Empty IF - ELSE - END-IF");
  ;}
    break;

  case 1071:

/* Line 1455 of yacc.c  */
#line 6031 "parser.y"
    { terminator_warning (TERM_IF); ;}
    break;

  case 1072:

/* Line 1455 of yacc.c  */
#line 6032 "parser.y"
    { terminator_clear (TERM_IF); ;}
    break;

  case 1073:

/* Line 1455 of yacc.c  */
#line 6040 "parser.y"
    { BEGIN_STATEMENT ("INITIALIZE", 0); ;}
    break;

  case 1074:

/* Line 1455 of yacc.c  */
#line 6042 "parser.y"
    {
        cb_emit_initialize_vars ((yyvsp[(3) - (7)]), (yyvsp[(4) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(6) - (7)]), (yyvsp[(7) - (7)]));
  ;}
    break;

  case 1075:

/* Line 1455 of yacc.c  */
#line 6048 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1076:

/* Line 1455 of yacc.c  */
#line 6049 "parser.y"
    { (yyval) = cb_true; ;}
    break;

  case 1077:

/* Line 1455 of yacc.c  */
#line 6053 "parser.y"
    { if(cb_flag_initalize_to_value || cb_flag_initialize_to_value) (yyval) = cb_true; else (yyval) = NULL; ;}
    break;

  case 1078:

/* Line 1455 of yacc.c  */
#line 6054 "parser.y"
    { (yyval) = cb_true; ;}
    break;

  case 1079:

/* Line 1455 of yacc.c  */
#line 6055 "parser.y"
    { (yyval) = (yyvsp[(1) - (3)]); ;}
    break;

  case 1080:

/* Line 1455 of yacc.c  */
#line 6059 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1081:

/* Line 1455 of yacc.c  */
#line 6061 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1082:

/* Line 1455 of yacc.c  */
#line 6065 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1083:

/* Line 1455 of yacc.c  */
#line 6067 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1084:

/* Line 1455 of yacc.c  */
#line 6071 "parser.y"
    { (yyval) = cb_build_pair ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); ;}
    break;

  case 1085:

/* Line 1455 of yacc.c  */
#line 6075 "parser.y"
    { (yyval) = cb_int (CB_CATEGORY_ALPHABETIC); ;}
    break;

  case 1086:

/* Line 1455 of yacc.c  */
#line 6076 "parser.y"
    { (yyval) = cb_int (CB_CATEGORY_ALPHANUMERIC); ;}
    break;

  case 1087:

/* Line 1455 of yacc.c  */
#line 6077 "parser.y"
    { (yyval) = cb_int (CB_CATEGORY_NUMERIC); ;}
    break;

  case 1088:

/* Line 1455 of yacc.c  */
#line 6078 "parser.y"
    { (yyval) = cb_int (CB_CATEGORY_ALPHANUMERIC_EDITED); ;}
    break;

  case 1089:

/* Line 1455 of yacc.c  */
#line 6079 "parser.y"
    { (yyval) = cb_int (CB_CATEGORY_NUMERIC_EDITED); ;}
    break;

  case 1090:

/* Line 1455 of yacc.c  */
#line 6080 "parser.y"
    { (yyval) = cb_int (CB_CATEGORY_NATIONAL); ;}
    break;

  case 1091:

/* Line 1455 of yacc.c  */
#line 6081 "parser.y"
    { (yyval) = cb_int (CB_CATEGORY_NATIONAL_EDITED); ;}
    break;

  case 1092:

/* Line 1455 of yacc.c  */
#line 6085 "parser.y"
    { if(cb_flag_initalize_to_value || cb_flag_initialize_to_value) (yyval) = cb_true; else (yyval) = NULL; ;}
    break;

  case 1093:

/* Line 1455 of yacc.c  */
#line 6086 "parser.y"
    { (yyval) = cb_true; ;}
    break;

  case 1094:

/* Line 1455 of yacc.c  */
#line 6095 "parser.y"
    { BEGIN_STATEMENT ("INITIATE", 0); ;}
    break;

  case 1095:

/* Line 1455 of yacc.c  */
#line 6097 "parser.y"
    {
    PENDING("INITIATE");
  ;}
    break;

  case 1096:

/* Line 1455 of yacc.c  */
#line 6108 "parser.y"
    {
        BEGIN_STATEMENT ("INSPECT", 0);
        sending_id = 0;
        inspect_keyword = 0;
  ;}
    break;

  case 1098:

/* Line 1455 of yacc.c  */
#line 6117 "parser.y"
    { save_tree_1 = (yyvsp[(1) - (1)]); sending_id = 0; ;}
    break;

  case 1099:

/* Line 1455 of yacc.c  */
#line 6118 "parser.y"
    { save_tree_1 = (yyvsp[(1) - (1)]); sending_id = 1; ;}
    break;

  case 1100:

/* Line 1455 of yacc.c  */
#line 6119 "parser.y"
    { save_tree_1 = (yyvsp[(1) - (1)]); sending_id = 1; ;}
    break;

  case 1103:

/* Line 1455 of yacc.c  */
#line 6128 "parser.y"
    { cb_emit_inspect (save_tree_1, (yyvsp[(1) - (1)]), cb_int0, 0); ;}
    break;

  case 1104:

/* Line 1455 of yacc.c  */
#line 6129 "parser.y"
    { cb_emit_inspect (save_tree_1, (yyvsp[(1) - (1)]), cb_int1, 1); ;}
    break;

  case 1105:

/* Line 1455 of yacc.c  */
#line 6130 "parser.y"
    { cb_emit_inspect (save_tree_1, (yyvsp[(1) - (1)]), cb_int0, 2); ;}
    break;

  case 1106:

/* Line 1455 of yacc.c  */
#line 6136 "parser.y"
    { cb_init_tarrying (); ;}
    break;

  case 1107:

/* Line 1455 of yacc.c  */
#line 6137 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1108:

/* Line 1455 of yacc.c  */
#line 6141 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1109:

/* Line 1455 of yacc.c  */
#line 6142 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1110:

/* Line 1455 of yacc.c  */
#line 6146 "parser.y"
    { (yyval) = cb_build_tarrying_data ((yyvsp[(1) - (2)])); ;}
    break;

  case 1111:

/* Line 1455 of yacc.c  */
#line 6147 "parser.y"
    { (yyval) = cb_build_tarrying_characters ((yyvsp[(2) - (2)])); ;}
    break;

  case 1112:

/* Line 1455 of yacc.c  */
#line 6148 "parser.y"
    { (yyval) = cb_build_tarrying_all (); ;}
    break;

  case 1113:

/* Line 1455 of yacc.c  */
#line 6149 "parser.y"
    { (yyval) = cb_build_tarrying_leading (); ;}
    break;

  case 1114:

/* Line 1455 of yacc.c  */
#line 6150 "parser.y"
    { (yyval) = cb_build_tarrying_trailing (); ;}
    break;

  case 1115:

/* Line 1455 of yacc.c  */
#line 6151 "parser.y"
    { (yyval) = cb_build_tarrying_value ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1116:

/* Line 1455 of yacc.c  */
#line 6157 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); inspect_keyword = 0; ;}
    break;

  case 1117:

/* Line 1455 of yacc.c  */
#line 6161 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1118:

/* Line 1455 of yacc.c  */
#line 6162 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1119:

/* Line 1455 of yacc.c  */
#line 6167 "parser.y"
    {
        (yyval) = cb_build_replacing_characters ((yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
        inspect_keyword = 0;
  ;}
    break;

  case 1120:

/* Line 1455 of yacc.c  */
#line 6171 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1121:

/* Line 1455 of yacc.c  */
#line 6175 "parser.y"
    { /* Nothing */ ;}
    break;

  case 1122:

/* Line 1455 of yacc.c  */
#line 6176 "parser.y"
    { inspect_keyword = 1; ;}
    break;

  case 1123:

/* Line 1455 of yacc.c  */
#line 6177 "parser.y"
    { inspect_keyword = 2; ;}
    break;

  case 1124:

/* Line 1455 of yacc.c  */
#line 6178 "parser.y"
    { inspect_keyword = 3; ;}
    break;

  case 1125:

/* Line 1455 of yacc.c  */
#line 6179 "parser.y"
    { inspect_keyword = 4; ;}
    break;

  case 1126:

/* Line 1455 of yacc.c  */
#line 6184 "parser.y"
    {
        switch (inspect_keyword) {
                case 1:
                        (yyval) = cb_build_replacing_all ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
                        break;
                case 2:
                        (yyval) = cb_build_replacing_leading ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
                        break;
                case 3:
                        (yyval) = cb_build_replacing_first ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
                        break;
                case 4:
                        (yyval) = cb_build_replacing_trailing ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
                        break;
                default:
                        cb_warning (_("INSPECT REPLACING missing a keyword, ALL assumed"));
                        (yyval) = cb_build_replacing_all ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
                        break;
        }
  ;}
    break;

  case 1127:

/* Line 1455 of yacc.c  */
#line 6210 "parser.y"
    {
        (yyval) = cb_build_converting ((yyvsp[(3) - (6)]), (yyvsp[(5) - (6)]), (yyvsp[(6) - (6)]));
  ;}
    break;

  case 1128:

/* Line 1455 of yacc.c  */
#line 6218 "parser.y"
    { (yyval) = cb_build_inspect_region_start (); ;}
    break;

  case 1129:

/* Line 1455 of yacc.c  */
#line 6220 "parser.y"
    { (yyval) = cb_build_inspect_region ((yyvsp[(1) - (4)]), (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); ;}
    break;

  case 1132:

/* Line 1455 of yacc.c  */
#line 6231 "parser.y"
    { BEGIN_STATEMENT ("MERGE", 0); ;}
    break;

  case 1134:

/* Line 1455 of yacc.c  */
#line 6241 "parser.y"
    { BEGIN_STATEMENT ("MOVE", 0); ;}
    break;

  case 1136:

/* Line 1455 of yacc.c  */
#line 6247 "parser.y"
    {
        cb_emit_move ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1137:

/* Line 1455 of yacc.c  */
#line 6251 "parser.y"
    {
        cb_emit_move_corresponding ((yyvsp[(2) - (4)]), (yyvsp[(4) - (4)]));
  ;}
    break;

  case 1138:

/* Line 1455 of yacc.c  */
#line 6262 "parser.y"
    { BEGIN_STATEMENT ("MULTIPLY", TERM_MULTIPLY); rounded_used = 0;;}
    break;

  case 1140:

/* Line 1455 of yacc.c  */
#line 6269 "parser.y"
    {
   /*CIT*/
        void* e1 = (yyvsp[(1) - (4)]);
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
        if (current_statement->handler1 || current_statement->handler2) {
            e1 = cb_check_optimized_field_1((yyvsp[(1) - (4)]));
            cb_check_optimized_field((yyvsp[(3) - (4)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(3) - (4)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(3) - (4)]), '*', e1, !rounded_used);
  ;}
    break;

  case 1141:

/* Line 1455 of yacc.c  */
#line 6284 "parser.y"
    {
   /*CIT*/
        void* e1 = (yyvsp[(1) - (6)]);
        void* e3 = (yyvsp[(3) - (6)]);
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
        if (current_statement->handler1 || current_statement->handler2) {
            e1 = cb_check_optimized_field_1((yyvsp[(1) - (6)]));
            e3 = cb_check_optimized_field_1((yyvsp[(3) - (6)]));
            cb_check_optimized_field((yyvsp[(5) - (6)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(5) - (6)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(5) - (6)]), 0, cb_build_binary_op (e1, '*', e3), !rounded_used);
  ;}
    break;

  case 1142:

/* Line 1455 of yacc.c  */
#line 6303 "parser.y"
    { terminator_warning (TERM_MULTIPLY); ;}
    break;

  case 1143:

/* Line 1455 of yacc.c  */
#line 6304 "parser.y"
    { terminator_clear (TERM_MULTIPLY); ;}
    break;

  case 1144:

/* Line 1455 of yacc.c  */
#line 6313 "parser.y"
    { BEGIN_STATEMENT ("OPEN", 0); ;}
    break;

  case 1147:

/* Line 1455 of yacc.c  */
#line 6321 "parser.y"
    {
        cb_tree l;
        void *e2 = (yyvsp[(2) - (6)]);
        if ((yyvsp[(5) - (6)])) {
            e2 = (yyvsp[(5) - (6)]);
        }
        for (l = (yyvsp[(4) - (6)]); l; l = CB_CHAIN (l)) {
                if (CB_VALUE (l) != cb_error_node) {
                        BEGIN_IMPLICIT_STATEMENT ();
                        cb_emit_open (CB_VALUE (l), e2, (yyvsp[(3) - (6)]), (yyvsp[(6) - (6)]));
                }
        }
  ;}
    break;

  case 1148:

/* Line 1455 of yacc.c  */
#line 6337 "parser.y"
    { (yyval) = cb_int (COB_OPEN_INPUT); ;}
    break;

  case 1149:

/* Line 1455 of yacc.c  */
#line 6338 "parser.y"
    { (yyval) = cb_int (COB_OPEN_OUTPUT); ;}
    break;

  case 1150:

/* Line 1455 of yacc.c  */
#line 6339 "parser.y"
    { (yyval) = cb_int (COB_OPEN_I_O); ;}
    break;

  case 1151:

/* Line 1455 of yacc.c  */
#line 6340 "parser.y"
    { (yyval) = cb_int (COB_OPEN_EXTEND); ;}
    break;

  case 1152:

/* Line 1455 of yacc.c  */
#line 6344 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1153:

/* Line 1455 of yacc.c  */
#line 6345 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1154:

/* Line 1455 of yacc.c  */
#line 6350 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1155:

/* Line 1455 of yacc.c  */
#line 6351 "parser.y"
    { (yyval) = cb_int0 ;/* ignored */ ;}
    break;

  case 1156:

/* Line 1455 of yacc.c  */
#line 6352 "parser.y"
    { (yyval) = cb_int (COB_LOCK_EXCLUSIVE);;}
    break;

  case 1157:

/* Line 1455 of yacc.c  */
#line 6356 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1158:

/* Line 1455 of yacc.c  */
#line 6357 "parser.y"
    { (yyval) = cb_int (COB_OPEN_INPUT_REVERSED); ;}
    break;

  case 1159:

/* Line 1455 of yacc.c  */
#line 6365 "parser.y"
    { BEGIN_STATEMENT ("PERFORM", TERM_PERFORM); ;}
    break;

  case 1161:

/* Line 1455 of yacc.c  */
#line 6371 "parser.y"
    {
        cb_emit_perform ((yyvsp[(2) - (2)]), (yyvsp[(1) - (2)]));
  ;}
    break;

  case 1162:

/* Line 1455 of yacc.c  */
#line 6375 "parser.y"
    {
        perform_stack = cb_cons ((yyvsp[(1) - (1)]), perform_stack);
        check_unreached = 0;
  ;}
    break;

  case 1163:

/* Line 1455 of yacc.c  */
#line 6380 "parser.y"
    {
        perform_stack = CB_CHAIN (perform_stack);
        cb_emit_perform ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]));
  ;}
    break;

  case 1164:

/* Line 1455 of yacc.c  */
#line 6385 "parser.y"
    {
        cb_emit_perform ((yyvsp[(1) - (2)]), NULL);
  ;}
    break;

  case 1165:

/* Line 1455 of yacc.c  */
#line 6392 "parser.y"
    { terminator_warning (TERM_PERFORM); ;}
    break;

  case 1166:

/* Line 1455 of yacc.c  */
#line 6393 "parser.y"
    { terminator_clear (TERM_PERFORM); ;}
    break;

  case 1167:

/* Line 1455 of yacc.c  */
#line 6398 "parser.y"
    {
        CB_REFERENCE ((yyvsp[(1) - (1)]))->length = cb_true; /* return from $1 */
        (yyval) = cb_build_pair ((yyvsp[(1) - (1)]), (yyvsp[(1) - (1)]));
  ;}
    break;

  case 1168:

/* Line 1455 of yacc.c  */
#line 6403 "parser.y"
    {
        CB_REFERENCE ((yyvsp[(3) - (3)]))->length = cb_true; /* return from $3 */
        (yyval) = cb_build_pair ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1169:

/* Line 1455 of yacc.c  */
#line 6411 "parser.y"
    {
        (yyval) = cb_build_perform_once (NULL);
  ;}
    break;

  case 1170:

/* Line 1455 of yacc.c  */
#line 6415 "parser.y"
    {
    (yyval) = cb_build_perform_forever (NULL);
  ;}
    break;

  case 1171:

/* Line 1455 of yacc.c  */
#line 6419 "parser.y"
    {
        (yyval) = cb_build_perform_times ((yyvsp[(1) - (2)]));
  ;}
    break;

  case 1172:

/* Line 1455 of yacc.c  */
#line 6423 "parser.y"
    {
        if ((yyvsp[(3) - (3)]) == NULL)  {
            (yyval) = cb_build_perform_forever (NULL);
        } else {
            cb_tree varying;
    
            varying = cb_list_init (cb_build_perform_varying (NULL, NULL, NULL, (yyvsp[(3) - (3)])));
            (yyval) = cb_build_perform_until ((yyvsp[(1) - (3)]), varying);
        }
  ;}
    break;

  case 1173:

/* Line 1455 of yacc.c  */
#line 6434 "parser.y"
    {
        (yyval) = cb_build_perform_until ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1174:

/* Line 1455 of yacc.c  */
#line 6440 "parser.y"
    { (yyval) = CB_BEFORE; ;}
    break;

  case 1175:

/* Line 1455 of yacc.c  */
#line 6441 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1176:

/* Line 1455 of yacc.c  */
#line 6445 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]) ;  ;}
    break;

  case 1177:

/* Line 1455 of yacc.c  */
#line 6446 "parser.y"
    { (yyval) = NULL ;;}
    break;

  case 1178:

/* Line 1455 of yacc.c  */
#line 6450 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1179:

/* Line 1455 of yacc.c  */
#line 6452 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 1180:

/* Line 1455 of yacc.c  */
#line 6457 "parser.y"
    {
        (yyval) = cb_build_perform_varying ((yyvsp[(1) - (7)]), (yyvsp[(3) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(7) - (7)]));
  ;}
    break;

  case 1181:

/* Line 1455 of yacc.c  */
#line 6470 "parser.y"
    { 
       BEGIN_STATEMENT_1 ("PRAGMA", 0, 0); 
  ;}
    break;

  case 1182:

/* Line 1455 of yacc.c  */
#line 6474 "parser.y"
    {
       struct cb_literal *li;
       if ( (yyvsp[(3) - (4)]) && CB_LITERAL_P( (yyvsp[(3) - (4)])) ) {
         li = CB_LITERAL((yyvsp[(3) - (4)]));
         if ( (strcasecmp((char*)li->data, "DEBUGOFF") == 0) ) {
            cb_disable_runtime_check = 1;
         } else if ( (strcasecmp((char*)li->data, "DEBUGON") == 0) ) {
            cb_disable_runtime_check = 0;
         }         
       }
       cb_emit_pragma ((yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
  ;}
    break;

  case 1187:

/* Line 1455 of yacc.c  */
#line 6499 "parser.y"
    { BEGIN_STATEMENT ("READ", TERM_READ); ;}
    break;

  case 1188:

/* Line 1455 of yacc.c  */
#line 6502 "parser.y"
    {
        if ((yyvsp[(3) - (11)]) != cb_error_node) {
                if ((yyvsp[(8) - (11)]) && (CB_FILE(cb_ref((yyvsp[(3) - (11)])))->lock_mode & COB_LOCK_AUTOMATIC)) {
                        cb_error (_("LOCK clause invalid with file LOCK AUTOMATIC"));
                } else if ((yyvsp[(9) - (11)]) &&
                      (CB_FILE(cb_ref((yyvsp[(3) - (11)])))->organization != COB_ORG_RELATIVE &&
                       CB_FILE(cb_ref((yyvsp[(3) - (11)])))->organization != COB_ORG_INDEXED) &&
                       (!cb_verify (cb_invalid_with_file_type, "KEY clause with wrong file type"))) {
                        cb_error (_("KEY clause invalid with this file type"));
                } else if (current_statement->handler_id == COB_EC_I_O_INVALID_KEY &&
                      (CB_FILE(cb_ref((yyvsp[(3) - (11)])))->organization != COB_ORG_RELATIVE &&
                       CB_FILE(cb_ref((yyvsp[(3) - (11)])))->organization != COB_ORG_INDEXED) && 
                       (!cb_verify (cb_invalid_with_file_type, "KEY clause with wrong file type"))) {
                       cb_error (_("INVALID KEY clause invalid with this file type"));
                } else {
                        cb_emit_read ((yyvsp[(3) - (11)]), (yyvsp[(4) - (11)]), (yyvsp[(7) - (11)]), (yyvsp[(9) - (11)]), (yyvsp[(6) - (11)]) == NULL ? (yyvsp[(8) - (11)]) : (yyvsp[(6) - (11)]) );
                }
        }
  ;}
    break;

  case 1189:

/* Line 1455 of yacc.c  */
#line 6524 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1190:

/* Line 1455 of yacc.c  */
#line 6525 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1191:

/* Line 1455 of yacc.c  */
#line 6529 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1192:

/* Line 1455 of yacc.c  */
#line 6531 "parser.y"
    {
        (yyval) = cb_int3;
  ;}
    break;

  case 1193:

/* Line 1455 of yacc.c  */
#line 6535 "parser.y"
    {
        (yyval) = cb_int1;
  ;}
    break;

  case 1194:

/* Line 1455 of yacc.c  */
#line 6539 "parser.y"
    {
        (yyval) = cb_int2;
  ;}
    break;

  case 1195:

/* Line 1455 of yacc.c  */
#line 6543 "parser.y"
    {
        (yyval) = cb_int3;
  ;}
    break;

  case 1196:

/* Line 1455 of yacc.c  */
#line 6547 "parser.y"
    {
        (yyval) = cb_int4;
  ;}
    break;

  case 1197:

/* Line 1455 of yacc.c  */
#line 6551 "parser.y"
    {
        (yyval) = cb_int1;
  ;}
    break;

  case 1198:

/* Line 1455 of yacc.c  */
#line 6557 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1199:

/* Line 1455 of yacc.c  */
#line 6559 "parser.y"
    {
        (yyval) = cb_int3;
  ;}
    break;

  case 1200:

/* Line 1455 of yacc.c  */
#line 6563 "parser.y"
    {
        (yyval) = cb_int1;
  ;}
    break;

  case 1201:

/* Line 1455 of yacc.c  */
#line 6567 "parser.y"
    {
        (yyval) = cb_int2;
  ;}
    break;

  case 1202:

/* Line 1455 of yacc.c  */
#line 6571 "parser.y"
    {
        (yyval) = cb_int3;
  ;}
    break;

  case 1203:

/* Line 1455 of yacc.c  */
#line 6575 "parser.y"
    {
        (yyval) = cb_int4;
  ;}
    break;

  case 1204:

/* Line 1455 of yacc.c  */
#line 6579 "parser.y"
    {
        (yyval) = cb_int1;
  ;}
    break;

  case 1205:

/* Line 1455 of yacc.c  */
#line 6585 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1206:

/* Line 1455 of yacc.c  */
#line 6586 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1210:

/* Line 1455 of yacc.c  */
#line 6595 "parser.y"
    { terminator_warning (TERM_READ); ;}
    break;

  case 1211:

/* Line 1455 of yacc.c  */
#line 6596 "parser.y"
    { terminator_clear (TERM_READ); ;}
    break;

  case 1212:

/* Line 1455 of yacc.c  */
#line 6606 "parser.y"
    {
        BEGIN_STATEMENT ("READY", 0);
        cb_emit_ready ();
  ;}
    break;

  case 1213:

/* Line 1455 of yacc.c  */
#line 6618 "parser.y"
    {
        BEGIN_STATEMENT ("RESET", 0);
        cb_emit_reset ();
  ;}
    break;

  case 1214:

/* Line 1455 of yacc.c  */
#line 6630 "parser.y"
    { BEGIN_STATEMENT ("RELEASE", 0); ;}
    break;

  case 1215:

/* Line 1455 of yacc.c  */
#line 6632 "parser.y"
    {
    if ((yyvsp[(3) - (4)]) != cb_error_node) {
        cb_emit_release ((yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
  }
  ;}
    break;

  case 1216:

/* Line 1455 of yacc.c  */
#line 6645 "parser.y"
    { BEGIN_STATEMENT ("RETURN", TERM_RETURN); ;}
    break;

  case 1217:

/* Line 1455 of yacc.c  */
#line 6648 "parser.y"
    {
    if ((yyvsp[(3) - (7)]) != cb_error_node) {
        cb_emit_return ((yyvsp[(3) - (7)]), (yyvsp[(5) - (7)]));
  }
  ;}
    break;

  case 1218:

/* Line 1455 of yacc.c  */
#line 6656 "parser.y"
    { terminator_warning (TERM_RETURN); ;}
    break;

  case 1219:

/* Line 1455 of yacc.c  */
#line 6657 "parser.y"
    { terminator_clear (TERM_RETURN); ;}
    break;

  case 1220:

/* Line 1455 of yacc.c  */
#line 6666 "parser.y"
    { BEGIN_STATEMENT ("REWRITE", TERM_REWRITE); ;}
    break;

  case 1221:

/* Line 1455 of yacc.c  */
#line 6669 "parser.y"
    {
        if ((yyvsp[(3) - (7)]) != cb_error_node) {
                cb_emit_rewrite ((yyvsp[(3) - (7)]), (yyvsp[(4) - (7)]), (yyvsp[(5) - (7)]));
        }
       if (cb_flag_fdclear){
           current_statement->final = cb_list_add(current_statement->final,
              cb_build_initialize_vars(cb_list_init ((yyvsp[(3) - (7)])),NULL,NULL,NULL,cb_flag_initalize_to_value || cb_flag_initialize_to_value ?  cb_true: NULL, 1));
        }
 
  ;}
    break;

  case 1222:

/* Line 1455 of yacc.c  */
#line 6682 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1223:

/* Line 1455 of yacc.c  */
#line 6684 "parser.y"
    {
        (yyval) = cb_int1;
  ;}
    break;

  case 1224:

/* Line 1455 of yacc.c  */
#line 6688 "parser.y"
    {
        (yyval) = cb_int2;
  ;}
    break;

  case 1225:

/* Line 1455 of yacc.c  */
#line 6694 "parser.y"
    { terminator_warning (TERM_REWRITE); ;}
    break;

  case 1226:

/* Line 1455 of yacc.c  */
#line 6695 "parser.y"
    { terminator_clear (TERM_REWRITE); ;}
    break;

  case 1227:

/* Line 1455 of yacc.c  */
#line 6705 "parser.y"
    {
        BEGIN_STATEMENT ("ROLLBACK", 0);
        cb_emit_rollback ();
  ;}
    break;

  case 1228:

/* Line 1455 of yacc.c  */
#line 6717 "parser.y"
    { BEGIN_STATEMENT ("SEARCH", TERM_SEARCH); ;}
    break;

  case 1230:

/* Line 1455 of yacc.c  */
#line 6724 "parser.y"
    {
        cb_emit_search ((yyvsp[(1) - (4)]), (yyvsp[(2) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)]));
  ;}
    break;

  case 1231:

/* Line 1455 of yacc.c  */
#line 6728 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1232:

/* Line 1455 of yacc.c  */
#line 6732 "parser.y"
    {
    cb_emit_search_all ((yyvsp[(2) - (7)]), (yyvsp[(3) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(7) - (7)]));
  ;}
    break;

  case 1233:

/* Line 1455 of yacc.c  */
#line 6738 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1234:

/* Line 1455 of yacc.c  */
#line 6739 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1235:

/* Line 1455 of yacc.c  */
#line 6743 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1236:

/* Line 1455 of yacc.c  */
#line 6745 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1237:

/* Line 1455 of yacc.c  */
#line 6749 "parser.y"
    {
    (yyval) = (yyvsp[(4) - (4)]);
  ;}
    break;

  case 1238:

/* Line 1455 of yacc.c  */
#line 6755 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1239:

/* Line 1455 of yacc.c  */
#line 6756 "parser.y"
    { (yyval) = (yyvsp[(1) - (2)]); CB_IF ((yyvsp[(1) - (2)]))->stmt2 = (yyvsp[(2) - (2)]); ;}
    break;

  case 1240:

/* Line 1455 of yacc.c  */
#line 6761 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1241:

/* Line 1455 of yacc.c  */
#line 6765 "parser.y"
    {
    (yyval) = cb_build_if ((yyvsp[(2) - (4)]), (yyvsp[(4) - (4)]), NULL);
  ;}
    break;

  case 1242:

/* Line 1455 of yacc.c  */
#line 6772 "parser.y"
    { terminator_warning (TERM_SEARCH); ;}
    break;

  case 1243:

/* Line 1455 of yacc.c  */
#line 6773 "parser.y"
    { terminator_clear (TERM_SEARCH); ;}
    break;

  case 1244:

/* Line 1455 of yacc.c  */
#line 6782 "parser.y"
    { BEGIN_STATEMENT ("SET", 0); ;}
    break;

  case 1251:

/* Line 1455 of yacc.c  */
#line 6798 "parser.y"
    {
        cb_emit_setenv ((yyvsp[(2) - (4)]), (yyvsp[(4) - (4)]));
  ;}
    break;

  case 1252:

/* Line 1455 of yacc.c  */
#line 6807 "parser.y"
    {
        cb_emit_set_to ((yyvsp[(1) - (4)]), cb_build_ppointer ((yyvsp[(4) - (4)])));
  ;}
    break;

  case 1253:

/* Line 1455 of yacc.c  */
#line 6811 "parser.y"
    {
        cb_emit_set_to ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1254:

/* Line 1455 of yacc.c  */
#line 6820 "parser.y"
    {
        cb_emit_set_up_down ((yyvsp[(1) - (4)]), (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)]));
  ;}
    break;

  case 1255:

/* Line 1455 of yacc.c  */
#line 6826 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1256:

/* Line 1455 of yacc.c  */
#line 6827 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1259:

/* Line 1455 of yacc.c  */
#line 6839 "parser.y"
    {
        cb_emit_set_on_off ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1262:

/* Line 1455 of yacc.c  */
#line 6853 "parser.y"
    {
        cb_emit_set_true ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 1263:

/* Line 1455 of yacc.c  */
#line 6857 "parser.y"
    {
        cb_emit_set_false ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 1264:

/* Line 1455 of yacc.c  */
#line 6868 "parser.y"
    { BEGIN_STATEMENT ("SORT", 0); ;}
    break;

  case 1266:

/* Line 1455 of yacc.c  */
#line 6874 "parser.y"
    {
        cb_tree x;
        if (!(CB_FILE_P (cb_ref ((yyvsp[(1) - (4)])))))
            x = (yyvsp[(1) - (4)])  ;
        else 
            x = cb_build_file_reference(CB_NAME((yyvsp[(1) - (4)])));          
        cb_emit_sort_init (x, (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)]));
        if (CB_FILE_P (cb_ref (x)) && (yyvsp[(2) - (4)]) == NULL) {
                cb_error (_("File sort requires KEY phrase"));
        }
    /* used in sort_input/sort_output */
    save_tree_1 = x;
    sort_input_proc = 0;
  ;}
    break;

  case 1267:

/* Line 1455 of yacc.c  */
#line 6889 "parser.y"
    {
        cb_emit_sort_finish (save_tree_1);
  ;}
    break;

  case 1268:

/* Line 1455 of yacc.c  */
#line 6896 "parser.y"
    {
        (yyval) = NULL;
  ;}
    break;

  case 1269:

/* Line 1455 of yacc.c  */
#line 6901 "parser.y"
    {
        cb_tree l;
        void *e5 =  (yyvsp[(5) - (5)]);
        if (e5 == NULL) {
            e5 = cb_list_init (NULL);
        }
        for (l = e5; l; l = CB_CHAIN (l)) {
                CB_PURPOSE (l) = (yyvsp[(3) - (5)]);
        }
        (yyval) = cb_list_append ((yyvsp[(1) - (5)]), e5);
  ;}
    break;

  case 1270:

/* Line 1455 of yacc.c  */
#line 6915 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1271:

/* Line 1455 of yacc.c  */
#line 6916 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1273:

/* Line 1455 of yacc.c  */
#line 6920 "parser.y"
    { /* nothing */ ;}
    break;

  case 1274:

/* Line 1455 of yacc.c  */
#line 6924 "parser.y"
    { (yyval) = cb_null; ;}
    break;

  case 1275:

/* Line 1455 of yacc.c  */
#line 6925 "parser.y"
    { (yyval) = cb_ref ((yyvsp[(3) - (3)])); ;}
    break;

  case 1276:

/* Line 1455 of yacc.c  */
#line 6930 "parser.y"
    {
    if (CB_FILE_P (cb_ref (save_tree_1))) {
                cb_error (_("File sort requires USING or INPUT PROCEDURE"));
        }
  ;}
    break;

  case 1277:

/* Line 1455 of yacc.c  */
#line 6936 "parser.y"
    {
    if (!CB_FILE_P (cb_ref (save_tree_1))) {
                cb_error (_("USING invalid with table SORT"));
        } else {
        cb_emit_sort_using (save_tree_1, (yyvsp[(2) - (2)]));
    }
  ;}
    break;

  case 1278:

/* Line 1455 of yacc.c  */
#line 6944 "parser.y"
    {
    if (!CB_FILE_P (cb_ref (save_tree_1))) {
                cb_error (_("INPUT PROCEDURE invalid with table SORT"));
        } else {
        cb_emit_sort_input (save_tree_1, (yyvsp[(4) - (4)]));
        sort_input_proc = 1;
    }
  ;}
    break;

  case 1279:

/* Line 1455 of yacc.c  */
#line 6956 "parser.y"
    {
    if (CB_FILE_P (cb_ref (save_tree_1))) {
                cb_error (_("File sort requires GIVING or OUTPUT PROCEDURE"));
        }
  ;}
    break;

  case 1280:

/* Line 1455 of yacc.c  */
#line 6962 "parser.y"
    {
    if (!CB_FILE_P (cb_ref (save_tree_1))) {
                cb_error (_("GIVING invalid with table SORT"));
        } else {
        cb_emit_sort_giving (save_tree_1, (yyvsp[(2) - (2)]), sort_input_proc);
    }
  ;}
    break;

  case 1281:

/* Line 1455 of yacc.c  */
#line 6970 "parser.y"
    {
    if (!CB_FILE_P (cb_ref (save_tree_1))) {
                cb_error (_("OUTPUT PROCEDURE invalid with table SORT"));
        } else {
        cb_emit_sort_output (save_tree_1, (yyvsp[(4) - (4)]));
    }
  ;}
    break;

  case 1282:

/* Line 1455 of yacc.c  */
#line 6985 "parser.y"
    { BEGIN_STATEMENT ("START", TERM_START); ;}
    break;

  case 1283:

/* Line 1455 of yacc.c  */
#line 6986 "parser.y"
    { (yyval) = cb_int (COB_EQ); start_cond = (yyval);;}
    break;

  case 1284:

/* Line 1455 of yacc.c  */
#line 6989 "parser.y"
    {
        
        if (CB_FILE_P (cb_ref ((yyvsp[(3) - (8)])))) {
                if (CB_FILE (cb_ref ((yyvsp[(3) - (8)])))->organization != COB_ORG_INDEXED &&
                     CB_FILE (cb_ref ((yyvsp[(3) - (8)])))->organization != COB_ORG_RELATIVE) {
                        cb_error (_("START not allowed on SEQUENTIAL files"));
                        (yyval) = cb_error_node;
                } else {
                        cb_emit_start ((yyvsp[(3) - (8)]), start_cond, (yyvsp[(6) - (8)]));
                }
        } else {
                if ((yyvsp[(3) - (8)]) != cb_error_node) {
                    cb_error_x ((yyvsp[(3) - (8)]), _("'%s' is not a file name"), CB_NAME ((yyvsp[(3) - (8)])));
                }
                (yyval) = cb_error_node;
        }
  ;}
    break;

  case 1285:

/* Line 1455 of yacc.c  */
#line 7009 "parser.y"
    { start_cond = cb_int (COB_EQ); (yyval) = NULL; ;}
    break;

  case 1286:

/* Line 1455 of yacc.c  */
#line 7010 "parser.y"
    { start_cond = (yyvsp[(3) - (4)]); (yyval) = (yyvsp[(4) - (4)]); ;}
    break;

  case 1287:

/* Line 1455 of yacc.c  */
#line 7014 "parser.y"
    { (yyval) = cb_int (((yyvsp[(1) - (2)]) == cb_int1) ? COB_NE : COB_EQ); ;}
    break;

  case 1288:

/* Line 1455 of yacc.c  */
#line 7015 "parser.y"
    { (yyval) = cb_int (((yyvsp[(1) - (2)]) == cb_int1) ? COB_LE : COB_GT); ;}
    break;

  case 1289:

/* Line 1455 of yacc.c  */
#line 7016 "parser.y"
    { (yyval) = cb_int (((yyvsp[(1) - (2)]) == cb_int1) ? COB_GE : COB_LT); ;}
    break;

  case 1290:

/* Line 1455 of yacc.c  */
#line 7017 "parser.y"
    { (yyval) = cb_int (((yyvsp[(1) - (2)]) == cb_int1) ? COB_LT : COB_GE); ;}
    break;

  case 1291:

/* Line 1455 of yacc.c  */
#line 7018 "parser.y"
    { (yyval) = cb_int (((yyvsp[(1) - (2)]) == cb_int1) ? COB_GT : COB_LE); ;}
    break;

  case 1292:

/* Line 1455 of yacc.c  */
#line 7022 "parser.y"
    { terminator_warning (TERM_START); ;}
    break;

  case 1293:

/* Line 1455 of yacc.c  */
#line 7023 "parser.y"
    { terminator_clear (TERM_START); ;}
    break;

  case 1294:

/* Line 1455 of yacc.c  */
#line 7032 "parser.y"
    { BEGIN_STATEMENT ("STOP", 0); ;}
    break;

  case 1296:

/* Line 1455 of yacc.c  */
#line 7039 "parser.y"
    {
        cb_emit_stop_run ((yyvsp[(2) - (2)]));
  ;}
    break;

  case 1297:

/* Line 1455 of yacc.c  */
#line 7043 "parser.y"
    {
        cb_verify (cb_stop_literal_statement, "STOP literal");
  ;}
    break;

  case 1298:

/* Line 1455 of yacc.c  */
#line 7049 "parser.y"
    {
        cb_emit_stop_abend ((yyvsp[(2) - (2)])); 
  ;}
    break;

  case 1299:

/* Line 1455 of yacc.c  */
#line 7056 "parser.y"
    { (yyval) = current_program->cb_return_code; ;}
    break;

  case 1300:

/* Line 1455 of yacc.c  */
#line 7057 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1301:

/* Line 1455 of yacc.c  */
#line 7058 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1302:

/* Line 1455 of yacc.c  */
#line 7059 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1303:

/* Line 1455 of yacc.c  */
#line 7068 "parser.y"
    { BEGIN_STATEMENT ("STRING", TERM_STRING); ;}
    break;

  case 1304:

/* Line 1455 of yacc.c  */
#line 7071 "parser.y"
    {
        cb_emit_string ((yyvsp[(3) - (8)]), (yyvsp[(5) - (8)]), (yyvsp[(6) - (8)]));
  ;}
    break;

  case 1305:

/* Line 1455 of yacc.c  */
#line 7077 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1306:

/* Line 1455 of yacc.c  */
#line 7078 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1307:

/* Line 1455 of yacc.c  */
#line 7082 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1308:

/* Line 1455 of yacc.c  */
#line 7083 "parser.y"
    { (yyval) = cb_build_pair (cb_int0, NULL); ;}
    break;

  case 1309:

/* Line 1455 of yacc.c  */
#line 7084 "parser.y"
    { (yyval) = cb_build_pair ((yyvsp[(3) - (3)]), NULL); ;}
    break;

  case 1310:

/* Line 1455 of yacc.c  */
#line 7088 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1311:

/* Line 1455 of yacc.c  */
#line 7089 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1312:

/* Line 1455 of yacc.c  */
#line 7093 "parser.y"
    { terminator_warning (TERM_STRING); ;}
    break;

  case 1313:

/* Line 1455 of yacc.c  */
#line 7094 "parser.y"
    { terminator_clear (TERM_STRING); ;}
    break;

  case 1314:

/* Line 1455 of yacc.c  */
#line 7103 "parser.y"
    { BEGIN_STATEMENT ("SUBTRACT", TERM_SUBTRACT); rounded_used = 0;;}
    break;

  case 1316:

/* Line 1455 of yacc.c  */
#line 7110 "parser.y"
    {
   /*CIT*/  
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
        if (current_statement->handler1 || current_statement->handler2) {
            cb_check_optimized_field((yyvsp[(3) - (4)]), 0);
            cb_check_optimized_field((yyvsp[(1) - (4)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(3) - (4)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(3) - (4)]), '-', cb_build_binary_list ((yyvsp[(1) - (4)]), '+'), !rounded_used);
  ;}
    break;

  case 1317:

/* Line 1455 of yacc.c  */
#line 7124 "parser.y"
    {
   /*CIT*/
        void * e3 = (yyvsp[(3) - (6)]);
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
        if (current_statement->handler1 || current_statement->handler2) {
            cb_check_optimized_field((yyvsp[(5) - (6)]), 0);
            e3 = cb_check_optimized_field_1(e3);
            cb_check_optimized_field((yyvsp[(1) - (6)]), 0);
        } else {
            cb_check_optimized_field((yyvsp[(5) - (6)]), 1);
        }
        cb_emit_arithmetic ((yyvsp[(5) - (6)]), 0, cb_build_binary_list (cb_cons (e3, (yyvsp[(1) - (6)])), '-'), !rounded_used );
  ;}
    break;

  case 1318:

/* Line 1455 of yacc.c  */
#line 7140 "parser.y"
    {
        void *e2 = (yyvsp[(2) - (6)]);
        void *e4 = (yyvsp[(4) - (6)]);
        if (CB_EXCEPTION_ENABLE (COB_EC_SIZE_OVERFLOW) ) {
            current_statement->handler_id = COB_EC_SIZE_OVERFLOW;
        } 
        if (current_statement->handler1 || current_statement->handler2 || (yyvsp[(5) - (6)]) != cb_int0) {
            e2 = cb_check_optimized_field_1(e2);
            e4 = cb_check_optimized_field_1(e4);
        }
        cb_emit_corresponding (cb_build_sub, e4, e2, (yyvsp[(5) - (6)]));
  ;}
    break;

  case 1319:

/* Line 1455 of yacc.c  */
#line 7155 "parser.y"
    { terminator_warning (TERM_SUBTRACT); ;}
    break;

  case 1320:

/* Line 1455 of yacc.c  */
#line 7156 "parser.y"
    { terminator_clear (TERM_SUBTRACT); ;}
    break;

  case 1321:

/* Line 1455 of yacc.c  */
#line 7166 "parser.y"
    {
    BEGIN_STATEMENT ("SUPPRESS", 0);
    PENDING("SUPPRESS");
  ;}
    break;

  case 1324:

/* Line 1455 of yacc.c  */
#line 7181 "parser.y"
    { BEGIN_STATEMENT ("TERMINATE", 0); ;}
    break;

  case 1325:

/* Line 1455 of yacc.c  */
#line 7183 "parser.y"
    {
    PENDING("TERMINATE");
  ;}
    break;

  case 1326:

/* Line 1455 of yacc.c  */
#line 7194 "parser.y"
    { BEGIN_STATEMENT ("TRANSFORM", 0); ;}
    break;

  case 1327:

/* Line 1455 of yacc.c  */
#line 7196 "parser.y"
    {
    cb_tree     x;

    x = cb_build_converting ((yyvsp[(5) - (7)]), (yyvsp[(7) - (7)]), cb_build_inspect_region_start ());
    cb_emit_inspect ((yyvsp[(3) - (7)]), x, cb_int0, 2);
  ;}
    break;

  case 1328:

/* Line 1455 of yacc.c  */
#line 7210 "parser.y"
    { BEGIN_STATEMENT ("UNLOCK", 0); ;}
    break;

  case 1329:

/* Line 1455 of yacc.c  */
#line 7212 "parser.y"
    {
        if ((yyvsp[(3) - (4)]) != cb_error_node) {
                cb_emit_unlock ((yyvsp[(3) - (4)]));
        }
  ;}
    break;

  case 1333:

/* Line 1455 of yacc.c  */
#line 7231 "parser.y"
    { BEGIN_STATEMENT ("UNSTRING", TERM_UNSTRING); ;}
    break;

  case 1334:

/* Line 1455 of yacc.c  */
#line 7235 "parser.y"
    {
        cb_emit_unstring ((yyvsp[(3) - (9)]), (yyvsp[(4) - (9)]), (yyvsp[(5) - (9)]), (yyvsp[(6) - (9)]), (yyvsp[(7) - (9)]));
  ;}
    break;

  case 1335:

/* Line 1455 of yacc.c  */
#line 7241 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1336:

/* Line 1455 of yacc.c  */
#line 7243 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1337:

/* Line 1455 of yacc.c  */
#line 7247 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1338:

/* Line 1455 of yacc.c  */
#line 7249 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 1339:

/* Line 1455 of yacc.c  */
#line 7254 "parser.y"
    {
        (yyval) = cb_build_unstring_delimited ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]));
  ;}
    break;

  case 1340:

/* Line 1455 of yacc.c  */
#line 7260 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(2) - (2)])); ;}
    break;

  case 1341:

/* Line 1455 of yacc.c  */
#line 7262 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1342:

/* Line 1455 of yacc.c  */
#line 7267 "parser.y"
    {
        (yyval) = cb_build_unstring_into ((yyvsp[(1) - (3)]), (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1343:

/* Line 1455 of yacc.c  */
#line 7273 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1344:

/* Line 1455 of yacc.c  */
#line 7274 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1345:

/* Line 1455 of yacc.c  */
#line 7278 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1346:

/* Line 1455 of yacc.c  */
#line 7279 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1347:

/* Line 1455 of yacc.c  */
#line 7283 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1348:

/* Line 1455 of yacc.c  */
#line 7284 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1349:

/* Line 1455 of yacc.c  */
#line 7288 "parser.y"
    { terminator_warning (TERM_UNSTRING); ;}
    break;

  case 1350:

/* Line 1455 of yacc.c  */
#line 7289 "parser.y"
    { terminator_clear (TERM_UNSTRING); ;}
    break;

  case 1354:

/* Line 1455 of yacc.c  */
#line 7307 "parser.y"
    {
        if (!in_declaratives) {
                cb_error (_("USE statement must be within DECLARATIVES"));
        } else if (!current_section) {
                cb_error (_("SECTION header missing before USE statement"));
        } else {
                current_section->need_begin = 1;
                current_section->need_return = 1;
                current_section->is_exception = 1;
                CB_EXCEPTION_ENABLE (COB_EC_I_O) = 1;
                if (use_global_ind) {
                    current_section->is_global = 1;
                    current_program->global_list =
                    cb_list_add (current_program->global_list,
                         CB_TREE (current_section));
                }
        }
  ;}
    break;

  case 1355:

/* Line 1455 of yacc.c  */
#line 7329 "parser.y"
    {
    use_global_ind = 0;
  ;}
    break;

  case 1356:

/* Line 1455 of yacc.c  */
#line 7333 "parser.y"
    {
    use_global_ind = 1;
    current_program->flag_global_use = 1;
  ;}
    break;

  case 1357:

/* Line 1455 of yacc.c  */
#line 7341 "parser.y"
    {
        cb_tree l;

        for (l = (yyvsp[(1) - (1)]); l; l = CB_CHAIN (l)) {
                if (CB_VALUE (l) != cb_error_node) {
            setup_use_file (CB_FILE (cb_ref (CB_VALUE (l))));
        }
    }
  ;}
    break;

  case 1358:

/* Line 1455 of yacc.c  */
#line 7351 "parser.y"
    {
    current_program->global_handler[COB_OPEN_INPUT].handler_label = current_section;
    current_program->global_handler[COB_OPEN_INPUT].handler_prog = current_program;
  ;}
    break;

  case 1359:

/* Line 1455 of yacc.c  */
#line 7356 "parser.y"
    {
    current_program->global_handler[COB_OPEN_OUTPUT].handler_label = current_section;
    current_program->global_handler[COB_OPEN_OUTPUT].handler_prog = current_program;
  ;}
    break;

  case 1360:

/* Line 1455 of yacc.c  */
#line 7361 "parser.y"
    {
    current_program->global_handler[COB_OPEN_I_O].handler_label = current_section;
    current_program->global_handler[COB_OPEN_I_O].handler_prog = current_program;
  ;}
    break;

  case 1361:

/* Line 1455 of yacc.c  */
#line 7366 "parser.y"
    {
    current_program->global_handler[COB_OPEN_EXTEND].handler_label = current_section;
    current_program->global_handler[COB_OPEN_EXTEND].handler_prog = current_program;
  ;}
    break;

  case 1374:

/* Line 1455 of yacc.c  */
#line 7398 "parser.y"
    {
        PENDING ("USE FOR DEBUGGING");
  ;}
    break;

  case 1377:

/* Line 1455 of yacc.c  */
#line 7410 "parser.y"
    {
    PENDING ("USE BEFORE REPORTING");
  ;}
    break;

  case 1378:

/* Line 1455 of yacc.c  */
#line 7421 "parser.y"
    { BEGIN_STATEMENT ("WRITE", TERM_WRITE); ;}
    break;

  case 1379:

/* Line 1455 of yacc.c  */
#line 7424 "parser.y"
    {
        if (((yyvsp[(3) - (8)]) != cb_error_node) && (yyvsp[(3) - (8)])) {
                cb_emit_write ((yyvsp[(3) - (8)]), (yyvsp[(4) - (8)]), (yyvsp[(6) - (8)]), (yyvsp[(5) - (8)]));
          if (cb_flag_fdclear){
              current_statement->final = cb_list_add(current_statement->final,
                 cb_build_initialize_vars(cb_list_init ((yyvsp[(3) - (8)])),NULL,NULL,NULL,cb_flag_initalize_to_value || cb_flag_initialize_to_value ?  cb_true: NULL, 1));
           }
      }
  ;}
    break;

  case 1380:

/* Line 1455 of yacc.c  */
#line 7436 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1381:

/* Line 1455 of yacc.c  */
#line 7437 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1382:

/* Line 1455 of yacc.c  */
#line 7442 "parser.y"
    {
        (yyval) = cb_int0;
  ;}
    break;

  case 1383:

/* Line 1455 of yacc.c  */
#line 7446 "parser.y"
    {
        (yyval) = cb_build_write_advancing_lines ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]));
  ;}
    break;

  case 1384:

/* Line 1455 of yacc.c  */
#line 7450 "parser.y"
    {
        (yyval) = cb_build_write_advancing_mnemonic ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1385:

/* Line 1455 of yacc.c  */
#line 7454 "parser.y"
    {
        (yyval) = cb_build_write_advancing_page ((yyvsp[(1) - (3)]));
  ;}
    break;

  case 1386:

/* Line 1455 of yacc.c  */
#line 7460 "parser.y"
    { (yyval) = CB_BEFORE; ;}
    break;

  case 1387:

/* Line 1455 of yacc.c  */
#line 7461 "parser.y"
    { (yyval) = CB_AFTER; ;}
    break;

  case 1391:

/* Line 1455 of yacc.c  */
#line 7470 "parser.y"
    { terminator_warning (TERM_WRITE); ;}
    break;

  case 1392:

/* Line 1455 of yacc.c  */
#line 7471 "parser.y"
    { terminator_clear (TERM_WRITE); ;}
    break;

  case 1393:

/* Line 1455 of yacc.c  */
#line 7486 "parser.y"
    {
        current_statement->handler_id = COB_EC_IMP_ACCEPT;
  ;}
    break;

  case 1394:

/* Line 1455 of yacc.c  */
#line 7494 "parser.y"
    {
        current_statement->handler_id = COB_EC_IMP_DISPLAY;
  ;}
    break;

  case 1395:

/* Line 1455 of yacc.c  */
#line 7502 "parser.y"
    {
        current_statement->handler_id = COB_EC_XML;
  ;}
    break;

  case 1397:

/* Line 1455 of yacc.c  */
#line 7509 "parser.y"
    {
    save_display_column = display_column;
    save_display_line = display_line;
    save_display_fgc = display_fgc;
    save_display_bgc = display_bgc;
    save_scroll = scroll;
    save_dispattrs = dispattrs;
    save_dispprompt = dispprompt; 
    check_unreached = 0;
  ;}
    break;

  case 1398:

/* Line 1455 of yacc.c  */
#line 7521 "parser.y"
    {
    current_statement->handler1 = (yyvsp[(4) - (4)]);
    display_column = save_display_column;
    display_line = save_display_line;
    display_fgc = save_display_fgc;
    display_bgc = save_display_bgc;
    scroll = save_scroll;
    dispattrs = save_dispattrs; 
    dispprompt = save_dispprompt;
  ;}
    break;

  case 1400:

/* Line 1455 of yacc.c  */
#line 7535 "parser.y"
    {
    save_display_column = display_column;
    save_display_line = display_line;
    save_display_fgc = display_fgc;
    save_display_bgc = display_bgc;
    save_scroll = scroll;
    save_dispattrs = dispattrs;
    save_dispprompt = dispprompt; 
    check_unreached = 0;
  ;}
    break;

  case 1401:

/* Line 1455 of yacc.c  */
#line 7546 "parser.y"
    {
    current_statement->handler2 = (yyvsp[(3) - (3)]);
    display_column = save_display_column;
    display_line = save_display_line;
    display_fgc = save_display_fgc;
    display_bgc = save_display_bgc;
    scroll = save_scroll;
    dispattrs = save_dispattrs; 
    dispprompt = save_dispprompt;
  ;}
    break;

  case 1403:

/* Line 1455 of yacc.c  */
#line 7560 "parser.y"
    {
    acc_exception_field = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1404:

/* Line 1455 of yacc.c  */
#line 7571 "parser.y"
    {
        current_statement->handler_id = COB_EC_SIZE;
  ;}
    break;

  case 1406:

/* Line 1455 of yacc.c  */
#line 7578 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1407:

/* Line 1455 of yacc.c  */
#line 7582 "parser.y"
    {
    current_statement->handler1 = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1409:

/* Line 1455 of yacc.c  */
#line 7589 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1410:

/* Line 1455 of yacc.c  */
#line 7593 "parser.y"
    {
    current_statement->handler2 = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1411:

/* Line 1455 of yacc.c  */
#line 7605 "parser.y"
    {
        current_statement->handler_id = COB_EC_OVERFLOW;
  ;}
    break;

  case 1413:

/* Line 1455 of yacc.c  */
#line 7612 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1414:

/* Line 1455 of yacc.c  */
#line 7616 "parser.y"
    {
    current_statement->handler1 = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1416:

/* Line 1455 of yacc.c  */
#line 7623 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1417:

/* Line 1455 of yacc.c  */
#line 7627 "parser.y"
    {
    current_statement->handler2 = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1418:

/* Line 1455 of yacc.c  */
#line 7639 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_AT_END;
        current_statement->handler1 = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1419:

/* Line 1455 of yacc.c  */
#line 7644 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_AT_END;
        current_statement->handler2 = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1420:

/* Line 1455 of yacc.c  */
#line 7649 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_AT_END;
        current_statement->handler1 = (yyvsp[(1) - (2)]);
        current_statement->handler2 = (yyvsp[(2) - (2)]);
  ;}
    break;

  case 1421:

/* Line 1455 of yacc.c  */
#line 7658 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1422:

/* Line 1455 of yacc.c  */
#line 7662 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1423:

/* Line 1455 of yacc.c  */
#line 7669 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1424:

/* Line 1455 of yacc.c  */
#line 7673 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1425:

/* Line 1455 of yacc.c  */
#line 7685 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_EOP;
        current_statement->handler1 = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1426:

/* Line 1455 of yacc.c  */
#line 7690 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_EOP;
        current_statement->handler2 = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1427:

/* Line 1455 of yacc.c  */
#line 7695 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_EOP;
        current_statement->handler1 = (yyvsp[(1) - (2)]);
        current_statement->handler2 = (yyvsp[(2) - (2)]);
  ;}
    break;

  case 1428:

/* Line 1455 of yacc.c  */
#line 7704 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1429:

/* Line 1455 of yacc.c  */
#line 7708 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1430:

/* Line 1455 of yacc.c  */
#line 7715 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1431:

/* Line 1455 of yacc.c  */
#line 7719 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1434:

/* Line 1455 of yacc.c  */
#line 7735 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_INVALID_KEY;
        current_statement->handler1 = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1435:

/* Line 1455 of yacc.c  */
#line 7740 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_INVALID_KEY;
        current_statement->handler2 = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1436:

/* Line 1455 of yacc.c  */
#line 7745 "parser.y"
    {
        current_statement->handler_id = COB_EC_I_O_INVALID_KEY;
        current_statement->handler1 = (yyvsp[(1) - (2)]);
        current_statement->handler2 = (yyvsp[(2) - (2)]);
  ;}
    break;

  case 1437:

/* Line 1455 of yacc.c  */
#line 7754 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1438:

/* Line 1455 of yacc.c  */
#line 7758 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1439:

/* Line 1455 of yacc.c  */
#line 7765 "parser.y"
    {
    check_unreached = 0;
  ;}
    break;

  case 1440:

/* Line 1455 of yacc.c  */
#line 7769 "parser.y"
    {
    (yyval) = (yyvsp[(3) - (3)]);
  ;}
    break;

  case 1441:

/* Line 1455 of yacc.c  */
#line 7779 "parser.y"
    { BEGIN_STATEMENT ("XML", TERM_XML); 
                             ;}
    break;

  case 1443:

/* Line 1455 of yacc.c  */
#line 7788 "parser.y"
    {
      cb_emit_xml_parse((yyvsp[(2) - (8)]), (yyvsp[(8) - (8)]), (yyvsp[(4) - (8)]), (yyvsp[(5) - (8)]));   
   ;}
    break;

  case 1444:

/* Line 1455 of yacc.c  */
#line 7792 "parser.y"
    {
      cb_emit_xml_generate((yyvsp[(2) - (5)]), (yyvsp[(4) - (5)]), (yyvsp[(5) - (5)]));         
   ;}
    break;

  case 1448:

/* Line 1455 of yacc.c  */
#line 7805 "parser.y"
    { (yyval) = NULL;;}
    break;

  case 1449:

/* Line 1455 of yacc.c  */
#line 7806 "parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 1450:

/* Line 1455 of yacc.c  */
#line 7810 "parser.y"
    { terminator_warning (TERM_XML); ;}
    break;

  case 1451:

/* Line 1455 of yacc.c  */
#line 7811 "parser.y"
    { terminator_clear (TERM_XML); ;}
    break;

  case 1452:

/* Line 1455 of yacc.c  */
#line 7815 "parser.y"
    { (yyval) = NULL;;}
    break;

  case 1453:

/* Line 1455 of yacc.c  */
#line 7816 "parser.y"
    { (yyval) = cb_int(1); ;}
    break;

  case 1454:

/* Line 1455 of yacc.c  */
#line 7820 "parser.y"
    { (yyval) = NULL;;}
    break;

  case 1455:

/* Line 1455 of yacc.c  */
#line 7821 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1456:

/* Line 1455 of yacc.c  */
#line 7830 "parser.y"
    {
    (yyval) = cb_one;
  ;}
    break;

  case 1457:

/* Line 1455 of yacc.c  */
#line 7834 "parser.y"
    {
    (yyval) = (yyvsp[(2) - (3)]);
  ;}
    break;

  case 1458:

/* Line 1455 of yacc.c  */
#line 7846 "parser.y"
    {
        (yyval) = cb_build_cond ((yyvsp[(1) - (1)]));
  ;}
    break;

  case 1459:

/* Line 1455 of yacc.c  */
#line 7853 "parser.y"
    {
        (yyval) = cb_build_expr ((yyvsp[(1) - (1)]));
  ;}
    break;

  case 1460:

/* Line 1455 of yacc.c  */
#line 7859 "parser.y"
    {
        current_expr = NULL;
  ;}
    break;

  case 1461:

/* Line 1455 of yacc.c  */
#line 7863 "parser.y"
    {
        (yyval) = cb_list_reverse (current_expr);
  ;}
    break;

  case 1464:

/* Line 1455 of yacc.c  */
#line 7875 "parser.y"
    { push_expr ('x', (yyvsp[(1) - (1)])); ;}
    break;

  case 1465:

/* Line 1455 of yacc.c  */
#line 7877 "parser.y"
    { push_expr ('(', NULL); ;}
    break;

  case 1466:

/* Line 1455 of yacc.c  */
#line 7878 "parser.y"
    { push_expr (')', NULL); ;}
    break;

  case 1467:

/* Line 1455 of yacc.c  */
#line 7880 "parser.y"
    { push_expr ('+', NULL); ;}
    break;

  case 1468:

/* Line 1455 of yacc.c  */
#line 7881 "parser.y"
    { push_expr ('-', NULL); ;}
    break;

  case 1469:

/* Line 1455 of yacc.c  */
#line 7882 "parser.y"
    { push_expr ('*', NULL); ;}
    break;

  case 1470:

/* Line 1455 of yacc.c  */
#line 7883 "parser.y"
    { push_expr ('/', NULL); ;}
    break;

  case 1471:

/* Line 1455 of yacc.c  */
#line 7884 "parser.y"
    { push_expr ('^', NULL); ;}
    break;

  case 1472:

/* Line 1455 of yacc.c  */
#line 7885 "parser.y"
    { push_expr ('a', NULL); ;}
    break;

  case 1473:

/* Line 1455 of yacc.c  */
#line 7886 "parser.y"
    { push_expr ('o', NULL); ;}
    break;

  case 1474:

/* Line 1455 of yacc.c  */
#line 7887 "parser.y"
    { push_expr ('y', NULL); ;}
    break;

  case 1475:

/* Line 1455 of yacc.c  */
#line 7888 "parser.y"
    { push_expr ('n', NULL); ;}
    break;

  case 1476:

/* Line 1455 of yacc.c  */
#line 7890 "parser.y"
    { push_expr ('=', NULL); ;}
    break;

  case 1477:

/* Line 1455 of yacc.c  */
#line 7891 "parser.y"
    { push_expr ('>', NULL); ;}
    break;

  case 1478:

/* Line 1455 of yacc.c  */
#line 7892 "parser.y"
    { push_expr ('<', NULL); ;}
    break;

  case 1479:

/* Line 1455 of yacc.c  */
#line 7893 "parser.y"
    { push_expr (']', NULL); ;}
    break;

  case 1480:

/* Line 1455 of yacc.c  */
#line 7894 "parser.y"
    { push_expr ('[', NULL); ;}
    break;

  case 1481:

/* Line 1455 of yacc.c  */
#line 7895 "parser.y"
    { push_expr ('~', NULL); ;}
    break;

  case 1482:

/* Line 1455 of yacc.c  */
#line 7896 "parser.y"
    { push_expr ('~', NULL); ;}
    break;

  case 1483:

/* Line 1455 of yacc.c  */
#line 7897 "parser.y"
    { push_expr ('[', NULL); ;}
    break;

  case 1484:

/* Line 1455 of yacc.c  */
#line 7898 "parser.y"
    { push_expr (']', NULL); ;}
    break;

  case 1485:

/* Line 1455 of yacc.c  */
#line 7899 "parser.y"
    { push_expr ('<', NULL); ;}
    break;

  case 1486:

/* Line 1455 of yacc.c  */
#line 7900 "parser.y"
    { push_expr ('>', NULL); ;}
    break;

  case 1487:

/* Line 1455 of yacc.c  */
#line 7901 "parser.y"
    { push_expr ('=', NULL); ;}
    break;

  case 1488:

/* Line 1455 of yacc.c  */
#line 7903 "parser.y"
    { push_expr ('!', NULL); ;}
    break;

  case 1489:

/* Line 1455 of yacc.c  */
#line 7904 "parser.y"
    { push_expr ('&', NULL); ;}
    break;

  case 1490:

/* Line 1455 of yacc.c  */
#line 7905 "parser.y"
    { push_expr ('|', NULL); ;}
    break;

  case 1491:

/* Line 1455 of yacc.c  */
#line 7907 "parser.y"
    { push_expr ('O', NULL); ;}
    break;

  case 1492:

/* Line 1455 of yacc.c  */
#line 7908 "parser.y"
    { push_expr ('9', NULL); ;}
    break;

  case 1493:

/* Line 1455 of yacc.c  */
#line 7909 "parser.y"
    { push_expr ('A', NULL); ;}
    break;

  case 1494:

/* Line 1455 of yacc.c  */
#line 7910 "parser.y"
    { push_expr ('L', NULL); ;}
    break;

  case 1495:

/* Line 1455 of yacc.c  */
#line 7911 "parser.y"
    { push_expr ('U', NULL); ;}
    break;

  case 1496:

/* Line 1455 of yacc.c  */
#line 7914 "parser.y"
    { push_expr ('P', NULL); ;}
    break;

  case 1497:

/* Line 1455 of yacc.c  */
#line 7915 "parser.y"
    { push_expr ('N', NULL); ;}
    break;

  case 1498:

/* Line 1455 of yacc.c  */
#line 7916 "parser.y"
    { push_expr ('S', NULL); ;}
    break;

  case 1499:

/* Line 1455 of yacc.c  */
#line 7917 "parser.y"
    { push_expr ('F', NULL); ;}
    break;

  case 1512:

/* Line 1455 of yacc.c  */
#line 7930 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1513:

/* Line 1455 of yacc.c  */
#line 7931 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 1514:

/* Line 1455 of yacc.c  */
#line 7935 "parser.y"
    {(yyval) = (yyvsp[(1) - (1)]);;}
    break;

  case 1515:

/* Line 1455 of yacc.c  */
#line 7936 "parser.y"
    {(yyval) = cb_int0; ;}
    break;

  case 1516:

/* Line 1455 of yacc.c  */
#line 7940 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1517:

/* Line 1455 of yacc.c  */
#line 7941 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 1521:

/* Line 1455 of yacc.c  */
#line 7950 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1522:

/* Line 1455 of yacc.c  */
#line 7951 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), '+', (yyvsp[(3) - (3)])); ;}
    break;

  case 1523:

/* Line 1455 of yacc.c  */
#line 7952 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), '-', (yyvsp[(3) - (3)])); ;}
    break;

  case 1524:

/* Line 1455 of yacc.c  */
#line 7953 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), '*', (yyvsp[(3) - (3)])); ;}
    break;

  case 1525:

/* Line 1455 of yacc.c  */
#line 7954 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), '/', (yyvsp[(3) - (3)])); ;}
    break;

  case 1526:

/* Line 1455 of yacc.c  */
#line 7955 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 1527:

/* Line 1455 of yacc.c  */
#line 7956 "parser.y"
    { (yyval) = cb_build_binary_op (cb_zero, '-', (yyvsp[(2) - (2)])); /*  cb_build_sign_value($2);*/;}
    break;

  case 1528:

/* Line 1455 of yacc.c  */
#line 7957 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), '^', (yyvsp[(3) - (3)])); ;}
    break;

  case 1529:

/* Line 1455 of yacc.c  */
#line 7958 "parser.y"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 1530:

/* Line 1455 of yacc.c  */
#line 7959 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), 'a', (yyvsp[(3) - (3)])); ;}
    break;

  case 1531:

/* Line 1455 of yacc.c  */
#line 7960 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), 'o', (yyvsp[(3) - (3)])); ;}
    break;

  case 1532:

/* Line 1455 of yacc.c  */
#line 7961 "parser.y"
    { (yyval) = cb_build_binary_op ((yyvsp[(1) - (3)]), 'y', (yyvsp[(3) - (3)])); ;}
    break;

  case 1533:

/* Line 1455 of yacc.c  */
#line 7962 "parser.y"
    { (yyval) = cb_build_not_value ((yyvsp[(2) - (2)])); ;}
    break;

  case 1534:

/* Line 1455 of yacc.c  */
#line 7974 "parser.y"
    {
        if (current_linage > 1) {
                cb_error (_("LINAGE-COUNTER must be qualified here"));
                (yyval) = cb_error_node;
        } else if (current_linage == 0) {
                cb_error (_("Invalid LINAGE-COUNTER usage"));
                (yyval) = cb_error_node;
        } else {
                (yyval) = linage_file->linage_ctr;
        }
  ;}
    break;

  case 1535:

/* Line 1455 of yacc.c  */
#line 7986 "parser.y"
    {
        if (CB_FILE_P (cb_ref ((yyvsp[(3) - (3)])))) {
                (yyval) = CB_FILE (cb_ref ((yyvsp[(3) - (3)])))->linage_ctr;
        } else {
                cb_error_x ((yyvsp[(3) - (3)]), _("'%s' is not a file name"), CB_NAME ((yyvsp[(3) - (3)])));
                (yyval) = cb_error_node;
        }
  ;}
    break;

  case 1536:

/* Line 1455 of yacc.c  */
#line 8000 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1537:

/* Line 1455 of yacc.c  */
#line 8002 "parser.y"
    { (yyval) = cb_list_append ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1538:

/* Line 1455 of yacc.c  */
#line 8006 "parser.y"
    { (yyval) = cb_build_pair ((yyvsp[(2) - (2)]), (yyvsp[(1) - (2)])); ;}
    break;

  case 1539:

/* Line 1455 of yacc.c  */
#line 8012 "parser.y"
    { cb_build_identifier ((yyvsp[(1) - (1)])); ;}
    break;

  case 1540:

/* Line 1455 of yacc.c  */
#line 8019 "parser.y"
    {
        cb_tree x;

        x = cb_ref ((yyvsp[(1) - (1)]));
        if (!CB_FIELD_P (x)) {
                (yyval) = cb_error_node;
        } else if (!CB_FIELD (x)->index_list) {
                cb_error_x ((yyvsp[(1) - (1)]), _("'%s' not indexed"), cb_name ((yyvsp[(1) - (1)])));
                cb_error_x (x, _("'%s' defined here"), cb_name (x));
                (yyval) = cb_error_node;
        } else {
                (yyval) = (yyvsp[(1) - (1)]);
        }
  ;}
    break;

  case 1541:

/* Line 1455 of yacc.c  */
#line 8039 "parser.y"
    {
    (yyval) = cb_list_init ((yyvsp[(1) - (1)]));
  ;}
    break;

  case 1542:

/* Line 1455 of yacc.c  */
#line 8043 "parser.y"
    {
    cb_tree     l;

    if ((yyvsp[(2) - (2)]) != cb_error_node) {
        for (l = (yyvsp[(1) - (2)]); l; l = CB_CHAIN (l)) {
            if (!strcasecmp (CB_NAME ((yyvsp[(2) - (2)])), CB_NAME (CB_VALUE (l)))) {
                cb_error_x ((yyvsp[(2) - (2)]), _("Multiple reference to '%s' "), CB_NAME ((yyvsp[(2) - (2)])));
            }
        }
        (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]));
    }
  ;}
    break;

  case 1543:

/* Line 1455 of yacc.c  */
#line 8059 "parser.y"
    {
        /*
        if (CB_FILE_P (cb_ref ($1))) {
                $$ = $1;
        } else {
        */
            cb_tree x;
            x = cb_build_file_reference(CB_NAME((yyvsp[(1) - (1)])));
            if (CB_FILE_P (cb_ref (x))) {
                (yyval) = x;          
            } else {
                    cb_error_x ((yyvsp[(1) - (1)]), _("'%s' is not a file name"), CB_NAME ((yyvsp[(1) - (1)])));
                    (yyval) = cb_error_node;
            }
        /*}*/
  ;}
    break;

  case 1544:

/* Line 1455 of yacc.c  */
#line 8080 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1545:

/* Line 1455 of yacc.c  */
#line 8082 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1546:

/* Line 1455 of yacc.c  */
#line 8086 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1547:

/* Line 1455 of yacc.c  */
#line 8092 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1548:

/* Line 1455 of yacc.c  */
#line 8094 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1549:

/* Line 1455 of yacc.c  */
#line 8099 "parser.y"
    {
        (yyval) = (yyvsp[(1) - (1)]);
        CB_REFERENCE ((yyval))->offset = CB_TREE (current_section);
        current_program->label_list = cb_cons ((yyval), current_program->label_list);
  ;}
    break;

  case 1553:

/* Line 1455 of yacc.c  */
#line 8114 "parser.y"
    {
        if (CB_LITERAL((yyvsp[(1) - (1)])) -> image) {
           (yyval) = cb_build_label_reference(cb_build_reference ((char *)(CB_LITERAL ((yyvsp[(1) - (1)]))->image)));
        } else {
           (yyval) = cb_build_label_reference(cb_build_reference ((char *)(CB_LITERAL ((yyvsp[(1) - (1)]))->data)));
        }
        (yyval)->source_file = (yyvsp[(1) - (1)])->source_file;
        (yyval)->source_line = (yyvsp[(1) - (1)])->source_line;
  ;}
    break;

  case 1554:

/* Line 1455 of yacc.c  */
#line 8128 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1555:

/* Line 1455 of yacc.c  */
#line 8129 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1556:

/* Line 1455 of yacc.c  */
#line 8134 "parser.y"
    {
        (yyval) = (yyvsp[(1) - (1)]);
        if (CB_REFERENCE_P((yyval)))
            CB_REFERENCE((yyval))->cb_ref_optinal = cb_warn_undefine;
        current_program->reference_list = cb_cons ((yyval), current_program->reference_list);
  ;}
    break;

  case 1557:

/* Line 1455 of yacc.c  */
#line 8143 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1558:

/* Line 1455 of yacc.c  */
#line 8144 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1561:

/* Line 1455 of yacc.c  */
#line 8156 "parser.y"
    {
        (yyval) = (yyvsp[(1) - (1)]);
        if (CB_REFERENCE ((yyval))->word->count > 0) {                
                redefinition_error ((yyval));
                (yyval) = cb_error_node;
        }
  ;}
    break;

  case 1562:

/* Line 1455 of yacc.c  */
#line 8166 "parser.y"
    {(yyval) = NULL;;}
    break;

  case 1563:

/* Line 1455 of yacc.c  */
#line 8167 "parser.y"
    {(yyval) = (yyvsp[(1) - (1)]);;}
    break;

  case 1564:

/* Line 1455 of yacc.c  */
#line 8179 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1565:

/* Line 1455 of yacc.c  */
#line 8180 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1566:

/* Line 1455 of yacc.c  */
#line 8181 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 1568:

/* Line 1455 of yacc.c  */
#line 8186 "parser.y"
    { (yyval) = cb_build_address ((yyvsp[(3) - (3)])); ;}
    break;

  case 1569:

/* Line 1455 of yacc.c  */
#line 8190 "parser.y"
    { (yyval) = cb_list_init ((yyvsp[(1) - (1)])); ;}
    break;

  case 1570:

/* Line 1455 of yacc.c  */
#line 8191 "parser.y"
    { (yyval) = cb_list_add ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 1572:

/* Line 1455 of yacc.c  */
#line 8196 "parser.y"
    { (yyval) = cb_build_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 1573:

/* Line 1455 of yacc.c  */
#line 8197 "parser.y"
    { (yyval) = cb_build_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 1574:

/* Line 1455 of yacc.c  */
#line 8198 "parser.y"
    { (yyval) = cb_build_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 1575:

/* Line 1455 of yacc.c  */
#line 8199 "parser.y"
    { (yyval) = cb_build_ppointer ((yyvsp[(4) - (4)])); ;}
    break;

  case 1576:

/* Line 1455 of yacc.c  */
#line 8200 "parser.y"
    { (yyval) = cb_build_address ((yyvsp[(3) - (3)])); ;}
    break;

  case 1581:

/* Line 1455 of yacc.c  */
#line 8208 "parser.y"
    { (yyval) = cb_build_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 1582:

/* Line 1455 of yacc.c  */
#line 8209 "parser.y"
    { (yyval) = cb_build_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 1583:

/* Line 1455 of yacc.c  */
#line 8210 "parser.y"
    { (yyval) = cb_build_length ((yyvsp[(3) - (3)])); ;}
    break;

  case 1589:

/* Line 1455 of yacc.c  */
#line 8222 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1590:

/* Line 1455 of yacc.c  */
#line 8223 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1597:

/* Line 1455 of yacc.c  */
#line 8246 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1602:

/* Line 1455 of yacc.c  */
#line 8258 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1606:

/* Line 1455 of yacc.c  */
#line 8265 "parser.y"
    { (yyval) = cb_zero; ;}
    break;

  case 1609:

/* Line 1455 of yacc.c  */
#line 8271 "parser.y"
    { (yyval) = cb_zero; ;}
    break;

  case 1610:

/* Line 1455 of yacc.c  */
#line 8275 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1612:

/* Line 1455 of yacc.c  */
#line 8290 "parser.y"
    { (yyval) = cb_build_identifier ((yyvsp[(1) - (1)])); ;}
    break;

  case 1613:

/* Line 1455 of yacc.c  */
#line 8294 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1614:

/* Line 1455 of yacc.c  */
#line 8295 "parser.y"
    { (yyval) = (yyvsp[(1) - (2)]); ;}
    break;

  case 1615:

/* Line 1455 of yacc.c  */
#line 8296 "parser.y"
    { (yyval) = (yyvsp[(1) - (2)]); ;}
    break;

  case 1616:

/* Line 1455 of yacc.c  */
#line 8297 "parser.y"
    { (yyval) = (yyvsp[(1) - (3)]); ;}
    break;

  case 1617:

/* Line 1455 of yacc.c  */
#line 8301 "parser.y"
    { (yyval) = cb_build_label_reference((yyvsp[(1) - (1)])); ;}
    break;

  case 1618:

/* Line 1455 of yacc.c  */
#line 8302 "parser.y"
    { (yyval) = cb_build_label_reference((yyvsp[(1) - (3)])); CB_REFERENCE ((yyval))->chain = (yyvsp[(3) - (3)]); ;}
    break;

  case 1619:

/* Line 1455 of yacc.c  */
#line 8306 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1620:

/* Line 1455 of yacc.c  */
#line 8307 "parser.y"
    { (yyval) = (yyvsp[(1) - (3)]); CB_REFERENCE ((yyvsp[(1) - (3)]))->chain = (yyvsp[(3) - (3)]); ;}
    break;

  case 1621:

/* Line 1455 of yacc.c  */
#line 8308 "parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); CB_REFERENCE ((yyvsp[(2) - (2)]))->chain = (yyvsp[(1) - (2)]); ;}
    break;

  case 1622:

/* Line 1455 of yacc.c  */
#line 8313 "parser.y"
    {
        (yyval) = (yyvsp[(0) - (3)]);
        if ((yyvsp[(2) - (3)]))
            CB_REFERENCE ((yyvsp[(0) - (3)]))->subs = cb_list_reverse ((yyvsp[(2) - (3)]));
  ;}
    break;

  case 1623:

/* Line 1455 of yacc.c  */
#line 8322 "parser.y"
    {
        CB_REFERENCE ((yyvsp[(0) - (4)]))->offset = (yyvsp[(2) - (4)]);
  ;}
    break;

  case 1624:

/* Line 1455 of yacc.c  */
#line 8326 "parser.y"
    {
        CB_REFERENCE ((yyvsp[(0) - (5)]))->offset = (yyvsp[(2) - (5)]);
        CB_REFERENCE ((yyvsp[(0) - (5)]))->length = (yyvsp[(4) - (5)]);
  ;}
    break;

  case 1625:

/* Line 1455 of yacc.c  */
#line 8338 "parser.y"
    {
    if (cb_tree_category ((yyvsp[(1) - (1)])) != CB_CATEGORY_NUMERIC) {
        cb_error (_("Integer value expected"));
    } else if (CB_LITERAL ((yyvsp[(1) - (1)]))->sign < 0 || CB_LITERAL ((yyvsp[(1) - (1)]))->scale) {
        cb_error (_("Integer value expected"));
    }
    (yyval) = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1626:

/* Line 1455 of yacc.c  */
#line 8350 "parser.y"
    {
    if (cb_tree_category ((yyvsp[(1) - (1)])) != CB_CATEGORY_NUMERIC) {
        cb_error (_("Integer value expected"));
    } else if (CB_LITERAL ((yyvsp[(1) - (1)]))->scale) {
        cb_error (_("Integer value expected"));
    }
    (yyval) = (yyvsp[(1) - (1)]);
  ;}
    break;

  case 1627:

/* Line 1455 of yacc.c  */
#line 8360 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1628:

/* Line 1455 of yacc.c  */
#line 8362 "parser.y"
    {
        (yyval) = (yyvsp[(2) - (2)]);
        if (CB_LITERAL_P ((yyvsp[(2) - (2)]))) {
                CB_LITERAL ((yyvsp[(2) - (2)]))->all = 1;
        }
  ;}
    break;

  case 1629:

/* Line 1455 of yacc.c  */
#line 8371 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1630:

/* Line 1455 of yacc.c  */
#line 8372 "parser.y"
    { (yyval) = cb_concat_literals ((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 1631:

/* Line 1455 of yacc.c  */
#line 8377 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1632:

/* Line 1455 of yacc.c  */
#line 8378 "parser.y"
    { (yyval) = cb_space; ;}
    break;

  case 1633:

/* Line 1455 of yacc.c  */
#line 8379 "parser.y"
    { (yyval) = cb_zero; ;}
    break;

  case 1634:

/* Line 1455 of yacc.c  */
#line 8380 "parser.y"
    { (yyval) = cb_quote; ;}
    break;

  case 1635:

/* Line 1455 of yacc.c  */
#line 8381 "parser.y"
    { (yyval) = cb_dirsep; ;}
    break;

  case 1636:

/* Line 1455 of yacc.c  */
#line 8382 "parser.y"
    { (yyval) = cb_high; ;}
    break;

  case 1637:

/* Line 1455 of yacc.c  */
#line 8383 "parser.y"
    { (yyval) = cb_low; ;}
    break;

  case 1638:

/* Line 1455 of yacc.c  */
#line 8384 "parser.y"
    { (yyval) = cb_null; ;}
    break;

  case 1639:

/* Line 1455 of yacc.c  */
#line 8393 "parser.y"
    {
        (yyval) = cb_build_intrinsic ((yyvsp[(1) - (2)]), NULL, (yyvsp[(2) - (2)]));
  ;}
    break;

  case 1640:

/* Line 1455 of yacc.c  */
#line 8397 "parser.y"
    {
        (yyval) = cb_build_intrinsic ((yyvsp[(1) - (2)]), NULL, (yyvsp[(2) - (2)]));
  ;}
    break;

  case 1641:

/* Line 1455 of yacc.c  */
#line 8401 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), cb_list_init ((yyvsp[(3) - (5)])), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1642:

/* Line 1455 of yacc.c  */
#line 8405 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), cb_list_init ((yyvsp[(3) - (5)])), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1643:

/* Line 1455 of yacc.c  */
#line 8409 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), cb_list_init ((yyvsp[(3) - (5)])), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1644:

/* Line 1455 of yacc.c  */
#line 8413 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1645:

/* Line 1455 of yacc.c  */
#line 8417 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1646:

/* Line 1455 of yacc.c  */
#line 8421 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1647:

/* Line 1455 of yacc.c  */
#line 8425 "parser.y"
    {
        (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1648:

/* Line 1455 of yacc.c  */
#line 8429 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), cb_list_init ((yyvsp[(3) - (5)])), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1649:

/* Line 1455 of yacc.c  */
#line 8433 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), cb_list_init ((yyvsp[(3) - (5)])), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1650:

/* Line 1455 of yacc.c  */
#line 8437 "parser.y"
    {
        (yyval) = cb_build_intrinsic ((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), NULL);
  ;}
    break;

  case 1651:

/* Line 1455 of yacc.c  */
#line 8441 "parser.y"
    {
        (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1652:

/* Line 1455 of yacc.c  */
#line 8445 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1653:

/* Line 1455 of yacc.c  */
#line 8449 "parser.y"
    {
    (yyval) = cb_build_intrinsic ((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]));
  ;}
    break;

  case 1654:

/* Line 1455 of yacc.c  */
#line 8453 "parser.y"
    {
        (yyval) = cb_build_intrinsic ((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), NULL);
  ;}
    break;

  case 1655:

/* Line 1455 of yacc.c  */
#line 8459 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1656:

/* Line 1455 of yacc.c  */
#line 8460 "parser.y"
    { (yyval) = cb_build_pair ((yyvsp[(2) - (4)]), NULL); ;}
    break;

  case 1657:

/* Line 1455 of yacc.c  */
#line 8461 "parser.y"
    { (yyval) = cb_build_pair ((yyvsp[(2) - (5)]), (yyvsp[(4) - (5)])); ;}
    break;

  case 1658:

/* Line 1455 of yacc.c  */
#line 8465 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1659:

/* Line 1455 of yacc.c  */
#line 8466 "parser.y"
    { (yyval) = (yyvsp[(2) - (3)]);   ;}
    break;

  case 1660:

/* Line 1455 of yacc.c  */
#line 8470 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1661:

/* Line 1455 of yacc.c  */
#line 8471 "parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 1662:

/* Line 1455 of yacc.c  */
#line 8477 "parser.y"
    {
        cb_tree x;

        x = cb_list_init ((yyvsp[(1) - (1)]));
        (yyval) = cb_list_add (x, cb_int0);
  ;}
    break;

  case 1663:

/* Line 1455 of yacc.c  */
#line 8484 "parser.y"
    {
        cb_tree x;

        x = cb_list_init ((yyvsp[(1) - (3)]));
        (yyval) = cb_list_add (x, cb_int1);
  ;}
    break;

  case 1664:

/* Line 1455 of yacc.c  */
#line 8491 "parser.y"
    {
        cb_tree x;

        x = cb_list_init ((yyvsp[(1) - (3)]));
        (yyval) = cb_list_add (x, cb_int2);
  ;}
    break;

  case 1665:

/* Line 1455 of yacc.c  */
#line 8501 "parser.y"
    {
        cb_tree x;

    x = cb_list_init ((yyvsp[(1) - (1)]));
        (yyval) = cb_list_add (x, cb_null);
  ;}
    break;

  case 1666:

/* Line 1455 of yacc.c  */
#line 8508 "parser.y"
    {
        cb_tree x;

    x = cb_list_init ((yyvsp[(1) - (3)]));
        (yyval) = cb_list_add (x, (yyvsp[(3) - (3)]));
  ;}
    break;

  case 1667:

/* Line 1455 of yacc.c  */
#line 8518 "parser.y"
    {
        cb_tree x;

    x = cb_list_init ((yyvsp[(1) - (1)]));
        (yyval) = cb_list_add (x, cb_null);
  ;}
    break;

  case 1668:

/* Line 1455 of yacc.c  */
#line 8525 "parser.y"
    {
        cb_tree x;

    x = cb_list_init ((yyvsp[(1) - (3)]));
        (yyval) = cb_list_add (x, cb_ref((yyvsp[(3) - (3)])));
  ;}
    break;

  case 1669:

/* Line 1455 of yacc.c  */
#line 8538 "parser.y"
    {
    non_const_word = 1;
  ;}
    break;

  case 1670:

/* Line 1455 of yacc.c  */
#line 8548 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1671:

/* Line 1455 of yacc.c  */
#line 8549 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1672:

/* Line 1455 of yacc.c  */
#line 8553 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1673:

/* Line 1455 of yacc.c  */
#line 8554 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1674:

/* Line 1455 of yacc.c  */
#line 8558 "parser.y"
    { (yyval) = NULL; ;}
    break;

  case 1675:

/* Line 1455 of yacc.c  */
#line 8559 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1676:

/* Line 1455 of yacc.c  */
#line 8563 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1677:

/* Line 1455 of yacc.c  */
#line 8564 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1678:

/* Line 1455 of yacc.c  */
#line 8565 "parser.y"
    { (yyval) = cb_int2; ;}
    break;

  case 1679:

/* Line 1455 of yacc.c  */
#line 8569 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1680:

/* Line 1455 of yacc.c  */
#line 8570 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1681:

/* Line 1455 of yacc.c  */
#line 8574 "parser.y"
    { if (cb_flag_optional_file) (yyval) = cb_int(COB_FILE_OPTIONAL); else (yyval) = cb_int0; ;}
    break;

  case 1682:

/* Line 1455 of yacc.c  */
#line 8575 "parser.y"
    { (yyval) = cb_int(COB_FILE_OPTIONAL_IN_SOURCE); ;}
    break;

  case 1683:

/* Line 1455 of yacc.c  */
#line 8576 "parser.y"
    { (yyval) = cb_int(COB_FILE_OPTIONAL_IN_SOURCE | COB_FILE_OPTIONAL); ;}
    break;

  case 1684:

/* Line 1455 of yacc.c  */
#line 8580 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1685:

/* Line 1455 of yacc.c  */
#line 8581 "parser.y"
    { (yyval) = cb_int1; rounded_used = 1;;}
    break;

  case 1686:

/* Line 1455 of yacc.c  */
#line 8585 "parser.y"
    { (yyval) = cb_int0; ;}
    break;

  case 1687:

/* Line 1455 of yacc.c  */
#line 8586 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1715:

/* Line 1455 of yacc.c  */
#line 8606 "parser.y"
    { (yyval) = cb_int1; ;}
    break;

  case 1735:

/* Line 1455 of yacc.c  */
#line 8616 "parser.y"
    { (yyval) = cb_int1; ;}
    break;



/* Line 1455 of yacc.c  */
#line 17351 "parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 8649 "parser.y"



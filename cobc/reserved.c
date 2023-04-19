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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libcob.h"
#include "cobc.h"
#include "tree.h"
#include "parser.h"

static struct {
    const char                              *name;
    const enum cb_system_name_category      category;
    const int                               token;
    cb_tree                                 node;
} system_table[] = {
    {"SYSIN",             CB_DEVICE_NAME,  COB_DEVICE_SYSIN, NULL},
    {"SYSIPT",            CB_DEVICE_NAME,  COB_DEVICE_SYSIN, NULL},
    {"SYSOUT",            CB_DEVICE_NAME,  COB_DEVICE_SYSOUT, NULL},
    {"SYSLIST",           CB_DEVICE_NAME,  COB_DEVICE_SYSOUT, NULL},
    {"SYSLST",            CB_DEVICE_NAME,  COB_DEVICE_SYSOUT, NULL},
    {"PRINTER",           CB_DEVICE_NAME,  COB_DEVICE_SYSOUT, NULL},
    {"SYSERR",            CB_DEVICE_NAME,  COB_DEVICE_SYSERR, NULL},
    {"CONSOLE",           CB_DEVICE_NAME,  COB_DEVICE_CONSOLE, NULL},
    {"TERMINAL",          CB_DEVICE_NAME,  COB_DEVICE_CONSOLE, NULL},
    {"C01",               CB_FEATURE_NAME, CB_FEATURE_C01, NULL},
    {"C02",               CB_FEATURE_NAME, CB_FEATURE_C02, NULL},
    {"C03",               CB_FEATURE_NAME, CB_FEATURE_C03, NULL},
    {"C04",               CB_FEATURE_NAME, CB_FEATURE_C04, NULL},
    {"C05",               CB_FEATURE_NAME, CB_FEATURE_C05, NULL},
    {"C06",               CB_FEATURE_NAME, CB_FEATURE_C06, NULL},
    {"C07",               CB_FEATURE_NAME, CB_FEATURE_C07, NULL},
    {"C08",               CB_FEATURE_NAME, CB_FEATURE_C08, NULL},
    {"C09",               CB_FEATURE_NAME, CB_FEATURE_C09, NULL},
    {"C10",               CB_FEATURE_NAME, CB_FEATURE_C10, NULL},
    {"C11",               CB_FEATURE_NAME, CB_FEATURE_C11, NULL},
    {"C12",               CB_FEATURE_NAME, CB_FEATURE_C12, NULL},
    {"CSP",               CB_FEATURE_NAME, CB_FEATURE_CSP, NULL},
    {"FORMFEED",          CB_FEATURE_NAME, CB_FEATURE_FORMFEED, NULL},
    {"S01",               CB_FEATURE_NAME, CB_FEATURE_C01, NULL},
    {"S02",               CB_FEATURE_NAME, CB_FEATURE_C02, NULL},
    {"S03",               CB_FEATURE_NAME, CB_FEATURE_C03, NULL},
    {"S04",               CB_FEATURE_NAME, CB_FEATURE_C04, NULL},
    {"S05",               CB_FEATURE_NAME, CB_FEATURE_C05, NULL},

    {"SW0",               CB_SWITCH_NAME,  CB_SWITCH_1 , NULL},
    {"SW1",               CB_SWITCH_NAME,  CB_SWITCH_2 , NULL},
    {"SW2",               CB_SWITCH_NAME,  CB_SWITCH_3 , NULL},
    {"SW3",               CB_SWITCH_NAME,  CB_SWITCH_4 , NULL},
    {"SW4",               CB_SWITCH_NAME,  CB_SWITCH_5 , NULL},
    {"SW5",               CB_SWITCH_NAME,  CB_SWITCH_6 , NULL},
    {"SW6",               CB_SWITCH_NAME,  CB_SWITCH_7 , NULL},
    {"SW7",               CB_SWITCH_NAME,  CB_SWITCH_8 , NULL},
    {"SW8",               CB_SWITCH_NAME,  CB_SWITCH_9 , NULL},
    {"SW9",               CB_SWITCH_NAME,  CB_SWITCH_10, NULL},
    {"SW10",              CB_SWITCH_NAME,  CB_SWITCH_11, NULL},
    {"SW11",              CB_SWITCH_NAME,  CB_SWITCH_12, NULL},
    {"SW12",              CB_SWITCH_NAME,  CB_SWITCH_13, NULL},
    {"SW13",              CB_SWITCH_NAME,  CB_SWITCH_14, NULL},
    {"SW14",              CB_SWITCH_NAME,  CB_SWITCH_15, NULL},
    {"SW15",              CB_SWITCH_NAME,  CB_SWITCH_16, NULL},

    {"SWITCH-0",          CB_SWITCH_NAME,  CB_SWITCH_1, NULL},
    {"SWITCH-1",          CB_SWITCH_NAME,  CB_SWITCH_2, NULL},
    {"SWITCH-2",          CB_SWITCH_NAME,  CB_SWITCH_3, NULL},
    {"SWITCH-3",          CB_SWITCH_NAME,  CB_SWITCH_4, NULL},
    {"SWITCH-4",          CB_SWITCH_NAME,  CB_SWITCH_5, NULL},
    {"SWITCH-5",          CB_SWITCH_NAME,  CB_SWITCH_6, NULL},
    {"SWITCH-6",          CB_SWITCH_NAME,  CB_SWITCH_7, NULL},
    {"SWITCH-7",          CB_SWITCH_NAME,  CB_SWITCH_8, NULL},
    {"SWITCH-8",          CB_SWITCH_NAME,  CB_SWITCH_9, NULL},
    {"SWITCH-9",          CB_SWITCH_NAME,  CB_SWITCH_10, NULL},
    {"SWITCH-10",         CB_SWITCH_NAME,  CB_SWITCH_11, NULL},
    {"SWITCH-11",         CB_SWITCH_NAME,  CB_SWITCH_12, NULL},
    {"SWITCH-12",         CB_SWITCH_NAME,  CB_SWITCH_13, NULL},
    {"SWITCH-13",         CB_SWITCH_NAME,  CB_SWITCH_14, NULL},
    {"SWITCH-14",         CB_SWITCH_NAME,  CB_SWITCH_15, NULL},
    {"SWITCH-15",         CB_SWITCH_NAME,  CB_SWITCH_16, NULL},
    {"SWITCH-16",         CB_SWITCH_NAME,  CB_SWITCH_17, NULL},
    {"SWITCH-17",         CB_SWITCH_NAME,  CB_SWITCH_18, NULL},
    {"SWITCH-18",         CB_SWITCH_NAME,  CB_SWITCH_19, NULL},
    {"SWITCH-19",         CB_SWITCH_NAME,  CB_SWITCH_20, NULL},
    {"SWITCH-20",         CB_SWITCH_NAME,  CB_SWITCH_21, NULL},
    {"SWITCH-21",         CB_SWITCH_NAME,  CB_SWITCH_22, NULL},
    {"SWITCH-22",         CB_SWITCH_NAME,  CB_SWITCH_23, NULL},
    {"SWITCH-23",         CB_SWITCH_NAME,  CB_SWITCH_24, NULL},
    {"SWITCH-24",         CB_SWITCH_NAME,  CB_SWITCH_25, NULL},
    {"SWITCH-25",         CB_SWITCH_NAME,  CB_SWITCH_26, NULL},
    {"SWITCH-26",         CB_SWITCH_NAME,  CB_SWITCH_27, NULL},
    {"SWITCH-27",         CB_SWITCH_NAME,  CB_SWITCH_28, NULL},
    {"SWITCH-28",         CB_SWITCH_NAME,  CB_SWITCH_29, NULL},
    {"SWITCH-29",         CB_SWITCH_NAME,  CB_SWITCH_30, NULL},
    {"SWITCH-30",         CB_SWITCH_NAME,  CB_SWITCH_31, NULL},
    {"SWITCH-31",         CB_SWITCH_NAME,  CB_SWITCH_32, NULL},
    {"SWITCH-32",         CB_SWITCH_NAME,  CB_SWITCH_33, NULL},
    {"SWITCH-33",         CB_SWITCH_NAME,  CB_SWITCH_34, NULL},
    {"SWITCH-34",         CB_SWITCH_NAME,  CB_SWITCH_35, NULL},
    {"SWITCH-35",         CB_SWITCH_NAME,  CB_SWITCH_36, NULL},
    /*{"SWITCH-36",         CB_SWITCH_NAME,  CB_SWITCH_37, NULL},*/
    {"CALL-CONVENTION",   CB_CALL_CONVENTION_NAME, 0, NULL},
    {NULL, 0, 0, NULL}
};

struct reserved {
    const char      *name;
    const int       token;
};

static int reserved_sorted = 0;
static struct reserved reserved_words[] = {
    {"ABEND",             ABEND},                 /* CIT */
    {"ACCEPT",            ACCEPT},                /* 2002 */
    {"ACCESS",            ACCESS},                /* 2002 */
    {"ACTIVE-CLASS",      -1},                    /* 2002 */
    {"ADD",               ADD},                   /* 2002 */
    {"ADDRESS",           ADDRESS},               /* 2002 */
    {"ADVANCING",         ADVANCING},             /* 2002 */
    {"AFTER",             AFTER},                 /* 2002 */
    {"ALIGNED",           -1},                    /* 2002 */
    {"ALL",               ALL},                   /* 2002 */
    {"ALLOCATE",          ALLOCATE},              /* 2002 */
    {"ALPHABET",          ALPHABET},              /* 2002 */
    {"ALPHABETIC",        ALPHABETIC},            /* 2002 */
    {"ALPHABETIC-LOWER",  ALPHABETIC_LOWER},      /* 2002 */
    {"ALPHABETIC-UPPER",  ALPHABETIC_UPPER},      /* 2002 */
    {"ALPHANUMERIC",      ALPHANUMERIC},          /* 2002 */
    {"ALPHANUMERIC-EDITED", ALPHANUMERIC_EDITED}, /* 2002 */
    {"ALSO",              ALSO},                  /* 2002 */
    {"ALTER",             ALTER},                 /* 85 */
    {"ALTERNATE",         ALTERNATE},             /* 2002 */
    {"AND",               AND},                   /* 2002 */
    {"ANY",               ANY},                   /* 2002 */
    {"ANYCASE",           -1},                    /* 2002 */
    {"APPLY",             APPLY},                 /* OBSOLET */
    {"ARE",               ARE},                   /* 2002 */
    {"AREA",              AREA},                  /* 2002 */
    {"AREAS",             AREA},                  /* 2002 */
    {"ARGUMENT-NUMBER",   ARGUMENT_NUMBER},       /* extension */
    {"ARGUMENT-VALUE",    ARGUMENT_VALUE},        /* extension */
    {"ARITHMETIC",        -1},                    /* 2002 (C/S) */
    {"AS",                AS},                    /* 2002 */
    {"ASCENDING",         ASCENDING},             /* 2002 */
    {"ASSIGN",            ASSIGN},                /* 2002 */
    {"AT",                AT},                    /* 2002 */
    {"ATTRIBUTE",         -1},                    /* 2002 (C/S) */
    {"AUTO",              AUTO},                  /* 2002 (C/S) */
    {"AUTO-SKIP",         AUTO},                  /* extension */
    {"AUTOMATIC",         AUTOMATIC},             /* extension */
    {"AUTOTERMINATE",     AUTO},                  /* extension */
    {"B-AND",             B_AND},                 /* 2002 */
    {"B-EXOR",            B_XOR},                 /* 2002 */
    {"B-NOT",             B_NOT},                 /* 2002 */
    {"B-OR",              B_OR},                  /* 2002 */
    {"B-XOR",             B_XOR},                 /* 2002 */
    {"BACKGROUND-COLOR",  BACKGROUND_COLOR},      /* 2002 (C/S) */
    {"BACKGROUND-COLOUR", BACKGROUND_COLOR},      /* MF   */
    {"BASED",             BASED},                 /* 2002 */
    {"BEEP",              BELL},                  /* extension */
    {"BEFORE",            BEFORE},                /* 2002 */
    {"BELL",              BELL},                  /* 2002 (C/S) */
    {"BINARY",            BINARY},                /* 2002 */
    {"BINARY-C-LONG",     BINARY_C_LONG},         /* Extension */
    {"BINARY-CHAR",       BINARY_CHAR},           /* 2002 */
    {"BINARY-DOUBLE",     BINARY_DOUBLE},         /* 2002 */
    {"BINARY-LONG",       BINARY_LONG},           /* 2002 */
    {"BINARY-SHORT",      BINARY_SHORT},          /* 2002 */
    {"BIT",               BIT},                   /* 2002 */
    {"BLANK",             BLANK},                 /* 2002 */
    {"BLINK",             BLINK},                 /* 2002 (C/S) */
    {"BLOCK",             BLOCK},                 /* 2002 */
    {"BOOLEAN",           -1},                    /* 2002 */
    {"BOTTOM",            BOTTOM},                /* 2002 */
    {"BY",                BY},                    /* 2002 */
    {"BYTE-LENGTH",       BYTE_LENGTH},           /* 2002 (C/S) */
    {"CALL",              CALL},                  /* 2002 */
    {"CANCEL",            CANCEL},                /* 2002 */
    {"CD",                -1},                    /* 2002 */
    {"CDECL",             CDECL},                 /* extension */
    {"CENTER",            -1},                    /* 2002 (C/S) */
    {"CENTURY-DATE",      CENTURY_DATE},          /* RM/COBOL */
    {"CENTURY-DAY",       CENTURY_DAY},           /* RM/COBOL */
    {"CF",                -1},                    /* 2002 */
    {"CH",                -1},                    /* 2002 */
    {"CHAIN",             CHAIN},                 /* extension */
    {"CHAINING",          CHAINING},              /* extension */
    {"CHANGED",           CHANGED},               /* MF   */
    {"CHARACTER",         CHARACTER},             /* 2002 */
    {"CHARACTERS",        CHARACTERS},            /* 2002 */
    {"CHECKPOINT",        CHECKPOINT},            /* CIT */
    {"CLASS",             CLASS},                 /* 2002 */
    {"CLASS-ID",          -1},                    /* 2002 */
    {"CLASSIFICATION",    -1},                    /* 2002 (C/S) */
    {"CLOSE",             CLOSE},                 /* 2002 */
    {"CODE",              CODE},                  /* 2002 */
    {"CODE-SET",          CODE_SET},              /* 2002 */
    {"COL",               COLUMN},                /* 2002 */
    {"COLLATING",         COLLATING},             /* 2002 */
    {"COLS",              COLUMNS},               /* 2002 */
    {"COLUMN",            COLUMN},                /* 2002 */
    {"COLUMNS",           COLUMNS},               /* 2002 */
    {"COMMA",             COMMA},                 /* 2002 */
    {"COMMAND-LINE",      COMMAND_LINE},          /* extension */
    {"COMMIT",            COMMIT},                /* extension */
    {"COMMON",            COMMON},                /* 2002 */
    {"COMMUNICATION",     -1},                    /* 2002 */
    {"COMP",              COMP},                  /* 2002 */
#ifdef  __MVS__ /* EBCDIC! */
    {"COMP-X",            COMP_X},                /* extension */
#endif
    {"COMP-1",            COMP_1},                /* extension */
    {"COMP-2",            COMP_2},                /* extension */
    {"COMP-3",            COMP_3},                /* extension */
    {"COMP-4",            COMP_4},                /* extension */
    {"COMP-5",            COMP_5},                /* extension */
    {"COMP-6",            COMP_6},                /* extension */
#ifndef __MVS__
    {"COMP-X",            COMP_X},                /* extension */
#endif
    {"COMPUTATIONAL",     COMP},                  /* 2002 */
#ifdef  __MVS__ /* EBCDIC! */
    {"COMPUTATIONAL-X",   COMP_X},                /* extension */
#endif
    {"COMPUTATIONAL-1",   COMP_1},                /* extension */
    {"COMPUTATIONAL-2",   COMP_2},                /* extension */
    {"COMPUTATIONAL-3",   COMP_3},                /* extension */
    {"COMPUTATIONAL-4",   COMP_4},                /* extension */
    {"COMPUTATIONAL-5",   COMP_5},                /* extension */
#ifndef __MVS__
    {"COMPUTATIONAL-X",   COMP_X},                /* extension */
#endif
    {"COMPUTE",           COMPUTE},               /* 2002 */
    {"CONDITION",         -1},                    /* 2002 */
    {"CONFIGURATION",     CONFIGURATION},         /* 2002 */
    {"CONSTANT",          CONSTANT},              /* 2002 */
    {"CONTAINS",          CONTAINS},              /* 2002 */
    {"CONTENT",           CONTENT},               /* 2002 */
    {"CONTINUE",          CONTINUE},              /* 2002 */
    {"CONTROL",           CONTROL},               /* 2002 */
    {"CONTROLS",          CONTROL},               /* 2002 */
    {"CONVERT",           CONVERT},               /* RM/COBOL */
    {"CONVERTING",        CONVERTING},            /* 2002 */
    {"COPY",              0},                     /* 2002 */
    {"CORE-INDEX",        CORE_INDEX},            /* OBSOLET */
    {"CORR",              CORRESPONDING},         /* 2002 */
    {"CORRESPONDING",     CORRESPONDING},         /* 2002 */
    {"COUNT",             COUNT},                 /* 2002 */
    {"CRT",               CRT},                   /* 2002 */
    {"CURRENCY",          CURRENCY},              /* 2002 */
    {"CURSOR",            CURSOR},                /* 2002 */
    {"CYCLE",             CYCLE},                 /* 2002 (C/S) */
    {"DATA",              DATA},                  /* 2002 */
    {"DATA-POINTER",      -1},                    /* 2002 */
    {"DATE",              DATE},                  /* 2002 */
    {"DAY",               DAY},                   /* 2002 */
    {"DAY-OF-WEEK",       DAY_OF_WEEK},           /* 2002 */
    {"DE",                DE},                    /* 2002 */
    {"DEBUGGING",         DEBUGGING},             /* 2002 */
    {"DECIMAL-POINT",     DECIMAL_POINT},         /* 2002 */
    {"DECLARATIVES",      DECLARATIVES},          /* 2002 */
    {"DEFAULT",           DEFAULT},               /* 2002 */
    {"DELETE",            DELETE},                /* 2002 */
    {"DELIMITED",         DELIMITED},             /* 2002 */
    {"DELIMITER",         DELIMITER},             /* 2002 */
    {"DEPENDING",         DEPENDING},             /* 2002 */
    {"DESCENDING",        DESCENDING},            /* 2002 */
    {"DESCRIPTOR",        DESCRIPTOR},            /* */
    {"DESTINATION",       -1},                    /* 2002 */
    {"DETAIL",            DETAIL},                /* 2002 */
    {"DISABLE",           -1},                    /* 2002 */
    {"DIR-SEPARATOR",     DIR_SEPARATOR},         /* extension */
    {"DISK",              DISK},                  /* extension */
    {"DISPLAY",           DISPLAY},               /* 2002 */
    {"DISPLAY-1",         DISPLAY_1},             /*  MF  */
    {"DIVIDE",            DIVIDE},                /* 2002 */
    {"DIVISION",          DIVISION},              /* 2002 */
    {"DOWN",              DOWN},                  /* 2002 */
    {"DUPLICATES",        DUPLICATES},            /* 2002 */
    {"DYNAMIC",           DYNAMIC},               /* 2002 */
    {"EBCDIC",            EBCDIC},                /* extension */
    {"EC",                -1},                    /* 2002 */
    {"ECHO",              ECHO},                  /* RM/COBOL*/
    {"EGI",               -1},                    /* 2002 */
    {"ELSE",              ELSE},                  /* 2002 */
    {"EMI",               -1},                    /* 2002 */
    {"EMPTY-CHECK",       REQUIRED},              /* MF   */
    {"ENABLE",            -1},                    /* 2002 */
    {"ENCODING",          ENCODING},              /* IBM  */
    {"END",               END},                   /* 2002 */
    {"END-ACCEPT",        END_ACCEPT},            /* 2002 */
    {"END-ADD",           END_ADD},               /* 2002 */
    {"END-CALL",          END_CALL},              /* 2002 */
    {"END-COMPUTE",       END_COMPUTE},           /* 2002 */
    {"END-DELETE",        END_DELETE},            /* 2002 */
    {"END-DISPLAY",       END_DISPLAY},           /* 2002 */
    {"END-DIVIDE",        END_DIVIDE},            /* 2002 */
    {"END-EVALUATE",      END_EVALUATE},          /* 2002 */
    {"END-EXHIBIT",       END_EXHIBIT},           /* MF   */
    {"END-IF",            END_IF},                /* 2002 */
    {"END-MULTIPLY",      END_MULTIPLY},          /* 2002 */
    {"END-OF-PAGE",       EOP},                   /* 2002 */
    {"END-PERFORM",       END_PERFORM},           /* 2002 */
    {"END-READ",          END_READ},              /* 2002 */
    {"END-RECEIVE",       -1},                    /* 2002 */
    {"END-RETURN",        END_RETURN},            /* 2002 */
    {"END-REWRITE",       END_REWRITE},           /* 2002 */
    {"END-SEARCH",        END_SEARCH},            /* 2002 */
    {"END-START",         END_START},             /* 2002 */
    {"END-STRING",        END_STRING},            /* 2002 */
    {"END-SUBTRACT",      END_SUBTRACT},          /* 2002 */
    {"END-UNSTRING",      END_UNSTRING},          /* 2002 */
    {"END-WRITE",         END_WRITE},             /* 2002 */
    {"END-XML",           END_XML},               /* externtion */
    {"ENTRY",             ENTRY},                 /* extension */
    {"ENTRY-CONVENTION",  -1},                    /* 2002 (C/S) */
    {"ENVIRONMENT",       ENVIRONMENT},           /* 2002 */
    {"ENVIRONMENT-NAME",  ENVIRONMENT_NAME},      /* extension */
    {"ENVIRONMENT-VALUE", ENVIRONMENT_VALUE},     /* extension */
    {"EO",                -1},                    /* 2002 */
    {"EOL",               EOL},                   /* 2002 (C/S) */
    {"EOP",               EOP},                   /* 2002 */
    {"EOS",               EOS},                   /* 2002 (C/S) */
    {"EQUAL",             EQUAL},                 /* 2002 */
    {"EQUALS",            EQUALS},                /* extension */
    {"ERASE",             ERASE},                 /* 2002 (C/S) */
    {"ERROR",             ERROR},                 /* 2002 */
    {"ESCAPE",            ESCAPE},                /* extension */
    {"ESI",               -1},                    /* 2002 */
    {"EVALUATE",          EVALUATE},              /* 2002 */
    {"EXCEPTION",         EXCEPTION},             /* 2002 */
    {"EXCEPTION-OBJECT",  -1},                    /* 2002 */
    {"EXCLUSIVE",         EXCLUSIVE},             /* extension */
    {"EXHIBIT",           EXHIBIT},               /* MF   */
    {"EXIT",              EXIT},                  /* 2002 */
    {"EXPANDS",           -1},                    /* 2002 (C/S) */
    {"EXTEND",            EXTEND},                /* 2002 */
    {"EXTERNAL",          EXTERNAL},              /* 2002 */
    {"FACTORY",           -1},                    /* 2002 */
    {"FAILURE",           FAILURE},               /* MVS */
    {"FALSE",             TOK_FALSE},             /* 2002 */
    {"FD",                FD},                    /* 2002 */
    {"FILE",              TOK_FILE},              /* 2002 */
    {"FILE-CONTROL",      FILE_CONTROL},          /* 2002 */
    {"FILE-ID",           FILE_ID},               /* extension */
    {"FILLER",            FILLER},                /* 2002 */
    {"FINAL",             FINAL},                 /* 2002 */
    {"FIRST",             FIRST},                 /* 2002 */
    {"FLOAT-EXTENDED",    -1},                    /* 2002 */
    {"FLOAT-LONG",        COMP_2},                /* 2002 */
    {"FLOAT-SHORT",       COMP_1},                /* 2002 */
    {"FOOTING",           FOOTING},               /* 2002 */
    {"FOR",               FOR},                   /* 2002 */
    {"FOREGROUND-COLOR",  FOREGROUND_COLOR},      /* 2002 (C/S) */
    {"FOREGROUND-COLOUR", FOREGROUND_COLOR},      /* MF   */
    {"FOREVER",           FOREVER},               /* 2002 (C/S) */
    {"FORMAT",            -1},                    /* 2002 */
    {"FREE",              FREE},                  /* 2002 */
    {"FROM",              FROM},                  /* 2002 */
    {"FULL",              FULL},                  /* 2002 (C/S) */
    {"FUNCTION",          FUNCTION},              /* 2002 */
    {"FUNCTION-ID",       FUNCTION_ID},           /* 2002 */
    {"FUNCTION-POINTER",  -1},                    /* 2008 */
    {"GENERATE",          GENERATE},              /* 2002 */
    {"GET",               -1},                    /* 2002 */
    {"GIVING",            GIVING},                /* 2002 */
    {"GLOBAL",            GLOBAL},                /* 2002 */
    {"GO",                GO},                    /* 2002 */
    {"GOBACK",            GOBACK},                /* 2002 */
    {"GREATER",           GREATER},               /* 2002 */
    {"GROUP",             GROUP},                 /* 2002 */
    {"GROUP-USAGE",       USAGE},                 /* 2002 */
    {"HEADING",           HEADING},               /* 2002 */
    {"HIGH",              HIGHLIGHT},             /* RM/COBOL */
    {"HIGH-VALUE",        HIGH_VALUE},            /* 2002 */
    {"HIGH-VALUES",       HIGH_VALUE},            /* 2002 */
    {"HIGHLIGHT",         HIGHLIGHT},             /* 2002 (C/S) */
    {"I-O",               I_O},                   /* 2002 */
    {"I-O-CONTROL",       I_O_CONTROL},           /* 2002 */
    {"ID",                IDENTIFICATION},        /* extension */
    {"IDENTIFICATION",    IDENTIFICATION},        /* 2002 */
    {"IF",                IF},                    /* 2002 */
    {"IGNORE",            IGNORE},                /* extension */
    {"IGNORING",          IGNORING},              /* 2002 (C/S) */
    {"IMPLEMENTS",        -1},                    /* 2002 (C/S) */
    {"IN",                IN},                    /* 2002 */
    {"INDEX",             INDEX},                 /* 2002 */
    {"INDEXED",           INDEXED},               /* 2002 */
    {"INDICATE",          INDICATE},              /* 2002 */
    {"INFINITY",          -1},                    /* 2002 */
    {"INHERITS",          -1},                    /* 2002 */
    {"INITIAL",           TOK_INITIAL},           /* 2002 */
    {"INITIALISE",        INITIALIZE},            /* 2002 */
    {"INITIALISED",       INITIALIZED},           /* 2002 */
    {"INITIALIZE",        INITIALIZE},            /* 2002 */
    {"INITIALIZED",       INITIALIZED},           /* 2002 */
    {"INITIATE",          INITIATE},              /* 2002 */
    {"INPUT",             INPUT},                 /* 2002 */
    {"INPUT-OUTPUT",      INPUT_OUTPUT},          /* 2002 */
    {"INSPECT",           INSPECT},               /* 2002 */
    {"INTERFACE",         -1},                    /* 2002 */
    {"INTERFACE-ID",      -1},                    /* 2002 */
    {"INTO",              INTO},                  /* 2002 */
    {"INTRINSIC",         INTRINSIC},             /* 2002 (C/S) */
    {"INVALID",           INVALID},               /* 2002 */
    {"INVOKE",            -1},                    /* 2002 */
    {"IS",                IS},                    /* 2002 */
    {"JUST",              JUSTIFIED},             /* 2002 */
    {"JUSTIFIED",         JUSTIFIED},             /* 2002 */
    {"KEPT",              KEPT},                  /* MF */
    {"KEY",               KEY},                   /* 2002 */
    {"LABEL",             LABEL},                 /* 85 */
    {"LAST",              LAST},                  /* 2002 */
    {"LC_ALL",            -1},                    /* 2002 (C/S) */
    {"LC_COLLATE",        -1},                    /* 2002 (C/S) */
    {"LC_CTYPE",          -1},                    /* 2002 (C/S) */
    {"LC_MESSAGES",       -1},                    /* 2002 (C/S) */
    {"LC_MONETARY",       -1},                    /* 2002 (C/S) */
    {"LC_NUMERIC",        -1},                    /* 2002 (C/S) */
    {"LC_TIME",           -1},                    /* 2002 (C/S) */
    {"LEADING",           LEADING},               /* 2002 */
    {"LEFT",              LEFT},                  /* 2002 */
    {"LENGTH",            LENGTH},                /* 2002 */
    {"LENGTH-AN",         LENGTH},                /* MF   */
    {"LENGTH-CHECK",      FULL},                  /* MF   */
    {"LESS",              LESS},                  /* 2002 */
    {"LIKE",              LIKE},                  /* MF   */
    {"LIMIT",             LIMIT},                 /* 2002 */
    {"LIMITS",            LIMITS},                /* 2002 */
    {"LINAGE",            LINAGE},                /* 2002 */
    {"LINAGE-COUNTER",    LINAGE_COUNTER},        /* 2002 */
    {"LINE",              LINE},                  /* 2002 */
    {"LINE-COUNTER",      -1},                    /* 2002 */
    {"LINES",             LINES},                 /* 2002 */
    {"LINKAGE",           LINKAGE},               /* 2002 */
    {"LOCAL-STORAGE",     LOCAL_STORAGE},         /* 2002 */
    {"LOCALE",            LOCALE},                /* 2002 */
    {"LOCK",              LOCK},                  /* 2002 */
    {"LOW",               LOWLIGHT},              /* RM/COBOL */
    {"LOW-VALUE",         LOW_VALUE},             /* 2002 */
    {"LOW-VALUES",        LOW_VALUE},             /* 2002 */
    {"LOWLIGHT",          LOWLIGHT},              /* 2002 (C/S) */
    {"MANUAL",            MANUAL},                /* 2002 (C/S) */
    {"MEMORY",            MEMORY},                /* 85 */
    {"MERGE",             MERGE},                 /* 2002 */
    {"MESSAGE",           -1},                    /* 2002 */
    {"METHOD",            -1},                    /* 2002 */
    {"METHOD-ID",         -1},                    /* 2002 */
    {"MINUS",             MINUS},                 /* 2002 */
    {"MODE",              MODE},                  /* 2002 */
    {"MOVE",              MOVE},                  /* 2002 */
    {"MULTIPLE",          MULTIPLE},              /* 2002 (C/S) */
    {"MULTIPLY",          MULTIPLY},              /* 2002 */
    {"NAMED",             NAMED},                 /* MF   */
    {"NATIONAL",          NATIONAL},              /* 2002 */
    {"NATIONAL-EDITED",   NATIONAL_EDITED},       /* 2002 */
    {"NATIVE",            NATIVE},                /* 2002 */
    {"NEGATIVE",          NEGATIVE},              /* 2002 */
    {"NESTED",            -1},                    /* 2002 */
    {"NEXT",              NEXT},                  /* 2002 */
    {"NO",                NO},                    /* 2002 */
    {"NO-ECHO",           SECURE},                /* MF   */  
    {"NONE",              -1},                    /* 2002 (C/S) */
    {"NORMAL",            -1},                    /* 2002 (C/S) */
    {"NOT",               NOT},                   /* 2002 */
    {"NULL",              TOK_NULL},              /* 2002 */
    {"NULLS",             TOK_NULL},              /* extension */
    {"NUMBER",            NUMBER},                /* 2002 */
    {"NUMBERS",           NUMBER},                /* 2002 (C/S) */
    {"NUMERIC",           NUMERIC},               /* 2002 */
    {"NUMERIC-EDITED",    NUMERIC_EDITED},        /* 2002 */
    {"OBJECT",            -1},                    /* 2002 */
    {"OBJECT-COMPUTER",   OBJECT_COMPUTER},       /* 2002 */
    {"OBJECT-REFERENCE",  -1},                    /* 2002 */
    {"OCCURS",            OCCURS},                /* 2002 */
    {"OF",                OF},                    /* 2002 */
    {"OFF",               OFF},                   /* 2002 */
    {"OMITTED",           OMITTED},               /* 2002 */
    {"ON",                ON},                    /* 2002 */
    {"ONLY",              ONLY},                  /* 2002 (C/S) */
    {"OPEN",              OPEN},                  /* 2002 */
    {"OPTIONAL",          OPTIONAL},              /* 2002 */
    {"OPTIONS",           -1},                    /* 2002 */
    {"OR",                OR},                    /* 2002 */
    {"ORDER",             ORDER},                 /* 2002 */
    {"ORGANISATION",      ORGANIZATION},          /* 2002 */
    {"ORGANIZATION",      ORGANIZATION},          /* 2002 */
    {"OTHER",             OTHER},                 /* 2002 */
    {"OUTPUT",            OUTPUT},                /* 2002 */
    {"OVERFLOW",          COB_OVERFLOW},          /* 2002 */
    {"OVERLINE",          OVERLINE},              /* extension */
    {"OVERRIDE",          -1},                    /* 2002 */
    {"PACKED-DECIMAL",    PACKED_DECIMAL},        /* 2002 */
    {"PADDING",           PADDING},               /* 2002 */
    {"PAGE",              PAGE},                  /* 2002 */
    {"PAGE-COUNTER",      -1},                    /* 2002 */
    {"PARAGRAPH",         PARAGRAPH},             /* 2002 (C/S) */
    {"PARSE",             PARSE},                 /* extention */
    {"PERFORM",           PERFORM},               /* 2002 */
    {"PF",                -1},                    /* 2002 */
    {"PH",                -1},                    /* 2002 */
    {"PIC",               0},                     /* 2002 */
    {"PICTURE",           0},                     /* 2002 */
    {"PLUS",              PLUS},                  /* 2002 */
    {"POINTER",           POINTER},               /* 2002 */
    {"POSITION",          POSITION},              /* 85 */
    {"POSITIVE",          POSITIVE},              /* 2002 */
    {"PRESENT",           PRESENT},               /* 2002 */
    {"PREVIOUS",          PREVIOUS},              /* 2002 (C/S) */
    {"PRINTER",           PRINTER},               /* extension */
    {"PRINTING",          PRINTING},              /* 2002 */
    {"PROCEDURE",         PROCEDURE},             /* 2002 */
    {"PROCEDURE-POINTER", PROGRAM_POINTER},       /* extension */
    {"PROCEDURES",        PROCEDURES},            /* extension */
    {"PROCEED",           PROCEED},               /* 85 */
    {"PROCESSING",        PROCESSING},            /* extention */
    {"PROGRAM",           PROGRAM},               /* 2002 */
    {"PROGRAM-ID",        PROGRAM_ID},            /* 2002 */
    {"PROGRAM-POINTER",   PROGRAM_POINTER},       /* 2002 */
    {"PROMPT",            PROMPT},                /* extension */
    {"PROPERTY",          -1},                    /* 2002 */
    {"PROTOTYPE",         -1},                    /* 2002 */
    {"PURGE",             -1},                    /* 2002 */
    {"QUEUE",             -1},                    /* 2002 */
    {"QUOTE",             QUOTE},                 /* 2002 */
    {"QUOTES",            QUOTE},                 /* 2002 */
    {"RAISE",             -1},                    /* 2002 */
    {"RAISING",           -1},                    /* 2002 */
    {"RANDOM",            RANDOM},                /* 2002 */
    {"RD",                RD},                    /* 2002 */
    {"READ",              READ},                  /* 2002 */
    {"READY",             READY},                 /* MF */
    {"RECEIVE",           -1},                    /* 2002 */
    {"RECORD",            RECORD},                /* 2002 */
    {"RECORD-OVERFLOW",   RECORD_OVERFLOW},       /* OBSOLET */
    {"RECORDING",         RECORDING},             /* extension */
    {"RECORDS",           RECORDS},               /* 2002 */
    {"RECURSIVE",         RECURSIVE},             /* 2002 (C/S) */
    {"REDEFINES",         REDEFINES},             /* 2002 */
    {"REEL",              REEL},                  /* 2002 */
    {"REFERENCE",         REFERENCE},             /* 2002 */
    {"RELATION",          -1},                    /* 2002 (C/S) */
    {"RELATIVE",          RELATIVE},              /* 2002 */
    {"RELEASE",           RELEASE},               /* 2002 */
    {"REMAINDER",         REMAINDER},             /* 2002 */
    {"REMOVAL",           REMOVAL},               /* 2002 */
    {"RENAMES",           RENAMES},               /* 2002 */
    {"REORG-CRITERIA",    REORG_CRITERIA},        /* OBSOLET */
    {"REPLACE",           -1},                    /* 2002 */
    {"REPLACING",         REPLACING},             /* 2002 */
    {"REPORT",            REPORT},                /* 2002 */
    {"REPORTING",         REPORTING},             /* 2002 */
    {"REPORTS",           REPORTS},               /* 2002 */
    {"REPOSITORY",        REPOSITORY},            /* 2002 */
    {"REQUIRED",          REQUIRED},              /* 2002 (C/S) */
    {"RESERVE",           RESERVE},               /* 2002 */
    {"RESET",             RESET},                 /* MF */
    {"RESUME",            -1},                    /* 2002 */
    {"RETRY",             -1},                    /* 2002 */
    {"RETURN",            RETURN},                /* 2002 */
    {"RETURNING",         RETURNING},             /* 2002 */
    {"REVERSE",           REVERSE_VIDEO},         /* RM/COBOL */
    {"REVERSED",          REVERSED},              /* MF   */
    {"REVERSE-VIDEO",     REVERSE_VIDEO},         /* 2002 (C/S) */
    {"REWIND",            REWIND},                /* 2002 */
    {"REWRITE",           REWRITE},               /* 2002 */
    {"RF",                -1},                    /* 2002 */
    {"RH",                -1},                    /* 2002 */
    {"RIGHT",             RIGHT},                 /* 2002 */
    {"ROLLBACK",          ROLLBACK},              /* extension */
    {"ROUNDED",           ROUNDED},               /* 2002 */
    {"RUN",               RUN},                   /* 2002 */
    {"SAME",              SAME},                  /* 2002 */
    {"SCREEN",            SCREEN},                /* 2002 */
    {"SCROLL",            SCROLL},                /* extension */
    {"SD",                SD},                    /* 2002 */
    {"SEARCH",            SEARCH},                /* 2002 */
    {"SECONDS",           -1},                    /* 2002 (C/S) */
    {"SECTION",           SECTION},               /* 2002 */
    {"SECURE",            SECURE},                /* 2002 (C/S) */
    {"SEGMENT",           -1},                    /* 2002 */
    {"SEGMENT-LIMIT",     SEGMENT_LIMIT},         /* 85 */
    {"SELECT",            SELECT},                /* 2002 */
    {"SELF",              -1},                    /* 2002 */
    {"SEND",              -1},                    /* 2002 */
    {"SENTENCE",          SENTENCE},              /* 2002 */
    {"SEPARATE",          SEPARATE},              /* 2002 */
    {"SEQUENCE",          SEQUENCE},              /* 2002 */
    {"SEQUENTIAL",        SEQUENTIAL},            /* 2002 */
    {"SET",               SET},                   /* 2002 */
    {"SHARING",           SHARING},               /* 2002 */
    {"SIGN",              SIGN},                  /* 2002 */
    {"SIGNED",            SIGNED},                /* 2002 (C/S) */
    {"SIGNED-INT",        SIGNED_INT},            /* extension */
    {"SIGNED-LONG",       SIGNED_LONG},           /* extension */
    {"SIGNED-SHORT",      SIGNED_SHORT},          /* extension */
    {"SIZE",              SIZE},                  /* 2002 */
    {"SORT",              SORT},                  /* 2002 */
    {"SORT-MERGE",        SORT_MERGE},            /* 2002 */
    {"SOURCE",            SOURCE},                /* 2002 */
    {"SOURCE-COMPUTER",   SOURCE_COMPUTER},       /* 2002 */
    {"SOURCES",           -1},                    /* 2002 */
    {"SPACE",             SPACE},                 /* 2002 */
    {"SPACES",            SPACE},                 /* 2002 */
    {"SPECIAL-NAMES",     SPECIAL_NAMES},         /* 2002 */
    {"STANDARD",          STANDARD},              /* 2002 */
    {"STANDARD-1",        STANDARD_1},            /* 2002 */
    {"STANDARD-2",        STANDARD_2},            /* 2002 */
    {"START",             START},                 /* 2002 */
    {"STATEMENT",         -1},                    /* 2002 (C/S) */
    {"STATIC",            STATIC},                /* extension */
    {"STATUS",            STATUS},                /* 2002 */
    {"STDCALL",           STDCALL},               /* extension */
    {"STEP",              -1},                    /* 2002 (C/S) */
    {"STOP",              STOP},                  /* 2002 */
    {"STRING",            STRING},                /* 2002 */
    {"STRONG",            -1},                    /* 2002 (C/S) */
    {"SUB-QUEUE-1",       -1},                    /* 2002 */
    {"SUB-QUEUE-2",       -1},                    /* 2002 */
    {"SUB-QUEUE-3",       -1},                    /* 2002 */
    {"SUBTRACT",          SUBTRACT},              /* 2002 */
    {"SUCCESS",           SUCCESS},               /* MVS */
    {"SUM",               SUM},                   /* 2002 */
    {"SUPER",             -1},                    /* 2002 */
    {"SUPPRESS",          SUPPRESS},              /* 2002 */
    {"SYMBOL",            -1},                    /* 2002 (C/S) */
    {"SYMBOLIC",          SYMBOLIC},              /* 2002 */
    {"SYNC",              SYNCHRONIZED},          /* 2002 */
    {"SYNCHRONIZED",      SYNCHRONIZED},          /* 2002 */
    {"SYSTEM-DEFAULT",    -1},                    /* 2002 */
    {"TAB",               TAB},                   /* RM/Cobol */
    {"TABLE",             -1},                    /* 2002 */
    {"TALLYING",          TALLYING},              /* 2002 */
    {"TAPE",              TAPE},                  /* 85 */
/*  {"TERMINAL",          -1},                     2002 */
    {"TERMINATE",         TERMINATE},             /* 2002 */
    {"TEST",              TEST},                  /* 2002 */
    {"TEXT",              -1},                    /* 2002 */
    {"THAN",              THAN},                  /* 2002 */
    {"THEN",              THEN},                  /* 2002 */
    {"THROUGH",           THRU},                  /* 2002 */
    {"THRU",              THRU},                  /* 2002 */
    {"TIME",              TIME},                  /* 2002 */
    {"TIMEOUT",           TIMEOUT},               /* MF   */
    {"TIMES",             TIMES},                 /* 2002 */
    {"TO",                TO},                    /* 2002 */
    {"TOP",               TOP},                   /* 2002 */
    {"TRAILING",          TRAILING},              /* 2002 */
    {"TRACE",             TRACE},                 /* MF */
    {"TRANSFORM",         TRANSFORM},             /* OSVS */
    {"TRUE",              TOK_TRUE},              /* 2002 */
    {"TYPE",              TYPE},                  /* 2002 */
    {"TYPEDEF",           TYPEDEF},               /* 2002 */
    {"UCS-4",             -1},                    /* 2002 (C/S) */
    {"UNDERLINE",         UNDERLINE},             /* 2002 (C/S) */
    {"UNIT",              UNIT},                  /* 2002 */
    {"UNIVERSAL",         -1},                    /* 2002 */
    {"UNLOCK",            UNLOCK},                /* 2002 */
    {"UNSIGNED",          UNSIGNED},              /* 2002 (C/S) */
    {"UNSIGNED-INT",      UNSIGNED_INT},          /* extension */
    {"UNSIGNED-LONG",     UNSIGNED_LONG},         /* extension */
    {"UNSIGNED-SHORT",    UNSIGNED_SHORT},        /* extension */
    {"UNSTRING",          UNSTRING},              /* 2002 */
    {"UNTIL",             UNTIL},                 /* 2002 */
    {"UP",                UP},                    /* 2002 */
    {"UPDATE",            UPDATE},                /* extension */
    {"UPON",              UPON},                  /* 2002 */
    {"USAGE",             USAGE},                 /* 2002 */
    {"USE",               USE},                   /* 2002 */
    {"USER-DEFAULT",      -1},                    /* 2002 */
    {"USING",             USING},                 /* 2002 */
    {"UTF-16",            -1},                    /* 2002 (C/S) */
    {"UTF-8",             -1},                    /* 2002 (C/S) */
    {"VAL-STATUS",        -1},                    /* 2002 */
    {"VALID",             -1},                    /* 2002 */
    {"VALIDATE",          -1},                    /* 2002 */
    {"VALIDATE-STATUS",   -1},                    /* 2002 */
    {"VALUE",             VALUE},                 /* 2002 */
    {"VALUES",            VALUE},                 /* 2002 */
    {"VARYING",           VARYING},               /* 2002 */
    {"WAIT",              WAIT},                  /* extension */
    {"WHEN",              WHEN},                  /* 2002 */
    {"WITH",              WITH},                  /* 2002 */
    {"WORDS",             WORDS},                 /* 85 */
    {"WORKING-STORAGE",   WORKING_STORAGE},       /* 2002 */
    {"WRITE",             WRITE},                 /* 2002 */
    {"WRITE-ONLY",        WRITE_ONLY},            /* OBSOLET */
    {"XML",               XML},                   /* extension */
    {"YYYYDDD",           YYYYDDD},               /* 2002 (C/S) */
    {"YYYYMMDD",          YYYYMMDD},              /* 2002 (C/S) */
    {"ZERO",              ZERO},                  /* 2002 */
    {"ZEROES",            ZERO},                  /* 2002 */
    {"ZEROS",             ZERO},                  /* 2002 */
};

#define NUM_RESERVED_WORDS      sizeof (reserved_words) / sizeof (struct reserved)

/* FUNCTION List */
/* Name, Arguments, Implemented, Enum intrinsic, Routine, Category, Can refmod */

static struct cb_intrinsic_table function_list[] = {
    { "ABS",                              1, 1, CB_INTR_ABS,
        "cob_intr_abs",
        CB_CATEGORY_NUMERIC, 0},
    { "ACOS",                             1, 1, CB_INTR_ACOS,
        "cob_intr_acos",
        CB_CATEGORY_NUMERIC, 0},
    { "ANNUITY",                          2, 1, CB_INTR_ANNUITY,
        "cob_intr_annuity",
        CB_CATEGORY_NUMERIC, 0},
    { "ASIN",                             1, 1, CB_INTR_ASIN,
        "cob_intr_asin",
        CB_CATEGORY_NUMERIC, 0},
    { "ATAN",                             1, 1, CB_INTR_ATAN,
        "cob_intr_atan",
        CB_CATEGORY_NUMERIC, 0},
    { "BOOLEAN-OF-INTEGER",               2, 0, CB_INTR_BOOLEAN_OF_INTEGER,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "BYTE-LENGTH",                      1, 1, CB_INTR_BYTE_LENGTH,
        "cob_intr_byte_length",
        CB_CATEGORY_NUMERIC, 0},
    { "CHAR",                             1, 1, CB_INTR_CHAR,
        "cob_intr_char",
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "CHAR-NATIONAL",                    1, 0, CB_INTR_CHAR_NATIONAL,
        NULL,
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "COMBINED-DATETIME",                2, 1, CB_INTR_COMBINED_DATETIME,
        "cob_intr_combined_datetime",
        CB_CATEGORY_NUMERIC, 0},
    { "CONCATENATE",                      -1, 1, CB_INTR_CONCATENATE,
        "cob_intr_concatenate",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "COS",                              1, 1, CB_INTR_COS,
        "cob_intr_cos",
        CB_CATEGORY_NUMERIC, 0},
    { "CURRENT-DATE",                     0, 1, CB_INTR_CURRENT_DATE,
        "cob_intr_current_date",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "CSTRING",                          1, 1, CB_INTR_CSTRING,
        "cob_intr_cstring",
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "DATE-OF-INTEGER",                  1, 1, CB_INTR_DATE_OF_INTEGER,
        "cob_intr_date_of_integer",
        CB_CATEGORY_NUMERIC, 0},
    { "DATE-TO-YYYYMMDD",                 -1, 1, CB_INTR_DATE_TO_YYYYMMDD,
        "cob_intr_date_to_yyyymmdd",
        CB_CATEGORY_NUMERIC, 0},
    { "DAY-OF-INTEGER",                   1, 1, CB_INTR_DAY_OF_INTEGER,
        "cob_intr_day_of_integer",
        CB_CATEGORY_NUMERIC, 0},
    { "DAY-TO-YYYYDDD",                   -1, 1, CB_INTR_DAY_TO_YYYYDDD,
        "cob_intr_day_to_yyyyddd",
        CB_CATEGORY_NUMERIC, 0},
    { "DISPLAY-OF",                       -1, 1, CB_INTR_DISPLAY_OF,
        "cob_intr_display_of_1",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "E",                                0, 1, CB_INTR_E,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "EXCEPTION-FILE",                   0, 1, CB_INTR_EXCEPTION_FILE,
        "cob_intr_exception_file",
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "EXCEPTION-FILE-N",                 0, 0, CB_INTR_EXCEPTION_FILE_N,
        NULL,
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "EXCEPTION-LOCATION",               0, 1, CB_INTR_EXCEPTION_LOCATION,
        "cob_intr_exception_location",
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "EXCEPTION-LOCATION-N",             0, 0, CB_INTR_EXCEPTION_LOCATION_N,
        NULL,
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "EXCEPTION-STATEMENT",              0, 1, CB_INTR_EXCEPTION_STATEMENT,
        "cob_intr_exception_statement",
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "EXCEPTION-STATUS",                 0, 1, CB_INTR_EXCEPTION_STATUS,
        "cob_intr_exception_status",
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "EXP",                              1, 1, CB_INTR_EXP,
        "cob_intr_exp",
        CB_CATEGORY_NUMERIC, 0},
    { "EXP10",                            1, 1, CB_INTR_EXP10,
        "cob_intr_exp10",
        CB_CATEGORY_NUMERIC, 0},
    { "FACTORIAL",                        1, 1, CB_INTR_FACTORIAL,
        "cob_intr_factorial",
        CB_CATEGORY_NUMERIC, 0},
    { "FRACTION-PART",                    1, 1, CB_INTR_FRACTION_PART,
        "cob_intr_fraction_part",
        CB_CATEGORY_NUMERIC, 0},
    { "HIGHEST-ALGEBRAIC",                1, 0, CB_INTR_HIGHEST_ALGEBRAIC,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "INTEGER",                          1, 1, CB_INTR_INTEGER,
        "cob_intr_integer",
        CB_CATEGORY_NUMERIC, 0},
    { "INTEGER-OF-BOOLEAN",               1, 0, CB_INTR_INTEGER_OF_BOOLEAN,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "INTEGER-OF-DATE",                  1, 1, CB_INTR_INTEGER_OF_DATE,
        "cob_intr_integer_of_date",
        CB_CATEGORY_NUMERIC, 0},
    { "INTEGER-OF-DAY",                   1, 1, CB_INTR_INTEGER_OF_DAY,
        "cob_intr_integer_of_day",
        CB_CATEGORY_NUMERIC, 0},
    { "INTEGER-PART",                     1, 1, CB_INTR_INTEGER_PART,
        "cob_intr_integer_part",
        CB_CATEGORY_NUMERIC, 0},
    { "LENGTH",                           1, 1, CB_INTR_LENGTH,
        "cob_intr_length",
        CB_CATEGORY_NUMERIC, 0},
    { "LENGTH-AN",                        1, 1, CB_INTR_LENGTH,
        "cob_intr_length",
        CB_CATEGORY_NUMERIC, 0},
    { "LOCALE-COMPARE",                   -1, 0, CB_INTR_LOCALE_COMPARE,
        NULL,
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "LOCALE-DATE",                      2, 1, CB_INTR_LOCALE_DATE,
        "cob_intr_locale_date",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "LOCALE-TIME",                      2, 1, CB_INTR_LOCALE_TIME,
        "cob_intr_locale_time",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "LOCALE-TIME-FROM-SECONDS",         2, 1, CB_INTR_LOCALE_TIME_FROM_SECS,
        "cob_intr_lcl_time_from_secs",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "LOG",                              1, 1, CB_INTR_LOG,
        "cob_intr_log",
        CB_CATEGORY_NUMERIC, 0},
    { "LOG10",                            1, 1, CB_INTR_LOG10,
        "cob_intr_log10",
        CB_CATEGORY_NUMERIC, 0},
    { "LOWER-CASE",                       1, 1, CB_INTR_LOWER_CASE,
        "cob_intr_lower_case",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "LOWEST-ALGEBRAIC",                 1, 0, CB_INTR_LOWEST_ALGEBRAIC,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "MAX",                              -1, 1, CB_INTR_MAX,
        "cob_intr_max",
        CB_CATEGORY_NUMERIC, 0},
    { "MEAN",                             -1, 1, CB_INTR_MEAN,
        "cob_intr_mean",
        CB_CATEGORY_NUMERIC, 0},
    { "MEDIAN",                           -1, 1, CB_INTR_MEDIAN,
        "cob_intr_median",
        CB_CATEGORY_NUMERIC, 0},
    { "MIDRANGE",                         -1, 1, CB_INTR_MIDRANGE,
        "cob_intr_midrange",
        CB_CATEGORY_NUMERIC, 0},
    { "MIN",                              -1, 1, CB_INTR_MIN,
        "cob_intr_min",
        CB_CATEGORY_NUMERIC, 0},
    { "MOD",                              2, 1, CB_INTR_MOD,
        "cob_intr_mod",
        CB_CATEGORY_NUMERIC, 0},
    { "NATIONAL-OF",                      -1, 1, CB_INTR_NATIONAL_OF,
        "cob_intr_national_of_1",
        CB_CATEGORY_NATIONAL, 1},
    { "NUMVAL",                           1, 1, CB_INTR_NUMVAL,
        "cob_intr_numval",
        CB_CATEGORY_NUMERIC, 0},
    { "NUMVAL-C",                         2, 1, CB_INTR_NUMVAL_C,
        "cob_intr_numval_c",
        CB_CATEGORY_NUMERIC, 0},
    { "NUMVAL-F",                         1, 0, CB_INTR_NUMVAL_F,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "ORD",                              1, 1, CB_INTR_ORD,
        "cob_intr_ord",
        CB_CATEGORY_NUMERIC, 0},
    { "ORD-MAX",                          -1, 1, CB_INTR_ORD_MAX,
        "cob_intr_ord_max",
        CB_CATEGORY_NUMERIC, 0},
    { "ORD-MIN",                          -1, 1, CB_INTR_ORD_MIN,
        "cob_intr_ord_min",
        CB_CATEGORY_NUMERIC, 0},
    { "PI",                               0, 1, CB_INTR_PI,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "PRESENT-VALUE",                    -1, 1, CB_INTR_PRESENT_VALUE,
        "cob_intr_present_value",
        CB_CATEGORY_NUMERIC, 0},
    { "RANDOM",                           -1, 1, CB_INTR_RANDOM,
        "cob_intr_random",
        CB_CATEGORY_NUMERIC, 0},
    { "RANGE",                            -1, 1, CB_INTR_RANGE,
        "cob_intr_range",
        CB_CATEGORY_NUMERIC, 0},
    { "REM",                              2, 1, CB_INTR_REM,
        "cob_intr_rem",
        CB_CATEGORY_NUMERIC, 0},
    { "REVERSE",                          1, 1, CB_INTR_REVERSE,
        "cob_intr_reverse",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "SECONDS-FROM-FORMATTED-TIME",      2, 1, CB_INTR_SECONDS_PAST_MIDNIGHT,
        "cob_intr_seconds_from_formatted_time",
        CB_CATEGORY_NUMERIC, 0},
    { "SECONDS-PAST-MIDNIGHT",            0, 1, CB_INTR_SECONDS_PAST_MIDNIGHT,
        "cob_intr_seconds_past_midnight",
        CB_CATEGORY_NUMERIC, 0},
    { "SIGN",                             1, 1, CB_INTR_SIGN,
        "cob_intr_sign",
        CB_CATEGORY_NUMERIC, 0},
    { "SIN",                              1, 1, CB_INTR_SIN,
        "cob_intr_sin",
        CB_CATEGORY_NUMERIC, 0},
    { "SQRT",                             1, 1, CB_INTR_SQRT,
        "cob_intr_sqrt",
        CB_CATEGORY_NUMERIC, 0},
    { "STANDARD-COMPARE",                 -1, 0, CB_INTR_STANDARD_COMPARE,
        NULL,
        CB_CATEGORY_ALPHANUMERIC, 0},
    { "STANDARD-DEVIATION",               -1, 1, CB_INTR_STANDARD_DEVIATION,
        "cob_intr_standard_deviation",
        CB_CATEGORY_NUMERIC, 0},
    { "STORED-CHAR-LENGTH",               1, 1, CB_INTR_STORED_CHAR_LENGTH,
        "cob_intr_stored_char_length",
        CB_CATEGORY_NUMERIC, 0},
    { "SUBSTITUTE",                       -1, 1, CB_INTR_SUBSTITUTE,
        "cob_intr_substitute",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "SUBSTITUTE-CASE",                  -1, 1, CB_INTR_SUBSTITUTE_CASE,
        "cob_intr_substitute_case",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "SUM",                              -1, 1, CB_INTR_SUM,
        "cob_intr_sum",
        CB_CATEGORY_NUMERIC, 0},
    { "TAN",                              1, 1, CB_INTR_TAN,
        "cob_intr_tan",
        CB_CATEGORY_NUMERIC, 0},
    { "TEST-DATE-YYYYMMDD",               1, 1, CB_INTR_TEST_DATE_YYYYMMDD,
        "cob_intr_test_date_yyyymmdd",
        CB_CATEGORY_NUMERIC, 0},
    { "TEST-DAY-YYYYDDD",                 1, 1, CB_INTR_TEST_DAY_YYYYDDD,
        "cob_intr_test_day_yyyyddd",
        CB_CATEGORY_NUMERIC, 0},
    { "TEST-NUMVAL",                      1, 0, CB_INTR_TEST_NUMVAL,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "TEST-NUMVAL-C",                    -1, 0, CB_INTR_TEST_NUMVAL_C,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "TEST-NUMVAL-F",                    1, 0, CB_INTR_TEST_NUMVAL_F,
        NULL,
        CB_CATEGORY_NUMERIC, 0},
    { "TRIM",                             2, 1, CB_INTR_TRIM,
        "cob_intr_trim",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "TRIML",                            1, 1, CB_INTR_TRIML,
        "cob_intr_triml",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "TRIMR",                            1, 1, CB_INTR_TRIMR,
        "cob_intr_trimr",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "DISPLAY-TO-UTF8",                      -1, 1, CB_INTR_TO_UTF8,
        "cob_intr_display_to_debugcp_1",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "UTF8-TO-DISPLAY",                      -1, 1, CB_INTR_FROM_UTF8,
        "cob_intr_debugcp_to_display_1",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "UPPER-CASE",                       1, 1, CB_INTR_UPPER_CASE,
        "cob_intr_upper_case",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "VARIANCE",                         -1, 1, CB_INTR_VARIANCE, 
        "cob_intr_variance",
        CB_CATEGORY_NUMERIC, 0},
    { "WHEN-COMPILED",                    0, 1, CB_INTR_WHEN_COMPILED,
        "cob_intr_when_compiled",
        CB_CATEGORY_ALPHANUMERIC, 1},
    { "YEAR-TO-YYYY",                     -1, 1, CB_INTR_YEAR_TO_YYYY,
        "cob_intr_year_to_yyyy",
        CB_CATEGORY_NUMERIC, 0}
};

#define NUM_INTRINSICS  sizeof(function_list) / sizeof(struct cb_intrinsic_table)
struct cb_intrinsic_table       *userdefined_functions = NULL;

static int
reserve_comp (const void *p1, const void *p2)
{
    return strcasecmp (p1, ((struct reserved *)p2)->name);
}

static int
intrinsic_comp (const void *p1, const void *p2)
{
    return strcasecmp (p1, ((struct cb_intrinsic_table *)p2)->name);
}

cb_tree
lookup_system_name (const char *name)
{
    int     i;

    for (i = 0; system_table[i].name != NULL; ++i) {
        if (! is_reserved_word_excluded (system_table[i].name)) {
            if (strcasecmp (name, system_table[i].name) == 0) {
                return system_table[i].node;
            }
        }
    }
    
    return cb_error_node;
}

int is_reserved_word_excluded (const char *name)
{
    struct noreserve        *noresptr;
    for ( noresptr = norestab; noresptr; noresptr = noresptr->next) {
        if (strcasecmp (name, noresptr->noresword) == 0) {
            return 1;
        }
    }
    return 0;
}

int
lookup_reserved_word (const char *name)
{
    struct reserved *p;

    p = bsearch (name, reserved_words, NUM_RESERVED_WORDS,
                 sizeof (struct reserved), reserve_comp);
    if (!p) {
        return 0;
    }
    if (is_reserved_word_excluded (name)) {
        return 0;
    }
    if (p->token != -1) {
        return p->token;
    }
    if (cb_relaxed_syntax_check) {
        cb_warning (_("'%s' reserved word, but not supported yet"), name);
    } else {
        cb_error (_("'%s' reserved word, but not supported yet"), name);
    }
    return 0;
}

static struct cb_intrinsic_table * 
cb_add_userdefined_intrinsic (const char *name)
{
    struct cb_intrinsic_table *cbp;

    cbp = userdefined_functions;
    while (cbp) {
        if (intrinsic_comp(name, cbp) == 0) {
            return cbp;
        }
        cbp = cbp->next;
    }

    cbp = cobc_malloc(sizeof(struct cb_intrinsic_table));
    if (userdefined_functions == NULL) {
        cbp->next = userdefined_functions;
    }
    userdefined_functions = cbp;
    cbp->name = strdup(name);
    cbp->intr_enum = CB_INTR_USER_DEFINE;
    cbp->refmod = 0;
    cbp->implemented = 1;
    cbp->intr_routine = strdup(cbp->name);
    cbp->args = -1;
    cbp->category = CB_CATEGORY_UNKNOWN;
    return cbp;
}

struct cb_intrinsic_table *
lookup_intrinsic (const char *name, const int checkres) {
    struct cb_intrinsic_table       *cbp;
    cb_tree                         l;

    if (checkres) {
        if (is_reserved_word_excluded (name)) {
            return NULL;
        }
    }
    cbp = bsearch (name, function_list, NUM_INTRINSICS,
                   sizeof (struct cb_intrinsic_table), intrinsic_comp);
    if (cbp && cbp->implemented) {
        return cbp;
    }
    for ( l = current_program->function_spec_list; l; l = CB_CHAIN (l) ) {
        if (strcasecmp(name,CB_NAME (CB_VALUE (l)))==0) {
            return cb_add_userdefined_intrinsic(name);
        }
    }
    
    return NULL;
}


void
cb_list_reserved (void)
{
    const char      *s;
    size_t  i;
    size_t  n;

    printf ("Reserved Words\n\n");
    for (i = 0; i < NUM_RESERVED_WORDS; ++i) {
        if (reserved_words[i].token != -1) {
            n = strlen (reserved_words[i].name);
            switch (n / 8) {
                case 0:
                    s = "\t\t\t\t";
                    break;
                case 1:
                    s = "\t\t\t";
                    break;
                case 2:
                    s = "\t\t";
                    break;
                default:
                    s = "\t";
                    break;
            }
            printf ("%s%s\n", reserved_words[i].name, s);
        }
    }
    printf ("Reserved Words not yet supported\n\n");
    for (i = 0; i < NUM_RESERVED_WORDS; ++i) {
        if (reserved_words[i].token == -1) {
            n = strlen (reserved_words[i].name);
            switch (n / 8) {
                case 0:
                    s = "\t\t\t\t";
                    break;
                case 1:
                    s = "\t\t\t";
                    break;
                case 2:
                    s = "\t\t";
                    break;
                default:
                    s = "\t";
                    break;
            }
            printf ("%s%s\n", reserved_words[i].name, s);
        }
    }
}

void
cb_list_intrinsics (void)
{
    const char      *s;
    size_t          i;
    size_t          n;

    printf ("Intrinsic Function (Implemented Y/N)\n\n");
    for (i = 0; i < NUM_INTRINSICS; ++i) {
        n = strlen (function_list[i].name);
        switch (n / 8) {
            case 0:
                s = "\t\t\t\t";
                break;
            case 1:
                s = "\t\t\t";
                break;
            case 2:
                s = "\t\t";
                break;
            default:
                s = "\t";
                break;
        }
        printf ("%s%s(%s)\n", function_list[i].name, s,
                function_list[i].implemented ? "Y" : "N");
    }
}

void
cb_list_mnemonics (void)
{
    size_t          i;

    printf ("Mnemonic names\n\n");
    for (i = 0; system_table[i].name != NULL; ++i) {
        if (! is_reserved_word_excluded (system_table[i].name)) {
            printf ("%s\n", system_table[i].name);
        }
    }
}


static int
cmpreserverd(const void *p1, const void *p2)
{
   return strcmp(((struct reserved *) p1)->name, ((struct reserved *) p2)->name);
}

static int
cmpfunction(const void *p1, const void *p2)
{
   return strcmp(((struct cb_intrinsic_table *) p1)->name, ((struct cb_intrinsic_table *) p2)->name);
}

void
cb_init_reserved (void)
{
    int     i;

    if (!reserved_sorted) {
        qsort(reserved_words, NUM_RESERVED_WORDS, sizeof(struct reserved),           cmpreserverd);
        qsort(function_list,  NUM_INTRINSICS,     sizeof(struct cb_intrinsic_table), cmpfunction);
        reserved_sorted = 1;
    }
    /* build system-name table */
    for (i = 0; system_table[i].name != NULL; ++i) {
        if (! is_reserved_word_excluded (system_table[i].name)) {
            system_table[i].node =
            cb_build_system_name (system_table[i].category, system_table[i].token);
        }
    }
    userdefined_functions = NULL;
}

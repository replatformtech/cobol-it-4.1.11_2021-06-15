/*                                                      -*- c -*-
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

%option 8bit
%option caseless
%option noyywrap
%option never-interactive

%{
#include "cobc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libcob.h>

#include "tree.h"
#include "enterprise/a2e.h"
#define YYSTYPE                 cb_tree

#include "parser.h"

#define SET_LOCATION(x)                         \
  (x)->source_file = (unsigned char *)cb_lex_source_file.file_name;           \
  (x)->source_line = cb_lex_source_file.line; \
  (x)->source_column = cb_lex_source_file.column;

struct cb_level_78 {
        struct cb_level_78      *next;
        struct cb_field         *fld78;
};

/* Global variables */
size_t                          cb_force_pid_literal = 0;

/* Local variables */
static struct cb_level_78       *lev78ptr = NULL;
static unsigned char            *plexbuff = NULL;
static size_t                   plexsize;
static size_t                   in_procedure;
static int                      last_token_is_dot = 0;
static int                      integer_is_label = 0;
static int                      inside_bracket = 0;
static int                      inside_repository = 0;
static int                      ignore_line=0;
static int                      in_debug_final_source = 0;
static FILE*                    exec_file = NULL;

static int read_literal (int mark, char mode);
static int scan_digit (char *text);
static int scan_x (char *text, int national);
static int scan_h (char *text);
static int scan_b (char *text);
static int scan_numeric (char *text);
static int scan_picture (char *text);
static void count_lines (char *text);
static void dump_exec_file_entry (char *text);

%}

%s DECIMAL_IS_PERIOD DECIMAL_IS_COMMA
%x PICTURE_STATE FUNCTION_STATE SET_STATE DOLLART_STATE DOLLART_STATE_REST EXEC_STATE 

%%
%{
        if (current_program) {
                if (current_program->decimal_point == '.') {
                        BEGIN DECIMAL_IS_PERIOD;
                } else {
                        BEGIN DECIMAL_IS_COMMA;
                }
        }

        /* We treat integer literals immediately after '.' as labels;
           that is, they must be level numbers or section names. */
        integer_is_label = 0;
        if (last_token_is_dot) {
                integer_is_label = 1;
                last_token_is_dot = 0;
        }
%}

<*>^"*#PRAGMA"    {                   
                  ignore_line = 1;
                  return PRAGMA; 
                  }
<*>^"*PRAGMA"    {                   
                  return PRAGMA; 
                  }
<*>^[ \t]*"PRAGMA"      { 
                  return PRAGMA; 
                  }

\n                {
                  if ((!ignore_line && !cb_raw_debug_line) || in_debug_final_source)
                    cb_lex_source_file.line++;
                  ignore_line = 0;
                  cb_final_source_line++;
                  cb_lex_source_file.column=1;
                  }

<*>^"*#DEBUG"[0-9]+" GUBED".*  {
                  /* line directive */
                  char *endp = yytext + 2;
                  char *b, *e, *c;
                  in_debug_final_source = 0;

                  b = strchr (endp, '"');   
                  if (b){
                     c = strdup (b+1);
                     e = strrchr (c, '"');
                     if (e) {
                        *e = '\0';
                        cobc_set_lex_file_name((unsigned char *)c);
                        cb_lex_source_file.line = strtol (yytext + 7, &endp, 10) - 1;
                        if (cb_raw_debug_line)
                           cb_lex_source_file.line++;
                     }
                  } else {
                     cb_lex_source_file.line = strtol (yytext + 7, &endp, 10) - 1;
                     if (cb_raw_debug_line)
                        cb_lex_source_file.line++;
                  }

                  cb_lex_source_file.column+= yyleng;
                  } 

<*>^"*#DEBUG"[0-9]+" H"[0-9]+.*  {
                  /* line directive */
                  char *endp = yytext + 2;
                  char *b, *e, *c;
                  in_debug_final_source = 0;

                  b = strchr (endp, 'H');   
                  if (b){
                     cobc_find_lex_hash(atoi(b+1));
                     cb_lex_source_file.line = strtol (yytext + 7, &endp, 10) - 1;
                     if (cb_raw_debug_line)
                        cb_lex_source_file.line++;
                     
                     b = strchr (b+2, '"');   
                     if (b){
                        c = strdup (b+1);
                        e = strrchr (c, '"');
                        if (e) {
                           *e = '\0';
                           cobc_set_lex_file_name((unsigned char *)c);                         
                        }
                     } 
                  } else {
                     cb_lex_source_file.line = strtol (yytext + 7, &endp, 10) - 1;
                     if (cb_raw_debug_line)
                        cb_lex_source_file.line++;
                  }


                  cb_lex_source_file.column+= yyleng;
                  } 

<*>^"*#DEBUG-CONT".*  {
                  /* line directive */
                  char *endp = yytext + 2;
                  char *b, *e, *c;

                  b = strchr (endp, '"');   
                  if (b){
                     c = strdup (b+1);
                     e = strrchr (c, '"');
                     if (e) {
                        *e = '\0';
                        if (cb_lex_source_file.file_name) {
                           e = malloc(strlen((char*)cb_lex_source_file.file_name)+ strlen(c)+2);
                           strcpy(e, (char*)cb_lex_source_file.file_name);
                           strcat(e, c);
                           free(c);
                           c = e;
                           cobc_set_lex_file_name((unsigned char *)c);
                           cb_lex_source_file.line = cb_lex_source_file.line -1;                        
                        }
                     }
                  }

                  cb_lex_source_file.column+= yyleng;
                  }

<*>^"*#DEBUG-FINAL GUBED".*  {                        
                        cobc_set_lex_file_name((unsigned char *)cb_final_source_file);
                        cb_lex_source_file.line = cb_final_source_line;
                        /*cb_keep_final_source = 1;*/
                  cb_lex_source_file.column+= yyleng;
                  in_debug_final_source = 0;
                  }

<*>^"*#DEBUG-SWITCH GUBED".*  {                        
                        cobc_set_lex_file_name((unsigned char *)cb_final_source_file);
                        cb_lex_source_file.line = cb_final_source_line;
                        cb_keep_final_source = 1;
                        in_debug_final_source = 1; 
                  }

<*>^"*##".*          {
                  /* Ignore */
                  ignore_line = 1;
                  cb_lex_source_file.column+= yyleng;
                  }

<*>^"*"$
<*>^"* ".*        {
                  /* original comment */
                  /* Ignore */
               cb_lex_source_file.column+= yyleng;
               if ( cb_source_format == CB_FORMAT_FIXED ) {
                  cb_check_feature_x (NULL, cb_syntax_ibm5_2, "Invalid * at column 8");
                  if (!cb_flag_loosy_comment)
                  {
                     cb_error("Invalid * at column 8");            
                  }
               }

               }
<*>^"*#>".*
<*>^" **>".*
<*>^"**>".*       {
                  /* original comment */
                  /* Ignore */
               cb_lex_source_file.column+= yyleng;
               }
^[ \t]*"$"     {
                    BEGIN DOLLART_STATE;
                  cb_lex_source_file.column+= yyleng;
                }

<DOLLART_STATE>{
    "SET"   {
                    BEGIN SET_STATE;
                  cb_lex_source_file.column+= yyleng;
            }
    \n      {       
            cb_lex_source_file.line++;
            cb_final_source_line++;
            cb_lex_source_file.column = 1;
            BEGIN INITIAL;      
            }
    .       { BEGIN  DOLLART_STATE_REST; cb_lex_source_file.column++; }
}

<DOLLART_STATE_REST>{
    \n      {       
            cb_lex_source_file.line++;
            cb_final_source_line++;
            cb_lex_source_file.column = 1;
            BEGIN INITIAL;      
            }
    .       { /* ignore */   }
}

<SET_STATE>{
    "DATACOMPRESS" {cb_data_compress = 1;}
    "DATACOMPRESS"[ \t\"\']+[0-9][\"\']? {cb_data_compress = scan_digit(yytext);}
    "KEYCOMPRESS"[ \t\"\']+[0-9][\"\']? {cb_key_compress = scan_digit(yytext);}
    "NODATACOMPRESS" {cb_data_compress =0; }
    "NOKEYCOMPRESS"  {cb_key_compress =0; }
    "LSUTF16"        {cb_ls_utf16=1; }
    "NOLSUTF16"      {cb_ls_utf16=0; }
    "INSERTNULL"     {cb_flag_line_seq_mf =1;}
    "INSERTNULL"[ \t]*[=]?[ \t]*[\"\']?"ON"     {cb_flag_line_seq_mf =1;}
    "INSERTNULL"[ \t]*[=]?[ \t]*[\"\']?"OFF"     {cb_flag_line_seq_mf =0;}
    "NOINSERTNULL"   {cb_flag_line_seq_mf =0;}
    "LINESEQMF"      {cb_flag_line_seq_mf =1;}
    "NOLINESEQMF"    {cb_flag_line_seq_mf =0;}
    "LINESEQDOS"      {cb_flag_line_seq_dos =1;}
    "NOLINESEQDOS"    {cb_flag_line_seq_dos =0;}
    "DEFAULTCALL"[ \t\"\']+[0-9]+[\"\']? {cb_data_compress = scan_digit(yytext);}
    "PERFORM-TYPE"[ \t]*[=]?[ \t]*[\"\']?"MF"         {cb_perform_osvs =0;}
    "PERFORM-TYPE"[ \t]*[=]?[ \t]*[\"\']?"OSVS"       {cb_perform_osvs =1;}
    "PERFORM-TYPE"[ \t]*[=]?[ \t]*[\"\']?"VSC2"       {cb_perform_osvs =1;}
    "PERFORM-TYPE"[ \t]*[=]?[ \t]*[\"\']?"ENTCOBOL"   {cb_perform_osvs =1;}
    "PERFORM-TYPE"[ \t]*[=]?[ \t]*[\"\']?"COB370"     {cb_perform_osvs =1;}
    \n      {       
            cb_lex_source_file.line++;
            cb_final_source_line++;
            BEGIN INITIAL;      
            }
    .       { /* ignore */   }
}


"PIC" |
"PICTURE"   {
                  BEGIN PICTURE_STATE;
                  cb_lex_source_file.column+= yyleng;
            }

"FUNCTION" {
        if (inside_repository) {
                return FUNCTION;
        }
        BEGIN FUNCTION_STATE;
        cb_lex_source_file.column+= yyleng;
}

"DIVISION" {
        inside_repository = 0;
        cb_lex_source_file.column+= yyleng;
        return DIVISION;
}

"PROGRAM-ID" {
        inside_repository = 0;
        cb_lex_source_file.column+= yyleng;
        return PROGRAM_ID;
 }

"REPOSITORY" {
        inside_repository = 1;
        cb_lex_source_file.column+= yyleng;
        return REPOSITORY;
}

[NGZ]?[\'\"] {
        /* string literal */
        cb_lex_source_file.column+= yyleng;
        if ((yytext[0] == 'N') || (yytext[0] == 'G') || (yytext[0] == 'Z') ||
            (yytext[0] == 'n') || (yytext[0] == 'g') || (yytext[0] == 'z'))
            return read_literal (yytext[1], yytext[0]);
        else 
            return read_literal (yytext[0], 0);
}

[NG]?X\'[^\'\n]*\' |
[NG]?X\"[^\"\n]*\" {
        /* X string literal */
        cb_lex_source_file.column+= yyleng;
        if ((yytext[0] == 'N') || (yytext[0] == 'G'))
           return scan_x (yytext + 3, 1);
        else 
           return scan_x (yytext + 2, 0);
}


"(" {
        inside_bracket++;
        cb_lex_source_file.column+= yyleng;
        return '(';
}

")" {
        cb_lex_source_file.column+= yyleng;
        if (inside_bracket > 0) {
                inside_bracket--;
        }
        return ')';
}

[0-9]+ {
        if (integer_is_label) {
                /* integer label */
                yylval = cb_build_reference (yytext);
                SET_LOCATION (yylval);
                cb_lex_source_file.column+= yyleng;
                return WORD;
        } else {
                /* numeric literal */
                int r = scan_numeric (yytext);
                cb_lex_source_file.column+= yyleng;
                return r;
        }
}

<EXEC_STATE>{
  "END-EXEC"[ \t\n]*  {
                            count_lines (yytext);
                            if (exec_file) {
                                fprintf(exec_file,"\n END-EXEC\n");
                                exec_file = NULL;
                            }
                            BEGIN INITIAL;
                        }
  ^"**>".*           {
                    /* original comment */
                    /* Ignore */
                    }
  \n                    {   
                            if (exec_file) {
                               fputs(yytext, exec_file);
                            }
                            cb_lex_source_file.line++; 
                            cb_final_source_line++;
                            cb_lex_source_file.column = 1;
                        }
 [ \t]+ |
 ;+ |
 .                     {   if (exec_file) {
                               fputs(yytext, exec_file);
                            }
                           cb_lex_source_file.column+= yyleng;
                        } 
}


<*>[ \t]+ {
        /* Ignore */
        cb_lex_source_file.column+= yyleng;
}

<*>;+ {
        cb_lex_source_file.column+= yyleng;
        if (inside_bracket) {
                return SEMI_COLON;
        }
        /* Ignore */
}


[0-9]+[eE][0-9]+ {
        if (integer_is_label) {
                /* label */
                yylval = cb_build_reference (yytext);
                SET_LOCATION (yylval);
                cb_lex_source_file.column+= yyleng;
                return WORD;
        } else {
           /* numeric literal */
           cb_lex_source_file.column+= yyleng;
           return scan_numeric (yytext);
        }
}

<DECIMAL_IS_PERIOD>[+-]?[0-9.]*[0-9]+([eE][+-]?[0-9]+)? {
        /* numeric literal */
        return scan_numeric (yytext);
}



<DECIMAL_IS_PERIOD>,+ {
        if (inside_bracket) {
                return COMMA_DELIM;
        }
        /* Ignore */
}

<DECIMAL_IS_COMMA>[+-]?[0-9]+[,]?[0-9]+([eE][+-]?[0-9]+)? {
        /* numeric literal */
        return scan_numeric (yytext);
}

<DECIMAL_IS_COMMA>[+-]?,[0-9]+([eE][+-]?[0-9]+)? {
        /* numeric literal */
        return scan_numeric (yytext);
}

<DECIMAL_IS_COMMA>[+-]?[0-9]+([eE][+-]?[0-9]+)? {
        /* numeric literal */
        return scan_numeric (yytext);
}

<DECIMAL_IS_COMMA>,, {
        unput (',');
}


<DECIMAL_IS_COMMA>, {
        if (inside_bracket) {
                return COMMA_DELIM;
        }
        /* Ignore */
}

H\'[^\'\n]*\' |
H\"[^\"\n]*\" {
        /* H numeric literal */
        return scan_h (yytext + 2);
}

B\'[^\'\n]*\' |
B\"[^\"\n]*\" {
        /* B numeric literal */
        return scan_b (yytext + 2);
}

"END"[ \t\n]+"PROGRAM" {
        count_lines (yytext);
        check_unreached = 0;
        return END_PROGRAM;
}

"END"[ \t\n]+"FUNCTION" {
        count_lines (yytext);
        check_unreached = 0;
        return END_FUNCTION;
}

"SCREEN"[ \t\n]+"CONTROL" {
        count_lines (yytext);
        return SCREEN_CONTROL;
}

"EVENT"[ \t\n]+"STATUS" {
        count_lines (yytext);
        return EVENT_STATUS;
}

"BLANK"[ \t\n]+"SCREEN" {
        count_lines (yytext);
        return BLANK_SCREEN;
}

"BLANK"[ \t\n]+"LINE" {
        count_lines (yytext);
        return BLANK_LINE;
}

"NO"[ \t\n]+"ADVANCING" {
        count_lines (yytext);
        return NO_ADVANCING;
}

"NOT"[ \t\n]+"ON"[ \t\n]+"SIZE"[ \t\n]+"ERROR"[ \t\n] |
"NOT"[ \t\n]+"SIZE"[ \t\n]+"ERROR"[ \t\n] {     
        count_lines (yytext);
        check_unreached = 0;
        return NOT_SIZE_ERROR;
        }

"ON"[ \t\n]+"SIZE"[ \t\n]+"ERROR"[ \t\n] |
"SIZE"[ \t\n]+"ERROR"[ \t\n] {  
        count_lines (yytext);
        check_unreached = 0;
        return SIZE_ERROR;
        }

"NOT"[ \t\n]+"ON"[ \t\n]+"EXCEPTION"[ \t\n] |
"NOT"[ \t\n]+"EXCEPTION"[ \t\n] {       
        count_lines (yytext);
        check_unreached = 0;
        return NOT_EXCEPTION;
        }

"ON"[ \t\n]+"EXCEPTION"[ \t\n] |
"EXCEPTION"[ \t\n] {    
        count_lines (yytext);
        check_unreached = 0;
        return EXCEPTION;
        }

"NOT"[ \t\n]+"ON"[ \t\n]+"OVERFLOW"[ \t\n] |
"NOT"[ \t\n]+"OVERFLOW"[ \t\n] {        
        count_lines (yytext);
        check_unreached = 0;
        return NOT_OVERFLOW;
        }

"NOT"[ \t\n]+"AT"[ \t\n]+"END"[ \t\n] |
"NOT"[ \t\n]+"END"[ \t\n] {     
        count_lines (yytext);
        check_unreached = 0;
        return NOT_END;
        }

"AT"[ \t\n]+"END"[ \t\n] {
        count_lines (yytext);
        check_unreached = 0;
        return END;
}

"ON"[ \t\n]+"OVERFLOW"[ \t\n] |
"OVERFLOW"[ \t\n] {     
        count_lines (yytext);
        check_unreached = 0;
        return COB_OVERFLOW;
        }

"NOT"[ \t\n]+"AT"[ \t\n]+"END-OF-PAGE"[ \t\n] |
"NOT"[ \t\n]+"AT"[ \t\n]+"EOP"[ \t\n] |
"NOT"[ \t\n]+"END-OF-PAGE"[ \t\n] |
"NOT"[ \t\n]+"EOP"[ \t\n] {     
        count_lines (yytext);
        check_unreached = 0;
        return NOT_EOP;
        }

"AT"[ \t\n]+"END-OF-PAGE"[ \t\n] |
"AT"[ \t\n]+"EOP"[ \t\n] |
"END-OF-PAGE"[ \t\n] |
"EOP"[ \t\n] {  
        count_lines (yytext);
        check_unreached = 0;
        return EOP;
        }

"NOT"[ \t\n]+"INVALID"[ \t\n]+"KEY"[ \t\n] {
        count_lines (yytext);
        check_unreached = 0;
        return NOT_INVALID_KEY;
        }

"NOT"[ \t\n]+"INVALID"[ \t\n] {
        count_lines (yytext);
        check_unreached = 0;
        return NOT_INVALID_KEY;
        }

"INVALID"[ \t\n]+"KEY"[ \t\n] {
        count_lines (yytext);
        check_unreached = 0;
        return INVALID_KEY;
        }

"INVALID"[ \t\n] {
        count_lines (yytext);
        check_unreached = 0;
        return INVALID_KEY;
        }

"UPON"[ \t\n]+"ENVIRONMENT-NAME" {
        count_lines (yytext);
        return UPON_ENVIRONMENT_NAME;
}

"UPON"[ \t\n]+"ENVIRONMENT-VALUE" {
        count_lines (yytext);
        return UPON_ENVIRONMENT_VALUE;
}

"UPON"[ \t\n]+"ARGUMENT-NUMBER" {
        count_lines (yytext);
        return UPON_ARGUMENT_NUMBER;
}

"UPON"[ \t\n]+"COMMAND-LINE" {
        count_lines (yytext);
        return UPON_COMMAND_LINE;
}

"ID"[ ]+"DIVISION"[ ]*"." |
"IDENTIFICATION"[ ]+"DIVISION"[ ]*"." {
        /* Ignore */
}

"SWITCH"[ ]+"0" {
        yylval = cb_build_reference ("SWITCH-0");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"1" {
        yylval = cb_build_reference ("SWITCH-1");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"2" {
        yylval = cb_build_reference ("SWITCH-2");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"3" {
        yylval = cb_build_reference ("SWITCH-3");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"4" {
        yylval = cb_build_reference ("SWITCH-4");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"5" {
        yylval = cb_build_reference ("SWITCH-5");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"6" {
        yylval = cb_build_reference ("SWITCH-6");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"7" {
        yylval = cb_build_reference ("SWITCH-7");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"8" {
        yylval = cb_build_reference ("SWITCH-8");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"9" {
        yylval = cb_build_reference ("SWITCH-9");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"10" {
        yylval = cb_build_reference ("SWITCH-10");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"11" {
        yylval = cb_build_reference ("SWITCH-11");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"12" {
        yylval = cb_build_reference ("SWITCH-12");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"13" {
        yylval = cb_build_reference ("SWITCH-13");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"14" {
        yylval = cb_build_reference ("SWITCH-14");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"15" {
        yylval = cb_build_reference ("SWITCH-15");
        SET_LOCATION (yylval);
        return WORD;
}

"SWITCH"[ ]+"16" {
        yylval = cb_build_reference ("SWITCH-16");
        SET_LOCATION (yylval);
        return WORD;
}


"."?[A-Z0-9\x80-\xff]([_A-Z0-9\x80-\xff-]*[A-Z0-9\x80-\xff]+)? {
        struct cb_word          *word;
        struct cb_level_78      *p78;
        struct cb_intrinsic_table       *cbp;
        cb_tree                 x;
        cb_tree                 y;
        cb_tree                 symb;
        size_t                  wordlen;
        int                     token;
        char                   *pname;
        char                   val[2];
        char                   *theword = yytext;
        char                   *subs;
        int                    user_const = 0;
        int                    is_dotword = 0;
    
        
        if (theword[0] == '.')
        {
           is_dotword = 1;
           theword++;
        }
        if (!is_dotword) {
           subs = cobc_find_constant(yytext,NULL, 1, NULL);
           if (subs) {
               theword = subs;
           }
           /* Check word length */
           wordlen = strlen (theword);
           /*CIT 
           if (wordlen > 31) {
                   cb_error (_("User defined name must be less than 32 characters"));
           }
           */

           if (!in_procedure && !cb_flag_ibm_mode)
           {
                   cbp = lookup_intrinsic (theword, 1);
                   if (cbp) {
                           if (strcasecmp (theword, "LOWER-CASE") == 0) {
                                   return LOWER_CASE_FUNC;
                           }
                           if (strcasecmp (theword, "UPPER-CASE") == 0) {
                                   return UPPER_CASE_FUNC;
                           }
                   }

           }
           /* Check FUNCTION name without keyword */
           if (in_procedure && ( functions_are_all || cb_flag_functions_all_intrinsic || cb_flag_mf_compat_parser)) {
                /*avoid conflict with COBOL verb LENGTH OF*/
                if (strcasecmp (theword, "LENGTH") == 0) {
                        return LENGTH;
                }
                cbp = lookup_intrinsic (theword, 1);
                if (cbp) {
                        yylval = cb_build_reference (theword);
                        SET_LOCATION (yylval);
                        if (strcasecmp (theword, "NATIONAL-OF") == 0) {
                                return NATIONAL_OF_FUNC;
                        }
                        if (strcasecmp (theword, "DISPLAY-OF") == 0) {
                                return DISPLAY_OF_FUNC;
                        }
                        if (strcasecmp (theword, "UPPER-CASE") == 0) {
                                return UPPER_CASE_FUNC;
                        }
                        if (strcasecmp (theword, "LOWER-CASE") == 0) {
                                return LOWER_CASE_FUNC;
                        }
                        if (functions_are_all || cb_flag_functions_all_intrinsic) {
                             if (strcasecmp (theword, "CONCATENATE") == 0) {
                                     return CONCATENATE_FUNC;
                             }
                             if (strcasecmp (theword, "CURRENT-DATE") == 0) {
                                     return CURRENT_DATE_FUNC;
                             }
                             if (strcasecmp (theword, "REVERSE") == 0) {
                                     return REVERSE_FUNC;
                             }
                             if (strcasecmp (theword, "SUBSTITUTE") == 0) {
                                     return SUBSTITUTE_FUNC;
                             }
                             if (strcasecmp (theword, "SUBSTITUTE-CASE") == 0) {
                                     return SUBSTITUTE_CASE_FUNC;
                             }
                             if (strcasecmp (theword, "TRIM") == 0) {
                                     return TRIM_FUNCTION;
                             }
                             if (cb_when_compiled_function_all)
                                 if (strcasecmp (theword, "WHEN-COMPILED") == 0) {
                                     return WHEN_COMPILED_FUNC;
                             }
                             if (strcasecmp (theword, "NUMVAL-C") == 0) {
                                     return NUMVALC_FUNC;
                             }
                             if (strcasecmp (theword, "LOCALE-DATE") == 0) {
                                     return LOCALE_DT_FUNC;
                             }
                             if (strcasecmp (theword, "LOCALE-TIME") == 0) {
                                     return LOCALE_DT_FUNC;
                             }
                             if (strcasecmp (theword, "LOCALE-TIME-FROM-SECONDS") == 0) {
                                     return LOCALE_DT_FUNC;
                             }
                             return FUNCTION_NAME;
                        }
                   }
           }

           /* Check reserved word */
           token = lookup_reserved_word (theword);
           if (token != 0) {
                   yylval = NULL;
                   return token;
           }
       
           for (p78 = lev78ptr; p78; p78 = p78->next) {
                   if (strcasecmp (theword, p78->fld78->name) == 0) {
                        if (non_const_word) {
                                if (cb_relaxed_syntax_check) {
                                   cb_warning (_("CONSTANT (78 level) may not be used here - '%s'"), theword);
                                   break;
                                } else {
                                   cb_error (_("CONSTANT (78 level) may not be used here - '%s'"), theword);
                                   yylval = cb_error_node;
                                   return WORD;
                                }

                        }
                        yylval = CB_VALUE (p78->fld78->values);
                        return LITERAL;
                   }
           }

           /* User word */
           if (cb_force_pid_literal) {
                   /* Force PROGRAM-ID / END PROGRAM */
                   cb_force_pid_literal = 0;
                   yylval = cb_build_alphanumeric_literal ((const unsigned char *)theword, wordlen, 0);
                   SET_LOCATION (yylval);
                   return PROGRAM_NAME;
           }
           for (symb = current_program->symbolic_list; symb; symb = CB_CHAIN(symb)) {
                   cb_tree v = CB_VALUE(symb);
                   if (v && CB_PAIR_P(v)) {
                       cb_tree px = CB_PAIR_X(v);
                       cb_tree py = CB_PAIR_Y(v);
                       if (px && py && 
                           CB_REFERENCE_P(px) && CB_LITERAL_P(py)) {
                           pname = (char*)CB_NAME(px);
                           if (strcasecmp (theword, pname) == 0) {
                               int v = cb_get_int(py);
                               val[0] = (v > 0 ? v -1 : 0);
                               val[1] = 0;
                               yylval = cb_build_alphanumeric_literal ((const unsigned char *)val, 1, 0);
                               SET_LOCATION (yylval);
                               return LITERAL;                            
                           }
                       }
                   }

           }
           
           /* find a user defined constant*/
           subs = cobc_find_constant(yytext,NULL, 0, &user_const);
           if (subs && user_const) {
               theword = subs;
               yylval = cb_build_alphanumeric_literal ((const unsigned char *)subs, strlen(subs), 0);
               SET_LOCATION (yylval);
               return LITERAL;                            
           }
        } else {
           wordlen = strlen (theword);
           if (cb_force_pid_literal) {
                /* Force PROGRAM-ID / END PROGRAM */
                cb_force_pid_literal = 0;
                yylval = cb_build_alphanumeric_literal ((const unsigned char *)theword, wordlen, 0);
                SET_LOCATION (yylval);
                return PROGRAM_NAME;
           }
        }
        if (is_dotword)
        {
           /* Check reserved word */
           token = lookup_reserved_word (theword);
           if (token) {
             {
               int i;
               for ( i = strlen (theword) - 1; i >= 0; --i )
                   unput( theword[i] );
             }

             last_token_is_dot = 1;
             yylval = NULL;
             return '.';
           }
            
        }

        yylval = cb_build_reference (theword);
        SET_LOCATION (yylval);

        /* Special name handling */
        word = CB_REFERENCE (yylval)->word;
        if (word->count > 0) {
                x = CB_VALUE (word->items);
                if (CB_SYSTEM_NAME_P (x)) {
                        return MNEMONIC_NAME;
                }
        } else {
                y = cb_global_ref(yylval);
                if (y && y != cb_error_node) {
                     if (CB_SYSTEM_NAME_P (y)) {
                              word->items = cb_list_add (word->items, y);
                              word->count++;
                              /*CB_REFERENCE (yylval)->values = y;*/
                              return MNEMONIC_NAME;
                      }
                }
        }
        /* Fix me - with the above rules this cannot happen
        if (theword[0] == '_' || theword[0] == '-') {
                cb_error (_("'_' or '-' is invalid as first character of user defined name"));
        }
        if (theword[wordlen - 1] == '_' || theword[wordlen - 1] == '-') {
                cb_error (_("'_' or '-' is invalid as last character of user defined name"));
        }
        */
        if (is_dotword)
            return DOTWORD;
        else 
            return WORD;
}

"<=" {
        yylval = NULL;
        return LE;
}

">=" {
        yylval = NULL;
        return GE;
}

"<>" {
        yylval = NULL;
        cb_check_feature_x (CB_TREE(current_statement), cb_syntax_ibm5_2, "Invalid <> , use NOT EQUAL");
        return NE;
}

"**" {
        yylval = NULL;
        return '^';
}

".." |
"." {
        cb_lex_source_file.column+= yyleng;
        last_token_is_dot = 1;
        yylval = NULL;
        if (strcmp(yytext,"..") == 0) 
            cb_check_feature_x (CB_TREE(current_statement), cb_syntax_ibm5_2, "Invalid .. at end of statement");
        return '.';
}

. {
        cb_lex_source_file.column++;
        yylval = NULL;
        return yytext[0];
}


<PICTURE_STATE>{
  "IS" {
        /* ignore */
  }
  \n   {
       cb_lex_source_file.line++;
       cb_final_source_line++;
       }
  [^ \t\n;]+    {
        BEGIN INITIAL;
        return scan_picture (yytext);
  }
}

<FUNCTION_STATE>{
  [a-z0-9-]+    {
        BEGIN INITIAL;
        yylval = cb_build_reference (yytext);
        SET_LOCATION (yylval);
        if (strcasecmp (yytext, "CONCATENATE") == 0) {
                return CONCATENATE_FUNC;
        }
        if (strcasecmp (yytext, "CURRENT-DATE") == 0) {
                return CURRENT_DATE_FUNC;
        }
        if (strcasecmp (yytext, "UPPER-CASE") == 0) {
                return UPPER_CASE_FUNC;
        }
        if (strcasecmp (yytext, "LOWER-CASE") == 0) {
                return LOWER_CASE_FUNC;
        }
        if (strcasecmp (yytext, "REVERSE") == 0) {
                return REVERSE_FUNC;
        }
        if (strcasecmp (yytext, "SUBSTITUTE") == 0) {
                return SUBSTITUTE_FUNC;
        }
        if (strcasecmp (yytext, "SUBSTITUTE-CASE") == 0) {
                return SUBSTITUTE_CASE_FUNC;
        }
        if (strcasecmp (yytext, "TRIM") == 0) {
                return TRIM_FUNCTION;
        }
        if (strcasecmp (yytext, "TRIML") == 0) {
                return TRIML_FUNCTION;
        }
        if (strcasecmp (yytext, "TRIMR") == 0) {
                return TRIMR_FUNCTION;
        }
        if (strcasecmp (yytext, "WHEN-COMPILED") == 0) {
                return WHEN_COMPILED_FUNC;
        }
        if (strcasecmp (yytext, "NUMVAL-C") == 0) {
                return NUMVALC_FUNC;
        }
        if (strcasecmp (yytext, "LOCALE-DATE") == 0) {
                return LOCALE_DT_FUNC;
        }
        if (strcasecmp (yytext, "LOCALE-TIME") == 0) {
                return LOCALE_DT_FUNC;
        }
        if (strcasecmp (yytext, "LOCALE-TIME-FROM-SECONDS") == 0) {
                return LOCALE_DT_FUNC;
        }
        if (strcasecmp (yytext, "NATIONAL-OF") == 0) {
                return NATIONAL_OF_FUNC;
        }
        if (strcasecmp (yytext, "DISPLAY-OF") == 0) {
                return DISPLAY_OF_FUNC;
        }
        return FUNCTION_NAME;
  }
  \n   {
       cb_lex_source_file.line++;
       cb_final_source_line++;
       }
  . {
        yylval = NULL;
        return yytext[0];
  }
}

^[ \t]*"EXEC"[ \t\n]+"SQL"[ \t\n] {   
                            count_lines (yytext);
                            BEGIN EXEC_STATE; 
                            dump_exec_file_entry(yytext) ;
                        }

^[ \t]*"EXEC"[ \t\n]+"CICS"[ \t\n] {   
                            count_lines (yytext);
                            BEGIN EXEC_STATE; 
                            dump_exec_file_entry(yytext) ;
                        }

^[ \t]*"EXEC"[ \t\n]+"DLI"[ \t\n]  {   
                            count_lines (yytext);
                            BEGIN EXEC_STATE; 
                            dump_exec_file_entry(yytext) ;
                        }

^[ \t]*"EXEC"[ \t\n] {   
                            count_lines (yytext);
                            exec_file = NULL;
                            BEGIN EXEC_STATE;  
                     }

<<EOF>> {
        last_token_is_dot = 0;
        integer_is_label = 0;
        inside_bracket = 0;
        inside_repository = 0;
        lev78ptr = NULL;
        yyterminate ();
}

%%

static int
read_literal (int mark, char mode)
{
        size_t          i = 0;
        int             c;

        if (!plexbuff) {
                plexbuff = cobc_malloc (COB_MINI_BUFF+2);
                plexsize = COB_MINI_BUFF;
        }

        while ((c = input ()) != EOF) {
                plexbuff[i++] = c;
                if ((c == '\n') || ((c == mark) && (c = input ()) != mark)) {
                        i--;
                        unput (c);
                        break;
                }
                if (i >= plexsize) {
                        plexsize *= 2;
                        plexbuff = cobc_realloc (plexbuff, plexsize +2);
                }
        }
        if (!i) {
                cb_warning (_("Alphanumeric literal has zero length"));
                cb_warning (_("A SPACE will be assumed"));
                i = 1;
                plexbuff[0] = ' ';
        }
        plexbuff[i] = 0;
        
        if ((mode == 'N' || mode == 'n') ||
            (mode == 'G' || mode == 'g')){
            if (i %2 == 1){
                cb_warning ("National literals must have a even numbers of bytes");
            }
            yylval = cb_build_alphanumeric_literal (plexbuff, i, 1);
            CB_LITERAL (yylval)->encoded = 1;
        } else 
        
        {
            if ((mode == 'Z' || mode == 'z')){
                plexbuff[i++] = 0;
            }

            yylval = cb_build_alphanumeric_literal (plexbuff, i, 0);

        }
        SET_LOCATION (yylval);
        return LITERAL;
}


static int 
scan_digit (char *text)
{
    while (*text) {
        if ((*text >= '0') && (*text <= '9')) {
            int i = *text -'0'; 
            text++;
            while (*text) {
                if ((*text >= '0') && (*text <= '9')) {
                    i *= 10;
                    i += *text -'0'; 
                } else {
                    return i;
                }
                text++;
            }
            return i;
        }
        text++;
    }
    return 0;
}

static int
scan_x (char *text, int national)
{
        unsigned char           *src;
        unsigned char           *dst;
        size_t                  currlen;
        int                     high = 1;
        int                     c;

        if (!plexbuff) {
                plexbuff = cobc_malloc (COB_MINI_BUFF);
                plexsize = COB_MINI_BUFF;
        }
        currlen = strlen (text);
        if (currlen > plexsize) {
                plexsize = currlen;
                plexbuff = cobc_realloc (plexbuff, plexsize);
        }
        dst = plexbuff;
        src = (unsigned char *)text;

        while (isalnum (*src)) {
                c = toupper (*src);
                if ('0' <= c && c <= '9') {
                        c = c - '0';
                } else if ('A' <= c && c <= 'F') {
                        c = c - 'A' + 10;
                } else {
                        goto error;
                }

                if (high) {
                        *dst = c << 4;
                } else {
                        *dst++ += c;
                }

                src++;
                high = 1 - high;
        }
        if (!high && (src -(unsigned char *)text) == 1) {
            high =1;
            *dst = *dst >> 4;
            ++dst;
        }
        if (high) {
                yylval = cb_build_alphanumeric_literal (plexbuff, dst - plexbuff, national);
                CB_LITERAL (yylval)->encoded = 1;
                SET_LOCATION (yylval);
                return LITERAL;
        }
        /* Fall through */
error:
        cb_error (_("Invalid X literal: %s"), text);
        yylval = cb_error_node;
        return LITERAL;
}

static int
scan_h (char *text)
{
        unsigned char   *p;
        unsigned long long       val = 0;
        int             c;
        char            buff[48];

        for (p = (unsigned char *)text; *p != '\'' && *p != '\"'; p++) {
                c = toupper (*p);
                if ('0' <= c && c <= '9') {
                        c = c - '0';
                } else if ('A' <= c && c <= 'F') {
                        c = c - 'A' + 10;
                } else {
                        goto error;
                }

                val = (val << 4) + c;
        }

#ifdef _WIN32
        sprintf (buff, "%I64u", val);
#else
        sprintf (buff, "%llu", val);
#endif
        yylval = cb_build_numeric_literal (0, (unsigned char *)buff, 0, text);
        SET_LOCATION (yylval);
        return LITERAL;

error:
        cb_error (_("Invalid H literal: %s"), text);
        yylval = cb_error_node;
        return LITERAL;
}

static int
scan_b (char *text)
{
        unsigned char   *p;
        unsigned long long       val = 0;
        int             c;
        char            buff[48];

        for (p = (unsigned char *)text; *p != '\'' && *p != '\"'; p++) {
                c = toupper (*p);
                if ('0' <= c && c <= '1') {
                        c = c - '0';
                } else {
                        goto error;
                }

                val = (val << 1) + c;
        }

#ifdef _WIN32
        sprintf (buff, "%I64u", val);
#else
        sprintf (buff, "%llu", val);
#endif
        yylval = cb_build_numeric_literal (0, (unsigned char *)buff, 0, text);
        SET_LOCATION (yylval);
        return LITERAL;

error:
        cb_error (_("Invalid B literal: %s"), text);
        yylval = cb_error_node;
        return LITERAL;
}

static int
scan_numeric (char *text)
{
        unsigned char   *s;
        int             sign;
        int             scale = 0;
        int             exp = 0;
        char            org[COB_SMALL_BUFF];

        strncpy(org, text, sizeof(org));
        /* get sign */
        sign = (*text == '+') ? 1 : (*text == '-') ? -1 : 0;
        if (sign) {
                text++;
        }
        /* check e+-999*/
        s = (unsigned char *)strchr (text, 'e');
        if (!s) s = (unsigned char *)strchr (text, 'E');
        if (s) {
                *s = 0;
                s++;
                if (!s) goto error;
                if (*s == '+')
                  s++;
                if (!s) goto error;
                exp = atoi((char*)s);
        }

        /* get decimal point */
        s = (unsigned char *)strchr (text, current_program->decimal_point);
        if (s) {
                scale = strlen ((char *)s) - 1;
                memmove (s, s + 1, (size_t)(scale + 1));
        }
        if (strchr (text, '.')) goto error;
        if (strchr (text, ',')) goto error;

        /* adjust e+-999*/
        if (exp) {
               scale -= exp;
        }

        yylval = cb_build_numeric_literal (sign, (unsigned char *)text, scale, org);
        SET_LOCATION (yylval);
        return LITERAL;

error:
        cb_error (_("Invalid numeric literal %s"), org);
        yylval = cb_error_node;
        return LITERAL;
}

static int
can_replace (const char *src1, const char *src2, const size_t size,
             const size_t iteration)
{
        const unsigned char     *p;

        if (strncasecmp (src1, src2, size)) {
                return 0;
        }
        p = (const unsigned char *)src1 + size;
        if (isalnum (*p) || *p == '-' || *p == '_') {
                return 0;
        }
        if (iteration) {
                p = (const unsigned char *)src1 - 1;
                if (isalnum (*p) || *p == '-' || *p == '_') {
                        return 0;
                }
        }
        return 1;
}

static int
scan_picture (char *text)
{
        unsigned char           *p;
        struct cb_level_78      *p78;
        size_t                  n;
        size_t                  i;
        size_t                  size;
        size_t                  sizep;
        char                    buff[COB_SMALL_BUFF];
        char                    buff2[COB_SMALL_BUFF];

        /* normalize the input */
        for (p = (unsigned char *)text; *p; p++) {
                /* unput trailing '.' or ',' */
                if (p[1] == 0 && (*p == '.' || *p == ',')) {
                        unput (*p);
                        *p = 0;
                        break;
                }
                /* upcase */
                *p = toupper (*p);
        }

        if (lev78ptr) {
                memset (buff, 0, sizeof (buff));
                memset (buff2, 0, sizeof (buff2));
                strcpy (buff, text);
                for (p78 = lev78ptr; p78; p78 = p78->next) {
                        if (p78->fld78->values == cb_error_node) {
                                strcpy (buff2, buff);
                                continue;
                        }
                        if (CB_VALUE(p78->fld78->values) == cb_error_node) {
                                strcpy (buff2, buff);
                                continue;
                        }
                        if (CB_LITERAL_P(CB_VALUE(p78->fld78->values))) {
                            p = (unsigned char *)CB_LITERAL(CB_VALUE(p78->fld78->values))->data;
                            size = strlen (p78->fld78->name);
                            sizep = strlen ((char *)p);
                            i = 0;
                            for (n = 0; n < strlen (buff); n++) {
                                    if (can_replace (&buff[n], p78->fld78->name, size, n)) {
                                            memcpy (&buff2[i], p, sizep);
                                            n += size - 1;
                                            i += sizep;
                                    } else {
                                            buff2[i++] = buff[n];
                                    }
                            }
                            buff2[i] = 0;
                            strcpy (buff, buff2);
                        } else {
                                strcpy (buff2, buff);
                                continue;
                        }
                }
                yylval = cb_build_picture (buff2);
        } else {
                yylval = cb_build_picture (text);
        }
        return PICTURE;
}

static void
count_lines (char *text)
{
        char *p;

        for (p = text; *p; p++) {
                if (*p == '\n') {
                        cb_lex_source_file.line++;
                        cb_final_source_line++;
                        cb_lex_source_file.column = 1;
                }
        }
}

void
cb_reset_in_procedure (void)
{
        in_procedure = 0;
}

void
cb_set_in_procedure (void)
{
        in_procedure = 1;
}

void
cb_reset_78 (void)
{
    lev78ptr = NULL;
}

struct cb_level_78      *
cb_add_78 (struct cb_field *f)
{
        struct cb_level_78      *p78;

        p78 = cobc_malloc (sizeof(struct cb_level_78));
        p78->fld78 = f;
        p78->next = lev78ptr;
        lev78ptr = p78;
        return p78;
}

struct cb_field *
check_level_78 (const char *name)
{
        struct cb_level_78      *p78;

        for (p78 = lev78ptr; p78; p78 = p78->next) {
                if (strcasecmp (name, p78->fld78->name) == 0) {
                        return p78->fld78;
                }
        }
        return NULL;
}

static int
is_numeric (const char *s) {
    int i = 0;
    if ( isdigit (s[i]) || (s[i] == '+') || (s[i] == '-')) {
        i++;
        for ( ; i < strlen(s); ++i ) {
            if ( !isdigit (s[i]) ) {
                return(0);
            }
        }
        return 1;
    } else 
        return 0;
}   

void
update_level_78 (const char *name, const char * value)
{
        struct cb_level_78      *p78;

        for (p78 = lev78ptr; p78; p78 = p78->next) {
                if (strcasecmp (name, p78->fld78->name) == 0) {
                        break;
                }
        }
        if (p78 == NULL) {
            cb_tree l = cb_build_reference (name);
            struct cb_field *f = CB_FIELD (cb_build_field (l));
            f->level = 01;
            f->flag_item_78 = 1;
            p78 =cb_add_78(f);            
        }
        if (is_numeric(value)){
            int sign = (*value == '+') ? 1 : (*value == '-') ? -1 : 0;
            p78->fld78->values = cb_list_init(cb_build_numeric_literal(sign, (unsigned char*)value,  0, NULL));    
        } else {
            p78->fld78->values = cb_list_init(cb_build_alphanumeric_literal((unsigned char*)value, strlen (value), 0));    
        }
        cb_validate_field(p78->fld78);
}

static void 
dump_exec_file_entry (char *text)
{
    char * section;
    char * paragraph;
    /* store current section */
    if (exec_file) {
        if(current_section)
        {
            section = (char*)current_section->name;
        }
        else
        {
            section = (char*)"MAIN SECTION";
        }
        
        /* store current paragraph */
        if(current_paragraph)
        {
            paragraph = (char*)current_paragraph->name;
        }
        else
        {
            paragraph = (char*)"MAIN PARAGRAPH";
        }
    
        fprintf(exec_file,"\n# %d \"%s\" \"%s\" \"%s\"\n",cb_lex_source_file.line, cb_lex_source_file.file_name, section, paragraph);
        fprintf(exec_file,"%s ", text);
    }

}

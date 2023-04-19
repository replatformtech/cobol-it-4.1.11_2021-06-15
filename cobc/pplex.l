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
%option prefix="pp"

%{
#include "cobc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ppparse.h"
#include "cit_types.h"
#include "cit_rtinterface.h"

extern char *strcasestr(const char *haystack, const char *needle);
extern int      is_reserved_word_excluded (const char *name);
static size_t   newline_count = 0;
static size_t   within_comment = 0;
static size_t   inside_bracket = 0;
static size_t   consecutive_quotation = 0;
static int      quotation_mark = 0;
static int      last_line_1 = -1;
static int      last_line_2 = -1;
static char *   pplast_cb_source_file = NULL;
static int      no_debug_out =0;
static int      cb_force_filename=1;


#define MF_CONDITIONAL_NONE              0
#define MF_CONDITIONAL_SKIPPING          1
#define MF_CONDITIONAL_SKIPPING_TO_END   2
#define MF_CONDITIONAL_INCLUDING         3
#define MF_CONDITIONAL_ERROR             4
static size_t   within_MF_conditional[100]  = {MF_CONDITIONAL_NONE};
static size_t   within_MF_conditional_stack = 0;


static struct cb_replace_list   *current_replace_list = NULL;

static struct cb_text_list      *text_queue = NULL;
static struct cb_text_list      *once_file = NULL;
       struct cb_text_list      *cb_once_list = NULL;

static struct copy_info {
        struct copy_info        *next;
        char                    *file;
        int                     line;
        int                     replacing;
        int                     quotation_mark;
        YY_BUFFER_STATE         buffer;
        char                   *copy_fname;
} *copy_stack = NULL;

#define YY_INPUT(buf,result,max_size) result = ppinput (buf, max_size);

static int ppinput (char *buf, int max_size);
static void ppecho (const char *text);
static void ppechodebug (const char *text);
static void pplinemark(void);
static void ppECHO(void);
static void ppECHO_LITTERAL(void);
static void start_exec_state(void);
static void ppflush_text_queue (void);

static void switch_to_buffer (int lineno, char *filename, YY_BUFFER_STATE buffer);

static int data_div_found = 0;
static void check_newline (void);
static void test_data_div (void);
static int inside_identification_div = 0;
static int need_debug = 0;
static int need_linemark = 0;
static int inside_procedure_div = 0;
static int inside_quote = 0;
static const char * get_cics_response_code(const char * resp);
static int inside_cobol_text = 0;

/* just for warning */
extern FILE *ppget_in  (void);
extern FILE *ppget_out  (void);
/* extern size_t ppget_leng  (void);*/
extern char *ppget_text  (void);
extern void ppset_in (FILE *  in_str );
extern void ppset_out (FILE *  out_str );
extern int ppget_debug  (void);
extern void ppset_debug (int  bdebug );
extern int pplex_destroy  (void);
extern int cb_pplinemark;
extern int cb_has_report_section;

/* list of CICS response code returns  IKI - Metrixware */
typedef struct cics_response {
	const char	*name;			/* response name */
	const char	*code;			/* response code */
} cics_response_t;

static cics_response_t cics_response_table[] = {
  {"NORMAL", "0"},         {"ERROR", "1"},          {"RDATT", "2"},          {"WRBRK", "3"},          {"EOF", "4"},
  {"EODS", "5"},           {"EOC", "6"},            {"INBFMH", "7"},         {"ENDINPT", "8"},        {"NONVAL", "9"},
  {"NOSTART", "10"},       {"TERMIDERR", "11"},     {"FILENOTFOUND", "12"},  {"NOTFND", "13"},        {"DUPREC", "14"},
  {"DUPKEY", "15"},        {"INVREQ", "16"},        {"IOERR", "17"},         {"NOSPACE", "18"},       {"NOTOPEN", "19"},
  {"ENDFILE", "20"},       {"ILLOGIC", "21"},       {"LENGERR", "22"},       {"QZERO", "23"},         {"SIGNAL", "24"},
  {"QBUSY", "25"},         {"ITEMERR", "26"},       {"PGMIDERR", "27"},      {"TRANSIDERR", "28"},    {"ENDDATA", "29"},
  {"INVTSREQ", "30"},      {"EXPIRED", "31"},       {"RETPAGE", "32"},       {"RTEFAIL", "33"},       {"RTESOME", "34"},
  {"TSIOERR", "35"},       {"MAPFAIL", "36"},       {"INVERRTERM", "37"},    {"INVMPSZ", "38"},       {"IGREQID", "39"},
  {"OVERFLOW", "40"},      {"INVLDC", "41"},        {"NOSTG", "42"},         {"JIDERR", "43"},        {"QIDERR", "44"},
  {"NOJBUFSP", "45"},      {"DSSTAT", "46"},        {"SELNERR", "47"},       {"FUNCERR", "48"},       {"UNEXPIN", "49"},
  {"NOPASSBKRD", "50"},    {"NOPASSBKWR", "51"},    {"SYSIDERR", "53"},      {"ISCINVREQ", "54"},     {"ENQBUSY", "55"},
  {"ENVDEFERR", "56"},     {"IGREQCD", "57"},       {"SESSIONERR", "58"},    {"SYSBUSY", "59"},       {"SESSBUSY", "60"},
  {"NOTALLOC", "61"},      {"CBIDERR", "62"},       {"INVEXITREQ", "63"},    {"INVPARTNSET", "64"},   {"INVPARTN", "65"},
  {"PARTNFAIL", "66"},     {"USERIDERR", "69"},     {"NOTAUTH", "70"},       {"SUPPRESSED", "72"},    {"RESIDERR", "75"},
  {"NOSPOOL", "80"},       {"TERMERR", "81"},       {"ROLLEDBACK", "82"},    {"END", "83"},           {"DISABLED", "84"},
  {"ALLOCERR", "85"},      {"STRELERR", "86"},      {"OPENERR", "87"},       {"SPOLBUSY", "88"},      {"SPOLERR", "89"},
  {"NODEIDERR", "90"},     {"TASKIDERR", "91"},     {"TCIDERR", "92"},       {"DSNNOTFOUND", "93"},   {"LOADING", "94"},
  {"MODELIDERR", "95"},    {"OUTDESCRERR", "96"},   {"PARTNERIDERR", "97"},  {"PROFILEIDERR", "98"},  {"NETNAMEIDERR", "99"},
  {"LOCKED", "100"},       {"RECORDBUSY", "101"},   {"UOWNOTFOUND", "102"},  {"UOWLNOTFOUND", "103"}, {"LINKABEND", "104"},
  {"CHANGED", "105"},      {"PROCESSBUSY", "106"},  {"ACTIVITYBUSY", "107"}, {"PROCESSERR", "108"},   {"ACTIVITYERR", "109"},
  {"CONTAINERERR", "110"}, {"EVENTERR", "111"},     {"TOKENERR", "112"},     {"NOTFINISHED", "113"},  {"POOLERR", "114"},
  {"TIMERERR", "115"},     {"SYMBOLERR", "116"},    {"TEMPLATERR", "117"},   {"NOTSUPERUSER", "118"}, {"CSDERR", "119"},
  {"DUPRES", "120"},       {"RESUNAVAIL", "121"},   {"CHANNELERR", "122"},   {"CCSIDERR", "123"},     {"TIMEDOUT", "124"},
  {"CODEPAGEERR", "125"},  {"INCOMPLETE", "126"},
  {NULL, NULL} /* NOTE: DO NOT REMOVE THIS LINE */
};

%}

WORD            [_0-9A-Z\x80-\xff-]*[_0-9A-Z\x80-\xff-]+
NUMRIC_LITERAL  [+-]?[0-9,.]*[0-9]
ALNUM_LITERAL   \"[^\"\n]*\"|\'[^\'\n]*\'
FNAME_WORD      [/\\:._0-9A-Z\x80-\xff-]*[_0-9A-Z\x80-\xff-]+

%option stack
%x PROCESS_STATE COPY_STATE PSEUDO_STATE LINKAGE_STATE EXEC_STATE REMARK_STATE INCLUDE_STATE CICS_RESPONSE_STATE EXEC_REPLACE_STATE

%%
%{
%}

<*>^"*PRAGMA"[ \t]+{WORD}+   {  ppECHO();  }
<*>^"*#PRAGMA"[ \t]+{WORD}+  {  ppECHO();  }


^[ \t]*("EJECT"|"SKIP1"|"SKIP2"|"SKIP3")[ \t]*\.?[ \t]*$    |
^[ \t]*"TITLE"[ \t]+.*       {  
                                 if (cb_flag_enable_listing_macro)
                                      ppecho ("**>"); 
                                    ppECHO();  
                             }

^[ \t]*"*CBL"[ \t]+"SOURCE"[ \t]*"."?  |
^[ \t]*"*CBL"[ \t]+"NOSOURCE"[ \t]*"."?  |
^[ \t]*"*CBL"[ \t]+"LIST"[ \t]*"."?  |
^[ \t]*"*CBL"[ \t]+"NOLIST"[ \t]*"."?  |
^[ \t]*"*CBL"[ \t]+"MAP"[ \t]*"."?  |
^[ \t]*"*CBL"[ \t]+"NOMAP"[ \t]*"."?  |
^[ \t]*"*CONTROL"[ \t]+"SOURCE"[ \t]*"."?  |
^[ \t]*"*CONTROL"[ \t]+"NOSOURCE"[ \t]*"."?  |
^[ \t]*"*CONTROL"[ \t]+"LIST"[ \t]*"."?  |
^[ \t]*"*CONTROL"[ \t]+"NOLIST"[ \t]*"."?  |
^[ \t]*"*CONTROL"[ \t]+"MAP"[ \t]*"."?  |
^[ \t]*"*CONTROL"[ \t]+"NOMAP"[ \t]*"."?  { ppecho (" "); }

"DFHRESP"[ \t]*"("  { if (cb_flag_syntax_only) {
                          BEGIN CICS_RESPONSE_STATE; 
                      }
                      else {
                          inside_bracket++;
                           ECHO;
                      }
                     }

<CICS_RESPONSE_STATE>{
  ")"    { BEGIN INITIAL; }
  {WORD} {
           const char * code = get_cics_response_code(yytext);
           if (code == NULL) /* BUG 4932 */
           {
             ppecho("DFHRESP(");
             ppecho(yytext);
             ppecho(")");
           }
           else
             ppecho(code);
         }
  .      { ECHO; }
  \n     { ECHO; } /* avoid warning: "-s option given but default rule can be matched" */
}


<*>"*>".*                 {   
                                if (inside_quote) {
                                    ppECHO();
                                } else {
                                   
                                  /*cb_lex_source_file.line++; ppecho ("\n"); */
                                }
                           }

^"**>".*                   {   ppECHO();
                               /*cb_lex_source_file.line++; 
                               if (cb_pplinemark) {
                                   if (cb_prepro_initial_pass){
                                        pplinemark();
                                   }
                               }
                               */
                           }

^[ \t]*"PROCESS"[ \t\n\r\.]+  {   
                                if (is_reserved_word_excluded("PROCESS")) {
                                    ppECHO();
                                } else {
                                    BEGIN PROCESS_STATE;
                                }
                                check_newline();
                            }

<PROCESS_STATE>{
  \n                        { BEGIN INITIAL; unput ('\n'); }
  .*                        { cb_warning (_("PROCESS statement is ignored")); }
}


"++COPY"[ \t\r]+ |
"COPY"[ \t\r]+  { 
  if (inside_quote)
      ppECHO();
  else { 
    if (cb_flag_keep_copy) ppECHO(); yy_push_state(COPY_STATE); return COPY; 
    }
}

^[ \t]*"SUM"?[ \t]*"OVERFLOW"[ \t]*.*"REPLACE".* { /*RW Stuff*/ppECHO(); }

"++REPLACE"[ \t\r]+ |
"REPLACE"[ \t\r]+  { 
  if (inside_quote)
      ppECHO();
  else {
    if (cb_flag_keep_copy) ppECHO(); yy_push_state(COPY_STATE); return REPLACE; 
    }
}

"++INCLUDE"[ \t\r]+ |
"INCLUDE"[ \t\r]+   {       
  if (inside_quote)
      ppECHO();
  else {
    if (cb_prepro_initial_pass ||   is_reserved_word_excluded("INCLUDE")) 
    {
        ppECHO();
    } else {
        if (cb_flag_keep_copy) ppECHO(); yy_push_state(COPY_STATE); return COPY; 
    }
  } 
}

<COPY_STATE>{
  [,;]?\n               {   ppECHO(); cb_lex_source_file.line++; }
  [,;]?[ \t]+             {   if (cb_flag_keep_copy) ppECHO(); /* ignore */ }
  ^"**>".*\n             {   cb_lex_source_file.line++; if (cb_flag_keep_copy) ppECHO(); else ppecho ("\n");/* ignore */ }
  "."                   {   if (cb_flag_keep_copy) ppECHO(); yy_pop_state(); return '.'; }
  "=="[ \t]*            {   if (cb_flag_keep_copy) ppECHO(); yy_push_state(PSEUDO_STATE); return EQEQ; }
  "("                   {   if (cb_flag_keep_copy) ppECHO(); return '('; }
  ")"                   {   if (cb_flag_keep_copy) ppECHO(); return ')'; }
  "ADD"                 {   if (cb_flag_keep_copy) ppECHO(); return ADD;}
  "BY"                  {   if (cb_flag_keep_copy) ppECHO(); return BY; }
  "IN"                  {   if (cb_flag_keep_copy) ppECHO(); return IN; }
  "OF"                  {   if (cb_flag_keep_copy) ppECHO(); return OF; }
  "OFF"                 {   if (cb_flag_keep_copy) ppECHO(); return OFF; }
  "SUPPRESS"            {   if (cb_flag_keep_copy) ppECHO(); return SUPPRESS; }
  "PRINTING"            {   if (cb_flag_keep_copy) ppECHO(); return PRINTING; }
  "REPLACING"           {   if (cb_flag_keep_copy) ppECHO(); return REPLACING; }
  "LEADING"             {   if (cb_flag_keep_copy) ppECHO(); return LEADING; }
  "TRAILING"            {   if (cb_flag_keep_copy) ppECHO(); return TRAILING; }
  {FNAME_WORD}     |
  {NUMRIC_LITERAL} |
  {ALNUM_LITERAL} |
  .                     {  if (cb_flag_keep_copy) ppECHO(); pplval.s = strdup (yytext); return TOKEN; }
}

<PSEUDO_STATE>{
  ^"**>".*\n             {   if (cb_flag_keep_copy) ppECHO(); else ppecho ("\n");cb_lex_source_file.line++;  /* ignore */ }
  [,;]?\n               {   ppECHO(); cb_lex_source_file.line++; }
  [,;]?[ \t]+             {   if (cb_flag_keep_copy) ppECHO(); pplval.s = strdup (" "); return TOKEN; }
  [ \t]*"=="            {   if (cb_flag_keep_copy) ppECHO(); yy_pop_state(); return EQEQ; }
  {WORD} |
  {NUMRIC_LITERAL} |
  {ALNUM_LITERAL} |
  .                     {   if (cb_flag_keep_copy) ppECHO(); pplval.s = strdup (yytext); return TOKEN; }
}

^"*##".*\n              {
                            cb_lex_source_file.line++;
                            pplinemark();
                        }
^[ \t]*"EXEC"[ \t\n\r]+"SQL"[ \t\n\r]+"INCLUDE"[ \t\n\r]+({FNAME_WORD}|{ALNUM_LITERAL})[ \t\n\r]+"END-EXEC"([ \t\n\r]*".")? {  
                           if (cb_flag_keep_copy) ppECHO(); 
                           else {
                              if (cb_flag_expand_include) {
                                  char *p = strdup(yytext);
                                  char *q;
                                  check_newline();
                                  q=strtok(p," \t\n");   /* skip EXEC*/
                                  q=strtok(NULL," \t\n"); /* skip SQL */
                                  q=strtok(NULL," \t\n"); /* skip INCLUDE */
                                  q=strtok(NULL," \t\n\"\'"); /* Get NAME */
                                  if (q){
                                    if (cb_flag_keep_copy) ppecho ("\n");
                                    ppcopy(q,NULL,NULL);
                                  }
                                  free(p);
                              } else {
                                  int needdot=0;
                                  char *last = &(yytext[strlen(yytext)-1]); 
                                  if (*last == '.' && inside_procedure_div) {
                                    *last = ' ';
                                    needdot = 1;
                                  }
                                  start_exec_state();
                                  if (inside_procedure_div ) {
                                     if (needdot)
                                        ppecho ("\n CONTINUE.\n");
                                     else 
                                        ppecho ("\n CONTINUE\n");
                                     if (!cb_flag_debug_exec) {
                                       need_debug = 1;
                                    }
                                  } 
                                  need_linemark = 1;
                                  yy_pop_state();
                              }
                           }
                        }


^[ \t]*"EXEC"[ \t\n\r]  {   
                            no_debug_out =1;
                            start_exec_state();
                        }

<EXEC_STATE>{
  "COPY"[ \t\r]+           {
                              if (cb_flag_expand_exec_copy) {
                                     if (cb_flag_keep_copy) ppECHO(); yy_push_state(COPY_STATE); return COPY; 

                              } else {
                                  ppECHO(); 
                              }
                           }
  "END-EXEC"([ \t\n\r]*".")?  {
                               int needdot=0;
                               char *last = &(yytext[strlen(yytext)-1]); 

                               no_debug_out =0;
                               if (*last == '.' && inside_procedure_div) {
                                 *last = ' ';
                                 needdot = 1;
                               }
                               check_newline();
                               ppECHO(); 
                               if (inside_procedure_div ) {
                                   if (needdot)
                                      ppecho ("\n CONTINUE.\n");
                                   else 
                                      ppecho ("\n CONTINUE\n");
                                   if (!cb_flag_debug_exec) {
                                     need_debug = 1;
                                   }
                               } 
                               need_linemark = 1;
                               yy_pop_state();
                        }
  \n                    {   ppECHO(); cb_lex_source_file.line++; }
  .                     {   
                            ppECHO(); 
                        } 
}

<EXEC_REPLACE_STATE>{
  "END-EXEC"([ \t\n\r]*".")?  {
                               int needdot=0;
                               char *last = &(yytext[strlen(yytext)-1]); 

                               no_debug_out =0;  
                               if (*last == '.' && inside_procedure_div) {
                                 *last = ' ';
                                 needdot = 1;
                               }
                               check_newline();
                               ppECHO(); 
                               if (inside_procedure_div ) {
                                   if (needdot)
                                      ppecho ("\n CONTINUE.\n");
                                   else 
                                      ppecho ("\n CONTINUE\n");
                                   if (!cb_flag_debug_exec) {
                                     need_debug = 1;
                                   }
                               } 
                               need_linemark = 1;
                               yy_pop_state();
                        }
  \n                    {   ppECHO(); cb_lex_source_file.line++; }
{ALNUM_LITERAL}         { ppECHO_LITTERAL(); }

{WORD} |
{NUMRIC_LITERAL} |
.                       { ppECHO(); }
}

    
<*>^[ \t]*"REPORT"[ \t\n\r]+"SECTION"[ \t\n\r\.]               { BEGIN INITIAL; test_data_div(); check_newline(); ppECHO(); cb_has_report_section = 1;}
    
<*>^[ \t]*"LINKAGE"[ \t\n\r]+"SECTION"[ \t\n\r\.]              { BEGIN INITIAL; test_data_div(); check_newline(); ppECHO(); return LINKAGE_SECTION;}
<*>^[ \t]*"WORKING-STORAGE"[ \t\n\r]+"SECTION"[ \t\n\r\.]      { BEGIN INITIAL; test_data_div(); check_newline(); ppECHO(); }
<*>^[ \t]*"LOCAL-STORAGE"[ \t\n\r]+"SECTION"[ \t\n\r\.]        { BEGIN INITIAL; test_data_div(); check_newline(); ppECHO(); }
<*>^[ \t]*"FILE"[ \t\n\r]+"SECTION"[ \t\n\r\.]                 { BEGIN INITIAL; test_data_div(); check_newline(); ppECHO(); }
<*>^[ \t]*"SCREEN"[ \t\n\r]+"SECTION"[ \t\n\r\.]               { BEGIN INITIAL; test_data_div(); check_newline(); ppECHO(); }
<*>^[ \t]*"DATA"[ \t\n\r]+"DIVISION"[ \t\n\r\.] { 
      BEGIN INITIAL; 
      check_newline();
      data_div_found = 1; 
      ppECHO(); 
      need_linemark = 1;
      inside_identification_div= 0; 
      inside_procedure_div=0;
      inside_cobol_text=1;
      }

<*>^[ \t]*"ENVIRONMENT"[ \t\n\r]+"DIVISION"[ \t\n\r\.] { 
   BEGIN INITIAL; 
   check_newline();
   ppECHO(); 
   need_linemark = 1;
   inside_identification_div= 0;
   inside_procedure_div=0;
   inside_cobol_text=1;
   }     

<*>^[ \t]*"PROCEDURE"[ \t\n\r]+"DIVISION"[ \t\n\r\.]  { 
   BEGIN INITIAL; 
   check_newline();
   ppECHO(); 
   need_linemark = 1;
   inside_identification_div= 0;
   inside_procedure_div=1;
   inside_cobol_text=1;
   cb_force_filename=1;
   }       

<*>^[ \t]*"ID"[ \t\n\r]+"DIVISION"[ \t\n\r\.]            {
   BEGIN INITIAL; 
   check_newline();
   ppecho ("IDENTIFICATION DIVISION.");                               
   need_linemark = 1;
   inside_identification_div= 1; 
   inside_procedure_div=0;
   inside_cobol_text=1;
   cb_force_filename=1;
}

<*>^[ \t]*"PROGRAM-ID"[ \t\n\r]*"."                    |
<*>^[ \t]*"IDENTIFICATION"[ \t\n\r]+"DIVISION"[ \t\n\r]*"." { 
   BEGIN INITIAL; 
   inside_cobol_text=1;
   check_newline();
   ppECHO(); 
   need_linemark = 1;
   inside_identification_div= 1; 
   inside_procedure_div=0;
   cb_force_filename=1;
   }       

^[ \t]*"AUTHOR" |
^[ \t]*"DATE-WRITTEN" |
^[ \t]*"DATE-MODIFIED" |
^[ \t]*"DATE-COMPILED" |
^[ \t]*"INSTALLATION" |
^[ \t]*"REMARKS" |
^[ \t]*"SECURITY" { 
                            if (inside_identification_div) {
                                ppechodebug ("**>"); ppECHO();  
                                check_newline();
                                need_linemark = 1;
                                BEGIN REMARK_STATE;
                            } else {
                                ppECHO();
                            }

                        }

<REMARK_STATE>{
  ^"COPY"[ \t\r]+   |      
  [ \t]+"COPY"[ \t\r]+  { yy_push_state(COPY_STATE); return COPY; }
  ^"**>".*               {   ppECHO(); }
  ^.                    {   ppechodebug ("**>");    ppECHO(); }
  \n                    {   cb_lex_source_file.line++; ppecho ("\n"); }
  .                     {   ppECHO (); }
}


\"|\'    {
      if (inside_quote) {
          if (inside_quote == yytext[0])
            inside_quote = 0;
      } else {
         inside_quote = yytext[0];
      }
      ppECHO ();
}

[,;]?\n     { 
               if (cb_prepro_initial_pass) {
                  ppECHO();
               } else {
                  ppecho ("\n"); 
               }
               cb_lex_source_file.line++; 
               if (cb_pplinemark) {
                   if (need_debug) {
                        ppechodebug ("*#PRAGMA \"DEBUGON\"\n");   
                   }
                   if (need_linemark || (cb_prepro_initial_pass)){
                        pplinemark();
                   }
               }
               need_debug = 0;
               need_linemark = 0;
               inside_quote = 0;
            }

^[ \t]+    { 
               ppECHO();
           }
[;]?[ \t]+ { 
               
               if (cb_prepro_initial_pass) {
                  ppECHO();
               } else {
                  yytext[0] = ' ';
                  /* Just erase the ;  keep format
                  ppecho (" "); 
                  */
                  ppECHO();
               }
            }

[,]?[ \t]+    {
               if (cb_prepro_initial_pass) {
                  ppECHO();
               } else {
                  if (inside_bracket) {
                     ppecho (", ");
                  } else {
                  yytext[0] = ' ';
                  /* Just erase the , keep format
                  ppecho (" "); 
                  */
                   ppECHO();
                  }
               }
            }  

"("         {
               inside_bracket++;
               ppecho ("(");
            }

")"         {
               if (inside_bracket) {
                  inside_bracket--;
               }
               ppecho (")");
            }

{ALNUM_LITERAL}  { ppECHO_LITTERAL(); }
{WORD} |
{NUMRIC_LITERAL} |
.           { ppECHO(); }

<<EOF>> {
        struct copy_info *p;
        char buffer[4096];

        p = copy_stack;

        yy_delete_buffer (YY_CURRENT_BUFFER);

        /* Terminate at the end of all input */
        if (p->next == NULL) {
                within_comment = 0;
                within_MF_conditional_stack = 0;
                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_NONE;
                newline_count = 0;
                inside_bracket = 0;
                current_replace_list = NULL;
                text_queue = NULL;
                once_file = NULL;
                copy_stack = NULL;
                quotation_mark = 0;
                consecutive_quotation = 0;
                last_line_1 = -1;
                last_line_2 = -1;
                data_div_found = 0;
                inside_identification_div = 0;
                inside_cobol_text=0;
                need_debug = 0;
                need_linemark = 0;
                inside_procedure_div = 0;
                pplast_cb_source_file = NULL;
                if (cb_pplinemark) {
                    if (!cb_flag_debug_exec){
                        ppechodebug ("*#PRAGMA \"DEBUGOFF\"\n");                               
                    }
                    ppechodebug("*##\n");
                }
                yyterminate ();
        }

        /* Close the current file */
        fclose (ppin);

        /* Switch to the last buffer */
        if (p->replacing) {
                pp_set_replace_list (NULL, 0, 0);
        }
        ppflush_text_queue ();
        if (cb_flag_copy_mark && p->copy_fname) {
            sprintf (buffer, "**>--SCCOPY %s\n", p->copy_fname);
            ppechodebug(buffer);
        }
        switch_to_buffer (p->line, p->file, p->buffer);
        quotation_mark = p->quotation_mark;
        copy_stack = p->next;
        if (p->copy_fname)
          free (p->copy_fname);
        free (p);
}

%%
static void
pplinemark(void) {
    char buffer[4096];
    char outbuffer[4096];
    if (cb_lex_source_file.line < 2 && !cb_flag_copy_mark){
        sprintf (buffer, "**>\"%s\"\n", cb_lex_source_file.file_name);
        ppechodebug(buffer);
    }
    if (cb_pplinemark) {
        if (!current_replace_list) 
        {
            if (((void*)cb_lex_source_file.file_name == (void*)pplast_cb_source_file) && ! cb_force_filename)
               sprintf (buffer, "*#DEBUG%d H%d\n", cb_lex_source_file.line, cb_lex_source_file.file_name_hash);
            else
               sprintf (buffer, "*#DEBUG%d H%d \"%s\"\n", cb_lex_source_file.line, cb_lex_source_file.file_name_hash, cb_lex_source_file.file_name);
            cb_force_filename=0;
            pplast_cb_source_file = (char*)cb_lex_source_file.file_name;
            if (cb_flag_split_debug_mark && (strlen(buffer) > 60))
            {
               char *p;
               int inc = 58;
               strcpy (outbuffer, buffer);
               p = buffer ;
               while(strlen(outbuffer) > 60) {
                  outbuffer[58] = '"';
                  outbuffer[59] = '\n';
                  outbuffer[60] = 0;
                  ppechodebug(outbuffer);     
                  p += inc;             
                  strcpy (outbuffer, "*#DEBUG-CONT\"");
                  inc = 58-strlen (outbuffer);
                  strcat(outbuffer, p);  
               }
               ppechodebug(outbuffer);
            }
            else 
            {
               ppechodebug(buffer);
            }
        }

    }
}


static void
check_newline (void)
{
    int i = ppleng;
    char * text = pptext;
    while (*text && i) {
        if (*text == '\n')
            cb_lex_source_file.line++;
        text++;
        i--;
    }
}


static void
test_data_div (void)
{
        if (!data_div_found)
        {
        /*        ppecho ("DATA DIVISION.\n");*/
                pplinemark();
                data_div_found =1;
        }
}

/*
static void
test_identification_div (void)
{                       
        if (!inside_identification_div)
        {
                cb_error (_("Invalid directive - Comment may only be found in INDENTIFICATION DIVISION"));
        }
}
*/
void
pp_set_replace_list (struct cb_replace_list *list, int add_list, int global)
{
    struct cb_replace_list   *p;
    for (p = list; p ; p = p->next) {
        p->global = global;
    }
    if (!add_list) {
       for (p = current_replace_list; p ; p = p->next) {
           while (p->next && (p->next->global == global))
              p->next = p->next->next;
       }
      if (current_replace_list && (current_replace_list->global == global)){
         current_replace_list = current_replace_list->next;
       } 
    } 
    if (current_replace_list) {
       for (p = current_replace_list; p->next; p = p->next) {
           /* just goto the end of the list*/
       }
       p->next = list;
    } else {
       current_replace_list = list;
    }
}


static void
switch_to_buffer (int line, char *file, YY_BUFFER_STATE buffer)
{
        unsigned char    *p;
        unsigned char    *f;

        cb_lex_source_file.line = line;
        f = (unsigned char *)strdup (file);
        for (p = f; *p; p++) {
                if (*p == '\\') {
                        *p = '/';
                }
        }
        cobc_set_lex_file_name((unsigned char *)f);
        yy_switch_to_buffer (buffer);
        pplinemark();
}

int
ppopen (char *name, struct cb_replace_list *replace_list)
{
        struct copy_info *p;
        struct cb_text_list *l;

        for (; newline_count > 0; newline_count--) {
                ungetc ('\n', ppin);
        }

        for (l=once_file; l; l = l->next) {
            if (strcmp(l->text, name) == 0) {
                /* do no open this file*/
                return 0; 
            }
        }
        /* Open the copy file */
        ppin = fopen (name, "rb");
        if (!ppin) {
                if (cb_lex_source_file.file_name) {
                        cb_error (_("%s: %s"), name, strerror (errno));
                } else {
                        perror (name);
                }
                return -1;
        }

        /* Add to dependency list */
        if (cb_depend_file) {
                cb_depend_list = cb_text_list_add (cb_depend_list, name);
        }

        /* Preserve the current buffer */
        p = cobc_malloc (sizeof (struct copy_info));
        p->line = cb_lex_source_file.line;
        p->file = cb_lex_source_file.file_name;
        p->copy_fname = ( char*)strdup(name);
        p->replacing = replace_list ? 1 : 0;
        p->buffer = YY_CURRENT_BUFFER;
        p->quotation_mark = quotation_mark;
        p->next = copy_stack;
        copy_stack = p;

        /* Switch to new buffer */
        if (replace_list) {
                pp_set_replace_list (replace_list, 0, 0);
        }
        switch_to_buffer (1, name, yy_create_buffer (ppin, YY_BUF_SIZE));
        if (replace_list) {
           ppechodebug("*#DEBUG-SWITCH GUBED\n");
        }

        if (cb_flag_copy_mark && p->next) {
            char buffer[4096];
            sprintf (buffer, "**>++SCCOPY %s\n", p->copy_fname);
            ppechodebug(buffer);
            pplinemark();
        }
        return 0;
}


int
ppcopy (char *iname, const char *lib, struct cb_replace_list *replace_list)
{
        struct cb_text_list     *l;
        struct cb_inc_list      *il;
        struct cb_text_list     *el;
        char                    *s;
        char                    name[COB_SMALL_BUFF];
        char                    buff1[COB_SMALL_BUFF];
        char                    buff2[COB_SMALL_BUFF];
        char                    given_name[COB_SMALL_BUFF];
        int                     ret=0;

        cobc_filename_replace_env(iname,name, COB_SMALL_BUFF, 0);
        memset (buff1, 0, sizeof(buff1));
        memset (buff2, 0, sizeof(buff2));
        if (lib) {
                sprintf (buff1, "%s/%s", lib, name);
                s = buff1;
        } else {
                s = name;
        }
        strcpy(given_name, s);

        /* Find the file */
        if (!cobc_exist_file (s)) {
                if (cb_flag_curdir_include) {
                    for (el = cb_extension_list; el; el = el->next) {
                            sprintf (buff2, "%s%s", s, el->text);
                            if (cobc_exist_file (buff2)) {
                                    s = buff2;
                                    ret= ppopen (buff2, replace_list);
                                    goto store_once;
                            }
                    }
                }
                if (*s != '/') {
                        for (il = cb_include_list; il; il = il->next) {
                                if (!has_COPY_lib_defined ||
                                    (!lib && !il->lib) ||
                                    (!lib && il->lib &&  (strcasecmp("SYSLIB", il->lib) == 0)) ||
                                    (lib  && !il->lib && (strcasecmp("SYSLIB", lib) == 0) )    ||
                                    (lib  && il->lib && (strcasecmp(il->lib, lib) == 0) )) {
                                        if (il->extlist)
                                            el = il->extlist;
                                        else 
                                            el = cb_extension_list;
                                        for (; el; el = el->next) {
                                                char * ext = (char*)el->text;
                                                if (ext == NULL) ext = (char*)"";
                                                sprintf (buff2, "%s/%s%s", il->path, name, ext);
                                                if (cobc_exist_file (buff2)) {
                                                        s = buff2;
                                                        ret = ppopen (buff2, replace_list);
                                                        goto store_once;
                                                }
                                        }
                                }
                        }
                }
                cb_error (_("%s: %s"), name, strerror (errno));
                return -1;
        }

        ret = ppopen (s, replace_list);
store_once:
        for (l=cb_once_list; l; l = l->next) {
            int ln = strlen(given_name);
            int lt = strlen(l->text) ;
            if ( ln >= lt) {
                if (strcmp(l->text, given_name+(ln -lt)) == 0) {
                    /* do no open this file*/
                    once_file = cb_text_list_add(once_file, s);
                    break; 
                }
            }
        }
        return ret;
}

/* Check directives */
/* This is horrible but we have to parse directives directly after the read */
/* as flex buffers up input and it is then too late to use the flex parser */
#undef DEBUGMF
/*#define DEBUGMF 1*/

#define TOK_SEP "\" =\t\n\r()"
static int 
extract_condition (char *key, char *value)
{
        char *p;
        int res = 1;
        p = strtok(NULL, TOK_SEP);
        if (p && strcasecmp(p, "not") == 0) {
           res = 0;
           p = strtok(NULL, TOK_SEP);
        }
        if (p)
        {                
                
                strcpy (key, p);
                again:
                p = strtok(NULL, TOK_SEP);
                strcpy (value, "");
                if (p) {
                        if (strcasecmp(p, "not") == 0) {
                           res = 0;
                           goto again;
                        }
                        if (strcasecmp(p, "unset") == 0){
                                return !res;
                        } else if (strcasecmp(p, "set") == 0){
                                return res;
                        } else if (strcasecmp(p, "defined") == 0){
                            return res;
                        }
                        strcpy (value, p);
                }
                return res;
        } else {
                *key =0;
                strcpy (value, "");
                return 0;
        }

}

static void 
update_constant (char *key, char *value) {
    struct cb_constant_list *l;

    l = cb_constants;
    while (l && key) {
       if (strcasecmp(l->key, key) == 0) {
          l->value = strdup(value);
          #ifdef DEBUGMF
          printf("update const %s = %s\n", key, value);
          #endif
          l->is_userconst = 1;
          return;
       }
       l = l->next;
    }
}

static int
check_MF_directive (char *buff, int line_size, int *is_condition)
{
        
        char    *p = buff;
        char    *copy = NULL;
        int     i;
        int     res = 0;
        char key[255];
        char value[255];
        char constant[255];
        int including;

        if (line_size < 4)
                return 0;
                
        for (i = 0; i < line_size ; i++) {
                if (*buff == '$') {
                        res =1;
                        break;
                } else if ((*buff != ' ') && (*buff != '\t')) {
                          return 0;
                }
                buff++;
        }
        if (!res)
                return 0;
        *is_condition =0;
        copy = strdup(buff);
        p = strtok(copy, TOK_SEP);

        including = (within_MF_conditional[within_MF_conditional_stack] != MF_CONDITIONAL_SKIPPING) && 
                    (within_MF_conditional[within_MF_conditional_stack] != MF_CONDITIONAL_SKIPPING_TO_END);

        if (strcasecmp(p, "$set") == 0) {
              if (including ) {
                p = strtok(NULL, TOK_SEP);
                while (p ) {
                        if (strcasecmp(p, "sourceformat") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                if (p  && (strcasecmp(p, "free") == 0)) {
                                        cb_source_format = CB_FORMAT_FREE;                                        
                                        cb_warning (_("MF directive - source format free"));
                                }
                                if (p  && (strcasecmp(p, "fixed") == 0)) {
                                        cb_source_format = CB_FORMAT_FIXED;                                        
                                        cb_warning (_("MF directive - source format fixed"));
                                }
                        } else if (strcasecmp(p, "PERFORM-TYPE") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                if (p  && (strcasecmp(p, "OSVS") == 0)) {
                                        cb_perform_osvs = 1;                                        
                                        cb_warning (_("MF directive - perform-type osvs"));
                                } else {
                                        cb_flag_sequential_line = 0;                                        
                                        cb_warning (_("MF directive - sequential record"));
                                }
                        } else if (strcasecmp(p, "sequential") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                if (p  && (strcasecmp(p, "line") == 0)) {
                                        cb_flag_sequential_line = 1;                                        
                                        cb_warning (_("MF directive - sequential line"));
                                } else {
                                        cb_flag_sequential_line = 0;                                        
                                        cb_warning (_("MF directive - sequential record"));
                                }
                        } else if (strcasecmp(p, "recmode") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                if (p  && (strcasecmp(p, "osvs") == 0)) {
                                        cb_flag_recmode_osvs = 1;                                        
                                        cb_warning (_("MF directive - recmode osvs"));
                                 } else if (p && (strcasecmp(p, "v") == 0)) {
                                        cb_flag_recmode_v = 1;                                        
                                        cb_warning (_("MF directive - recmode V"));
                                 } else if (p && (strcasecmp(p, "f") == 0)) {
                                        cb_flag_recmode_v = 0;                                        
                                        cb_warning (_("MF directive - recmode F"));
                                 } else {
                                        cb_flag_recmode_osvs = 0;                                        
                                        cb_warning (_("MF directive - recmode UNSUPPORTED"));
                                }
                        } else if (strcasecmp(p, "sticky-linkage") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                if (p ) {
                                        i= atoi(p);
                                        cb_sticky_linkage = i;                                        
                                        cb_warning (_("MF directive - sticky-linkage %d"), i);
                                }
                        } else if (strcasecmp(p, "INITCALL") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                while (p ) {
                                        cb_initcall_list = cb_text_list_add(cb_initcall_list, p);                
                                        p = strtok(NULL, TOK_SEP);
                                }
                        } else if (strcasecmp(p, "LS-MODE") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                if (p  && (strcasecmp(p, "off") == 0)) {
                                        cb_flag_line_seq_recording_mode = 0;                                        
                                } else {
                                   cb_flag_line_seq_recording_mode = 1;
                                }
                        } else if (strcasecmp(p, "NOINITCALL") == 0) {
                             cb_initcall_list = NULL;
                        } else if (strcasecmp(p, "DEFAULTCALL") == 0) {
                        } else if (strcasecmp(p, "LSUTF16") == 0) {
                        } else if (strcasecmp(p, "NOLSUTF16") == 0) {
                        } else if (strcasecmp(p, "DATACOMPRESS") == 0) {
                        } else if (strcasecmp(p, "NODATACOMPRESS") == 0) {
                        } else if (strcasecmp(p, "KEYCOMPRESS") == 0) {
                        } else if (strcasecmp(p, "NOKEYCOMPRESS") == 0) {
                        } else if (strcasecmp(p, "lineseqmf") == 0) {
                        } else if (strcasecmp(p, "NOlineseqmf") == 0) {
                        } else if (strcasecmp(p, "lineseqdos") == 0) {
                        } else if (strcasecmp(p, "NOlineseqdos") == 0) {
                        } else if (strcasecmp(p, "INSERTNULL") == 0) {
                        } else if (strcasecmp(p, "NOINSERTNULL") == 0) {
                        } else if (strcasecmp(p, "spzero") == 0) {
                                cb_flag_mf_spzero = 1;
                                cb_warning (_("MF directive - spzero"));
                        } else if (strcasecmp(p, "odoslide") == 0) {
                                cb_flag_odo_slide = 1;
                                cb_warning (_("MF directive - odoslide"));
                        } else if (strcasecmp(p, "trunc") == 0) {
                                cb_binary_truncate = 1;
                                cb_warning (_("MF directive - trunc"));
                        } else if (strcasecmp(p, "notrunc") == 0) {
                                cb_binary_truncate = 0;
                                cb_warning (_("MF directive - notrunc"));
                        } else if (strcasecmp(p, "cics") == 0) {
                                cb_flag_cics = 1;
                                cb_warning (_("MF directive - cics"));
                        } else if (strcasecmp(p, "defaultbyte") == 0) {
                                p = strtok(NULL, TOK_SEP);
                                if (p ) {
                                        i= atoi(p);
                                        cb_use_defaultbyte = 1;      
                                        cb_defaultbyte = i;                                  
                                        cb_warning (_("MF directive - defaultbyte %d"), i);
                                }
                        } else if (strcasecmp(p, "constant") == 0) {
                                p += strlen("constant") +1;
                                p = cobc_add_constant(p, 0, 1);
                        } else if (cobc_find_constant(p, constant, 0, NULL)) {
                            strncpy (key, p, sizeof(key));
                            p = strtok(NULL, TOK_SEP);
                            if (p) {
                                update_constant(key,p);
                            } else {
                                update_constant(key,(char*)"");
                            }
                        }
                        if (p)
                                p = strtok(NULL, TOK_SEP);

                }                   
              }
        } else if (strcasecmp(p, "$if") == 0) {
                int val = 0;
                *is_condition =1;
                #ifdef DEBUGMF
                printf("condition %s\n", buff);
                #endif
                if ((within_MF_conditional[within_MF_conditional_stack] != MF_CONDITIONAL_NONE) && 
                    (within_MF_conditional_stack < 99)){
                    within_MF_conditional_stack ++;
                    if (within_MF_conditional[within_MF_conditional_stack-1] == MF_CONDITIONAL_INCLUDING) {
                        within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_NONE;
                    } else {
                        within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING_TO_END;
                    }
                }                        
                if (within_MF_conditional[within_MF_conditional_stack] != MF_CONDITIONAL_SKIPPING_TO_END) {
                    do {
                            val = extract_condition(key, value);
                            if (key[0]) {
                                #ifdef DEBUGMF
                                printf("key %s , value = %s val =%d \n ", key,value, val);
                                #endif
                                if (strcasecmp(key, "p64") == 0) {
                                        if (((sizeof(void*) > 4) && (val == 1)) || 
                                            ((sizeof(void*) <= 4) && (val == 0))) {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                                        } else {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING;
                                        }
                                }else if ((strcasecmp(key, "IBM-1140") == 0) ||
                                          (strcasecmp(key, "IBM-1147") == 0) ||
                                          (strcasecmp(key, "EBCDIC-CHARSET") == 0)) {
                                        if (((CHAR_SP == 0x40) && (val == 1)) || 
                                            ((CHAR_SP != 0x40) && (val == 0))) {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                                        } else {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING;
                                        }
                                } else if (strcasecmp(key, "CIT") == 0) {
                                        if (val == 0)
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING;
                                        else 
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                                } else if (strcasecmp(key, "debug") == 0) {
                                          if (((cb_flag_source_location || cb_flag_debugging_line) && (val == 1)) ||
                                              (!(cb_flag_source_location || cb_flag_debugging_line) && (val == 0)))
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                                        else
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING;
                               
                                } else if (cobc_find_constant(key, constant, 0, NULL)) {
                                         if (strlen(value) == 0) {
                                             strcpy (value, constant);
                                         }
                                         if ((strcasecmp(constant, value) == 0 && val ) || 
                                             (strcasecmp(constant, value) != 0 && !val)) {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                                         } else {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING;
                                         }
                                } else  {
                                         if (!val ) {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                                         } else {
                                                within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING;
                                         }
                                }
                            }
                    } while (key[0] )   ;               
                }
              
        } else if (strcasecmp(p, "$else") == 0) {
                *is_condition =1;
                if (within_MF_conditional[within_MF_conditional_stack] != MF_CONDITIONAL_SKIPPING_TO_END) {
                      if (within_MF_conditional[within_MF_conditional_stack] == MF_CONDITIONAL_SKIPPING)
                            within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                      else if (within_MF_conditional[within_MF_conditional_stack] == MF_CONDITIONAL_INCLUDING)
                            within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_SKIPPING;
                      else if (within_MF_conditional[within_MF_conditional_stack] == MF_CONDITIONAL_ERROR) {
                            within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_INCLUDING;
                            cb_warning (_("MF directive - $else with unsupported $if : code included"));
                      } else {
                            cb_warning (_("MF directive - $else without $if"));
                      }
                }
        } else if (strcasecmp(p, "$end") == 0) {
                  *is_condition =1;
                  if (within_MF_conditional[within_MF_conditional_stack] == MF_CONDITIONAL_NONE){
                        cb_warning (_("MF directive - $end without $if"));
                  }
                  within_MF_conditional[within_MF_conditional_stack] = MF_CONDITIONAL_NONE;
                  if (within_MF_conditional_stack > 0 )
                    within_MF_conditional_stack --;
        } else {
                cb_warning (_("Unsupported MF directive '%s' - ignored"), p);
        }
        if(copy)
            free(copy);
        return res;
        
}

static void
check_directive (char *buff, int *line_size)
{
        char    *s;
        char    *dirptr;
        size_t  cnt;
        int     n;
        char    sbuff[5][256];

        if (cb_source_format == CB_FORMAT_FIXED) {
                if (*line_size < 8) {
                        return;
                }
                if (buff[6] != ' ') {
                        return;
                }
                s = &buff[7];
        } else {
                s = buff;
        }
        for ( ; *s == ' '; s++) {
                ;
        }
        dirptr = s;
        if (*s != '>') {
                return;
        }
        s++;
        if (*s != '>') {
                return;
        }
        s++;
        if (*s == 'D') {
                if (cb_flag_debugging_line) {
                        memset (dirptr, ' ', 3);
                } else {
                        for (cnt = 0; cnt < newline_count; cnt++) {
                                buff[cnt] = '\n';
                        }
                        buff[cnt] = 0;
                        strcat (buff, "      *> DEBUG\n");
                        *line_size = strlen (buff);
                        newline_count = 0;
                }
                return;
        }
        memset (sbuff[0], 0, sizeof (sbuff));
        n = sscanf (s, "%255s %255s %255s %255s %255s",
                        sbuff[0], sbuff[1], sbuff[2], sbuff[3], sbuff[4]);
        for (cnt = 0; cnt < newline_count; cnt++) {
                buff[cnt] = '\n';
        }
        buff[cnt] = 0;
        newline_count = 0;
        strcat (buff, "      *> DIRECTIVE\n");
        *line_size = strlen (buff);

        if (n >= 2 && !strcasecmp (sbuff[0], "SOURCE")) {
            switch (n) {
            case 2:
                    if (!strcasecmp (sbuff[1], "FIXED")) {
                            cb_source_format = CB_FORMAT_FIXED;
                            return;
                    }
                    if (!strcasecmp (sbuff[1], "FREE")) {
                            cb_source_format = CB_FORMAT_FREE;
                            return;
                    }
                    break;
            case 3:
                    if (strcasecmp (sbuff[1], "FORMAT") &&
                        strcasecmp (sbuff[1], "IS")) {
                            break;
                    }
                    if (!strcasecmp (sbuff[2], "FIXED")) {
                            cb_source_format = CB_FORMAT_FIXED;
                            return;
                    }
                    if (!strcasecmp (sbuff[2], "FREE")) {
                            cb_source_format = CB_FORMAT_FREE;
                            return;
                    }
                    break;
            default:
                    if (strcasecmp (sbuff[1], "FORMAT")) {
                            break;
                    }
                    if (strcasecmp (sbuff[2], "IS")) {
                            break;
                    }
                    if (!strcasecmp (sbuff[3], "FIXED")) {
                            cb_source_format = CB_FORMAT_FIXED;
                            return;
                    }
                    if (!strcasecmp (sbuff[3], "FREE")) {
                            cb_source_format = CB_FORMAT_FREE;
                            return;
                    }
                    break;
            }
        } else if (n == 1 && !strcasecmp (sbuff[0], "ONCE")) {
            if (copy_stack && copy_stack->copy_fname){
              once_file = cb_text_list_add(once_file, copy_stack->copy_fname);
            }
            return;
        }

        cb_warning (_("Invalid directive - ignored"));
}

/*
 * Read line
 */

static int is_comment_char(unsigned char c) {
return (c == '*' || c  == '/');
}

static int
ppinput (char *buff, int max_size)
{

        char    *bp;
        unsigned char *p;
        size_t  gotcr;
        size_t  continuation = 0;
        int     ipchar;
        int     i;
        int     n;
        int     coln;
        int     lastcolumn; 
        int     tabcnt;
        int     next_newline_count=0;
        int     is_condition;
        int     in_line_comment =0;

start:
        if (!ppin)
            return 0;
        /* read a line */
        gotcr = 0;
        ipchar = 0;
        tabcnt = 0;
        inside_quote = 0;
        if (cb_first_tab_width == 0)
            cb_first_tab_width = cb_tab_width;
        for (n = 0; n < max_size && ipchar != '\n'; ) {
                ipchar = getc (ppin);
                if (ipchar == 0 && cb_flag_mf_ctrl_escaped) {
                    ipchar = getc (ppin);
                    if (ipchar  != EOF) {
                        buff[n++] = ipchar;
                        continue;
                    }
                }
                if (ipchar == EOF) {                        
                        if (n == 0) {
                              if (newline_count) {
                                    n = newline_count;
                                    newline_count = 0;
                                    memset (buff, '\n', n);
                                    buff[n] = 0;
                                    return n;
                              } else {
                                    return 0; 
                              }
                        }
                        buff[n++] = '\n';
                        buff[n] = 0;
                        break;
                        /*
                        if (newline_count) {
                           memmove (buff + newline_count, buff, strlen (buff) + 1);
                           memset (buff, '\n', newline_count);
                        }
                        newline_count = 0;
                        return strlen (buff);
                        */
                }  
                if (gotcr) {
                        /*COBOL-IT*/
                        if (ipchar == '\r') {
                                buff[n++] = '\n';
                        } else if (ipchar != '\n') {
                                buff[n++] = '\r';
                        }
                        gotcr = 0;
                }
                if (ipchar == '\r') {
                        gotcr = 1;
                } else if (ipchar == '\t') {
                        buff[n++] = ' ';
                        if (tabcnt++ || (n >= cb_first_tab_width)) {
                            while ((n-cb_first_tab_width) % cb_tab_width != 0) {
                                    buff[n++] = ' ';
                            }
                        } else {
                            while (n % cb_first_tab_width != 0) {
                                    buff[n++] = ' ';

                            }
                        }
                } else if (ipchar  != EOF){
                        buff[n++] = ipchar;
                }
        }

        if (buff[n - 1] != '\n') {
                cb_warning (_("Line not terminated by a newline"));
                buff[n++] = '\n';
        }
        buff[n]= 0;
        next_newline_count=0;
#ifdef DEBUGMF
        printf("read : '%s'", buff);
#endif
        lastcolumn = cb_text_column;
        p = (unsigned char *)buff;
        /* when processing utf-8 sources find the cb_text_column utf8 chars*/
        if (cb_utf8_sources) {
           lastcolumn = 0;
           memset (&buff[n], 0, 8);
           for (i = 0; i < cb_text_column; i++) {
              if (p[lastcolumn] == 0) {
                 break;
              } else if (p[lastcolumn] < 0xC0) {
                lastcolumn++;
              } else if (p[lastcolumn] >= 0xC0 && p[lastcolumn] <= 0xDF) {
                lastcolumn+=2;
              } else if (p[lastcolumn] >= 0xE0 && p[lastcolumn] <= 0xEF) {
                lastcolumn+=3;
              } else if (p[lastcolumn] >= 0xF0 && p[lastcolumn] <= 0xF7) {
                lastcolumn+=4;
              } else if (p[lastcolumn] >= 0xF8 && p[lastcolumn] <= 0xFB) {
                lastcolumn+=5;
              } else if (p[lastcolumn] >= 0xFC && p[lastcolumn] <= 0xFD) {
                lastcolumn+=6;
              } else {
                lastcolumn++;
              }
           }
           if (lastcolumn<cb_text_column) {
              lastcolumn =cb_text_column;
           }
        }

        /*First clean end of line if needed */
        bp = &buff[6];
        if ((cb_source_format == CB_FORMAT_FIXED) && (n > lastcolumn+1) && 
            !( is_comment_char(*bp)) ) {

                buff[lastcolumn] = '\n';
                buff[lastcolumn+1] = 0;
                n = n > lastcolumn+1 ? lastcolumn+1 : n;
        }
        check_directive (buff, &n);

        if (cb_source_format != CB_FORMAT_FIXED) {
               bp = &buff[0];
        } else {
            if ( n < 7) {
              /* just line mark, skip line*/
               newline_count ++;
               goto start;
               
            } else if (is_comment_char(buff[0]) && cb_flag_mfcomment) {
              /* MF Do not print comment in with * at column 1 just line mark, skip line*/
               newline_count ++;
               goto start;
            } else {
               bp = &buff[6];
            }
        }        
        if (is_comment_char(*bp)) {
                if (quotation_mark || consecutive_quotation ) {
                   /* when comment in consecutive string just skip the line*/
                   newline_count ++;
                   goto start;
                }
                
                /* comment line */
                /*Keep the pragma */
                /*keep comment for listing */
                if (((n>=8) && memcmp(bp, "*PRAGMA ", 8) == 0) ||
                    ((n>=9) && memcmp(bp, "*#PRAGMA ", 9) == 0) ||
                    ((n>=7) && cb_flag_keep_org_line && memcmp(bp, "*#DEBUG", 7) == 0) ||                    
                    (*bp == '*' && *(bp+1) == '#' && *(bp+2) == '#') )
                {

                  goto end_return;
                } else {
                  /* insert **> before the line */
                  /* insert newlines at the start of the buffer */
                  *bp = '>';      
                  memmove (bp +2, bp, strlen (bp) + 1);
                  *bp = '*';      
                  *(bp+1) = '*';      
                  goto end_return;
               }
        }
        
        if ( check_MF_directive (bp, n, &is_condition)) {
              /*
              ++newline_count;
              memset (buff, '\n', newline_count);
              buff[newline_count] = 0;
              newline_count = 0;
              */
              if (is_condition) {
                 /* skip MF conditional . */
                 /* insert **> before the line */
                 /* insert newlines at the start of the buffer */
                   newline_count ++;
                   goto start;
              }
                  
              goto end_return;

        } else  if ((within_MF_conditional[within_MF_conditional_stack] == MF_CONDITIONAL_SKIPPING) || 
                  (within_MF_conditional[within_MF_conditional_stack] == MF_CONDITIONAL_SKIPPING_TO_END) || 
                  (within_MF_conditional[within_MF_conditional_stack] == MF_CONDITIONAL_ERROR)) {
              /* skip MF conditional . */
              ++newline_count;
              goto start;
        }
        /* nothing more to do with free format */
        if (cb_source_format != CB_FORMAT_FIXED) {
                /*fprintf(stderr, "parser '%s'\n", buff);*/
              goto end_return;
        }

        /* line too short */
        if (n < 8) {
                newline_count++;
                goto start;
        }
        
        /* erase 6 first characters*/
        memset (buff, ' ', 6);

        /* check the indicator (column 7) */
        bp = buff + 7;
        switch (buff[6]) {
        case ' ':
        case '\t':
                break;
        case '\r':
        case '\n':
                newline_count++;
                goto start;
        case '-':
                if (cb_prepro_initial_pass) {
                     memmove (buff + newline_count, bp-1, strlen (bp-1) + 1);
                     memset (buff, '\n', newline_count);
                     newline_count = 0;
                     return strlen(buff);
                }
                if(cb_flag_continuation_line){
                  if(quotation_mark)
                     continuation = 1;
                }
                else
                  continuation = 1;
                break;
        case 'd':
        case 'D':
                /* debugging line */
                if (cb_flag_debugging_line) {
                        break;
                }
                newline_count++;
                goto start;
        default:
                /* invalid indicator */
                if (cb_check_feature_x (NULL, cb_syntax_ibm5_2, "Invalid indicator '%c' at column 7", buff[6])) {
                   if (!cb_flag_mf_compat_parser) {
                       cb_error (_("Invalid indicator '%c' at column 7 (use -fmf-compta-parse to disable error)"), buff[6]);
                       return YY_NULL;
                   } else {
                       cb_warning (_("Invalid indicator '%c' at column 7"), buff[6]);
                   }
                }
        }
                
        /* skip comments that follow after AUTHORS, etc. */
        if (within_comment) {
                if (buff[7] == ' ' ) {
                        newline_count++;
                        goto start;
                }
                within_comment = 0;
        }

        /* check the text that is longer than cb_text_column */
        if (n > lastcolumn + 1) {

                /* show warning if it is not whitespaces */
                if (cb_warn_column_overflow && last_line_2 < cb_lex_source_file.line - 1) {
                        for (coln = lastcolumn; coln < n; coln++) {
                                if (buff[coln] != ' ' && buff[coln] != '\n') {
                                        cb_warning (_("Source text after column %d"),
                                                    cb_text_column);
                                        break;
                                }
                        }
                }

                /* remove it */
                strcpy (buff + lastcolumn, "\n");
                last_line_2 = cb_lex_source_file.line;
                n = lastcolumn + 1;
        }
        
        /* skip blank lines */
        for (i = 7; buff[i] == ' '; i++) ;
        if (buff[i] == '\n') {
                newline_count++;
                goto start;
        }

        if (continuation ) {
                /* line continuation */
                for (; *bp == ' '; bp++) ;
                /* validate concatenation */
                if (consecutive_quotation) {
                        if (bp[0] == quotation_mark && bp[1] == quotation_mark) {
                                bp++;
                        } 
                        /* CIT 
                        else {
                                cb_error (_("Invalid line continuation"));
                                return YY_NULL;
                        }
                        */
                        quotation_mark = 0;
                        consecutive_quotation = 0;
                } else if (quotation_mark) {
                        /* literal concatenation */
                        if (*bp == quotation_mark) {
                                bp++;
                        } else {
                                cb_warning (_("Invalid line continuation 2"));                                
                        }
                } else {
                     if ((*bp == '\'' || *bp == '\"')) {
                        if (strchr(bp+1,*bp) == 0) {
                            cb_warning (_("Invalid line continuation 1"));
                            *bp = ' ';
                            bp++; 
                         }
                     }
                }

        } else {
                /* normal line */
                if (!consecutive_quotation && quotation_mark) cb_check_feature_x (NULL, cb_syntax_ibm5_2, "Non closed quote");                
                quotation_mark = 0;
                consecutive_quotation = 0;
        }
        in_line_comment = 0;
        /* check if string literal is to be continued */
        for (i = bp - buff; (buff[i] != '\n') && (buff[i] != 0); i++) {
                if ((buff[i] == '*') && (buff[i+1] == '>') && (quotation_mark == 0)) {
                  in_line_comment = 1;
                }
                if (((buff[i] == '\'' || buff[i] == '\"')) && !in_line_comment)  {
                        if (quotation_mark == 0 ) {
                                /* literal start */
                                quotation_mark = buff[i];
                        } else if (quotation_mark == buff[i]) {
                                if (i == lastcolumn - 1) {
                                        /* consecutive quotation */
                                        consecutive_quotation = 1;
                                } else {
                                        /* literal end */
                                        quotation_mark = 0;
                                }
                        }
                }
        }
        /* truncate trailing spaces, including the newline */
        if (quotation_mark) {               
                for (; i < lastcolumn;) {
                        buff[i++] = ' ';
                }
                buff[i] = 0;
        } else {
                for (i--; buff[i] == ' '; i--) ;
                if (buff[i] == '\'' || buff[i] == '\"') {
                        buff[++i] = ' ';
                }
                buff[i + 1] = 0;
        }
        next_newline_count=1;

end_return:
        if (continuation ) {
                memmove (buff, bp, strlen (bp) + 1);
                newline_count++;
        } else {
               /* insert newlines at the start of the buffer */
                memmove (buff + newline_count, bp, strlen (bp) + 1);
                memset (buff, '\n', newline_count);
                newline_count = next_newline_count;
        }
        /*DEBUG*/
        /*fprintf(stderr, "parser '%s'\n",buff);*/
#ifdef DEBUGMF
        printf("return : '%s'\n", buff);
#endif
        return strlen (buff);
}

static struct cb_text_list *
text_list_add (struct cb_text_list *list, const char *text, int size) {
    struct cb_text_list *p;
    struct cb_text_list *l;
    char *s = cobc_malloc (size+1);
    
    p = cobc_malloc (sizeof (struct cb_text_list));
    strncpy(s, text, size);
    p->text = s;
    p->next = NULL;
    if ( !list ) {
        return(p);
    } else {
        for ( l = list; l->next; l = l->next ) {
            ;
        }
        l->next = p;
        return(list);
    }
}

static void
ppflush_text_queue (void)
{
    for (; text_queue; text_queue = text_queue->next) {
            fputs (text_queue->text, ppout);
    }
}

static int 
ppiscomment(const char *text)
{
   
   return ((text[0] == '*') && ((text[1] == '>') || (text[1] == '*' && text[2] == '>')));
}


#undef DEBUG_REPLACING  
static void
ppecho1 (const char *text, int size)
{
        struct cb_replace_list  *r;
        struct cb_text_list     *l;
        struct cb_text_list     *last = NULL;
        struct cb_text_list     *first = NULL;
        struct cb_text_list     *last_queue = NULL;
        struct cb_text_list     *first_queue = NULL;
        struct cb_text_list     *queue;
        unsigned int            mode = 0;
        int                     offset = 0;
        char                    *p;
        char                    *q;

        if (size == 0)
            return;
        if (!inside_cobol_text && text[0] != '\n')
            return;
        if (cb_flag_makesyn_prepro_patch && text[0] != '.')
        {
           char * subs = cobc_find_constant((char*)text,NULL, 1, NULL);
           if (subs) {
               text = subs;
               size = strlen(subs);
           }
        }

        if (text_queue == NULL && (text[0] == ' ' || text[0] == '\n')) {
                fwrite( text, size, 1, ppout );
        } else if (!current_replace_list) {
                ppflush_text_queue ();
                fwrite( text, size, 1, ppout );
        } else {
                /* Do replacement */
                #ifdef DEBUG_REPLACING
                printf("ppecho1 %d : %s \n",cb_lex_source_file.line, text);
                #endif
                text_queue = text_list_add (text_queue, text, size);
                while (text_queue) {
                    for (r = current_replace_list; r; r = r->next) {
                            first_queue = queue = text_queue;
                            mode = 0;
                            for (first = l = r->old_text; l; last=l, l = l->next) {
                                    while (l && (l->text[0] == ' '  || 
                                                 l->text[0] == '\n' || 
                                                 /* l->text[0] == '*'  ||*/
                                                 ppiscomment(l->text)  ||
                                                 l->text[0] == 1    || 
                                                 l->text[0] == 2    || 
                                                 l->text[0] == 3 ))   {
                                            if (l->text[0] == 1 || l->text[0] == 2 || l->text[0] == 3) {
                                                 mode = l->text[0];
                                                 first = l->next;
                                            }
                                            l = l->next;
                                    }
                                    if (l == NULL) {
                                            break;
                                    }
                                    while (queue && (queue->text[0] == ' '  ||
                                                     queue->text[0] == '\n' ||
                                                     ppiscomment(queue->text) ))   {
                                            queue = queue->next;
                                    }
                                    if (queue == NULL) {
                                            return; /* partial match */
                                    }
                                    /*
                                     mode = 0 : EQUAL all token
                                     mode = 1 : LEADING TOKEN
                                     mode = 2 : TRAILING TOKEN
                                     mode = 3 : PARTIAL Replacement
                                    */
                                    if ((mode == 0) && (strcasecmp (l->text, queue->text) != 0)) {
                                            #ifdef DEBUG_REPLACING
                                            printf("NO %s %s %s\n",text, r->old_text->text, l->text);
                                            #endif
                                            break;
                                    } else if ((mode == 1) &&
                                               (((l->next && strcasecmp (l->text, queue->text) != 0)) ||
                                                (strncasecmp (l->text, queue->text, strlen(l->text)) != 0))) {
                                           /*LEADING */ 
                                            #ifdef DEBUG_REPLACING
                                            printf("NOL %s %s %s\n",text, r->old_text->text, l->text);                                  
                                            #endif
                                           break;
                                    } else if (mode == 2){ 
                                           if (l && l == first) {
                                              /*TRAILING*/
                                              if (strlen(l->text) > strlen(queue->text) ) {
                                                 #ifdef DEBUG_REPLACING
                                                 printf("NOT1 %s %s %s\n",text, r->old_text->text, l->text);
                                                 #endif
                                                 break;
                                              }
                                              offset = strlen(queue->text) - strlen(l->text);
                                              if (strcasecmp (l->text, queue->text + offset) != 0)  {
                                                 #ifdef DEBUG_REPLACING
                                                 printf("NOT2 %s %s %s\n",text, r->old_text->text, l->text);
                                                 #endif
                                                 break;
                                              }
                                           } else if (strcasecmp (l->text, queue->text) != 0) {
                                                 #ifdef DEBUG_REPLACING
                                                 printf("NOT3 %s %s %s\n",text, r->old_text->text, l->text);
                                                 #endif
                                                 break;
                                           }
                                           
                                    } else if (mode == 3){
                                         /* PARTIAL Replace*/
                                           char *match = strcasestr(queue->text, l->text);
                                           if (!match) {
                                              #ifdef DEBUG_REPLACING
                                              printf("NOP %s %s %s\n",text, r->old_text->text, l->text);
                                              #endif
                                              break;
                                           } else {
                                              offset = match - queue->text;
                                              #ifdef DEBUG_REPLACING
                                              printf("OK3 %s %s %s %d\n",text, r->old_text->text, l->text, offset );
                                              #endif
                                           }
                                    } else if (mode > 3){
                                           cb_error (_("INTERNAL compiler error in replacing (%d)"), mode);
    
                                    }
                                    #ifdef DEBUG_REPLACING
                                    printf("OK %s %s %s\n",text, r->old_text->text, l->text);
                                    #endif
                                    switch (mode)
                                    {
                                       case 0:
                                           /*just continue*/
                                           break;
                                       case 1: 
                                           break;
                                       case 2: 
                                           break;
                                       case 3:
                                           p = strdup(queue->text);
                                           p [offset] = 0;
                                           fputs (p, ppout);
                                           if (r->new_text)
                                              fputs (r->new_text->text, ppout);
                                           q = p + offset  + strlen(l->text);
                                           if (strlen(q))
                                             fputs (q, ppout);

                                           free(p);
                                           
                                           break;
                                    }
                                    last_queue = queue;
                                    queue = queue->next;
                            }
                            if (l == NULL) {
                                    /* match */
                                    switch (mode )
                                    {
                                       case 0: 
                                          for (l = r->new_text; l; l = l->next) {
                                                 fputs (l->text, ppout);
                                          }
                                          break;
                                       case 1:
                                          for (l = r->new_text; l; l = l->next) {
                                                 fputs (l->text, ppout);
                                          }
                                          if (last_queue && (strlen(last_queue->text) > strlen(last->text)))
                                             fputs (last_queue->text + strlen(last->text), ppout);

                                          break;
                                       case 2:
                                           offset = strlen(first_queue->text) - strlen(first->text);
                                           if (offset > 0) {
                                             p = strdup(first_queue->text);
                                             p[offset] = 0;
                                             fputs (p, ppout);
                                             free(p);
                                           }
                                           for (l = r->new_text; l; l = l->next) {
                                              fputs (l->text, ppout);
                                           }
                                           break;

                                    }
    /*
                                    text_queue = queue ? queue->next : NULL;
    */
                                    text_queue = queue;
                                    continue;
                            }
                    }
    
                    /* no match */
                    /*for (; text_queue; ) {*/
                    if (text_queue) {
                            do {
                               fputs (text_queue->text, ppout);
                               l = text_queue->next;
                               free ((char*)text_queue->text);
                               free (text_queue);
                               text_queue = l;
                            } while (text_queue && (text_queue->text[0] == ' '  ||
                                         text_queue->text[0] == '\n' ||
                                         ppiscomment(text_queue->text) ));   
                    }
                }
        }
}

static void
ppecho (const char *text) {
    ppecho1 (text, strlen(text));
}

static void
ppechodebug (const char *text) {
    /*ppflush_text_queue();*/
    if (!no_debug_out)
       fputs (text, ppout);
}

static void
ppECHO (void) {
    ppecho1 (pptext, ppleng);
}

static void
ppECHO_LITTERAL (void) {
    int i;
    int please_escape = 0;
    unsigned char * p;
    char * q;
    int sz;
    static char *buffer = NULL; 
    static int nbuffer = 0;

    if (cb_flag_mf_ctrl_escaped && (ppleng > 0) && 
        ((pptext[0] == '\"') || (pptext[0] == '\''))) {
        p = (unsigned char *)pptext;
        for (i = 0; i < ppleng && !please_escape ; i++, p++) {
            if (*p < 0x20)
                please_escape =1;    
        }
        if (please_escape){
            sz = (ppleng*2)+10; 
            if (sz > nbuffer){
                if (buffer)
                    free(buffer);
                buffer = malloc(sz);
            }
            strcpy(buffer, "X\"");
            p = (unsigned char *)pptext+1;
            q = buffer+2;
            for (i = 0; i < ppleng-2 ; i++, p++, q+=2) {
                sprintf (q, "%02X", (int) *p);
            }
            strcat (buffer, "\"");
            ppecho1 (buffer, strlen(buffer));
            return;
        }        
    }
    ppecho1 (pptext, ppleng);
}

static void 
start_exec_state(void){
    need_debug = 0;
    need_linemark= 0;
    check_newline();
    if (cb_pplinemark) {
        if (inside_procedure_div) {
            if (!cb_flag_debug_exec){
                ppecho ("*#PRAGMA \"DEBUGOFF\"\n");                               
            }
        } 
        ppecho("*##\n");
    }
    if (cb_flag_copy_exec_replace ) {
      yy_push_state(EXEC_REPLACE_STATE); 
    } else {
      yy_push_state(EXEC_STATE); 
    }
    ppECHO();
    if ((ext_prepro_name == NULL) && (cb_compile_level != CB_LEVEL_PREPROCESS) &&
        !cb_flag_syntax_only && !cb_flag_source_validate) {
        cb_error (_("EXEC statement is ignored Code generation is canceled"));
        cb_compile_level = CB_LEVEL_TRANSLATE;
    }

}

/*
	this method return the code corresponding to the CICS response
	http://www.gatorspit.com/tips/dfhresp-cics-codes.html
	AUTHOR: K.I METRRIXWARE MAGHREB
	MODIFIED: CBI
*/
/* BUG 4932 >>> */ 
static const char * get_cics_response_code(const char * resp)
{
cics_response_t *p = cics_response_table;

  while (p->name != NULL)
  {
    if (strcasecmp(resp, p->name) == 0)
      break;
    p++;
  }
  return p->code;
}
/* <<< BUG 4932 */ 


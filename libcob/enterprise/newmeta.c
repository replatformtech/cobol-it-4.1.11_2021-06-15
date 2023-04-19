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
#include "newmeta.h"
/* XMLBooster-generated code (Version 2.16.8)
   This code is generated automatically. It is not meant
   to be maintained or even read. As it is generated, 
   it does not follow any coding standard. Please refrain
   from performing any change directly on this generated 
   code, as it might be overwritten anytime. */

static BOOLEAN laccept_BOOLEAN(XMLB_CONTEXT ctxt)
{
/* Skip white space: 99 , Unique: 1 */
XMLB_SKIP_WHITE_SPACE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'y':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'e':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"s",1)) {
return TRUE;
} else {
return TRUE;
} /* If */
 XMLB_BREAK /* 102 */
default:
return TRUE;
} /* Switch */
 XMLB_BREAK /* 101 */
 case 't':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT_STR(ctxt,"rue",3);
return TRUE;
 XMLB_BREAK /* 103 */
 case 'o':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'n':
XMLB_ADVANCE(ctxt);
return TRUE;
 XMLB_BREAK /* 105 */
 case 'f':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'f',"f");
return FALSE;
 XMLB_BREAK /* 106 */
default:
XMLB_FAIL(ctxt, "Other character expected (107)");
} /* Switch */
 XMLB_BREAK /* 104 */
 case 'n':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'o':
XMLB_ADVANCE(ctxt);
return FALSE;
 XMLB_BREAK /* 109 */
default:
return FALSE;
} /* Switch */
 XMLB_BREAK /* 108 */
 case 'f':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT_STR(ctxt,"alse",4);
return FALSE;
 XMLB_BREAK /* 110 */
 case 'Y':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'E':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"S",1)) {
return TRUE;
} else {
return TRUE;
} /* If */
 XMLB_BREAK /* 112 */
default:
return TRUE;
} /* Switch */
 XMLB_BREAK /* 111 */
 case 'T':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'r':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'u',"u");
XMLB_ACCEPT(ctxt,'e',"e");
return TRUE;
 XMLB_BREAK /* 114 */
 case 'R':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'U',"U");
XMLB_ACCEPT(ctxt,'E',"E");
return TRUE;
 XMLB_BREAK /* 115 */
default:
XMLB_FAIL(ctxt, "Other character expected (116)");
} /* Switch */
 XMLB_BREAK /* 113 */
 case 'O':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'n':
XMLB_ADVANCE(ctxt);
return TRUE;
 XMLB_BREAK /* 118 */
 case 'f':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'f',"f");
return FALSE;
 XMLB_BREAK /* 119 */
 case 'N':
XMLB_ADVANCE(ctxt);
return TRUE;
 XMLB_BREAK /* 120 */
 case 'F':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'F',"F");
return FALSE;
 XMLB_BREAK /* 121 */
default:
XMLB_FAIL(ctxt, "Other character expected (122)");
} /* Switch */
 XMLB_BREAK /* 117 */
 case 'N':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'O':
XMLB_ADVANCE(ctxt);
return FALSE;
 XMLB_BREAK /* 124 */
default:
return FALSE;
} /* Switch */
 XMLB_BREAK /* 123 */
 case 'F':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'a':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT_STR(ctxt,"lse",3);
return FALSE;
 XMLB_BREAK /* 126 */
 case 'A':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT_STR(ctxt,"LSE",3);
return FALSE;
 XMLB_BREAK /* 127 */
default:
XMLB_FAIL(ctxt, "Other character expected (128)");
} /* Switch */
 XMLB_BREAK /* 125 */
 case '1':
XMLB_ADVANCE(ctxt);
return TRUE;
 XMLB_BREAK /* 129 */
 case '0':
XMLB_ADVANCE(ctxt);
return FALSE;
 XMLB_BREAK /* 130 */
default:
XMLB_FAIL(ctxt, "Other character expected (131)");
} /* Switch */
XMLB_FAIL(ctxt, "Internal error accept_BOOLEAN");
return FALSE;
}

static char laccept_pc_data_char(XMLB_CONTEXT ctxt)
{
  int  i;
  char _result = '_';
switch (XMLB_CURRENT(ctxt)) {
 case 'q':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT_STR(ctxt,"uot;",4);
goto l136;
 XMLB_BREAK /* 138 */
 case 'n':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT_STR(ctxt,"bsp;",4);
goto l135;
 XMLB_BREAK /* 139 */
 case 'l':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'t',"t");
XMLB_ACCEPT(ctxt,';',";");
goto l133;
 XMLB_BREAK /* 140 */
 case 'g':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'t',"t");
XMLB_ACCEPT(ctxt,';',";");
goto l134;
 XMLB_BREAK /* 141 */
 case 'a':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'p':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT_STR(ctxt,"os;",3);
goto l137;
 XMLB_BREAK /* 143 */
 case 'm':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'p',"p");
XMLB_ACCEPT(ctxt,';',";");
goto l132;
 XMLB_BREAK /* 144 */
default:
XMLB_FAIL(ctxt, "Other character expected (145)");
} /* Switch */
 XMLB_BREAK /* 142 */
 case '#':
XMLB_ADVANCE(ctxt);
XMLB_ACCEPT(ctxt,'x',"x");
goto l138;
 XMLB_BREAK /* 146 */
default:
XMLB_FAIL(ctxt, "Other character expected (147)");
} /* Switch */
/* Begin dispatch Label */
l132:;
_result = XMLB_AMPERSAND;
goto l148;
/* Begin dispatch Label */
l133:;
_result = XMLB_LESS;
goto l148;
/* Begin dispatch Label */
l134:;
_result = XMLB_GREATER;
goto l148;
/* Begin dispatch Label */
l135:;
_result = XMLB_NBSP;
goto l148;
/* Begin dispatch Label */
l136:;
_result = XMLB_QUOT;
goto l148;
/* Begin dispatch Label */
l137:;
_result = XMLB_APOS;
goto l148;
/* Begin dispatch Label */
l138:;
i = xmlb_accept_hexa(ctxt);
if (i < 0)
{
XMLB_FAIL(ctxt, "Internal error accept_hexa");
}
_result = (char) i;
XMLB_ACCEPT(ctxt,';',";");
goto l148;
l148:; /* End of dispatch */
 return _result;
}

static int laccept_pc_data(XMLB_CONTEXT ctxt, 
                           char *origin,
                           int max_size,
                           char closing_ch,
                           WS_MODE ws_mode)
{
  BOOLEAN go_on = TRUE;
  char ch;
  char lastch = '.';
  char * target = origin;
while (go_on) {
  go_on = FALSE;
  ch = XMLB_CURRENT(ctxt);
while ((ch != '<') && (ch != closing_ch)) {
      XMLB_ADVANCE_UNSAFE(ctxt);
if (ch == '&') {
          lastch = '.';
          ch = laccept_pc_data_char(ctxt);
} else {
if (ws_mode >= WS_REPLACE) {
if (ch == '\t' || ch == '\n' || ch == '\r') {
ch = ' ';
} /* If */
if (ws_mode == WS_COLLAPSE) {
if ((ch == ' ') && ((lastch == ' ') || (origin == target))) {
ch = (char)0;
} else {
lastch = ch;
} /* If */
} else {
lastch = ch;
} /* If */
} /* If */
} /* If */
if (ch) {
      *target = ch;
      target ++;
if (max_size == 0) {
XMLB_FAIL(ctxt, "Buffer overflow");
} /* If */
      max_size --;
} /* If */
      ch = XMLB_CURRENT(ctxt);
} /* While */
if (ch == '<') {
  char * cdata;
if (XMLB_LOOKAHEAD_STR(ctxt,"<![CDATA[",9)) {
     cdata = xmlb_accept_until(ctxt, "]]>");
while (*cdata != 0) {
if (max_size == 0) {
XMLB_FAIL(ctxt, "Buffer overflow");
} /* If */
       max_size --;
       *target = *cdata;
       target ++;
       cdata ++;
} /* While */
      go_on = TRUE;
} else {
if (XMLB_LOOKAHEAD_STR(ctxt,"<?",2)) {
XMLB_SKIP_TILL(ctxt, '?');
XMLB_ACCEPT(ctxt,'?',"?");
XMLB_ACCEPT(ctxt,'>',">");
go_on = TRUE;
} /* If */
} /* If */
} /* If */
} /* While */
if (ws_mode == WS_COLLAPSE && lastch == ' ' && target > origin) {
target --;
} /* If */
  *target = 0;
  XMLB_CHECK_EOF(ctxt);
  return target - origin + 1;
}

static char * laccept_dyn_pc_data (XMLB_CONTEXT ctxt,
                                   char closing_ch,
                                   WS_MODE ws_mode)
{
  char temp [PCDATA_BUFFER_SIZE + 5];
  int i;
  
  i = laccept_pc_data (ctxt, temp, 
           PCDATA_BUFFER_SIZE, closing_ch, ws_mode);
  return xmlb_strdupl (ctxt, temp, i);
}

static void lunparse_pcdata (UNPARSE_FUNC uf, void * ctxt, char *p)
{
while (*p)
{
  switch (*p)
        {
case XMLB_AMPERSAND:
  uf (ctxt, "&amp;");
  break;
case XMLB_LESS:
  uf (ctxt, "&lt;");
  break;
case XMLB_GREATER:
  uf (ctxt, "&gt;");
  break;
case XMLB_QUOT:
  uf (ctxt, "&quot;");
  break;
case XMLB_APOS:
  uf (ctxt, "&apos;");
  break;
  default:
    XMLB_UNPARSE_CHAR(uf,ctxt,*p);
    break;
        }
  p++;
  }
}
static BOOLEAN is_null_e(e *obj); 
static void parse_body_e(XMLB_CONTEXT ctxt, e *obj); 
static void parse_e(XMLB_CONTEXT ctxt, e *obj); 
static void lunparse_e(UNPARSE_FUNC uf, void * ctxt, e *obj
)
;
static void lunparse_body_e(UNPARSE_FUNC uf, void * ctxt, e *obj)
;
static e * laccept_e(XMLB_CONTEXT ctxt); 
static void init_e(e * obj);
static BOOLEAN is_null_creator(creator *obj); 
static void parse_body_creator(XMLB_CONTEXT ctxt, creator *obj); 
static void parse_creator(XMLB_CONTEXT ctxt, creator *obj); 
static void lunparse_creator(UNPARSE_FUNC uf, void * ctxt, creator *obj
)
;
static void lunparse_body_creator(UNPARSE_FUNC uf, void * ctxt, creator *obj)
;
static creator * laccept_creator(XMLB_CONTEXT ctxt); 
static void init_creator(creator * obj);
static BOOLEAN is_null_license(license *obj); 
static void parse_body_license(XMLB_CONTEXT ctxt, license *obj); 
static void parse_license(XMLB_CONTEXT ctxt, license *obj); 
static void lunparse_license(UNPARSE_FUNC uf, void * ctxt, license *obj
)
;
static void lunparse_body_license(UNPARSE_FUNC uf, void * ctxt, license *obj)
;
static license * laccept_license(XMLB_CONTEXT ctxt); 
static void init_license(license * obj);
void newmeta_generic_unparse(UNPARSE_FUNC uf, void * ctxt, void*obj,
                    char*tag,
                    BOOLEAN type_id)
{
  while (obj != NULL) {
    switch (XMLB_SIG(obj))
    {
    case (SIG_e):
      lunparse_e(uf, ctxt, (e*)obj);
      break;
    case (SIG_creator):
      lunparse_creator(uf, ctxt, (creator*)obj);
      break;
    case (SIG_license):
      lunparse_license(uf, ctxt, (license*)obj);
      break;
    default:;
    }
  obj = XMLB_NEXT(obj);
  }
}
void newmeta_generic_serialize(void*obj, char * beg, char *end)
  {
  XMLB_SERIALIZE_CONTROL ctrl;
  ctrl.beg = beg;
  ctrl.current = beg;
  ctrl.end = end;
  newmeta_generic_unparse(xmlb_write_to_mem, (void*)&ctrl, obj, NULL, FALSE);
  }
void newmeta_generic_save(void*obj, char * fname)
  {
  FILE * out;
  out = fopen (fname, "w");
  if ( out != 0 ) { 
    newmeta_generic_unparse(xmlb_write_to_file, 
               (void*)out, obj, NULL, FALSE);
    fclose (out);
  }
  }
static void parse_body_e(XMLB_CONTEXT ctxt, e *obj )
{
 if ( obj != NULL ) {
 char quote_char;
/* Indicator */
/* Parse PC data */
((e*)obj) -> aValue = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
 }
}

static void parse_e(XMLB_CONTEXT ctxt, e *obj)
{
 if ( obj != NULL ) {
 char quote_char;

/* Skip white space: 410 , Unique: 2 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Skip white space: 411 , Unique: 3 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
parse_body_e(ctxt, obj);
/* Skip white space: 412 , Unique: 4 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</e>",4);
 }
}

static void lunparse_body_e(UNPARSE_FUNC uf, void * ctxt, e *obj)
{
 int i;
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->aValue);
}

static void lunparse_e(UNPARSE_FUNC uf, void * ctxt, e *obj
)
{
int i;
if (obj == NULL) {
return;
} /* If */
uf(ctxt, "<e");
uf(ctxt, ">");
lunparse_body_e(uf, ctxt, obj);
uf(ctxt, "</e>");
}
static void init_e(e * obj)
{
  memset (obj, 0, sizeof(e));
  obj->sig = SIG_e;
}

static BOOLEAN is_null_e(e *obj) 
{
if (obj->aValue != NULL) {
 return FALSE;
} /* If */
return TRUE;
}
e * new_e(XMLB_CONTEXT ctxt)
{
  e * obj;
  obj = (e*) XMLB_MALLOC (ctxt, sizeof( struct _e));
  init_e(obj);
  return obj;
}

void dispose_e(XMLB_CONTEXT ctxt, e*obj)
{
  xmlb_free (ctxt, obj,  sizeof( struct _e));
}

static e * laccept_e(XMLB_CONTEXT ctxt)
{
  e * obj;
  obj = new_e(ctxt);
  parse_e(ctxt, obj
);
XMLB_COMMIT_e(ctxt,obj);
  return obj;
}

static void parse_body_creator(XMLB_CONTEXT ctxt, creator *obj )
{
 if ( obj != NULL ) {
 char quote_char;
int v155;
int v156;
int v157;
int v158;
int v159;
int v160;
char * v163;
/* Perm */
/* Skip white space: 809 , Unique: 5 */
XMLB_SKIP_WHITE_SPACE(ctxt);
v155 = 0 ; 
v156 = 0 ; 
v157 = 0 ; 
v158 = 0 ; 
v159 = 0 ; 
v160 = 0 ; 
/* Begin Loop */
l161:;
v163 = ctxt->ptr;
switch (XMLB_CURRENT(ctxt)) {
 case '<':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'z':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ip",2)) {
goto l150;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l164;
} /* If */
 XMLB_BREAK /* 166 */
 case 'u':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"uid",3)) {
goto l149;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l164;
} /* If */
 XMLB_BREAK /* 167 */
 case 'c':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'o':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'u':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ntry",4)) {
goto l154;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l164;
} /* If */
 XMLB_BREAK /* 170 */
 case 'm':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"panyName",8)) {
goto l152;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l164;
} /* If */
 XMLB_BREAK /* 171 */
default:
goto l164;
} /* Switch */
 XMLB_BREAK /* 169 */
 case 'i':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ty",2)) {
goto l153;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l164;
} /* If */
 XMLB_BREAK /* 172 */
default:
goto l164;
} /* Switch */
 XMLB_BREAK /* 168 */
 case 'a':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ddress",6)) {
goto l151;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l164;
} /* If */
 XMLB_BREAK /* 173 */
default:
goto l164;
} /* Switch */
 XMLB_BREAK /* 165 */
default:
goto l164;
} /* Switch */
/* Begin dispatch Label */
l149:;
/* Before group on lookahead: <uuid */
if (! v155) {
v155 = 1 ; 
XMLB_MOVE_BACK(ctxt,5);
/* Skip white space: 813 , Unique: 6 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Enclosed */
XMLB_ACCEPT_STR(ctxt,"<uuid",5);
if (XMLB_IS_ALNUM(ctxt)) {
XMLB_FAIL(ctxt, "White space expected after TAG");
} /* If */
/* Skip white space: 814 , Unique: 7 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((creator*)obj) -> auuid = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 8 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</uuid>",7);
/* Skip white space: 812 , Unique: 9 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* End enclosed */
/* Skip white space: 6512 , Unique: 10 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <uuid */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <uuid");
} /* If */
goto l174;
/* Begin dispatch Label */
l150:;
/* Before group on lookahead: <zip */
if (! v156) {
v156 = 1 ; 
XMLB_MOVE_BACK(ctxt,4);
/* Skip white space: 813 , Unique: 11 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<zip",4)) {
/* Skip white space: 814 , Unique: 12 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((creator*)obj) -> azip = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 13 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</zip>",6);
/* Skip white space: 812 , Unique: 14 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 15 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <zip */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <zip");
} /* If */
goto l174;
/* Begin dispatch Label */
l151:;
/* Before group on lookahead: <address */
if (! v157) {
v157 = 1 ; 
XMLB_MOVE_BACK(ctxt,8);
/* Skip white space: 813 , Unique: 16 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<address",8)) {
/* Skip white space: 814 , Unique: 17 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((creator*)obj) -> aaddress = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 18 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</address>",10);
/* Skip white space: 812 , Unique: 19 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 20 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <address */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <address");
} /* If */
goto l174;
/* Begin dispatch Label */
l152:;
/* Before group on lookahead: <companyName */
if (! v158) {
v158 = 1 ; 
XMLB_MOVE_BACK(ctxt,12);
/* Skip white space: 813 , Unique: 21 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Enclosed */
XMLB_ACCEPT_STR(ctxt,"<companyName",12);
if (XMLB_IS_ALNUM(ctxt)) {
XMLB_FAIL(ctxt, "White space expected after TAG");
} /* If */
/* Skip white space: 814 , Unique: 22 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((creator*)obj) -> acompanyName = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 23 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</companyName>",14);
/* Skip white space: 812 , Unique: 24 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* End enclosed */
/* Skip white space: 6512 , Unique: 25 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <companyName */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <companyName");
} /* If */
goto l174;
/* Begin dispatch Label */
l153:;
/* Before group on lookahead: <city */
if (! v159) {
v159 = 1 ; 
XMLB_MOVE_BACK(ctxt,5);
/* Skip white space: 813 , Unique: 26 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<city",5)) {
/* Skip white space: 814 , Unique: 27 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((creator*)obj) -> acity = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 28 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</city>",7);
/* Skip white space: 812 , Unique: 29 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 30 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <city */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <city");
} /* If */
goto l174;
/* Begin dispatch Label */
l154:;
/* Before group on lookahead: <country */
if (! v160) {
v160 = 1 ; 
XMLB_MOVE_BACK(ctxt,8);
/* Skip white space: 813 , Unique: 31 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<country",8)) {
/* Skip white space: 814 , Unique: 32 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((creator*)obj) -> acountry = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 33 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</country>",10);
/* Skip white space: 812 , Unique: 34 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 35 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <country */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <country");
} /* If */
goto l174;
/* Begin dispatch Label */
l164:;
XMLB_RESTORE_PTR(ctxt,v163);
goto l162;
goto l174;
l174:; /* End of dispatch */
goto l161;
l162:;
/* End Loop */
if (! v155) {
XMLB_FAIL(ctxt, "Missing mandatory element in permutation: <uuid [0]");
} /* If */
if (! v158) {
XMLB_FAIL(ctxt, "Missing mandatory element in permutation: <companyName [0]");
} /* If */
/* End Perm */
 }
}

static void parse_creator(XMLB_CONTEXT ctxt, creator *obj)
{
 if ( obj != NULL ) {
 char quote_char;

/* Skip white space: 410 , Unique: 36 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Skip white space: 411 , Unique: 37 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
parse_body_creator(ctxt, obj);
/* Skip white space: 412 , Unique: 38 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</creator>",10);
 }
}

static void lunparse_body_creator(UNPARSE_FUNC uf, void * ctxt, creator *obj)
{
 int i;
/* Unparsing Perm */
if (obj->auuid != NULL) {
/* Unparsing Enclosed */
uf(ctxt, "<uuid>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->auuid);
uf(ctxt, "</uuid>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
if (obj->azip != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: zip */
if (obj->azip != NULL) {
uf(ctxt, "<zip>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->azip);
uf(ctxt, "</zip>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: zip */
} /* If */
if (obj->aaddress != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: address */
if (obj->aaddress != NULL) {
uf(ctxt, "<address>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->aaddress);
uf(ctxt, "</address>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: address */
} /* If */
if (obj->acompanyName != NULL) {
/* Unparsing Enclosed */
uf(ctxt, "<companyName>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->acompanyName);
uf(ctxt, "</companyName>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
if (obj->acity != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: city */
if (obj->acity != NULL) {
uf(ctxt, "<city>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->acity);
uf(ctxt, "</city>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: city */
} /* If */
if (obj->acountry != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: country */
if (obj->acountry != NULL) {
uf(ctxt, "<country>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->acountry);
uf(ctxt, "</country>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: country */
} /* If */
}

static void lunparse_creator(UNPARSE_FUNC uf, void * ctxt, creator *obj
)
{
int i;
if (obj == NULL) {
return;
} /* If */
uf(ctxt, "<creator");
uf(ctxt, ">");
uf(ctxt, "\n");
lunparse_body_creator(uf, ctxt, obj);
uf(ctxt, "</creator>");
}
static void init_creator(creator * obj)
{
  memset (obj, 0, sizeof(creator));
  obj->sig = SIG_creator;
}

static BOOLEAN is_null_creator(creator *obj) 
{
if (obj->azip != NULL) {
 return FALSE;
} /* If */
if (obj->acompanyName != NULL) {
 return FALSE;
} /* If */
if (obj->acity != NULL) {
 return FALSE;
} /* If */
if (obj->acountry != NULL) {
 return FALSE;
} /* If */
if (obj->aaddress != NULL) {
 return FALSE;
} /* If */
if (obj->auuid != NULL) {
 return FALSE;
} /* If */
return TRUE;
}
creator * new_creator(XMLB_CONTEXT ctxt)
{
  creator * obj;
  obj = (creator*) XMLB_MALLOC (ctxt, sizeof( struct _creator));
  init_creator(obj);
  return obj;
}

void dispose_creator(XMLB_CONTEXT ctxt, creator*obj)
{
  xmlb_free (ctxt, obj,  sizeof( struct _creator));
}

static creator * laccept_creator(XMLB_CONTEXT ctxt)
{
  creator * obj;
  obj = new_creator(ctxt);
  parse_creator(ctxt, obj
);
XMLB_COMMIT_creator(ctxt,obj);
  return obj;
}

static void parse_body_license(XMLB_CONTEXT ctxt, license *obj )
{
 if ( obj != NULL ) {
 char quote_char;
int v192;
int v193;
int v194;
int v195;
int v196;
int v197;
int v198;
int v199;
int v200;
int v201;
int v202;
int v203;
int v204;
int v205;
int v206;
int v207;
int v208;
char * v211;
e* v239;
e* v251;
/* Perm */
/* Skip white space: 809 , Unique: 39 */
XMLB_SKIP_WHITE_SPACE(ctxt);
v192 = 0 ; 
v193 = 0 ; 
v194 = 0 ; 
v195 = 0 ; 
v196 = 0 ; 
v197 = 0 ; 
v198 = 0 ; 
v199 = 0 ; 
v200 = 0 ; 
v201 = 0 ; 
v202 = 0 ; 
v203 = 0 ; 
v204 = 0 ; 
v205 = 0 ; 
v206 = 0 ; 
v207 = 0 ; 
v208 = 0 ; 
/* Begin Loop */
l209:;
v211 = ctxt->ptr;
switch (XMLB_CURRENT(ctxt)) {
 case '<':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'z':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ip",2)) {
goto l179;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 214 */
 case 'v':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ersion",6)) {
switch (XMLB_CURRENT(ctxt)) {
 case 'P':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"atch",4)) {
goto l177;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 216 */
 case 'M':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'i':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"nor",3)) {
goto l176;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 218 */
 case 'a':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"jor",3)) {
goto l175;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 219 */
default:
goto l212;
} /* Switch */
 XMLB_BREAK /* 217 */
default:
goto l212;
} /* Switch */
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 215 */
 case 'u':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"uid",3)) {
goto l178;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 220 */
 case 's':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'u':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"bscriptionType",14)) {
goto l182;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 222 */
 case 'i':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"gnature",7)) {
goto l186;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 223 */
default:
goto l212;
} /* Switch */
 XMLB_BREAK /* 221 */
 case 'p':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'r':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"oducts",6)) {
goto l191;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 225 */
 case 'l':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"atforms",7)) {
goto l190;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 226 */
default:
goto l212;
} /* Switch */
 XMLB_BREAK /* 224 */
 case 'n':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"oRuntimeBreak",13)) {
goto l189;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 227 */
 case 'e':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"xpires",6)) {
goto l180;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 228 */
 case 'd':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"emoPurpose",10)) {
goto l188;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 229 */
 case 'c':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'r':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"eator",5)) {
goto l187;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 231 */
 case 'o':
XMLB_ADVANCE(ctxt);
switch (XMLB_CURRENT(ctxt)) {
 case 'u':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ntry",4)) {
goto l184;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 233 */
 case 'm':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"panyName",8)) {
goto l183;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 234 */
default:
goto l212;
} /* Switch */
 XMLB_BREAK /* 232 */
 case 'i':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ty",2)) {
goto l185;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 235 */
default:
goto l212;
} /* Switch */
 XMLB_BREAK /* 230 */
 case 'a':
XMLB_ADVANCE(ctxt);
if (XMLB_LOOKAHEAD_STR(ctxt,"ddress",6)) {
goto l181;
} else {
XMLB_MOVE_BACK(ctxt,1);
goto l212;
} /* If */
 XMLB_BREAK /* 236 */
default:
goto l212;
} /* Switch */
 XMLB_BREAK /* 213 */
default:
goto l212;
} /* Switch */
/* Begin dispatch Label */
l175:;
/* Before group on lookahead: <versionMajor */
if (! v192) {
v192 = 1 ; 
XMLB_MOVE_BACK(ctxt,13);
/* Skip white space: 813 , Unique: 40 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<versionMajor",13)) {
/* Skip white space: 814 , Unique: 41 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> aversionMajor = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 42 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</versionMajor>",15);
/* Skip white space: 812 , Unique: 43 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 44 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <versionMajor */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <versionMajor");
} /* If */
goto l237;
/* Begin dispatch Label */
l176:;
/* Before group on lookahead: <versionMinor */
if (! v193) {
v193 = 1 ; 
XMLB_MOVE_BACK(ctxt,13);
/* Skip white space: 813 , Unique: 45 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<versionMinor",13)) {
/* Skip white space: 814 , Unique: 46 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> aversionMinor = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 47 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</versionMinor>",15);
/* Skip white space: 812 , Unique: 48 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 49 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <versionMinor */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <versionMinor");
} /* If */
goto l237;
/* Begin dispatch Label */
l177:;
/* Before group on lookahead: <versionPatch */
if (! v194) {
v194 = 1 ; 
XMLB_MOVE_BACK(ctxt,13);
/* Skip white space: 813 , Unique: 50 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<versionPatch",13)) {
/* Skip white space: 814 , Unique: 51 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> aversionPatch = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 52 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</versionPatch>",15);
/* Skip white space: 812 , Unique: 53 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 54 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <versionPatch */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <versionPatch");
} /* If */
goto l237;
/* Begin dispatch Label */
l178:;
/* Before group on lookahead: <uuid */
if (! v195) {
v195 = 1 ; 
XMLB_MOVE_BACK(ctxt,5);
/* Skip white space: 813 , Unique: 55 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<uuid",5)) {
/* Skip white space: 814 , Unique: 56 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> auuid = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 57 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</uuid>",7);
/* Skip white space: 812 , Unique: 58 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 59 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <uuid */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <uuid");
} /* If */
goto l237;
/* Begin dispatch Label */
l179:;
/* Before group on lookahead: <zip */
if (! v196) {
v196 = 1 ; 
XMLB_MOVE_BACK(ctxt,4);
/* Skip white space: 813 , Unique: 60 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<zip",4)) {
/* Skip white space: 814 , Unique: 61 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> azip = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 62 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</zip>",6);
/* Skip white space: 812 , Unique: 63 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 64 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <zip */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <zip");
} /* If */
goto l237;
/* Begin dispatch Label */
l180:;
/* Before group on lookahead: <expires */
if (! v197) {
v197 = 1 ; 
XMLB_MOVE_BACK(ctxt,8);
/* Skip white space: 813 , Unique: 65 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<expires",8)) {
/* Skip white space: 814 , Unique: 66 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> aexpires = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 67 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</expires>",10);
/* Skip white space: 812 , Unique: 68 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 69 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <expires */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <expires");
} /* If */
goto l237;
/* Begin dispatch Label */
l181:;
/* Before group on lookahead: <address */
if (! v198) {
v198 = 1 ; 
XMLB_MOVE_BACK(ctxt,8);
/* Skip white space: 813 , Unique: 70 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<address",8)) {
/* Skip white space: 814 , Unique: 71 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> aaddress = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 72 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</address>",10);
/* Skip white space: 812 , Unique: 73 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 74 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <address */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <address");
} /* If */
goto l237;
/* Begin dispatch Label */
l182:;
/* Before group on lookahead: <subscriptionType */
if (! v199) {
v199 = 1 ; 
XMLB_MOVE_BACK(ctxt,17);
/* Skip white space: 813 , Unique: 75 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<subscriptionType",17)) {
/* Skip white space: 814 , Unique: 76 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> asubscriptionType = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 77 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</subscriptionType>",19);
/* Skip white space: 812 , Unique: 78 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 79 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <subscriptionType */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <subscriptionType");
} /* If */
goto l237;
/* Begin dispatch Label */
l183:;
/* Before group on lookahead: <companyName */
if (! v200) {
v200 = 1 ; 
XMLB_MOVE_BACK(ctxt,12);
/* Skip white space: 813 , Unique: 80 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Enclosed */
XMLB_ACCEPT_STR(ctxt,"<companyName",12);
if (XMLB_IS_ALNUM(ctxt)) {
XMLB_FAIL(ctxt, "White space expected after TAG");
} /* If */
/* Skip white space: 814 , Unique: 81 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> acompanyName = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 82 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</companyName>",14);
/* Skip white space: 812 , Unique: 83 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* End enclosed */
/* Skip white space: 6512 , Unique: 84 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <companyName */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <companyName");
} /* If */
goto l237;
/* Begin dispatch Label */
l184:;
/* Before group on lookahead: <country */
if (! v201) {
v201 = 1 ; 
XMLB_MOVE_BACK(ctxt,8);
/* Skip white space: 813 , Unique: 85 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<country",8)) {
/* Skip white space: 814 , Unique: 86 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> acountry = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 87 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</country>",10);
/* Skip white space: 812 , Unique: 88 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 89 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <country */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <country");
} /* If */
goto l237;
/* Begin dispatch Label */
l185:;
/* Before group on lookahead: <city */
if (! v202) {
v202 = 1 ; 
XMLB_MOVE_BACK(ctxt,5);
/* Skip white space: 813 , Unique: 90 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<city",5)) {
/* Skip white space: 814 , Unique: 91 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> acity = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 92 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</city>",7);
/* Skip white space: 812 , Unique: 93 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 94 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <city */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <city");
} /* If */
goto l237;
/* Begin dispatch Label */
l186:;
/* Before group on lookahead: <signature */
if (! v203) {
v203 = 1 ; 
XMLB_MOVE_BACK(ctxt,10);
/* Skip white space: 813 , Unique: 95 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Enclosed */
XMLB_ACCEPT_STR(ctxt,"<signature",10);
if (XMLB_IS_ALNUM(ctxt)) {
XMLB_FAIL(ctxt, "White space expected after TAG");
} /* If */
/* Skip white space: 814 , Unique: 96 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> asignature = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 97 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</signature>",12);
/* Skip white space: 812 , Unique: 98 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* End enclosed */
/* Skip white space: 6512 , Unique: 99 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <signature */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <signature");
} /* If */
goto l237;
/* Begin dispatch Label */
l187:;
/* Before group on lookahead: <creator */
if (! v204) {
v204 = 1 ; 
XMLB_MOVE_BACK(ctxt,8);
/* Element Ref : creator */
/* Skip white space: 806 , Unique: 100 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* If optional... */
if (XMLB_OPENINGTAG_STR(ctxt,"<creator",8)) {
obj->acreator = laccept_creator(ctxt);
XMLB_SET_SON(obj,obj->acreator);
/* Endif optional... */
} /* If */
/* Skip white space: 807 , Unique: 101 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Skip white space: 6512 , Unique: 102 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <creator */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <creator");
} /* If */
goto l237;
/* Begin dispatch Label */
l188:;
/* Before group on lookahead: <demoPurpose */
if (! v205) {
v205 = 1 ; 
XMLB_MOVE_BACK(ctxt,12);
/* Skip white space: 813 , Unique: 103 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<demoPurpose",12)) {
/* Skip white space: 814 , Unique: 104 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> ademoPurpose = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 105 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</demoPurpose>",14);
/* Skip white space: 812 , Unique: 106 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 107 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <demoPurpose */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <demoPurpose");
} /* If */
goto l237;
/* Begin dispatch Label */
l189:;
/* Before group on lookahead: <noRuntimeBreak */
if (! v206) {
v206 = 1 ; 
XMLB_MOVE_BACK(ctxt,15);
/* Skip white space: 813 , Unique: 108 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<noRuntimeBreak",15)) {
/* Skip white space: 814 , Unique: 109 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Indicator */
/* Parse PC data */
((license*)obj) -> anoRuntimeBreak = laccept_dyn_pc_data(ctxt, (char)0,WS_PRESERVE);/* Regexp */
/* Skip white space: 811 , Unique: 110 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</noRuntimeBreak>",17);
/* Skip white space: 812 , Unique: 111 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 112 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <noRuntimeBreak */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <noRuntimeBreak");
} /* If */
goto l237;
/* Begin dispatch Label */
l190:;
/* Before group on lookahead: <platforms */
if (! v207) {
v207 = 1 ; 
XMLB_MOVE_BACK(ctxt,10);
/* Skip white space: 813 , Unique: 113 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<platforms",10)) {
/* Skip white space: 814 , Unique: 114 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Repeat */
/* Skip white space: 804 , Unique: 115 */
XMLB_SKIP_WHITE_SPACE(ctxt);
v239 = NULL;
while(XMLB_OPENINGTAG_STR(ctxt, "<e",2)) {
if (obj->aplatformsCount>=15) {
XMLB_FAIL(ctxt, "Full array: platforms");
} /* If */
parse_e(ctxt, &obj->aplatforms[obj->aplatformsCount]);
obj->aplatformsCount++;
/* Skip white space: 801 , Unique: 116 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* monomorphic Loop */
if (obj->aplatformsCount == 0) {
XMLB_FAIL(ctxt, "At least one element expected in repetition");
} /* If */
/* EndRepeat */
/* Skip white space: 811 , Unique: 117 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</platforms>",12);
/* Skip white space: 812 , Unique: 118 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 119 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <platforms */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <platforms");
} /* If */
goto l237;
/* Begin dispatch Label */
l191:;
/* Before group on lookahead: <products */
if (! v208) {
v208 = 1 ; 
XMLB_MOVE_BACK(ctxt,9);
/* Skip white space: 813 , Unique: 120 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* Optional Enclosed */
if (XMLB_OPENINGTAG_STR(ctxt,"<products",9)) {
/* Skip white space: 814 , Unique: 121 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
/* Repeat */
/* Skip white space: 804 , Unique: 122 */
XMLB_SKIP_WHITE_SPACE(ctxt);
v251 = NULL;
while(XMLB_OPENINGTAG_STR(ctxt, "<e",2)) {
if (obj->aproductsCount>=15) {
XMLB_FAIL(ctxt, "Full array: products");
} /* If */
parse_e(ctxt, &obj->aproducts[obj->aproductsCount]);
obj->aproductsCount++;
/* Skip white space: 801 , Unique: 123 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* monomorphic Loop */
if (obj->aproductsCount == 0) {
XMLB_FAIL(ctxt, "At least one element expected in repetition");
} /* If */
/* EndRepeat */
/* Skip white space: 811 , Unique: 124 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</products>",11);
/* Skip white space: 812 , Unique: 125 */
XMLB_SKIP_WHITE_SPACE(ctxt);
} /* If */
/* End enclosed */
/* Skip white space: 6512 , Unique: 126 */
XMLB_SKIP_WHITE_SPACE(ctxt);
/* After  group on lookahead: <products */
} else {
XMLB_FAIL(ctxt, "Duplicate element in permutation: <products");
} /* If */
goto l237;
/* Begin dispatch Label */
l212:;
XMLB_RESTORE_PTR(ctxt,v211);
goto l210;
goto l237;
l237:; /* End of dispatch */
goto l209;
l210:;
/* End Loop */
if (! v200) {
XMLB_FAIL(ctxt, "Missing mandatory element in permutation: <companyName [0]");
} /* If */
if (! v203) {
XMLB_FAIL(ctxt, "Missing mandatory element in permutation: <signature [0]");
} /* If */
/* End Perm */
 }
}

static void parse_license(XMLB_CONTEXT ctxt, license *obj)
{
 if ( obj != NULL ) {
 char quote_char;
int v262;

/* Skip white space: 410 , Unique: 127 */
XMLB_SKIP_WHITE_SPACE(ctxt);
{
/* Accept Attributes */
v262 = 0 ; 
/* Begin Loop */
l263:;
if (XMLB_LOOKAHEAD_STR(ctxt,"version=",8)) {
goto l262;
} else {
goto l265;
} /* If */
/* Begin dispatch Label */
l262:;
/* Handling attribute version */
/* Also handles alien attributes with prefix version */
if (v262) {
XMLB_FAIL(ctxt, "Duplicate attribute: version");
} /* If */
v262 = 1 ; 
XMLB_ACCEPT_QUOTE(ctxt, quote_char);
obj->aversion = xmlb_fetch_int(ctxt);
XMLB_ACCEPT(ctxt, quote_char,"(Quote)");
/* Skip white space: 818 , Unique: 128 */
XMLB_SKIP_WHITE_SPACE(ctxt);
goto l266;
/* Final default label */
/* Begin dispatch Label */
l265:;
if (! v262) {
XMLB_FAIL(ctxt, "Mandatory attribute missing: version in license");
} /* If */
goto l264;
goto l266;
l266:; /* End of dispatch */
goto l263;
l264:;
/* End Loop */
}
/* Skip white space: 411 , Unique: 129 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT(ctxt,'>',">");
parse_body_license(ctxt, obj);
/* Skip white space: 412 , Unique: 130 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"</license>",10);
 }
}

static void lunparse_body_license(UNPARSE_FUNC uf, void * ctxt, license *obj)
{
 int i;
/* Unparsing Perm */
if (obj->aversionMajor != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: versionMajor */
if (obj->aversionMajor != NULL) {
uf(ctxt, "<versionMajor>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->aversionMajor);
uf(ctxt, "</versionMajor>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: versionMajor */
} /* If */
if (obj->aversionMinor != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: versionMinor */
if (obj->aversionMinor != NULL) {
uf(ctxt, "<versionMinor>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->aversionMinor);
uf(ctxt, "</versionMinor>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: versionMinor */
} /* If */
if (obj->aversionPatch != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: versionPatch */
if (obj->aversionPatch != NULL) {
uf(ctxt, "<versionPatch>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->aversionPatch);
uf(ctxt, "</versionPatch>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: versionPatch */
} /* If */
if (obj->auuid != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: uuid */
if (obj->auuid != NULL) {
uf(ctxt, "<uuid>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->auuid);
uf(ctxt, "</uuid>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: uuid */
} /* If */
if (obj->azip != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: zip */
if (obj->azip != NULL) {
uf(ctxt, "<zip>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->azip);
uf(ctxt, "</zip>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: zip */
} /* If */
if (obj->aexpires != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: expires */
if (obj->aexpires != NULL) {
uf(ctxt, "<expires>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->aexpires);
uf(ctxt, "</expires>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: expires */
} /* If */
if (obj->aaddress != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: address */
if (obj->aaddress != NULL) {
uf(ctxt, "<address>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->aaddress);
uf(ctxt, "</address>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: address */
} /* If */
if (obj->asubscriptionType != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: subscriptionType */
if (obj->asubscriptionType != NULL) {
uf(ctxt, "<subscriptionType>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->asubscriptionType);
uf(ctxt, "</subscriptionType>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: subscriptionType */
} /* If */
if (obj->acompanyName != NULL) {
/* Unparsing Enclosed */
uf(ctxt, "<companyName>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->acompanyName);
uf(ctxt, "</companyName>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
if (obj->acountry != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: country */
if (obj->acountry != NULL) {
uf(ctxt, "<country>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->acountry);
uf(ctxt, "</country>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: country */
} /* If */
if (obj->acity != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: city */
if (obj->acity != NULL) {
uf(ctxt, "<city>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->acity);
uf(ctxt, "</city>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: city */
} /* If */
if (obj->asignature != NULL) {
/* Unparsing Enclosed */
uf(ctxt, "<signature>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->asignature);
uf(ctxt, "</signature>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
if (obj->acreator != NULL) {
/* Unparsing ElementRef */
if (obj->acreator != NULL) {
/* Unparsing element ref: creator */
lunparse_creator(uf, ctxt, obj->acreator);
} /* If */
} /* If */
if (obj->ademoPurpose != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: demoPurpose */
if (obj->ademoPurpose != NULL) {
uf(ctxt, "<demoPurpose>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->ademoPurpose);
uf(ctxt, "</demoPurpose>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: demoPurpose */
} /* If */
if (obj->anoRuntimeBreak != NULL) {
/* Unparsing Enclosed */
/* Testing for empty content: noRuntimeBreak */
if (obj->anoRuntimeBreak != NULL) {
uf(ctxt, "<noRuntimeBreak>");
/* Unparsing PcData */
lunparse_pcdata(uf, ctxt, obj->anoRuntimeBreak);
uf(ctxt, "</noRuntimeBreak>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: noRuntimeBreak */
} /* If */
if (obj->aplatformsCount != 0) {
/* Unparsing Enclosed */
/* Testing for empty content: platforms */
if (obj->aplatformsCount != 0) {
uf(ctxt, "<platforms>");
uf(ctxt, "\n");
/* Unparsing Repeat */
/* Unparsing C repetition: platforms */
for (i=0; i < obj->aplatformsCount; i++)
lunparse_e(uf, ctxt, &obj->aplatforms[i]
);
uf(ctxt, "</platforms>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: platforms */
} /* If */
if (obj->aproductsCount != 0) {
/* Unparsing Enclosed */
/* Testing for empty content: products */
if (obj->aproductsCount != 0) {
uf(ctxt, "<products>");
uf(ctxt, "\n");
/* Unparsing Repeat */
/* Unparsing C repetition: products */
for (i=0; i < obj->aproductsCount; i++)
lunparse_e(uf, ctxt, &obj->aproducts[i]
);
uf(ctxt, "</products>");
/* Father is not a mixed */
uf(ctxt, "\n");
} /* If */
/* After Testing for empty content: products */
} /* If */
}

static void lunparse_license(UNPARSE_FUNC uf, void * ctxt, license *obj
)
{
int i;
if (obj == NULL) {
return;
} /* If */
uf(ctxt, "<license");
uf(ctxt, "\n");
uf(ctxt, " version=\"");
xmlb_unparse_int (uf, ctxt, obj->aversion);
uf(ctxt, "\"");
uf(ctxt, "\n");
uf(ctxt, ">");
uf(ctxt, "\n");
lunparse_body_license(uf, ctxt, obj);
uf(ctxt, "</license>");
}
static void init_license(license * obj)
{
  memset (obj, 0, sizeof(license));
  obj->sig = SIG_license;
}

static BOOLEAN is_null_license(license *obj) 
{
if (obj->aversion != 0) {
 return FALSE;
} /* If */
if (obj->aversionMajor != NULL) {
 return FALSE;
} /* If */
if (obj->aversionMinor != NULL) {
 return FALSE;
} /* If */
if (obj->aversionPatch != NULL) {
 return FALSE;
} /* If */
if (obj->auuid != NULL) {
 return FALSE;
} /* If */
if (obj->azip != NULL) {
 return FALSE;
} /* If */
if (obj->aexpires != NULL) {
 return FALSE;
} /* If */
if (obj->aaddress != NULL) {
 return FALSE;
} /* If */
if (obj->asubscriptionType != NULL) {
 return FALSE;
} /* If */
if (obj->acompanyName != NULL) {
 return FALSE;
} /* If */
if (obj->acountry != NULL) {
 return FALSE;
} /* If */
if (obj->acity != NULL) {
 return FALSE;
} /* If */
if (obj->asignature != NULL) {
 return FALSE;
} /* If */
if (obj->ademoPurpose != NULL) {
 return FALSE;
} /* If */
if (obj->anoRuntimeBreak != NULL) {
 return FALSE;
} /* If */
if (obj->aproductsCount != 0) {
 return FALSE;
} /* If */
if (obj->aplatformsCount != 0) {
 return FALSE;
} /* If */
if (obj->acreator != NULL) {
 return FALSE;
} /* If */
return TRUE;
}
license * new_license(XMLB_CONTEXT ctxt)
{
  license * obj;
  obj = (license*) XMLB_MALLOC (ctxt, sizeof( struct _license));
  init_license(obj);
  return obj;
}

void dispose_license(XMLB_CONTEXT ctxt, license*obj)
{
  xmlb_free (ctxt, obj,  sizeof( struct _license));
}

static license * laccept_license(XMLB_CONTEXT ctxt)
{
  license * obj;
  obj = new_license(ctxt);
  parse_license(ctxt, obj
);
XMLB_COMMIT_license(ctxt,obj);
  return obj;
}

void unparse_license(UNPARSE_FUNC f, void * ctxt, license *obj)
{
lunparse_license(f, ctxt, obj);
}

BOOLEAN save_license(license *obj, char * fname)
{
  FILE * out;
  out = fopen (fname, "w");
  if (out == NULL)
    return FALSE;
  lunparse_license(xmlb_write_to_file, 
                             (void*)out, obj);
  fclose (out);
  return TRUE;
}

int unparsed_length_license(license *obj)
{
  int res = 0;
  lunparse_license(xmlb_compute_unparsed_length, (void*)& res, obj
);
  return res;
}

BOOLEAN serialize_license(license *obj, char * beg, char * end)
{
  XMLB_SERIALIZE_CONTROL ctrl;
  ctrl.beg = beg;
  ctrl.current = beg;
  ctrl.end = end;
  lunparse_license(xmlb_write_to_mem, (void*)&ctrl, obj
);
  if (ctrl.current == NULL) return FALSE;
  *(ctrl.current)=0;
  return TRUE;
}

license* accept_license(XMLB_CONTEXT ctxt)
{
license * obj = NULL;
/* Skip white space: 414 , Unique: 131 */
XMLB_SKIP_WHITE_SPACE(ctxt);
XMLB_ACCEPT_STR(ctxt,"<license",8);
if (XMLB_IS_ALNUM(ctxt)) {
XMLB_FAIL(ctxt, "White space expected after TAG");
} /* If */
  obj = laccept_license(ctxt);
  return obj;
}
license* newmeta_generic_accept(XMLB_CONTEXT ctxt)
{
  return accept_license(ctxt);
}
#ifndef XMLB_NO_TAG_TABLE
char * tag_table [] = {
       "creator", 
       "e", 
       "license", 
       NULL};
#endif

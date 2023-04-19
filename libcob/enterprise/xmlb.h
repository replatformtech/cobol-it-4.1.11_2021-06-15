/*CIT_BEGIN_ENTERPRISE*/
#ifdef SWIG
%module xmlb
%{
#include "xmlb.h"
#include "xmlbu.h"
%}
%include "xmlbu.h"
%include "xmlb.h"
#endif
#ifndef XMLB_H
#define XMLB_H 1
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <setjmp.h>
#include <xmlbu.h>
#ifndef BOOLEAN
#ifndef XMLB_BOOLEANS_DEFINED
#define BOOLEAN int
#endif
#endif
#ifndef TRUE
#define TRUE    ((BOOLEAN)1)
#endif

#ifndef FALSE
#define FALSE   ((BOOLEAN)0)
#endif
#ifndef XMLB_NOFLOAT
#ifndef XMLB_DOUBLE
#define XMLB_DOUBLE double
#endif
#endif

#define QUOTE1  '\''
#define QUOTE2  '"'
#ifdef XMLB_DYNAMIC
#ifdef BUFFER_SIZE
#undef BUFFER_SIZE
#endif
#else
#ifndef BUFFER_SIZE
#define BUFFER_SIZE (256*1024)
#endif
#endif
#ifdef XMLB_DYNAMIC
#ifndef PAGE_SIZE
#define PAGE_SIZE (256*1024)
#endif
#else
#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
#endif
#ifndef PCDATA_BUFFER_SIZE
#define PCDATA_BUFFER_SIZE (16*1024)
#endif

typedef struct
  {
    void *next;
    unsigned sig;
    void *father;
    void *first_son;
    void *sibling;
  } XMLB_MINIMAL_HEADER;
#define XMLB_NEXT(p)       (((XMLB_MINIMAL_HEADER*)p)->next)
#define XMLB_FIRST_SON(p)  (((XMLB_MINIMAL_HEADER*)p)->first_son)
#define XMLB_SIG(p)        (((XMLB_MINIMAL_HEADER*)p)->sig)
#define XMLB_SET_NEXT(p,q) ((((XMLB_MINIMAL_HEADER*)p)->next)=q)
#define XMLB_SET_SIBLING(p,q) ((((XMLB_MINIMAL_HEADER*)p)->sibling)=q)
#define XMLB_SET_FIRST_SON(p,q) ((((XMLB_MINIMAL_HEADER*)p)->first_son)=q)
#define XMLB_SET_FATHER(p,q) ((((XMLB_MINIMAL_HEADER*)p)->father)=q)
#ifdef XMLB_DYNAMIC
typedef struct _S_XMLB_PAGE
  {
    struct _S_XMLB_PAGE * next;
    unsigned used;
    union
      {
#ifndef XMLB_NOFLOAT
        XMLB_DOUBLE d;
#endif
        void *ptr;
        unsigned u;
        char buffer [PAGE_SIZE];
      } aligned;
  } S_XMLB_PAGE;

typedef S_XMLB_PAGE * XMLB_PAGE;
#endif
typedef struct
  {
    char *first, *last, *ptr, *err_ptr;
#ifndef SWIG
    jmp_buf jump_buffer;
    BOOLEAN jmp_buf_ready;
#endif
    char *err_msg;
#ifndef SWIG
  #ifdef XMLB_DYNAMIC
    XMLB_PAGE  page;
    char * owned_input_buffer;
  #else
    char *malloc_first, *malloc_last, *malloc_top;
    union
      {
#ifndef XMLB_NOFLOAT
        double d;
#endif
        void *ptr;
        unsigned u;
        char buffer [BUFFER_SIZE+32];
      } aligned;
  #endif
#endif
  } S_XMLB_CONTEXT;
typedef S_XMLB_CONTEXT *XMLB_CONTEXT;
#ifdef SWIG
%{
typedef S_XMLB_CONTEXT *XMLB_CONTEXT;
%}
%extend S_XMLB_CONTEXT {
  S_XMLB_CONTEXT()
  {
    S_XMLB_CONTEXT *ctxt = (S_XMLB_CONTEXT*) malloc (sizeof(S_XMLB_CONTEXT));
    xmlb_init_context(ctxt);

    return ctxt;
  }

  ~S_XMLB_CONTEXT()
  {
    free(self);
  }
};
#endif
void   xmlb_init_context(XMLB_CONTEXT ctxt);
void xmlb_restart(XMLB_CONTEXT ctxt);
void   xmlb_dispose_context(XMLB_CONTEXT ctxt);
BOOLEAN  xmlb_load(XMLB_CONTEXT ctxt, char * fname);
BOOLEAN  xmlb_load_binary(XMLB_CONTEXT ctxt, char * fname);
void xmlb_restart(XMLB_CONTEXT ctxt);
void   xmlb_set_parse_string(XMLB_CONTEXT ctxt, char * ptr);
void   xmlb_set_parse_buffer(XMLB_CONTEXT ctxt, char * beg,
                                                char * end);
void   xmlb_reset_context(XMLB_CONTEXT ctxt);
void   xmlb_dump_state(XMLB_CONTEXT);
#define XMLB_NBSP      ' '
#define XMLB_AMPERSAND '&'
#define XMLB_LESS      '<'
#define XMLB_GREATER   '>'
#define XMLB_QUOT      '"'
#define XMLB_QUOTE     '"'
#define XMLB_APOS      '\''
#define XMLB_CATCH_ERROR(ctxt) if(((ctxt)->jmp_buf_ready = TRUE), \
                                  !setjmp((ctxt)->jump_buffer))
#define XMLB_RECOVER else
typedef void (*UNPARSE_FUNC) (void*,char*);
void xmlb_write_to_file (void * ctxt, char * a);
typedef struct
  {
    char *beg;
    char *end;
    char *current;
  } XMLB_SERIALIZE_CONTROL;
void xmlb_write_to_mem (void * ctxt, char * a);
void xmlb_compute_unparsed_length(void * ctxt, char * a);
#ifndef XMLB_MALLOC
#define XMLB_MALLOC(ctxt,size) xmlb_malloc(ctxt,size)
#endif
#ifndef XMLB_ALIGN_STEP
#define XMLB_ALIGN_STEP 4
#endif

#define XMLB_ALIGN(x)  (XMLB_ALIGN_STEP*((x + XMLB_ALIGN_STEP - 1)/XMLB_ALIGN_STEP))
#ifndef XMLB_COMMIT
#define XMLB_COMMIT(X,Y)
#endif
#ifndef XMLB_BREAK
#define XMLB_BREAK break;
#endif
#define XMLB_CURRENT(ctxt) (*(ctxt->ptr))
#define XMLB_NEXTCH(ctxt) (*(ctxt->ptr+1))
#define XMLB_ADVANCE(ctxt) {XMLB_ADVANCE_UNSAFE(ctxt); \
                            XMLB_CHECK_EOF(ctxt);}
#define XMLB_ADVANCE_UNSAFE(ctxt) {ctxt->ptr++;}
#define XMLB_BACK(ctxt) {ctxt->ptr--;}
#define XMLB_IS_ALNUM(ctxt) (isalnum(XMLB_CURRENT(ctxt)))
#define XMLB_MOVE_BACK(ctxt,i) {ctxt->ptr-=i;}
#define XMLB_RESTORE_PTR(ctxt,p) {ctxt->ptr=p;}
#ifdef XMLB_FLAT_MODE
#define XMLB_SET_SON
#else
#define XMLB_SET_SON(f,s)                       \
  if(f&&s)                                      \
    {                                           \
      XMLB_SET_FATHER(s,f);                     \
      XMLB_SET_SIBLING(s,(XMLB_MINIMAL_HEADER*)f->first_son);   \
      XMLB_SET_FIRST_SON(f,s);                  \
    }
#endif
#define XMLB_ACCEPT(ctxt,c,str)                         \
     {                                              \
       if(XMLB_CURRENT(ctxt)==c)                    \
         XMLB_ADVANCE_UNSAFE(ctxt)                  \
        else                                        \
         {                                          \
           XMLB_FAIL(ctxt,(char*)"Character expected:"str);    \
         }                                          \
     }
#define XMLB_SKIP(ctxt,c)                         \
     {                                              \
       if(XMLB_CURRENT(ctxt)==c)                    \
         XMLB_ADVANCE_UNSAFE(ctxt)                  \
     }
#define XMLB_ACCEPT_NL XMLB_ACCEPT(ctxt,'\n',"(NewLine)")
#define XMLB_SKIP_NL   XMLB_SKIP(ctxt,'\n')
#define XMLB_EOF(ctxt)  (ctxt->ptr >= ctxt->last ? TRUE : FALSE)
BOOLEAN xmlb_weak_eof(XMLB_CONTEXT ctxt);
#define XMLB_CHECK_EOF(ctxt)                        \
   {                                                \
     if(XMLB_EOF(ctxt))                             \
       XMLB_FAIL(ctxt,(char*)"Unexpected end of file\n");  \
   }
#define XMLB_LOOKAHEAD_STR(ctxt,p,len)              \
  (memcmp(ctxt->ptr,p,len) ==0?((ctxt->ptr+=len),TRUE):FALSE)
#define XMLB_OPENINGTAG_STR(ctxt,p,len)   (                          \
    (                                                                \
       (memcmp(ctxt->ptr,p,len) ==0) && !isalnum(*(ctxt->ptr+len))   \
    )?                                                               \
       ((ctxt->ptr+=len),TRUE):FALSE)
#define XMLB_ACCEPT_STR(ctxt,p,len)                 \
    {                                               \
      if(!XMLB_LOOKAHEAD_STR(ctxt,p,len))           \
        XMLB_FAIL(ctxt,(char*)"String expected: "p);     \
    }
#define XMLB_ACCEPT_NC_STR(ctxt,p,len)              \
    {                                               \
      if(!XMLB_LOOKAHEAD_STR(ctxt,p,len))           \
        XMLB_FAIL(ctxt,(char*)"String expected[2]");       \
    }
#define XMLB_SKIP_TILL(ctxt,q)                          \
    {                                                   \
      char * t;                                         \
      t = (char*)memchr(ctxt->ptr,q,                    \
                        (ctxt->last - ctxt->ptr));      \
      if(t == NULL)                                     \
        XMLB_FAIL(ctxt, (char*)"Unexpected end of file");    \
      ctxt->ptr = t;                                    \
    }
#define XMLB_ACCEPT_QUOTE(ctxt,xmlb_quote)            \
    {                                                 \
      if (XMLB_CURRENT(ctxt) == QUOTE1 ||             \
          XMLB_CURRENT(ctxt) == QUOTE2)               \
        {                                             \
          xmlb_quote = XMLB_CURRENT(ctxt);            \
          XMLB_ADVANCE_UNSAFE(ctxt);                  \
        }                                             \
      else                                            \
        {                                             \
          XMLB_FAIL(ctxt, (char*)"Quote expected");          \
        }                                             \
    }
#define XMLB_UNPARSE_CHAR(uf,ctxt,p) xmlb_unparse_char(uf,ctxt,p)
#ifndef SWIG
void * xmlb_malloc(XMLB_CONTEXT, unsigned);
void   xmlb_free (XMLB_CONTEXT, void * ptr, unsigned len);
char * xmlb_strdupl(XMLB_CONTEXT ctxt, char *p, int len);
char * xmlb_strdup(XMLB_CONTEXT, char *);
#endif
#ifndef SWIG
void   xmlb_fetch_buffer(XMLB_CONTEXT, char *, int, char);
char * xmlb_slice_from (XMLB_CONTEXT, char*);
int    xmlb_fetch_pcdata (XMLB_CONTEXT ctxt,
                          char *target,
                          int max_size);
char * xmlb_fetch_dyn_pcdata (XMLB_CONTEXT ctxt);
int    xmlb_check_int_min(XMLB_CONTEXT ctxt, int min, int val);
int    xmlb_check_int_max(XMLB_CONTEXT ctxt, int max, int val);
int    xmlb_fetch_int(XMLB_CONTEXT);
long   xmlb_check_long_min(XMLB_CONTEXT ctxt, long min, long val);
long   xmlb_check_long_max(XMLB_CONTEXT ctxt, long max, long val);
long   xmlb_fetch_long(XMLB_CONTEXT);
#ifndef XMLB_NOFLOAT
XMLB_DOUBLE xmlb_check_double_min(XMLB_CONTEXT ctxt, XMLB_DOUBLE min, XMLB_DOUBLE val);
XMLB_DOUBLE xmlb_check_double_max(XMLB_CONTEXT ctxt, XMLB_DOUBLE max, XMLB_DOUBLE val);
XMLB_DOUBLE xmlb_fetch_double(XMLB_CONTEXT);
#endif
BOOLEAN xmlb_fetch_boolean (XMLB_CONTEXT ctxt);
int    xmlb_used_buffer(XMLB_CONTEXT);
char * xmlb_fetch_pchar(XMLB_CONTEXT, char);
void   xmlb_skip_until (XMLB_CONTEXT ctxt, char c);
void   xmlb_fail(XMLB_CONTEXT ctxt, char *msg);
void   xmlb_notify_err(XMLB_CONTEXT ctxt, char *msg);
char   xmlb_accept_quote(XMLB_CONTEXT ctxt);
int    xmlb_accept_hexa(XMLB_CONTEXT);
#endif
#ifndef SWIG
void xmlb_unparse_int (UNPARSE_FUNC uf, void * ctxt, int value);
void xmlb_unparse_long (UNPARSE_FUNC uf, void * ctxt, long value);
#ifndef XMLB_NOFLOAT
void xmlb_unparse_real (UNPARSE_FUNC uf, void * ctxt, XMLB_DOUBLE value);
#endif
void xmlb_unparse_char(UNPARSE_FUNC uf, void * ctxt, char p);
#endif
#define XMLB_SKIP_WHITE_SPACE(ctxt) xmlb_skip_white_space(ctxt);
#ifndef XMLB_FAIL
#define XMLB_FAIL(ctxt,msg) xmlb_fail(ctxt,(char*)msg);
#endif

#undef XMLB_ACCEPT_QUOTE
#define XMLB_ACCEPT_QUOTE(ctxt,xmlb_quote) \
              xmlb_quote=xmlb_accept_quote(ctxt);
extern char * xmlb_cleanup_line_feeds(char * str);
#ifndef SWIG
void   xmlb_skip_white_space (XMLB_CONTEXT ctxt);
void   xmlb_fetch_until(XMLB_CONTEXT ctxt,
            char * search,
            char * buff,
            unsigned limit_size);
char * xmlb_accept_until(XMLB_CONTEXT ctxt,
            char * text);
#endif
typedef enum WS_MODE
  {
    WS_PRESERVE,
    WS_REPLACE,
    WS_COLLAPSE
  } WS_MODE;
#endif

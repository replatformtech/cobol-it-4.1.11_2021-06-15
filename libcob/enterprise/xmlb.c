/*CIT_BEGIN_ENTERPRISE*/
#include <xmlb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef XMLB_DYNAMIC
static void xmlb_new_page(XMLB_CONTEXT ctxt)
{
  XMLB_PAGE p;

  p = (XMLB_PAGE) malloc (sizeof(S_XMLB_PAGE));
  // memset (p, 0, sizeof(S_XMLB_PAGE));



  p->used = 0;
  p->aligned.d = 0;
  p->aligned.ptr = NULL;
  p->aligned.u = 0;

  p->next = ctxt->page;
  ctxt->page = p;
}
/*****************************************/
static void * xmlb_malloc_in_page(XMLB_PAGE page, unsigned size)
{
  void * res;
  unsigned corr;

  if (size + page->used > PAGE_SIZE)
    return NULL;
  res = & page->aligned.buffer[page->used];
  page->used += size;
  corr = page->used % XMLB_ALIGN_STEP;
  if (corr != 0)
    {
      page->used += (XMLB_ALIGN_STEP - corr);
      /* assert(page->used % ALIGN_STEP == 0); */
    }
  /* assert(((unsigned)res) % ALIGN_STEP == 0); */
  return res;
}
/*****************************************/
static void xmlb_free_pages(XMLB_CONTEXT ctxt)
{
  XMLB_PAGE page, p;
  page = ctxt->page;
  while (page != NULL)
  {
    p = page;
    page = page->next;
    free(p);
  }
  ctxt->page = NULL;
}
/*****************************************/
static void xmlb_free_input_buffer(XMLB_CONTEXT ctxt)
{
  if (ctxt->owned_input_buffer != NULL)
  {
    free(ctxt->owned_input_buffer);
    ctxt->owned_input_buffer = NULL;
  }
}
/*****************************************/
static void xmlb_alloc_input_buffer(XMLB_CONTEXT ctxt, unsigned size)
{
  xmlb_free_input_buffer(ctxt);
  ctxt->owned_input_buffer = (char *) malloc (size + 300);
}
#endif

static long xmlb_file_length(FILE *p)
{
   long res;

   fseek(p, 0L, SEEK_END);
   res = ftell(p);
   fseek(p, 0L, SEEK_SET);
   return res;
}
/**************************************/
#ifndef XMLB_DYNAMIC
int xmlb_available (XMLB_CONTEXT ctxt)
{
  return ctxt->malloc_top - ctxt->malloc_last;
}
#endif
/**************************************/
void * xmlb_malloc(XMLB_CONTEXT ctxt, unsigned size)
{
  void * res;
#ifndef XMLB_DYNAMIC
  res = ctxt->malloc_last;
  size = XMLB_ALIGN(size);
  ctxt->malloc_last += size;
  if (ctxt->malloc_last >= ctxt->malloc_top)
    {
      XMLB_FAIL(ctxt, "Buffer full");
    }
#else
  if (size > PAGE_SIZE)
    {
      XMLB_FAIL(ctxt, "Allocation exceeds buffer size");
    }
  if (ctxt-> page == NULL)
    xmlb_new_page (ctxt);
  res = xmlb_malloc_in_page (ctxt->page, size);
  if (res == NULL)
    {
      xmlb_new_page (ctxt);
      res = xmlb_malloc_in_page (ctxt->page, size);
      assert(res != NULL);
    }
#endif
  return res;
}
/*******************************************************/
void   xmlb_free (XMLB_CONTEXT ctxt, void * ptr, unsigned len)
{
#ifndef XMLB_DYNAMIC
  if ( (char *)ptr >= ctxt->malloc_first &&
      (char *)ptr + XMLB_ALIGN(len) <= ctxt->malloc_last)
      ctxt->malloc_last = ptr;
#endif
}
/*******************************************************/
char * xmlb_strdupl(XMLB_CONTEXT ctxt, char *p, int len)
{
  char * res;

#ifndef XMLB_STRDUP_NO_STRICT
  if (len == 0 || p == NULL || *p == '\0')
#else
  if (p == NULL)
#endif
    return NULL;
  res = (char *) xmlb_malloc (ctxt, len+1);
  memcpy (res, p, len);
  res[len] = '\0';
  return res;
}
/*******************************************************/
char * xmlb_strdup(XMLB_CONTEXT ctxt, char *p)
{
  return xmlb_strdupl(ctxt, p, strlen(p));
}
/*******************************************************/
char*  xmlb_slice_from(XMLB_CONTEXT ctxt, char * ptr)
{
  return xmlb_strdupl (ctxt, ptr, ctxt->ptr - ptr);
}
/*******************************************************/
void   xmlb_fetch_buffer(XMLB_CONTEXT ctxt, char * buff, int size, char term)
{
  char *p;
  int len;

  p = (char *) memchr(ctxt->ptr,term,ctxt->last - ctxt->ptr);
  if (p == NULL)
    XMLB_FAIL(ctxt, "Unexpected end of buffer[2]");
  len = (p-ctxt->ptr);
  if ((p == NULL) || (len > size))
    XMLB_FAIL(ctxt, "Buffer size overflow");
  memcpy (buff, ctxt->ptr, len);
  buff[len] = 0;
  ctxt->ptr = p;
}
/*******************************************************/
int xmlb_check_int_min(XMLB_CONTEXT ctxt, int min, int val)
{
  if (val < min)
    xmlb_fail (ctxt, "The inclusive lower bound failure");

  return val;
}

/*******************************************************/
int xmlb_check_int_max(XMLB_CONTEXT ctxt, int max, int val)
{
  if (val > max)
    xmlb_fail (ctxt, "The inclusive upper bound failure");

  return val;
}
/*******************************************************/
int    xmlb_fetch_int(XMLB_CONTEXT ctxt)
{
  BOOLEAN negate=FALSE;
  int res;
  char c;

  xmlb_skip_white_space (ctxt);
  res = 0;
  if (XMLB_CURRENT(ctxt) == '-')
    {
      negate = TRUE;
      XMLB_ADVANCE(ctxt);
    }
   else if (XMLB_CURRENT(ctxt) == '+')
    {
      XMLB_ADVANCE(ctxt);
    }
  while (isdigit(c = XMLB_CURRENT(ctxt)))
    {
      res = (res*10)+(c-'0');
      XMLB_ADVANCE(ctxt);
    }


  if (negate)
    res = -res;

  return res;
}
/*******************************************************/
long xmlb_check_long_min(XMLB_CONTEXT ctxt, long min, long val)
{
  if (val < min)
    xmlb_fail (ctxt, "The inclusive lower bound failure");

  return val;
}
/*******************************************************/
long xmlb_check_long_max(XMLB_CONTEXT ctxt, long max, long val)
{
  if (val > max)
    xmlb_fail (ctxt, "The inclusive upper bound failure");

  return val;
}
/*******************************************************/
long    xmlb_fetch_long(XMLB_CONTEXT ctxt)
{
  BOOLEAN negate=FALSE;
  long res;
  char c;

  xmlb_skip_white_space (ctxt);
  res = 0L;
  if (XMLB_CURRENT(ctxt) == '-')
    {
      negate = TRUE;
      XMLB_ADVANCE(ctxt);
    }
   else if (XMLB_CURRENT(ctxt) == '+')
    {
      XMLB_ADVANCE(ctxt);
    }
  while (isdigit(c = XMLB_CURRENT(ctxt)))
    {
      res = (res*10)+(c-'0');
      XMLB_ADVANCE(ctxt);
    }
  if (negate)
    return -res;
   else
    return res;
}
/*******************************************************/
#ifndef XMLB_NOFLOAT
XMLB_DOUBLE xmlb_check_double_min(XMLB_CONTEXT ctxt, XMLB_DOUBLE min, XMLB_DOUBLE val)
{
  if (val < min)
    xmlb_fail (ctxt, "The inclusive lower bound failure");

  return val;
}

/*******************************************************/
XMLB_DOUBLE xmlb_check_double_max(XMLB_CONTEXT ctxt, XMLB_DOUBLE max, XMLB_DOUBLE val)
{
  if (val > max)
    xmlb_fail (ctxt, "The inclusive upper bound failure");

  return val;
}
/*******************************************************/
XMLB_DOUBLE xmlb_fetch_double(XMLB_CONTEXT ctxt)
{
  int exp = 0;
  char c;
  XMLB_DOUBLE res = 0.0;
  XMLB_DOUBLE mult = 0.1;
  int sign = 1;
  int i;

  if (XMLB_CURRENT(ctxt) == '-')
    {
      sign = -1;
      XMLB_ADVANCE(ctxt);
    }
   else if (XMLB_CURRENT(ctxt) == '+')
      XMLB_ADVANCE(ctxt);
  while (isdigit(c = XMLB_CURRENT(ctxt)))
    {
      res = (res*10.0)+(XMLB_DOUBLE)(c-'0');
      XMLB_ADVANCE(ctxt);
    }
  if (XMLB_CURRENT(ctxt) == '.')
  {
    XMLB_ADVANCE(ctxt);
    while (isdigit(c = XMLB_CURRENT(ctxt)))
      {
        res += mult * (XMLB_DOUBLE)(c-'0');
        mult /= 10.0;
        XMLB_ADVANCE(ctxt);
      }
  }
  c = XMLB_CURRENT(ctxt);
  if (c == 'e' || c == 'E')
  {
    XMLB_ADVANCE(ctxt);
    exp = xmlb_fetch_int(ctxt);
    if (exp > 0)
      {
        for (i=0; i < exp; i++)
          res *= 10.0;
      }
     else
      {
        for (i=0; i > exp; i--)
          res /= 10.0;
      }
  }
  return sign * res;
}

/*********************************************************/
void xmlb_unparse_real (UNPARSE_FUNC uf, void * ctxt, XMLB_DOUBLE value)
{
  char a[40];
  char *p, *q;

  sprintf (a, "%10.10f", value);
  p = a;
  while (*p == ' ')
  {
    p++;
  }
  q = p + strlen(p);
  while (*(q-1) == '0' && *(q-2) != '.')
  {
    q--;
    *q = 0;
  }
  uf (ctxt, p);
}
#endif
/*******************************************************/
char * xmlb_fetch_pchar(XMLB_CONTEXT ctxt, char quote_char)
{
  char temp [2048];
  char *p;

  p = temp;
  while (XMLB_CURRENT(ctxt) != quote_char)
    {
      *p = XMLB_CURRENT(ctxt);
      p++;
      XMLB_ADVANCE(ctxt);
    }
  XMLB_CHECK_EOF(ctxt);
  *p = 0;
  return xmlb_strdup (ctxt, temp);
}

#define LAST(v) (&(v[sizeof(v)/sizeof(v[1])-1]))

/*
static char * put_buffer_padding (char * a)
{
  int i;

  for (i=0; i<=255; i++)
    {
      *a = i;
      a++;
    }
  return a;
}
*/

#ifndef XMLB_DYNAMIC
static void ajust_malloc_last (XMLB_CONTEXT ctxt)
  {
      while ((ctxt->malloc_last - ctxt->first) % XMLB_ALIGN_STEP != 0)
        ctxt->malloc_last ++;
      ctxt->malloc_first = ctxt->malloc_last;
      ctxt->malloc_top = LAST(ctxt->aligned.buffer);
  }
#endif

static void xmlb_reinit_context(XMLB_CONTEXT ctxt)
{
  ctxt->jmp_buf_ready = FALSE;
  ctxt->err_msg = NULL;
  ctxt->err_ptr = NULL;
}

#define FILLING_SIZE 500

/*********************************************************/
void xmlb_restart(XMLB_CONTEXT ctxt)
{
  xmlb_reinit_context (ctxt);
  ctxt->ptr = ctxt->first;
#ifdef XMLB_DYNAMIC
  xmlb_free_pages(ctxt);
#else
  ctxt->malloc_last = ctxt->malloc_first;
#endif
}
/*********************************************************/
static BOOLEAN  local_load(XMLB_CONTEXT ctxt, char * fname, char * mode)
{
  FILE *f;
  int i;
  unsigned l;
  unsigned char c;

  xmlb_reinit_context(ctxt);
  f = fopen(fname, mode);
  if (f == NULL)
    return FALSE;
#ifdef XMLB_DYNAMIC
  l = xmlb_file_length(f);
  xmlb_alloc_input_buffer(ctxt, l);
  ctxt->first = ctxt->owned_input_buffer;
  i = fread (ctxt->first, 1, l, f);
  fclose (f);
#else
  ctxt->first = ctxt->aligned.buffer;
  i = fread (ctxt->first, 1, BUFFER_SIZE, f);
  fclose (f);
  if (i + FILLING_SIZE >= BUFFER_SIZE)
    {
      fprintf (stderr, "Buffer full while reading %s!\n", fname);
      return FALSE;
    }
#endif
  ctxt->ptr = ctxt->first;
  ctxt->last = ctxt->first + i;
#ifndef XMLB_DYNAMIC
  ctxt->malloc_last = ctxt->last + 17;
  ajust_malloc_last (ctxt);
#endif
  return TRUE;
}
/*********************************************************/
BOOLEAN  xmlb_load(XMLB_CONTEXT ctxt, char * fname)
{
  return local_load (ctxt, fname, "r");
}
/*********************************************************/
BOOLEAN  xmlb_load_binary(XMLB_CONTEXT ctxt, char * fname)
{
  return local_load (ctxt, fname, "rb");
}
/*********************************************************/
void xmlb_set_parse_string(XMLB_CONTEXT ctxt, char * ptr)
{
  int len;


  xmlb_reinit_context(ctxt);
  len = strlen(ptr);
#ifdef XMLB_DYNAMIC
  xmlb_alloc_input_buffer (ctxt, len);
  ctxt->first = ctxt->owned_input_buffer;
#else
  if (len + FILLING_SIZE >= BUFFER_SIZE)
    {
        fprintf (stderr, "Buffer too small !\n");
        return;
    }
  ctxt->first = ctxt->aligned.buffer;
#endif
  ctxt->ptr = ctxt->first;
  memcpy (ctxt->first, ptr, len);
  ctxt->last = ctxt->first + len;
#ifndef XMLB_DYNAMIC
  ctxt->malloc_last = ctxt->last + 17;
  ajust_malloc_last(ctxt);
#endif
}
/*********************************************************/
void   xmlb_set_parse_buffer(XMLB_CONTEXT ctxt, char * beg, char * end)
{
  xmlb_reinit_context(ctxt);
  ctxt->first = beg;
  if (end == NULL)
    end = beg + strlen (beg);
  ctxt->last = end;
  ctxt->ptr = beg;
#ifdef XMLB_DYNAMIC
  xmlb_free_pages(ctxt);
  xmlb_free_input_buffer (ctxt);
#else
  ctxt->malloc_last = ctxt->aligned.buffer;
  ajust_malloc_last(ctxt);
#endif
}
/*********************************************************/
void   xmlb_reset_context(XMLB_CONTEXT ctxt)
{
  unsigned i;
#ifdef XMLB_DYNAMIC
  xmlb_free_pages(ctxt);
#else
  i = ctxt->malloc_last - ctxt->malloc_first;
  ctxt->malloc_last = ctxt->malloc_first;
  memset (ctxt->malloc_last, 0, i);
#endif
  ctxt->ptr = ctxt->first;
  xmlb_reinit_context (ctxt);
}
/*********************************************************/
void   xmlb_init_context(XMLB_CONTEXT ctxt)
{
  memset (ctxt, 0, sizeof(S_XMLB_CONTEXT));
#ifdef XMLB_DYNAMIC
  ctxt->page = NULL;
  ctxt->owned_input_buffer = NULL;
  ctxt->ptr = NULL;
  ctxt->first = NULL;
  ctxt->last = NULL;
#else
  ctxt->ptr = ctxt->aligned.buffer;
  ctxt->first = ctxt->ptr;
  ctxt->last = ctxt->ptr;
  ctxt->malloc_last = ctxt->last + 17;
  ajust_malloc_last(ctxt);
  ctxt->malloc_top = LAST(ctxt->aligned.buffer);
#endif
  assert(ctxt->err_ptr == NULL);
}
/*********************************************************/
void   xmlb_dispose_context(XMLB_CONTEXT ctxt)
{
#ifdef XMLB_DYNAMIC
  xmlb_free_pages (ctxt);
  xmlb_free_input_buffer(ctxt);
#endif
  memset (ctxt, 0, sizeof(S_XMLB_CONTEXT));
}
/*********************************************************/
void   xmlb_dump_state(XMLB_CONTEXT ctxt)
{
  char *from, *to, *ref;

  if (ctxt->err_ptr)
    ref = ctxt->err_ptr;
   else
    ref = ctxt->ptr;

  printf ("%d:%d:", ref - ctxt->first,
                    ctxt->last - ctxt->first);
  from = ref - 40;
  if (from < ctxt->first)
    from = ctxt->first;
  to = ref + 40;
  if (to > ctxt->last)
    to = ctxt->last;
  printf ("...");
  while (from < to)
    {
      if (from == ref)
        printf ("^");
      printf ("%c", *from);
      from ++;
    }
  printf ("...\n");
}
/*********************************************************/
#ifndef XMLB_DYNAMIC
int xmlb_used_buffer(XMLB_CONTEXT ctxt)
{
  return ctxt->malloc_last - ctxt->first;
}
#endif
/*********************************************************/
void xmlb_skip_until (XMLB_CONTEXT ctxt, char c)
{
  XMLB_SKIP_TILL(ctxt,c);
}
/*********************************************************/
void xmlb_skip_white_space (XMLB_CONTEXT ctxt)
{
  while (TRUE)
    {
      while (XMLB_CURRENT(ctxt) <= ' ')
        XMLB_ADVANCE_UNSAFE(ctxt);
      XMLB_CHECK_EOF(ctxt);
      if (XMLB_CURRENT(ctxt) != '<')
        return;
      if (XMLB_LOOKAHEAD_STR(ctxt,"<!--",4))
        {
          BOOLEAN go_on = TRUE;
          while(go_on)
            {
              XMLB_SKIP_TILL(ctxt, '-');
              if (XMLB_LOOKAHEAD_STR(ctxt, "-->", 3))
                go_on = FALSE;
               else
                XMLB_ADVANCE_UNSAFE(ctxt);
              XMLB_CHECK_EOF(ctxt);
            }
        }
       else
        {
          char n = XMLB_NEXTCH(ctxt);
          if (n == '?' || n == '!')
            {
              int level = 1;
              while (level > 0)
                {
                  XMLB_ADVANCE_UNSAFE(ctxt);
                  XMLB_CHECK_EOF(ctxt);
                  if (XMLB_CURRENT(ctxt) == '<')
                    level ++;
                   else if (XMLB_CURRENT(ctxt) == '>')
                    level --;
                }
              XMLB_ADVANCE_UNSAFE(ctxt);
              XMLB_CHECK_EOF(ctxt);
            }
           else
            return;
        }
    }
}
/*********************************************************/
void xmlb_notify_err(XMLB_CONTEXT ctxt, char *msg)
{
  if (ctxt->ptr > ctxt->err_ptr)
    {
      ctxt->err_ptr = ctxt->ptr;
      ctxt->err_msg = msg;
    }
  /* xmlb_dump_state(ctxt); */
}
/*********************************************************/
void xmlb_fail2(XMLB_CONTEXT ctxt, char *msg, char *param)
  {
    xmlb_notify_err(ctxt, msg);
    if (ctxt->jmp_buf_ready)
      {
        longjmp (ctxt->jump_buffer, 7);
      }
    printf("%s%s\n", msg, param);
    xmlb_dump_state(ctxt);
    exit(1);
  }

/*********************************************************/
void xmlb_fail(XMLB_CONTEXT ctxt, char *msg)
  {
    xmlb_fail2(ctxt, msg, "");
  }
/*********************************************************/
char xmlb_accept_quote(XMLB_CONTEXT ctxt)
{
  char res;

  if (XMLB_CURRENT(ctxt) == QUOTE1 || XMLB_CURRENT(ctxt) == QUOTE2)
    {
      res = XMLB_CURRENT(ctxt);
      XMLB_ADVANCE_UNSAFE(ctxt);
      return res;
    }
   else
    {
      XMLB_FAIL(ctxt, "Quote expected");
    }
  return 0;
}
/*********************************************************/
static int hex_digit(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}
/*********************************************************/
int xmlb_accept_hexa(XMLB_CONTEXT ctxt)
{
  char ch;
  int res;
  int i;

  ch = XMLB_CURRENT(ctxt);
  res = hex_digit(ch);
  if (res < 0)
    return -1;
  XMLB_ADVANCE(ctxt);
  ch = XMLB_CURRENT(ctxt);
  i = hex_digit(ch);
  if (i >= 0)
  {
    res = (16 * res) + i;
    XMLB_ADVANCE(ctxt);
  }
  return res;
}
/*********************************************************/
void xmlb_fetch_until(XMLB_CONTEXT ctxt,
                      char * search,
                      char * buff,
                      unsigned limit_size)
{
  char * start, *stop;
  char first, c;
  int len;

  start = ctxt->ptr;
  len = strlen (search);
  first = *search;
  /* printf ("(%s|", search); */
  while (TRUE)
    {
      /* printf ("."); */
      XMLB_SKIP_TILL(ctxt,first);
      XMLB_CHECK_EOF(ctxt);
      stop = ctxt->ptr;
      if (XMLB_LOOKAHEAD_STR(ctxt,search,len))
        {
          if (buff != NULL)
            {
              len = stop - start;
              if (len > limit_size)
                XMLB_FAIL(ctxt, "Buffer overflow while fetching alien text");
              c = *stop;
              *stop = 0;
              strcpy (buff, start);
              *stop = c;
            }
          /* printf ("%d)", len); */
          return;
        }
       else
        XMLB_ADVANCE_UNSAFE(ctxt);
    }
}
/*********************************************************/
char * xmlb_accept_until(XMLB_CONTEXT ctxt, char * text)
{
  char buff[4*2048];
  xmlb_fetch_until(ctxt,text,buff,4*2048-4);
  return xmlb_strdup(ctxt,buff);
}
/*********************************************************/
void xmlb_unparse_int (UNPARSE_FUNC uf, void * ctxt, int value)
{
  char a[40];

  sprintf (a, "%d", value);
  uf (ctxt, a);
}
/*********************************************************/
void xmlb_unparse_long (UNPARSE_FUNC uf, void * ctxt, long value)
{
  char a[40];

  sprintf (a, "%ld", value);
  uf (ctxt, a);
}
/*********************************************************/
void xmlb_unparse_char(UNPARSE_FUNC uf, void * ctxt, char p)
{
  if (isprint(p) || isspace(p))
  {
    char buff[2];
    buff[1] = 0;
    buff[0] = p;
    uf (ctxt, buff);
  }
  else
  {
    char b[20];
    sprintf(b, "&#x%x;", (unsigned char) p);
    uf (ctxt, b);
  }
}
/*********************************************************/
void xmlb_write_to_file (void * ctxt, char * a)
{
  FILE * out;

  if (a != NULL) {
    out = (FILE *) ctxt;
    fputs (a, out);
  }
}
/*********************************************************/
void xmlb_compute_unparsed_length(void * ctxt, char * a)
{
  int * len = (int*) ctxt;
  if (a != NULL) {
    *len += strlen(a);
  }
}
/*********************************************************/
void xmlb_write_to_mem (void * ctxt, char * a)
{
  XMLB_SERIALIZE_CONTROL * ctrl;
  int len;

  if ( a != NULL ) {
    ctrl = (XMLB_SERIALIZE_CONTROL *) ctxt;
    if (ctrl->current == NULL)
      return;
    len = strlen (a);
    if (ctrl->current + len >= ctrl->end)
    {
      *(ctrl->current) = 0;
      ctrl->current = NULL;
      return;
    }
    memcpy (ctrl->current, a, len);
    ctrl->current += len;
    *ctrl->current = '\0';
  }
}
/*********************************************************/
char * xmlb_cleanup_line_feeds(char * str)
{
  char * p = str;
  char * q = str;
  while (*p)
    {
      if (*p != '\n')
        {
	  *q = *p;
	  q++;
        }
      p++;
    }
  *q = 0;
  return str;
}
/*********************************************************/
BOOLEAN xmlb_weak_eof(XMLB_CONTEXT ctxt)
{
  BOOLEAN res;
  while (! XMLB_EOF(ctxt) && isspace(XMLB_CURRENT(ctxt)))
   {
      XMLB_ADVANCE (ctxt);
   }
  res = XMLB_EOF(ctxt);
  /*
  if (! res)
    printf ("weak eof returned false !!!\n");
  */
  return res;
}

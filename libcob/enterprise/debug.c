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
#ifdef _MSC_VER
#  include <windows.h>
#  include <sddl.h>
#  include <WinError.h>
#  define pid_t DWORD
#endif

#include "config.h"
#include "defaults.h"
#include "globaldefine.h"
#include "defaults.h"

#include "e4c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif
#if HAVE_FCNTL_H
#  include    <fcntl.h>
#endif
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef	HAVE_SIGNAL_H
#  include <signal.h>
#endif
#include 	<ctype.h>
#include	<libcob.h>
#if defined(HAVE_PTHREAD_H)
#  include <sched.h>
#  include <pthread.h>

#endif



#include "termio.h"
#include "coblocal.h"
#include "citkey.h"
#include "debug.h"
#include "debugdb.h"
#ifdef HAVE_READLINE_READLINE_H
#  include "readline/readline.h"
#  include "readline/history.h"
#endif

#ifdef	HAVE_SIGNAL_H
typedef void (*cob_sighandler_t)(int);
#endif

#ifdef _MSC_VER
#  define DebuggerExitException		0xE0000010
#  define DebuggerThreadExitException 0xE0000011
#else
E4C_DEFINE_EXCEPTION(DebuggerExitException, "Debugger exit exception", RuntimeException);
E4C_DEFINE_EXCEPTION(DebuggerThreadExitException, "Debugger Thread exit exception", DebuggerExitException);
#endif

#define BUFF_SIZE 2048
static int debug_max_occur = 500;
static int debug_thread_no = 0;
int cob_debugdb_trace = 0;
#define debug_message if (cob_debugdb_trace) printf

//static debug_context       cob_debug_context = {0};
//static volatile int        cob_debug_thread_started = 0;
static void debug_display_line(COB_RTD, cob_module *mod, int start, int end);
void *cob_runtime_debugger_debugdb = NULL;
char *cob_runtime_debugger_debugdb_name = NULL;

char*
cob_basename(const char *name)
{
   const char *base = name;

   while (*name)
   {
      if (*name == '/' || *name == '\\')
      {
         base = name + 1;
      }
      name++;
   }
   return(char *)base;
}

#define MAX_MODULE_FIND_LOOP 50

static cob_module*
find_module(COB_RTD, const char *source)
{
   cob_module *mod;
   int cnt = MAX_MODULE_FIND_LOOP;
   char *p = cob_basename(source);
   mod = rtd->current_module;
   while (mod && cnt--)
   {
      cob_lines_debug_info *linfo = mod->lines_debug_info;
      if (linfo) {
          char *last_s = mod->source_file;
          for (;  linfo->line_nr; linfo++)
          {
             if (linfo->source_file != last_s)
             {
                if (strcasecmp(cob_basename(linfo->source_file), p) == 0)
                {
                   return mod;
                }
                last_s = (char *)linfo->source_file;
             }
          }
      }
      mod = COB_NEXT_MODULE(mod);
   }
   return (NULL);
}

static cob_module*
find_module_name(COB_RTD, char *module_name)
{
   cob_module *mod;
   int cnt = MAX_MODULE_FIND_LOOP;
   mod = rtd->current_module;
   while (mod && cnt--)
   {
      if (strcasecmp(mod->module_name, module_name) == 0)
      {
         return (mod);
      }
      mod = COB_NEXT_MODULE(mod);
   }
   return (NULL);
}

static void
debug_nano_wait(void)
{
#  ifdef _WIN32
   Sleep(1);
#  else
   struct timespec tsec;
   tsec.tv_sec = 0;
   tsec.tv_nsec = 20000000;
   nanosleep(&tsec, NULL);
#  endif
}

static int
debug_find_function_idx(cob_module *mod, char *function)
{
   int i = 0;
   if (mod->lines_debug_info && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
   {
      while (mod->lines_debug_info[i].line_nr)
      {
         if (mod->lines_debug_info[i].label &&
             strcasecmp(function, mod->lines_debug_info[i].label) == 0)
         {
            return (i);
         }
         i++;
      }
   }
   return (-1);
}

static int
debug_find_line_idx(cob_module *mod, const char *source, int linenr) 
{
   if (mod->lines_debug_info && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
   {
      if (mod->lines_debug_info[0].line_nr)
      {
          char *src = cob_basename(source);
          int src_ok = !strcasecmp(cob_basename(mod->lines_debug_info[0].source_file), src);
          int i = 0;
          int j = i + 1;
          while (mod->lines_debug_info[j].line_nr) 
          {
              if (mod->lines_debug_info[j].source_file == mod->lines_debug_info[i].source_file) 
              {
                  if (src_ok) {
                      if (linenr >= mod->lines_debug_info[i].line_nr && linenr < mod->lines_debug_info[j].line_nr)
                      {
                          return i;
                      }
                  }
              }
              else
              {
                  if (src_ok) 
                  {
                      if (linenr == mod->lines_debug_info[i].line_nr) 
                      {
                          return i;
                      }
                      src_ok = 0;
                  }
                  else
                  {
                      src_ok = !strcasecmp(cob_basename(mod->lines_debug_info[j].source_file), src);
                  }
              }
              i = j++;
          }
          if (src_ok)
          {
              if (linenr == mod->lines_debug_info[i].line_nr) 
              {
                  return i;
              }
          }
       }
   }
   return -1;
}

static cob_ufield*
cob_debug_find_cob_field_1(COB_RTD,
                           cob_module *mod,
                           char *field_name,
                           cob_ufield *field_buffer,
                           char *full_field_name,
                           cob_field_debug_info **found_fdi,
                           int *enable_cache);
static cob_module* find_module_at_level(COB_RTD, int s);

static void
debug_extract_module_name(COB_RTD, debug_context *const dbc, char **p, cob_module **mod)
{
   if (*(*p) == '@')
   {
      char *tmp_strtok = NULL;
      *p = COB_STRTOK(*p + 1, ".", &tmp_strtok);
      if (*p)
      {
         *mod = find_module_name(rtd, *p);
         *p = COB_STRTOK(NULL, " ", &tmp_strtok);
      }
   }
   else
   {
      *mod = find_module_at_level(rtd, dbc->current_stack_frame);
   }
}
static cob_field*
debug_get_cob_field_from_name(COB_RTD, debug_context *const dbc, char *name, cob_ufield *found_cob_field)
{
   char buff[COB_SMALL_BUFF];
   char *p = buff;
   cob_module *mod = NULL;
   cob_ufield *res = NULL;

   strncpy(buff, name, COB_SMALL_BUFF);
   debug_extract_module_name(rtd, dbc, &p, &mod);
   if (p && mod)
   {
      if (mod->fields_debug_info)
      {
         if (cob_debug_find_cob_field_1(rtd, mod, p, found_cob_field, NULL, NULL, NULL))
         {
            if (found_cob_field->fld.data)
            {
               res = found_cob_field;
            }
         }
      }
   }
   return(cob_field *)res;
}

static void
debug_reload_breakpoint(COB_RTD, int EnteringModule)
{
   struct break_point *br;
   cob_module *mod;
   int i;
   int condidx = 0;
   int cnt = 2;
   debug_context *const dbc = cob_get_debug_context(rtd);
   register cob_lines_debug_info *p;

   if (rtd->current_module)
   {
      dbc->flag_need_reload_breakpoint = 0;
      /*clean all breakpoint*/
      mod = rtd->current_module;
      while (mod && cnt--)
      {
         if (mod->lines_debug_info && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
         {
            p = mod->lines_debug_info;
            for (;  p->line_nr; p++)
            {
               p->debug_status = 0;
            }
         }
         mod = COB_NEXT_MODULE(mod);
      }
      memset(dbc->value_condition, 0, sizeof(dbc->value_condition));
      /* find module*/
      cob_lock_mutex();
      br = dbc->break_point;
      condidx = 0;
      while (br)
      {
         if (br->br_type)
         {
            if (br->br_type == COB_DEBUG_VALUE_CHANGE_BREAKPOINT)
            {
               if (condidx < COB_MAX_VALUE_CONDITION && br->condition.valid_condition && br->condition.varname)
               {
                  cob_ufield found_cob_field;
                  if (debug_get_cob_field_from_name(rtd, dbc, br->condition.varname, &found_cob_field) && found_cob_field.fld.data)
                  {
                     dbc->value_condition[condidx] = &br->condition;
                     br->condition.curvalue = found_cob_field.fld.data;
                     br->condition.size = found_cob_field.fld.size;
                     br->condition.refvalue = cob_malloc(rtd, found_cob_field.fld.size);
                     memcpy(br->condition.refvalue, br->condition.curvalue, br->condition.size);
                     condidx++;
                  }
               }
            }
            else
            {
               if (br->source)
               {
                  mod = find_module(rtd, br->source);
               }
               else
               {
                  mod = rtd->current_module;
               }
               if (mod)
               {
                  register cob_lines_debug_info *lineinfo = NULL;

                  cob_runtime_debugger_open_lines_debugdb(rtd, mod);
                  if (mod->lines_debug_info  && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
                  {
                     if (br->function)
                     {
                        i = debug_find_function_idx(mod, br->function);
                        if (i >= 0)
                        {
                           lineinfo = &mod->lines_debug_info[i];
                           lineinfo->debug_status = br->br_type;
                           if (br->br_type == COB_DEBUG_TEMPORARY_BREAKPOINT)
                           {
                              br->br_type = 0;
                           }
                        }
                     }
                     else if (br->linenr)
                     {
                        if (br->linenr > 0)
                        {
                           i = debug_find_line_idx(mod, br->source, br->linenr); 
                           if (i >= 0)
                           {
                              lineinfo = &mod->lines_debug_info[i];
                              lineinfo->debug_status = br->br_type;
                              if (br->br_type == COB_DEBUG_TEMPORARY_BREAKPOINT)
                              {
                                 br->br_type = 0;
                              }
                           }
                        }
                        else
                        {
                           if (EnteringModule && br->linenr == COB_DEBUG_BR_LINENR_MODULE_ENTRY)
                           {
                              /* just break at module entry */
                              dbc->flag_stop_next_statement = 1;
                              if (br->br_type == COB_DEBUG_TEMPORARY_BREAKPOINT)
                              {
                                 br->br_type = 0;
                              }
                           }
                        }
                     }
                     if (lineinfo)
                     {
                        if (br->condition.valid_condition && br->condition.no_break_count)
                        {
                           lineinfo->condition = &br->condition;
                        }
                        else
                        {
                           lineinfo->condition = NULL;
                        }
                     }

                  }
               }
               else if (br->linenr && br->source)
               {
                  char *pbn;
                  mod = rtd->current_module;
                  cob_runtime_debugger_open_lines_debugdb(rtd, mod);
                  if (mod && mod->lines_debug_info  && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
                  {
                     pbn = cob_basename(br->source);
                     p = mod->lines_debug_info;
                     for (; p->line_nr; p++)
                     {
                        if (p->line_nr == br->linenr)
                        {
                           if (strcasecmp(pbn, p->source_file) == 0)
                           {
                              p->debug_status = br->br_type;
                              if (br->br_type == COB_DEBUG_TEMPORARY_BREAKPOINT)
                              {
                                 br->br_type = 0;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
         else if (br->condition.valid_condition)
         {
            if (br->condition.varname)
            {
               free(br->condition.varname);
            }
            if (br->condition.refvalue)
            {
               cob_free(br->condition.refvalue);
            }
            memset(&br->condition, 0, sizeof(br->condition));
         }
         br = br->next;
      }
      cob_unlock_mutex();
   }
}

static void
flush_out(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (dbc->cob_db_trace)
   {
      fflush(dbc->cob_db_trace);
   }
   if (dbc->cob_db_out)
   {
      fflush(dbc->cob_db_out);
   }
   if (dbc->cob_db_event_out)
   {
      fflush(dbc->cob_db_event_out);
   }
}

static void
close_files(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (dbc->cob_db_event_out)
   {
      fclose(dbc->cob_db_event_out);
   }
   dbc->cob_db_event_out = NULL;
   if (dbc->cob_db_out)
   {
      fclose(dbc->cob_db_out);
   }
   dbc->cob_db_out = NULL;
   if (dbc->cob_db_in)
   {
      fclose(dbc->cob_db_in);
   }
   dbc->cob_db_in = NULL;
   if (dbc->cob_db_trace)
   {
      fprintf(dbc->cob_db_trace, "Closing Debugger interface \n");
   }
}

static void
fout(COB_RTD, const char *s, ...)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   int err;
   va_list l;
   if (!dbc->cob_db_out)
   {
      return;
   }
   va_start(l, s);
   err = ferror(dbc->cob_db_out);
   if (err)
   {
      return;
   }
   if (vfprintf(dbc->cob_db_out,   s, l) < 0)
   {
      err = ferror(dbc->cob_db_out);
      if (err)
      {
         if (dbc->cob_db_trace)
         {
            fprintf(dbc->cob_db_trace, "fout error : %d %s\n", err, cob_strerror(rtd, err));
         }
         close_files(rtd);
      }
   }
   fflush(dbc->cob_db_out);
   va_end(l);

   if (dbc->cob_db_trace)
   {
      va_start(l, s);
      vfprintf(dbc->cob_db_trace, s, l);
      va_end(l);
   }
}

static void
event_fout(COB_RTD, const char *s, ...)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   va_list l;

   if (!dbc->cob_db_event_out)
   {
      return;
   }
   va_start(l, s);
   if (vfprintf(dbc->cob_db_event_out,   s, l) < 0)
   {
      int err = ferror(dbc->cob_db_event_out);
      if (err)
      {
         if (dbc->cob_db_trace)
         {
            fprintf(dbc->cob_db_trace, "event_fout error : %d %s\n", err, cob_strerror(rtd, err));
         }
         close_files(rtd);
      }
   }
   fflush(dbc->cob_db_event_out);
   va_end(l);

   if (dbc->cob_db_trace)
   {
      va_start(l, s);
      vfprintf(dbc->cob_db_trace, s, l);
      va_end(l);
   }
}

static void
out_error(COB_RTD, const char *msg)
{
   if (!msg)
   {
      msg = "Unknown error";
   }
   fout(rtd, "error:%s\n", msg);
}

static char*
clean_quote(char *p)
{
   char *q;
   if (p)
   {
      while (*p == '"')
      {
         p++;
      }
      q = p + (strlen(p) - 1);
      while (*q == '"' && p < q)
      {
         *q = 0;
         q--;
      }
   }
   return (p);
}

static void
cmd_gdb_version(COB_RTD)
{
   fout(rtd, "~\"COBOL-IT cobcdb %s\\n\"\n", COB_VERSION);
   fout(rtd, "^done\n");
}

static void
cmd_kill(COB_RTD)
{
   /* do kill app*/
   cob_runtime_debugger_cleanup(rtd);
   if (rtd->current_module)
   {
      cob_terminate_exec(rtd);
      rtd->current_module = NULL;
      if (e4c_context_is_ready())
      {
         throw (DebuggerExitException, "cmd_kill");
      }
      exit(0);
   }
   else
   {
      out_error(rtd, "The program is not being run");
   }
}

COB_DLL_EXPIMP void cob_runtime_debugger_output_event(COB_RTD, const char *reason);
static void
actual_cmd_exit(COB_RTD, int exception)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   /* do kill app*/
   cob_runtime_debugger_output_event(rtd, "-event-program-exited");
   flush_out(rtd);
   cob_runtime_debugger_cleanup(rtd);
   dbc->flag_debugger_active = 0;
   cob_debug_callback = NULL;
   if (rtd->current_module)
   {
      cob_terminate_exec(rtd);
      rtd->current_module = NULL;
   }
   else
   {

   }
   cob_call_close_all_handles(rtd);
   dbc->flag_exit_debug = 1;
   if (e4c_context_is_ready())
   {
      if (exception)
      {
         throw (DebuggerExitException, "cmd_exec");
      }
      else
      {
         e4c_context_end();
      }
   }

   exit(0);
}

static void
cmd_exit(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
#  ifdef _MSC_VER
   dbc->flag_exit_debug = 1;
   ExitThread(0);
#  else
#     if HAVE_PTHREAD_H
   dbc->flag_exit_debug = 1;
   if (e4c_context_is_ready())
   {
      throw (DebuggerThreadExitException, "cmd_exit");
   }
   pthread_exit(NULL);
   exit(0);
#     else
   actual_cmd_exit(rtd, 1);
#     endif
#  endif
}

static void
cmd_detach(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   dbc->flag_continue_running = 1;
   close_files(rtd);
}

static void cmd_data_set(COB_RTD);
static void cmd_data_set_hex(COB_RTD);


static int
exec_prolog(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   cit_runtime_t *rtd_r = cob_enterprise_get_region(rtd, dbc->current_region);

   if (rtd_r && rtd_r->current_module)
   {
      return (1);
   }
   else
   {
      out_error(rtd, "The program is not being run.");
      return (0);
   }
}


static void
out_module_position(COB_RTD, char prefix, cob_module *mod, int level, int is_event)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   const char *format = "%c%d %s () at %s!%d\n";
   if (is_event)
   {
      event_fout(rtd, format, prefix, level, mod->module_name, (mod->debug_cur_source_file), mod->debug_cur_line);
      if (!dbc->flag_machine_command && !dbc->flag_disable_source)
      {
         debug_display_line(rtd, mod, mod->debug_cur_line, mod->debug_cur_line);
      }
   }
   else
   {
      fout(rtd, format, prefix, level, mod->module_name, (mod->debug_cur_source_file), mod->debug_cur_line);
   }
}



static void
out_breakpoint(COB_RTD, struct break_point *br)
{
   {
      int linenr = br->linenr;
      char Buffer[1024];
      if (br->condition.valid_condition && br->condition.no_break_count)
      {
         sprintf(Buffer, " count %d ", br->condition.no_break_count);
      }
      else
      {
         Buffer[0] = 0;
      }
      if (br->br_type == COB_DEBUG_VALUE_CHANGE_BREAKPOINT)
      {
         fout(rtd, "Breakpoint %d%s on value change %s \n", br->br_nr, Buffer, br->condition.varname);
      }
      else
      {
         if (br->function)
         {
            fout(rtd, "Breakpoint %d%s in %s at %s\n", br->br_nr, Buffer, br->function, br->source);
         }
         else
         {
            if (linenr < 0)
            {
               linenr = 0;
            }
            fout(rtd, "Breakpoint %d%s at %s ! %d\n", br->br_nr, Buffer, br->source, linenr);
         }
      }
   }
}



static void
cmd_break_delete(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char *s;
   s = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   if (s)
   {
      int nr;
      struct break_point *r;
      nr = atoi(s);

      cob_lock_mutex();
      r = dbc->break_point;
      while (r)
      {
         if (r->br_type && r->br_nr == nr)
         {
            r->br_type = 0;
            dbc->flag_need_reload_breakpoint = 1;
            /*reload_breakpoint (rtd, 0);*/
            cob_unlock_mutex();
            fout(rtd, "^done\n");
            return;
         }
         r = r->next;
      }
   }
   cob_unlock_mutex();
   out_error(rtd, "No Breakpoint");
}


static struct break_point*
cmd_break_add_breakpoint(COB_RTD, debug_context *const dbc)
{
   struct break_point *r;
   struct break_point *br;

   cob_lock_mutex();
   if (dbc->break_point)
   {
      r = dbc->break_point;
      while (r && r->br_type)
      {
         br = r;
         r = r->next;
      }
      if (r)
      {
         br = r;
         if (br->source) cob_free(br->source);
         if (br->function) cob_free(br->function);
         r->br_type = 0;
      }
      else
      {
         r = br;
         br = cob_malloc(rtd, sizeof(struct break_point));
         r->next = br;
      }
   }
   else
   {
      br = cob_malloc(rtd, sizeof(struct break_point));
      dbc->break_point = br;
   }
   dbc->breap_point_id++;
   br->br_nr = dbc->breap_point_id;
   cob_unlock_mutex();
   return br;
}


static
void cmd_break_insert_check_count(COB_RTD, struct break_point *br)
{
   char *t;
   t = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   if (t && strcmp(t, "count") == 0)
   {
      t = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (t && isdigit(*t))
      {
         int bcount = atoi(t);
         if (bcount > 0)
         {
            br->condition.no_break_count = bcount;
            br->condition.valid_condition = 1;
         }
      }
   }

}

static
void cmd_break_insert_check_duplicate(debug_context *const dbc, struct break_point *br)
{
   struct break_point *r;
   cob_lock_mutex();
   r = dbc->break_point;
   while (r)
   {
      if (r != br && r->br_type)
      {
         if (r->source && br->source && strcasecmp(r->source, br->source) == 0)
         {
            if (r->linenr && r->linenr == br->linenr)
            {
               break;
            }
            if (r->function && br->function && strcasecmp(r->function, br->function) == 0)
            {
               break;
            }
         }
         else if (r->condition.valid_condition && br->condition.valid_condition &&
                  r->condition.varname && br->condition.varname &&
                  strcasecmp(r->condition.varname, br->condition.varname) == 0)
         {
            break;
         }
      }
      r = r->next;
   }
   if (r)
   {
      br->br_nr = r->br_nr;
      r->br_type = 0;
   }
   cob_unlock_mutex();

}


static void
cmd_break_insert(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char *p;
   char *t;
   int is_tmp = 0;
   p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   if (p && strcmp(p, "-t") == 0)
   {
      is_tmp = 1;
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   }
   if (p && strcmp(p, "-v") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p)
      {
         struct break_point *br = cmd_break_add_breakpoint(rtd, dbc);
         br->br_type = COB_DEBUG_VALUE_CHANGE_BREAKPOINT;
         br->condition.varname = strdup(p);
         br->condition.valid_condition = 1;
         cmd_break_insert_check_duplicate(dbc, br);
         cmd_break_insert_check_count(rtd, br);
         out_breakpoint(rtd, br);
         dbc->flag_need_reload_breakpoint = 1;
      }
   }
   else if (p)
   {
      struct break_point *br = cmd_break_add_breakpoint(rtd, dbc);
      br->br_type = is_tmp ? COB_DEBUG_TEMPORARY_BREAKPOINT : COB_DEBUG_REGULAR_BREAKPOINT;
      t = strchr(p, '!');
      if (t)
      {
         *t = 0;
         if (strlen(p) == 0)
         {
            br->source = strdup(rtd->current_module->debug_cur_source_file); 
         }
         else
         {
            br->source = strdup(p);
         }
         t++;
         if (strlen(t) == 0)
         {
            t = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
         }
         if (t)
         {
            if (isdigit(*t))
            {
               int linenr = atoi(t);
               if (linenr > 0)
               {
                  br->linenr = linenr;
               }
               else
               {
                  br->linenr = COB_DEBUG_BR_LINENR_MODULE_ENTRY; /*ask for a break at module entry */
               }

            }
            else
            {
               br->function = strdup(t);
            }
         }
         else
         {
            out_error(rtd, "no break point defined.");

         }
      }
      else
      {
         br->function = strdup(p);
         if (rtd->current_module)
         {
            br->source = strdup(rtd->current_module->source_file);
         }
      }
      cmd_break_insert_check_duplicate(dbc, br);
      cmd_break_insert_check_count(rtd, br);
      out_breakpoint(rtd, br);
      dbc->flag_need_reload_breakpoint = 1;
      /*reload_breakpoint (rtd, 0);*/
   }
   else
   {
      out_error(rtd, "The program is not being run.");
   }

}

static void
cmd_thread_list_ids(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   {
      fout(rtd, "[Current thread is 1 (thread %d)]\n", dbc->external_pid);
   }

}

static void
cmd_exec_run(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char *p;
   char args[4096];

   if (exec_prolog(rtd))
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      strcpy(args, "");
      while (p)
      {
         strcat(args, p);
         strcat(args, " ");
      }
      cob_set_command_line(rtd, rtd->current_module->module_name, args);
      dbc->flag_continue_running = 1;
   }
}

static void
cmd_exec_continue(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (exec_prolog(rtd))
   {
      dbc->flag_continue_running = 1;
      event_fout(rtd, "-event-continue\n");
   }
}

static void
cmd_exec_next(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (exec_prolog(rtd))
   {
      dbc->flag_continue_running = 1;
      cob_runtime_debugger_open_lines_debugdb(rtd, rtd->current_module);
      if (rtd->current_module && rtd->current_module->lines_debug_info)
      {
         dbc->flag_go_next_statement = 1;
      }
      else
      {
         dbc->flag_go_next_statement = 0;
         dbc->flag_stop_next_statement = 1;
      }
      event_fout(rtd, "-event-next\n");
   }
}

static void
cmd_exec_stop(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (exec_prolog(rtd))
   {
      dbc->flag_go_next_statement = 0;
      dbc->flag_stop_next_statement = 1;
   }
}

static void
cmd_exec_return(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (exec_prolog(rtd))
   {
      dbc->flag_continue_running = 1;
      cob_runtime_debugger_open_lines_debugdb(rtd, rtd->current_module);
      if (rtd->current_module && rtd->current_module->lines_debug_info)
      {
         dbc->flag_go_next_statement = 1;
         dbc->perform_count = 1;

      }
      else
      {
         dbc->flag_go_next_statement = 0;
         dbc->flag_stop_next_statement = 1;
      }
      event_fout(rtd, "-event-contreturn\n");
   }
}

static void
cmd_exec_step(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (exec_prolog(rtd))
   {
      dbc->flag_continue_running = 1;
      dbc->flag_stop_next_statement = 1;
      event_fout(rtd, "-event-step\n");
   }
}

static cob_module*
find_module_at_level(COB_RTD, int s)
{
   int l = 0;
   if (rtd->current_module)
   {
      cob_module *mod = rtd->current_module;
      struct cob_stack_debug_info *dbx;
      if (mod)
      {
         mod->debug_cur_source_file = mod->cur_source_file;
         mod->debug_cur_line = mod->cur_line;
      }
      while (mod && (l < s))
      {
         if (mod->lines_debug_info  && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
         {
            dbx = mod->debug_stack_frame_curpos;
            while (dbx > mod->debug_stack_frame_start && (l < s))
            {

               if (dbx->line_nr)
               {
                  mod->debug_cur_source_file = dbx->source_file;
                  mod->debug_cur_line = dbx->line_nr;
               }
               dbx--;
               l++;
            }
         }
         if (l < s)
         {
            mod = COB_NEXT_MODULE(mod);
            l++;
            if (mod)
            {
               mod->debug_cur_source_file = mod->cur_source_file;
               mod->debug_cur_line = mod->cur_line;
            }
         }
      }
      if (s == (l))
      {
         return (mod);
      }
   }
   return (NULL);
}

static void
cmd_set(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   /*todo*/
   char *p;
   p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   if (p && strcasecmp(p, "prompt") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p)
      {
         dbc->prompt = strdup(p);
      }
      else
      {
         dbc->prompt = "(cobcdb)";
      }

   }
   else if (p && strcasecmp(p, "machine") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p && strcasecmp(p, "off") == 0)
      {
         dbc->flag_machine_command = 0;
      }
      else
      {
         dbc->flag_machine_command = 1;
      }
   }
   else if (p && strcasecmp(p, "sources") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p && strcasecmp(p, "off") == 0)
      {
         dbc->flag_disable_source = 1;
         dbc->flag_force_readline_command = -1;
      }
      else
      {
         dbc->flag_disable_source = 0;
      }
   }
   else if (p && strcasecmp(p, "readline") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p && strcasecmp(p, "off") == 0)
      {
         dbc->flag_force_readline_command = -1;
      }
      else
      {
         dbc->flag_force_readline_command = 1;
      }
   }
   else if (p && strcasecmp(p, "term") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p)
      {
#        ifdef HAVE_READLINE
         rl_terminal_name = strdup(p);
         rl_initialize();
#        endif
      }
   }
   else if (p && strcasecmp(p, "var") == 0)
   {
      cmd_data_set(rtd);
   }
   else if (p && strcasecmp(p, "varh") == 0)
   {
      cmd_data_set_hex(rtd);
   }
   else if (p && strcasecmp(p, "profiling") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p)
      {
         if (strcasecmp(p, "dump") == 0)
         {
            p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
         }
         if (p && strcasecmp(p, "off") == 0)
         {
            rtd->disable_profiling_files = 1;
         }
         else
         {
            rtd->disable_profiling_files = 0;
         }
      }
      fout(rtd, "profiling dump to file is %s\n", rtd->disable_profiling_files ? "off" : "on");

   }
   else if (p && strcasecmp(p, "maxdata") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p && isdigit(*p))
      {
         dbc->max_debug_data_size = atoi(p);
      }
      
   }
   else if (p && strcasecmp(p, "trace") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p)
      {
         if (strcasecmp(p, "on") == 0)
         {
            if (!dbc->cob_db_trace)
            {
               dbc->cob_db_trace = fopen("cobcdb.out", "w");
            }
         } else if (p && strcasecmp(p, "off") == 0)
         {
            if (dbc->cob_db_trace)
            {
               fclose(dbc->cob_db_trace);
               dbc->cob_db_trace = NULL;
            }
         }
         else
         {
            dbc->cob_db_trace = fopen(p, "w");
         }
         
         fout(rtd, "trace set to %s (%s)\n", p, dbc->cob_db_trace ? "on" : "off");
      } else {
         out_error(rtd, "invalid trace input (need on/off/filename).");
      }

   }

   else if (p && strcasecmp(p, "region") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p)
      {
         int r = atoi(p);
         if (r >= COB_MAX_RTD_REGION || r < 0)
         {
            out_error(rtd, "invalid region number.");
         }
         else
         {
            cit_runtime_t *myrtd = rtd;
            cob_module *mod;
            myrtd = cob_enterprise_get_region(rtd, r);
            if (myrtd && myrtd->cob_initialized && myrtd->current_module)
            {
               dbc->current_region = r;
               mod = find_module_at_level(myrtd, 0);
               if (mod) out_module_position(rtd, 'R', mod, dbc->current_region, 0);
               else fout(rtd, "R%d at <unknown>\n", dbc->current_region);
            }
            else
            {
               out_error(rtd, "non active region number.");
            }
         }

      }
   }
}

static void
cmd_stack_list_frames(COB_RTD)
{
   int l = 0;
   /*todo*/
   if (rtd->current_module)
   {
      cob_module *mod = rtd->current_module;
      mod = find_module_at_level(rtd, l);
      while (mod)
      {
         out_module_position(rtd, '#', mod, l, 0);
         l++;
         mod = find_module_at_level(rtd, l);
      }
   }
   else
   {
      out_error(rtd, "no program loaded");
   }
}

static void
cmd_up(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   cob_module *mod;
   char *p;
   int c = 1;

   p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   if (p)
   {
      c = atoi(p);
   }
   mod = find_module_at_level(rtd, dbc->current_stack_frame - c);
   if (mod)
   {
      dbc->current_stack_frame = dbc->current_stack_frame - c;
      out_module_position(rtd, '#', mod, dbc->current_stack_frame, 0);
   }
   else
   {
      out_error(rtd, "No frame at that level");
   }

}

static void
cmd_stack_select_frames(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   int s;
   char *p;
   p = clean_quote(COB_STRTOK(NULL, " ", &rtd->debug_strtok_save));
   if (p && *p == 0)
   {
      out_error(rtd, "incomplet command");
   }
   if (p)
   {
      s = atoi(p);
      if (rtd->current_module)
      {
         cob_module *mod = find_module_at_level(rtd, s);
         if (mod)
         {
            dbc->current_stack_frame = s;
            out_module_position(rtd, '#', mod, s, 0);
         }
         else
         {
            out_error(rtd, "No frame at that level");
         }
      }
      else
      {
         out_error(rtd, "no program loaded");
      }
   }
   else
   {
      out_error(rtd, "incomplet command");
   }
}

static void
cmd_replace(COB_RTD)
{
   char *p, *q;
   debug_context *const dbc = cob_get_debug_context(rtd);
   p = COB_STRTOK(NULL, " ,;:", &rtd->debug_strtok_save);
   if (p)
   {
      q = COB_STRTOK(NULL, " ,;:", &rtd->debug_strtok_save);
      if (q)
      {
         if (COB_STRTOK(NULL, " ,;:", &rtd->debug_strtok_save))
         {
            out_error(rtd, "replace: too many arguments");
         }
         else
         {
            replace_buffer_t **z = &(dbc->replace_list);
            while (*z)
            {
               z = &((*z)->next);
            }
            *z = cob_malloc(rtd, sizeof(replace_buffer_t));
            (*z)->oldprefix = cob_malloc(rtd, strlen(p) + 1);
            strcpy((*z)->oldprefix, p);
            (*z)->newprefix = cob_malloc(rtd, strlen(q) + 1);
            strcpy((*z)->newprefix, q);
            (*z)->next = NULL;
         }
      }
      else
      {
         if (*p == '?' && *(p + 1) == '\0')
         {
            replace_buffer_t *z1 = dbc->replace_list;
            while (z1)
            {
               fout(rtd, "%s : %s\n", z1->oldprefix, z1->newprefix);
               z1 = z1->next;
            }
         }
         else
         {
            out_error(rtd, "replace: missing argument");
         }
      }
   }
   else
   {
      dbc->replace_list = NULL;
   }
}

static void
cmd_source(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   cob_module *mod;
   char *p;
   int start, end;

   mod = find_module_at_level(rtd, dbc->current_stack_frame);
   if (mod)
   {
      start = mod->debug_cur_line;
      end = -1;
      p = COB_STRTOK(NULL, " ,;:", &rtd->debug_strtok_save);
      if (p)
      {
         if (*p == '*')
         {
            start = 1;
            end = 9999999;
         }
         else if (*p == '+')
         {
            end = start;
            start = max(start - 5, 0);
            p++;
            end += atoi(p);
         }
         else if (*p == '-')
         {
            p++;
            start = max(start - atoi(p), 0);
         }
         else
         {
            start = atoi(p);
         }
         p = COB_STRTOK(NULL, " ,;:", &rtd->debug_strtok_save);
         if (p)
         {
            if (*p == '*')
            {
               end = 9999999;
            }
            else if (*p == '+')
            {
               end = start;
               p++;
               end += atoi(p);
            }
            else
            {
               end = atoi(p);
            }
         }
      }
      if (end < 0)
      {
         int base = start;
         start = max(base - 5, 0);
         end = base + 5;
      }
      debug_display_line(rtd, mod, start, end);

   }
   else
   {
      out_error(rtd, "No frame at that level");
   }
}

static inline void
write_field_debug_data_to_file(COB_RTD, FILE *F, int max_data_size)
{
   int size = rtd->field_textual_rep_size;
   if (max_data_size > 0 && size > max_data_size)
   {
      size = max_data_size;
   }
   if (max_data_size>=0)
   {
      fprintf(F, "[%d]\"", size);
      fwrite(rtd->field_textual_rep, size, 1, F);
      fprintf(F, "\"");
   }
   else {
      fprintf(F, "[%d]", size);
   }
}

static void
display_field_debug_data(COB_RTD, cob_field *field, int offset)
{

   debug_context *const dbc = cob_get_debug_context(rtd);

   try
   {
      if (field->data)
      {
         cob_one_field_display_extended(rtd, field, 0, offset, DISPLAY_ONE_FIELD_MODE_DEBUG);
      }
      else
      {
         cob_one_field_display_prinf(rtd, 0, (char *)"<NULL POINTER>", DISPLAY_ONE_FIELD_MODE_DEBUG);
      }

   }
   catch (RuntimeException)
   {
      cob_one_field_display_prinf(rtd, 0, (char *)"<INVALID DATA>", DISPLAY_ONE_FIELD_MODE_DEBUG);
   }
   write_field_debug_data_to_file(rtd, dbc->cob_db_out, dbc->max_debug_data_size);
   if (dbc->cob_db_trace) write_field_debug_data_to_file(rtd, dbc->cob_db_trace, dbc->max_debug_data_size);

}

#define MAX_FIELD_CACHE 50
typedef struct {
    cob_ufield cob_field;
    cob_module *mod;
    char *name;
    char *full_name;
    cob_field_debug_info *fdi;
} debug_field_cache_t;

static void
debug_init_field_cache(COB_RTD)
{
   if (rtd->debug_field_cache == NULL)
   {
      rtd->debug_field_cache = cob_malloc(rtd, sizeof(debug_field_cache_t) * (MAX_FIELD_CACHE + 1));
      rtd->debug_field_cache_idx = 0;
   }
}

void
debug_clear_field_cache(COB_RTD)
{
   if (rtd->debug_field_cache)
   {
      int i;
      debug_field_cache_t *p = (debug_field_cache_t *)(rtd->debug_field_cache);

      rtd->debugger_clear_field_cache =0;
      for (i = 0; i < MAX_FIELD_CACHE; i++)
      {
         if (p[i].name)
         {
            free(p[i].name);
            free(p[i].full_name);
         }
      }
      cob_free(rtd->debug_field_cache);
      rtd->debug_field_cache = NULL;
      rtd->debug_field_cache_idx = 0;
   }
}

static int
find_variable_in_cache(COB_RTD, char *name, char *full_name,
                       cob_ufield *found_cob_field, cob_module **found_mod)
{
   int i;
   debug_field_cache_t *p;
   if (rtd->debugger_clear_field_cache)
   {
      debug_clear_field_cache(rtd);
   }
   p = (debug_field_cache_t *)(rtd->debug_field_cache);
   if (p) {
       for (i = 0; i < MAX_FIELD_CACHE; i++)
       {
          char *c = p[i].name;
          if (c &&
              ((strcmp(name, c) == 0) || (strcmp(name, p[i].full_name) == 0)))
          {
             *found_cob_field = p[i].cob_field;
             *found_mod = p[i].mod;
             strcpy(full_name, p[i].full_name);
             return 1;
          }
       }
   }
   return 0;
}

static int
set_variable_in_cache(COB_RTD, char *name, char *full_name,
                      cob_ufield *found_cob_field, cob_module **found_mod, cob_field_debug_info **found_fdi)
{
   int i;
   debug_field_cache_t *p = (debug_field_cache_t *)(rtd->debug_field_cache);
   i = rtd->debug_field_cache_idx;
   if (p) {
       if (p[i].name)
       {
          free(p[i].name);
          free(p[i].full_name);
       }
       p[i].cob_field = *found_cob_field;
       p[i].mod = *found_mod;
       p[i].name = strdup(name);
       p[i].full_name = strdup(full_name);
       p[i].fdi = *found_fdi;
       rtd->debug_field_cache_idx++;
   }
   if (rtd->debug_field_cache_idx >= MAX_FIELD_CACHE)
   {
      rtd->debug_field_cache_idx = 0;
   }
   return 0;
}

static int
find_variable(COB_RTD, cob_ufield *found_cob_field, cob_module **found_mod, char *pbuffer, cob_field_debug_info **found_fdi)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char *p;
   cob_module *mod = NULL;
   int ret_error = 1;
   char name[COB_SMALL_BUFF];

   p = clean_quote(COB_STRTOK(NULL, " ", &rtd->debug_strtok_save));
   if (p && *p == 0)
   {
      out_error(rtd, "field not found");
      return 1;
   }
   debug_init_field_cache(rtd);
   if (p)
   {
      if (rtd->current_module)
      {
         if (find_variable_in_cache(rtd, p, pbuffer, found_cob_field, found_mod))
         {
            ret_error = 0;
         }
         else
         {
            strncpy(name, p, sizeof(name));
            debug_extract_module_name(rtd, dbc, &p, &mod);
            if (p && mod)
            {
               if (mod->fields_debug_info)
               {
                  int do_cache = 1;
                  if (cob_debug_find_cob_field_1(rtd, mod, p, found_cob_field, pbuffer, found_fdi, &do_cache))
                  {
                     cob_field_debug_info *tmp_found_fdi = *found_fdi;
                     ret_error = 0;
                     *found_mod = mod;
                     if (do_cache && found_cob_field->fld.data && *found_fdi && tmp_found_fdi->field_id >  0 )
                     {
                        set_variable_in_cache(rtd, name, pbuffer, found_cob_field, found_mod, found_fdi);
                     }
                  }
                  else
                  {
                     out_error(rtd, "field not found");
                  }
               }
               else
               {
                  out_error(rtd, "no debug info");
               }

            }
            else
            {
               out_error(rtd, "frame stack error");
            }
         }
      }
      else
      {
         out_error(rtd, "no program loaded");
      }
   }
   else
   {
      ret_error = 2;
   }

   return ret_error;
}

static void
cmd_data_evaluate_expression(COB_RTD)
{
   cob_ufield fld;
   cob_module *mod = NULL;
   char pbuffer[COB_SMALL_BUFF];
   int i = 1;
   int res;
   cob_field_debug_info *found_fdi;

   while (1)
   {
      res = find_variable(rtd, &fld, &mod, pbuffer, &found_fdi);
      if (res)
      {
         if (res == 2)
         {
            if (i == 1)
            {
               out_error(rtd, "incomplete command");
            }
            return;
         }
      }
      else
      {

         fout(rtd, "$%d = @%s.%s ", i, mod->module_name, pbuffer);
         display_field_debug_data(rtd, COB_AS_COB_FIELD(&fld), 0);
         fout(rtd, "\n");
      }
      i++;
   }
}

static void
cmd_data_evaluate_expression_hex(COB_RTD)
{

   cob_ufield fld;
   cob_module *mod = NULL;
   char pbuffer[COB_SMALL_BUFF];
   int i, size, cnt = 1;
   int res;
   cob_field_debug_info *found_fdi;

   while (1)
   {
      res = find_variable(rtd, &fld, &mod, pbuffer, &found_fdi);
      if (res)
      {
         if (res == 2)
         {
            if (cnt == 1)
            {
               out_error(rtd, "incomplete command");
            }
            return;
         }
      }
      else
      {

         fout(rtd, "$%d = @%s.%s ", cnt, mod->module_name, pbuffer);
         size = fld.fld.size;
         fout(rtd, "[%d]\"", 2 * size);
         for (i = 0; i < size; ++i)
         {
            fout(rtd, "%02x", (int)fld.fld.data[i]);
         }
         fout(rtd, "\"\n");
      }
      cnt++;
   }

}

static int charToHex(char c)
{
   if (c >= 'A' && c <= 'F') return c - 'A' + 10;
   if (c >= 'a' && c <= 'f') return c - 'a' + 10;
   if (c >= '0' && c <= '9') return c - '0';
   return 0;
}

static char*
cmd_data_set_get_value(COB_RTD, char *command, cob_ufield *found_cob_field, cob_module **found_mod, char *pbuffer)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char *p;
   char *val;
   cob_module *mod = NULL;

   p = clean_quote(COB_STRTOK(NULL, " ", &rtd->debug_strtok_save));
   if (p && strcmp(p, command) == 0)
   {
      p = clean_quote(COB_STRTOK(NULL, " ", &rtd->debug_strtok_save));
   }
   if (p && *p == 0)
   {
      out_error(rtd, "field not found");
      return 0;
   }

   if (p)
   {

      if (rtd->current_module)
      {
         debug_extract_module_name(rtd, dbc, &p, &mod);
         if (p && mod)
         {
            if (mod->fields_debug_info)
            {
               if (cob_debug_find_cob_field(rtd, mod, p, found_cob_field, pbuffer))
               {
                  val = clean_quote(p + strlen(p) + 1);
                  if (val && *val)
                  {
                     *found_mod = mod;
                     return val;

                  }
                  else
                  {
                     out_error(rtd, "no value");
                  }
               }
               else
               {
                  out_error(rtd, "field not found");
               }
            }
            else
            {
               out_error(rtd, "no debug info");
            }

         }
         else
         {
            out_error(rtd, "frame stack error");
         }
      }
      else
      {
         out_error(rtd, "no program loaded");
      }
   }
   else
   {
      out_error(rtd, "incomplete command");
   }
   return NULL;
}

static void
cmd_data_set_hex(COB_RTD)
{
   cob_ufield found_cob_field;
   cob_module *mod = NULL;
   char pbuffer[COB_SMALL_BUFF];
   int i, len;
   char *val = cmd_data_set_get_value(rtd, (char *)"varh", &found_cob_field, &mod, pbuffer);
   if (!val) return;

   len = strlen(val);
   if (len % 2)
   {
      out_error(rtd, "invalid length (len %% 2 != 0)");
      return;
   }
   len /= 2;
   if (COB_FIELD_IS_BITSARRAY(&found_cob_field.fld) || COB_FIELD_IS_BITS(&found_cob_field.fld))
   {
      if (len > 8)
      {
         out_error(rtd, "invalid length (len > 8)");
         return;
      }
   }
   else
   {
      if (len != found_cob_field.fld.size)
      {
         out_error(rtd, "invalid length (len != found_cob_field.size)");
         return;
      }
   }
   if (found_cob_field.fld.data)
   {
      if (COB_FIELD_IS_BITSARRAY(&found_cob_field.fld) || COB_FIELD_IS_BITS(&found_cob_field.fld))
      {
         unsigned long long l = 0;
         for (i = 0; i < len && i < sizeof(unsigned long long); ++i)
         {
            unsigned int val1 = charToHex(val[i * 2]);
            unsigned int val2 = charToHex(val[i * 2 + 1]);
            l = l << 8;
            l |= (val1 << 4) | val2;
         }
         cob_set_ull(rtd, &found_cob_field.fld, l);
      }
      else
      {
         for (i = 0; i < len; ++i)
         {
            int val1 = charToHex(val[i * 2]);
            int val2 = charToHex(val[i * 2 + 1]);
            found_cob_field.fld.data[i] = (val1 << 4) | val2;
         }
      }
      fout(rtd, "$1 = @%s.%s ", mod->module_name, pbuffer);
      display_field_debug_data(rtd, &found_cob_field.fld, 0);
   }
   else
   {
      const char *invalid_data = "Invalid data";
      fout(rtd, "$1 = @%s.%s [%d]\"%s\"", mod->module_name, pbuffer, strlen(invalid_data), invalid_data);
   }
   fout(rtd, "\n");
}

static void
cmd_data_set(COB_RTD)
{
   cob_ufield found_cob_field;
   cob_module *mod = NULL;
   char pbuffer[COB_SMALL_BUFF];

   char *val = cmd_data_set_get_value(rtd, (char *)"var", &found_cob_field, &mod, pbuffer);

   if (!val) return;

   if (found_cob_field.fld.data)
   {
      cob_set_debugcp_string(rtd, &found_cob_field.fld, val);

      fout(rtd, "$1 = @%s.%s ", mod->module_name, pbuffer);
      display_field_debug_data(rtd, &found_cob_field.fld, 0);
   }
   else
   {
      const char *invalid_data = "Invalid data";
      fout(rtd, "$1 = @%s.%s [%d]\"%s\"", mod->module_name, pbuffer, strlen(invalid_data), invalid_data);
   }
   fout(rtd, "\n");
}

static void
cmd_whatis(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char *p;
   cob_module *mod = NULL;
   cob_ufield found_cob_field;

   p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   if (p && rtd->current_module)
   {
      debug_extract_module_name(rtd, dbc, &p, &mod);
      if (mod)
      {
         if (mod->fields_debug_info && cob_debug_find_cob_field(rtd, mod, p, &found_cob_field, NULL))
         {

            switch (found_cob_field.fld.attr->type)
            {
               default:
                  fout(rtd, "type = char *\n"); return;
               case COB_TYPE_GROUP:
                  fout(rtd, "type = COB_GROUP\n"); return;
               case COB_TYPE_BOOLEAN:
                  fout(rtd, "type = COB_BOOLEAN\n"); return;
               case COB_TYPE_NUMERIC:
                  fout(rtd, "type = COB_NUMERIC\n"); return;
               case COB_TYPE_NUMERIC_BINARY:
                  fout(rtd, "type = COB_NUMERIC_BINARY\n"); return;
               case COB_TYPE_NUMERIC_PACKED:
                  fout(rtd, "type = COB_NUMERIC_PACKED\n"); return;
               case COB_TYPE_NUMERIC_FLOAT:
                  fout(rtd, "type = COB_NUMERIC_FLOAT\n"); return;
               case COB_TYPE_NUMERIC_DOUBLE:
                  fout(rtd, "type = COB_NUMERIC_DOUBLE\n"); return;
               case COB_TYPE_NUMERIC_EDITED:
                  fout(rtd, "type = COB_NUMERIC_EDITED\n"); return;
               case COB_TYPE_ALPHANUMERIC:
                  fout(rtd, "type = COB_ALPHANUMERIC\n"); return;
            }

            fout(rtd, "type = char *\n");
            return;
         }
         if (debug_find_function_idx(mod, p) >= 0)
         {
            fout(rtd, "type = void ()\n");
            return;
         }
      }
   }
   out_error(rtd, "No symbol found");
}

static int
cob_runtime_debugger_open_debugdb_find_module(void *db, cob_module *mod)
{
   int fieldcnt = 0;
   int linescnt = 0;
   int res = 0;
   if (db && res != -1)
   {
      if (!res)
      {
         res = debugdb_find_module(db, mod->module_name, mod->source_file, mod->build_stamp, &fieldcnt, &linescnt);
         if (res == 0)
         {
            mod->debugdb_moduleid = 0;
            mod->debugdb_fdi_count = 0;
            mod->debugdb_lines_count = 0;
            res = 0;
         }
         else
         {
            mod->debugdb_moduleid = res;
            mod->debugdb_fdi_count = fieldcnt;
            mod->debugdb_lines_count = linescnt;
            mod->lines_debug_info_version = LINES_DEBUG_INFO_VERSION;
         }
      }
   }
   return res;
}

static void
cob_runtime_debug_trace(COB_RTD, const char *fmt, ...)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   va_list ap;

   if (dbc && dbc->cob_db_trace)
   {
      va_start(ap, fmt);
      vfprintf(dbc->cob_db_trace, fmt, ap);
      va_end(ap);
   }
}

static void*
cob_runtime_debugger_try_debugdb(COB_RTD, const  char *s, cob_module *mod)
{
   void *db = NULL;
   struct stat st;
   char   filename[COB_SMALL_BUFF];
   char   *sb;
   int i;

   cob_runtime_debug_trace(rtd, "Try a debugdb file for module :%s:\n", mod->module_name);
   for (i = 0; i < rtd->resolve_size && !db; i++)
   {
      if (rtd->resolve_path[i] == NULL)
      {
         sprintf(filename, "%s", s);
      }
      else
      {
         sprintf(filename, "%s%c%s", rtd->resolve_path[i], DIRSEPC, s);
      }
      cob_runtime_debug_trace(rtd, "Try a debugdb file :%s:\n", filename);
      if (cob_stat(filename, &st) == 0)
      {
         cob_runtime_debug_trace(rtd, "debugdb file exist :%s:\n", filename);
         db = debugdb_opendb(filename, 0);
      }
   }
   if (!db)
   {
      sb = cob_basename(s);
      if (s != sb)
      {
         db = cob_runtime_debugger_try_debugdb(rtd, sb, mod);
      }
      else
      {
         cob_runtime_debug_trace(rtd, "No debugdb found for module  :%s:\n", mod->module_name);
      }
   }
   else
   {
      int id = cob_runtime_debugger_open_debugdb_find_module(db, mod);
      if (id)
      {
         cob_runtime_debug_trace(rtd, "module %s found with id : %d\n", mod->module_name, id);
         mod->debugdb_moduleid = id;
      }
      else
      {
         cob_runtime_debug_trace(rtd, "No id found for module %s\n", mod->module_name);
         db = debugdb_closedb(db);
      }
   }
   return db;
}


static int
cob_runtime_debugger_actual_open_debugdb(COB_RTD, cob_module *mod)
{
   char *s = getenv("COB_DEBUGDB");
   void *db = NULL;
   if (s)
   {
      db = cob_runtime_debugger_try_debugdb(rtd, s, mod);
   }
   if (!db && mod->debugdb_name)
   {
      db = cob_runtime_debugger_try_debugdb(rtd, mod->debugdb_name, mod);
   }
   if (!db)
   {
      mod->debugdb_moduleid = -1;
      return 0;
   }
   else
   {
      if (cob_runtime_debugger_debugdb) debugdb_closedb(cob_runtime_debugger_debugdb);
      cob_runtime_debugger_debugdb = db;
      return 1;
   }
}
int
cob_runtime_debugger_open_debugdb(COB_RTD, cob_module *mod)
{
   int modid;

   if (mod && mod->debugdb_moduleid != -1)
   {
      modid = cob_runtime_debugger_open_debugdb_find_module(cob_runtime_debugger_debugdb, mod);
      if (modid)
      {
         mod->debugdb_moduleid = modid;
         return 1;
      }
      else
      {
         if (cob_runtime_debugger_actual_open_debugdb(rtd, mod)) return 1;
         mod->debugdb_moduleid = -1;
      }
   }
   return 0;

}

cob_field_debug_info*
cob_runtime_debugger_open_field_debugdb(COB_RTD, cob_module *mod, cob_field_debug_info *fdi)
{

   cob_field_debug_info *res = fdi;

   if (fdi[0].field_id == -3)
   {
      if (!cob_runtime_debugger_open_debugdb(rtd, mod))
      {
         return NULL;
      }
      if (cob_runtime_debugger_debugdb != (void *)-1)
      {
         if (!mod->debugdb_fdi_store && mod->debugdb_moduleid > 0)
         {
            int fieldcnt = mod->debugdb_fdi_count;

            mod->debugdb_fdi_store = calloc(1, sizeof(cob_field_debug_info) * fieldcnt + 1);
            /*memset (mod->debugdb_fdi_store, 0, sizeof(cob_field_debug_info) * fieldcnt + 1 );*/

            if (mod->debugdb_fdi_store)
            {
               int  occurs;
               int  groupid;
               int  fieldid;
               int  childidx, idx;
               char buffer[1024];
               debugdb_start_listfield(cob_runtime_debugger_debugdb, mod->debugdb_moduleid);
               fieldid = debugdb_listfield(cob_runtime_debugger_debugdb, &occurs, buffer, &groupid, &childidx, &idx);
               while (fieldid)
               {
                  if (idx < fieldcnt)
                  {
                     cob_field_debug_info *p = &(mod->debugdb_fdi_store[idx]);
                     *(int *)&p->field_id = fieldid;
                     *(char **)&p->name   = strdup(buffer);
                     *(int *)&p->occur    = occurs;
                     if ((childidx > 0) && childidx < fieldcnt)
                     {
                        *(void **)&p->child    = &(mod->debugdb_fdi_store[childidx]);
                     }
                     if (groupid == 0 && mod->debugdb_fields_debug_info == NULL)
                     {
                        mod->debugdb_fields_debug_info = p;
                     }
                     fieldid = debugdb_listfield(cob_runtime_debugger_debugdb, &occurs, buffer, &groupid, &childidx, &idx);
                  }
                  else
                  {
                     fprintf(stderr, "DEBUGDB field idx (%d) >= fieldcnt (%d) \n", idx, fieldcnt);
                  }
               }
               debugdb_reset_listfield(cob_runtime_debugger_debugdb);
            }
         }
         if (mod->debugdb_fields_debug_info)
         {
            res = mod->debugdb_fields_debug_info;
         }
      }
   }
   return res;

}

static char*
cob_runtime_debugger_loc_strdup(cob_module *mod, char *s)
{
   if (!s)
   {
      return NULL;
   }
   if (mod->debugdb_cachepath && (strcmp(mod->debugdb_cachepath, s) == 0))
   {
   }
   else
   {
      mod->debugdb_cachepath = strdup(s);
   }
   return mod->debugdb_cachepath;
}

cob_lines_debug_info*
cob_runtime_debugger_open_lines_debugdb(COB_RTD, cob_module *mod)
{
   int linescnt = 0;

   if (!mod->lines_debug_info && mod->debugdb_moduleid != -1)
   {
      if (!cob_runtime_debugger_open_debugdb(rtd, mod))
      {
         return NULL;
      }
      if (cob_runtime_debugger_debugdb != (void *)-1)
      {
         if (!mod->lines_debug_info && mod->debugdb_moduleid > 0)
         {
            linescnt = mod->debugdb_lines_count;
            mod->lines_debug_info = calloc(1, sizeof(cob_lines_debug_info) * linescnt + 1);
            /*memset (mod->lines_debug_info, 0, sizeof(cob_lines_debug_info) * linescnt +1 );*/

            if (mod->debugdb_moduleid != -1 && mod->lines_debug_info)
            {
               int  issection;
               int  lineid;
               int  idx;
               char buffer[1024];
               char *src;
               debugdb_start_listlines(cob_runtime_debugger_debugdb, mod->debugdb_moduleid);
               lineid = debugdb_listlines(cob_runtime_debugger_debugdb, &idx, &lineid, buffer, &issection, &src);
               mod->debugdb_cachepath = NULL;
               while (lineid)
               {
                  if (idx < linescnt)
                  {
                     cob_lines_debug_info *p = &(mod->lines_debug_info[idx]);
                     *(int *)&p->line_nr  = lineid;
                     if (buffer[0])
                     {
                        *(char **)&p->label  = strdup(buffer);
                     }
                     *(char *)&p->is_section    = issection;
                     *(char **)&p->source_file  = cob_runtime_debugger_loc_strdup(mod, src);
                     lineid = debugdb_listlines(cob_runtime_debugger_debugdb, &idx, &lineid, buffer, &issection, &src);
                  }
                  else
                  {
                     fprintf(stderr, "DEBUGDB line idx (%d) >= linescnt (%d) \n", idx, linescnt);
                  }
               }
               debugdb_reset_listlines(cob_runtime_debugger_debugdb);
               mod->debugdb_cachepath = NULL;
            }
         }
      }
   }
   return mod->lines_debug_info;

}

static void
cob_debug_print_variables(COB_RTD,
                          cob_module *mod,
                          cob_field_debug_info *fdi,
                          int base_offset,
                          int indent_level,
                          char *prefix,
                          int in_linkage,
                          int this_one_only)
{
   int i, j, occ;
   char newprefix[COB_SMALL_BUFF];
   cob_field *fl;
   cob_ufield uf;
   void *data_storage = mod->data_storage;
   get_cob_field_t get_cob_field = mod->get_cob_field;
   char *name = (char *)"";

   fdi = cob_runtime_debugger_open_field_debugdb(rtd, mod, fdi);

   for (i = 0; fdi[i].name && (!this_one_only || i < 1); ++i)
   {
      if (fdi[i].field_id == -2)
      {
         in_linkage = 1;
      }
      for (j = 0; j < indent_level; ++j) fout(rtd, "\t");
      try
      {
         fl = get_cob_field(data_storage, fdi[i].field_id);       
      }
      catch (RuntimeException)
      {
         fl = NULL;
      }
      if (fl)
      {
         uf.fld = *fl;
      } else {
         memset (&uf, 0, sizeof (uf.fld));
      }
      if (!this_one_only)
      {
         name = (char *)(fdi[i].name);
      }
      if (fdi[i].child)
      {
         if (fdi[i].occur > 1 && fl)
         {
            fout(rtd, "%s%s(%d)\n", prefix, name, fdi[i].occur);
            for (occ = 0; occ < fdi[i].occur && occ < debug_max_occur; ++occ)
            {
               for (j = 0; j < indent_level + 1; ++j) fout(rtd, "\t");
               fout(rtd, "%s%s(%d) = ", prefix, name, occ + 1);
               display_field_debug_data(rtd, &uf.fld, base_offset + uf.fld.size * occ);
               fout(rtd, "\n");
               sprintf(newprefix, "%s%s(%d).", prefix, name, occ + 1);
               cob_debug_print_variables(rtd, mod, fdi[i].child, base_offset + uf.fld.size * occ, indent_level + 2, newprefix, in_linkage, 0);
            }
         }
         else
         {
            fout(rtd, "%s%s\n", prefix, name);
            sprintf(newprefix, "%s%s.", prefix, name);
            cob_debug_print_variables(rtd, mod, fdi[i].child, base_offset, indent_level + 1, newprefix, in_linkage, 0);
         }
      }
      else
      {
         if (fdi[i].occur > 1 && fl)
         {
            fout(rtd, "%s%s(%d)\n", prefix, name, fdi[i].occur);
            for (occ = 0; occ < fdi[i].occur && occ < debug_max_occur; ++occ)
            {
               for (j = 0; j < indent_level + 1; ++j) fout(rtd, "\t");
               fout(rtd, "%s%s(%d) = ", prefix, name, occ + 1);
               if (COB_FIELD_IS_BITSARRAY(&uf.fld))
               {
                  uf.ba_fld.index = occ;
                  display_field_debug_data(rtd, &uf.fld, base_offset);
               }
               else
               {
                  display_field_debug_data(rtd, &uf.fld, base_offset + uf.fld.size * occ);
               }
               fout(rtd, "\n");
            }
         }
         else
         {
            /*				fout(rtd, "%s = \"", fdi[i].name);
            cob_one_field_display(rtd, f, dbc->cob_db_out, base_offset);
                            fout(rtd, "\"\n");*/
            fout(rtd, "%s%s = ", prefix, name);
            display_field_debug_data(rtd, &uf.fld, base_offset);
            fout(rtd, "\n");
         }
      }
   }

}

static void
cmd_info(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   /*todo*/
   cob_module *mod;
   char *p;
   char prefix[COB_SMALL_BUFF];
   int i;
   int cnt = MAX_MODULE_FIND_LOOP;
   int res;

   p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
   if (p && strcasecmp(p, "sources") == 0)
   {
      if (rtd->current_module)
      {
         mod = rtd->current_module;
         fout(rtd, "Source files \n");
         while (mod && cnt--)
         {
            fout(rtd, "%s\n", mod->source_file);
            mod = COB_NEXT_MODULE(mod);
         }
         /*out_module_position(rtd->current_module);*/
      }
   }
   else if (p && strcasecmp(p, "target") == 0)
   {
      fout(rtd, "Child PID %d \n", getpid());
   }
   else if (p && strcasecmp(p, "locals") == 0)
   {
      cob_module *mod = find_module_at_level(rtd, dbc->current_stack_frame);
      if (mod)
      {
         if (mod->fields_debug_info)
         {
            int save = -1;
            p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
            if (p ) {
               if (isdigit(*p))
               {
                  dbc->max_debug_data_size = atoi(p);
               }
               else if (p && *p =='-')
               {  
                  save = dbc->max_debug_data_size;
                  dbc->max_debug_data_size = -1;
               }
            }
            {
                cob_field_debug_info *fdi = mod->fields_debug_info;
                sprintf(prefix, "@%s.", mod->module_name);
                cob_debug_print_variables(rtd, mod, fdi, 0, 0, prefix, 0, 0);
            }
            if (save >=0)
            {
               dbc->max_debug_data_size = save;
            }
         }
         else
         {
            out_error(rtd, "no debug info\n");
         }
      }
      else
      {
         out_error(rtd, "frame stack error\n");
      }
   }
   else if (p && strcasecmp(p, "var") == 0)
   {
      cob_module *mod = find_module_at_level(rtd, dbc->current_stack_frame);
      if (mod)
      {
         if (mod->fields_debug_info)
         {
            char pbuffer[COB_SMALL_BUFF];
            cob_field_debug_info *fdi = NULL;
            cob_ufield fld;
            res = find_variable(rtd, &fld, &mod, pbuffer, &fdi);
            if (res == 0)
            {
               sprintf(prefix, "@%s.%s", mod->module_name, pbuffer);
               cob_debug_print_variables(rtd, mod, fdi, 0, 0, prefix, 0, 1);
            }
         }
         else
         {
            out_error(rtd, "no debug info\n");
         }
      }
      else
      {
         out_error(rtd, "frame stack error\n");
      }

   }
   else if (p && strcasecmp(p, "line") == 0)
   {
      p = COB_STRTOK(NULL, " ", &rtd->debug_strtok_save);
      if (p && !isdigit(*p))
      {
         cob_ufield found_cob_field;

         if (p && rtd->current_module)
         {
            mod = find_module_at_level(rtd, dbc->current_stack_frame);
            if (mod && mod->fields_debug_info  && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
            {
               if (cob_debug_find_cob_field(rtd, mod, p, &found_cob_field, NULL))
               {
                  /* dump declaration line*/
                  return;
               }
               i = debug_find_function_idx(mod, p);
               if (i >= 0)
               {
                  fout(rtd, "Line %d of %s label is %s\n",
                       mod->lines_debug_info[i].line_nr, mod->source_file, mod->lines_debug_info[i].label);
                  return;
               }
            }
         }
      }
      out_error(rtd, "No line info");
   }
   else if (p && strcasecmp(p, "break") == 0)
   {
      struct break_point *br;
      br = dbc->break_point;
      if (br)
      {
         int brcnt = 0;
         while (br)
         {
            if (br->br_type)
            {
               out_breakpoint(rtd, br);
               brcnt++;
            }
            br = br->next;
         }
         if (brcnt == 0)
         {
            out_error(rtd, "No break defined");
         }
      }
      else
      {
         out_error(rtd, "No break info");
      }
   }
   else if (p && strcasecmp(p, "regions") == 0)
   {
      int ok = 0;
      cit_runtime_t *myrtd = rtd;
      for (i = 0; i < COB_MAX_RTD_REGION; i++)
      {
         myrtd = cob_enterprise_get_region(rtd, i);
         if (myrtd && myrtd->cob_initialized && myrtd->current_module)
         {
            cob_module *mod = myrtd->current_module;
            ok = 1;
            mod = find_module_at_level(myrtd, 0);
            if (mod) out_module_position(rtd, 'R', mod, i, 0);
            else fout(rtd, "R%d at <unknown>\n", i);
         }
      }
      if (!ok)
      {
         out_error(rtd, "no program loaded");
      }
   }
   else if (p && strcasecmp(p, "region") == 0)
   {
      int ok = 0;
      cit_runtime_t *myrtd = rtd;
      myrtd = cob_enterprise_get_region(rtd, dbc->current_region);
      if (myrtd && myrtd->cob_initialized && myrtd->current_module)
      {
         cob_module *mod = myrtd->current_module;
         ok = 1;
         mod = find_module_at_level(myrtd, 0);
         if (mod) out_module_position(rtd, 'R', mod, dbc->current_region, 0);
         else fout(rtd, "R%d at <unknown>\n", dbc->current_region);
      }
      else
      {
         fout(rtd, "R%d at <unknown>\n", dbc->current_region);
      }
   }
   else if (p && strcasecmp(p, "profiling") == 0)
   {
      int ok = 0;
      if (dbc->cob_db_out)
      {
         ok = cob_profiling_dump_all(rtd, dbc->cob_db_out);
      }
      if (!ok)
      {
         out_error(rtd, "No profiling info");
      }
   }
}

typedef struct {
    const char *cli;
    void  (*func)(cit_runtime_t *);
} cmd_list_t;

static cmd_list_t cmd_list[] = {
   { "version",  cmd_gdb_version },
   { "break",    cmd_break_insert },
   { "delete",   cmd_break_delete },
   { "thread",   cmd_thread_list_ids },
   { "run",      cmd_exec_run },
   { "continue", cmd_exec_continue },
   { "step",     cmd_exec_step },
   { "next",     cmd_exec_next },
   { "contreturn", cmd_exec_return },
   { "bt",       cmd_stack_list_frames },
   { "backtrace", cmd_stack_list_frames },
   { "frame",    cmd_stack_select_frames },
   { "print",    cmd_data_evaluate_expression },
   { "printh",   cmd_data_evaluate_expression_hex },
   { "up",       cmd_up },
   { "kill",     cmd_kill },
   { "info",     cmd_info },
   { "set",      cmd_set },
   { "stop",     cmd_exec_stop },
   { "whatis",   cmd_whatis },
   { "quit",     cmd_exit },
   { "detach",   cmd_detach },
   { "list",     cmd_source },
   { "replace",  cmd_replace },

   { NULL, NULL }
};

static int debug_region_separated = 0;

static int
get_actual_prompt(COB_RTD, int blocking)
{
   int region;
   debug_context *const dbc = cob_get_debug_context(rtd);

   if (dbc->command_buff.buffer == NULL)
   {
      dbc->command_buff.buffer = calloc(1, 2048);
      dbc->command_buff.size = 2048;
   }
   flush_out(rtd);
   if (!rtd->debug_prompted)
   {
      fout(rtd, dbc->prompt);
      fout(rtd, "\n");
      rtd->debug_prompted = 1;
   }
   flush_out(rtd);
   if (cob_runtime_debugger_readprompt(rtd, &dbc->command_buff, blocking))
   {
      dbc->flag_continue_running = 1;
   }
   else
   {
      rtd->debug_prompted = 0;

      if (debug_region_separated)
      {
         cob_runtime_debugger_exec_command(rtd, dbc->command_buff.buffer);
      }
      else
      {
         struct  cit_runtime_s *current_rtd;
         while (dbc->setting_current_region);
         {
            debug_nano_wait();
         }
         region = dbc->current_region;
         current_rtd = cob_enterprise_get_region(rtd, region);
         if (!current_rtd || !current_rtd->cob_initialized)
         {
            current_rtd = rtd;
         }
         cob_runtime_debugger_exec_command(current_rtd, dbc->command_buff.buffer);
      }
   }
   /*if ( !dbc->cob_db_in )
       dbc->flag_exit_debug = 1;
   */

   return (dbc->flag_continue_running);
}


#if HAVE_PTHREAD_H
static
int blockSignal(int block)
{
   sigset_t set;
   int s;

   /* Block SIGQUIT and SIGUSR1; other threads created by main()
      will inherit a copy of the signal mask. */

   sigemptyset(&set);
   sigaddset(&set, SIGQUIT);
   sigaddset(&set, SIGINT);
   sigaddset(&set, SIGHUP);
   sigaddset(&set, SIGUSR1);
   sigaddset(&set, SIGUSR2);
   if (block)
   {
      s = pthread_sigmask(SIG_BLOCK, &set, NULL);
   }
   else
   {
      s = pthread_sigmask(SIG_UNBLOCK, &set, NULL);

   }
   return 0;
}
#endif

static void cob_debug_context_init(COB_RTD);

#ifdef _MSC_VER
#  if _MSC_VER >= 1500
DWORD WINAPI debug_thread_proc(_In_  LPVOID arg)
#  else
DWORD WINAPI debug_thread_proc(LPVOID arg)
#  endif
#else
static void *debug_thread_proc(void *arg)
#endif
{
   COB_RTD = (cit_runtime_t *)arg;
   debug_context *const dbc = cob_get_debug_context(rtd);
   dbc->external_thread_no = debug_thread_no++;

   if (!dbc->cob_db_trace)
   {
      char *p;
      char buff[COB_SMALL_BUFF];
      p = getenv("COB_DEBUG_TRACE");
      if (p)
      {
         sprintf(buff, "%s_%d.%d.txt", p, dbc->external_did, dbc->external_thread_no);
         dbc->cob_db_trace = fopen(buff, "w");
         if (!dbc->cob_db_trace)
         {
            fprintf(stderr, "COBOL-IT: Can't create Debugger trace %s for write (%s)\n", buff, cob_strerror((cit_runtime_t *)arg, errno));
            fflush(stderr);
         }
      }
   }
   dbc->flag_debugger_active = 0;
   dbc->cob_debug_thread_started = 1;
   while (!dbc->flag_exit_debug)
   {
      if (dbc->cob_db_trace)
      {
         fprintf(dbc->cob_db_trace, "Restarting debugger Thread\n");
      }
      dbc->flag_stop_next_statement = 0;
      dbc->prompt = "(cobcdb)";
      dbc->breap_point_id = 0;
      cob_debug_context_init((cit_runtime_t *)arg);
      dbc->flag_go_next_statement = 0;
      dbc->flag_stop_next_statement = 1;
      dbc->flag_debugger_active = 1;
#     if HAVE_PTHREAD_H
      pthread_testcancel();
#     endif
      while (dbc->cob_db_in)
      {
#        if HAVE_PTHREAD_H
         pthread_testcancel();
#        endif
         get_actual_prompt(rtd, 1);
      }
      dbc->flag_debugger_active = 0;

   }
   /*printf("exit debug_thread_proc\n");*/
   return NULL;
}

static void
get_prompt(COB_RTD, int blocking, int do_exit)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (blocking)
   {
      dbc->flag_stop_next_statement = 0;
      dbc->flag_go_next_statement = 0;
      dbc->perform_count = 0;
      dbc->flag_continue_running = 0;
   }
#  ifdef _MSC_VER
   if (blocking)
   {
      while ((dbc->flag_debugger_active &&
              dbc->cob_db_in &&
              !dbc->flag_continue_running && !dbc->flag_exit_debug))
      {
         Sleep(1);
         if (!dbc->cob_db_in)
         {
            if (dbc->cob_db_trace)
            {
               fprintf(dbc->cob_db_trace, "get_prompt dbc->cob_db_in == NULL\n");
            }
            close_files(rtd);
         }
         else
         {
            int err = ferror(dbc->cob_db_in);
            if (err)
            {
               if (dbc->cob_db_trace)
               {
                  fprintf(dbc->cob_db_trace, "get_prompt error : %d %s\n", err, cob_strerror(rtd, err));
               }
               close_files(rtd);
            }
         }
         if (dbc->flag_need_reload_breakpoint)
         {
            debug_reload_breakpoint(rtd, 0);
         }

      }
      if (do_exit && dbc->flag_exit_debug)
      {
         HANDLE *h;
         h = dbc->debugger_thread;
         WaitForSingleObject(*h, INFINITE);
         actual_cmd_exit(rtd, 0);
      }
   }

#  else
#     if HAVE_PTHREAD_H
   if (blocking)
   {
      while (dbc->flag_debugger_active &&
             dbc->cob_db_in && !dbc->flag_continue_running && !dbc->flag_exit_debug)
      {
         sched_yield();
         debug_nano_wait();
         if (dbc->flag_need_reload_breakpoint)
         {
            debug_reload_breakpoint(rtd, 0);
         }
      }
      if (do_exit && dbc->flag_exit_debug)
      {
         pthread_t *t = dbc->debugger_thread;
         pthread_join(*t, NULL);
         actual_cmd_exit(rtd, 0);
      }
   }
#     else
   while (dbc->flag_debugger_active && dbc->cob_db_in && !dbc->flag_continue_running && !dbc->flag_exit_debug)
   {
      get_actual_prompt(rtd, blocking);
   }
#     endif
#  endif
   if (do_exit && dbc->flag_need_reload_breakpoint)
   {
      debug_reload_breakpoint(rtd, 0);
   }

}

/* **************************************************************** */
/*                                                                  */
/*                  Interface to Readline Completion                */
/*                                                                  */
/* **************************************************************** */

static char* command_generator(const char *text, int state);
static char** debugcommand_completion(const char *text, int start, int end);

#ifdef HAVE_READLINE_READLINE_H
/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names if this is the first word in the line, or on filenames
   if not. */
static void
initialize_readline(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   /* Allow conditional parsing of the ~/.inputrc file. */
   rl_readline_name = "cobcdb";
   rl_already_prompted = 1;
   rl_catch_signals = 0;
   rl_catch_sigwinch = 0;
   rl_instream = dbc->cob_db_in;
   rl_outstream = dbc->cob_db_out;

   /* Tell the completer that we want a crack first. */
   rl_attempted_completion_function = debugcommand_completion;
}

/* Attempt to complete on the contents of TEXT.  START and END bound the
   region of rl_line_buffer that contains the word to complete.  TEXT is
   the word to complete.  We can use the entire contents of rl_line_buffer
   in case we want to do some simple parsing.  Return the array of matches,
   or NULL if there aren't any. */
static char**
debugcommand_completion(const char *text, int start, int end)
{
   char **matches;

   matches = (char **)NULL;

   /* If this word is at the start of the line, then it is a command
      to complete.  Otherwise it is the name of a variable. */
   if (start == 0) matches = rl_completion_matches(text, command_generator);

   return (matches);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
static char*
command_generator(const char *text, int state)
{
   static int list_index, len;
   const char *name;

   /* If this is a new word to complete, initialize now.  This includes
      saving the length of TEXT for efficiency, and initializing the index
      variable to 0. */
   if (!state)
   {
      list_index = 0;
      len = strlen(text);
   }

   /* Return the next name which partially matches from the command list. */
   name = cmd_list[list_index].cli;
   while (name)
   {
      list_index++;

      if (strncmp(name, text, len) == 0) return (strdup(name));
      name = cmd_list[list_index].cli;
   }

   /* If no names matched, then return NULL. */
   return ((char *)NULL);
}
#endif

static int read_one_char (FILE * dbin)
{
   unsigned char cr;
   int r = read(fileno(dbin), &cr, 1) ;
    if (r > 0 )
      return cr;
   else 
   {
      return -1;
   }
}

static char*
cob_runtime_debugger_readline(COB_RTD, cmd_buffer_t *cmd_buffer, int blocking)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   int          c;

#  ifdef HAVE_READLINE_READLINE_H
   if (!dbc->flag_machine_command &&
       blocking &&
       ( (dbc->flag_force_readline_command > 0)))
   {
      char *p;
      int  l;
      initialize_readline(rtd);
      p = readline(dbc->prompt);
      if (p)
      {
         l = strlen(p);
         while (l >= cmd_buffer->size)
         {
            cmd_buffer->size *= 2;
            cmd_buffer->buffer = realloc(cmd_buffer->buffer, cmd_buffer->size);
         }
         if (strlen(p) > 0)
         {
            strcpy(cmd_buffer->buffer, p);
            add_history(p);
         }
         free(p);
         return (cmd_buffer->buffer);
      }
   }
   else
#  endif
   {
      if (rtd->debug_input_buffer == NULL)
      {
         rtd->debug_input_buffer = cob_malloc(rtd, 1024);
         rtd->debug_input_buffer_size = 1024;
      }
      do
      {
         errno = 0;
         if (blocking)
         {
            if (dbc->cob_db_in)
            {
               c = fgetc(dbc->cob_db_in);
            }
            else
            {
               c = -1;
            }
         }
         else
         {
#           ifndef _MSC_VER
            if (dbc->cob_db_in)
            {
               fd_set        fds;
               struct timeval t;
               FD_ZERO(&fds);
               FD_SET(fileno(dbc->cob_db_in), &fds);
               memset(&t, 0, sizeof(t));
               c = -1;
               if (select(fileno(dbc->cob_db_in) + 1, &fds, NULL, NULL, &t))
               {
                  c = fgetc(dbc->cob_db_in);
               }
            }
            else
            {
               c = -1;
            }

#           else
            c = -1;
#           endif
         }
         if (c < 0)
         {
            int err = errno;
            if (!dbc->cob_db_in)
            {
               if (dbc->cob_db_trace)
               {
                  fprintf(dbc->cob_db_trace, "cob_runtime_debugger_readline dbc->cob_db_in == NULL\n");
               }
               close_files(rtd);
            }
            else if (err != 0 && err != EINTR)
            {
               if (dbc->cob_db_trace)
               {
                  fprintf(dbc->cob_db_trace, "cob_runtime_debugger_readline error : %d : %s\n", errno, cob_strerror(rtd, errno));
               }
               close_files(rtd);
            }
            else
            {
               c = 0;
            }
         }
         else
         {
            rtd->debug_input_buffer[rtd->debug_byte_in_buffer] = c;
            if (rtd->debug_byte_in_buffer >= rtd->debug_input_buffer_size)
            {
               rtd->debug_input_buffer_size *= 2;
               rtd->debug_input_buffer = realloc(rtd->debug_input_buffer, rtd->debug_input_buffer_size);

            }
            if (rtd->debug_input_buffer[rtd->debug_byte_in_buffer] == '\n' )
            {
               while (rtd->debug_byte_in_buffer >= 0 && 
                      (rtd->debug_input_buffer[rtd->debug_byte_in_buffer] == '\r' || 
                       rtd->debug_input_buffer[rtd->debug_byte_in_buffer] == ' '  ||
                       rtd->debug_input_buffer[rtd->debug_byte_in_buffer] == '\t' ||
                       rtd->debug_input_buffer[rtd->debug_byte_in_buffer] == '\n'))
               {
                  rtd->debug_byte_in_buffer--;
               }
               rtd->debug_byte_in_buffer++;
               while ((cmd_buffer->size - 1) < rtd->debug_byte_in_buffer)
               {
                  cmd_buffer->size *= 2;
                  cmd_buffer->buffer = realloc(cmd_buffer->buffer, cmd_buffer->size);
               }
               memcpy(cmd_buffer->buffer, rtd->debug_input_buffer, rtd->debug_byte_in_buffer);
               cmd_buffer->buffer[rtd->debug_byte_in_buffer] = 0;
               rtd->debug_input_buffer[0] = 0;
               rtd->debug_byte_in_buffer = 0;
               return (cmd_buffer->buffer);
            }
            rtd->debug_byte_in_buffer++;
         }
      }
      while (dbc->cob_db_in &&  (c >= 0));
   }
   return (NULL);
}

int
cob_runtime_debugger_readprompt(COB_RTD, cmd_buffer_t *cmd_buffer, int blocking)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char    *p = NULL;
   int     res = 0;

   p = cob_runtime_debugger_readline(rtd, cmd_buffer, blocking);
   if ((p == NULL))
   {
      res = -1;
   }
   else
   {
      if (p && dbc->cob_db_trace)
      {
         fprintf(dbc->cob_db_trace, "%s\n", cmd_buffer->buffer);
      }
   }
   return (res);
}

int
cob_runtime_debugger_exec_command(COB_RTD, char *cmd)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   int j, found;
   char *p;
   int   please_exit = 0;
   int   i_start_e4c = 0;

   if (!e4c_context_is_ready())
   {
      e4c_context_begin(E4C_TRUE);
      i_start_e4c = 1;
   }
   j = strlen(cmd);
   if (!j) return dbc->flag_exit_debug;
   --j;

   while (j && (cmd[j] < 32))
   {
      cmd[j] = 0;
      j--;
   }
   try
   {
      p = COB_STRTOK(cmd, " ,", &rtd->debug_strtok_save);
      j = 0;
      found = 0;

      if (p)
      {
         while (cmd_list[j].cli)
         {
            {
               if (cmd_list[j].cli && (strncasecmp(cmd_list[j].cli, p, strlen(p)) == 0) && cmd_list[j].func)
               {
                  found = 1;
                  cmd_list[j].func(rtd);
                  flush_out(rtd);
                  break;
               }
            }
            j++;
         }
      }
      if (!found)
      {
         out_error(rtd, "command unknown");
      }
      /*
      if (!rtd->debug_prompted) {
          fout(rtd,dbc->prompt);
          fout(rtd,"\n");
          flush_out(rtd);
          rtd->debug_prompted = 1;
      } 
      */
   }
   catch (RuntimeException)
   {
#     ifdef _MSC_VER
      if (GetExceptionCode() == DebuggerExitException)
      {
         please_exit = 1;
      }
      else if (GetExceptionCode() == DebuggerThreadExitException)
      {
         please_exit = 2;
      }
#     else
      const e4c_exception *exception = e4c_get_exception();
      if (e4c_is_instance_of(exception, &DebuggerExitException))
      {
         please_exit = 1;
         if (exception->type == &DebuggerThreadExitException)
         {
            please_exit = 2;
         }
      }
#     endif
      else
      {
         out_error(rtd, "debugger runtime error");
      }
   }
   if (e4c_context_is_ready() && (i_start_e4c || please_exit))
   {
      e4c_context_end();
   }
   if (please_exit)
   {
#     if HAVE_PTHREAD_H
      if (please_exit == 2)
      {
         pthread_exit(NULL);
      }
      exit(0);
#     endif
   }
   return (dbc->flag_exit_debug);
}

void
cob_runtime_debugger_output_event(COB_RTD, const char *reason)
{
   /*debug_context * const dbc = cob_get_debug_context(rtd);*/
   cob_module  *mod = find_module_at_level(rtd, 0);
   {
      event_fout(rtd, "%s ", reason);
   }

   if (mod)
   {
      out_module_position(rtd, '#', mod, 0, 1);
   }
   else
   {
      event_fout(rtd, "\n");
   }
   /*
   fout(rtd,dbc->prompt);
     fout(rtd,"\n");
   */
}

static int cob_runtime_debugger_install_thread(COB_RTD, int did);


static inline int debug_callback_fct_manage_break(COB_RTD, debug_context *const dbc)
{
   cob_module *const mod = rtd->current_module;
   struct cob_condition_debug_info **value_condition;
   int cnt = 0;
   /* check break point*/
   for (value_condition = dbc->value_condition; *value_condition && cnt < COB_MAX_VALUE_CONDITION; cnt++, value_condition++)
   {
      if ((*value_condition)->valid_condition)
      {
         if (memcmp((*value_condition)->refvalue, (*value_condition)->curvalue, (*value_condition)->size) != 0)
         {
            if ((*value_condition)->no_break_count > 0)
            {
               (*value_condition)->no_break_count--;
               memcpy((*value_condition)->refvalue, (*value_condition)->curvalue, (*value_condition)->size);
               break;
            }
            cob_runtime_debugger_output_event(rtd, "-event-breakpoint-hit");
            event_fout(rtd, "-event-value-change %s\n", (*value_condition)->varname);
            get_prompt(rtd, 1, 1);
            if (*value_condition && (*value_condition)->valid_condition) memcpy((*value_condition)->refvalue, (*value_condition)->curvalue, (*value_condition)->size);
            return 1;
         }
      }
   }
   if (mod && mod->lines_debug_info)
   {
      cob_lines_debug_info *const  debug_info = &(mod->lines_debug_info[mod->cur_line_debug_idx]);
      if (debug_info->debug_status)
      {
         if (debug_info->condition)
         {
            if (debug_info->condition->no_break_count > 0)
            {
               debug_info->condition->no_break_count--;
               return 0;
            }
         }
         cob_runtime_debugger_output_event(rtd, "-event-breakpoint-hit");
         if (debug_info->debug_status == COB_DEBUG_TEMPORARY_BREAKPOINT)
         {
            debug_info->debug_status = 0;
         }
         get_prompt(rtd, 1, 1);
         return 1;
      }
   }
   return 0;
}

void
cob_runtime_debugger_switch_region(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (!dbc->flag_debugger_active || !dbc->debugger_thread)
   {
      return;
   }
   dbc->perform_count = 0;
   debug_clear_field_cache(rtd);
   debug_reload_breakpoint(rtd, 0);
}

void 
cob_runtime_debugger_clear_field_cache (COB_RTD)
{
   rtd->debugger_clear_field_cache = 1;
}

void
debug_callback_fct(COB_RTD, int code)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   if (!dbc->flag_debugger_active)
   {
      return;
   }
   if (!dbc->debugger_thread)
   {
      return;
   }
   if (!debug_region_separated)
   {
      dbc->setting_current_region = 1;
      dbc->current_region = rtd->rtd_region_nr;
      dbc->setting_current_region = 0;
   }
   else
   {
      dbc->setting_current_region = 0;
   }
   if (dbc->flag_need_reload_breakpoint)
   {
      debug_reload_breakpoint(rtd, 0);
   }
   switch (code)
   {
      case COB_DBCALLBACK_PERFORM_ENTER:
         if (dbc->flag_go_next_statement)
         {
            dbc->perform_count++;
         }
         break;
      case COB_DBCALLBACK_PERFORM_EXIT:
         if (dbc->flag_go_next_statement)
         {
            dbc->perform_count--;
         }
         break;
      case COB_DBCALLBACK_ENTER_MODULE:
         debug_reload_breakpoint(rtd, 1);
         rtd->debugger_clear_field_cache = 1;
         break;
      case COB_DBCALLBACK_LEAVE_MODULE:
         /*to do reload line table*/
         rtd->debugger_clear_field_cache = 1;
         debug_reload_breakpoint(rtd, 0);
         break;
      case COB_DBCALLBACK_EXIT:
         cob_runtime_debugger_output_event(rtd, "-event-program-exited");
         while (!dbc->flag_exit_debug && dbc->flag_debugger_active)
         {
            get_prompt(rtd, 1, 0);
         }
         break;
      case COB_DBCALLBACK_CPU_EXCEPTION:
         cob_runtime_debugger_output_event(rtd, "-event-cpu-exception");
         while (!dbc->flag_exit_debug && dbc->flag_debugger_active)
         {
            get_prompt(rtd, 1, 1);
         }
         break;
      case COB_DBCALLBACK_ENTER_EXCEPTION:
         if (dbc->flag_go_next_statement)
         {
            /* dbc->flag_stop_next_statement = 1; */
         }
         break;
      case COB_DBCALLBACK_SETLOCATION:
         /*to do I have a break point ?*/
         if (dbc->flag_stop_next_statement)
         {
            if (!debug_callback_fct_manage_break(rtd, dbc))
            {
               cob_runtime_debugger_output_event(rtd, "-event-end-stepping-range");
               get_prompt(rtd, 1, 1);
               /*cob_runtime_debugger_init();*/
               runtime_check_enterprise_key();
               cob_debug_callback = debug_callback_fct;
            }

         }
         else if (dbc->flag_go_next_statement &&
                  dbc->perform_count <= 0)
         {
            if (!debug_callback_fct_manage_break(rtd, dbc))
            {
               cob_runtime_debugger_output_event(rtd, "-event-end-stepping-range");
               get_prompt(rtd, 1, 1);
            }
         }
         else
         {
            debug_callback_fct_manage_break(rtd, dbc);
         }
         break;
   }
}

int
cob_runtime_debugger_did_parse(debug_context *dbc, char *s)
{
   char buffer[1024];
   char *p;
   char *d;
   int res = 0;

   strncpy(buffer, s, 1024);
   d = COB_STRTOK(buffer, ".", &p);
   if (d)
   {
      res = atoi(d);
      d = COB_STRTOK(NULL, ".", &p);
      if (d)
      {
         dbc->external_thread_no = atoi(d);
         dbc->external_pid = res;
      }
      else
      {
         dbc->external_thread_no = 0;
      }
   }
   dbc->external_did = res;
   return res;
}

char*
cob_runtime_debugger_pipe_name(debug_context *dbc, int bin, char *buffer)
{
   const char *type = (bin == 4 ? "r" : bin == 3 ? "p" : bin == 2 ? "e" : (bin ? "i" : "o"));
   char b[100];
   sprintf(b, "%d_%d", dbc->external_pid, dbc->external_thread_no);
#  ifndef _MSC_VER
   sprintf(buffer, "%s/debug_%s_%s.cit", cob_debug_tmp_dir(), b, type);
#  else
   if (bin != 4) sprintf(buffer, "\\\\.\\Pipe\\citdebug%s%s", b, type); //bin? "i": "o");
   else sprintf(buffer, "\\Pipe\\citdebug%s%s", b, type); //bin? "i": "o");
#  endif
   return (buffer);
}

#define CITDEBUGINFO_BASE "citdebuginfo_"

char*
cob_runtime_debugger_info_name_pid(debug_context *dbc, char *buffer)
{
   sprintf(buffer, "%s/%s%d.%d", cob_debug_tmp_dir(), CITDEBUGINFO_BASE, dbc->external_pid, dbc->external_thread_no);
   return buffer;
}

char*
cob_runtime_debugger_info_name_did(debug_context *dbc, char *buffer)
{
   sprintf(buffer, "%s/%s%d", cob_debug_tmp_dir(), CITDEBUGINFO_BASE, dbc->external_did);
   return buffer;
}

char*
cob_runtime_debugger_info_name(debug_context *dbc, char *buffer)
{
   if (dbc->external_did == dbc->external_pid)
   {
      return cob_runtime_debugger_info_name_pid(dbc, buffer);
   }
   else
   {
      return cob_runtime_debugger_info_name_did(dbc, buffer);
   }
}

char*
cob_runtime_debugger_info_base(COB_RTD,  char *buffer)
{
   strcpy(buffer, CITDEBUGINFO_BASE);
   return (buffer);
}


void
cob_runtime_debugger_write_info(COB_RTD, debug_context *const dbc)
{
   char buff[COB_SMALL_BUFF];
   FILE *f;

   if (!dbc->external_did)
   {
      dbc->external_did = dbc->external_pid;
   }
   sprintf(dbc->nameinfo, "%s", cob_runtime_debugger_info_name(dbc, buff));
   sprintf(dbc->namein, "%s", cob_runtime_debugger_pipe_name(dbc, 1, buff));
   sprintf(dbc->nameout, "%s", cob_runtime_debugger_pipe_name(dbc, 0, buff));
   sprintf(dbc->nameenv, "%s", cob_runtime_debugger_pipe_name(dbc, 2, buff));
   unlink(dbc->nameinfo);
   f = fopen(dbc->nameinfo, "w");
   if (f)
   {
      fprintf(f, "%d\n", dbc->external_pid);
      fprintf(f, "%d\n", dbc->external_thread_no);
      fprintf(f, "%s\n", dbc->namein);
      fprintf(f, "%s\n", dbc->nameout);
      fprintf(f, "%s\n", dbc->nameenv);
      if (rtd->current_module)
      {
         fprintf(f, "%s\n", rtd->current_module->module_name);
      }
      else if (rtd->debug_current_module_name)
      {
         fprintf(f, "%s\n", rtd->debug_current_module_name);
      }
      fclose(f);
      return;
   }
   else
   {
      fprintf(stderr, "COBOL-IT: Can't create Debugger info %s (%s)\n", dbc->nameinfo, cob_strerror(rtd, errno));
      fflush(stderr);
   }
}
#ifdef _WIN32
// CreateMyDACL.
//    Create a security descriptor that contains the DACL
//    you want.
//    This function uses SDDL to make Deny and Allow ACEs.
//
// Parameter:
//    SECURITY_ATTRIBUTES * pSA
//    Pointer to a SECURITY_ATTRIBUTES structure. It is your
//    responsibility to properly initialize the
//    structure and to free the structure's
//    lpSecurityDescriptor member when you have
//    finished using it. To free the structure's
//    lpSecurityDescriptor member, call the
//    LocalFree function.
//
// Return value:
//    FALSE if the address to the structure is NULL.
//    Otherwise, this function returns the value from the
//    ConvertStringSecurityDescriptorToSecurityDescriptor
//    function.

BOOL CreateMyDACL(SECURITY_ATTRIBUTES *pSA)
{
   // Define the SDDL for the DACL. This example sets
   // the following access:
   //     Built-in guests are denied all access.
   //     Anonymous logon is denied all access.
   //     Authenticated users are allowed
   //     read/write/execute access.
   //     Administrators are allowed full control.
   // Modify these values as needed to generate the proper
   // DACL for your application.
   TCHAR *szSD = TEXT("D:")       // Discretionary ACL
   TEXT("(A;OICI;GA;;;BG)")     //
                                // Allow full control
                                // built-in guests
   TEXT("(A;OICI;GA;;;AN)")     //
                                // Allow full control
                                // anonymous logon
   TEXT("(A;OICI;GA;;;AU)")     // Allow
                                // Allow full control
                                // to authenticated
                                // users
   TEXT("(A;OICI;GA;;;BA)");    // Allow full control
                                // to administrators

   if (NULL == pSA) return FALSE;

   return ConvertStringSecurityDescriptorToSecurityDescriptor(
      szSD,
      SDDL_REVISION_1,
      &(pSA->lpSecurityDescriptor),
      NULL);
}

#endif

void
cob_debug_identify_socket(COB_RTD)
{
   static const char *socket_type_command = "command";
   static const char *socket_type_event = "event";
   debug_context *const dbc = cob_get_debug_context(rtd);
   fprintf(dbc->cob_db_out, "%s:%d\n", socket_type_command, getpid());
   fprintf(dbc->cob_db_event_out, "%s:%d\n", socket_type_event, getpid());
   fflush(dbc->cob_db_out);
   fflush(dbc->cob_db_event_out);
}


static void
cob_debug_context_init(COB_RTD)
{
   int res;
   char *p;
   int fd;
   char buff[COB_SMALL_BUFF];
   debug_context *const dbc = cob_get_debug_context(rtd);
#  ifdef _MSC_VER
   SECURITY_ATTRIBUTES *pSA = NULL;
   SECURITY_ATTRIBUTES  sa;
   DWORD PipeErr;
#  else
   mode_t  mode = S_IRWXU | S_IRWXG;
#  endif


   if (dbc->cob_debug_inside_init)
   {
      return;
   }
   dbc->cob_debug_inside_init = 1;
   if (getenv("COB_DEBUG_ALLUSER")) rtd->debug_acl_alluser = 1;

#  ifndef _MSC_VER
   if (rtd->debug_acl_alluser)
   {
      mode = mode  | S_IRWXO;
   }
   if (!dbc->cob_db_in)
   {
      cob_runtime_debugger_write_info(rtd, dbc);
      unlink(dbc->namein);
      unlink(dbc->nameout);
      unlink(dbc->nameenv);
#        if HAVE_MKFIFO
      res = mkfifo(dbc->namein, mode);
      if (!res)
      {
         res = mkfifo(dbc->nameout, mode);
         res = mkfifo(dbc->nameenv, mode);
      }
      else
      {
         fprintf(stderr, "COBOL-IT: Can't create Debugger interface %s (%s)\n", dbc->namein, cob_strerror(rtd, errno));
         fflush(stderr);
         dbc->cob_debug_inside_init = 0;
         return;
      }
#        elif HAVE_MKNOD
      res = mknod(dbc->namein, S_IFIFO, 0);
      if (!res)
      {
         res = mknod(dbc->nameout, S_IFIFO | mode, 0);
         res = mknod(dbc->nameenv, S_IFIFO | mode, 0);
      }
      else
      {
         fprintf(stderr, "COBOL-IT: Can't create Debugger interface %s (%s)\n", dbc->namein, cob_strerror(rtd, errno));
         fflush(stderr);
         dbc->cob_debug_inside_init = 0;
         return;
      }
#        else
      fprintf(stderr, "COBOL-IT: Debugger interface not available on this platform\n");
      fflush(stderr);
      dbc->cob_debug_inside_init = 0;
      return;
#        endif
      if (rtd->debug_acl_alluser)
      {
         chmod(dbc->nameinfo, mode);
         chmod(dbc->namein, mode);
         chmod(dbc->nameout, mode);
         chmod(dbc->nameenv, mode);
      }
      if (res)
      {
         fprintf(stderr, "COBOL-IT: Can't create Debugger interface %s (%s)\n", dbc->nameout, cob_strerror(rtd, errno));
         fflush(stderr);
      }
      do
      {
         fd = open(dbc->namein, O_RDONLY);
         dbc->cob_db_in = NULL;
         if (fd > 0)
         {
            dbc->cob_db_in  = fdopen(fd, "r");
         }
         /* When debuuger is close , it may exit from open with an error*/
         if (dbc->flag_debugger_closing) return;
         if (!dbc->cob_db_in && (errno != EINTR))
         {
            fprintf(stderr, "COBOL-IT: Can't open Debugger interface %s for read (%s)\n", dbc->namein, cob_strerror(rtd, errno));
            fflush(stderr);
         }
      }
      while (!dbc->cob_db_in && errno == EINTR);
      do
      {
         fd = open(dbc->nameout, O_WRONLY);
         dbc->cob_db_out = NULL;
         if (fd > 0)
         {
            dbc->cob_db_out = fdopen(fd, "w");
         }
         if (!dbc->cob_db_out && (errno != EINTR))
         {
            fprintf(stderr, "COBOL-IT: Can't open Debugger interface %s for write (%s)\n", dbc->nameout, cob_strerror(rtd, errno));
            fflush(stderr);
            fclose(dbc->cob_db_in);
            dbc->cob_db_in = NULL;
         }
      }
      while (!dbc->cob_db_out && errno == EINTR);
      do
      {
         fd = open(dbc->nameenv, O_WRONLY);
         dbc->cob_db_event_out = NULL;
         if (fd > 0)
         {
            dbc->cob_db_event_out = fdopen(fd, "w");
         }
         if (!dbc->cob_db_event_out && (errno != EINTR))
         {
            fprintf(stderr, "COBOL-IT: Can't open Debugger interface %s for write (%s)\n", dbc->nameenv, cob_strerror(rtd, errno));
            fflush(stderr);
            fclose(dbc->cob_db_in);
            fclose(dbc->cob_db_out);
            dbc->cob_db_in = NULL;
            dbc->cob_db_out = NULL;
         }
      }
      while (!dbc->cob_db_event_out && errno == EINTR);
      cob_debug_identify_socket(rtd);
   }
#  else
/* MSC interface*/
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.bInheritHandle = FALSE;
   if (rtd->debug_acl_alluser)
   {
      if (CreateMyDACL(&sa))
      {

         pSA = &sa;
      }
   }
   if (dbc->cob_db_in && ferror(dbc->cob_db_in))
   {
      dbc->cob_db_in = NULL;
   }
   if (!dbc->cob_db_in)
   {
      //int remote = dbc->remote_gui != NULL;
      HANDLE ihd, ohd, ehd;

      cob_runtime_debugger_write_info(rtd, dbc);
      {
         //SECURITY_ATTRIBUTES  sa;

         //sa.nLength = sizeof(SECURITY_ATTRIBUTES);
         //sa.bInheritHandle = FALSE;

         // Call function to set the DACL. The DACL
         // is set in the SECURITY_ATTRIBUTES
         // lpSecurityDescriptor member.
         //if (!CreateMyDACL(&sa))
         //{
         // Error encountered; generate message and exit.
         //	 printf("Failed CreateMyDACL\n");
         //	 exit(1);
         // }
         ihd = CreateNamedPipe(dbc->namein, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 1024, 1024, 100, pSA);
         if (dbc->flag_debugger_closing) return;
         if (ihd == INVALID_HANDLE_VALUE)
         {
            fprintf(stderr, "COBOL-IT: Can't create Debugger interface %s (%s)\n", dbc->namein, cob_strerror(rtd, GetLastError()));
            fflush(stderr);
            dbc->cob_debug_inside_init = 0;
            return;
         }
         ohd = CreateNamedPipe(dbc->nameout, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 1024, 1024, 100, pSA);
         if (ohd == INVALID_HANDLE_VALUE)
         {
            fprintf(stderr, "COBOL-IT: Can't create Debugger interface %s (%s)\n", dbc->nameout, cob_strerror(rtd, GetLastError()));
            fflush(stderr);
            dbc->cob_debug_inside_init = 0;
            return;
         }
         ehd = CreateNamedPipe(dbc->nameenv, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 1024, 1024, 100, pSA);
         if (ehd == INVALID_HANDLE_VALUE)
         {
            fprintf(stderr, "COBOL-IT: Can't create Debugger interface %s (%s)\n", dbc->nameenv, cob_strerror(rtd, GetLastError()));
            fflush(stderr);
            dbc->cob_debug_inside_init = 0;
            return;
         }

      }

      do
      {
         dbc->cob_db_in = 0;
         PipeErr = 0;
         ConnectNamedPipe(ihd, NULL);
         PipeErr = GetLastError();
         if (PipeErr == 0 || PipeErr == ERROR_PIPE_CONNECTED)
         {
            dbc->cob_db_in = fdopen(_open_osfhandle((intptr_t)ihd, 0), "r");
            /* When debuuger is close , it may exit from open with an error*/
            if (dbc->flag_debugger_closing) return;
            if (!dbc->cob_db_in)
            {
               fprintf(stderr, "COBOL-IT: Can't open Debugger interface %s for read (%s)\n", dbc->namein, cob_strerror(rtd, GetLastError()));
               fflush(stderr);
            }
         }
      }
      while (!dbc->cob_db_in);

      ConnectNamedPipe(ohd, NULL);
      dbc->cob_db_out = fdopen(_open_osfhandle((intptr_t)ohd, 0), "w");
      ConnectNamedPipe(ehd, NULL);
      dbc->cob_db_event_out = fdopen(_open_osfhandle((intptr_t)ehd, 0), "w");
      cob_debug_identify_socket(rtd);

   }
#  endif

   if (!dbc->cob_db_trace)
   {
      p = getenv("COB_DEBUG_TRACE");
      if (p)
      {
         sprintf(buff, "%s_%d.txt", p, dbc->external_did);
         dbc->cob_db_trace = fopen(buff, "w");
         if (!dbc->cob_db_trace)
         {
            fprintf(stderr, "COBOL-IT: Can't create Debugger trace %s for write (%s)\n", buff, cob_strerror(rtd, errno));
            fflush(stderr);
         }
      }

   }
   dbc->cob_debug_inside_init = 0;
}

static void
cob_runtime_debugger_start_thread(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   int res = 0;

   if (dbc->startingReadingThread)
   {
      return;
   }
   dbc->startingReadingThread = 1;
   if (dbc->debugger_thread == NULL)
   {
#     ifdef _MSC_VER
      HANDLE *h;
      DWORD   dwThreadIdArray;

      dbc->debugger_thread = cob_malloc(rtd, sizeof(HANDLE));
      h = dbc->debugger_thread;
      *h = CreateThread(
           NULL,                   // default security attributes
           0,                      // use default stack size
           debug_thread_proc,     // thread function name
           rtd,                   // argument to thread function
           0,                      // use default creation flags
           &dwThreadIdArray);      // returns the thread identifier
      if (*h == NULL) res = -1;
      else while (!((volatile int)dbc->cob_debug_thread_started))
         {
            Sleep(1);
         }

#     else
#        if HAVE_PTHREAD_H
      pthread_t *t;

      dbc->debugger_thread = cob_malloc(rtd, sizeof(pthread_t));
      t = dbc->debugger_thread;
      res = pthread_create(t, NULL, debug_thread_proc, rtd);
      while (!res && !((volatile int)dbc->cob_debug_thread_started))
      {
         sched_yield();
      }
#        endif
#     endif
      if (res)
      {
         dbc->flag_debugger_active = 0;
         fprintf(stderr, "COBOL-IT: Can't create Debugger thread (%s)\n", cob_strerror(rtd, errno));
         fflush(stderr);
         cob_free(dbc->debugger_thread);
         dbc->debugger_thread = NULL;
         return;
      }
      if (dbc->cob_db_trace)
      {
         fprintf(dbc->cob_db_trace, "Debugger reading thread started\n");
      }

   }
}

void
cob_runtime_debugger_close_module(COB_RTD, cob_module *mod)
{
   int i;
   if (mod->debugdb_moduleid > 0)
   {
      if (mod->debugdb_fdi_store)
      {
         struct cob_field_debug_info     *p = mod->debugdb_fdi_store;
         for (i = 0; i < mod->debugdb_fdi_count; i++)
         {
            if (p->name)
            {
               free((void *)p->name);
            }
            p++;
         }
         free(mod->debugdb_fdi_store);
         mod->debugdb_fdi_store = NULL;
         mod->debugdb_fields_debug_info = NULL;
      }
      if (mod->lines_debug_info && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
      {
         struct cob_lines_debug_info     *p = mod->lines_debug_info;
         char *last_source_file = NULL;
         for (i = 0; i < mod->debugdb_lines_count; i++)
         {
            if (p->label)
            {
               free((void *)p->label);
            }
            if (p->source_file && p->source_file != last_source_file)
            {
               last_source_file = (char *)p->source_file;
               free((char *)p->source_file);
            }
            p++;
         }
         free(mod->lines_debug_info);
         mod->lines_debug_info = NULL;
         mod->debugdb_cachepath = NULL;
      }
      mod->debugdb_moduleid = 0;
      mod->debugdb_fdi_count = 0;
      mod->debugdb_lines_count = 0;
   }

}

int
cob_runtime_debugger_cleanup(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char buffer[COB_MEDIUM_BUFF];
   dbc->flag_debugger_active = 0;
   dbc->flag_debugger_closing = 1;
   if (dbc->debugger_thread)
   {
#     ifdef _MSC_VER
      HANDLE *h = dbc->debugger_thread;
      TerminateThread(*h, 0);
#     else
#        if HAVE_PTHREAD_H
      pthread_t *t = dbc->debugger_thread;
      pthread_cancel(*t);
      if (dbc->testmemory_thread)
      {
         pthread_t *t = dbc->testmemory_thread;
         pthread_cancel(*t);
         dbc->testmemory_thread = NULL;
      }
#        endif
#     endif
      dbc->debugger_thread = NULL;
   }
   if (dbc->testmemory_fd[0])
   {
      close(dbc->testmemory_fd[0]);
   }
   if (dbc->testmemory_fd[1])
   {
      close(dbc->testmemory_fd[1]);
   }
   if (dbc->cob_db_in)
   {
      fclose(dbc->cob_db_in);
      dbc->cob_db_in = NULL;
   }
   if (dbc->cob_db_out)
   {
      fclose(dbc->cob_db_out);
      dbc->cob_db_out = NULL;
   }
   if (dbc->cob_db_event_out)
   {
      fclose(dbc->cob_db_event_out);
      dbc->cob_db_event_out = NULL;
   }
   if (dbc->cob_db_trace)
   {
      fclose(dbc->cob_db_trace);
      dbc->cob_db_trace = NULL;
   }
   if (dbc->command_buff.buffer)
   {
      cob_free(dbc->command_buff.buffer);
   }
   if (rtd->debug_current_module_name)
   {
      cob_free(rtd->debug_current_module_name);
   }
   unlink(dbc->nameenv);
   unlink(dbc->nameout);
   unlink(dbc->namein);
   unlink(dbc->nameinfo);
   unlink(cob_runtime_debugger_info_name_did(dbc, buffer));
   unlink(cob_runtime_debugger_info_name_pid(dbc, buffer));
   return (0);
}

/*
static void 
cob_minimal_init (COB_RTD) {
    char *s;
    if ( !rtd->cob_initialized ) {
        s = getenv ("COB_ERROR_FILE");
        rtd->cob_err_file = stderr;
        if ( s ) {
            FILE *f;
            f = fopen(s, "a");
            if ( f ) {
                rtd->cob_err_file = f;
            }
        }
        rtd->cob_tmpdir = cob_debug_tmp_dir();
    }

}
*/

/* Initalise the debbuger context if needed, and requiere break at next statement*/
int
rtd_cob_runtime_debugger_activate(void)
{
   COB_RTD = cob_get_rtd();
   return cob_runtime_debugger_activate(rtd, 0);
}

static int
cob_runtime_debugger_install_thread(COB_RTD, int did)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   char *s;

   if (did == 0)
   {
      if (dbc->external_did == 0)
      {
         s = getenv("COB_DEBUG_ID");
         if ((did == 0) && s)
         {
            did = atoi(s);
         }
         if (did == 0)
         {
            did = getpid();
         }
      }
      else
      {
         did = dbc->external_did;
      }
   }

   dbc->external_did = did;
   dbc->external_pid = getpid();
   cob_runtime_debugger_write_info(rtd, dbc);

   cob_runtime_debugger_start_thread(rtd);
   return (0);
}

int
cob_runtime_debugger_activate(COB_RTD, int did)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   cob_lock_mutex();
   runtime_check_enterprise_key();
   cob_runtime_debugger_install_thread(rtd, did);
   cob_debug_callback = debug_callback_fct;
   dbc->flag_stop_next_statement = 1;
   cob_unlock_mutex();
   while (!cob_get_debug_context(rtd)->flag_debugger_active) debug_nano_wait();

   return (0);
}

int
cob_runtime_debugger_init_rtd(COB_RTD)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   int did = 0;
   char *s;
   rtd->runtime_debug_started = 1;
   if (dbc->debugger_thread == NULL)
   {
      s = getenv("COB_DEBUG_MAX_OCCURS");
      if (s)
      {
         debug_max_occur = atoi(s);
      }
      s = getenv("COB_DEBUG_ID");
      if (s)
      {
         did = atoi(s);
      }
      if (did == 0)
      {
         did = getpid();
      }
      dbc->external_did = did;
      dbc->external_pid = getpid();
      cob_lock_mutex();
      cob_runtime_debugger_install_thread(rtd, did);
      cob_unlock_mutex();
   }
   return (0);
}

int
cob_runtime_debugger_init(void)
{
   COB_RTD = cob_get_rtd();
   return cob_runtime_debugger_init_rtd(rtd);
}

void
cob_runtime_debugger_set_module_name(char *name)
{
   COB_RTD = cob_get_rtd();
   if (rtd->debug_current_module_name)
   {
      cob_free(rtd->debug_current_module_name);
   }
   if (name)
   {
      rtd->debug_current_module_name = strdup(name);
   }
   else
   {
      rtd->debug_current_module_name = NULL;
   }
}


int
cob_runtime_remote_debugger_init(void)
{
   COB_RTD = cob_get_rtd();
   debug_context *const dbc = cob_get_debug_context(rtd);
   dbc->remote_gui = (char *)"";
   return cob_runtime_debugger_init();
}

/*CobolIT*/
static cob_ufield*
debug_build_if_match(cob_module *mod,
                     cob_ufield *field_buffer,
                     debug_fld_reques_type *field_request,
                     int *fld_indexes,
                     const char *prefix, int base_offset,
                     char *full_field_name, cob_field_debug_info **found_fdi)
{
   cob_field *f;
   char buffer[COB_SMALL_BUFF];
   char pbuffer[COB_SMALL_BUFF];
   cob_field_debug_info *fdi;

   buffer[0] = 0;
   for (;; field_request++)
   {
      fdi = field_request->fdi;
      if (fdi)
      {
         strcat(buffer,  fdi->name);
         if ((fdi->occur > 1) && fld_indexes && *fld_indexes)             
         {
            if (fdi->occur >= *fld_indexes)
            {
               sprintf(pbuffer, "(%d)", *fld_indexes);
               strcat(buffer, pbuffer);
               try
               {
                  f = mod->get_cob_field(mod->data_storage, fdi->field_id);                  
               }
               catch (RuntimeException)
               {
                  f = NULL;
               }
               if (f && f->attr && f->data)
               {
                  if (!COB_FIELD_IS_BITSARRAY(f))
                  {
                     base_offset += ((*fld_indexes - 1) * f->size);
                  }
                  else
                     field_request->bitfield_index = *fld_indexes;
               }
            }
            fld_indexes++;
         }
      }
      if ((field_request + 1)->name == NULL)
      {
         break;
      }
      strcat(buffer,  ".");
   }

   try
   {
      f = mod->get_cob_field(mod->data_storage, fdi->field_id);
   }
   catch (RuntimeException)
   {
      f = NULL;
   }
   if (f && f->attr && f->data && field_request && field_buffer)
   {
      field_buffer->fld = *f;
      if (COB_FIELD_IS_BITSARRAY(f))
      {
         field_buffer->ba_fld.index = field_request->bitfield_index - 1;
         if (field_buffer->ba_fld.index < 0)
         {
            field_buffer->ba_fld.index = 0;
         }
      }
      field_buffer->fld.data += base_offset;
      if (full_field_name)
      {
         sprintf(full_field_name, "%s%s", prefix, buffer);
      }
      if (found_fdi)
      {
         *found_fdi = fdi;
      }
      return (field_buffer);
   }
   return (NULL);
}

static int
debug_could_match(cob_field_debug_info *fdi, debug_fld_reques_type *field_request, int recurse)
{
   int  i;

   /*printf("fn = %s  : buff = %s \n", field_request->name, fdi->name);*/

   if ((strcasecmp(fdi->name, field_request->name) == 0))
   {
      field_request->fdi = fdi;
      if ((field_request + 1)->name == NULL)
      {
         return (1);
      }
      else if (fdi->child)
      {
         for (i = 0; fdi->child[i].field_id; i++)
         {
            if (debug_could_match(&fdi->child[i], field_request + 1, 1))
            {
               return 1;
            }
         }
      }
   }
   else if (recurse && fdi->child)
   {
      for (i = 0; fdi->child[i].field_id; i++)
      {
         if (debug_could_match(&fdi->child[i], field_request, 1))
         {
            return 1;
         }
      }
   }
   return (0);
}


static cob_ufield*
debug_find_field(COB_RTD, cob_module *mod,
                 cob_ufield *field_buffer,
                 cob_field_debug_info *fdi,
                 debug_fld_reques_type *field_request,
                 int *field_indexes,
                 const char *prefix, int base_offset,
                 char *full_field_name,
                 cob_field_debug_info **found_fdi)
{
   char buffer[COB_SMALL_BUFF];
   cob_field *f;
   cob_field sf;
   cob_ufield *p;
   void *module_storage = mod->data_storage;


   int i, l;
   int off;

   if (found_fdi)
   {
      *found_fdi = NULL;
   }
   fdi = cob_runtime_debugger_open_field_debugdb(rtd, mod, fdi);

   for (i = 0; fdi && fdi[i].name; i++)
   {
      if (debug_could_match(&fdi[i], field_request, 0))
      {
         p = debug_build_if_match(mod, field_buffer,
                                  field_request, field_indexes,
                                  prefix,  base_offset, full_field_name, found_fdi);
         if (p)
         {
            return (p);
         }
      }

      if (fdi[i].child)
      {
         if (fdi[i].occur > 1)
         {
            try
            {
               f = mod->get_cob_field(module_storage, fdi[i].field_id);
            }
            catch (RuntimeException)
            {
               f = NULL;
            }
            if (f && f->data && f->attr)
            {
               sf = *f;
               l = 0;
               if (field_indexes){
                  if (*field_indexes) l = *field_indexes -1;
                  field_indexes++;
               }
               off = base_offset;
               sprintf(buffer, "%s%s(%d).", prefix, fdi[i].name, l + 1);
               if (!COB_FIELD_IS_BITSARRAY(&sf))
               {
                  off = base_offset + (l * sf.size);
               }
               p = debug_find_field(rtd, mod, field_buffer,  fdi[i].child, field_request, field_indexes, buffer, off,
                                    full_field_name, found_fdi);
               if (p)
               {
                  return (p);
               }              
            }
         }
         else
         {
            sprintf(buffer, "%s%s.", prefix, fdi[i].name);
            p = debug_find_field(rtd, mod, field_buffer,  fdi[i].child, field_request, field_indexes, buffer, base_offset,
                                 full_field_name, found_fdi);
            if (p)
            {
               return (p);
            }
         }
      }
   }
   return (NULL);
}

static cob_ufield*
cob_debug_find_cob_field_1(COB_RTD,
                           cob_module *mod,
                           char *field_name,
                           cob_ufield *field_buffer,
                           char *full_field_name,
                           cob_field_debug_info **found_fdi,
                           int *enable_cache)
{
   cob_ufield *result = NULL;
   debug_fld_reques_type   debug_field_name[99];
   int field_indexes[99];
   /*char *field_name_copy = strdup(field_name); */
   char field_name_copy[COB_SMALL_BUFF];
   int i = 0;
   int idx_i = 0;
   char *ptk;
   char *p;
   char *save = NULL;
   int asked_idx =1;
   strncpy(field_name_copy, field_name, sizeof(field_name_copy));
   ptk = field_name_copy;
   if (found_fdi)
   {
      *found_fdi = NULL;
   }
   memset (debug_field_name, 0, sizeof(debug_field_name));
   memset (field_indexes, 0, sizeof(field_indexes)); 
   do
   {
      debug_field_name[i].name = COB_STRTOK(ptk, ".", &save);
      ptk = NULL;
      if (debug_field_name[i].name)
      {
         p = strchr(debug_field_name[i].name, '(');
         if (p)
         {
            asked_idx = 1;
            *p = 0;
            p++;
            if (*p)
            {
               asked_idx = atoi(p);
               if (asked_idx <= 0 || isalpha(*p))
               {
                  char *var = strdup(p);
                  char *q;
                  q = strchr(var, ')');
                  if (q)
                  {
                     cob_ufield found_cob_field;
                     cob_field *fidx;
                     *q = 0;
                     fidx = debug_get_cob_field_from_name(rtd, cob_get_debug_context(rtd), var, &found_cob_field);
                     if (fidx)
                     {
                        asked_idx = cob_get_int(rtd, fidx);
                        if (enable_cache)
                        {
                           *enable_cache = 0;
                        }
                     }
                  }
                  free(var);
                  if (asked_idx <= 0) asked_idx=1;
               }
               field_indexes[idx_i++] = asked_idx;
            }
         }
      }
   }
   while (debug_field_name[i++].name && i < 99);
   if (mod  && mod->fields_debug_info && mod->get_cob_field)
   {
      result = debug_find_field(rtd, mod, field_buffer, mod->fields_debug_info, debug_field_name, field_indexes,
                                "", 0, full_field_name, found_fdi);
   }
   /*free(field_name_copy);*/
   return result;
}

cob_ufield*
cob_debug_find_cob_field(COB_RTD,
                         cob_module *mod,
                         char *field_name,
                         cob_ufield *field_buffer,
                         char *full_field_name)
{
   cob_field_debug_info *found_fdi;
   return cob_debug_find_cob_field_1(rtd, mod, field_name, field_buffer, full_field_name, &found_fdi, NULL);
}


debug_context* cob_get_debug_context(COB_RTD)
{

   if (!debug_region_separated)
   {
      cit_runtime_t *rtd_r0 = cob_enterprise_get_region_0(rtd);
      return &rtd_r0->cob_debug_context;
   }
   else
   {
      return &rtd->cob_debug_context;
   }
}

int cob_runtime_set_debug_region_separated(COB_RTD, int activate)
{
   debug_region_separated = activate;
   return activate;
}

cob_field * cob_debug_true_false (COB_RTD, int val)
{
    static cob_field_attr  attr;
    static cob_field   field;

    COB_ATTR_INIT(COB_TYPE_ALPHANUMERIC, 0, 0, 0, NULL);
    field.attr = &attr;
    if (val) {
        field.data = (unsigned char *)"TRUE";
        field.size = 4;
    } else {
        field.data = (unsigned char *)"FALSE";
        field.size = 5;
    }
    return &field;
}


static char* filename_replace(COB_RTD, const char *oldname, char *newname)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   replace_buffer_t *z1 = dbc->replace_list;
   strcpy(newname, oldname);
   while (z1)
   {
      if (strstr(oldname, z1->oldprefix) == oldname)
      {
         strcat(strcpy(newname, z1->newprefix), oldname + strlen(z1->oldprefix));
         break;
      }
      z1 = z1->next;
   }
   return newname;
}

/* Line display */

static void debug_display_line(COB_RTD, cob_module *mod, int start, int end)
{
   debug_context *const dbc = cob_get_debug_context(rtd);
   int debug_idx = -1;
   char buff[COB_SMALL_BUFF];
   char *p;
   int error = 0;
   char debug_new_source_file[COB_SMALL_BUFF];
   if (dbc->source_fname != mod->debug_cur_source_file)
   {
      if (dbc->source != NULL)
      {
         fclose(dbc->source);
      }
      dbc->source = NULL;
      dbc->source_fname = NULL;
      dbc->source_linenr = 0;
   }
   if (dbc->source == NULL && mod->debug_cur_source_file)
   {
      dbc->source = fopen(filename_replace(rtd, mod->debug_cur_source_file, debug_new_source_file), "r");

      if (dbc->source)
      {
         dbc->source_fname = mod->debug_cur_source_file;
         dbc->source_linenr = 0;
      }
   }
   if (dbc->source)
   {
      if (start <= dbc->source_linenr)
      {
         rewind(dbc->source);
         dbc->source_linenr = 0;
      }
      while (dbc->source_linenr < (start - 1) && !feof(dbc->source))
      {
         p = fgets(buff, sizeof(buff), dbc->source);
         if (p)
         {
            dbc->source_linenr++;
         }
      }
      if (feof(dbc->source))
      {
         error = 1;
      }
      else
      {
         cob_runtime_debugger_open_lines_debugdb(rtd, mod);
         if (mod->lines_debug_info && mod->lines_debug_info_version >= LINES_DEBUG_INFO_VERSION)
         {
            debug_idx = 0;
            while (mod->lines_debug_info[debug_idx].line_nr && (mod->lines_debug_info[debug_idx].line_nr <= dbc->source_linenr))
            {
               debug_idx++;
            }
         }

         while (dbc->source_linenr < (end) && !feof(dbc->source))
         {
            p = fgets(buff, sizeof(buff), dbc->source);
            if (p)
            {
               char c = '.';
               dbc->source_linenr++;
               if (debug_idx >= 0 && mod->lines_debug_info[debug_idx].line_nr)
               {
                  if (mod->lines_debug_info[debug_idx].line_nr == dbc->source_linenr)
                  {
                     if (mod->lines_debug_info[debug_idx].debug_status)
                     {
                        c = '*';
                     }
                     debug_idx++;
                  }
               }
               if (dbc->source_linenr == mod->debug_cur_line)
               {
                  c = '>';
               }
               fout(rtd, ".%07d%c %s", dbc->source_linenr, c, p);
            }
         }
      }
   }
   else
   {
      error = 1;
   }

   if (error  && ((end - start) != 0))
   {
      out_error(rtd, "no sources");
   }

}
#ifdef HAVE_PTHREAD_H
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void
cob_lock_mutex(void)
{
   pthread_mutex_lock(&mutex);
}

void
cob_unlock_mutex(void)
{
   pthread_mutex_unlock(&mutex);
}
#else
#  ifdef _WIN32
static HANDLE ghMutex = NULL;

void
cob_lock_mutex(void)
{
   if (!ghMutex)
   {
      ghMutex = CreateMutex(NULL,              // default security attributes
                            FALSE,             // initially not owned
                            NULL);             // unnamed mutex
   }
   WaitForSingleObject(ghMutex,    // handle to mutex
                       INFINITE);  // no time-out interval

}

void
cob_unlock_mutex(void)
{
   if (ghMutex)
   {
      ReleaseMutex(ghMutex);
   }
}

#  else
void
cob_lock_mutex(void)
{
}
void
cob_unlock_mutex(void)
{
}
#  endif
#endif


/*
 * Copyright (C) 2004-2008 Roger While
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

#include	<stdio.h>
#include	<string.h>
#include	<libcob.h>
#ifdef _MSC_VER
    #include <windows.h>
#endif
#define COB_PN_RUNTIME   "RUNTIME"

static void
print_usage (void)
{
    printf ("Usage: cobcrun PROGRAM [param ...]\n");
    printf ("or   : cobcrun --help (-h)             Print this help\n");
    printf ("or   : cobcrun --version (-V)          Print Version\n");
    printf ("or   : cobcrun --debug (-d)            Suspend and wait debugger\n");
	printf ("or   : cobcrun --debug (-d) --remote (-r) Same as above but uses a separate file for events\n");
    printf ("or   : cobcrun --reload                Reload checkpoint\n");
    printf ("or   : cobcrun --checkpoint <checkpoint file> Set checkpoint filename\n");
#ifdef _MSC_VER
    printf ("or   : cobcrun --console (-c) Create a new console\n");
#endif
    printf ("       \n");
}


int
main (int argc, char **argv)
{
    int debug = 0;
    int cnt = 1;
    int found = 1;
    int cparam =0;
	int remote = 0;
    union {
        int (*func)();
        void    *func_void;
    } unifunc;
    COB_RTD = cob_get_rtd(); 

    if ( argc <= 1 ) {
        print_usage ();
        return 1;
    }
    if ( !strncmp (argv[1], "--help", 10) ||
         !strncmp (argv[1], "-h", 4) ) {
        print_usage ();
        return 0;
    }
    if ( !strncmp (argv[1], "--version", 10) ||
         !strncmp (argv[1], "-V", 4) ) {
        cob_print_version (COB_PN_RUNTIME);
        return 0;
    }
    if ( !strncmp (argv[1], "--debug", 10) ||
         !strncmp (argv[1], "-d", 4) ) {
        debug = 1;
        cnt ++;
    }
    while ((cnt < (argc-1)) && found) {
        found = 0;
        if ( !strncmp (argv[cnt], "--C-param", 10) &&
             cnt < (argc-1) ) {
            cnt ++;
            cparam=1;
        }
        if ( !strncmp (argv[cnt], "--reload", 10) &&
             cnt < (argc-1) ) {
            cnt ++;
            cob_set_context_mode(rtd, COB_CONTEXT_RELOAD);
        }
        if ( !strncmp (argv[cnt], "--checkpoint-erase", 20) &&
             cnt < (argc-1) ) {
            cnt ++;
            found = 1;
            cob_set_auto_erase_context_file(rtd, 1);
        }
        if ( !strncmp (argv[cnt], "--checkpoint", 10) &&
             cnt < (argc-1) ) {
            cnt ++;
            cob_set_context_appli_prefix(rtd, argv[cnt]);
            cnt ++;
        }
        if( !strncmp (argv[cnt], "--remote", 10) ||
            !strncmp(argv[cnt], "-r", 4) ) {
            remote = 1;
            cnt ++;
        }
        if ( !strncmp (argv[cnt], "--console", 10) ||
             !strncmp (argv[cnt], "-c", 4) ) {
    #ifdef _MSC_VER
            FreeConsole();
            AllocConsole();
    #endif
            cnt ++;
        }

    }
    if ( cnt >= argc ) {
        fprintf(stderr, "Invalid PROGRAM name\n");
        return 1;
    }
    cob_init (rtd, argc - cnt, &argv[cnt]);
    unifunc.func_void = cob_resolve (rtd, argv[cnt]);
    if ( unifunc.func_void == NULL ) {
        cob_call_error (rtd);
    }
    if ( debug ) {
		if( remote ) {
			cob_runtime_remote_debugger_init();
		}
		else 
        {
			cob_runtime_debugger_init ();
		}
        cob_runtime_debugger_activate(rtd, 0);
    }
    if ( cparam ) {
#ifdef  WORDS_BIGENDIAN
        cob_stop_run (rtd, unifunc.func (argc,argv,NULL,NULL));
#else        
        cob_stop_run (rtd, unifunc.func (COB_BSWAP_32_CONSTANT(argc),argv,NULL,NULL));
#endif
    } else
    {
        cob_stop_run (rtd, unifunc.func(cob_linkage_command_line(rtd), NULL, NULL, NULL, NULL, NULL) );
    }
}

#ifdef _MSC_VER
int __stdcall WinMain (void*h, void*ph, const char *c, int wc)
{
    int locargc;
    static char *locargv[64];

    locargc = 0;

    memset (locargv, 0, sizeof(locargv));
    cob_cmdline_to_argv(cob_get_rtd(), "cobcdb",c, &locargc, locargv, 64);
    return main (locargc, locargv);
}
#endif

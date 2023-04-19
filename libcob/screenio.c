/*
 * Copyright (C) 2001-2007 Keisuke Nishida
 * Copyright (C) 2007 Roger While
  * Copyright (C) 2008-2020 Cobol-IT
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1,
 * or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

#include "config.h"
#include "config.h"
#include "defaults.h"
#include "globaldefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _MSC_VER
    #include <windows.h>
#endif
#ifdef	HAVE_UNISTD_H
    #include <unistd.h>
#endif

#ifdef HAVE_NCURSES_H
    #include <ncurses.h>
    #define COB_GEN_SCREENIO
#elif defined(HAVE_NCURSES_NCURSES_H)
    #include <ncurses/ncurses.h>
    #define COB_GEN_SCREENIO
#elif defined(HAVE_PDCURSES_H)
    #include <pdcurses.h>
    #define COB_GEN_SCREENIO
#elif defined(HAVE_CURSES_H)
    #include <curses.h>
    #define COB_GEN_SCREENIO
#else
    #error "CIT Need a curses like library"
#endif

#include "move.h"
#include "coblocal.h"
#include "screenio.h"
#include "termio.h"
#include "intrinsic.h"

/* Global variables */

#ifdef COB_GEN_SCREENIO

struct cob_inp_struct {
    cob_screen  *scr;
    size_t      up_index;
    size_t      down_index;
    int         this_y;
    int         this_x;
};

    #define	COB_INP_SIZE	1920 * sizeof(struct cob_inp_struct)
static char FieldInputFiller = ' ';
static int FieldInputAttribute = 0;
static int InsertToggleMode = 0;
static int Debug_Screen = 0;
/* Local variables */

/* Local functions */
static void refresh_accept_field (COB_RTD, cob_field *f,const int attr, const char prompt);
static int  local_cob_field_accept (COB_RTD, cob_field *f, int input_size, 
                                    int sline, int scolumn,
                                    int *pccolumn,
                                    int fgc, int bgc, 
                                    const int attr, const char prompt, int forscreen);

static void 
cob_screen_check_control (COB_RTD, cob_screen *s);

static void CIT_addch(COB_RTD,int a) 
{
    int line, col;
    int mline, mcol;
    getyx (stdscr, line, col);
    addch(a); 
    getyx (stdscr, mline, mcol);
    if ( line != mline ) {
        rtd->cob_screen_eol=1;
    } else {
        rtd->cob_screen_eol=0;
    }
    if ( Debug_Screen )
        refresh();
}

static void CIT_addnstr (COB_RTD,char *data, int size)
{
    int i;
    for ( i=0 ; i < size && *data; i++ ) {
        CIT_addch(rtd, *data);
        data ++;
    }

}

static int 
map_ctr_status (COB_RTD, int fret)
{
    char         datbuf[8];
    unsigned int stat = fret;

    if ( rtd->current_module ) {
        if ( cob_enterprise_map_ctr_status(rtd, fret, &stat) )
            return stat;
        if ( rtd->current_module->crt_status ) {
            if ( COB_FIELD_IS_NUMERIC (rtd->current_module->crt_status) ) {
                cob_set_int (rtd, rtd->current_module->crt_status, fret);
            } else {
                sprintf(datbuf, "%4.4d", fret);
                memcpy (rtd->current_module->crt_status->data, datbuf, min(rtd->current_module->crt_status->size, 4));
            }
        }
    }
    return(int)stat;
}

static int 
cob_check_move_pos (COB_RTD, int * line, int * col, int force)
{
    cob_field   *f;
    int         i;
    int         sline;
    int         scolumn;

    if ( rtd->current_module->cursor_pos ) {
        f = rtd->current_module->cursor_pos;
        if ( force ||  memcmp(f->data, rtd->cursorpos_shadow, min(f->size, 6)) ) {
            i = cob_get_int(rtd,f);
            if ( COB_FIELD_IS_NUMERIC (f) &&
                 COB_FIELD_TYPE (f) != COB_TYPE_NUMERIC_DISPLAY ) {
                sline = i / 1000;
                scolumn = i % 1000;
            } else {
                if ( f->size < 6 ) {
                    sline = i / 100;
                    scolumn = i % 100;
                } else {
                    sline = i / 1000;
                    scolumn = i % 1000;
                }
            }
            sline --;
            scolumn --;
            getyx (stdscr, *line, *col);
            if ( sline >= 0 )   *line = sline;
            if ( scolumn >= 0 ) *col = scolumn;
            return 1;
        }
    }
    return 0;
}

static void
get_line_column (COB_RTD, cob_field *fline, cob_field *fcol, int *line, int *col)
{
    int l;
    int c;
    int p;

    getyx (stdscr, *line, *col);
    cob_check_move_pos(rtd,line,col, 0); 
    if ( fline == NULL && fcol == NULL ) {
        return;
    } else if ( fline == NULL ) {
        c = cob_get_int (rtd, fcol);
        if ( c > 0 ) {
            c--;
        }
        *col = c;
        return;
    }

    p = cob_get_int (rtd, fline);

    if ( fcol == NULL ) {
        if ( fline->size == 4 ) {
            l = p / 100;
            c = p % 100;
        } else if (fline->size <= 2){
            if (p >0) {
                p--;
            }
            *line = p;
            return;
        } else{
            l = p / 1000;
            c = p % 1000;
        }
    } else {
        l = p;
        c = cob_get_int (rtd, fcol);
    }
    if ( l > 0 ) {
        l--;
    }
    if ( c > 0 ) {
        c--;
    }
    *line = l;
    *col = c;
}

static void
cob_screen_attr (COB_RTD, int fgc, int bgc, const int attr, int for_display)
{
    size_t      i;
    int         styles = 0;
    int         line;
    int         column;
    short       fgcolor;
    short       bgcolor;
    short       fgdef;
    short       bgdef;

    attrset (A_NORMAL);
    if ( attr & COB_SCREEN_REVERSE ) {
        styles |= A_REVERSE;
    }
    if ( attr & COB_SCREEN_HIGHLIGHT ) {
        styles |= A_BOLD;
    }
    if ( attr & COB_SCREEN_BLINK ) {
        styles |= A_BLINK;
    }
    if ( attr & COB_SCREEN_UNDERLINE ) {
        styles |= A_UNDERLINE;
    }
    if ( styles ) {
        attron (styles);
    }
    if ( (rtd->cob_has_color) ) {
        fgcolor = (rtd->fore_color);
        bgcolor = (rtd->back_color);
        if ( fgc >= 0 ) {
            switch ( fgc ) {
                case COB_SCREEN_BLACK:
                    fgcolor = COLOR_BLACK;
                    break;
                case COB_SCREEN_BLUE:
                    fgcolor = COLOR_BLUE;
                    break;
                case COB_SCREEN_GREEN:
                    fgcolor = COLOR_GREEN;
                    break;
                case COB_SCREEN_CYAN:
                    fgcolor = COLOR_CYAN;
                    break;
                case COB_SCREEN_RED:
                    fgcolor = COLOR_RED;
                    break;
                case COB_SCREEN_MAGENTA:
                    fgcolor = COLOR_MAGENTA;
                    break;
                case COB_SCREEN_YELLOW:
                    fgcolor = COLOR_YELLOW;
                    break;
                case COB_SCREEN_WHITE:
                    fgcolor = COLOR_WHITE;
                    break;
                default:
                    break;
            }
        }
        if ( bgc >= 0 ) {
            switch ( bgc ) {
                case COB_SCREEN_BLACK:
                    bgcolor = COLOR_BLACK;
                    break;
                case COB_SCREEN_BLUE:
                    bgcolor = COLOR_BLUE;
                    break;
                case COB_SCREEN_GREEN:
                    bgcolor = COLOR_GREEN;
                    break;
                case COB_SCREEN_CYAN:
                    bgcolor = COLOR_CYAN;
                    break;
                case COB_SCREEN_RED:
                    bgcolor = COLOR_RED;
                    break;
                case COB_SCREEN_MAGENTA:
                    bgcolor = COLOR_MAGENTA;
                    break;
                case COB_SCREEN_YELLOW:
                    bgcolor = COLOR_YELLOW;
                    break;
                case COB_SCREEN_WHITE:
                    bgcolor = COLOR_WHITE;
                    break;
                default:
                    break;
            }
        }
        for ( i = 0; i < (size_t)COLOR_PAIRS; i++ ) {
            pair_content ((short)i, &fgdef, &bgdef);
            if ( fgdef == fgcolor && bgdef == bgcolor ) {
                break;
            }
            if ( fgdef == 0 && bgdef == 0 ) {
                init_pair ((short)i, fgcolor, bgcolor);
                break;
            }
        }
        if ( i != (size_t)COLOR_PAIRS ) {
    #ifdef	HAVE_COLOR_SET
            color_set (COLOR_PAIR(i), (void *)0);
    #else
            attrset (COLOR_PAIR(i));
    #endif
            bkgdset (COLOR_PAIR(i));
        } else {
            /*attrset (A_NORMAL);*/
        }
    }
    if ( for_display ) {
        if ( attr & COB_SCREEN_BLANK_SCREEN ) {
            getyx (stdscr, line, column);
            clear ();
            move (line, column);
        }
        if ( attr & COB_SCREEN_BLANK_LINE ) {
            getyx (stdscr, line, column);
            move (line, 0);
            clrtoeol ();
            move (line, column);
        }
        if ( attr & COB_SCREEN_ERASE_EOL ) {
            clrtoeol ();
        }
        if ( attr & COB_SCREEN_ERASE_EOS ) {
            clrtobot ();
        }
        if ( attr & COB_SCREEN_BELL ) {
            beep ();
        }
    }
}
static bool cob_env_on(char *s){
    return (s &&  ( *s == 'Y' || *s == 'y' || *s == '1' || *s == 'O' || *s == 'o') );
}
static void COB_NOINLINE
cob_screen_init (COB_RTD)
{
    char    *s;
    int      f, b, i;

    if ( !(rtd->cob_screen_initialized) ) {

        s = getenv ("COB_SCREEN_DEBUG");
        if ( s ) {
            Debug_Screen= 1;
        }
        s = getenv ("COB_SCREEN_INPUT_FILLER");
        if ( s ) {
            FieldInputFiller= *s;
        }
        s = getenv ("COB_SCREEN_INPUT_UNDERLINED");
        if (cob_env_on(s)) {
            FieldInputAttribute |= COB_SCREEN_UNDERLINE;
            FieldInputFiller = '_';
        }
        s = getenv ("COB_SCREEN_INPUT_REVERSED");
        if ( cob_env_on(s)) {
            FieldInputAttribute |= COB_SCREEN_REVERSE;
        }
        s = getenv ("COB_SCREEN_INPUT_BOLDED");
        if ( cob_env_on(s) ) {
            FieldInputAttribute |= COB_SCREEN_HIGHLIGHT;
        }
        s = getenv ("COB_SCREEN_INPUT_INSERT_TOGGLE");
        if ( cob_env_on(s) ) {
            InsertToggleMode =1;
        }
        s = getenv ("COB_SCREEN_UPDATE_FIRST_KEY_ERASE");
        if ( cob_env_on(s) ) {
            rtd->cob_screen_update_first_key_erase =1;
        }
        s = getenv ("COB_SCREEN_DISABLE_REFORMAT");
        if ( cob_env_on(s) ) {
            rtd->cob_screen_disable_reformat = 1;
        }
        s = getenv ("COB_SCREEN_DISABLE_RIGHT_JUSTIFY");
        if ( cob_env_on(s) ) {
            rtd->cob_screen_disable_rightjustify = 1;
        }

        s = getenv ("COB_SCREEN_EXCEPTIONS");
        if ( s ) {
            if ( *s == 'Y' || *s == 'y' || *s == '1' ) {
                rtd->cob_extended_status = 1;
                rtd->cob_use_esc = 1;
                rtd->cob_screen_key_raw = 0;
                s = getenv ("COB_SCREEN_ESC");
                if ( s ) {
                    if ( *s == 'N' || *s == 'n' || *s == '0' ) {
                        rtd->cob_use_esc = 0;
                    }
                }
                s = getenv ("COB_SCREEN_RAW_KEYS");
                if ( s ) {
                    if ( *s == 'Y' || *s == 'y' || *s == '1' ) {
                        rtd->cob_screen_key_raw = 1;
                    }
                }
            }
        }
        fflush (stdout);
        if ( rtd->cob_err_file ) {
            fflush (rtd->cob_err_file);
        }
        if ( !initscr () ) {
            cob_runtime_error (rtd, "Failed to initialize curses");
            cob_stop_abend (rtd, COBRE_CURSES_FAIL);
        }
        cbreak ();
        keypad (stdscr, 1);
        nl ();
        noecho ();
        if ( has_colors () ) {
            start_color ();
            pair_content ((short)0, &(rtd->fore_color), &(rtd->back_color));
            if ( COLOR_PAIRS ) {
                (rtd->cob_has_color) = 1;
            }
            if ( COLOR_PAIRS >= 256 ) {
                i = 0;
                for ( f = rtd->fore_color; f < rtd->fore_color + 16; f++ ) {
                    for ( b = rtd->back_color; b < rtd->back_color + 16; b++ ) {
                        init_pair ((short)i, f % 16, b %16 );
                        i++;
                    }
                }
            }
        }

        attrset (A_NORMAL);
        getmaxyx (stdscr, (rtd->cob_max_y), (rtd->cob_max_x));
        (rtd->cob_screen_initialized) = 1;
    }
}

void
cob_screen_terminate (COB_RTD)
{
    if ( (rtd->cob_screen_initialized) ) {
        (rtd->cob_screen_initialized) = 0;
        endwin ();
    }
    if ( rtd->screen_edit_field.data ) {
        cob_free(rtd->screen_edit_field.data);
        rtd->screen_edit_field.data = NULL;
    }
}


static void COB_NOINLINE
cob_check_pos_status (COB_RTD, int fret)
{
    cob_field   *f;
    int         sline;
    int         scolumn;
    char        datbuf[8];

    rtd->last_crtstatus = fret;
    if ( fret ) {
        cob_set_exception (rtd, COB_EC_IMP_ACCEPT);
    }
    map_ctr_status(rtd,fret);
    if ( rtd->current_module->cursor_pos ) {
        getyx (stdscr, sline, scolumn);
        sline ++;
        scolumn++;
        f = rtd->current_module->cursor_pos;
        if ( COB_FIELD_IS_NUMERIC (f) &&
             COB_FIELD_TYPE (f) != COB_TYPE_NUMERIC_DISPLAY ) {
            sline *= 1000;
            sline += scolumn;
            cob_set_int (rtd, f, sline);
        } else {
            if ( f->size < 6 ) {
                sline *= 100;
                sline += scolumn;
                sprintf(datbuf, "%4.4d", sline);
                memcpy (f->data, datbuf, 4);
            } else {
                sline *= 1000;
                sline += scolumn;
                sprintf(datbuf, "%6.6d", sline);
                memcpy (f->data, datbuf, 6);
            }
        }
        memcpy (rtd->cursorpos_shadow, f->data, min(f->size,6));
    }
}

static int
cob_getch (COB_RTD, int attr)
{
    int x, t;
    if ( rtd->accept_timeout ) {
        t = cob_get_int (rtd, rtd->accept_timeout);
        timeout(t*1000);
    } else {
        timeout(-1);        
    }
    x = getch();
    if ( x < 255 && isalpha(x) ) {
        if ( attr &  COB_SCREEN_UPPERCASE || rtd->accept_to_uppercase ) {
            return(toupper(x));
        } else if ( attr &  COB_SCREEN_LOWERCASE ) {
            return(tolower(x));    
        }
    }
    return x;
}

static void
cob_screen_puts (COB_RTD, cob_screen *s, cob_field *f, int for_display)
{
    unsigned char   *p;
    int             size;
    int             y;
    int             x;
    int             line;
    int             column;
    int             dataend;
    int             v;

    getyx (stdscr, y, x);
    if ( !s->line ) {
        line = y;
    } else {
        line = cob_get_int (rtd, s->line) - 1;
        if ( line < 0 ) {
            line = y;
        }
    }
    if ( !s->column ) {
        if ( !s->line ) {
            column = x; 
        } else {
            column = 0;
        }
    } else {
        column = cob_get_int (rtd, s->column) - 1;
        if ( column < 0 ) {
            column = x;
        }
    }
    if ( s->attr & COB_SCREEN_LINE_PLUS ) {
        if ( rtd->cob_screen_eol ) {
            line = y + line;
        } else {
            line = y + line + 1;
        }
    } else if ( s->attr & COB_SCREEN_LINE_MINUS ) {
        if ( rtd->cob_screen_eol ) {
            line = y - ((line+1) + 1);
        } else {
            line = y - (line + 1);
        }
        if ( line < 0 )
            line = 0;
    }
    if ( s->attr & COB_SCREEN_COLUMN_PLUS ) {
        if ( x > 0 ) {
            column = (x) + column;
        }
    } else if ( s->attr & COB_SCREEN_COLUMN_MINUS ) {
        if ( (x-1) > column ) {
            column = (x - 1) - column ;
        } else {
            column = 0;
        }

    }
    rtd->cob_screen_eol =0;
    move (line, column);
    (rtd->cob_current_y) = line;
    (rtd->cob_current_x) = column;
    if ( f ) {
        int display_size = s->input_size ? cob_get_int(rtd,f) : f->size; 

        if (display_size > f->size) {
            display_size = f->size;
        }
        if ( s->attr & COB_SCREEN_INPUT ) {

            cob_screen_attr (rtd, s->foreg, s->backg, s->attr | FieldInputAttribute, for_display);
            p = cob_enterprise_RTLGetReversedString(rtd, f->data, f->size);
            for ( dataend = f->size-1; dataend >=0 ; dataend-- ) {
                if ( p[dataend] > ' ' ) {
                    break;
                }
            }

            for ( size = 0; size < display_size; size++, p++ ) {
                if ( s->attr & COB_SCREEN_SECURE ) {
                    CIT_addch (rtd, '*');
                } else if ( (*p <= ' ') && (size > dataend) ) {
                    if ( s->prompt ) {
                        CIT_addch (rtd, s->prompt);
                    } else {
                        CIT_addch (rtd, FieldInputFiller);
                    }
                } else {
                    v = *p;
                    if ( v < ' ' ) {
                        v = ' ';
                    }
                    if ( s->attr &  COB_SCREEN_UPPERCASE ) {
                        CIT_addch (rtd, toupper(v));
                    } else if ( s->attr &  COB_SCREEN_LOWERCASE ) {
                        CIT_addch (rtd, tolower(v));
                    } else {
                        CIT_addch (rtd, v);
                    }
                }
            }
        } else {
            cob_screen_attr (rtd, s->foreg, s->backg, s->attr, for_display);
            if ( for_display ) {
                CIT_addnstr (rtd, (char *)cob_enterprise_RTLGetReversedString(rtd, f->data, f->size), (int)display_size);
            }
            else{
                column = column + (int)display_size;
                move(line,column);
            }
        }
        refresh ();
    }
}

static int
cob_screen_validate_input (COB_RTD, cob_screen *s)
{
    int i, sz;

    if ( !s->field ) {
        return 1;
    }
    sz = 0;
    for ( i = 0 ; i < s->field->size; i++ ) {
        if ( (s->field->data[i] != ' ') && (s->field->data[i] != 0) ) {
            sz ++;
        }
    }
    if ( s->attr & COB_SCREEN_FULL ) {
        if ( (sz != 0) && (sz != s->field->size) ) {
            return 0;
        }
    }
    if ( s->attr & COB_SCREEN_REQUIRED ) {
        if ( sz == 0 ) {
            return 0;
        }
    }
    return 1;
}

    #define ADIS_UP  0x0001
    #define ADIS_DN  0x0002
    #define ADIS_LT  0x0004
    #define ADIS_RT  0x0008
    #define ADIS_TB  0x0010
    #define ADIS_BT  0x0020

static int 
check_ADIS (COB_RTD, int keyp)
{
    int fret = 0;
    if ( rtd->accept_key_as_fkey ) {
        if ( keyp == KEY_UP && rtd->accept_key_as_fkey & ADIS_UP ) {
            fret= 2003;
        } else
            if ( keyp == KEY_DOWN && rtd->accept_key_as_fkey & ADIS_DN ) {
            fret= 2004;
        } else
            if ( keyp == KEY_LEFT && rtd->accept_key_as_fkey & ADIS_LT ) {
            fret= 2013;
        } else
            if ( keyp == KEY_RIGHT && rtd->accept_key_as_fkey & ADIS_RT ) {
            fret= 2014;
        } else
            if ( keyp == 011 && rtd->accept_key_as_fkey & ADIS_TB ) {
            fret= 011;
        }
        if ( keyp == KEY_BTAB && rtd->accept_key_as_fkey & ADIS_BT ) {
            fret= 2012;
        }
    }
    if ( keyp == ERR && rtd->accept_timeout ) {
        fret = 9001;
    }
    return fret;
}

static void
cob_screen_get_all (COB_RTD, int MainAttr)
{
    struct cob_inp_struct   *sptr;
    struct cob_inp_struct   *snextptr;
    struct cob_field        *f;
    cob_screen              *s;
    int                     keyp;
    int                     sline;
    int                     scolumn;
    int                     cindex;
    int                     rightpos;
    int                     ungetched;
    int                     startidx;
    int                     fret;

    sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp));
    s = sptr->scr;
    sline = sptr->this_y;
    scolumn = sptr->this_x;
    rightpos = scolumn + s->field->size - 1;
    cindex = 0;
    move (sline, scolumn);
    if ( cob_check_move_pos(rtd,&sline,&scolumn, 0) ) {
        /* find field best matching field for sline scolumn*/
        startidx = rtd->curr_index;
        while ( (sptr->down_index != startidx) &&
                (sptr->this_y < sline) ) {
            rtd->curr_index = sptr->down_index;
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index);
            s = sptr->scr;
            rightpos = sptr->this_x + s->field->size - 1;
        }
        sline = sptr->this_y;
        startidx = rtd->curr_index;
        while ( (rightpos < scolumn) &&
                (rtd->curr_index < (rtd->totl_index -1)) ) {
            snextptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index +1);
            if ( sptr->this_y != snextptr->this_y ) {
                break;
            }
            rtd->curr_index += 1;
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index);
            s = sptr->scr;
            rightpos = sptr->this_x + s->field->size - 1;
        }
        if ( scolumn > rightpos ) {
            scolumn = rightpos ;
        } else if ( sptr->this_x > scolumn ) {
            scolumn = sptr->this_x;
        }
        cindex = scolumn - sptr->this_x;
    }
    ungetched = 0;
    for ( ; ; ) {
        f = s->field;
        cob_screen_check_control(rtd,s);
        keyp = local_cob_field_accept(rtd,f, s->input_size ? cob_get_int(rtd,s->input_size): 0,
                                      sptr->this_y, sptr->this_x,&cindex,
                                      s->foreg,s->backg,s->attr | MainAttr ,s->prompt,1);
        if ( (keyp == KEY_ENTER || keyp == '\n') && cob_screen_validate_input(rtd, s) ) {
            break;
        }
        if ( (keyp > KEY_F0 && keyp < KEY_F(65)) && cob_screen_validate_input(rtd, s) ) {
            (rtd->global_return) = 1000 + keyp - KEY_F0;
            break;
        }
        fret = check_ADIS(rtd,keyp);
        if ( fret != 0 ) {
            (rtd->global_return)=fret;
            break;
        }
        if ( (rtd->cob_extended_status) ) {
            if ( keyp == KEY_PPAGE && cob_screen_validate_input(rtd, s) ) {
                (rtd->global_return) = 2001;
                break;
            }
            if ( keyp == KEY_NPAGE && cob_screen_validate_input(rtd, s) ) {
                (rtd->global_return) = 2002;
                break;
            }
            if ( keyp == KEY_PRINT && cob_screen_validate_input(rtd, s) ) {
                (rtd->global_return) = 2006;
                break;
            }
            if ( (rtd->cob_use_esc) ) {
                if ( keyp == 033 ) {
                    (rtd->global_return) = 2005;
                    break;
                }
            }
        }
        if ( (keyp == KEY_NEXT || keyp == 011)  && cob_screen_validate_input(rtd, s) ) {
            if ( (rtd->curr_index) < (rtd->totl_index) - 1 ) {
                (rtd->curr_index)++;
            } else {
                (rtd->curr_index) = 0;
            }
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index);
            s = sptr->scr;
            cindex = 0;
            continue;
        }
        if ( (keyp == KEY_PREVIOUS || keyp == KEY_BTAB) && cob_screen_validate_input(rtd, s) ) {
            if ( (rtd->curr_index) > 0 ) {
                (rtd->curr_index)--;
            } else {
                (rtd->curr_index) = (rtd->totl_index) - 1;
            }
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index);
            s = sptr->scr;
            if ( ungetched ) {
                ungetched = 0;
                cindex = s->field->size;
            } else {
                cindex = 0;
            }
            continue;
        }

        if ( keyp == KEY_UP && cob_screen_validate_input(rtd, s) ) {
            (rtd->curr_index) = sptr->up_index;
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index);
            s = sptr->scr;
            cindex = 0;
            (rtd->global_return) = 2003;
            continue;
        }
        if ( keyp == KEY_DOWN && cob_screen_validate_input(rtd, s) ) {
            (rtd->curr_index) = sptr->down_index;
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index);
            s = sptr->scr;
            cindex = 0;
            (rtd->global_return) = 2004;
            continue;
        }
        if ( keyp == KEY_HOME && cob_screen_validate_input(rtd, s) ) {
            (rtd->curr_index) = 0;
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp));
            s = sptr->scr;
            cindex = 0;
            if(rtd->cob_screen_key_raw) {
                (rtd->global_return) = 2007;
            }
            continue;
        }
        if ( keyp == KEY_END && cob_screen_validate_input(rtd, s) ) {

            (rtd->curr_index) = (rtd->totl_index) - 1;
            sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->curr_index);
            s = sptr->scr;

            if(rtd->cob_screen_key_raw) {
                (rtd->global_return) = 2008;
            }
            continue;
        }
        if ( keyp == KEY_IC ) {
            if ( rtd->cob_screen_key_raw ) {
                (rtd->global_return) = 2009;
             }
            continue;
        }
        if ( keyp == KEY_DC ) {
            if ( rtd->cob_screen_key_raw ) {
                (rtd->global_return) = 2010;
             }
            continue;
        }
        if ( keyp == KEY_EOL ) {
            if ( rtd->cob_screen_key_raw ) {
                (rtd->global_return) = 2011;
             }
            continue;
        }
        if ( keyp == KEY_BACKSPACE || keyp == ('H' & 037) ) {
            if ( (s->attr & COB_SCREEN_AUTO) && cob_screen_validate_input(rtd, s) ) {

                ungetched = 1;
                ungetch (KEY_END);
                ungetch (KEY_BACKSPACE);
                ungetch (KEY_PREVIOUS); 

            } else {
                if ( !(s->attr &  COB_SCREEN_NO_BELL) ) {
                    beep();
                }
            }
            continue;
        }
        if ( keyp == KEY_LEFT ) {
            if ( cob_screen_validate_input(rtd, s) ) {
                ungetched = 1;
                ungetch (KEY_PREVIOUS);
            } else {
                if ( !(s->attr &  COB_SCREEN_NO_BELL) ) {
                    beep();
                }
            }
            continue;
        }
        if ( keyp == KEY_RIGHT ) {
            if ( cob_screen_validate_input(rtd, s) ) {
                ungetch (KEY_NEXT);
            } else {
                if ( !(s->attr &  COB_SCREEN_NO_BELL) ) {
                    beep();
                }
            }
            continue;
        }
        if ( (cindex == (f->size-1)) && ((s->attr & COB_SCREEN_AUTO) || (MainAttr & COB_SCREEN_AUTO)) ) {
            if ( (rtd->curr_index) <= (rtd->totl_index - 1) ) {
                if ( cob_screen_validate_input(rtd, s) ) {
                    ungetch (KEY_NEXT);
                    continue;
                }
            }
            if ( !(s->attr &  COB_SCREEN_NO_BELL) ) {
                beep();
            }
            continue;
        }
        if ( !(s->attr &  COB_SCREEN_NO_BELL) ) {
            beep();
        }
    }
    refresh ();
}

static int
compare_yx (COB_RTD, const void *m1, const void *m2)
{
    const struct cob_inp_struct *s1;
    const struct cob_inp_struct *s2;

    s1 = m1;
    s2 = m2;
    if ( s1->this_y < s2->this_y ) {
        return -1;
    }
    if ( s1->this_y > s2->this_y ) {
        return 1;
    }
    if ( s1->this_x < s2->this_x ) {
        return -1;
    }
    if ( s1->this_x > s2->this_x ) {
        return 1;
    }
    return 0;
}

static void
cob_prep_input (COB_RTD, cob_screen *s)
{
    struct cob_inp_struct   *sptr;

    switch ( s->type ) {
        case COB_SCREEN_TYPE_GROUP:
            cob_screen_puts (rtd, s, NULL, 0);
            for ( s = s->child; s; s = s->next ) {
                cob_prep_input (rtd, s);
            }
            break;
        case COB_SCREEN_TYPE_FIELD:
            cob_screen_puts (rtd, s, s->field, 0);
            if ( s->attr & COB_SCREEN_INPUT ) {
                sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + (rtd->totl_index);
                sptr->scr = s;
                sptr->this_y = (rtd->cob_current_y);
                sptr->this_x = (rtd->cob_current_x);
                (rtd->totl_index)++;
            }
            break;
        case COB_SCREEN_TYPE_VALUE:
            cob_screen_puts (rtd, s, s->value, 0);
            /*
            if ( s->occurs ) {
                for ( n = 1; n < s->occurs; ++n ) {
                    cob_screen_puts (rtd, s, s->value);
                }
            }*/
            break;
        case COB_SCREEN_TYPE_ATTRIBUTE:
            cob_screen_puts (rtd, s, NULL, 0);
            cob_screen_attr (rtd, s->foreg, s->backg, s->attr, 0);
            break;
    }
}

static int 
cob_screen_set_control_attr (int src, int is_no, int att)
{
    if ( is_no ) {
        src &= ~att;
    } else {
        src |= att;
    }
    return src;
}

static void 
cob_screen_check_control (COB_RTD, cob_screen *s)
{
    char *p, *t;
    char *save=NULL;
    int  is_no=0;
    int  attr = 0;

    s->backg = -1;
    s->foreg = -1;
    if ( s->foreg_field ) {
        s->foreg = cob_get_int(rtd,s->foreg_field);
    }
    if ( s->backg_field ) {
        s->backg = cob_get_int(rtd,s->backg_field);
    }
    if ( s->control ) {
        attr = s->attr_base;
        p = cob_malloc(rtd,s->control->size+1);
        memcpy (p, s->control->data, s->control->size);
        t = COB_STRTOK(p, " ,", &save);
        while ( t ) {
            if ( STRCASEEQ(t, "NO") ) {
                is_no = 1;
            } else {
                if ( STRCASEEQ(t, "BELL") || STRCASEEQ(t, "BEEP") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_BELL);
                } else if ( STRCASEEQ(t, "BLINK") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_BLINK);
                } else if ( STRCASEEQ(t, "HIGHLIGHT") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_HIGHLIGHT);
                } else if ( STRCASEEQ(t, "LOWLIGHT") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_LOWLIGHT);
                } else if ( STRCASEEQ(t, "REVERSE-VIDEO") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_REVERSE);
                } else if ( STRCASEEQ(t, "UNDERLINE") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_UNDERLINE);
                } else if ( STRCASEEQ(t, "AUTO") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_AUTO);
                } else if ( STRCASEEQ(t, "SECURE") || STRCASEEQ(t, "NO-ECHO") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_SECURE);
                } else if ( STRCASEEQ(t, "FULL") || STRCASEEQ(t, "LENGTH-CHECK") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_FULL);
                } else if ( STRCASEEQ(t, "REQUIRED") || STRCASEEQ(t, "EMPTY-CHECK") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_REQUIRED);
                } else if ( STRCASEEQ(t, "PROMPT") ) {
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_PROMPT);
                } else if ( STRCASEEQ(t, "PROTECT") ) {
                    is_no = is_no ? 0 : 1; 
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_INPUT);
                } else if ( STRCASEEQ(t, "ZERO-FILL") ) {
                    is_no = is_no ? 0 : 1; 
                    attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_UPDATE);
                } else if ( STRCASEEQ(t, "BLANK") ) {
                    t = COB_STRTOK(NULL, " ,", &save);
                    if ( t ) {
                        if ( STRCASEEQ(t, "LINE") ) {
                            attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_BLANK_LINE);
                        } else if ( STRCASEEQ(t, "SCREEN") ) {
                            attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_BLANK_SCREEN);
                        } else {
                            attr = cob_screen_set_control_attr(attr, is_no, COB_SCREEN_BLANK_LINE);
                            continue;
                        }
                    }
                } else if ( STRCASEEQ(t, "FOREGROUND-COLOR") || 
                            STRCASEEQ(t, "FOREGROUND-COLOUR") ) {
                    t = COB_STRTOK(NULL, " ,", &save);
                    if ( t ) {
                        if ( STRCASEEQ(t, "IS") ) {
                            t = COB_STRTOK(NULL, " ,", &save);
                        }
                        if ( t ) {
                            s->foreg = atoi(t);
                        }
                    }
                } else if ( STRCASEEQ(t, "BACKGROUND-COLOR") || 
                            STRCASEEQ(t, "BACKGROUND-COLOUR") ) {
                    t = COB_STRTOK(NULL, " ,", &save);
                    if ( t ) {
                        if ( STRCASEEQ(t, "IS") ) {
                            t = COB_STRTOK(NULL, " ,", &save);
                        }
                        if ( t ) {
                            s->backg = atoi(t);
                        }
                    }
                }
                is_no = 0;
            }
            t = COB_STRTOK(NULL, " ,", &save);
        }
        s->attr = attr;
        cob_free(p);
    }
}

/* Global functions */

void
cob_screen_display (COB_RTD, cob_screen *s, cob_field *line, cob_field *column)
{

    if ( !(rtd->cob_screen_initialized) ) {
        cob_screen_init (rtd);
    }
    cob_screen_check_control(rtd,s);
    if ( s->attr & COB_SCREEN_BLANK_SCREEN ) {
        int line, column;
        getyx (stdscr, line, column);
        clear ();
        move (line, column);
    }
    switch ( s->type ) {
        case COB_SCREEN_TYPE_GROUP:
            cob_screen_puts (rtd, s, NULL, 1);
            for ( s = s->child; s; s = s->next ) {
                cob_screen_display (rtd, s, line, column);
            }
            break;
        case COB_SCREEN_TYPE_FIELD:
            cob_screen_puts (rtd, s, s->field, 1);
            break;
        case COB_SCREEN_TYPE_VALUE:
            cob_screen_puts (rtd, s, s->value, 1);
            break;
        case COB_SCREEN_TYPE_ATTRIBUTE:
            cob_screen_puts (rtd, s, NULL, 1);
            cob_screen_attr (rtd, s->foreg, s->backg, s->attr, 1);
            break;
    }
    refresh ();
}

void 
cob_screen_accept_escape (COB_RTD, cob_field *f)
{
    int  crtstatus;
    int  escstat = 0;
    char datbuf[10];
    if ( rtd->current_module->crt_status ) {
        crtstatus= cob_get_int(rtd,rtd->current_module->crt_status);
    } else {
        crtstatus= rtd->last_crtstatus;
    }
    if ( (crtstatus > 1000) && (crtstatus < 2000) ) {
        escstat = (crtstatus - 1000) + 1; /* 2= F1, 3= F2 ...*/ 
    } else if ( crtstatus == 2005 ) {
        escstat = 1;
    }

    if ( COB_FIELD_IS_NUMERIC (f) ) {
        cob_set_int (rtd, f, escstat);
    } else if ( f->size >= 4 ) {
        sprintf(datbuf, "%4.4d", escstat);
        memcpy (f->data, datbuf, 4);
    }
}

void
cob_screen_accept (COB_RTD, cob_screen *s, cob_field *line, cob_field *column)
{
    cob_screen_accept_1(rtd, s, line, column, 0);
}

void
cob_screen_accept_1 (COB_RTD, cob_screen *s, cob_field *line, cob_field *column, int mainattr)
{
    struct cob_inp_struct   *sptr;
    struct cob_inp_struct   *sptr2;
    size_t          idx;
    size_t          n;
    size_t          posu;
    size_t          posd;
    size_t          prevy;
    size_t          firsty;
    int             starty;

    if ( !(rtd->cob_screen_initialized) ) {
        cob_screen_init (rtd);
    }
    cob_screen_check_control(rtd,s);

    if ( !((struct cob_inp_struct*)(rtd->cob_base_inp)) ) {
        rtd->cob_base_inp= cob_malloc (rtd, COB_INP_SIZE);
    } else {
        memset (((struct cob_inp_struct*)(rtd->cob_base_inp)), 0, COB_INP_SIZE);
    }
    rtd->cob_exception_code = 0;
    (rtd->cob_current_y) = 0;
    (rtd->cob_current_x) = 0;
    (rtd->totl_index) = 0;
    move (0, 0);
    cob_prep_input (rtd, s);
    /* No input fields is an error */
    if ( !(rtd->totl_index) ) {
        cob_check_pos_status (rtd, 8000);
        rtd->accept_timeout = NULL;
        return;
    }
    cob_qsort (((struct cob_inp_struct*)(rtd->cob_base_inp)), (rtd->totl_index), sizeof(struct cob_inp_struct), compare_yx, rtd);
    sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp));
    starty = sptr->this_y;
    posu = 0;
    posd = 0;
    prevy = 0;
    firsty = 0;
    /* Set up array for Cursor UP/DOWN */
    for ( n = 0; n < (rtd->totl_index); n++ ) {
        sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + n;
        if ( sptr->this_y > starty ) {
            if ( !firsty ) {
                firsty = n;
            }
            starty = sptr->this_y;
            sptr2 = ((struct cob_inp_struct*)(rtd->cob_base_inp)) + posd;
            for ( idx = posd; idx < n; idx++, sptr2++ ) {
                sptr2->down_index = n;
            }
            posu = prevy;
            prevy = n;
            posd = n;
        }
        sptr->up_index = posu;
    }
    sptr = ((struct cob_inp_struct*)(rtd->cob_base_inp));
    for ( n = 0; n < firsty; n++, sptr++ ) {
        sptr->up_index = posd;
    }
    (rtd->curr_index) = 0;
    (rtd->global_return) = 0;
    cob_screen_get_all (rtd, mainattr);
    cob_check_pos_status (rtd, (rtd->global_return));
    rtd->accept_timeout = NULL;
}

void
cob_field_display (COB_RTD, cob_field *f, cob_field *line, cob_field *column,
                   cob_field *fgc, cob_field *bgc, cob_field *scroll,
                   const int cst_attr)
{
    int sline;
    int scolumn;
    int fc = -1;
    int bc = -1;
    int attr = cst_attr;

    if ( !(rtd->cob_screen_initialized) ) {
        cob_screen_init (rtd);
    }

    if (f && COB_FIELD_IS_NATIONAL(f) ) {
        f = cob_intr_display_of(rtd,1,f);
    }

    if ( scroll ) {
        sline = cob_get_int (rtd, scroll);
        if ( attr & COB_SCREEN_SCROLL_DOWN ) {
            sline = -sline;
        }
        scrollok (stdscr, 1);
        scrl (sline);
        scrollok (stdscr, 0);
        refresh ();
    }
    get_line_column (rtd, line, column, &sline, &scolumn);
    move (sline, scolumn);
    if ( f == &cob_space ) {
        attr |= COB_SCREEN_ERASE_EOS;
    }
    if ( fgc ) {
        fc = cob_get_int(rtd,fgc);
    }
    if ( bgc ) {
        bc = cob_get_int(rtd,bgc);
    }
    cob_screen_attr (rtd, fc, bc, attr, 1);
    if (f && f != &cob_space ) {
        addnstr ((char *)cob_enterprise_RTLGetReversedString(rtd, f->data, (int)f->size), (int)f->size);
    }
    /*move (sline, scolumn+(int)f->size);*/

    refresh ();
}

static void 
initdsp_accept_field (COB_RTD, cob_field *f,const int attr, const char prompt)
{
    unsigned char   *p;
    int             fret;
    size_t          count;

    p = f->data;
    for ( count = 0; count < f->size; count++ ) {
        if ( attr & COB_SCREEN_SECURE ) {
            CIT_addch (rtd, '*');
        } else if ( attr & COB_SCREEN_UPDATE ) {
            if ( *p < ' ' ) {
                fret = ' ';
            } else {
                fret = *p;
            }
            CIT_addch (rtd, (unsigned int)fret);
            p++;
        } else if ( attr & COB_SCREEN_PROMPT ) {
            if ( prompt ) {
                CIT_addch (rtd, prompt);
            } else {
                CIT_addch (rtd, FieldInputFiller);
            }
        } else {
            CIT_addch (rtd, ' ');
        }
    }
}

static void 
refresh_accept_field (COB_RTD, cob_field *f,const int attr, const char prompt)
{
    unsigned char   *p;
    int             fret;
    int             count;
    int             dataend=-1;

    p = f->data;
    for ( dataend = f->size-1; dataend >=0 ; dataend-- ) {
        if ( p[dataend] > ' ' ) {
            break;
        }
    }
    for ( count = 0 ;count <= dataend; count++ ) {
        if ( attr & COB_SCREEN_SECURE ) {
            CIT_addch (rtd, '*');
        } else {
            fret = *p++;
            if ( fret < ' ' ) {
                fret = ' ';
            }
            CIT_addch (rtd, (unsigned int)fret);
        }
    }
    for ( ;count < f->size; count++ ) {
        if ( attr & COB_SCREEN_SECURE ) {
            CIT_addch (rtd, '*');
        } else if ( attr & COB_SCREEN_UPDATE ) {
            fret = *p++;
            if ( fret < ' ' ) {
                fret = ' ';
            }
            CIT_addch (rtd, (unsigned int)fret);
        } else if ( attr & COB_SCREEN_PROMPT ) {
            if ( prompt ) {
                CIT_addch (rtd, prompt);
            } else {
                CIT_addch (rtd, FieldInputFiller);
            }
        } else {
            CIT_addch (rtd, ' ');
        }
    }
}


static cob_field * 
cob_screen_format (COB_RTD, cob_field *f, int foredit, int input_size)
{
    cob_field_attr  attr;
    unsigned char   *p = NULL;
    int size;

    if ( rtd->screen_edit_field.data ) {
        cob_free(rtd->screen_edit_field.data)
    }
    if ( COB_FIELD_IS_NUMERIC(f) ) {
        size = f->attr->digits;
        if ( COB_FIELD_HAVE_SIGN(f) ) {
            size ++;
        }
        COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, size, 0,
                       0, NULL);
        rtd->screen_edit_attr =  attr;
        p = cob_malloc(rtd,rtd->screen_edit_attr.digits+5);
        rtd->screen_edit_field.data = p;
        rtd->screen_edit_field.size = rtd->screen_edit_attr.digits;
        rtd->screen_edit_field.attr = &rtd->screen_edit_attr;
    } else {
        if ( COB_FIELD_IS_NATIONAL (f) ) {
            COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, f->size / 2 , 0,
                           0, NULL);

        } else {
            COB_ATTR_INIT (COB_TYPE_ALPHANUMERIC, f->size, 0,
                           0, NULL);
        }
        rtd->screen_edit_attr =  attr;
        p = cob_malloc(rtd,f->size+5);
        rtd->screen_edit_field.data = p;
        rtd->screen_edit_field.size = f->size;
        rtd->screen_edit_field.attr = &rtd->screen_edit_attr;
    }
    if (input_size > 0) {
        rtd->screen_edit_field.size = rtd->screen_edit_field.size > input_size ? input_size : rtd->screen_edit_field.size;
    }
    cob_move(rtd, f, &rtd->screen_edit_field);
    if ( foredit && COB_FIELD_IS_NUMERIC_OR_EDITED(f) && !rtd->cob_screen_disable_rightjustify ) {
        unsigned char*begin = p;
        int i=0;
        for ( ;*begin==CHAR_SP && begin < p+rtd->screen_edit_field.size; begin++ ) {
        }
        if ( begin > p ) {
            while ( begin < p+rtd->screen_edit_field.size ) {
                p[i] = *begin;
                begin++;
                i++;
            }
            if ( i ) {
                while ( i < rtd->screen_edit_field.size ) {
                    p[i] = CHAR_SP;
                    i++;
                }
            }
        }

    }
    return &rtd->screen_edit_field;
}

static unsigned char *local_strnchr(unsigned  char *str, size_t len, int character) {
    unsigned char *end = str + len;
    char c = (char)character;
    do {
        if ( *str == c ) {
            return str;
        }
    } while ( ++str < end );
    return NULL;
}


static int
local_cob_field_accept (COB_RTD, cob_field *finput, int input_size,
                        int sline, int scolumn,
                        int *pcindex,
                        int fgc,  int bgc, 
                        const int attr, const char prompt, int forscreen)
{
    unsigned char   *p;
    unsigned char   *data_end;
    int             keyp=0;
    int             fret;
    int             ateof;
    int             gotbacksp;
    int             insert_one = 0;
    int             firstkey;
    int             next_key = 0;
    int             exit_loop = 0;
    int             next_key_cnt=0;
    int             mline, mcol;
    int size;
    cob_field       * f;

    fret = 0;
    ateof = 0;
    gotbacksp = 0;
    firstkey = 1;
    getmaxyx(stdscr, mline, mcol);
    if ( !rtd->cob_screen_disable_reformat ) {
        f = cob_screen_format(rtd,finput, 1, input_size);
    } else {
        f = finput;
    }
    for ( ; ; ) {

        if ( (*pcindex) < 0 ) {
            (*pcindex) = 0;
        }
        if ( (*pcindex) >= f->size ) {
            (*pcindex) = f->size -1;
        }
        insert_one = 0;
        p = f->data + ((*pcindex));
        move (sline, scolumn);
        cob_screen_attr (rtd, fgc, bgc, attr | FieldInputAttribute, 0);
        refresh_accept_field(rtd,f,attr,prompt);
        move (sline + ((*pcindex) / mcol) , scolumn + ((*pcindex) % mcol));
        refresh ();
        if ( exit_loop ) {
            break;
        }
        if ( next_key ) {
            keyp = next_key;
            next_key_cnt--;
            if ( next_key_cnt <= 0 ) {
                next_key=0;
            }
        } else {
            keyp = cob_getch (rtd, attr);
        }
        if ( keyp == KEY_ENTER || keyp == '\n' ) {
            break;
        }
        if ( keyp > KEY_F0 && keyp < KEY_F(65) ) {
            fret = 1000 + keyp - KEY_F0;
            break;
        }
        fret = check_ADIS(rtd,keyp);
        if ( fret != 0 ) {
            break;
        }
        if ( (rtd->cob_extended_status) || forscreen ) {
            if ( keyp == KEY_PPAGE ) {
                fret = 2001;
                break;
            }
            if ( keyp == KEY_NPAGE ) {
                fret = 2002;
                break;
            }
            if ( keyp == KEY_UP ) {
                fret = 2003;
                break;
            }
            if ( keyp == KEY_DOWN ) {
                fret = 2004;
                break;
            }
            if ( keyp == KEY_PRINT ) {
                fret = 2006;
                break;
            }
            if ( (rtd->cob_use_esc) ) {
                if ( keyp == 033 ) {
                    fret = 2005;
                    break;
                }
            }
            if ( rtd->cob_screen_key_raw ) {
                if ( keyp == KEY_HOME ) {
                    fret = 2007;
                    break;
                }
                if ( keyp == KEY_END ) {
                    fret = 2008;
                    break;
                }
                if ( keyp == KEY_IC ) {
                    fret = 2009;
                    break;
                }
                if ( keyp == KEY_DC ) {
                    fret = 2010;
                    break;
                }
                if ( keyp == KEY_EOL ) {
                    fret = 2011;
                    break;
                }
            }

        
            if ( (keyp == 0x09 || keyp == KEY_BTAB || keyp == KEY_PREVIOUS || keyp == KEY_NEXT) /* && forscreen*/ ) {
                if ( !forscreen ) {
                    if ( keyp == 0x09 || keyp == KEY_NEXT ) {
                        fret = 0x09;
                    } else {
                        fret = 2012;
                    }
                }
                break;
            }
        }
        p = f->data + ((*pcindex));
        if ( keyp == KEY_DC || keyp == KEY_EOL ) {
            firstkey = 0;
            if ( (*pcindex) >=0 ) {
                ateof = 0;
                *p = ' ';
                if ( (*pcindex) < f->size ) {
                    if ( keyp == KEY_EOL ) {
                        memset (p, ' ', f->size - (*pcindex) );
                    } else {
                        memmove (p, p+1, f->size - (*pcindex) -1);
                        *(f->data+(f->size-1)) = ' ';
                    }
                }
            }
            continue;
        }
        if ( keyp == KEY_IC ) {
            firstkey = 0;
            if ( InsertToggleMode ) {
                rtd->screen_insert_mode=~rtd->screen_insert_mode;
            } else {
                if ( (*pcindex) >= 0 ) {
                    if ( (*pcindex) < f->size ) {
                        if ( f->data[f->size-1] > ' ' ) {
                            if ( !(attr &  COB_SCREEN_NO_BELL) ) {
                                beep();
                            }
                            continue;
                        }
                        memmove (p+1, p, f->size - (*pcindex) -1);
                        *p = ' ';
                    }
                }
            }
            continue;
        }
        if ( keyp == KEY_BACKSPACE || keyp == ('H' & 037) ) {
            firstkey = 0;
            if ( (*pcindex) > 0 ) {
                memmove (p-1, p, f->size - (*pcindex) );
                *(f->data+(f->size-1)) = ' ';
                (*pcindex)--;
                ateof = 0;
                gotbacksp = 1;
                continue;
            } else {
                if ( forscreen ) break;
            }
        }
        if ( keyp == KEY_HOME ) {
            firstkey = 0;
            ateof = 0;
            if ( (*pcindex) > 0 ) {
                gotbacksp = 0;
                (*pcindex) = 0;
                continue;
            } else {
                if ( forscreen ) break;
            }
            firstkey = 0;
        }
        if ( keyp == KEY_END ) {
            firstkey = 0;
            if ( (*pcindex) < f->size -1 ) {
                (*pcindex) = f->size - 1;
                ateof = 0;
                gotbacksp = 0;
                firstkey = 0;
                continue;
            } else {
                if ( forscreen ) break;
            }
        }
        if ( keyp == KEY_LEFT ) {
            firstkey = 0;
            gotbacksp = 0;
            ateof = 0;
            if ( (*pcindex) > 0 ) {
                (*pcindex)--;
                continue;
            } else {
                if ( forscreen ) break;
            }
        }
        if ( keyp == KEY_RIGHT ) {
            firstkey = 0;
            gotbacksp = 0;
            if ( (*pcindex) < f->size -1 ) {
                (*pcindex)++;
                continue;
            } else {
                if ( forscreen ) break;
            }
        }
        if ( keyp > 037 && keyp < (int)A_CHARTEXT ) {
            if ( firstkey && ((*pcindex) == 0 ) && rtd->cob_screen_update_first_key_erase ) {
                memset (f->data, ' ', f->size);
            }
            firstkey = 0;
            if ( COB_FIELD_IS_NUMERIC_OR_EDITED (finput) ) {
                if ( rtd->current_module &&  (keyp == rtd->current_module->decimal_point ) ) {
                    unsigned char *pp= local_strnchr(f->data, f->size , keyp);
                    if ( pp && pp > p ) {
                        next_key = KEY_DC;
                        next_key_cnt = pp -p;
                    } else if ( pp == NULL) {
                        next_key = KEY_EOL;
                        next_key_cnt = 1;
                    } else if ( pp < p) {
                        beep();
                        continue;
                    }
                } else if ( (keyp < '0' || keyp > '9') && 
                            keyp != '+' && keyp != '-' ) {
                    if ( !(attr &  COB_SCREEN_NO_BELL) ) {
                        beep();
                    }
                    continue;
                } else if ( rtd->current_module &&  (*p == rtd->current_module->decimal_point ) ) {
                    if (f->data[f->size-1] == 0 || f->data[f->size-1] == ' ') {
                        insert_one =1;
                    } else {
                        next_key = keyp;
                        next_key_cnt = 1;
                        keyp = rtd->current_module->decimal_point;
                    }
                }
            }
            gotbacksp = 0;
            if ( (*pcindex) < f->size ) {
                if ( ateof ) {
                    if ( !(attr &  COB_SCREEN_NO_BELL) ) {
                        beep();
                    }
                } else {
                    if ( rtd->screen_insert_mode || insert_one ) {
                        if ( f->data[f->size-1] > ' ' ) {
                            if ( !(attr &  COB_SCREEN_NO_BELL) ) {
                                beep();
                            }
                            continue;
                        }
                        memmove (p+1, p, f->size - (*pcindex) -1);
                    }
                    *p = keyp;
                    (*pcindex)++;
                    if ( ((*pcindex) == (f->size)) && (attr & COB_SCREEN_AUTO) ) {
                        exit_loop =1;
                        continue;
                    }
                    if ( (*pcindex) >= f->size ) ateof = 1;
                }
            }
            continue;
        }
        gotbacksp = 0;
        if ( !(attr &  COB_SCREEN_NO_BELL) ) {
            beep();
        }
    }
    if ( !firstkey && (f != finput) ) {
        if ( COB_FIELD_JUSTIFIED (finput) ) {

            data_end = f->data + f->size - 1;
            p = data_end;
            while( *p == CHAR_SP && p >= f->data) {
                p--;
            }
            if(p >= f->data && p < data_end) {
                size =  p - f->data + 1;
                COB_MEMCPY(data_end - size + 1, f->data, size );
                memset(f->data, CHAR_SP, f->size - size);
            }
        }
        cob_move(rtd,f, finput);
    }
    if ( !forscreen ) {
        cob_check_pos_status (rtd, fret);
        refresh ();
        return(fret);
    } else if ( !rtd->cob_screen_disable_reformat ) {
        f = cob_screen_format(rtd,finput, 0, 0);
        move (sline, scolumn);
        cob_screen_attr (rtd, fgc, bgc, attr | FieldInputAttribute, 0);
        refresh_accept_field(rtd,f,attr,prompt);
        move (sline + ((*pcindex) / mcol) , scolumn + ((*pcindex) % mcol));
        refresh ();
    }
    return keyp;
}

void 
cob_accept_set_timeout (COB_RTD, cob_field *timeout)
{
    rtd->accept_timeout = timeout;
}

void
cob_field_accept_with_status (COB_RTD, cob_field *f, cob_field *line, cob_field *column,
                              cob_field *fgc, cob_field *bgc, cob_field *scroll,
                              const int attr, const char prompt, cob_field *status)
{
    int             keyp;
    int             sline;
    int             scolumn;
    int             ccol=0;
    int             fret;
    int             fc = -1;
    int             bc = -1;

    if ( !(rtd->cob_screen_initialized) ) {
        cob_screen_init (rtd);
    }

    if ( scroll ) {
        keyp = cob_get_int (rtd, scroll);
        if ( attr & COB_SCREEN_SCROLL_DOWN ) {
            keyp = -keyp;
        }
        scrollok (stdscr, 1);
        scrl (keyp);
        scrollok (stdscr, 0);
        refresh ();
    }
    rtd->cob_exception_code = 0;
    get_line_column (rtd, line, column, &sline, &scolumn);
    if ( !(attr & COB_SCREEN_UPDATE) ) {
        if ( COB_FIELD_IS_NUMERIC (f) ) {
            cob_move (rtd, &cob_zero, f);
        } else {
            memset (f->data, ' ', f->size);
        }
    }
    move (sline, scolumn);
    if ( fgc ) {
        fc = cob_get_int(rtd,fgc);
    }
    if ( bgc ) {
        bc = cob_get_int(rtd,bgc);
    }
    cob_screen_attr (rtd, fc, bc, attr | FieldInputAttribute, 0);
    initdsp_accept_field(rtd,f,attr,prompt);
    fret = local_cob_field_accept(rtd,f,0 , sline,scolumn, &ccol, fc,bc,attr,prompt, 0);
    fret = (int)map_ctr_status(rtd,fret);
    if ( status ) {
        cob_set_int(rtd,status,fret);
    }
    if ( fret ) {
        cob_set_exception (rtd, COB_EC_IMP_ACCEPT);
    }
    rtd->accept_timeout = NULL;
}

void
cob_field_accept (COB_RTD, cob_field *f, cob_field *line, cob_field *column,
                  cob_field *fgc, cob_field *bgc, cob_field *scroll,
                  const int attr, const char prompt)
{
    cob_field_accept_with_status(rtd,f,line,column, fgc, bgc, scroll, attr, prompt, NULL);
}

void
cob_screen_line_col (COB_RTD, cob_field *f, const int l_or_c)
{
    if ( !(rtd->cob_screen_initialized) ) {
        cob_screen_init (rtd);
    }
    if ( !l_or_c ) {
        cob_set_int (rtd, f, (int)LINES);
    } else {
        cob_set_int (rtd, f, (int)COLS);
    }
}

void
cob_screen_set_mode (COB_RTD, const size_t smode)
{
    if ( !smode ) {
        refresh ();
        def_prog_mode ();
        endwin ();
    } else {
        reset_prog_mode ();
        refresh ();
    }
}

void
cob_screen_newline (COB_RTD)
{
    int sline;
    int scolumn;

    if ( !(rtd->cob_screen_initialized) ) {
        cob_screen_init (rtd);
    }

    get_line_column (rtd, NULL, NULL, &sline, &scolumn);
    sline++;
    scolumn = 0;
    move (sline, scolumn);

}

void
cob_screen_append (COB_RTD, char * str, int size)
{
    if ( !(rtd->cob_screen_initialized) ) {
        cob_screen_init (rtd);
    }
    addnstr ((char *)cob_enterprise_RTLGetReversedString(rtd, (unsigned char*)str, size), size);
}

typedef struct xAF_Param {
    unsigned char bit_pair_setting;
    unsigned char bit_map_section;
    unsigned char bit_pair_number;
    unsigned char filler;
}xAF_Param;

int
rtd_CBL_XAF (COB_RTD, unsigned char *code, unsigned char *param) {
    xAF_Param *p;

    COB_CHK_PARMS (CBL_XAF, 2);
    p = (xAF_Param *)param;
    if ( code && param && *code == 1 ) {
        switch ( p->bit_pair_number ) {
            case 85:
                rtd->accept_to_uppercase =p->bit_pair_setting;
                break;
            case 5:
                if ( p->bit_pair_setting == 1 ) {
                    rtd->accept_key_as_fkey |= ADIS_UP | ADIS_DN;
                } else {
                    rtd->accept_key_as_fkey &= ~(ADIS_UP | ADIS_DN);
                }
                break;
            case 3:
                if ( p->bit_pair_setting == 1 ) {
                    rtd->accept_key_as_fkey |= ADIS_UP | ADIS_DN | ADIS_LT | ADIS_RT;
                } else {
                    rtd->accept_key_as_fkey &= ~(ADIS_UP | ADIS_DN | ADIS_LT | ADIS_RT);
                }
                break;
            case 11:
                if ( p->bit_pair_setting == 1 ) {
                    rtd->accept_key_as_fkey |= ADIS_TB | ADIS_BT;
                } else {
                    rtd->accept_key_as_fkey &= ~(ADIS_TB | ADIS_BT);
                }
                break;
            default:
                *code = 255;
                break;
        }
    } else {
        if ( code ) {
            *code = 255;
        }
    }
    return(0);
}

int
CBL_XAF (unsigned char *cnt, unsigned char *parm) {
    COB_RTD = cob_get_rtd();
    return rtd_CBL_XAF(rtd, cnt, parm);
}

#else

void
cob_screen_terminate (COB_RTD)
{
}

void
cob_screen_display (COB_RTD, cob_screen *s, cob_field *line, cob_field *column)
{
}

void
cob_screen_accept (COB_RTD, cob_screen *s, cob_field *line, cob_field *column)
{
}

void
cob_screen_accept_1 (COB_RTD, cob_screen *s, cob_field *line, cob_field *column, int attr)
{
}

void
cob_screen_line_col (COB_RTD, cob_field *f, const int l_or_c)
{
}

void
cob_screen_set_mode (COB_RTD, const size_t smode)
{
}

void
cob_screen_newline (COB_RTD)
{
}

void
cob_screen_append (COB_RTD, char * str, int size)
{
}

#endif

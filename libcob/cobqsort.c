/*
 *  qsort.c: standard quicksort algorithm
 *
 *  Modifications from vanilla NetBSD source:
 *    Add do ... while() macro fix
 *    Remove __inline, _DIAGASSERTs, __P
 *    Remove ill-considered "swap_cnt" switch to insertion sort,
 *    in favor of a simple check for presorted input.
 * 
 *  Modified from PostgreSQL: pgsql/src/port/qsort.c,v 1.12
 *    Add thread safe context to cmp fct 
 *    Copyright 2009 COBOL-IT
 */


/*-
 * Copyright (c) 1992, 1993
 *  The Regents of the University of California.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
static char *med3(char *a, char *b, char *c,
                  int (*cmp) (void *, const void *, const void *), void * contex);
static void swapfunc(char *, char *, size_t, int);
void cob_qsort(void *a, size_t n, size_t es, int (*cmp) (void *, const void *, const void *), void * contex);

/*
 * Qsort routine based on J. L. Bentley and M. D. McIlroy,
 * "Engineering a sort function",
 * Software--Practice and Experience 23 (1993) 1249-1265.
 * We have modified their original by adding a check for already-sorted input,
 * which seems to be a win per discussions on pgsql-hackers around 2006-03-21.
 */
#define swapcode(TYPE, parmi, parmj, n) \
do {        \
    size_t i = (n) / sizeof (TYPE);         \
    TYPE *pi = (TYPE *)(void *)(parmi);         \
    TYPE *pj = (TYPE *)(void *)(parmj);         \
    do {                        \
        TYPE    t = *pi;            \
        *pi++ = *pj;                \
        *pj++ = t;              \
        } while (--i > 0);              \
} while (0)

#define SWAPINIT(a, es) swaptype = ((char *)(a) - (char *)0) % sizeof(long) || \
    (es) % sizeof(long) ? 2 : (es) == sizeof(long)? 0 : 1;

static void
swapfunc(char *a, char *b, size_t n, int swaptype)
{
    if ( swaptype <= 1 )
        swapcode(long, a, b, n);
    else
        swapcode(char, a, b, n);
}

#define swap(a, b)                      \
    if (swaptype == 0) {                    \
        long t = *(long *)(void *)(a);          \
        *(long *)(void *)(a) = *(long *)(void *)(b);    \
        *(long *)(void *)(b) = t;           \
    } else                          \
        swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n) if ((n) > 0) swapfunc((a), (b), (size_t)(n), swaptype)

#define Min(x, y)		((x) < (y) ? (x) : (y))

static char *
med3(char *a, char *b, char *c, int (*cmp) (void * , const void *, const void *), void * contex)
{
    return cmp(contex, a, b) < 0 ?
    (cmp(contex, b, c) < 0 ? b : (cmp(contex, a, c) < 0 ? c : a))
    : (cmp(contex, b, c) > 0 ? b : (cmp(contex, a, c) < 0 ? a : c));
}

void
cob_qsort(void *a, size_t n, size_t es, int (*cmp) (void *, const void *, const void *), void * contex)
{
    char       *pa,
    *pb,
    *pc,
    *pd,
    *pl,
    *pm,
    *pn;
    int         d,
    r,
    swaptype,
    presorted;

    loop:SWAPINIT(a, es);
    if ( n < 7 ) {
        for ( pm = (char *) a + es; pm < (char *) a + n * es; pm += es )
            for ( pl = pm; pl > (char *) a && cmp(contex, pl - es, pl) > 0;
                pl -= es )
                swap(pl, pl - es);
        return;
    }
    presorted = 1;
    for ( pm = (char *) a + es; pm < (char *) a + n * es; pm += es ) {
        if ( cmp(contex, pm - es, pm) > 0 ) {
            presorted = 0;
            break;
        }
    }
    if ( presorted )
        return;
    pm = (char *) a + (n / 2) * es;
    if ( n > 7 ) {
        pl = (char *) a;
        pn = (char *) a + (n - 1) * es;
        if ( n > 40 ) {
            d = (n / 8) * es;
            pl = med3(pl, pl + d, pl + 2 * d, cmp, contex);
            pm = med3(pm - d, pm, pm + d, cmp, contex);
            pn = med3(pn - 2 * d, pn - d, pn, cmp, contex);
        }
        pm = med3(pl, pm, pn, cmp, contex);
    }
    swap(a, pm);
    pa = pb = (char *) a + es;
    pc = pd = (char *) a + (n - 1) * es;
    for ( ;; ) {
        while ( pb <= pc && (r = cmp(contex, pb, a)) <= 0 ) {
            if ( r == 0 ) {
                swap(pa, pb);
                pa += es;
            }
            pb += es;
        }
        while ( pb <= pc && (r = cmp(contex, pc, a)) >= 0 ) {
            if ( r == 0 ) {
                swap(pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if ( pb > pc )
            break;
        swap(pb, pc);
        pb += es;
        pc -= es;
    }
    pn = (char *) a + n * es;
    r = Min(pa - (char *) a, pb - pa);
    vecswap(a, pb - r, r);
    r = Min(pd - pc, pn - pd - es);
    vecswap(pb, pn - r, r);
    if ( (r = pb - pa) > es )
        cob_qsort(a, r / es, es, cmp, contex);
    if ( (r = pd - pc) > es ) {
        /* Iterate rather than recurse to save stack space */
        a = pn - r;
        n = r / es;
        goto loop;
    }
/*      qsort(pn - r, r / es, es, cmp);*/
}

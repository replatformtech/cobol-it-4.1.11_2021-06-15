bin_PROGRAMS = cobc 
cobc_SOURCES = cobc.c cobc.h config.c tree.c tree.h reserved.c error.c \
	parser.c parser.h scanner.c field.c typeck.c codegen.c \
	ppparse.c ppparse.h pplex.c config.def flag.def warning.def longopt.def \
	call.def enterprise.c 

AM_CFLAGS = $(COBC_USRCFLAGS)
cobc_CFLAGS = $(AM_CFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/libcob 
cobc_LDADD = $(COBC_LIBS) $(COB_EXPORT_DYN) -lcitsupport -lcitenterprise $(LIB_DL) 


BUILT_SOURCES = parser.c  scanner.c ppparse.c  pplex.c 
                

EXTRA_DIST = ppparse.y pplex.l parser.y scanner.l \
             parser.h ppparse.h 
CLEANFILES = parser.output

.l.c:
	flex -Ca -s -o$@ $<
	sleep 2
	touch $@

.y.c:
	bison -o $@ $<
	sleep 2
	touch $@

bin_PROGRAMS = cobcrun 
bin_SCRIPTS = cobol-it-setup.sh 
cobcrun_SOURCES = cobcrun.c

AM_CFLAGS =  $(COBC_USRCFLAGS)
AM_LDFLAGS = $(COB_EXPORT_DYN)
cobcrun_CFLAGS = $(AM_CFLAGS) -I. -I$(top_srcdir)
cobcrun_LDADD = -L$(top_builddir)/libcob -l$(COB_LIB_NAME) $(LIBCOB_LIBS)


cobcrun_ladir = @COB_LIBS_DIR@
cobcrun_la_DATA =  cobcrun.c cobcrun.o 

EXTRA_DIST = cobol-it-setup.base

if CIT_ENTERPRISE
include $(srcdir)/own_tools.am
endif

cobol-it-setup.sh: $(top_builddir)/config.status  cobol-it-setup.base
	@echo "#setup the needed environement variables"  	>cobol-it-setup.sh
	@echo "DEFAULT_CITDIR=${prefix}" 			>>cobol-it-setup.sh		
	@cat cobol-it-setup.base 				>>cobol-it-setup.sh




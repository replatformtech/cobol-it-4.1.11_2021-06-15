# CIT enterprise runtime library
lib_LTLIBRARIES = libcitenterprise.la
libcitenterprise_la_SOURCES = citenterprise_runtime.c citenterprise_runtime.h optimizer.c 

libcitenterprise_la_CFLAGS = $(AM_CFLAGS) $(LIBCOB_CFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/libcob 
libcitenterprise_la_LIBADD = -L$(top_builddir)/libcob -l$(COB_LIB_NAME) $(COB_LIBS) 
libcitenterprise_la_LDFLAGS = -version-info $(COB_LIB_VERSION) 
AM_LDFLAGS = $(COB_FIX_LIB)


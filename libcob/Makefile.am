lib_LTLIBRARIES = libcobit.la
libcobit_la_SOURCES = common.c call.c strings.c move.c numeric.c \
	intrinsic.c fileio.c  termio.c screenio.c \
        cobqsort.c grisu.c\
	zlib/adler32.c zlib/compress.c 	zlib/crc32.c  	zlib/deflate.c \
	zlib/gzclose.c zlib/gzlib.c 	zlib/gzread.c 	zlib/gzwrite.c  zlib/infback.c \
	zlib/inffast.c zlib/inflate.c 	zlib/inftrees.c zlib/trees.c 	zlib/uncompr.c zlib/zutil.c

libcobit_la_CFLAGS = $(AM_CFLAGS) $(LIBCOB_CFLAGS) -I. -I./zlib -I$(top_srcdir) 
libcobit_la_LIBADD = $(LIBCOB_LIBSTATIC) $(LIBCOB_LIBS) 
libcobit_la_LDFLAGS = -version-info  $(COB_LIB_VERSION) 
AM_LDFLAGS = $(COB_FIX_LIB)

libcobit_ladir = @COB_LIBS_DIR@
libcobit_la_DATA =  cittuxedo.c citdb2.c

EXTRA_DIST = coblocal.h cittuxedo.c citdb2.c zlib/crc32.h zlib/deflate.h zlib/gzguts.h zlib/inffast.h zlib/inffixed.h zlib/inflate.h \
	zlib/inftrees.h  zlib/trees.h zlib/zconf.h zlib/zlib.h zlib/zutil.h branchlut.c digitslut.h grisu.h readdir_win.h

pkgincludedir = $(includedir)/libcob
pkginclude_HEADERS = byteswap.h common.h move.h numeric.h exception.def \
	termio.h fileio.h screenio.h strings.h call.h intrinsic.h \
	codegen.h system.def cit_types.h gen_oper.h cit_rtinterface.h 

libcobit_la_SOURCES +=	enterprise/newmeta.c enterprise/xmlb.c enterprise/citkey.c enterprise/sha1.c enterprise/citenterprise.c \
	enterprise/mf_extfh.c enterprise/a2e.c enterprise/debug.c enterprise/citbigint.c  enterprise/e4c.c enterprise/sqlite3.c \
	enterprise/debugdb.c

EXTRA_DIST += enterprise/newmeta.h enterprise/xmlb.h enterprise/xmlbu.h enterprise/sha1.h enterprise/citkey.h enterprise/mf_extfh.h \
	enterprise/citenterprise.h mf_extfh.h citenterprise.h enterprise/a2e.h stringutils.c stringutils.h enterprise/citbigint.h \
	debug.h enterprise/debugdb.h cit_extsm.c

pkginclude_HEADERS += enterprise/citenterprise.h enterprise/a2e.h enterprise/mf_extfh.h enterprise/debug.h enterprise/e4c.h \
			enterprise/sqlite3.h cob_decimal.h 

libcobit_la_CFLAGS += -I./enterprise

	 



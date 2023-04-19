#                                                       -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright (C) 2001-2007 Keisuke Nishida
# Copyright (C) 2007 Roger While
# Copyright (C) 2008 COBOL-IT
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to
# the Free Software Foundation, 51 Franklin Street, Fifth Floor
# Boston, MA 02110-1301 USA

AC_PREREQ([2.63])

AC_INIT([COBOL-IT],['`sh version.sh`'],[support@cobol-it.com],[cobol-it])

AC_CONFIG_MACRO_DIR([m4])
# AC_CONFIG_SRCDIR([libcob.h])
AC_CONFIG_HEADERS([config.h])


# Libtool -version-info for libgmp.la and libmp.la.  See "Versioning" in the
# libtool manual.
#
#	CURRENT:REVISION:AGE
#
# 1. No interfaces changed, only implementations (good): Increment REVISION.
#
# 2. Interfaces added, none removed (good): Increment CURRENT, increment
#    AGE, set REVISION to 0.
#
# 3. Interfaces removed (BAD, breaks upward compatibility): Increment
#    CURRENT, set AGE and REVISION to 0.
#
export COB_MINIMAL_MINOR_VERSION=0
COB_LIB_VERSION="7:0:0"
LT_INIT(static)
AC_SUBST([LIBTOOL_DEPS])


AC_MSG_CHECKING(if --enable-debug option specified)
AC_ARG_ENABLE(debug,
	[AS_HELP_STRING([--enable-debug],[Build a debugging version.])],
	[cit_debug="$enable_debug"], [cit_debug="no"])
AC_MSG_RESULT($cit_debug)

AC_MSG_CHECKING(if --enable-profile option specified)
AC_ARG_ENABLE(profile,
	[AS_HELP_STRING([--enable-profile],[Build a profiling version.])],
	[cit_profile="$enable_profile"], [cit_profile="no"])
AC_MSG_RESULT($cit_profile)
# Set CFLAGS/CXXFLAGS.  We MUST set the flags before we call autoconf
# compiler configuration macros, because if we don't, they set CFLAGS
# to no optimization and -g, which isn't what we want.
CFLAGS=${CFLAGS-""}
CXXFLAGS=${CXXFLAGS-"$CFLAGS"}
LDFLAGS=${LDFLAGS-""}

# Note for SUN Solaris (gcc)
# export/setenv CFLAGS "-m64 -mptr64"
# export/setenv LDFLAGS "-m64 -mptr64 -L/usr/local/lib/sparcv9"
# ./configure --libdir=/usr/local/lib/sparcv9
#

AM_INIT_AUTOMAKE([gnu no-texinfo.tex])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC(gcc xlc cc)
AC_PROG_CXX(gcc xlc cc)
AC_CHECK_TOOLS(AR, ar, :)

# Hack for AIX 64 bit
AIX64=0
# Hack for AIX 64 bit (gcc)
# Required -
# export/setenv CFLAGS=-maix64
# export/setenv LDFLAGS=-maix64
if echo $CFLAGS | grep 'aix64' 1>/dev/null 2>&1
then
	AIX64=1
fi
# Hack for AIX 64 bit (xlc)
# Required -
# export/setenv CFLAGS=-q64
# export/setenv LDFLAGS=-q64
if echo $CFLAGS | grep 'q64' 1>/dev/null 2>&1
then
	AIX64=1
fi
if test "$AIX64" = "1" 
then
	export AR="$AR -X64"
	export NM="/usr/ccs/bin/nm -X64 -B"
	COB_ARFLAGS="$COB_ARFLAGS -X64"
	X64_FLAGS=" -X64 "
fi

# Autoheader templates
AH_TEMPLATE([WITH_ICU], [Compile with ICU unicode library])
AH_TEMPLATE([WITH_ICU_GENERIC], [Compile with ICU unicode library No uncv.h and no version sufix])
AH_TEMPLATE([WITH_ICU_CIT], [Compile with ICU unicode library with _cit suffix])
AH_TEMPLATE([WITH_MF_KEYS], [Compile with support of license key check])

AH_TEMPLATE([WITH_INDEX_EXTFH], [Compile with an external ISAM handler])
AH_TEMPLATE([WITH_SEQRA_EXTFH], [Compile with an external SEQ/RAN handler])

# AH_TEMPLATE([WITH_LFS64], [Compile with large file system 64])
AH_TEMPLATE([COB_EXPERIMENTAL], [Enable experimemental code (Developers only!)])
AH_TEMPLATE([COB_PARAM_CHECK], [Enable CALL parameter checking])
AH_TEMPLATE([COB_EXPORT_DYN], [Compile/link option for exporting symbols])
AH_TEMPLATE([COB_PIC_FLAGS], [Compile/link option for PIC code])
AH_TEMPLATE([COB_SHARED_OPT], [Compile/link option for shared code])
AH_TEMPLATE([COB_STRIP_CMD], [Strip command])
AH_TEMPLATE([USE_LIBDL], [Use system dynamic loader])
AH_TEMPLATE([PATCH_LEVEL], [Define a patch level])
AH_TEMPLATE([WITH_VARSEQ], [Define variable sequential file format])
# AH_TEMPLATE([DLLHIDDEN], [Has visibility attribute])
AH_TEMPLATE([HAVE_PSIGN_OPT], [Has -Wno-pointer-sign])
AH_TEMPLATE([HAVE_ATTRIBUTE_ALIGNED], [Has __attribute__((aligned))])
AH_TEMPLATE([HAVE_ATTRIBUTE_ALIGNED8], [Has __attribute__((aligned(8)))])
AH_TEMPLATE([HAVE_TIMEZONE], [Has timezone variable])
AH_TEMPLATE([COB_EXEEXT], [Executable extension])
AH_TEMPLATE([COB_EXTRA_FLAGS], [Extra Compiler Flags])
AH_TEMPLATE([__USE_STRING_INLINES], [Compiler optimization])
AH_TEMPLATE([COB_HAS_INLINE], [Can use inline keyword])
AH_TEMPLATE([COB_NO_SELFOPEN], [Can not dlopen self])
AH_TEMPLATE([COB_STRFTIME], [Can use strftime for timezone])
AH_TEMPLATE([COB_LI_IS_LL], [long int is long long])
AH_TEMPLATE([HAVE__THREAD_ATTR], [Can use __THREAD keyword])
AH_TEMPLATE([HAVE_PTHREAD_H], [Have usable libpthread])
AH_TEMPLATE([COB_HAS_THREAD], [cobc and runtime are thread safe])
AH_TEMPLATE([HAVE_COLOR_SET], [curses has color_set function])
AH_TEMPLATE([COB_PTR_64BITS], [Use Pointer are 64 bits long])
AH_TEMPLATE([WITH_CTREE], [Use Faircom c-Tree])
AH_TEMPLATE([CIT_ENTERPRISE], [Is CIT enterprise compilation])
AH_TEMPLATE([COB_LIBICU], [ICU Lib used in staid of libiconv])
AH_TEMPLATE([CIT_EBCDIC_CHARSET], [Is EBCDIC Charset used as default by the Runtime (Entrerprise only)])
AH_TEMPLATE([CIT_DUAL_CHARSET], [Is EBCDIC/ASCII Charset both available (Entrerprise only)])
AH_TEMPLATE([PLATFORM])
AH_TEMPLATE([COB_MINIMAL_MINOR_VERSION])

AC_DEFINE(__USE_STRING_INLINES)
AC_DEFINE_UNQUOTED(COB_MINIMAL_MINOR_VERSION, $COB_MINIMAL_MINOR_VERSION)

STATICLIBPATH=""
COB_STATICLIBPATH=""
COB_STATICCFLAGS=""
AC_ARG_WITH(static-lib-path,
  [  --with-static-lib-path=<path>      (COBOL-IT) specify the static libs path],
  [ STATICLIBPATH=$withval/lib
  COB_STATICLIBPATH=-L$STATICLIBPATH
  COB_STATICCFLAGS=-I$withval/include
  ], [ COB_STATICLIBPATH=""] )

LIBCOB_LIBSTATIC="$COB_STATICLIBPATH "
LIBCOB_LIBS=" -lm  $LIBCOB_LIBS"
LIBCOB_CFLAGS="$COB_STATICCFLAGS $COB_STATICCFLAGS $lt_prog_compiler_pic $LIBCOB_CFLAGS"
COBC_LIBS="$COB_STATICLIBPATH $COBC_LIBS "
COBC_USRCFLAGS="$COB_STATICCFLAGS $COBC_USRCFLAGS"


# Configure options.
COB_CC=$CC
AC_ARG_WITH(cc,
  [  --with-cc=<cc>          (COBOL-IT) specify the C compiler used by cobc],
  [ COB_CC=$withval ], [ COB_CC=$CC] )

COB_AR=$AR
AC_ARG_WITH(ar,
  [  --with-ar=<ar>          (COBOL-IT) specify the ar archiver used by cobc],
  [ COB_AR=$withval ], [ COB_AR=$AR] )

COB_ARFLAGS=" -rc "
AC_ARG_WITH(arflags,
  [  --with-arflags=<fl>     (COBOL-IT) specify the ar flags to add used by cobc],
  [ COB_ARFLAGS=$withval ], [ COB_ARFLAGS=" -rc "] )


AC_PROG_CPP
AM_PROG_CC_C_O
AC_PROVIDE([AC_PROG_F77])
AC_PROVIDE([AC_PROG_CXX])
with_tags=
AC_PROG_LN_S
AC_PROG_INSTALL
AC_LIBTOOL_DLOPEN
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h sys/types.h signal.h fcntl.h malloc.h locale.h errno.h\
	stddef.h wchar.h dlfcn.h sys/time.h sys/ioctl.h sys/wait.h limits.h sys/file.h])
AC_CHECK_HEADERS([langinfo.h libintl.h locale.h stdlib.h string.h strings.h unistd.h ])
#AC_CHECK_HEADERS([readline/readline.h])
#readline required for unix in staticlib !
AC_DEFINE([HAVE_READLINE_READLINE_H], [1], [gnu readline required on unix])

# Check if rpath is disabled
AC_MSG_CHECKING(whether to use rpath)
AC_ARG_ENABLE(rpath,
	[AC_HELP_STRING([--disable-rpath],
			[Patches libtool to not use rpath in the libraries produced.])],
	[xerces_cv_rpath="$enable_rpath"], [xerces_cv_rpath="yes"])
AC_MSG_RESULT($xerces_cv_rpath)

# Patch libtool to not use rpath if requested.
#
AC_CONFIG_COMMANDS([libtool-rpath-patch],
[if test "$libtool_patch_use_rpath" = "no"; then
   echo config.status: patching libtool to not use rpath
   sed < libtool > libtool-2 's/^hardcode_libdir_flag_spec.*$'/'hardcode_libdir_flag_spec=" -D__LIBTOOL_NO_RPATH__ "/'
   mv libtool-2 libtool
   chmod 755 libtool
fi],
[libtool_patch_use_rpath=$xerces_cv_rpath])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_BIGENDIAN
AC_TYPE_SIZE_T
AC_STRUCT_TM
# AX_C___ATTRIBUTE__
AC_C_INLINE
if test "$ac_cv_c_inline" != no 
then
	AC_DEFINE(COB_HAS_INLINE,1)
fi


# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memmove memset memmem setlocale fcntl fsync fdatasync sync setsid strerror strcasecmp \
                strncasecmp strcasestr strchr strrchr strdup strstr strtol realpath gettimeofday mkfifo mknod flock])

# Check for timezone
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]], [[timezone = 3600;]])],
	[AC_DEFINE(HAVE_TIMEZONE, 1)],
	[])

# Checks for internationalization stuff
#AM_ICONV
AM_LANGINFO_CODESET

LDFLAGS="$LDFLAGS -lm" 
SAVE_LDFLAGS=$LDFLAGS
LDFLAGS="$COB_STATICLIBPATH $LDFLAGS"
CFLAGS="$COB_STATICCFLAGS $CFLAGS"

# Checks for workable strftime
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
	#include <time.h>]], [[
	time_t	ticks;
	int	ret;
	char	buff[16];
	ticks = time(NULL);
	ret = strftime(buff, 15, "%z", localtime(&ticks));
	if (ret != 5) {
		return 1;
	}
	if (buff[0] != '+' && buff[0] != '-') {
		return 1;
	}
	return 0;]])],
	[AC_DEFINE(COB_STRFTIME)],
	[],
	[])

# Checks for size of long
AC_RUN_IFELSE([AC_LANG_PROGRAM([[]], [[
	if (sizeof(long int) == sizeof(long long))
		return 0;
	return 1;
	]])],
	[AC_DEFINE(COB_LI_IS_LL)],
	[],
	[])

# Checks for size of (void*)
AC_RUN_IFELSE([AC_LANG_PROGRAM([[]], [[
	if (sizeof(void*) > 4)
		return 0;
	return 1;
	]])],
	[AC_DEFINE(COB_PTR_64BITS)
	 COB_64BITS=1
        ],
	[COB_64BITS=0],
	[])

AC_ARG_WITH(seqra-extfh,
  [  --with-seqra-extfh      (COBOL-IT) Use external SEQ/RAN file handler],
  [ if test "$with_seqra_extfh" = "yes"
    then
	AC_DEFINE(WITH_SEQRA_EXTFH)
    fi ], [])

#add  -I.../include


if test "x$prefix" = "xNONE"
then
  prefix="${ac_default_prefix}"
fi

COB_INCLUDEDIR="${prefix}/include"
COB_LIBDIR="${prefix}/lib"
if test "x$RPMBUILDCOBOLITDIR" != "x"
then
BUILD_INCLUDEDIR="$RPMBUILDCOBOLITDIR/${prefix}/include"
BUILD_LIBDIR="$RPMBUILDCOBOLITDIR/${prefix}/lib"
else
BUILD_INCLUDEDIR="$COB_INCLUDEDIR"
BUILD_LIBDIR="$COB_LIBDIR"
fi


COB_CFLAGS="$CFLAGS -I$COB_INCLUDEDIR"
# Include CFLAGS in COB_CFLAGS without optimization/debug options
cob_temp_flags=`echo "$CFLAGS " | sed -e 's/@<:@+-@:>@O@<:@0-9@:>@//' | sed -e 's/-O / /' | sed -e 's/-g / /' | sed -e 's/$-O//' | sed -e 's/$-g//'`
COB_CFLAGS="$COB_CFLAGS $cob_temp_flags"

CFLAGS="$CFLAGS -I$BUILD_INCLUDEDIR "
LDFLAGS="$LDFLAGS -L$BUILD_LIBDIR "
COB_LIBS_DIR="$COB_LIBDIR/$PACKAGE_TARNAME"

DEFINE_DL="no"
COB_TRY_DL="yes"

AC_ARG_WITH(dl,
  [  --with-dl               (COBOL-IT) use system dynamic loader (default)],
  [case $with_dl in  
    yes)	COB_TRY_DL="yes"
		;;
    no)		COB_TRY_DL="no"
		;;
    *)		AC_MSG_ERROR(["--with/without-dl can not have a value"])
		;;
  esac])

# Checks for dl/ltdl.

if test "x$ac_cv_header_dlfcn_h" = "xyes"
then
	AC_DEFINE(USE_LIBDL)
else
	AC_CHECK_HEADERS([ltdl.h], , AC_MSG_ERROR(ltdl.h is required))
	AC_CHECK_LIB([ltdl], [lt_dlopen], , AC_MSG_ERROR(libltdl is required))
	LIB_DL="-lltdl"
	COB_TRY_DL = "no"
fi
if test "$DEFINE_DL" = "no" -a "$COB_TRY_DL" = "yes"
then
    AC_CHECK_LIB([dl], [dlopen], [
                DEFINE_DL="yes"
		LIB_DL="-ldl"
		], [])

    if test "$DEFINE_DL" = "no"
    then
	AC_CHECK_LIB([c], [dlopen], [DEFINE_DL="yes"], [])
	if test "$DEFINE_DL" = "no"
	then
		AC_CHECK_HEADERS([ltdl.h], , AC_MSG_ERROR(ltdl.h is required))
		AC_CHECK_LIB([ltdl], [lt_dlopen], , AC_MSG_ERROR(libltdl is required))
		LIB_DL="-lltdl"
	fi
    fi
fi

AC_MSG_CHECKING(checking lib dl)
AC_MSG_RESULT($LIB_DL)

LIBCOB_LIBS="$LIBCOB_LIBS $LIB_DL"
LIBS="$LIBS $LIB_DL"

COB_LIBICU="no"
AC_MSG_CHECKING(icu)
AC_CHECK_LIB([icuuccit -licudatacit], [ucnv_open_51_cit],   
				COB_LIBICU="-licuuccit -licudatacit"
				AC_DEFINE(WITH_ICU_CIT),				
				AC_CHECK_LIB([icuuc -licudata], [ucnv_open],   
					COB_LIBICU="-licuuc -licudata"
					AC_DEFINE(WITH_ICU_GENERIC)))
if test "$COB_LIBICU" = "no"
then
	AC_CHECK_HEADERS([unicode/ucnv.h], 
		COB_LIBICU="-licuuc -licudata",
		AC_MSG_RESULT(Not found)
		)
fi

if test "$COB_LIBICU" = "no"
then
	AC_MSG_ERROR(libicuuccit is required for ICU)
else
	AC_DEFINE(WITH_ICU)
	AC_DEFINE(COB_LIBICU, "$COB_LIBICU")
	LIBCOB_LIBS="$LIBCOB_LIBS $COB_LIBICU"
	AC_MSG_RESULT(OK)
fi


AC_ARG_ENABLE(experimental,
  [  --enable-experimental   (COBOL-IT) enable experimental code (Developers only!)],
  [if test "$enable_experimental" = "yes"
   then
	AC_DEFINE(COB_EXPERIMENTAL)
   fi])


AC_MSG_CHECKING(--enable-citenterprise)
AC_ARG_ENABLE(citenterprise,
	[AS_HELP_STRING([--enable-citenterprise],[Enable CIT Enterprise additional features.])],
	[ENABLECITENT="yes"],
	[ENABLECITENT="no"]
)
AC_MSG_RESULT($ENABLECITENT)

AS_IF([test "x$enable_citenterprise" == "xyes"], [
AC_DEFINE([CIT_ENTERPRISE])
CIT_OWN_TOOLS="cobcdb cobccmap dd_extfh cobmf"
CIT_OWN_SCRIPTS="citprocob.sh xcobcdb xcobcrun"
])

AM_CONDITIONAL([CIT_ENTERPRISE], [test "x$enable_citenterprise" == "xyes"])

AC_MSG_CHECKING(--enable-keycheck)
AC_ARG_ENABLE(keycheck,
	[AS_HELP_STRING([--enable-keycheck],[Enable MF License key check.])],
	[ENABLEKEYCHECK="yes"
	AC_DEFINE(WITH_MF_KEYS)], 
	[ENABLEKEYCHECK="no"])
AC_MSG_RESULT($ENABLEKEYCHECK)

CIT_EBCDIC_CHARSET_DEF=0
AC_MSG_CHECKING(--enable-ebcdic-charset)
AC_ARG_ENABLE(ebcdic-charset,
	[AS_HELP_STRING([--enable-ebcdic-charset],[Build compiler and runtime using EBCDIC charset as default (Enterprise only).])],
	[ENABLEEBCDICCHARSET="yes"
	AC_DEFINE(CIT_EBCDIC_CHARSET)]
	CIT_EBCDIC_CHARSET_DEF=1, [ENABLEEBCDICCHARSET="no"])
AC_MSG_RESULT($ENABLEEBCDICCHARSET)

CIT_DUAL_CHARSET_DEF=0
AC_MSG_CHECKING(--enable-dual-charset)
AC_ARG_ENABLE(dual-charset,
	[AS_HELP_STRING([--enable-dual-charset],[Build compiler and runtime using both EBCDIC/ASCII charset (Enterprise only).])],
	[ENABLEDUALCHARSET="yes"
	AC_DEFINE(CIT_DUAL_CHARSET)]
	CIT_DUAL_CHARSET_DEF=1, [ENABLEDUALCHARSET="no"])
AC_MSG_RESULT($ENABLEDUALCHARSET)


AC_MSG_CHECKING(--enable-thread-safe)
AC_ARG_ENABLE(thread-safe,
	[AS_HELP_STRING([--enable-thread-safe],[Build Thread safe code.])],
	[CIT_THREAD_SAFE="yes"
	CFLAGS="$CFLAGS -D_REENTRANT"
	COB_CFLAGS="$COB_CFLAGS -D_REENTRANT"], [CIT_THREAD_SAFE="no"])
AC_MSG_RESULT($CIT_THREAD_SAFE)

CIT_PTHREAD="no"
AC_MSG_CHECKING(--with-pthread)
AC_ARG_WITH(pthread,
	[AS_HELP_STRING([--with-pthread],[Use pthread in staid of __thread])],
	[CIT_PTHREAD="yes"], [CIT_PTHREAD="no"])
AC_MSG_RESULT($CIT_PTHREAD)


AC_ARG_ENABLE(param-check,
  [  --enable-param-check    (COBOL-IT) enable CALL parameter checking],
  [if test "$enable_param_check" = "yes"
   then
	AC_DEFINE(COB_PARAM_CHECK)
   fi])

PATCH_LEVEL=0
AC_ARG_WITH(patch-level, [  --with-patch-level      (COBOL-IT) define a patch level (default 0)],
[case $with_patch_level in  
    yes)	AC_MSG_ERROR(["You must give --with-patch-level an argument."])
		;;
    no)		AC_MSG_ERROR(["--without-patch-level not supported."])
		;;
    [[0-9]]*)	PATCH_LEVEL=$with_patch_level
		;;
    *)		AC_MSG_ERROR(["You must use a numeric patch level"])
		;;
esac])
AC_DEFINE_UNQUOTED(PATCH_LEVEL, $PATCH_LEVEL)

WITH_VARSEQ=0
AC_ARG_WITH(varseq, [  --with-varseq           (COBOL-IT) define variable sequential format (default 0)],
[case $with_varseq in  
    yes)	AC_MSG_ERROR(["You must give --with-varseq an argument."])
		;;
    no)		AC_MSG_ERROR(["--without-varseq not supported."])
		;;
    [[0-3]])	WITH_VARSEQ=$with_varseq
		;;
    *)		AC_MSG_ERROR(["Invalid --with-varseq argument"])
		;;
esac])
AC_DEFINE_UNQUOTED(WITH_VARSEQ, $WITH_VARSEQ)

save_libs="$LIBS"


# Checks for getopt_long.
# Cygwin has getopt_long_only prototype in kpathsea/getopt.h
# FreeBSD has gnugetopt library - needs checking

#AC_CHECK_FUNC([getopt_long_only],
#  [AC_CHECK_HEADERS([kpathsea/getopt.h getopt.h], [], [])],
#  [AC_LIBOBJ(getopt)
#   AC_LIBOBJ(getopt1)
#   AC_LIBSOURCE(getopt.h)])

# Checks __thread 
if test "$CIT_THREAD_SAFE" = "yes"; then
if test "$CIT_PTHREAD" = "no"; then
 AC_MSG_CHECKING(support __thread)
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[__thread int mythreadvar;]], [[
	mythreadvar = 0;
	return mythreadvar;
	]])],
	[AC_DEFINE(HAVE__THREAD_ATTR)
	AC_DEFINE(COB_HAS_THREAD)],
	[CIT_THREAD_SAFE="no"
	 CIT_PTHREAD = "yes"]
	)

AC_MSG_RESULT(${CIT_THREAD_SAFE})
fi

if test "$CIT_PTHREAD" = "yes"; then
 AC_CHECK_HEADERS([pthread.h], [AC_CHECK_LIB([pthread], [pthread_setspecific], 
                 [               AC_DEFINE(HAVE_PTHREAD_H)
                                 AC_DEFINE(COB_HAS_THREAD)
                                 LIBCOB_LIBS="$LIBCOB_LIBS -lpthread"
                                 SAVE_LDFLAGS="$SAVE_LDFLAGS -lpthread"
                 ], 
	                                     [CIT_THREAD_SAFE="no"]) ],
			       [CIT_THREAD_SAFE="no"])
 AC_MSG_CHECKING(usable libpthread)
 AC_MSG_RESULT(${CIT_THREAD_SAFE})
fi
fi




# Checks for gmp.
#GMPSAVE_CFLAGS="$CFLAGS"
#GMPSAVE_LDFLAGS="$LDFLAGS"
#export CFLAGS="$COB_STATICCFLAGS $CFLAGS"
#export LDFLAGS="$COB_STATICLIBPATH $LDFLAGS"

#AC_CHECK_HEADERS([gmp.h], , AC_MSG_ERROR(gmp.h is required))
#AC_CHECK_LIB([gmp], [__gmp_randinit], , AC_MSG_ERROR(GMP 3 or later is required))
LIBCOB_LIBSTATIC="$LIBCOB_LIBSTATIC -lgmp"
LIBCOB_LIBSTATIC="$LIBCOB_LIBSTATIC -lexpat -lreadline"
#CFLAGS="$GMPSAVE_CFLAGS"
#LDFLAGS="$GMPSAVE_LDFLAGS"

# Solaris has nanosleep in other libraries
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]],
  [[nanosleep (NULL, NULL);]])],
  [],
  [AC_CHECK_LIB([rt], [nanosleep])
   if test "x$ac_cv_lib_rt_nanosleep" = "xyes"
   then
     LIBCOB_LIBS="$LIBCOB_LIBS -lrt"
   else
     AC_CHECK_LIB([posix4], [nanosleep])
     if test "x$ac_cv_lib_posix4_nanosleep" = "xyes"
     then
       LIBCOB_LIBS="$LIBCOB_LIBS -lposix4"
     fi
   fi])

# Checks for gettext.
#AM_GNU_GETTEXT([external])
#AM_GNU_GETTEXT_VERSION(0.13)
#if test "x$LTLIBINTL" != "x"
#then
#  COBC_LIBS="$COBC_LIBS $LTLIBINTL"
#  LIBCOB_LIBS="$LIBCOB_LIBS $LTLIBINTL"
#fi

# Checks for ncurses/pdcurses/curses.
USE_NCURSES="no"
AC_CHECK_HEADERS([ncurses.h], AC_CHECK_LIB([ncurses], [initscr], [USE_NCURSES="ncurses"]))

if test "$USE_NCURSES" = "no"
then
	AC_CHECK_HEADERS([ncurses/ncurses.h], AC_CHECK_LIB([ncurses], [initscr], [USE_NCURSES="ncurses"]))
fi
if test "$USE_NCURSES" = "no"
then
	AC_CHECK_HEADERS([pdcurses.h], AC_CHECK_LIB([pdcurses], [initscr], [USE_NCURSES="pdcurses"]))
fi
if test "$USE_NCURSES" = "no"
then
	AC_CHECK_HEADERS([curses.h], AC_CHECK_LIB([curses], [initscr], [USE_NCURSES="curses"]))
fi

if test "$USE_NCURSES" != "no"
then
  LIBCOB_LIBS="$LIBCOB_LIBS -l$USE_NCURSES"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  #ifdef HAVE_NCURSES_H
  #include <ncurses.h>
  #elif defined (HAVE_NCURSES_NCURSES_H)
  #include <ncurses/ncurses.h>
  #elif defined (HAVE_PDCURSES_H)
  #include <pdcurses.h>
  #elif defined (HAVE_CURSES_H)
  #include <curses.h>
  #endif]], [[
  color_set (0, NULL);]])],
  [AC_DEFINE(HAVE_COLOR_SET)],
  [],
  [])
fi

# Some host required libs

case $host_os in
  mingw*)
	;;
  cygwin*)
	;;
  darwin* | rhapsody*)
	;;
  hpux*)
	;;
  aix*)
	;;
  solaris*)
	LIBCOB_LIBS="$LIBCOB_LIBS -lsocket -lnsl" 
	;;
esac


# COBOL-IT Configuration

COB_LIB_NAME="cobit"
COB_LIBS="-L$COB_LIBDIR -l$COB_LIB_NAME $LIBCOB_LIBS "
COB_LDFLAGS=""
COB_LDFLAGS="$SAVE_LDFLAGS -L$COB_LIBDIR"

COB_SHARED_DIR="$datadir/$PACKAGE_TARNAME"
COB_DOC_DIR="$COB_SHARED_DIR/doc"
COB_CONFIG_DIR="$COB_SHARED_DIR/config"
COB_COPY_DIR="$COB_SHARED_DIR/copy"
COB_LIBRARY_PATH=".:$COB_LIBS_DIR"
# COB_MODULE_EXT=`echo "$acl_cv_shlibext" | sed -e 's/dll\.a/dll/'`
COB_MODULE_EXT=`echo "$shrext_cmds" | cut -b 2- `
LIBS="$save_libs"
COB_EXPORT_DYN="`eval echo $export_dynamic_flag_spec`"
COB_PIC_FLAGS=`echo "$lt_prog_compiler_pic" | sed -e 's/^ //'`
COB_SHARED_OPT="-shared"
COB_SAMPLES_DIR="$prefix/samples"
COB_EXEEXT="$EXEEXT"
COB_EXTRA_FLAGS=""



# Check gcc 4 option
MYOLDCFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wno-pointer-sign"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[int testint;]])],
	[AC_DEFINE(HAVE_PSIGN_OPT, 1)],
	[CFLAGS="$MYOLDCFLAGS"])
CFLAGS="$MYOLDCFLAGS"

# Check if aligned attribute works
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
	[[char testchar1[4] __attribute__((aligned));]])],
	[AC_DEFINE(HAVE_ATTRIBUTE_ALIGNED, 1)],
	[])


# Check if aligned(8) attribute works
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
	[[char testchar2[4] __attribute__((aligned(8)));]])],
	[AC_DEFINE(HAVE_ATTRIBUTE_ALIGNED8, 1)],
	[])

# Special stuff

AH_VERBATIM([_XOPEN_SOURCE_EXTENDED],
[/* Define to 1 if on HPUX.  */
#ifndef _XOPEN_SOURCE_EXTENDED
# undef _XOPEN_SOURCE_EXTENDED
#endif])dnl

COB_OPTSIZE_FLAG=${OPTSIZE_FLAG-"-Os"}
COB_OPTIMIZE_FLAG=${OPTIMIZE_FLAG-"-O2"}
COB_FIX_LIB="$COB_EXPORT_DYN"
PLATFORM=$host_os
case $host_os in
  mingw*)
	if test "$prefix" = "NONE"
	then
	COB_CONFIG_DIR="`cd /usr && pwd -W`/local/share/$PACKAGE_TARNAME/config"
	COB_COPY_DIR="`cd /usr && pwd -W`/local/share/$PACKAGE_TARNAME/copy"
	fi
	COB_EXPORT_DYN="-Wl,--export-all-symbols -Wl,--enable-auto-import"
	COB_FIX_LIB="$COB_EXPORT_DYN"
	PLATFORM="WINDOWS"
	;;
  cygwin*)
	COB_EXPORT_DYN="-Wl,--export-all-symbols -Wl,--enable-auto-import"
	COB_FIX_LIB="$COB_EXPORT_DYN"
	PLATFORM="WINDOWS"
	;;
  darwin* | rhapsody*)
	COB_SHARED_OPT="-bundle -flat_namespace -undefined suppress"
	PLATFORM="MAC"
	;;
  hpux*)
	PLATFORM="HPUX"
	if test "`basename $CC`" != "gcc"
	then
		COB_OPTIMIZE_FLAG=${OPTIMIZE_FLAG-"+O2"}
		COB_OPTSIZE_FLAG=${OPTSIZE_FLAG-"+O2 +Osize"}
		COB_SHARED_OPT="-b"
		AC_DEFINE([_XOPEN_SOURCE_EXTENDED],1)
		rm -f hptest*
		echo 'int main() { return 0; }' > hptest.c
		${CC} ${CFLAGS} +Olit=all -o hptest hptest.c > hptest.out 2>&1

		if test $? -ne 0 -o -s hptest.out
		then
			CFLAGS="$CFLAGS +ESlit "
			COB_CFLAGS="$COB_CFLAGS +ESlit -w"
		else
                        case `uname -m` in
                           9000*)
			        CFLAGS="$CFLAGS +ESlit "
			        COB_CFLAGS="$COB_CFLAGS +ESlit -w "
                                ;;
                           *)
			        CFLAGS="$CFLAGS +Olit=all "
			        COB_CFLAGS="$COB_CFLAGS +Olit=all -w -notrigraph "
                                ;;
			esac
		fi
		rm -f hptest*
	fi
	;;
  aix*)
	PLATFORM="AIX"
	COB_EXPORT_DYN="-Wl,-bexpall -Wl,-brtl"
	if test "`basename $CC`" != "gcc"
	then
		CFLAGS="$CFLAGS -Q -qro -qroconst -qchars=signed"
		COB_OPTIMIZE_FLAG=${OPTIMIZE_FLAG-"-O2 -qmaxmem=-1"}		
		COB_OPTSIZE_FLAG=${OPTSIZE_FLAG-"-qcompact -qmaxmem=-1"}		
		COB_SHARED_OPT="-G"
		COB_FIX_LIB=""
		# COB_FIX_LIB="-Wc,-G $COB_EXPORT_DYN"
		COB_CFLAGS="$COB_CFLAGS -qchars=signed -w -qmaxmem=-1 -qnotrigraph"
	else
		COB_FIX_LIB="$COB_EXPORT_DYN"
	fi
	;;
  solaris*)
	PLATFORM="SOLARIS"
	if test "`basename $CC`" != "gcc"
	then
		COB_OPTIMIZE_FLAG=${OPTIMIZE_FLAG-"-xO2"}		
		COB_OPTSIZE_FLAG=${OPTSIZE_FLAG-"-xO2 -xspace"}		
		COB_CFLAGS="$COB_CFLAGS -xstrconst -w "
		COB_SHARED_OPT="-G"
	#else
	#	LIBCOB_CFLAGS="$LIBCOB_CFLAGS -g "
	fi
	;;
  linux*)
	PLATFORM="LINUX"
	echo "LINUX on " $host_cpu
	case $host_cpu in
		x86*)
			if test "$cit_debug" = "no"
			then
				if test "`basename $COB_CC`" = "gcc"
				then
					cob_temp_flags="`${COB_CC} ${srcdir}/cpucheck.c -o cpucheck 1>/dev/null 2>&1 && ./cpucheck 2>/dev/null`"
					rm -f cpucheck${EXEEXT}
					if test "x$cob_temp_flags" != "x"
					then
						CFLAGS="$CFLAGS $cob_temp_flags -finline-functions -fomit-frame-pointer "
					else
						CFLAGS="$CFLAGS -fomit-frame-pointer"
					fi
					COB_EXTRA_FLAGS=" $cob_temp_flags -finline-functions "
					COB_CFLAGS="$COB_CFLAGS -fomit-frame-pointer"
				fi
			fi
			;;
		s390*)
			PLATFORM="zLINUX"
			;;
	esac

	;;

esac
AC_DEFINE_UNQUOTED(PLATFORM, "$PLATFORM")
CFLAGSNOOPT="$CFLAGS"
# If the user wants a debugging environment, change any compiler optimization
# flags to -g.  We used to add -g to the -O compiler flags, but compilers are
# good enough at code re-organization that debugging with -O no longer works.
# If you want to compile with a different set of flags, specify CFLAGS in the
# environment before configuring.
if test "$cit_debug" = "yes"; then
	AC_DEFINE(DEBUG)
	AH_TEMPLATE(DEBUG, [Define to 1 if you want a debugging version.])
	CFLAGS="$CFLAGS -g"
else
	CFLAGS="$CFLAGS $COB_OPTIMIZE_FLAG"
fi

if test "$cit_profile" = "yes"; then
	CFLAGS="$CFLAGS -p"
fi

PWD=`pwd`
CFLAGS="-I. -I$PWD/libcob $CFLAGS"

if test "`basename $CC`" = "gcc"
then
	CFLAGS="$CFLAGS -fsigned-char -Wall -Wwrite-strings -Wmissing-prototypes -Wno-format-y2k"
fi

if test "x$lt_cv_dlopen_self" != "xyes"
then
	AC_DEFINE(COB_NO_SELFOPEN,1)
fi


# Generate the output

AC_DEFINE_UNQUOTED(COB_EXPORT_DYN, "$COB_EXPORT_DYN")
AC_DEFINE_UNQUOTED(COB_PIC_FLAGS, "$COB_PIC_FLAGS")
AC_DEFINE_UNQUOTED(COB_SHARED_OPT, "$COB_SHARED_OPT")
AC_DEFINE_UNQUOTED(COB_EXEEXT, "$COB_EXEEXT")
AC_DEFINE_UNQUOTED(COB_EXTRA_FLAGS, "$COB_EXTRA_FLAGS")
if test "x$old_striplib" != "x"
then
  AC_DEFINE_UNQUOTED(COB_STRIP_CMD, "$old_striplib")
else 
  if test "x$STRIP" != "x"
  then
    AC_DEFINE_UNQUOTED(COB_STRIP_CMD, "$STRIP")
  fi
fi
AC_SUBST(COB_CC)
AC_SUBST(COB_AR)
AC_SUBST(COB_CFLAGS)
AC_SUBST(COB_LDFLAGS)
AC_SUBST(COB_ARFLAGS)
AC_SUBST(COB_LIBS)
AC_SUBST(COB_LIB_NAME)
AC_SUBST(COB_LIBS_DIR)
AC_SUBST(COB_LIB_VERSION)
AC_SUBST(COB_CONFIG_DIR)
AC_SUBST(COB_SAMPLES_DIR)
AC_SUBST(COB_DOC_DIR)
AC_SUBST(COB_COPY_DIR)
AC_SUBST(COB_LIBRARY_PATH)
AC_SUBST(COB_MODULE_EXT)
AC_SUBST(COBC_LIBS)
AC_SUBST(COBC_USRCFLAGS)
AC_SUBST(LIBCOB_LIBSTATIC)
AC_SUBST(LIBCOB_CFLAGS)
AC_SUBST(LIBCOB_LIBS)
AC_SUBST(LIB_DL)
AC_SUBST(COB_EXPORT_DYN)
AC_SUBST(COB_PIC_FLAGS)
AC_SUBST(COB_SHARED_OPT)
COB_BIGENDIAN="$ac_cv_c_bigendian"
AC_SUBST(COB_BIGENDIAN)
AC_SUBST(COB_FIX_LIB)
AC_SUBST(COB_HAS_INLINE)
AC_SUBST(COB_HAS_THREAD)
AC_SUBST(COB_OPTIMIZE_FLAG)
AC_SUBST(COB_OPTSIZE_FLAG)
#AC_SUBST(COB_STRIP_CMD)
AC_SUBST(COB_LIBICU)
AC_SUBST(CFLAGSNOOPT)
AC_SUBST(CIT_EBCDIC_CHARSET_DEF)
AC_SUBST(CIT_DUAL_CHARSET_DEF)
AC_SUBST(PLATFORM)
AC_SUBST(COB_MINIMAL_MINOR_VERSION)
AC_SUBST(COB_64BITS)
AC_SUBST(CIT_OWN_TOOLS)
AC_SUBST(CIT_OWN_SCRIPTS)


AC_CONFIG_FILES([cob-config
		Makefile lib/Makefile libcob/Makefile 
		cobc/Makefile bin/Makefile config/Makefile
		copy/Makefile 
		samples/Makefile
		libcob/cit_types.h build.patchversion])
#		test/atlocal test/Makefile test/cobol85/Makefile
AC_OUTPUT
AC_CONFIG_FILES([
		libcitenterprise/Makefile])

AC_OUTPUT

echo ""
echo "COBOL-IT Configuration:"
echo ""
echo "  PLATFORM             ${PLATFORM}"
echo "  CC                   ${CC}"
echo "  COB_CC               ${COB_CC}"
echo "  COB_AR               ${COB_AR}"
echo "  CFLAGS               ${CFLAGS}"
echo "  COB_CFLAGS           ${COB_CFLAGS}"
echo "  COB_EXTRA_FLAGS      ${COB_EXTRA_FLAGS}"
echo "  COB_OPTIMIZE_FLAG    ${COB_OPTIMIZE_FLAG}"
echo "  COB_OPTSIZE_FLAG     ${COB_OPTSIZE_FLAG}"
echo "  LDFLAGS              ${LDFLAGS}"
echo "  COB_LDFLAGS          ${COB_LDFLAGS}"
echo "  COB_CONFIG_DIR       ${COB_CONFIG_DIR}"
echo "  COB_COPY_DIR         ${COB_COPY_DIR}"
echo "  COB_LIBRARY_PATH     ${COB_LIBRARY_PATH}"
echo "  COB_MODULE_EXT       ${COB_MODULE_EXT}"
echo "  COB_SHARED_OPT       ${COB_SHARED_OPT}"
echo "  COB_PIC_FLAGS        ${COB_PIC_FLAGS}"
echo "  COB_EXPORT_DYN       ${COB_EXPORT_DYN}"
echo "  LIBCOB_CFLAGS        ${LIBCOB_CFLAGS}"
echo "  LIBCOB_LIBS          ${LIBCOB_LIBS}"
echo "  COBC_USRCFLAGS       ${COBC_USRCFLAGS}"
echo "  COBC_LIBS            ${COBC_LIBS}"
if test "x$COB_STRIP_CMD" != "x"
then
  echo "  COB_STRIP_CMD        ${COB_STRIP_CMD}"
fi
if test "${DEFINE_DL}" = "yes" 
then
  echo "  Dynamic loading      System"
else
  echo "  Dynamic loading      Libtool"
fi
echo ""
#echo "  Use gettext for international messages:      ${USE_NLS}"
if test "$with_seqra_extfh" = "yes"
then
  echo "  Use static external SEQ/RAN file handler     yes"
fi
echo "  Use fcntl for file locking:                  ${ac_cv_func_fcntl}"
echo "  Use ncurses/pdcurses/curses for screen I/O:  ${USE_NCURSES}"
echo "  Thread safe                                  ${CIT_THREAD_SAFE}"
echo "  CIT ENTERPRISE                               ${enable_citenterprise}"
echo "  MF KEY Check				     ${enable_keycheck}"
echo ""

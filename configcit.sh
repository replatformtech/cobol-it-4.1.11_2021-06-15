#!/bin/sh
# Copyright (C) 2008 COBOL-IT
#

if [ "x$PREFIX" = "x" ]
then
    SYS=`uname`
    if [ "x$SYS" = "xMINGW32_NT-5.1" ]
    then
        echo Windows config
        PREFIX="C:/Cobol/CobolIT_mingw"
    else
        echo Unix config
        PREFIX="/opt/cobol-it"
    fi
fi
    
CFLAGS="-I$PREFIX/include $CFLAGS"
LDFLAGS="-L$PREFIX/lib $LDFLAGS"
export CFLAGS
export LDFLAGS
if [ "x$BUILDCOBOLITDEBUG" = "x" ]
then
   sh ./configure --prefix $PREFIX  --with-vbisam --enable-shared=yes --enable-static=no $*
else
   sh ./configure --prefix $PREFIX  --with-vbisam --enable-shared=yes --enable-static=yes --enable-debug --enable-profile $*
fi



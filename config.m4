dnl $Id$
dnl config.m4 for extension xz

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(xz, for xz support,
[  --with-xz             Include xz support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(xz, whether to enable xz support,
dnl Make sure that the comment is aligned:
dnl [  --enable-xz           Enable xz support])

if test "$PHP_XZ" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-xz -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/xz.h"  # you most likely want to change this
  dnl if test -r $PHP_XZ/$SEARCH_FOR; then # path given as parameter
  dnl   XZ_DIR=$PHP_XZ
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for xz files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       XZ_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$XZ_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the xz distribution])
  dnl fi

  dnl # --with-xz -> add include path
  dnl PHP_ADD_INCLUDE($XZ_DIR/include)

  dnl # --with-xz -> check for lib and symbol presence
  LIBNAME=lzma # you may want to change this
  LIBSYMBOL=lzma_stream_encoder # you most likely want to change this

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $XZ_DIR/lib, XZ_SHARED_LIBADD)
    AC_DEFINE(HAVE_XZLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong xz lib version or lib not found])
  ],[
    -L$XZ_DIR/lib -lm
  ])
  PHP_SUBST(XZ_SHARED_LIBADD)

  PHP_NEW_EXTENSION(xz, xz.c xz_fopen_wrapper.c, $ext_shared)
fi

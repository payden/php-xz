dnl +----------------------------------------------------------------------+
dnl | XZ Extension                                                         |
dnl +----------------------------------------------------------------------+
dnl | Copyright (c) 1997-2015 The PHP Group                                |
dnl +----------------------------------------------------------------------+
dnl | This source file is subject to version 3.01 of the PHP license,      |
dnl | that is bundled with this package in the file LICENSE, and is        |
dnl | available through the world-wide-web at the following url:           |
dnl | http://www.php.net/license/3_01.txt                                  |
dnl | If you did not receive a copy of the PHP license and are unable to   |
dnl | obtain it through the world-wide-web, please send a note to          |
dnl | license@php.net so we can mail you a copy immediately.               |
dnl +----------------------------------------------------------------------+
dnl | Authors: Payden Sutherland <payden@paydensutherland.com>             |
dnl |          Dan Ungureanu <udan1107@gmail.com>                          |
dnl |          authors of the `zlib` extension (for guidance)              |
dnl +----------------------------------------------------------------------+

PHP_ARG_WITH(xz, for xz support,
[  --with-xz             Include xz support])

if test "$PHP_XZ" != "no"; then

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

  PHP_NEW_EXTENSION(xz, xz.c xz_fopen_wrapper.c utils.c, $ext_shared)
fi

/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Payden Sutherland <payden@paydensutherland.com>              |
  |         Mostly looked at ext/zlib for guidance                       |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_XZ_H
#define PHP_XZ_H

extern zend_module_entry xz_module_entry;
#define phpext_xz_ptr &xz_module_entry

#define XZ_INBUF_SIZE 4096
#define XZ_OUTBUF_SIZE 4096

#ifdef PHP_WIN32
#	define PHP_XZ_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_XZ_API __attribute__ ((visibility("default")))
#else
#	define PHP_XZ_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(xz);
PHP_MSHUTDOWN_FUNCTION(xz);
PHP_RINIT_FUNCTION(xz);
PHP_RSHUTDOWN_FUNCTION(xz);
PHP_MINFO_FUNCTION(xz);
PHP_FUNCTION(xzopen);

php_stream *php_stream_xzopen(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(xz)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(xz)
*/

/* In every utility function you add that needs to use variables 
   in php_xz_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as XZ_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define XZ_G(v) TSRMG(xz_globals_id, zend_xz_globals *, v)
#else
#define XZ_G(v) (xz_globals.v)
#endif

#endif	/* PHP_XZ_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

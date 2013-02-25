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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "php_xz.h"

/* If you declare any globals in php_xz.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(xz)
*/

/* True global resources - no need for thread safety here */
static int le_xz;


ZEND_BEGIN_ARG_INFO(arginfo_xzread, 0)
  ZEND_ARG_INFO(0, fp)
  ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xzwrite, 0)
  ZEND_ARG_INFO(0, fp)
  ZEND_ARG_INFO(0, str)
  ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xzclose, 0)
  ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xzpassthru, 0)
  ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

/* {{{ xz_functions[]
 *
 * Every user visible function must have an entry in xz_functions[].
 */
const zend_function_entry xz_functions[] = {
  PHP_FE(xzopen, NULL)
  PHP_FALIAS(xzread, fread, arginfo_xzread)
  PHP_FALIAS(xzwrite, fwrite, arginfo_xzwrite)
  PHP_FALIAS(xzclose, fclose, arginfo_xzclose)
  PHP_FALIAS(xzpassthru, fpassthru, arginfo_xzpassthru)
  PHP_FE_END  /* Must be the last line in xz_functions[] */
};
/* }}} */

/* {{{ xz_module_entry
 */
zend_module_entry xz_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  "xz",
  xz_functions,
  PHP_MINIT(xz),
  PHP_MSHUTDOWN(xz),
  PHP_RINIT(xz),    /* Replace with NULL if there's nothing to do at request start */
  PHP_RSHUTDOWN(xz),  /* Replace with NULL if there's nothing to do at request end */
  PHP_MINFO(xz),
#if ZEND_MODULE_API_NO >= 20010901
  "0.1", /* Replace with version number for your extension */
#endif
  STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XZ
ZEND_GET_MODULE(xz)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("xz.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_xz_globals, xz_globals)
    STD_PHP_INI_ENTRY("xz.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_xz_globals, xz_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_xz_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_xz_init_globals(zend_xz_globals *xz_globals)
{
  xz_globals->global_value = 0;
  xz_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xz)
{
  /* If you have INI entries, uncomment these lines 
  REGISTER_INI_ENTRIES();
  */
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(xz)
{
  /* uncomment this line if you have INI entries
  UNREGISTER_INI_ENTRIES();
  */
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(xz)
{
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(xz)
{
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xz)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "xz support", "enabled");
  php_info_print_table_header(2, "liblzma version", lzma_version_string());
  php_info_print_table_end();

  /* Remove comments if you have entries in php.ini
  DISPLAY_INI_ENTRIES();
  */
}
/* }}} */


PHP_FUNCTION(xzopen)
{
  char *filename;
  char *mode;
  int filename_len, mode_len;
  php_stream *stream;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filename, &filename_len, &mode, &mode_len) == FAILURE) {
    return;
  }
  stream = php_stream_xzopen(NULL, filename, mode, 0, NULL, NULL STREAMS_CC TSRMLS_CC);

  if (!stream) {
    RETURN_FALSE;
  }

  php_stream_to_zval(stream, return_value);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

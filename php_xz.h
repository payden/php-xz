/*
    +----------------------------------------------------------------------+
    | XZ Extension                                                         |
    +----------------------------------------------------------------------+
    | Copyright (c) 1997-2015 The PHP Group                                |
    +----------------------------------------------------------------------+
    | This source file is subject to version 3.01 of the PHP license,      |
    | that is bundled with this package in the file LICENSE, and is        |
    | available through the world-wide-web at the following url:           |
    | http://www.php.net/license/3_01.txt                                  |
    | If you did not receive a copy of the PHP license and are unable to   |
    | obtain it through the world-wide-web, please send a note to          |
    | license@php.net so we can mail you a copy immediately.               |
    +----------------------------------------------------------------------+
    | Authors: Payden Sutherland <payden@paydensutherland.com>             |
    |          Dan Ungureanu <udan1107@gmail.com>                          |
    |          authors of the `zlib` extension (for guidance)              |
    +----------------------------------------------------------------------+
*/

#ifndef PHP_XZ_H
#define PHP_XZ_H

extern zend_module_entry xz_module_entry;
extern php_stream_wrapper php_stream_xz_wrapper;

#define phpext_xz_ptr &xz_module_entry

/**
 * The default size of the buffer used for compression and decompression.
 */
#define XZ_BUFFER_SIZE                  4096

#ifdef PHP_WIN32
    #define PHP_XZ_API __declspec(dllexport)
#elif defined(__GNUC__) && (__GNUC__ >= 4)
    #define PHP_XZ_API __attribute__ ((visibility("default")))
#else
    #define PHP_XZ_API
#endif

#ifdef ZTS
    #include "TSRM.h"
#endif

/**
 * Function called when module initializes.
 */
PHP_MINIT_FUNCTION(xz);

/**
 * Function called when module shuts down.
 */
PHP_MSHUTDOWN_FUNCTION(xz);

/**
 * Function called to retrieve module's information.
 */
PHP_MINFO_FUNCTION(xz);

/**
 * Opens a XZ stream.
 */
PHP_FUNCTION(xzopen);

/**
 * Encodes a given string.
 */
PHP_FUNCTION(xzencode);

/**
 * Decodes a given string.
 */
PHP_FUNCTION(xzdecode);

/**
 * Function that opens a stream. It is internally used for `xzopen`.
 *
 * @param wrapper
 * @param path
 * @param mode_pass
 * @param options
 * @param opened_path
 *
 * @return
 */
php_stream *php_stream_xzopen(php_stream_wrapper *wrapper, const char *path,
    const char *mode_pass, int options, char **opened_path,
    php_stream_context *context STREAMS_DC TSRMLS_DC);

#ifdef ZTS
    #define XZ_G(v) TSRMG(xz_globals_id, zend_xz_globals *, v)
#else
    #define XZ_G(v) (xz_globals.v)
#endif

#endif

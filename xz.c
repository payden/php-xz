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

#include <lzma.h>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#include "ext/standard/file.h"
#include "ext/standard/info.h"

#include "utils.h"

#include "php_xz.h"

/**
 * Parameters for `xzread`.
 */
ZEND_BEGIN_ARG_INFO(arginfo_xzread, 0)
    ZEND_ARG_INFO(0, fp)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

/**
 * Parameters for `xzwrite`.
 */
ZEND_BEGIN_ARG_INFO(arginfo_xzwrite, 0)
    ZEND_ARG_INFO(0, fp)
    ZEND_ARG_INFO(0, str)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

/**
 * Parameters for `xzwrite`.
 */
ZEND_BEGIN_ARG_INFO(arginfo_xzclose, 0)
    ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

/**
 * Parameters for `xzpassthru`.
 */
ZEND_BEGIN_ARG_INFO(arginfo_xzpassthru, 0)
    ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

/**
 * Parameters for `xzencode`.
 */
ZEND_BEGIN_ARG_INFO(arginfo_xzencode, 0)
    ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

/**
 * Parameters for `xzdecode`.
 */
ZEND_BEGIN_ARG_INFO(arginfo_xzdecode, 0)
    ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

/**
 * Table containing functions exported by this module.
 */
const zend_function_entry xz_functions[] = {
    PHP_FE(xzdecode, NULL)
    PHP_FE(xzopen, NULL)
    PHP_FE(xzencode, NULL)
    PHP_FALIAS(xzread, fread, arginfo_xzread)
    PHP_FALIAS(xzwrite, fwrite, arginfo_xzwrite)
    PHP_FALIAS(xzclose, fclose, arginfo_xzclose)
    PHP_FALIAS(xzpassthru, fpassthru, arginfo_xzpassthru)
#ifndef PHP_FE_END
    { NULL, NULL, NULL, 0, 0 }
#else
    PHP_FE_END
#endif
};

/**
 * Table describing module.
 */
zend_module_entry xz_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "xz",
    xz_functions,
    PHP_MINIT(xz),
    PHP_MSHUTDOWN(xz),
    NULL, // PHP_RINIT
    NULL, // PHP_RSHUTDOWN
    PHP_MINFO(xz),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1.1",
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_XZ
    ZEND_GET_MODULE(xz)
#endif

/**
 * INI entries.
 */
PHP_INI_BEGIN()

    /**
     * Default compression level. Affects `xzencode` and `xzopen`, but only when
     * the level was not specified.
     */
    PHP_INI_ENTRY("xz.compression_level", "5", PHP_INI_ALL, NULL)

    /**
     * The maximum amount of memory that can be used when decompressing.
     * `0` stands for unlimited.
     */
    PHP_INI_ENTRY("xz.max_memory", "0", PHP_INI_SYSTEM, NULL)

PHP_INI_END()

PHP_MINIT_FUNCTION(xz)
{
    REGISTER_INI_ENTRIES();
    php_register_url_stream_wrapper("compress.lzma", &php_stream_xz_wrapper TSRMLS_CC);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(xz)
{
    php_unregister_url_stream_wrapper("compress.lzma" TSRMLS_CC);
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_MINFO_FUNCTION(xz)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "xz support", "enabled");
    php_info_print_table_header(2, "liblzma version", lzma_version_string());
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

PHP_FUNCTION(xzopen)
{
    char *filename = NULL, *mode = NULL;
    size_t filename_len = 0, mode_len = 0;
    unsigned long compression_level = INI_INT("xz.compression_level");

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &filename, &filename_len, &mode, &mode_len, &compression_level) == FAILURE) {
        return;
    }

    char *mode_to_pass = emalloc(mode_len + 32);
    snprintf(mode_to_pass, mode_len + 32, "%s:%lu", mode, compression_level);

    php_stream *stream = php_stream_xzopen(NULL, filename, mode_to_pass, 0, NULL, NULL STREAMS_CC TSRMLS_CC);

    if (!stream) {
        RETURN_BOOL(0);
    }

    php_stream_to_zval(stream, return_value);
}

PHP_FUNCTION(xzencode)
{
    /** The string to be encoded. */
    uint8_t *in = NULL;
    /** The length of the string to be encoded */
    size_t in_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_len) == FAILURE) {
        return;
    }
    /** The output string (encoded). */
    uint8_t *out = NULL;
    /** The length of the output string. */
    size_t out_len = 0;

    /** Internal buffer used for encoding. */
    uint8_t buff[XZ_BUFFER_SIZE];

    lzma_options_lzma opt_lzma2;
    if (lzma_lzma_preset(&opt_lzma2, INI_INT("xz.compression_level"))) {
        RETURN_BOOL(0);
    }

    lzma_filter filters[] = {
        { .id = LZMA_FILTER_LZMA2, .options = &opt_lzma2 },
        { .id = LZMA_VLI_UNKNOWN,  .options = NULL },
    };

    // Initializing encoder.
    lzma_stream strm = LZMA_STREAM_INIT;
    if (lzma_stream_encoder(&strm, filters, LZMA_CHECK_CRC64) != LZMA_OK) {
        RETURN_BOOL(0);
    }

    // Setting the input source.
    strm.avail_in = in_len;
    strm.next_in = in;

    // Setting the output source.
    strm.avail_out = XZ_BUFFER_SIZE;
    strm.next_out = buff;

    // Encoding the string.
    lzma_ret status = LZMA_OK;
    while (strm.avail_in != 0) {
        status = lzma_code(&strm, LZMA_RUN);
        // More memory is required.
        if (strm.avail_out == 0) {
            out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE);
            out_len += XZ_BUFFER_SIZE;
            strm.avail_out = XZ_BUFFER_SIZE;
            strm.next_out = buff;
        }
    }

    // Finish encoding.
    while (status != LZMA_STREAM_END) {
        status = lzma_code(&strm, LZMA_FINISH);
        // An error occured.
        if ((status != LZMA_STREAM_END) && (status != LZMA_OK)) {
            lzma_end(&strm);
            RETURN_LONG(status);
        }
        // More memory is required.
        if (strm.avail_out == 0) {
            out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE);
            out_len += XZ_BUFFER_SIZE;
            strm.avail_out = XZ_BUFFER_SIZE;
            strm.next_out = buff;
        }
    }

    // Merging last fragment.
    out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE - strm.avail_out);
    out_len += XZ_BUFFER_SIZE - strm.avail_out;

    lzma_end(&strm);

    RETURN_STRINGL(out, out_len,  0);
}

PHP_FUNCTION(xzdecode)
{
    /** The string to be encoded. */
    uint8_t *in = NULL;
    /** The length of the string to be encoded */
    size_t in_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_len) == FAILURE) {
        return;
    }

    /** The output string (encoded). */
    uint8_t *out = NULL;
    /** The length of the output string. */
    size_t out_len = 0;

    /** Internal buffer used for encoding. */
    uint8_t buff[XZ_BUFFER_SIZE];

    // Initializing decoder.
    lzma_stream strm = LZMA_STREAM_INIT;
    uint64_t mem = INI_INT("xz.max_memory");
    if (lzma_auto_decoder(&strm, mem ? mem : UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK) {
        RETURN_BOOL(0);
    }

    // Setting the input source.
    strm.avail_in = in_len;
    strm.next_in = in;

    // Setting the output source.
    strm.avail_out = XZ_BUFFER_SIZE;
    strm.next_out = buff;

    // Decoding the string
    lzma_ret status = LZMA_OK;
    while (strm.avail_in != 0) {
        status = lzma_code(&strm, LZMA_RUN);
        // More memory is required.
        if (strm.avail_out == 0) {
            out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE);
            out_len += XZ_BUFFER_SIZE;
            strm.avail_out = XZ_BUFFER_SIZE;
            strm.next_out = buff;
        }
    }

    // Merging last fragment.
    out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE - strm.avail_out);
    out_len += XZ_BUFFER_SIZE - strm.avail_out;

    lzma_end(&strm);

    RETURN_STRINGL(out, out_len,  0);
}

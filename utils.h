/*
	+----------------------------------------------------------------------+
	| PHP Version 5                                                        |
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

#ifndef __UTILS_H__
#define __UTILS_H__

/* Merges two memory fragments by reallocating the first one.
   Returns a pointer to the first memory segment or, if reallocated, to the new
   address. */
void *memmerge(void *ptr1, void *ptr2, size_t len1, size_t len2);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
--TEST--
Test xzopen() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xz")) {
	print "skip - XZ extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : resource xzopen(string filename, string mode)
 * Description: Open a xz file and return a xz file pointer 
 * Source code: ext/xz/xz.c
 * Alias to functions: 
 */

echo "*** Testing xzopen() : error conditions ***\n";


//Test gzopen with one more than the expected number of arguments
echo "\n-- Testing xzopen() function with invalid mode --\n";
$filename = 'string_val';
$mode = 'a';
var_dump( xzopen($filename, $mode) );

?>
===DONE===
--EXPECTF--
*** Testing xzopen() : error conditions ***

-- Testing xzopen() function with invalid mode --

Warning: xzopen(): Can only open in read (r) or write (w) mode.
bool(false)
===DONE===

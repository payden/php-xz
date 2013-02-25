--TEST--
Test xzopen() function : basic functionality
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
  print "skip - XZ extension not loaded";
}
?>
--FILE--
<?php
/* Prototype : resource xzopen(string filename, string mode)
 * Description: Open a .xz file and return a xz stream.
 * Source code: ext/xz/xz.c
 */

echo "*** Testing xzopen() : basic functionality ***\n";

$filename = dirname(__FILE__) . "/001.txt.xz";
$mode = 'r';

$fh = xzopen($filename, $mode);
xzpassthru($fh);
xzclose($fh);
?>
===DONE===
--EXPECTF--
*** Testing xzopen() : basic functionality ***
"Three Rings for the Elven-kings under the sky,
Seven for the Dwarf-lords in halls of stone,
Nine for Mortal Men, doomed to die,
One for the Dark Lord on his dark throne
In the Land of Mordor where the Shadows lie.
One Ring to rule them all, One Ring to find them,
One Ring to bring them all and in the darkness bind them.
In the Land of Mordor where the Shadows lie"
===DONE===

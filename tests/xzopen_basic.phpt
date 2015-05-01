--TEST--
Test `xzopen`: basic functionality.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	print("XZ extension is not loaded!");
}
?>
--FILE--
<?php
$fh = xzopen(dirname(__FILE__) . '/001.txt.xz', 'r');

xzpassthru($fh);

xzclose($fh);
?>
--EXPECTF--
"Three Rings for the Elven-kings under the sky,
Seven for the Dwarf-lords in halls of stone,
Nine for Mortal Men, doomed to die,
One for the Dark Lord on his dark throne
In the Land of Mordor where the Shadows lie.
One Ring to rule them all, One Ring to find them,
One Ring to bring them all and in the darkness bind them.
In the Land of Mordor where the Shadows lie"

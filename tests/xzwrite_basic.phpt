--TEST--
Test function xzwrite() by calling it with its expected arguments
--SKIPIF--
<?php 
if (!extension_loaded("xz")) {
	print "skip - XZ extension not loaded"; 
}
?>
--FILE--
<?php

$filename = "temp.txt.gz";
$h = xzopen($filename, 'w');
$str = "Here is the string to be written. ";
$length = 10;
var_dump(xzwrite( $h, $str ) );
var_dump(xzwrite( $h, $str, $length ) );
xzclose($h);

$h = xzopen($filename, 'r');
xzpassthru($h);
xzclose($h);
echo "\n";
unlink($filename);
?>
===DONE===
--EXPECT--
int(34)
int(10)
Here is the string to be written. Here is th
===DONE===

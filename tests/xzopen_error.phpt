--TEST--
Test `xzopen`: error conditions.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
    print("XZ extension is not loaded!");
}
?>
--FILE--
<?php
var_dump(xzopen('/dev/null', 'a'));
?>
--EXPECTF--
Warning: xzopen(): Can only open in read (r) or write (w) mode.%s
bool(false)

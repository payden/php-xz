--TEST--
Test `xzencode` and `xzdecode`: big inputs.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	print("XZ extension is not loaded!");
}
?>
--FILE--
<?php

/**
 * The size of the chunk that is going to be repeated.
 * @var int
 */
$chunkSize = 1 * 1024 * 1024; // 1 MB

/**
 * The number of chunks.
 * @var integer
 */
$chunkNumber = 8; // 256 chunks of 1 MB = 256 MB

/**
 * A random chunk of data.
 * @var string
 */
$chunk = '';

// Generating a random chunk.
for ($i = 0; $i < $chunkSize; ++$i) {
	$chunk .= chr(rand(0, 255)); // adds one random byte
}

/**
 * A random string of size `$size`.
 * @var string
 */
$str = '';

// Generating a random string.
for ($i = 0; $i < $chunkNumber; ++$i) {
	$str .= $chunk;
}

var_dump($chunkSize * $chunkNumber == strlen($str));
$encoded = xzencode($str);
print("encoding finished\n");
$decoded = xzdecode($encoded);
print("decoding finished\n");
var_dump($str == $decoded);

?>
--EXPECTF--
bool(true)
encoding finished
decoding finished
bool(true)

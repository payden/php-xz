php-xz
======

PHP Extension providing xz (LZMA2) compression/decompression via PHP streams.

# Installation

To install as module, perform the following steps:

$> cd /path/to/php-5.4.12/ext
$> git clone https://github.com/payden/php-xz xz
$> cd xz && phpize && ./configure && make && sudo make install

Add 'extension = xz.so' to your php.ini.

# Basic usage:

```php
<?php
$fh = xzopen("/tmp/test.xz", "w");
xzwrite($fh, "Data you would like compressed and written.\n");
xzclose($fh);
$fh = xzopen("/tmp/test.xz", "r");
echo xzread($fh, 1024);
xzclose($fh);
?>
```

php-xz
======

[![Build Status](https://travis-ci.org/udan11/php-xz.svg?branch=master)](https://travis-ci.org/udan11/php-xz)

PHP Extension providing xz (LZMA2) compression/decompression via PHP streams.

## Installation

To install as module, perform the following steps:

```bash
cd /path/to/php/extension_dir
git clone https://github.com/payden/php-xz xz
cd xz && phpize && ./configure && make && sudo make install
```

Do not forget to add `extension = xz.so` to your `php.ini`.

__TIP:__ If you want to quickly find out where the extension directory is located, you can use the following command: `php -i | grep 'extension_dir'`.

## Requirements

This module depends on `git`, `php5-dev` and `liblzma-dev`. If you are using Ubuntu, you can easily install all of them by typing the following command in your terminal:

```bash
sudo apt-get install git php5-dev liblzma-dev
```

## Basic usage

```php
<?php

$fh = xzopen("/tmp/test.xz", "w");
xzwrite($fh, "Data you would like compressed and written.\n");
xzclose($fh);

$fh = xzopen("/tmp/test.xz", "r");
xzpassthru($fh);
xzclose($fh);
```

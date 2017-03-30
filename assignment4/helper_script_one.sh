#!/bin/bash

rmmod char_driver
rm -rf /dev/char_driver
mknod /dev/char_driver c 61 0
make
insmod char_driver.ko
gcc char_test.c -o char_test -g -Wall
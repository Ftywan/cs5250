#!/bin/bash

rmmod char_driver
echo "removed char module"
rm -rf /dev/char_driver
echo "removed char driver"
mknod /dev/char_driver c 61 0
echo "created a new char driver"
make clean
echo "finished cleaning files"
make all
echo "finished compiling char_driver"
insmod char_driver.ko
echo "inserted char driver"
gcc char_test.c -o char_test -g -Wall
echo "compiled char_test.c"
gcc test.c -o test -g -Wall
echo "compiled test.c"
gcc test2.c -o test2 -g -Wall
echo "compiled test2.c"
dmesg -C
echo "cleared dmesg contents"
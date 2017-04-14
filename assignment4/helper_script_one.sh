#!/bin/bash

if [[ "$EUID" -ne 0 ]]; then
    echo "script must be run as root"
    exit 1
fi

function check_return()
{
    local str="$@"
    if [[ "$?" -ne 0 ]]; then
        echo "operation $str failed.. quitting now.."
        exit 2
    else 
        echo $str
        printf "\n"
    fi
}

rmmod char_driver
check_return "removed char module"
rm -rf /dev/char_driver
check_return "removed char driver"
mknod /dev/char_driver c 61 0
check_return "created a new char driver"
make clean
check_return "finished cleaning files"
make all
check_return "finished compiling char_driver"
insmod char_driver.ko
check_return "inserted char driver"
gcc char_test.c -o char_test -g -Wall
check_return "compiled char_test.c"
gcc test.c -o test -g -Wall
check_return "compiled test.c"
gcc test2.c -o test2 -g -Wall
check_return "compiled test2.c"
dmesg -C
check_return "cleared dmesg contents"

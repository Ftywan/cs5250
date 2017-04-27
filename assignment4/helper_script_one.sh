#!/bin/bash

if [[ "$EUID" -ne 0 ]]; then
    echo "script must be run as root"
    exit 1
fi

function check_return()
{
    local str="$@"
    if [[ "$?" -ne 0 ]]; then
        echo "task \"$str\" failed. quitting now."
        exit 2
    else 
        echo "finished \"$str\" task"
        printf "\n"
    fi
}

rmmod char_driver
check_return "remove char module"

rm -rf /dev/char_driver
check_return "remove char driver"

mknod /dev/char_driver c 61 0
check_return "create a new char driver"

make clean
check_return "clean files"

make all
check_return "compile char_driver"

insmod char_driver.ko
check_return "insert char driver"

gcc char_test.c -o char_test -g -Wall
check_return "compile char_test.c"

gcc test.c -o test -g -Wall
check_return "compile test.c"

gcc test2.c -o test2 -g -Wall
check_return "compile test2.c"

dmesg -C
check_return "clear dmesg contents"

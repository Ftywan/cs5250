#!/bin/bash

echo "copying 4MB into char driver"
./char_test --size 4
cat /dev/char_driver 
printf "\n*new line due to printf printed by test script for clarity\n"
echo "copying 4MB + 1 byte into char driver"
./char_test --size 41
cat /dev/char_driver
printf "\n*new line due to printf printed by test script for clarity\n"
echo "copying 5MB into char driver"
./char_test --size 5
cat /dev/char_driver
printf "\n*new line due to printf printed by test script for clarity\n"
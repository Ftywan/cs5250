#!/bin/bash

cat /dev/char_driver 
echo abc > /dev/char_driver 
cat /dev/char_driver 
echo defg > /dev/char_driver 
cat /dev/char_driver
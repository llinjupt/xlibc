#!/bin/sh
gcc test.c -o test -std=c11
./test
gcc test.c -o test -std=c99
./test
rm -f test

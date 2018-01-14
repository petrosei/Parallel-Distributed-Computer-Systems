#!/bin/sh
rm meanshift_serial
gcc -O3 -o meanshift_serial meanshift_serial.c -lm

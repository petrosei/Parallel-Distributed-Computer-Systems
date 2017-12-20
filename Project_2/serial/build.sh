#!/bin/sh
rm knn_serial
mpicc -O3 -o knn_serial knn_serial.c

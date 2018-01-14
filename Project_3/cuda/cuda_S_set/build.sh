#!/bin/sh
rm meanshift_cuda
nvcc -O3 -o meanshift_cuda meanshift_cuda.cu
#gcc -O3 -o meanshift_cuda meanshift_cuda.c

#!/bin/sh
rm meanshift_cuda_shared
nvcc -O3 -o meanshift_cuda_shared meanshift_cuda_shared.cu
#gcc -O3 -o meanshift_cuda meanshift_cuda.c

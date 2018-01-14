#!/bin/sh
rm knn_mpi
mpicc -O3 -o knn_mpi knn_mpi.c

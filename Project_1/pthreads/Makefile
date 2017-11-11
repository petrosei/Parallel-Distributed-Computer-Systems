
CC = gcc
CFLAGS = -lpthread
CNAME = pthreads_parallel.c
EXECNAME = pthreads_parallel

pthreads:
	$(CC) -O3 -o $(EXECNAME) $(CNAME) $(CFLAGS)
DEBUG:
	 $(CC) -g -O3 -o $(EXECNAME) $(CNAME) $(CFLAGS)

clean:
	rm $(EXECNAME)


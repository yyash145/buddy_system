CC=gcc
CFLAGS=-g -O0 -fPIC -Werror -Wall

TESTS=t-test1 test1
HEADERS=utils.h malloc.h

all:	${TESTS} libmalloc.so

clean:
	rm -rf *.o *.so ${TESTS}

libmalloc.so: utils.o malloc.o calloc.o free.o realloc.o reallocarray.o posix_memalign.o memalign.o mallinfo.o malloc_stats.o
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all $^ -o $@

%: %.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) $< -c -o $@

check:	libmalloc.so ${TESTS}
	LD_PRELOAD=`pwd`/libmalloc.so ./test1

check1:	libmalloc.so ${TESTS}
	LD_PRELOAD=`pwd`/libmalloc.so ./t-test1

dist: clean
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar


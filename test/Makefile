CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

# build the .o files, then use ar to create(cr) the libutils.a in the lib directory
.PHONY:				all clean

all:					test_indexio

test_indexio:			test_indexio.c
									gcc $(CFLAGS) test_indexio.c $(LIBS) -o $@

clean:
							rm -rf test_indexio *~ *.o

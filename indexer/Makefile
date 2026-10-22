CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

# build the .o files, then use ar to create(cr) the libutils.a in the lib directory
.PHONY:				all clean

all:					indexer

indexer:			indexer.c
							gcc $(CFLAGS) indexer.c $(LIBS) -o $@

#test:					test.c
#							gcc $(CFLAGS) test.c $(LIBS) -o $@

clean:
							rm -rf indexer

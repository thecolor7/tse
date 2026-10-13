CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

# build the .o files, then use ar to create(cr) the libutils.a in the lib directory
all:					crawler

crawler:			crawler.c
							gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
							rm -rf *.o 

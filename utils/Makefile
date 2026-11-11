CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=lhash.o lqueue.o queue.o hash.o webpage.o pageio.o indexio.o testLhash.o

# build the .o files, then use ar to create(cr) the libutils.a in the lib directory
all:					$(OFILES)
							ar cr ../lib/libutils.a $(OFILES)

#should handle all of the .o files
%.o:					%.c %.h
							gcc $(CFLAGS) -c $<

#need one of these per test
testLhash:		testLhash.o lhash.o hash.o queue.o
							gcc $(CFLAGS) lhash.o hash.o queue.o testLhash.o -o $@ -lpthread

clean:
							rm -rf *.o testLhash 

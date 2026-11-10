CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=lhash.o lqueue.o queue.o hash.o webpage.o pageio.o indexio.o testLhash.o

# build the .o files, then use ar to create(cr) the libutils.a in the lib directory
all:					$(OFILES)
							ar cr ../lib/libutils.a $(OFILES)

#should handle all of the .o files
%.o:					%.c %.h
							gcc $(CFLAGS) -c $<

#need one of these per test
testLhash:		lhash.o testLhash.o
							gcc $(CFLAGS) lhash.o testLhash.o -o $@

clean:
							rm -rf *.o testLhash 

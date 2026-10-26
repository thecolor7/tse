CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o pageio.o

# build the .o files, then use ar to create(cr) the libutils.a in the lib directory
all:					$(OFILES)
							ar cr ../lib/libutils.a $(OFILES)

#should handle all of the .o files
%.o:					%.c %.h
							gcc $(CFLAGS) -c $<

#need one of these per test
#testmfile:		hash.o queue.o testmfile.o
#							gcc $(CFLAGS) hash.o queue.o testmfile.o -o $@

clean:
							rm -rf *.o

CFLAGS=-Wall -pedantic -std=c11 -I. -g

all: 					testQput testQapply testQremove

#should handle all of the .o files
%.o:					%.c %.h
							gcc $(CFLAGS) -c $<

#need one of these per test
testQput: 		queue.o testQput.o
							gcc $(CFLAGS) queue.o testQput.o -o $@

testQapply:   queue.o testQapply.o
							gcc $(CFLAGS) queue.o testQapply.o -o $@

testQremove:	queue.o testQremove.o
							gcc $(CFLAGS) queue.o testQremove.o -o $@

clean:
							rm -rf *.o

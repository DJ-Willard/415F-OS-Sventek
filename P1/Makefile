CFLAGS=-g -W -Wall
PROGRAMS=cpubound iobound thv1 thv2 thv3 thv4

all: $(PROGRAMS)

cpubound: cpubound.o
	gcc -o $@ $^

iobound: iobound.o
	gcc -o $@ $^

thv1: thv1.o p1fxns.o
	gcc -o $@ $^

thv2: thv2.o p1fxns.o
	gcc -g -o $@ $^

thv3: thv3.o p1fxns.o
	gcc -o $@ $^

thv4: thv4.o p1fxns.o
	gcc -o $@ $^

p1fxns.o: p1fxns.c
cpubound.o: cpubound.c
iobound.o: iobound.c
thv1.o: thv1.c
thv2.o: thv2.c
thv3.o: thv3.c
thv4.o: thv4.c

clean:
	rm -f $(PROGRAMS) *.o

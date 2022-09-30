CFLAGS=-W -Wall -I/usr/local/include -g
LDFLAGS=-L/usr/local/lib -g
PROGRAMS=longtest stringtest
OBJECTS=hashbag.o llistbag.o sort.o
LIBRARIES=-lADTs

all: $(PROGRAMS)

longtest: longtest.o $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

stringtest: stringtest.o $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

longtest.o: longtest.c bag.h
stringtest.o: stringtest.c bag.h
hashbag.o: hashbag.c hashbag.h bag.h
llistbag.o: llistbag.c llistbag.h bag.h
sort.o: sort.c sort.h

clean:
	rm -f $(PROGRAMS) longtest.o stringtest.o $(OBJECTS)

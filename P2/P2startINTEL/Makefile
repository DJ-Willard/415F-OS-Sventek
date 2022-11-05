CFLAGS=-W -Wall -g
LDFLAGS=-g
OBJECTS=testharness.o diskdriver.o
LIBRARIES=./libTH.a

mydemo: $(OBJECTS)
	gcc -o $@ $^ $(LIBRARIES) -lpthread

clean:
	rm -f diskdriver.o mydemo

diskdriver.o: diskdriver.c

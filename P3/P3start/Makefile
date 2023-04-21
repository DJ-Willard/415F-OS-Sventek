CFLAGS=-W -Wall -I/usr/local/include -g -DDEBUG
LDFLAGS=-L/usr/local/lib -g
PROGRAMS=psbv1 psbv2 psbv3 psbv4 echoclient subscriber publisher listchannels listsubscribers
LIBRARIES=-lBXP -lpthread -lADTs

all: $(PROGRAMS)

psbv1: psbv1.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

psbv2: psbv2.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

psbv3: psbv3.o sort.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

psb4: psbv4.o sort.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

echoclient: echoclient.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

subscriber: subscriber.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

publisher: publisher.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

listchannels: listchannels.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

listsubscribers: listsubscribers.o
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

psbv1.o: psbv1.c
psbv2.o: psbv2.c
psbv3.o: psbv3.c
psbv4.o: psbv4.c
echoclient.o: echoclient.c
subscriber.o: subscriber.c
publisher.o: publisher.c
sort.o: sort.c
listchannels.o: listchannels.c
listsubscribers.o: listsubscribers.c

CFLAGS=-W -Wall -I/usr/local/include -g
LDFLAGS=-L/usr/local/lib -g
PROGRAMS=longtest stringtest
OBJECTS=hashset.o llistset.o sort.o
LIBRARIES=-lADTs

all: $(PROGRAMS)

longtest: longtest.o $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

stringtest: stringtest.o $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

longtest.o: longtest.c set.h
stringtest.o: stringtest.c set.h
hashset.o: hashset.c hashset.h set.h
llistset.o: llistset.c llistset.h set.h
sort.o: sort.c sort.h

clean:
	rm -f $(PROGRAMS) longtest.o stringtest.o $(OBJECTS)

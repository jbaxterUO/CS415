CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -L/usr/local/lib -g
LDLIBS = -lADTs -lc

PROGRAMS = uspsv1 uspsv2 uspsv3 uspsv4
OBJECTS1= uspsv1.o p1fxns.o 
OBJECTS2= uspsv2.o p1fxns.o 
OBJECTS3= uspsv3.o p1fxns.o
OBJECTS4= uspsv4.o p1fxns.o


all: $(PROGRAMS)

uspsv1: $(OBJECTS1)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

uspsv2: $(OBJECTS2)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

uspsv3: $(OBJECTS3)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

uspsv4: $(OBJECTS4)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
uspsv1.o: uspsv1.c p1fxns.h 
uspsv2.o: uspsv2.c p1fxns.h 
uspsv3.o: uspsv3.c p1fxns.h 
uspsv4.o: uspsv4.c p1fxns.h 
p1fxns.o: p1fxns.c p1fxns.h 


clean:
	rm -f $(PROGRAMS) $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4)

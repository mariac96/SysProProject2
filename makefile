
CC=gcc

CFLAGS=-g

all: Monitor\
		travelMonitor

Monitor: main.o functions.o citizens.o country.o hash_funcs.o hashing.o skiplist.o virus.o pipes.o requests.o bloom.o
	$(CC) main.o functions.o citizens.o country.o hash_funcs.o hashing.o skiplist.o virus.o pipes.o requests.o bloom.o -o Monitor -lm

travelMonitor: parent.o bloom.o pipes.o hash_funcs.o requests.o
	$(CC) parent.o bloom.o pipes.o hash_funcs.o requests.o -o travelMonitor -lm

parent.o: parent.c bloom.h pipes.h hash_funcs.h requests.h
	$(CC) $(CFLAGS) -c parent.c

bloom.o: bloom.c bloom.h hash_funcs.h
	$(CC) $(CFLAGS) -c bloom.c

requests.o: requests.c requests.h bloom.h
	$(CC) $(CFLAGS) -c requests.c

main.o: main.c functions.h hashing.h pipes.h requests.h
	$(CC) $(CFLAGS) -c main.c

functions.o: functions.c citizens.h skiplist.h virus.h pipes.h requests.h
	$(CC) $(CFLAGS) -c functions.c pipes.c

citizens.o: citizens.c citizens.h hashing.h country.h virus.h
	$(CC) $(CFLAGS) -c citizens.c

country.o: country.c country.h
	$(CC) $(CFLAGS) -c country.c

hash_funcs.o: hash_funcs.c hash_funcs.h
	$(CC) $(CFLAGS) -c hash_funcs.c

hashing.o: hashing.c hashing.h citizens.h virus.h country.h
	$(CC) $(CFLAGS) -c hashing.c

skiplist.o: skiplist.c skiplist.h citizens.h
	$(CC) $(CFLAGS) -c skiplist.c

virus.o: virus.c virus.h hash_funcs.h hashing.h skiplist.h -lm
	$(CC) $(CFLAGS) -c virus.c

pipes.o: pipes.c pipes.h
		$(CC) $(CFLAGS) -c pipes.c -lm

clean:
	rm -f *.o vaccineMonitor

#1ª Entrega Projeto Sistemas Operativos 
#93588 - Joao Lopes
#93598 - Matias Sommer


# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2019-20

CC   = gcc
LD   = gcc
CFLAGS =-Wall -std=gnu99 -I../
LDFLAGS=-lm -pthread

# A phony target is one that is not really the name of a file
# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean run

all: tecnicofs-nosync tecnicofs-mutex tecnicofs-rwlock

tecnicofs-nosync: lib/bst.o lib/hash.o fs.o main.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o tecnicofs-nosync lib/bst.o lib/hash.o fs.o main.o

tecnicofs-mutex: lib/bst.o lib/hash.o fsMutex.o mainMutex.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o tecnicofs-mutex lib/bst.o lib/hash.o fsMutex.o mainMutex.o

tecnicofs-rwlock: lib/bst.o lib/hash.o fsRwlock.o mainRwlock.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o tecnicofs-rwlock lib/bst.o lib/hash.o fsRwlock.o mainRwlock.o

lib/bst.o: lib/bst.c lib/bst.h
	$(CC) $(CFLAGS) -o lib/bst.o -c lib/bst.c

lib/hash.o: lib/hash.c lib/hash.h
	$(CC) $(CFLAGS) -o lib/hash.o -c lib/hash.c

fs.o: fs.c fs.h lib/bst.h lib/hash.o
	$(CC) $(CFLAGS) -o fs.o -c fs.c

fsMutex.o: fs.c fs.h lib/bst.h lib/hash.o
	$(CC) $(CFLAGS) -DMUTEX -o fsMutex.o -c fs.c

fsRwlock.o: fs.c fs.h lib/bst.h lib/hash.o
	$(CC) $(CFLAGS) -DRWLOCK -o fsRwlock.o -c fs.c


main.o: main.c fs.h lib/bst.h lib/hash.h
	$(CC) $(CFLAGS) -o main.o -c main.c

mainMutex.o: main.c fs.h lib/bst.h lib/hash.h
	$(CC) $(CFLAGS) -DMUTEX -o  mainMutex.o -c main.c

mainRwlock.o: main.c fs.h lib/bst.h lib/hash.h
	$(CC) $(CFLAGS) -DRWLOCK -o mainRwlock.o -c main.c

clean:
	@echo Cleaning...
	rm -f lib/*.o *.o tecnicofs

run: tecnicofs
	./tecnicofs

CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=bloomFilter

all: $(EXEC)

bloomFilter: ../../../src/srwsn/networklayer/BloomFilter.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o ../../../src/srwsn/networklayer/BloomFilter.o

mrproper: clean
	rm -rf $(EXEC)
	

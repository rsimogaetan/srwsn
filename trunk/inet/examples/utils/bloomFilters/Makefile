CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=bloomFilter

all: $(EXEC)

bloomFilter: BloomFilter.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o 

mrproper: clean
	rm -rf $(EXEC)
	

CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=tableRare

all: $(EXEC)

tableRare: ../../../src/srwsn/networklayer/TableRare.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o ../../../src/srwsn/networklayer/TableRare.o

mrproper: clean
	rm -rf $(EXEC)
	

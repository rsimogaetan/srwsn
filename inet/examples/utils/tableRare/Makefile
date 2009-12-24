CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=tableRare

all: $(EXEC)

tableRare: TableRare.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
	
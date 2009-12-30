CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=mactoid

all: $(EXEC)

mactoid: MACToID.o 
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o 

mrproper: clean
	rm -rf $(EXEC)
	

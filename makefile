CC=gcc
CFLAGS=-Isrc
DEPS=./src 

SRCDIR=./src
ODIR=$(SRCDIR)/obj
BINDIR=./bin

LIBS=-lpthread -lrt

$(ODIR)/%.o: $(SRCDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

queue: $(ODIR)/queue.o
	$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(BINDIR)/*

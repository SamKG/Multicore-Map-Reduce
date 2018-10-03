BINDIR=./bin
SRCDIR=./src
INCDIR=$(SRCDIR)/include
ODIR=$(SRCDIR)/obj

CC=gcc
CFLAGS=-I$(INCDIR)
DEPS=./src 

LIBS=-lpthread -lrt

$(ODIR)/%.o: $(SRCDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

queue: $(ODIR)/queue.o
	$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(BINDIR)/*

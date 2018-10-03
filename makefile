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


.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(BINDIR)/*

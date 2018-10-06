BINDIR=bin
SRCDIR=src
INCDIR=$(SRCDIR)/include
ODIR=$(SRCDIR)/obj

CC=gcc
CFLAGS=-I $(INCDIR)
DEPS=./src 

LIBS=-lpthread -lrt

$(ODIR)/%.o: $(SRCDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

.PHONY: clean
test: $(ODIR)/test.o $(ODIR)/processpool.o $(ODIR)/queue.o $(ODIR)/sharedmem.o
	$(CC) $^ -o $(BINDIR)/$@ $(CFLAGS) $(LIBS)
	
clean:
	rm -f $(ODIR)/*.o $(BINDIR)/*

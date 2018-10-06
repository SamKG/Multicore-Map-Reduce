BINDIR=bin
SRCDIR=src
INCDIR=$(SRCDIR)/include
ODIR=$(SRCDIR)/obj

CC=gcc
CFLAGS=-I $(INCDIR) -Wfatal-errors -g
DEPS=./src 

LIBS=-lpthread -lrt

$(ODIR)/%.o: $(SRCDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

mapred: $(ODIR)/mapred.o $(ODIR)/processpool.o $(ODIR)/queue.o $(ODIR)/sharedmem.o
	$(CC) $^ -o $(BINDIR)/$@ $(CFLAGS) $(LIBS)

test: $(ODIR)/test.o $(ODIR)/processpool.o $(ODIR)/queue.o $(ODIR)/sharedmem.o
	$(CC) $^ -o $(BINDIR)/$@ $(CFLAGS) $(LIBS)
	
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(BINDIR)/*

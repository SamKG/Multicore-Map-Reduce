BINDIR=bin
SRCDIR=src
INCDIR=$(SRCDIR)/include
ODIR=$(SRCDIR)/obj

KDIR=/lib/modules/`uname -r`/build
CC=gcc
CFLAGS= -Wfatal-errors -Wall -g -std=c11 -O2 -Wall -isystem /lib/modules/`uname -r`/build/include -D_KERNEL -DMODULE
DEPS=

LIBS=

$(ODIR)/%.o: $(SRCDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

cryptctl: $(SRCDIR)/cryptctl.c $(DEPS)
	make -C $(KDIR) M=$(PWD)/$(SRCDIR) modules
test: $(ODIR)/test.o $(DEPS)
	$(CC) $^ -o $(BINDIR)/$@ $(CFLAGS) $(LIBS)
	
.PHONY: clean
clean:
	make -C $(KDIR) M=$(PWD)/$(SRCDIR) clean 

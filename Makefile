#!/bin/sh
# ¿lo de arriba esta bien? ¿no deberia ser /bin/make -f $@ o algo asi?

CC = gcc
MAKE = make
EXE = aspectrum
VERSION = 0.1.6.2

default : aspectrum

# lugar donde se instalan las cosas y donde se buscan por defecto.
#DESTDIR = /usr
DESTDIR = /usr/local

# remember to uncomment the adecuated target for each arquitecture.

# target: Linux/GCC
#include Makefile.lnx

# Target: MS-DOS/DJGPP
#include Makefile.dos

# Target: Windows/mingw32
include Makefile.min


# esto no se pa que valdra. ( es pa debug )
DBGLIB = -lmss -g
DBGDEF = -DMSS -D_DEBUG_


all: aspectrum

aspectrum: dep $(AGUPLIB) $(objects) 
	$(CC) $(EXTRA) -o $(EXE) $(objects) $(AGUPLIB) $(LFLAGS) 

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(AGUPLIB):
	$(MAKE) -C $(AGUPDIR)

clean:
	-$(RM) *.o 
	-$(RM) $(EXE)$(EXT) 
	-$(RM) core
	-$(RM) deps
	-$(RM) contrib\getopt.o
	-$(RM) contrib/getopt.o
	-$(MAKE) -C $(AGUPDIR) clean

todo: clean all

install: aspectrum
	install -d $(DESTDIR)/bin
	install -d $(DESTDIR)/share/$(EXE)
	install -d $(DESTDIR)/share/doc/$(EXE)
	install -s $(EXE)$(EXT) $(DESTDIR)/bin/
	install font.dat font.fnt spectrum.rom $(DESTDIR)/share/$(EXE)/
	install doc/* $(DESTDIR)/share/doc/$(EXE)/

uninstall:
	rm -rf $(DESTDIR)/share/doc/$(EXE)
	rm -rf $(DESTDIR)/share/$(EXE) 
	rm $(DESTDIR)/bin/$(EXE)$(EXT)
#	-rmdir $(DESTDIR)/share/doc
#	-rmdir $(DESTDIR)/share
#	-rmdir $(DESTDIR)/bin
	
-include deps

# some unused stuff
debug:
	$(CC) $(EXTRA) -o $(EXE) $(FILES) $(ALL_FLAGS) $(DBGLIB) $(DBGDEF)

#.phony clean default all

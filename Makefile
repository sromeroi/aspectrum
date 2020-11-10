#!/bin/sh
# ¿lo de arriba esta bien? ¿no deberia ser /bin/make -f $@ o algo asi?

CC = gcc
EXE = aspectrum
VERSION = 0.1.7

default : aspectrum

# lugar donde se instalan las cosas y donde se buscan por defecto.
#DESTDIR = /usr
DESTDIR = /usr/local

# Autodetection de la arquitectura thanks to agup makefile
ifdef DJDIR
	include Makefile.dos
else
ifdef MINGDIR
	include Makefile.min
else
	include Makefile.lnx 
endif
endif


# esto no se pa que valdra. ( es pa debug )
DBGLIB = -lmss -g
DBGDEF = -DMSS -D_DEBUG_


all: aspectrum docs

aspectrum: dep $(AGUPLIB) $(objects)
	$(CC) $(EXTRA) -o $(EXE) $(objects) $(AGUPLIB) $(LFLAGS) 

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(AGUPLIB):
	$(MAKE) -C $(AGUPDIR) agup_lib

docs:
	$(MAKE) -C doc

clean:
	-$(RM) *.o 
	-$(RM) $(EXE)$(EXT) 
	-$(RM) core
	-$(RM) deps
	-$(RM) contrib/getopt.o
	-$(MAKE) -C $(AGUPDIR) clean
	-$(MAKE) -C doc clean

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

#.phony clean default all docs

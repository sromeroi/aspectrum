#!/bin/sh
# ¿lo de arriba esta bien?

CC = gcc
EXE = aspectrum
VERSION = 0.1.5

# remember to uncomment the adecuated dep target for each target.

# linux
DESTDATPATH = /usr/share/$(EXE)
LFLAGS = `allegro-config --libs`
CFLAGS = -Wall -O3 -mcpu=pentium `allegro-config --cflags` \
	-DVERSION=\"$(VERSION)\" -DDESTDAT=\"$(DESTDATPATH)\"
RM= rm

# windos (mingw32)
#LFLAGS = -lalleg 
#CFLAGS = -DVERSION=\"$(VERSION)\" -DNO_GETOPTLONG -Wall -O2 -mwindows
#RM= del

# esto no se pa que valdra. ( es pa debug )
DBGLIB = -lmss -g
DBGDEF = -DMSS -D_DEBUG_

# esto todabia no lo uso
DESTDOCPATH = /usr/share/doc/$(EXE)
DESTEXEPATH = /usr/bin

# fuentes y objetos para linux. 
files = v_alleg.c snaps.c graphics.c menu.c debugger.c main.c z80.c \
disasm.c 
objects=v_alleg.o snaps.o graphics.o menu.o debugger.o main.o z80.o \
disasm.o 

# objetos para windows las fuentes no hacen falta por que se usa makedeps.bat
#objects=v_alleg.o snaps.o graphics.o menu.o debugger.o main.o z80.o \
#disasm.o contrib/getopt.o

all: aspectrum

aspectrum: dep $(objects) 
	$(CC) $(EXTRA) -o $(EXE) $(objects) $(LFLAGS) 

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

#linux dep
dep:
	echo >deps
	for i in $(files) ; do $(CC) -MM $$i >>deps ; done

#dos dep
#dep:
#	makedeps.bat

clean:
	-$(RM) *.o 
	-$(RM) $(EXE) 
	-$(RM) deps

todo: clean all

-include deps

# some unused stuff
debug:
	$(CC) $(EXTRA) -o $(EXE) $(FILES) $(ALL_FLAGS) $(DBGLIB) $(DBGDEF)



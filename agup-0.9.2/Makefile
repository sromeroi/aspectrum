# This is some point between initial hackism, and preping for 'brella.

CC := gcc
CFLAGS := -W -Wall -O2
AR := ar
ARFLAGS := rucs

ifdef DJDIR
	EXLIBS := -lalleg
	EXE := .exe
else
ifdef MINGDIR
	EXLIBS := -lalleg
	EXE := .exe
else
	# Assume Unix.
	EXLIBS := `allegro-config --libs`
endif
endif

all: agup_lib examples

include rules.mk

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

EXAMP := examp$(EXE)

examples: $(EXAMP)

$(EXAMP): examp.o $(AGUP_LIB)
	$(CC) $(CFLAGS) -o $(EXAMP) $^ $(EXLIBS) 

clean:
ifdef DJDIR
	   -del $(EXAMP)
	   -del *.o
	   -del $(AGUP_LIB)
	   -del core
else
	   -rm -f $(EXAMP) examp.o $(AGUP_LIB) $(AGUP_OBJS) core
endif

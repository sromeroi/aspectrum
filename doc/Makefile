#!/bin/sh
# ¿lo de arriba esta bien? ¿no deberia ser /bin/make -f $@ o algo asi?


DOCS=Changelog.txt INSTALL.txt License.txt README.txt WORKING.txt AUTHORS.txt \
     CHANGES.txt NOT_WORKING.txt TODO.txt

# solo lo construimos en Win32
ifdef MINGDIR

RM=del
make-txt : $(DOCS)

$(DOCS): %.txt : %
	copy $< $<.txt
else
RM=rm -f
make-txt:
	-echo nothing to do
endif

clean:
	-$(RM) *.txt

 

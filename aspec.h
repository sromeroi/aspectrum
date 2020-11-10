typedef unsigned char u8;


// needed by debugger 
void screenRedraw_forZXDEB (void);
void keyboardHandler_forZXDEB (void);
// all includes needed 


#include "main.h"
#include "z80.h"

#ifndef CPP_COMPILATION
#include "macros.c"
#else
#include "macros.cpp"
#endif

#include "sound.h"
#include "v_alleg.h"
#include "snaps.h"		// da pawa!
#include "debugger.h"
#include "disasm.h"
#include "graphics.h"

extern Z80Regs spectrumZ80;


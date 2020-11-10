#ifndef _MAIN_H
#define _MAIN_H 1

#include "v_alleg.h"
#define GS_HAYMOUSE 0x01
#define GS_GUNSTICK 0x02
#define GS_INACTIVE 0x04

struct tipo_emuopt {
    char romfile[256] ;
    char snapfile[256] ;
    char tapefile[256] ;
    unsigned int gunstick ;
    BITMAP *raton_bmp ;
    char joytype ;
    char joy1key[5] ;
    } ;

struct tipo_hwopt {
	int port_ff;
	int TSTATES_PER_LINE;
	int TOP_BORDER_LINES;
	int BOTTOM_BORDER_LINES;
	int SCANLINES;	
	int tstate_border_left;
	int tstate_graphic_zone;
	int tstate_border_right;
	};

int emuMain( int argc, char *argv[] );
int Z80Initialization (void);

#endif  /* main.h */  

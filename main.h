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

#endif  /* main.h */  

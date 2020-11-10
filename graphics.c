#include "stdafx.h"
/*=====================================================================
  graphics.c -> This file includes all the graphical functions
                for the ASpectrum emulator.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 Copyright (c) 2000 Santiago Romero Iglesias.
 Email: sromero@escomposlinux.org
 ======================================================================*/

#ifdef _DEBUG_
#include <mss.h>
#endif

#include <stdio.h>
#include "aspec.h"
#include "graphics.h"
#include "z80.h"


extern void ExitEmulator( void );
extern byte *RAM;
extern Z80Regs spectrumZ80;
extern unsigned short int Pixeles[192];
extern unsigned short int Atributos[192];
extern int cycle;
//extern volatile int frame_counter;
//extern volatile int target_cycle;
//extern volatile int last_fps;
extern void target_incrementor();
extern void count_frames();

unsigned int colors[256];

/*-----------------------------------------------------------------
 Redraw the entire screen from the speccy's VRAM.
 It reads the from the 16384 memory address the bytes of the
 VideoRAM needed to rebuild the screen and the border.
------------------------------------------------------------------*/
void DisplayScreen( Z80Regs *regs )
{
  int y;
extern int v_border;
extern int v_res;
//I supous that this is a global flags that change n times by sec
// but I not have timer by now.
  static int f_flash=1,f_flash2=0;
  f_flash2=(f_flash2++)%20;
  if (f_flash2<10) f_flash=1; else f_flash=0;

  gacquire_bitmap();

// why pelotas gadquirimos y grelasamos el bitmap si estamos usando vscreen
//     y esta no se muestra???
// why pelotas no usamos vram para vscreen ¿somebody understand the doc?

  /* stupid-border-implementation (tm) by NoP/Compiler */
  /* using allegro hline (acelerated were abalible)    */

  for(y=0; y<v_border; y++ )
  {
        displayborderscanline(y);
        displayborderscanline(v_res-1-y);
  }
  
  for( y=0; y<192; y++ )
      displayscanline2(y,f_flash,regs);

  grelease_bitmap();
}

/* -------------------------------------------------------------------------
 *   Explico: usar 3 arrays y 1º llenarlos y luego leerlos es igual o mas lento
 *   que leer de la memoria directamente, por que al final, a los array se
 *   convierten en direccionamiento indirecto y un par de multiplicaciones
 *   asi que no hay razon para hacerlo dos veces 
 *
 *  dir_p explicao en pag. 274 del curso de codigo maquina de MH 
 * ¿ se pue optimizar mas?, yo digo que no X'DDD
 * -------------------------------------------------------------------------*/
void displayborderscanline(int y)
{
   ghline(0,y,319,spectrumZ80.BorderColor);
}

void displayscanline2( int y, int f_flash, Z80Regs *regs )
{
  int x,row,col,dir_p,dir_a,pixeles,tinta,papel,atributos;
  extern int v_border;

  row = y+v_border;    // 4 y 32 son los offset de la parte grafica de la pantalla.
  col = 32;     // 32+256+32=320  4+192+4=200  total 320x200 de resolucion.

  dir_p = 0x4000+((y & 0xC0)<<5) +((y & 0x7)<<8) +((y & 0x38)<<2);
  dir_a = 0x5800+(32*(y>>3));
 
  ghline(  0,row, 31,spectrumZ80.BorderColor);
  ghline(288,row,319,spectrumZ80.BorderColor);

  for(x=0; x<32; x++ )
  {
    pixeles = regs->RAM[dir_p++];
    atributos=regs->RAM[dir_a++];

    if ( ((atributos & 0x80)==0) || (f_flash==0) )
    {
      tinta=(atributos & 0x07) + ((atributos & 0x40)>>3);
      papel=(atributos & 0x78)>>3;
    } else {
      papel=(atributos & 0x07) + ((atributos & 0x40)>>3);
      tinta=(atributos & 0x78)>>3;
    }
	  
    gPutPixel(col++,row,( (pixeles & 0x80) ? tinta : papel ));	  
    gPutPixel(col++,row,( (pixeles & 0x40) ? tinta : papel ));	  
    gPutPixel(col++,row,( (pixeles & 0x20) ? tinta : papel ));	  
    gPutPixel(col++,row,( (pixeles & 0x10) ? tinta : papel ));	  
    gPutPixel(col++,row,( (pixeles & 0x08) ? tinta : papel ));	  
    gPutPixel(col++,row,( (pixeles & 0x04) ? tinta : papel ));	  
    gPutPixel(col++,row,( (pixeles & 0x02) ? tinta : papel ));	  
    gPutPixel(col++,row,( (pixeles & 0x01) ? tinta : papel ));	  
  }
}


/*-----------------------------------------------------------------
 Redraw a single scanline of screen from the speccy's VRAM.
------------------------------------------------------------------*/
void DisplayScanLine( int linea, Z80Regs *regs )
{
   gacquire_bitmap();
   displayscanline2 (linea,0,regs);
   grelease_bitmap();
}


/*-----------------------------------------------------------------
 Clear the screen with an specified colour (colores[] based).
------------------------------------------------------------------*/
void ClearScreen( char color )
{
   gclear_to_color(color);
}


/*-----------------------------------------------------------------
 void PutChar8x16(x, y, ascii, fuente, fg_color, bg_color );
 Writes the character (ascii) in colors fg=fg_color, bg=bg_color
 using the font (font) into the (x,y) screen coordinates.
------------------------------------------------------------------*/
void PutChar8x16( int x, int y, unsigned char ascii,
                  char *font , unsigned long fg_color, unsigned long bg_color )
{
   char height, car, charheight;
   int pos,f;

   height = 16;
   charheight = 8;
   pos = ( height*( ascii ));

   /* draw all the individual pixels of the character */
   for(f=0; f<charheight; f++)
   {
      car=font[pos+f];
      if( car & 0x80 ) { gPutPixel(x  , y+f, fg_color); }
      else             { gPutPixel(x  , y+f, bg_color); }
      if( car & 0x40 ) { gPutPixel(x+1, y+f, fg_color); }
      else             { gPutPixel(x+1, y+f, bg_color); }
      if( car & 0x20 ) { gPutPixel(x+2, y+f, fg_color); }
      else             { gPutPixel(x+2, y+f, bg_color); }
      if( car & 0x10 ) { gPutPixel(x+3, y+f, fg_color); }
      else             { gPutPixel(x+3, y+f, bg_color); }
      if( car & 0x08 ) { gPutPixel(x+4, y+f, fg_color); }
      else             { gPutPixel(x+4, y+f, bg_color); }
      if( car & 0x04 ) { gPutPixel(x+5, y+f, fg_color); }
      else             { gPutPixel(x+5, y+f, bg_color); }
      if( car & 0x02 ) { gPutPixel(x+6, y+f, fg_color); }
      else             { gPutPixel(x+6, y+f, bg_color); }
      if( car & 0x01 ) { gPutPixel(x+7, y+f, fg_color); }
      else             { gPutPixel(x+7, y+f, bg_color); }

   }

}

/*-----------------------------------------------------------------
 void GFXprintf( x, y, *sentence, *font, fg_color, bg_color, incr);
 Writes to the screen the string pointed by sentence at (x,y)
 using the specified colours and font increments (like -1).
------------------------------------------------------------------*/
void GFXprintf( int x, int y, char *sentence, char *font,
                char fg_color, char bg_color, char incr)
{
   unsigned long longi;
   unsigned long ink32, paper32;
   int f, pos, nx;

   ink32 = fg_color;
   paper32 = bg_color;
   x-=2; incr+=8;
   longi=strlen(sentence);

   /* write all the characters to screen */
   for(f=0, nx=x ;f<longi; f++, nx+=incr)
   {
      pos = x+(f<<3);
      PutChar8x16( nx,y,sentence[f],font,ink32,paper32);
   }

   //UpdateRect( x, y, nx-x, 16);
   gUpdateRect( x, y, nx-x, 16 );
   
}


/*-----------------------------------------------------------------
 void GFXprintf( x, y, *sentence, *font, fg_color, bg_color, incr);
 Writes to the screen the string pointed by sentence at (x,y)
 using the specified colours and font increments (like -1).
------------------------------------------------------------------*/
void GFXprintf_tovideo( int x, int y, char *sentence, char *font,
                char fg_color, char bg_color, char incr)
{
   unsigned long longi;
   unsigned long ink32, paper32;
   int f, pos, nx;

   ink32 = fg_color;
   paper32 = bg_color;
   x-=2; incr+=8;
   longi=strlen(sentence);

   /* write all the characters to screen */
   for(f=0, nx=x ;f<longi; f++, nx+=incr)
   {
      pos = x+(f<<3);
      PutChar8x16( nx,y,sentence[f],font,ink32,paper32);
   }
}
   
/*-----------------------------------------------------------------
 int GFXgets( x, y, *cadena, *font, fg_color, bg_color, max );
 This function reads a string from the keyboard (reads up to
 max-1 characters) allowing simple input commands (delete,
 enter, etc.).
------------------------------------------------------------------*/

int GFXgets( int x, int y, char *cadena, char *font,
             int fg_color, int bg_color, int max )
{
   char car, tecla=0;
   int offs=0;	

   /* Draw initial cursor. */
   PutChar8x16( x+(offs*8), y, '_', font, fg_color, bg_color );
   gUpdateRect( x+(offs*8), y, 8, 16);

   /* while the user does not press ENTER... */
   while( tecla != '\r' && tecla != '\n' )
   {
      tecla = readkey();

      if( (tecla >=' ' && tecla <= 125 ) && offs < max-2 )
      {
         cadena[offs] = tecla;
         cadena[offs+1] = '\0';

        // erase cursor
         if( offs < max-1 )
            PutChar8x16( x+(offs*8), y, '_', font, fg_color, bg_color );

        // draw pressed key
        PutChar8x16( x+(offs*8), y, tecla, font, fg_color, bg_color );

        if( offs < max-2 )
           PutChar8x16( x+((offs+1)*8), y, '_', font, fg_color, bg_color );

       // advance cursor
       if( offs < max-2 ) offs++;
     }

     // if the user presses BACKSPACE...
     else if( tecla == '\b' && offs > 0 )
     {
        PutChar8x16( x+(offs*8), y, ' ', font, bg_color, bg_color );
		offs--;
        car = cadena[offs];
        cadena[offs]='\0';
        PutChar8x16( x+(offs*8), y, '_', font, fg_color, bg_color );
     }

     gUpdateRect( x, y, max*8, 16 );
     
     // if the user presses ENTER, end edition
     if( ( tecla == '\n' || tecla =='\r' ) && offs > 0)
     {
        cadena[offs+1]='\0';
        return(offs);
     }

   }
   return 0;
}



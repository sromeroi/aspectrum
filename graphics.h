/*=====================================================================
  Graphics.h -> Header file for graphics.c

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

#ifndef GRAPHICS_H
#define GRAPHICS_H

unsigned int colors[256];

void InitGraphics( void );
//void fast_putpixel1(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel);
//void fast_putpixel2(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel);
//void fast_putpixel3(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel);
//void fast_putpixel4(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel);
//void (*fast_putpixel)(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel);
void DisplayScreen( Z80Regs *regs );
void DisplayScanLine( int linea, Z80Regs *regs );
void displayscanline2( int y, int f_flash, Z80Regs *regs );

/* Speccy colours */
/*
static RGB colores[16] = {
  {   0/4,   0/4,   0/4},
  {   0/4,   0/4, 205/4},
  { 205/4,   0/4,   0/4},
  { 205/4,   0/4, 205/4},
  {   0/4, 205/4,   0/4},
  {   0/4, 205/4, 205/4},
  { 205/4, 205/4,   0/4},
  { 212/4, 212/4, 212/4},
  {   0/4,   0/4,   0/4},
  {   0/4,   0/4, 255/4},
  { 255/4,   0/4,   0/4},
  { 255/4,   0/4, 255/4},
  {   0/4, 255/4,   0/4},
  {   0/4, 255/4, 255/4},
  { 255/4, 255/4,   0/4},
  { 255/4, 255/4, 255/4}
};
*/
//#define PutPixel(dst,x,y,color) _putpixel16(dst,x,y, colors[color] );

#define PutPixel(dst,x,y,color) (dst->line[y])[x]=colors[color]

//  (*fast_putpixel)((dst),(x),(y), color)
/* PutPixel for 2x2 drawing on the screen */
/*
#define PutPixel(dst,x,y,color)                    \
  (*fast_putpixel)((dst),((x)<<1),((y)<<1), color);  \
  (*fast_putpixel)((dst),((x)<<1)+1,((y)<<1), color)
*/

//#define UpdateRect(x,y,w,h) blit( vscreen, screen, x, y, x, y, w, h );

#endif

/*=====================================================================
	"platform dependant" file
	to port emu to other platforms / libraries , just "translate" all the
	funcions in this file to the target platform / library.
=====================================================================*/

#ifdef _DEBUG_
#include <mss.h>
#endif

// to fix:
// macro END_OF_MAIN and END_OF_FUNCTION

#include <stdio.h>
#include <string.h>
#include "z80.h"
#include "snaps.h"

#include <allegro.h>
#include "monofnt.h"
#include "graphics.h"
#include "debugger.h"
#include "v_alleg.h"
#include "main.h"


extern void target_incrementor( void );
extern void count_frames( void );

extern volatile int frame_counter;
extern volatile int target_cycle;
extern volatile int last_fps;

// generic key handler ( = key using allegro)
volatile char *gkey;
// allegro virtual screen
BITMAP *vscreen;
extern Z80Regs spectrumZ80;


/*-----------------------------------------------------------------
 ExitEmulator( void );
 This function is attached to exit() and called when the program
 ends, it must finish threads, SDL and other library/mem stuff.
------------------------------------------------------------------*/
// function called when you exit from the emulator (unitialization here)
void ExitEmulator( void )
{
   if( vscreen != NULL )
	  destroy_bitmap(vscreen);
   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   allegro_exit();
//   fcloseall();
   free(spectrumZ80.RAM);                                 /* free RAM */
   exit(0);
}


// clear the visible screen
void gclear (void)
{
	clear(screen);
}


// dumps memory from the virtual screen to the visible screen
// it CAN be a flip_screen   ( but you'll have to swap the screen and vscreen pointers)
void dumpVirtualToScreen (void)
{
		blit( vscreen, screen, 0, 0, 0, 0, 320, 200 );
}

// draws text in the virtual screen
void gtextout ( char *b, int x, int y, int color )
{
		textout( vscreen, font, b, x,y, color );
}

// draws text in the virtual screen with no background
void gtextoutb ( char *b, int x, int y, int color, FONT *tfont )
{
      text_mode( -1 );
		textout( vscreen, tfont, b, x,y, color );
      text_mode( 0 );
}

// sets the "index" colour to the "p" colour in the active palette.
// if you're not under a palette-based video mode this function can
// be empty, but the PutPixel function will have to "translate" from speccy
// colours (0-15) to real colors
void gset_color (int index,gRGB *p)
{
		set_color (index,(RGB *)p);
}

// put platform specific initialization code here
void init_wrapper (void)
{
		gkey=key;
}

// puts a pixel (col) in the virtual screen. look gset_color for more details.
// col is a "speccy color"
void gPutPixel (int x, int y, int col )
{
		PutPixel (vscreen,x,y,col);
}


// generic acquire bitmap function
void gacquire_bitmap(void)
{
		acquire_bitmap (vscreen);
}

// generic release bitmap function
void grelease_bitmap(void)
{
		release_bitmap(vscreen);
}



/*----------------------------------------------------------------
  Calls initialization functions, initializes system. This function must:
   * Initialize graphic system (create virtual screen if needed, init gfx, ...)
   * Initialize keyboard system
   * Initialize timers:
	  - Function count_frames must be called once every second
	  - Function target_incrementor must be called 50 times a second
----------------------------------------------------------------*/
void InitSystem (void)
{
		// inits everything (for allegro)
		InitGraphics();
}

/*-----------------------------------------------------------------
 InitGraphics( video_flags );
 This function inits all the graphics and SDL stuff...

 Thanks to Nathan Strong and Sam Lantinga for such an example
 (stars.c). I took a look at stars.c by Nathan Strong (ported
 to SDL by Sam Lantinga) and learnt how to use SDL on my small
 projects (and the big ones :-).
------------------------------------------------------------------*/
void InitGraphics( void )
{
   int i, depth;
   PALETTE specpal;

   allegro_init();
   install_keyboard();
   install_timer();

   set_color_depth(8);
   if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0) != 0)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error setting graphics mode\n%s\n", allegro_error);
      ExitEmulator();
   }

   // if we're on windowed mode, update color conversion tables...

   if( (depth=desktop_color_depth()) > 8 )
   {
      for (i = 0; i <16; i++)
        colors[i] = makecol( colores[i].r*4, colores[i].g*4, colores[i].b*4 );
   
//   printf("Vale, me equivoque y soy un estupido, DIMELO en aleasoft@geocites.com\n");
   }
   else
   {
      for (i = 0; i <16; i++)
      {
         colors[i] = i;
		 gset_color(i, &colores[i] );
      }
   }
/*
      for (i = 0; i <16; i++)
      {
		 specpal[i].r=colores[i].r*4;
		 specpal[i].g=colores[i].g*4;
		 specpal[i].b=colores[i].b*4;
      }
set_pallete(specpal);   
*/

   LOCK_VARIABLE(last_fps);
   LOCK_VARIABLE(frame_counter);
   LOCK_VARIABLE(target_cycle);
//   LOCK_FUNCTION(count_frames);
//   LOCK_FUNCTION(target_incrementor);
   install_int_ex(count_frames, BPS_TO_TIMER(1));
   install_int_ex(target_incrementor, BPS_TO_TIMER(50));
   last_fps = frame_counter = target_cycle = 0;

   vscreen = create_bitmap(320, 200);
   if( vscreen == NULL )
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Not enough memory: couldn't create vscreen.\n%s\n",
                       allegro_error);
      ExitEmulator();
   }
   printf("Working at %d bpp\n", bitmap_color_depth(screen) );
   clear(vscreen);
}


// draw filled rectangles
void gbox( int x1, int y1, int x2, int y2, int color )
{
   rectfill( vscreen, x1, y1, x2, y2, color );
} 

// draw rectangles
void grectangle( int x1, int y1, int x2, int y2, int color )
{
   rect( vscreen, x1, y1, x2, y2, color );
}
// draw hlines
void ghline(int x1, int y1, int x2, int col)
{
	int x;
    for(x=x1; x<=x2; x++ )
       PutPixel(vscreen, x, y1, col );
//	hline(vscreen,x1,y1,x2,col);
}


// cls with specified color
void gclear_to_color(int color)
{
	clear_to_color (vscreen,color);
}

// transfers from (x,y) to (x+w,y+h) from the virtual screen to the visible screen
void gUpdateRect( int x, int y, int w,int h)
{
		blit (vscreen,screen,x,y,x,y,w,h);
}

#if ALLEGRO_WIP_VERSION >= 38
#define CLEARBITMAP clear_bitmap
#else
#define CLEARBITMAP clear
#endif

void v_initmouse(void)
{
   extern struct tipo_emuopt emuopt;
   int color_b,color_n;
	
   if (install_mouse()!=-1) 
   {
   	emuopt.gunstick|=GS_HAYMOUSE;

	// dibujar puntero 
   	emuopt.raton_bmp=create_bitmap(16,16);
	CLEARBITMAP(emuopt.raton_bmp);
	color_b=makecol(255,255,255);
	color_n=makecol(0,0,0);
	circle(emuopt.raton_bmp,8,8,7,color_n);		
	circle(emuopt.raton_bmp,8,8,6,color_b);		
	putpixel(emuopt.raton_bmp,8,8,color_b);	
	putpixel(emuopt.raton_bmp,7,8,color_n);	
	putpixel(emuopt.raton_bmp,9,8,color_n);	
	putpixel(emuopt.raton_bmp,8,7,color_n);	
	putpixel(emuopt.raton_bmp,8,9,color_n);	

   	if ((emuopt.gunstick & GS_GUNSTICK)!=0) 
   	{
 		set_mouse_sprite(emuopt.raton_bmp);
   		set_mouse_sprite_focus(8,8);
   		show_mouse(screen);
	}
   }

}

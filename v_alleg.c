#include "stdafx.h"
/*=====================================================================
 *	"platform dependant" file, used to port the source code to
 *      other platforms / libraries , just "translate" all the
 *	funcions in this file to the target platform / library.
 *
 *	To be a really portable emulator none of de *.[ch] must have a
 *      #include <allegro.h> or any related to allegro.
 *      so be posible create a v_alleg.[ch] that depend of sdl p.e. and
 *      not of allegro.
 *=====================================================================*/

#ifdef _DEBUG_
#include <mss.h>
#endif

// COMMENTS:
// to fix:
// macro END_OF_MAIN and END_OF_FUNCTION

#include <stdio.h>
#include <string.h>

#include <allegro5/allegro5.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_native_dialog.h>

#include "v_alleg.h"
#include "z80.h"
//#include "snaps.h"
//#include "main.h"
//#include "monofnt.h"     //FIXME check if need 
//#include "graphics.h"
//#include "debugger.h"
#include "main.h"
//#include "sound.h"
//#include "mem.h"
#include "menu.h"

#ifdef I_HAVE_AGUP
#include <agup.h>
//#include <agtk.h>
//#include <aphoton.h>
//#include <awin95.h>
//#include <aase.h>
#endif

//extern void target_incrementor (void);
//extern void count_frames (void);

//extern volatile int frame_counter;
//extern volatile int target_cycle;
//extern volatile int last_fps;
int fps;
int framecounter;
ALLEGRO_TIMER *emuclock;

// generic key handler ( = key using allegro)
//volatile char *gkey;
// allegro virtual screen
ALLEGRO_BITMAP *vscreen; // This is the screen bitmap
ALLEGRO_BITMAP *screen;  // This is the work screen to put things
ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP *mouseicon;
ALLEGRO_MOUSE_STATE mousestatus;
ALLEGRO_FONT *font;
//ALLEGRO_MENU *menuprinc;

extern int v_res;

ALLEGRO_VOICE *voces;
ALLEGRO_MIXER *mixer;

extern Z80Regs spectrumZ80;
unsigned int colors[256];
//static DATAFILE *datafile = NULL;

#define NUMCOLORSPALETE 17
ALLEGRO_COLOR paleta[NUMCOLORSPALETE];  //no idea why have a red color on pos #17

static int colores[NUMCOLORSPALETE][3] = { 
  {  0 ,   0 ,   0 }, 
  {  0 ,   0 , 192 }, 
  {192 ,   0 ,   0 }, 
  {192 ,   0 , 192 }, 
  {  0 , 192 ,   0 }, 
  {  0 , 192 , 192 }, 
  {192 , 192 ,   0 }, 
  {192 , 192 , 192 }, 
  {  0 ,   0 ,   0 }, 
  {  0 ,   0 , 255 }, 
  {255 ,   0 ,   0 }, 
  {255 ,   0 , 255 }, 
  {  0 , 255 ,   0 }, 
  {  0 , 255 , 255 }, 
  {255 , 255 , 0   }, 
  {255 , 255 , 255 },
  {255 ,   0 ,   0 } 
/*
  Old colour palette:
  
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
*/ 
};

/*-----------------------------------------------------------------
 ExitEmulator( void );
 This function is attached to exit() and called when the program
 ends, it must finish threads, SDL and other library/mem stuff.
------------------------------------------------------------------*/
// function called when you exit from the emulator (unitialization here)
void ExitEmulator (void){
  //unload_datafile (datafile);
  if (screen != NULL)
    al_destroy_bitmap (screen);
//PENDING  set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
#ifdef I_HAVE_AGUP
  agup_shutdown ();
#endif

  end_spectrum();
  al_uninstall_system();
//  fcloseall();
  exit (0);
}

// clear the visible screen
void gclear (void){
  al_clear_to_color(paleta[0]);
}


// dumps memory from the virtual screen to the visible screen
void dumpVirtualToScreen(void) {
  char b[64];
  v_scaremouse ();
  sprintf (b, "%d ", fps);
  //ASprintf(b); ASprintf("\n");
  al_hold_bitmap_drawing(true);
  al_draw_text(font, paleta[0], 4, v_res-17, 0, b);
  al_draw_text(font, paleta[0], 5, v_res-16, 0, b);
  al_draw_text(font, paleta[15],4, v_res-16, 0, b);
  al_hold_bitmap_drawing(false);
	//gtextout (b, 4, v_res - 17, 0);
	//gtextout (b, 5, v_res - 16, 0);
	//gtextout (b, 4, v_res - 16, 15);
  al_set_target_bitmap(vscreen);
//  al_draw_bitmap(screen,0,0,0);
  al_draw_scaled_bitmap(screen,0,0,320,240,0,0,640,480,0);
  al_flip_display();
  al_set_target_bitmap(screen);
  v_unscaremouse ();
}

// draws text in the virtual screen
void gtextout (char *b, int x, int y, int color){
//  textout (vscreen, font, b, x, y, color);
// FIXME perhaps use al_get_text_dimensions to delete background???
  al_draw_text(font,paleta[color],x,y,ALLEGRO_ALIGN_LEFT,b);
}

// draws text in the virtual screen with no background
void gtextoutb (char *b, int x, int y, int color, ALLEGRO_FONT * tfont){
  al_draw_text(font,paleta[color],x,y,ALLEGRO_ALIGN_LEFT,b);
//  text_mode (-1);
//  textout (vscreen, tfont, b, x, y, color);
//  text_mode (0);
}

// sets the "index" colour to the "p" colour in the active palette.
// if you're not under a palette-based video mode this function can
// be empty, but the PutPixel function will have to "translate" from speccy
// colours (0-15) to real colors
//void gset_color (int index, gRGB * p){
//  set_color (index, (RGB *) p);
//}

// put platform specific initialization code here
//void init_wrapper (void){
// gkey = key;
//}

// puts a pixel (col) in the virtual screen. look gset_color for more details.
// col is a "speccy color"
void gPutPixel (int x, int y, int col){
  al_put_pixel(x, y, paleta[col]);
}


// generic acquire bitmap function
void gacquire_bitmap (void){
  //acquire_bitmap (vscreen);
  al_set_target_bitmap(screen);
}

// generic release bitmap function
void grelease_bitmap (void){
//  release_bitmap (vscreen);
}



void InitSystem (void){
//ASprintf("antes allegro \n");
//  allegro_init ();
  al_init();
//ASprintf("antes keyboards \n");
  al_install_keyboard();
  al_install_mouse();
  al_init_native_dialog_addon();
// inits everything (for allegro)
//ASprintf("antes graficos \n");
  InitGraphics();
//ASprintf("antes de menu\n");
  MainMenu();
//ASprintf("despues de graficos \n");
  // Timer inicialization for sync and FPS calculus  
//ASprintf("antes timer \n");
//  install_timer ();
//  LOCK_VARIABLE (last_fps);
//  LOCK_VARIABLE (frame_counter);
//  LOCK_VARIABLE (target_cycle);
//   LOCK_FUNCTION(count_frames);
//   LOCK_FUNCTION(target_incrementor);
//PENDING  install_int_ex (count_frames, BPS_TO_TIMER (1));
//PENDING  install_int_ex (target_incrementor, BPS_TO_TIMER (50));
//last_fps = frame_counter = target_cycle = 0;
emuclock = al_create_timer(1.0/50.0);
al_start_timer(emuclock);


  // Inizialite the GUI
#ifdef I_HAVE_AGUP
  /* estaria bien crear 16 tonos de gris en una parte de la paleta no 
     usada para mejorar como se ven los engines 

     for (i = 0; i <16; i++)
     if( (depth=desktop_color_depth()) > 8 )
     {
     colors[250+i] = makecol( i*16, i*16, i*16 );
     } else {
     gRGB colorin;
     colorin.r=i*16;
     colorin.g=i*16;
     colorin.b=i*16;
     gset_color(i,&colorin);
     }
   */
  //init de theme GUI
  agup_init (aphoton_theme);
  gui_fg_color = agup_fg_color;
  gui_bg_color = agup_bg_color;
  gui_shadow_box_proc = d_agup_shadow_box_proc;
  gui_button_proc = d_agup_button_proc;
  gui_edit_proc = d_agup_edit_proc;
  gui_text_list_proc = d_agup_text_list_proc;
#endif

//ASprintf("antes sonido \n"); 
  gInitSound ();
//ASprintf("despues sonido\n");
  MainMenu();
}

int v_framecheck(void){
  int n;
  int ret=0;
  framecounter++;
  n=al_get_timer_count(emuclock);
  while (framecounter>n) {
    n=al_get_timer_count(emuclock);
  }
  if (framecounter<n) ret=1;  //if we are 2 delayed, frameskip.
  if (n>=50){
    fps=framecounter;
    framecounter=0;
    al_set_timer_count(emuclock,0);
  }
  return ret;
}

void InitGraphics (void){
  int i, depth;
//  FILE *archivo;
//  PALETTE specpal;
  extern int v_border;
  al_init_primitives_addon();
  al_init_font_addon();
  al_init_ttf_addon();
  
  al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_GTK_TOPLEVEL );
  al_set_new_display_option(ALLEGRO_COLOR_SIZE,16,ALLEGRO_SUGGEST);
  display = al_create_display(640,480);
  al_set_window_title (display, "ASpectrum emulator");
//ASprintf("creando vscreen\n");
  vscreen = al_get_backbuffer(display);
  /*
  set_color_depth (8);
  if (set_gfx_mode (GFX_AUTODETECT, 320, v_res, 0, 0) != 0)
    {
      if (set_gfx_mode (GFX_SAFE, 320, v_res, 0, 0) != 0)
	{
	  set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
	  allegro_message ("Error setting graphics mode\n%s\n",
			   allegro_error);
	  ExitEmulator ();
	}
    }
  */
  
//  ASprintf("se pedia %i y se obtubo %i\n",v_res,SCREEN_H);
// FIXME screeen must be 512x384, the real spectrum resolution, and then cut & copy to vscreen when need.
  //v_res = al_get_display_height(display);
  v_res=240;
  v_border = (v_res - 192) / 2;
  al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);  //optimized for pixel-acess
  screen=al_create_bitmap(320,240);
  al_set_target_bitmap(screen);
  // if we're on windowed mode, update color conversion tables...
/*
  if ((depth = desktop_color_depth ()) > 8){
      ASprintf("desktop_color_depth y bitmap_color_depth devuelve diferente valor ?????\n");
      for (i = 0; i < 16; i++)
	colors[i] =
	  makecol (colores[i].r * 4, colores[i].g * 4, colores[i].b * 4);
  } else {
      ASprintf("Vale, al final solo 8bit por pixel\n");
      for (i = 0; i < 16; i++)
	    {
	      colors[i] = i;
	      gset_color (i, &colores[i]);
	    }
  }
*/
/*
      for (i = 0; i <16; i++)
      {
		 specpal[i].r=colores[i].r*4;
		 specpal[i].g=colores[i].g*4;
		 specpal[i].b=colores[i].b*4;
      }
   set_pallete(specpal);   
*/
  for(i=0;i<NUMCOLORSPALETE;i++){
    paleta[i]=al_map_rgb(colores[i][0],colores[i][1],colores[i][2]);
  }

  //datafile = load_datafile (find_file("font.dat"));
  //font = datafile[0].dat;
  font = al_create_builtin_font();
  /*
  const char *font_file = "./font.ttf"; 
  font = al_load_font(font_file,8,0);
  if (!font) {
    printf("Error al cargar tipo de letra\n");
    exit (1);
  } 
  */
/*
  if (vscreen == NULL)
    {
      set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
      allegro_message ("Not enough memory: couldn't create vscreen.\n%s\n",
		       allegro_error);
      ExitEmulator ();
    }
  ASprintf("Working at %d bpp\n", bitmap_color_depth (screen));
  clear (vscreen);
*/
ASprintf("End of InitGraphics()\n");
}


// draw filled rectangles
void gbox (int x1, int y1, int x2, int y2, int color){
//  rectfill (vscreen, x1, y1, x2, y2, color);
  al_draw_filled_rectangle(x1, y1, x2, y2, paleta[color]);
}

// draw rectangles
void grectangle (int x1, int y1, int x2, int y2, int color){
//  rect (vscreen, x1, y1, x2, y2, color);
al_draw_rectangle(x1,y1,x2,y2,paleta[color],0);
}

// draw hlines
void ghline (int x1, int y1, int x2, int col){
  int x;
  for (x = x1; x <= x2; x++)
//    PutPixel (vscreen, x, y1, col);
al_draw_pixel(x,y1,paleta[col]);
//      hline(vscreen,x1,y1,x2,col);
}


// cls with specified color
void gclear_to_color (int color){
  al_clear_to_color (paleta[color]);
}

// transfers from (x,y) to (x+w,y+h) from the virtual screen to the visible screen
void gUpdateRect (int x, int y, int w, int h){
  al_set_target_bitmap(vscreen);
  al_draw_bitmap_region(screen, x, y,  w, h, x, y,0);
  al_set_target_bitmap(screen);
}


ALLEGRO_MOUSE_CURSOR *mousecursor;
void v_initmouse (void) {
  extern tipo_emuopt emuopt;
  ALLEGRO_COLOR color_b, color_n;

  if (al_install_mouse ()){
      emuopt.gunstick |= GS_HAYMOUSE;

      // dibujar puntero 
      mouseicon = al_create_bitmap (16, 16);
       al_set_target_bitmap(mouseicon);
      al_clear_to_color (al_map_rgb (255, 255, 255));
      color_b = al_map_rgb (255, 255, 255);
      color_n = al_map_rgb (0, 0, 0);
      al_draw_circle (8, 8, 7, color_n,0);
      al_draw_circle (8, 8, 6, color_b,0);
      al_put_pixel (8, 8, color_b);
      al_put_pixel (7, 8, color_n);
      al_put_pixel (9, 8, color_n);
      al_put_pixel (8, 7, color_n);
      al_put_pixel (8, 9, color_n);

      if ((emuopt.gunstick & GS_GUNSTICK) != 0) {
          mousecursor = al_create_mouse_cursor(mouseicon,8,8);
          //set_mouse_sprite (emuopt.raton_bmp);
          //set_mouse_sprite_focus (8, 8);
          al_show_mouse_cursor (display);
      }
  }

}
/*
int galert (const char *s1, const char *s2, const char *s3, const char *b1,
	const char *b2, int c1, int c2) {
  int a;
//    gui_fg_color = 0;
//    gui_bg_color = 7;
//PENDING  a = alert (s1, s2, s3, b1, b2, c1, c2);
  return a;
}*/



/*-------------------------------------------------------------------
 Keyboard Routines
 Move from main.c to v_alleg.c due portability, is much depend of
 allegro and some architectures hasn't keyboard at all :-)
 -------------------------------------------------------------------*/
ALLEGRO_KEYBOARD_STATE estadoteclas;
/* al principio no habia teclas pulsadas... */
int fila[5][5];
void init_keyboard(void){
  fila[1][1] = fila[1][2] = fila[2][2] = fila[3][2] = fila[4][2] =
    fila[4][1] = fila[3][1] = fila[2][1] = 0xFF;
}

/*-----------------------------------------------------------------
 UpdateKeyboard( void );
 Updates the keyboard variables used on the return of IN function.
------------------------------------------------------------------*/
void UpdateKeyboard (void){
/*=== This adds the row/column/data value for each key on spectrum kerb ===*/
#define NUM_KEYB_KEYS 256
  enum SpecKeys
  {
    SPECKEY_0, SPECKEY_1, SPECKEY_2, SPECKEY_3, SPECKEY_4, SPECKEY_5,
    SPECKEY_6, SPECKEY_7, SPECKEY_8, SPECKEY_9, SPECKEY_A, SPECKEY_B,
    SPECKEY_C, SPECKEY_D, SPECKEY_E, SPECKEY_F, SPECKEY_G, SPECKEY_H,
    SPECKEY_I, SPECKEY_J, SPECKEY_K, SPECKEY_L, SPECKEY_M, SPECKEY_N,
    SPECKEY_O, SPECKEY_P, SPECKEY_Q, SPECKEY_R, SPECKEY_S, SPECKEY_T,
    SPECKEY_U, SPECKEY_V, SPECKEY_W, SPECKEY_X, SPECKEY_Y, SPECKEY_Z,
    SPECKEY_SPACE, SPECKEY_ENTER,
    SPECKEY_SHIFT, SPECKEY_ALT, SPECKEY_CTRL
  };
// UNUSED ??
//  static unsigned char teclas_fila[NUM_KEYB_KEYS][3] = {
//    {1, 2, 0xFE}, /* 0 */ {1, 1, 0xFE}, /* 1 */ {1, 1, 0xFD},	/* 2 */
//    {1, 1, 0xFB}, /* 3 */ {1, 1, 0xF7}, /* 4 */ {1, 1, 0xEF},	/* 5 */
//    {1, 2, 0xEF}, /* 6 */ {1, 2, 0xF7}, /* 7 */ {1, 2, 0xFB},	/* 8 */
//    {1, 2, 0xFD},		/* 9 */
//    {3, 1, 0xFE}, /* a */ {4, 2, 0xEF}, /* b */ {4, 1, 0xF7},	/* c */
//    {3, 1, 0xFB}, /* d */ {2, 1, 0xFB}, /* e */ {3, 1, 0xF7},	/* f */
//    {3, 1, 0xEF}, /* g */ {3, 2, 0xEF}, /* h */ {2, 2, 0xFB},	/* i */
//    {3, 2, 0xF7}, /* j */ {3, 2, 0xFB}, /* k */ {3, 2, 0xFD},	/* l */
//    {4, 2, 0xFB}, /* m */ {4, 2, 0xF7}, /* n */ {2, 2, 0xFD},	/* o */
//    {2, 2, 0xFE}, /* p */ {2, 1, 0xFE}, /* q */ {2, 1, 0xF7},	/* r */
//    {3, 1, 0xFD}, /* s */ {2, 1, 0xEF}, /* t */ {2, 2, 0xF7},	/* u */
//    {4, 1, 0xEF}, /* v */ {2, 1, 0xFD}, /* w */ {4, 1, 0xFB},	/* x */
//    {2, 2, 0xEF}, /* y */ {4, 1, 0xFD},	/* z */
//    {4, 2, 0xFE}, /*SPACE*/
//      {3, 2, 0xFE}, /*ENTER*/
//      {4, 1, 0xFE}, /*RSHIFT*/ {4, 2, 0xFD}, /*ALT*/ {1, 2, 0xEF}, /*CTRL*/
//  };


  /* reset the spectrum row and column keyboard signals */

  fila[1][1] = fila[1][2] = fila[2][2] = fila[3][2] =
    fila[4][2] = fila[4][1] = fila[3][1] = fila[2][1] = 0xFF;


  /* change row and column signals according to pressed key */
  al_get_keyboard_state(&estadoteclas);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_Z))
    fila[4][1] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_X))
    fila[4][1] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_C))
    fila[4][1] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_V))
    fila[4][1] &= (0xEF);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_RSHIFT) || al_key_down(&estadoteclas,ALLEGRO_KEY_LSHIFT))
    fila[4][1] &= (0xFE);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_A))
    fila[3][1] &= (0xFE);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_S))
    fila[3][1] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_D))
    fila[3][1] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_F))
    fila[3][1] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_G))
    fila[3][1] &= (0xEF);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_Q))
    fila[2][1] &= (0xFE);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_W))
    fila[2][1] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_E))
    fila[2][1] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_R))
    fila[2][1] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_T))
    fila[2][1] &= (0xEF);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_1))
    fila[1][1] &= (0xFE);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_2))
    fila[1][1] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_3))
    fila[1][1] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_4))
    fila[1][1] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_5))
    fila[1][1] &= (0xEF);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_0))
    fila[1][2] &= (0xFE);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_9))
    fila[1][2] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_8))
    fila[1][2] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_7))
    fila[1][2] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_6))
    fila[1][2] &= (0xEF);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_P))
    fila[2][2] &= (0xFE);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_O))
    fila[2][2] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_I))
    fila[2][2] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_U))
    fila[2][2] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_Y))
    fila[2][2] &= (0xEF);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_ENTER))
    fila[3][2] &= (0xFE);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_L))
    fila[3][2] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_K))
    fila[3][2] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_J))
    fila[3][2] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_H))
    fila[3][2] &= (0xEF);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_SPACE))
    fila[4][2] &= (0xFE);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_ALT) || al_key_down(&estadoteclas,ALLEGRO_KEY_ALT))
    fila[4][2] &= (0xFD);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_M))
    fila[4][2] &= (0xFB);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_N))
    fila[4][2] &= (0xF7);
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_B))
    fila[4][2] &= (0xEF);

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_BACKSPACE))
    {
      fila[4][1] &= (0xFE);
      fila[1][2] &= (0xFE);
    }
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_TAB))
    {
      fila[4][1] &= (0xFE);
      fila[4][2] &= (0xFD);
    }
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_CAPSLOCK))
	{
		fila[1][1] &= (0xFD);
		fila[4][1] &= (0xFE);
	}

  if (al_key_down(&estadoteclas,ALLEGRO_KEY_UP))
	{
		fila[1][2] &= (0xF7);
		fila[4][1] &= (0xFE);
	}	  
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_DOWN))
	{
		fila[1][2] &= (0xEF);
		fila[4][1] &= (0xFE);
	}	  
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_LEFT))
	{
		fila[1][1] &= (0xEF);
		fila[4][1] &= (0xFE);
	}	  
  if (al_key_down(&estadoteclas,ALLEGRO_KEY_RIGHT))
	{
		fila[1][2] &= (0xFB);
		fila[4][1] &= (0xFE);
	}	  
  

	
  /* emulate SINCLAIR JOYSTICK 1 using cursor pad and Ctrl :)
   *
   * One should replace those lines for:
   * 
   *  int cursor_up, cursor_down, cursor_left, cursor_right;
   *  if( key[KEY_UP] )  fila[X][X] &= (code_cursor_up);
   *  etc...
   * 
   * This would allow to emulate OPQA<SPACE> or INTERF1 or 2
   * or define custom keys for the cursor of the pc.
   */
/* by now the cursors are used as cursors, mainly for 128K menues.
#define CUP    SPECKEY_9
#define CDOWN  SPECKEY_8
#define CRIGHT SPECKEY_7
#define CLEFT  SPECKEY_6
#define FIRE   SPECKEY_0
#define filas teclas_fila

  if (gkey[KEY_UP])
    fila[filas[CUP][0]][filas[CUP][1]] &= (filas[CUP][2]);
  if (gkey[KEY_DOWN])
    fila[filas[CDOWN][0]][filas[CDOWN][1]] &= (filas[CDOWN][2]);
  if (gkey[KEY_RIGHT])
    fila[filas[CRIGHT][0]][filas[CRIGHT][1]] &= (filas[CRIGHT][2]);
  if (gkey[KEY_LEFT])
    fila[filas[CLEFT][0]][filas[CLEFT][1]] &= (filas[CLEFT][2]);
  if (gkey[KEY_RCONTROL])
    fila[filas[FIRE][0]][filas[FIRE][1]] &= (filas[FIRE][2]);

#undef filas
*/
}

void gupdatekeys(void){ 
  al_get_keyboard_state(&estadoteclas);
}
bool gkeypressed(int tecla){
  return al_key_down(&estadoteclas, tecla);
}

void v_cleankeys(void){
  al_clear_keyboard_state(display);
  al_get_keyboard_state(&estadoteclas);
}

#define  SOUND_BY_STREAM
#ifdef SOUND_BY_STREAM
// ******************** GOOD WAY **********************
// audiostreams

int gSoundInited = 0; 
ALLEGRO_AUDIO_STREAM *audioStream;

void gInitSound (void){
  initSoundLog ();

// PENDING reserve_voices (3, -1);
//  if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
  if (!al_install_audio ()){
      ASprintf("Sound init error\n");
      return;
  }
  voces = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16,ALLEGRO_CHANNEL_CONF_2);
  if (!voces) {
      ASprintf("Could not create ALLEGRO_VOICE.\n");
      return;
  }
  //if (!al_reserve_samples(1)) {  //check, must be 7 for 128K? beeper+3sound chanel + 3 noise chanel??
  //    ASprintf("Sound sample error\n");
  //    return;
  //  }
  //mixer = al_get_default_mixer();
  mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
  if (!al_attach_mixer_to_voice(mixer, voces)) {
      ASprintf("error atando mixer a voz\n");
      return;
  }
  audioStream = al_create_audio_stream (2,882, 44100, ALLEGRO_AUDIO_DEPTH_UINT8, ALLEGRO_CHANNEL_CONF_1); //PENDING ¿this sound on both speakers?
  if (!al_attach_audio_stream_to_mixer(audioStream,mixer)) {  //check, must be 7 for 128K? beeper+3sound chanel + 3 noise chanel??
      ASprintf("Stream attach error\n");
      return;
    }
  ASprintf("Sonido Iniciado correctamente\n");
  gSoundInited = 1;
}

byte * gGetSampleBuffer (void){
  byte *ptr;
  if (!al_is_audio_installed())
    return NULL;

  while (1)
    {
 // PENDING     ptr = (byte *) get_audio_stream_buffer (audioStream);
      if (ptr != NULL)
	break;
    }
  return ptr;
}


void gPlaySound (void){
  if (!gSoundInited)
    return;
// PENDING  free_audio_stream_buffer (audioStream);
}

void gSoundSync (void){
  // no need for soundsync, while in gGetSampleBuffer does the work
}
#endif //ifdef SOUND_BY_STREAM


#ifdef SOUND_BY_SAMPLE
// ******************** MSDOS - WAY **********************
#define NSAMPS 1
SAMPLE *smp[NSAMPS];
int smpvoice;
int cursamp = 0;
int gSoundInited = 0;

void gInitSound (void){
  int i, j;
  initSoundLog ();

  reserve_voices (1, -1);
  if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
    {
      allegro_message ("Sound error");
      return;
    }

#define SAMPLE_SIZE 882*2
  for (j = 0; j < NSAMPS; j++)
    smp[j] = create_sample (8, 0, 44100, SAMPLE_SIZE);
  gSoundInited = 1;

  for (j = 0; j < NSAMPS; j++)
    for (i = 0; i < SAMPLE_SIZE; i++)
      {
	((byte *) (smp[j]->data))[i] = 128;
      }
  playMainSample ();

}

void playMainSample (void){
  smpvoice = play_sample (smp[0], 255, 128, 1000, 1);	// play always loopin
}

volatile int getVoicePos (void){
  return voice_get_position (smpvoice);
}

byte * gGetSampleBuffer (void)
{
  int pos;
  if (!gSoundInited)
    return NULL;

  pos = getVoicePos ();
  if (pos >= 882)
    return (byte *) smp[0]->data;
  else
    return ((byte *) smp[0]->data) + 882;
}

void gPlaySound (void){
  if (!gSoundInited)
    return;
  // void... in this version. anyway function must exist, as it will 
  // be called from sound.cpp (other "drivers" will need it)
}

void gSoundSync (void){
  static int last_sample_pos = 0, current_sample_pos;

  // this is the best way (I've found) to handle sound in msdos
  do
    {
      current_sample_pos = getVoicePos ();
      if (last_sample_pos == 0)
	if (current_sample_pos > 882)
	  break;
      if (last_sample_pos == 882)
	if (current_sample_pos < 882)
	  break;
    }
  while (1);
  if (current_sample_pos > 882)
    last_sample_pos = 882;
  else
    last_sample_pos = 0;
}
#endif // ifdef SOUND_BY_SAMPLE


#ifdef SOUND_METHOD_2
// ******************** METHOD 2 - WAY **********************
// not good enough under windows (voice_get_position has not enough resolution, lots of background sound)
// left here cos it could run nice on linux...

#define NSAMPS 3
SAMPLE *smp[NSAMPS];
int smpvoice = -1;
int cursamp = 0;
int gSoundInited = 0;

void gInitSound (void){
  int i, j;
  initSoundLog ();

  reserve_voices (3, -1);
  if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
    {
      allegro_message ("Sound error");
      return;
    }

#define SAMPLE_SIZE 882
  for (j = 0; j < NSAMPS; j++)
    smp[j] = create_sample (8, 0, 44100, SAMPLE_SIZE);
  gSoundInited = 1;

  for (j = 0; j < NSAMPS; j++)
    for (i = 0; i < SAMPLE_SIZE; i++)
      {
	((byte *) (smp[j]->data))[i] = 128;
      }


}

byte * gGetSampleBuffer (void){
  byte *ptr;
  if (!gSoundInited)
    return NULL;

  ptr = (byte *) smp[cursamp]->data;
  return ptr;

}


void gPlaySound (void){
  int rv;
  if (!gSoundInited)
    return;

  if (smpvoice != -1)
    while (1)
      {
	rv = voice_get_position (smpvoice);
	if (rv == -1)
	  break;
      }

  smpvoice = play_sample (smp[cursamp], 255, 128, 1000, 0);
  cursamp++;
  if (cursamp == NSAMPS)
    cursamp = 0;


}

void gSoundSync (void){
  // no need for soundsync, voice_get_position in gPlaySound does the work

}

#endif // ifdef SOUND_METHOD_2

int referencehelp_proc(void)
{
  //PALETTE pal,old_pal;
  ALLEGRO_BITMAP *image;
  char *archivo;
  //get_palette(old_pal);
  archivo=find_file("keys.pcx");
  image = al_load_bitmap(archivo);
  free(archivo);
  //set_palette(pal);
  //scare_mouse();
  //rectfill(screen,0,0,screen->w,screen->h,0);
  al_clear_to_color(al_map_rgb(0,0,0));
  //blit(image,screen,0,0,0,0,320,200);
  //unscare_mouse();
  al_set_target_bitmap(vscreen);
  al_draw_bitmap(image,0,0,0);
  al_flip_display();
  al_set_target_bitmap(screen);
  //PENDING readkey();		
  al_destroy_bitmap(image);
  //set_palette(old_pal);
  //PENDING ni idea de para que es: selected_opt = DIALOG_REFERENCEKEYS;
  return D_CLOSE;
}
int mouse_b(void){
  al_get_mouse_state(&mousestatus);
  return mousestatus.buttons;
}
int mouse_x(void){
  al_get_mouse_state(&mousestatus);
  return mousestatus.x;
}
int mouse_y(void){
  al_get_mouse_state(&mousestatus);
  return mousestatus.y;
}

void v_scaremouse(void){
  ;
}
void v_unscaremouse(void){
  ;
}
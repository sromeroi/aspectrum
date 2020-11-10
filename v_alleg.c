#include "stdafx.h"
/*=====================================================================
	"platform dependant" file, used to port the source code to
   other platforms / libraries , just "translate" all the
	funcions in this file to the target platform / library.
=====================================================================*/

#ifdef _DEBUG_
#include <mss.h>
#endif

// COMMENTS:
// to fix:
// macro END_OF_MAIN and END_OF_FUNCTION

#include <stdio.h>
#include <string.h>
#include "aspec.h"
#include "z80.h"
#include "snaps.h"
#include "main.h"

#include <allegro.h>
#include "monofnt.h"
#include "graphics.h"
#include "debugger.h"
#include "v_alleg.h"
#include "main.h"

#ifdef I_HAVE_AGUP
#include <agup.h>
#include <agtk.h>
#include <aphoton.h>
#include <awin95.h>
#include <aase.h>
#endif

extern void target_incrementor (void);
extern void count_frames (void);

extern volatile int frame_counter;
extern volatile int target_cycle;
extern volatile int last_fps;

// generic key handler ( = key using allegro)
volatile char *gkey;
// allegro virtual screen
BITMAP *vscreen;
extern Z80Regs spectrumZ80;
extern unsigned int colors[256];


/*-----------------------------------------------------------------
 ExitEmulator( void );
 This function is attached to exit() and called when the program
 ends, it must finish threads, SDL and other library/mem stuff.
------------------------------------------------------------------*/
// function called when you exit from the emulator (unitialization here)
void
ExitEmulator (void)
{
  unload_datafile (datafile);
  if (vscreen != NULL)
    destroy_bitmap (vscreen);
  set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
#ifdef I_HAVE_AGUP
  agup_shutdown ();
#endif

  allegro_exit ();
//   fcloseall();
  free (spectrumZ80.RAM);	/* free RAM */
  exit (0);
}


// clear the visible screen
void
gclear (void)
{
  clear (screen);
}


// dumps memory from the virtual screen to the visible screen
// it CAN be a flip_screen   ( but you'll have to swap the screen and vscreen pointers)
void
dumpVirtualToScreen (void)
{
  extern int v_res;
  blit (vscreen, screen, 0, 0, 0, 0, 320, v_res);
}

// draws text in the virtual screen
void
gtextout (char *b, int x, int y, int color)
{
  textout (vscreen, font, b, x, y, color);
}

// draws text in the virtual screen with no background
void
gtextoutb (char *b, int x, int y, int color, FONT * tfont)
{
  text_mode (-1);
  textout (vscreen, tfont, b, x, y, color);
  text_mode (0);
}

// sets the "index" colour to the "p" colour in the active palette.
// if you're not under a palette-based video mode this function can
// be empty, but the PutPixel function will have to "translate" from speccy
// colours (0-15) to real colors
void
gset_color (int index, gRGB * p)
{
  set_color (index, (RGB *) p);
}

// put platform specific initialization code here
void
init_wrapper (void)
{
  gkey = key;
}

// puts a pixel (col) in the virtual screen. look gset_color for more details.
// col is a "speccy color"
void
gPutPixel (int x, int y, int col)
{
  PutPixel (vscreen, x, y, col);
}


// generic acquire bitmap function
void
gacquire_bitmap (void)
{
  acquire_bitmap (vscreen);
}

// generic release bitmap function
void
grelease_bitmap (void)
{
  release_bitmap (vscreen);
}



void
InitSystem (void)
{
  // inits everything (for allegro)
//ASprintf("antes graficos \n");
  InitGraphics ();
//ASprintf("antes sonido \n");
  gInitSound ();
//ASprintf("despues sonido\n");

}

void
InitGraphics (void)
{
  int i, depth;
	FILE *archivo;
//   PALETTE specpal;
  extern int v_res;
  extern int v_border;
//ASprintf("antes allegro \n");
  allegro_init ();
//ASprintf("antes keyboards \n");
  install_keyboard ();
//ASprintf("antes timer \n");
  install_timer ();

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
//  ASprintf("se pedia %i y se obtubo %i\n",v_res,SCREEN_H);
  v_res = SCREEN_H;
  v_border = (v_res - 192) / 2;

  // if we're on windowed mode, update color conversion tables...

  if ((depth = desktop_color_depth ()) > 8)
    {
      ASprintf("Vale, al final salen mas de 8bit por pixel\n");
      for (i = 0; i < 16; i++)
	colors[i] =
	  makecol (colores[i].r * 4, colores[i].g * 4, colores[i].b * 4);
    }
  else
    {
      ASprintf("Vale, al final solo 8bit por pixel\n");
      for (i = 0; i < 16; i++)
	{
	  colors[i] = i;
	  gset_color (i, &colores[i]);
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
//ASprintf("primer uso de find_file\n");
  archivo=find_file("font.dat");
  datafile = load_datafile ("font.dat");
//ASprintf("usadofind_file\n");
  free(archivo);
//ASprintf("liberado\n");

/* lo comento por que find_file termina el programa si no lo encuentra.
	if (!datafile)
    {
      set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
      allegro_message ("File font.dat does not exist...\n");
      ExitEmulator ();
    };
*/
 font = datafile[0].dat;

  LOCK_VARIABLE (last_fps);
  LOCK_VARIABLE (frame_counter);
  LOCK_VARIABLE (target_cycle);
//   LOCK_FUNCTION(count_frames);
//   LOCK_FUNCTION(target_incrementor);
  install_int_ex (count_frames, BPS_TO_TIMER (1));
  install_int_ex (target_incrementor, BPS_TO_TIMER (50));
  last_fps = frame_counter = target_cycle = 0;

//ASprintf("creando vscreen\n");

  vscreen = create_bitmap (320, v_res);
  if (vscreen == NULL)
    {
      set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
      allegro_message ("Not enough memory: couldn't create vscreen.\n%s\n",
		       allegro_error);
      ExitEmulator ();
    }
  ASprintf("Working at %d bpp\n", bitmap_color_depth (screen));
  clear (vscreen);

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

}


// draw filled rectangles
void
gbox (int x1, int y1, int x2, int y2, int color)
{
  rectfill (vscreen, x1, y1, x2, y2, color);
}

// draw rectangles
void
grectangle (int x1, int y1, int x2, int y2, int color)
{
  rect (vscreen, x1, y1, x2, y2, color);
}

// draw hlines
void
ghline (int x1, int y1, int x2, int col)
{
  int x;
  for (x = x1; x <= x2; x++)
    PutPixel (vscreen, x, y1, col);
//      hline(vscreen,x1,y1,x2,col);
}


// cls with specified color
void
gclear_to_color (int color)
{
  clear_to_color (vscreen, color);
}

// transfers from (x,y) to (x+w,y+h) from the virtual screen to the visible screen
void
gUpdateRect (int x, int y, int w, int h)
{
  blit (vscreen, screen, x, y, x, y, w, h);
}

#if ALLEGRO_WIP_VERSION >= 38
#define CLEARBITMAP clear_bitmap
#else
#define CLEARBITMAP clear
#endif

void
v_initmouse (void)
{
  extern struct tipo_emuopt emuopt;
  int color_b, color_n;

  if (install_mouse () != -1)
    {
      emuopt.gunstick |= GS_HAYMOUSE;

      // dibujar puntero 
      emuopt.raton_bmp = create_bitmap (16, 16);
      CLEARBITMAP (emuopt.raton_bmp);
      color_b = makecol (255, 255, 255);
      color_n = makecol (0, 0, 0);
      circle (emuopt.raton_bmp, 8, 8, 7, color_n);
      circle (emuopt.raton_bmp, 8, 8, 6, color_b);
      putpixel (emuopt.raton_bmp, 8, 8, color_b);
      putpixel (emuopt.raton_bmp, 7, 8, color_n);
      putpixel (emuopt.raton_bmp, 9, 8, color_n);
      putpixel (emuopt.raton_bmp, 8, 7, color_n);
      putpixel (emuopt.raton_bmp, 8, 9, color_n);

      if ((emuopt.gunstick & GS_GUNSTICK) != 0)
	{
	  set_mouse_sprite (emuopt.raton_bmp);
	  set_mouse_sprite_focus (8, 8);
	  show_mouse (screen);
	}
    }

}

int
galert (const char *s1, const char *s2, const char *s3, const char *b1,
	const char *b2, int c1, int c2)
{
  int a;
//    gui_fg_color = 0;
//    gui_bg_color = 7;
  a = alert (s1, s2, s3, b1, b2, c1, c2);
  return a;
}


#ifdef SOUND_BY_STREAM
// ******************** WIN32 - WAY **********************
// audiostreams

int gSoundInited = 0;
AUDIOSTREAM *audioStream;

void
gInitSound (void)
{
  initSoundLog ();

  reserve_voices (3, -1);
  if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
    {
      printf("Sound error\n");
      return;
    }

  printf("Sonido Iniciado correctamente\n");
  audioStream = play_audio_stream (882, 8, 0, 44100, 255, 128);
  gSoundInited = 1;


}

u8 *
gGetSampleBuffer (void)
{
  u8 *ptr;
  if (!gSoundInited)
    return NULL;

  while (1)
    {
      ptr = (u8 *) get_audio_stream_buffer (audioStream);
      if (ptr != NULL)
	break;
    }
  return ptr;
}


void
gPlaySound (void)
{
  if (!gSoundInited)
    return;
  free_audio_stream_buffer (audioStream);

}

void
gSoundSync (void)
{
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

void
gInitSound (void)
{
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
	((u8 *) (smp[j]->data))[i] = 128;
      }
  playMainSample ();

}

void
playMainSample (void)
{
  smpvoice = play_sample (smp[0], 255, 128, 1000, 1);	// play always loopin
}

volatile int
getVoicePos (void)
{
  return voice_get_position (smpvoice);
}

u8 *
gGetSampleBuffer (void)
{
  int pos;
  if (!gSoundInited)
    return NULL;

  pos = getVoicePos ();
  if (pos >= 882)
    return (u8 *) smp[0]->data;
  else
    return ((u8 *) smp[0]->data) + 882;
}

void
gPlaySound (void)
{
  if (!gSoundInited)
    return;
  // void... in this version. anyway function must exist, as it will 
  // be called from sound.cpp (other "drivers" will need it)
}

void
gSoundSync (void)
{
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

void
gInitSound (void)
{
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
	((u8 *) (smp[j]->data))[i] = 128;
      }


}

u8 *
gGetSampleBuffer (void)
{
  u8 *ptr;
  if (!gSoundInited)
    return NULL;

  ptr = (u8 *) smp[cursamp]->data;
  return ptr;

}


void
gPlaySound (void)
{
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

void
gSoundSync (void)
{
  // no need for soundsync, voice_get_position in gPlaySound does the work

}

#endif // ifdef SOUND_METHOD_2

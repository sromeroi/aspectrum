#include "stdafx.h"
/*=====================================================================
  ASpectrum Emulator. This is our contribution to the Spectrum World.
  We're trying to release our simple, useable and portable Spectrum
  emulator, always thinking in the GNU/Linux community.

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

 Copyright (c) 2000 Santiago Romero Iglesias
 Email: sromero@escomposlinux.org
 ======================================================================*/

#ifdef _DEBUG_
#include <mss.h>
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef NO_GETOPTLONG
#include "contrib/getopt.h"
#else
#include <getopt.h>
#endif // NO_GETOPTLONG

#include "aspec.h"
#include "langs.h"
#include "z80.h"
#include "snaps.h"

#include "monofnt.h"
#include "graphics.h"
#include "debugger.h"
#include "menu.h"


struct tipo_emuopt emuopt =
  { "\0", "\0", "\0", GS_INACTIVE, NULL, 'n', {'o', 'p', 'q', 'a', ' '} };

// hardware definitions for spectrum 48K
struct tipo_hwopt hwopt = { 0xFF, 24, 128, 24, 48, 224, 16, 48, 192, 48, 8,
  224, 64, 192, 56, 24, 128, 72
};

int v_res = 240;
int v_border = 24;

// switch between display by scanlines or display at end of frame
//int displayByScanlines=1;

// generic wrapper
extern volatile char *gkey;

/* Some global variables used in the emulator */
Z80Regs spectrumZ80;
FILE *tapfile;
char fname[512];
char *argvzero;
volatile int last_fps, frame_counter;
volatile int target_cycle;

/*----------------------------------------------------------------*/
int debug = 0, main_tecla, hay_tecla, scanl = 0, quit_thread = 0;
int fila[5][5];

/* 0 = english
 * 1 = spanish
 * 2 = valencia/catala
*/
int language = 0;

void ExitEmulator (void);
void CreateVideoTables (void);
void UpdateKeyboard (void);
void target_incrementor (void);
void count_frames (void);


// now global
char tfont[4096];

// to know if sound is ok
extern int gSoundInited;


int
Z80Initialization (void)
{
  FILE *fp;
  int i;
  /* we get memory and load font, spectrum ROM and
     possible snapshots selected in the command line */

  printf("ASpectrum GNU pure C Z80 / Spectrum Emulator V " VERSION "\n"
	  "(C) 2000-2003 Santiago Romero (NoP/Compiler), Kak & Alvaro Alea.\n"
	  "http://aspectrum.sf.net\n"
	  "Powered by Allegro 4 - http://alleg.sf.net\n"
	  "Distributed under the terms of GNU Public License V2\n\n");
  spectrumZ80.RAM = (byte *) malloc (65536);
  if (spectrumZ80.RAM == NULL)
    {
      printf("\n Z80: Error allocating RAM memory, exiting...\n\n");
      return (0);
    }

  init_wrapper ();

  // Open the FONT.FNT file
/*
	#ifdef DESTDAT
  if ((fp = fopen (DESTDAT "/font.fnt", "rb")) != NULL)
    printf("using " DESTDAT "/font.fnt\n");
  else
#endif
  if ((fp = fopen ("font.fnt", "rb")) != NULL){
    printf("using  ./font.fnt\n");
  }
  else
    {
      printf("File font.fnt does not exist...\n");
      return (0);
    };
  fread (tfont, 4096, 1, fp);
  fclose (fp);
*/
	fp=findopen_file("font.fnt");
	fread (tfont, 4096, 1, fp);
  	fclose (fp);


  i = 0;

  // Open the rom file
  if (emuopt.romfile[0] != 0)
    {
      if ((fp = fopen (emuopt.romfile, "rb")) != NULL)
	  { 
		  printf ("Using ROM file %s\n", emuopt.romfile);
	  }
      else
	{
	  printf("The ROM file does not exists.\n");
	  return (0);
	}
    }
/*
	
	#ifdef DESTDAT
  else if ((fp = fopen (DESTDAT "/spectrum.rom", "rb")) != NULL) {
    printf("using " DESTDAT "/spectrum.rom\n");
  }
#endif
  else if ((fp = fopen ("spectrum.rom", "rb")) != NULL){
    printf("using spectrum.rom\n");
  }
  else
    {
      printf("File spectrum.rom does not exist ...\n");
      return (0);
    };
*/
  fp=findopen_file("spectrum.rom");	
  while (!feof (fp))
    spectrumZ80.RAM[i++] = fgetc (fp);
  fclose (fp);

  // COMMENT: Is this needed? -> CreateVideoTables();
  Z80Reset (&spectrumZ80, 69888);
  Z80FlagTables ();
  return 1;

}


#ifdef ZXDEB
void screenRedraw_forZXDEB (void)
{
  DisplayScreen (&spectrumZ80);
  dumpVirtualToScreen ();

}

int ZXDEB_break;
void
keyboardHandler_forZXDEB (void)
{
  UpdateKeyboard ();
  if (gkey[KEY_ESC])
    ZXDEB_break = 1;

}
#endif

/*
#ifndef ZXDEBUG_MFC
int main (int argc, char *argv[])
{
  return emuMain (argc, argv);
}
#endif
*/
/*----------------------------------------------------------------
 Main function. It inits all the emulator stuff and executes it.
----------------------------------------------------------------*/
#ifndef ZXDEBUG_MFC
int main (int argc, char *argv[])
#else
int emuMain (int argc, char *argv[])
#endif
{
  int target_tstate, current_tstate;
  char b[1024];
  char done = 0, value;
  int offs = 0, poke, option;
  FILE *fp;
  int c, tecla = 0;
  static int f_flash = 1, f_flash2 = 0;

  extern char *optarg;
  extern int optind, opterr, optopt;
  static struct option long_options[] = {
    {"rom", 1, NULL, 'r'},
    {"snap", 1, NULL, 's'},
    {"tape", 1, NULL, 't'},
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'V'},
    {"debug", 0, NULL, 'd'},
    {"joy", 1, NULL, 'j'},
    {0, 0, 0, 0}
  };

  // needed later to find the executable dir
  argvzero=argv[0];

// first of all do the parser for options arguments
// codigo de control de argumentos pasados al programa.
//opterr=0; // pa que narices valia esto ????

#ifndef ZXDEB

#ifdef NO_GETOPTLONG
  while ((c = getopt (argc, argv, "r:s:t:hVdj:")) != -1)
#else
  while ((c =
	  getopt_long (argc, argv, "r:s:t:hVdj:", long_options, NULL)) != -1)
#endif
    {
      switch (c)
	{
	case 'r':
	  strncpy (emuopt.romfile, optarg, 255);
	  break;
	case 's':
	  strncpy (emuopt.snapfile, optarg, 255);
	  break;
	case 't':
	  strncpy (emuopt.tapefile, optarg, 255);
	  break;
	case 'V':
	  ASprintf("ASpectrum Version " VERSION "\n");
	  done = 1;
	  break;
	case 'd':
	  debug = 1;
	  break;
	case 'j':
	  if (strstr (optarg, "G") != NULL)
	    emuopt.gunstick |= GS_GUNSTICK;
	  if (strstr (optarg, "k") != NULL)
	    ;
	  break;
	case ':':
	  printf("Lack of parameters\n");
	case 'h':
	case '?':
	  printf("ASpectrum GNU pure C Z80 / Spectrum Emulator V " VERSION
		  "\n"
		  "(C) 2000-2003 Santiago Romero (NoP/Compiler), Kak & Alvaro Alea\n"
	  	  "http://aspectrum.sf.net\n"
	  	  "Powered by Allegro 4 - http://alleg.sf.net\n"
		  "Distribuited under the terms of GNU Public License V2\n\n"
		  "Use of Aspectrum:\n"
		  "   aspectrum [options] [snapshot or tape file]\n\n"
		  "Options can be:\n"
		  "   -r --rom romfile  use romfile instead own romfile.\n"
		  "   -s --snap file    load snapshot at startup\n"
		  "                     suported snapshot files are .SP .SNA .Z80\n"
		  "   -t --tape file    use file as tape for load routines.\n"
		  "   -d --debug        start the emulator paused in debug mode.\n"
		  "   -V --version      echo the version of the emulator.\n"
		  "   -h --help         this help.\n"
		  "   -j --joy def      enable joystick, def is a string of caracter\n"
		  "	                for each joystick, see doc for more help.\n");
	  done = 1;
	  break;
	};
    };
  if (done != 0)
    return (0);
  // parameter error = direct exit 
#endif // ZXDEB endif

  Z80Initialization ();
  // AS_printf("Z80 Initialization completed\n");

  // check if it's the last arg
  if ((optind + 1) < argc)
    {
      printf("excess of unknow args\n");
      return (-1);
    }
  else if ((optind + 1) == argc)
    {
      if (strstr (argv[optind], ".z80") != NULL ||
	  strstr (argv[optind], ".Z80") != NULL ||
	  strstr (argv[optind], ".sp") != NULL ||
	  strstr (argv[optind], ".SP") != NULL ||
	  strstr (argv[optind], ".sna") != NULL ||
	  strstr (argv[optind], ".SNA") != NULL)
	strncpy (emuopt.snapfile, argv[optind], 255);
      else if (strstr (argv[optind], ".tap") != NULL ||
	       strstr (argv[optind], ".TAP") != NULL ||
	       strstr (argv[optind], ".tzx") != NULL ||
	       strstr (argv[optind], ".TZX") != NULL)
	strncpy (emuopt.tapefile, argv[optind], 255);
      else
	{
	  printf("Args unknow\n");
	  return (-1);
	}
    }
  // AS_printf("Posible argumento indentificado.\n");
	
  // Check and open tape file if needed
  if (emuopt.tapefile[0] == 0){
    printf("Not using tape.\n");
  }
  else if ((fp=InitTape(fp)) != NULL)
    {
      printf ("Using tape file %s.\n", emuopt.tapefile);
	  // AS_printf("Main:%x\n",fp);
	  tapfile = fp;
    }
  else
    {
      printf("Tape file %s does not exist.\n", emuopt.tapefile);
      return (-1);
    };

  // Check and open snapshot file if needed
  if (emuopt.snapfile[0] == 0) {
    printf("No loading snapshot.\n");
  }
  else if ((fp = fopen (emuopt.snapfile, "rb")) != NULL)
    {
      printf("Using snapshot file %s.\n", emuopt.snapfile);
      fclose (fp);
      if (!LoadSnapshot (&spectrumZ80, emuopt.snapfile)) {
	     printf("\n Any wrong in snapshot file. clean boot.\n");
	  }
    }
  else
    {
      printf("Snapshot file %s does not exist.\n", emuopt.snapfile);
      return (-1);
    };

  // Init all the graphic stuff:

//ASprintf("antes de initsystem\n");
  InitSystem ();
  set_window_title ("ASpectrum emulator");
//ASprintf("despues de initsystem\n");
  v_initmouse ();
  ClearScreen (7);

  // Init main variables:
  hay_tecla = main_tecla = 0;

  fila[1][1] = fila[1][2] = fila[2][2] = fila[3][2] = fila[4][2] =
    fila[4][1] = fila[3][1] = fila[2][1] = 0xFF;

  // If we start on debug mode we need to update the debugger screen:
  if (debug)
    {
      ClearScreen (0);
      Z80Dump (&spectrumZ80, tfont);
      DrawInstruction (&spectrumZ80, tfont);
      ShowMem (&spectrumZ80, offs, tfont);
      DrawHelp (tfont);
      debug = 1;
      tecla = '.';
    }

  initSoundLog ();		// first sound log initialization
//ASprintf("entrando en el bucle\n");

  // MAIN LOOP
  while (!done)
    {
      // Read a key to interpret if available
      if (keypressed ())
	     tecla = readkey ();
	  //printf("tecla=%i\n",tecla);
      if ((tecla >> 8 == gKEY_ESC) || (tecla >>8 == KEY_TILDE ) )
	{
	  // call the menu and get the selected option
	  scare_mouse ();
	  option = MainMenu (&spectrumZ80, tfont);
	  unscare_mouse ();

	  // simulate a keypress depending on the choosen option
	  switch (option)
	    {
	    case DIALOG_DEBUGGER_0:
	      tecla = gKEY_F1 << 8;
	      debug = 0;
	      break;
	    case DIALOG_DEBUGGER_1:
	      tecla = gKEY_F1 << 8;
	      debug = 1;
	      break;
	    case DIALOG_SNAP_SAVE:
	      tecla = gKEY_F2 << 8;
	      break;
	    case DIALOG_SNAP_LOAD:
	      tecla = gKEY_F3 << 8;
	      break;
	    case DIALOG_SAVE_SCR:
	      tecla = gKEY_F4 << 8;
	      break;
	    case DIALOG_RESET:
	      tecla = gKEY_F5 << 8;
	      break;
	    case DIALOG_OPEN_TAPE:
	      tecla = gKEY_F6 << 8;
	      break;
	      //   case 7 : tecla = gKEY_F7  << 8 ; break;
	    case DIALOG_CHANGE_LANG:
	      tecla = gKEY_F8 << 8;
	      break;
	    case DIALOG_QUIT:
	      tecla = gKEY_F10 << 8;
	      break;
	    case DIALOG_REWIND_TAPE:
		  RewindTAP (&spectrumZ80, tapfile);
		  break;
		};

	}
      switch (tecla >> 8)
	{

	case gKEY_F2:
	  if (FileMenu (tfont, DIALOG_SNA, fname) != 0)
	    SaveSnapshot (&spectrumZ80, fname);
	  tecla = gKEY_ESC << 8;
	  //debug = 1 - debug;
	  break;

	case gKEY_F3:
	  if (FileMenu (tfont, DIALOG_SNAyC, fname) != 0)
	    LoadSnapshot (&spectrumZ80, fname);
	  tecla = gKEY_ESC << 8;
	  //debug = 1 - debug;
	  break;

	case gKEY_F4:
	  if (FileMenu (tfont, DIALOG_SCR, fname) != 0)
	    SaveScreenshot (&spectrumZ80, fname);
	  tecla = gKEY_ESC << 8;
	  //debug = 1 - debug;
	  break;

	case gKEY_F5:
	  Z80Reset (&spectrumZ80, 69888);
	  tecla = gKEY_ESC << 8;
	  //debug = 1 - debug;
	  break;

	case gKEY_F6:
	  if (FileMenu (tfont, DIALOG_TAP, fname) != 0)
		  {
			if (emuopt.tapefile[0] != 0)
				fclose (fp);
	      	strncpy (emuopt.tapefile, fname, 255);
	      	if ((fp=InitTape(fp))!= NULL)
				{
		  			ASprintf("Using tape file %s.\n", emuopt.tapefile);
		  			tapfile = fp;
				}
	    }
	  tecla = gKEY_ESC << 8;
	  //debug = 1 - debug;
	  break;

	case gKEY_F7:
	  tecla = gKEY_ESC << 8;
	  menuopciones ();
	  //debug = 1 - debug;
	  break;

	case gKEY_F8:
	  if (language < LANGUAGES - 1)
	    language++;
	  else
	    language = 0;
	  tecla = gKEY_ESC << 8;
	  //debug = 1 - debug;
	  break;

	case gKEY_F12:
	  DebuggerHelp (tfont);
	  tecla = gKEY_ESC << 8;
	  //debug = 1 - debug;
	  break;

	case gKEY_F10:
	  if (language == 1)
	    {
	      if (galert ("Esto cerrara Aspectrum.", "",
			  "Confirme que desea cerrar el programa.", "Si",
			  "No", 13, 27) == 1)
		done = 1;
	      break;
	    }
	  else
	    {
	      if (galert ("This will close Aspectrum", "",
			  "Are you sure?", "Yes", "No", 13, 27) == 1)
		done = 1;
	      break;
	    }
	};

      if ((tecla >> 8) == gKEY_F1)
	{
	  if (debug == 0)
	    {
	      ClearScreen (0);
	      gclear ();
	      Z80Dump (&spectrumZ80, tfont);
	      DrawInstruction (&spectrumZ80, tfont);
	      ShowMem (&spectrumZ80, offs, tfont);
	      DrawHelp (tfont);
	      tecla = '.';
	      debug = 1;
	    }
	  else
	    {
	      debug = target_cycle = 0;
	      ClearScreen (0);
	      DisplayScreen (&spectrumZ80);
	    }
	}

      // the meaning of the keyb depends on being or not in debug mode:
      switch (debug)
	{
	  // emulation mode;
	case 0:
	  f_flash2++;
	  if (f_flash2 >= 32)
	    f_flash2 = 0;
	  f_flash = (f_flash2 < 16 ? 0 : 1);

	  // if there is enough time, draw frame:
	  if (target_cycle < 2 || frame_counter == 0)
	    {
	      // no visible upper border
	      target_tstate =
		(hwopt.ts_line *
		 (hwopt.line_upbo + hwopt.line_poin - v_border)) -
		hwopt.ts_lebo;
	      current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
	      hwopt.port_ff = 0xFF;
	      Z80Run (&spectrumZ80, target_tstate - current_tstate);

	      // visible upper border         
	      for (scanl = 0; scanl < v_border; scanl++)
		{
		  target_tstate += hwopt.ts_line;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);
		  displayborderscanline (scanl);
		}

	      // Now run the emulator for all the real screen (192 lines)
	      for (scanl = 0; scanl < 192; scanl++)
		{
		  // left border
		  target_tstate += hwopt.ts_lebo;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff = 0xFF;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);

		  // screen
		  target_tstate += hwopt.ts_grap;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff = 0x00;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);

		  // right border
		  target_tstate += (hwopt.ts_ribo + hwopt.ts_hore);
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff = 0xFF;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);

		  displayscanline2 (scanl, f_flash, &spectrumZ80);
		}

	      // visible bottom border
	      hwopt.port_ff = 0xFF;
	      for (scanl = 192 + v_border; scanl < v_res; scanl++)
		{
		  target_tstate += hwopt.ts_line;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);
		  displayborderscanline (scanl);
		}

	      // the last lines (56+16 lines - border)
	      // Run it for 56 lines covering bottom border and ray return
	      Z80Run (&spectrumZ80, spectrumZ80.ICount);

	      //Calc FPS
	      sprintf (b, "%d ", last_fps);
	      gtextout (b, 4, v_res - 16, 15);
	      scare_mouse ();
	      dumpVirtualToScreen ();
	      unscare_mouse ();

	    }
	  else
	    {
	      // If we have not enough time, don't draw the screen,
	      // only emulate

	      // no visible upper border
	      target_tstate =
		(hwopt.ts_line * (hwopt.line_upbo + hwopt.line_poin)) -
		hwopt.ts_lebo;
	      current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
	      hwopt.port_ff = 0xFF;
	      Z80Run (&spectrumZ80, target_tstate - current_tstate);

	      // Now run the emulator for all the real screen (192 lines)
	      for (scanl = 0; scanl < 192; scanl++)
		{
		  // left border
		  target_tstate += hwopt.ts_lebo;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff = 0xFF;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);

		  // Screen
		  target_tstate += hwopt.ts_grap;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff = 0x00;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);

		  // right border 
		  target_tstate += (hwopt.ts_ribo + hwopt.ts_hore);
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff = 0xFF;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);
		}
	      // visible bottom border
	      hwopt.port_ff = 0xFF;
	      Z80Run (&spectrumZ80, spectrumZ80.ICount);
	    }

	  // Speed control without sound
	  if (!gSoundInited)
	    while (target_cycle == 0);
	  else
	    {
	      gSoundSync ();	// wait for 1/50th
	      soundDump ();
	      target_cycle = 0;
	    }

	  target_cycle--;
	  frame_counter++;
	  UpdateKeyboard ();
	  break;


	  // Debug mode:         
	case 1:
	  switch (tecla & 0xFF)
	    {

	      // show the Spectrum current screen, wait a key and return
	    case 's':
	      ClearScreen (0);
	      DisplayScreen (&spectrumZ80);
	      dumpVirtualToScreen ();
	      readkey ();
	      ClearScreen (0);
	      gclear ();
	      DrawHelp (tfont);
	      ShowMem (&spectrumZ80, offs, tfont);
	      DrawHelp (tfont);
	      Z80Dump (&spectrumZ80, tfont);
	      DrawInstruction (&spectrumZ80, tfont);
	      break;

	      // Run the emulator until PC = given address
	    case 't':
	      GetHexValue (2, 130, lang_runto_t[language], b, tfont, 6, 0, 6);
	      spectrumZ80.TrapAddress = strtol (b, (char **) NULL, 16);
	      while (!keypressed () &&
		     spectrumZ80.PC.W != spectrumZ80.TrapAddress)
		{
		  Z80Run (&spectrumZ80, 1);
		  Z80Dump (&spectrumZ80, tfont);
		  DrawInstruction (&spectrumZ80, tfont);
		  ShowMem (&spectrumZ80, offs, tfont);
		}
	      break;

	      // Run the emulator on simulation mode until PC = given address
	    case 'w':
	      GetHexValue (2, 130, lang_runto_w[language], b, tfont, 6, 0, 6);
	      spectrumZ80.TrapAddress = strtol (b, (char **) NULL, 16);
	      spectrumZ80.dobreak = 1;
	      debug = 0;
	      Z80Dump (&spectrumZ80, tfont);
	      DrawInstruction (&spectrumZ80, tfont);
	      ShowMem (&spectrumZ80, offs, tfont);
	      DrawHelp (tfont);
	      break;

	      // Poke memory
	    case 'p':
	      GetHexValue (2, 130, lang_poke[language], b, tfont, 6, 0, 6);
	      poke = strtol (b, (char **) NULL, 16);
	      GetHexValue (2, 130, lang_value[language], b, tfont, 6, 0, 6);
	      value = strtol (b, (char **) NULL, 16);
	      Z80MemWrite (poke, value, &spectrumZ80);
	      Z80Dump (&spectrumZ80, tfont);
	      DrawInstruction (&spectrumZ80, tfont);
	      ShowMem (&spectrumZ80, offs, tfont);
	      DrawHelp (tfont);
	      break;

	      // Run the emulation until ICount < 100 (near interrupt)
	    case 'i':
	      debug = 0;
	      while (spectrumZ80.ICount > 50 && !keypressed ())
		{
		  Z80Run (&spectrumZ80, 1);
		}
	      debug = 1;
	      tecla = '.';
	      Z80Dump (&spectrumZ80, tfont);
	      DrawInstruction (&spectrumZ80, tfont);
	      ShowMem (&spectrumZ80, offs, tfont);
	      DrawHelp (tfont);
	      break;

	      // change the value of the Z80 registers:
	    case 'A':
	      GetHexValue (2, 130, lang_change_af2[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.AFs.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'a':
	      GetHexValue (2, 130, lang_change_af[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.AF.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'B':
	      GetHexValue (2, 130, lang_change_bc2[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.BCs.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'b':
	      GetHexValue (2, 130, lang_change_bc[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.BC.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'D':
	      GetHexValue (2, 130, lang_change_de2[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.DEs.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'd':
	      GetHexValue (2, 130, lang_change_de[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.DE.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'H':
	      GetHexValue (2, 130, lang_change_hl2[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.HLs.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'h':
	      GetHexValue (2, 130, lang_change_hl[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.HL.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'x':
	      GetHexValue (2, 130, lang_change_ix[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.IX.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'y':
	      GetHexValue (2, 130, lang_change_iy[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.IY.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'S':
	      GetHexValue (2, 130, lang_change_sp[language], b, tfont, 6, 0,
			   6);
	      spectrumZ80.SP.W = strtol (b, (char **) NULL, 16);
	      break;
	    case 'n':
	      if (offs > 0)
		offs -= 2;
	      ShowMem (&spectrumZ80, offs, tfont);
	      break;
	    case 'm':
	      if (offs < 0xFFFF - 18)
		offs += 2;
	      ShowMem (&spectrumZ80, offs, tfont);
	      break;
	    case 'l':
	      GetHexValue (2, 130, lang_change_mem[language], b, tfont, 6, 0,
			   6);
	      offs = strtol (b, (char **) NULL, 16);
	      if (offs > 0xFFFF - 18)
		offs = 0xFFFF - 18;
	      else if (offs < 0)
		offs = 0;
	      if (offs & 1)
		offs--;
	      ShowMem (&spectrumZ80, offs, tfont);
	      break;

	      // Advance emulation by 1 instruction
	    case ' ':
	    case '\r':
	    case '\n':
	      Z80Run (&spectrumZ80, 1);
	      DrawInstruction (&spectrumZ80, tfont);
	      Z80Dump (&spectrumZ80, tfont);
	      ShowMem (&spectrumZ80, offs, tfont);
	      break;
	    }
	  break;
	}
      tecla = 0;


    }				// while (!done)

  return (1);
}

END_OF_MAIN ();
// When compiling under MSDOS you should comment the above line...


/*-----------------------------------------------------------------
 CreateVideoTables ( void );
 Creates tables for direct access to videomemory pixels and attr.
------------------------------------------------------------------*/
/*
void CreateVideoTables ( void )
{
   int x = 0, y;

   for( y=0; y < 192; y++)
   {
       Pixeles[y] = 0x4000 + ((y >> 6) * 2048) +
                        (((y >> 3) & 0x07) << 5) +
                        ((y & 0x07) << 8) +
                        ((x >> 3) & 0x1f);
	   Atributos[y] = 22528 + ((x >> 3) & 0x1f) +
                        ((y >> 3) << 5);
   }
}

*/
/*-----------------------------------------------------------------
 UpdateKeyboard( void );
 Updates the keyboard variables used on the return of IN function.
------------------------------------------------------------------*/
void
UpdateKeyboard (void)
{

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

  static unsigned char teclas_fila[NUM_KEYB_KEYS][3] = {
    {1, 2, 0xFE}, /* 0 */ {1, 1, 0xFE}, /* 1 */ {1, 1, 0xFD},	/* 2 */
    {1, 1, 0xFB}, /* 3 */ {1, 1, 0xF7}, /* 4 */ {1, 1, 0xEF},	/* 5 */
    {1, 2, 0xEF}, /* 6 */ {1, 2, 0xF7}, /* 7 */ {1, 2, 0xFB},	/* 8 */
    {1, 2, 0xFD},		/* 9 */
    {3, 1, 0xFE}, /* a */ {4, 2, 0xEF}, /* b */ {4, 1, 0xF7},	/* c */
    {3, 1, 0xFB}, /* d */ {2, 1, 0xFB}, /* e */ {3, 1, 0xF7},	/* f */
    {3, 1, 0xEF}, /* g */ {3, 2, 0xEF}, /* h */ {2, 2, 0xFB},	/* i */
    {3, 2, 0xF7}, /* j */ {3, 2, 0xFB}, /* k */ {3, 2, 0xFD},	/* l */
    {4, 2, 0xFB}, /* m */ {4, 2, 0xF7}, /* n */ {2, 2, 0xFD},	/* o */
    {2, 2, 0xFE}, /* p */ {2, 1, 0xFE}, /* q */ {2, 1, 0xF7},	/* r */
    {3, 1, 0xFD}, /* s */ {2, 1, 0xEF}, /* t */ {2, 2, 0xF7},	/* u */
    {4, 1, 0xEF}, /* v */ {2, 1, 0xFD}, /* w */ {4, 1, 0xFB},	/* x */
    {2, 2, 0xEF}, /* y */ {4, 1, 0xFD},	/* z */
    {4, 2, 0xFE}, /*SPACE*/
      {3, 2, 0xFE}, /*ENTER*/
      {4, 1, 0xFE}, /*RSHIFT*/ {4, 2, 0xFD}, /*ALT*/ {1, 2, 0xEF}, /*CTRL*/
  };


  /* reset the spectrum row and column keyboard signals */

  fila[1][1] = fila[1][2] = fila[2][2] = fila[3][2] =
    fila[4][2] = fila[4][1] = fila[3][1] = fila[2][1] = 0xFF;


  /* change row and column signals according to pressed key */
  /* HEY THIS DONT USE V_ALLEGRO.H DEF use ALLEGRO.H
     but by "motivos personales" I DONT CHANGE THIS X'D */

  if (gkey[KEY_Z])
    fila[4][1] &= (0xFD);
  if (gkey[KEY_X])
    fila[4][1] &= (0xFB);
  if (gkey[KEY_C])
    fila[4][1] &= (0xF7);
  if (gkey[KEY_V])
    fila[4][1] &= (0xEF);
  if (gkey[KEY_RSHIFT] || key[KEY_LSHIFT])
    fila[4][1] &= (0xFE);

  if (gkey[KEY_A])
    fila[3][1] &= (0xFE);
  if (gkey[KEY_S])
    fila[3][1] &= (0xFD);
  if (gkey[KEY_D])
    fila[3][1] &= (0xFB);
  if (gkey[KEY_F])
    fila[3][1] &= (0xF7);
  if (gkey[KEY_G])
    fila[3][1] &= (0xEF);

  if (gkey[KEY_Q])
    fila[2][1] &= (0xFE);
  if (gkey[KEY_W])
    fila[2][1] &= (0xFD);
  if (gkey[KEY_E])
    fila[2][1] &= (0xFB);
  if (gkey[KEY_R])
    fila[2][1] &= (0xF7);
  if (gkey[KEY_T])
    fila[2][1] &= (0xEF);

  if (gkey[KEY_1])
    fila[1][1] &= (0xFE);
  if (gkey[KEY_2])
    fila[1][1] &= (0xFD);
  if (gkey[KEY_3])
    fila[1][1] &= (0xFB);
  if (gkey[KEY_4])
    fila[1][1] &= (0xF7);
  if (gkey[KEY_5])
    fila[1][1] &= (0xEF);

  if (gkey[KEY_0])
    fila[1][2] &= (0xFE);
  if (gkey[KEY_9])
    fila[1][2] &= (0xFD);
  if (gkey[KEY_8])
    fila[1][2] &= (0xFB);
  if (gkey[KEY_7])
    fila[1][2] &= (0xF7);
  if (gkey[KEY_6])
    fila[1][2] &= (0xEF);

  if (gkey[KEY_P])
    fila[2][2] &= (0xFE);
  if (gkey[KEY_O])
    fila[2][2] &= (0xFD);
  if (gkey[KEY_I])
    fila[2][2] &= (0xFB);
  if (gkey[KEY_U])
    fila[2][2] &= (0xF7);
  if (gkey[KEY_Y])
    fila[2][2] &= (0xEF);

  if (gkey[KEY_ENTER])
    fila[3][2] &= (0xFE);
  if (gkey[KEY_L])
    fila[3][2] &= (0xFD);
  if (gkey[KEY_K])
    fila[3][2] &= (0xFB);
  if (gkey[KEY_J])
    fila[3][2] &= (0xF7);
  if (gkey[KEY_H])
    fila[3][2] &= (0xEF);

  if (gkey[KEY_SPACE])
    fila[4][2] &= (0xFE);
  if (gkey[KEY_ALT] || key[KEY_ALT])
    fila[4][2] &= (0xFD);
  if (gkey[KEY_M])
    fila[4][2] &= (0xFB);
  if (gkey[KEY_N])
    fila[4][2] &= (0xF7);
  if (gkey[KEY_B])
    fila[4][2] &= (0xEF);

  if (gkey[KEY_BACKSPACE])
    {
      fila[4][1] &= (0xFE);
      fila[1][2] &= (0xFE);
    }
  if (gkey[KEY_TAB])
    {
      fila[4][1] &= (0xFE);
      fila[4][2] &= (0xFD);
    }
  if (gkey[KEY_CAPSLOCK])
	{
		fila[1][1] &= (0xFD);
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
   * or define custom keys for the cursor.
   */
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
}


/*-----------------------------------------------------------------
 Used to count the Frames Per Second on the game.
 Do frame_counter++ after each frame draw.
------------------------------------------------------------------*/
void
count_frames (void)
{
  last_fps = frame_counter;
  frame_counter = 0;
}

END_OF_FUNCTION (count_frames);
/* When compiling under MSDOS you should comment the above line... */


/*-----------------------------------------------------------------
 Used to control the game speed. In the game loop, do like:
   do
   {
       get_keyboard_input();
       draw_one_frame_vsync_and_blit();
       frame_counter++:
       while (target_cycle > cycle)
          { do_one_game_cycle(); cycle++; }
   } while (!end_game);
------------------------------------------------------------------*/
void
target_incrementor (void)
{
  target_cycle++;
}

END_OF_FUNCTION (target_incrementor);
/* When compiling under MSDOS you should comment the above line... */

#ifndef ENABLE_LOGS
void ASprintf(char *string, ...)
{

}
#endif


// End Of Code :)

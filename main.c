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

#include "main.h"
#include "sound.h"

#include "langs.h"
#include "z80.h"
#include "snaps.h"
#include "mem.h"

#include "monofnt.h"
#include "graphics.h"
#include "debugger.h"
#include "v_alleg.h"
#include "menu.h"


tipo_emuopt emuopt = {"\0","\0","\0",GS_INACTIVE, NULL,'n',{'o', 'p', 'q', 'a', ' '} };

// hardware definitions for spectrum 48K
tipo_hwopt hwopt = { 0xFF, 24, 128, 24, 48, 224, 16, 48, 192, 48, 8,
  224, 64, 192, 56, 24, 128, 72, SPECMDL_48K };

int v_res = 240;
int v_border = 24;

// switch between display by scanlines or display at end of frame
//int displayByScanlines=1;

// generic wrapper
extern volatile char *gkey;
extern int gSoundInited;

/* Some global variables used in the emulator */
Z80Regs spectrumZ80;
FILE *tapfile;
char fname[512];
char *argvzero;
volatile int last_fps, frame_counter;
volatile int target_cycle;

/*----------------------------------------------------------------*/
int debug = 0, main_tecla, scanl = 0, quit_thread = 0; // hay_tecla,


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


// now global PENDING -> check that is this. used in line 359
char tfont[4096];

// to know if sound is ok
//extern int gSoundInited;

extern tipo_mem mem;

#define STANDAR_COPYRIGHT  "ASpectrum GNU pure C Z80 / Spectrum Emulator V " VERSION "\n" \
	  "(C) 2000-2020 Santiago Romero (NoP/Compiler), Kak & Alvaro Alea.\n" \
	  "http://aspectrum.sf.net & https://github.com/alvaroalea/aspectrum\n" \
	  "Powered by Allegro 5 - https://liballeg.org/\n" \
	  "Distributed under the terms of GNU Public License V2\n\n" \


int Z80Initialization (void){
  FILE *fp;
  /* we get memory and load font, spectrum ROM and
     possible snapshots selected in the command line */

/*
  printf("ASpectrum GNU pure C Z80 / Spectrum Emulator V " VERSION "\n"
	  "(C) 2000-2004 Santiago Romero (NoP/Compiler), Kak & Alvaro Alea.\n"
	  "http://aspectrum.sf.net\n"
	  "Powered by Allegro 4 - http://alleg.sf.net\n"
	  "Distributed under the terms of GNU Public License V2\n\n");
*/

  printf( STANDAR_COPYRIGHT );

  //fp=findopen_file("font.fnt");
  //fread (tfont, 4096, 1, fp);
  //fclose (fp);

  if (init_spectrum(hwopt.hw_model,emuopt.romfile)!=0){ 
  printf("Error al inicializar el Hardware Spectrum\n");
  exit (1);
  }
  spectrumZ80.RAM=mem.p; // por compatibilidad
  
  // init_wrapper ();

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
  static int frameskip = 0;
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
    {"model",1, NULL,'m'},
    {0, 0, 0, 0}
  };

  // needed later to find the executable dir
  argvzero=argv[0];

// first of all do the parser for options arguments
// codigo de control de argumentos pasados al programa.
//opterr=0; // pa que narices valia esto ????

#ifndef ZXDEB

#ifdef NO_GETOPTLONG
  while ((c = getopt (argc, argv, "r:s:t:hVdj:m:")) != -1)
#else
  while ((c =
	  getopt_long (argc, argv, "r:s:t:hVdj:m:", long_options, NULL)) != -1)
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
	case 'm':
	  hwopt.hw_model=optarg[0] -0x30 ;
	  break;
	case ':':
	  printf("Lack of parameters\n");
	case 'h':
	case '?':
	  printf( STANDAR_COPYRIGHT 
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
		  "	                 for each joystick, see doc for more help.\n"
		  "   -m --model num    select the model of spectrum to emulate:\n"
		  "                     num=1 => ZX Spectrum 16K\n"
		  "                     num=2 => ZX Spectrum 48K\n"
		  "                     num=3 => Inves ZX Spectrum+ 48K\n"
		  "                     num=4 => ZX Spectrum 128K\n"
		  "                     num=5 => ZX Spectrum +2\n"
		  "                     num=6 => ZX Spectrum +2A/+3\n"
		  "                     num=7 => ZX Spectrum 48K w/ Interface I (NO YET)\n"
		  "                     num=8 => ZX Spectrum 48K w/ Multiface (NO YET)\n"
		  "");
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
  if ((optind + 1) < argc){
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
//ASprintf("despues de initsystem\n");
  v_initmouse ();
//ASprintf("despues de v_initmouse\n");
  ClearScreen (7);

  // Init main variables:
  //hay_tecla = main_tecla = 0;

  init_keyboard();
//ASprintf("despues de init keyboard\n");

  initSoundLog ();		// first sound log initialization
//  ASprintf("despues de initSoundLog\n");

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

ASprintf("entrando en el bucle\n");
  
  // MAIN LOOP
  while (!done) {
	tecla = 0;

    // Read a key to interpret if available
	gupdatekeys();
	if(1==2){
	/*
    if ( gkeypressed(gKEY_ESC) || gkeypressed(gKEY_TILDE) ){
	  // call the menu and get the selected option
	  v_scaremouse ();
	  option = MainMenu (&spectrumZ80, tfont);
	  v_unscaremouse ();

      // simulate a keypress depending on the choosen option
	  switch (option) {
	    case DIALOG_DEBUGGER_0:
	      tecla = gKEY_F1;
	      debug = 0;
	      break;
	    case DIALOG_DEBUGGER_1:
	      tecla = gKEY_F1;
	      debug = 1;
	      break;
	    case DIALOG_SNAP_SAVE:
	      tecla = gKEY_F2;
	      break;
	    case DIALOG_SNAP_LOAD:
	      tecla = gKEY_F3;
	      break;
	    case DIALOG_SAVE_SCR:
	      tecla = gKEY_F4;
	      break;
	    case DIALOG_RESET:
	      tecla = gKEY_F5;
	      break;
	    case DIALOG_OPEN_TAPE:
	      tecla = gKEY_F6;
	      break;
	    case DIALOG_OPTIONS:
		  tecla = gKEY_F7; 
		  break;
		case DIALOG_HARDWARE:
		  tecla = gKEY_F9;
		  break;		
	    case DIALOG_CHANGE_LANG:
	      tecla = gKEY_F8;
	      break;
	    case DIALOG_QUIT:
	      tecla = gKEY_F10;
	      break;
	    case DIALOG_REWIND_TAPE:
		  RewindTAP (&spectrumZ80, tapfile);
		  break;
	  };
	  */
	} else {
		//FIXME check key by key
		     if (gkeypressed(gKEY_F1)) { tecla=gKEY_F1; }
		else if (gkeypressed(gKEY_F2)) { tecla=gKEY_F2; }
		else if (gkeypressed(gKEY_F3)) { tecla=gKEY_F3; }
		else if (gkeypressed(gKEY_F4)) { tecla=gKEY_F4; }
		else if (gkeypressed(gKEY_F5)) { tecla=gKEY_F5; }
		else if (gkeypressed(gKEY_F6)) { tecla=gKEY_F6; }
		else if (gkeypressed(gKEY_F7)) { tecla=gKEY_F7; }
		else if (gkeypressed(gKEY_F8)) { tecla=gKEY_F8; }
		else if (gkeypressed(gKEY_F9)) { tecla=gKEY_F9; }
		else if (gkeypressed(gKEY_F10)) { tecla=gKEY_F10; }
		else if (gkeypressed(gKEY_F11)) { tecla=gKEY_F11; }
		else if (gkeypressed(gKEY_F12)) { tecla=gKEY_F12; }
	}
	//ASprintf("tecla=%i\n",tecla);
    switch (tecla){

	  case gKEY_F2:
	    if (FileMenu (DIALOG_SNA, fname) != 0)
	      SaveSnapshot (&spectrumZ80, fname);
	    tecla = 0;
	    //debug = 1 - debug;
	    break;

	  case gKEY_F3:
	    ASprintf("lanzando dialogo...\n");
		if (FileMenu (DIALOG_SNAyC, fname) != 0){
			ASprintf ("F:%s",fname);
			LoadSnapshot (&spectrumZ80, fname);
		}
		tecla = 0;
		//debug = 1 - debug;
		break;

	  case gKEY_F4:
		if (FileMenu (DIALOG_SCR, fname) != 0)
			SaveScreenshot (&spectrumZ80, fname);
		tecla = 0;
		//debug = 1 - debug;
		break;

	  case gKEY_F5:
		reset_spectrum();
		Z80Reset (&spectrumZ80, 69888);
		tecla = 0;
		//debug = 1 - debug;
		break;

	  case gKEY_F6:
		if (FileMenu (DIALOG_TAP, fname) != 0){
			if (emuopt.tapefile[0] != 0)
				fclose (fp);
			strncpy (emuopt.tapefile, fname, 255);
			if ((fp=InitTape(fp))!= NULL){
				ASprintf("Using tape file %s.\n", emuopt.tapefile);
				tapfile = fp;
			}
		}
		tecla = 0;
		//debug = 1 - debug;
		break;

	  case gKEY_F7:
		tecla = gKEY_ESC;
		menuopciones ();
		//debug = 1 - debug;
		break;

	  case gKEY_F8:
		if (language < LANGUAGES - 1)
			language++;
		else
			language = 0;
		tecla = gKEY_ESC;
		//debug = 1 - debug;
		break;

	  case gKEY_F9:
		tecla = gKEY_ESC;
		menuhardware();
		//debug = 1 - debug;
		break;

		
	  case gKEY_F12:
		DebuggerHelp (tfont);
		tecla = gKEY_ESC << 8;
		//debug = 1 - debug;
		break;

	  case gKEY_F10:
	    // PENDING - LOCALIZE THIS.
		tecla = NULL;
		if (language == 1){
			if (v_alertYesNo ("Cerrar Aspectrum?","Esto cerrara el emulador.", "Confirme que desea cerrar el programa.") == 1)
				done = 1;
		break;
		} else {
			if (v_alertYesNo ("Close Aspectrum?","This will close the emulator.", "Please, Confirm to close.") == 1)
				done = 1;
		break;
		}
	};

    if ((tecla) == gKEY_F1){
	  if (debug == 0) {
	      ClearScreen (0);
	      gclear ();
	      Z80Dump (&spectrumZ80, tfont);
	      DrawInstruction (&spectrumZ80, tfont);
	      ShowMem (&spectrumZ80, offs, tfont);
	      DrawHelp (tfont);
	      tecla = '.';
	      debug = 1;
	  } else {
	      debug = target_cycle = 0;
	      ClearScreen (0);
	      DisplayScreen (&spectrumZ80);
	  }
	}

      // the meaning of the keyb depends on being or not in debug mode:
    switch (debug){
	case 0: 	  // emulation mode
	  f_flash2++;
	  if (f_flash2 >= 32)
	    f_flash2 = 0;
	  f_flash = (f_flash2 < 16 ? 0 : 1);
//ASprintf("P\n");
	  // if there is enough time, draw frame:
	  if (!frameskip) {
//ASprintf("1\n");
	      // no visible upper border
	      target_tstate =
		      (hwopt.ts_line * (hwopt.line_upbo + hwopt.line_poin - v_border)) - hwopt.ts_lebo;
	      current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
	      hwopt.port_ff &= 0xF0;
          if (hwopt.int_type==NORMAL) spectrumZ80.petint=1;
	      Z80Run (&spectrumZ80, target_tstate - current_tstate);
	      // visible upper border         
//ASprintf("2\n");
	      for (scanl = 0; scanl < v_border; scanl++) {
		     target_tstate += hwopt.ts_line;
		     current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		     Z80Run (&spectrumZ80, target_tstate - current_tstate);
		     displayborderscanline (scanl);
		  }

	      // Now run the emulator for all the real screen (192 lines)
//ASprintf("3\n");
          if (hwopt.int_type==INVES) spectrumZ80.petint=1;
	      for (scanl = 0; scanl < 192; scanl++) {
//ASprintf("  line %i\n",scanl);
		     // left border
		     target_tstate += hwopt.ts_lebo;
		     current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		     hwopt.port_ff &= 0xF0;
		     Z80Run (&spectrumZ80, target_tstate - current_tstate);

		     // screen
		     target_tstate += hwopt.ts_grap;
		     current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		     hwopt.port_ff |= 0x0F;
		     Z80Run (&spectrumZ80, target_tstate - current_tstate);

   		  // right border
		     target_tstate += (hwopt.ts_ribo + hwopt.ts_hore);
		     current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		     hwopt.port_ff &= 0xF0;
		     Z80Run (&spectrumZ80, target_tstate - current_tstate);

   		  displayscanline2 (scanl, f_flash, &spectrumZ80);
		  }

	      // visible bottom border
//ASprintf("4\n");
	      hwopt.port_ff &= 0xF0;
	      for (scanl = 192 + v_border; scanl < v_res; scanl++) {
		  target_tstate += hwopt.ts_line;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);
		  displayborderscanline (scanl);
		  }
//ASprintf("5\n");
	      // the last lines (56+16 lines - border)
	      // Run it for 56 lines covering bottom border and ray return
	      Z80Run (&spectrumZ80, spectrumZ80.ICount);
//ASprintf("6\n");

	      dumpVirtualToScreen ();

//ASprintf("7\n");
	    } else {
	      // If we have not enough time, don't draw the screen,
	      // only emulate

	      // no visible upper border
	      target_tstate =
		(hwopt.ts_line * (hwopt.line_upbo + hwopt.line_poin)) -
		hwopt.ts_lebo;
	      current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
	      hwopt.port_ff &= 0xF0;
	      Z80Run (&spectrumZ80, target_tstate - current_tstate);

	      // Now run the emulator for all the real screen (192 lines)
	      for (scanl = 0; scanl < 192; scanl++)
		{
		  // left border
		  target_tstate += hwopt.ts_lebo;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff &= 0xF0;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);

		  // Screen
		  target_tstate += hwopt.ts_grap;
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff |= 0x0F;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);

		  // right border 
		  target_tstate += (hwopt.ts_ribo + hwopt.ts_hore);
		  current_tstate = spectrumZ80.IPeriod - spectrumZ80.ICount;
		  hwopt.port_ff &= 0xF0;
		  Z80Run (&spectrumZ80, target_tstate - current_tstate);
		}
	      // visible bottom border
	      hwopt.port_ff &= 0xF0;
	      Z80Run (&spectrumZ80, spectrumZ80.ICount);
	    }
//ASprintf("8\n");
	  // Speed control without sound

	  if (gSoundInited){
	    soundDump();
	  }
/*
	    while (target_cycle == 0);
	  else
	    {
	      gSoundSync ();	// wait for 1/50th
	      soundDump ();
	      target_cycle = 0;
	    }
*/
//ASprintf("9\n");
	  //target_cycle--;
	  //frame_counter++;
	  v_framecheck();
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
	      //PENDING readkey ();
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
	    //PENDING  while (!keypressed () &&
		while ( spectrumZ80.PC.W != spectrumZ80.TrapAddress)
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
	    //PENDING  while (spectrumZ80.ICount > 50 && !keypressed ())
		  while (spectrumZ80.ICount > 50 )
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

    }				// while (!done)
  return (1);
}

//END_OF_MAIN ();
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
 Used to count the Frames Per Second on the game.
 Do frame_counter++ after each frame draw.
------------------------------------------------------------------*/
//void count_frames (void){
// last_fps = frame_counter;
//  frame_counter = 0;
//}

//END_OF_FUNCTION (count_frames);
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
//void target_incrementor (void){
//  target_cycle++;
//}

//END_OF_FUNCTION (target_incrementor);
/* When compiling under MSDOS you should comment the above line... */

// Check Main.h for #ifdef ENABLE_LOGS
#ifndef ENABLE_LOGS
void ASprintf(char *string, ...){ } // ignore if no logs.
#endif

// End Of Code :)
/*=====================================================================
  menu.h -> Header file for menu.c.

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
  
#ifndef _MENU_H_
#define _MENU_H_

#define D_EXIT 0
#define D_SELECTED 1
#define D_O_K 2
#define D_CLOSE 3

enum filedialogconst { DIALOG_SNAyC=0, DIALOG_SNA, DIALOG_SCR, DIALOG_TAP };

enum dialogconst { DIALOG_DEBUGGER_0=0, DIALOG_DEBUGGER_1, DIALOG_SNAP_SAVE, 
		DIALOG_SNAP_LOAD, DIALOG_QUIT, DIALOG_RESET, DIALOG_SAVE_SCR, 
	    DIALOG_OPEN_TAPE, DIALOG_CHANGE_LANG, DIALOG_REWIND_TAPE, 
	    DIALOG_REFERENCEKEYS, DIALOG_ABOUT, DIALOG_OPTIONS, DIALOG_HARDWARE} ;

int MainMenu (Z80Regs * regs, char *tfont);

void DrawSelected (int x1, int y1, int x2, int y2, char *text, int bgcolor,int fgcolor, int textbgselcolor, char *tfont);

int FileMenu (char type, char *filename);

int menuopciones (void);

int menuhardware (void);

int v_alertYesNo (const char *Title, const char *Head, const char *Text);

int v_alertErrOK (const char *Title, const char *Head, const char *Text);

#endif	/*  */

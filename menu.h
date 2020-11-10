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

int MainMenu (Z80Regs * regs, char *tfont);

void DrawSelected (int x1, int y1, int x2, int y2, char *text, int bgcolor,
		    
int fgcolor, int textbgselcolor, char *tfont);

int FileMenu (char *tfont, char type, char *filename);

int menuopciones (void);

#define DIALOG_SNAyC  0
#define DIALOG_SNA 1
#define DIALOG_SCR 2
#define DIALOG_TAP 3
#define DIALOG_DEBUGGER_0  0
#define DIALOG_DEBUGGER_1  1
#define DIALOG_SNAP_SAVE   2
#define DIALOG_SNAP_LOAD   3
#define DIALOG_QUIT        4
#define DIALOG_RESET       5
#define DIALOG_SAVE_SCR    6
#define DIALOG_OPEN_TAPE   7
#define DIALOG_CHANGE_LANG 8
#define DIALOG_REWIND_TAPE 9
#define DIALOG_REFERENCEKEYS 10
#define DIALOG_ABOUT 11
#endif	/* 
 */

/*=====================================================================
  Debugger.h -> Header file for debugger.c.

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

void DrawInstruction (Z80Regs *, char *);

void GetHexValue (int, int, char *, char *, char *, int, int, int);

//void ClearScreen (char);

void DrawHelp (char *);

void ShowMem (Z80Regs *, int, char *);

void Z80Dump (Z80Regs *, char *);

int DebuggerHelp (char *tfont);




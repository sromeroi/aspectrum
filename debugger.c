#include "stdafx.h"
/*=====================================================================
  Debugger.c -> This file includes all the debugging functions
                for the emulator, called from the main loop.

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
#include "z80.h"
#include "debugger.h"
#include "langs.h"

extern int language;

/*-----------------------------------------------------------------
 With GetHexValue() the debugger can read an hex value from the
 keyboard and assign it to any Z80 variable...
------------------------------------------------------------------*/
void
GetHexValue (int x, int y, char *str, char *result,
	     char *font, int fg_color, int bg_color, int max)
{
  GFXprintf (x, y, str, font, fg_color, bg_color, -1);
  GFXgets (((strlen (str) + 1) * 7) + x, y, result, font, fg_color, bg_color,
	   max);
  GFXprintf (x, y, "                                  ", font, fg_color,
	     bg_color, -1);
}


/*-----------------------------------------------------------------
 This function draws the currect [PC] instruction and the next
 4 instructions in the main debugger screen.
------------------------------------------------------------------*/
void
DrawInstruction (Z80Regs * regs, char *font)
{
  char b[1024];
  int i, sizeinst, newpc;
  char opcode_data[256];
  char instruction[256];

#define INSTR_START_ROW  60
  Z80Regs newregs;
  memcpy (&newregs, regs, sizeof (newregs));

//      sizeinst = XXXX( instruction, regs->PC.W, regs );
  sizeinst = Z80Disassembler (&newregs, instruction, opcode_data);
  GFXprintf (2, INSTR_START_ROW, "  ", font, 6, 0, -1);
  sprintf (b, "-> %04Xh %-15s", regs->PC.W, instruction);
  GFXprintf (2, INSTR_START_ROW, b, font, 6, 0, -1);

  newpc = regs->PC.W;
  for (i = 0; i < 5; i++)
    {
      newpc += sizeinst;
      newregs.PC.W = newpc;
//       sizeinst = XXXX( instruction, newpc, regs );
      sizeinst = Z80Disassembler (&newregs, instruction, opcode_data);
      sprintf (b, "   %04Xh %-15s", newpc, instruction);
      GFXprintf (2, INSTR_START_ROW + 10 + (10 * i), b, font, 6, 0, -1);
    }
}


/*-----------------------------------------------------------------
 ShowMem() draws the memory table, and the [SP] and [PC] contents
 to the debugger screen.
------------------------------------------------------------------*/
void
ShowMem (Z80Regs * regs, int offset, char *font)
{
  char b[256];
  int i;
//    char instruccion[256];

  /* Draw on the screen the memory contents */
  GFXprintf (319 - (15 * 7), 12, "Mem:", font, 7, 0, -1);
  GFXprintf (319 - (15 * 7), 12 + ((9) * 9), "(SP)", font, 2, 0, -1);
  GFXprintf (319 - (15 * 7), 12 + ((12) * 9), "(PC)", font, 3, 0, -1);

  /* display 8 words of memory */
  for (i = 0; i < 9; i++)
    {
      sprintf (b, "%04X %04X", offset + (i * 2),
	       (Z80MemRead (offset + (i * 2), regs)) +
	       (Z80MemRead (offset + (i * 2) + 1, regs) << 8));
      GFXprintf (319 - (10 * 7), 12 + (i * 9), b, font, 7, 0, -1);
    }

  /* draw 3 words of memory pointed by SP */
  for (i = 0; i < 3; i++)
    {
      sprintf (b, "%04X %04X", regs->SP.W + (i * 2),
	       (Z80MemRead (regs->SP.W + (i * 2), regs)) +
	       (Z80MemRead (regs->SP.W + (i * 2) + 1, regs) << 8));
      GFXprintf (319 - (10 * 7), 12 + ((i + 9) * 9), b, font, 2, 0, -1);
    }

  /* draw 2 words pointed by PC (to see opcodes) */
  for (i = 0; i < 2; i++)
    {
      sprintf (b, "%04X %04X", regs->PC.W + (i * 2),
	       (Z80MemRead (regs->PC.W + (i * 2), regs)) +
	       (Z80MemRead (regs->PC.W + (i * 2) + 1, regs) << 8));
      GFXprintf (319 - (10 * 7), 12 + ((i + 12) * 9), b, font, 3, 0, -1);
    }

}


/*-----------------------------------------------------------------
 This function draws the debugger main and status bar.
 Is a simple help of the debbuger commands.
------------------------------------------------------------------*/
void
DrawHelp (char *font)
{
  GFXprintf (2, 20, lang_debugger_help1[language], font, 7, 0, -1);
  GFXprintf (2, 2, lang_debugger_title[language], font, 0, 7, -1);
  GFXprintf (2, 199 - 18, lang_debugger_status1[language], font, 6, 2, -1);
  GFXprintf (2, 199 - 9, lang_debugger_status2[language], font, 6, 2, -1);
}


/*-----------------------------------------------------------------
  void Z80Dump( Z80Regs *regs );
  Dumps the register's contents ... (for debugging purposes).
------------------------------------------------------------------*/
void
Z80Dump (Z80Regs * regs, char *font)
{
  char b[1024];
#define INICIOz80dump 142

  sprintf (b, " AF =%04Xh  BC =%04Xh  DE =%04Xh  HL =%04Xh  ",
	   regs->AF.W, regs->BC.W, regs->DE.W, regs->HL.W);
  GFXprintf (2, INICIOz80dump, b, font, 7, 1, -1);
  sprintf (b, " AF\'=%04Xh  BC\'=%04Xh  DE\'=%04Xh  HL\'=%04Xh  ",
	   regs->AFs.W, regs->BCs.W, regs->DEs.W, regs->HLs.W);
  GFXprintf (2, INICIOz80dump + 9, b, font, 7, 1, -1);
  sprintf (b, " PC =%04Xh  IX =%04Xh  IY =%04Xh  SP =%04Xh  ",
	   regs->PC.W, regs->IX.W, regs->IY.W, regs->SP.W);
  GFXprintf (2, INICIOz80dump + 18, b, font, 7, 1, -1);
  sprintf (b, " R  =%02Xh    I  =%02Xh    Cy =%05d  IM =%d      ",
	   regs->R.W, regs->I, regs->ICount, regs->IM);
  GFXprintf (2, INICIOz80dump + 27, b, font, 7, 1, -1);

}


/*-----------------------------------------------------------------
 * This function implements the help screen. 
 ------------------------------------------------------------------*/
int
DebuggerHelp (char *tfont)
{
  int i;
//   int fontw = 8;
  int fonth = 12;
  int menux = 6, menuy = 4, menuw = 305, menuh = 191;
  int bgcolor = 15, titlecolor = 14;
  int fgcolor = 0;

  gbox (menux, menuy, menux + menuw, menuy + menuh, bgcolor);
  grectangle (menux + 1, menuy + 1, menux + menuw - 1, menuy + menuh - 1,
	      fgcolor);
  gbox (menux + 1, menuy + 1, menux + menuw - 1, menuy + 1 + fonth, fgcolor);
  GFXprintf_tovideo (menux + 7, menuy + 4, lang_debugger_helptitle[language],
		     tfont, titlecolor, fgcolor, 0);

  for (i = 0; i < NUM_DEBUGGER_HELPLINES; i++)
    GFXprintf_tovideo (menux + 12, (fonth * i) + 32,
		       lang_debugger_text[(language *
					   NUM_DEBUGGER_HELPLINES) + i],
		       tfont, fgcolor, bgcolor, -1);
  dumpVirtualToScreen ();
  readkey ();

  return 1;
}

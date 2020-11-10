/*=====================================================================
 langs.h header file.

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

#ifndef _LANGS_H_
#define _LANGS_H_

#define LANGUAGES       3

/* MAIN.C STRINGS: */
extern char lang_runto_t[LANGUAGES][80];
extern char lang_runto_w[LANGUAGES][80];
extern char lang_poke[LANGUAGES][80];
extern char lang_value[LANGUAGES][80];
extern char lang_change_af[LANGUAGES][80];
extern char lang_change_bc[LANGUAGES][80];
extern char lang_change_de[LANGUAGES][80];
extern char lang_change_hl[LANGUAGES][80];
extern char lang_change_af2[LANGUAGES][80];
extern char lang_change_bc2[LANGUAGES][80];
extern char lang_change_de2[LANGUAGES][80];
extern char lang_change_hl2[LANGUAGES][80];
extern char lang_change_sp[LANGUAGES][80];
extern char lang_change_ix[LANGUAGES][80];
extern char lang_change_iy[LANGUAGES][80];
extern char lang_change_mem[LANGUAGES][80];

/* DEBUGGER STRINGS: */
extern char lang_debugger_help1[LANGUAGES][80];
extern char lang_debugger_title[LANGUAGES][80];
extern char lang_debugger_status1[LANGUAGES][80];
extern char lang_debugger_status2[LANGUAGES][80];

#define NUM_DEBUGGER_HELPLINES 13
extern char lang_debugger_helptitle[LANGUAGES][80];
extern char lang_debugger_text[LANGUAGES * NUM_DEBUGGER_HELPLINES][80];

/* MENU.C STRINGS:  */
#define NUM_MENU_OPTIONS      10
extern char lang_menu_title[LANGUAGES][80];
extern char lang_main_options[NUM_MENU_OPTIONS * LANGUAGES][80];

#define FILEBOX_TYPES 4
extern char lang_filemenu[FILEBOX_TYPES * LANGUAGES][80];
extern char lang_generaloptions[LANGUAGES][30];
extern char lang_emulagunstick[LANGUAGES][30];
extern char lang_soundactive[LANGUAGES][20];

#endif

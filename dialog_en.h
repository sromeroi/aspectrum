/*=====================================================================

 English dialogs 

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



MENU options_menu_EN[] = {
  {"&Snapshot", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Tape", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Video", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Audio", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Options\tF7", opciones_proc, NULL, 0, NULL},
  {"&Hardware", hardware_proc, NULL, 0, NULL},
  {"&Recording", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Change Language\tF8", changelang_proc, NULL, 0, NULL},
  {0}
};

MENU file_menu_EN[] = {
  {"&Save snapshot\tF2", save_proc, NULL, 0, NULL},
  {"&Load snapshot\tF3", load_proc, NULL, 0, NULL},
  {"&Save SCR screenshot\tF4", savescr_proc, NULL, 0, NULL},
  {"", NULL, NULL, 0, NULL},
  {"&Exit\tF10", quit_proc, NULL, 0, NULL},
  {0}
};

MENU tape_menu_EN[] = {
  {"&Open Tape\tF6", opentape_proc, NULL, 0, NULL},
  {"&Browse Tape", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Rewind Tape", rewindtape_proc, NULL, 0, NULL},
  {"&Play", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Stop", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Record", dummy_proc, NULL, D_DISABLED, NULL},
  {0}
};

MENU machine_menu_EN[] = {
  {"&Run debugger\tF1", debugger_proc, NULL, 0, NULL},
  {"&Reset\tF5", reset_proc, NULL, 0, NULL},
  {"&NMI", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Select hardware", dummy_proc, NULL, 0, NULL},
  {0}
};

MENU help_menu_EN[] = {
	{"&Keyboard Reference",referencehelp_proc,NULL,0,NULL},
	{"&About Aspectrum",about_proc,NULL,0,NULL},
	{0}
};

MENU main_menu_EN[] = {
  {"&File", dummy_proc, file_menu_EN, 0, NULL},
  {"&Machine", dummy_proc, machine_menu_EN, 0, NULL},
  {"&Options", dummy_proc, options_menu_EN, 0, NULL},
  {"&Tape", dummy_proc, tape_menu_EN, 0, NULL},
  {"&Help", dummy_proc, help_menu_EN, 0, NULL},
  {0}
};

DIALOG menu_dlg_EN[] = {
  /* (proc)           (x) (y)  (w)  (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)                       */
  {d_menu_proc, 1, 1, 319, 11, 14, 17, 0, 0, 0, 0, main_menu_EN, NULL, NULL},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F1 << 8, 0, 0, 0, debugger_proc},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F2 << 8, 0, 0, 0, load_proc},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F3 << 8, 0, 0, 0, save_proc},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F4 << 8, 0, 0, 0, savescr_proc},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F5 << 8, 0, 0, 0, reset_proc},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F6 << 8, 0, 0, 0, opentape_proc},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F8 << 8, 0, 0, 0, changelang_proc},
  {d_keyboard_proc, 0, 0, 0, 0, 0, 0, gKEY_F10 << 8, 0, 0, 0, quit_proc},
  {NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL}
};

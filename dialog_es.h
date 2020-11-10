/*=====================================================================

 Spanish dialogs 

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


MENU options_menu_ES[] = {
  {"&Snapshots", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Cinta", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Video", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Audio", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Opciones\tF7", opciones_proc, NULL, 0, NULL},
  {"&Hardware", hardware_proc, NULL, 0, NULL},
  {"&Grabaciones", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Cambiar lenguaje\tF8", changelang_proc, NULL, 0, NULL},
  {0}
};

MENU file_menu_ES[] = {
  {"&Salvar snapshot\tF2", save_proc, NULL, 0, NULL},
  {"&Cargar snapshot\tF3", load_proc, NULL, 0, NULL},
  {"&Salvar imagen SCR\tF4", savescr_proc, NULL, 0, NULL},
  {"", NULL, NULL, 0, NULL},
  {"&Salir\tF10", quit_proc, NULL, 0, NULL},
  {0}
};

MENU tape_menu_ES[] = {
  {"&Abrir cinta\tF6", opentape_proc, NULL, 0, NULL},
  {"&Ver cinta", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Rebobinar cinta", rewindtape_proc, NULL, 0, NULL},
  {"&Play", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Stop", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Grabar", dummy_proc, NULL, D_DISABLED, NULL},
  {0}
};

MENU machine_menu_ES[] = {
  {"&Ejecutar debugger\tF1", debugger_proc, NULL, 0, NULL},
  {"&Reset\tF5", reset_proc, NULL, 0, NULL},
  {"&NMI", dummy_proc, NULL, D_DISABLED, NULL},
  {"&Elegir hardware", dummy_proc, NULL, 0, NULL},
  {0}
};

MENU help_menu_ES[] = {
	{"&Chuleta del Teclado",referencehelp_proc,NULL,0,NULL},
	{"&Acerca de Aspectrum",about_proc,NULL,0,NULL},
	{0}
};


MENU main_menu_ES[] = {
  {"&Fichero", dummy_proc, file_menu_ES, 0, NULL},
  {"&Maquina", dummy_proc, machine_menu_ES, 0, NULL},
  {"&Opciones", dummy_proc, options_menu_ES, 0, NULL},
  {"&Cinta", dummy_proc, tape_menu_ES, 0, NULL},
  {"&Ayuda", dummy_proc, help_menu_ES, 0, NULL},
  {0}
};

DIALOG menu_dlg_ES[] = {
  /* (proc)           (x) (y)  (w)  (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)                       */
  {d_menu_proc, 1, 1, 319, 11, 14, 17, 0, 0, 0, 0, main_menu_ES, NULL, NULL},
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

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
static char lang_runto_t[LANGUAGES][80] = {
  "Run to Address (hex):",
  "Ejecutar hasta (hex):",
  "Executar fins  (hex):"
};

static char lang_runto_w[LANGUAGES][80] = {
  "Trap Address (hex):",
  "Punto de ruptura (hex):",
  "Punt de ruptura (hex):"
};

static char lang_poke[LANGUAGES][80] = {
  "Address to poke (hex):",
  "Cambiar direccion (hex):",
  "Canviar direccio (hex):"
};

static char lang_value[LANGUAGES][80] = {
  "Value (byte, hex):",
  "Valor (byte, hex):",
  "Valor (byte, hex):"
};

static char lang_change_af[LANGUAGES][80] = {
  "Change AF register (hex):",
  "Cambiar registro AF (hex):",
  "Canviar registre AF (hex):"
};

static char lang_change_bc[LANGUAGES][80] = {
  "Change BC register (hex):",
  "Cambiar registro BC (hex):",
  "Canviar registre BC (hex):"
};

static char lang_change_de[LANGUAGES][80] = {
  "Change DE register (hex):",
  "Cambiar registro DE (hex):",
  "Canviar registre DE (hex):"
};

static char lang_change_hl[LANGUAGES][80] = {
  "Change HL register (hex):",
  "Cambiar registro HL (hex):",
  "Canviar registre HL (hex):"
};

static char lang_change_af2[LANGUAGES][80] = {
  "Change AF' register (hex):",
  "Cambiar registro AF' (hex):",
  "Canviar registre AF' (hex):"
};

static char lang_change_bc2[LANGUAGES][80] = {
  "Change BC' register (hex):",
  "Cambiar registro BC '(hex):",
  "Canviar registre BC' (hex):"
};

static char lang_change_de2[LANGUAGES][80] = {
  "Change DE' register (hex):",
  "Cambiar registro DE' (hex):",
  "Canviar registre DE' (hex):"
};

static char lang_change_hl2[LANGUAGES][80] = {
  "Change HL' register (hex):",
  "Cambiar registro HL' (hex):",
  "Canviar registre HL' (hex):"
};


static char lang_change_sp[LANGUAGES][80] = {
  "Change SP register (hex):",
  "Cambiar registro SP (hex):",
  "Canviar registre SP (hex):"
};

static char lang_change_ix[LANGUAGES][80] = {
  "Change IX register (hex):",
  "Cambiar registro IX (hex):",
  "Canviar registre IX (hex):"
};

static char lang_change_iy[LANGUAGES][80] = {
  "Change IY register (hex):",
  "Cambiar registro IY (hex):",
  "Canviar registre IY (hex):"
};

static char lang_change_mem[LANGUAGES][80] = {
  "Go to mem address (hex):",
  "Direccion de memoria (hex):",
  "Direccio de memoria (hex):"
};



/* DEBUGGER STRINGS: */

static char lang_debugger_help1[LANGUAGES][80] = {
  "   ADDR  OPCODE",
  "   DIREC OPCODE",
  "   DIREC OPCODE"
};

static char lang_debugger_title[LANGUAGES][80] = {
  " ASpectrum Debugger     - Allegro frontend   ",
  " Depurador de ASpectrum - Frontend Allegro   ",
  " Depurador de ASpectrum - Frontend Allegro   "
};

static char lang_debugger_status1[LANGUAGES][80] = {
  "                                             ",
  "                                             ",
  "                                             "
};

static char lang_debugger_status2[LANGUAGES][80] = {
  " F12 = Aspectrum Debugger keybindings & help ",
  " F12 = Teclas y ayuda del depurador integrado",
  " F12 = Ajuda i tecles del depurador integrat "
};

#define NUM_DEBUGGER_HELPLINES 13

static char lang_debugger_helptitle[LANGUAGES][80] = {
  "Aspectrum Debugger Help",
  "Ayuda del depurador de Aspectrum",
  "Ajuda del depurador de Aspectrum"
};

static char lang_debugger_text[LANGUAGES * NUM_DEBUGGER_HELPLINES][80] = {
  "ESC = Exit debugger   F10 = Exit emulator",
  "F5 = Reset Z80 CPU      p = Poke memory  ",
  " ",
  "a,b,c,d,h,y = Change AF,BC,DE,HL,IX,IY   ",
  "A,B,C,D,H,S = Change AF',BC',DE',HL',SP  ",
  " ",
  "n,m,l = Move memory window start address ",
  "t = RunTo address    w = Runto simulating",
  "i = Run to Cycles<100    s = Show screen ",
  " ",
  "Enter/Space = execute current instruction",
  " ",
  "             F1 = Main menu              ",

  "ESC = Cerrar depurador        F10 = Salir",
  "F5 = Resetear Z80     p = Cambiar memoria",
  " ",
  "a,b,c,d,h,y = Cambiar AF,BC,DE,HL,IX,IY  ",
  "A,B,C,D,H,S = Cambiar AF',BC',DE',HL',SP ",
  " ",
  "n,m,l = Mover ventana de memoria         ",
  "t = Ejecutar hasta    w = Ejec. simulando",
  "i = Ir a Cycles<100      s = ver pantalla",
  " ",
  "Enter/Espacio = ejecutar instruc. actual ",
  " ",
  "           F1 = menu principal           ",

  "ESC = Tancar depurador        F10 = Eixir",
  "F5 = Resetejar Z80    p = Canviar memoria",
  " ",
  "a,b,c,d,h,y = Canviar AF,BC,DE,HL,IX,IY  ",
  "A,B,C,D,H,S = Canviar AF',BC',DE',HL',SP ",
  " ",
  "n,m,l = Moure finestra de memoria        ",
  "t = Executar fins      w = Exec. simulant",
  "i = Anar a Cycles<100   s = vore pantalla",
  " ",
  "Enter/Espai = executar instruccio actual ",
  " ",
  "           F1 = menu principal           "
};


/* MENU.C STRINGS:  */

#define NUM_MENU_OPTIONS      10

static char lang_menu_title[LANGUAGES][80] = {
  "ASpectrum Emulator: Main Menu",
  "Emulador ASpectrum: Menu Principal",
  "Emulador ASpectrum: Menu Principal"
};

static char lang_main_options[NUM_MENU_OPTIONS * LANGUAGES][80] = {
  "Debugger                     ESC",
  "Return to emulator            F1",
  "Save snapshot                 F2",
  "Load snapshot                 F3",
  "Save screenshot               F4",
  "Reset                         F5",
  "Tape Options                  F6",
  "Generic Options               F7",
  "Change language               F8",
  "Exit emulator                F10",

  "Depurador de codigo          ESC",
  "Volver al emulador            F1",
  "Grabar snapshot               F2",
  "Cargar snapshot               F3",
  "Grabar captura de pantalla    F4",
  "Resetear                      F5",
  "Opciones de cinta             F6",
  "Opciones generales            F7",
  "Cambiar idioma                F8",
  "Salir del emulador           F10",

  "Depurador de codi            ESC",
  "Tornar al emulador            F1",
  "Salvar snapshot               F2",
  "Cargar snapshot               F3",
  "Salvar captura de pantalla    F4",
  "Resetejar                     F5",
  "Opcions de cinta              F6",
  "Opcions generals              F7",
  "Canviar idioma                F8",
  "Eixir del emulador           F10"
};

#define FILEBOX_TYPES 4

static char lang_filemenu[FILEBOX_TYPES * LANGUAGES][80] = {
  "Select SNA/SP/Z80 file to load",
  "Select SNA file to save",
  "Select SCR file to save screen",
  "Select TAP file for LOAD \"\"",
  "Seleccione SNA/SP/Z80 a cargar",
  "Seleccione fichero SNA a grabar",
  "Seleccione fichero SCR a grabar",
  "Seleccione TAP para LOAD \"\"",
  "Seleccione SNA, SP o Z80 a obrir",
  "Seleccione fitxer SNA a salvar",
  "Seleccione fitxer SCR a salvar",
  "Seleccione TAP per a LOAD \"\""
};

static char lang_generaloptions[LANGUAGES][30] = {
  "General Options",
  "Opciones Generales",
  "Opcions generals"
};

static char lang_emulagunstick[LANGUAGES][30] = {
  "&Gunstick Emulation",
  "Emulacion de &Gunstick",
  "Emulacion de &Gunstick"
};

static char lang_soundactive[LANGUAGES][20] = {
  "&Sound active",
  "&Sonido activo",
  "&Sonido activo"
};



#endif

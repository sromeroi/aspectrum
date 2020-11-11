/*=====================================================================
  snaps.h    -> Header file for snaps.c.

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

#define FALSE 0
#define TRUE 1
enum tipos_archivo { TYPE_NULL=0, TYPE_TZX ,TYPE_TAP, TYPE_Z80, TYPE_SNA,
			TYPE_SP, TYPE_SCR }; 
int typeoffile(char *);


char LoadSnapshot (Z80Regs * regs, char *filename);
char LoadSP (Z80Regs *, FILE *);
char LoadSNA (Z80Regs *, FILE *);
char LoadZ80 (Z80Regs *, FILE *);
char LoadSCR (Z80Regs *, FILE *);

char SaveSP (Z80Regs *, FILE *);
char SaveSNA (Z80Regs *, FILE *);
char SaveZ80 (Z80Regs *, FILE *);
char SaveSCR (Z80Regs *, FILE *);
char SaveSnapshot (Z80Regs * regs, char *filename);
char SaveScreenshot (Z80Regs * regs, char *filename);


/* RUTINAS DE CINTA: GENERICAS */
FILE *InitTape(FILE *fp);
char LoadTAP (Z80Regs *, FILE *);
char RewindTAP (Z80Regs *, FILE *);

/* RUTINAS DE CINTA: ESPECIFICAS */
char TAP_init(FILE *fp);
char TAP_loadblock(Z80Regs * regs, FILE * fp);
char TAP_rewind(FILE *fp);

char TZX_init(FILE *fp);
char TZX_loadblock(Z80Regs * regs, FILE * fp);
char TZX_rewind();
char TZX_genindex(FILE *fp);

/* busca el archivo en los sitios habituales y(opcional) lo abre como rb */
FILE *findopen_file(char *file);
char *find_file(char *file);

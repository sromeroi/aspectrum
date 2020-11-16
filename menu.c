/*=====================================================================
  menu.c ->    This file includes all the menu functions
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
#include "v_alleg.h"
#include <allegro5/allegro_native_dialog.h>
#include "z80.h"
#include "langs.h"
#include "menu.h"
#include "monofnt.h"
#include "main.h"
#include "snaps.h"
#include "mem.h"
extern ALLEGRO_DISPLAY *display;
extern int language;

#ifdef NO_USE_MENU
int MainMenu (Z80Regs * regs, char *tfont){ return 0; }
void DrawSelected (int x1, int y1, int x2, int y2, char *text, int bgcolor, int fgcolor, int textbgselcolor, char *tfont){ return 0; }
int menuopciones (void){ return 0; }
int menuhardware (void){ return 0; }


/*---------------------------------------------------------------------------
// Filebox selection popup :-), it receives the Type of popup (Load SNA,
// Save SNA, Save SCR...) and returns the selected filename.
*--------------------------------------------------------------------------*/
//int FileMenu (char *tfont, char type, char *filename){ return 0; }
int FileMenu (char type, char *filename){
  extern tipo_emuopt emuopt;
  int ret;
  ALLEGRO_FILECHOOSER *dialogo;

  const char extensions[FILEBOX_TYPES][80] = {
    "SNA;SP;Z80;SCR",  //carga
    "SCR",
    "SNA;SP;Z80;SCR",  //graba
    "TAP;TZX"          //cintas
  };
  const int mode[FILEBOX_TYPES]={
    ALLEGRO_FILECHOOSER_FILE_MUST_EXIST,
    ALLEGRO_FILECHOOSER_SAVE,
    ALLEGRO_FILECHOOSER_SAVE,
    0
  };
  /*
  if (emuopt.gunstick & GS_GUNSTICK)
    set_mouse_sprite (NULL);
  */
  ASprintf("Creando dialogo\n");
  dialogo = al_create_native_file_dialog(NULL,lang_filemenu[(language * FILEBOX_TYPES) + type],extensions[type],mode[type]);
  ret =  al_show_native_file_dialog(display, dialogo);
  al_destroy_native_file_dialog(dialogo);
  ASprintf("Destruyendo dialogo\n");
  
  //ret = file_select_ex (lang_filemenu[(language * FILEBOX_TYPES) + type], filename, extensions[type] ,512, 290, 170);

  // si usamos gunstick volvemos a poner el punto de mira 
  /*
  if (emuopt.gunstick & GS_GUNSTICK){
      set_mouse_sprite (emuopt.raton_bmp);
      set_mouse_sprite_focus (8, 8);
  }
  */
  return (ret);
}


#else  // def NO_USE_MENU

#ifdef I_HAVE_AGUP
#include <agup.h>
//#include <agtk.h>
//#include <aphoton.h>
//#include <awin95.h>
//#include <aase.h>
#define GUI_check_proc d_agup_check_proc
#define GUI_radio_proc d_agup_radio_proc
#else
#define GUI_check_proc d_check_proc
#define GUI_radio_proc d_radio_proc
#endif


#define NUM_FILES 9

extern int language;
static volatile int selected_opt = -1;

int dummy_proc (void)
{
  if (language == 0)
    galert ("Still unimplemented", NULL, NULL, "Ok", NULL, 0, 0);
  else if (language == 1)
    galert ("Todavia sin implementar", NULL, NULL, "Ok", NULL, 0, 0);
  return D_O_K;
}

int quit_proc (void)
{
  selected_opt = DIALOG_QUIT;
  return D_CLOSE;
}

int load_proc (void)
{
  selected_opt = DIALOG_SNAP_LOAD;
  return D_CLOSE;
}

int save_proc (void)
{
  selected_opt = DIALOG_SNAP_SAVE;
  return D_CLOSE;
}

int debugger_proc (void)
{
  selected_opt = DIALOG_DEBUGGER_0;
  return D_CLOSE;
}

int reset_proc (void)
{
  selected_opt = DIALOG_RESET;
  return D_CLOSE;
}

int savescr_proc (void)
{
  selected_opt = DIALOG_SAVE_SCR;
  return D_CLOSE;
}

int opentape_proc (void)
{
  selected_opt = DIALOG_OPEN_TAPE;
  return D_CLOSE;
}

int rewindtape_proc (void)
{
  selected_opt = DIALOG_REWIND_TAPE;
  return D_CLOSE;
}
int opciones_proc (void)
{
  selected_opt = DIALOG_OPTIONS;
  return D_CLOSE;
}
int hardware_proc (void)
{
  selected_opt = DIALOG_HARDWARE;
  return D_CLOSE;
}

int changelang_proc (void)
{
  selected_opt = DIALOG_CHANGE_LANG;
  return D_CLOSE;
}



int about_proc (void)
{
   DIALOG dlg[] ={
      { d_agup_window_proc, 0, 0, 250, 100, 0, 0, 0, 0, 0, 0,(void *)"Acerca de...", NULL, NULL },
      { d_ctext_proc, 125, 30, 0, 0, agup_fg_color, agup_bg_color, 0, 0, 0, 0, (void *)"Aspectrum Version: "VERSION, NULL, NULL },
      { d_ctext_proc, 125, 40, 0, 0, agup_fg_color, agup_bg_color, 0, 0, 0, 0, (void *)"(C) 2000-2003 Santiago Romero, Kak y Alvaro Alea", NULL, NULL },
      { d_ctext_proc, 125, 50, 0, 0, agup_fg_color, agup_bg_color, 0, 0, 0, 0, (void *)"Distribuido bajo licencia GPL V2", NULL, NULL },
      {gui_button_proc, 85, 75, 70, 16, 0, 0, 13, D_EXIT, 0, 0, "OK", NULL, NULL},
      { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }

    };

   centre_dialog(dlg);
   do_dialog(dlg, -1);
   return D_CLOSE;

/*  alert("Aspectrum Version: "VERSION,"(C) 2000-2003 Santiago Romero, Kak y Alvaro Alea",
	"Distribuido bajo licencia GPL V2","OK",NULL,13,27);
  selected_opt = DIALOG_ABOUT;
  return D_REDRAW;
*/

}



/*-----------------------------------------------------------------
 * This function implements the main options menu.
 ------------------------------------------------------------------*/
int
MainMenu (Z80Regs * regs, char *tfont)
{

//  int i, end = 0, keypress;
  int selected = 0;
//  int fonth = 12;
//  int menux = 20, menuy = 15, menuw = 280, menuh = 170;
//  int bgcolor = 15, titlecolor = 14;
//  int bgselcolor = 11, textbgselcolor = 13;
//  int fgcolor = 0;

#include "dialog_en.h"
#include "dialog_es.h"
  selected_opt=-1;
  selected = -1;
  set_dialog_color (menu_dlg_EN, gui_fg_color, gui_bg_color);
  set_dialog_color (menu_dlg_ES, gui_fg_color, gui_bg_color);
  if (language == 0)
    do_dialog (menu_dlg_EN, -1);
  else
    do_dialog (menu_dlg_ES, -1);

  return selected_opt;
}  

/*
    gbox( menux, menuy, menux+menuw, menuy+menuh, bgcolor );
    grectangle( menux+1, menuy+1, menux+menuw-1, menuy+menuh-1, fgcolor );
    gbox( menux+1, menuy+1, menux+menuw-1, menuy+1+fonth, fgcolor );
    GFXprintf_tovideo( menux+7, menuy+4, lang_menu_title[language],
                       tfont, titlecolor, fgcolor, 0);
   
    
    for(i=0; i<NUM_MENU_OPTIONS; i++)
      GFXprintf_tovideo( menux+12, (fonth*i)+42, 
                         lang_main_options[(language*NUM_MENU_OPTIONS)+i],
                         tfont, fgcolor, bgcolor, 0);
       
   
    while(!end)
    {
       DrawSelected( menux+2, (fonth*selected)+42-3,
                     menux+menuw-2, (fonth*selected)+42+fonth-3,
                     lang_main_options[(language*NUM_MENU_OPTIONS)+selected],
                     fgcolor, bgselcolor, textbgselcolor, tfont );

       dumpVirtualToScreen();
       DrawSelected( menux+2, (fonth*selected)+42-3,
                     menux+menuw-2, (fonth*selected)+42+fonth-3,
                     lang_main_options[(language*NUM_MENU_OPTIONS)+selected],
                     fgcolor, bgcolor, bgcolor, tfont );
       keypress = readkey();
       switch( keypress >> 8 )
       {
         case KEY_DOWN: if( selected < NUM_MENU_OPTIONS-1 ) selected++;
                        break;
         case KEY_UP:   if( selected > 0 ) selected--;
                        break;
         case KEY_ESC:  return(0);
                        break;
         case KEY_F1:   return(1);
                        break;
         case KEY_F2:   return(2);
                        break;
         case KEY_F3:   return(3);
                        break;
         case KEY_F4:   return(4);
                        break;
         case KEY_F5:   return(5);
                        break;
         case KEY_F6:   return(1);
                        break;
         case KEY_F7:   return(7);
                        break;
         case KEY_F8:  return(8);
                        break;
         case KEY_F10:  return(3);
                        break;

         case KEY_ENTER:
         case KEY_SPACE:
                        if( selected <= 8 )
                            return(selected);
                        else return(10);
       };
    }
  }
*/



// Draws the selected or unselected option...
void
DrawSelected (int x1, int y1, int x2, int y2, char *text, int fgcolor,
	      int bgcolor, int textbgcolor, char *tfont)
{
  gbox (x1, y1, x2, y2, bgcolor);
  GFXprintf_tovideo (x1 + 12 - 2, y1 + 3, text, tfont, fgcolor, textbgcolor,
		     0);
}




/*
char *lista_joys(int index int *list_size)
{
	switch (index)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		default: if (index<0)
		{
			list_size=3;
			return NULL;
		}
		
	}
}
*/

int
menuopciones (void)
{
  extern tipo_emuopt emuopt;
  DIALOG dialogo[] = {
    { d_agup_window_proc, 0, 0, 195, 90, 0, 0, 0, 0, 0, 0,(void *)lang_generaloptions[language], NULL, NULL },
//  {gui_shadow_box_proc, 0, 0, 195, 90, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
//  {d_text_proc, 25, 5, 100, 16, 0, 0, 0, 0, 0, 0, lang_generaloptions[language], NULL, NULL},
    {GUI_check_proc, 10, 30, 50, 8, 0, 0, 'G', 0, 1, 0,lang_emulagunstick[language], NULL, NULL},
    {GUI_check_proc, 10, 45, 50, 8, 0, 0, 'S', 0, 1, 0,lang_soundactive[language], NULL, NULL},
    {gui_button_proc, 40, 70, 70, 16, 0, 0, 13, D_EXIT, 0, 0, "OK", NULL, NULL},
    {gui_button_proc, 120, 70, 70, 16, 0, 0, 27, D_EXIT, 0, 0, "Cancel", NULL, NULL},
//     {d_list_proc,10,30,50,16, 0,0,NULL,d1,d2,lista_joys,*dp1,*dp2},
    {NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
  };

  /* ponemos en gris el boton, o el raton en flecha segun las
     opciones 
   */
  if (!(emuopt.gunstick & GS_HAYMOUSE))
    dialogo[1].flags |= D_DISABLED;
  if (emuopt.gunstick & GS_GUNSTICK)
    {
      dialogo[1].flags |= D_SELECTED;
      set_mouse_sprite (NULL);
    }
  /* centramos el cuadro de dialogo y si se pulsa aceptar ocultamos
     o mostramos el raton segun sea necesario
   */
  centre_dialog (dialogo);
  set_dialog_color (dialogo, gui_fg_color, gui_bg_color);
  if (popup_dialog (dialogo, 2) == 3)	//2 del que lleva el foco, 3 del boton de aceptar.
    {
      if (dialogo[1].flags & D_SELECTED)
	{
	  emuopt.gunstick |= GS_GUNSTICK;
	  show_mouse (screen);
	}
      else
	{
	  emuopt.gunstick &= ~GS_GUNSTICK;
	  set_mouse_sprite (NULL);
	  show_mouse (NULL);

	}
    }
  /* si usamos gunstick volvemos a poner el punto de mira
   */
  if (emuopt.gunstick & GS_GUNSTICK)
    {
      set_mouse_sprite (emuopt.raton_bmp);
      set_mouse_sprite_focus (8, 8);
    }
  return (0);
}


int
menuhardware (void)
{
  extern tipo_hwopt hwopt;
  extern Z80Regs spectrumZ80;	
  int c;
  DIALOG dialogo[] = {
    {d_agup_window_proc, 0, 0, 195, 155, 0, 0, 0, 0, 0, 0,"Seleccion de Hardware", NULL, NULL },
    {gui_button_proc, 30, 130, 70, 16, 0, 0, 13, D_EXIT, 0, 0, "OK", NULL, NULL},
    {gui_button_proc, 110, 130, 70, 16, 0, 0, 27, D_EXIT, 0, 0, "Cancel", NULL, NULL},
    {GUI_radio_proc, 10, 30, 50, 8, 0, 0, 0, 0, 1, 0,"Spectrum 16K", (void *)1,(void *)0},
    {GUI_radio_proc, 10, 45, 50, 8, 0, 0, 0, 0, 1, 0,"Spectrum 48K", (void *)1,(void *)0},
    {GUI_radio_proc, 10, 60, 50, 8, 0, 0, 0, 0, 1, 0,"Inves Spectrum+ 48K", (void *)1,(void *)0},
    {GUI_radio_proc, 10, 75, 50, 8, 0, 0, 0, 0, 1, 0,"Spectrum 128K Espanol", (void *)1,(void *)0},
    {GUI_radio_proc, 10, 90, 50, 8, 0, 0, 0, 0, 1, 0,"Spectrum +2 (128K)", (void *)1,(void *)0},
    {GUI_radio_proc, 10,105, 50, 8, 0, 0, 0, 0, 1, 0,"Spectrum +3 (128K)",(void *)1,(void *)0},
    {NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
  };

  /* Selecionamos el tipo de arquitetura actual.
   */
  dialogo[hwopt.hw_model+2].flags |= D_SELECTED;
       
   /* centramos el cuadro de dialogo y si se pulsa aceptar ocultamos
     o mostramos el raton segun sea necesario
   */
  centre_dialog (dialogo);
  set_dialog_color (dialogo, gui_fg_color, gui_bg_color);
  if (popup_dialog (dialogo, 2) == 1)	//2 del que lleva el foco, 1 del boton de aceptar.
    {
     for(c=3;c<8;c++) if ((dialogo[c].flags & D_SELECTED)==D_SELECTED) break ;
	 //printf("Sale %i\n",c-2);
	 end_spectrum();
	 init_spectrum(c-2,"");
	 //printf("Inicio %i\n",c-2);
	 Z80Reset (&spectrumZ80, 69888);
    }
  return (0);
}
#endif // else - ifdef NO_USE_MENU
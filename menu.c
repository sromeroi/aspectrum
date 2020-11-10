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

#include "v_alleg.h"
#include <stdio.h>
#include "z80.h"
#include "langs.h"
#include "menu.h"
#include "monofnt.h"
#include "main.h"

#define NUM_FILES 9

extern int language;

/*-----------------------------------------------------------------
 * This function implements the main options menu.
 ------------------------------------------------------------------*/
int MainMenu( Z80Regs *regs, char *tfont )
{
    int i, end = 0, selected = 0, keypress;
    //int fontw = 8, 
    int fonth=12;
    int menux = 20, menuy = 15, menuw = 280, menuh = 170;
    int bgcolor = 15, titlecolor=14;
    int bgselcolor = 11, textbgselcolor=13;
    int fgcolor = 0;
   
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
         case KEY_F10:  return(10);
                        break;

         case KEY_ENTER:
         case KEY_SPACE:
                        if( selected <= 8 )
                            return(selected);
                        else return(10);
       };
    }
}


// Draws the selected or unselected option...
void DrawSelected( int x1, int y1, int x2, int y2, char *text, int fgcolor,
                   int bgcolor, int textbgcolor, char *tfont )
{
    gbox( x1, y1, x2, y2, bgcolor );
    GFXprintf_tovideo( x1+12-2, y1+3, text, tfont, fgcolor, textbgcolor, 0);
}


/*---------------------------------------------------------------------------
// Filebox selection popup :-), it receives the Type of popup (Load SNA,
// Save SNA, Save SCR...) and returns the selected filename.
*--------------------------------------------------------------------------*/

int FileMenu( char *tfont, char type, char *filename )
{
    extern struct tipo_emuopt emuopt;
//    int i, current=0, end = 0, selected = 0, keypress;
//    int fontw = 8, fonth=12;
//    int menux = 18, menuy = 70, menuw = 284, menuh = 50;
//    int bgcolor = 15; 
//    int titlecolor=14;
//    int bgselcolor = 11, textbgselcolor=13;
//    int fgcolor = 0;
	int ret;
	
    char extensions[FILEBOX_TYPES][80] =
    { 
        "SNA;SP;Z80",
        "SNA",
        "SCR",
	   "TAP"
    };
       
    gui_fg_color = 0;
    gui_bg_color = 7;

   if (emuopt.gunstick & GS_GUNSTICK) set_mouse_sprite(NULL);

#if ALLEGRO_WIP_VERSION >= 37 || ALLEGRO_VERSION >= 4
#define FILEBUF ,255,
#else
#define FILEBUF ,
#endif
 ret=file_select_ex( lang_filemenu[(language*FILEBOX_TYPES)+type],
                    filename, extensions[type] FILEBUF 290, 170 );
   /* si usamos gunstick volvemos a poner el punto de mira
   */
   if (emuopt.gunstick & GS_GUNSTICK)
   {
      set_mouse_sprite(emuopt.raton_bmp);
      set_mouse_sprite_focus(8,8);
   }

   return(ret);
/*
    gbox( menux, menuy, menux+menuw, menuy+menuh, bgcolor );
    grectangle( menux+1, menuy+1, menux+menuw-1, menuy+menuh-1, fgcolor );
    gbox( menux+1, menuy+1, menux+menuw-1, menuy+1+fonth, fgcolor );
    GFXprintf_tovideo( menux+7, menuy+4, 
           lang_filemenu[(language*FILEBOX_TYPES)+type], tfont,
           titlecolor, fgcolor, 0);
   
    dumpVirtualToScreen();  
    GFXgets( menux+4, menuy+25, filename, tfont, 0, 15, 36 );
*/
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

int menuopciones(void)
{
   extern struct tipo_emuopt emuopt;
   DIALOG dialogo[]=
   {
      {d_shadow_box_proc,0,0,195,90, 0,15,0,0,0,0,NULL,NULL,NULL},       
      {d_text_proc,25,5,100,16, 0,15,0,0,0,0,
         lang_generaloptions[language],NULL,NULL},	
      {d_check_proc,10,20,50,8, 0,15,'G',0,1,0,
         lang_emulagunstick[language], NULL,NULL}, 
      {d_button_proc,40,70,70,16, 0,7,13 ,D_EXIT,0,0,"OK",NULL,NULL},
      {d_button_proc,120,70,70,16, 0,7,27 ,D_EXIT,0,0,"Cancel",NULL,NULL}, 
      {d_check_proc,10,35,50,8, 0,15,'S',0,1,0,
         lang_soundactive[language], NULL,NULL}, 
//      {d_list_proc,10,30,50,16,0,15,NULL,d1,d2,lista_joys,*dp1,*dp2},
      {NULL,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL}
   };

   /* ponemos en gris el boton, o el raton en flecha segun las
      opciones 
   */
   if (!(emuopt.gunstick & GS_HAYMOUSE)) dialogo[2].flags|=D_DISABLED;
   if (emuopt.gunstick & GS_GUNSTICK) 
   {
      dialogo[2].flags|=D_SELECTED;
      set_mouse_sprite(NULL);
   }
   /* centramos el cuadro de dialogo y si se pulsa aceptar ocultamos
      o mostramos el raton segun sea necesario
   */
   centre_dialog(dialogo);
   if (popup_dialog(dialogo,2)==3) //2 del que lleva el foco, 3 del boton de aceptar.
   {
      if (dialogo[2].flags & D_SELECTED) 
      {
          emuopt.gunstick|=GS_GUNSTICK;
          show_mouse(screen);
      } else { 
         emuopt.gunstick&= ~GS_GUNSTICK;
         set_mouse_sprite(NULL);
         show_mouse(NULL);
      }
   }   
   /* si usamos gunstick volvemos a poner el punto de mira
   */
   if (emuopt.gunstick & GS_GUNSTICK)
   {
      set_mouse_sprite(emuopt.raton_bmp);
      set_mouse_sprite_focus(8,8);
   }

/* TODO
hacer que parezca un raton (y no el p.d.m) cuando se llama al cuadro de dialogo
*/

   return(0);
}


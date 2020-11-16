#ifndef _V_ALLEG_H
#define _V_ALLEG_H 1

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/keyboard.h>

typedef struct gRGB {
  unsigned char r, g, b;
  unsigned char filler;
 } gRGB; 

// "borrowed" from allegro
  
#define gKEY_A ALLEGRO_KEY_A
#define gKEY_B ALLEGRO_KEY_B
#define gKEY_C ALLEGRO_KEY_C
#define gKEY_D ALLEGRO_KEY_D
#define gKEY_E ALLEGRO_KEY_E
#define gKEY_F ALLEGRO_KEY_F
#define gKEY_G ALLEGRO_KEY_G
#define gKEY_H ALLEGRO_KEY_H
#define gKEY_I ALLEGRO_KEY_I
#define gKEY_J ALLEGRO_KEY_J
#define gKEY_K ALLEGRO_KEY_K
#define gKEY_L ALLEGRO_KEY_L
#define gKEY_M ALLEGRO_KEY_M                
#define gKEY_N ALLEGRO_KEY_N
#define gKEY_O ALLEGRO_KEY_O              
#define gKEY_P ALLEGRO_KEY_P             
#define gKEY_Q ALLEGRO_KEY_Q            
#define gKEY_R ALLEGRO_KEY_R           
#define gKEY_S ALLEGRO_KEY_S          
#define gKEY_T ALLEGRO_KEY_T                
#define gKEY_U ALLEGRO_KEY_U               
#define gKEY_V ALLEGRO_KEY_V              
#define gKEY_W ALLEGRO_KEY_W             
#define gKEY_X ALLEGRO_KEY_X            
#define gKEY_Y ALLEGRO_KEY_Y           
#define gKEY_Z ALLEGRO_KEY_Z          
#define gKEY_0 ALLEGRO_KEY_0         
#define gKEY_1 ALLEGRO_KEY_1        
#define gKEY_2 ALLEGRO_KEY_2       
#define gKEY_3 ALLEGRO_KEY_3          
#define gKEY_4 ALLEGRO_KEY_4
#define gKEY_5 ALLEGRO_KEY_5              
#define gKEY_6 ALLEGRO_KEY_6              
#define gKEY_7 ALLEGRO_KEY_7              
#define gKEY_8 ALLEGRO_KEY_8              
#define gKEY_9 ALLEGRO_KEY_9              
#define gKEY_0_PAD ALLEGRO_KEY_0_PAD          
#define gKEY_1_PAD ALLEGRO_KEY_1_PAD          
#define gKEY_2_PAD ALLEGRO_KEY_2_PAD           
#define gKEY_3_PAD ALLEGRO_KEY_3_PAD           
#define gKEY_4_PAD ALLEGRO_KEY_4_PAD            
#define gKEY_5_PAD ALLEGRO_KEY_5_PAD            
#define gKEY_6_PAD ALLEGRO_KEY_6_PAD            
#define gKEY_7_PAD ALLEGRO_KEY_7_PAD            
#define gKEY_8_PAD ALLEGRO_KEY_8_PAD            
#define gKEY_9_PAD ALLEGRO_KEY_9_PAD            
#define gKEY_F1 ALLEGRO_KEY_F1               
#define gKEY_F2 ALLEGRO_KEY_F2
#define gKEY_F3 ALLEGRO_KEY_F3               
#define gKEY_F4 ALLEGRO_KEY_F4               
#define gKEY_F5 ALLEGRO_KEY_F5               
#define gKEY_F6 ALLEGRO_KEY_F6               
#define gKEY_F7 ALLEGRO_KEY_F7               
#define gKEY_F8 ALLEGRO_KEY_F8               
#define gKEY_F9 ALLEGRO_KEY_F9               
#define gKEY_F10 ALLEGRO_KEY_F10              
#define gKEY_F11 ALLEGRO_KEY_F11 
#define gKEY_F12 ALLEGRO_KEY_F12            
#define gKEY_ESC ALLEGRO_KEY_ESCAPE
#define gKEY_LSHIFT ALLEGRO_KEY_LSHIFT
#define gKEY_RSHIFT ALLEGRO_KEY_RSHIFT
#define gKEY_ENTER ALLEGRO_KEY_ENTER
#define gKEY_SPACE ALLEGRO_KEY_SPACE
#define gKEY_ALT ALLEGRO_KEY_ALT
#define gKEY_UP ALLEGRO_KEY_UP
#define gKEY_DOWN ALLEGRO_KEY_DOWN
#define gKEY_LEFT ALLEGRO_KEY_LEFT
#define gKEY_RIGHT ALLEGRO_KEY_RIGHT
#define gKEY_RCONTROL ALLEGRO_KEY_RCONTROL
#define gKEY_BACKSPACE ALLEGRO_KEY_BACKSPACE
#define gKEY_SLASH ALLEGRO_KEY_SLASH
#define gKEY_TILDE ALLEGRO_KEY_TILDE



void gExitEmulator (void);

void gset_color (int index, gRGB * p);

void gclear (void);

void dumpVirtualToScreen (void);

void gtextout (char *b, int x, int y, int color);

void gtextoutb (char *b, int x, int y, int color, ALLEGRO_FONT * tfont);

//void init_wrapper (void);

void gacquire_bitmap (void);

void grelease_bitmap (void);

void InitGraphics (void);

void gclear_to_color (int color);

void gUpdateRect (int x, int y, int sizeX, int sizeY);

void gPutPixel (int x, int y, int col);

void gbox (int x1, int y1, int x2, int y2, int color);

void grectangle (int x1, int y1, int x2, int y2, int color);

void ghline (int x1, int y1, int x2, int color);

void InitSystem (void);

void v_initmouse (void);

//int galert (const char *s1, const char *s2, const char *s3, const char *b1, const char *b2, int c1, int c2);
int v_alertYesNo (const char *Title, const char *Head, const char *Text);

int v_alertErrOK (const char *Title, const char *Head, const char *Text);

// sound functions 
void gInitSound (void);

unsigned char *gGetSampleBuffer (void);

void gPlaySound (void);

int getVoicePos (void);

void playMainSample (void);

void gSoundSync (void);

// keyboard functions
bool gkeypressed(int tecla);

void gupdatekeys(void);

int referencehelp_proc(void);

int mouse_x();

int mouse_y();

int mouse_b();

void v_scaremouse(void);

void v_unscaremouse(void);

int v_framecheck(void);

#endif	/* v_alleg.h */ 
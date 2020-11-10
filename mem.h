/*
Distribuido GPL (C) 2003 Alvaro Alea.
*/
#ifndef MEM_H
#define MEM_H

//#define word unsigned int // definidos en z80.h
//#define byte unsigned short int 


typedef struct   //estructura de memoria
{
   byte *p;  // pointer to memory poll
   int mp;    // bitmap for page bits in z80 mem
   int md;    // bitmap for dir bits in z80 mem
   int np;    // number of pages of memory 
   int ro[16];  // map of pages for read  (map ar alwais a offset for *p) 
   int wo[16];  // map of pages for write
   int sro[16]; // map of system memory pages for read (to remember when perife-
   int swo[16]; // map of system memory pages for write         -rical is paged)
   int vn;    // number of video pages
   int vo[2];  // map of video memory
   int roo;   // offset to dummy page for readonly emulation
   	      //   Precalculated data
   int mr;    // times left rotations for page ( number of zero of mp)
   int sp;    // size of pages (hFFFF / mp)
} tipo_mem;

#define RO_PAGE 1
#define RW_PAGE 0
#define SYSTEM_PAGE 0
#define NOSYS_PAGE 1

byte readmem(word dir);
void writemem(word dir, byte data);
byte readvmem(word offset, int page);

void pagein(int size, int bloq, int page, int ro, int issystem);
void pageout(int size, int bloq, int page);

int init_spectrum(int model,char *romfile);
int end_spectrum(void);
int reset_spectrum(void);

int init_48k(char *romfile);
int init_16k(char *romfile);
int init_inves(char *romfile);
int init_plus2(char *romfile);

int init_128k(char *romfile);
int reset_128k(void);
void outbankm_128k(byte dato);

int init_48k_if1(char *romfile);

#endif  // #ifdef MEM_H

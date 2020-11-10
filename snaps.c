#include "stdafx.h"
/*=====================================================================
  snaps.c    -> This file includes all the snapshot handling functions
                for the emulator, called from the main loop and when
                the user wants to load/save snapshots files.

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
#include <string.h>
#include "z80.h"
#include "snaps.h"

extern byte *RAM;
extern FILE *tapfile;

// Generic routines based on the filename extension:
char LoadSnapshot( Z80Regs *regs, char *fname )
{
  FILE *snafp;
  
  snafp=fopen(fname, "rb");
  if( snafp != NULL ) 
  {
    if( strstr(fname, ".sp") != NULL  ||
        strstr(fname, ".SP") != NULL )
           LoadSP( regs, snafp );

    else if( strstr(fname, ".sna") != NULL ||
             strstr(fname, ".SNA") != NULL) 
           LoadSNA( regs, snafp );

    else if( strstr(fname, ".z80") != NULL ||
             strstr(fname, ".Z80") != NULL)
           LoadZ80( regs, snafp );
    else if( strstr(fname, ".tap") != NULL ||
             strstr(fname, ".TAP") != NULL)
    {
        tapfile = snafp;
    }
    
    if( tapfile != snafp )
       fclose(snafp);
    
    return(1);
  }
  return(0);
}


// Generic routines based on the filename extension:
char LoadTapfile( Z80Regs *regs, char *fname )
{
/*
 * FILE *snafp;
  
  snafp=fopen(fname, "rb");
  if( snafp != NULL ) 
  {
     LoadTAP()
    else if( strstr(fname, ".z80") != NULL ||
             strstr(fname, ".Z80") != NULL)
           LoadZ80( regs, snafp );
//    else if( strstr(fname, ".tap") != NULL ||
//             strstr(fname, ".TAP") != NULL)
//        tapfile = fp;
//        
    fclose(snafp);
    return(1);
  }
  */
  return(0);
}



char SaveSnapshot( Z80Regs *regs, char *fname )
{
  FILE *snafp;
  
  snafp=fopen(fname, "wb");
  if( snafp != NULL ) 
  {
//    if( strstr(fname, ".sp") != NULL  ||
//        strstr(fname, ".SP") != NULL )
//           SaveSP( regs, snafp );
/*
    else if( strstr(fname, ".sna") != NULL ||
             strstr(fname, ".SNA") != NULL) 
*/
           SaveSNA( regs, snafp );
/*
    else if( strstr(fname, ".z80") != NULL ||
             strstr(fname, ".Z80") != NULL)
           SaveZ80( regs, snafp );
*/
    fclose(snafp);
    return(1);
  }
  return(0);
}


char SaveScreenshot( Z80Regs *regs, char *fname )
{
  FILE *snafp;
  snafp=fopen(fname, "wb");
  if( snafp != NULL )
  {
     SaveSCR( regs, snafp );
     fclose( snafp );
     return(1);
  }
  return(0);
}



/*-----------------------------------------------------------------
 char LoadZ80( Z80Regs *regs, FILE *fp );
 This loads a .Z80 file from disk to the Z80 registers/memory.

 void UncompressZ80 (int tipo, int pc, Z80Regs *regs, FILE *fp)
 This load and uncompres a Z80 block to pc adress memory.

 The Z80 Load Routine is (C) 2001 Alvaro Alea Fdz. 
 e-mail: ALEAsoft@yahoo.com  Distributed under GPL2	
------------------------------------------------------------------*/
#define Z80BL_V1UNCOMP 0
#define Z80BL_V1COMPRE 1
#define Z80BL_V2UNCOMP 3
#define Z80BL_V2COMPRE 4      

void UncompressZ80 (int tipo, int pc, Z80Regs *regs, FILE *fp)
{
   byte  c,last,last2,dato;
   int   cont,num,limit;

   limit=pc+((tipo==Z80BL_V1UNCOMP || tipo==Z80BL_V1COMPRE) ? 0xbfff : 0x3fff);
//   printf (" PC es: %x\n",pc);
//   printf (" Limite puesto en: %x\n",limit);
   if ((tipo=Z80BL_V1UNCOMP) || (tipo==Z80BL_V2UNCOMP))
   {
//      printf (" Chicos bloque desco, a saco\n");
      for( cont=pc; cont<=limit; cont++ ) regs->RAM[cont] = fgetc(fp);
   } else {		
//      printf (" Comprimido, se complica\n");
      last=0; last2=0; c=0;
      do 
      {
         last2=last;
	     last=c;
	     c=fgetc(fp);
	     /* detectamos un bloque comprimido */
	     if ((last==0xED) && (c==0xED)) 
	     {
	        num=fgetc(fp);
	        /* Detectamos el EOF */
	        if ((tipo==Z80BL_V1COMPRE) && (last2==0x00) && (num==0x00)) break;
	            dato=fgetc(fp);
	          for(cont=0;cont<num;cont++) regs->RAM[pc++]=dato; 		
 	       c=0;/*evita ED ED xx yy ED zz uu reconozca zz uu como comprimido*/ 
	        continue;
	     } else {
	        /* habia un 0xED pero no era de comprimido */	
	        if ((last==0xED) && (c!=0xED)) regs->RAM[pc++]=last;
	        /* Si es ED esperamos a la siguente y si no a memoria */	
	        if (c!=0xED) regs->RAM[pc++]=c; 
	     }
      } while ((pc<=limit))  ;
   }
}

char LoadZ80( Z80Regs *regs, FILE *fp )
{
   int f,tam,ini,sig,pag,ver=0;	
   byte buffer[85];  /* creo que con bastante menos servia */
//   printf("Me han llamado pa cargar Z80 y leo la cabezera\n");
   fread(buffer,86,1,fp);
   /* Comprobamos que version del fichero es */
   if ((word)buffer[6]==0) 
   {
      if ((word)buffer[30]==23) 
      {
      	printf("Fichero Z80: Version 2.0\n");
      	ver=2;
      }
      else if ((word)buffer[30]==54) 
      {
      	printf("Fichero Z80: Version 3.0\n");
      	ver=3;
      }
      else printf("Fichero Z80: Version Desconocida (%04X)\n",(word)buffer[30]);
      
      /* version>=2 permite hardware diverso */
      switch (buffer[34]) /* si no es 48K nos salimos */
      {
      	case 0: printf("Hardware 48K\n"); break;
      	case 1: printf("Hardware 48K + IF1\n"); return(-1);
      	case 2: printf("Hardware 48K + SAMRAM\n"); return(-1);
      	case 3: printf((ver==2)?"Hardware 128K\n":"48K + M.G.T.\n"); return(-1);
      	case 4: printf((ver==2)?"Hardware 128K + IF1\n":"128K\n"); return(-1);
      	case 5: printf("Hardware 128K + IF1\n"); return(-1);
      	case 6: printf("Hardware 128K + M.G.T\n"); return(-1);
      	default:printf("hardware desconocido, %x\n",buffer[34]); return (-1);
	  }
      sig=30+2+buffer[30]; /*ni idea de por que el +2 */
      for(f=0;f<3;f++)
      {	
   	 fseek(fp,sig,SEEK_SET); 
//         printf("seekee bloque y calculo tamaño %i\n",f);
   	 /* no estoy seguro de que de una sola vez los coja en orden */
   	 tam=fgetc(fp); tam=tam+(fgetc(fp)<<8); 
         pag=fgetc(fp);
         sig=sig+tam+3;
		printf(" pagina a cargar es: %i, tamaño:%x\n",pag,tam);
         switch (pag)
         {
	    case 4:ini=0x8000; break;
	    case 5:ini=0xc000; break;
	    case 8:ini=0x4000; break;
	    default: ini=0x4000; 
	    	printf("Algo raro pasa con ese Snapshot, remitalo al autor para debug\n");
	    	break;
         }
//     printf("llamando a la parienta\n");
	 UncompressZ80((tam==0xffff?Z80BL_V2UNCOMP:Z80BL_V2COMPRE),ini,regs,fp);
//     printf("parienta llamanda y reponde!!!\n");
      }
      regs->PC.B.l  = buffer[32];
      regs->PC.B.h  = buffer[33];
   } else {
      printf("Fichero Z80: Version 1.0\n");
      if (buffer[12] & 0x20) 
      {
         printf("Fichero Z80: Comprimido\n");	
	 fseek(fp,30,SEEK_SET); /* si era V1 leimos demasiado antes */
	 UncompressZ80(Z80BL_V1COMPRE,0x4000,regs,fp);			
      } else {
         printf("Fichero Z80: Sin comprimir\n");	
	 fseek(fp,30,SEEK_SET); /* si era V1 leimos demasiado antes */
   	 UncompressZ80(Z80BL_V1UNCOMP,0x4000,regs,fp);			
      }
      regs->PC.B.l  = buffer[ 6];
      regs->PC.B.h  = buffer[ 7];
   }

   regs->AF.B.h  = buffer[ 0];
   regs->AF.B.l  = buffer[ 1];
   regs->BC.B.l  = buffer[ 2];
   regs->BC.B.h  = buffer[ 3];
   regs->HL.B.l  = buffer[ 4];
   regs->HL.B.h  = buffer[ 5];
   regs->SP.B.l  = buffer[ 8];
   regs->SP.B.h  = buffer[ 9];
   regs->I       = buffer[10];
   regs->R.B.l   = buffer[11];
   regs->R.B.h   = 0;
   regs->BorderColor = ((buffer[12] & 0x0E) >> 1);
   regs->DE.B.l  = buffer[13];
   regs->DE.B.h  = buffer[14];
   regs->BCs.B.l = buffer[15];
   regs->BCs.B.h = buffer[16];
   regs->DEs.B.l = buffer[17];
   regs->DEs.B.h = buffer[18];
   regs->HLs.B.l = buffer[19];
   regs->HLs.B.h = buffer[20];
   regs->AFs.B.h = buffer[21];
   regs->AFs.B.l = buffer[22];
   regs->IY.B.l  = buffer[23];
   regs->IY.B.h  = buffer[24];
   regs->IX.B.l  = buffer[25];
   regs->IX.B.h  = buffer[26];
   regs->IFF1    = buffer[27] & 0x01;
   regs->IFF2    = buffer[28] & 0x01;
   regs->IM      = buffer[29] & 0x03;

   return(0);
}

/*-----------------------------------------------------------------
 char LoadSP( Z80Regs *regs, FILE *fp );
 This loads a .SP file from disk to the Z80 registers/memory.
------------------------------------------------------------------*/
char LoadSP( Z80Regs *regs, FILE *fp )
{
   unsigned short length, start, sword;
   int f;
   byte buffer[80];
   fread(buffer,38,1,fp);

   /* load the .SP header: */
   length        = (buffer[3] << 8) + buffer[2];
   start         = (buffer[5] << 8) + buffer[4];
   regs->BC.B.l  = buffer[ 6];
   regs->BC.B.h  = buffer[ 7];
   regs->DE.B.l  = buffer[ 8];
   regs->DE.B.h  = buffer[ 9];
   regs->HL.B.l  = buffer[10];
   regs->HL.B.h  = buffer[11];
   regs->AF.B.l  = buffer[12];
   regs->AF.B.h  = buffer[13];
   regs->IX.B.l  = buffer[14];
   regs->IX.B.h  = buffer[15];
   regs->IY.B.l  = buffer[16];
   regs->IY.B.h  = buffer[17];
   regs->BCs.B.l  = buffer[18];
   regs->BCs.B.h  = buffer[19];
   regs->DEs.B.l  = buffer[20];
   regs->DEs.B.h  = buffer[21];
   regs->HLs.B.l  = buffer[22];
   regs->HLs.B.h  = buffer[23];
   regs->AFs.B.l  = buffer[24];
   regs->AFs.B.h  = buffer[25];
   regs->R.W = 0;
   regs->R.B.l = buffer[26];
   regs->I = buffer[27];
   regs->SP.B.l  = buffer[28];
   regs->SP.B.h  = buffer[29];
   regs->PC.B.l  = buffer[30];
   regs->PC.B.h  = buffer[31];
   regs->BorderColor = buffer[34];
   sword = (buffer[37] << 8) | buffer [36] ;

/*
   fgetc(fp); fgetc(fp);

   fread( &length, 1, sizeof(unsigned short), fp);
   fread( &start, 1, sizeof(unsigned short), fp);
   fread( &(regs->BC.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->DE.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->HL.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->AF.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->IX.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->IY.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->BCs.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->DEs.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->HLs.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->AFs.W), 1, sizeof(unsigned short), fp);
   regs->R.W = fgetc(fp);
   regs->I   = fgetc(fp);
   fread( &(regs->SP.W), 1, sizeof(unsigned short), fp);
   fread( &(regs->PC.W), 1, sizeof(unsigned short), fp);
   fgetc(fp);
   fgetc(fp);
   regs->BorderColor = fgetc(fp);
   fgetc(fp);
   fread( &sword, 1, sizeof(unsigned short), fp);
*/

   printf("\nSP_PC = %04X, SP_START =  %d,  SP_LENGTH = %d\n",
          regs->PC, start, length);

   /* interrupt mode */
   regs->IFF1 = regs->IFF2 = 0;
   if( sword & 0x4 ) regs->IFF2 = 1;
   if( sword & 0x8 ) regs->IM = 0;
   else
   {
      if( sword & 0x2 ) regs->IM = 2;
      else regs->IM = 1;
   }
   if( sword & 0x1 ) regs->IFF1 = 1;


   if( sword & 0x16 ) printf("\n\nPENDING INTERRUPT!!\n\n");
   else               printf("\n\nno pending interrupt.\n\n");


   for( f=0; f<=length; f++ )
     if( start+f < 65536 )
       regs->RAM[start+f] = fgetc(fp);

   return(0);
}


/*-----------------------------------------------------------------
 char LoadSNA( Z80Regs *regs, FILE *fp );
 This loads a .SNA file from disk to the Z80 registers/memory.
------------------------------------------------------------------*/
char LoadSNA( Z80Regs *regs, FILE *fp )
{
   byte buffer[27];

   /* load the .SNA header */
   fread(buffer,27,1,fp);

   regs->I        = buffer[ 0];
   regs->HLs.B.l  = buffer[ 1];
   regs->HLs.B.h  = buffer[ 2];
   regs->DEs.B.l  = buffer[ 3];
   regs->DEs.B.h  = buffer[ 4];
   regs->BCs.B.l  = buffer[ 5];
   regs->BCs.B.h  = buffer[ 6];
   regs->AFs.B.l  = buffer[ 7];
   regs->AFs.B.h  = buffer[ 8];
   regs->HL.B.l   = buffer[ 9];
   regs->HL.B.h   = buffer[10];
   regs->DE.B.l   = buffer[11];
   regs->DE.B.h   = buffer[12];
   regs->BC.B.l   = buffer[13];
   regs->BC.B.h   = buffer[14];
   regs->IY.B.l = buffer[15];
   regs->IY.B.h = buffer[16];
   regs->IX.B.l = buffer[17];
   regs->IX.B.h = buffer[18];
   regs->IFF1 = regs->IFF2 = (buffer[19]&0x04) >>2;
   regs->R.W  = buffer[20];
   regs->AF.B.l = buffer[21];
   regs->AF.B.h = buffer[22];
   regs->SP.B.l =buffer[23];
   regs->SP.B.h =buffer[24];
   regs->IM = buffer[25];
   regs->BorderColor = buffer[26];

   fread(regs->RAM+16384, 0x4000*3, 1, fp );
   regs->PC.B.l = Z80MemRead(regs->SP.W, regs);
   regs->SP.W++;
   regs->PC.B.h = Z80MemRead(regs->SP.W, regs);
   regs->SP.W++;

   return(0);
}


/*-----------------------------------------------------------------
 char SaveSNA( Z80Regs *regs, FILE *fp );
 This saves a .SNA file from the Z80 registers/memory to disk.
------------------------------------------------------------------*/
char SaveSNA( Z80Regs *regs, FILE *fp )
{

   unsigned char sptmpl, sptmph;

   /* save the .SNA header */
   fputc( regs->I, fp );
   fputc( regs->HLs.B.l, fp );
   fputc( regs->HLs.B.h, fp );
   fputc( regs->DEs.B.l, fp );
   fputc( regs->DEs.B.h, fp );
   fputc( regs->BCs.B.l, fp );
   fputc( regs->BCs.B.h, fp );
   fputc( regs->AFs.B.l, fp );
   fputc( regs->AFs.B.h, fp );
   fputc( regs->HL.B.l, fp );
   fputc( regs->HL.B.h, fp );
   fputc( regs->DE.B.l, fp );
   fputc( regs->DE.B.h, fp );
   fputc( regs->BC.B.l, fp );
   fputc( regs->BC.B.h, fp );
   fputc( regs->IY.B.l, fp );
   fputc( regs->IY.B.h, fp );
   fputc( regs->IX.B.l, fp );
   fputc( regs->IX.B.h, fp );
   fputc( regs->IFF1 << 2, fp );
   fputc( regs->R.W & 0xFF, fp );
   fputc( regs->AF.B.l, fp );
   fputc( regs->AF.B.h, fp );

   sptmpl = Z80MemRead( regs->SP.W-1, regs );
   sptmph = Z80MemRead( regs->SP.W-2, regs );

   /* save PC on the stack */
   Z80MemWrite( --(regs->SP.W), regs->PC.B.h, regs);
   Z80MemWrite( --(regs->SP.W), regs->PC.B.l, regs);

   fputc( regs->SP.B.l, fp );
   fputc( regs->SP.B.h, fp );
   fputc( regs->IM, fp );
   fputc( regs->BorderColor, fp );

   /* save the RAM contents */
   fwrite( regs->RAM+16384, 48*1024, 1, fp );

   /* restore the stack and the SP value */
   regs->SP.W += 2;
   Z80MemWrite( regs->SP.W-1, sptmpl, regs );
   Z80MemWrite( regs->SP.W-2, sptmph, regs );
   return(0);
}



/*-----------------------------------------------------------------
 char SaveSCR( Z80Regs *regs, FILE *fp );
 This saves a .SCR file from the Spectrum videomemory.
------------------------------------------------------------------*/
char SaveSCR( Z80Regs *regs, FILE *fp )
{
   int i;

   /* Save the contents of VideoRAM to file: write the 6192 bytes
    * starting on the memory address 16384 */
   
   for( i=0; i<6912; i++ )
     fputc( Z80MemRead( 16384+i,regs ), fp );
   
   return(0);
}



/*-----------------------------------------------------------------
 char LoadTAP( Z80Regs *regs, FILE *fp );
 This loads a .TAP file from disk to the Z80 registers/memory,
 almost at zero speed :-)

    WARNING: EXPERIMENTAL X'DDDDDD
------------------------------------------------------------------*/
char LoadTAP( Z80Regs *regs, FILE *fp )
{
  int blow, bhi, bytes, f, howmany;
  unsigned int where;

  printf("\n--- Trying to load from tape: reached %04Xh ---\n", regs->PC.W );
  printf("On enter:  A=%d, IX=%d, DE=%d\n", regs->AF.B.h, regs->IX.W, regs->DE.W );

/*
  // auto tape-rewind function on end-of-file
  if( feof(fp) )
  {
      printf("END OF FILE: Tape rewind to 0...\n");
      fseek( fp, 0, SEEK_SET );
  }
*/

  blow = fgetc(fp);
  bhi = fgetc(fp);
  bytes = (bhi << 8) | blow;
  printf("%d bytes to read on file, DE=%d requested.\n", bytes-2, regs->DE.W );

  where = regs->IX.W;
  fgetc(fp);                           /* read flag type and ignore it */

  /* determine how many bytes to read. If there are less bytes in
     the tap block than the requested DE bytes, generate the read
    error by setting the C_FLAG on F... */
  howmany = regs->DE.W;
  if ( bytes-2 < howmany )
  {
    howmany = bytes-2;
    (regs->AF.B.l &= ~(C_FLAG));
    printf("Generating a tape load error (tapbytes < DE)...\n" );
    regs->IX.W += bytes-2;
  }
  else
    regs->IX.W += regs->DE.W;

  for( f=0; f<howmany; f++ )
  {
     Z80MemWrite( where + f, fgetc(fp), regs );
     if( howmany == 17 && f<=10 ) putchar( Z80MemRead( where+f, regs ));
  }

  if( howmany == 17 ) printf("\n");
  fgetc(fp);                           /* read checksum (and ignore it :-) */

  /* if load was successful, reset C_FLAG */
  if( howmany == regs->DE.W )
     (regs->AF.B.l |= (C_FLAG));

  regs->DE.W = 0;
  printf("On exit:  A=%d, IX=%d, DE=%d, F=%02Xh, PC=%04Xh\n",
          regs->AF.B.h, regs->IX.W, regs->DE.W, regs->AF.B.l,
          (Z80MemRead(regs->SP.W+1, regs)<<8)|Z80MemRead(regs->SP.W, regs) );

  return(0);
}



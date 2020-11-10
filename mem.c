/*
 * Distribuido GPL (C) 2003 Alvaro Alea.
 *
 * TODO:
 *
 * Meterlo en el codigo central. (parte DONE)
 * rutinas de paginacion para cada uno
 * Hacer un GUI para la selecion del HW 
 * Compatibilizar con snapshots / cintas
 * Reactivar el uso de Cartuchos Sinclair DONE
 * Hacer gui para el transpate
 */

#include <stdio.h>
#include <stdlib.h>
#include "z80.h"
#include "snaps.h"
#include "main.h"
#include "mem.h"

tipo_mem mem;

byte readmem(word dir){
	int page;
	byte v;
	page= ( dir & mem.mp ) >> mem.mr ;
	v=*( mem.p + mem.ro[page] + (dir & mem.md)) ;
	return v;
}

void writemem(word dir, byte data){
	int page;
	page= ( dir & mem.mp ) >> mem.mr ;
	*( mem.p + mem.wo[page] + (dir & mem.md))=data ;
}

byte readvmem(word offset, int page){
	return *( mem.p + mem.vo[page] + offset) ;
}


/* por que estas rutinas y no paginar directamente sobre los arrais???
 * Supon la arquitectura del +2 y la del +2 con el pokeador automatico de MH
 * en el primero el tamaño de la pagina es de 16K mientras que en el segundo es de 2K
 * Con esto deberiamos poder aprobechar la rutina de paginacion del 128 en los dos casos
 * ya que esta rutina tiene en cuenta el tamaño de pagina con el que trabajamos y hace los
 * ajustes necesarios
 */
void pagein(int size, int block, int page, int ro, int issystem){
	int npag,cf,c,d;
	npag=size/mem.sp;
	d=block*npag;
	c=page*npag;
	cf=c+npag;
	for(;c<cf;c++,d+=mem.sp)
		if ( mem.ro[c]==mem.sro[c] ) { // no hay periferico
			if (issystem==SYSTEM_PAGE) {  		// y pagino systema
				mem.ro[c]=d;
				mem.sro[c]=d;
				mem.wo[c]=(ro==RW_PAGE?d:mem.roo);
				mem.swo[c]=(ro==RW_PAGE?d:mem.roo);
			  } else {    		// y meto periferico
				mem.ro[c]=d;
				mem.wo[c]=(ro==RW_PAGE?d:mem.roo);
			  }
		 } else {    		// si hay periferico
			  if (issystem==SYSTEM_PAGE) {  		// los cambios solo se notaran en el futuro.
				   mem.sro[c]=d;
				   mem.swo[c]=(ro==RW_PAGE?d:mem.roo);
			  } else {  		// y cambio de periferico.
				   mem.ro[c]=d;
				   mem.wo[c]=(ro==RW_PAGE?d:mem.roo);
			  }
		 }
        
}

void pageout(int size, int bloq, int page){
	int npag,c;
	npag=size/mem.sp;
	c=page*npag;
	mem.ro[c]=mem.sro[c];
	mem.wo[c]=mem.swo[c];
}

int init_spectrum(int model,char *romfile){
	int ret;
	switch (model) {
		case SPECMDL_16K:
			ret=init_16k(romfile);
			break;
		case SPECMDL_48K:
			ret=init_48k(romfile);
			break;
		case SPECMDL_INVES:
			ret=init_inves(romfile);
			break;
		case SPECMDL_128K:
			ret=init_128k(romfile);
			break;
		case SPECMDL_PLUS2:
			ret=init_plus2(romfile);
			break;
/*		case SPECMDL_48KIF1:
			ret=init_48k_if1(romfile);
			break;
*/		default:
			printf("MODELO NO SOPORTADO %x\n",model);
			ret=1;
			break;
		}
	return ret;
}

extern tipo_hwopt hwopt;

int reset_spectrum(void){
	int ret;
	switch (hwopt.hw_model) {
		case SPECMDL_16K:
		case SPECMDL_48K:
		case SPECMDL_INVES:
			ret=0;
			break;
		case SPECMDL_128K:
		case SPECMDL_PLUS2:
			ret=reset_128k();
			break;
/*		case SPECMDL_48KIF1:
			ret=init_48k_if1(romfile);
			break;
*/		default:
			printf("MODELO NO SOPORTADO %x\n",hwopt.hw_model);
			ret=1;
			break;
		}
	return ret;
}


int end_spectrum(){
    free (mem.p);	/* free RAM */
	return 0;
}

//Ejemplos de rutinas de inicializacion de 3 modelos de spectrum.

//extern int TSTATES_PER_LINE;
//extern int TOP_BORDER_LINES;
//extern int BOTTOM_BORDER_LINES, 
//extern int SCANLINES;

int init_48k(char *romfile){
	FILE *fp;
	int i;
	printf(__FILE__": Init 48K hardware.\n");
	mem.md= 0x3FFF ;
	mem.mp= 0xC000 ;
	mem.mr= 14;
	mem.np=4;
	mem.sp=16*1024;
	mem.p=(byte *)malloc((1+3+1)*mem.sp);
	if (mem.p==NULL){
		printf("NO MEMORY...\n");
		return 1;
	}
	// cargar rom de 16K
	if (romfile[0]==0)
		fp=findopen_file("spectrum.rom");
	else 
		fp=findopen_file(romfile);	
	i=0;
	while (!feof (fp))
		*(mem.p+i++) = fgetc (fp);
	fclose (fp);
	mem.roo=4*mem.sp;
	mem.vn=1;

	mem.ro[0]=mem.sro[0]=0;
	mem.wo[0]=mem.swo[0]=mem.roo;
	mem.ro[1]=mem.sro[1]=mem.wo[1]=mem.swo[1]=mem.sp;
	mem.ro[2]=mem.sro[2]=mem.wo[2]=mem.swo[2]=2*mem.sp;
	mem.ro[3]=mem.sro[3]=mem.wo[3]=mem.swo[3]=3*mem.sp;
	mem.vo[0]=0x4000;

	// ULA config
	hwopt.port_ff=0xFF;			// 0xff = emulate the port,  0x00 alwais 0xFF
	hwopt.ts_lebo=24;			  // left border t states
	hwopt.ts_grap=128;			 // graphic zone t states
	hwopt.ts_ribo=24;			  // right border t states
	hwopt.ts_hore=48;			  // horizontal retrace t states
	hwopt.ts_line=224;			 // to speed the calc, the sum of 4 abobe
	hwopt.line_poin=16;		    // lines in retraze post interrup
	hwopt.line_upbo=48;		    // lines of upper border
	hwopt.line_grap=192;		   // lines of graphic zone = 192
	hwopt.line_bobo=48;		    // lines of bottom border
	hwopt.line_retr=8;		     // lines of the retrace
	hwopt.TSTATES_PER_LINE=224;
	hwopt.TOP_BORDER_LINES=64;
	hwopt.SCANLINES=192;
	hwopt.BOTTOM_BORDER_LINES=56;
	hwopt.tstate_border_left=24;
	hwopt.tstate_graphic_zone=128;
	hwopt.tstate_border_right=72;
	hwopt.hw_model=SPECMDL_48K;
	hwopt.int_type=NORMAL;
	hwopt.videopage=0;
	return 0;
	}

/* "Inves Spectrum +" is a (spanish clone) 48K, but:
 * without port FF. 
 * Int at start of screen (normal has int at0 start of border).
 * 16K RAM maped at 0x0000 as writeonly that modify INs function.
 * Custom ROM.
 */
int init_inves(char *romfile){
	FILE *fp;
	int i;
	printf(__FILE__": Init Inves Spectrum+ hardware.\n");
	mem.md= 0x3FFF ;
	mem.mp= 0xC000 ;
	mem.mr= 14;
	mem.np=4;
	mem.sp=16*1024;
	mem.p=(byte *)malloc((1+3+1)*mem.sp);
	if (mem.p==NULL){
		printf("NO MEMORY...\n");
		return 1;
	}
	// cargar rom de 16K
	if (romfile[0]==0)
		fp=findopen_file("inves.rom");
	else 
		fp=findopen_file(romfile);	
	i=0;
	while (!feof (fp))
		*(mem.p+i++) = fgetc (fp);
	fclose (fp);
    for(i=0x10000;i<0x14000;i++) mem.p[i]=0xff ;
    
	mem.roo=0x10000; //4*mem.sp;
	mem.vn=1;

	mem.ro[0]=mem.sro[0]=0;
	mem.wo[0]=mem.swo[0]=mem.roo;
	mem.ro[1]=mem.sro[1]=mem.wo[1]=mem.swo[1]=mem.sp;
	mem.ro[2]=mem.sro[2]=mem.wo[2]=mem.swo[2]=2*mem.sp;
	mem.ro[3]=mem.sro[3]=mem.wo[3]=mem.swo[3]=3*mem.sp;
	mem.vo[0]=0x4000;

	// ULA config
	hwopt.port_ff=0x00;			// 0xff = emulate the port,  0x00 alwais 0xFF
	hwopt.ts_lebo=24;			  // left border t states
	hwopt.ts_grap=128;			 // graphic zone t states
	hwopt.ts_ribo=24;			  // right border t states
	hwopt.ts_hore=48;			  // horizontal retrace t states
	hwopt.ts_line=224;			 // to speed the calc, the sum of 4 abobe
	hwopt.line_poin=16;		    // lines in retraze post interrup
	hwopt.line_upbo=48;		    // lines of upper border
	hwopt.line_grap=192;		   // lines of graphic zone = 192
	hwopt.line_bobo=48;		    // lines of bottom border
	hwopt.line_retr=8;		     // lines of the retrace
	hwopt.TSTATES_PER_LINE=224;
	hwopt.TOP_BORDER_LINES=64;
	hwopt.SCANLINES=192;
	hwopt.BOTTOM_BORDER_LINES=56;
	hwopt.tstate_border_left=24;
	hwopt.tstate_graphic_zone=128;
	hwopt.tstate_border_right=72;
	hwopt.hw_model=SPECMDL_INVES;
	hwopt.int_type=INVES;
	hwopt.videopage=0;
	return 0;
	}
	
	
int init_16k(char *romfile){
	FILE *fp;
	int i;
	printf(__FILE__": Init 16K hardware.\n");
	mem.md= 0x3FFF ;
	mem.mp= 0xC000 ;
	mem.mr= 14;
	mem.np=4;
	mem.sp=16*1024;
	mem.p=(byte *)malloc((1+1+1)*mem.sp);
	if (mem.p==NULL){
		printf("NO MEMORY...\n");
		return 1;
	}
	// cargar rom de 16K
	if (romfile[0]==0)
		fp=findopen_file("spectrum.rom");
	else 
		fp=findopen_file(romfile);
	i=0;
	while (!feof (fp))
		*(mem.p+i++) = fgetc (fp);
	fclose (fp);
	mem.roo=2*mem.sp;
	mem.vn=1;

	mem.ro[0]=mem.sro[0]=0;
	mem.wo[0]=mem.swo[0]=mem.roo;
	mem.ro[1]=mem.sro[1]=mem.wo[1]=mem.swo[1]=mem.sp;
	mem.ro[2]=mem.sro[2]=mem.wo[2]=mem.swo[2]=mem.sp;
	mem.ro[3]=mem.sro[3]=mem.wo[3]=mem.swo[3]=mem.sp;
	mem.vo[0]=0x4000;

	// ULA config
	hwopt.port_ff=0xFF;			// 0xff = emulate the port,  0x00 alwais 0xFF
	hwopt.ts_lebo=24;			  // left border t states
	hwopt.ts_grap=128;			 // graphic zone t states
	hwopt.ts_ribo=24;			  // right border t states
	hwopt.ts_hore=48;			  // horizontal retrace t states
	hwopt.ts_line=224;			 // to speed the calc, the sum of 4 abobe
	hwopt.line_poin=16;		    // lines in retraze post interrup
	hwopt.line_upbo=48;		    // lines of upper border
	hwopt.line_grap=192;		   // lines of graphic zone = 192
	hwopt.line_bobo=48;		    // lines of bottom border
	hwopt.line_retr=8;		     // lines of the retrace
	hwopt.TSTATES_PER_LINE=224;
	hwopt.TOP_BORDER_LINES=64;
	hwopt.SCANLINES=192;
	hwopt.BOTTOM_BORDER_LINES=56;
	hwopt.tstate_border_left=24;
	hwopt.tstate_graphic_zone=128;
	hwopt.tstate_border_right=72;
	hwopt.hw_model=SPECMDL_16K;
	hwopt.int_type=NORMAL;
	hwopt.videopage=0;
	return 0;
}

int init_128k(char *romfile){
	FILE *fp;
	int i;
	mem.md= 0x3FFF ;
	mem.mp= 0xC000 ;
	mem.mr= 14;
	mem.np=4;
	mem.sp=16*1024;
	mem.vn=2;
	mem.p=(byte *)malloc((8+2+1)*mem.sp);
	// cargar rom de 32K
	fp=findopen_file("128spanish.rom");	
	i=mem.sp*8;
	while (!feof (fp))
		*(mem.p+i++) = fgetc (fp);
	fclose (fp);
	mem.roo=10*mem.sp;
	mem.ro[0]=mem.sro[0]=mem.sp*8;
	mem.wo[0]=mem.swo[0]=mem.roo;
	mem.ro[1]=mem.sro[1]=mem.wo[1]=mem.swo[1]=5*mem.sp;
	mem.ro[2]=mem.sro[2]=mem.wo[2]=mem.swo[2]=2*mem.sp;
	mem.ro[3]=mem.sro[3]=mem.wo[3]=mem.swo[3]=0*mem.sp;
	mem.vo[0]=5*mem.sp;
	mem.vo[1]=7*mem.sp;
	// ULA config
	hwopt.port_ff=0xFF;			// 0xff = emulate the port,  0x00 alwais 0xFF
	hwopt.ts_lebo=24;			  // left border t states
	hwopt.ts_grap=128;			 // graphic zone t states
	hwopt.ts_ribo=24;			  // right border t states
	hwopt.ts_hore=48;			  // horizontal retrace t states
	hwopt.ts_line=224;			 // to speed the calc, the sum of 4 abobe
	hwopt.line_poin=16;		    // lines in retraze post interrup
	hwopt.line_upbo=48;		    // lines of upper border
	hwopt.line_grap=192;		   // lines of graphic zone = 192
	hwopt.line_bobo=48;		    // lines of bottom border
	hwopt.line_retr=8;		     // lines of the retrace
	hwopt.TSTATES_PER_LINE=224;
	hwopt.TOP_BORDER_LINES=64;
	hwopt.SCANLINES=192;
	hwopt.BOTTOM_BORDER_LINES=56;
	hwopt.tstate_border_left=24;
	hwopt.tstate_graphic_zone=128;
	hwopt.tstate_border_right=72;
	hwopt.hw_model=SPECMDL_128K;
	hwopt.int_type=NORMAL;
	hwopt.videopage=0;
	hwopt.BANKM=0x00;
	return 0;
}

int reset_128k(void){
   hwopt.BANKM=0x00; /* necesario para que la siguiente linea funcione */
   outbankm_128k(0x00);
   return 0;
}

void outbankm_128k(byte dato){
 
   if ((hwopt.BANKM | 0xDF) == 0xFF) return; /* El bloqueo en el bit 5 */
   
   hwopt.BANKM=dato;
   hwopt.videopage= (dato & 0x08) >> 3 ;
   mem.ro[0]=mem.sro[0]=mem.sp*(8 + ((dato & 0x10) >> 4) );
//  pagein(0x4000,0,(dato & 0x10) >> 4,RO_PAGE,SYSTEM_PAGE);
   // definitivamente la pagina 5 se queda quieta y es la ula la que accede a la 7 o la 5.
   // mem.ro[1]=mem.sro[1]=mem.wo[1]=mem.swo[1]=( (dato & 0x08)==0x08 ? 7 : 5 ) *mem.sp;
   mem.ro[3]=mem.sro[3]=mem.wo[3]=mem.swo[3]=(dato & 0x07)*mem.sp;
//  pagein(0x4000,3,dato & 0x07,RW_PAGE,SYSTEM_PAGE);

}	

int init_plus2(char *romfile){
	int a,i;
	FILE *fp;
	a=init_128k(romfile);
	fp=findopen_file("plus2.rom");	
	i=mem.sp*8;
	while (!feof (fp))
		*(mem.p+i++) = fgetc (fp);
	fclose (fp);
	hwopt.hw_model=SPECMDL_PLUS2;
	return a;
}


int init_48k_if1(char *romfile){
	FILE *fp;
	int i;
	mem.md= 0x1FFF ;
	mem.mp= 0xE000 ;
	mem.mr= 13;
	mem.np=8;
	mem.sp=8*1024;
	mem.vn=1;
	mem.roo=8*mem.sp;

	mem.p=(byte *)malloc((16+48+8+8)*1024);
	// cargar rom de 16K y rom de 8K
	fp=findopen_file("spectrum.rom");	
	i=0;
	while (!feof (fp))
		*(mem.p+i++) = fgetc (fp);
	fclose (fp);
	fp=findopen_file("if1.rom");	
	i=0x10000;
	while (!feof (fp))
		*(mem.p+i++) = fgetc (fp);
	fclose (fp);
	//mem.ro[mem.np]={0*mem.sp,1*mem.sp,2*mem.sp,3*mem.sp,4*mem.sp,5*mem.sp,6*mem.sp,7*mem.sp};
	//mem.wo[mem.np]={mem.roo,mem.roo,2*mem.sp,3*mem.sp,4*mem.sp,5*mem.sp,6*mem.sp,7*mem.sp};
	//mem.sro[mem.np]={0*mem.sp,1*mem.sp,2*mem.sp,3*mem.sp,4*mem.sp,5*mem.sp,6*mem.sp,7*mem.sp};
	//mem.swo[mem.np]={mem.roo,mem.roo,2*mem.sp,3*mem.sp,4*mem.sp,5*mem.sp,6*mem.sp,7*mem.sp};
	//mem.vo[mem.vn]={1*16384};
	// ULA cfg
	return 1;
}

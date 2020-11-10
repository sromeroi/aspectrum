#include "stdafx.h"
// various machine initialization (48,plus2, ...)
#include <stdio.h>
#include "aspec.h"

extern int TSTATES_PER_LINE, TOP_BORDER_LINES, BOTTOM_BORDER_LINES, SCANLINES;


char initializeError[1024];	// text for error,

int
Z80MachineInitialization (void)
{
  // get a better way to switch between them


//      initialize48();
  return initialize128 ();

}



int
initialize48 (void)
{
  FILE *f;


  // load rom bank 0 only
  f = fopen ("spectrum.rom", "rb");
  fread (spectrumZ80.ROMbank[0], 1, 16384, f);
  fclose (f);

  spectrumZ80.paging_disabled = 1;
  spectrumZ80.shadow_screen = 0;

  // setup pages
  spectrumZ80.page[0] = spectrumZ80.ROMbank[ROM0];
  spectrumZ80.page[1] = spectrumZ80.RAMbank[RAM5];
  spectrumZ80.page[2] = spectrumZ80.RAMbank[RAM2];
  spectrumZ80.page[3] = spectrumZ80.RAMbank[RAM0];
  spectrumZ80.pagenum[0] = ROM0 + 32768;
  spectrumZ80.pagenum[1] = RAM5;
  spectrumZ80.pagenum[2] = RAM2;
  spectrumZ80.pagenum[3] = RAM0;

  TSTATES_PER_LINE = 224;
  TOP_BORDER_LINES = 64;
  BOTTOM_BORDER_LINES = 56;
  SCANLINES = 192;

  return 1;
}


int
initialize128 (void)
{
  FILE *f;


  // load rom bank 0 only
  f = fopen ("128.rom", "rb");
  if (f == NULL)
    printf ("128.rom not found. get ready to crash\n");
  fread (spectrumZ80.ROMbank[0], 1, 16384, f);
  fread (spectrumZ80.ROMbank[1], 1, 16384, f);
  fclose (f);

  spectrumZ80.paging_disabled = 0;
  spectrumZ80.shadow_screen = 0;
  // setup pages
  spectrumZ80.page[0] = spectrumZ80.ROMbank[ROM0];
  spectrumZ80.page[1] = spectrumZ80.RAMbank[RAM5];
  spectrumZ80.page[2] = spectrumZ80.RAMbank[RAM2];
  spectrumZ80.page[3] = spectrumZ80.RAMbank[RAM0];
  spectrumZ80.pagenum[0] = RAM0 + 32768;
  spectrumZ80.pagenum[1] = RAM5;
  spectrumZ80.pagenum[2] = RAM2;
  spectrumZ80.pagenum[3] = RAM0;

  TSTATES_PER_LINE = 228;
  TOP_BORDER_LINES = 63;
  BOTTOM_BORDER_LINES = 56;
  SCANLINES = 192;

  return 1;

}

// control of port 0x7ffd
void
port_7ffd (Z80Regs * regs, byte value)
{
  // check BIT 5
  if (value & 32)
    regs->paging_disabled = 1;
  if (regs->paging_disabled)
    return;

  // check bit 4 : page rom (warning! 128k & plus2 only)
  spectrumZ80.page[0] = spectrumZ80.ROMbank[(value >> 4) & 1];
  spectrumZ80.pagenum[0] = ((value >> 4) & 1) + 32768;

  // bit 3: shadow screen
  spectrumZ80.shadow_screen = (value >> 3) & 1;

  // check bits 0,1,2
  if (spectrumZ80.pagenum[3] != (value & 7))
    {
      spectrumZ80.page[3] = spectrumZ80.RAMbank[value & 7];
      spectrumZ80.pagenum[3] = value & 7;
    }

}

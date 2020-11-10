/*=====================================================================

  Sound code

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
 ======================================================================*/

#include "stdafx.h"
#define MAXSOUNDBUFFER 68192

#include <stdio.h>
#include "z80.h"
#include "v_alleg.h"

#include "sound.h"

extern Z80Regs spectrumZ80;
extern int gSoundInited;

int soundData[MAXSOUNDBUFFER];
// sound data. 
// Legend: 
//          lower part: TState where the out was generated. 
//          Bit 18 -> out #fe,0 ->0  out #fe,16 -> 1
int currentSoundPtr = -1;
// place in the soundData buffer where we'll store next out #fe


// initialize port #fe logging
void
initSoundLog (void)
{
  if (currentSoundPtr == -1)	// first time initialization
    {
      soundData[0] = 0;
      currentSoundPtr = 1;
    }
  else
    {				// at least this is the second frame
      // keep last's frame sound status
      soundData[0] = soundData[currentSoundPtr - 1] & 262144;
      currentSoundPtr = 1;
    }
}


// logSound logs sound data for an entire t-state cycle. Once finished
// sound is pushed to the Audio Card (latency = 1/50th)
void
logSound (int state)
{
  int val;

  if (currentSoundPtr >= MAXSOUNDBUFFER)
    return;			// no space, no log (should never happen, buffer is big enough)
  if (state > 0)
    state = 262144;		// 0 or 16 -> 0 or 262144

  if ((soundData[currentSoundPtr - 1] & 262144) == state)
    return;			// no need to log if it's the same state than before

  val = spectrumZ80.IPeriod - spectrumZ80.ICount;	// get current tstate
  val += state;			// add him beeper state (should be NOT (previous state)  )

  soundData[currentSoundPtr] = val;	// store
  currentSoundPtr++;

}

// builds a sound buffer "frame"
void
soundDump (void)
{
  int cur_sound, tgt;
  int i, j, cur;
  float tmp;
  byte *beeperSound;
//FILE *f;

  if (!gSoundInited)
    return;			// no init -> no sound -> bye

  // create sound, 44100, mono, 8 bits unsigned

  cur_sound = soundData[0] & 262144;	// get current beeper status
  if (cur_sound)
    cur_sound = 255;		// convert it to 0 or 255 (unsigned sound data)
  else
    cur_sound = 0;

  cur = 0;
  beeperSound = gGetSampleBuffer ();	// get the place to put the new sound
  if (beeperSound != NULL)
    {

      for (i = 1; i < currentSoundPtr; i++)	// for each logged port data
	{
	  tgt = soundData[i] & 262143;	// get tsate
	  tmp = tgt;
	  tmp *= (882.0 / 69888.0);	// scale to 0..881 not so slow as it seems, it's just an fmul ;)
	  j = (int) tmp;

	  for (; cur < j; cur++)
	    beeperSound[cur] = cur_sound;	// fill the buffer with current status until a new out #fe appears

	  cur_sound = soundData[i] & 262144;	// get next status
	  if (cur_sound)
	    cur_sound = 255;
	  else
	    cur_sound = 0;
	}

      // end filling all the buffer till its end
      for (; cur < 882; cur++)
	beeperSound[cur] = cur_sound;
    }
  initSoundLog ();		// reset sound logging

  gPlaySound ();		// let the music play :) (waits til last buffer was played)
}


// produce clean sound (to avoid repetitive loops in menus)
void
clearSoundBuffer (void)
{
  byte *ptr;
  int i, j;

  for (j = 0; j < 1; j++)
    {
      ptr = gGetSampleBuffer ();
      for (i = 0; i < 882; i++)
	ptr[i] = 128;
      gPlaySound ();
    }

}

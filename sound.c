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
#include <allegro5/allegro_audio.h>
#include "z80.h"
#include "v_alleg.h"

#include "sound.h"

extern Z80Regs spectrumZ80;
extern int gSoundInited;
extern ALLEGRO_AUDIO_STREAM *audioStream;

int soundData[MAXSOUNDBUFFER];  // FIXME be sure int=32bits.
// sound data. 
// Legend: 
//          lower part: TState where the out was generated. 
//          Bit 18 -> out #fe,0 ->0  out #fe,16 -> 1
int currentSoundPtr = -1;
// place in the soundData buffer where we'll store next out #fe


// initialize port #fe logging
void initSoundLog (void){
  if (currentSoundPtr == -1)	// first time initialization
    {
      soundData[0] = 0;
    }
  else
    {				// at least this is the second frame
      // keep last's frame sound status
      soundData[0] = soundData[currentSoundPtr - 1] & 0x40000;
    }
      currentSoundPtr = 1;
}


// logSound logs sound data for an entire t-state cycle. Once finished
// sound is pushed to the Audio Card (latency = 1/50th)
void logSound (int state){
  int val;

  if (currentSoundPtr >= MAXSOUNDBUFFER)
    return;			// no space, no log (should never happen, buffer is big enough)

  //PENDING si escuchamos el valor por el ear se generan 2 valores de tension
  // ademas en el inves, se hace un XOR entre ellos, por lo que pueden quedar mudos.
  // en la llamada ya se filtra mal con un and.

  if (state > 0)
    state = 0x40000;		// 0 or 16 -> 0 or 262144

  if ((soundData[currentSoundPtr - 1] & 0x40000) == state)
    return;			// no need to log if it's the same state than before

  val = spectrumZ80.IPeriod - spectrumZ80.ICount;	// get current tstate
  val += state;			// add him beeper state (should be NOT (previous state)  )

  soundData[currentSoundPtr] = val;	// store
  currentSoundPtr++;
}

// builds a sound buffer "frame"
void soundDump (void){
  //printf("soundDump1\n");
  int cur_sound, tgt;
  int i, j, cur;
  float tmp;
  uint8_t *beeperSound;
  //FILE *f;

  //if (!gSoundInited)
  //  return;			// no init -> no sound -> bye

  // create sound, 44100, mono, 8 bits unsigned

  cur_sound = soundData[0] & 0x40000;	// get current beeper status
  if (cur_sound)
    cur_sound = 255;		// convert it to 0 or 255 (unsigned sound data)
  else
    cur_sound = 0;

  cur = 0;
  //printf("soundDump2\n");
  do {
    beeperSound=al_get_audio_stream_fragment(audioStream);
  } while (beeperSound==NULL);
  //beeperSound = gGetSampleBuffer ();	// al_get_audio_stream_fragment  get the place to put the new sound
  //if (beeperSound != NULL)
  //  {
  //printf("soundDump3 %i\n",beeperSound);
  
      for (i = 1; i < currentSoundPtr; i++)	{ // for each logged port data
        tgt = soundData[i] & 0x3FFFF;	// get tsate
        tmp = tgt;
        tmp *= (882.0 / 69888.0);	// scale to 0..881 not so slow as it seems, it's just an fmul ;)
        j = (int) tmp;

        for (; cur < j; cur++){
           //printf("%i:%i->%i\n",cur,j,cur_sound);
           beeperSound[cur] = cur_sound;	// fill the buffer with current status until a new out #fe appears
        }
        cur_sound = soundData[i] & 0x40000;	// get next status
        if (cur_sound)
          cur_sound = 255;
        else
          cur_sound = 0;
	    }
  //printf("soundDump4\n");

      // end filling all the buffer till its end
      for (; cur < 882; cur++)
	      beeperSound[cur] = cur_sound;
//  }
  al_set_audio_stream_fragment(audioStream,beeperSound);
  if (!al_get_audio_stream_playing(audioStream)){
    al_set_audio_stream_playing(audioStream,1);
    printf("Tocala otra vez Sam\n");
  }
  initSoundLog ();		// reset sound logging
   //gPlaySound ();		// let the music play :) (waits til last buffer was played)
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

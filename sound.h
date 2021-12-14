/*=====================================================================

 sound.h

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
#ifndef _SOUND_H
#define _SOUND_H 1

#include <stdint.h>
#include "ayemu_8912.h"

void initSound (void);

void initSoundLog (void);

void logSound (int state);

void soundDump (void);

void clearSoundBuffer (void);

void ay8912_outFF(uint8_t reg);

void ay8912_outBF(uint8_t val);

uint8_t ay8912_inFF(void);

void ay8912_reset(void);

#endif	/* _SOUND_H */
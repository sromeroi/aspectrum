ASPECTRUM
=========
*POWERED BY Linux*

  Copyright (c) 2000
  Santiago Romero Iglesias  -  sromero@escomposlinux.org


  WHAT IS THIS PROGRAM FOR
-----------------------------------------------------------------------

 ASpectrum Emulator: This is our contribution to the Spectrum World.
 We're trying to release our simple, useable and portable Spectrum
 emulator, always thinking in the GNU/Linux community.


  HOW TO USE THIS PROGRAM
-----------------------------------------------------------------------

 By the moment, just run it without arguments to run the 48K spectrum
 ROM, or give it a filename to load snapshots  or tape files...

    ./aspectrum file.sna     (or .sp, or .tap, .tzx or .z80).

 Remember that loading of .tap files requieres a LOAD "" in the
 BASIC interpreter of the emulator. The LOAD command is put by
 pressing the 'J' key, and the " by pressing ALT+P (like in the
 real Spectrum).

 Also even the .tzx files are supported any kind of custom or turbo
 loading squeme is not supporting and don't work by now.
 
 For a resume of aspectrum command line args use "./aspectrum --help"

 You'll need Allegro 4.x to run this program.
 The font.* files MUST be located at the same directory where you
 run the aspectrum binary, or in /usr/share/aspectrum

 spectrum.rom MUST be in the same directory, in /usr/share/aspectrum
 or specified the ROM file by command line args.

 You can also specific a dump of a Interface 2 Cardtrige. 

 To use Gunstick Emulation you must have a working mouse and call
 the emulator with "./aspectrum -j G" (CASE SENSITIVE), or activate
 the gunstick emulation in General Options menu (F7).

 One time you start the gunstick emulation can avoid the "888"s locating
 the mouse in the border of the screen.

 You can emulate several version of Spectrum using de -m param, but
 the snapshots feature is only full functional on 48K.  see the help message
 to more information about what versions are avalible.

 NOTE THAT THIS EMULATOR IT'S NOT USER-FRIENDLY AS IT'S STILL IN
 FULL DEVELOPMENT STAGE.

 To compile the debug version you'll need to install MSS (Memory
 Supervision System) that generates .log files reporting memory
 management (pointer errors, allocations, etc.).


  ABOUT THIS PROGRAM
-----------------------------------------------------------------------

 * Currently it emulates several of the Sinclair/Investronica/Amstrad 
   spectrums, at least for memory and z80 (no disk or 128K Sound) until 
   I fix all the wrong Z80 opcodes (bugs, bugs, bugs :) and until all the
   undocumented opcodes are implemented. Then I'll add support for
   other features.

 * There are not implemented opcodes (although almost all games work,
   so they are not used very often) and ___at least one bug___ in the
   Z80 emulation that makes some games to fail (see WORKING file).

 * The emulator has a built-in debugger (very simple) that I use to
   search bugs (I usually comparte the output of my emulation with
   the real spectrum or another emulators).

 * It can load and save .SNA, .SP and .Z80 snapshot files.

 * It can load .TAP and .TZX files at "zero" speed (I've still not implemented
   the real-time loading routines), by using LOAD "" on the emulator.
   It still does not load protected games nor games launched by
   non-standard loaders. LOAD "" is writen pressing J and ALT+P and ALT+P.

 * I've tested the emulator with more than 100 games, and almost
   all were working right. If you found a game that don't work or refuse to
   load, and if you think that it not use any of the unsupported features,
   please, fell free to send it to the autors, to examine it.

 * All the coding style, documentation, etc. sucks :).


 LICENSE
-----------------------------------------------------------------------
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

 (...Read the file License on the emulator package...)


  THANKS TO...
-----------------------------------------------------------------------

 * Marat Fayzullin, Raul Glez, Philip Kendall and Ignacio Burguer:
   These people helped me a lot with the emulator: he explained me about
   Z80 emulation, pointed me to the right documentation, and so on.
   Ignacio tested my emulator against their own emulator to tell me
   which opcodes were not correctly emulated... to all THANKS A LOT.

   All these people have their own Z80 or Spectrum emulator, look for
   them on any search engine:

     Marat Fayzullin : z80 emulator (Z80 microprocessor).
     Raul Glez: R80 emulator (Spectrum).
     Philip Kendall: Fuse emulator (Spectrum).
     Ignacio Burguer: GLECK emulator (Spectrum).
     Z80user, Kak, Hylian, Kel: From #spectrum on IRC-Hispano.

 * Alvaro Alea, for submitting the first Aspectrum patch giving support
   to load Z80 snapshot files on the emulator.

 * Gines Flores, for trying to help me on the emulator.

 * Jose Manuel Claros, for the help on all the spectrum stuff.

 * Pedro Gimeno, for creating his Spectrum emulator (and for the readme!).

 * Grzegorz Adam Hankiewicz, for helping me on learning Allegro...
   and of course all the Allegro Development Team, for creating such
   a nice programming library. Read the allegro's AUTHORS file for
   all the people who has contributed to Shawn Hargreaves' initial Allegro.

 * Aspectrum use AGUP library, see http://agup.sourceforge.net and the source 
   code of Aspectrum for more information.

  Santiago Romero AkA NoP/Compiler.
  sromero@escomposlinux.org

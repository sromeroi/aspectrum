// machine data
#ifndef MACHINES_H
#define MACHINES_H

 int initialize48 (void);

int initialize128 (void);

int initializePlus2 (void);


void initPaging (void);

void port_7ffd (Z80Regs * regs, byte value);


int Z80MachineInitialization (void);




#endif	/* 
 */
  

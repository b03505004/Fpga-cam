/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2003 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/

/* IO Header file for Nios II Toolchain */

#include "io.h"



volatile signed long IORD(alt_32 BASE, alt_32 REGNUM){
  void* wanted_address = lw_virtual_base +  ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ((alt_u8*)BASE) + ((REGNUM) * (SYSTEM_BUS_WIDTH/8))) & ( unsigned long)( HW_REGS_MASK ) );
  //printf("IORead: %x\t%x\t%x\t%x\n", wanted_address, *((volatile alt_32 *)wanted_address), BASE, REGNUM);
  return  *((volatile alt_32 *)wanted_address);
}

void IOWR(alt_32 BASE, alt_32 REGNUM, alt_32 DATA){
  void* wanted_address = lw_virtual_base +  ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ((alt_u8*)BASE) + ((REGNUM) * (SYSTEM_BUS_WIDTH/8))) & ( unsigned long)( HW_REGS_MASK ) );
  
  *((volatile alt_32 *)wanted_address) = (DATA);
  //printf("IOWrite: %x\t%x\t%x\t%x\t%x\t%x\n", wanted_address, DATA, *((volatile alt_32 *)wanted_address), BASE, REGNUM, IORD(BASE, REGNUM+2));
  return;
}


void IOWR_32DIRECT(alt_32* BASE, alt_32 OFFSET, alt_32 DATA){
  void* wanted_address = lw_virtual_base +  ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ((alt_u8*)BASE) + (OFFSET)) & ( unsigned long)( HW_REGS_MASK ) );
  
  *((volatile alt_32 *)wanted_address) = (DATA);
  return;
}


signed long IORD_32DIRECT(alt_32* BASE, alt_32 OFFSET){
  void* wanted_address = lw_virtual_base +  ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ((alt_u8*)BASE) + (OFFSET)) & ( unsigned long)( HW_REGS_MASK ) );
  
  return  *((volatile alt_32 *)wanted_address);
}

void IOWR_16DIRECT(alt_32* BASE, alt_32 OFFSET, alt_16 DATA){
  void* wanted_address = lw_virtual_base +  ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ((alt_u8*)BASE) + (OFFSET)) & ( unsigned long)( HW_REGS_MASK ) );
  
  *((volatile alt_16 *)wanted_address) = (DATA);
  return;
}

void IOWR_8DIRECT(alt_32* BASE, alt_32 OFFSET, alt_8 DATA){
  void* wanted_address = lw_virtual_base +  ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ((alt_u8*)BASE) + (OFFSET)) & ( unsigned long)( HW_REGS_MASK ) );
  
  *((volatile alt_8 *)wanted_address) = (DATA);
  return;
}
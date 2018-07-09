#ifndef __IO_H__
#define __IO_H__

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

#include "alt_types.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef SYSTEM_BUS_WIDTH
#define SYSTEM_BUS_WIDTH 32
#endif

#define ALT_LWFPGASLVS_OFST 0xFF200000
#define HW_REGS_MASK ( 0x04000000 - 1 )

extern void *lw_virtual_base;
//signed long IORD(BASE, REGNUM);

//void IOWR(BASE, REGNUM, DATA);

/*#define __LW_VIRTUAL_BASE_NATIVE(BASE, REGNUM) \
  (((alt_u8*)lw_virtual_base)) + ())
*/
#define __IO_CALC_ADDRESS_NATIVE(BASE, REGNUM) \
	((void *)(lw_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ((alt_u8*)BASE) + ((REGNUM) * (SYSTEM_BUS_WIDTH/8)) ) & ( unsigned long)( HW_REGS_MASK ) )))

  //((void *)(((int)lw_virtual_base)+((unsigned long)ALT_LWFPGASLVS_OFST) + ((alt_u8*)BASE) + ((REGNUM) * (SYSTEM_BUS_WIDTH/8))))
#define IORD(BASE, REGNUM) \
  *((volatile int *)(__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM))))
  //__builtin_ldwio (__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM)))
#define IOWR(BASE, REGNUM, DATA) \
  (*((volatile int *)(__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM))))) = (DATA)
  //__builtin_stwio (__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM)), (DATA))

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */

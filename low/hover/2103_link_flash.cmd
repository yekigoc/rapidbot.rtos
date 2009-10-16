/* ****************************************************************************************************** */
/*   demo2106_blink_ram.cmd				LINKER  SCRIPT                                  	              */
/*                                                                                                        */
/*                                                                                                        */
/*   The Linker Script defines how the code and data emitted by the GNU C compiler and assembler are  	  */
/*   to be loaded into memory (code goes into RAM, variables go into RAM).                 			  */
/*                                                                                                        */
/*   Any symbols defined in the Linker Script are automatically global and available to the rest of the   */
/*   program.                                                                                             */
/*                                                                                                        */
/*   To force the linker to use this LINKER SCRIPT, just add the -T demo2106_blink_ram.cmd directive      */
/*   to the linker flags in the makefile.                                                                 */
/*                                                                                                        */
/*   			LFLAGS  =  -Map main.map -nostartfiles -T demo2106_blink_ram.cmd                          */
/*                                                                                                        */
/*                                                                                                        */
/*   The Philips boot loader supports the ISP (In System Programming) via the serial port and the IAP     */
/*   (In Application Programming) for flash programming from within your application.                     */
/*                                                                                                        */
/*   The boot loader uses RAM memory and we MUST NOT load variables or code in these areas.               */
/*                                                                                                        */
/*   RAM used by boot loader:  0x40000120 - 0x400001FF  (223 bytes) for ISP variables                     */
/*                             0x4000FFE0 - 0x4000FFFF  (32 bytes)  for ISP and IAP variables             */
/*                             0x4000FEE0 - 0x4000FFDF  (256 bytes) stack for ISP and IAP                 */
/*                                                                                                        */
/*                                                                                                        */
/*                              MEMORY MAP                                                                */
/*                      |                                 |0x40010000                                     */
/*            .-------->|---------------------------------|                                               */
/*            .         |                                 |0x4000FFFF                                     */
/*            .         |     variables and stack         |                                               */
/*            .         |     for Philips boot loader     |                                               */
/*            .         |         288 bytes               |                                               */
/*            .         |   Do not put anything here      |0x4000FEE0                                     */
/*            .         |---------------------------------|                                               */
/*            .         |    UDF Stack  4 bytes           |0x4000FEDC  <---------- _stack_end             */
/*            .         |---------------------------------|                                               */
/*            .         |    ABT Stack  4 bytes           |0x4000FED8                                     */
/*            .         |---------------------------------|                                               */
/*            .         |    FIQ Stack  4 bytes           |0x4000FED4                                     */
/*            .         |---------------------------------|                                               */
/*            .         |    IRQ Stack  4 bytes           |0x4000FED0                                     */
/*            .         |---------------------------------|                                               */
/*            .         |    SVC Stack  4 bytes           |0x4000FECC                                     */
/*            .         |---------------------------------|                                               */
/*            .         |                                 |0x4000FEC8                                	  */
/*            .         |     stack area for user program |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |.................................|                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |          free ram               |                                               */
/*           ram        |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |.................................|0x40000464 <---------- _bss_end                */
/*            .         |                                 |                                               */
/*            .         |  .bss   uninitialized variables |                                               */
/*            .         |.................................|0x40000448 <---------- _bss_start, _edata      */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |  .data  initialized variables   |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                    							  */
/*            .         |---------------------------------|0x40000430 <---------- _etext, _data           */
/*            .         |                                 |                                               */
/*            .         |                                 |0x40000344   main                              */
/*            .         |                                 |0x400002c4   Initialize                        */
/*            .         |   .text  C Code                 |0x400002c0   UNDEF_Routine                     */
/*            .         |                                 |0x400002bc   SWI_Routine                       */
/*            .         |        408 bytes                |0x400002b8   FIQ_Routine                       */
/*            .         |                                 |0x400002b4   IRQ_Routine                       */
/*            .         |---------------------------------|0x40000298   feed                              */
/*            .         |   .text  Startup Code(assembler)|                                               */
/*            .         |          116 bytes              |                                               */
/*            .         |---------------------------------|0x40000200                                     */
/*            .         |     variables used by           |0x400001FF                                     */
/*            .         |     Philips boot loader         |                                               */
/*            .         |           223 bytes             |                                               */
/*            .         |                                 |                                               */
/*            .         |   Do not put anything here      |                                               */
/*            .         |                                 |                                               */
/*            .         |---------------------------------|0x40000040                                     */
/*            .         |   .text Interrupt Vectors       |                                               */
/*            .         |         (re-mapped)             |0x40000000                                     */
/*            .         |          64 bytes               |                                               */
/*            .-------->|---------------------------------|                                               */
/*                      |                                 |                                               */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                      |                                 |                                               */
/*           .--------> |---------------------------------|                                               */
/*           .          |                                 |0x0001FFFF                                     */
/*           .          |                                 |                                               */
/*           .          |       unused flash eprom        |                                               */
/*           .          |                                 |                                               */
/*         flash        |          65472 bytes            |                                               */
/*           .          |                                 |                                               */
/*           .          |---------------------------------|0x00000040                                     */
/*           .          | Interrupt Vector Table   flash  |                                               */
/*           .          |          64 bytes               |                                               */
/*           .--------->|---------------------------------|0x00000000                                     *
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*  Author:  James P. Lynch                                                                               */
/*                                                                                                        */
/* ****************************************************************************************************** */


/* identify the Entry Point  */

/*ENTRY(Reset_Handler)*/
ENTRY(_startup)


/* specify the LPC2106 memory areas  */

MEMORY 
{
	flash : ORIGIN = 0x00000000, LENGTH = 32K	/* free FLASH EPROM area  */
	ram_isp_low(A) : ORIGIN = 0x40000040, LENGTH = 223
	ram : ORIGIN = 0x40000120, LENGTH = 8K		/* free RAM area  */
	ram_isp_high(A) : ORIGIN = 0x40001FE0, LENGTH = 32
}



/* define a global symbol _stack_end  */

_stack_end = 0x40001FDC;



/* now define the output sections  */

SECTIONS 
{
	.  = 0;
	startup : { *(.startup)} >flash  		/* the startup code goes into FLASH */
	
	

	.text :
	{
		*(.text)
		*(.rodata)
		*(.rodata*)
		*(.glue_7)
		*(.glue_7t)
		_etext = .;
	} >flash  							/* put all the above into FLASH */
	

	

	.data :			
	{
		_data = .;	
		*(.data)	
		_edata = .;	
	} >ram AT >flash     	

	.bss :	
	{
		_bss_start = .;
		*(.bss)		
	} >ram			

	. = ALIGN(4);		
	_bss_end = . ;		
}	
	
	_end = .;							/* define a global symbol marking the end of application RAM */

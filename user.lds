/*
 *  ZeOS - jcosta septembre 2006
 *  user.lds - Linker Script to create user memory image
 */

ENTRY(main)
SECTIONS
{
  . = 0x100000; /* User CODE will start at this address */
  .text : {
       *(.text.main);
       *(.text)
  }
  
  . = 0x107500;
  .thread_wrapper_sec : { *(.thread_wrapper_sec) }
  
  . = 0x108000; /* User DATA will start at this address */
  .rodata : { *(.rodata) }      /* Read Only Data */
  .data : { *(.data) }          /* Normal Data */
  .bss : { *(.bss) }            /* Uninitialized Data */
}

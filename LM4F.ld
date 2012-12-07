/*
Thumb2 Newlib Toolchain example project
Written by Elias Önal <EliasOenal@gmail.com>, released as public domain.

This is a minimal linkerscript that should be easy to port to new microcontrollers.
At this point it supports only C and will lead to weird behavior when used for C++.
*/

ENTRY(rst_handler)

MEMORY
{ 
  FLASH (rx) : ORIGIN = 0x0000000, LENGTH = 0x00040000
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

_estack = 0x20008000; /* Upper stack boundary */
_sstack = (_estack - 512); /* Lower stack boundary */

SECTIONS {
  .isr_vector :
  {
    . = ALIGN(4);
          KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
      _etext = .;
      _sidata = _etext;
  } > FLASH

  .data : AT ( _sidata )
  {
    . = ALIGN(4);
          _sdata = . ;
          _data = . ;
    *(.data)
    *(.data.*)
    . = ALIGN(4);
    _edata = . ;
  } >RAM

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    _bss = .;
    *(.bss)
    *(.bss.*)
    . = ALIGN(4);
      _ebss = . ;
  } >RAM
}

. = ALIGN(4);
end = .;
_end = .;

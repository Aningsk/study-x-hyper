/**
 * Linker script for hypervisor image
 * Notice: to use #include, need to process the linker script with cpp
 */
#include "arch/layout.h"

MEMORY
{
    RAM (rwx) : ORIGIN = HIMAGE_VADDR, LENGTH = 512M
}

PHDRS
{
    text PT_LOAD FLAGS(5); /* R=4 + X=1, is 5 */
    data PT_LOAD FLAGS(6); /* R=4 + W=2, is 6 */
}

EXTERN(_start)
ENTRY(_start)

SECTIONS
{
    /* hyper image link virtual addr, here equal to phys addr */
    . = HIMAGE_VADDR;

    .text.boot : {
      KEEP(*(.text.boot))
    } > RAM :text

    . = ALIGN(4096);

    .text : {
      *(.text) *(.text.*)
    } > RAM :text

    .rodata : {
      *(.rodata) *(.rodata.*)
    } > RAM :text

    /* End RX */
    . = ALIGN(4096);

    .data : {
      __data_start = .;
      *(.data) *(.data.*)
      __data_end = .;
    } > RAM :data

    . = ALIGN(4096);

    .bss : {
      __bss_start = .;
      *(.bss .bss.*)
      __bss_end = .;
    } > RAM :data

    . = ALIGN(4096);

    blk_pool_start = .;
    . += (0x1000 * 3);
    blk_pool_end   = .;
    . = ALIGN(4096);

    HIMAGE_END = .;
}

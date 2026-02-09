OUTPUT_ARCH("aarch64")

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80200000, LENGTH = 128M
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
    . = 0x80200000;
    .text : {
      *(.text) *(.text.*)
    } > RAM :text

    .rodata : {
      *(.rodata) *(.rodata.*)
    } > RAM :text

    .data : {
      *(.data) *(.data.*)
    } > RAM :data

    . = ALIGN(4096);
    .bss : {
      __bss_start = .;
      *(.bss .bss.*)
      __bss_end = .;
    } > RAM :data
}

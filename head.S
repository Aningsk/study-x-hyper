.section .text.boot, "ax"
.global  _start
.type    _start, function
.align 4

_start:
    /* Set stack for c code */
    adrp    x0, sp_stack
    /* Get Current code id */
    mrs     x1, mpidr_el1
    and     x1, x1, #0x0f
    add     x2, x1, 1
    mov     x3, #0x1000
    mul     x3, x3, x2
    add     x0, x0, x3
    mov     sp, x0
    bl      main
    /* spin here */
hang:
    wfe
    b       hang
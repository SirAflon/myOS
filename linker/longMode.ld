ENTRY(long_mode_start)

PHDRS {
    text PT_LOAD FLAGS(5);   /* R + X */
    data PT_LOAD FLAGS(6);   /* R + W */
}

SECTIONS {
    . = 0x80000;

    .text : {*(.text*)} :text
    .rodata : {*(.rodata*)} :text
    .data : {*(.data*)} :data
    .bss : {*(.bss*) *(COMMON)} :data

    _kernel_end = .;
}
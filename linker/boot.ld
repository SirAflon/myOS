ENTRY(_start)

SECTIONS{
    . = 0x7C00;

    .text : {*(.text*)}
    .data : {*(.data*)}
    .bss : {*(.bss*) *(COMMON)}

    . = 0x7C00 + 510;
    .bootsig : {
        BYTE(0x55)
        BYTE(0xAA)
    }
}
EXTERN(vector_table)
ENTRY(reset_handler)
MEMORY
{
 ram (rwx) : ORIGIN = 0x20000000, LENGTH = 48K
 rom (rx) : ORIGIN = 0x08000000, LENGTH = 126K
 settings (rx) : ORIGIN = 0x0801f800, LENGTH = 2K
 ram2 (rwx) : ORIGIN = 0x10000000, LENGTH = 16K
}
SECTIONS
{
 .text : {
  *(.vectors)
  *(.text*)
  . = ALIGN(4);
  *(.rodata*)
  . = ALIGN(4);
 } >rom
 .preinit_array : {
  . = ALIGN(4);
  __preinit_array_start = .;
  KEEP (*(.preinit_array))
  __preinit_array_end = .;
 } >rom
 .init_array : {
  . = ALIGN(4);
  __init_array_start = .;
  KEEP (*(SORT(.init_array.*)))
  KEEP (*(.init_array))
  __init_array_end = .;
 } >rom
 .fini_array : {
  . = ALIGN(4);
  __fini_array_start = .;
  KEEP (*(.fini_array))
  KEEP (*(SORT(.fini_array.*)))
  __fini_array_end = .;
 } >rom
 .ARM.extab : {
  *(.ARM.extab*)
 } >rom
 .ARM.exidx : {
  __exidx_start = .;
  *(.ARM.exidx*)
  __exidx_end = .;
 } >rom
 . = ALIGN(8);
 .settings (NOLOAD) : {
  *(.settings)
  KEEP (*(.settings))
 } >settings
 . = ALIGN(4);
 _etext = .;
 .data : {
  _data = .;
  *(.data*)
  . = ALIGN(4);
  _edata = .;
 } >ram AT >rom
 _data_loadaddr = LOADADDR(.data);
 .bss : {
  *(.bss*)
  *(COMMON)
  . = ALIGN(4);
  _ebss = .;
 } >ram
 .ram2 : {
  *(.ram2*)
  . = ALIGN(4);
 } >ram2
 /DISCARD/ : { *(.eh_frame) }
 . = ALIGN(4);
 end = .;
}
PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));

MEMORY
{
  intram      (rwx) : ORIGIN = 0x0000, LENGTH = 0xC4
  extram      (rwx) : ORIGIN = 0x2000, LENGTH = 0x1000
}

SECTIONS
{
  .text :
  {
    *(.main)
    *(.text)
  } > extram

  .data :
  {
    *(.data)
    *(.rodata)
  } > extram

  .bss :
  {
    _bss_start = .;
    *(.bss)
    *(COMMON)
    _bss_end = .;
  } > extram

  .softregs :
  {
    *(.softregs)
  } > intram
}

/* Internal peripherals */
_PORTA  = 0x1000;
_TCNTH  = 0x100E;
_TMSK2  = 0x1024;
_PACTL  = 0x1026;
_SCCR2  = 0x102D;
_SCSR   = 0x102E;
_SCDR   = 0x102F;

/* External peripherals */
_LCD_CTRL = 0x0600;
_LCD_DATA = 0x0601;
_STEPPER  = 0x0700;

/* Interrupt vectors */
_SCI_VECTOR  = 0x00C4;

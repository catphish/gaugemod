MEMORY
{
  ram      (rwx) : ORIGIN = 0x2000, LENGTH = 0x6000
}

SECTIONS
{
  .text :
  {
    *(.main)
  } > ram
}

_.z = 0x0000;
_.xy = 0x0002;
_.tmp = 0x0004;
_.d1 = 0x0010;
_.d2 = 0x0012;
_.d3 = 0x0014;
_.d4 = 0x0016;

_PORTA = 0x1000;
_SCSR = 0x102E;
_SCDR = 0x102F;
_ISR_TABLE_RTI = 0x00EB;
_ISR_TABLE_SWI = 0x00F4;

_STEPPER = 0x0700;

_LCD_CTRL = 0x0600;
_LCD_DATA = 0x0601;

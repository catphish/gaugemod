MEMORY
{
  ram      (rwx) : ORIGIN = 0x0000, LENGTH = 0x0100
}

SECTIONS
{
  .text : {
    *(.text)
  } > ram
}

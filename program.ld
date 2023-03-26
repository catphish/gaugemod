MEMORY
{
  ram      (rwx) : ORIGIN = 0x2000, LENGTH = 0x6000
}

SECTIONS
{
  .text : {
    *(.text)
  } > ram
}

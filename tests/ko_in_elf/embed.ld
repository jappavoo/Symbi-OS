SECTIONS
{
  .kmod : ALIGN(4)
  {
    KEEP(greeter.o(.data))      /* Ensure section isn't stripped */
  }

  /DISCARD/ : { *(*); } /* Drop all other sections to keep ELF minimal */
}

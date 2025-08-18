SECTIONS {
  .kmod : { KEEP(greeter.o(.data)) }
}

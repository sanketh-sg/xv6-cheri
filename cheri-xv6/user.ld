OUTPUT_ARCH("riscv")
ENTRY(_user_entry)

CAP_ALIGN = 0x20;

PHDRS {
  text PT_LOAD FLAGS(5); /* R + X */
  rodata PT_LOAD FLAGS(4); /* R */
  data PT_LOAD FLAGS(6); /* R + W */
}

SECTIONS {

  . = 0x00000000;
  /*. = 0x80200000;*/

  /* Text section (executable code) */
  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
    PROVIDE(etext = .);
  } :text


  /* Place unwind sections after text */
  .eh_frame_hdr : {
    *(.eh_frame_hdr)
  } :text


  .eh_frame : {
    *(.eh_frame)
  } :text


  /* Read-only data */
  .rodata : {
    . = ALIGN(CAP_ALIGN);
    *(.srodata .srodata.*)
    *(.rodata .rodata.*)
  } :rodata


  /* Capability relocation data */
  . = ALIGN(CAP_ALIGN);
  __cap_relocs : {
    *(__cap_relocs __cap_relocs.*)
  } :data


  /* Capability table - MUST come before .data */
  .captable : {
    . = ALIGN(CAP_ALIGN);
    *(.captable)
  } :data
  

  /* Initialized data */
  .data : {
    . = ALIGN(CAP_ALIGN);
    *(.sdata .sdata.*)
    *(.data .data.*)
  } :data


  /* Uninitialized data (BSS) */
  .bss ALIGN(0x1000): {
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .;
  } : data

  PROVIDE(end = .);
}

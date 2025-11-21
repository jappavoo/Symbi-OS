#include "common.h"
#include <asm/ptrace.h>



INTERPOSER_HANDLER(df_jmp_to_c, df_c_entry);


__attribute((unused)) void df_c_entry(struct pt_regs *pt_r){
  // Error code on DF is 0
  pt_r->orig_ax = USER_FT | WR_FT;
}


INTERPOSER_HANDLER(tf_jmp_to_c, tf_c_entry);

__attribute((unused)) void tf_c_entry(struct pt_regs *pt_r){
  // Stack or bust.
  /* char ins[] = "ins fetch\n"; */
  /* char wr[] = " wr\n"; */
  /* char str[] = "Stack\n"; */

  /* void (*tu_printk)(char *) = (void *)0xffffffff81c87049 ; */
  /* tu_printk(str); */


  if(pt_r->orig_ax & WR_FT){
    return;
  }

  pt_r->orig_ax |= USER_FT;

  /* /\* Are we an instruction fetch? *\/ */
  /* if(pt_r->error_code & INS_FETCH){ */
  /*   // We don't need to special case when in ring 3. */
  /*   if(! (pt_r->error_code & USER_FT)  ){ */
  /*     // Are we user code? */
  /*     // Could look in cr2, but by def rip caused the fault here. */
  /*     // This is modeled after kern:fault_in_kernel_space */
  /*     if(pt_r->rip < ( (1UL << 47) - 4096) ){ */
  /*       /// Lie that code was running in user mode. */
  /*       pt_r->error_code |= USER_FT; */
  /*       /\* tu_printk(ins); *\/ */
  /*       /\* print_ef(); *\/ */
  /*       /\* myprintki("my_ctr %d\n", my_ctr++); *\/ */
  /*     } */
  /*   } */
  /* } */
}
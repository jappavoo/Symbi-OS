#define PRESENT 1
#define WR_FT   1<<1
#define USER_FT 1<<2
#define INS_FETCH 1<<4


//this might become problematic in the kernel
#define NEW_HANDLER(FN)                      \
  __asm__(".text \n\t .align 16 \n\t .globl \t" #FN "\n\t" #FN ":");

#define CALL_TARG(FN) \
__asm__("call " #FN);

#define RET_TO_PG_FT                            \
  __asm__(" push   %rax                     \n\t\
  mov    $0xffffffff81001290,%rax           \n\t\
  xor    (%rsp),%rax /*Arlo's trick*/       \n\t\
  xor    %rax,(%rsp)                        \n\t\
  xor    (%rsp),%rax                        \n\t\
  ret                                           \
");

#define GET_PT_REG_PTR  __asm__("movq %rsp, %rdi");


#define PUSH_REGS \
  __asm__("\
  pushq   %rdi		/* pt_regs->di */ \n\t\
  pushq   %rsi		/* pt_regs->si */ \n\t\
  pushq	  %rdx		/* pt_regs->dx */ \n\t\
  pushq   %rcx		/* pt_regs->cx */ \n\t\
  pushq   %rax		/* pt_regs->ax */ \n\t\
  pushq   %r8		/* pt_regs->r8 */ \n\t\
  pushq   %r9		/* pt_regs->r9 */ \n\t\
  pushq   %r10		/* pt_regs->r10 */ \n\t\
  pushq   %r11		/* pt_regs->r11 */ \n\t\
  pushq	  %rbx		/* pt_regs->rbx */ \n\t\
  pushq	  %rbp		/* pt_regs->rbp */ \n\t\
  pushq	  %r12		/* pt_regs->r12 */ \n\t\
  pushq	  %r13		/* pt_regs->r13 */ \n\t\
  pushq	  %r14		/* pt_regs->r14 */ \n\t\
  pushq	  %r15		/* pt_regs->r15 */ \
");


#define POP_REGS \
__asm__("\
	popq %r15 \n\t\
	popq %r14 \n\t\
	popq %r13 \n\t\
	popq %r12 \n\t\
	popq %rbp \n\t\
	popq %rbx \n\t\
	popq %r11 \n\t\
	popq %r10 \n\t\
	popq %r9  \n\t\
	popq %r8  \n\t\
	popq %rax \n\t\
	popq %rcx \n\t\
	popq %rdx \n\t\
	popq %rsi \n\t\
	popq %rdi \
");



#define INTERPOSER_HANDLER(LAB, TARG)      \
  NEW_HANDLER(LAB)                              \
    PUSH_REGS                                   \
    GET_PT_REG_PTR                              \
    CALL_TARG(TARG)                             \
    POP_REGS                                    \
    RET_TO_PG_FT


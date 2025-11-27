#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#include "LINF/sym_all.h"
#include "L1/stack_switch.h"

static int module_loaded = 0;
static int verbose       = 1;

extern unsigned long kallsyms_lookup_name(const char *name);

#define VPRINTF(fmt, ...) do {   if (verbose) {				    fprintf(stderr, fmt, ##__VA_ARGS__);	  }						} while(0)

extern const uint8_t _binary_ext_ko_start[];
extern const uint8_t _binary_ext_ko_end[];
extern const uint8_t _binary_ext_ko_size;

// extern int load_module(struct load_info *info, char *uargs,int flags);
extern int __x64_sys_init_module(void* args);


//__x64_sys_init_module expects the arguments to be on the stack
void do_load_module(void* umod, unsigned long len, char* uargs, int* ret_out) {
    //prepare argument passing
    // mov    0x60(%rdi),%rdx
    // mov    0x68(%rdi),%rsi
    // mov    0x70(%rdi),%rdi
    
    uint64_t args[3];
    args[0] = (uint64_t)uargs; //rdx
    args[1] = (uint64_t)len; //rsi
    args[2] = (uint64_t)umod; //rdi

    int ret = __x64_sys_init_module((void*)(args) - 0x60);
    if (ret_out) {
        *ret_out = ret;
    }
}

static int
resolve_sym(char *name, void **value)
{
  char *error;
  dlerror();  // clear as per manpage
  *value = dlsym(RTLD_DEFAULT, name);
  error = dlerror();
  if (error != NULL) {
    fprintf(stderr, "%s\n", error);
    return 0;
  }
  return 1;
}

static int
force_symres_now()
{
  void *value;
  // force symbol resolution before we elevate
  
  //  volatile void * initmod_sym = dlsym(RTLD_DEFAULT, "__x64_sys_init_module");
  //  volatile void * topofstack_sym = dlsym(RTLD_DEFAULT, "cpu_current_top_of_stack");
  // kludge to force symbol reference and thus resolution if the above
  // does not work.... also see misc/find_kallsyms.c
  // volatile void * ksym = (void *)__x64_sys_init_module;
  // fprintf(stderr,"ksym:%p\n", ksym);
  //  fprintf(stderr, "__x64_sys_init_module:%p cpu_current_top_of_stack:%p\n",
  //  initmod_sym, topofstack_sym);
  
  // lookup symbols to avoid problems once elevated -- touch symbol tables
  if (!resolve_sym("__x64_sys_init_module", &value)) return 0;
  if (!resolve_sym("cpu_current_top_of_stack", &value)) return 0;
  return 1;
}

uintptr_t gsread(uintptr_t offset)
{
  uintptr_t val;
#if 1
  __asm__ volatile (
		    "mov %%gs:(%1), %0" 
		    : "=r" (val)
		    : "r" (offset)
		    : "memory" );
#else
  val = __readgsqword(offset);
#endif
  return val;
}

// Repoduced from symlib but removes dependency on
// hard coded cpu_current_top_of_stack gs offset

// Var better be in memory
#define DPLD_PRESERVE_USER_STACK(var) \
  asm volatile("mov %%rsp, %0" : "=m"(var) : : "memory");	\

#define DPLD_SWITCH_TO_KERN_STACK(offset) \
  asm volatile("mov %%gs:(%0), %%rsp" : : "r" (offset) :);

#define DPLD_RESTORE_USER_STACK(var)		\
  asm volatile("mov %0, %%rsp" : : "m"(var));

// These must be used as a pair
#define DPLD_ON_KERN_STACK(ktos)		\
  sym_elevate();				\
  uint64_t user_stack;				\
  DPLD_PRESERVE_USER_STACK(user_stack);		\
  DPLD_SWITCH_TO_KERN_STACK(ktos);

#define DPLD_ON_USER_STACK()		 \
  DPLD_RESTORE_USER_STACK(user_stack);	 \
  sym_lower();

// Combine the two above so we don't have to remember to call both
// but put all of user code inbetween
#define DPLD_ON_KERN_STACK_DO(ktos,fn)			\
  DPLD_ON_KERN_STACK(ktos);				\
  fn;							\
  DPLD_ON_USER_STACK();


//assume sym_elevate has been called before this function
int load_ext_module() {
  VPRINTF("starting load_ext_module\n");
  int ret = 0;
  size_t size = (size_t)&_binary_ext_ko_size;
  char * uargs = "name=Hansi";
  uintptr_t ktos;
  
  if (force_symres_now()==0) {
    fprintf(stderr, "ERROR: failed to resolve symbols needed\n");
    return 0;
  }
  if (!resolve_sym("cpu_current_top_of_stack", (void **)&ktos)) {
    VPRINTF("failed to resolve cpu_current_top_of_stack\n"); 
  }
  
  VPRINTF("ktos=%lx\n", ktos);
  
  VPRINTF("starting load_module\n");
  
  VPRINTF("do_load_module: umod=%p len=%lu uargs=%p\n", uargs, size, uargs);
  
  DPLD_ON_KERN_STACK_DO(ktos,
			do_load_module((void*)_binary_ext_ko_start, size, uargs, &ret));
  
  VPRINTF("do_load_module: exited __x64_sys_init_module ret=%d\n", ret);
  
  
  VPRINTF("exited load_module ret=%d\n", ret);
  return ret;
}

//resolves a symbol by name, loading the module if necessary
//this is for symbols from the kernel module included in our fat binary
void* dpld_resolver(char* symbol_name) {
  VPRINTF("%s: Resolving symbol %s\n", __func__, symbol_name);
  
  if (!module_loaded) {
    int rc;

    rc = load_ext_module();
    if (rc != 0) {
      VPRINTF("Failed to load ext module: %d\n", rc);
      exit(1);
    }
    VPRINTF("Loaded kallsyms module\n");
    
    module_loaded = 1;
  }
  
  sym_elevate();
  unsigned long addr = kallsyms_lookup_name(symbol_name);
  VPRINTF("Resolved symbol %s to address %p\n", symbol_name, (void*)addr);
  sym_lower();
  
  if (addr == 0) {
    VPRINTF("Symbol %s not found!\n", symbol_name);
    //exit program! We have a linkage problem
    exit(1);
  }
  return (void*)addr;
}

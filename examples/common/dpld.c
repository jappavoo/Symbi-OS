#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <elf.h>
#include "LINF/sym_all.h"
#include "L1/stack_switch.h"

#include "dpld.h"

static int module_loaded = 0;
static int verbose       = 1;

extern unsigned long kallsyms_lookup_name(const char *name);

#define VPRINTF(fmt, ...) do { \
  if (verbose) {				\
    fprintf(stderr, fmt, ##__VA_ARGS__);	\
  }						\
} while(0)

// These symbols are provided by the linker (from ext.o)
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

//assume sym_elevate has been called before this function
int load_included_module() {
    VPRINTF("starting load_included_module\n");
    
    int ret = 0;
    size_t size = (size_t)&_binary_ext_ko_size;
	
    VPRINTF("starting load_module\n");
    char * uargs = "name=Hansi";


    VPRINTF("do_load_module: umod=%p len=%lu uargs=%p\n", uargs, size, uargs);

    SYM_ON_KERN_STACK_DO(do_load_module((void*)_binary_ext_ko_start, size, uargs, &ret));
    
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
    
    VPRINTF("Loading kallsyms module\n");
    
    rc = load_included_module();
    if (rc != 0) {
      VPRINTF("Failed to load kallsyms module: %d\n", rc);
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

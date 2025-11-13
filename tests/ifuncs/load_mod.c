#include "load_mod.h"

FILE *ko_log_file = NULL;


void print_load_info(const struct load_info *info) {
    if (!info) {
        PRINTF("load_info: NULL\n");
        return;
    }
    PRINTF("load_info {\n");
    PRINTF("  name: %s\n", info->name ? info->name : "(null)");
    PRINTF("  mod: %p\n", info->mod);
    PRINTF("  hdr: %p\n", info->hdr);
    PRINTF("  len: %lu\n", info->len);
    PRINTF("  sechdrs: %p\n", info->sechdrs);
    PRINTF("  secstrings: %s\n", info->secstrings ? info->secstrings : "(null)");
    PRINTF("  strtab: %s\n", info->strtab ? info->strtab : "(null)");
    PRINTF("  symoffs: %lu\n", info->symoffs);
    PRINTF("  stroffs: %lu\n", info->stroffs);
    PRINTF("  init_typeoffs: %lu\n", info->init_typeoffs);
    PRINTF("  core_typeoffs: %lu\n", info->core_typeoffs);
    PRINTF("  debug: %p\n", info->debug);
    PRINTF("  num_debug: %u\n", info->num_debug);
    PRINTF("  sig_ok: %d\n", info->sig_ok);
    PRINTF("  mod_kallsyms_init_off: %lu\n", info->mod_kallsyms_init_off);
    PRINTF("  index: { sym: %u, str: %u, mod: %u, vers: %u, info: %u, pcpu: %u }\n",
           info->index.sym, info->index.str, info->index.mod,
           info->index.vers, info->index.info, info->index.pcpu);
    PRINTF("}\n");
}

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
    PRINTF("starting load_included_module\n");
    
    int ret = 0;
    size_t size = (size_t)&_binary_greeter_ko_size;
	
    PRINTF("starting load_module\n");
    char * uargs = "name=Hansi";


    PRINTF("do_load_module: umod=%p len=%lu uargs=%p\n", uargs, size, uargs);

    SYM_ON_KERN_STACK_DO(do_load_module((void*)_binary_greeter_ko_start, size, uargs, &ret));
    
    PRINTF("do_load_module: exited __x64_sys_init_module ret=%d\n", ret);
    
    
    PRINTF("exited load_module ret=%d\n", ret);
	return ret;
}
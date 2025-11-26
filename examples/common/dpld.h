#ifndef __DPLD_H__
#define __DPLD_H__

extern void *dpld_resolver(char * symbol_name);

//macro to define a function as an ifunc and to define a simple resolver that calls the base_resolver
#define DECLARE_IFUNC(name, rettype, args) \
    rettype name args __attribute__((ifunc(#name "_resolver"))); \
    static rettype (*name##_resolver(void)) args { \
        return (rettype (*) args) dpld_resolver(#name); \
    }  

#endif // __DPLD_H__

static void* base_resolver(char* symbol_name) {
    printf("base_resolver: Resolving symbol %s\n", symbol_name);
    
    return (void*)printf;
}
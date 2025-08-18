#include <stdio.h>
// #define __gs_based __attribute__((section(".gs_based")))

// // Declare a function with the custom attribute
// extern void my_special_function() __gs_based;

// extern int my_special_variable __gs_based;

// // Declare a variable with the custom attribute
// extern int my_special_variable __gs_based;
// // Declare the thread-local variable as extern
// extern __thread int tls_shared_variable;

// // Declare the function from the shared library
// extern void print_tls_variable();

// int main() {
//     printf("Initial TLS Variable: %d\n", tls_shared_variable);
//     tls_shared_variable = 100;
//     print_tls_variable();
//     return 0;
// }

extern int* my_pointer;
extern void my_special_function(void);
extern int my_special_variable;



int main() {
    // my_special_function();

    printf("my pointer: %p\n", my_pointer);
    printf("my_special_variable: %p\n", &my_special_function);
}

// gcc -fpic -c main.c -o main.o -mcmodel=large
// gcc -fpic main.o -o program -L. -lmylib
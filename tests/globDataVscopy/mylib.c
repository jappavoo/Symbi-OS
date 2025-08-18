#include <stdio.h>

// Define a thread-local variable in the shared library
__attribute__((visibility("default")))
int my_special_variable = 42;

__attribute__((visibility("default")))
int* my_pointer;

__attribute__((visibility("default")))
void my_special_function() {
    my_pointer = &my_special_variable;
    printf("my_special_variable in Shared Library: %d\n", my_special_variable);
}
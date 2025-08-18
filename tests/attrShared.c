#include <stdio.h>

// Define a thread-local variable in the shared library
int my_special_variable = 42;

void my_special_function() {
    printf("my_special_variable in Shared Library: %d\n", my_special_variable);
}
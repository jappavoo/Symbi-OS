#include <stdio.h>

// Define a thread-local variable in the shared library
__thread int tls_shared_variable = 42;

void print_tls_variable() {
    printf("TLS Variable in Shared Library: %d\n", tls_shared_variable);
}
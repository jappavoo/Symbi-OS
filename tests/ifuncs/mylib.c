#include <stdio.h>
#include <string.h>


int my_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}
/* Export as strcmp with default version 'myv' */
asm(".symver my_strcmp,strcmp@myv");

int my_strcmp2(const char* s1, const char* s2) {
    return strcmp(s1, s2);
}
asm(".symver my_strcmp2,strcmp@myv2");

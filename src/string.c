#include "types.h"
#include "string.h"

val strlen(val* s) {
    static val len;
    while(*s) {
        ++s;
        ++len;
    }
    return len;
}

void strcat(val* dst, val* src) {
    while(*dst)
        ++dst;

    while(*src) {
        *dst = *src;
        ++dst;
        ++src;
    }

    *dst = '\0';
}

void strncpy(val* dst, val* src, val n) {
    static val i;
    for(i = 0; i < n; ++i) {
        *dst = *src;
        ++dst;
        ++src;
    }
}

val* itoa(val n, val* str) {
    val* ptr = str + 4;
    *ptr = '\0';

    do {
        *--ptr='0' + n%10;
        n /= 10;
    } while (0 < n);

    return ptr;
}

void memset(val *ptr, val v, val n) {
    static val i;
    for(i = 0; i < n; i++) {
        ptr[i] = v;
    }
}

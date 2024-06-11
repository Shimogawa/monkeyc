#include "monkey.h"
#include <stdio.h>

__attribute__((noinline)) int plus1(int i) {
    return i + 1;
}

__attribute__((noinline)) int plus2(int i) {
    return i + 2;
}

int main(void) {
    printf("%d\n", plus1(3));
    monkeyc_patched p = monkeyc_patch(plus1, plus2);
    printf("%d\n", plus1(3));
    monkeyc_unpatch(&p);
    printf("%d\n", plus1(3));
    return 0;
}

#include "monkey.h"
#include <stdio.h>

#ifndef NDEBUG
#define ASSERT(condition, message)                                           \
    do {                                                                     \
        if (!(condition)) {                                                  \
            fprintf(stderr,                                                  \
                    "Assertion `" #condition "` failed in %s line %d: %s\n", \
                    __FILE__, __LINE__, message);                            \
            exit(1);                                                         \
        }                                                                    \
    } while (0)
#else
#error "This test file must be run in debug mode"
#endif

__attribute__((noinline)) int plus1(int i) {
    return i + 1;
}

__attribute__((noinline)) int plus2(int i) {
    return i + 2;
}

int main(void) {
    ASSERT(plus1(1) == 2, "plus1(1) should be 2");
    monkeyc_patched p = monkeyc_patch(plus1, plus2);
    ASSERT(plus1(1) == 3, "plus1(1) should be 3 after patching");
    monkeyc_unpatch(&p);
    ASSERT(plus1(1) == 2, "plus1(1) should be 2 after removing patch");

    return 0;
}

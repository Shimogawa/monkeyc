# Monkey patching in C

A single-header library that provides monkey patching in C.

**THIS HEADER SHOULD ONLY BE USED FOR TESTING PURPOSES.**

## Usage

```c
#include "monkey.h"
#include <stdio.h>

__attribute__((noinline)) int plus1(int i) {
    return i + 1;
}

__attribute__((noinline)) int plus2(int i) {
    return i + 2;
}

int main(void) {
    printf("%d\n", plus1(3));  // 4
    monkeyc_patched p = monkeyc_patch(plus1, plus2);
    printf("%d\n", plus1(3));  // 5
    monkeyc_unpatch(&p);
    printf("%d\n", plus1(3));  // 4
    return 0;
}
```

## References

- https://github.com/mehcode/guerrilla
- https://github.com/bouk/monkey



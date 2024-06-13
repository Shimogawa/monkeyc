# Monkey patching in C and C++

![example workflow](https://github.com/Shimogawa/monkeyc/actions/workflows/cmake.yml/badge.svg)

A single-header library that provides monkey patching in C and C++.

**THIS HEADER SHOULD ONLY BE USED FOR TESTING PURPOSES.**

## Usage

### C

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

### C++

```c++
#include "monkey.h"
#include <iostream>

__attribute__((noinline)) int plus1(int i) {
    return i + 1;
}

int main(void) {
    std::cout << plus1(1) << std::endl;
    {
        auto g = monkeyc::patch(plus1, [](int i) { return i + 2; });
        std::cout << plus1(1) << std::endl;
    }
    std::cout << plus1(1) << std::endl;
    return 0;
}
```

## References

- https://github.com/mehcode/guerrilla
- https://github.com/bouk/monkey



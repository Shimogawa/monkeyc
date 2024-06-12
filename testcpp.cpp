#include "monkey.h"
#include <iostream>

#ifndef NDEBUG
#define ASSERT(condition, message)                                             \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(1);                                                      \
        }                                                                      \
    } while (false)
#else
#error "This test file must be run in debug mode"
#endif

__attribute__((noinline)) int plus1(int i) {
    return i + 1;
}

__attribute__((noinline)) int plus2(int i) {
    return i + 2;
}

class A {
public:
    int c = 3;
    int a();
};

int A::a() {
    return this->c;
}

int breakA(A *a) {
    return a->c + 1;
}

void test_normal_function() {
    ASSERT(plus1(1) == 2, "plus1(1) should be 2");
    {
        auto g = monkeyc::patch(plus1, plus2);
        ASSERT(plus1(1) == 3, "plus1(1) should be 3 after patching");
    }
    ASSERT(plus1(1) == 2, "plus1(1) should be 2 after removing patch");
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
void test_member_function() {
    A a;
    ASSERT(a.a() == 3, "a.a() should be 3");
    {
        auto g = monkeyc::patch(&A::a, breakA);
        ASSERT(a.a() == 4, "a.a() should be 4 after patching");
        a.c = 5;
        ASSERT(a.a() == 6, "a.a() should be 6 after modifying member var");
    }
    ASSERT(a.a() == 5, "a.a() should be 5 after removing patch");
}
#pragma clang diagnostic pop

void test_lambda() {
    ASSERT(plus1(1) == 2, "plus1(1) should be 2");
    {
        auto g = monkeyc::patch(&plus1, [](int i) { return i + 2; });
        ASSERT(plus1(1) == 3, "plus1(1) should be 3 after patching");
    }
    ASSERT(plus1(1) == 2, "plus1(1) should be 2 after removing patch");
}

int main() {
    test_normal_function();
    test_member_function();
    test_lambda();
    return 0;
}
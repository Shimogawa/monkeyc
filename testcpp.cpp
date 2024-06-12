#include "monkey.h"
#include <iostream>

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

int main() {
    std::cout << plus1(1) << std::endl;
    {
        auto g = monkeyc::patch(plus1, plus2);
        std::cout << plus1(1) << std::endl;
    }
    std::cout << plus1(1) << std::endl;

    std::cout << "-----" << std::endl;

    A a;
    std::cout << a.a() << std::endl;
    {
        auto g = monkeyc::patch(&A::a, breakA);
        std::cout << a.a() << std::endl;
    }
    std::cout << a.a() << std::endl;

    std::cout << "-----" << std::endl;

    std::cout << plus1(1) << std::endl;
    {
        auto g = monkeyc::patch(&plus1, [](int i) { return i + 114514; });
        std::cout << plus1(1) << std::endl;
    }
    std::cout << plus1(1) << std::endl;
}
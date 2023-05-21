#include <iostream>
#include <vector>

struct A { int x; };
struct B { char y; };
struct C { int z[1024]; };

union Base {
    A a;
    B b;
    C c;
};

enum Tag {
    A_TAG,
    B_TAG,
    C_TAG
};

union SuperFloat {
    float f;
    int i;
};

int RawMantissa(SuperFloat sf) {
    return sf.i & ((1 << 23) - 1);
}

int main() {
    SuperFloat sf;
    sf.f = 10.5f;
    int mantissa = RawMantissa(sf);
    std::cout << sf.i << std::endl;

    // -------------------------------
    std::vector<std::pair<Base, Tag>> v;
    Base base;
    std::cout << sizeof(base) << std::endl;
    base.a = A();
    std::cout << sizeof(base) << std::endl;
    v.push_back({base, A_TAG});
    base.b = B();
    std::cout << sizeof(base) << std::endl;
    v.push_back({base, B_TAG});
    base.c = C();
    std::cout << sizeof(base) << std::endl;
    v.push_back({base, C_TAG});
}

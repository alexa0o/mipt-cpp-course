#include <array>

class A {
public:
    A() = delete;
    A(int) { }
};

int main() {
    std::array<A, 3> a { 1, 2, 3 };
}

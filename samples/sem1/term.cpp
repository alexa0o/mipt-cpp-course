#include <iostream>
#include <cstdlib>
#include <exception>

void f() {
    std::cout << "Exception!!!\n" << std::flush;
    std::abort();
}

int main() {
    std::set_terminate(f);
    throw 1;
}

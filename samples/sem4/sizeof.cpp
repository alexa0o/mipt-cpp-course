#include <vector>
#include <array>
#include <span>
#include <iostream>


int main() {
    std::vector<int> vector;
    auto sv = std::span{vector};
    std::cout << sizeof(sv) << '\n';

    std::array<int, 10> array;
    auto sa = std::span{array};
    std::cout << sizeof(sa) << '\n';

    int c_array[10];
    auto sca = std::span{c_array};
    std::cout << sizeof(sca) << '\n';
}

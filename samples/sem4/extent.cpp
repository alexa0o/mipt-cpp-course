#include <span>
#include <array>
#include <iostream>
#include <vector>

template<typename T, std::size_t length>
void print_extent(std::span<T, length> span) {
    std::cout << std::hex << "0x" << span.extent << '\n';
}

int main() {
    std::array array = {1,2,3,4,5};

    auto span = std::span{array};
    print_extent(span);

    print_extent(span.subspan(1, 3));

    std::vector<int> vector;
    print_extent(std::span{vector});

    int c_array[] = {42};
    print_extent(std::span{c_array});
}

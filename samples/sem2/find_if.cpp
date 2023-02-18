#include <vector>
#include <iostream>

template< class InputIt, class UnaryPredicate >
constexpr InputIt my_find_if( InputIt first, InputIt last, UnaryPredicate p ) {
    for (InputIt it = first; it != last; ++it) {
        if (p(*it)) {
            return it;
        }
    }
    return last;
}

bool is_even(int v) {
    return v % 2 == 0;
}

int main() {
    std::vector<int> v{ 1, 2, 3, 4, 5 };
    auto it = my_find_if(v.begin(), v.end(), is_even);
    std::cout << *it << std::endl;
}

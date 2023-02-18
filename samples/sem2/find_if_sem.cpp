#include <vector>
#include <iostream>

template <class InputIt, class UnaryPredicat>
InputIt my_find_if(InputIt begin, InputIt end, UnaryPredicat p) {
    for (; begin != end; ++begin) {
        if (p(*begin)) {
            break;
        }
    }

    return begin;
}

bool is_even(int x) {
    return x % 2 == 0;
}

int main() {
    std::vector<int> v{ 1, 2, 2, 3 };
    if (my_find_if(v.begin(), v.end(), is_even) != v.end()) {
        std::cout << "Even is: " << *my_find_if(v.begin(), v.end(), is_even) << std::endl;
    }
}

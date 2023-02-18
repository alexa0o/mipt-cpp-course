#include <algorithm>
#include <iostream>
#include <vector>

template<class ForwardIt, class T>
ForwardIt my_remove(ForwardIt first, ForwardIt last, const T& value)
{
    first = std::find(first, last, value);
    if (first != last)
        for (ForwardIt i = first; ++i != last;)
            if (!(*i == value))
                *first++ = std::move(*i);
    return first;
}

int main() {
    std::vector<int> v{ 1, 2, 3, 4, 5 };
    my_remove(v.begin(), v.end(), 3);
    for (auto x : v) {
        std::cout << x << std::endl;
    }
    std::cout << "Size: " << v.size() << "\nCapacity: " << v.capacity() << std::endl;
}

#include <algorithm>
#include <vector>
#include <iostream>

template< class ForwardIt, class T >
constexpr ForwardIt my_remove( ForwardIt first, ForwardIt last, const T& value ) {
    // 1, 2, 3, 4, 5 -> 1, 2, 4, 5, 5
    // 1, 2, 4, 4, 5
    // 1, 2, 4, 5, 5
    first = std::find(first, last, value);
    if (first != last) {
        for (ForwardIt i = first; ++i != last;) {
            if (*i != value) {
                *first++ = *i;
            }
        }
    }

    return first;
}

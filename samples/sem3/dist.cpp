#include <set>
#include <iterator>
#include <iostream>

namespace detail {
 
template<class It>
typename std::iterator_traits<It>::difference_type 
    do_distance(It first, It last, std::input_iterator_tag)
{
    typename std::iterator_traits<It>::difference_type result = 0;
    while (first != last) {
        ++first;
        ++result;
    }
    return result;
}
 
template<class It>
typename std::iterator_traits<It>::difference_type 
    do_distance(It first, It last, std::random_access_iterator_tag)
{
    return last - first;
}
 
} // namespace detail
 
template<class It>
typename std::iterator_traits<It>::difference_type 
    mdistance(It first, It last)
{
    return detail::do_distance(first, last,
                               typename std::iterator_traits<It>::iterator_category());
}

int main() {
    std::set<int> s{ 1, 2, 3, 4, 5, 6 };
    std::cout << mdistance(s.begin(), s.end()) << std::endl;
}

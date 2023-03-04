#include <iostream>

template <class T>
class my_istream_iterator {
public:
    my_istream_iterator(std::istream& in) : in_(in) {
        in_ >> value_;
    }

    my_istream_iterator(const my_istream_iterator<T>& mit) : in_(mit.in_), value_(mit.value_) {
    }

    my_istream_iterator<T>& operator++() {
        in_ >> value_;
        return *this;
    }

    T& operator*() {
        return value_;
    }

private:
    std::istream& in_;
    T value_;
};

int main() {
    my_istream_iterator<int> mit1(std::cin);
    auto mit2 = mit1;
    ++mit1; // 0 1 2 3 4 : mit1 = 1, mit2 = 0
    ++mit2; // 0 1 2 3 4 : mit1 = 1, mit2 = 2
}

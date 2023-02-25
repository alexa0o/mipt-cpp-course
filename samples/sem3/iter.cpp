#include <vector>

template <class T, bool IsConst>
class VectorIterator {
public:
    using Type = std::conditional_t<IsConst, const T, T>;
    using VectorType = std::conditional_t<IsConst, const std::vector<T>, std::vector<T>>;

    VectorIterator(VectorType& v, size_t indx = 0)
    : v_(v), indx_(indx) { }

    VectorIterator(VectorIterator<T, false>& vi)
    : v_(vi.v_), indx_(vi.indx_) { }

    VectorIterator<T, IsConst> operator+(size_t value) {
        return VectorIterator<T, IsConst>{v_, indx_ + value};
    }

    VectorIterator<T, IsConst>& operator+=(size_t value) {
        indx_ += value;
        return *this;
    }

    VectorIterator<T, IsConst>& operator++() {
        ++indx_;
        return *this;
    }

    Type& operator*() {
        return v_[indx_];
    }

    Type* operator->() {
        return &v_[indx_];
    }

    friend VectorIterator<T, !IsConst>;
public:
    using difference_type = size_t;
    using iterator_category = std::input_iterator_tag;
    using value_type = Type;
    using reference = Type&;
private:
    VectorType& v_;
    size_t indx_;
};

int main() {
    std::vector<int> v{ 1, 2, 3 };
    VectorIterator<int, false> vi(v);
    VectorIterator<int, true> cvi = vi;
    VectorIterator<int, true> cvi1 = cvi;
    std::advance(vi, 1);
}

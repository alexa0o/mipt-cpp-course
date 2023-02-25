#include <vector>
#include <iterator>
#include <iostream>

template <class T, bool IsConst = false>
class VectorIterator {
public:
    using Type = std::conditional_t<IsConst, const T, T>;
    using VectorType = std::conditional_t<IsConst, const std::vector<T>, std::vector<T>>;

    VectorIterator(VectorType& v, size_t indx = 0)
    : v_(v), indx_(indx) { }

    template <bool IsFromConst>
    VectorIterator(VectorIterator<T, IsFromConst>& vi)
    : v_(vi.v_), indx_(vi.indx_) {
        static_assert(IsConst || IsFromConst == false, "Create non-const iterator from const");
    }

    VectorIterator<T, IsConst> operator+(size_t value) {
        return VectorIterator<T, IsConst>{v_, indx_ + value};
    }

    VectorIterator<T, IsConst>& operator+=(size_t value) {
        indx_ += value;
        return *this;
    }

    Type& operator*() {
        return v_[indx_];
    }

    Type* operator->() {
        return &v_[indx_];
    }

    friend VectorIterator<T, !IsConst>;
private:
    VectorType& v_;
    size_t indx_;
};

template <class T>
VectorIterator<T> GetBegin(std::vector<T>& v) {
    return VectorIterator<T>(v, 0);
}

template <class T>
VectorIterator<T> GetEnd(std::vector<T>& v) {
    return VectorIterator<T>(v, v.size());
}

template <class T>
VectorIterator<T, true> GetCBegin(const std::vector<T>& v) {
    return VectorIterator<T, true>(v, 0);
}

template <class T>
VectorIterator<T, true> GetCEnd(const std::vector<T>& v) {
    return VectorIterator<T, true>(v, v.size());
}
int main() {
    std::vector<int> v{ 1, 2, 3, 4, 5 };
    auto cvi = GetCBegin(v);
    auto vi = GetBegin(v);
    auto cvi2 = vi;
}

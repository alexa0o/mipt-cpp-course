#include <gtest/gtest.h>

#include <vector>
#include <string>

#include "transform.hpp"

namespace nvector {
bool p(const int& x) {
    return x % 2 != 0;
}

void f(int& x) {
    x *= 2;
}
}

TEST(TransformIfWorks, vector) {
    std::vector<int> v{1, 2, 3, 4};
    TransformIf(v.data(), v.data() + v.size(),
                nvector::p, nvector::f);
    std::vector<int> expected{2, 2, 6, 4};
    ASSERT_EQ(v, expected);
}

namespace nstring {
bool p(const char&) {
    return true;
}

void f(char& c) {
    c = std::toupper(c);
}
}

TEST(TransformIfWorks, string) {
    std::string s("abracadabra");
    TransformIf(s.data(), s.data() + s.size(),
                nstring::p, nstring::f);
    std::string expected("ABRACADABRA");
    ASSERT_EQ(s, expected);
}

namespace nvector1 {
bool p(const int& x) {
    return x != 0;
}

void f(int& x) {
    x = 42 / x;
}
}

TEST(TransformIfWorks, vector1) {
    std::vector<int> v{-42, 0, 42};
    TransformIf(v.data(), v.data() + v.size(),
                nvector1::p, nvector1::f);
    std::vector<int> expected{-1, 0, 1};
    ASSERT_EQ(v, expected);
}

namespace pthrows {
bool p(const std::string& s) {
    if (s.size() == 4) {
        throw std::string("don't do this");
    }
    return true;
}

void f(std::string& s) {
    s.erase(0, 1);
}
}

TEST(TransformIfPThrows, pthrows) {
    std::vector<std::string> data{"aba", "caba"};
    auto expected = data;
    
    ASSERT_THROW(TransformIf(data.data(), data.data() + data.size(),
                             pthrows::p, pthrows::f), std::string);
    ASSERT_EQ(data, expected);
}

namespace fthrows {
bool p(const std::string&) {
    return true;
}

void f(std::string& s) {
    s.erase(0, 1);
    if (s.size() == 3) {
        throw std::string("don't do this");
    }
}
}

TEST(TransformIfFThrows, fthrows) {
    std::vector<std::string> data{"aba", "caba"};
    auto expected = data;
    
    ASSERT_THROW(TransformIf(data.data(), data.data() + data.size(),
                             fthrows::p, fthrows::f), std::string);
    ASSERT_EQ(data, expected);
}

class Int {
public:
    Int() = default;
    Int(int value) : value(value) { }
    Int(Int&&) = delete;
    Int(const Int& rhs) {
        UpdateCounter();
        value = rhs.value;
    }

    Int& operator=(const Int& rhs) {
        UpdateCounter();
        value = rhs.value;
        return *this;
    }

    Int& operator=(Int&&) = delete;

    static void Reset(int max_count) {
        counter_ = 0;
        max_count_ = max_count;
    }

    static void UpdateCounter() {
        if (++counter_ == max_count_) {
            throw std::logic_error("Fail");
        }
    }

    bool operator==(const Int& rhs) const {
        return value == rhs.value;
    }

    int value;
private:
    static int counter_;
    static int max_count_;
};

int Int::counter_ = 0;
int Int::max_count_ = 0;

std::ostream& operator<<(std::ostream& os, const Int& i) {
    os << i.value;
    return os;
}

namespace copy_fails {
bool p(const Int&) {
    return true;
}

void f(Int& i) {
    i.value = i.value * i.value; 
}

}

TEST(CopyFails, copy_fails) {
    Int::Reset(100);
    std::vector<Int> data{ Int(1), Int(2), Int(3), Int(4), Int(5) };
    Int::Reset(3);
    TransformIf(data.data(), data.data() + data.size(),
                copy_fails::p, copy_fails::f);
    for (int i = 0; i < 5; ++i) {
        ASSERT_EQ(data[i], Int((i + 1) * (i + 1)));
    }
}

namespace all_fails {
bool p(const Int&) {
    return true;
}

void f(Int& i) {
    i.value = i.value * i.value;
    if (i.value == 9) {
        throw std::logic_error("Illegal square");
    }
}
}

TEST(AllFails, all_fails) {
    Int::Reset(100);
    std::vector<Int> data{ Int(1), Int(2), Int(3), Int(4), Int(5) };
    Int::Reset(3);

    ASSERT_THROW(TransformIf(data.data(), data.data() + data.size(),
                             all_fails::p, all_fails::f), std::logic_error);
}

namespace ponly {
bool p(const Int& val) {
    if (val.value == 3) {
        throw 42;
    }
    return val.value == 2;
}

void f(Int& val) {
    val.value += 2;
}
}

TEST(CopyPOnly, ponly) {
    Int::Reset(100);
    std::vector<Int> data{Int(1), Int(2), Int(3)};
    auto expected = data;
    Int::Reset(3);

    ASSERT_THROW(TransformIf(data.data(), data.data() + data.size(),
                             ponly::p, ponly::f), int);
    ASSERT_EQ(data, expected);
}

namespace stress {
bool p(const Int& val) {
    return val.value % 2 == 0;
}

constexpr int max_count = 1000;

void f(Int& val) {
    if (val.value++ == max_count) {
        throw std::runtime_error("Stop");
    }
}
}

TEST(Stress, stress) {
    std::vector<Int> data;
    constexpr int len = 1e6;
    constexpr int iterations = 1e4;

    for (int i = 0; i < len; ++i) {
        data.emplace_back(i);
    }

    for (int i = 0; i < iterations; ++i) {
        Int::Reset(100500);
        ASSERT_THROW(TransformIf(data.data(), data.data() + data.size(),
                                 stress::p, stress::f), std::runtime_error);
        for (int j = 0; j <= stress::max_count; ++j) {
            ASSERT_EQ(data[j], j);
        }
    }
}

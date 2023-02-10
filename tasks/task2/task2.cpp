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

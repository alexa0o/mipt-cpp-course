#include <gtest/gtest.h>

#include <algorithm>
#include <forward_list>
#include <iterator>
#include <numeric>
#include <map>
#include <vector>

#include "indexed_iterator.hpp"

TEST(IterateOver, vector) {
    std::vector<int> v { 0, 1, 2, 3, 4, 5 };
    for (auto ii = CreateIndexedIterator(v.begin()); ii != v.end(); ++ii) {
        ASSERT_EQ(ii.index(), *ii);
    }

    auto ii = CreateIndexedIterator(v.begin());
    auto ii_3 = ii + 3;
    ASSERT_EQ(ii_3.index(), 3);
    ASSERT_EQ(ii_3 - ii, 3);
}

TEST(IteratorTraits, find) {
    std::vector<int> v { 0, 1, 2, 3, 4, 5 };
    auto ii = CreateIndexedIterator(v.begin());
    auto ii_end = CreateIndexedIterator(v.end());
    auto ii_3 = std::find(ii, ii_end, 3);
    ASSERT_EQ(ii_3.index(), 3);
}

TEST(IteratorTraits, distance) {
    std::string s{"Indexed iterator is very cool"};
    auto ii = CreateIndexedIterator(s.begin());
    auto ii_3 = CreateIndexedIterator(s.begin() + 3);
    const auto dist = std::distance(ii, ii_3);
    ASSERT_EQ(dist, 3);
}

TEST(IteratorTraits, bidirectional) {
    std::map<std::string, int> m{ {"zero", 0}, {"one", 1}, {"two", 2}, {"three", 3} };
    auto ii = CreateIndexedIterator(m.begin());
    auto ii_2 = ii;
    std::advance(ii_2, 2);
    const auto dist = std::distance(ii, ii_2);
    ASSERT_EQ(dist, ii_2.index() - ii.index());
}

TEST(IteratorTraits, constpost) {
    std::vector<int> v { 0, 1, 2 };
    auto ii = CreateIndexedIterator(v.cbegin());
    ASSERT_EQ(0, *(ii++));
    ASSERT_EQ(1, (ii++).index());
    ASSERT_EQ(2, ii.index());
}

TEST(IteratorTraits, const) {
    std::vector<int> v { 0, 1, 2 };
    const auto ii = CreateIndexedIterator(v.begin());
    ASSERT_EQ(0, *ii);
}

TEST(IteratorTraits, forward) {
    std::forward_list<std::string> list {"cpp", "is", "very", "cool", "language"};
    auto ii = CreateIndexedIterator(list.begin());
    auto ii_4 = ii;
    std::advance(ii_4, 4);
    const auto dist = std::distance(ii, ii_4);
    ASSERT_EQ(ii_4.index(), dist);
}

TEST(IterateOver, stress) {
    const size_t iterations = 1e7;
    std::vector<int> v(iterations);
    std::iota(v.begin(), v.end(), 0);
    auto iter = CreateIndexedIterator(v.begin());
    for (size_t i = 0; i < iterations; ++i, ++iter) {
        ASSERT_EQ(i, iter.index());
    }
}

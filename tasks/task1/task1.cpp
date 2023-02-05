#include <gtest/gtest.h>

#include "adapter.hpp"

TEST(SomeLibrary, simpleTest) {
    SomeLibrary::ValueHolder valueHolder("new_holder", 42);

    ASSERT_EQ(valueHolder.GetValue(), 42);

    for (auto value : { 1, 72, -3, 137, 0 }) {
        SetValue(valueHolder, value);
        ASSERT_EQ(valueHolder.GetValue(), value);
    }

    ASSERT_EQ(valueHolder.GetName(), "new_holder");
}

#include <gtest/gtest.h>

#include <vector>

#include "allocator.hpp"

struct Object {
    uint64_t d[5];

    static PoolAllocator allocator;

    static void *operator new(size_t size) {
        return allocator.allocate(size);
    }
 
    static void operator delete(void *ptr, size_t size) {
        return allocator.deallocate(ptr, size);
    }
};

constexpr size_t chunksPerBlock = 8;
PoolAllocator Object::allocator{chunksPerBlock};

TEST(Default, array) {
    constexpr size_t size = 1e4;
    Object* array[size];

    for (size_t i = 0; i < size; ++i) {
        array[i] = new Object();
    }

    for (size_t i = 1; i < size; ++i) {
        if (i % chunksPerBlock != 0) {
            ASSERT_EQ(sizeof(Object), reinterpret_cast<uintptr_t>(array[i]) - reinterpret_cast<uintptr_t>(array[i - 1]));
        }
    }
}

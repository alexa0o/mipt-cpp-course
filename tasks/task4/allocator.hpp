#pragma once

// When chunk is free 'next' contains pointer to a next free chunk
// When it's allocated, this space is used by user
struct Chunk {
    Chunk* next;
};

class PoolAllocator {
public:
    PoolAllocator(size_t chunksPerBlock) : chunksPerBlock_(chunksPerBlock), alloc_(nullptr) { }

    void* allocate(size_t size) {
        if (alloc_ == nullptr) {
            alloc_ = allocateBlock(size);
        }
        // your code goes here
    }

    void deallocate(void* chunk, size_t size) {
        // your code goes here
    }

private:
    size_t chunksPerBlock_;
    Chunk* alloc_;

private:
    Chunk* allocateBlock(size_t chunkSize) {
        // your code goes here
    }
};

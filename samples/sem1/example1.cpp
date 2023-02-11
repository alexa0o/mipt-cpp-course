// g++ example1.cpp -std=c++2a -fsanitize=leak
#include <iostream>

struct Throw { };

class MemoryAllocator {
public:
    MemoryAllocator() {
        std::cout << "MemoryAllocator()\n";
        ptr_ = new int[42];
        throw Throw();
    }

    ~MemoryAllocator() {
        delete ptr_;
    }
private:
    int* ptr_;
};

int main() {
    try {
        MemoryAllocator memA;
    } catch (Throw& exception) {
        std::cout << "Catched!\n";
    }
}

// g++ example1.cpp -std=c++2a -fsanitize=leak
#include <iostream>
#include <memory>

struct Throw { };

class MemoryAllocator {
public:
    MemoryAllocator() {
        std::cout << "MemoryAllocator()\n";
        ptr_ = std::make_unique<int>(42);
        throw Throw();
    }

private:
    std::unique_ptr<int> ptr_;
};

int main() {
    try {
        MemoryAllocator memA;
    } catch (Throw& exception) {
        std::cout << "Catched!\n";
    }
}

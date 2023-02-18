#include <iostream>
#include <cstring>
#include <stdexcept>

class FiveValuesHolder {
public:
    FiveValuesHolder(int values[], size_t size) {
        if (size < five_) {
            throw std::invalid_argument("Too small values array");
        }
        std::memcpy(values_, values, five_ * sizeof(int));
    }

    int* begin() {
        return values_;
    }

    int* end() {
        return values_ + five_;
    }
private:
    constexpr static int five_ = 5;

    int values_[five_];
};

int main() {
    int values[5] = { 1, 2, 3, 4, 5 };
    FiveValuesHolder fvh(values, 5);

    for (auto value : fvh) {
        std::cout << value << std::endl;
    }
}

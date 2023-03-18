#include <iostream>

class Iota {
    class IotaIterator {
    public:
        IotaIterator(int value, int step) : Value_(value), Step_(step) { }

        IotaIterator& operator++() {
            Value_ += Step_;
            return *this;
        }

        int operator*() const {
            return Value_;
        }

        bool operator!=(const IotaIterator& ii) const {
            return Step_ > 0 ? Value_ < ii.Value_ : Value_ > ii.Value_;
        }
    private:
        int Value_;
        int Step_;
    };

public:

    Iota(int a, int b, int step = 1) : L_(a), R_(b), Step_(step) { }

    IotaIterator begin() {
        return IotaIterator(L_, Step_);
    }

    IotaIterator end() {
        return IotaIterator(R_, Step_);
    }

private:
    int L_;
    int R_;
    int Step_;
};

int main() {
    for (int x : Iota(20, 10, -2)) {
        std::cout << x << std::endl;
    }
}

#include <iostream>

class TwoPtrHolder {
public:
    TwoPtrHolder(int* ptrA, int* ptrB)
        : ptrA_(ptrA), ptrB_(ptrB)
    { }

    ~TwoPtrHolder() {
        delete ptrA_;
        delete ptrB_;
    }
private:
    int* ptrA_;
    int* ptrB_;
};

int main() {
    TwoPtrHolder tph(new int(42), new int(3));
}

#include <iostream>

struct Throw { };

class ThrowInDestructor {
public:
    ~ThrowInDestructor() {
        throw Throw();
    }
};

void f() {
    ThrowInDestructor tid;
    throw Throw();
}

int main() {
    f();
}

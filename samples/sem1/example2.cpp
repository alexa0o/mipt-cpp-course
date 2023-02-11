#include <iostream>

struct Throw { };

class ThrowInDestructor {
public:
    ~ThrowInDestructor() {
        throw Throw();
    }
};

int main() {
    try {
        ThrowInDestructor tid;
    } catch (Throw& thr) {

    }
}

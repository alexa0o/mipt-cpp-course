#include <iostream>
#include <vector>

struct Throw { };

class ThrowInDestructor {
public:
    ThrowInDestructor(std::vector<int>* v) : v_(v) { }

    ~ThrowInDestructor() {
        try {
            v_->push_back(1);
        } catch (std::exception& e) {

        }
    }
private:
    std::vector<int>* v_;
};

int main() {
    try {
        std::vector<int> v;
        ThrowInDestructor tid(&v);
    } catch (Throw& thr) {

    }
}

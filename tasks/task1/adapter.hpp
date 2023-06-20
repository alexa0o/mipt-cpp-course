#include "some_library.hpp"


struct Twin {
    std::string name_;
    int value_;
};

inline void SetValue(SomeLibrary::ValueHolder& valueHolder, int value) {

    reinterpret_cast<Twin*>(&valueHolder)->value_ = value;

    return;
}

#include <iostream>
#include <string>
#include <type_traits>

template <class T>
T&& fwd(T&& t) {
    static_assert(std::is_same_v<T, std::string>);
    return static_cast<T&&>(t);
}

int main() {
    std::string a;
    fwd<std::string>(std::string());
    //fwd<std::string&>(a);
}

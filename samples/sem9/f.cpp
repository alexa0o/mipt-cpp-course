#include <iostream>
#include <string>
#include <type_traits>

template <typename T>
T&& my_forward(std::remove_reference_t<T>&& x) {
    return static_cast<T&&> (x);
}

template <typename T>
T&& my_forward(std::remove_reference_t<T>& x) {
    return static_cast<T&&> (x);
}

void doSmthImpl(std::string s) {

}

template <class Ts...>
void doSmth(Ts&& s, ...) {
    doSmthImpl(my_forward<Ts>(s), ...); // ????
}

int get_42() {
    return 42;
}

std::string getName() {
    std::string name("Petr I");
    // smth with name
    return name;
}

class A {
public:
    template <class T>
    A(T&& t) { }

    A(const A& a) { }
};

int main() {
    std::string a("Hello world");
    std::cout << a << std::endl;
    doSmth(a);
    std::cout << a << std::endl;

    int x = get_42();
    std::string name = getName();

    A a1(1);
    A a2(a1);
}

struct StringHolder {
    StringHolder(std::string& s) : s(std::move(s)) { }

    std::string s;
};


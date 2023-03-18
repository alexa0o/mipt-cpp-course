1. range - std::vector
2. numbers_in | ... | view::filter(even) | view::transform(half) | view::filter(xxx) | view::take(3):
int value;
Iterator:: operator++() {
    while (!filter(++value));
}

Iterator:: oprator*() {
    return func(value);
}

---------

template <class F, class Ts...>
void Do() {
    F();
    Do<Ts...>();
}

template <class filter>
Iterator:: operator++() {
    while (!filter(++value));
}

void F() {
    iterator.operator<filter>++();
}

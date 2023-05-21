
template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

template <class Ts...> overload(Ts...) -> overload<Ts...>;

int main() {
    std::visit
    (
      overload
      (
        [](const int& i) { PRINT("int: " + i); },
        [](const std::string& s) { PRINT("it's a string: " + s); },
        [](const float& f) { PRINT("float" + f); }
      ),
      yourVariant;
    );
}

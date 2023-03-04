#include <sstream>
#include <string>
#include <map>
#include <iomanip>
#include <iostream>

std::string FormatMap(const std::map<int, float>& m) {
    std::stringstream ss;
    ss << std::setprecision(6);
    for (const auto& it : m) {
        ss << it.first << ':' << it.second << ';';
    }
    return ss.str();
}

int main() {
    std::map<int, float> m { {0, 1.12345}, {1, 2.123456}, {2, 3.1234567} };
    std::cout << FormatMap(m);
}

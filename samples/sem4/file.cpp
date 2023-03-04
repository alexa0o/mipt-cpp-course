#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("file.cpp");
    std::string word;
    while (in >> word) {
        std::cout << word << std::endl;
    }
}

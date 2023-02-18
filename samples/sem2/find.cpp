#include <algorithm>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v{ 1, 2, 3, 4, 5 };
    if (std::find(v.begin(), v.end(), 3) != v.end()) {
        std::cout << "3 is here!" << std::endl;
    }
}

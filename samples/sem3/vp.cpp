#include <vector>

int main() {
    std::vector<std::pair<int, int>> vp;
    vp.push_back(std::make_pair(42, 1));
    vp.begin()->first;
}

#include <map>
#include <iostream>

int main() {
    std::map<int, float> num_map;
    num_map[3] = 3.14f;
    num_map[4] = 4.5f;
    num_map[5] = 5.55f;
    num_map[1] = 1.1f;

    for (auto it = num_map.begin(); it != num_map.end(); ++it) {
        std::cout << it->first << ", " << it->second << std::endl;
    }
}

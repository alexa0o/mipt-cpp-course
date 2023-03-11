#include <list>
#include <unordered_map>

using namespace std;

class LRUCache {
public:
    LRUCache(int capacity) : cap_(capacity) {
        
    }
    
    int get(int key) {
        if (map_.find(key) == map_.end()) {
            return -1;
        }
        list_.splice(list_.begin(), list_, map_[key]);
        return map_[key]->second;
    }
    
    void put(int key, int value) {
        if (map_.find(key) != map_.end()) {
            list_.splice(list_.begin(), list_, map_[key]);
            map_[key]->second = value;
            return;
        }
        if (list_.size() == cap_) {
            const auto d_key = list_.back().first;
            list_.pop_back();
            map_.erase(d_key);
        }
        list_.push_front({key, value});
        map_[key] = list_.begin();
    }

private:
    const int cap_;
    list<pair<int, int>> list_;
    unordered_map<int, list<pair<int, int>>::iterator> map_;
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */

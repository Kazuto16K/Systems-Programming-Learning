#include<iostream>
#include<vector>
#include<list>
#include<cstring>
#include<utility>
#include<functional>
#include<string>
#include<sstream>

template<typename K, typename V>
class HashTable{
private:
    std::vector<std::list<std::pair<K,V>>> buckets;
    size_t count;
    static constexpr double MAX_LOAD_FACTOR = 0.75;

    size_t hashKey(const K& key) const{
        // std::hash gives us a raw hash; we still write our own
        // table logic (index mapping, collisions, resize) ourselves
        return std::hash<K>{}(key) % buckets.size();
    }

    void rehash(){
        std::vector<std::list<std::pair<K,V>>> oldBuckets = std::move(buckets);
        buckets.assign(oldBuckets.size()*2, {});
        count =0;
        for(auto& bucket: oldBuckets){
            for(auto& [k,v]:bucket){
                insert(k,v); // reinsert into new larger table
            }
        }
    }

public:
    explicit HashTable(size_t initialBuckets = 16)
        : buckets(initialBuckets), count(0){}
    
    void insert(const K& key, const V& value){
        double loadFactor = static_cast<double>(count+1)/buckets.size();
        if(loadFactor > MAX_LOAD_FACTOR) rehash();

        size_t idx = hashKey(key);
        for(auto& [k,v]: buckets[idx]){
            if(k==key){
                v = value;
                return;
            }
        }

        buckets[idx].emplace_back(key,value);
        count+=1;
    }

    bool find(const K& key,V& outValue) const {
        size_t idx = hashKey(key);
        for (const auto& [k, v] : buckets[idx]) {
            if (k == key) { outValue = v; return true; }
        }
        return false;
    }

    bool remove(const K& key) {
        size_t idx = hashKey(key);
        auto& bucket = buckets[idx];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) { bucket.erase(it); --count; return true; }
        }
        return false;
    }

    bool contains(const K& key) const{
        V dummy;
        return find(key, dummy);
    }

    V& operator[](const K& key){
        double loadFactor = static_cast<double>(count+1)/buckets.size();
        if(loadFactor > MAX_LOAD_FACTOR) rehash();

        size_t idx = hashKey(key);
        for(auto& [k,v]: buckets[idx]){
            if(k==key) return v;
        }
        buckets[idx].emplace_back(key, V{});
        count+=1;
        return buckets[idx].back().second;
    }
    
    size_t size() const { return count; }

    bool empty() const { return count == 0; }

    void printTable() const {   // for debugging/visualization
        for (size_t i = 0; i < buckets.size(); ++i) {
            std::cout << i << ": ";
            for (const auto& [k, v] : buckets[i]) std::cout << "(" << k << "," << v << ") ";
            std::cout << "\n";
        }
    }


};

int main() {
    HashTable<std::string, int> wordCount;

    std::string text = "the quick brown fox jumps over the lazy dog the fox runs";
    std::istringstream iss(text);
    std::string word;

    // Insert / update counts
    while (iss >> word) {
        wordCount[word]++;   // operator[] creates entry if missing, then increments
    }

    std::cout << "Word frequencies:\n";
    wordCount.printTable();

    // Lookup
    std::string query = "fox";
    int freq;
    if (wordCount.find(query, freq))
        std::cout << "\n'" << query << "' appears " << freq << " times\n";
    else
        std::cout << "\n'" << query << "' not found\n";

    // Remove
    wordCount.remove("dog");
    std::cout << "\nAfter removing 'dog':\n";
    wordCount.printTable();

    // Size check
    std::cout << "\nTotal unique words: " << wordCount.size() << "\n";

    return 0;
}
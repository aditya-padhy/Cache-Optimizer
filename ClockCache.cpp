#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <random>
#include <chrono>

using namespace std;

struct CacheEntry {
    string filename;
    string data;
    bool used;
};

class ClockCache {
public:
    ClockCache(int capacity) : capacity(capacity), pointer(0), hits(0), misses(0) {}

    string accessFile(const string& filename) {
        totalAccesses++;
        if (cacheMap.find(filename) != cacheMap.end()) {
            cacheEntries[cacheMap[filename]].used = true;
            hits++;
            return cacheEntries[cacheMap[filename]].data;
        }
        misses++;
        string data = generateData(filename);
        if (cacheEntries.size() < capacity) {
            cacheEntries.push_back({filename, data, true});
            cacheMap[filename] = cacheEntries.size() - 1;
        } else {
            evictAndInsert(filename, data);
        }
        return data;
    }

    void displayCache() const {
        cout << "Cache Contents:\n";
        for(const auto &entry : cacheEntries){
            cout << entry.filename << " ";
        }
        cout << "\n";
    }

    void displayMetrics() const {
        cout << "Performance Metrics:\n";
        cout << "Total Accesses: " << totalAccesses << "\n";
        cout << "Cache Hits: " << hits << "\n";
        cout << "Cache Misses: " << misses << "\n";
        double hitRate = (totalAccesses > 0) ? (static_cast<double>(hits) / totalAccesses) * 100.0 : 0.0;
        cout << "Hit Rate: " << hitRate << "%\n";
    }

private:
    int capacity;
    int pointer;
    vector<CacheEntry> cacheEntries;
    unordered_map<string, int> cacheMap;
    int hits;
    int misses;
    int totalAccesses = 0;

    void evictAndInsert(const string& filename, const string& data){
        while(true){
            if(!cacheEntries[pointer].used){
                cacheMap.erase(cacheEntries[pointer].filename);
                cacheEntries[pointer] = {filename, data, true};
                cacheMap[filename] = pointer;
                pointer = (pointer + 1) % capacity;
                break;
            }
            cacheEntries[pointer].used = false;
            pointer = (pointer + 1) % capacity;
        }
    }

    string generateData(const string& filename){
        return "Simulated data for " + filename;
    }
};

int main(){
    // Setup
    ClockCache cache(5);
    vector<string> filenames = {"file1.txt", "file2.txt", "file3.txt", "file4.txt", "file5.txt", "file6.txt", "file7.txt","file8.txt","file8.txt","file9.txt","file10.txt"};
    
    // Random access simulation
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, static_cast<int>(filenames.size()) -1);
    int simulationRuns = 100;

    auto start = chrono::high_resolution_clock::now();

    for(int i = 0; i < simulationRuns; ++i){
        int idx = dis(gen);
        cache.accessFile(filenames[idx]);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // Display results
    cache.displayCache();
    cache.displayMetrics();
    cout << "Simulation Time: " << elapsed.count() << " seconds\n";

    return 0;
}
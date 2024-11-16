#ifndef CACHE_OPTIMIZER_H
#define CACHE_OPTIMIZER_H

#include <unordered_map>
#include <list>
#include <string>
#include <iostream>

class CacheOptimizer {
private:
    struct CacheItem {
        std::string fileData;
        int frequency;
        bool dirty;
        std::list<std::string>::iterator lruPos;
    };

    int capacity;
    int hits, misses;
    std::unordered_map<std::string, CacheItem> cache;
    std::list<std::string> lruOrder;  // For maintaining LRU order
    std::unordered_map<std::string, std::string> mainMemory; // Simulating main memory

    void evict();  // Hybrid LRU-LFU eviction method

public:
    CacheOptimizer(int cap);
    void accessFile(const std::string &filePath, const std::string &fileData = "", bool write = false);
    void printMetrics() const;
    void displayMainMemory() const;
};

#endif // CACHE_OPTIMIZER_H

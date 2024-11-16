#ifndef CACHE_OPTIMIZER_H
#define CACHE_OPTIMIZER_H

#include <unordered_map>
#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

class CacheOptimizer {
public:
    struct CacheItem {
        std::string fileData;
        int frequency;
        bool dirty;
        std::list<std::string>::iterator lruPos;
    };

    int capacity;
    int hits, misses, evictions, writebacks;
    std::unordered_map<std::string, CacheItem> cache;
    std::list<std::string> lruOrder;  // For maintaining LRU order
    std::unordered_map<std::string, std::string> mainMemory; // Simulating main memory
    std::unordered_map<std::string, std::vector<std::string>> accessPatterns; // Tracks access patterns for proactive caching
    std::unordered_map<std::string, int> accessCounts; // Tracks how often a file was accessed
    std::chrono::steady_clock::time_point lastPatternAnalysisTime;
    int patternAnalysisInterval;  // Time in seconds to re-analyze access patterns
    int adaptiveCacheThreshold;  // Threshold to resize the cache dynamically

    void evict();  // Hybrid LRU-LFU eviction method
    void analyzeAccessPatterns();  // Analyzes file access patterns periodically
    void adjustCacheSize();  // Adjusts the cache size dynamically based on access patterns

public:
    CacheOptimizer(int cap, int patternInterval = 30, int adaptiveThreshold = 100);
    void accessFile(const std::string &filePath, const std::string &fileData = "", bool write = false);
    void printMetrics() const;
    void displayMainMemory() const;
};

#endif // CACHE_OPTIMIZER_H

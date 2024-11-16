#include "CacheOptimizer.h"

CacheOptimizer::CacheOptimizer(int cap, int patternInterval, int adaptiveThreshold) 
    : capacity(cap), hits(0), misses(0), evictions(0), writebacks(0),
      patternAnalysisInterval(patternInterval), adaptiveCacheThreshold(adaptiveThreshold) {
    // Initialize main memory with some dummy files (for demonstration)
    mainMemory["file1"] = "Content of file1";
    mainMemory["file2"] = "Content of file2";
    mainMemory["file3"] = "Content of file3";
    mainMemory["file4"] = "Content of file4";
    mainMemory["file5"] = "Content of file5";
    
    lastPatternAnalysisTime = std::chrono::steady_clock::now();
}

void CacheOptimizer::accessFile(const std::string &filePath, const std::string &fileData, bool write) {
    auto it = cache.find(filePath);

    // Track access history
    accessCounts[filePath]++;

    // Proactive caching: if file has a known next likely access, pre-cache it
    if (accessPatterns.find(filePath) != accessPatterns.end()) {
        for (const auto& nextFile : accessPatterns[filePath]) {
            auto itNext = cache.find(nextFile);
            if (itNext == cache.end()) {  // File not in cache, pre-cache it
                std::string data = mainMemory[nextFile];
                lruOrder.push_front(nextFile);
                cache[nextFile] = {data, 1, false, lruOrder.begin()};
                std::cout << "Pre-cached: " << nextFile << std::endl;
            }
        }
    }

    if (it != cache.end()) {  // Cache hit
        hits++;
        it->second.frequency++;  // Increase frequency count
        lruOrder.erase(it->second.lruPos);  // Update LRU position
        lruOrder.push_front(filePath);  // Move accessed file to the front
        it->second.lruPos = lruOrder.begin();
        
        if (write) {  // If write access, update fileData and set dirty bit
            it->second.fileData = fileData;
            it->second.dirty = true;
        }
    } else {  // Cache miss
        misses++;

        // Fetch the file from main memory
        std::string data = mainMemory[filePath];
        
        if (cache.size() >= capacity) {
            evict();
        }

        // Add new file to cache
        lruOrder.push_front(filePath);
        cache[filePath] = {data, 1, false, lruOrder.begin()};  // Set dirty to false by default
        if (write) {
            cache[filePath].fileData = fileData;
            cache[filePath].dirty = true;  // Set dirty if it's a write access
        }

        // Update access patterns for proactive caching
        for (auto& entry : cache) {
            accessPatterns[entry.first].push_back(filePath);
        }
    }

    // Periodically analyze access patterns
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastPatternAnalysisTime).count() >= patternAnalysisInterval) {
        analyzeAccessPatterns();
        lastPatternAnalysisTime = now;
    }

    // Adjust cache size based on access patterns
    adjustCacheSize();
}

void CacheOptimizer::evict() {
    // Find the least frequently used file with the least recency (i.e., at the end of lruOrder)
    auto lru_it = lruOrder.rbegin();
    std::string toEvict = *lru_it;

    // Identify the file with the lowest frequency among the least recent files
    for (const auto &entry : cache) {
        if (entry.second.frequency < cache[toEvict].frequency ||
            (entry.second.frequency == cache[toEvict].frequency && entry.second.lruPos == lruOrder.end())) {
            toEvict = entry.first;
        }
    }

    // Check if the file is dirty, write back to main memory if needed
    if (cache[toEvict].dirty) {
        mainMemory[toEvict] = cache[toEvict].fileData;  // Write back to main memory
        writebacks++;
        std::cout << "Evicted and wrote back: " << toEvict << " (Hybrid LRU-LFU, Dirty)" << std::endl;
    } else {
        std::cout << "Evicted: " << toEvict << " (Hybrid LRU-LFU)" << std::endl;
    }

    // Remove the file from cache and LRU list
    lruOrder.erase(cache[toEvict].lruPos);
    cache.erase(toEvict);
    evictions++;
}

void CacheOptimizer::analyzeAccessPatterns() {
    std::cout << "Analyzing access patterns..." << std::endl;
    // Analyze access patterns and predict the next likely file to be accessed
    for (const auto& entry : accessPatterns) {
        const std::string& currentFile = entry.first;
        const std::vector<std::string>& nextFiles = entry.second;

        // Update access patterns: if file A is followed by file B multiple times, add B as the next likely access for A
        if (nextFiles.size() > 1) {
            for (size_t i = 0; i < nextFiles.size() - 1; ++i) {
                accessPatterns[currentFile].push_back(nextFiles[i + 1]);
            }
        }
    }
}

void CacheOptimizer::adjustCacheSize() {
    if (misses > adaptiveCacheThreshold) {
        capacity++;  // Increase cache size dynamically if access frequency increases
        std::cout << "Increasing cache size to: " << capacity << std::endl;
    } else if (hits > adaptiveCacheThreshold && capacity > 2) {
        capacity--;  // Decrease cache size if access rate drops and cache size is large
        std::cout << "Decreasing cache size to: " << capacity << std::endl;
    }
}

void CacheOptimizer::printMetrics() const {
    double hitRate = (double)hits / (hits + misses) * 100;
    double missRate = (double)misses / (hits + misses) * 100;

    std::cout << "Cache Performance Metrics:" << std::endl;
    std::cout << "Hits: " << hits << " | Misses: " << misses << std::endl;
    std::cout << "Evictions: " << evictions << " | Writebacks: " << writebacks << std::endl;
    std::cout << "Hit Rate: " << hitRate << "% | Miss Rate: " << missRate << "%" << std::endl;
}

// New: Display the contents of main memory
void CacheOptimizer::displayMainMemory() const {
    std::cout << "Main Memory Contents:" << std::endl;
    for (const auto &entry : mainMemory) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}
// #include <iostream>
// #include <vector>
// #include <ctime>
// #include <cstdlib>
// #include <algorithm>
// #include <random>
// #include "CacheOptimizer.h"

// // Function to simulate a delay using a busy-wait loop
// void simulateDelay(int milliseconds) {
//     std::clock_t start_time = std::clock();
//     while ((std::clock() - start_time) * 1000 / CLOCKS_PER_SEC < milliseconds) {
//         // Busy wait (simulating delay)
//     }
// }

// int main() {
//      // Set up a CacheOptimizer with:
//     // capacity = 2, pattern analysis every 5 seconds, adaptive cache threshold of 5
//     CacheOptimizer cache(2, 5, 5);

//     // Simulate a random sequence of file accesses
//     std::vector<std::string> fileSequence = {"file1", "file2", "file3", "file1", "file4", "file2", "file3", "file4", "file5"};

//     // Randomize the access order using std::shuffle (requires a random engine)
//     std::random_device rd;
//     std::mt19937 g(rd());  // Mersenne Twister random number generator
//     std::shuffle(fileSequence.begin(), fileSequence.end(), g);

//     // Access each file in a randomized sequence
//     for (const std::string& file : fileSequence) {
//         // Simulate file write for some files (file1 and file3 in this case)
//         bool write = (file == "file1" || file == "file3");  // Files to be written to
//         std::string newContent = (write ? "Updated " + file + " content" : "");

//         std::cout << "\nAccessing: " << file << std::endl;
//         // Access the file (write if needed)
//         cache.accessFile(file, newContent, write);

//         // Simulate a delay of 500ms between file accesses
//         simulateDelay(500);
//     }

//     // Display the cache performance metrics
//     cache.printMetrics();

//     // Display the contents of main memory to verify write-backs
//     cache.displayMainMemory();

//     return 0;
// }

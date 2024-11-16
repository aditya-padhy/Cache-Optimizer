#include "CacheOptimizer.h"

CacheOptimizer::CacheOptimizer(int cap) 
    : capacity(cap), hits(0), misses(0) {
    // Initialize main memory with some dummy files (for demonstration)
    mainMemory["file1"] = "Content of file1";
    mainMemory["file2"] = "Content of file2";
    mainMemory["file3"] = "Content of file3";
    mainMemory["file4"] = "Content of file4";
    mainMemory["file5"] = "Content of file5";
}

void CacheOptimizer::accessFile(const std::string &filePath, const std::string &fileData, bool write) {
    auto it = cache.find(filePath);

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
    }
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
        std::cout << "Evicted and wrote back: " << toEvict << " (Hybrid LRU-LFU, Dirty)" << std::endl;
    } else {
        std::cout << "Evicted: " << toEvict << " (Hybrid LRU-LFU)" << std::endl;
    }

    // Remove the file from cache and LRU list
    lruOrder.erase(cache[toEvict].lruPos);
    cache.erase(toEvict);
}

void CacheOptimizer::printMetrics() const {
    double hitRate = (double)hits / (hits + misses) * 100;
    double missRate = (double)misses / (hits + misses) * 100;

    std::cout << "Cache Performance Metrics:" << std::endl;
    std::cout << "Hits: " << hits << " | Misses: " << misses << std::endl;
    std::cout << "Hit Rate: " << hitRate << "% | Miss Rate: " << missRate << "%" << std::endl;
}

// New: Display the contents of main memory
void CacheOptimizer::displayMainMemory() const {
    std::cout << "Main Memory Contents:" << std::endl;
    for (const auto &entry : mainMemory) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}

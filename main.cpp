#include "CacheOptimizer.h"
#include <iostream>

int main() {
    CacheOptimizer cache(3);  // Cache capacity of 2

    // Accessing files and modifying some of them
    cache.accessFile("file1", "Updated content of file1", true);  // Write access
    cache.accessFile("file2");
    cache.accessFile("file3");  
    cache.accessFile("file2"); //Hit
    cache.accessFile("file4"); //Will trigger eviction
    cache.accessFile("file3"); //Hit
    cache.accessFile("file4"); //Hit

    // Display the cache performance metrics
    cache.printMetrics();

    // Display the contents of main memory to verify write-backs
    cache.displayMainMemory();

    return 0;
}


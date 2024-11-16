// #include "CacheOptimizer.h"
// #include <iostream>

// int main() {
//     CacheOptimizer cache(3);  // Cache capacity of 3

//     // Test case 1: Accessing files with both read and write operations
//     cache.accessFile("file1", "Updated content of file1", true);  // Write access
//     cache.accessFile("file2");
//     cache.accessFile("file3");  
//     cache.accessFile("file2"); // Expected Hit
//     cache.accessFile("file4"); // Should trigger eviction
//     cache.accessFile("file3"); // Expected Hit
//     cache.accessFile("file4"); // Expected Hit

//     // Test case 2: Exceeding capacity and checking eviction with dirty write-back
//     cache.accessFile("file5", "Updated content of file5", true);  // Should trigger eviction
//     cache.accessFile("file2");  // Expected Hit
//     cache.accessFile("file6");  // Should trigger another eviction

//     // Display the cache performance metrics
//     cache.printMetrics();

//     // Display the contents of main memory to verify write-backs
//     cache.displayMainMemory();

//     return 0;
//}
#include "CacheOptimizer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

int main() {
    CacheOptimizer cache(3);  // Set cache capacity to 3

    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Predefined list of files in main memory
    std::vector<std::string> files = {"file1", "file2", "file3", "file4", "file5"};

    // Simulate 15 random file accesses
    for (int i = 0; i < 15; ++i) {
        // Randomly choose a file to access
        std::string fileToAccess = files[std::rand() % files.size()];

        // Randomly decide if the access is a read or write
        bool isWriteAccess = std::rand() % 2;
        std::string fileData = "Random content for " + fileToAccess;

        // Access the file, specifying read or write mode
        cache.accessFile(fileToAccess, fileData, isWriteAccess);

        // For demonstration purposes, print a message about the access type
        if (isWriteAccess) {
            std::cout << "Random Write Access to " << fileToAccess << std::endl;
        } else {
            std::cout << "Random Read Access to " << fileToAccess << std::endl;
        }
    }

    // Display the cache performance metrics after random access simulation
    cache.printMetrics();

    // Display the contents of main memory to verify write-backs
    cache.displayMainMemory();

    return 0;
}

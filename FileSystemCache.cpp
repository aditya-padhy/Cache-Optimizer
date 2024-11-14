#include <bits/stdc++.h>
using namespace std;

// Class representing a file in the filesystem
class File {
public:
    string name;
    string content;
    int size;

    File(string name = "", string content = "") : name(name), content(content), size(content.size()) {}
};

// Class representing the filesystem
class FileSystem {
private:
    unordered_map<string, File> files;

public:
    // Add a new file to the filesystem
    void addFile(const string& name, const string& content) {
        if (files.find(name) != files.end()) {
            cout << "File '" << name << "' already exists. Overwriting content.\n";
            files[name].content = content;
            files[name].size = content.size();
        } else {
            files[name] = File(name, content);
            cout << "File '" << name << "' added to filesystem.\n";
        }
    }

    // Read a file's content
    string readFile(const string& name) {
        if (files.find(name) != files.end()) {
            // Simulate disk I/O delay
            cout << "Reading file '" << name << "' from disk.\n";
            return files[name].content;
        } else {
            cout << "File '" << name << "' not found in filesystem.\n";
            return "";
        }
    }

    // Write content to a file
    void writeFile(const string& name, const string& content) {
        if (files.find(name) != files.end()) {
            files[name].content = content;
            files[name].size = content.size();
            cout << "File '" << name << "' updated in filesystem.\n";
        } else {
            cout << "File '" << name << "' does not exist. Creating new file.\n";
            addFile(name, content);
        }
    }

    // List all files in the filesystem
    void listFiles() {
        cout << "Files in filesystem:\n";
        for (const auto& pair : files) {
            cout << " - " << pair.first << " (Size: " << pair.second.size << " bytes)\n";
        }
    }

    // Get all file names
    vector<string> getAllFileNames() {
        vector<string> names;
        for (const auto& pair : files) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// Class representing the Cache with LFU eviction policy
class Cache {
private:
    int capacity;
    int currentSize;

    // Structure to hold cache entries
    struct CacheEntry {
        File file;
        int frequency;
        int timestamp; // To resolve ties in frequency
    };

    unordered_map<string, CacheEntry> cacheMap;
    // Min-heap to determine which file to evict
    // Pair of (frequency, timestamp) and file name
    priority_queue<pair<pair<int, int>, string>, vector<pair<pair<int, int>, string>>, std::greater<pair<pair<int, int>, string>>> minHeap;
    int globalTimestamp;

public:
    Cache(int capacity) : capacity(capacity), currentSize(0), globalTimestamp(0) {}

    bool isCached(const string& name) {
        return cacheMap.find(name) != cacheMap.end();
    }

    // Get a file from cache
    File get(const string& name) {
        if (isCached(name)) {
            cacheMap[name].frequency += 1;
            cacheMap[name].timestamp = globalTimestamp++;
            minHeap.push({{cacheMap[name].frequency, cacheMap[name].timestamp}, name});
            cout << "Cache hit for file '" << name << "'.\n";
            return cacheMap[name].file;
        } else {
            cout << "Cache miss for file '" << name << "'.\n";
            // Return an empty file if not in cache
            return File();
        }
    }

    // Add a file to the cache
    void put(const File& file) {
        if (capacity == 0) return;

        if (isCached(file.name)) {
            // Update the file content and frequency
            cacheMap[file.name].file = file;
            cacheMap[file.name].frequency += 1;
            cacheMap[file.name].timestamp = globalTimestamp++;
            minHeap.push({{cacheMap[file.name].frequency, cacheMap[file.name].timestamp}, file.name});
            cout << "File '" << file.name << "' updated in cache.\n";
            return;
        }

        if (currentSize >= capacity) {
            // Evict the least frequently used file
            while (!minHeap.empty()) {
                auto top = minHeap.top();
                minHeap.pop();
                string evictName = top.second;
                // Check if this entry is the current one
                if (cacheMap.find(evictName) != cacheMap.end() &&
                    cacheMap[evictName].frequency == top.first.first &&
                    cacheMap[evictName].timestamp == top.first.second) {
                    // Evict this file
                    cacheMap.erase(evictName);
                    currentSize--;
                    cout << "Evicted file '" << evictName << "' from cache.\n";
                    break;
                }
            }
        }

        // Add the new file to cache
        CacheEntry entry;
        entry.file = file;
        entry.frequency = 1;
        entry.timestamp = globalTimestamp++;
        cacheMap[file.name] = entry;
        minHeap.push({{entry.frequency, entry.timestamp}, file.name});
        currentSize++;
        cout << "File '" << file.name << "' added to cache.\n";
    }

    // Display cache contents
    void displayCache() {
        cout << "Current Cache Contents:\n";
        for (const auto& pair : cacheMap) {
            cout << " - " << pair.first << " (Freq: " << pair.second.frequency << ")\n";
        }
    }
};

// Class to optimize cache based on access patterns
class CacheOptimizer {
private:
    unordered_map<string, int> accessFrequency;

public:
    // Record an access to a file
    void recordAccess(const string& name) {
        accessFrequency[name]++;
    }

    // Get the top N frequently accessed files
    vector<string> getTopN(int N) {
        // Use a max heap to get top N
        priority_queue<pair<int, string>> maxHeap;
        for (const auto& pair : accessFrequency) {
            maxHeap.push({pair.second, pair.first});
        }

        vector<string> topFiles;
        while (N > 0 && !maxHeap.empty()) {
            topFiles.push_back(maxHeap.top().second);
            maxHeap.pop();
            N--;
        }
        return topFiles;
    }

    // Reset access frequencies
    void reset() {
        accessFrequency.clear();
    }
};

// Integrate FileSystem, Cache, and CacheOptimizer
class FileSystemCacheOptimizer {
private:
    FileSystem fs;
    Cache cache;
    CacheOptimizer optimizer;

public:
    FileSystemCacheOptimizer(int cacheCapacity) : cache(cacheCapacity) {}

    // Add a file to the filesystem
    void addFile(const string& name, const string& content) {
        fs.addFile(name, content);
    }

    // Read a file's content
    string readFile(const string& name) {
        optimizer.recordAccess(name);
        if (cache.isCached(name)) {
            File file = cache.get(name);
            return file.content;
        } else {
            string content = fs.readFile(name);
            if (!content.empty()) {
                File file(name, content);
                cache.put(file);
            }
            return content;
        }
    }

    // Write content to a file
    void writeFile(const string& name, const string& content) {
        fs.writeFile(name, content);
        optimizer.recordAccess(name);
        if (cache.isCached(name)) {
            File file(name, content);
            cache.put(file);
        }
    }

    // List all files
    void listFiles() {
        fs.listFiles();
    }

    // Optimize cache based on access patterns
    void optimizeCache(int topN) {
        vector<string> topFiles = optimizer.getTopN(topN);
        cout << "Optimizing cache with top " << topN << " frequently accessed files.\n";
        for (const string& name : topFiles) {
            if (!cache.isCached(name)) {
                string content = fs.readFile(name);
                if (!content.empty()) {
                    File file(name, content);
                    cache.put(file);
                }
            }
        }
        optimizer.reset();
    }

    // Display cache contents
    void displayCache() {
        cache.displayCache();
    }
};

// Main function to demonstrate the filesystem with cache optimizer
int main() {
    // Initialize FileSystemCacheOptimizer with cache capacity of 3 files
    FileSystemCacheOptimizer fsCacheOpt(3);

    // Add files to the filesystem
    fsCacheOpt.addFile("file1.txt", "This is the content of file1.");
    fsCacheOpt.addFile("file2.txt", "This is the content of file2.");
    fsCacheOpt.addFile("file3.txt", "This is the content of file3.");
    fsCacheOpt.addFile("file4.txt", "This is the content of file4.");
    fsCacheOpt.addFile("file5.txt", "This is the content of file5.");

    cout << "\nInitial list of files:\n";
    fsCacheOpt.listFiles();

    cout << "\n--- File Access Simulation ---\n";

    // Simulate file accesses
    fsCacheOpt.readFile("file1.txt"); // Cache miss
    fsCacheOpt.readFile("file2.txt"); // Cache miss
    fsCacheOpt.readFile("file1.txt"); // Cache hit
    fsCacheOpt.readFile("file3.txt"); // Cache miss
    fsCacheOpt.readFile("file1.txt"); // Cache hit
    fsCacheOpt.readFile("file4.txt"); // Cache miss - should trigger eviction
    fsCacheOpt.readFile("file2.txt"); // Cache hit or miss based on eviction
    fsCacheOpt.readFile("file5.txt"); // Cache miss - should trigger eviction

    cout << "\nCache state after accesses:\n";
    fsCacheOpt.displayCache();

    // Optimize cache based on access patterns
    cout << "\n--- Optimizing Cache ---\n";
    fsCacheOpt.optimizeCache(2);

    cout << "\nCache state after optimization:\n";
    fsCacheOpt.displayCache();

    // Further accesses
    cout << "\n--- Further File Accesses ---\n";
    fsCacheOpt.readFile("file1.txt"); // Likely in cache
    fsCacheOpt.readFile("file5.txt"); // Depending on optimization
    fsCacheOpt.writeFile("file3.txt", "Updated content of file3."); // Update and cache
    fsCacheOpt.readFile("file3.txt"); // Cache hit

    cout << "\nFinal Cache state:\n";
    fsCacheOpt.displayCache();

    return 0;
}
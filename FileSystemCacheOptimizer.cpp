#include <bits/stdc++.h>
using namespace std;

// Structure to hold performance metrics
struct PerformanceMetrics {
    int totalAccesses;
    int cacheHits;
    int cacheMisses;
    double hitRatio;
    double missRatio;
    double totalAccessTime; // in milliseconds

    PerformanceMetrics() : totalAccesses(0), cacheHits(0), cacheMisses(0), hitRatio(0.0), missRatio(0.0), totalAccessTime(0.0) {}

    void updateMetrics(bool hit, double accessTime) {
        totalAccesses++;
        if (hit) cacheHits++;
        else cacheMisses++;
        totalAccessTime += accessTime;
        hitRatio = (totalAccesses > 0) ? ((double)cacheHits / totalAccesses) * 100.0 : 0.0;
        missRatio = (totalAccesses > 0) ? ((double)cacheMisses / totalAccesses) * 100.0 : 0.0;
    }

    void display() const {
        cout << fixed << setprecision(2);
        cout << "\n--- Performance Metrics ---\n";
        cout << "Total Accesses : " << totalAccesses << endl;
        cout << "Cache Hits     : " << cacheHits << endl;
        cout << "Cache Misses   : " << cacheMisses << endl;
        cout << "Hit Ratio      : " << hitRatio << " %" << endl;
        cout << "Miss Ratio     : " << missRatio << " %" << endl;
        cout << "Total Access Time: " << totalAccessTime << " ms" << endl;
        if (totalAccesses > 0) {
            cout << "Average Access Time: " << (totalAccessTime / totalAccesses) << " ms" << endl;
        }
        cout << "----------------------------\n";
    }
};

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
            // Simulate disk I/O delay (e.g., 100 ms for disk access)
           // this_thread::sleep_for(chrono::milliseconds(100));
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
    void listFiles() const {
        cout << "Files in filesystem:\n";
        for (const auto& pair : files) {
            cout << " - " << pair.first << " (Size: " << pair.second.size << " bytes)\n";
        }
    }

    // Get all file names
    vector<string> getAllFileNames() const {
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
        int timestamp; // To resolve ties in frequency (older entries have lower timestamps)
    };

    unordered_map<string, CacheEntry> cacheMap;

    // Min-heap to determine which file to evict based on frequency and timestamp
    // The pair contains (frequency, timestamp) as the key for ordering, and file name as the value
    priority_queue<pair<pair<int, int>, string>, vector<pair<pair<int, int>, string>>, std::greater<pair<pair<int, int>, string>>> minHeap;

    int globalTimestamp;

public:
    Cache(int capacity) : capacity(capacity), currentSize(0), globalTimestamp(0) {}

    // Check if a file is in the cache
    bool isCached(const string& name) const {
        return cacheMap.find(name) != cacheMap.end();
    }

    // Get a file from cache
    File get(const string& name) {
        if (isCached(name)) {
            // Update frequency and timestamp
            cacheMap[name].frequency += 1;
            cacheMap[name].timestamp = globalTimestamp++;
            // Push the updated entry into the minHeap
            minHeap.push({{cacheMap[name].frequency, cacheMap[name].timestamp}, name});
            return cacheMap[name].file;
        } else {
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
            return;
        }

        if (currentSize >= capacity) {
            // Evict the least frequently used file
            while (!minHeap.empty()) {
                auto top = minHeap.top();
                minHeap.pop();
                string evictName = top.second;
                // Verify if this is the current entry
                if (cacheMap.find(evictName) != cacheMap.end() &&
                    cacheMap[evictName].frequency == top.first.first &&
                    cacheMap[evictName].timestamp == top.first.second) {
                    // Evict this file
                    cacheMap.erase(evictName);
                    currentSize--;
                    cout << "Evicted file '" << evictName << "' from cache (LFU Policy).\n";
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
    void displayCache() const {
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
    vector<string> getTopN(int N) const {
        // Use a max heap to retrieve top N
        priority_queue<pair<int, string>> maxHeap;
        for (const auto& pair : accessFrequency) {
            maxHeap.emplace(pair.second, pair.first);
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
    PerformanceMetrics metrics;

public:
    FileSystemCacheOptimizer(int cacheCapacity) : cache(cacheCapacity) {}

    // Add a file to the filesystem
    void addFile(const string& name, const string& content) {
        fs.addFile(name, content);
    }

    // Read a file's content
    string readFile(const string& name) {
        auto start = chrono::high_resolution_clock::now();
        optimizer.recordAccess(name);
        if (cache.isCached(name)) {
            // Cache hit: access time is minimal (e.g., 1 ms)
            File file = cache.get(name);
            auto end = chrono::high_resolution_clock::now();
            double accessTime = 1.0; // in milliseconds
            metrics.updateMetrics(true, accessTime);
            cout << "Cache hit for file '" << name << "'. Access Time: " << accessTime << " ms\n";
            return file.content;
        } else {
            // Cache miss: access time includes disk I/O (e.g., 100 ms)
            string content = fs.readFile(name);
            if (!content.empty()) {
                File file(name, content);
                cache.put(file);
            }
            auto end = chrono::high_resolution_clock::now();
            double accessTime = 100.0; // in milliseconds
            metrics.updateMetrics(false, accessTime);
            cout << "Cache miss for file '" << name << "'. Access Time: " << accessTime << " ms\n";
            return content;
        }
    }

    // Write content to a file
    void writeFile(const string& name, const string& content) {
        auto start = chrono::high_resolution_clock::now();
        fs.writeFile(name, content);
        optimizer.recordAccess(name);
        bool hit = false;
        double accessTime = 0.0;
        if (cache.isCached(name)) {
            // Update cache entry
            File file(name, content);
            cache.put(file);
            hit = true;
            accessTime = 1.0; // cache write is fast
            cout << "Updated file '" << name << "' in cache. Access Time: " << accessTime << " ms\n";
        } else {
            // Cache miss on write
            accessTime = 100.0; // writing to disk
            cout << "File '" << name << "' not in cache. Write to disk. Access Time: " << accessTime << " ms\n";
        }
        metrics.updateMetrics(hit, accessTime);
    }

    // List all files
    void listFiles() const {
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
    void displayCache() const {
        cache.displayCache();
    }

    // Display performance metrics
    void displayPerformanceMetrics() const {
        metrics.display();
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
    fsCacheOpt.readFile("file3.txt"); // Cache hit
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

    // Display performance metrics
    fsCacheOpt.displayPerformanceMetrics();

    return 0;
}
# File System Cache Optimizer

## Overview

This project focuses on implementing different approaches designed to improve the efficiency of file system
operations by optimizing the use of the file system cache. The cache is an in-memory buffer that temporarily
holds frequently accessed file data, minimizing disk I/O operations, which are generally slower than memory
access.

## Approaches Used
1. **Approach 1** : This approach uses a hybrid LRU-LFU replacement policy to modify the cache. Additionally, it verifies correct write-back operations for evicted dirty files.
2. **Approach 2** : This method includes features like adaptive resizing, hybrid LRU-LFU eviction, write-back mechanism and performance metrics to analyse file access efficiency.
3. **Approach 3** : 
4. **Approach 4** : This approach used a clock'based eviction mechanism to manage cache entries using a circular pointer to traverse and evaluate cache entries for eviction.

## Project Structure
- `approach/` : Folders that contain program and test files for implementing the cache optimization techniques
- `README.md` : Overview of the project and instructions for setup and usage.

  ## Getting Started
  To get started with this project, follow these steps:
  1. Clone the repository to your local machine.
  2. Make sure you run the 'C' files in a Linux OS (Eg. Ubuntu)
  3. Compile and Run the program files to simulate the cache optimization process.

## Contributors
- [Rishi Patel](https://github.com/Rishyy37)
- [Raunak Singh](https://github.com/RaunakSingh0312)
- [Aditya Padhy](https://github.com/aditya-padhy/Cache-Optimizer)
- [Keshav Khandelwal](https://github.com/Keshav970)

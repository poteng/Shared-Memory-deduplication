# Shared-Memory-deduplication

Use C++ to implement a red black trees and a system to simulate [Shared Memory De-duplication](https://www.ibm.com/developerworks/linux/library/l-kernel-shared-memory/index.html) which is a popular application used on Linux Kernel.

## Approach
Develop an algorithm that mimics the de-duplication operations in Linux Kernel. 

## Memory simulation
Instead of using real memory, the program simulate the memory operations using a byte arrayy, which will be considered to be the available memory.

## Operations
The system has the following operations: 

### Load
Accept a list of <page id, hash of page content>. 

### Update
Accept another list. If page id already loaded, update its hash.

### De-dupulicate
Run algorithm explained in [IBM's website](https://www.ibm.com/developerworks/linux/library/l-kernel-shared-memory/index.html) and display if any memory is freed.


## Test cases
Three test data set will be randomly generated in the program.

The first set is only for testing Load(), which only loads data of 5 pages into the unstable tree.  

The second set (10 pages) is for testing Update() and Deduplicate(). One of them intends to have the same hash value as one page from the first set, and another two have same hash value with each other’s. These two pairs show the function of merging (the will be moved to stable tree for merging). 

The third set (5 pages) is for testing Update(). One of them intends to have the same page id as one of pairs mentioned above. This is for testing the function of moving changed pages to unstable tree. 

After each function (Load and Update), the trees will show for checking in detail.

## Installation
Build and run [Shared Memory deduplication](SharedMemoryDeduplication.cpp) file

## Details
See [Report](Report.pdf) to understand more about implementation, time complexity and examples.

# Shared-Memory-deduplication

Use red black trees to simulate shared memory de-duplication which is a popular application used on Linux Kernel

The system has the following operations: 

#### Load
Accept a list of <page id, hash of page content>. 

#### Update
Accept another list. If page id already loaded, update its hash.

#### De-dupulicate
Run algorithm explained in [IBM's website](https://www.ibm.com/developerworks/linux/library/l-kernel-shared-memory/index.html) and display if any memory is freed.

An algorithm that mimics the de-duplication operations in Linux Kernel is developed.

## Details
See [Report](Report.pdf) to understand more about implementation, time complexity and examples.

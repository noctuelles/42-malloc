# My own general purpose memory allocator 

This implementation use **segregated memory pools**, **explicit free list** and **first fit** allocation strategy.

## What does it feature ?

- [x] **Time complexity** of the malloc operation is *linear in the number of free blocks*, and free operation is *constant*.
- [x] **Defragmentation** is done on the fly by coalescing adjacent free blocks.
- [x] **Thread-safe** implementation using a global mutex.
- [x] **Tunables** parameters (via environnement variables) to adjust the memory pool size, memory pool extension size.
- [x] **Unit tested** with [Unity](https://www.throwtheswitch.org/unity).

## How this project could be improved ?

- [ ] Use of segregated free lists to reduce the time complexity of the malloc operation, averaging **best-fit** with a **first-fit** strategy.
- [ ] **Mutex** *per memory pool* to make the implementation more efficient in a multithreaded environment.
- [ ] Use of more efficient data structures to store free blocks (e.g. **binary search tree**).

## How does it works ?
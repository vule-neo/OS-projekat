
#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP
#include "../lib/hw.h"

class MemoryAllocator{
public:
    static void * kmem_alloc(size_t size);
    static int kmem_free(void* addr);

    struct blockHeader{
        size_t size;
        blockHeader * next = nullptr;
        blockHeader * prev = nullptr;
    };

    static size_t NUM_OF_BLOCKS;
    static blockHeader * freeHead;
    static blockHeader * usedHead;
    static void memBegin();
    static void memClean();

    static void addToOrderedUsedList(blockHeader * whatToAdd, size_t size,uint64 remaining);
    static void tryMergeBothEndsAfterFree(blockHeader * back, blockHeader * front, blockHeader *freeingBlk, void* addr);
};

#endif //PROJECT_BASE_MEMORYALLOCATOR_HPP

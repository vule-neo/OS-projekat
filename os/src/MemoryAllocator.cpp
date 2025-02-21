#include "../h/MemoryAllocator.hpp"
#include "../h/print.hpp"

MemoryAllocator::blockHeader * MemoryAllocator::freeHead = nullptr;
MemoryAllocator::blockHeader * MemoryAllocator::usedHead = nullptr;
size_t MemoryAllocator::NUM_OF_BLOCKS = 0;

void MemoryAllocator::memBegin() {
    usedHead = nullptr;
    freeHead = (blockHeader*)HEAP_START_ADDR;//ako treba %MEM_BLOCK_SIZE = 0 onda: freeHead = (blockHeader*)((uint64)HEAP_START_ADDR+48);
    freeHead->size = (size_t)((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR - sizeof(blockHeader)) /
            MEM_BLOCK_SIZE;
    NUM_OF_BLOCKS = freeHead->size;
    freeHead->next = freeHead->prev = nullptr;
}


//parametar size je u broju blokova velicine MEM_BLOCK_SIZE(64)
void *MemoryAllocator::kmem_alloc(size_t size) {

    if (size <= 0 || size >NUM_OF_BLOCKS){
        //printString("\nSize parameter is out of range.\n");
        return nullptr;
    }
    blockHeader * iterFree = freeHead;
    if (!iterFree){
        //printString("Kernel didn't boot properly.");
        return nullptr;
    }

    blockHeader * prevBlk = nullptr;

    while (iterFree && iterFree->size < size){
        prevBlk = iterFree;
        iterFree = iterFree->next;
    }

    if (!iterFree){
        //printString("No free memory.");
        return nullptr;
    }

    uint64 remaining = iterFree->size - size;//koliko je blokova ostalo
    if (remaining >= 2){//blockHeader je 24 bajta dajem mu citav blok
        //ostao blok za free listu
        blockHeader *newFreeBlock = (blockHeader*)((char*)iterFree + MEM_BLOCK_SIZE + size * MEM_BLOCK_SIZE);
        newFreeBlock->size = (size_t)(remaining - 1);
        if (prevBlk) {
            newFreeBlock->next = prevBlk->next;
            newFreeBlock->prev = prevBlk;
            prevBlk->next = newFreeBlock;
        }else {
            newFreeBlock->next = freeHead->next;
            freeHead = newFreeBlock;
        }
        //blockHeader koji je bio za free, sada se ulancava u used
        addToOrderedUsedList(iterFree, size, 0);
    }else{
        //blockHeader koji je bio za free, sada se ulancava u used
        if(freeHead == iterFree) freeHead = iterFree->next;//da se promijeni glava free liste ako se prvi zauzme a on je glava
        addToOrderedUsedList(iterFree, size, remaining);
    }

    return (void*)((char*)iterFree + MEM_BLOCK_SIZE);// ovdje sizeof(blockHeader)
}


int MemoryAllocator::kmem_free(void *addr) {

    if (addr == nullptr){
        //printStringg("Undefined address.");
        return -1;
    }

    blockHeader * usedIter = usedHead;

    for(; usedIter && (char*)usedIter + MEM_BLOCK_SIZE != addr; usedIter = usedIter->next);// ovdje sizeof(blockHeader)

    if(!usedIter){
        //printStringg("Provided memory address wasn't previously allocated.");
        return -1;
    }

    blockHeader * beforeFreed = usedIter->prev;
    blockHeader * afterFreed = usedIter->next;

    blockHeader * iterFree = freeHead;
    blockHeader * prevFree = nullptr;
    while(iterFree && (char*)iterFree + MEM_BLOCK_SIZE + iterFree->size*MEM_BLOCK_SIZE <= addr){
        prevFree = iterFree;
        iterFree = iterFree->next;
    }

    //spajanje sa prethodnim, sledecim
    //zauzeti blok nema slobodnog prethodnog, ima i prethodnog i sledbenika, nema sledbenika.
    tryMergeBothEndsAfterFree(iterFree, prevFree, usedIter, addr);

    if(beforeFreed) beforeFreed->next = afterFreed;
    else usedHead = afterFreed;
    if(afterFreed) afterFreed->prev = beforeFreed;

    return 0;
}


void MemoryAllocator::addToOrderedUsedList(MemoryAllocator::blockHeader *whatToAdd, size_t size, uint64 remaining) {
    if (!usedHead){
        usedHead = (blockHeader*)whatToAdd;
        usedHead->size = size+remaining;
    }else{
        blockHeader * usedIter = usedHead;
        blockHeader * newUsed = whatToAdd;
        blockHeader * usedPrev = nullptr;
        for(; usedIter && (char*)usedIter + usedIter->size*MEM_BLOCK_SIZE +
                          MEM_BLOCK_SIZE <= (char*)whatToAdd; usedPrev = usedIter, usedIter = usedIter->next);

        newUsed->next = usedIter;
        newUsed->prev = usedPrev;
        newUsed->size = size+remaining;
        if(!usedPrev){
            usedHead->prev = newUsed;
            usedHead = newUsed;
        }else{
            usedPrev->next = newUsed;
        }
        if (usedIter){
            usedIter->prev = newUsed;
        }

    }
}

void MemoryAllocator::tryMergeBothEndsAfterFree(MemoryAllocator::blockHeader *back,
                                                MemoryAllocator::blockHeader *front,
                                                MemoryAllocator::blockHeader *freeingBlk,
                                                void* addr)
{

    if(front && (char*)front + front->size*MEM_BLOCK_SIZE + 2*MEM_BLOCK_SIZE== addr){
        if(back && (char*)freeingBlk + freeingBlk->size*MEM_BLOCK_SIZE + MEM_BLOCK_SIZE== (char*)back){
            //ima i prije i poslije free blok
            front->size = front->size + back->size + freeingBlk->size + 2;//2 bloka za blockheader-e
            front->next = back->next;
            back->prev = front;
        }else{
            front->size = front->size + freeingBlk->size + 1;

        }
    }else{

        if(back && (char*)freeingBlk + freeingBlk->size*MEM_BLOCK_SIZE + MEM_BLOCK_SIZE== (char*)back){
            freeingBlk->size += back->size + 1;
            if (!front) {
                freeHead = freeingBlk;
                freeingBlk->next = back->next;
                freeingBlk->prev = nullptr;
            }else{
                front->next = freeingBlk;
                freeingBlk->prev = front;
                freeingBlk->next = back->next;
            }
        }else{//nema ni neposredno ispred ni iza prazan blok nema sazimanja
            if (!front) {
                freeHead = freeingBlk;
                freeingBlk->next = back;
                freeingBlk->prev = nullptr;
            }else{
                front->next = freeingBlk;
                freeingBlk->prev = front;
                freeingBlk->next = back;
                back->prev = freeingBlk;
            }

        }
    }
}

void MemoryAllocator::memClean() {
    usedHead = nullptr;
    freeHead = nullptr;
    NUM_OF_BLOCKS = 0;
}



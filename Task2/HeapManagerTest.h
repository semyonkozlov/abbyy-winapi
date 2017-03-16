#include <Windows.h>

#include "HeapManager.h"

class SomeClassCustomHeap {
private:
    static CHeapManager heapManager;

    int i;
    double d;

public:

    void* operator new(std::size_t size);
    void operator delete(void* p);
};

class SomeClassStandartHeap {
private:
    static HANDLE heapHandle;

    int i;
    double d;

public:
    void* operator new(std::size_t size);
    void operator delete(void* p);

};
#include "HeapManagerTest.h"

const int HeapInitSize = 100;
const int HeapMaxSize = 1'000'000;

CHeapManager SomeClassCustomHeap::heapManager( HeapInitSize, HeapMaxSize );

void* SomeClassCustomHeap::operator new(std::size_t size)
{
    return heapManager.Alloc( size );
}

void SomeClassCustomHeap::operator delete(void * p)
{
    heapManager.Free( p );
}

HANDLE SomeClassStandartHeap::heapHandle = HeapCreate( HEAP_NO_SERIALIZE, HeapInitSize, HeapMaxSize );

void* SomeClassStandartHeap::operator new(std::size_t size)
{
    return HeapAlloc(heapHandle, NULL, size);
}

void SomeClassStandartHeap::operator delete(void* p)
{
    HeapFree( heapHandle, NULL, p );
}



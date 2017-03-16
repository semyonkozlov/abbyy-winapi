#pragma once

#include <Windows.h>

void PrintError( DWORD messageId );

void FindAllocatedMemorySizeLimit();

void FragmentMemory( int numAllocs );

void FindNumKernelObjectsLimit();
void FindNumHandlesLimit();

void FindNumGdiObjectsLimit();

void EmptyFunction();
int FunctionWithParameters( int a, char b, long c );
void FindFunctionMemoryUsage();

void FindPathLimit();
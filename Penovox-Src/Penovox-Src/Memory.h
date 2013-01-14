#ifndef _MEMORY_H
#define _MEMORY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <pin.H>

#include "Debug.h"
#include "Platform-win32.h"

#define MEM_FLAG_DIRTY	0x01

typedef struct _M_MEM_DESCRIPTOR
{
	WINDOWS::DWORD	dwBaseAddress;
	WINDOWS::BYTE	dwFlag;
	WINDOWS::DWORD	dwSize;
} M_MEM_DESCRIPTOR;

typedef struct _M_MEM_LIST_NODE
{
	M_MEM_DESCRIPTOR	pDescriptor;
	_M_MEM_LIST_NODE	*pNext;
	_M_MEM_LIST_NODE	*pPrev;
} M_MEM_LIST_NODE;

typedef struct _M_TRIE_NODE
{
	WINDOWS::BYTE	bFlag;
	_M_TRIE_NODE	*pChildren[2];

} M_TRIE_NODE;

typedef struct _M_TRIE
{
	M_TRIE_NODE *root;
} M_TRIE;

VOID MemoryInit();
VOID MemoryAddDirty(WINDOWS::DWORD dwTarget, WINDOWS::DWORD dwSize);
VOID MemoryAddModuleLoadRegion(WINDOWS::DWORD dwBase, WINDOWS::DWORD dwSize);
BOOL MemoryIsDirty(WINDOWS::DWORD dwTarget, WINDOWS::DWORD dwSize);
VOID MemoryDbgDump();

#endif
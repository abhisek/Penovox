#include "Memory.h"

//==== Start: Memory Store Implementation
static
M_MEM_LIST_NODE	*mRootStore;
static
PIN_MUTEX	mMutex;

#define LOCKED_DO(x)	do {	\
							PIN_MutexLock(&mMutex);		\
							x;							\
							PIN_MutexUnlock(&mMutex);	\
						} while(0)

// 4KB PAGE
#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE-1))
#define PAGE_BASE(x)	(x & PAGE_MASK)
#define PAGE_ALIGN(x)	((x + PAGE_SIZE + 1) & PAGE_MASK)

static
VOID _mStoreInit()
{
	mRootStore = (M_MEM_LIST_NODE*) malloc(sizeof(M_MEM_LIST_NODE));
	if(mRootStore == NULL) {
		DMSG("mStore: Failed to allocate memory to initialize Memory Store");
		PIN_ExitProcess(1);
	}

	mRootStore->pDescriptor.dwBaseAddress = 0;
	mRootStore->pDescriptor.dwSize = 0;
	mRootStore->pNext = mRootStore->pPrev = NULL;
}

static
M_MEM_LIST_NODE *_mStoreGetNodeByAddress(WINDOWS::DWORD dwAddr)
{
	M_MEM_LIST_NODE	*m = mRootStore;

	do {
		if(m->pDescriptor.dwBaseAddress == dwAddr)
			return m;

		m = m->pNext;
	} while(m);

	// New Block
	//DMSG("Creating new BLOCK for 0x%08x", dwAddr);

	m = (M_MEM_LIST_NODE*) malloc(sizeof(M_MEM_LIST_NODE));
	if(m == NULL) {
		DMSG("mStore: Failed to allocate memory for new block");
		PIN_ExitProcess(1);
	}

	m->pDescriptor.dwBaseAddress = dwAddr;
	m->pDescriptor.dwFlag = 0;
	m->pNext = mRootStore;
	m->pPrev = NULL;
	LOCKED_DO((mRootStore = m));

	return m;
}

static
VOID _mStoreSetDirty(WINDOWS::DWORD dwTarget, WINDOWS::DWORD dwSize)
{
	M_MEM_LIST_NODE *m;
	WINDOWS::DWORD	dwPageAddr1 = PAGE_BASE(dwTarget);
	WINDOWS::DWORD	dwPageAddr2 = PAGE_BASE(dwTarget + dwSize);

	m = _mStoreGetNodeByAddress(dwPageAddr1);
	LOCKED_DO({
		m->pDescriptor.dwFlag |= MEM_FLAG_DIRTY;
	});

	//if(dwPageAddr2 != dwPageAddr1) {
	//	m = _mStoreGetNodeByAddress(dwPageAddr1);
	//	LOCKED_DO(m->pDescriptor.dwFlag |= MEM_FLAG_DIRTY);
	//}
}

static
VOID _mStoreClear(WINDOWS::DWORD dwTarget, WINDOWS::DWORD dwSize)
{
	M_MEM_LIST_NODE *m;
	WINDOWS::DWORD	dwPageAddr1 = PAGE_BASE(dwTarget);
	WINDOWS::DWORD	dwPageAddr2 = PAGE_BASE(dwTarget + dwSize);

	m = _mStoreGetNodeByAddress(dwPageAddr1);
	LOCKED_DO(m->pDescriptor.dwFlag &= ~MEM_FLAG_DIRTY);

	if(dwPageAddr2 != dwPageAddr1) {
		m = _mStoreGetNodeByAddress(dwPageAddr1);
		LOCKED_DO(m->pDescriptor.dwFlag &= ~MEM_FLAG_DIRTY);
	}
}

static
BOOL _mStoreIsDirty(WINDOWS::DWORD dwTarget)
{
	M_MEM_LIST_NODE *m;

	m = _mStoreGetNodeByAddress(PAGE_BASE(dwTarget));
	return !!(m->pDescriptor.dwFlag & MEM_FLAG_DIRTY);
}


//==== End: Memory Store Implementation

BOOL MemoryIsDirty(WINDOWS::DWORD dwTarget, WINDOWS::DWORD dwSize)
{
	return _mStoreIsDirty(dwTarget);
}

/* Find Memory Block and increase Range or create new Memory Block */
VOID MemoryAddDirty(WINDOWS::DWORD dwTarget, WINDOWS::DWORD dwSize)
{
	//DMSG("[Memory] Adding Dirty: 0x%08x [%d]", dwTarget, dwSize);
	_mStoreSetDirty(dwTarget, dwSize);
}

VOID MemoryAddModuleLoadRegion(WINDOWS::DWORD dwBase, WINDOWS::DWORD dwSize)
{
	WINDOWS::DWORD i = dwBase;
	WINDOWS::DWORD x = dwBase + dwSize;

	while(i < x) {
		_mStoreClear(i, 1);
		i += PAGE_SIZE;
	}
}

VOID MemoryDbgDump()
{
	M_MEM_LIST_NODE	*m = mRootStore;

	while(m) {
		if(m->pDescriptor.dwFlag & MEM_FLAG_DIRTY)
			DMSG("[MEM_DBG] PageAddr: 0x%08x Flag: 0x%02x", m->pDescriptor.dwBaseAddress, m->pDescriptor.dwFlag);
		m = m->pNext;
	}
}

VOID MemoryInit()
{	
	PIN_MutexInit(&mMutex);
	_mStoreInit();
}
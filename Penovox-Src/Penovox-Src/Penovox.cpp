#include "Penovox.h"
#include "Debug.h"
#include "Platform-win32.h"
#include "Memory.h"

static VOID PvxHandleMemoryWrite(VOID *ip, VOID *pWriteAddr, WINDOWS::DWORD dwWriteSize)
{
	//DMSG("0x%08x: W(%d) @ 0x%08x", (WINDOWS::DWORD) ip, (WINDOWS::DWORD) dwWriteSize, 
	//	pWriteAddr);

	MemoryAddDirty((WINDOWS::DWORD) pWriteAddr, dwWriteSize);
}

static VOID PvxHandleBranch(VOID *ip, VOID *pTargetAddr)
{
	//DMSG("Branch: 0x%08x -> 0x%08x", (WINDOWS::DWORD) ip, (WINDOWS::DWORD) pTargetAddr);
}

VOID PvxHandleModuleLoad(IMG img)
{
	MemoryAddModuleLoadRegion(IMG_StartAddress(img), IMG_SizeMapped(img));
}

VOID PvxHandleInstruction(INS ins)
{
	if(INS_IsMemoryWrite(ins)) {
		INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) PvxHandleMemoryWrite, 
			IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_END);
	}

	if(INS_IsBranchOrCall(ins)) {
		INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) PvxHandleBranch,
			IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);
	}
}
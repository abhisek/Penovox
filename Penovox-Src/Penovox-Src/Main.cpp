#include <iostream>
#include <fstream>
#include <sstream>
#include <pin.H>

#include "Debug.h"
#include "Penovox.h"
#include "Memory.h"

VOID INS_InstFunc(INS ins, VOID *pv)
{
	PvxHandleInstruction(ins);
}

VOID TRACE_InstFunc(TRACE trace, VOID *pv)
{
}

VOID IMG_InstFunc(IMG img, VOID *pv)
{
	DMSG("ImageLoad (0x%08x: %08d): %s", IMG_StartAddress(img), IMG_SizeMapped(img), 
		IMG_Name(img).c_str());

	PvxHandleModuleLoad(img);
}

static
VOID Fini(INT32 code, VOID *v)
{
	MemoryDbgDump();
	return;
}

static
INT32 Usage()
{
	return 0;
}

int main(int argc, char **argv)
{
	PIN_InitSymbols();

	if(PIN_Init(argc, argv))
		return Usage();

	DEBUG_INIT("debug.txt");
	MemoryInit();

	INS_AddInstrumentFunction(INS_InstFunc, NULL);
	TRACE_AddInstrumentFunction(TRACE_InstFunc, NULL);
	IMG_AddInstrumentFunction(IMG_InstFunc, NULL);

	PIN_AddFiniFunction(Fini, 0);
	PIN_StartProgram();

	return 0;
}
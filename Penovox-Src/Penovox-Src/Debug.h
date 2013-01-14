#ifndef _DEBUG_H
#define _DEBUG_H

extern FILE *_dbgLogFd;

VOID DebugInit(char *pFilename);

#define DEBUG_INIT(name)	DebugInit(name)
#define DMSG(x, ...)		do { fprintf(_dbgLogFd, "[DBG] " x "\n", __VA_ARGS__); fflush(_dbgLogFd); } while(0)

#endif
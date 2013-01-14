/* Debug utilities goes here */
#include <pin.H>

FILE *_dbgLogFd = NULL;

VOID DebugInit(char *pFilename)
{
	_dbgLogFd = fopen(pFilename, "w");
	
	if(!_dbgLogFd) {
		PIN_WriteErrorMessage("Failed to open Debug Log File", 0, PIN_ERR_FATAL, 0);
		PIN_ExitProcess(1);
	}
}
#ifndef _PLATFORM_WIN32_H
#define _PLATFORM_WIN32_H

namespace WINDOWS
{
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WinNt.h>
    #include <WinError.h>
    #include <Winsock2.h>
	#include <stdlib.h>
}

#endif
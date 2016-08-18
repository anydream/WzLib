// 111118

#ifndef __MemLeaks_h__
#define __MemLeaks_h__

	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	#define CHECK_MEM_LEAKS	(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF))

#endif
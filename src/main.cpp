#include "game.hpp"
#include "Box2D/Box2D.h"

// CRT's memory leak detection
#if defined(_MSC_VER)
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

int main()
{
#if defined(_MSC_VER)
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//	_CrtSetBreakAlloc(308);
#endif
#endif
	Shoop shoop(1366, 768);
	shoop.Run();

	return 0;
}
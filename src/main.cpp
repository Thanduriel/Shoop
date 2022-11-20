#include "game.hpp"
#include "Box2D/Box2D.h"
#include "gameplay/bots/learning.hpp"

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
//	_CrtSetBreakAlloc(168);
#endif
#endif
#ifdef NDEBUG
	spdlog::set_level(spdlog::level::info);
#else
	spdlog::set_level(spdlog::level::debug);
#endif
//	Learning::ReinforcmentLoop loop("testData_", "gamelogsTest_");
//	loop.Run(8000);
//	loop.Evaluate();
//	return 0;

	Shoop shoop(1366, 768);
	shoop.Run();

	return 0;
}
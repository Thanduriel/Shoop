#include "game.hpp"
#include "box2d/box2d.h"
#include "gameplay/bots/learning.hpp"

// CRT's memory leak detection
#if defined(_MSC_VER)
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef LEARNING_AI
#include <thread>
#endif

int main(int argc , char** argv)
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

#ifdef LEARNING_AI
	const std::string netName = argc > 1 ? argv[1] : "net_";
	const std::string logName = argc > 2 ? argv[2] : "gameLogs_";
	const int gamesPerEpoch = argc > 3 ? atoi(argv[3]) : 4096;
	const int numThreads = argc > 4 ? atoi(argv[4]) : 0;
	Learning::ReinforcmentLoop loop(netName, logName);
	loop.Run(gamesPerEpoch, numThreads);
//	loop.Evaluate();
	return 0;
#else
	Shoop shoop(1366, 768);
	shoop.Run();
#endif

	return 0;
}
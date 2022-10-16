#pragma once

#include "utils/config.hpp"

#include <memory>
#include <vector>

namespace Game {
	class GameState;
}

class Shoop
{
public:
	Shoop(int _sizeX, int _sizeY);
	~Shoop();

	void Run();
	Utils::Config& Config() { return m_config; }
private:
	std::vector<std::unique_ptr<Game::GameState>> m_states;

	bool m_noDraw;
	float m_targetFrameTime;
	Utils::Config m_config;
};
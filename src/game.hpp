#pragma once

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
private:
	std::vector<std::unique_ptr<Game::GameState>> m_states;

	float m_targetFrameTime;
};
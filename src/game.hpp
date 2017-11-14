#pragma once

#include <memory>
#include <vector>
#include "SFML/Graphics.hpp"

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
	// should probably be moved closer to other graphic related classes
	sf::RenderWindow m_window;
};
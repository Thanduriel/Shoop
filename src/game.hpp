#pragma once

#include "SFML/Graphics.hpp"

class Shoop
{
public:
	Shoop(int _sizeX, int _sizeY);

	void Run();
private:
	// should probably be moved closer to other graphic related classes
	sf::RenderWindow m_window;
};
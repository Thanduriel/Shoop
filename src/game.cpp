#include "game.hpp"
#include "Box2D/Box2D.h"

Shoop::Shoop(int _sizeX, int _sizeY)
	: m_window(sf::VideoMode(_sizeX, _sizeY), "shoop")
{
	// actually link something from Box2D
	b2World world(b2Vec2(0.f, -9.81f));
}

void Shoop::Run()
{
	sf::Clock clock;

	while (m_window.isOpen())
	{
		// handle events
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

		sf::Time elapsed = clock.restart();


		m_window.clear(sf::Color(0, 255, 0));
		m_window.display();
	}
}
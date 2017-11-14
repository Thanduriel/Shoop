#include "game.hpp"
#include "Box2D/Box2D.h"
#include "gamestates/gsplay.hpp"
#include "resources.hpp"

Shoop::Shoop(int _sizeX, int _sizeY)
	: m_window(sf::VideoMode(_sizeX, _sizeY), "shoop")
{
	// actually link something from Box2D
	b2World world(b2Vec2(0.f, -9.81f));
}

// explicit definition here for the destructor of GameState
Shoop::~Shoop()
{
}

void Shoop::Run()
{
	sf::Clock clock;

	const sf::Image& t = Resources::Load<sf::Image>("pixel");

	m_states.emplace_back(new Game::GSPlay());

	while (m_states.size() && m_window.isOpen())
	{
		// handle events
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

		sf::Time elapsed = clock.restart();

		Game::GameState* currentState = m_states.back().get();
		currentState->Process(elapsed.asSeconds());
		currentState->Draw();

		auto newState = currentState->GetNewState();
		if (currentState->IsFinished()) m_states.pop_back();
		if (newState) m_states.emplace_back(std::move(newState));

		// currentState could be invalid at this point, but the address is still correct
		if (m_states.size() && currentState != m_states.back().get()) m_states.back()->OnBegin();

		m_window.clear(sf::Color(100, 149, 237));
		m_window.display();
	}
}
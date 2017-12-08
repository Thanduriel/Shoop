#include "game.hpp"
#include "Box2D/Box2D.h"
#include "gamestates/gsplay.hpp"
#include "resources.hpp"
#include "SFML/System.hpp"
#include "graphics/device.hpp"
#include "SFML/Graphics.hpp"

using namespace Graphics;

Shoop::Shoop(int _sizeX, int _sizeY)
{
	Device::Init(_sizeX, _sizeY);
}

// explicit definition here for the destructor of GameState
Shoop::~Shoop()
{
	Graphics::Device::Close();
}

void Shoop::Run()
{
	sf::Clock clock;

	m_states.emplace_back(new Game::GSPlay());

	while (m_states.size() && Device::GetWindow().isOpen())
	{
		// handle events
		sf::Event event;
		while (Device::GetWindow().pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				Device::GetWindow().close();
		}

		sf::Time elapsed = clock.restart();
		// temporary frame limiter; todo do better timing with frame cap
		sf::sleep(sf::milliseconds(2));

		Device::GetWindow().clear(sf::Color(100, 149, 237));
		Game::GameState* currentState = m_states.back().get();
		currentState->Process(elapsed.asSeconds());
		currentState->Draw(Device::GetWindow());

		auto newState = currentState->GetNewState();
		if (currentState->IsFinished()) m_states.pop_back();
		if (newState) m_states.emplace_back(std::move(newState));
		if (m_states.size() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) 
			m_states.pop_back();
		// currentState could be invalid at this point, but the address is still correct
		if (m_states.size() && currentState != m_states.back().get()) m_states.back()->OnBegin();

		Device::GetWindow().display();
	}
}
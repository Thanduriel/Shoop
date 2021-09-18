#include "game.hpp"
#include "Box2D/Box2D.h"
#include "gamestates/gsplay.hpp"
#include "resources.hpp"
#include "SFML/System.hpp"
#include "graphics/device.hpp"
#include "SFML/Graphics.hpp"
#include "spdlog/spdlog.h"

using namespace Graphics;

Shoop::Shoop(int _sizeX, int _sizeY)
	: m_targetFrameTime(1.f / 60.f),
	m_config(Utils::CONFIG_PATH)
{
	const Utils::ConfigSection& videoSettings = m_config.GetSection("video");
	Device::Init(videoSettings.GetValue<int>("ResolutionX"), 
		videoSettings.GetValue<int>("ResolutionY"),
		videoSettings.GetValue<int>("Fullscreen"));
	Device::GetWindow().setKeyRepeatEnabled(false);
}

// explicit definition here for the destructor of GameState
Shoop::~Shoop()
{
	Graphics::Device::Close();
}

void Shoop::Run()
{
	spdlog::info("Running the game");

	m_states.emplace_back(new Game::GSPlay(m_config));

	sf::Clock clock;

	while (m_states.size() && Device::GetWindow().isOpen())
	{
		// handle events
		sf::Event event;
		while (Device::GetWindow().pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				Device::GetWindow().close();
		}

		const sf::Time frameTime = clock.restart();
	//	Device::GetWindow().setTitle(std::to_string(frameTime.asSeconds()));;

		Game::GameState* currentState = m_states.back().get();
		currentState->Process(frameTime.asSeconds());
		
		Device::GetWindow().clear(sf::Color(100, 149, 237));
		currentState->Draw(Device::GetWindow());
		Device::GetWindow().display();

		auto newState = currentState->GetNewState();
		if (currentState->IsFinished()) m_states.pop_back();
		if (newState) m_states.emplace_back(std::move(newState));
		if (m_states.size() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) 
			m_states.pop_back();
		// currentState could be invalid at this point, but the address is still correct
		if (m_states.size() && currentState != m_states.back().get()) m_states.back()->OnBegin();

		// limit frame rate
		const sf::Time workTime = clock.getElapsedTime();
		const sf::Time wait = sf::seconds(m_targetFrameTime) - workTime;
		if(wait.asMilliseconds() > 1) sf::sleep(wait);
	}
}
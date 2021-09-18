#include "device.hpp"
#include "SFML/Graphics.hpp"
#include "spdlog/spdlog.h"

namespace Graphics {

	sf::RenderWindow* Device::m_window;

	const Utils::ConfigSection::Initializer<int, 3> VideoSettings(
		{ {
			{"ResolutionX", 1400},
			{"ResolutionY", 1050},
			{"Fullscreen", 0}
		} });

	const float WORLD_TO_SCREEN = 70.f;

	void Device::Init(int _sizeX, int _sizeY, bool _fullScreen)
	{
		spdlog::info("Initializing graphics device");

		m_window = new sf::RenderWindow();
		Resize(_sizeX, _sizeY, _fullScreen);
	}

	void Device::Close()
	{
		delete m_window;
	}

	void Device::Resize(int _sizeX, int _sizeY, bool _fullScreen)
	{
		sf::ContextSettings settings;
		settings.antialiasingLevel = 4;
		const sf::Uint32 style = _fullScreen ? sf::Style::Fullscreen
			: sf::Style::Titlebar | sf::Style::Close;
		m_window->create(sf::VideoMode(_sizeX, _sizeY), "shoop", style, settings);

		SetView(true);
	}

	Math::Vec2 Device::ToScreenSpace(Math::Vec2 _position)
	{
		_position.y = GetSizeWorldSpace().y - _position.y;
		return _position;
	}

	float Device::GetWorldToScreenFactor()
	{
		return WORLD_TO_SCREEN;
	}

	Math::Vec2 Device::GetSizeWorldSpace()
	{
		return Math::Vec2(m_window->getSize()) / WORLD_TO_SCREEN;
	}

	void Device::SetView(bool _worldView)
	{
		m_window->setView(_worldView ? 
			sf::View(sf::FloatRect(Math::Vec2(0.f), GetSizeWorldSpace()))
			: m_window->getDefaultView());
	}
}
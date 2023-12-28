#include "device.hpp"
#include "SFML/Graphics.hpp"
#include "spdlog/spdlog.h"

namespace Graphics {

	sf::RenderWindow* Device::m_window = nullptr;
	bool Device::m_isDummy = false;
	Math::Vec2U Device::m_size = {};

	const Utils::ConfigSection::Initializer<int, 4> VideoSettings(
		{ {
			{"ResolutionX", 1400},
			{"ResolutionY", 1050},
			{"Fullscreen", 0},
			{"NoDraw", 0}
		} });

	const float WORLD_TO_SCREEN = 70.f;

	std::mutex g_deviceMutex;

	void Device::Init(int _sizeX, int _sizeY, bool _fullScreen)
	{
		// even with multiple game instances just one window is needed
		std::scoped_lock<std::mutex> lock(g_deviceMutex);
		if (!m_window)
		{
			spdlog::debug("Initializing graphics device");

			m_window = new sf::RenderWindow();
			Resize(_sizeX, _sizeY, _fullScreen);
			m_window->setKeyRepeatEnabled(false);

			const sf::Image& icon = Resources::Load<sf::Image>("icon");
			m_window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		}
	}

	void Device::InitDummy(int _sizeX, int _sizeY)
	{
		std::scoped_lock<std::mutex> lock(g_deviceMutex);
		if(!m_isDummy){
			spdlog::debug("Initializing dummy device");
			m_isDummy = true;
			m_size = Math::Vec2U(_sizeX, _sizeY);
		}
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
		m_size = m_window->getSize();
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
		return Math::Vec2(m_size) / WORLD_TO_SCREEN;
	}

	void Device::SetView(bool _worldView)
	{
		m_window->setView(_worldView ? 
			sf::View(sf::FloatRect(Math::Vec2(0.f), GetSizeWorldSpace()))
			: m_window->getDefaultView());
	}
}
#include "device.hpp"
#include "SFML/Graphics.hpp"

namespace Graphics {

	sf::RenderWindow* Device::m_window;

	const float WORLD_TO_SCREEN = 50.f;

	void Device::Init(int _sizeX, int _sizeY)
	{
		m_window = new sf::RenderWindow();
		Resize(_sizeX, _sizeY);
	}

	void Device::Close()
	{
		delete m_window;
	}

	void Device::Resize(int _sizeX, int _sizeY)
	{
		sf::ContextSettings settings;
		settings.antialiasingLevel = 4;
		m_window->create(sf::VideoMode(_sizeX, _sizeY), "shoop",
			sf::Style::Titlebar | sf::Style::Close, settings);

		// adjust view
		sf::View view(sf::FloatRect(Math::Vec2(0.f), GetSizeWorldSpace()));
		m_window->setView(view);
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
}
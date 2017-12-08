#include "device.hpp"
#include "SFML/Graphics.hpp"

namespace Graphics {

	sf::RenderWindow* Device::m_window;

	const float WORLD_TO_SCREEN = 50.f;

	void Device::Init(int _sizeX, int _sizeY)
	{
		m_window = new sf::RenderWindow(sf::VideoMode(_sizeX, _sizeY), "shoop");

		// adjust view
		sf::View view(sf::FloatRect(Math::Vec2(0.f), GetSizeWorldSpace()));
		m_window->setView(view);
	}

	void Device::Close()
	{
		delete m_window;
	}

	Math::Vec2 Device::ToScreenSpace(Math::Vec2 _position)
	{
		_position.y = GetSizeWorldSpace().y - _position.y;
		return _position;
	}

	Math::Vec2 Device::GetSizeWorldSpace()
	{
		return Math::Vec2(m_window->getSize()) / WORLD_TO_SCREEN;
	}
}
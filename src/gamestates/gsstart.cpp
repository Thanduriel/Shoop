#include "gsstart.hpp"
#include "utils/config.hpp"
#include "spdlog/spdlog.h"
#include "resources.hpp"
#include "gsplay.hpp"
#include "graphics/device.hpp"
#include "input/gamepadinputmanager.hpp"

#include <SFML/Graphics.hpp>

namespace Game {

	GSStart::GSStart(const Utils::Config& _config)
		: m_config(_config)
	{
		const sf::Texture& background = Resources::Load<sf::Texture>("Background");
		m_background.setTexture(background, true);
		const Math::Vec2U windowSize = Graphics::Device::GetWindow().getSize();
		const Math::Vec2U spriteSize = m_background.getTexture()->getSize();

		const float scale = static_cast<float>(windowSize.y) / spriteSize.y;
		m_background.setScale(sf::Vector2f(scale, scale));
		m_background.setPosition(Math::Vec2(windowSize) * 0.5f);
		m_background.setOrigin(Math::Vec2(spriteSize) * 0.5f);
		const float edge = windowSize.x - spriteSize.x * scale;
		m_leftExtension.setTexture(background);
		m_leftExtension.setTextureRect(sf::IntRect(0, 0, 1, spriteSize.y));
		m_leftExtension.setScale(sf::Vector2f(edge * 0.5f, scale));
		m_rightExtension.setTexture(background);
		m_rightExtension.setTextureRect(sf::IntRect(spriteSize.x - 1, 0, 1, spriteSize.y));
		m_rightExtension.setScale(sf::Vector2f(edge * 0.5f, scale));
		m_rightExtension.setPosition(sf::Vector2f(windowSize.x - edge * 0.5f, 0.f));
	}

	void GSStart::Process(float _deltaTime)
	{
	}

	void GSStart::Draw(sf::RenderWindow& _window)
	{
		Graphics::Device::SetView(false);
		_window.clear(sf::Color(152, 197, 149));

		_window.draw(m_leftExtension);
		_window.draw(m_rightExtension);
		_window.draw(m_background);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)
			|| sf::Joystick::isButtonPressed(0,static_cast<unsigned>(Input::GamePadButton::A)))
		{
			m_newState = std::make_unique<GSPlay>(m_config);
		}
	}

}
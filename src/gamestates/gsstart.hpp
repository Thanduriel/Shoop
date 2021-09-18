#pragma once

#include "gamestate.hpp"
#include <SFML/Graphics.hpp>

namespace Utils {
	class Config;
}

namespace Game {

	class GSStart : public GameState
	{
	public:
		GSStart(const Utils::Config& _config);

		void Process(float _deltaTime) override;
		void Draw(sf::RenderWindow& _window) override;
	private:
		const Utils::Config& m_config;
		sf::Sprite m_background;
		sf::Sprite m_leftExtension;
		sf::Sprite m_rightExtension;
	};
}
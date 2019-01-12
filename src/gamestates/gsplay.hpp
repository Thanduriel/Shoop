#pragma once

#include "gamestate.hpp"
#include "gameplay/scene.hpp"

namespace Utils {
	class Config;
}

namespace Game {

	class GSPlay : public GameState
	{
	public:
		GSPlay(const Utils::Config& _config);

		void Process(float _deltaTime) override;
		void Draw(sf::RenderWindow& _window) override;

	private:
		Scene m_scene;
	};
}
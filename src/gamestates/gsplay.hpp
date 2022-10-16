#pragma once

#include "gamestate.hpp"
#include "gameplay/scene.hpp"
#include "gameplay/elements/player/playercontrollercomponent.hpp"

namespace Utils {
	class Config;
}

namespace Game {

	class Map;
	class Rules;
	class TextComponent;

	class GSPlay : public GameState
	{
	public:
		GSPlay(const Utils::Config& _config);
		~GSPlay();

		void Process(float _deltaTime) override;
		void Draw(sf::RenderWindow& _window) override;
	private: 

		Scene m_scene;
		std::unique_ptr<Rules> m_rules;
		bool m_autoReset;

		ControllerContainer m_controllers;
		TextComponent* m_scoreScreen;

		std::unique_ptr<PlayerControllerComponent> m_controller1;
		std::unique_ptr<PlayerControllerComponent> m_controller2;

		std::unique_ptr<Input::InputInterface> m_input1;
		std::unique_ptr<Input::InputInterface> m_input2;
	};
}
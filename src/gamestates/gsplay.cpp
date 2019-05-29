#include "gsplay.hpp"
#include "utils/config.hpp"
#include "gameplay/rules/classic.hpp"
#include "gameplay/maps/map.hpp"
#include "spdlog/spdlog.h"

// test related
#include "gameplay/elements/spriteomponent.hpp"
#include "resources.hpp"
#include "gameplay/elements/player/sheep.hpp"
#include "gameplay/elements/wall.hpp"
#include "graphics/device.hpp"
#include "gameplay/elements/ground.hpp"
#include "gameplay/elements/physicalparticle.hpp"
#include "generators/random.hpp"
#include "gameplay/elements/factory.hpp"
#include "gameplay/elements/player/playercontrollercomponent.hpp"
#include "input/keyboardinputmanager.hpp"
#include "SFML/Graphics.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "gameplay/elements/textcomponent.hpp"

namespace Game {

	using namespace Math;

	std::unique_ptr<PlayerControllerComponent> controller1;
	std::unique_ptr<PlayerControllerComponent> controller2;
	ControllerContainer controllers;
	TextComponent* scoreScreen;

	GSPlay::GSPlay(const Utils::Config& _config)
		: m_rules(new Classic(controllers, 42))
	{
		spdlog::info("Creating main state");

		Map* map = new Map(m_scene);
		m_scene.Add(*map);
		m_rules->SetMap(*map);

		static Input::KeyBoardInputInterface input1(_config.GetSection("keyboard1"));
		static Input::KeyBoardInputInterface input2(_config.GetSection("keyboard2"));

		Sheep* sheep1 = new Sheep(Vec2());
		Sheep* sheep2 = new Sheep(Vec2());
		controller1 = std::make_unique<PlayerControllerComponent>(*sheep1, input2);
		controller2 = std::make_unique<PlayerControllerComponent>(*sheep2, input1);
		controllers.push_back(controller1.get());
		controllers.push_back(controller2.get());
		// register after to also register the controllers
		m_scene.Add(*sheep1);
		m_scene.Add(*sheep2);

		auto scoreActor = new SingleComponentActor<TextComponent>(Graphics::Device::GetSizeWorldSpace() * Vec2(0.5f, 0.75f),
			Resources::Load<sf::Font>("Anaktoria"), 64);
		m_scene.Add(*scoreActor);
		scoreScreen = &scoreActor->GetComponent();

		m_rules->Start();
	}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.CleanUp();
		m_scene.RegisterActors();

		m_scene.Process(_deltaTime);

		m_rules->Process(_deltaTime);
		if (m_rules->IsOver()) Finish();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		m_scene.Draw(_window);

		scoreScreen->SetText(m_rules->GetScore());
	}
}
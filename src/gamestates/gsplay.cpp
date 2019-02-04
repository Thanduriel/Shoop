#include "gsplay.hpp"
#include "utils/config.hpp"

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
#include "gameplay/maps/map.hpp"

namespace Game {

	using namespace Math;

	Sheep* actor;
	std::unique_ptr<FactoryComponent> testFactory;
	std::unique_ptr<PlayerControllerComponent> controller;

	GSPlay::GSPlay(const Utils::Config& _config)
	{
		static Input::KeyBoardInputInterface input(_config.GetSection("keyboard1"));

		actor = new Sheep(Graphics::Device::GetSizeWorldSpace() * 0.5f);
		testFactory = std::make_unique<FactoryComponent>(*actor, Vec2(0.f, 0.9f));
		controller = std::make_unique<PlayerControllerComponent>(*actor, input);
	//	actor->SetScale(Vec2(100.f));
	//	actor->GetComponent().SetPosition(Math::Vec2(0.5f, 0.f));
		m_scene.Add(*actor);

		Map map(m_scene);
	}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.Process(_deltaTime);

		

		// should probably happen after the draw
		m_scene.CleanUp();
		m_scene.RegisterActors();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		m_scene.Draw(_window);
	}
}
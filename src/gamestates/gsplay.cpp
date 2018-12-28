#include "gsplay.hpp"

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

namespace Game {

	using namespace Math;

	Sheep* actor;
	std::unique_ptr<FactoryComponent> testFactory;
	std::unique_ptr<PlayerControllerComponent> controller;

	Input::KeyBoardInputInterface input;

	GSPlay::GSPlay()
	{

		Actor* wall = new Wall(Graphics::Device::GetSizeWorldSpace() * Math::Vec2(0.5f, 0.1f), Math::Vec2(10.f, 1.f), PhysicsInfo::IsGround);
		m_scene.Add(*wall);
		actor = new Sheep(Graphics::Device::GetSizeWorldSpace() * 0.5f);
		testFactory = std::make_unique<FactoryComponent>(*actor, Vec2(0.f, 0.9f));
		controller = std::make_unique<PlayerControllerComponent>(*actor, input);
	//	actor->SetScale(Vec2(100.f));
	//	actor->GetComponent().SetPosition(Math::Vec2(0.5f, 0.f));
		m_scene.Add(*actor);

		using namespace Generators;
		Generators::CurveGen generator;
		Curve curve = generator.Generate(0xACCCC, 8, Vec2(11.f, 2.f), Vec2(14.f, 5.f));
		curve[0].y = 8.f;
		curve[1].y = 3.8f;
		curve[2].y = 3.7f;
		curve[4].y = 3.6f;
		curve[5].y = 3.55f;
		curve[6].y = 3.5f;
		curve[7].y = 8.6f;

		m_scene.Add(*new GroundPlane(generator.SampleSmooth<>(curve, 30)));
	}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.Process(_deltaTime);

		static float sum = 0.f;

		sum += _deltaTime;
		if (sum > 1.15f)
		{
		//	actor->Jump();
			sum = 0.f;
	
		/*	for (int i = 0; i < 20; ++i)
				testFactory->CreateTransformedV<PhysicalParticle>(Vec2(0.f), 0.f,
					Vec2(Generators::g_random.Uniform(-0.1f,0.1f),Generators::g_random.Uniform(4.5f,8.2f)),
					Resources::Load<sf::Texture>("wheel"),
					Generators::g_random.Uniform(1.5f, 4.f), 0.1f);*/
		}

		// should probably happen after the draw
		m_scene.CleanUp();
		m_scene.RegisterActors();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		m_scene.Draw(_window);
	}
}
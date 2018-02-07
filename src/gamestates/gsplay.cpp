#include "gsplay.hpp"

// test related
#include "gameplay/elements/spriteomponent.hpp"
#include "resources.hpp"
#include "gameplay/elements/player/sheep.hpp"
#include "gameplay/elements/wall.hpp"
#include "graphics/device.hpp"
#include "gameplay/elements/ground.hpp"

namespace Game {

	using namespace Math;

	Sheep* actor;

	GSPlay::GSPlay()
	{
		Actor* wall = new Wall(Graphics::Device::GetSizeWorldSpace() * Math::Vec2(0.5f, 0.1f), Math::Vec2(10.f, 1.f));
		m_scene.Add(*wall);
		actor = new Sheep(Graphics::Device::GetSizeWorldSpace() * 0.5f);
	//	actor->SetScale(Vec2(100.f));
	//	actor->GetComponent().SetPosition(Math::Vec2(0.5f, 0.f));
		m_scene.Add(*actor);

		using namespace Generators;
		Generators::CurveGen generator;
		Curve curve = generator.Generate(0xACCCC, 8, Vec2(11.f, 2.f), Vec2(14.f, 5.f));
		curve[0].y = 4.f;
		curve[1].y = 4.f;
		curve[6].y = 4.f;
		curve[7].y = 4.f;

		m_scene.Add(*new GroundPlane(generator.SampleSmooth<>(curve, 30)));
	}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.Process(_deltaTime);

		static float sum = 0.f;
		sum += _deltaTime;
		if (sum > 1.f)
		{
			actor->Jump();
			sum = 0.f;
		}

		// should probably happen after the draw
		m_scene.CleanUp();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		m_scene.Draw(_window);
	}
}
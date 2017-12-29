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

	Actor* actor;

	GSPlay::GSPlay()
	{
		Actor* wall = new Wall(Graphics::Device::GetSizeWorldSpace() * Math::Vec2(0.5f, 0.1f), Math::Vec2(10.f, 1.f));
		m_scene.Add(*wall);
		actor = new Sheep(Graphics::Device::GetSizeWorldSpace() * 0.5f);
	//	actor->SetScale(Vec2(100.f));
	//	actor->GetComponent().SetPosition(Math::Vec2(0.5f, 0.f));
		m_scene.Add(*actor);

		m_scene.Add(*new GroundPlane(Vec2(0.4f, 0.f), Vec2(18.f, 7.f)));
	}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.Process(_deltaTime);

		// should probably happen after the draw
		m_scene.CleanUp();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		m_scene.Draw(_window);
	}
}
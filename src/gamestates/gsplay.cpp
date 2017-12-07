#include "gsplay.hpp"

// test related
#include "gameplay/elements/spriteomponent.hpp"
#include "resources.hpp"
#include "gameplay/elements/player/sheep.hpp"

namespace Game {

	using namespace Math;

	Actor* actor;

	GSPlay::GSPlay()
	{
		actor = new Sheep(Vec2(100.f, 0.f));
	//	actor->SetScale(Vec2(100.f));
	//	actor->GetComponent().SetPosition(Math::Vec2(0.5f, 0.f));
		m_scene.Add(*actor);
	}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.Process(_deltaTime);

		actor->SetPosition(actor->GetPosition() + Vec2(_deltaTime * 10.f));
		actor->SetRotation(actor->GetRotation() + _deltaTime);

		// should probably happen after the draw
		m_scene.CleanUp();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		m_scene.Draw(_window);
	}
}
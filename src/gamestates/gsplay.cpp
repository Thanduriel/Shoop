#include "gsplay.hpp"

// test related
#include "gameplay/elements/spriteomponent.hpp"
#include "resources.hpp"

namespace Game {

	using namespace Math;

	SpriteActor* actor;

	GSPlay::GSPlay()
	{
		actor = new SingleComponentActor<SpriteComponent>(Vec2(), Resources::Load<sf::Texture>("pixel"));
		actor->SetScale(Vec2(100.f));
		m_scene.Add(*actor);
	}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.Process(_deltaTime);

		actor->SetPosition(actor->GetPosition() + Vec2(_deltaTime * 10.f));
		actor->SetRotation(actor->GetRotation() + _deltaTime * 16.f);

		// should probably happen after the draw
		m_scene.CleanUp();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		m_scene.Draw(_window);
	}
}
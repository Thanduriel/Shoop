#include "controllercomponent.hpp"
#include "jumpcomponent.hpp"
#include "math/vector.hpp"

#include "gameplay/elements/physicscomponent.hpp"
//todo: use keyboardmanager instead
#include "SFML/Window.hpp"

namespace Game {

	ControllerComponent::ControllerComponent(Actor& _actor, JumpComponent& _jump, 
		PhysicsBodyComponent& _wheelBody,
		PhysicsBodyComponent& _headBody)
		: ProcessComponent(_actor),
		m_jump(_jump),
		m_wheelBody(_wheelBody),
		m_headBody(_headBody)
	{

	}

	void ControllerComponent::Process(float _deltaTime)
	{
		using Key = sf::Keyboard::Key;
		if (sf::Keyboard::isKeyPressed(Key::Space))
			m_jump.Jump();

		using namespace Math;
		Vec2 up = m_actor.GetRotated(Vec2(0.f, 1.f));
		const float s = std::acos(std::abs(up.y)) * 1.5f;
		if (sf::Keyboard::isKeyPressed(Key::Left))
			m_headBody.Get().ApplyTorque(s, true);
		if (sf::Keyboard::isKeyPressed(Key::Right))
			m_headBody.Get().ApplyTorque(-s, true);

		if (sf::Keyboard::isKeyPressed(Key::Up))
			m_wheelBody.Get().ApplyTorque(1.f, true);
		if (sf::Keyboard::isKeyPressed(Key::Down))
			m_wheelBody.Get().ApplyTorque(-1.f, true);;


	}
}
#include "controllercomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "jumpcomponent.hpp"

namespace Game {

	ControllerComponent::ControllerComponent(Actor& _actor)
		: ProcessComponent(_actor),
		m_jump(*_actor.GetComponent<JumpComponent>()),
		m_wheelBody(*_actor.GetComponent<PhysicsBodyComponentD>()),
		m_headBody(*_actor.GetComponent<PhysicsBodyComponentD,1>())
	{

	}
}
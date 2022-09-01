#include "controllercomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "jumpcomponent.hpp"

namespace Game {

	ControllerComponent::ControllerComponent(Actor& _actor)
		: ProcessComponent(_actor)
	{

	}
}
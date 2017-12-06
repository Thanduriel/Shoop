#include "component.hpp"
#include "actor.hpp"

namespace Game {

	Component::Component(Actor& _actor, Type _type)
		: m_actor(_actor), m_type(_type) 
	{ 
		_actor.GetComponents().push_back(this); 
	}

	Component::~Component()
	{}
}
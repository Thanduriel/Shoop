#include "component.hpp"
#include "actor.hpp"

namespace Game {

	Component::Component(Actor& _actor, Type _type)
		: m_actor(&_actor), m_type(_type) 
	{ 
		_actor.GetComponents().push_back(this); 
	}

	Component::~Component()
	{}

	void Component::Attach(Actor& _actor)
	{
		auto& components = m_actor->GetComponents();
		auto it = std::find(components.begin(), components.end(), this);
		assert(it != components.end());
		components.erase(it);

		m_actor = &_actor;
		_actor.GetComponents().push_back(this);
	}
}
#include "component.hpp"
#include "actor.hpp"
#include "utils/assert.hpp"

namespace Game {

	Component::Component(Actor& _actor, Type _type, bool _shouldRegister)
		: m_actor(&_actor), m_type(_type) , m_shouldRegister(_shouldRegister)
	{ 
		_actor.GetComponents().push_back(this); 
	}

	Component::~Component()
	{}

	void Component::Attach(Actor& _actor)
	{
		// remove from last actor
		auto& components = m_actor->GetComponents();
		auto it = std::find(components.begin(), components.end(), this);
		Assert(it != components.end(), "Component was not owned by an actor.");
		components.erase(it);
		// is already registered
		m_shouldRegister = false;

		m_actor = &_actor;
		_actor.GetComponents().push_back(this);
	}
}
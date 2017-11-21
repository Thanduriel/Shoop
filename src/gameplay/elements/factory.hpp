#pragma once

#include <vector>
#include <memory>
#include "gameplay/core/component.hpp"
#include "gameplay/core/singlecomponentactor.hpp"

namespace Game {

	class FactoryComponent : public Component
	{
	public:
		FactoryComponent(Actor& _actor);

		// Add a dynamically created Actor to later be registered by the scene.
		void Add(Actor& _actor);

		// Creates a new Actor in place with the given arguments.
		template<typename T, typename... Args>
		T& Create(Args&&... _args)
		{
			m_createdActors.emplace_back(new T(std::forward<Args>(_args)...));
		}

	private:
		std::vector<std::unique_ptr<Actor>> m_createdActors;

		friend class Scene; // required to fetch the actors
	};

	using ActorFactory = SingleComponentActor<FactoryComponent>;
}
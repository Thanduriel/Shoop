#pragma once

#include <vector>
#include <memory>
#include "gameplay/core/component.hpp"
#include "gameplay/core/singlecomponentactor.hpp"

namespace Game {

	class FactoryComponent : public Component, public Math::Transformation
	{
	public:
		FactoryComponent(Actor& _actor, Math::Vec2 _position = {}, float _rotation = 0.f);

		// Add a dynamically created Actor to later be registered by the scene.
		void Add(Actor& _actor);

		// Creates a new Actor in place with the given arguments.
		template<typename T, typename... Args>
		T& Create(Args&&... _args)
		{
			T* actor = new T(std::forward<Args>(_args)...);
			m_createdActors.emplace_back(*actor);

			return *actor;
		}

		// Creates a new actor in place and applies the factory's transformation.
		template<typename T, typename... Args>
		T& CreateTransformed(Math::Vec2 _position, float _rotation, Args&&... _args)
		{
			const Transformation transform = m_actor.Transform(*this);

			const Math::Vec2 p = transform.Transform(_position);

			T* actor = new T(p, _rotation + transform.GetRotation(),
				std::forward<Args>(_args)...);
			m_createdActors.emplace_back(actor);

			return *actor;
		}

		// Creates a new actor in place and applies the factory's transformation.
		// Also rotates the given velocity.
		template<typename T, typename... Args>
		T& CreateTransformedV(Math::Vec2 _position, float _rotation, Math::Vec2 _velocity, 
			Args&&... _args)
		{
			const Transformation transform = m_actor.Transform(*this);

			const Math::Vec2 p = transform.Transform(_position);
			const Math::Vec2 v = transform.GetRotated(_velocity);
			
			T* actor = new T(p, _rotation + transform.GetRotation(), _velocity,
				std::forward<Args>(_args)...);
			m_createdActors.emplace_back(actor);

			return *actor;
		}

	private:
		std::vector<std::unique_ptr<Actor>> m_createdActors;

		friend class Scene; // required to fetch the actors
	};

	using ActorFactory = SingleComponentActor<FactoryComponent>;
}
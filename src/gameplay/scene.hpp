#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Box2D/Box2D.h"

namespace sf {
	class RenderWindow;
}

namespace Game {

	class Actor;
	class Component;
	class DrawComponent;
	class ProcessComponent;
	class FactoryComponent;

	/* Scene ******************************
	 * Manages lifetimes, processing and drawing order of game objects.
	 */
	class Scene
	{
	public:
		Scene();
		~Scene();

		// Takes ownership of a dynamically allocated _actor.
		void Add(Actor& _actor);

		void Process(float _deltaTime);
		void Draw(sf::RenderWindow& _window) const;

		// Removes destroyed Actors and associated components.
		void CleanUp();
		// Registers Actors from known factories.
		void RegisterActors();
	private:
		void Register(Component& _component);

		std::vector < std::unique_ptr<Actor>> m_actors;
		std::vector<std::reference_wrapper<DrawComponent>> m_drawComponents;
		std::vector<std::reference_wrapper<ProcessComponent>> m_processComponents;
		std::vector<std::reference_wrapper<FactoryComponent>> m_factoryComponents;

		b2World m_physicsWorld;
	};
}
#pragma once

#include <vector>
#include <memory>
#include <functional>

namespace Game {

	class Actor;
	class Component;
	class DrawComponent;
	class ProcessComponent;

	/* Scene ******************************
	 * Manages lifetimes, processing and drawing order of game objects.
	 */
	class Scene
	{
	public:
		~Scene();

		// Takes ownership of a dynamically allocated _actor.
		void Add(Actor& _actor);

		void Process(float _deltaTime);
		void Draw() const;
		void CleanUp();
	private:
		void Register(Component& _component);

		std::vector < std::unique_ptr<Actor>> m_actors;
		std::vector<std::reference_wrapper<DrawComponent>> m_drawComponents;
		std::vector<std::reference_wrapper<ProcessComponent>> m_processComponents;
	};
}
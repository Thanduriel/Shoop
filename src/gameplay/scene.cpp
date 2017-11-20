#include <algorithm>

#include "scene.hpp"
#include "core/actor.hpp"
#include "core/component.hpp"

namespace Game {

	// destructor is defined here so that unique_ptr can find Actor::~Actor
	Scene::~Scene()
	{}

	void Scene::Add(Actor& _actor)
	{
		m_actors.emplace_back(&_actor);
		for (auto& component : _actor.GetComponents())
			Register(component);
	}

	// **************************************************************** //
	void Scene::Process(float _deltaTime)
	{

	}

	void Scene::Draw() const
	{

	}

	// **************************************************************** //
	template <typename T>
	void RemoveDestroyed(std::vector<T>& _container)
	{
		auto it = std::remove_if(_container.begin(), _container.end(), [](const T& _component)
		{
			return _component.get().GetActor().IsDestroyed();
		});
		_container.erase(it, _container.end());
	}

	void Scene::CleanUp()
	{
		// unregister components
		RemoveDestroyed(m_processComponents);
		RemoveDestroyed(m_drawComponents);

		// actual destruction is last
		auto it = std::remove_if(m_actors.begin(), m_actors.end(), [](const std::unique_ptr<Actor>& _actor)
		{
			return _actor->IsDestroyed();
		});
		m_actors.erase(it, m_actors.end());
	}

	// **************************************************************** //
	void Scene::Register(Component& _component)
	{
		switch (_component.GetType())
		{
		case Component::Type::Process: 
			m_processComponents.emplace_back(static_cast<ProcessComponent&>(_component));
			break;
		case Component::Type::Draw:
			m_drawComponents.emplace_back(static_cast<DrawComponent&>(_component));
		}
	}
}
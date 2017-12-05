#include <algorithm>

#include "scene.hpp"
#include "core/actor.hpp"
#include "core/component.hpp"
#include "gameplay/elements/factory.hpp"
#include "utils/assert.hpp"
#include "gameplay/elements/physicscomponent.hpp"

namespace Game {

	Scene::Scene()
		: m_physicsWorld(Math::Vec2(0.f, -9.81f))
	{
		Details::PhysicsWorldWrapper::m_world = &m_physicsWorld;
	}

	// destructor is defined here so that unique_ptr can find Actor::~Actor
	Scene::~Scene()
	{
		Details::PhysicsWorldWrapper::m_world = nullptr;
	}

	void Scene::Add(Actor& _actor)
	{
		m_actors.emplace_back(&_actor);
		for (auto& component : _actor.GetComponents())
			Register(component);
	}

	// **************************************************************** //
	void Scene::Process(float _deltaTime)
	{
		for (ProcessComponent& component : m_processComponents)
			component.Process(_deltaTime);

		m_physicsWorld.Step(_deltaTime, 8, 8);
	}

	void Scene::Draw(sf::RenderWindow& _window) const
	{
		for (auto& component : m_drawComponents)
			component.get().Draw(_window);
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

	void Scene::RegisterActors()
	{
		// collect actors created by all known factories
		// it is possible that new factories are added, making any iterator invalid
		const size_t size = m_factoryComponents.size();
		for (size_t i = 0; i < size; ++i)
		{
			auto& container = m_factoryComponents[i].get().m_createdActors;
			while (container.size())
			{
				Add(*container.back().release());
				container.pop_back();
			}
		}
	}

	// **************************************************************** //
	void Scene::Register(Component& _component)
	{
		switch (_component.GetType())
		{
		case Component::Type::Process: 
			if(static_cast<ProcessComponent&>(_component).CanTick())
				m_processComponents.emplace_back(static_cast<ProcessComponent&>(_component));
			break;
		case Component::Type::Draw:
			m_drawComponents.emplace_back(static_cast<DrawComponent&>(_component));
			break;
		case Component::Type::Factory:
			m_factoryComponents.emplace_back(static_cast<FactoryComponent&>(_component));
			break;
		default:
			Assert(false, "This component type is unknown.");
			break;
		}
	}
}
#include <algorithm>

#include "scene.hpp"
#include "core/actor.hpp"
#include "core/component.hpp"
#include "gameplay/elements/factory.hpp"
#include "utils/assert.hpp"
#include "gameplay/elements/physicscomponent.hpp"

namespace Game {
	// the contact listener does not have any state.
	ContactListener g_contactListener;

	Scene::Scene()
		: m_physicsWorld(Math::Vec2(0.f, -9.81f))
	{
		m_physicsWorld.SetContactListener(&g_contactListener);
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
		for (auto component : _actor.GetComponents())
			Register(*component);
	}

	// **************************************************************** //
	void Scene::Process(float _deltaTime)
	{
		m_physicsWorld.Step(_deltaTime, 8, 3);

		for (ProcessComponent& component : m_processComponents)
			component.Process(_deltaTime);
	}

	void Scene::Draw(sf::RenderWindow& _window) const
	{
		for (auto& components : m_drawComponents)
			for(auto component : components)
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
		for(auto& components : m_drawComponents) RemoveDestroyed(components);

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
		if (!_component.ShouldRegister()) return;
		switch (_component.GetType())
		{
		case Component::Type::Process: 
			m_processComponents.emplace_back(static_cast<ProcessComponent&>(_component));
			break;
		case Component::Type::Draw:
		{
			DrawComponent& component = static_cast<DrawComponent&>(_component);
			m_drawComponents[static_cast<size_t>(component.GetDrawingOrder())].emplace_back(component);
			break;
		}
		case Component::Type::Factory:
			m_factoryComponents.emplace_back(static_cast<FactoryComponent&>(_component));
			break;
		default:
			Assert(false, "This component type is unknown.");
			break;
		}
	}
}
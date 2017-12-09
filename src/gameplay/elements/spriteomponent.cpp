#include "spriteomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "SFML/Graphics.hpp"
#include "math/helpers.hpp"
#include "graphics/device.hpp"

namespace Game {

	using namespace Graphics;
	using namespace Math;

	SpriteComponent::SpriteComponent(Actor& _actor, const sf::Texture& _texture, Math::Vec2 _size, Math::Vec2 _origin)
		: DrawComponent(_actor),
		m_sprite(_texture)
	{
		const Math::Vec2 size(_texture.getSize());

		Vec2 ratio = Device::GetWorldToScreenFactor() * _size / size;
		m_baseScale = ratio / Device::GetWorldToScreenFactor();
		m_sprite.setScale(Vec2(1.f/50.f));

		m_sprite.setOrigin(Math::Vec2(size.x * _origin.x, size.y * _origin.y));
	}

	void SpriteComponent::Draw(sf::RenderWindow& _window)
	{
		using namespace Math;
		const Transformation transform = m_actor.Transform(*this);
		
		Math::Vec2 p = transform.GetPosition();
		m_sprite.setPosition(Device::ToScreenSpace(p));
		m_sprite.setScale(m_baseScale * transform.GetScale());
		m_sprite.setRotation(-Math::ToDegree(transform.GetRotation()));

		_window.draw(m_sprite);
	}


}
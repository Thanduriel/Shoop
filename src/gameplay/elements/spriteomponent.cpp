#include "spriteomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "SFML/Graphics.hpp"
#include "math/helpers.hpp"

namespace Game {

	SpriteComponent::SpriteComponent(Actor& _actor, const sf::Texture& _texture, Math::Vec2 _origin)
		: DrawComponent(_actor),
		m_sprite(_texture)
	{
		const auto size = _texture.getSize();
		m_sprite.setOrigin(Math::Vec2(size.x * _origin.x, size.y * _origin.y));
	}

	void SpriteComponent::Draw(sf::RenderWindow& _window)
	{
		using namespace Math;
		const Transformation transform = m_actor.Transform(*this);
		
		m_sprite.setPosition(transform.GetPosition());
		m_sprite.setScale(transform.GetScale());
		m_sprite.setRotation(Math::ToDegree(transform.GetRotation()));

		_window.draw(m_sprite);
	}


}
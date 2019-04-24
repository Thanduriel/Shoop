#include "spriteomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "SFML/Graphics.hpp"
#include "math/helpers.hpp"
#include "graphics/device.hpp"

namespace Game {

	using namespace Graphics;
	using namespace Math;

	SpriteComponent::SpriteComponent(Actor& _actor, const sf::Texture& _texture, Math::Vec2 _size, 
		Math::Vec2 _origin, DrawingOrder _drawingOrder)
		: DrawComponent(_actor, _drawingOrder),
		m_sprite(_texture)
	{
		const Math::Vec2 size(_texture.getSize());

		const Vec2 ratio = _size / size;
		m_baseScale = ratio;
	//	m_sprite.setScale(m_baseScale);

		m_sprite.setOrigin(Math::Vec2(size.x * _origin.x, size.y * _origin.y));
	}

	SpriteComponent::SpriteComponent(Actor& _actor, const sf::Texture& _texture, float _sizeX,
		Math::Vec2 _origin, DrawingOrder _drawingOrder)
		: SpriteComponent(_actor, _texture, Math::Vec2(_sizeX, 
			_sizeX * static_cast<float>(_texture.getSize().y) / _texture.getSize().x), _origin, _drawingOrder)
	{
		
	}

	void SpriteComponent::Draw(sf::RenderWindow& _window)
	{
		using namespace Math;
		const Transformation transform = m_actor->Transform(*this);
		
		Math::Vec2 p = transform.GetPosition();
		m_sprite.setPosition(Device::ToScreenSpace(p));
		m_sprite.setScale(m_baseScale * transform.GetScale());
		m_sprite.setRotation(-Math::ToDegree(transform.GetRotation()));

		_window.draw(m_sprite);
	}

	void SpriteComponent::FlipX(bool _flip)
	{
		m_baseScale.x = std::abs(m_baseScale.x) * (_flip ? -1.f : 1.f);
	}

	void SpriteComponent::FlipY(bool _flip)
	{
		m_baseScale.y = std::abs(m_baseScale.y) * (_flip ? -1.f : 1.f);
	}

	void GlobalSpriteComponent::Draw(sf::RenderWindow& _window)
	{
		using namespace Math;
		const Transformation& transform = *this;

		Math::Vec2 p = transform.GetPosition();
		m_sprite.setPosition(Device::ToScreenSpace(p));
		m_sprite.setScale(m_baseScale * transform.GetScale());
		m_sprite.setRotation(-Math::ToDegree(transform.GetRotation()));

		_window.draw(m_sprite);
	}

}
#pragma once

#include "gameplay/core/component.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "math/transformation.hpp"
#include "SFML/Graphics.hpp"

namespace Game {

	class SpriteComponent : public DrawComponent, public Math::Transformation
	{
	public:
		SpriteComponent(Actor& _actor, const sf::Texture& _texture, 
			Math::Vec2 _size,
			Math::Vec2 _origin = Math::Vec2(0.5f),
			DrawingOrder _drawingOrder = DrawingOrder::Middle);

		// Computes the size y to preserve the aspect ratio.
		SpriteComponent(Actor& _actor, const sf::Texture& _texture,
			float _sizeX,
			Math::Vec2 _origin = Math::Vec2(0.5f),
			DrawingOrder _drawingOrder = DrawingOrder::Middle);


		void Draw(sf::RenderWindow& _window) override;

		// Direct access to the underlying sprite.
		sf::Sprite& GetSprite() { return m_sprite; }
	protected:
		sf::Sprite m_sprite;
		Math::Vec2 m_baseScale;
	};

	// Uses its transformation as global, allowing it to be attached to a physics body.
	class GlobalSpriteComponent : public SpriteComponent
	{
	public:
		using SpriteComponent::SpriteComponent;

		void Draw(sf::RenderWindow& _window) override;
	};

	using SpriteActor = SingleComponentActor<SpriteComponent>;
}
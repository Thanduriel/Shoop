#pragma once

#include "gameplay/core/component.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "math/transformation.hpp"
#include "SFML/Graphics.hpp"

namespace Game {

	class SpriteComponent : public DrawComponent, public Math::Transformation
	{
	public:
		SpriteComponent(Actor& _actor, const sf::Texture& _texture, Math::Vec2 _origin = Math::Vec2(0.5f));

		void Draw(sf::RenderWindow& _window) override;
	private:
		sf::Sprite m_sprite;
	};

	using SpriteActor = SingleComponentActor<SpriteComponent>;
}
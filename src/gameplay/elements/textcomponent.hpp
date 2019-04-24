#pragma once

#include "gameplay/core/component.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "math/transformation.hpp"
#include "SFML/Graphics.hpp"

namespace Game {

	class TextComponent : public DrawComponent, public Math::Transformation
	{
	public:
		TextComponent(Actor& _actor, const sf::Font& _font,
			unsigned _fontSize,
			Math::Vec2 _origin = Math::Vec2(0.5f, 0.f),
			DrawingOrder _drawingOrder = DrawingOrder::Front);

		void Draw(sf::RenderWindow& _window) override;

		void SetText(const std::string& _text);
	private:
		void UpdateScale();

		Math::Vec2 m_baseSize;
		Math::Vec2 m_baseScale;
		Math::Vec2 m_origin;
		sf::Text m_text;
	};
}
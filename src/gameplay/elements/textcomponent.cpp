#include "textcomponent.hpp"
#include "graphics/device.hpp"
#include "math/helpers.hpp"

namespace Game {

	TextComponent::TextComponent(Actor& _actor, const sf::Font& _font,
		unsigned _fontSize,
		Math::Vec2 _origin,
		DrawingOrder _drawingOrder)
		: DrawComponent(_actor, _drawingOrder),
		m_baseScale(1.f / Graphics::Device::GetWorldToScreenFactor()),
		m_origin(_origin)
	{
		m_text.setCharacterSize(_fontSize);
		m_text.setOrigin(_origin);
		UpdateScale();
		m_text.setFont(_font);
	}

	void TextComponent::Draw(sf::RenderWindow& _window)
	{
		using namespace Math;
		using namespace Graphics;
		const Transformation transform = m_actor->Transform(*this);

		const Math::Vec2 p = transform.GetPosition();
		m_text.setPosition(Device::ToScreenSpace(p));
		m_text.setScale(m_baseScale * transform.GetScale());
		m_text.setRotation(-Math::ToDegree(transform.GetRotation()));

		_window.draw(m_text);
	}

	void TextComponent::SetText(const std::string& _text)
	{
		m_text.setString(_text);
		UpdateScale();
	}

	void TextComponent::UpdateScale()
	{
		sf::FloatRect textRect = m_text.getLocalBounds();
		m_text.setOrigin(textRect.left + textRect.width * m_origin.x,
			textRect.top + textRect.height * m_origin.y);
		
	//	m_baseScale = m_baseSize / Math::Vec2(rect.width, rect.height);
	}
}
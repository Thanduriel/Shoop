#pragma once

#include "math/vector.hpp"
#include "utils/config.hpp"

namespace sf {
	class RenderWindow;
}

namespace Graphics {

	class Device
	{
	public:
		static void Init(int _sizeX, int _sizeY, bool _fullScreen);
		static void InitDummy(int _sizeX, int _sizeY);
		static void Close();

		static void Resize(int _sizeX, int _sizeY, bool _fullScreen);

		// Transforms a logical position to screen space.
		static Math::Vec2 ToScreenSpace(Math::Vec2 _position);

		// Transforms a screen position to world space
		//Math::Vec2 ToWorldSpace(Math::Vec2 _position);
		static float GetWorldToScreenFactor();

		static sf::RenderWindow& GetWindow() { return *m_window; }
		static Math::Vec2 GetSizeWorldSpace();

		static void SetView(bool _worldView);
	private:
		Device() = delete;
		Device(const Device&) = delete;

		static sf::RenderWindow* m_window;
		static bool m_isDummy;
		static Math::Vec2U m_size;
	};

	const extern Utils::ConfigSection::Initializer<int, 4> VideoSettings;
}
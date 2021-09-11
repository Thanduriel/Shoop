#include "map.hpp"
#include "gameplay/scene.hpp"
#include "gameplay/elements/wall.hpp"
#include "graphics/device.hpp"
#include "gameplay/elements/spriteomponent.hpp"
#include "resources.hpp"
#include "generators/random.hpp"
#include "gameplay/elements/ground.hpp"
#include "gameplay/elements/lifetimecomponent.hpp"

namespace Game {

	using namespace Math;

	Map::Map(Scene& _scene)
		: m_factory(*this)
	{
		const Vec2 worldSize = Graphics::Device::GetSizeWorldSpace();
		SpriteActor* act = new SpriteActor(worldSize*0.5f, 
			Resources::Load<sf::Texture>("backgroundClassicBack"), 
			worldSize,
			Vec2(0.5f),
			DrawComponent::DrawingOrder::Background);
		_scene.Add(*act);
		act = new SpriteActor(worldSize*0.5f,
			Resources::Load<sf::Texture>("backgroundClassicFront"),
			worldSize,
			Vec2(0.5f),
			DrawComponent::DrawingOrder::Front);
		_scene.Add(*act);

		// factor to use original size values
		const Vec2 scale = worldSize / Vec2(80.f, 60.f);

		// flags
		const int numFlags = 4;
		bool colorToggle = false;
		const Vec2 start = Vec2(0.90972f, 0.466f) * worldSize;
		const Vec2 end = Vec2(1.f - 0.90972f, 0.466f) * worldSize;
		for (int i = 0; i < numFlags; ++i)
		{
			colorToggle = !colorToggle;
			SpriteActor* act = new SpriteActor(start + (end-start) * (static_cast<float>(i) / (numFlags-1)),
				Resources::Load<sf::Texture>(colorToggle ? "flagGreen" : "flagRed"),
				1.5f,
				Vec2(0.5f),
				DrawComponent::DrawingOrder::Background);
			_scene.Add(*act);
			act->GetComponent().FlipX(!colorToggle);
		}
		m_winnerFlagPos = Vec2(0.5f) * worldSize;

		MakeBorders(_scene);


		using namespace Generators;
		Generators::CurveGen generator;
		Curve curve{Vec2(0.f, 6.f), Vec2(5.f, 6.f),  // Vec2(4.f, 6.f)
			Vec2(24.f, 3.f), Vec2(40.f, 4.f), Vec2(56.f, 2.f), 
			Vec2(72.f, 6.f), Vec2(80.f, 6.f)};

		for (Vec2& v : curve)
			v *= scale;

		// the original is at y = 16.f but the ground is to high then
		_scene.Add(*new GroundPlane(generator.SampleSmooth<BlendCos>(curve, 26), Vec2(0.f, 15.f) * scale));

		m_spawnPositions = { Vec2(5.0f, 50.0f) * scale, Vec2(75.0f, 50.0f) * scale };
	}

	void Map::ShowWinnerFlags(int _color, float _duration)
	{
		class WinnerFlag : public Actor
		{
		public:
			WinnerFlag(Vec2 _position, int _color, float _duration)
				: Actor(_position),
				m_sprite(THISACTOR, Resources::Load<sf::Texture>(_color == -1 ? "flagGray" :
					(_color == 0 ? "flagGreen" : "flagRed")),
					2.5f,
					Vec2(0.5f),
					DrawComponent::DrawingOrder::Background),
				m_lifeTime(THISACTOR, _duration)
			{
			}
		private:
			SpriteComponent m_sprite;
			LifetimeComponent m_lifeTime;
		};
		WinnerFlag* act = new WinnerFlag(m_winnerFlagPos, _color, _duration);
		m_factory.Add(*act);
	}

	void Map::MakeBorders(Scene& _scene, BorderFlags _borders)
	{
		const Vec2 worldSize = Graphics::Device::GetSizeWorldSpace();

		constexpr PhysicsInfo::Flags flags = 0;

		if (_borders & Border::Bot)
			_scene.Add(*new Wall(worldSize * Vec2(0.5f, -0.05f), worldSize * Vec2(1.f, 0.1f), flags | PhysicsInfo::IsGround));
		if (_borders & Border::Top)
			_scene.Add(*new Wall(worldSize * Vec2(0.5f, 1.05f), worldSize * Vec2(1.f, 0.1f), flags));
		if (_borders & Border::Left)
			_scene.Add(*new Wall(worldSize * Vec2(-0.05f, 0.5f), worldSize * Vec2(0.1f, 1.f), flags));
		if (_borders & Border::Right)
			_scene.Add(*new Wall(worldSize * Vec2(1.05f, 0.5f), worldSize * Vec2(0.1f, 1.f), flags));
	}
}
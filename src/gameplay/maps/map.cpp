#include "map.hpp"
#include "gameplay/scene.hpp"
#include "gameplay/elements/wall.hpp"
#include "graphics/device.hpp"
#include "gameplay/elements/spriteomponent.hpp"
#include "resources.hpp"
#include "generators/random.hpp"
#include "gameplay/elements/ground.hpp"

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

		MakeBorders(_scene);


		using namespace Generators;
		Generators::CurveGen generator;
		Curve curve{Vec2(0.f, 6.f), Vec2(4.f, 6.f), 
			Vec2(24.f, 3.f), Vec2(40.f, 4.f), Vec2(56.f, 2.f), 
			Vec2(72.f, 6.f), Vec2(80.f, 6.f)};

		const Vec2 scale = worldSize / Vec2(80.f, 60.f);
		for (Vec2& v : curve)
			v *= scale;

		// the original is at y = 16.f but the ground is to high then
		_scene.Add(*new GroundPlane(generator.SampleSmooth<BlendCos>(curve, 26), Vec2(0.f, 15.f) * scale));

		m_spawnPositions = { Vec2(5.0f, 50.0f) * scale, Vec2(75.0f, 50.0f) * scale };
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
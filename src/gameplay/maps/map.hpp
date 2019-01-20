#pragma once

namespace Game {

	class Scene;

	class Map
	{
	public:
		Map(Scene& _scene);
		
	private:
		enum Border {
			Left = 0x1,
			Right = 0x2,
			Top = 0x4,
			Bot = 0x8
		};
		using BorderFlags = unsigned;

		void MakeBorders(Scene& _scene, BorderFlags _borders = Left | Right | Top | Bot);
	};
}
#pragma once
#include "../../../Math/Tensor/Tensor.h"
namespace Direct2D
{
	struct Alignment
	{
		enum class Horizontal { left, right, center };
		enum class Vertical { top, bottom, center };

		Horizontal horizontal{ Horizontal::left };
		Vertical vertical{ Vertical::top };

		constexpr Math::float2 getTopLeftPosition(Math::float2 position, const Math::float2& size)const noexcept
		{
			switch (horizontal)
			{
				case Horizontal::left:break;
				case Horizontal::right:position[0] -= size[0]; break;
				case Horizontal::center:position[0] -= size[0] * 0.5f;
			}
			switch (vertical)
			{
				case Vertical::top:break;
				case Vertical::bottom:position[1] -= size[1]; break;
				case Vertical::center:position[1] -= size[1] * 0.5f; break;
			}
			return position;
		}

		constexpr bool operator==(const Alignment&)const noexcept = default;
	};
}
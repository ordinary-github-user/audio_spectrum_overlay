#pragma once
#include "../Math/Tensor/Tensor.h"
#include "WinAPI/User32.h"

namespace Windows
{
	//SM_CXSCREEN
	//SM_CYSCREEN
	[[nodiscard]]int getScreenWidth() { return User32::GetSystemMetrics(SM_CXVIRTUALSCREEN); }
	[[nodiscard]]int getScreenHeight() { return User32::GetSystemMetrics(SM_CYVIRTUALSCREEN); }
	[[nodiscard]]Math::Vector2<i32> getScreenSize() { return { getScreenWidth(),getScreenHeight() }; }

	[[nodiscard]]int getFirstScreenWidth() { return User32::GetSystemMetrics(SM_CXSCREEN); }
	[[nodiscard]]int getFirstScreenHeight() { return User32::GetSystemMetrics(SM_CYSCREEN); }
	[[nodiscard]]Math::Vector2<i32> getFirstScreenSize() { return { getFirstScreenWidth(),getFirstScreenHeight() }; }
}
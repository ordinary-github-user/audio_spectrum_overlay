#pragma once
#include <bit>
#include "../../Util/Algorithm/Find.h"
#include "../../Util/Concepts/Sig.h"
#include "../../Util/DataStructures/TString.h"
#include "../../Util/Macro/FlipBool.h"
#include "../../Util/Pattern.h"
#include "../../Util/Random/RNG.h"
#include "../Message.h"
#include "../WinAPI/Win32u.h"
#include "../Window.h"
#include "../WindowClass.h"
#include "Keycode.h"
struct LockStates
{
	bool capLock;
	bool numLock;
	bool scrollLock;
	void init()noexcept
	{
		capLock = Win32u::NtUserGetKeyState(UNDERLYING(Vkey::cap)) & 1;//capLock = User32::GetKeyState(Vkey::cap) & 1;
		numLock = Win32u::NtUserGetKeyState(UNDERLYING(Vkey::numlock)) & 1;//numLock = User32::GetKeyState(Vkey::numlock) & 1;
		scrollLock = Win32u::NtUserGetKeyState(UNDERLYING(Vkey::scroll_lock)) & 1;//scrollLock = User32::GetKeyState(Vkey::scroll_lock) & 1;
	}
};

template<usize Size>
StaticString<c16, Size> rawInputDeviceName(HANDLE hDevice)noexcept
{
	StaticString<c16, Size> result;
	u32 size{ Size };
	u32 retSize{ Win32u::NtUserGetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, result.data(), &size) };
	result.modify_size(static_cast<typename StaticString<c16, Size>::Index>((retSize > Size) ? u32{} : retSize));
	while(result.size() && result.back() == '\0')
		result.modify_size(result.size() - 1);
	return result;
}

ENUM(HidUsageID, u16)
{
	pointer = 1,
	mouse = 2,
	joystick = 4,
	gamepad = 5,
	keyboard = 6,
	keypad = 7,
	multi_axis_controller = 8,
};

ENUM(HidUsagePage, u16)
{
	generic = 1,
	game = 5,
	led = 8,
	button = 9
};

struct KeyInput
{
private:
	bool m_isPressed{};
public:
	void updateDown()noexcept { m_isPressed = true; }
	void updateUp()noexcept { m_isPressed = false; }
	bool isPressed()const noexcept { return m_isPressed; }
	template<class Stream>
	friend Stream& operator<<(Stream& os, const KeyInput& key)noexcept
	{
		os << key.isPressed();
		return os;
	}
};

template<bool>struct RawInputFakeInputImpl;
template<>struct RawInputFakeInputImpl<1> {};
template<>struct RawInputFakeInputImpl<0> 
{
	inline static GrowableArray<HANDLE> s_fakeDevices{};
	inline static GrowableArray<HANDLE> s_realDevices{};
};

template<bool> struct RawInputLockStateImpl;
template<>struct RawInputLockStateImpl<0> {};
template<>struct RawInputLockStateImpl<1>
{
	inline static LockStates s_lockStates{};
};

template<bool> struct RawInputMouseMoveImpl;
template<>struct RawInputMouseMoveImpl<0> {};
template<>struct RawInputMouseMoveImpl<1>
{
protected:
	inline static Math::Vector2<i32> s_mouseMove{};
public:
	static const Math::Vector2<i32>& mouseMove() noexcept { return s_mouseMove; }
};

template<bool> struct RawInputKeyboardImpl;
template<>struct RawInputKeyboardImpl<0> {};
template<>struct RawInputKeyboardImpl<1>
{
	inline static std::array<KeyInput, []static
	{
		usize result{};
		for (const KeyId key : Enum<KeyId>::entries)
			if (UNDERLYING(key) > result)
				result = UNDERLYING(key);
		return result + 1;
	}()> s_keys{};
};

struct RawInputConfig
{
	bool keyboard = true;
	bool mouse = true;
	bool mouseMove = true;
	bool lockState = false;
	bool allowFakeInput = false;
	consteval bool valid()const noexcept { return (keyboard || mouse) && (!lockState || keyboard) && (!mouseMove || mouse); }
};

template<RawInputConfig cfg = RawInputConfig{}>
requires(cfg.valid())
struct RawInput: RawInputFakeInputImpl<cfg.allowFakeInput>, RawInputLockStateImpl<cfg.lockState>, RawInputMouseMoveImpl<cfg.mouseMove>, RawInputKeyboardImpl<cfg.keyboard || cfg.mouse>
{
private:
	using RawInputFakeInput = RawInputFakeInputImpl<cfg.allowFakeInput>;
	using RawInputLockState = RawInputLockStateImpl<cfg.lockState>;
	using RawInputMouseMove = RawInputMouseMoveImpl<cfg.mouseMove>;
	using RawInputKeyboard  = RawInputKeyboardImpl <cfg.keyboard || cfg.mouse>;
	static constexpr usize deviceCount{ cfg.mouse + cfg.keyboard }; STATIC_ASSERT(deviceCount > 0);
	static constexpr RAWINPUTDEVICE makeDevice(Windows::Window window, HidUsageID use, u32 flags)noexcept
	{
		return 
		{
			.usUsagePage{UNDERLYING(HidUsagePage::generic)},
			.usUsage{UNDERLYING(use)},
			.dwFlags{flags},
			.hwndTarget{window.handle()}
		};
	}
	static bool registerDevices(const std::array<RAWINPUTDEVICE, deviceCount>& devices)noexcept { return 0 != Win32u::NtUserRegisterRawInputDevices(devices.data(), deviceCount, sizeof(RAWINPUTDEVICE)); }
public:
	static bool dispose()noexcept
	{
		static constexpr u32 FlagsDispose{ RIDEV_REMOVE };
		constexpr std::array<RAWINPUTDEVICE, deviceCount> devices
		{
			[] static->std::array<RAWINPUTDEVICE, deviceCount>
			{
				if constexpr (cfg.mouse && cfg.keyboard)
					return {
						makeDevice(0, HidUsageID::mouse, FlagsDispose),
						makeDevice(0, HidUsageID::keyboard, FlagsDispose)
					};
				else if constexpr (cfg.mouse)
					return {makeDevice(0, HidUsageID::mouse, FlagsDispose)};
				else if constexpr (cfg.keyboard)
					return {makeDevice(0, HidUsageID::keyboard, FlagsDispose)};
			}()
		};
		return registerDevices(devices);
	}

	static KeyInput& key(KeyId keyId) noexcept { return RawInputKeyboard::s_keys[UNDERLYING(keyId)]; }

	static void beginFrame() noexcept {}
	static void endFrame() noexcept 
	{
		if constexpr (cfg.mouseMove)
			RawInputMouseMove::s_mouseMove = {};
		if constexpr (cfg.mouse)
		{
			if (key(KeyId::wheel_up).isPressed())
				key(KeyId::wheel_up).updateUp();
			if (key(KeyId::wheel_down).isPressed())
				key(KeyId::wheel_down).updateUp();
		}
	}

	RawInput(HWND hwnd)noexcept { if (!attach(hwnd))CRASH(); }

	static bool attach(Windows::Window window)noexcept
	{
		static constexpr u32 FlagsInit{ RIDEV_NOLEGACY | RIDEV_INPUTSINK };
		const std::array<RAWINPUTDEVICE, deviceCount> devices
		{
			[window] ->std::array<RAWINPUTDEVICE, deviceCount>
			{
				if constexpr (cfg.mouse && cfg.keyboard)
					return {
						makeDevice(window,HidUsageID::mouse,FlagsInit),
						makeDevice(window,HidUsageID::keyboard,FlagsInit)
					};
				else if constexpr (cfg.mouse)
					return {makeDevice(window, HidUsageID::mouse, FlagsInit)};
				else if constexpr (cfg.keyboard)
					return {makeDevice(window, HidUsageID::keyboard, FlagsInit)};
			}()
		};
		if constexpr (cfg.lockState)
			RawInputLockState::s_lockStates.init();
		return registerDevices(devices);
	}
	static bool detach() noexcept { return dispose(); }

	static bool capsLock() noexcept requires(cfg.lockState) { return RawInputLockState::s_lockStates.capLock; }
	static bool numLock() noexcept requires(cfg.lockState) { return RawInputLockState::s_lockStates.numLock; }
	static bool scrollLock() noexcept requires(cfg.lockState) { return RawInputLockState::s_lockStates.scrollLock; }

	template<u32 msg>
	requires(msg == WM_INPUT)
	static void processMsg(LPARAM lParam)noexcept
	{
		RAWINPUT rawInput;
		u32 sizeInBytes{ sizeof(RAWINPUT) };
		if (const u32 result{ Win32u::NtUserGetRawInputData(std::bit_cast<HRAWINPUT>(lParam), RID_INPUT, &rawInput, &sizeInBytes, sizeof(RAWINPUTHEADER)) }; result == 0 || result == (u32)-1)
			return;
		const tagRAWINPUTHEADER& header{ rawInput.header };
		if constexpr (!cfg.allowFakeInput)
			static_assert(0);
		switch (header.dwType)
		{
			case RIM_TYPEKEYBOARD:
				if constexpr (cfg.keyboard)
				{
					const RAWKEYBOARD& rawKeyboard{ rawInput.data.keyboard };
					const u16 flags{ rawKeyboard.Flags };
					//LOG(static_cast<ScanCode>(rawKeyboard.MakeCode), ':', flags & RI_KEY_E0, ':', static_cast<Vkey>(rawKeyboard.VKey));
					const Opt<KeyId> vk{ translate(static_cast<ScanCode>(rawKeyboard.MakeCode), flags & RI_KEY_E0,static_cast<Vkey>(rawKeyboard.VKey)) };
					//const u8 vk{ScanCode::toVkeySyscall(rawKeyboard.MakeCode)};
					if (vk) [[likely]]
					{
						if (flags & RI_KEY_BREAK)
							key(*vk).updateUp();
						else
						{
							if constexpr (cfg.lockState)
							{
								if (!key(*vk).isPressed())
									switch (*vk)
									{
										break; case KeyId::scroll_lock:	FLIP_BOOL(RawInputLockState::s_lockStates.scrollLock);
										break; case KeyId::numlock:		FLIP_BOOL(RawInputLockState::s_lockStates.numLock);
										break; case KeyId::cap:			FLIP_BOOL(RawInputLockState::s_lockStates.capLock);
										break; default: {}
									}
							}
							key(*vk).updateDown();
						}
					}
				}
				break;
			case RIM_TYPEMOUSE:
				if constexpr (cfg.mouse)
				{
					const RAWMOUSE& rawMouse{ rawInput.data.mouse };
					if (rawMouse.usFlags == MOUSE_MOVE_RELATIVE)
					{
						if constexpr (cfg.mouseMove)RawInputMouseMove::s_mouseMove += Math::Vector2<i32>{ rawMouse.lLastX, rawMouse.lLastY };
						if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)		key(KeyId::mouse_left).updateDown();
						if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)		key(KeyId::mouse_left).updateUp();
						if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)	key(KeyId::mouse_right).updateDown();
						if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)		key(KeyId::mouse_right).updateUp();
						if (rawMouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)	key(KeyId::mouse_middle).updateDown();
						if (rawMouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)		key(KeyId::mouse_middle).updateUp();
						if (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)		key(KeyId::xButton1).updateDown();
						if (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)			key(KeyId::xButton1).updateUp();
						if (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)		key(KeyId::xButton2).updateDown();
						if (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)			key(KeyId::xButton2).updateUp();
						if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL)				key(std::bit_cast<i16>(rawMouse.usButtonData) > 0 ? KeyId::wheel_up : KeyId::wheel_down).updateDown();
					}
					else
					{
						//Absolute Movement
						//VM only
					}
				}
				break;
		}
	}

	static void peekMsgLoop(Windows::Window hwnd, $SigAnyOf<void(), IterationControl(), void(const MSG), IterationControl(const MSG)> auto&& action)noexcept
	{
		MSG msg;
		for(;;) [[likely]]
		{
			while (Windows::peekMessage(msg, hwnd.handle()))
				if (msg.message == WM_INPUT)
				{
					processMsg<WM_INPUT>(msg.lParam);
					NTDLL::NtdllDefWindowProc_W(msg.hwnd, WM_INPUT, msg.wParam, msg.lParam);
				}
			beginFrame();
			/* */if constexpr ($Sig<decltype(action), void()                     >)action();
			else if constexpr ($Sig<decltype(action), void(const MSG)            >)action(msg);
			else if constexpr ($Sig<decltype(action), IterationControl()         >) { if (IterationControl::stop == action())break; }
			else if constexpr ($Sig<decltype(action), IterationControl(const MSG)>) { if (IterationControl::stop == action(msg))break; }
			endFrame();
		}
	}

	static void getMsgLoop(Windows::Window hwnd, $SigAnyOf<void(), IterationControl(), void(const MSG), IterationControl(const MSG)> auto&& action)noexcept
	{
		MSG msg;
		while (Windows::getMessage(msg, hwnd.handle())) [[likely]]
		{
			if (msg.message == WM_INPUT)
			{
				processMsg<WM_INPUT>(msg.lParam);
				NTDLL::NtdllDefWindowProc_W(msg.hwnd, WM_INPUT, msg.wParam, msg.lParam);
			}
			beginFrame();
			/* */if constexpr ($Sig<decltype(action), void()                     >)action();
			else if constexpr ($Sig<decltype(action), void(const MSG)            >)action(msg);
			else if constexpr ($Sig<decltype(action), IterationControl()         >) { if (IterationControl::stop == action())break; }
			else if constexpr ($Sig<decltype(action), IterationControl(const MSG)>) { if (IterationControl::stop == action(msg))break; }
			endFrame();
		}
	}

	static Opt<Windows::Window> attachToDummy()noexcept { return attachToDummy(randString16<8, 16>(), FixedString{ L"" }); }

	static Opt<Windows::Window> attachToDummy($StringOf<c16> auto&& className, $StringOf<c16> auto&& title)noexcept
	{
		Windows::WindowClassEx classEx{ className, NTDLL::NtdllDefWindowProc_W.address() };
		if (!classEx.reg())
			return {};
		const Windows::Window window{ Windows::Window::create({}, {}, className, title) };
		if (!attach(window)) [[unlikely]]
		{
			classEx.unreg();
			return {};
		}
		return window;
	}

	template<class Stream>
	static Stream& toStream(Stream& os)noexcept
	{
		if constexpr (cfg.mouseMove)
			os << TS("Mouse  Move = ") << RawInputMouseMove::mouseMove() << TS("       \n");
		if constexpr (cfg.keyboard)
		{
			if constexpr (cfg.lockState)
			{
				os << TS("Caps   Lock = ") << capsLock() << '\n';
				os << TS("Num    Lock = ") << numLock() << '\n';
				os << TS("Scroll Lock = ") << scrollLock() << '\n';
			}
			Fold::for_each<Enum<KeyId>::entries>([&](auto I)LAMBDA_INLINE
			{
				if (key(I).isPressed())
					os << enum_name<I()> << '\n';
			});
		}
		return os;
	}
	template<class Stream>
	friend Stream& operator<<(Stream& os, const RawInput&)noexcept { return toStream(os); }
};
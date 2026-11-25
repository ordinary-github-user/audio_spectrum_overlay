#pragma once
#include "../../Util/DataStructures/Optional.h"
#include "../../Util/EnumDefinition.h"
#include "../WinSock2_NoMinMax.h"

DEFINE_ENUM(KeyId, u16,
	(a, 'A'),
	(b, 'B'),
	(c, 'C'),
	(d, 'D'),
	(e, 'E'),
	(f, 'F'),
	(g, 'G'),
	(h, 'H'),
	(i, 'I'),
	(j, 'J'),
	(k, 'K'),
	(l, 'L'),
	(m, 'M'),
	(n, 'N'),
	(o, 'O'),
	(p, 'P'),
	(q, 'Q'),
	(r, 'R'),
	(s, 'S'),
	(t, 'T'),
	(u, 'U'),
	(v, 'V'),
	(w, 'W'),
	(x, 'X'),
	(y, 'Y'),
	(z, 'Z'),
	(_0, '0'),
	(_1, '1'),
	(_2, '2'),
	(_3, '3'),
	(_4, '4'),
	(_5, '5'),
	(_6, '6'),
	(_7, '7'),
	(_8, '8'),
	(_9, '9'),
	(mouse_left, VK_LBUTTON),
	(mouse_right, VK_RBUTTON),
	(mouse_middle, VK_MBUTTON),
	(xButton1, VK_XBUTTON1),
	(xButton2, VK_XBUTTON2),
	(numpad_0, VK_NUMPAD0),
	(numpad_1, VK_NUMPAD1),
	(numpad_2, VK_NUMPAD2),
	(numpad_3, VK_NUMPAD3),
	(numpad_4, VK_NUMPAD4),
	(numpad_5, VK_NUMPAD5),
	(numpad_6, VK_NUMPAD6),
	(numpad_7, VK_NUMPAD7),
	(numpad_8, VK_NUMPAD8),
	(numpad_9, VK_NUMPAD9),
	(numpad_mul, VK_MULTIPLY),
	(numpad_plus, VK_ADD),
	(numpad_minus, VK_SUBTRACT),
	(numpad_dec, VK_DECIMAL),
	(numpad_div, VK_DIVIDE),
	(numlock, VK_NUMLOCK),
	(f1, VK_F1),
	(f2, VK_F2),
	(f3, VK_F3),
	(f4, VK_F4),
	(f5, VK_F5),
	(f6, VK_F6),
	(f7, VK_F7),
	(f8, VK_F8),
	(f9, VK_F9),
	(f10, VK_F10),
	(f11, VK_F11),
	(f12, VK_F12),
	(f13, VK_F13),
	(f14, VK_F14),
	(f15, VK_F15),
	(f16, VK_F16),
	(f17, VK_F17),
	(f18, VK_F18),
	(f19, VK_F19),
	(f20, VK_F20),
	(f21, VK_F21),
	(f22, VK_F22),
	(f23, VK_F23),
	(f24, VK_F24),
	(win_left, VK_LWIN),
	(win_right, VK_RWIN),
	(shift_left, VK_LSHIFT),
	(shift_right, VK_RSHIFT),
	(control_left, VK_LCONTROL),
	(control_right, VK_RCONTROL),
	(alt_left, VK_LMENU),
	(alt_right, VK_RMENU),
	(esc, VK_ESCAPE),
	(back_tick, VK_OEM_3),
	(tab, VK_TAB),
	(cap, VK_CAPITAL),
	(snapshot, VK_SNAPSHOT),//printscreen
	(scroll_lock, VK_SCROLL),
	(pause_break, VK_PAUSE),
	(insert, VK_INSERT),
	(del, VK_DELETE),
	(home, VK_HOME),
	(end, VK_END),
	(page_up, VK_PRIOR),
	(page_down, VK_NEXT),
	(left, VK_LEFT),
	(right, VK_RIGHT),
	(up, VK_UP),
	(down, VK_DOWN),
	(minus, VK_OEM_MINUS),
	(equal, VK_OEM_PLUS),
	(back_slash, VK_OEM_5),
	(back_space, VK_BACK),
	(square_bracket_left, VK_OEM_4),
	(square_bracket_right, VK_OEM_6),
	(enter, VK_RETURN),
	(semicolon, VK_OEM_1),
	(quote, VK_OEM_7),
	(comma, VK_OEM_COMMA),
	(period, VK_OEM_PERIOD),
	(slash, VK_OEM_2),
	(apps, VK_APPS),//simulatedrightclick
	(space, VK_SPACE),
	(shift, VK_SHIFT),
	(control, VK_CONTROL),
	(alt, VK_MENU),
	(sleep, VK_SLEEP),
	(select, VK_SELECT),
	(print, VK_PRINT),
	(execute, VK_EXECUTE),
	(help, VK_HELP),
	(separator, VK_SEPARATOR),
	(numpad_equal, VK_OEM_NEC_EQUAL),
	(clear, VK_CLEAR),
	(numpad_enter, 0x100),
	(wheel_up, 0x101),
	(wheel_down, 0x102)
);
ENUM(Vkey, u8)
{
	a = 'A',
	b = 'B',
	c = 'C',
	d = 'D',
	e = 'E',
	f = 'F',
	g = 'G',
	h = 'H',
	i = 'I',
	j = 'J',
	k = 'K',
	l = 'L',
	m = 'M',
	n = 'N',
	o = 'O',
	p = 'P',
	q = 'Q',
	r = 'R',
	s = 'S',
	t = 'T',
	u = 'U',
	v = 'V',
	w = 'W',
	x = 'X',
	y = 'Y',
	z = 'Z',
	_0 = '0',
	_1 = '1',
	_2 = '2',
	_3 = '3',
	_4 = '4',
	_5 = '5',
	_6 = '6',
	_7 = '7',
	_8 = '8',
	_9 = '9',
	mouse_left = VK_LBUTTON,
	mouse_right = VK_RBUTTON,
	mouse_middle = VK_MBUTTON,
	xButton1 = VK_XBUTTON1,
	xButton2 = VK_XBUTTON2,
	numpad_0 = VK_NUMPAD0,
	numpad_1 = VK_NUMPAD1,
	numpad_2 = VK_NUMPAD2,
	numpad_3 = VK_NUMPAD3,
	numpad_4 = VK_NUMPAD4,
	numpad_5 = VK_NUMPAD5,
	numpad_6 = VK_NUMPAD6,
	numpad_7 = VK_NUMPAD7,
	numpad_8 = VK_NUMPAD8,
	numpad_9 = VK_NUMPAD9,
	numpad_mul = VK_MULTIPLY,
	numpad_plus = VK_ADD,
	numpad_minus = VK_SUBTRACT,
	numpad_dec = VK_DECIMAL,
	numpad_div = VK_DIVIDE,
	numlock = VK_NUMLOCK,
	f1 = VK_F1,
	f2 = VK_F2,
	f3 = VK_F3,
	f4 = VK_F4,
	f5 = VK_F5,
	f6 = VK_F6,
	f7 = VK_F7,
	f8 = VK_F8,
	f9 = VK_F9,
	f10 = VK_F10,
	f11 = VK_F11,
	f12 = VK_F12,
	f13 = VK_F13,
	f14 = VK_F14,
	f15 = VK_F15,
	f16 = VK_F16,
	f17 = VK_F17,
	f18 = VK_F18,
	f19 = VK_F19,
	f20 = VK_F20,
	f21 = VK_F21,
	f22 = VK_F22,
	f23 = VK_F23,
	f24 = VK_F24,
	win_left = VK_LWIN,
	win_right = VK_RWIN,
	shift_left = VK_LSHIFT,
	shift_right = VK_RSHIFT,
	control_left = VK_LCONTROL,
	control_right = VK_RCONTROL,
	alt_left = VK_LMENU,
	alt_right = VK_RMENU,
	esc = VK_ESCAPE,
	back_tick = VK_OEM_3,
	tab = VK_TAB,
	cap = VK_CAPITAL,
	snapshot = VK_SNAPSHOT,//print screen
	scroll_lock = VK_SCROLL,
	pause_break = VK_PAUSE,
	insert = VK_INSERT,
	del = VK_DELETE,
	home = VK_HOME,
	end = VK_END,
	page_up = VK_PRIOR,
	page_down = VK_NEXT,
	left = VK_LEFT,
	right = VK_RIGHT,
	up = VK_UP,
	down = VK_DOWN,
	minus = VK_OEM_MINUS,
	equal = VK_OEM_PLUS,
	back_slash = VK_OEM_5,
	back_space = VK_BACK,
	square_bracket_left = VK_OEM_4,
	square_bracket_right = VK_OEM_6,
	enter = VK_RETURN,
	semicolon = VK_OEM_1,
	quote = VK_OEM_7,
	comma = VK_OEM_COMMA,
	period = VK_OEM_PERIOD,
	slash = VK_OEM_2,
	apps = VK_APPS,//simulated right click
	space = VK_SPACE,
	shift = VK_SHIFT,
	control = VK_CONTROL,
	alt = VK_MENU,
	sleep = VK_SLEEP,
	select = VK_SELECT,
	print = VK_PRINT,
	execute = VK_EXECUTE,
	help = VK_HELP,
	separator = VK_SEPARATOR,
	numpad_equal = VK_OEM_NEC_EQUAL,
	clear = VK_CLEAR,
};
ENUM(ScanCode, u8)
{
	esc = 0x01,
	_1 = 0x02,
	_2 = 0x03,
	_3 = 0x04,
	_4 = 0x05,
	_5 = 0x06,
	_6 = 0x07,
	_7 = 0x08,
	_8 = 0x09,
	_9 = 0x0A,
	_0 = 0x0B,
	minus = 0x0C,
	equal = 0x0D,
	back_space = 0x0E,
	tab = 0x0F,
	q = 0x10,
	w = 0x11,
	e = 0x12,
	r = 0x13,
	t = 0x14,
	y = 0x15,
	u = 0x16,
	i = 0x17,
	o = 0x18,
	p = 0x19,
	square_bracket_left = 0x1A,
	square_bracket_right = 0x1B,
	enter = 0x1C,
	ctrl = 0x1D,
	a = 0x1E,
	s = 0x1F,
	d = 0x20,
	f = 0x21,
	g = 0x22,
	h = 0x23,
	j = 0x24,
	k = 0x25,
	l = 0x26,
	semicolon = 0x27,
	quote = 0x28,
	back_tick = 0x29,
	shift_left = 0x2A,
	back_slash = 0x2B,
	z = 0x2C,
	x = 0x2D,
	c = 0x2E,
	v = 0x2F,
	b = 0x30,
	n = 0x31,
	m = 0x32,
	comma = 0x33,
	period = 0x34,
	slash = 0x35,
	shift_right = 0x36,
	snapshot = 0x37,
	alt = 0x38,
	space = 0x39,
	cap = 0x3A,
	f1 = 0x3B,
	f2 = 0x3C,
	f3 = 0x3D,
	f4 = 0x3E,
	f5 = 0x3F,
	f6 = 0x40,
	f7 = 0x41,
	f8 = 0x42,
	f9 = 0x43,
	f10 = 0x44,
	num = 0x45,
	scroll_lock = 0x46,
	//home = 0x47,
	//up = 0x48,
	//pgup = 0x49,
	//gray_minus = 0x4A,
	//left = 0x4B,
	//center = 0x4C,
	//right = 0x4D,
	//gray_plus = 0x4E,
	//end = 0x4F,
	//down = 0x50,
	//pgdn = 0x51,
	//ins = 0x52,
	del = 0x53,
	f11 = 0x57,
	f12 = 0x58,
	win_left = 0x5B,
	win_right = 0x5C,
	apps = 0x5D,
	numpad_0 = 0x52,
	numpad_1 = 0x4F,
	numpad_2 = 0x50,
	numpad_3 = 0x51,
	numpad_4 = 0x4B,
	numpad_5 = 0x4C,
	numpad_6 = 0x4D,
	numpad_7 = 0x47,
	numpad_8 = 0x48,
	numpad_9 = 0x49,
	numpad_minus = 0x4A,
	numpad_plus = 0x4E,
};

//static u8 toVkey(u16 sc, bool isEnhancedKey, u16 vkey)
//{
//	//m_scanCode{static_cast<u8>(User32::MapVirtualKeyW(vkey,MAPVK_VK_TO_VSC))},
//	const u8 result{ static_cast<u8>(Win32u::NtUserMapVirtualKeyEx(sc, MAPVK_VSC_TO_VK_EX, 0, 0)) };
//	switch (result)
//	{
//		case Vkey::insert:
//	}
//	return ;
//}

INLINE static constexpr Opt<KeyId> translate(ScanCode sc, bool isEnhancedKey, Vkey vkey)noexcept
{
	switch (vkey)
	{
		case Vkey::esc:return KeyId::esc;
		case Vkey::f1: return KeyId::f1;
		case Vkey::f2: return KeyId::f2;
		case Vkey::f3: return KeyId::f3;
		case Vkey::f4: return KeyId::f4;
		case Vkey::f5: return KeyId::f5;
		case Vkey::f6: return KeyId::f6;
		case Vkey::f7: return KeyId::f7;
		case Vkey::f8: return KeyId::f8;
		case Vkey::f9: return KeyId::f9;
		case Vkey::f10:return KeyId::f10;
		case Vkey::f11:return KeyId::f11;
		case Vkey::f12:return KeyId::f12;
		case Vkey::f13:return KeyId::f13;
		case Vkey::f14:return KeyId::f14;
		case Vkey::f15:return KeyId::f15;
		case Vkey::f16:return KeyId::f16;
		case Vkey::f17:return KeyId::f17;
		case Vkey::f18:return KeyId::f18;
		case Vkey::f19:return KeyId::f19;
		case Vkey::f20:return KeyId::f20;
		case Vkey::f21:return KeyId::f21;
		case Vkey::f22:return KeyId::f22;
		case Vkey::f23:return KeyId::f23;
		case Vkey::f24:return KeyId::f24;
		case Vkey::a:  return KeyId::a;
		case Vkey::b:  return KeyId::b;
		case Vkey::c:  return KeyId::c;
		case Vkey::d:  return KeyId::d;
		case Vkey::e:  return KeyId::e;
		case Vkey::f:  return KeyId::f;
		case Vkey::g:  return KeyId::g;
		case Vkey::h:  return KeyId::h;
		case Vkey::i:  return KeyId::i;
		case Vkey::j:  return KeyId::j;
		case Vkey::k:  return KeyId::k;
		case Vkey::l:  return KeyId::l;
		case Vkey::m:  return KeyId::m;
		case Vkey::n:  return KeyId::n;
		case Vkey::o:  return KeyId::o;
		case Vkey::p:  return KeyId::p;
		case Vkey::q:  return KeyId::q;
		case Vkey::r:  return KeyId::r;
		case Vkey::s:  return KeyId::s;
		case Vkey::t:  return KeyId::t;
		case Vkey::u:  return KeyId::u;
		case Vkey::v:  return KeyId::v;
		case Vkey::w:  return KeyId::w;
		case Vkey::x:  return KeyId::x;
		case Vkey::y:  return KeyId::y;
		case Vkey::z:  return KeyId::z;

		case Vkey::_0:  return KeyId::_0;
		case Vkey::_1:  return KeyId::_1;
		case Vkey::_2:  return KeyId::_2;
		case Vkey::_3:  return KeyId::_3;
		case Vkey::_4:  return KeyId::_4;
		case Vkey::_5:  return KeyId::_5;
		case Vkey::_6:  return KeyId::_6;
		case Vkey::_7:  return KeyId::_7;
		case Vkey::_8:  return KeyId::_8;
		case Vkey::_9:  return KeyId::_9;
		case Vkey::back_tick:return KeyId::back_tick;
		case Vkey::minus:return KeyId::minus;
		case Vkey::equal:return KeyId::equal;
		case Vkey::back_space:return KeyId::back_space;

		case Vkey::tab:return KeyId::tab;
		case Vkey::square_bracket_left:return KeyId::square_bracket_left;
		case Vkey::square_bracket_right:return KeyId::square_bracket_right;
		case Vkey::back_slash:return KeyId::back_slash;

		case Vkey::cap:return KeyId::cap;
		case Vkey::semicolon:return KeyId::semicolon;
		case Vkey::quote:return KeyId::quote;

		case Vkey::comma:return KeyId::comma;
		case Vkey::period:return KeyId::period;

		case Vkey::space:return KeyId::space;

		case Vkey::numpad_minus:return KeyId::numpad_minus;
		case Vkey::numpad_plus:return KeyId::numpad_plus;
		case Vkey::numpad_div:return KeyId::numpad_div;
		case Vkey::numpad_mul:return KeyId::numpad_mul;

		case Vkey::enter:return isEnhancedKey ? KeyId::numpad_enter : KeyId::enter;
		case Vkey::slash:return KeyId::slash;
		case Vkey::snapshot:return KeyId::snapshot;
		case Vkey::apps:return KeyId::apps;
		case Vkey::numlock:return KeyId::numlock;
		case Vkey::pause_break:return KeyId::pause_break;
		case Vkey::scroll_lock:return KeyId::scroll_lock;
		case Vkey::numpad_dec:return KeyId::numpad_dec;
		case Vkey::del:return isEnhancedKey?KeyId::del: KeyId::numpad_dec;
		case Vkey::insert		:return isEnhancedKey?KeyId::insert		:KeyId::numpad_0;
		case Vkey::end			:return isEnhancedKey?KeyId::end		:KeyId::numpad_1;
		case Vkey::down			:return isEnhancedKey?KeyId::down		:KeyId::numpad_2;
		case Vkey::page_down	:return isEnhancedKey?KeyId::page_down	:KeyId::numpad_3;
		case Vkey::left			:return isEnhancedKey?KeyId::left		:KeyId::numpad_4;
		case Vkey::clear		:return isEnhancedKey?KeyId::clear		:KeyId::numpad_5;
		case Vkey::right		:return isEnhancedKey?KeyId::right		:KeyId::numpad_6;
		case Vkey::home			:return isEnhancedKey?KeyId::home		:KeyId::numpad_7;
		case Vkey::up			:return isEnhancedKey?KeyId::up			:KeyId::numpad_8;
		case Vkey::page_up		:return isEnhancedKey?KeyId::page_up	:KeyId::numpad_9;
		case Vkey::numpad_0:return KeyId::numpad_0;
		case Vkey::numpad_1:return KeyId::numpad_1;
		case Vkey::numpad_2:return KeyId::numpad_2;
		case Vkey::numpad_3:return KeyId::numpad_3;
		case Vkey::numpad_4:return KeyId::numpad_4;
		case Vkey::numpad_5:return KeyId::numpad_5;
		case Vkey::numpad_6:return KeyId::numpad_6;
		case Vkey::numpad_7:return KeyId::numpad_7;
		case Vkey::numpad_8:return KeyId::numpad_8;
		case Vkey::numpad_9:return KeyId::numpad_9;

		case Vkey::alt:return isEnhancedKey? KeyId::alt_right:KeyId::alt_left;
		case Vkey::control:return isEnhancedKey? KeyId::control_right:KeyId::control_left;

		case Vkey::control_left:return KeyId::control_left;
		case Vkey::control_right:return KeyId::control_right;
		case Vkey::alt_left:return KeyId::alt_left;
		case Vkey::alt_right:return KeyId::alt_right;

		default:
			switch (sc)
			{
				case ScanCode::shift_left:
					if (Vkey::shift == vkey)return KeyId::shift_left;
					else return {};
				case ScanCode::shift_right:return KeyId::shift_right;
				case ScanCode::win_left:return KeyId::win_left;
				case ScanCode::win_right:return KeyId::win_right;
				default: return {};
			}
	}
}
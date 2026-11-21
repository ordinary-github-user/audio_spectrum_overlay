#pragma once
#include "AllUnique.h"
#include "Compiler.h"
#include "ConceptForEach.h"
#include "ConceptNotCvref.h"
#include "ConceptSig.h"
#include "ConceptString.h"
#include "Constant.h"
#include "ElementOf.h"
#include "FixedString.h"
#include "IndexOf.h"
#include "StaticAssert.h"
#include "StaticError.h"
#include "TypeAt.h"
struct KeyState
{
	u8 pressed : 1;
	u8 changed : 1;
};

template<FixedString name_, std::array keys_>
requires($StringOfCharSize<decltype(name_), 1>)
struct StaticKeybind
{
	static constexpr FixedString name{ name_ };
	static constexpr std::array keys{ keys_ };
	using Key = ElementOf<decltype(keys)>;
};
#if MSVC//msvc bug: 'details::KeyStatesTest::<lambda_1847>::()::<lambda_1848_>::operator ()': a call of a non-static member function requires an object
	#define KEY_STATE_STATIC_OPERATOR_CALL(...) __VA_ARGS__.operator()()
#else
	#define KEY_STATE_STATIC_OPERATOR_CALL(...) std::remove_cvref_t<decltype(__VA_ARGS__)>::operator()()
#endif
template<FixedString, auto...>struct DynamicKeybind;
template<FixedString name_, auto getter>
requires($StringOfCharSize<decltype(name_), 1> && std::is_reference_v<decltype(KEY_STATE_STATIC_OPERATOR_CALL(getter))> && requires{ {KEY_STATE_STATIC_OPERATOR_CALL(getter)}->$ForEach; })
struct DynamicKeybind<name_, getter>
{
	static constexpr FixedString name{ name_ };
	static constexpr auto const& keys{ KEY_STATE_STATIC_OPERATOR_CALL(getter) };
	using Key = ElementOf<decltype(keys)>;
};
template<FixedString name_, auto getter>
requires($StringOfCharSize<decltype(name_), 1> && std::is_reference_v<decltype(KEY_STATE_STATIC_OPERATOR_CALL(getter))> && !requires{ {KEY_STATE_STATIC_OPERATOR_CALL(getter)}->$ForEach; })
struct DynamicKeybind<name_, getter>
{
	static constexpr FixedString name{ name_ };
	using Key = std::remove_cvref_t<decltype(KEY_STATE_STATIC_OPERATOR_CALL(getter))>;
	static constexpr const Key& key{ KEY_STATE_STATIC_OPERATOR_CALL(getter) };
};
#undef KEY_STATE_STATIC_OPERATOR_CALL
namespace details
{
	template<class>constexpr bool is_static_keybind{};
	template<class>constexpr bool is_dynamic_keybind{};
	template<auto..._>constexpr bool is_static_keybind<StaticKeybind<_...>>{true};
	template<auto..._>constexpr bool is_dynamic_keybind<DynamicKeybind<_...>>{true};
}
template<class T>concept $StaticKeyBind = details::is_static_keybind<std::remove_cvref_t<T>>;
template<class T>concept $DynamicKeyBind = details::is_dynamic_keybind<std::remove_cvref_t<T>>;

template<auto is_pressed, $NotCvref...binds>
requires(sizeof...(binds) > 0 && all_unique<binds::name...> && (...&&$Sig<decltype(is_pressed), bool(const typename binds::Key)>))
struct KeyStates
{
private:
	KeyState states[sizeof...(binds)]{};
	template<usize I>
	INLINE constexpr void updateKey()noexcept
	{
		if constexpr($StaticKeyBind<TYPE_AT<I, binds...>>)
			[&] <usize...J>(std::index_sequence<J...>)
			{
				static constexpr auto keys{ TYPE_AT<I, binds...>::keys };
				const bool pressed{ (...&&is_pressed(constant<keys[J]>)) };
				states[I].changed = states[I].pressed != pressed;
				states[I].pressed = pressed;
			}(std::make_index_sequence<TYPE_AT<I, binds...>::keys.size()>{});
		else if constexpr ($DynamicKeyBind<TYPE_AT<I, binds...>> && requires{TYPE_AT<I, binds...>::keys; })
		{
			static constexpr auto const& keys{ TYPE_AT<I, binds...>::keys };
			bool pressed{ true };
			for (const auto key : keys)
				if (!is_pressed(key))
					pressed = false;
			states[I].changed = states[I].pressed != pressed;
			states[I].pressed = pressed;
		}
		else if constexpr ($DynamicKeyBind<TYPE_AT<I, binds...>> && requires{TYPE_AT<I, binds...>::key; })
		{
			const bool pressed{ is_pressed(TYPE_AT<I, binds...>::key) };
			states[I].changed = states[I].pressed != pressed;
			states[I].pressed = pressed;
		}
		else STATIC_ERROR(bind not supported);
	}
	template<usize...I>
	INLINE constexpr void updateImpl(std::index_sequence<I...>)noexcept
	{
		(..., updateKey<I>());
	}
public:
	INLINE constexpr void update()noexcept
	{
		updateImpl(std::make_index_sequence<sizeof...(binds)>{});
	}
	INLINE constexpr void reset()noexcept
	{
		for (KeyState& state : states)
			state = KeyState{ .pressed{},.changed{} };
	}

	template<FixedString name>requires((...+usize{name == binds::name})==1)INLINE constexpr bool pressed     ()const noexcept { static constexpr auto index{index_of_value<name, binds::name...>}; return  states[index].pressed; }
	template<FixedString name>requires((...+usize{name == binds::name})==1)INLINE constexpr bool released    ()const noexcept { static constexpr auto index{index_of_value<name, binds::name...>}; return !states[index].pressed; }
	template<FixedString name>requires((...+usize{name == binds::name})==1)INLINE constexpr bool justPressed ()const noexcept { static constexpr auto index{index_of_value<name, binds::name...>}; return  states[index].pressed && states[index].changed; }
	template<FixedString name>requires((...+usize{name == binds::name})==1)INLINE constexpr bool justReleased()const noexcept { static constexpr auto index{index_of_value<name, binds::name...>}; return !states[index].pressed && states[index].changed; }

	static constexpr void print(auto& os)noexcept
	{
		static constexpr auto maxNameLen{ compact<Algorithm::max(binds::name.size()...)> };
		[&] <usize...I>(std::index_sequence<I...>) 
		{
			(..., [&] 
			{
				using Bind = TYPE_AT<I, binds...>;
				static constexpr FixedString name{ Bind::name };
				static constexpr FixedString name_padded{ name + FixedString{' '}.repeat<maxNameLen-name.size()>()};
				if constexpr (I == 0)os << constant<name_padded + ": ">;
				else/*             */os << constant<"\n" + name_padded + ": ">;
				if constexpr (requires{Bind::keys; })
				{
					bool first{ true };
					for (const auto key : Bind::keys)
					{
						if (first) first = false;
						else os << ',';
						os << key;
					}
				}
				else if constexpr (requires{Bind::key; })
					os << Bind::key;
			}());
		}(std::make_index_sequence<sizeof...(binds)>{});
	}
};

namespace details::KeyStatesTest
{
	enum class TestKey : u8 { a, b, c, d };
	static constexpr std::array a{ TestKey::a };
	static constexpr std::array b{ TestKey::b,TestKey::c };
	static constexpr TestKey key{ TestKey::d };//msvc produce ICE if `key` is placed inside static_assert
	STATIC_ASSERT([]static
	{
		KeyStates
		<
			[](TestKey)static{return true; }, 
			StaticKeybind<"bind#0", a>,
			DynamicKeybind<"bind#1",[]static -> auto&{return b; }>,
			DynamicKeybind<"bind#2",[]static -> auto&{return key; }>,
			StaticKeybind<"bind#3", { TestKey::a,TestKey::b,TestKey::c }>
		> keyStates;
		keyStates.update();
		if (!keyStates.pressed<"bind#0">())return false;
		if (!keyStates.pressed<"bind#1">())return false;
		if (!keyStates.pressed<"bind#2">())return false;
		if (!keyStates.pressed<"bind#3">())return false;
		if (!keyStates.justPressed<"bind#0">())return false;
		if (!keyStates.justPressed<"bind#1">())return false;
		if (!keyStates.justPressed<"bind#2">())return false;
		if (!keyStates.justPressed<"bind#3">())return false;
		keyStates.update();
		if (!keyStates.pressed<"bind#0">())return false;
		if (!keyStates.pressed<"bind#1">())return false;
		if (!keyStates.pressed<"bind#2">())return false;
		if (!keyStates.pressed<"bind#3">())return false;
		if (keyStates.justPressed<"bind#0">())return false;
		if (keyStates.justPressed<"bind#1">())return false;
		if (keyStates.justPressed<"bind#2">())return false;
		if (keyStates.justPressed<"bind#3">())return false;
		return true;
	}());
}
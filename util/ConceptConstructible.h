#pragma once
template<class T, class U>
concept $Constructible = requires(T arg) { U{ arg }; };
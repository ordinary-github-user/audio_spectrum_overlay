#pragma once
template<class T, auto...>struct delay_instantiation_impl { using type = T; };
template<class T, auto...x>using delay_instantiation = delay_instantiation_impl<T, x...>::type;
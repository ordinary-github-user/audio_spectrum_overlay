#pragma once
template<class T>concept $Empty = __is_empty(T);
template<class T>concept $NotEmpty = !__is_empty(T);
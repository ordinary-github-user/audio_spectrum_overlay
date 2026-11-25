#pragma once
template<class T>
concept $HasBeginEnd = requires(const T container) { container.begin()!=container.end(); };
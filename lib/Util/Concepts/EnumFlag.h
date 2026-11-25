#pragma once
template<class T>concept $Enum = __is_enum(T) && requires{is_enum(T{}); };
template<class T>concept $Flag = __is_enum(T) && requires{is_flag(T{}); };
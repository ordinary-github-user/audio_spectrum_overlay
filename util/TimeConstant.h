#pragma once
#include "Compact.h"
constexpr auto TICKSPERMIN{ compact<600000000> };
constexpr auto TICKSPERSEC{ compact<10000000> };
constexpr auto TICKSPERMSEC{ compact<10000> };
constexpr auto SECSPERDAY{ compact<86400> };
constexpr auto SECSPERHOUR{ compact<3600> };
constexpr auto SECSPERMIN{ compact<60> };
constexpr auto MINSPERHOUR{ compact<60> };
constexpr auto HOURSPERDAY{ compact<24> };
constexpr auto EPOCHWEEKDAY{ compact<1> };/* Jan 1, 1601 was Monday */
constexpr auto DAYSPERWEEK{ compact<7> };
constexpr auto EPOCHYEAR{ compact<1601> };
constexpr auto DAYSPERNORMALYEAR{ compact<365> };
constexpr auto DAYSPERLEAPYEAR{ compact<366> };
constexpr auto MONSPERYEAR{ compact<12> };
constexpr auto DAYSPERQUADRICENTENNIUM{ compact<365 * 400 + 97> };
constexpr auto DAYSPERNORMALQUADRENNIUM{ compact<365 * 4 + 1> };
#pragma once

#include <string>

struct BigSpecial {
	const std::string value{ "" };
	const std::string symbol{ "" };
};

namespace bigspecials {
	const BigSpecial complex_infinity = { "ComplexInfinity", "∞~" };
	const BigSpecial negative_infinity = { "NegativeInfinity", "-∞" };
	const BigSpecial positive_infinity = { "PositiveInfinity", "∞", };
	const BigSpecial undefined = { "Undefined", "0 ∞",  };
}
#pragma once

#include "IR.hpp"

#include <string_view>

namespace sluggish {

// Compiles source into IR functions and writes the existing intermediate dumps.
std::vector<IRFunction> compile(std::string_view source);

} // namespace sluggish

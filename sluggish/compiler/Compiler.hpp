#pragma once

#include "IR.hpp"

#include <string_view>

namespace sluggish {

// Compiles source into typed IR and writes the existing intermediate dumps.
BasicBlock compile(std::string_view source);

} // namespace sluggish

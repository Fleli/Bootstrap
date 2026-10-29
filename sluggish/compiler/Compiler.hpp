#pragma once

#include "IR.hpp"

#include <string_view>

namespace sluggish {

// Compiles source into a program owning its types and IR functions, and writes intermediate dumps.
IRProgram compile(std::string_view source);

} // namespace sluggish

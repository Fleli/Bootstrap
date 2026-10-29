#pragma once

#include "IR.hpp"
#include "resolver/Resolver.hpp"

namespace sluggish {

// Lowers each function into one block and writes intermediate/lower.txt.
std::vector<IRFunction> lower(const Resolver& resolver);

} // namespace sluggish

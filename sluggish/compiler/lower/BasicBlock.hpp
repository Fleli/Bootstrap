#pragma once

#include "IR.hpp"
#include "resolver/Resolver.hpp"

namespace sluggish {

// Lowers into one block and writes intermediate/lower.txt.
BasicBlock lower(const Resolver& resolver);

} // namespace sluggish

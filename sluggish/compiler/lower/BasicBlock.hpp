#pragma once

#include "IR.hpp"
#include "resolver/Resolver.hpp"

namespace sluggish {

// Lowers into one block and writes intermediate/lower.txt.
// Function lowering is not implemented yet.
BasicBlock lower(const Resolver& resolver);

} // namespace sluggish

#pragma once

#include "IR.hpp"
#include "isa/Instruction.hpp"

#include <vector>

namespace simplecpu {

// Builds only zero-argument main, starting at RAM word 0 and returning in r0.
// Data follows the code, with one word per storage slot.
std::vector<Instruction> build_assembly(const std::vector<sluggish::IRFunction>& functions);

} // namespace simplecpu

#pragma once

#include "IR.hpp"
#include "isa/Instruction.hpp"

#include <vector>

namespace simplecpu {

// Code starts at RAM word 0. Data follows it, with one word per storage slot.
std::vector<Instruction> build_assembly(const sluggish::BasicBlock& block);

} // namespace simplecpu

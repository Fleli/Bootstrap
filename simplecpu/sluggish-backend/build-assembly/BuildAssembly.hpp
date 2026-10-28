#pragma once

#include "IR.hpp"

#include <string>

namespace simplecpu {

// Code starts at RAM word 0. Data follows it, with one word per storage slot.
std::string build_assembly(const sluggish::BasicBlock& block);

} // namespace simplecpu

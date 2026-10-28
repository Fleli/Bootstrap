#pragma once

#include "isa/Instruction.hpp"

#include <cstdint>
#include <vector>

namespace simplecpu {

std::vector<std::uint32_t> assemble(const std::vector<Instruction>& instructions);

} // namespace simplecpu

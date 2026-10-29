#pragma once

#include "IR.hpp"
#include "isa/Instruction.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace simplecpu {

struct AssemblyFunction {

    std::string name;
    std::uint32_t address = 0; // Entry address in encoded RAM words.
    std::vector<Instruction> instructions;

};

// Builds main at RAM word 0, followed by the other functions in source order.
// Each function returns in r0 and halts. Separate storage follows all code.
std::vector<AssemblyFunction> build_assembly(const std::vector<sluggish::IRFunction>& functions);

} // namespace simplecpu

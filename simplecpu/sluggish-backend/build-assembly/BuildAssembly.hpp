#pragma once

#include "IR.hpp"
#include "isa/Instruction.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace simplecpu {

inline constexpr std::uint32_t main_result_address = 0;
inline constexpr std::uint32_t program_address = 1;
inline constexpr std::uint32_t ram_words = 1024;

struct AssemblyFunction {

    std::string name;
    std::uint32_t address = 0; // Entry address in encoded RAM words.
    std::vector<Instruction> instructions;

};

// Builds _start, main and the remaining functions. Calls use stack frames;
// _start supplies main_result_address as main's return storage, then halts.
std::vector<AssemblyFunction> build_assembly(const std::vector<sluggish::IRFunction>& functions);

// Call operands initially hold function indices; build_assembly resolves addresses.
AssemblyFunction build_function(const sluggish::IRFunction& function,
    const std::unordered_map<std::string, std::uint32_t>& function_indices);

} // namespace simplecpu

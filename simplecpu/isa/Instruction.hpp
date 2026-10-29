#pragma once

#include <cstdint>
#include <string>

namespace simplecpu {

enum class Opcode {
    Halt,
    StoreImm,
    LoadImm,
    Store,
    Load,
    Add,
    Push,
    Pop,
    Call,
    Ret,
};

struct Instruction {

    Opcode opcode;
    std::uint32_t operand_a = 0;
    std::uint32_t operand_b = 0;
    std::uint32_t operand_c = 0;

    // One assembly line, without indentation or a trailing newline.
    std::string text() const;

};

} // namespace simplecpu

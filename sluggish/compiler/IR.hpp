#pragma once

#include "type-system/Type.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sluggish {

struct BasicBlockInstruction {
    virtual ~BasicBlockInstruction() = default;
};

// Slots identify storage within a function. The hardware backend decides its location.
struct AllocateInstruction : public BasicBlockInstruction {

    std::size_t slot;
    const Type* type = nullptr;

};

struct StoreInstruction : public BasicBlockInstruction {

    std::size_t slot;
    std::uint16_t value;

};

struct CopyInstruction : public BasicBlockInstruction {

    std::size_t source_slot;
    std::size_t destination_slot;

};

// Ends the block and returns from the function.
struct ReturnInstruction : public BasicBlockInstruction {

    std::uint16_t value = 0;
    std::optional<std::size_t> source_slot; // Empty means return the literal value.

};

struct BasicBlock {
    std::vector<std::unique_ptr<BasicBlockInstruction>> instructions;
};

// Argument slots hold incoming values supplied by the caller.
struct IRArgument {

    std::size_t slot;
    const Type* type = nullptr;

};

struct IRFunction {

    std::string name;
    const Type* return_type = nullptr;
    std::vector<IRArgument> arguments;
    std::vector<BasicBlock> blocks;

};

} // namespace sluggish

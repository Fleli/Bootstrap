#pragma once

#include "type-system/Type.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace sluggish {

struct BasicBlockInstruction {
    virtual ~BasicBlockInstruction() = default;
};

// Slots identify local storage. The hardware backend decides its location.
struct AllocateInstruction : public BasicBlockInstruction {

    std::size_t slot;
    const Type* type = nullptr;

};

struct StoreInstruction : public BasicBlockInstruction {

    std::size_t slot;
    std::uint16_t value;

};

struct BasicBlock {
    std::vector<std::unique_ptr<BasicBlockInstruction>> instructions;
};

} // namespace sluggish

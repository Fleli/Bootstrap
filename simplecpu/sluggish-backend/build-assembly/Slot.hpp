#pragma once

#include "type-system/Type.hpp"
#include "isa/Instruction.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

namespace simplecpu {

struct Slot {

    std::uint32_t offset;
    const sluggish::Type* type;

    // If present, use the pointer value stored in pointer_slot as the base instead of FP.
    std::optional<std::size_t> pointer_slot;

};

// Address calculations use r3 and r6, preserving values held in r4 and r5.
void load_frame_address(std::uint32_t offset, std::vector<Instruction>& instructions);

void load_slot_address(const Slot& slot, const std::unordered_map<std::size_t, Slot>& slots,
    std::vector<Instruction>& instructions, std::uint32_t word_offset = 0);

} // namespace simplecpu

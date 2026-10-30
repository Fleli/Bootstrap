#include "Slot.hpp"

namespace simplecpu {

void load_frame_address(std::uint32_t offset, std::vector<Instruction>& instructions) {

    instructions.push_back({Opcode::LoadImm, 3, offset});
    instructions.push_back({Opcode::Add, 3, 1, 3});

}

void load_slot_address(const Slot& slot, const std::unordered_map<std::size_t, Slot>& slots,
    std::vector<Instruction>& instructions, std::uint32_t word_offset) {

    const auto offset = slot.offset + word_offset;

    if (!slot.pointer_slot.has_value()) {

        load_frame_address(offset, instructions);
        return;

    }

    load_slot_address(slots.at(*slot.pointer_slot), slots, instructions);
    instructions.push_back({Opcode::Load, 3, 3});

    if (offset != 0) {

        instructions.push_back({Opcode::LoadImm, 6, offset});
        instructions.push_back({Opcode::Add, 3, 3, 6});

    }

}

} // namespace simplecpu

#include "BuildInstruction.hpp"

namespace simplecpu {

void build_copy(const sluggish::CopyInstruction& copy,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions) {

    const auto source = slots.at(copy.source_slot);
    const auto destination = slots.at(copy.destination_slot);
    const auto words = storage_words(source.type);

    for (std::uint32_t i = 0; i < words; ++i) {

        load_slot_address(source, slots, instructions, i);
        instructions.push_back({Opcode::Load, 4, 3});
        load_slot_address(destination, slots, instructions, i);
        instructions.push_back({Opcode::Store, 3, 4});

    }

}

} // namespace simplecpu

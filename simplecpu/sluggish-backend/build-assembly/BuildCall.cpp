#include "BuildInstruction.hpp"

namespace simplecpu {

void build_call(const sluggish::CallInstruction& call,
    const std::unordered_map<std::size_t, Slot>& slots,
    const std::unordered_map<std::string, std::uint32_t>& function_indices,
    std::vector<Instruction>& instructions) {

    load_slot_address(slots.at(call.destination_slot), slots, instructions);
    instructions.push_back({Opcode::Push, 3});

    std::uint32_t pushed_words = 0;

    for (const auto slot : call.argument_slots) {

        const auto argument = slots.at(slot);
        const auto words = storage_words(argument.type);
        pushed_words += words;

        // Reverse each value's words so its first word has the lowest address.
        for (std::uint32_t i = words; i > 0; --i) {

            load_slot_address(argument, slots, instructions, i - 1);
            instructions.push_back({Opcode::Load, 4, 3});
            instructions.push_back({Opcode::Push, 4});

        }

    }

    instructions.push_back({Opcode::Call, function_indices.at(call.function_name)});
    instructions.push_back({Opcode::LoadImm, 3, pushed_words + 1});
    instructions.push_back({Opcode::Add, 2, 2, 3});

}

} // namespace simplecpu

#include "BuildInstruction.hpp"

namespace simplecpu {

void build_return(const sluggish::ReturnInstruction& returned,
    const sluggish::Type* return_type, std::uint32_t argument_words,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions) {

    const auto words = storage_words(return_type);
    load_frame_address(argument_words + 2, instructions);
    instructions.push_back({Opcode::Load, 5, 3});

    for (std::uint32_t i = 0; i < words; ++i) {

        if (returned.source_slot.has_value()) {

            load_slot_address(slots.at(*returned.source_slot), slots, instructions, i);
            instructions.push_back({Opcode::Load, 4, 3});

        } else {

            instructions.push_back({Opcode::LoadImm, 4, returned.value});

        }

        instructions.push_back({Opcode::Store, 5, 4});

        if (i + 1 < words) {

            instructions.push_back({Opcode::LoadImm, 3, 1});
            instructions.push_back({Opcode::Add, 5, 5, 3});

        }

    }

    instructions.push_back({Opcode::LoadImm, 3, 0});
    instructions.push_back({Opcode::Add, 2, 1, 3});
    instructions.push_back({Opcode::Pop, 1});
    instructions.push_back({Opcode::Ret});

}

} // namespace simplecpu

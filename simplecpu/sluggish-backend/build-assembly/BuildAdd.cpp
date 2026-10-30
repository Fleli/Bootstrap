#include "BuildInstruction.hpp"

namespace simplecpu {

void build_add(const sluggish::AddInstruction& add,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions) {

    load_slot_address(slots.at(add.left_slot), slots, instructions);
    instructions.push_back({Opcode::Load, 4, 3});
    load_slot_address(slots.at(add.right_slot), slots, instructions);
    instructions.push_back({Opcode::Load, 5, 3});
    instructions.push_back({Opcode::Add, 4, 4, 5});
    load_slot_address(slots.at(add.destination_slot), slots, instructions);
    instructions.push_back({Opcode::Store, 3, 4});

}

} // namespace simplecpu

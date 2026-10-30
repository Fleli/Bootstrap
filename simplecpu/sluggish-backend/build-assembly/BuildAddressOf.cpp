#include "BuildInstruction.hpp"

namespace simplecpu {

void build_address_of(const sluggish::AddressOfInstruction& address,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions) {

    load_slot_address(slots.at(address.source_slot), slots, instructions);
    instructions.push_back({Opcode::LoadImm, 4, 0});
    instructions.push_back({Opcode::Add, 4, 3, 4});
    load_slot_address(slots.at(address.destination_slot), slots, instructions);
    instructions.push_back({Opcode::Store, 3, 4});

}

} // namespace simplecpu

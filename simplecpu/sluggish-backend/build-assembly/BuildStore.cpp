#include "BuildInstruction.hpp"

namespace simplecpu {

void build_store(const sluggish::StoreInstruction& store,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions) {

    load_slot_address(slots.at(store.slot), slots, instructions);
    instructions.push_back({Opcode::LoadImm, 4, store.value});
    instructions.push_back({Opcode::Store, 3, 4});

}

} // namespace simplecpu

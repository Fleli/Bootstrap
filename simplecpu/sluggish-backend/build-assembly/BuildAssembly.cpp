#include "BuildAssembly.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>

namespace simplecpu {

std::vector<Instruction> build_assembly(const sluggish::BasicBlock& block) {

    // Each store_imm occupies three words; the final halt occupies one.
    std::uint64_t next_address = 1;
    const std::uint64_t maximum_address = std::numeric_limits<std::uint32_t>::max();

    for (const auto& instruction : block.instructions) {

        if (dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

            next_address += 3;

            if (next_address > maximum_address + 1) {
                throw std::runtime_error("SimpleCPU code exceeds the address space");
            }

        }

    }

    std::vector<Instruction> assembly;
    std::unordered_map<std::size_t, std::uint32_t> addresses;

    for (const auto& instruction : block.instructions) {

        if (const auto* allocate = dynamic_cast<const sluggish::AllocateInstruction*>(instruction.get())) {

            if (allocate->type == nullptr || allocate->type->bit_width() == 0 || allocate->type->bit_width() > 32) {
                throw std::runtime_error("SimpleCPU requires types between 1 and 32 bits");
            }

            if (next_address > maximum_address) {
                throw std::runtime_error("SimpleCPU data exceeds the address space");
            }

            if (!addresses.emplace(allocate->slot, next_address).second) {
                throw std::runtime_error("Duplicate storage slot: " + std::to_string(allocate->slot));
            }

            ++next_address;

        } else if (const auto* store = dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

            const auto address = addresses.find(store->slot);

            if (address == addresses.end()) {
                throw std::runtime_error("Unallocated storage slot: " + std::to_string(store->slot));
            }

            assembly.push_back({Opcode::StoreImm, address->second, store->value});

        } else {

            throw std::runtime_error("Unsupported instruction for SimpleCPU");

        }

    }

    assembly.push_back({Opcode::Halt});
    return assembly;

}

} // namespace simplecpu

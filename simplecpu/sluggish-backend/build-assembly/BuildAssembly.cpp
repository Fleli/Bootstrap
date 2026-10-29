#include "BuildAssembly.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>

namespace simplecpu {

std::vector<Instruction> build_assembly(const std::vector<sluggish::IRFunction>& functions) {

    const sluggish::IRFunction* entry = nullptr;

    for (const auto& function : functions) {
        if (function.name == "main") {
            entry = &function;
            break;
        }
    }

    if (entry == nullptr) {
        throw std::runtime_error("Missing entry function: main");
    }

    if (!entry->arguments.empty()) {
        throw std::runtime_error("main must not take arguments");
    }

    if (entry->blocks.size() != 1) {
        throw std::runtime_error("SimpleCPU requires one basic block in main");
    }

    const auto& block = entry->blocks.front();

    if (block.instructions.empty() || dynamic_cast<const sluggish::ReturnInstruction*>(block.instructions.back().get()) == nullptr) {
        throw std::runtime_error("main must end with a return");
    }

    // Start with the final halt; count the other instruction words below.
    std::uint64_t next_address = 1;
    const std::uint64_t maximum_address = std::numeric_limits<std::uint32_t>::max();

    for (const auto& instruction : block.instructions) {

        if (dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

            next_address += 3;

        } else if (dynamic_cast<const sluggish::CopyInstruction*>(instruction.get())) {

            next_address += 6;

        } else if (const auto* returned = dynamic_cast<const sluggish::ReturnInstruction*>(instruction.get())) {

            if (instruction.get() != block.instructions.back().get()) {
                throw std::runtime_error("Return must be the final instruction in main");
            }

            next_address += 2;

            if (returned->source_slot.has_value()) {
                ++next_address;
            }

        }

        if (next_address > maximum_address + 1) {
            throw std::runtime_error("SimpleCPU code exceeds the address space");
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

        } else if (const auto* copy = dynamic_cast<const sluggish::CopyInstruction*>(instruction.get())) {

            const auto source = addresses.at(copy->source_slot);
            const auto destination = addresses.at(copy->destination_slot);

            assembly.push_back({Opcode::LoadImm, 0, source});
            assembly.push_back({Opcode::Load, 1, 0});
            assembly.push_back({Opcode::LoadImm, 0, destination});
            assembly.push_back({Opcode::Store, 0, 1});

        } else if (const auto* returned = dynamic_cast<const sluggish::ReturnInstruction*>(instruction.get())) {

            if (returned->source_slot.has_value()) {

                const auto address = addresses.at(*returned->source_slot);
                assembly.push_back({Opcode::LoadImm, 0, address});
                assembly.push_back({Opcode::Load, 0, 0});

            } else {

                assembly.push_back({Opcode::LoadImm, 0, returned->value});

            }

        } else {

            throw std::runtime_error("Unsupported instruction for SimpleCPU");

        }

    }

    assembly.push_back({Opcode::Halt});
    return assembly;

}

} // namespace simplecpu

#include "BuildAssembly.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>

namespace simplecpu {

std::vector<AssemblyFunction> build_assembly(const std::vector<sluggish::IRFunction>& functions) {

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

    std::vector<const sluggish::IRFunction*> ordered_functions{entry};

    for (const auto& function : functions) {
        if (&function != entry) {
            ordered_functions.push_back(&function);
        }
    }

    std::uint64_t next_address = 0;
    const std::uint64_t maximum_address = std::numeric_limits<std::uint32_t>::max();
    std::vector<AssemblyFunction> assembly;

    for (const auto* function : ordered_functions) {

        if (function->blocks.size() != 1) {
            throw std::runtime_error("SimpleCPU requires one basic block in " + function->name);
        }

        const auto& block = function->blocks.front();

        if (block.instructions.empty() || dynamic_cast<const sluggish::ReturnInstruction*>(block.instructions.back().get()) == nullptr) {
            throw std::runtime_error(function->name + " must end with a return");
        }

        if (next_address > maximum_address) {
            throw std::runtime_error("SimpleCPU code exceeds the address space");
        }

        const std::uint32_t address = next_address;
        assembly.push_back({function->name, address, {}});
        ++next_address; // Each function ends with a halt.

        for (const auto& instruction : block.instructions) {

            if (dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

                next_address += 3;

            } else if (dynamic_cast<const sluggish::CopyInstruction*>(instruction.get())) {

                next_address += 6;

            } else if (dynamic_cast<const sluggish::AddInstruction*>(instruction.get())) {

                next_address += 10;

            } else if (const auto* returned = dynamic_cast<const sluggish::ReturnInstruction*>(instruction.get())) {

                if (instruction.get() != block.instructions.back().get()) {
                    throw std::runtime_error("Return must be the final instruction in " + function->name);
                }

                next_address += 2;

                if (returned->source_slot.has_value()) {
                    ++next_address;
                }

            } else if (dynamic_cast<const sluggish::AllocateInstruction*>(instruction.get()) == nullptr) {

                throw std::runtime_error("Unsupported instruction for SimpleCPU in " + function->name);

            }

            if (next_address > maximum_address + 1) {
                throw std::runtime_error("SimpleCPU code exceeds the address space");
            }

        }

    }

    for (std::size_t i = 0; i < ordered_functions.size(); ++i) {

        const auto& function = *ordered_functions[i];
        auto& instructions = assembly[i].instructions;
        std::unordered_map<std::size_t, std::uint32_t> addresses;

        const auto allocate_slot = [&](std::size_t slot, const sluggish::Type* type) {

            if (type == nullptr || type->bit_width() == 0 || type->bit_width() > 32) {
                throw std::runtime_error("SimpleCPU requires types between 1 and 32 bits in " + function.name);
            }

            if (next_address > maximum_address) {
                throw std::runtime_error("SimpleCPU data exceeds the address space");
            }

            if (!addresses.emplace(slot, next_address).second) {
                throw std::runtime_error("Duplicate storage slot in " + function.name + ": " + std::to_string(slot));
            }

            ++next_address;

        };

        for (const auto& argument : function.arguments) {
            allocate_slot(argument.slot, argument.type);
        }

        for (const auto& instruction : function.blocks.front().instructions) {

            if (const auto* allocate = dynamic_cast<const sluggish::AllocateInstruction*>(instruction.get())) {

                allocate_slot(allocate->slot, allocate->type);

            } else if (const auto* store = dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

                const auto address = addresses.find(store->slot);

                if (address == addresses.end()) {
                    throw std::runtime_error("Unallocated storage slot in " + function.name + ": " + std::to_string(store->slot));
                }

                instructions.push_back({Opcode::StoreImm, address->second, store->value});

            } else if (const auto* copy = dynamic_cast<const sluggish::CopyInstruction*>(instruction.get())) {

                const auto source = addresses.at(copy->source_slot);
                const auto destination = addresses.at(copy->destination_slot);

                instructions.push_back({Opcode::LoadImm, 0, source});
                instructions.push_back({Opcode::Load, 1, 0});
                instructions.push_back({Opcode::LoadImm, 0, destination});
                instructions.push_back({Opcode::Store, 0, 1});

            } else if (const auto* add = dynamic_cast<const sluggish::AddInstruction*>(instruction.get())) {

                const auto left = addresses.at(add->left_slot);
                const auto right = addresses.at(add->right_slot);
                const auto destination = addresses.at(add->destination_slot);

                instructions.push_back({Opcode::LoadImm, 0, left});
                instructions.push_back({Opcode::Load, 1, 0});
                instructions.push_back({Opcode::LoadImm, 0, right});
                instructions.push_back({Opcode::Load, 2, 0});
                instructions.push_back({Opcode::Add, 1, 1, 2});
                instructions.push_back({Opcode::LoadImm, 0, destination});
                instructions.push_back({Opcode::Store, 0, 1});

            } else if (const auto* returned = dynamic_cast<const sluggish::ReturnInstruction*>(instruction.get())) {

                if (returned->source_slot.has_value()) {

                    const auto address = addresses.at(*returned->source_slot);
                    instructions.push_back({Opcode::LoadImm, 0, address});
                    instructions.push_back({Opcode::Load, 0, 0});

                } else {

                    instructions.push_back({Opcode::LoadImm, 0, returned->value});

                }

            }

        }

        instructions.push_back({Opcode::Halt});

    }

    return assembly;

}

} // namespace simplecpu

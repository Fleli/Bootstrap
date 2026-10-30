#include "BuildAssembly.hpp"
#include "BuildInstruction.hpp"

#include <stdexcept>

namespace simplecpu {

AssemblyFunction build_function(const sluggish::IRFunction& function,
    const std::unordered_map<std::string, std::uint32_t>& function_indices) {

    if (function.blocks.size() != 1) {
        throw std::runtime_error("SimpleCPU requires one basic block in " + function.name);
    }

    const auto& block = function.blocks.front();

    if (block.instructions.empty() || dynamic_cast<const sluggish::ReturnInstruction*>(block.instructions.back().get()) == nullptr) {
        throw std::runtime_error(function.name + " must end with a return");
    }

    std::uint32_t local_words = 0;
    std::uint32_t argument_words = 0;
    std::unordered_map<std::size_t, Slot> slots;

    const auto add_slot = [&](std::size_t slot, Slot storage) {

        if (!slots.emplace(slot, storage).second) {
            throw std::runtime_error("Duplicate storage slot in " + function.name + ": " + std::to_string(slot));
        }

    };

    // The last argument is closest to FP; each argument starts at its first word.
    for (std::size_t i = function.arguments.size(); i > 0; --i) {

        const auto& argument = function.arguments[i - 1];
        add_slot(argument.slot, {argument_words + 2, argument.type, {}});
        argument_words += storage_words(argument.type);

        if (argument_words + 3 > ram_words) {
            throw std::runtime_error("Too many argument words for SimpleCPU in " + function.name);
        }

    }

    for (const auto& instruction : block.instructions) {
        if (const auto* allocate = dynamic_cast<const sluggish::AllocateInstruction*>(instruction.get())) {

            local_words += storage_words(allocate->type);

            if (local_words + argument_words + 3 > ram_words) {
                throw std::runtime_error("Frame exceeds SimpleCPU RAM in " + function.name);
            }

            // Unsigned subtraction encodes a negative offset for the 32-bit add.
            add_slot(allocate->slot, {0u - local_words, allocate->type, {}});

        }
    }

    AssemblyFunction assembly;
    assembly.name = function.name;
    auto& instructions = assembly.instructions;

    instructions.push_back({Opcode::Push, 1});
    instructions.push_back({Opcode::LoadImm, 3, 0});
    instructions.push_back({Opcode::Add, 1, 2, 3});
    instructions.push_back({Opcode::LoadImm, 3, 0u - local_words});
    instructions.push_back({Opcode::Add, 2, 2, 3});

    for (const auto& instruction : block.instructions) {

        if (dynamic_cast<const sluggish::AllocateInstruction*>(instruction.get())) {
            continue;
        }

        if (const auto* member = dynamic_cast<const sluggish::MemberInstruction*>(instruction.get())) {

            add_slot(member->destination_slot, build_member_slot(*member, slots));

        } else if (const auto* dereference = dynamic_cast<const sluggish::DereferenceInstruction*>(instruction.get())) {

            const auto* type = slots.at(dereference->source_slot).type->pointee();
            add_slot(dereference->destination_slot, {0, type, dereference->source_slot});

        } else if (const auto* address = dynamic_cast<const sluggish::AddressOfInstruction*>(instruction.get())) {

            build_address_of(*address, slots, instructions);

        } else if (const auto* store = dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

            build_store(*store, slots, instructions);

        } else if (const auto* copy = dynamic_cast<const sluggish::CopyInstruction*>(instruction.get())) {

            build_copy(*copy, slots, instructions);

        } else if (const auto* add = dynamic_cast<const sluggish::AddInstruction*>(instruction.get())) {

            build_add(*add, slots, instructions);

        } else if (const auto* call = dynamic_cast<const sluggish::CallInstruction*>(instruction.get())) {

            build_call(*call, slots, function_indices, instructions);

        } else if (const auto* returned = dynamic_cast<const sluggish::ReturnInstruction*>(instruction.get())) {

            if (instruction.get() != block.instructions.back().get()) {
                throw std::runtime_error("Return must be the final instruction in " + function.name);
            }

            build_return(*returned, function.return_type, argument_words, slots, instructions);

        } else {

            throw std::runtime_error("Unsupported instruction for SimpleCPU in " + function.name);

        }

    }

    return assembly;

}

} // namespace simplecpu

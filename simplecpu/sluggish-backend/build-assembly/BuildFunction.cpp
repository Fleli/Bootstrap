#include "BuildAssembly.hpp"

#include <stdexcept>

namespace simplecpu {
namespace {

struct Slot {

    std::uint32_t offset;
    const sluggish::Type* type;

};

// r3 holds addresses; r4 and r5 hold values while addresses are calculated.
void load_frame_address(std::uint32_t offset, std::vector<Instruction>& instructions) {

    instructions.push_back({Opcode::LoadImm, 3, offset});
    instructions.push_back({Opcode::Add, 3, 1, 3});

}

} // namespace

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

    const auto add_slot = [&](std::size_t slot, const sluggish::Type* type, std::uint32_t offset) {

        if (!slots.emplace(slot, Slot{offset, type}).second) {
            throw std::runtime_error("Duplicate storage slot in " + function.name + ": " + std::to_string(slot));
        }

    };

    // The last argument is closest to FP; each argument starts at its first word.
    for (std::size_t i = function.arguments.size(); i > 0; --i) {

        const auto& argument = function.arguments[i - 1];
        add_slot(argument.slot, argument.type, argument_words + 2);
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
            add_slot(allocate->slot, allocate->type, 0u - local_words);

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

            const auto source = slots.at(member->source_slot);
            const auto& field = source.type->members.at(member->member_index);
            auto offset = source.offset;

            for (std::size_t i = 0; i < member->member_index; ++i) {
                offset += storage_words(source.type->members[i].type);
            }

            add_slot(member->destination_slot, field.type, offset);

        } else if (const auto* store = dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

            load_frame_address(slots.at(store->slot).offset, instructions);
            instructions.push_back({Opcode::LoadImm, 4, store->value});
            instructions.push_back({Opcode::Store, 3, 4});

        } else if (const auto* copy = dynamic_cast<const sluggish::CopyInstruction*>(instruction.get())) {

            const auto source = slots.at(copy->source_slot);
            const auto destination = slots.at(copy->destination_slot);
            const auto words = storage_words(source.type);

            for (std::uint32_t i = 0; i < words; ++i) {

                load_frame_address(source.offset + i, instructions);
                instructions.push_back({Opcode::Load, 4, 3});
                load_frame_address(destination.offset + i, instructions);
                instructions.push_back({Opcode::Store, 3, 4});

            }

        } else if (const auto* add = dynamic_cast<const sluggish::AddInstruction*>(instruction.get())) {

            load_frame_address(slots.at(add->left_slot).offset, instructions);
            instructions.push_back({Opcode::Load, 4, 3});
            load_frame_address(slots.at(add->right_slot).offset, instructions);
            instructions.push_back({Opcode::Load, 5, 3});
            instructions.push_back({Opcode::Add, 4, 4, 5});
            load_frame_address(slots.at(add->destination_slot).offset, instructions);
            instructions.push_back({Opcode::Store, 3, 4});

        } else if (const auto* call = dynamic_cast<const sluggish::CallInstruction*>(instruction.get())) {

            load_frame_address(slots.at(call->destination_slot).offset, instructions);
            instructions.push_back({Opcode::Push, 3});

            std::uint32_t pushed_words = 0;

            for (const auto slot : call->argument_slots) {

                const auto argument = slots.at(slot);
                const auto words = storage_words(argument.type);
                pushed_words += words;

                // Reverse each value's words so its first word has the lowest address.
                for (std::uint32_t i = words; i > 0; --i) {

                    load_frame_address(argument.offset + i - 1, instructions);
                    instructions.push_back({Opcode::Load, 4, 3});
                    instructions.push_back({Opcode::Push, 4});

                }

            }

            instructions.push_back({Opcode::Call, function_indices.at(call->function_name)});
            instructions.push_back({Opcode::LoadImm, 3, pushed_words + 1});
            instructions.push_back({Opcode::Add, 2, 2, 3});

        } else if (const auto* returned = dynamic_cast<const sluggish::ReturnInstruction*>(instruction.get())) {

            if (instruction.get() != block.instructions.back().get()) {
                throw std::runtime_error("Return must be the final instruction in " + function.name);
            }

            const auto words = storage_words(function.return_type);
            load_frame_address(argument_words + 2, instructions);
            instructions.push_back({Opcode::Load, 5, 3});

            for (std::uint32_t i = 0; i < words; ++i) {

                if (returned->source_slot.has_value()) {

                    load_frame_address(slots.at(*returned->source_slot).offset + i, instructions);
                    instructions.push_back({Opcode::Load, 4, 3});

                } else {

                    instructions.push_back({Opcode::LoadImm, 4, returned->value});

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

        } else {

            throw std::runtime_error("Unsupported instruction for SimpleCPU in " + function.name);

        }

    }

    return assembly;

}

} // namespace simplecpu

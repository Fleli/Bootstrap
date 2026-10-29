#include "BuildAssembly.hpp"

#include <stdexcept>

namespace simplecpu {
namespace {

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

    if (function.arguments.size() + 3 > ram_words) {
        throw std::runtime_error("Too many arguments for SimpleCPU in " + function.name);
    }

    std::uint32_t local_words = 0;
    const std::uint32_t argument_words = function.arguments.size();
    std::unordered_map<std::size_t, std::uint32_t> offsets;

    const auto allocate_slot = [&](std::size_t slot, const sluggish::Type* type, std::uint32_t offset) {

        if (type == nullptr || type->bit_width() == 0 || type->bit_width() > 32) {
            throw std::runtime_error("SimpleCPU requires types between 1 and 32 bits in " + function.name);
        }

        if (!offsets.emplace(slot, offset).second) {
            throw std::runtime_error("Duplicate storage slot in " + function.name + ": " + std::to_string(slot));
        }

    };

    for (std::size_t i = 0; i < function.arguments.size(); ++i) {

        const auto& argument = function.arguments[i];
        allocate_slot(argument.slot, argument.type, argument_words + 1 - i);

    }

    for (const auto& instruction : block.instructions) {
        if (const auto* allocate = dynamic_cast<const sluggish::AllocateInstruction*>(instruction.get())) {

            ++local_words;

            if (local_words + argument_words + 3 > ram_words) {
                throw std::runtime_error("Frame exceeds SimpleCPU RAM in " + function.name);
            }

            // Unsigned subtraction encodes a negative offset for the 32-bit add.
            allocate_slot(allocate->slot, allocate->type, 0u - local_words);

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

        if (const auto* store = dynamic_cast<const sluggish::StoreInstruction*>(instruction.get())) {

            load_frame_address(offsets.at(store->slot), instructions);
            instructions.push_back({Opcode::LoadImm, 4, store->value});
            instructions.push_back({Opcode::Store, 3, 4});

        } else if (const auto* copy = dynamic_cast<const sluggish::CopyInstruction*>(instruction.get())) {

            load_frame_address(offsets.at(copy->source_slot), instructions);
            instructions.push_back({Opcode::Load, 4, 3});
            load_frame_address(offsets.at(copy->destination_slot), instructions);
            instructions.push_back({Opcode::Store, 3, 4});

        } else if (const auto* add = dynamic_cast<const sluggish::AddInstruction*>(instruction.get())) {

            load_frame_address(offsets.at(add->left_slot), instructions);
            instructions.push_back({Opcode::Load, 4, 3});
            load_frame_address(offsets.at(add->right_slot), instructions);
            instructions.push_back({Opcode::Load, 5, 3});
            instructions.push_back({Opcode::Add, 4, 4, 5});
            load_frame_address(offsets.at(add->destination_slot), instructions);
            instructions.push_back({Opcode::Store, 3, 4});

        } else if (const auto* call = dynamic_cast<const sluggish::CallInstruction*>(instruction.get())) {

            load_frame_address(offsets.at(call->destination_slot), instructions);
            instructions.push_back({Opcode::Push, 3});

            for (const auto slot : call->argument_slots) {

                load_frame_address(offsets.at(slot), instructions);
                instructions.push_back({Opcode::Load, 4, 3});
                instructions.push_back({Opcode::Push, 4});

            }

            const std::uint32_t argument_count = call->argument_slots.size();
            instructions.push_back({Opcode::Call, function_indices.at(call->function_name)});
            instructions.push_back({Opcode::LoadImm, 3, argument_count + 1});
            instructions.push_back({Opcode::Add, 2, 2, 3});

        } else if (const auto* returned = dynamic_cast<const sluggish::ReturnInstruction*>(instruction.get())) {

            if (instruction.get() != block.instructions.back().get()) {
                throw std::runtime_error("Return must be the final instruction in " + function.name);
            }

            if (returned->source_slot.has_value()) {

                load_frame_address(offsets.at(*returned->source_slot), instructions);
                instructions.push_back({Opcode::Load, 4, 3});

            } else {

                instructions.push_back({Opcode::LoadImm, 4, returned->value});

            }

            load_frame_address(argument_words + 2, instructions);
            instructions.push_back({Opcode::Load, 3, 3});
            instructions.push_back({Opcode::Store, 3, 4});

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

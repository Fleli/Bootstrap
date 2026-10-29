#include "BasicBlock.hpp"
#include "stage-emit/Emit.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace sluggish {

namespace {

BasicBlock lower_statements(const std::vector<std::unique_ptr<ResolvedStatement>>& statements) {

    BasicBlock block;
    std::unordered_map<const ResolvedDeclaration*, std::size_t> slots;

    for (const auto& statement : statements) {

        if (const auto* declaration = dynamic_cast<const ResolvedDeclaration*>(statement.get())) {

            auto instruction = std::make_unique<AllocateInstruction>();
            instruction->slot = slots.size();
            instruction->type = declaration->type;

            slots.emplace(declaration, instruction->slot);
            block.instructions.push_back(std::move(instruction));

        } else if (const auto* assignment = dynamic_cast<const ResolvedAssignment*>(statement.get())) {

            if (assignment->source == nullptr) {

                auto instruction = std::make_unique<StoreInstruction>();
                instruction->slot = slots.at(assignment->declaration);
                instruction->value = assignment->value;
                block.instructions.push_back(std::move(instruction));

            } else {

                auto instruction = std::make_unique<CopyInstruction>();
                instruction->source_slot = slots.at(assignment->source);
                instruction->destination_slot = slots.at(assignment->declaration);
                block.instructions.push_back(std::move(instruction));

            }

        } else {

            throw std::runtime_error("Unsupported statement in lowering");

        }

    }

    std::string output = "BasicBlock 0:\n";

    for (const auto& instruction : block.instructions) {

        if (const auto* allocate = dynamic_cast<const AllocateInstruction*>(instruction.get())) {

            output += "    Allocate        s" + std::to_string(allocate->slot) + ", " + allocate->type->name() + "\n";

        } else if (const auto* store = dynamic_cast<const StoreInstruction*>(instruction.get())) {

            output += "    Store           s" + std::to_string(store->slot) + ", " + std::to_string(store->value) + "\n";

        } else if (const auto* copy = dynamic_cast<const CopyInstruction*>(instruction.get())) {

            output += "    Copy            s" + std::to_string(copy->destination_slot) + ", s" + std::to_string(copy->source_slot) + "\n";

        }

    }

    emit("lower.txt", output);

    return block;

}

} // namespace

BasicBlock lower(const Resolver& resolver) {

    if (!resolver.functions.empty()) {
        throw std::runtime_error("Function lowering is not implemented yet");
    }

    return lower_statements({});

}

} // namespace sluggish

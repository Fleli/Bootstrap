#include "BasicBlock.hpp"

#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace sluggish {

BasicBlock lower(const Resolver& resolver) {

    BasicBlock block;
    std::unordered_map<const Declaration*, std::size_t> slots;

    for (const auto& statement : resolver.statements) {

        if (const auto* declaration = dynamic_cast<const Declaration*>(statement.get())) {

            auto instruction = std::make_unique<AllocateInstruction>();
            instruction->slot = slots.size();
            instruction->type_name = declaration->type_name;

            slots.emplace(declaration, instruction->slot);
            block.instructions.push_back(std::move(instruction));

        } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement.get())) {

            auto instruction = std::make_unique<StoreInstruction>();
            instruction->slot = slots.at(assignment->declaration);
            instruction->value = assignment->value;
            block.instructions.push_back(std::move(instruction));

        } else {

            throw std::runtime_error("Unsupported statement in lowering");

        }

    }

    return block;

}

} // namespace sluggish

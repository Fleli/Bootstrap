#include "BasicBlock.hpp"

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace sluggish {

IRFunction lower_function(const ResolvedFunction& function) {

    IRFunction lowered;
    lowered.name = function.name;
    lowered.return_type = function.return_type;

    BasicBlock block;
    std::size_t next_slot = 0;
    std::unordered_map<const ResolvedDeclaration*, std::size_t> slots;

    for (const auto& argument : function.arguments) {

        const auto slot = next_slot++;
        slots.emplace(argument.get(), slot);
        lowered.arguments.push_back({slot, argument->type});

    }

    for (const auto& statement : function.statements) {

        if (const auto* declaration = dynamic_cast<const ResolvedDeclaration*>(statement.get())) {

            const auto slot = allocate_slot(declaration->type, block, next_slot);
            slots.emplace(declaration, slot);

        } else if (const auto* assignment = dynamic_cast<const ResolvedAssignment*>(statement.get())) {

            const auto destination_slot = lower_expression(assignment->target.get(), slots, block, next_slot);
            const auto* value = assignment->value.get();

            if (const auto* number = dynamic_cast<const ResolvedNumberExpression*>(value)) {

                auto instruction = std::make_unique<StoreInstruction>();
                instruction->slot = destination_slot;
                instruction->value = number->value;
                block.instructions.push_back(std::move(instruction));

            } else {

                auto instruction = std::make_unique<CopyInstruction>();
                instruction->source_slot = lower_expression(value, slots, block, next_slot);
                instruction->destination_slot = destination_slot;
                block.instructions.push_back(std::move(instruction));

            }

        } else if (const auto* returned = dynamic_cast<const ResolvedReturn*>(statement.get())) {

            const auto* value = returned->value.get();
            auto instruction = std::make_unique<ReturnInstruction>();

            if (const auto* number = dynamic_cast<const ResolvedNumberExpression*>(value)) {

                instruction->value = number->value;

            } else {

                instruction->source_slot = lower_expression(value, slots, block, next_slot);

            }

            block.instructions.push_back(std::move(instruction));
            break;

        } else {

            throw std::runtime_error("Unsupported statement in lowering");

        }

    }

    lowered.blocks.push_back(std::move(block));
    return lowered;

}

} // namespace sluggish

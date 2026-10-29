#include "BasicBlock.hpp"
#include "stage-emit/Emit.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace sluggish {

namespace {

std::size_t allocate_slot(const Type* type, BasicBlock& block, std::size_t& next_slot) {

    const auto slot = next_slot++;
    auto instruction = std::make_unique<AllocateInstruction>();
    instruction->slot = slot;
    instruction->type = type;
    block.instructions.push_back(std::move(instruction));
    return slot;

}

std::size_t lower_expression(const ResolvedExpression* expression,
    const std::unordered_map<const ResolvedDeclaration*, std::size_t>& slots,
    BasicBlock& block, std::size_t& next_slot) {

    if (const auto* variable = dynamic_cast<const ResolvedVariableExpression*>(expression)) {
        return slots.at(variable->declaration);
    }

    if (const auto* number = dynamic_cast<const ResolvedNumberExpression*>(expression)) {

        const auto slot = allocate_slot(number->type, block, next_slot);
        auto instruction = std::make_unique<StoreInstruction>();
        instruction->slot = slot;
        instruction->value = number->value;
        block.instructions.push_back(std::move(instruction));
        return slot;

    }

    if (const auto* addition = dynamic_cast<const ResolvedAdditionExpression*>(expression)) {

        const auto left_slot = lower_expression(addition->left.get(), slots, block, next_slot);
        const auto right_slot = lower_expression(addition->right.get(), slots, block, next_slot);
        const auto destination_slot = allocate_slot(addition->type, block, next_slot);

        auto instruction = std::make_unique<AddInstruction>();
        instruction->left_slot = left_slot;
        instruction->right_slot = right_slot;
        instruction->destination_slot = destination_slot;
        block.instructions.push_back(std::move(instruction));
        return destination_slot;

    }

    throw std::runtime_error("Unsupported expression in lowering");

}

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

            const auto* value = assignment->value.get();

            if (const auto* number = dynamic_cast<const ResolvedNumberExpression*>(value)) {

                auto instruction = std::make_unique<StoreInstruction>();
                instruction->slot = slots.at(assignment->declaration);
                instruction->value = number->value;
                block.instructions.push_back(std::move(instruction));

            } else {

                auto instruction = std::make_unique<CopyInstruction>();
                instruction->source_slot = lower_expression(value, slots, block, next_slot);
                instruction->destination_slot = slots.at(assignment->declaration);
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

} // namespace

std::vector<IRFunction> lower(const Resolver& resolver) {

    std::vector<IRFunction> functions;

    for (const auto& function : resolver.functions) {
        functions.push_back(lower_function(*function));
    }

    std::string output;

    for (const auto& function : functions) {

        output += "Function        " + function.return_type->name() + " " + function.name + "\n";

        for (const auto& argument : function.arguments) {
            output += "    Argument        s" + std::to_string(argument.slot) + ", " + argument.type->name() + "\n";
        }

        for (std::size_t i = 0; i < function.blocks.size(); ++i) {

            output += "    BasicBlock " + std::to_string(i) + ":\n";

            for (const auto& instruction : function.blocks[i].instructions) {

                if (const auto* allocate = dynamic_cast<const AllocateInstruction*>(instruction.get())) {

                    output += "        Allocate        s" + std::to_string(allocate->slot) + ", " + allocate->type->name() + "\n";

                } else if (const auto* store = dynamic_cast<const StoreInstruction*>(instruction.get())) {

                    output += "        Store           s" + std::to_string(store->slot) + ", " + std::to_string(store->value) + "\n";

                } else if (const auto* copy = dynamic_cast<const CopyInstruction*>(instruction.get())) {

                    output += "        Copy            s" + std::to_string(copy->destination_slot) + ", s" + std::to_string(copy->source_slot) + "\n";

                } else if (const auto* add = dynamic_cast<const AddInstruction*>(instruction.get())) {

                    output += "        Add             s" + std::to_string(add->destination_slot) + ", s" + std::to_string(add->left_slot) + ", s" + std::to_string(add->right_slot) + "\n";

                } else if (const auto* returned = dynamic_cast<const ReturnInstruction*>(instruction.get())) {

                    output += "        Return          ";

                    if (returned->source_slot.has_value()) {
                        output += "s" + std::to_string(*returned->source_slot);
                    } else {
                        output += std::to_string(returned->value);
                    }

                    output += "\n";

                }

            }

        }

    }

    emit("lower.txt", output);
    return functions;

}

} // namespace sluggish

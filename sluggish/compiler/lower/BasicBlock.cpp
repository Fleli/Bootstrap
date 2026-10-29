#include "BasicBlock.hpp"
#include "stage-emit/Emit.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace sluggish {

namespace {

IRFunction lower_function(const ResolvedFunction& function) {

    IRFunction lowered;
    lowered.name = function.name;
    lowered.return_type = function.return_type;

    BasicBlock block;
    std::unordered_map<const ResolvedDeclaration*, std::size_t> slots;

    for (const auto& argument : function.arguments) {

        const auto slot = slots.size();
        slots.emplace(argument.get(), slot);
        lowered.arguments.push_back({slot, argument->type});

    }

    for (const auto& statement : function.statements) {

        if (const auto* declaration = dynamic_cast<const ResolvedDeclaration*>(statement.get())) {

            auto instruction = std::make_unique<AllocateInstruction>();
            instruction->slot = slots.size();
            instruction->type = declaration->type;

            slots.emplace(declaration, instruction->slot);
            block.instructions.push_back(std::move(instruction));

        } else if (const auto* assignment = dynamic_cast<const ResolvedAssignment*>(statement.get())) {

            const auto* value = assignment->value.get();

            if (const auto* number = dynamic_cast<const ResolvedNumberExpression*>(value)) {

                auto instruction = std::make_unique<StoreInstruction>();
                instruction->slot = slots.at(assignment->declaration);
                instruction->value = number->value;
                block.instructions.push_back(std::move(instruction));

            } else if (const auto* variable = dynamic_cast<const ResolvedVariableExpression*>(value)) {

                auto instruction = std::make_unique<CopyInstruction>();
                instruction->source_slot = slots.at(variable->declaration);
                instruction->destination_slot = slots.at(assignment->declaration);
                block.instructions.push_back(std::move(instruction));

            } else {

                throw std::runtime_error("Unsupported assignment expression in lowering");

            }

        } else if (const auto* returned = dynamic_cast<const ResolvedReturn*>(statement.get())) {

            const auto* value = returned->value.get();
            auto instruction = std::make_unique<ReturnInstruction>();

            if (const auto* number = dynamic_cast<const ResolvedNumberExpression*>(value)) {

                instruction->value = number->value;

            } else if (const auto* variable = dynamic_cast<const ResolvedVariableExpression*>(value)) {

                instruction->source_slot = slots.at(variable->declaration);

            } else {

                throw std::runtime_error("Unsupported return expression in lowering");

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

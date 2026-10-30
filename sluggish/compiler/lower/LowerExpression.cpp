#include "BasicBlock.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sluggish {

std::size_t lower_expression(const ResolvedExpression* expression,
    const std::unordered_map<const ResolvedDeclaration*, std::size_t>& slots,
    BasicBlock& block, std::size_t& next_slot) {

    if (const auto* variable = dynamic_cast<const ResolvedVariableExpression*>(expression)) {
        return slots.at(variable->declaration);
    }

    if (const auto* address = dynamic_cast<const ResolvedAddressOfExpression*>(expression)) {

        const auto source_slot = lower_expression(address->operand.get(), slots, block, next_slot);
        const auto destination_slot = allocate_slot(address->type, block, next_slot);

        auto instruction = std::make_unique<AddressOfInstruction>();
        instruction->source_slot = source_slot;
        instruction->destination_slot = destination_slot;
        block.instructions.push_back(std::move(instruction));
        return destination_slot;

    }

    if (const auto* dereference = dynamic_cast<const ResolvedDereferenceExpression*>(expression)) {

        const auto source_slot = lower_value(dereference->operand.get(), slots, block, next_slot);
        const auto destination_slot = next_slot++;

        auto instruction = std::make_unique<DereferenceInstruction>();
        instruction->source_slot = source_slot;
        instruction->destination_slot = destination_slot;
        block.instructions.push_back(std::move(instruction));
        return destination_slot;

    }

    if (const auto* member = dynamic_cast<const ResolvedMemberExpression*>(expression)) {

        const auto source_slot = lower_expression(member->object.get(), slots, block, next_slot);
        const auto destination_slot = next_slot++;

        auto instruction = std::make_unique<MemberInstruction>();
        instruction->source_slot = source_slot;
        instruction->member_index = member->member_index;
        instruction->destination_slot = destination_slot;
        block.instructions.push_back(std::move(instruction));
        return destination_slot;

    }

    if (const auto* number = dynamic_cast<const ResolvedNumberExpression*>(expression)) {

        const auto slot = allocate_slot(number->type, block, next_slot);
        auto instruction = std::make_unique<StoreInstruction>();
        instruction->slot = slot;
        instruction->value = number->value;
        block.instructions.push_back(std::move(instruction));
        return slot;

    }

    if (const auto* call = dynamic_cast<const ResolvedCallExpression*>(expression)) {

        auto instruction = std::make_unique<CallInstruction>();
        instruction->function_name = call->function->name;

        for (const auto& argument : call->arguments) {
            instruction->argument_slots.push_back(lower_value(argument.get(), slots, block, next_slot));
        }

        const auto destination_slot = allocate_slot(call->type, block, next_slot);
        instruction->destination_slot = destination_slot;
        block.instructions.push_back(std::move(instruction));
        return destination_slot;

    }

    if (const auto* addition = dynamic_cast<const ResolvedAdditionExpression*>(expression)) {

        const auto left_slot = lower_value(addition->left.get(), slots, block, next_slot);
        const auto right_slot = lower_value(addition->right.get(), slots, block, next_slot);
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

} // namespace sluggish

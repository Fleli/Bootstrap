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

} // namespace sluggish

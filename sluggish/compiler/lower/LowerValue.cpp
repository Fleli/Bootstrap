#include "BasicBlock.hpp"

#include <memory>
#include <utility>

namespace sluggish {

std::size_t lower_value(const ResolvedExpression* expression,
    const std::unordered_map<const ResolvedDeclaration*, std::size_t>& slots,
    BasicBlock& block, std::size_t& next_slot) {

    const auto source_slot = lower_expression(expression, slots, block, next_slot);

    if (!expression->is_lvalue()) {
        return source_slot;
    }

    const auto destination_slot = allocate_slot(expression->type, block, next_slot);

    auto instruction = std::make_unique<CopyInstruction>();
    instruction->source_slot = source_slot;
    instruction->destination_slot = destination_slot;
    block.instructions.push_back(std::move(instruction));
    return destination_slot;

}

} // namespace sluggish

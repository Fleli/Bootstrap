#include "BasicBlock.hpp"

#include <memory>
#include <utility>

namespace sluggish {

std::size_t allocate_slot(const Type* type, BasicBlock& block, std::size_t& next_slot) {

    const auto slot = next_slot++;
    auto instruction = std::make_unique<AllocateInstruction>();
    instruction->slot = slot;
    instruction->type = type;
    block.instructions.push_back(std::move(instruction));
    return slot;

}

} // namespace sluggish

#pragma once

#include "IR.hpp"
#include "resolver/Resolver.hpp"

#include <cstddef>
#include <unordered_map>

namespace sluggish {

// Lowers each function into one block and writes intermediate/lower.txt.
std::vector<IRFunction> lower(const Resolver& resolver);

IRFunction lower_function(const ResolvedFunction& function);

std::size_t allocate_slot(const Type* type, BasicBlock& block, std::size_t& next_slot);

std::size_t lower_expression(const ResolvedExpression* expression,
    const std::unordered_map<const ResolvedDeclaration*, std::size_t>& slots,
    BasicBlock& block, std::size_t& next_slot);

} // namespace sluggish

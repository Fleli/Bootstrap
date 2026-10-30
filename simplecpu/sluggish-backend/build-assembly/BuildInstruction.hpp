#pragma once

#include "BuildAssembly.hpp"
#include "Slot.hpp"

namespace simplecpu {

Slot build_member_slot(const sluggish::MemberInstruction& member,
    const std::unordered_map<std::size_t, Slot>& slots);

void build_address_of(const sluggish::AddressOfInstruction& address,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions);

void build_store(const sluggish::StoreInstruction& store,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions);

void build_copy(const sluggish::CopyInstruction& copy,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions);

void build_add(const sluggish::AddInstruction& add,
    const std::unordered_map<std::size_t, Slot>& slots, std::vector<Instruction>& instructions);

void build_call(const sluggish::CallInstruction& call,
    const std::unordered_map<std::size_t, Slot>& slots,
    const std::unordered_map<std::string, std::uint32_t>& function_indices,
    std::vector<Instruction>& instructions);

void build_return(const sluggish::ReturnInstruction& returned, const sluggish::Type* return_type,
    std::uint32_t argument_words, const std::unordered_map<std::size_t, Slot>& slots,
    std::vector<Instruction>& instructions);

} // namespace simplecpu

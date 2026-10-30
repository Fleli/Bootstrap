#include "BuildInstruction.hpp"

namespace simplecpu {

Slot build_member_slot(const sluggish::MemberInstruction& member,
    const std::unordered_map<std::size_t, Slot>& slots) {

    auto storage = slots.at(member.source_slot);
    const auto& field = storage.type->members.at(member.member_index);

    for (std::size_t i = 0; i < member.member_index; ++i) {
        storage.offset += storage_words(storage.type->members[i].type);
    }

    storage.type = field.type;
    return storage;

}

} // namespace simplecpu

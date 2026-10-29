#include "Resolver.hpp"

#include <stdexcept>

namespace sluggish {

void Resolver::resolve_struct_members(const StructDefinition& definition) {

    auto& type = *types.structs.at(definition.name);

    for (const auto& member : definition.members) {

        if (!type.member_indices.emplace(member.name, type.members.size()).second) {
            throw std::runtime_error("Duplicate member: " + definition.name + "." + member.name);
        }

        type.members.push_back({member.name, types.resolve(member.type_name)});

    }

}

} // namespace sluggish

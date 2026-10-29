#include "Resolver.hpp"

#include <stdexcept>
#include <unordered_set>

namespace sluggish {

void Resolver::resolve_struct_members(const StructDefinition& definition) {

    auto& type = *types.structs.at(definition.name);
    std::unordered_set<std::string> names;

    for (const auto& member : definition.members) {

        if (!names.insert(member.name).second) {
            throw std::runtime_error("Duplicate member: " + definition.name + "." + member.name);
        }

        type.members.push_back({member.name, types.resolve(member.type_name)});

    }

}

} // namespace sluggish

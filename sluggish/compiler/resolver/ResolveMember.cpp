#include "Resolver.hpp"

#include <memory>
#include <stdexcept>

namespace sluggish {

std::unique_ptr<ResolvedExpression> Resolver::resolve_member_expression(const MemberExpression& member,
    const Scope& scope, const Type* expected_type) {

    auto resolved = std::make_unique<ResolvedMemberExpression>();
    resolved->object = resolve_expression(member.object.get(), scope);
    const auto* type = resolved->object->type;

    if (!type->is_struct()) {
        throw std::runtime_error("Type " + type->name() + " has no members");
    }

    const auto found = type->member_indices.find(member.name);

    if (found == type->member_indices.end()) {
        throw std::runtime_error("Unknown member: " + type->name() + "." + member.name);
    }

    resolved->member_index = found->second;
    resolved->type = type->members[resolved->member_index].type;

    if (expected_type != nullptr && resolved->type != expected_type) {
        throw std::runtime_error("Expected " + expected_type->name() + ", got " + resolved->type->name());
    }

    return resolved;

}

} // namespace sluggish

#include "Resolver.hpp"

#include <memory>
#include <stdexcept>

namespace sluggish {

std::unique_ptr<ResolvedExpression> Resolver::resolve_address_of_expression(const AddressOfExpression& address,
    const Scope& scope, const Type* expected_type) {

    auto resolved = std::make_unique<ResolvedAddressOfExpression>();
    resolved->operand = resolve_expression(address.operand.get(), scope);

    if (!resolved->operand->is_lvalue()) {
        throw std::runtime_error("Address-of requires an lvalue");
    }

    resolved->type = types.pointer_to(resolved->operand->type);

    if (expected_type != nullptr && resolved->type != expected_type) {
        throw std::runtime_error("Expected " + expected_type->name() + ", got " + resolved->type->name());
    }

    return resolved;

}

std::unique_ptr<ResolvedExpression> Resolver::resolve_dereference_expression(const DereferenceExpression& dereference,
    const Scope& scope, const Type* expected_type) {

    auto resolved = std::make_unique<ResolvedDereferenceExpression>();
    resolved->operand = resolve_expression(dereference.operand.get(), scope);
    const auto* type = resolved->operand->type;

    if (!type->is_pointer()) {
        throw std::runtime_error("Cannot dereference " + type->name());
    }

    resolved->type = type->pointee();

    if (expected_type != nullptr && resolved->type != expected_type) {
        throw std::runtime_error("Expected " + expected_type->name() + ", got " + resolved->type->name());
    }

    return resolved;

}

} // namespace sluggish

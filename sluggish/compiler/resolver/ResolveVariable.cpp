#include "Resolver.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sluggish {

std::unique_ptr<ResolvedExpression> Resolver::resolve_variable_expression(const VariableExpression& variable,
    const Scope& scope, const Type* expected_type) {

    const auto* declaration = scope.find(variable.name);

    if (declaration == nullptr) {
        throw std::runtime_error("Undeclared variable: " + variable.name);
    }

    auto reference = std::make_unique<ResolvedVariableExpression>();
    reference->type = declaration->type;
    reference->declaration = declaration;
    std::unique_ptr<ResolvedExpression> resolved = std::move(reference);

    if (declaration->is_self) {

        auto self = std::make_unique<ResolvedDereferenceExpression>();
        self->type = declaration->type->pointee();
        self->operand = std::move(resolved);
        resolved = std::move(self);

    }

    if (expected_type != nullptr && resolved->type != expected_type) {
        throw std::runtime_error("Expected " + expected_type->name() + ", got " + resolved->type->name());
    }

    return resolved;

}

} // namespace sluggish

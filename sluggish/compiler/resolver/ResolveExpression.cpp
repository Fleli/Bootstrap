#include "Resolver.hpp"

#include <memory>
#include <stdexcept>

namespace sluggish {

std::unique_ptr<ResolvedExpression> resolve_expression(const Expression* expression, const Scope& scope, const Type* expected_type) {

    if (const auto* number = dynamic_cast<const NumberExpression*>(expression)) {

        auto resolved = std::make_unique<ResolvedNumberExpression>();
        resolved->type = expected_type;
        resolved->value = expected_type->parse_literal(number->value);
        return resolved;

    }

    if (const auto* variable = dynamic_cast<const VariableExpression*>(expression)) {

        const auto* declaration = scope.find(variable->name);

        if (declaration == nullptr) {
            throw std::runtime_error("Undeclared variable: " + variable->name);
        }

        if (declaration->type != expected_type) {
            throw std::runtime_error("Expected " + expected_type->name() + ", got " + declaration->type->name());
        }

        auto resolved = std::make_unique<ResolvedVariableExpression>();
        resolved->type = declaration->type;
        resolved->declaration = declaration;
        return resolved;

    }

    if (const auto* addition = dynamic_cast<const AdditionExpression*>(expression)) {

        auto resolved = std::make_unique<ResolvedAdditionExpression>();
        resolved->left = resolve_expression(addition->left.get(), scope, expected_type);
        resolved->right = resolve_expression(addition->right.get(), scope, resolved->left->type);
        resolved->type = resolved->left->type;
        return resolved;

    }

    throw std::runtime_error("Unsupported expression in resolver");

}

} // namespace sluggish

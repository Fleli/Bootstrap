#include "Resolver.hpp"

#include <memory>
#include <stdexcept>

namespace sluggish {

std::unique_ptr<ResolvedExpression> Resolver::resolve_expression(const Expression* expression, const Scope& scope, const Type* expected_type) {

    if (const auto* number = dynamic_cast<const NumberExpression*>(expression)) {

        if (expected_type == nullptr) {
            throw std::runtime_error("A number requires an expected word type");
        }

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

        if (expected_type != nullptr && declaration->type != expected_type) {
            throw std::runtime_error("Expected " + expected_type->name() + ", got " + declaration->type->name());
        }

        auto resolved = std::make_unique<ResolvedVariableExpression>();
        resolved->type = declaration->type;
        resolved->declaration = declaration;
        return resolved;

    }

    if (const auto* member = dynamic_cast<const MemberExpression*>(expression)) {
        return resolve_member_expression(*member, scope, expected_type);
    }

    if (const auto* address = dynamic_cast<const AddressOfExpression*>(expression)) {
        return resolve_address_of_expression(*address, scope, expected_type);
    }

    if (const auto* dereference = dynamic_cast<const DereferenceExpression*>(expression)) {
        return resolve_dereference_expression(*dereference, scope, expected_type);
    }

    if (const auto* call = dynamic_cast<const CallExpression*>(expression)) {

        if (scope.find(call->name) != nullptr) {
            throw std::runtime_error("Not callable: " + call->name);
        }

        const auto symbol = function_symbols.find(call->name);

        if (symbol == function_symbols.end()) {
            throw std::runtime_error("Unknown function: " + call->name);
        }

        const auto* function = symbol->second;

        if (call->arguments.size() != function->arguments.size()) {
            throw std::runtime_error("Wrong argument count for " + call->name + ": expected " +
                std::to_string(function->arguments.size()) + ", got " + std::to_string(call->arguments.size()));
        }

        if (expected_type != nullptr && function->return_type != expected_type) {
            throw std::runtime_error("Expected " + expected_type->name() + ", got " + function->return_type->name());
        }

        auto resolved = std::make_unique<ResolvedCallExpression>();
        resolved->type = function->return_type;
        resolved->function = function;

        for (std::size_t i = 0; i < call->arguments.size(); ++i) {
            resolved->arguments.push_back(resolve_expression(call->arguments[i].get(), scope, function->arguments[i]->type));
        }

        return resolved;

    }

    if (const auto* addition = dynamic_cast<const AdditionExpression*>(expression)) {

        auto resolved = std::make_unique<ResolvedAdditionExpression>();
        resolved->left = resolve_expression(addition->left.get(), scope, expected_type);

        if (!resolved->left->type->is_word()) {
            throw std::runtime_error("Addition requires word operands");
        }

        resolved->right = resolve_expression(addition->right.get(), scope, resolved->left->type);
        resolved->type = resolved->left->type;
        return resolved;

    }

    throw std::runtime_error("Unsupported expression in resolver");

}

} // namespace sluggish

#include "Resolver.hpp"

#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace sluggish {

namespace {

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

    throw std::runtime_error("Unsupported expression in resolver");

}

} // namespace

Resolver::Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements) {

    std::unordered_set<std::string> names;

    for (const auto& statement : parsed_statements) {

        const auto* function = dynamic_cast<const Function*>(statement.get());

        if (function == nullptr) {
            throw std::runtime_error("Expected a function in resolver");
        }

        if (!names.insert(function->name).second) {
            throw std::runtime_error("Duplicate function: " + function->name);
        }

        auto resolved = std::make_unique<ResolvedFunction>();
        resolved->name = function->name;
        resolved->return_type = resolve_type(function->return_type_name);

        Scope function_scope;

        for (const auto& argument : function->arguments) {

            if (function_scope.symbols.find(argument.name) != function_scope.symbols.end()) {
                throw std::runtime_error("Duplicate argument: " + argument.name);
            }

            auto declaration = std::make_unique<ResolvedDeclaration>();
            declaration->name = argument.name;
            declaration->type = resolve_type(argument.type_name);

            function_scope.symbols.emplace(declaration->name, declaration.get());
            resolved->arguments.push_back(std::move(declaration));

        }

        resolve(function->body.get(), function_scope, *resolved);

        bool has_return = false;

        for (const auto& body_statement : resolved->statements) {
            if (dynamic_cast<const ResolvedReturn*>(body_statement.get()) != nullptr) {
                has_return = true;
                break;
            }
        }

        if (!has_return) {
            throw std::runtime_error("Missing return in function: " + function->name);
        }

        functions.push_back(std::move(resolved));

    }

}

void Resolver::resolve(const Statement* statement, Scope& current_scope, ResolvedFunction& function) {

    if (const auto* block = dynamic_cast<const Block*>(statement)) {

        Scope child;
        child.parent = &current_scope;

        for (const auto& child_statement : block->statements) {
            resolve(child_statement.get(), child, function);
        }

        return;

    }

    if (const auto* returned = dynamic_cast<const Return*>(statement)) {

        auto resolved = std::make_unique<ResolvedReturn>();
        resolved->value = resolve_expression(returned->value.get(), current_scope, function.return_type);

        function.statements.push_back(std::move(resolved));
        return;

    }

    const Expression* value = nullptr;
    const ResolvedDeclaration* target = nullptr;

    if (const auto* declaration = dynamic_cast<const Declaration*>(statement)) {

        if (current_scope.symbols.find(declaration->name) != current_scope.symbols.end()) {
            throw std::runtime_error("Duplicate declaration: " + declaration->name);
        }

        auto resolved = std::make_unique<ResolvedDeclaration>();
        resolved->name = declaration->name;
        resolved->type = resolve_type(declaration->type_name);

        target = resolved.get();

        current_scope.symbols.emplace(target->name, target);
        function.statements.push_back(std::move(resolved));

        if (declaration->initializer == nullptr) {
            return;
        }

        value = declaration->initializer.get();

    } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement)) {

        target = current_scope.find(assignment->name);

        if (target == nullptr) {
            throw std::runtime_error("Undeclared variable: " + assignment->name);
        }

        value = assignment->value.get();

    } else {

        throw std::runtime_error("Unsupported statement in resolver");

    }

    auto resolved = std::make_unique<ResolvedAssignment>();
    resolved->declaration = target;
    resolved->value = resolve_expression(value, current_scope, target->type);

    function.statements.push_back(std::move(resolved));

}

} // namespace sluggish

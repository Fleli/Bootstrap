#include "Resolver.hpp"

#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace sluggish {

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
        const auto& value = returned->value;

        if (value.type == TokenType::Number) {

            resolved->value = function.return_type->parse_literal(value.body);

        } else if (value.type == TokenType::Symbol) {

            resolved->source = current_scope.find(value.body);

            if (resolved->source == nullptr) {
                throw std::runtime_error("Undeclared variable: " + value.body);
            }

            if (resolved->source->type != function.return_type) {
                throw std::runtime_error("Cannot return " + resolved->source->type->name() + " from " + function.return_type->name() + " function " + function.name);
            }

        } else {

            throw std::runtime_error("Unsupported return value in resolver");

        }

        function.statements.push_back(std::move(resolved));
        return;

    }

    Token value{};
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

        if (declaration->initializer.body.empty()) {
            return;
        }

        value = declaration->initializer;

    } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement)) {

        target = current_scope.find(assignment->name);

        if (target == nullptr) {
            throw std::runtime_error("Undeclared variable: " + assignment->name);
        }

        value = assignment->value;

    } else {

        throw std::runtime_error("Unsupported statement in resolver");

    }

    auto resolved = std::make_unique<ResolvedAssignment>();
    resolved->declaration = target;

    if (value.type == TokenType::Number) {

        resolved->value = target->type->parse_literal(value.body);

    } else if (value.type == TokenType::Symbol) {

        resolved->source = current_scope.find(value.body);

        if (resolved->source == nullptr) {
            throw std::runtime_error("Undeclared variable: " + value.body);
        }

        if (resolved->source->type != target->type) {
            throw std::runtime_error("Cannot assign " + resolved->source->type->name() + " to " + target->type->name());
        }

    } else {

        throw std::runtime_error("Unsupported assignment value in resolver");

    }

    function.statements.push_back(std::move(resolved));

}

} // namespace sluggish

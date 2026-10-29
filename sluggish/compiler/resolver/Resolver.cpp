#include "Resolver.hpp"

#include <stdexcept>
#include <utility>

namespace sluggish {

Resolver::Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements) {

    for (const auto& statement : parsed_statements) {
        if (const auto* definition = dynamic_cast<const StructDefinition*>(statement.get())) {
            types.register_struct(definition->name);
        }
    }

    for (const auto& statement : parsed_statements) {
        if (const auto* definition = dynamic_cast<const StructDefinition*>(statement.get())) {
            resolve_struct_members(*definition);
        }
    }

    types.check_recursive_members();

    for (const auto& statement : parsed_statements) {
        if (dynamic_cast<const StructDefinition*>(statement.get()) == nullptr) {
            register_function_signature(statement.get());
        }
    }

    for (const auto& statement : parsed_statements) {
        if (dynamic_cast<const Function*>(statement.get()) != nullptr) {
            resolve_function_body(statement.get());
        }
    }

}

void Resolver::register_function_signature(const Statement* statement) {

    const auto* function = dynamic_cast<const Function*>(statement);

    if (function == nullptr) {
        throw std::runtime_error("Expected a function in resolver");
    }

    if (function_symbols.find(function->name) != function_symbols.end()) {
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

    function_symbols.emplace(resolved->name, resolved.get());
    functions.push_back(std::move(resolved));

}

void Resolver::resolve_function_body(const Statement* statement) {

    const auto* function = dynamic_cast<const Function*>(statement);
    auto* resolved = function_symbols.at(function->name);

    Scope function_scope;

    for (const auto& argument : resolved->arguments) {
        function_scope.symbols.emplace(argument->name, argument.get());
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

}

} // namespace sluggish

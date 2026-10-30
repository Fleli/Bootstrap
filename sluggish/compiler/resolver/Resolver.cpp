#include "Resolver.hpp"

#include <stdexcept>

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

        if (const auto* definition = dynamic_cast<const StructDefinition*>(statement.get())) {
            register_method_signatures(*definition);
        } else {
            register_function_signature(statement.get());
        }

    }

    for (const auto& statement : parsed_statements) {
        if (dynamic_cast<const Function*>(statement.get()) != nullptr) {
            resolve_function_body(statement.get());
        }
    }

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

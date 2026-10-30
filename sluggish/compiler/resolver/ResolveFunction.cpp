#include "Resolver.hpp"

#include <stdexcept>

namespace sluggish {

void Resolver::resolve_method_bodies(const StructDefinition& definition) {

    const auto* owner = types.structs.at(definition.name).get();

    for (const auto& method : definition.methods) {
        resolve_function_body(method.get(), owner);
    }

}

void Resolver::resolve_function_body(const Statement* statement, const Type* owner) {

    const auto* function = dynamic_cast<const Function*>(statement);
    const auto& symbols = owner == nullptr ? function_symbols : method_symbols.at(owner);
    auto* resolved = symbols.at(function->name);

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
        throw std::runtime_error("Missing return in function: " + resolved->name);
    }

}

} // namespace sluggish

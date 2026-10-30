#include "Resolver.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sluggish {

void Resolver::register_method_signatures(const StructDefinition& definition) {

    const auto* owner = types.structs.at(definition.name).get();

    for (const auto& method : definition.methods) {
        register_function_signature(method.get(), owner);
    }

}

void Resolver::register_function_signature(const Statement* statement, const Type* owner) {

    const auto* function = dynamic_cast<const Function*>(statement);

    if (function == nullptr) {
        throw std::runtime_error("Expected a function in resolver");
    }

    auto& symbols = owner == nullptr ? function_symbols : method_symbols[owner];
    const auto name = owner == nullptr ? function->name : owner->name() + "." + function->name;

    if (symbols.find(function->name) != symbols.end()) {
        throw std::runtime_error("Duplicate function: " + name);
    }

    auto resolved = std::make_unique<ResolvedFunction>();
    resolved->name = name;
    resolved->return_type = types.resolve(function->return_type_name);

    Scope function_scope;

    if (owner != nullptr) {

        auto self = std::make_unique<ResolvedDeclaration>();
        self->name = "self";
        self->type = types.pointer_to(owner);
        function_scope.symbols.emplace(self->name, self.get());
        resolved->arguments.push_back(std::move(self));

    }

    for (const auto& argument : function->arguments) {

        if (function_scope.symbols.find(argument.name) != function_scope.symbols.end()) {
            throw std::runtime_error("Duplicate argument: " + argument.name);
        }

        auto declaration = std::make_unique<ResolvedDeclaration>();
        declaration->name = argument.name;
        declaration->type = types.resolve(argument.type_name);

        function_scope.symbols.emplace(declaration->name, declaration.get());
        resolved->arguments.push_back(std::move(declaration));

    }

    symbols.emplace(function->name, resolved.get());
    functions.push_back(std::move(resolved));

}

} // namespace sluggish

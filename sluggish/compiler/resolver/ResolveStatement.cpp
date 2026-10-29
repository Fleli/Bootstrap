#include "Resolver.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sluggish {

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
    std::unique_ptr<ResolvedExpression> target;

    if (const auto* declaration = dynamic_cast<const Declaration*>(statement)) {

        if (current_scope.symbols.find(declaration->name) != current_scope.symbols.end()) {
            throw std::runtime_error("Duplicate declaration: " + declaration->name);
        }

        auto resolved = std::make_unique<ResolvedDeclaration>();
        resolved->name = declaration->name;
        resolved->type = types.resolve(declaration->type_name);

        const auto* binding = resolved.get();

        current_scope.symbols.emplace(binding->name, binding);
        function.statements.push_back(std::move(resolved));

        if (declaration->initializer == nullptr) {
            return;
        }

        auto variable = std::make_unique<ResolvedVariableExpression>();
        variable->type = binding->type;
        variable->declaration = binding;
        target = std::move(variable);
        value = declaration->initializer.get();

    } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement)) {

        target = resolve_expression(assignment->target.get(), current_scope);

        if (!target->is_lvalue()) {
            throw std::runtime_error("Assignment target is not an lvalue");
        }

        value = assignment->value.get();

    } else {

        throw std::runtime_error("Unsupported statement in resolver");

    }

    auto resolved = std::make_unique<ResolvedAssignment>();
    resolved->target = std::move(target);
    resolved->value = resolve_expression(value, current_scope, resolved->target->type);

    function.statements.push_back(std::move(resolved));

}

} // namespace sluggish

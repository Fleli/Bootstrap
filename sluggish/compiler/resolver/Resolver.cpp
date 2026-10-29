#include "Resolver.hpp"

#include <stdexcept>
#include <utility>

namespace sluggish {

Resolver::Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements) {

    for (const auto& statement : parsed_statements) {

        Token value{};
        const ResolvedDeclaration* target = nullptr;

        if (const auto* declaration = dynamic_cast<const Declaration*>(statement.get())) {

            if (scope.find(declaration->name) != nullptr) {
                throw std::runtime_error("Duplicate declaration: " + declaration->name);
            }

            auto resolved = std::make_unique<ResolvedDeclaration>();
            resolved->name = declaration->name;
            resolved->type = resolve_type(declaration->type_name);

            target = resolved.get();

            scope.symbols.emplace(target->name, target);
            statements.push_back(std::move(resolved));

            if (declaration->initializer.body.empty()) {
                continue;
            }

            value = declaration->initializer;

        } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement.get())) {

            target = scope.find(assignment->name);

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

            resolved->source = scope.find(value.body);

            if (resolved->source == nullptr) {
                throw std::runtime_error("Undeclared variable: " + value.body);
            }

            if (resolved->source->type != target->type) {
                throw std::runtime_error("Cannot assign " + resolved->source->type->name() + " to " + target->type->name());
            }

        } else {

            throw std::runtime_error("Unsupported assignment value in resolver");

        }

        statements.push_back(std::move(resolved));

    }

}

} // namespace sluggish

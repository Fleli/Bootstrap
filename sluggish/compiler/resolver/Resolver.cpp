#include "Resolver.hpp"

#include <stdexcept>
#include <utility>

namespace sluggish {

Resolver::Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements) {

    for (const auto& statement : parsed_statements) {

        if (const auto* declaration = dynamic_cast<const Declaration*>(statement.get())) {

            if (scope.find(declaration->name) != nullptr) {
                throw std::runtime_error("Duplicate declaration: " + declaration->name);
            }

            auto resolved = std::make_unique<ResolvedDeclaration>();
            resolved->name = declaration->name;
            resolved->type = resolve_type(declaration->type_name);

            const ResolvedDeclaration* target = resolved.get();

            scope.symbols.emplace(target->name, target);
            statements.push_back(std::move(resolved));

            if (!declaration->initializer.body.empty()) {

                if (declaration->initializer.type != TokenType::Number) {
                    throw std::runtime_error("Variable values are not supported in the resolver yet");
                }

                auto initializer = std::make_unique<ResolvedAssignment>();
                initializer->value = target->type->parse_literal(declaration->initializer.body);
                initializer->declaration = target;
                statements.push_back(std::move(initializer));

            }

        } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement.get())) {

            const ResolvedDeclaration* target = scope.find(assignment->name);

            if (target == nullptr) {
                throw std::runtime_error("Undeclared variable: " + assignment->name);
            }

            if (assignment->value.type != TokenType::Number) {
                throw std::runtime_error("Variable values are not supported in the resolver yet");
            }

            auto resolved = std::make_unique<ResolvedAssignment>();
            resolved->value = target->type->parse_literal(assignment->value.body);
            resolved->declaration = target;
            statements.push_back(std::move(resolved));

        } else {

            throw std::runtime_error("Unsupported statement in resolver");

        }

    }

}

} // namespace sluggish

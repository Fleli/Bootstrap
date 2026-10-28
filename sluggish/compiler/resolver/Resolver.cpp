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

            auto resolved = std::make_unique<Declaration>();
            resolved->type_name = declaration->type_name;
            resolved->name = declaration->name;

            const Declaration* target = resolved.get();

            scope.symbols.emplace(target->name, target);
            statements.push_back(std::move(resolved));

            if (!declaration->initializer.empty()) {

                auto initializer = std::make_unique<Assignment>();
                initializer->name = declaration->name;
                initializer->value = declaration->initializer;
                initializer->declaration = target;
                statements.push_back(std::move(initializer));

            }

        } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement.get())) {

            const Declaration* target = scope.find(assignment->name);

            if (target == nullptr) {
                throw std::runtime_error("Undeclared variable: " + assignment->name);
            }

            auto resolved = std::make_unique<Assignment>();
            resolved->name = assignment->name;
            resolved->value = assignment->value;
            resolved->declaration = target;
            statements.push_back(std::move(resolved));

        } else {

            throw std::runtime_error("Unsupported statement in resolver");

        }

    }

}

} // namespace sluggish

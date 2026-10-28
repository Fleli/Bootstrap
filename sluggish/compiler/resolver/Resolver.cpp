#include "Resolver.hpp"

#include <stdexcept>
#include <utility>

namespace sluggish {

Resolver::Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements) {

    for (const auto& statement : parsed_statements) {

        if (const auto* declaration = dynamic_cast<const Declaration*>(statement.get())) {

            auto resolved = std::make_unique<Declaration>();
            resolved->type_name = declaration->type_name;
            resolved->name = declaration->name;
            statements.push_back(std::move(resolved));

            if (!declaration->initializer.empty()) {

                auto initializer = std::make_unique<Assignment>();
                initializer->name = declaration->name;
                initializer->value = declaration->initializer;
                statements.push_back(std::move(initializer));

            }

        } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement.get())) {

            auto resolved = std::make_unique<Assignment>();
            resolved->name = assignment->name;
            resolved->value = assignment->value;
            statements.push_back(std::move(resolved));

        } else {

            throw std::runtime_error("Unsupported statement in resolver");

        }

    }

}

} // namespace sluggish

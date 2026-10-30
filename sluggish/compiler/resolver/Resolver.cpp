#include "Resolver.hpp"

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

        if (const auto* definition = dynamic_cast<const StructDefinition*>(statement.get())) {
            resolve_method_bodies(*definition);
        } else {
            resolve_function_body(statement.get());
        }

    }

}

} // namespace sluggish

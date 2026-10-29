#pragma once

#include "Scope.hpp"
#include "ResolvedStatement.hpp"
#include "parse/Parser.hpp"
#include "type-system/TypeTable.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace sluggish {

class Resolver {
public:

    TypeTable types;
    std::vector<std::unique_ptr<ResolvedFunction>> functions;

    // Each function owns its arguments and flattened body statements.
    explicit Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements);

private:

    std::unordered_map<std::string, ResolvedFunction*> function_symbols;

    void resolve_struct_members(const StructDefinition& definition);

    void register_function_signature(const Statement* statement);
    void resolve_function_body(const Statement* statement);

    void resolve(const Statement* statement, Scope& current_scope, ResolvedFunction& function);
    std::unique_ptr<ResolvedExpression> resolve_expression(const Expression* expression, const Scope& scope, const Type* expected_type = nullptr);
    std::unique_ptr<ResolvedExpression> resolve_member_expression(const MemberExpression& member, const Scope& scope, const Type* expected_type);

};

} // namespace sluggish

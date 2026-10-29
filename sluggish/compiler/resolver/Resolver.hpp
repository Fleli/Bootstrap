#pragma once

#include "Scope.hpp"
#include "ResolvedStatement.hpp"
#include "parse/Parser.hpp"

#include <memory>
#include <vector>

namespace sluggish {

class Resolver {
public:

    std::vector<std::unique_ptr<ResolvedFunction>> functions;

    // Each function owns its arguments and flattened body statements.
    explicit Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements);

private:
    void resolve(const Statement* statement, Scope& current_scope, ResolvedFunction& function);

};

std::unique_ptr<ResolvedExpression> resolve_expression(const Expression* expression, const Scope& scope, const Type* expected_type);

} // namespace sluggish

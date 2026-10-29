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

} // namespace sluggish

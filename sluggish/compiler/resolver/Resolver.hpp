#pragma once

#include "Scope.hpp"
#include "ResolvedStatement.hpp"
#include "parse/Parser.hpp"

#include <memory>
#include <vector>

namespace sluggish {

class Resolver {
public:

    std::vector<std::unique_ptr<ResolvedStatement>> statements;
    Scope scope;

    // Initialized declarations become a declaration followed by an assignment.
    explicit Resolver(const std::vector<std::unique_ptr<Statement>>& parsed_statements);

};

} // namespace sluggish

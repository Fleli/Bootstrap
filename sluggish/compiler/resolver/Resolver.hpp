#pragma once

#include "parse/Parser.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace sluggish {

class Resolver {
public:

    std::vector<std::unique_ptr<Statement>> statements;

    explicit Resolver(std::vector<std::unique_ptr<Statement>> parsed_statements) {
        statements = std::move(parsed_statements);
    }

};

} // namespace sluggish

#pragma once

#include "lex/Token.hpp"

#include <memory>
#include <string>
#include <vector>

namespace sluggish {

struct Statement {
    virtual ~Statement() = default;
};

struct Declaration : public Statement {
    std::string type_name;
    std::string name;
    std::string initializer; // Number text; empty when absent.
};

struct Assignment : public Statement {
    std::string name;
    std::string value; // Number text.
};

// Parses the entire token sequence; throws std::runtime_error on invalid syntax.
std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens);

} // namespace sluggish

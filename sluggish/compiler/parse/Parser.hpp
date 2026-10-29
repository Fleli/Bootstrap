#pragma once

#include "lex/Token.hpp"

#include <memory>
#include <string>
#include <vector>

namespace sluggish {

struct Statement {
    virtual ~Statement() = default;
};

struct Block : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
};

struct Declaration : public Statement {
    
    std::string name;
    std::string type_name;
    Token initializer{}; // Number or Symbol; empty body when absent.
    
};

struct Assignment : public Statement {
    
    std::string name;
    Token value{}; // Number or Symbol.

};

// Parses the entire token sequence; throws std::runtime_error on invalid syntax.
// Writes the tree to intermediate/parse.txt, indenting statements inside blocks.
std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens);

} // namespace sluggish

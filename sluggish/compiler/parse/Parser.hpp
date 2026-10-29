#pragma once

#include "lex/Token.hpp"

#include <memory>
#include <string>
#include <vector>

namespace sluggish {

struct Statement {
    virtual ~Statement() = default;
};

struct Argument {

    std::string name;
    std::string type_name;

};

struct Function : public Statement {

    std::string name;
    std::string return_type_name;
    std::vector<Argument> arguments;
    std::unique_ptr<Statement> body;

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

// Parses top-level functions only; throws std::runtime_error on invalid syntax.
// Writes the tree to intermediate/parse.txt, indenting function bodies and blocks.
std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens);

} // namespace sluggish

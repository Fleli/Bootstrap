#pragma once

#include "lex/Token.hpp"

#include <memory>
#include <string>
#include <vector>

namespace sluggish {

struct Expression {
    virtual ~Expression() = default;
};

struct NumberExpression : public Expression {
    std::string value;
};

struct VariableExpression : public Expression {
    std::string name;
};

struct AdditionExpression : public Expression {

    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

};

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
    std::unique_ptr<Expression> initializer; // Null when absent.
    
};

struct Assignment : public Statement {
    
    std::string name;
    std::unique_ptr<Expression> value;

};

struct Return : public Statement {
    std::unique_ptr<Expression> value;
};

// Parses top-level functions only; throws std::runtime_error on invalid syntax.
// Writes the tree to intermediate/parse.txt, indenting function bodies and blocks.
std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens);

} // namespace sluggish

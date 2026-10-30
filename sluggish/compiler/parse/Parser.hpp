#pragma once

#include "lex/Token.hpp"

#include <cstddef>
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

struct AddressOfExpression : public Expression {
    std::unique_ptr<Expression> operand;
};

struct DereferenceExpression : public Expression {
    std::unique_ptr<Expression> operand;
};

struct MemberExpression : public Expression {

    std::string name;
    std::unique_ptr<Expression> object;

};

struct CallExpression : public Expression {

    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;

};

struct AdditionExpression : public Expression {

    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

};

struct Statement {
    virtual ~Statement() = default;
};

struct StructMember {

    std::string name;
    std::string type_name;

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

struct StructDefinition : public Statement {

    std::string name;
    std::vector<StructMember> members;
    std::vector<std::unique_ptr<Function>> methods;

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
    
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;

};

struct Return : public Statement {
    std::unique_ptr<Expression> value;
};

// Parses top-level functions and structs; throws std::runtime_error on invalid syntax.
// Writes the tree to intermediate/parse.txt, indenting bodies, blocks and members.
std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens);

const Token& consume(const std::vector<Token>& tokens, std::size_t& i, TokenType type, const std::string& expected);

std::string parse_type(const std::vector<Token>& tokens, std::size_t& i);

std::unique_ptr<Expression> parse_expression(const std::vector<Token>& tokens, std::size_t& i);
std::unique_ptr<Statement> parse_statement(const std::vector<Token>& tokens, std::size_t& i);
std::unique_ptr<StructDefinition> parse_struct(const std::vector<Token>& tokens, std::size_t& i);

// Parses the arguments and body after the return type and name have been consumed.
std::unique_ptr<Function> parse_function(const std::vector<Token>& tokens, std::size_t& i,
    std::string return_type_name, std::string name);

std::string format_statement(const Statement* statement, const std::string& indentation);

} // namespace sluggish

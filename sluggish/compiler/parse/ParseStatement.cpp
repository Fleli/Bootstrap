#include "Parser.hpp"

#include <memory>
#include <stdexcept>

namespace sluggish {

std::unique_ptr<Statement> parse_statement(const std::vector<Token>& tokens, std::size_t& i) {

    if (i < tokens.size() && tokens[i].type == TokenType::Struct) {
        throw std::runtime_error("Struct definitions are only allowed at top level");
    }

    if (i < tokens.size() && tokens[i].type == TokenType::LeftBrace) {

        ++i;
        auto block = std::make_unique<Block>();

        while (i < tokens.size() && tokens[i].type != TokenType::RightBrace) {
            block->statements.push_back(parse_statement(tokens, i));
        }

        consume(tokens, i, TokenType::RightBrace, "'}'");
        return block;

    }

    if (i < tokens.size() && tokens[i].type == TokenType::Return) {

        ++i;
        auto statement = std::make_unique<Return>();
        statement->value = parse_expression(tokens, i);

        consume(tokens, i, TokenType::Semicolon, "';'");
        return statement;

    }

    if (i + 1 < tokens.size() && tokens[i].type == TokenType::Symbol && tokens[i + 1].type == TokenType::Symbol) {

        auto declaration = std::make_unique<Declaration>();
        declaration->type_name = tokens[i++].body;
        declaration->name = tokens[i++].body;

        if (i < tokens.size() && tokens[i].type == TokenType::LeftParen) {
            throw std::runtime_error("Function definitions are only allowed at top level");
        }

        if (i < tokens.size() && tokens[i].type == TokenType::EqualSign) {

            ++i;
            declaration->initializer = parse_expression(tokens, i);

        }

        consume(tokens, i, TokenType::Semicolon, "';'");
        return declaration;

    }

    auto statement = std::make_unique<Assignment>();
    statement->target = parse_expression(tokens, i);

    consume(tokens, i, TokenType::EqualSign, "'='");
    statement->value = parse_expression(tokens, i);
    consume(tokens, i, TokenType::Semicolon, "';'");
    return statement;

}

} // namespace sluggish

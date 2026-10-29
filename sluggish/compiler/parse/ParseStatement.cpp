#include "Parser.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sluggish {

std::unique_ptr<Statement> parse_statement(const std::vector<Token>& tokens, std::size_t& i) {

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

    std::string name = consume(tokens, i, TokenType::Symbol, "a name").body;
    const bool is_declaration = i < tokens.size() && tokens[i].type == TokenType::Symbol;

    std::string type_name;

    if (is_declaration) {

        type_name = name;
        name = tokens[i].body;
        ++i;

        if (i < tokens.size() && tokens[i].type == TokenType::LeftParen) {
            throw std::runtime_error("Function definitions are only allowed at top level");
        }

    }

    std::unique_ptr<Expression> value;

    if (i < tokens.size() && tokens[i].type == TokenType::EqualSign) {

        ++i;
        value = parse_expression(tokens, i);

    } else if (!is_declaration) {

        throw std::runtime_error("Expected '=' at token " + std::to_string(i));

    }

    consume(tokens, i, TokenType::Semicolon, "';'");

    if (is_declaration) {

        auto statement = std::make_unique<Declaration>();
        statement->name = name;
        statement->type_name = type_name;
        statement->initializer = std::move(value);
        return statement;

    }

    auto statement = std::make_unique<Assignment>();
    statement->name = name;
    statement->value = std::move(value);
    return statement;

}

} // namespace sluggish

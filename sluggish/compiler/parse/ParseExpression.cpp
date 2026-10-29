#include "Parser.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace sluggish {

namespace {

std::unique_ptr<Expression> parse_primary(const std::vector<Token>& tokens, std::size_t& i) {

    if (i < tokens.size() && tokens[i].type == TokenType::Number) {

        auto expression = std::make_unique<NumberExpression>();
        expression->value = tokens[i++].body;
        return expression;

    }

    if (i < tokens.size() && tokens[i].type == TokenType::Symbol) {

        auto expression = std::make_unique<VariableExpression>();
        expression->name = tokens[i++].body;
        return expression;

    }

    if (i < tokens.size() && tokens[i].type == TokenType::LeftParen) {

        ++i;
        auto expression = parse_expression(tokens, i);
        consume(tokens, i, TokenType::RightParen, "')'");
        return expression;

    }

    throw std::runtime_error("Expected an expression at token " + std::to_string(i));

}

} // namespace

std::unique_ptr<Expression> parse_expression(const std::vector<Token>& tokens, std::size_t& i) {

    auto expression = parse_primary(tokens, i);

    while (i < tokens.size() && tokens[i].type == TokenType::PlusSign) {

        ++i;
        auto addition = std::make_unique<AdditionExpression>();
        addition->left = std::move(expression);
        addition->right = parse_primary(tokens, i);
        expression = std::move(addition);

    }

    return expression;

}

} // namespace sluggish

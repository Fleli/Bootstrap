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

        const std::string name = tokens[i++].body;

        if (i < tokens.size() && tokens[i].type == TokenType::LeftParen) {

            ++i;
            auto expression = std::make_unique<CallExpression>();
            expression->name = name;

            if (i < tokens.size() && tokens[i].type != TokenType::RightParen) {

                while (true) {

                    expression->arguments.push_back(parse_expression(tokens, i));

                    if (i == tokens.size() || tokens[i].type != TokenType::Comma) {
                        break;
                    }

                    ++i;

                }

            }

            consume(tokens, i, TokenType::RightParen, "')' after call arguments");
            return expression;

        }

        auto expression = std::make_unique<VariableExpression>();
        expression->name = name;
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

std::unique_ptr<Expression> parse_postfix(const std::vector<Token>& tokens, std::size_t& i) {

    auto expression = parse_primary(tokens, i);

    while (i < tokens.size() && tokens[i].type == TokenType::Dot) {

        ++i;
        auto member = std::make_unique<MemberExpression>();
        member->name = consume(tokens, i, TokenType::Symbol, "a member name after '.'").body;
        member->object = std::move(expression);
        expression = std::move(member);

    }

    return expression;

}

} // namespace

std::unique_ptr<Expression> parse_expression(const std::vector<Token>& tokens, std::size_t& i) {

    auto expression = parse_postfix(tokens, i);

    while (i < tokens.size() && tokens[i].type == TokenType::PlusSign) {

        ++i;
        auto addition = std::make_unique<AdditionExpression>();
        addition->left = std::move(expression);
        addition->right = parse_postfix(tokens, i);
        expression = std::move(addition);

    }

    return expression;

}

} // namespace sluggish

#include "Parser.hpp"

namespace sluggish {

std::vector<std::unique_ptr<Expression>> parse_call_arguments(const std::vector<Token>& tokens, std::size_t& i) {

    consume(tokens, i, TokenType::LeftParen, "'(' before call arguments");
    std::vector<std::unique_ptr<Expression>> arguments;

    if (i < tokens.size() && tokens[i].type != TokenType::RightParen) {

        while (true) {

            arguments.push_back(parse_expression(tokens, i));

            if (i == tokens.size() || tokens[i].type != TokenType::Comma) {
                break;
            }

            ++i;

        }

    }

    consume(tokens, i, TokenType::RightParen, "')' after call arguments");
    return arguments;

}

} // namespace sluggish

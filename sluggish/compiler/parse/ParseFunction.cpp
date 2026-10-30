#include "Parser.hpp"

#include <memory>
#include <utility>

namespace sluggish {

std::unique_ptr<Function> parse_function(const std::vector<Token>& tokens, std::size_t& i,
    std::string return_type_name, std::string name) {

    auto function = std::make_unique<Function>();
    function->return_type_name = std::move(return_type_name);
    function->name = std::move(name);
    consume(tokens, i, TokenType::LeftParen, "'(' after a function name");

    if (i < tokens.size() && tokens[i].type != TokenType::RightParen) {

        while (true) {

            Argument argument;
            argument.type_name = parse_type(tokens, i);
            argument.name = consume(tokens, i, TokenType::Symbol, "an argument name").body;
            function->arguments.push_back(std::move(argument));

            if (i == tokens.size() || tokens[i].type != TokenType::Comma) {
                break;
            }

            ++i;

        }

    }

    consume(tokens, i, TokenType::RightParen, "')'");
    function->body = parse_statement(tokens, i);
    return function;

}

} // namespace sluggish

#include "Parser.hpp"
#include "stage-emit/Emit.hpp"

#include <stdexcept>
#include <utility>

namespace sluggish {

const Token& consume(const std::vector<Token>& tokens, std::size_t& i, TokenType type, const std::string& expected) {

    if (i == tokens.size() || tokens[i].type != type) {
        throw std::runtime_error("Expected " + expected + " at token " + std::to_string(i));
    }

    return tokens[i++];

}

std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens) {

    std::size_t i = 0;
    std::vector<std::unique_ptr<Statement>> definitions;

    while (i < tokens.size()) {

        if (tokens[i].type == TokenType::Struct) {
            definitions.push_back(parse_struct(tokens, i));
            continue;
        }

        auto function = std::make_unique<Function>();
        function->return_type_name = parse_type(tokens, i);
        function->name = consume(tokens, i, TokenType::Symbol, "a function name").body;
        consume(tokens, i, TokenType::LeftParen, "'(' after a top-level function name");

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
        definitions.push_back(std::move(function));

    }

    std::string output;

    for (const auto& definition : definitions) {
        output += format_statement(definition.get(), "");
    }

    emit("parse.txt", output);
    return definitions;

}

} // namespace sluggish

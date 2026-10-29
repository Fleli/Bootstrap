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
    std::vector<std::unique_ptr<Statement>> functions;

    while (i < tokens.size()) {

        auto function = std::make_unique<Function>();
        function->return_type_name = consume(tokens, i, TokenType::Symbol, "a return type").body;
        function->name = consume(tokens, i, TokenType::Symbol, "a function name").body;
        consume(tokens, i, TokenType::LeftParen, "'(' after a top-level function name");

        if (i < tokens.size() && tokens[i].type != TokenType::RightParen) {

            while (true) {

                Argument argument;
                argument.type_name = consume(tokens, i, TokenType::Symbol, "an argument type").body;
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
        functions.push_back(std::move(function));

    }

    std::string output;

    for (const auto& function : functions) {
        output += format_statement(function.get(), "");
    }

    emit("parse.txt", output);
    return functions;

}

} // namespace sluggish

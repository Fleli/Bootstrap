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

        auto return_type_name = parse_type(tokens, i);
        auto name = consume(tokens, i, TokenType::Symbol, "a function name").body;
        definitions.push_back(parse_function(tokens, i, std::move(return_type_name), std::move(name)));

    }

    std::string output;

    for (const auto& definition : definitions) {
        output += format_statement(definition.get(), "");
    }

    emit("parse.txt", output);
    return definitions;

}

} // namespace sluggish

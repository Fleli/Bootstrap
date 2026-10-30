#include "Parser.hpp"

#include <memory>
#include <utility>

namespace sluggish {

std::unique_ptr<StructDefinition> parse_struct(const std::vector<Token>& tokens, std::size_t& i) {

    consume(tokens, i, TokenType::Struct, "'struct'");
    auto definition = std::make_unique<StructDefinition>();
    definition->name = consume(tokens, i, TokenType::Symbol, "a struct name").body;
    consume(tokens, i, TokenType::LeftBrace, "'{' after a struct name");

    while (i < tokens.size() && tokens[i].type != TokenType::RightBrace) {

        auto type_name = parse_type(tokens, i);
        auto name = consume(tokens, i, TokenType::Symbol, "a member name").body;

        if (i < tokens.size() && tokens[i].type == TokenType::LeftParen) {

            definition->methods.push_back(parse_function(tokens, i, std::move(type_name), std::move(name)));
            continue;

        }

        consume(tokens, i, TokenType::Semicolon, "';' after a member");
        definition->members.push_back({std::move(name), std::move(type_name)});

    }

    consume(tokens, i, TokenType::RightBrace, "'}'");
    return definition;

}

} // namespace sluggish

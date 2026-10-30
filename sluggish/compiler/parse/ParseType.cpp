#include "Parser.hpp"

namespace sluggish {

std::string parse_type(const std::vector<Token>& tokens, std::size_t& i) {

    std::string name = consume(tokens, i, TokenType::Symbol, "a type name").body;

    while (i < tokens.size() && tokens[i].type == TokenType::Asterisk) {
        name += '*';
        ++i;
    }

    return name;

}

} // namespace sluggish

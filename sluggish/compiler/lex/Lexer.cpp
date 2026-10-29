#include "Lexer.hpp"
#include "stage-emit/Emit.hpp"

#include <stdexcept>

namespace sluggish {

std::vector<Token> lex(std::string_view source) {
    
    const auto is_digit = [](char c) { return c >= '0' && c <= '9'; };
    
    const auto is_name_start = [](char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    };

    std::vector<Token> tokens;
    std::size_t i = 0;
    
    while (i < source.size()) {
        const char c = source[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f') {
            ++i;
            continue;
        }

        const auto start = i;
        ++i;
        TokenType type;
        if (is_name_start(c)) {
            while (i < source.size() && (is_name_start(source[i]) || is_digit(source[i]))) {
                ++i;
            }
            type = TokenType::Symbol;
        } else if (is_digit(c)) {
            while (i < source.size() && is_digit(source[i])) {
                ++i;
            }
            type = TokenType::Number;
        } else if (c == '=') {
            type = TokenType::EqualSign;
        } else if (c == '+') {
            type = TokenType::PlusSign;
        } else if (c == ';') {
            type = TokenType::Semicolon;
        } else if (c == '{') {
            type = TokenType::LeftBrace;
        } else if (c == '}') {
            type = TokenType::RightBrace;
        } else if (c == '(') {
            type = TokenType::LeftParen;
        } else if (c == ')') {
            type = TokenType::RightParen;
        } else if (c == ',') {
            type = TokenType::Comma;
        } else {
            throw std::runtime_error("Unexpected character at byte " + std::to_string(start));
        }

        const auto body = source.substr(start, i - start);

        if (type == TokenType::Symbol && body == "return") {
            type = TokenType::Return;
        }

        tokens.push_back({type, std::string(body)});
    }

    std::string output;

    for (const auto& token : tokens) {

        switch (token.type) {
            case TokenType::Number: output += "Number          "; break;
            case TokenType::Symbol: output += "Symbol          "; break;
            case TokenType::EqualSign: output += "EqualSign       "; break;
            case TokenType::PlusSign: output += "PlusSign        "; break;
            case TokenType::Semicolon: output += "Semicolon       "; break;
            case TokenType::LeftBrace: output += "LeftBrace       "; break;
            case TokenType::RightBrace: output += "RightBrace      "; break;
            case TokenType::LeftParen: output += "LeftParen       "; break;
            case TokenType::RightParen: output += "RightParen      "; break;
            case TokenType::Comma: output += "Comma           "; break;
            case TokenType::Return: output += "Return          "; break;
        }

        output += token.body + "\n";

    }

    emit("lex.txt", output);

    return tokens;
}

} // namespace sluggish

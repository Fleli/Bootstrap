#pragma once

#include <string>

namespace sluggish {

enum class TokenType {
    Number,
    Symbol,
    EqualSign,
    PlusSign,
    Semicolon,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    Comma,
    Dot,
    Return,
    Struct,
};

struct SourceReference {};

class Token {
public:
    TokenType type;
    std::string body;
    SourceReference source_reference{};
};

} // namespace sluggish

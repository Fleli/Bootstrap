#pragma once

#include <string>

namespace sluggish {

enum class TokenType {
    String,
    Number,
    Symbol,
    Init,
    Private,
    Self,
};

struct SourceReference {};

class Token {
public:
    TokenType type;
    std::string body;
    SourceReference source_reference{};
};

} // namespace sluggish

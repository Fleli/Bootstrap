#pragma once

#include "Token.hpp"

#include <string_view>
#include <vector>

namespace sluggish {

// Names, decimal integers, '=' and ';'. Whitespace is skipped.
// Unsupported characters throw std::runtime_error.
std::vector<Token> lex(std::string_view source);

} // namespace sluggish

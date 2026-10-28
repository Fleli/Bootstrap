#include "lex/Lexer.hpp"
#include "parse/Parser.hpp"

#include <exception>
#include <iostream>
#include <sstream>

int main() {

    std::ostringstream source;
    source << std::cin.rdbuf();

    try {
        const auto tokens = sluggish::lex(source.str());
        sluggish::parse(tokens);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

}

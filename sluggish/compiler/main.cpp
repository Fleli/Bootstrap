#include "lex/Lexer.hpp"
#include "lower/BasicBlock.hpp"
#include "parse/Parser.hpp"
#include "resolver/Resolver.hpp"

#include <exception>
#include <iostream>
#include <sstream>

int main() {

    std::ostringstream source;
    source << std::cin.rdbuf();

    try {
        const auto tokens = sluggish::lex(source.str());
        const auto parsed = sluggish::parse(tokens);

        const sluggish::Resolver resolver(parsed);
        const auto block = sluggish::lower(resolver);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

}

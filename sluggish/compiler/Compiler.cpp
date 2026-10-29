#include "Compiler.hpp"
#include "lex/Lexer.hpp"
#include "lower/BasicBlock.hpp"
#include "parse/Parser.hpp"
#include "resolver/Resolver.hpp"

namespace sluggish {

std::vector<IRFunction> compile(std::string_view source) {

    const auto tokens = lex(source);
    const auto parsed = parse(tokens);

    const Resolver resolver(parsed);
    return lower(resolver);

}

} // namespace sluggish

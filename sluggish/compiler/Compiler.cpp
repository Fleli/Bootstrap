#include "Compiler.hpp"
#include "lex/Lexer.hpp"
#include "lower/BasicBlock.hpp"
#include "parse/Parser.hpp"
#include "resolver/Resolver.hpp"

#include <utility>

namespace sluggish {

IRProgram compile(std::string_view source) {

    const auto tokens = lex(source);
    const auto parsed = parse(tokens);

    Resolver resolver(parsed);

    IRProgram program;
    program.functions = lower(resolver);
    program.types = std::move(resolver.types);
    return program;

}

} // namespace sluggish

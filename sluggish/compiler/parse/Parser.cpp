#include "Parser.hpp"
#include "stage-emit/Emit.hpp"

#include <stdexcept>
#include <utility>

namespace sluggish {

namespace {

const Token& consume(const std::vector<Token>& tokens, std::size_t& i, TokenType type, const std::string& expected) {

    if (i == tokens.size() || tokens[i].type != type) {
        throw std::runtime_error("Expected " + expected + " at token " + std::to_string(i));
    }

    return tokens[i++];

}

std::unique_ptr<Statement> parse_statement(const std::vector<Token>& tokens, std::size_t& i) {

    if (i < tokens.size() && tokens[i].type == TokenType::LeftBrace) {

        ++i;
        auto block = std::make_unique<Block>();

        while (i < tokens.size() && tokens[i].type != TokenType::RightBrace) {
            block->statements.push_back(parse_statement(tokens, i));
        }

        consume(tokens, i, TokenType::RightBrace, "'}'");
        return block;

    }

    std::string name = consume(tokens, i, TokenType::Symbol, "a name").body;
    const bool is_declaration = i < tokens.size() && tokens[i].type == TokenType::Symbol;

    std::string type_name;

    if (is_declaration) {

        type_name = name;
        name = tokens[i].body;
        ++i;

        if (i < tokens.size() && tokens[i].type == TokenType::LeftParen) {
            throw std::runtime_error("Function definitions are only allowed at top level");
        }

    }

    Token value{};

    if (i < tokens.size() && tokens[i].type == TokenType::EqualSign) {

        ++i;

        // Will generalize to expressions later.
        if (i == tokens.size() || (tokens[i].type != TokenType::Number && tokens[i].type != TokenType::Symbol)) {
            throw std::runtime_error("Expected a number or variable name at token " + std::to_string(i));
        }

        value = tokens[i];
        ++i;

    } else if (!is_declaration) {

        throw std::runtime_error("Expected '=' at token " + std::to_string(i));

    }

    consume(tokens, i, TokenType::Semicolon, "';'");

    if (is_declaration) {

        auto statement = std::make_unique<Declaration>();
        statement->name = name;
        statement->type_name = type_name;
        statement->initializer = value;
        return statement;

    }

    auto statement = std::make_unique<Assignment>();
    statement->name = name;
    statement->value = value;
    return statement;

}

std::string format_statement(const Statement* statement, const std::string& indentation) {

    std::string output;

    if (const auto* function = dynamic_cast<const Function*>(statement)) {

        output += indentation + "Function        " + function->return_type_name + " " + function->name + "(";

        for (std::size_t i = 0; i < function->arguments.size(); ++i) {

            if (i != 0) {
                output += ", ";
            }

            const auto& argument = function->arguments[i];
            output += argument.type_name + " " + argument.name;

        }

        output += ")\n";
        output += format_statement(function->body.get(), indentation + "    ");

    } else if (const auto* block = dynamic_cast<const Block*>(statement)) {

        output += indentation + "Block {\n";

        for (const auto& child : block->statements) {
            output += format_statement(child.get(), indentation + "    ");
        }

        output += indentation + "}\n";

    } else if (const auto* declaration = dynamic_cast<const Declaration*>(statement)) {

        output += indentation + "Declaration     " + declaration->type_name + " " + declaration->name;

        if (!declaration->initializer.body.empty()) {
            output += " = " + declaration->initializer.body;
        }

        output += "\n";

    } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement)) {

        output += indentation + "Assignment      " + assignment->name + " = " + assignment->value.body + "\n";

    }

    return output;

}

} // namespace

std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens) {

    std::size_t i = 0;
    std::vector<std::unique_ptr<Statement>> functions;

    while (i < tokens.size()) {

        auto function = std::make_unique<Function>();
        function->return_type_name = consume(tokens, i, TokenType::Symbol, "a return type").body;
        function->name = consume(tokens, i, TokenType::Symbol, "a function name").body;
        consume(tokens, i, TokenType::LeftParen, "'(' after a top-level function name");

        if (i < tokens.size() && tokens[i].type != TokenType::RightParen) {

            while (true) {

                Argument argument;
                argument.type_name = consume(tokens, i, TokenType::Symbol, "an argument type").body;
                argument.name = consume(tokens, i, TokenType::Symbol, "an argument name").body;
                function->arguments.push_back(std::move(argument));

                if (i == tokens.size() || tokens[i].type != TokenType::Comma) {
                    break;
                }

                ++i;

            }

        }

        consume(tokens, i, TokenType::RightParen, "')'");
        function->body = parse_statement(tokens, i);
        functions.push_back(std::move(function));

    }

    std::string output;

    for (const auto& function : functions) {
        output += format_statement(function.get(), "");
    }

    emit("parse.txt", output);
    return functions;

}

} // namespace sluggish

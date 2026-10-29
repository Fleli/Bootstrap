#include "Parser.hpp"
#include "stage-emit/Emit.hpp"

#include <stdexcept>
#include <utility>

namespace sluggish {

namespace {

std::vector<std::unique_ptr<Statement>> parse_statements(const std::vector<Token>& tokens, std::size_t& i) {
    
    std::vector<std::unique_ptr<Statement>> statements;

    while (i < tokens.size() && tokens[i].type != TokenType::RightBrace) {

        if (tokens[i].type == TokenType::LeftBrace) {

            ++i;

            auto block = std::make_unique<Block>();
            block->statements = parse_statements(tokens, i);

            if (i == tokens.size()) {
                throw std::runtime_error("Expected '}' at token " + std::to_string(i));
            }

            ++i;
            statements.push_back(std::move(block));
            continue;

        }
        
        if (tokens[i].type != TokenType::Symbol) {
            throw std::runtime_error("Expected a name at token " + std::to_string(i));
        }
        
        std::string name = tokens[i].body;
        ++i;

        const bool is_declaration = i < tokens.size() && tokens[i].type == TokenType::Symbol;
        
        std::string type_name;
        
        if (is_declaration) {
            
            type_name = name;
            name = tokens[i].body;
            ++i;
            
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

        if (i == tokens.size() || tokens[i].type != TokenType::Semicolon) {
            
            throw std::runtime_error("Expected ';' at token " + std::to_string(i));
            
        }
        
        ++i;

        if (is_declaration) {
            
            auto statement = std::make_unique<Declaration>();
            statement->type_name = type_name;
            statement->name = name;
            statement->initializer = value;
            statements.push_back(std::move(statement));
            
        } else {
            
            auto statement = std::make_unique<Assignment>();
            statement->name = name;
            statement->value = value;
            statements.push_back(std::move(statement));
            
        }
    }

    return statements;

}

std::string format_statements(const std::vector<std::unique_ptr<Statement>>& statements, const std::string& indentation) {
    
    std::string output;

    for (const auto& statement : statements) {

        if (const auto* block = dynamic_cast<const Block*>(statement.get())) {

            output += indentation + "Block {\n";
            output += format_statements(block->statements, indentation + "    ");
            output += indentation + "}";

        } else if (const auto* declaration = dynamic_cast<const Declaration*>(statement.get())) {

            output += indentation + "Declaration     " + declaration->type_name + " " + declaration->name;

            if (!declaration->initializer.body.empty()) {
                output += " = " + declaration->initializer.body;
            }

        } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement.get())) {

            output += indentation + "Assignment      " + assignment->name + " = " + assignment->value.body;

        }

        output += "\n";

    }

    return output;

}

} // namespace

std::vector<std::unique_ptr<Statement>> parse(const std::vector<Token>& tokens) {

    std::size_t i = 0;
    auto statements = parse_statements(tokens, i);

    if (i != tokens.size()) {
        throw std::runtime_error("Unexpected '}' at token " + std::to_string(i));
    }

    emit("parse.txt", format_statements(statements, ""));

    return statements;
    
}

} // namespace sluggish

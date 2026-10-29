#include "Parser.hpp"

#include <stdexcept>
#include <string>

namespace sluggish {

namespace {

std::string format_expression(const Expression* expression) {

    if (const auto* number = dynamic_cast<const NumberExpression*>(expression)) {
        return number->value;
    }

    if (const auto* variable = dynamic_cast<const VariableExpression*>(expression)) {
        return variable->name;
    }

    if (const auto* addition = dynamic_cast<const AdditionExpression*>(expression)) {
        return "(" + format_expression(addition->left.get()) + " + " + format_expression(addition->right.get()) + ")";
    }

    throw std::runtime_error("Unsupported expression in parser output");

}

} // namespace

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

        if (declaration->initializer != nullptr) {
            output += " = " + format_expression(declaration->initializer.get());
        }

        output += "\n";

    } else if (const auto* assignment = dynamic_cast<const Assignment*>(statement)) {

        output += indentation + "Assignment      " + assignment->name + " = " + format_expression(assignment->value.get()) + "\n";

    } else if (const auto* returned = dynamic_cast<const Return*>(statement)) {

        output += indentation + "Return          " + format_expression(returned->value.get()) + "\n";

    }

    return output;

}

} // namespace sluggish

#include "Parser.hpp"

#include <stdexcept>
#include <string>

namespace sluggish {

namespace {

std::string format_expression(const Expression* expression);

std::string format_call(const std::string& name, const std::vector<std::unique_ptr<Expression>>& arguments) {

    std::string output = name + "(";

    for (std::size_t i = 0; i < arguments.size(); ++i) {

        if (i != 0) {
            output += ", ";
        }

        output += format_expression(arguments[i].get());

    }

    return output + ")";

}

std::string format_expression(const Expression* expression) {

    if (const auto* number = dynamic_cast<const NumberExpression*>(expression)) {
        return number->value;
    }

    if (const auto* variable = dynamic_cast<const VariableExpression*>(expression)) {
        return variable->name;
    }

    if (const auto* member = dynamic_cast<const MemberExpression*>(expression)) {
        return format_expression(member->object.get()) + "." + member->name;
    }

    if (const auto* call = dynamic_cast<const CallExpression*>(expression)) {
        return format_call(call->name, call->arguments);
    }

    if (const auto* call = dynamic_cast<const MethodCallExpression*>(expression)) {
        return format_call(format_expression(call->object.get()) + "." + call->name, call->arguments);
    }

    if (const auto* address = dynamic_cast<const AddressOfExpression*>(expression)) {
        return "(&" + format_expression(address->operand.get()) + ")";
    }

    if (const auto* dereference = dynamic_cast<const DereferenceExpression*>(expression)) {
        return "(*" + format_expression(dereference->operand.get()) + ")";
    }

    if (const auto* addition = dynamic_cast<const AdditionExpression*>(expression)) {
        return "(" + format_expression(addition->left.get()) + " + " + format_expression(addition->right.get()) + ")";
    }

    throw std::runtime_error("Unsupported expression in parser output");

}

} // namespace

std::string format_statement(const Statement* statement, const std::string& indentation) {

    std::string output;

    if (const auto* definition = dynamic_cast<const StructDefinition*>(statement)) {

        output += indentation + "Struct          " + definition->name + "\n";

        for (const auto& member : definition->members) {
            output += indentation + "    Member          " + member.type_name + " " + member.name + "\n";
        }

        for (const auto& method : definition->methods) {
            output += format_statement(method.get(), indentation + "    ");
        }

    } else if (const auto* function = dynamic_cast<const Function*>(statement)) {

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

        output += indentation + "Assignment      " + format_expression(assignment->target.get()) + " = " + format_expression(assignment->value.get()) + "\n";

    } else if (const auto* returned = dynamic_cast<const Return*>(statement)) {

        output += indentation + "Return          " + format_expression(returned->value.get()) + "\n";

    }

    return output;

}

} // namespace sluggish

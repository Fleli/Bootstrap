#include "BasicBlock.hpp"
#include "stage-emit/Emit.hpp"

#include <string>

namespace sluggish {

std::vector<IRFunction> lower(const Resolver& resolver) {

    std::vector<IRFunction> functions;

    for (const auto& function : resolver.functions) {
        functions.push_back(lower_function(*function));
    }

    std::string output;

    for (const auto& function : functions) {

        output += "Function        " + function.return_type->name() + " " + function.name + "\n";

        for (const auto& argument : function.arguments) {
            output += "    Argument        s" + std::to_string(argument.slot) + ", " + argument.type->name() + "\n";
        }

        for (std::size_t i = 0; i < function.blocks.size(); ++i) {

            output += "    BasicBlock " + std::to_string(i) + ":\n";

            for (const auto& instruction : function.blocks[i].instructions) {

                if (const auto* allocate = dynamic_cast<const AllocateInstruction*>(instruction.get())) {

                    output += "        Allocate        s" + std::to_string(allocate->slot) + ", " + allocate->type->name() + "\n";

                } else if (const auto* store = dynamic_cast<const StoreInstruction*>(instruction.get())) {

                    output += "        Store           s" + std::to_string(store->slot) + ", " + std::to_string(store->value) + "\n";

                } else if (const auto* copy = dynamic_cast<const CopyInstruction*>(instruction.get())) {

                    output += "        Copy            s" + std::to_string(copy->destination_slot) + ", s" + std::to_string(copy->source_slot) + "\n";

                } else if (const auto* member = dynamic_cast<const MemberInstruction*>(instruction.get())) {

                    output += "        Member          s" + std::to_string(member->destination_slot) + ", s" + std::to_string(member->source_slot) + ", " + std::to_string(member->member_index) + "\n";

                } else if (const auto* add = dynamic_cast<const AddInstruction*>(instruction.get())) {

                    output += "        Add             s" + std::to_string(add->destination_slot) + ", s" + std::to_string(add->left_slot) + ", s" + std::to_string(add->right_slot) + "\n";

                } else if (const auto* call = dynamic_cast<const CallInstruction*>(instruction.get())) {

                    output += "        Call            s" + std::to_string(call->destination_slot) + ", " + call->function_name + "(";

                    for (std::size_t i = 0; i < call->argument_slots.size(); ++i) {

                        if (i != 0) {
                            output += ", ";
                        }

                        output += "s" + std::to_string(call->argument_slots[i]);

                    }

                    output += ")\n";

                } else if (const auto* returned = dynamic_cast<const ReturnInstruction*>(instruction.get())) {

                    output += "        Return          ";

                    if (returned->source_slot.has_value()) {
                        output += "s" + std::to_string(*returned->source_slot);
                    } else {
                        output += std::to_string(returned->value);
                    }

                    output += "\n";

                }

            }

        }

    }

    emit("lower.txt", output);
    return functions;

}

} // namespace sluggish

#include "BuildAssembly.hpp"
#include "assembler/Assembler.hpp"

#include <stdexcept>

namespace simplecpu {

std::vector<AssemblyFunction> build_assembly(const std::vector<sluggish::IRFunction>& functions) {

    const sluggish::IRFunction* entry = nullptr;

    for (const auto& function : functions) {
        if (function.name == "main") {
            entry = &function;
            break;
        }
    }

    if (entry == nullptr) {
        throw std::runtime_error("Missing entry function: main");
    }

    if (!entry->arguments.empty()) {
        throw std::runtime_error("main must not take arguments");
    }

    std::vector<const sluggish::IRFunction*> ordered_functions{entry};

    for (const auto& function : functions) {
        if (&function != entry) {
            ordered_functions.push_back(&function);
        }
    }

    std::unordered_map<std::string, std::uint32_t> function_indices;

    for (std::size_t i = 0; i < ordered_functions.size(); ++i) {

        const auto& name = ordered_functions[i]->name;

        if (name == "_start") {
            throw std::runtime_error("_start is reserved for SimpleCPU startup");
        }

        if (!function_indices.emplace(name, i + 1).second) {
            throw std::runtime_error("Duplicate function: " + name);
        }

    }

    std::vector<AssemblyFunction> assembly{
        {"_start", program_address, {
            {Opcode::LoadImm, 1, 0},
            {Opcode::LoadImm, 2, ram_words},
            {Opcode::LoadImm, 3, main_result_address},
            {Opcode::Push, 3},
            {Opcode::Call, function_indices.at("main")},
            {Opcode::LoadImm, 3, 1},
            {Opcode::Add, 2, 2, 3},
            {Opcode::Halt}
        }}
    };

    for (const auto* function : ordered_functions) {
        assembly.push_back(build_function(*function, function_indices));
    }

    std::uint64_t next_address = program_address;

    for (auto& function : assembly) {

        function.address = next_address;
        next_address += assemble(function.instructions).size();

        if (next_address + 3 > ram_words) {
            throw std::runtime_error("SimpleCPU program leaves no room for main's call frame");
        }

    }

    // Replace function indices with final addresses after all code sizes are known.
    for (auto& function : assembly) {
        for (auto& instruction : function.instructions) {
            if (instruction.opcode == Opcode::Call) {
                instruction.operand_a = assembly.at(instruction.operand_a).address;
            }
        }
    }

    return assembly;

}

} // namespace simplecpu

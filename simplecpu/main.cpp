#include "Compiler.hpp"
#include "assembler/Assembler.hpp"
#include "emulator/Emulator.hpp"
#include "sluggish-backend/build-assembly/BuildAssembly.hpp"

#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {

    if (argc > 2 || (argc == 2 && std::string(argv[1]) != "--run")) {
        std::cerr << "Usage: simplecpu [--run] < source.slug\n";
        return 1;
    }

    std::ostringstream source;
    source << std::cin.rdbuf();

    try {

        const auto functions = sluggish::compile(source.str());
        const auto assembly = simplecpu::build_assembly(functions);

        std::vector<std::uint32_t> words;

        for (const auto& function : assembly) {

            const auto function_words = simplecpu::assemble(function.instructions);
            words.insert(words.end(), function_words.begin(), function_words.end());

        }

        std::filesystem::create_directories("simplecpu/intermediate");
        std::ofstream text("simplecpu/intermediate/assembly.txt");
        std::ofstream hex("simplecpu/intermediate/program.hex");

        for (const auto& function : assembly) {

            text << function.name << ":\n";

            for (const auto& instruction : function.instructions) {
                text << "    " << instruction.text() << "\n";
            }

            text << "\n";

        }

        hex << std::hex << std::uppercase << std::setfill('0');
        for (auto word : words) {
            hex << std::setw(8) << word << "\n";
        }

        text.close();
        hex.close();

        if (!text || !hex) {
            throw std::runtime_error("Could not write CPU intermediate output");
        }

        if (argc == 2) {

            simplecpu::Emulator cpu(simplecpu::ram_words);

            for (std::size_t i = 0; i < words.size(); ++i) {
                cpu.ram.at(simplecpu::program_address + i) = words[i];
            }

            const auto initial_ram = cpu.ram;
            cpu.pc = simplecpu::program_address;
            cpu.stack_floor = simplecpu::program_address + words.size();
            cpu.run();

            for (std::size_t address = 0; address < cpu.ram.size(); ++address) {
                if (cpu.ram[address] != initial_ram[address]) {
                    std::cout << "RAM[" << address << "] = " << cpu.ram[address] << "\n";
                }
            }

            std::cout << "Return value: " << cpu.ram.at(simplecpu::main_result_address) << "\n";

        }

    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

}

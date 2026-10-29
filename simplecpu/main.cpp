#include "Compiler.hpp"
#include "assembler/Assembler.hpp"
#include "emulator/Emulator.hpp"
#include "sluggish-backend/build-assembly/BuildAssembly.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {

    if (argc > 2 || (argc == 2 && std::string(argv[1]) != "--run")) {
        std::cerr << "Usage: simplecpu [--run] < source.slug\n";
        return 1;
    }

    std::ostringstream source;
    source << std::cin.rdbuf();

    try {

        const auto functions = sluggish::compile(source.str());

        if (!functions.empty()) {
            throw std::runtime_error("Function assembly is not implemented yet");
        }

        const auto assembly = simplecpu::build_assembly(sluggish::BasicBlock{});
        const auto words = simplecpu::assemble(assembly);

        std::filesystem::create_directories("simplecpu/intermediate");
        std::ofstream text("simplecpu/intermediate/assembly.txt");
        std::ofstream hex("simplecpu/intermediate/program.hex");

        text << "block_0:\n";
        for (const auto& instruction : assembly) {
            text << "    " << instruction.text() << "\n";
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

            simplecpu::Emulator cpu(1024);

            for (std::size_t i = 0; i < words.size(); ++i) {
                cpu.ram.at(i) = words[i];
            }

            const auto initial_ram = cpu.ram;
            cpu.run();

            for (std::size_t address = 0; address < cpu.ram.size(); ++address) {
                if (cpu.ram[address] != initial_ram[address]) {
                    std::cout << "RAM[" << address << "] = " << cpu.ram[address] << "\n";
                }
            }

        }

    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

}

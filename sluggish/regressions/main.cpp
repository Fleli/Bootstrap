#include "Compiler.hpp"
#include "assembler/Assembler.hpp"
#include "emulator/Emulator.hpp"
#include "sluggish-backend/build-assembly/BuildAssembly.hpp"

#include <algorithm>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::uint32_t run_source(const std::string& source) {

    const auto functions = sluggish::compile(source);
    const auto assembly = simplecpu::build_assembly(functions);

    std::size_t address = 0;
    simplecpu::Emulator cpu(1024);

    for (const auto& function : assembly) {

        const auto words = simplecpu::assemble(function.instructions);

        for (auto word : words) {
            cpu.ram.at(address++) = word;
        }

    }

    cpu.run();
    return cpu.registers[0];

}

int main(int argc, char* argv[]) {

    if (argc > 2) {
        std::cerr << "Usage: regressions [directory]\n";
        return 1;
    }

    try {

        const std::filesystem::path directory = argc == 2 ? argv[1] : "sluggish/regressions";
        std::vector<std::filesystem::path> files;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".slug") {
                files.push_back(entry.path());
            }
        }

        std::sort(files.begin(), files.end());

        if (files.empty()) {
            throw std::runtime_error("No .slug files found in " + directory.string());
        }

        std::size_t failed = 0;

        for (const auto& path : files) {

            try {

                std::ifstream file(path);
                std::string expected;

                if (!std::getline(file, expected)) {
                    throw std::runtime_error("Could not read expected output from the first line");
                }

                if (!expected.empty() && expected.back() == '\r') {
                    expected.pop_back();
                }

                std::ostringstream source;
                source << file.rdbuf();

                const auto actual = std::to_string(run_source(source.str()));

                if (actual != expected) {
                    throw std::runtime_error("expected " + expected + ", got " + actual);
                }

                std::cout << "PASS " << path.string() << '\n';

            } catch (const std::exception& error) {

                std::cerr << "FAIL " << path.string() << ": " << error.what() << '\n';
                ++failed;

            }

        }

        std::cout << files.size() - failed << "/" << files.size() << " passed\n";
        return failed == 0 ? 0 : 1;

    } catch (const std::exception& error) {

        std::cerr << error.what() << '\n';
        return 1;

    }

}

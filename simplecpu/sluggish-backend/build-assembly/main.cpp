#include "BuildAssembly.hpp"
#include "Compiler.hpp"

#include <exception>
#include <iostream>
#include <sstream>

int main() {

    std::ostringstream source;
    source << std::cin.rdbuf();

    try {
        const auto block = sluggish::compile(source.str());
        std::cout << simplecpu::build_assembly(block);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

}

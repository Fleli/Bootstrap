#include "Compiler.hpp"

#include <exception>
#include <iostream>
#include <sstream>

int main() {

    std::ostringstream source;
    source << std::cin.rdbuf();

    try {
        sluggish::compile(source.str());
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

}

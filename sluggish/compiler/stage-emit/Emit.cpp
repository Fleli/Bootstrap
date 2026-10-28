#include "Emit.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace sluggish {

void emit(const std::string& filename, const std::string& text) {

    const std::string directory = "sluggish/compiler/intermediate/";
    std::filesystem::create_directories(directory);

    const std::string path = directory + filename;
    std::ofstream file(path);
    file << text;
    file.close();

    if (!file) {
        throw std::runtime_error("Could not write " + path);
    }

}

} // namespace sluggish

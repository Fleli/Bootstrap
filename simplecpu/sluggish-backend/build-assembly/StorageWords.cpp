#include "BuildAssembly.hpp"

#include <stdexcept>

namespace simplecpu {

std::uint32_t storage_words(const sluggish::Type* type) {

    if (type == nullptr) {
        throw std::runtime_error("Missing type for SimpleCPU storage");
    }

    if (!type->is_struct()) {

        if (type->bit_width() == 0 || type->bit_width() > 32) {
            throw std::runtime_error("SimpleCPU requires words between 1 and 32 bits");
        }

        return 1;

    }

    std::uint32_t words = 0;

    for (const auto& member : type->members) {

        words += storage_words(member.type);

        if (words > ram_words) {
            throw std::runtime_error("Type exceeds SimpleCPU RAM: " + type->name());
        }

    }

    // Even an empty struct has a storage location.
    return words == 0 ? 1 : words;

}

} // namespace simplecpu

#include "Type.hpp"

#include <stdexcept>

namespace sluggish {

Type::Type(unsigned int bits) : width(bits) {

    if (bits == 0 || bits > 16) {
        throw std::runtime_error("Unsupported word width: " + std::to_string(bits));
    }

}

std::string Type::name() const {
    return "word_" + std::to_string(width);
}

std::uint16_t Type::parse_literal(const std::string& text) const {

    if (text.empty()) {
        throw std::runtime_error("Expected a decimal literal for " + name());
    }

    std::uint32_t value = 0;
    const std::uint32_t maximum = (std::uint32_t{1} << width) - 1;

    for (char character : text) {

        if (character < '0' || character > '9') {
            throw std::runtime_error("Invalid decimal literal: " + text);
        }

        const std::uint32_t digit = character - '0';

        if (value > maximum / 10 || (value == maximum / 10 && digit > maximum % 10)) {
            throw std::runtime_error("Value " + text + " does not fit in " + name());
        }

        value = value * 10 + digit;

    }

    return value;

}

const Type* resolve_type(const std::string& name) {

    if (name == "word_8") {
        return &word_8;
    }

    if (name == "word_16") {
        return &word_16;
    }

    throw std::runtime_error("Unknown type: " + name);

}

} // namespace sluggish

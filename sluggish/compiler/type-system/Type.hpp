#pragma once

#include <cstdint>
#include <string>

namespace sluggish {

// The built-in types are unsigned words.
class Type {
public:

    explicit Type(unsigned int bits);

    unsigned int bit_width() const { return width; }
    std::string name() const;
    std::uint16_t parse_literal(const std::string& text) const;

private:
    unsigned int width;
};

inline const Type word_8{8};
inline const Type word_16{16};

const Type* resolve_type(const std::string& name);

} // namespace sluggish

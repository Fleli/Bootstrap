#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sluggish {

class Type;

struct TypeMember {

    std::string name;
    const Type* type = nullptr;

};

class Type {
public:

    std::vector<TypeMember> members;

    explicit Type(unsigned int bits);
    explicit Type(std::string name);

    bool is_struct() const { return structure; }
    // Scalar word width; zero for structs, whose storage layout is decided later.
    unsigned int bit_width() const { return width; }
    std::string name() const;
    std::uint16_t parse_literal(const std::string& text) const;

private:
    bool structure = false;
    unsigned int width = 0;
    std::string type_name;
};

inline const Type word_8{8};
inline const Type word_16{16};

const Type* resolve_type(const std::string& name);

} // namespace sluggish

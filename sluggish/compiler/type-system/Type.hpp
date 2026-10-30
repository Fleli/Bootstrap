#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
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
    std::unordered_map<std::string, std::size_t> member_indices;

    explicit Type(unsigned int bits);
    explicit Type(std::string name);
    explicit Type(const Type* pointee);

    bool is_word() const { return kind == Kind::Word; }
    bool is_struct() const { return kind == Kind::Struct; }
    bool is_pointer() const { return kind == Kind::Pointer; }
    const Type* pointee() const { return pointed_type; }
    // Scalar word width; zero for structs and pointers, whose storage layout is decided later.
    unsigned int bit_width() const { return width; }
    std::string name() const;
    std::uint16_t parse_literal(const std::string& text) const;

private:
    enum class Kind { Word, Struct, Pointer };

    Kind kind = Kind::Word;
    unsigned int width = 0;
    std::string type_name;
    const Type* pointed_type = nullptr;
};

inline const Type word_8{8};
inline const Type word_16{16};

const Type* resolve_type(const std::string& name);

} // namespace sluggish

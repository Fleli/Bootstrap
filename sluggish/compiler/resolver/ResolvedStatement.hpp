#pragma once

#include "type-system/Type.hpp"

#include <cstdint>
#include <string>

namespace sluggish {

struct ResolvedStatement {
    virtual ~ResolvedStatement() = default;
};

struct ResolvedDeclaration : public ResolvedStatement {

    std::string name;
    const Type* type = nullptr;

};

struct ResolvedAssignment : public ResolvedStatement {

    std::uint16_t value = 0;
    const ResolvedDeclaration* declaration = nullptr;
    const ResolvedDeclaration* source = nullptr; // Null means the RHS is the literal value.

};

} // namespace sluggish

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

    std::uint16_t value;
    const ResolvedDeclaration* declaration = nullptr;

};

} // namespace sluggish

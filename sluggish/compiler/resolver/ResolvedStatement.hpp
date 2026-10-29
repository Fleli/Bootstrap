#pragma once

#include "type-system/Type.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

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

struct ResolvedFunction {

    std::string name;
    const Type* return_type = nullptr;
    std::vector<std::unique_ptr<ResolvedDeclaration>> arguments;
    std::vector<std::unique_ptr<ResolvedStatement>> statements;

};

} // namespace sluggish

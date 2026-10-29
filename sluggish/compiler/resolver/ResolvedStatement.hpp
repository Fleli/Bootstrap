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

struct ResolvedExpression {

    const Type* type = nullptr;

    virtual ~ResolvedExpression() = default;

};

struct ResolvedNumberExpression : public ResolvedExpression {
    std::uint16_t value = 0;
};

struct ResolvedVariableExpression : public ResolvedExpression {
    const ResolvedDeclaration* declaration = nullptr;
};

struct ResolvedAssignment : public ResolvedStatement {

    std::unique_ptr<ResolvedExpression> value;
    const ResolvedDeclaration* declaration = nullptr;

};

struct ResolvedReturn : public ResolvedStatement {
    std::unique_ptr<ResolvedExpression> value;
};

struct ResolvedFunction {

    std::string name;
    const Type* return_type = nullptr;
    std::vector<std::unique_ptr<ResolvedDeclaration>> arguments;
    std::vector<std::unique_ptr<ResolvedStatement>> statements;

};

} // namespace sluggish

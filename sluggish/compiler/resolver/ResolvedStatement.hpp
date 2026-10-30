#pragma once

#include "type-system/Type.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace sluggish {

struct ResolvedFunction;

struct ResolvedStatement {
    virtual ~ResolvedStatement() = default;
};

struct ResolvedDeclaration : public ResolvedStatement {

    std::string name;
    const Type* type = nullptr;

    // Source-level self refers to the pointee of this hidden argument.
    bool is_self = false;

};

struct ResolvedExpression {

    const Type* type = nullptr;

    virtual bool is_lvalue() const { return false; }
    virtual ~ResolvedExpression() = default;

};

struct ResolvedNumberExpression : public ResolvedExpression {
    std::uint16_t value = 0;
};

struct ResolvedVariableExpression : public ResolvedExpression {

    const ResolvedDeclaration* declaration = nullptr;

    bool is_lvalue() const override { return true; }

};

struct ResolvedAddressOfExpression : public ResolvedExpression {
    std::unique_ptr<ResolvedExpression> operand;
};

struct ResolvedDereferenceExpression : public ResolvedExpression {

    std::unique_ptr<ResolvedExpression> operand;

    bool is_lvalue() const override { return true; }

};

struct ResolvedMemberExpression : public ResolvedExpression {

    std::size_t member_index;
    std::unique_ptr<ResolvedExpression> object;

    bool is_lvalue() const override { return object->is_lvalue(); }

};

struct ResolvedCallExpression : public ResolvedExpression {

    const ResolvedFunction* function = nullptr;
    std::vector<std::unique_ptr<ResolvedExpression>> arguments;

};

struct ResolvedAdditionExpression : public ResolvedExpression {

    std::unique_ptr<ResolvedExpression> left;
    std::unique_ptr<ResolvedExpression> right;

};

struct ResolvedAssignment : public ResolvedStatement {

    std::unique_ptr<ResolvedExpression> target;
    std::unique_ptr<ResolvedExpression> value;

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

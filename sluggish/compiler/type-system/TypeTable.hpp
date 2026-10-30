#pragma once

#include "Type.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace sluggish {

class TypeTable {
public:

    std::unordered_map<std::string, std::unique_ptr<Type>> structs;

    void register_struct(const std::string& name);
    const Type* pointer_to(const Type* pointee);
    const Type* resolve(const std::string& name);
    void check_recursive_members() const;

private:
    std::unordered_map<const Type*, std::unique_ptr<Type>> pointers;

};

} // namespace sluggish

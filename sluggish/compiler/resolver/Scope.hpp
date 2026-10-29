#pragma once

#include "ResolvedStatement.hpp"

#include <string>
#include <unordered_map>

namespace sluggish {

class Scope {
public:

    const Scope* parent = nullptr;

    // Names map to declarations owned by Resolver::statements.
    std::unordered_map<std::string, const ResolvedDeclaration*> symbols;

    const ResolvedDeclaration* find(const std::string& name) const {

        const auto symbol = symbols.find(name);

        if (symbol != symbols.end()) {
            return symbol->second;
        }

        if (parent != nullptr) {
            return parent->find(name);
        }

        return nullptr;

    }

};

} // namespace sluggish

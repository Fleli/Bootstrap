#pragma once

#include "parse/Parser.hpp"

#include <string>
#include <unordered_map>

namespace sluggish {

class Scope {
public:

    // Names map to declarations owned by Resolver::statements.
    std::unordered_map<std::string, const Declaration*> symbols;

    const Declaration* find(const std::string& name) const {

        const auto symbol = symbols.find(name);

        if (symbol == symbols.end()) {
            return nullptr;
        }

        return symbol->second;

    }

};

} // namespace sluggish

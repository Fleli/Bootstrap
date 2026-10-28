#pragma once

#include "resolver/Resolver.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace sluggish {

struct BasicBlockInstruction {
    virtual ~BasicBlockInstruction() = default;
};

// Slots identify local storage. The hardware backend decides its location.
struct AllocateInstruction : public BasicBlockInstruction {

    std::size_t slot;
    std::string type_name;

};

struct StoreInstruction : public BasicBlockInstruction {

    std::size_t slot;
    std::string value; // Number text until we define the built-in type.

};

struct BasicBlock {
    std::vector<std::unique_ptr<BasicBlockInstruction>> instructions;
};

BasicBlock lower(const Resolver& resolver);

} // namespace sluggish

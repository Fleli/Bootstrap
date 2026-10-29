#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace simplecpu {

class Emulator {
public:

    bool halted = false;
    std::uint32_t pc = 0;
    std::uint32_t stack_floor = 0; // End of loaded code; zero disables stack bounds checks.
    std::uint32_t registers[256]{}; // Register indices are 8 bits.
    std::vector<std::uint32_t> ram;

    explicit Emulator(std::size_t ram_words) : ram(ram_words) {}

    // Execute encoded instructions from pc. Stack programs must initialize r2.
    void run();

};

} // namespace simplecpu

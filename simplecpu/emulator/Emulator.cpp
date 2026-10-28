#include "Emulator.hpp"

#include <stdexcept>
#include <string>

namespace simplecpu {

void Emulator::run() {

    while (!halted) {

        const auto instruction = ram.at(pc++);
        const auto opcode = instruction >> 24;
        const auto a = (instruction >> 16) & 0xff;
        const auto b = (instruction >> 8) & 0xff;

        switch (opcode) {

            case 0: // halt
                halted = true;
                break;

            case 1: { // store_imm

                const auto address = ram.at(pc++);
                const auto value = ram.at(pc++);
                ram.at(address) = value;
                break;

            }

            case 2: // load_imm
                registers[a] = ram.at(pc++);
                break;

            case 3: // store
                ram.at(registers[a]) = registers[b];
                break;

            case 4: // load
                registers[a] = ram.at(registers[b]);
                break;

            default:
                throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));

        }

    }

}

} // namespace simplecpu

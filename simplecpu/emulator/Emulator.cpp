#include "Emulator.hpp"

#include <stdexcept>
#include <string>

namespace simplecpu {

void Emulator::run() {

    while (!halted) {

        const auto previous_stack_pointer = registers[2];
        const auto instruction = ram.at(pc++);
        const auto opcode = instruction >> 24;
        const auto a = (instruction >> 16) & 0xff;
        const auto b = (instruction >> 8) & 0xff;
        const auto c = instruction & 0xff;

        if (stack_floor != 0 && (opcode == 6 || opcode == 8) && registers[2] <= stack_floor) {
            throw std::runtime_error("Stack overlaps program code");
        }

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

            case 5: // add
                registers[a] = registers[b] + registers[c];
                break;

            case 6: { // push

                const auto value = registers[a];
                --registers[2];
                ram.at(registers[2]) = value;
                break;

            }

            case 7: { // pop

                const auto value = ram.at(registers[2]);
                ++registers[2];
                registers[a] = value;
                break;

            }

            case 8: { // call

                const auto address = ram.at(pc++);
                --registers[2];
                ram.at(registers[2]) = pc;
                pc = address;
                break;

            }

            case 9: // ret
                pc = ram.at(registers[2]);
                ++registers[2];
                break;

            default:
                throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));

        }

        if (stack_floor != 0 && registers[2] != previous_stack_pointer &&
            (registers[2] < stack_floor || registers[2] > ram.size())) {
            throw std::runtime_error("Stack exceeds available RAM");
        }

    }

}

} // namespace simplecpu

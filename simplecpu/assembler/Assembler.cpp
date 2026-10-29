#include "Assembler.hpp"

#include <stdexcept>

namespace simplecpu {

std::vector<std::uint32_t> assemble(const std::vector<Instruction>& instructions) {

    std::vector<std::uint32_t> words;

    for (const auto& instruction : instructions) {

        const auto a = instruction.operand_a;
        const auto b = instruction.operand_b;
        const auto c = instruction.operand_c;

        switch (instruction.opcode) {

            case Opcode::Halt:
                words.push_back(0);
                break;

            case Opcode::StoreImm:
                words.push_back(1u << 24);
                words.push_back(a);
                words.push_back(b);
                break;

            case Opcode::LoadImm:
                if (a > 255) {
                    throw std::runtime_error("Register exceeds r255");
                }

                words.push_back((2u << 24) | (a << 16));
                words.push_back(b);
                break;

            case Opcode::Store:
                if (a > 255 || b > 255) {
                    throw std::runtime_error("Register exceeds r255");
                }

                words.push_back((3u << 24) | (a << 16) | (b << 8));
                break;

            case Opcode::Load:
                if (a > 255 || b > 255) {
                    throw std::runtime_error("Register exceeds r255");
                }

                words.push_back((4u << 24) | (a << 16) | (b << 8));
                break;

            case Opcode::Add:
                if (a > 255 || b > 255 || c > 255) {
                    throw std::runtime_error("Register exceeds r255");
                }

                words.push_back((5u << 24) | (a << 16) | (b << 8) | c);
                break;

            case Opcode::Push:
                if (a > 255) {
                    throw std::runtime_error("Register exceeds r255");
                }

                words.push_back((6u << 24) | (a << 16));
                break;

            case Opcode::Pop:
                if (a > 255) {
                    throw std::runtime_error("Register exceeds r255");
                }

                words.push_back((7u << 24) | (a << 16));
                break;

            case Opcode::Call:
                words.push_back(8u << 24);
                words.push_back(a);
                break;

            case Opcode::Ret:
                words.push_back(9u << 24);
                break;

            default:
                throw std::runtime_error("Unknown SimpleCPU opcode");

        }

    }

    return words;

}

} // namespace simplecpu

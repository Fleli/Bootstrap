#include "Instruction.hpp"

#include <stdexcept>

namespace simplecpu {

std::string Instruction::text() const {

    switch (opcode) {

        case Opcode::Halt:
            return "halt";

        case Opcode::StoreImm:
            return "store_imm       " + std::to_string(operand_a) + ", " + std::to_string(operand_b);

        case Opcode::LoadImm:
            return "load_imm        r" + std::to_string(operand_a) + ", " + std::to_string(operand_b);

        case Opcode::Store:
            return "store           r" + std::to_string(operand_a) + ", r" + std::to_string(operand_b);

        case Opcode::Load:
            return "load            r" + std::to_string(operand_a) + ", r" + std::to_string(operand_b);

        case Opcode::Add:
            return "add             r" + std::to_string(operand_a) + ", r" + std::to_string(operand_b) + ", r" + std::to_string(operand_c);

        case Opcode::Push:
            return "push            r" + std::to_string(operand_a);

        case Opcode::Pop:
            return "pop             r" + std::to_string(operand_a);

        case Opcode::Call:
            return "call            " + std::to_string(operand_a);

        case Opcode::Ret:
            return "ret";

    }

    throw std::runtime_error("Unknown SimpleCPU opcode");

}

} // namespace simplecpu

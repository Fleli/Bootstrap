#include "Emulator.hpp"

#include <iostream>

int main() {

    simplecpu::Emulator cpu(9);

    // Encoded example until the assembler is added.
    cpu.ram = {
        0x01000000, 7, 6,      // store_imm 7, 6
        0x01000000, 8, 35000,  // store_imm 8, 35000
        0,                    // halt
        0, 0                  // variable storage
    };

    cpu.run();

    std::cout << cpu.ram[7] << "\n";
    std::cout << cpu.ram[8] << "\n";

}

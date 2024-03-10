#pragma once

#include <memory>
#include <string>
#include <map>
#include <iostream>

#include "base.h"
#include "instructions.h"

class cpu {
public:
    cpu();
    ~cpu();

    status decodeInstruction();
    status executeInstruction();
private:
    cpu_base_properties cpuProperties;

    cpu_register_t* instructionPtr;
    instructions::instruction_base* currentInstruction;

    cpu_register_t statusRegister;
    const std::unique_ptr<cpu_register_t[]> registers;
    const std::unique_ptr<std::uint8_t[]> memory;
    std::map<cpu_register_t, instructions::instruction_base*> cpuInstructions;
};

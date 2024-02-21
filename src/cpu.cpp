#include "cpu.h"

cpu::cpu() : cpuProperties(), instructionPtr(nullptr), currentInstruction(nullptr), statusRegister(0),
             registers(new cpu_register_t[cpuProperties.registersCount]), memory(new std::uint8_t[cpuProperties.memorySize])
{
    cpu_register_t opCode = 0x0;
    cpu_register_t opCodeOffset = 0x1 << (cpuProperties.registerSize - cpuProperties.bitsPerInstruction);

    cpuInstructions[opCode] = new instructions::load(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
    cpuInstructions[opCode] = new instructions::store(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
    cpuInstructions[opCode] = new instructions::load_immediate(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
    cpuInstructions[opCode] = new instructions::addition(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
    cpuInstructions[opCode] = new instructions::subtraction(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
    cpuInstructions[opCode] = new instructions::multiplication(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
    cpuInstructions[opCode] = new instructions::shift_right_logical(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
    cpuInstructions[opCode] = new instructions::shift_left_logical(registers.get(), memory.get(), cpuProperties); opCode += opCodeOffset;
}

cpu::~cpu()
{
    for (auto instruction : cpuInstructions)
        delete instruction.second;
}

status cpu::decodeInstruction()
{
    status st = status::STATUS_OK;
    cpu_register_t instruction = *instructionPtr;
    cpu_register_t instructionOpCode = cpuProperties.instructionMask & instruction;
    currentInstruction = cpuInstructions[instructionOpCode];
    currentInstruction->setCurrentInstruction(instruction);
    if ((st = currentInstruction->decodeOperands()) < status::UNKNOWN_WARNING) {
        std::cerr << "Error: cpu::decodeInstruction, code - " << (int)st << std::endl;
        return st;
    }

    return st;
}

status cpu::executeInstruction()
{
    status st = status::STATUS_OK;
    if ((st = currentInstruction->executeInstruction()) < status::UNKNOWN_WARNING) {
        std::cerr << "Error: cpu::executeInstruction, code - " << (int)st << std::endl;
        return st;
    }

    return st;
}

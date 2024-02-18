#pragma once

#include <string>
#include <iostream>

#include "base.h"

class cpu;

namespace instructions {

class instruction_base {
public:
    virtual ~instruction_base() = default;
    virtual status decodeOperands();
    virtual status executeInstruction();
    inline void setCurrentInstruction(const cpu_register_t _currentInstruction) { currentInstruction = _currentInstruction; }
protected:
    instruction_base(const std::string& _name, cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);

    const std::string name;
    cpu_register_t currentInstruction;
    cpu_register_t* const registers;
    std::uint8_t* const memory;
    const cpu_base_properties& cpuProperties;
};

class load: public instruction_base {
public:
    load(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status decodeOperands() override;
    status executeInstruction() override;
protected:
    std::uint32_t dstRegisterIndex;
    std::uint32_t srcAddressRegisterIndex;
    cpu_register_t immediateMemoryOffset;
};

class store : public instruction_base {
public:
    store(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status decodeOperands() override;
    status executeInstruction() override;
protected:
    std::uint32_t dstAddressRegisterIndex;
    std::uint32_t srcRegisterIndex;
    cpu_register_t immediateMemoryOffset;
};

class load_immediate : public instruction_base {
public:
    load_immediate(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status decodeOperands() override;
    status executeInstruction() override;
protected:
    std::uint32_t dstRegisterIndex;
    cpu_register_t data;
    bool isUpper;
};

}

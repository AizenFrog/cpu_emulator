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


class math_base : public instruction_base {
public:
    status decodeOperands() override;
protected:
    math_base(const std::string& _name, cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);

    bool isImmediate;
    std::uint32_t dstSrcRegisterIndex;
    cpu_register_t srcData;
};

class addition : public math_base {
public:
    addition(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

class subtraction : public math_base {
public:
    subtraction(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

class multiplication : public math_base {
public:
    multiplication(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

class shift_right_logical : public math_base {
public:
    shift_right_logical(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

class shift_left_logical : public math_base {
public:
    shift_left_logical(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

class bitwise_not : public instruction_base {
public:
    bitwise_not(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status decodeOperands() override;
    status executeInstruction() override;
protected:
    std::uint32_t dstSrcRegisterIndex;
};

class bitwise_base : public instruction_base {
public:
    status decodeOperands() override;
protected:
    bitwise_base(const std::string& _name, cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);

    std::uint32_t dstRegisterIndex;
    std::uint32_t srcRegisterIndex;
};

class bitwise_and : public bitwise_base {
public:
    bitwise_and(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

class bitwise_or : public bitwise_base {
public:
    bitwise_or(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

class bitwise_xor : public bitwise_base {
public:
    bitwise_xor(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties);
    status executeInstruction() override;
};

}

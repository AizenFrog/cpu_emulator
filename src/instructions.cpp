#include "instructions.h"

namespace instructions {

instruction_base::instruction_base(const std::string& _name,
                                   cpu_register_t* const _registers,
                                   std::uint8_t* const _memory,
                                   const cpu_base_properties& _cpuProperties) :
    name(_name),
    currentInstruction(0),
    registers(_registers),
    memory(_memory),
    cpuProperties(_cpuProperties) {}

status instruction_base::decodeOperands()
{
    return status::DECODE_UNKNOWN_INSTRUCTION;
}

status instruction_base::executeInstruction()
{
    return status::ATTEMPT_TO_EXECUTE_UNKNOWN_INSTRUCTION;
}

load::load(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties) :
    instruction_base("ld", _registers, _memory, _cpuProperties), dstRegisterIndex(0x0), srcAddressRegisterIndex(0x0),immediateMemoryOffset(0x0) {}

status load::decodeOperands()
{
    cpu_register_t registerMask = cpuProperties.registersCount - 1;
    std::uint32_t dstRegisterOffset = cpuProperties.registerSize - cpuProperties.bitsPerInstruction - cpuProperties.bitsPerRegister;
    std::uint32_t srcRegisterOffset = dstRegisterOffset - cpuProperties.bitsPerRegister;
    cpu_register_t dstRegisterMask = registerMask << dstRegisterOffset;
    cpu_register_t srcRegisterMask = registerMask << srcRegisterOffset;
    cpu_register_t immediateAddressMask = (0x1 << srcRegisterOffset) - 1;

    dstRegisterIndex = (dstRegisterMask & currentInstruction) >> dstRegisterOffset;
    srcAddressRegisterIndex = (srcRegisterMask & currentInstruction) >> srcRegisterOffset;
    immediateMemoryOffset = getSignValue<cpu_register_t>(immediateAddressMask & currentInstruction, srcRegisterOffset - 1);
    return status::STATUS_OK;
}

status load::executeInstruction()
{
    std::uint32_t efficientAddress;
    if (immediateMemoryOffset & signBitMask)
        efficientAddress = (cpu_register_t)(registers[srcAddressRegisterIndex] + immediateMemoryOffset);
    else
        efficientAddress = registers[srcAddressRegisterIndex] + immediateMemoryOffset;

    if (efficientAddress >= cpuProperties.memorySize) {
        LOG("load::executeInstruction()", status::OUT_OF_MEMORY_ERROR);
        return status::OUT_OF_MEMORY_ERROR;
    }
    if (efficientAddress > cpuProperties.memorySize - sizeof(cpu_register_t)) {
        std::uint32_t bytesToLoad = cpuProperties.memorySize - efficientAddress;
        registers[dstRegisterIndex] = 0;
        for (std::uint32_t i = 0; i < bytesToLoad; ++i)
            registers[dstRegisterIndex] |= (cpu_register_t)memory[efficientAddress + i] << (BITS_IN_BYTE * i);

        LOG("load::executeInstruction()", status::LAST_MEMORY_BYTE_WARNING);
        return status::LAST_MEMORY_BYTE_WARNING;
    }

    registers[dstRegisterIndex] = *reinterpret_cast<cpu_register_t*>(&memory[efficientAddress]);

    return status::STATUS_OK;
}

store::store(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties) :
    instruction_base("st", _registers, _memory, _cpuProperties), dstAddressRegisterIndex(0x0), srcRegisterIndex(0x0), immediateMemoryOffset(0x0) {}

status store::decodeOperands()
{
    cpu_register_t registerMask = cpuProperties.registersCount - 1;
    std::uint32_t dstRegisterOffset = cpuProperties.registerSize - cpuProperties.bitsPerInstruction - cpuProperties.bitsPerRegister;
    std::uint32_t srcRegisterOffset = dstRegisterOffset - cpuProperties.bitsPerRegister;
    cpu_register_t dstRegisterMask = registerMask << dstRegisterOffset;
    cpu_register_t srcRegisterMask = registerMask << srcRegisterOffset;
    cpu_register_t immediateAddressMask = (0x1 << srcRegisterOffset) - 1;

    dstAddressRegisterIndex = (dstRegisterMask & currentInstruction) >> dstRegisterOffset;
    srcRegisterIndex = (srcRegisterMask & currentInstruction) >> srcRegisterOffset;
    immediateMemoryOffset = getSignValue<cpu_register_t>(immediateAddressMask & currentInstruction, srcRegisterOffset - 1);
    return status::STATUS_OK;
}

status store::executeInstruction()
{
    std::uint32_t efficientAddress;
    if (immediateMemoryOffset & signBitMask)
        efficientAddress = (cpu_register_t)(registers[dstAddressRegisterIndex] + immediateMemoryOffset);
    else
        efficientAddress = registers[dstAddressRegisterIndex] + immediateMemoryOffset;

    if (efficientAddress >= cpuProperties.memorySize) {
        LOG("store::executeInstruction()", status::OUT_OF_MEMORY_ERROR);
        return status::OUT_OF_MEMORY_ERROR;
    }
    if (efficientAddress > cpuProperties.memorySize - sizeof(cpu_register_t)) {
        std::uint32_t bytesToStore = cpuProperties.memorySize - efficientAddress;
        for (std::uint32_t i = 0; i < bytesToStore; ++i)
            memory[efficientAddress + i] = registers[srcRegisterIndex] >> (BITS_IN_BYTE * i);

        LOG("store::executeInstruction()", status::LAST_MEMORY_BYTE_WARNING);
        return status::LAST_MEMORY_BYTE_WARNING;
    }

    *reinterpret_cast<cpu_register_t*>(&memory[efficientAddress]) = registers[srcRegisterIndex];
    return status::STATUS_OK;
}

load_immediate::load_immediate(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties) :
    instruction_base("ldi", _registers, _memory, _cpuProperties), dstRegisterIndex(0x0), data(0x0), isUpper(false) {}

status load_immediate::decodeOperands()
{
    cpu_register_t registerMask = cpuProperties.registersCount - 1;
    std::uint32_t dstRegisterOffset = cpuProperties.registerSize - cpuProperties.bitsPerInstruction - cpuProperties.bitsPerRegister;
    std::uint32_t isUpperOffset = dstRegisterOffset - 1;
    cpu_register_t dstRegisterMask = registerMask << dstRegisterOffset;
    cpu_register_t isUpperBitMask = 0x1 << isUpperOffset;
    cpu_register_t immediateValueMask = (0x1 << BITS_IN_BYTE * (sizeof(cpu_register_t) / 2)) - 1;

    dstRegisterIndex = (currentInstruction & dstRegisterMask) >> dstRegisterOffset;
    isUpper = currentInstruction & isUpperBitMask;
    data = currentInstruction & immediateValueMask;
    return status::STATUS_OK;
}

status load_immediate::executeInstruction()
{
    std::uint32_t offset = BITS_IN_BYTE * (sizeof(cpu_register_t) / 2);
    if (isUpper) {
        registers[dstRegisterIndex] &= (cpu_register_t)-1 >> offset;
        registers[dstRegisterIndex] |= data << offset;
    }
    else {
        registers[dstRegisterIndex] &= (cpu_register_t)-1 << offset;
        registers[dstRegisterIndex] |= data;
    }
    return status::STATUS_OK;
}


math_base::math_base(const std::string& _name,
                     cpu_register_t* const _registers,
                     std::uint8_t* const _memory,
                     const cpu_base_properties& _cpuProperties) :
    instruction_base(_name, _registers, _memory, _cpuProperties), isImmediate(false), dstSrcRegisterIndex(0x0), srcData(0x0) {}

status math_base::decodeOperands()
{
    std::uint32_t isImmediateOffset =  cpuProperties.registerSize - cpuProperties.bitsPerInstruction - 1;
    std::uint32_t dstRegisterOffset = isImmediateOffset - cpuProperties.bitsPerRegister;
    cpu_register_t registerMask = cpuProperties.registersCount - 1;
    cpu_register_t isImmediateBitMask = 0x1 << isImmediateOffset;
    cpu_register_t dstRegisterMask = registerMask << dstRegisterOffset;

    isImmediate = currentInstruction & isImmediateBitMask;
    dstSrcRegisterIndex = (currentInstruction & dstRegisterMask) >> dstRegisterOffset;
    if (isImmediate) {
        cpu_register_t immediateValueMask = (0x1 << dstRegisterOffset) - 1;
        srcData = getSignValue<cpu_register_t>(currentInstruction & immediateValueMask, dstRegisterOffset - 1);
    }
    else {
        cpu_register_t srcRegisterOffset = dstRegisterOffset - cpuProperties.bitsPerRegister;
        cpu_register_t srcRegisterMask = registerMask << srcRegisterOffset;
        srcData = (currentInstruction & srcRegisterMask) >> srcRegisterOffset;
    }

    return status::STATUS_OK;
}

addition::addition(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties) :
    math_base("add", _registers, _memory, _cpuProperties) {}

status addition::executeInstruction()
{
    if (isImmediate)
        registers[dstSrcRegisterIndex] += srcData;
    else
        registers[dstSrcRegisterIndex] += registers[srcData];

    return status::STATUS_OK;
}

subtraction::subtraction(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties) :
    math_base("sub", _registers, _memory, _cpuProperties) {}

status subtraction::executeInstruction()
{
    if (isImmediate)
        registers[dstSrcRegisterIndex] -= srcData;
    else
        registers[dstSrcRegisterIndex] -= registers[srcData];

    return status::STATUS_OK;
}

multiplication::multiplication(cpu_register_t* const _registers, std::uint8_t* const _memory, const cpu_base_properties& _cpuProperties) :
    math_base("mul", _registers, _memory, _cpuProperties) {}

status multiplication::executeInstruction()
{
    if (isImmediate)
        registers[dstSrcRegisterIndex] *= srcData;
    else
        registers[dstSrcRegisterIndex] *= registers[srcData];

    return status::STATUS_OK;
}

shift_right_logical::shift_right_logical(cpu_register_t* const _registers,
                                         std::uint8_t* const _memory,
                                         const cpu_base_properties& _cpuProperties) :
    math_base("srl", _registers, _memory, _cpuProperties) {}

status shift_right_logical::executeInstruction()
{
    if (isImmediate) {
        if (srcData > cpuProperties.registerSize) {
            LOG("shift_right_logical::executeInstruction()", status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
            return status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH;
        }
        registers[dstSrcRegisterIndex] >>= srcData;
    }
    else {
        if (registers[srcData] > cpuProperties.registerSize) {
            LOG("shift_right_logical::executeInstruction()", status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
            return status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH;
        }
        registers[dstSrcRegisterIndex] >>= registers[srcData];
    }

    return status::STATUS_OK;
}

shift_left_logical::shift_left_logical(cpu_register_t* const _registers,
                                       std::uint8_t* const _memory,
                                       const cpu_base_properties& _cpuProperties) :
    math_base("sll", _registers, _memory, _cpuProperties) {}

status shift_left_logical::executeInstruction()
{
    if (isImmediate) {
        if (srcData > cpuProperties.registerSize) {
            LOG("shift_left_logical::executeInstruction()", status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
            return status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH;
        }
        registers[dstSrcRegisterIndex] <<= srcData;
    }
    else {
        if (registers[srcData] > cpuProperties.registerSize) {
            LOG("shift_left_logical::executeInstruction()", status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
            return status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH;
        }
        registers[dstSrcRegisterIndex] <<= registers[srcData];
    }

    return status::STATUS_OK;
}

}

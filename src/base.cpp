#include <cmath>
#include "base.h"

// Max address space that can be indexed by cpu is depends on register
// width, so if we have 16-bit register, it can indexed 2^16 bytes.
cpu_base_properties::cpu_base_properties(const std::uint32_t _maxInstructionsCount,
                                         const std::uint32_t _registersCount,
                                         const std::uint32_t _registerSize) :
    maxInstructionsCount(_maxInstructionsCount),
    registersCount(_registersCount),
    registerSize(_registerSize),
    bitsPerInstruction(log2(maxInstructionsCount)),
    bitsPerRegister(log2(registersCount)),
    instructionMask(0xffffffffffffffff << (registerSize - bitsPerInstruction)), // 0xff.. is long because of 64-bit cpu in future
    memorySize(maxSupportedMemory) {}

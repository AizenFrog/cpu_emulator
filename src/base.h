#pragma once

#include <cstdint>

#define KB(a) (a * 1024)
#define BITS_IN_BYTE 8

using cpu_register_t = std::uint16_t;

constexpr std::uint64_t maxSupportedMemory = (std::uint64_t)((cpu_register_t)-1) + 1;
constexpr std::uint32_t signBitIndex = (sizeof(cpu_register_t) * 8) - 1;
constexpr cpu_register_t signBitMask = 0x1 << signBitIndex;

enum class status : std::int32_t {
    UNKNOWN_ERROR = -1000,
    DECODE_UNKNOWN_INSTRUCTION,
    ATTEMPT_TO_EXECUTE_UNKNOWN_INSTRUCTION,
    OUT_OF_MEMORY_ERROR,
    SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH,
    UNKNOWN_WARNING = -500,
    LAST_MEMORY_BYTE_WARNING, // if load 64K - 1 byte, because load at least 2 bytes
    STATUS_OK = 0
};

#ifdef LOGGING
#define LOG(function, code)                                         \
    if ((int)(code) < 500)                                          \
        std::cerr << "Error: " function ", " #code << std::endl;    \
    else                                                            \
        std::cerr << "Warning: " function ", " #code << std::endl;
#else
#define LOG(function, code)
#endif

template <typename T>
inline T getSignValue(T value, std::uint32_t lastBitIndex)
{
    T mainValueMask = (0x1 << lastBitIndex) - 1;
    T result = value & mainValueMask;
    if (value & (0x1 << lastBitIndex))
        result = result | ((T)-1 << lastBitIndex);
    return result;
}

struct cpu_base_properties {
    cpu_base_properties(const std::uint32_t _maxInstructionsCount = 16, const std::uint32_t _registersCount = 8, const std::uint32_t _registerSize = 16);

    const std::uint32_t maxInstructionsCount;
    const std::uint32_t registersCount;
    const std::uint32_t registerSize; // in bits

    const std::uint32_t bitsPerInstruction;
    const std::uint32_t bitsPerRegister;

    const cpu_register_t instructionMask;

    const std::uint32_t memorySize;
};

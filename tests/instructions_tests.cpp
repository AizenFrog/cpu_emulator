#include <memory>

#include "gtest/gtest.h"
#include "instructions.h"

#define DECODE_AND_EXECUTE(st) instruction.decodeOperands(); \
                               st = instruction.executeInstruction();

class InstructionsTests : public testing::Test {
protected:
    InstructionsTests() : testing::Test(), memory(new std::uint8_t[KB(64)]{}), registers(new cpu_register_t[8]{}), properties() {}

    std::unique_ptr<std::uint8_t[]> memory;
    std::unique_ptr<cpu_register_t[]> registers;
    cpu_base_properties properties;
};

// Load 2 bytes from 0 and 1 addresses to 0 register
TEST_F(InstructionsTests, base_load)
{
    status st;
    instructions::load instruction(registers.get(), memory.get(), properties);
    memory[0] = 170; memory[1] = 170;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)43690);
}

TEST_F(InstructionsTests, load_with_immediate_value)
{
    status st;
    instructions::load instruction(registers.get(), memory.get(), properties);
    memory[1] = 170; memory[2] = 170;
    registers[1] = 0;
    cpu_register_t currentInstruction = 0b0000'0000'0100'0001; // dst register 0, src register value 0, immediate value 1
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)43690);
}

TEST_F(InstructionsTests, load_with_negative_immediate_value)
{
    status st;
    instructions::load instruction(registers.get(), memory.get(), properties);
    memory[0] = 170; memory[1] = 170;
    registers[1] = 32;
    cpu_register_t currentInstruction = 0b0000'0000'0110'0000; // dst register 0, src register value 0, immediate value 1
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)43690);
}

TEST_F(InstructionsTests, load_register_and_immediate)
{
    status st;
    instructions::load instruction(registers.get(), memory.get(), properties);
    memory[5] = 170; memory[6] = 170;
    registers[1] = (cpu_register_t)4;
    cpu_register_t currentInstruction = 0b0000'0000'0100'0001; // dst register 0, src register value 4, immediate value 1
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)43690);
}

TEST_F(InstructionsTests, load_last_byte)
{
    status st;
    instructions::load instruction(registers.get(), memory.get(), properties);
    memory[KB(64) - 1] = 170;
    registers[1] = (cpu_register_t)65535;
    cpu_register_t currentInstruction = 0b0000'0000'0100'0000; // dst register 0, src 1, immediate value 0
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::LAST_MEMORY_BYTE_WARNING);
    EXPECT_EQ(registers[0], (cpu_register_t)170);
}

TEST_F(InstructionsTests, load_out_of_memory)
{
    status st;
    instructions::load instruction(registers.get(), memory.get(), properties);
    registers[1] = (cpu_register_t)65535;
    cpu_register_t currentInstruction = 0b0000'0000'0100'0001; // dst register 0, src 1, immediate value 0
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::OUT_OF_MEMORY_ERROR);
}

TEST_F(InstructionsTests, base_store)
{
    status st;
    instructions::store instruction(registers.get(), memory.get(), properties);
    registers[0] = 43690;
    cpu_register_t currentInstruction = 0b0000'0010'0000'0000; // dst register 1 => 0, src register value 43690, immediate value 0
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(memory[0], 170);
    EXPECT_EQ(memory[1], 170);
}

TEST_F(InstructionsTests, store_with_immediate_value)
{
    status st;
    instructions::store instruction(registers.get(), memory.get(), properties);
    registers[0] = 43690;
    cpu_register_t currentInstruction = 0b0000'0010'0000'0001; // dst register 0, src register value 0, immediate value 1
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(memory[1], 170);
    EXPECT_EQ(memory[2], 170);
}

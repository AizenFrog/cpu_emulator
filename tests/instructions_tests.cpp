#include <memory>

#include "gtest/gtest.h"
#include "instructions.h"

#define DECODE_AND_EXECUTE(st) instruction.decodeOperands(); \
                               st = instruction.executeInstruction();

class InstructionsTests : public testing::Test {
protected:
    InstructionsTests() : testing::Test(), memory(new std::uint8_t[maxSupportedMemory]{}), registers(new cpu_register_t[8]{}), properties() {}

    std::unique_ptr<std::uint8_t[]> memory;
    std::unique_ptr<cpu_register_t[]> registers;
    cpu_base_properties properties;

    status st;
};

// Load 2 bytes from 0 and 1 addresses to 0 register
TEST_F(InstructionsTests, base_load)
{
    instructions::load instruction(registers.get(), memory.get(), properties);
    memory[0] = 170; memory[1] = 170;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)43690);
}

TEST_F(InstructionsTests, load_with_immediate_value)
{
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
    instructions::load instruction(registers.get(), memory.get(), properties);
    memory[maxSupportedMemory - 1] = 170;
    registers[1] = maxSupportedMemory - 1;
    cpu_register_t currentInstruction = 0b0000'0000'0100'0000; // dst register 0, src 1, immediate value 0
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::LAST_MEMORY_BYTE_WARNING);
    EXPECT_EQ(registers[0], (cpu_register_t)170);
}

TEST_F(InstructionsTests, load_out_of_memory)
{
    instructions::load instruction(registers.get(), memory.get(), properties);
    registers[1] = maxSupportedMemory - 1;
    cpu_register_t currentInstruction = 0b0000'0000'0100'0001; // dst register 0, src 1, immediate value 0
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::OUT_OF_MEMORY_ERROR);
}


TEST_F(InstructionsTests, base_store)
{
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
    instructions::store instruction(registers.get(), memory.get(), properties);
    registers[0] = 43690;
    cpu_register_t currentInstruction = 0b0000'0010'0000'0001; // dst register 1, src register 0, immediate value 1
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(memory[1], 170);
    EXPECT_EQ(memory[2], 170);
}

TEST_F(InstructionsTests, store_with_negative_immediate_value)
{
    instructions::store instruction(registers.get(), memory.get(), properties);
    registers[0] = 43690; registers[1] = 32;
    cpu_register_t currentInstruction = 0b0000'0010'0010'0000; // dst register 1, src register 0, immediate value -32
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(memory[0], 170);
    EXPECT_EQ(memory[1], 170);
}

TEST_F(InstructionsTests, store_register_and_immediate)
{
    instructions::store instruction(registers.get(), memory.get(), properties);
    registers[0] = 43690; registers[1] = 32;
    cpu_register_t currentInstruction = 0b0000'0010'0000'0010; // dst register 1, src register 0, immediate value 2
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(memory[34], 170);
    EXPECT_EQ(memory[35], 170);
}

TEST_F(InstructionsTests, store_last_byte)
{
    instructions::store instruction(registers.get(), memory.get(), properties);
    registers[0] = 43690; registers[1] = maxSupportedMemory - 1;
    cpu_register_t currentInstruction = 0b0000'0010'0000'0000; // dst register 1, src register 0, immediate value 0
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::LAST_MEMORY_BYTE_WARNING);
    EXPECT_EQ(memory[maxSupportedMemory - 1], 170);
}

TEST_F(InstructionsTests, store_out_of_memory)
{
    instructions::store instruction(registers.get(), memory.get(), properties);
    registers[1] = maxSupportedMemory - 1;
    cpu_register_t currentInstruction = 0b0000'0010'0000'0001; // dst register 1, src register 0, immediate value 1
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::OUT_OF_MEMORY_ERROR);
}


TEST_F(InstructionsTests, load_immediate_lower_byte)
{
    instructions::load_immediate instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'1010'1010; // dst register 0, isUpper bit is false, immediate value 170
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 170);
}

TEST_F(InstructionsTests, load_immediate_upper_byte)
{
    instructions::load_immediate instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0001'1010'1010; // dst register 0, isUpper bit is true, immediate value 170
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 43520);
}

TEST_F(InstructionsTests, load_immediate_lower_byte_than_upper_byte)
{
    instructions::load_immediate instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'1010'1010; // dst register 0, isUpper bit is false, immediate value 170
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);
    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 170);

    currentInstruction = 0b0000'0001'1010'1010; // dst register 0, isUpper bit is true, immediate value 170
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);
    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 43690);
}

TEST_F(InstructionsTests, load_immediate_upper_byte_than_lower_byte)
{
    instructions::load_immediate instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0001'1010'1010; // dst register 0, isUpper bit is true, immediate value 170
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);
    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 43520);

    currentInstruction = 0b0000'0000'1010'1010; // dst register 0, isUpper bit is false, immediate value 170
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);
    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 43690);
}


TEST_F(InstructionsTests, addition_with_positive_immediate_value)
{
    instructions::addition instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0111'1111; // isImmediate bit is true, dst register 0, immediate value 127
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 5;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 132);
}

TEST_F(InstructionsTests, addition_with_negative_immediate_value)
{
    instructions::addition instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'1010'1010; // isImmediate bit is true, dst register 0, immediate value -86
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 5;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)-81);
}

TEST_F(InstructionsTests, addition_with_register_value_base)
{
    instructions::addition instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, scr register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 5, registers[1] = 10;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 15);
}

TEST_F(InstructionsTests, addition_with_register_value_with_overflow)
{
    instructions::addition instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, scr register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 32769, registers[1] = 32768;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 1);
}


TEST_F(InstructionsTests, subtraction_with_positive_immediate_value)
{
    instructions::subtraction instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0000'0101; // isImmediate bit is true, dst register 0, immediate value 5
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 10;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 5);
}

TEST_F(InstructionsTests, subtraction_with_negative_immediate_value)
{
    instructions::subtraction instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'1010'1010; // isImmediate bit is true, dst register 0, immediate value -86
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 5;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 91);
}

TEST_F(InstructionsTests, subtraction_with_register_value_base)
{
    instructions::subtraction instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, scr register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 10, registers[1] = 5;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 5);
}

TEST_F(InstructionsTests, subtraction_with_register_value_with_overflow)
{
    instructions::subtraction instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, scr register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 10, registers[1] = 11;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)-1);
}


TEST_F(InstructionsTests, multiplication_with_positive_immediate_value)
{
    instructions::multiplication instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0000'0101; // isImmediate bit is true, dst register 0, immediate value 5
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 10;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 50);
}

TEST_F(InstructionsTests, multiplication_with_negative_immediate_value)
{
    instructions::multiplication instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'1010'1010; // isImmediate bit is true, dst register 0, immediate value -86
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 5;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], (cpu_register_t)-430);
}

TEST_F(InstructionsTests, multiplication_with_register_value_base)
{
    instructions::multiplication instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, scr register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 10, registers[1] = 5;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 50);
}

TEST_F(InstructionsTests, multiplication_with_register_value_with_overflow)
{
    instructions::multiplication instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, scr register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 1024, registers[1] = 65;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 1024);
}


TEST_F(InstructionsTests, shift_right_logical_immediate_base)
{
    instructions::shift_right_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0000'0010; // isImmediate bit is true, dst register 0, immediate value 2
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 24;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 24 >> 2);
}

TEST_F(InstructionsTests, shift_right_logical_immediate_more_than_16_bits)
{
    instructions::shift_right_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0001'1000; // isImmediate bit is true, dst register 0, immediate value 24
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}

TEST_F(InstructionsTests, shift_right_logical_negative_immediate)
{
    instructions::shift_right_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0010'0000; // isImmediate bit is true, dst register 0, immediate value -32
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}

TEST_F(InstructionsTests, shift_right_logical_base)
{
    instructions::shift_right_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 24; registers[1] = 2;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 24 >> 2);
}

TEST_F(InstructionsTests, shift_right_logical_more_than_16_bits)
{
    instructions::shift_right_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[1] = 24;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}

TEST_F(InstructionsTests, shift_right_logical_negative)
{
    instructions::shift_right_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[1] = (cpu_register_t)-1;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}


TEST_F(InstructionsTests, shift_left_logical_immediate_base)
{
    instructions::shift_left_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0000'0010; // isImmediate bit is true, dst register 0, immediate value 2
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 6;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 6 << 2);
}

TEST_F(InstructionsTests, shift_left_logical_immediate_more_than_16_bits)
{
    instructions::shift_left_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0001'1000; // isImmediate bit is true, dst register 0, immediate value 24
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}

TEST_F(InstructionsTests, shift_left_logical_negative_immediate)
{
    instructions::shift_left_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'1000'0010'0000; // isImmediate bit is true, dst register 0, immediate value -32
    instruction.setCurrentInstruction(currentInstruction);
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}

TEST_F(InstructionsTests, shift_left_logical_base)
{
    instructions::shift_left_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 6; registers[1] = 2;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 6 << 2);
}

TEST_F(InstructionsTests, shift_left_logical_more_than_16_bits)
{
    instructions::shift_left_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[1] = 24;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}

TEST_F(InstructionsTests, shift_left_logical_negative)
{
    instructions::shift_left_logical instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0010'0000; // isImmediate bit is false, dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[1] = (cpu_register_t)-1;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::SHIFT_BY_NEGATIVE_VALUE_OR_VALUE_MORE_THAN_CPU_BIT_DEPTH);
}

TEST_F(InstructionsTests, bitwise_not)
{
    instructions::bitwise_not instruction(registers.get(), memory.get(), properties);
    registers[0] = 0b0101'0101'0101'0101;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 0b1010'1010'1010'1010);
}

TEST_F(InstructionsTests, bitwise_and)
{
    instructions::bitwise_and instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0100'0000; // dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 164;
    registers[1] = 21;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 4);
}

TEST_F(InstructionsTests, bitwise_or)
{
    instructions::bitwise_or instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0100'0000; // dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 164;
    registers[1] = 21;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 181);
}

TEST_F(InstructionsTests, bitwise_xor)
{
    instructions::bitwise_xor instruction(registers.get(), memory.get(), properties);
    cpu_register_t currentInstruction = 0b0000'0000'0100'0000; // dst register 0, src register 1
    instruction.setCurrentInstruction(currentInstruction);
    registers[0] = 164;
    registers[1] = 21;
    DECODE_AND_EXECUTE(st);

    EXPECT_EQ(st, status::STATUS_OK);
    EXPECT_EQ(registers[0], 177);
}

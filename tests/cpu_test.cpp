#include "../src/cpu.h"
#include "../src/memory.h"
#include <gtest/gtest.h>

class CPUTest : public ::testing::Test
{
protected:
    Memory memory;
    CPU cpu;

    CPUTest() : cpu(&memory) {}

    void SetUp()
    {
        cpu.reset();
        memory.reset();
    }

    void TearDown()
    {
    }
};

//* LDA TESTS *//

TEST_F(CPUTest, LDAImmediate)
{
    memory.write(0x0200, 0xA9); // load LDA opcode with immediate addressing
    memory.write(0x0201, 0x42); // load literal we want to load into A
    memory.write(0x0202, 0x00); // Break CPU execution

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9); // 2 for LDA + 7 for BRK
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDAZeroPage)
{
    memory.write(0x0200, 0xA5);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    memory.write(0x0080, 0x34);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x34);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDAZeroPageX)
{
    memory.write(0x0200, 0xB5);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setX(0x02);
    memory.write(0x0082, 0x64);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x64);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDAAbsolute)
{
    memory.write(0x0200, 0xAD);
    memory.write(0x0201, 0x10);
    memory.write(0x0202, 0x30);
    memory.write(0x0203, 0x00);

    memory.write(0x3010, 0x34);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x34);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, LDAAbsoluteX)
{
    memory.write(0x0200, 0xBD);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    memory.write(0x3132, 0x78);

    cpu.setX(0x12);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x78);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, LDAAbsoluteY)
{
    memory.write(0x0200, 0xB9);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    memory.write(0x3132, 0x78);

    cpu.setY(0x12);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x78);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, LDAIndirectX)
{
    memory.write(0x0200, 0xA1);
    memory.write(0x0201, 0x70);
    memory.write(0x0202, 0x00);

    cpu.setX(0x05);

    memory.write(0x0075, 0x23);
    memory.write(0x0076, 0x30);

    memory.write(0x3023, 0xA5);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0xA5);
    ASSERT_TRUE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 13);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDAIndirectY)
{
    memory.write(0x0200, 0xB1);
    memory.write(0x0201, 0x70);
    memory.write(0x0202, 0x00);

    memory.write(0x0070, 0x43);
    memory.write(0x0071, 0x35);

    cpu.setY(0x10);

    memory.write(0x3553, 0x23);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x23);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 12);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

//* LDX TESTS *//

TEST_F(CPUTest, LDXImmediate)
{
    memory.write(0x0200, 0xA2); // load LDX opcode with immediate addressing
    memory.write(0x0201, 0x42); // load literal we want to load into A
    memory.write(0x0202, 0x00); // Break CPU execution

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9); // 2 for LDX + 7 for BRK
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDXZeroPage)
{
    memory.write(0x0200, 0xA6);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    memory.write(0x0080, 0x34);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x34);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDXZeroPageY)
{
    memory.write(0x0200, 0xB6);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setY(0x02);
    memory.write(0x0082, 0x64);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x64);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDXAbsolute)
{
    memory.write(0x0200, 0xAE);
    memory.write(0x0201, 0x10);
    memory.write(0x0202, 0x30);
    memory.write(0x0203, 0x00);

    memory.write(0x3010, 0x34);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x34);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, LDXAbsoluteY)
{
    memory.write(0x0200, 0xBE);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    memory.write(0x3132, 0x78);

    cpu.setY(0x12);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x78);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

//* LDY TESTS *//

TEST_F(CPUTest, LDYImmediate)
{
    memory.write(0x0200, 0xA0); // load LDX opcode with immediate addressing
    memory.write(0x0201, 0x42); // load literal we want to load into A
    memory.write(0x0202, 0x00); // Break CPU execution

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9); // 2 for LDX + 7 for BRK
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDYZeroPage)
{
    memory.write(0x0200, 0xA4);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    memory.write(0x0080, 0x34);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x34);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDYZeroPageX)
{
    memory.write(0x0200, 0xB4);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setX(0x02);
    memory.write(0x0082, 0x64);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x64);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, LDYAbsolute)
{
    memory.write(0x0200, 0xAC);
    memory.write(0x0201, 0x10);
    memory.write(0x0202, 0x30);
    memory.write(0x0203, 0x00);

    memory.write(0x3010, 0x34);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x34);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, LDYAbsoluteX)
{
    memory.write(0x0200, 0xBC);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    memory.write(0x3132, 0x78);

    cpu.setX(0x12);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x78);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

//* STA TESTS *//

TEST_F(CPUTest, STAZeroPage)
{
    memory.write(0x0200, 0x85);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setA(0x34);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0080), 0x34);
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STAZeroPageX)
{
    memory.write(0x0200, 0x95);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setX(0x02);
    cpu.setA(0x64);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0082), 0x64);
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STAAbsoluteX)
{
    memory.write(0x0200, 0x9D);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    cpu.setA(0x78);
    cpu.setX(0x12);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x3132), 0x78);
    EXPECT_EQ(cpu.getCycles(), 12);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, STAAbsoluteY)
{
    memory.write(0x0200, 0x99);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    cpu.setA(0x78);
    cpu.setY(0x12);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x3132), 0x78);
    EXPECT_EQ(cpu.getCycles(), 12);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, STAIndirectX)
{
    memory.write(0x0200, 0x81);
    memory.write(0x0201, 0x70);
    memory.write(0x0202, 0x00);

    cpu.setX(0x05);

    memory.write(0x0075, 0x23);
    memory.write(0x0076, 0x30);

    cpu.setA(0xA5);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x3023), 0xA5);
    EXPECT_EQ(cpu.getCycles(), 13);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STAIndirectY)
{
    memory.write(0x0200, 0x91);
    memory.write(0x0201, 0x70);
    memory.write(0x0202, 0x00);

    memory.write(0x0070, 0x43);
    memory.write(0x0071, 0x35);

    cpu.setY(0x10);

    cpu.setA(0x23);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x3553), 0x23);
    EXPECT_EQ(cpu.getCycles(), 13);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STXZeroPage)
{
    memory.write(0x0200, 0x86);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setX(0x34);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0080), 0x34);
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STXZeroPageY)
{
    memory.write(0x0200, 0x96);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setY(0x02);
    cpu.setX(0x64);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0082), 0x64);
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STXAbsolute)
{
    memory.write(0x0200, 0x8E);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    cpu.setX(0x78);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x3120), 0x78);
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, STYZeroPage)
{
    memory.write(0x0200, 0x84);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setY(0x34);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0080), 0x34);
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STYZeroPageX)
{
    memory.write(0x0200, 0x94);
    memory.write(0x0201, 0x80);
    memory.write(0x0202, 0x00);

    cpu.setX(0x02);
    cpu.setY(0x64);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0082), 0x64);
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, STYAbsolute)
{
    memory.write(0x0200, 0x8C);
    memory.write(0x0201, 0x20);
    memory.write(0x0202, 0x31);
    memory.write(0x0203, 0x00);

    cpu.setY(0x78);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x3120), 0x78);
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, TAX) // implied addressing
{
    memory.write(0x0200, 0xAA);
    memory.write(0x0201, 0x00);

    cpu.setA(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, TAY) // implied addressing
{
    memory.write(0x0200, 0xA8);
    memory.write(0x0201, 0x00);

    cpu.setA(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, TSX) // implied addressing
{
    memory.write(0x0200, 0xBA);
    memory.write(0x0201, 0x00);

    cpu.setSP(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, TXA) // implied addressing
{
    memory.write(0x0200, 0x8A);
    memory.write(0x0201, 0x00);

    cpu.setX(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, TXS) // implied addressing
{
    memory.write(0x0200, 0x9A);
    memory.write(0x0201, 0x00);

    cpu.setX(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getSP(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, TYA) // implied addressing
{
    memory.write(0x0200, 0x98);
    memory.write(0x0201, 0x00);

    cpu.setY(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0x42);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, PHA)
{
    memory.write(0x0200, 0x48);
    memory.write(0x0201, 0x00);

    EXPECT_EQ(cpu.getSP(), 0xFF);

    cpu.setA(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getSP(), 0xFF - 1);
    EXPECT_EQ(memory.read(0x01FF), 0x42);
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, PHP)
{
    memory.write(0x0200, 0x08);
    memory.write(0x0201, 0x00);

    cpu.setSR(0x42);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getSP(), 0xFF - 1);
    EXPECT_EQ(memory.read(0x01FF), 0x42);
    ASSERT_TRUE(cpu.flag_is_set(CPU::BREAK) && cpu.flag_is_set(CPU::IGNORED));
    EXPECT_EQ(cpu.getCycles(), 10);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, PLA)
{
    memory.write(0x0200, 0x68);
    memory.write(0x0201, 0x00);

    cpu.setSP(0xFF - 1);
    memory.write(0x01FF - 1, 0xA2);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getA(), 0xA2);
    EXPECT_EQ(cpu.getSP(), 0xFF);
    ASSERT_TRUE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, PLP)
{
    memory.write(0x0200, 0x28);
    memory.write(0x0201, 0x00);

    cpu.setSP(0xFF - 1);
    memory.write(0x01FF - 1, 0xA2);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getSR(), 0xA2);
    EXPECT_EQ(cpu.getSP(), 0xFF);
    EXPECT_EQ(cpu.getCycles(), 11);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, DECzeropage)
{
    memory.write(0x0200, 0xC6);
    memory.write(0x0201, 0x42);
    memory.write(0x0202, 0x00);

    memory.write(0x0042, 0x09);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0042), 0x08);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 12);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, DECzeropageX)
{
    memory.write(0x0200, 0xD6);
    memory.write(0x0201, 0x40);
    memory.write(0x0202, 0x00);

    cpu.setX(0x02);

    memory.write(0x0042, 0x09);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0042), 0x08);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 13);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, DECabsolute)
{
    //? tests segfault when opcode is D6, should investigate
    memory.write(0x0200, 0xCE);
    memory.write(0x0201, 0x40);
    memory.write(0x0202, 0x40);
    memory.write(0x0203, 0x00);

    memory.write(0x4040, 0x09);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x4040), 0x08);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 13);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, DECabsoluteX)
{
    memory.write(0x0200, 0xDE);
    memory.write(0x0201, 0x40);
    memory.write(0x0202, 0x40);
    memory.write(0x0203, 0x00);

    cpu.setX(0x02);

    memory.write(0x4042, 0x09);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x4042), 0x08);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 14);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, DEX)
{
    memory.write(0x0200, 0xCA);
    memory.write(0x0201, 0x00);

    cpu.setX(0x09);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x08);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, DEY)
{
    memory.write(0x0200, 0x88);
    memory.write(0x0201, 0x00);

    cpu.setY(0x09);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x08);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, INCzeropage)
{
    memory.write(0x0200, 0xE6);
    memory.write(0x0201, 0x42);
    memory.write(0x0202, 0x00);

    memory.write(0x0042, 0x08);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0042), 0x09);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 12);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, INCzeropageX)
{
    memory.write(0x0200, 0xF6);
    memory.write(0x0201, 0x40);
    memory.write(0x0202, 0x00);

    cpu.setX(0x02);

    memory.write(0x0042, 0x08);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x0042), 0x09);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 13);
    EXPECT_EQ(cpu.getPC(), 0x0203);
}

TEST_F(CPUTest, INCabsolute)
{
    memory.write(0x0200, 0xEE);
    memory.write(0x0201, 0x40);
    memory.write(0x0202, 0x40);
    memory.write(0x0203, 0x00);

    memory.write(0x4040, 0x09);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x4040), 0x0A);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 13);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, INCabsoluteX)
{
    memory.write(0x0200, 0xFE);
    memory.write(0x0201, 0x40);
    memory.write(0x0202, 0x40);
    memory.write(0x0203, 0x00);

    cpu.setX(0x02);

    memory.write(0x4042, 0x09);

    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(memory.read(0x4042), 0x0A);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 14);
    EXPECT_EQ(cpu.getPC(), 0x0204);
}

TEST_F(CPUTest, INX)
{
    memory.write(0x0200, 0xE8);
    memory.write(0x0201, 0x00);

    cpu.setX(0x09);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getX(), 0x0A);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, INY)
{
    memory.write(0x0200, 0xC8);
    memory.write(0x0201, 0x00);

    cpu.setY(0x09);
    cpu.setPC(0x0200);
    cpu.run();

    EXPECT_EQ(cpu.getY(), 0x0A);
    ASSERT_FALSE(cpu.flag_is_set(CPU::NEGATIVE));
    ASSERT_FALSE(cpu.flag_is_set(CPU::ZERO));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

TEST_F(CPUTest, CLC)
{
    memory.write(0x0200, 0x18);
    memory.write(0x0201, 0x00);

    cpu.set(CPU::CARRY);
    ASSERT_TRUE(cpu.flag_is_set(CPU::CARRY));

    cpu.setPC(0x0200);
    cpu.run();

    ASSERT_FALSE(cpu.flag_is_set(CPU::CARRY));
    EXPECT_EQ(cpu.getCycles(), 9);
    EXPECT_EQ(cpu.getPC(), 0x0202);
}

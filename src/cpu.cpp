#include "cpu.h"

//* Get functions *//

Byte CPU::getA() const { return A; }
Byte CPU::getX() const { return X; }
Byte CPU::getY() const { return Y; }
Byte CPU::getSP() const { return SP; }
Byte CPU::getSR() const { return SR; }
Word CPU::getPC() const { return PC; }
Word CPU::getCycles() const { return clock_cycles; }

//* Set functions *//

void CPU::setA(Byte b) { A = b; }
void CPU::setX(Byte b) { X = b; }
void CPU::setY(Byte b) { Y = b; }
void CPU::setSP(Byte b) { SP = b; }
void CPU::setSR(Byte b) { SR = b; }
void CPU::setPC(Word address) { PC = address; }

CPU::CPU(Memory *memory)
{
    this->memory = memory;
    A = X = Y = 0x00;
    SP = 0xFF;
    SR = 0x00;
    PC = 0x0000;

    clock_cycles = 0;
    interrupt = false;
    effective_address = 0x0000;
}

void CPU::reset()
{
    memory->reset();
    A = X = Y = 0x00;
    SP = 0xFF;
    SR = 0x00;
    PC = 0x0000;

    clock_cycles = 0;
    interrupt = false;
    effective_address = 0x0000;
}

void CPU::run()
{
    while (!interrupt)
    {
        opcode = memory->read(PC);
        PC++;

        auto it = lookup.find(opcode);
        // lookup[opcode] has undesired behavior if opcode does not exist in the map
        Instruction ins = it->second;

        // how to keep track of cycles
        (this->*ins.addressing)();

        // what is this syntax?
        (this->*ins.execute)();
        clock_cycles += ins.cycles;
    }
}

void CPU::set(flags f)
{
    SR |= f;
}

void CPU::clear(flags f)
{
    SR &= ~f;
}

bool CPU::flag_is_set(flags f)
{
    return (SR & f) != 0;
}

//** Address Modes **//

Byte CPU::accumulator()
{
    // value at PC will just be A, indicating
    // to perform a bit shift on the accumulator.
    // so we just increment the PC.
    PC++;
    return 0;
}

Byte CPU::immediate()
{
    effective_address = PC;
    PC++;
    return 0;
}

Byte CPU::implied()
{
    return 0;
}

Byte CPU::relative()
{
    return 0;
}

Byte CPU::zeropage()
{
    Byte low_byte = memory->read(PC);
    PC++;
    effective_address = 0x0000 | low_byte;
    return 0;
}

Byte CPU::zeropageX()
{
    Byte low_byte = memory->read(PC) + X;
    PC++;
    effective_address = 0x0000 | low_byte;
    return 0;
}

Byte CPU::zeropageY()
{
    Byte low_byte = memory->read(PC) + Y;
    PC++;
    effective_address = 0x0000 | low_byte;
    return 0;
}

Byte CPU::absolute()
{
    Word low_byte = memory->read(PC);
    PC++;

    Word high_byte = memory->read(PC);
    PC++;

    effective_address = (high_byte << 8) | low_byte;
    return 0;
}

Byte CPU::absoluteX()
{
    Word low_byte = memory->read(PC);
    PC++;

    Word high_byte = memory->read(PC);
    PC++;

    if ((low_byte + (Word)X) > 0xFF)
    {
        // overflow between the low bytes indicated page boundary is crossed
        clock_cycles++;
    }

    effective_address = ((high_byte << 8) | low_byte) + X;
    return 0;
}

Byte CPU::absoluteY()
{
    Word low_byte = memory->read(PC);
    PC++;

    Word high_byte = memory->read(PC);
    PC++;

    if ((low_byte + (Word)Y) > 0xFF)
    {
        // overflow between the low bytes indicated page boundary is crossed
        clock_cycles++;
    }

    effective_address = ((high_byte << 8) | low_byte) + Y;
    return 0;
}

Byte CPU::indirectX()
{
    Byte low_byte = memory->read(PC) + X;
    PC++;

    Word lookup_address = 0x0000 | low_byte;
    low_byte = memory->read(lookup_address);
    Word high_byte = memory->read(lookup_address + 1);

    effective_address = (high_byte << 8) | low_byte;
    return 0;
}

Byte CPU::indirectY()
{
    Word low_byte = memory->read(PC);
    PC++;

    Word lookup_address = 0x0000 | low_byte;

    low_byte = memory->read(lookup_address);
    Word high_byte = memory->read(lookup_address + 1);

    if ((low_byte + (Word)Y) > 0xFF)
    {
        // overflow between the low bytes indicates page boundary is crossed
        clock_cycles++;
    }

    effective_address = ((high_byte << 8) | low_byte) + Y;
    return 0;
}

//** Instruction Handlers **//

void CPU::BRK()
{
    interrupt = true;
}

void CPU::modify_negative_flag(Byte data)
{
    if (data & 0x80)
    {
        set(NEGATIVE);
    }
    else
    {
        clear(NEGATIVE);
    }
}

void CPU::modify_zero_flag(Byte data)
{
    if (data == 0x00)
    {
        set(ZERO);
    }
    else
    {
        clear(ZERO);
    }
}

void CPU::LDA()
{
    Byte data = memory->read(effective_address);
    modify_negative_flag(data);
    modify_zero_flag(data);
    A = data;
}

void CPU::LDX()
{
    Byte data = memory->read(effective_address);
    modify_negative_flag(data);
    modify_zero_flag(data);
    X = data;
}

void CPU::LDY()
{
    Byte data = memory->read(effective_address);
    modify_negative_flag(data);
    modify_zero_flag(data);
    Y = data;
}

void CPU::STA()
{
    memory->write(effective_address, A);
}

void CPU::STX()
{
    memory->write(effective_address, X);
}

void CPU::STY()
{
    memory->write(effective_address, Y);
}

void CPU::TAX()
{
    modify_negative_flag(A);
    modify_zero_flag(A);
    X = A;
}

void CPU::TAY()
{
    modify_negative_flag(A);
    modify_zero_flag(A);
    Y = A;
}

void CPU::TSX()
{
    modify_negative_flag(SP);
    modify_zero_flag(SP);
    X = SP;
}

void CPU::TXA()
{
    modify_negative_flag(X);
    modify_zero_flag(X);
    A = X;
}

void CPU::TXS()
{
    SP = X;
}

void CPU::TYA()
{
    modify_negative_flag(Y);
    modify_zero_flag(Y);
    A = Y;
}

//* Stack Opcodes *//

void CPU::PHA()
{
    Word stack_address = 0x0100 | SP;
    memory->write(stack_address, A);
    SP--;
}

void CPU::PHP()
{
    Word stack_address = 0x0100 | SP;
    memory->write(stack_address, SR);
    SP--;
    set(BREAK);
    set(IGNORED);
}

void CPU::PLA()
{
    Word stack_address = 0x0100 | SP;
    Byte data = memory->read(stack_address);
    modify_negative_flag(data);
    modify_zero_flag(data);
    A = data;
    SP++;
}

void CPU::PLP()
{
    Word stack_address = 0x0100 | SP;
    Byte data = memory->read(stack_address);
    SR = data;
    SP++;
}

//* Decrements & Increments *//

void CPU::DEC()
{
    Byte data = (memory->read(effective_address));
    data--;
    memory->write(effective_address, data);
    modify_negative_flag(data);
    modify_zero_flag(data);
}

void CPU::DEX()
{
    X--;
    modify_negative_flag(X);
    modify_zero_flag(X);
}

void CPU::DEY()
{
    Y--;
    modify_negative_flag(Y);
    modify_zero_flag(Y);
}

void CPU::INC()
{
    Byte data = (memory->read(effective_address));
    data++;
    memory->write(effective_address, data);
    modify_negative_flag(data);
    modify_zero_flag(data);
}

void CPU::INX()
{
    X++;
    modify_negative_flag(X);
    modify_zero_flag(X);
}

void CPU::INY()
{
    Y++;
    modify_negative_flag(Y);
    modify_zero_flag(Y);
}

//* Arithmetic Operations *//

// needs tests
void CPU::ADC()
{
    Word data = (memory->read(effective_address));

    Word value = (Word)A + data + (Word)flag_is_set(CARRY);
    if (value > 0xFF)
    {
        set(CARRY);
    }
    if (~(data ^ (Word)A) & ((Word)A ^ value) & 0x0080)
    {
        set(OVERFLOW);
    }
    modify_negative_flag(value);
    modify_zero_flag(value);

    A = value & 0x00FF;
}

// needs tests
void CPU::SBC()
{
    Word data = (memory->read(effective_address)) ^ 0x00FF;

    Word value = (Word)A + data + (Word)flag_is_set(CARRY);
    if (value & 0xFF00)
    {
        set(CARRY);
    }
    if ((value ^ (Word)A) & (value & data) & 0x0080)
    {
        set(OVERFLOW);
    }
    modify_negative_flag(value);
    modify_zero_flag(value);

    A = value & 0x00FF;
}

//* Logical Operations *//

// needs tests
void CPU::AND()
{
    Byte data = memory->read(effective_address);
    A = A & data;
    modify_negative_flag(A);
    modify_zero_flag(A);
}

void CPU::EOR()
{
    Byte data = memory->read(effective_address);
    A = A ^ data;
    modify_negative_flag(A);
    modify_zero_flag(A);
}

void CPU::ORA()
{
    Byte data = memory->read(effective_address);
    A = A | data;
    modify_negative_flag(A);
    modify_zero_flag(A);
}

//* Shift & Rotate Instructions *//

void CPU::ASL()
{
    // addressing mode is accumulator
    if (opcode == 0x0A)
    {
        if ((A & (1 << 7)) != 0)
        {
            set(CARRY);
        }
        else
        {
            clear(CARRY);
        }

        A << 1;
        modify_negative_flag(A);
        modify_zero_flag(A);
        return;
    }
    Byte data = memory->read(effective_address);
    if ((data & (1 << 7)) != 0)
    {
        set(CARRY);
    }
    else
    {
        clear(CARRY);
    }

    data << 1;
    modify_negative_flag(data);
    modify_zero_flag(data);
    memory->write(effective_address, data);
}

void CPU::LSR()
{
    // addressing mode is accumulator
    if (opcode == 0x4A)
    {
        if ((A & 0x1) != 0)
        {
            set(CARRY);
        }
        else
        {
            clear(CARRY);
        }

        A >> 1;
        modify_zero_flag(A);
        return;
    }

    Byte data = memory->read(effective_address);
    if ((data & 0x1) != 0)
    {
        set(CARRY);
    }
    else
    {
        clear(CARRY);
    }

    data >> 1;
    modify_zero_flag(data);
    memory->write(effective_address, data);
}

void CPU::CLC()
{
    clear(CARRY);
}

void CPU::CLD()
{
    clear(DECIMAL);
}

void CPU::CLI()
{
    clear(INTERRUPT);
}

void CPU::CLV()
{
    clear(OVERFLOW);
}

void CPU::SEC()
{
    set(CARRY);
}

void CPU::SED()
{
    set(DECIMAL);
}

void CPU::SEI()
{
    set(INTERRUPT);
}

void CPU::CMP()
{
    Byte data = memory->read(effective_address);

    if (A == data)
    {
        set(ZERO);
        set(CARRY);
        clear(NEGATIVE);
    }
    else if (A < data)
    {
        clear(ZERO);
        clear(CARRY);
        modify_negative_flag(A - data);
    }
    else
    {
        clear(ZERO);
        set(CARRY);
        modify_negative_flag(A - data);
    }
}

void CPU::CPX()
{
    Byte data = memory->read(effective_address);

    if (X == data)
    {
        set(ZERO);
        set(CARRY);
        clear(NEGATIVE);
    }
    else if (X < data)
    {
        clear(ZERO);
        clear(CARRY);
        modify_negative_flag(X - data);
    }
    else
    {
        clear(ZERO);
        set(CARRY);
        modify_negative_flag(X - data);
    }
}

void CPU::CPY()
{
    Byte data = memory->read(effective_address);

    if (Y == data)
    {
        set(ZERO);
        set(CARRY);
        clear(NEGATIVE);
    }
    else if (Y < data)
    {
        clear(ZERO);
        clear(CARRY);
        modify_negative_flag(Y - data);
    }
    else
    {
        clear(ZERO);
        set(CARRY);
        modify_negative_flag(Y - data);
    }
}

void CPU::BCC()
{
    if (!flag_is_set(CARRY))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

void CPU::BCS()
{
    if (flag_is_set(CARRY))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

void CPU::BEQ()
{
    if (flag_is_set(ZERO))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

void CPU::BMI()
{
    if (flag_is_set(NEGATIVE))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

void CPU::BNE()
{
    if (!flag_is_set(ZERO))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

void CPU::BPL()
{
    if (!flag_is_set(NEGATIVE))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

void CPU::BVC()
{
    if (!flag_is_set(OVERFLOW))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

void CPU::BVS()
{
    if (flag_is_set(OVERFLOW))
    {
        // jump
        Byte offset = memory->read(PC);

        // Check if crossing a page boundary
        if ((PC & 0xFF00) != ((PC + offset) & 0xFF00))
        {
            clock_cycles++;
        }

        // Update PC with the new value
        PC += offset;
        clock_cycles++;
    }
}

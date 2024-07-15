#ifndef CPU_H
#define CPU_H

#include <string>
#include <map>

#include "memory.h"
#include "types.h"

// Instruction Set: https://www.masswerk.at/6502/6502_instruction_set.html

class CPU
{
private:
    Memory *memory;
    Byte A, X, Y; // Accumulator, X, Y registers
    Byte SP;      // stack pointer
    Byte SR;      // status register
    Word PC;      // program counter

    Word clock_cycles;

    struct Instruction
    {
        void (CPU::*execute)(void);
        Byte (CPU::*addressing)(void);
        Byte cycles;
    };

    // is it okay to define this here?
    // Instruction opcode maps to {Mneomnic, Addressing Mode, Num. of clock cycles}
    std::map<Byte, Instruction> lookup = {
        {0x00, {&CPU::BRK, &CPU::implied, 7}},
        //* LDA OPCODES *//
        {0xA9, {&CPU::LDA, &CPU::immediate, 2}},
        {0xA5, {&CPU::LDA, &CPU::zeropage, 3}},
        {0xB5, {&CPU::LDA, &CPU::zeropageX, 4}},
        {0xAD, {&CPU::LDA, &CPU::absolute, 4}},
        {0xBD, {&CPU::LDA, &CPU::absoluteX, 4}},
        {0xB9, {&CPU::LDA, &CPU::absoluteY, 4}},
        {0xA1, {&CPU::LDA, &CPU::indirectX, 6}},
        {0xB1, {&CPU::LDA, &CPU::indirectY, 5}},
        //* LDX OPCODES *//
        {0xA2, {&CPU::LDX, &CPU::immediate, 2}},
        {0xA6, {&CPU::LDX, &CPU::zeropage, 3}},
        {0xB6, {&CPU::LDX, &CPU::zeropageY, 4}},
        {0xAE, {&CPU::LDX, &CPU::absolute, 4}},
        {0xBE, {&CPU::LDX, &CPU::absoluteY, 4}},
        //* LDY OPCODES *//
        {0xA0, {&CPU::LDY, &CPU::immediate, 2}},
        {0xA4, {&CPU::LDY, &CPU::zeropage, 3}},
        {0xB4, {&CPU::LDY, &CPU::zeropageX, 4}},
        {0xAC, {&CPU::LDY, &CPU::absolute, 4}},
        {0xBC, {&CPU::LDY, &CPU::absoluteX, 4}},
        //* STA OPCODES *//
        {0x85, {&CPU::STA, &CPU::zeropage, 3}},
        {0x95, {&CPU::STA, &CPU::zeropageX, 4}},
        {0x8D, {&CPU::STA, &CPU::absolute, 4}},
        {0x9D, {&CPU::STA, &CPU::absoluteX, 5}},
        {0x99, {&CPU::STA, &CPU::absoluteY, 5}},
        {0x81, {&CPU::STA, &CPU::indirectX, 6}},
        {0x91, {&CPU::STA, &CPU::indirectY, 6}},
        //* STX OPCODES *//
        {0x86, {&CPU::STX, &CPU::zeropage, 3}},
        {0x96, {&CPU::STX, &CPU::zeropageY, 4}},
        {0x8E, {&CPU::STX, &CPU::absolute, 4}},
        //* STY OPCODES *//
        {0x84, {&CPU::STY, &CPU::zeropage, 3}},
        {0x94, {&CPU::STY, &CPU::zeropageX, 4}},
        {0x8C, {&CPU::STY, &CPU::absolute, 4}},
        //* T__ OPCODES *//
        {0xAA, {&CPU::TAX, &CPU::implied, 2}},
        {0xA8, {&CPU::TAY, &CPU::implied, 2}},
        {0xBA, {&CPU::TSX, &CPU::implied, 2}},
        {0x8A, {&CPU::TXA, &CPU::implied, 2}},
        {0x9A, {&CPU::TXS, &CPU::implied, 2}},
        {0x98, {&CPU::TYA, &CPU::implied, 2}},
        //* Stack OPCODES *//
        {0x48, {&CPU::PHA, &CPU::implied, 3}},
        {0x08, {&CPU::PHP, &CPU::implied, 3}},
        {0x68, {&CPU::PLA, &CPU::implied, 4}},
        {0x28, {&CPU::PLP, &CPU::implied, 4}},
        //* Decrements & Increments *//
        {0xC6, {&CPU::DEC, &CPU::zeropage, 5}},
        {0xD6, {&CPU::DEC, &CPU::zeropageX, 6}},
        {0xCE, {&CPU::DEC, &CPU::absolute, 6}},
        {0xDE, {&CPU::DEC, &CPU::absoluteX, 7}},
        {0xCA, {&CPU::DEX, &CPU::implied, 2}},
        {0x88, {&CPU::DEY, &CPU::implied, 2}},
        {0xE6, {&CPU::INC, &CPU::zeropage, 5}},
        {0xF6, {&CPU::INC, &CPU::zeropageX, 6}},
        {0xEE, {&CPU::INC, &CPU::absolute, 6}},
        {0xFE, {&CPU::INC, &CPU::absoluteX, 7}},
        {0xE8, {&CPU::INX, &CPU::implied, 2}},
        {0xC8, {&CPU::INY, &CPU::implied, 2}},
        //* Arithmetic Operations *//
        {0x69, {&CPU::ADC, &CPU::implied, 2}},
        {0x65, {&CPU::ADC, &CPU::zeropage, 3}},
        {0x75, {&CPU::ADC, &CPU::zeropageX, 4}},
        {0x6D, {&CPU::ADC, &CPU::absolute, 4}},
        {0x7D, {&CPU::ADC, &CPU::absoluteX, 4}},
        {0x79, {&CPU::ADC, &CPU::absoluteY, 4}},
        {0x61, {&CPU::ADC, &CPU::indirectX, 6}},
        {0x71, {&CPU::ADC, &CPU::indirectY, 5}},
        {0xE9, {&CPU::SBC, &CPU::implied, 2}},
        {0xE5, {&CPU::SBC, &CPU::zeropage, 3}},
        {0xF5, {&CPU::SBC, &CPU::zeropageX, 4}},
        {0xED, {&CPU::SBC, &CPU::absolute, 4}},
        {0xFD, {&CPU::SBC, &CPU::absoluteX, 4}},
        {0xF9, {&CPU::SBC, &CPU::absoluteY, 4}},
        {0xE1, {&CPU::SBC, &CPU::indirectX, 6}},
        {0xF1, {&CPU::SBC, &CPU::indirectY, 5}},
        //* Logical Operations *//
        {0x29, {&CPU::AND, &CPU::immediate, 2}},
        {0x25, {&CPU::AND, &CPU::zeropage, 3}},
        {0x35, {&CPU::AND, &CPU::zeropageX, 4}},
        {0x2D, {&CPU::AND, &CPU::absolute, 4}},
        {0x3D, {&CPU::AND, &CPU::absoluteX, 4}},
        {0x39, {&CPU::AND, &CPU::absoluteY, 4}},
        {0x21, {&CPU::AND, &CPU::indirectX, 6}},
        {0x31, {&CPU::AND, &CPU::indirectY, 5}},
        {0x49, {&CPU::EOR, &CPU::immediate, 2}},
        {0x45, {&CPU::EOR, &CPU::zeropage, 3}},
        {0x55, {&CPU::EOR, &CPU::zeropageX, 4}},
        {0x4D, {&CPU::EOR, &CPU::absolute, 4}},
        {0x5D, {&CPU::EOR, &CPU::absoluteX, 4}},
        {0x59, {&CPU::EOR, &CPU::absoluteY, 4}},
        {0x41, {&CPU::EOR, &CPU::indirectX, 6}},
        {0x51, {&CPU::EOR, &CPU::indirectY, 5}},
        {0x09, {&CPU::ORA, &CPU::immediate, 2}},
        {0x05, {&CPU::ORA, &CPU::zeropage, 3}},
        {0x15, {&CPU::ORA, &CPU::zeropageX, 4}},
        {0x0D, {&CPU::ORA, &CPU::absolute, 4}},
        {0x1D, {&CPU::ORA, &CPU::absoluteX, 4}},
        {0x19, {&CPU::ORA, &CPU::absoluteY, 4}},
        {0x01, {&CPU::ORA, &CPU::indirectX, 6}},
        {0x11, {&CPU::ORA, &CPU::indirectY, 5}},
        //* Shift & Rotate *//
        {0x0A, {&CPU::ASL, &CPU::accumulator, 2}},
        {0x06, {&CPU::ASL, &CPU::zeropage, 5}},
        {0x16, {&CPU::ASL, &CPU::zeropageX, 6}},
        {0x0E, {&CPU::ASL, &CPU::absolute, 6}},
        {0x1E, {&CPU::ASL, &CPU::absoluteX, 7}},
        {0x4A, {&CPU::LSR, &CPU::accumulator, 2}},
        {0x46, {&CPU::LSR, &CPU::zeropage, 5}},
        {0x56, {&CPU::LSR, &CPU::zeropageX, 6}},
        {0x4E, {&CPU::LSR, &CPU::absolute, 6}},
        {0x5E, {&CPU::LSR, &CPU::absoluteX, 7}},
        // {0x2A, {&CPU::ROL, &CPU::accumulator, 2}},
        // {0x26, {&CPU::ROL, &CPU::zeropage, 5}},
        // {0x36, {&CPU::ROL, &CPU::zeropageX, 6}},
        // {0x2E, {&CPU::ROL, &CPU::absolute, 6}},
        // {0x3E, {&CPU::ROL, &CPU::absoluteX, 7}},
        //* Flag Instructions *//
        {0x18, {&CPU::CLC, &CPU::implied, 2}},
        {0xD8, {&CPU::CLD, &CPU::implied, 2}},
        {0x58, {&CPU::CLI, &CPU::implied, 2}},
        {0xB8, {&CPU::CLV, &CPU::implied, 2}},
        {0x38, {&CPU::SEC, &CPU::implied, 2}},
        {0xF8, {&CPU::SED, &CPU::implied, 2}},
        {0x78, {&CPU::SEI, &CPU::implied, 2}},
        //* Comparisons *//
        {0xC9, {&CPU::CMP, &CPU::immediate, 2}},
        {0xC5, {&CPU::CMP, &CPU::zeropage, 3}},
        {0xD5, {&CPU::CMP, &CPU::zeropageX, 4}},
        {0xCD, {&CPU::CMP, &CPU::absolute, 4}},
        {0xDD, {&CPU::CMP, &CPU::absoluteX, 4}},
        {0xD9, {&CPU::CMP, &CPU::absoluteY, 4}},
        {0xC1, {&CPU::CMP, &CPU::indirectX, 6}},
        {0xD1, {&CPU::CMP, &CPU::indirectY, 5}},
        {0xE0, {&CPU::CPX, &CPU::immediate, 2}},
        {0xE4, {&CPU::CPX, &CPU::zeropage, 3}},
        {0xEC, {&CPU::CPX, &CPU::absolute, 4}},
        {0xC0, {&CPU::CPY, &CPU::immediate, 2}},
        {0xC4, {&CPU::CPY, &CPU::zeropage, 3}},
        {0xCC, {&CPU::CPY, &CPU::absolute, 4}},
        //* Conditional Branching *//
        {0x90, {&CPU::BCC, &CPU::relative, 2}},
        {0xB0, {&CPU::BCS, &CPU::relative, 2}},
        {0xF0, {&CPU::BEQ, &CPU::relative, 2}},
        {0x30, {&CPU::BMI, &CPU::relative, 2}},
        {0xD0, {&CPU::BNE, &CPU::relative, 2}},
        {0x10, {&CPU::BPL, &CPU::relative, 2}},
        {0x50, {&CPU::BVC, &CPU::relative, 2}},
        {0x70, {&CPU::BVS, &CPU::relative, 2}},
    };

    bool interrupt;
    Byte opcode;
    Word effective_address;

public:
    CPU(Memory *memory);
    void reset();
    void run();

    enum flags : Byte
    {
        NEGATIVE = 1 << 7,  // sign bit is set
        OVERFLOW = 1 << 6,  // overflow is detected
        IGNORED = 1 << 5,   // unused flag
        BREAK = 1 << 4,     //
        DECIMAL = 1 << 3,   // sets ALU to decimal mode for add,sub
        INTERRUPT = 1 << 2, // blocks IRQ
        ZERO = 1 << 1,      // indicates value of all zero bits
        CARRY = 1 << 0      // used as buffer and borrow in arithmetic ops
    };

    void set(flags f);
    void clear(flags f);
    bool flag_is_set(flags f);
    void modify_negative_flag(Byte data);
    void modify_zero_flag(Byte data);

    //** Address Modes **//

    Byte accumulator(); // exclusive to bit shifting instructions
    Byte immediate();
    Byte implied();
    Byte relative();
    Byte zeropage();
    Byte zeropageX();
    Byte zeropageY();
    Byte absolute();
    Byte absoluteX();
    Byte absoluteY();
    Byte indirectX();
    Byte indirectY();

    //** Instruction Handlers **//

    void BRK(); // Stop cpu execution.
    void LDA(); // Load A with memory.
    void LDX(); // Load register X with memory.
    void LDY(); // Load register Y with memory.
    void STA(); // Store the contents of A into memory.
    void STX(); // Store the contents of X into memory.
    void STY(); // Store the contents of Y into memory.
    void TAX(); // Transfer contents of A into X.
    void TAY(); // Transfer contents of A into Y.
    void TSX(); // Transfer contents of stack pointer into X.
    void TXA(); // Transfer contents of X into A.
    void TXS(); // Transfer contents of X into stack pointer.
    void TYA(); // Transfer contents of Y into A.
    void PHA(); // Push contents of A onto the stack.
    void PHP(); // Push processor status onto the stack.
    void PLA(); // Pull value from top of stack and load into A.
    void PLP(); // Pull processor status register from stack and load into A.
    void DEC(); // Decrement value in memory.
    void DEX(); // Decrement value in X register.
    void DEY(); // Decrement value in Y register.
    void INC(); // Increment value in memory.
    void INX(); // Increment value in X register.
    void INY(); // Increment value in Y register.
    void ADC(); // Add memory to A with carry.
    void SBC(); // Subtract memory to A with borrow.
    void AND(); // Logical AND memory with A.
    void EOR(); // XOR memory with A.
    void ORA(); // OR memory with A.
    void ASL(); // Arithmetic shift left.
    void LSR(); // Logical shift right.
    // void ROL(); // Rotate left.

    void CLC(); // Clear carry flag.
    void CLD();
    void CLI();
    void CLV();
    void SEC();
    void SED();
    void SEI();
    void CMP();
    void CPX();
    void CPY();
    void BCC();
    void BCS();
    void BEQ();
    void BMI();
    void BNE();
    void BPL();
    void BVC();
    void BVS();

    //** Get functions **//

    Byte getA() const;  // get the value in the A register
    Byte getX() const;  // get the value in the X register
    Byte getY() const;  // get the value in the Y register
    Byte getSP() const; // get the value of the stack pointer
    Byte getSR() const; // get the value of the status register
    Word getPC() const; // get the value of the program counter
    Word getCycles() const;
    // const means it will not modify state of object

    //** Set functions **//

    void setA(Byte b);        // set the value of the A register
    void setX(Byte b);        // set the value of the X register
    void setY(Byte b);        // set the value of the Y register
    void setSP(Byte b);       // set the value fo the stack pointer
    void setSR(Byte b);       // set the value of the statuts register
    void setPC(Word address); // set the value of the program counter
};

#endif // CPU_H

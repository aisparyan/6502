#include "memory.h"

Memory::Memory()
{
    for (auto i = 0; i <= MAX_MEM; i++)
    {
        RAM[i] = 0x00;
    }
}

Memory::~Memory()
{
}

void Memory::reset()
{
    for (auto i = 0; i <= MAX_MEM; i++)
    {
        RAM[i] = 0x00;
    }
}

Byte Memory::read(Word address)
{
    return RAM[address];
}

void Memory::write(Word address, Byte data)
{
    RAM[address] = data;
}

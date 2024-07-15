#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include <cstdint>

class Memory
{
private:
    // 256 memory pages, each containing 256 bytes.
    static const std::uint16_t MAX_MEM = (256 * 256) - 1; // 64KB
    Byte RAM[MAX_MEM];

public:
    Memory();
    ~Memory();
    void reset();
    Byte read(Word address);
    void write(Word address, Byte data);
};

#endif // MEMORY_H

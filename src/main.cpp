#include "cpu.h"
#include "memory.h"

int main()
{
    Memory memory;
    CPU cpu(&memory);

    // inline a program //

    memory.write(0x0200, 0xA2); // load LDA opcode with immediate addressing
    memory.write(0x0201, 0x42); // load literal we want to load into A
    memory.write(0x0202, 0x00); // Break CPU execution

    cpu.setPC(0x0200);
    cpu.run();
    return 0;
}

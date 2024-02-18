#include <iostream>
#include "cpu.h"

int main(int argc, char** argv)
{
    cpu core;
    status st = status::STATUS_OK;
    while(1) {
        core.decodeInstruction();
        core.executeInstruction();
    }
    return 0;
}

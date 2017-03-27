#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "../global/defines.h"

const int CORESIZE = 8000;
const int INT_REGS_COUNT = 7;
const int FLOAT_REGS_COUNT = 6;

class VMashine
{
public:
    VMashine();

    bool load(const std::string &filename, int address);
    void execute();

private:
    void interrupt(short code);

private:
    unsigned char memory[CORESIZE];

    short regs[INT_REGS_COUNT];
    float regsFloat[FLOAT_REGS_COUNT];

    short &pc, &sp;
    float &esp;

    bool exit;
};
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

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

VMashine::VMashine()
: pc(regs[6]), sp(regs[5]), esp(regsFloat[5])
{
}

bool VMashine::load(const std::string &filename, int address)
{
    std::ifstream program(filename.c_str(), std::ios::binary);
    if(!program.is_open())
    {
        std::cerr << "Can not open file " << filename << std::endl;
        return false;
    }

    program.seekg(0,std::ios::end);
    size_t length = program.tellg();

    program.seekg(0,std::ios::beg);
    program.read((char*)(memory+address), length);
    program.close();

    pc = address;
    sp = length+1;

    return true;
}

void VMashine::execute()
{
    exit = false;
    while(!exit)
    {
        switch(memory[pc])
        {
        case 19:
            break;

        case 20:
        {
            short intCode = *((short*)(memory+pc+1));
            interrupt(intCode);
            pc+=2;
        } break;

        case 1:
        {
            short num = *((short*)(memory+pc+1));
            size_t reg = *(memory+pc+3);
            regs[reg] = num;
            pc+=3;
        } break;

        case 2:
        {
            short num = *((short*)(memory+pc+1));
            short addr = *((short*)(memory+pc+3));
            *((short*)(memory+addr)) = num;
            pc+=4;
        } break;

        case 3:
        {
            short num = *((short*)(memory+pc+1));
            size_t reg = *(memory+pc+3);
            short addr = regs[reg];
            *((short*)(memory+addr)) = num;
            pc+=5;
        } break;

        case 7:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            regs[reg2] = regs[reg1];
            pc+=2;
        } break;

        case 8:
        {
            size_t reg = *(memory+pc+1);
            short addr = *((short*)(memory+pc+2));
            *((short*)(memory+addr)) = regs[reg];
            pc+=3;
        } break;

        case 9:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            short addr = regs[reg2];
            *((short*)(memory+addr)) = regs[reg1];
            pc+=2;
        } break;

        case 23:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            regs[reg2] += regs[reg1];
            pc+=2;
        } break;

        default:
        {
            std::cerr << "\nUnknown command! " << (int)memory[pc] << "\n";
            return;
        }
        }
    pc++;
    }

}

void VMashine::interrupt(short code)
{
    switch(code)
    {
    case 10:
        exit = true;
        break;

    case 11:
        std::cout << regs[0] << std::endl;
        break;

    case 12:
        std::cout << regsFloat[0] << std::endl;
        break;

    case 13:
        std::cin >> regs[0];
        break;

    case 14:
        std::cin >> regsFloat[0];
        break;

    default:
        std::cerr << "\nUnknown interrupt!\n";
        return;
    }
}

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        std::cerr << "No program specified!\n";
        return 1;
    }

    VMashine vm;

    if(!vm.load(argv[1], 100))
        return 1;

    vm.execute();

    return 0;
}

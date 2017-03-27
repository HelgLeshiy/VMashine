#include "translator.h"

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        std::cerr << "No input file!\n";
        exit(1);
    }
    if(argc == 2)
    {
        std::cerr << "No output file!\n";
        exit(1);
    }

    Translator mAsm;
    mAsm.translate(argv[1], argv[2]);

    if(argc == 4)
    {
        if(strcmp(argv[3], "-p") == 0)
            mAsm.printBCode();
    }

    return 0;
}
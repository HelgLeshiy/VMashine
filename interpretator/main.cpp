#include "vmashine.h"

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
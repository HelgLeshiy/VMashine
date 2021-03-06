#include "translator.h"

Translator::Translator()
{
    initRegs();
    curline = 0;
    cur = 0;
}

void Translator::initRegs()
{
    regs["ah"] = 0;
    regs["bh"] = 1;
    regs["ch"] = 2;
    regs["dh"] = 3;
    regs["eh"] = 4;
    regs["sp"] = 5;
    regs["pc"] = 6;

    regsFloat["eax"] = 0;
    regsFloat["ebx"] = 1;
    regsFloat["ecx"] = 2;
    regsFloat["edx"] = 3;
    regsFloat["eex"] = 4;
    regsFloat["esp"] = 5;
}


void Translator::checkArgc(int needArgs, int curArgs)
{
    if(needArgs < curArgs)
        error(curline, "Too few arguments");
    else if(needArgs > curArgs)
        error(curline, "Too many arguments");
}

bool Translator::isRegister(const std::string &str)
{
    return regs.find(str) != regs.end();
}

bool Translator::isFloatRegister(const std::string &str)
{
    return regsFloat.find(str) != regs.end();
}

bool Translator::isMemory(std::string &str)
{
    if(str[0] == '%')
    {
        str.erase(0,1);
        return true;
    }
    return false;
}

void Translator::putByte(unsigned char byte, int &cur)
{
    memcpy((void*)(bcode+cur), (void*)&byte, 1);
    cur++;
}

void Translator::putNumber(const std::string &n, int &cur)
{
    short num = std::strtol(n.c_str(), 0, 10);
    memcpy((void*)(bcode+cur), (void*)&num, sizeof(short));
    cur+=sizeof(short);
}

void Translator::putFloat(const std::string &n, int &cur)
{
    float num = std::strtof(n.c_str(), 0);
    memcpy((void*)(bcode+cur), (void*)&num, sizeof(float));
    cur+=sizeof(float);
}

void Translator::putRegister(const std::string &reg, int &cur)
{
    unsigned char r = regs[reg];
    memcpy((void*)(bcode+cur), (void*)&r, sizeof(unsigned char));
    cur+=sizeof(unsigned char);
}

void Translator::putFloatRegister(const std::string &reg, int &cur)
{
    unsigned char r = regsFloat[reg];
    memcpy((void*)(bcode+cur), (void*)&r, sizeof(unsigned char));
    cur+=sizeof(unsigned char);
}


void Translator::cmdMov(std::vector<std::string> &args, int &cur)
{
    checkArgc(3, args.size());

    if(isNumber(args[1]))
    {
        if(isRegister(args[2]))
        {
            putByte(OP_MOV_1, cur);
            putNumber(args[1], cur);
            putRegister(args[2], cur);
        }
        else if(isMemory(args[2]))
        {
            if(isNumber(args[2]))
            {
                putByte(OP_MOV_2, cur);
                putNumber(args[1], cur);
                putNumber(args[2], cur);
            }
            else if(isRegister(args[2]))
            {
                putByte(OP_MOV_3, cur);
                putNumber(args[1], cur);
                putRegister(args[2], cur);
            }
            else
            {
                error(curline, "Wrong second argument");
            }
        }
        else
        {
            error(curline, "Wrong second argument");
        }
    }
    else if(isFloat(args[1]))
    {
        if(isFloatRegister(args[2]))
        {
            putByte(OP_MOV_4, cur);
            putFloat(args[1], cur);
            putFloatRegister(args[2], cur);
        }
        else if(isMemory(args[2]))
        {
            if(isNumber(args[2]))
            {
                putByte(OP_MOV_5, cur);
                putFloat(args[1], cur);
                putNumber(args[2], cur);
            }
            else if(isFloatRegister(args[2]))
            {
                putByte(OP_MOV_6, cur);
                putFloat(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else
            {
                error(curline, "Wrong second argument");
            }
        }
        else
        {
            error(curline, "Wrong second argument");
        }
    }
    else if(isRegister(args[1]))
    {
        if(isRegister(args[2]))
        {
            putByte(OP_MOV_7, cur);
            putRegister(args[1], cur);
            putRegister(args[2], cur);
        }
        else if(isMemory(args[2]))
        {
            if(isNumber(args[2]))
            {
                putByte(OP_MOV_8, cur);
                putRegister(args[1], cur);
                putNumber(args[2], cur);
            }
            else if(isRegister(args[2]))
            {
                putByte(OP_MOV_9, cur);
                putRegister(args[1], cur);
                putRegister(args[2], cur);
            }
            else
            {
                error(curline, "Wrong second argument");
            }
        }
        else if(isFloatRegister(args[2]))
        {
            putByte(OP_MOV_17, cur);
            putRegister(args[1], cur);
            putFloatRegister(args[2], cur);
        }
        else
        {
            error(curline, "Wrong second argument");
        }
    }
    else if(isFloatRegister(args[1]))
    {
        if(isFloatRegister(args[2]))
        {
            putByte(OP_MOV_10, cur);
            putFloatRegister(args[1], cur);
            putFloatRegister(args[2], cur);
        }
        else if(isMemory(args[2]))
        {
            if(isNumber(args[2]))
            {
                putByte(OP_MOV_11, cur);
                putFloatRegister(args[1], cur);
                putNumber(args[2], cur);
            }
            else if(isFloatRegister(args[2]))
            {
                putByte(OP_MOV_12, cur);
                putFloatRegister(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else
            {
                error(curline, "Wrong second argument");
            }
        }
        else if(isRegister(args[2]))
        {
            putByte(OP_MOV_18, cur);
            putFloatRegister(args[1], cur);
            putRegister(args[2], cur);
        }
        else
        {
            error(curline, "Wrong second argument");
        }
    }
    else if(isMemory(args[1]))
    {
        if(isNumber(args[1]))
        {
            if(isRegister(args[2]))
            {
                putByte(OP_MOV_13, cur);
                putNumber(args[1], cur);
                putRegister(args[2], cur);
            }
            else if(isFloatRegister(args[2]))
            {
                putByte(OP_MOV_14, cur);
                putNumber(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else
            {
                error(curline, "Wrong second argument");
            }
        }
        else if(isRegister(args[1]))
        {
            if(isRegister(args[2]))
            {
                putByte(OP_MOV_15, cur);
                putRegister(args[1], cur);
                putRegister(args[2], cur);
            }
            else if(isFloatRegister(args[2]))
            {
                putByte(OP_MOV_16, cur);
                putRegister(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else
            {
                error(curline, "Wrong second argument");
            }
        }
        else
        {
            error(curline, "Wrong second argument");
        }
    }
    else
    {
        error(curline, "Wrong first argument");
    }
}

void Translator::cmdPass(std::vector<std::string> &args, int &cur)
{
    putByte(OP_PASS, cur);
}

void Translator::cmdInt(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(!isNumber(args[1]))
        error(curline, "Wrong first argument");

    putByte(OP_INT, cur);
    putNumber(args[1], cur);
}

void Translator::cmdAdd(std::vector<std::string> &args, int &cur)
{
    checkArgc(3, args.size());

    if(isNumber(args[1]) && isRegister(args[2]))
    {
        putByte(OP_ADD_1, cur);
        putNumber(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isRegister(args[1]) && isRegister(args[2]))
    {
        putByte(OP_ADD_2, cur);
        putRegister(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isFloat(args[1]) && isFloatRegister(args[2]))
    {
        putByte(OP_ADD_3, cur);
        putFloat(args[1], cur);
        putFloatRegister(args[2], cur);
    }
    else if(isFloatRegister(args[1]) && isFloatRegister(args[2]))
    {
        putByte(OP_ADD_4, cur);
        putFloatRegister(args[1], cur);
        putFloatRegister(args[2], cur);
    }
    else
    {
        error(curline, "Wrong arguments");
    }
}

void Translator::cmdSub(std::vector<std::string> &args, int &cur)
{
    checkArgc(3, args.size());

    if(isNumber(args[1]) && isRegister(args[2]))
    {
        putByte(OP_SUB_1, cur);
        putNumber(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isRegister(args[1]) && isRegister(args[2]))
    {
        putByte(OP_SUB_2, cur);
        putRegister(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isFloat(args[1]) && isFloatRegister(args[2]))
    {
        putByte(OP_SUB_3, cur);
        putFloat(args[1], cur);
        putFloatRegister(args[2], cur);
    }
    else if(isFloatRegister(args[1]) && isFloatRegister(args[2]))
    {
        putByte(OP_SUB_4, cur);
        putFloatRegister(args[1], cur);
        putFloatRegister(args[2], cur);
    }
    else
    {
        error(curline, "Wrong arguments");
    }
}

void Translator::cmdMul(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_MUL_1, cur);
        putNumber(args[1], cur);
    }
    else if(isRegister(args[1]))
    {
        putByte(OP_MUL_2, cur);
        putRegister(args[1], cur);
    }
    else if(isFloat(args[1]))
    {
        putByte(OP_MUL_3, cur);
        putFloat(args[1], cur);
    }
    else if(isFloatRegister(args[1]))
    {
        putByte(OP_MUL_4, cur);
        putFloatRegister(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdDiv(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_DIV_1, cur);
        putNumber(args[1], cur);
    }
    else if(isRegister(args[1]))
    {
        putByte(OP_DIV_2, cur);
        putRegister(args[1], cur);
    }
    else if(isFloat(args[1]))
    {
        putByte(OP_DIV_3, cur);
        putFloat(args[1], cur);
    }
    else if(isFloatRegister(args[1]))
    {
        putByte(OP_DIV_4, cur);
        putFloatRegister(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdJmp(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_JMP_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdJe(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_JE_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdJg(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_JG_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdJl(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_JL_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdJne(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_JNE_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdCmp(std::vector<std::string> &args, int &cur)
{
    checkArgc(3, args.size());

    if(isNumber(args[1]) && isRegister(args[2]))
    {
        putByte(OP_CMP_1, cur);
        putNumber(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isRegister(args[1]) && isRegister(args[2]))
    {
        putByte(OP_CMP_2, cur);
        putRegister(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isFloat(args[1]) && isFloatRegister(args[2]))
    {
        putByte(OP_CMP_3, cur);
        putFloat(args[1], cur);
        putFloatRegister(args[2], cur);
    }
    else if(isFloatRegister(args[1]) && isFloatRegister(args[2]))
    {
        putByte(OP_CMP_4, cur);
        putFloatRegister(args[1], cur);
        putFloatRegister(args[2], cur);
    }
    else
        error(curline, "Wrong arguments");
}

void Translator::cmdPush(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isNumber(args[1]))
    {
        putByte(OP_PUSH_1, cur);
        putNumber(args[1], cur);
    }
    else if(isRegister(args[1]))
    {
        putByte(OP_PUSH_2, cur);
        putRegister(args[1], cur);
    }
    else if(isFloat(args[1]))
    {
        putByte(OP_PUSH_3, cur);
        putFloat(args[1], cur);
    }
    else if(isFloatRegister(args[1]))
    {
        putByte(OP_PUSH_4, cur);
        putFloatRegister(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdPop(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isRegister(args[1]))
    {
        putByte(OP_POP_1, cur);
        putRegister(args[1], cur);
    }
    else if(isFloatRegister(args[1]))
    {
        putByte(OP_POP_2, cur);
        putFloatRegister(args[1], cur);
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdCall(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isMemory(args[1]))
    {
        if(isNumber(args[1]))
        {
            putByte(OP_CALL_1, cur);
            putNumber(args[1], cur);
        }
        else if(isRegister(args[1]))
        {
            putByte(OP_CALL_2, cur);
            putRegister(args[1], cur);
        }
        else
            error(curline, "Wrong first argument");
    }
    else
        error(curline, "Wrong first argument");
}

void Translator::cmdRet(std::vector<std::string> &args, int &cur)
{
    putByte(OP_RET, cur);
}

void Translator::cmdAnd(std::vector<std::string> &args, int &cur)
{
    checkArgc(3, args.size());

    if(isNumber(args[1]) && isRegister(args[2]))
    {
        putByte(OP_AND_1, cur);
        putNumber(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isRegister(args[1]) && isRegister(args[2]))
    {
        putByte(OP_AND_2, cur);
        putRegister(args[1], cur);
        putRegister(args[2], cur);
    }
    else
        error(curline, "Wrong arguments");
}

void Translator::cmdOr(std::vector<std::string> &args, int &cur)
{
    checkArgc(3, args.size());

    if(isNumber(args[1]) && isRegister(args[2]))
    {
        putByte(OP_OR_1, cur);
        putNumber(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isRegister(args[1]) && isRegister(args[2]))
    {
        putByte(OP_OR_2, cur);
        putRegister(args[1], cur);
        putRegister(args[2], cur);
    }
    else
        error(curline, "Wrong arguments");
}

void Translator::cmdXor(std::vector<std::string> &args, int &cur)
{
    checkArgc(3, args.size());

    if(isNumber(args[1]) && isRegister(args[2]))
    {
        putByte(OP_XOR_1, cur);
        putNumber(args[1], cur);
        putRegister(args[2], cur);
    }
    else if(isRegister(args[1]) && isRegister(args[2]))
    {
        putByte(OP_XOR_2, cur);
        putRegister(args[1], cur);
        putRegister(args[2], cur);
    }
    else
        error(curline, "Wrong arguments");
}

void Translator::cmdNot(std::vector<std::string> &args, int &cur)
{
    checkArgc(2, args.size());

    if(isRegister(args[1]))
    {
        putByte(OP_NOT, cur);
        putRegister(args[1], cur);
    }
    else
        error(curline, "Wrong arguments");
}


void Translator::translate(const std::string &infile, const std::string &outfile)
{
    std::ifstream source(infile.c_str());
    if(!source.is_open())
    {
        std::cerr << "Can not open file " << infile << "!\n";
        exit(1);
    }

    while(!source.eof())
    {
        std::vector<std::string> args;

        std::getline(source, tmp);
        if(tmp.length() == 0)
            continue;

        split(tmp, args);
        curline++;

        if(args[0] == "pass")
            cmdPass(args, cur);
        else if(args[0] == "mov")
            cmdMov(args, cur);
        else if(args[0] == "int")
            cmdInt(args, cur);
        else if(args[0] == "add")
            cmdAdd(args, cur);
        else if(args[0] == "sub")
            cmdSub(args, cur);
        else if(args[0] == "mul")
            cmdMul(args, cur);
        else if(args[0] == "div")
            cmdDiv(args, cur);
        else if(args[0] == "jmp")
            cmdJmp(args, cur);
        else if(args[0] == "je")
            cmdJe(args, cur);
        else if(args[0] == "jg")
            cmdJg(args, cur);
        else if(args[0] == "jl")
            cmdJl(args, cur);
        else if(args[0] == "jne")
            cmdJne(args, cur);
        else if(args[0] == "cmp")
            cmdCmp(args, cur);
        else
            error(curline, std::string("Unknown command!") + args[0]);
    }

    source.close();

    std::ofstream fout(outfile.c_str(), std::ios::binary);
    fout.write((char*)bcode, cur);
    fout.close();
}

void Translator::printBCode()
{
    for(int i = 0; i < cur; ++i)
    {
        std::cout << (int)bcode[i] << ' ';
    }
}
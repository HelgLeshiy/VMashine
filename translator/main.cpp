#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

#include "../global/defines.h"

const int MAXSIZE = 5000;

class Translator
{
public:
    Translator();
    void translate(const std::string &infile, const std::string &outfile);
    void printBCode();

private:
    void cmdMov(std::vector<std::string> &args, int &cur);
    void cmdPass(std::vector<std::string> &args, int &cur);
    void cmdInt(std::vector<std::string> &args, int &cur);
    void cmdAdd(std::vector<std::string> &args, int &cur);
    void cmdSub(std::vector<std::string> &args, int &cur);
    void cmdMul(std::vector<std::string> &args, int &cur);
    void cmdDiv(std::vector<std::string> &args, int &cur);
    void cmdJmp(std::vector<std::string> &args, int &cur);
    void cmdJe(std::vector<std::string> &args, int &cur);
    void cmdJg(std::vector<std::string> &args, int &cur);
    void cmdJl(std::vector<std::string> &args, int &cur);
    void cmdJne(std::vector<std::string> &args, int &cur);
    void cmdCmp(std::vector<std::string> &args, int &cur);
    void cmdPush(std::vector<std::string> &args, int &cur);
    void cmdPop(std::vector<std::string> &args, int &cur);
    void cmdCall(std::vector<std::string> &args, int &cur);
    void cmdRet(std::vector<std::string> &args, int &cur);
    void cmdAnd(std::vector<std::string> &args, int &cur);
    void cmdOr(std::vector<std::string> &args, int &cur);
    void cmdXor(std::vector<std::string> &args, int &cur);
    void cmdNot(std::vector<std::string> &args, int &cur);

private:
    void initRegs();
    void split(const std::string &str, std::vector<std::string> &args);
    void error(int line, const std::string &msg);

    bool isNumber(const std::string &str);
    bool isFloat(const std::string &str);
    bool isRegister(const std::string &str);
    bool isFloatRegister(const std::string &str);
    bool isMemory(std::string &str);

    void putByte(unsigned char byte, int &cur);
    void putNumber(const std::string &n, int &cur);
    void putFloat(const std::string &n, int &cur);
    void putRegister(const std::string &reg, int &cur);
    void putFloatRegister(const std::string &reg, int &cur);

private:
    unsigned char bcode[MAXSIZE];
    std::map<std::string, unsigned> regs;
    std::map<std::string, unsigned> regsFloat;

    int curline, cur;
    std::string tmp;
};

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

void Translator::split(const std::string &str, std::vector<std::string> &args)
{
    args.clear();

    std::string delim(" ");
    size_t prev = 0;
    size_t next;
    size_t delta = delim.length();

    while( ( next = str.find( " ", prev ) ) != std::string::npos )
    {
        args.push_back( str.substr( prev, next-prev ) );
        prev = next + delta;
    }
    args.push_back( str.substr( prev ) );
}

void Translator::error(int line, const std::string &msg)
{
    std::cerr << "\nError! Line " << line << ": " << msg << " \"" << tmp << "\"\n";
    exit(1);
}

bool Translator::isNumber(const std::string &str)
{
    int i = 0;
    if(str[i] == '-') i++;

    for(; i < str.length(); ++i)
    {
        if(!std::isdigit(str[i]))
            return false;
    }

    return true;
}

bool Translator::isFloat(const std::string &str)
{
    bool point = false;
    for(int i = 0; i < str.length(); i++)
        if(str[i] == '.')
        {
            point = true;
            break;
        }
    if(!point) return false;

    int i = 0;
    if(str[i] == '-') i++;

    for(; i < str.length(); ++i)
    {
        if(!std::isdigit(str[i]) && str[i]!='.')
            return false;
    }
    return true;
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
    if(args.size() != 3)
        error(curline, "Syntax error");

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
                error(curline, "Syntax error");
            }
        }
        else
        {
            error(curline, "Syntax error");
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
                error(curline, "Syntax error");
            }
        }
        else
        {
            error(curline, "Syntax error");
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
                error(curline, "Syntax error");
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
            error(curline, "Syntax error");
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
                error(curline, "Syntax error");
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
            error(curline, "Syntax error");
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
                error(curline, "Syntax error");
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
                error(curline, "Syntax error");
            }
        }
        else
        {
            error(curline, "Syntax error");
        }
    }
    else
    {
        error(curline, "Syntax error");
    }
}

void Translator::cmdPass(std::vector<std::string> &args, int &cur)
{
    putByte(OP_PASS, cur);
}

void Translator::cmdInt(std::vector<std::string> &args, int &cur)
{
    if(args.size()!=2)
        error(curline, "Syntax error");

    if(!isNumber(args[1]))
        error(curline, "Syntax error");

    putByte(OP_INT, cur);
    putNumber(args[1], cur);
}

void Translator::cmdAdd(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 3)
        error(curline, "Syntax error");

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
        error(curline, "Syntax error");
    }
}

void Translator::cmdSub(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 3)
        error(curline, "Syntax error");

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
        error(curline, "Syntax error");
    }
}

void Translator::cmdMul(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 2)
        error(curline, "Syntax error");

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
        error(curline, "Syntax error");
}

void Translator::cmdDiv(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 2)
        error(curline, "Syntax error");

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
        error(curline, "Syntax error");
}

void Translator::cmdJmp(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 2)
        error(curline, "Syntax error");

    if(isNumber(args[1]))
    {
        putByte(OP_JMP_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Syntax error");
}

void Translator::cmdJe(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 2)
        error(curline, "Syntax error");

    if(isNumber(args[1]))
    {
        putByte(OP_JE_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Syntax error");
}

void Translator::cmdJg(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 2)
        error(curline, "Syntax error");

    if(isNumber(args[1]))
    {
        putByte(OP_JG_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Syntax error");
}

void Translator::cmdJl(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 2)
        error(curline, "Syntax error");

    if(isNumber(args[1]))
    {
        putByte(OP_JL_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Syntax error");
}

void Translator::cmdJne(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 2)
        error(curline, "Syntax error");

    if(isNumber(args[1]))
    {
        putByte(OP_JNE_1, cur);
        putNumber(args[1], cur);
    }
    else
        error(curline, "Syntax error");
}

void Translator::cmdCmp(std::vector<std::string> &args, int &cur)
{
    if(args.size() != 3)
        error(curline, "Syntax error");

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
        error(curline, "Syntax error");
}

void Translator::cmdPush(std::vector<std::string> &args, int &cur)
{

}

void Translator::cmdPop(std::vector<std::string> &args, int &cur)
{

}

void Translator::cmdCall(std::vector<std::string> &args, int &cur)
{

}

void Translator::cmdRet(std::vector<std::string> &args, int &cur)
{

}

void Translator::cmdAnd(std::vector<std::string> &args, int &cur)
{

}

void Translator::cmdOr(std::vector<std::string> &args, int &cur)
{

}

void Translator::cmdXor(std::vector<std::string> &args, int &cur)
{

}

void Translator::cmdNot(std::vector<std::string> &args, int &cur)
{

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

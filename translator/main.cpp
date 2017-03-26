#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

const int MAXSIZE = 5000;

class Translator
{
public:
    Translator();
    void translate(const std::string &infile, const std::string &outfile);

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
        {
            putByte(19, cur);
        }
        else if(args[0] == "mov")
        {
            if(args.size() != 3)
                error(curline, "Syntax error");

            if(isNumber(args[1]))
            {
                if(isRegister(args[2]))
                {
                    putByte(1, cur);
                    putNumber(args[1], cur);
                    putRegister(args[2], cur);
                }
                else if(isMemory(args[2]))
                {
                    if(isNumber(args[2]))
                    {
                        putByte(2, cur);
                        putNumber(args[1], cur);
                        putNumber(args[2], cur);
                    }
                    else if(isRegister(args[2]))
                    {
                        putByte(3, cur);
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
                    putByte(4, cur);
                    putFloat(args[1], cur);
                    putFloatRegister(args[2], cur);
                }
                else if(isMemory(args[2]))
                {
                    if(isNumber(args[2]))
                    {
                        putByte(5, cur);
                        putFloat(args[1], cur);
                        putNumber(args[2], cur);
                    }
                    else if(isFloatRegister(args[2]))
                    {
                        putByte(6, cur);
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
                    putByte(7, cur);
                    putRegister(args[1], cur);
                    putRegister(args[2], cur);
                }
                else if(isMemory(args[2]))
                {
                    if(isNumber(args[2]))
                    {
                        putByte(8, cur);
                        putRegister(args[1], cur);
                        putNumber(args[2], cur);
                    }
                    else if(isRegister(args[2]))
                    {
                        putByte(9, cur);
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
                    putByte(17, cur);
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
                    putByte(10, cur);
                    putFloatRegister(args[1], cur);
                    putFloatRegister(args[2], cur);
                }
                else if(isMemory(args[2]))
                {
                    if(isNumber(args[2]))
                    {
                        putByte(11, cur);
                        putFloatRegister(args[1], cur);
                        putNumber(args[2], cur);
                    }
                    else if(isFloatRegister(args[2]))
                    {
                        putByte(12, cur);
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
                    putByte(18, cur);
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
                        putByte(13, cur);
                        putNumber(args[1], cur);
                        putRegister(args[2], cur);
                    }
                    else if(isFloatRegister(args[2]))
                    {
                        putByte(14, cur);
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
                        putByte(15, cur);
                        putRegister(args[1], cur);
                        putRegister(args[2], cur);
                    }
                    else if(isFloatRegister(args[2]))
                    {
                        putByte(16, cur);
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
        else if(args[0] == "int")
        {
            if(args.size()!=2)
                error(curline, "Syntax error");

            if(!isNumber(args[1]))
                error(curline, "Syntax error");

            putByte(20, cur);
            putNumber(args[1], cur);
        }
        else if(args[0] == "add")
        {
            if(args.size() != 3)
                error(curline, "Syntax error");

            if(isNumber(args[1]) && isRegister(args[2]))
            {
                putByte(22, cur);
                putNumber(args[1], cur);
                putRegister(args[2], cur);
            }
            else if(isRegister(args[1]) && isRegister(args[2]))
            {
                putByte(23, cur);
                putRegister(args[1], cur);
                putRegister(args[2], cur);
            }
            else if(isFloat(args[1]) && isFloatRegister(args[2]))
            {
                putByte(24, cur);
                putFloat(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else if(isFloatRegister(args[1]) && isFloatRegister(args[2]))
            {
                putByte(25, cur);
                putFloatRegister(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else
            {
                error(curline, "Syntax error");
            }
        }
        else if(args[0] == "sub")
        {
            if(args.size() != 3)
                error(curline, "Syntax error");

            if(isNumber(args[1]) && isRegister(args[2]))
            {
                putByte(26, cur);
                putNumber(args[1], cur);
                putRegister(args[2], cur);
            }
            else if(isRegister(args[1]) && isRegister(args[2]))
            {
                putByte(27, cur);
                putRegister(args[1], cur);
                putRegister(args[2], cur);
            }
            else if(isFloat(args[1]) && isFloatRegister(args[2]))
            {
                putByte(28, cur);
                putFloat(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else if(isFloatRegister(args[1]) && isFloatRegister(args[2]))
            {
                putByte(29, cur);
                putFloatRegister(args[1], cur);
                putFloatRegister(args[2], cur);
            }
            else
            {
                error(curline, "Syntax error");
            }
        }
        else
            error(curline, "Syntax error");
    }

    source.close();

    for(int i = 0; i < cur; i++)
        std::cout << i << '\t' << (int)bcode[i] << std::endl;

    std::ofstream fout(outfile.c_str(), std::ios::binary);
    fout.write((char*)bcode, cur);
    fout.close();
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

    return 0;
}

#pragma once

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
    void checkArgc(int needArgs, int curArgs);

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
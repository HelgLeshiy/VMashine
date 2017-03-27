#include "misc.h"

void split(const std::string &str, std::vector<std::string> &args)
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

void error(int line, const std::string &msg)
{
    std::cerr << "\nError! Line " << line << ": " << msg << "\n";
    exit(1);
}

bool isNumber(const std::string &str)
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

bool isFloat(const std::string &str)
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
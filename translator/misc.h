#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>

void split(const std::string &str, std::vector<std::string> &args);
void error(int line, const std::string &msg);
bool isNumber(const std::string &str);
bool isFloat(const std::string &str);
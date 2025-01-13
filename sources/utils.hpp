#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cctype>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>

void trim(std::string &str);
int countChar(const std::string &str, char ch);
std::vector<std::string> splitString(std::string& input);
int stringToInt(std::string& str);

#endif
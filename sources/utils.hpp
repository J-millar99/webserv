#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cctype>
#include <list>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/stat.h>

void trim(std::string &str);
int countChar(const std::string &str, char ch);
std::vector<std::string> splitString(std::string& input);
int stringToInt(std::string& str);
bool isDirectory(const std::string& path);
bool isRegularFile(const std::string& path);
bool checkPathFormat(const std::string& path);

#endif
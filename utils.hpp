#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <string>

int rand(int cap);
double rand01();
bool coin_flip(double p);
std::pair<std::string,std::string> split(std::string str, char delim);

#endif
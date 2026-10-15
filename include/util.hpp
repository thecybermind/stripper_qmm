/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2026
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#ifndef STRIPPER_QMM_UTIL_HPP
#define STRIPPER_QMM_UTIL_HPP

#include <string>

std::string str_tolower(std::string str);
bool str_stristr(const std::string& haystack, const std::string& needle);
int str_stricmp(const std::string& s1, const std::string& s2);
bool str_striequal(const std::string& s1, const std::string& s2);

// "safe" strncpy that always null-terminates
char* strncpyz(char* dest, const char* src, std::size_t count); 

#endif // STRIPPER_QMM_UTIL_HPP

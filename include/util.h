/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2026
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#ifndef STRIPPER_QMM_UTIL_H
#define STRIPPER_QMM_UTIL_H

#include <string>

std::string str_tolower(std::string str);
int str_stristr(std::string haystack, std::string needle);
int str_stricmp(std::string s1, std::string s2);
int str_striequal(std::string s1, std::string s2);

// "safe" strncpy that always null-terminates
char* strncpyz(char* dest, const char* src, std::size_t count); 

#endif // STRIPPER_QMM_UTIL_H

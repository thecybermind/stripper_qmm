/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2026
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#define _CRT_SECURE_NO_WARNINGS 1
#include <cstring>
#include <string>
#include <cctype>
#include "util.hpp"

std::string str_tolower(std::string str) {
    for (auto& c : str)
        c = (char)std::tolower((unsigned char)c);

    return str;
}


bool str_stristr(const std::string& haystack, const std::string& needle) {
    return str_tolower(haystack).find(str_tolower(needle)) != std::string::npos;
}


int str_stricmp(const std::string& s1, const std::string& s2) {
    return str_tolower(s1).compare(str_tolower(s2));
}


bool str_striequal(const std::string& s1, const std::string& s2) {
    return str_stricmp(s1, s2) == 0;
}


// "safe" strncpy that always null-terminates
char* strncpyz(char* dest, const char* src, size_t count) {
    if (!dest || !src || !count)
        return dest;
    // strncpy may not null-terminate if it writes a full buffer
    char* ret = strncpy(dest, src, count);
    // so we null-terminate manually
    if (count)
        dest[count - 1] = '\0';
    else
        dest[0] = '\0';
    return ret;
}

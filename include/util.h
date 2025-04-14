/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __STRIPPER_QMM_UTIL_H__
#define __STRIPPER_QMM_UTIL_H__

#include <stdio.h>

int str_stristr(std::string haystack, std::string needle);
int str_stricmp(std::string s1, std::string s2);
int str_striequal(std::string s1, std::string s2);

// read a single line from a string buffer. store in "out" string, return next buf pointer to start from, null if eof
#if defined(GAME_MOHAA)
const char* read_line(const char* buf, std::string& out);
#elif defined(GAME_Q2R)
// read a single line from a FILE*. store in "out" string, return true if eof
bool read_line(FILE* f, std::string& out);
#else
// read a single line from a file handle. store in "out" string, return true if eof
bool read_line(fileHandle_t f, std::string& out);
#endif

#endif // __STRIPPER_QMM_UTIL_H__

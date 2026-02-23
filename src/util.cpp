/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2026
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#define _CRT_SECURE_NO_WARNINGS 1
#include "version.h"
#include <qmmapi.h>
#include <cstring>
#include <string>
#include "game.h"
#include "util.h"


std::string str_tolower(std::string str) {
	for (auto& c : str)
		c = (char)std::tolower((unsigned char)c);

	return str;
}


int str_stristr(std::string haystack, std::string needle) {
	return str_tolower(haystack).find(str_tolower(needle)) != std::string::npos;
}


int str_stricmp(std::string s1, std::string s2) {
	return str_tolower(s1).compare(str_tolower(s2));
}


int str_striequal(std::string s1, std::string s2) {
	return str_stricmp(s1, s2) == 0;
}


// "safe" strncpy that always null-terminates
char* strncpyz(char* dest, const char* src, std::size_t count) {
	char* ret = strncpy(dest, src, count);
	dest[count - 1] = '\0';
	return ret;
}

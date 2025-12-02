/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#define _CRT_SECURE_NO_WARNINGS 1
#include "version.h"
#include <string>
#include <qmmapi.h>
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


// read a single line from a file handle. store in out string, return false if eof
bool read_line(fileHandle_t f, std::string& out) {
	char buf = -1;
#if defined(GAME_MOHAA)
	int cmdread = G_FS_READ_QMM;
#else
	int cmdread = G_FS_READ;
#endif
	g_syscall(cmdread, &buf, 1, f);
	if (buf == -1)
		return false;

	// read text until we hit the end of the file
	while (buf != -1) {
		out += buf;

		// exit on newline (in the case of "\r\n", the "\n" will be ltrimmed from the next line)
		if (buf == '\r' || buf == '\n')
			break;

		buf = -1;
		g_syscall(cmdread, &buf, 1, f);
	}

	// ltrim
	unsigned int li = 0;
	while (li < out.size() && std::isspace(out[li]))
		++li;
	out = out.substr(li);

	// rtrim
	int ri = (int)out.size() - 1;
	while (ri >= 0 && std::isspace(out[ri]))
		ri--;
	out = out.substr(0, ri + 1);

	return true;
}

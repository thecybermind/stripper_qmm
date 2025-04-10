/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#include "version.h"
#include <string>
#include <qmmapi.h>
#include "game.h"
#include "util.h"

int str_stristr(std::string haystack, std::string needle) {
	for (auto& c : haystack)
		c = (char)std::tolower((unsigned char)c);
	for (auto& c : needle)
		c = (char)std::tolower((unsigned char)c);

	return haystack.find(needle) != std::string::npos;
}

int str_stricmp(std::string s1, std::string s2) {
	for (auto& c : s1)
		c = (char)std::tolower((unsigned char)c);
	for (auto& c : s2)
		c = (char)std::tolower((unsigned char)c);

	return s1.compare(s2);
}

int str_striequal(std::string s1, std::string s2) {
	return str_stricmp(s1, s2) == 0;
}

// read a single line from a file handle. store in out string, return false if eof
bool read_line(fileHandle_t f, std::string& out) {
	std::string line = "";

	char buf = -1;
	g_syscall(G_FS_READ, &buf, 1, f);
	if (buf == -1)
		return false;

	// read text until we hit the end of the file
	while (buf != -1) {
		out += buf;

		// exit on newline (in the case of "\r\n", the "\n" will be ltrimmed from the next line
		if (buf == '\r' || buf == '\n')
			break;

		buf = -1;
		g_syscall(G_FS_READ, &buf, 1, f);
	}

	// ltrim
	int i = 0;
	while (i < out.size() && std::isspace(out[i]))
		++i;
	out = out.substr(i);

	// rtrim
	i = out.size() - 1;
	while (i >= 0 && std::isspace(out[i]))
		i--;
	out = out.substr(0, i + 1);

	return true;
}


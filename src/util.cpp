/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#include "version.h"
#include <qmmapi.h>
#include "game.h"
#include "util.h"

//read a single line from a file handle
const char* read_line(fileHandle_t f, int fs) {
	static char temp[1024];
	char* p = temp;
	
	//how many bytes we've read
	static int fct = 0;

	//reset flag
	if (fs == -1)
		fct = 0;

	temp[0] = '\0';
	int i = 0;

	char buf = '.';
	g_syscall(G_FS_READ, &buf, 1, f); ++fct;
	if (fct > fs)
		return NULL;

	//read text until we hit a newline
	while ((buf != '\r') && (buf != '\n') && (i < sizeof(temp) - 1)) {
		temp[i++] = buf;
		if (fct >= fs)
			break;
		g_syscall(G_FS_READ, &buf, 1, f); ++fct;
	}
	temp[i] = '\0';

	while (isspace(*p))
		++p;

	for (--i; i >= 0 && isspace(temp[i]); --i) {
		temp[i] = '\0';
	}

	return p;
}

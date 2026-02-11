/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2026
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#ifndef STRIPPER_QMM_VERSION_H
#define STRIPPER_QMM_VERSION_H

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

#define STRIPPER_QMM_VERSION_MAJOR	2
#define STRIPPER_QMM_VERSION_MINOR	4
#define STRIPPER_QMM_VERSION_REV	6

#define STRIPPER_QMM_VERSION		STRINGIFY(STRIPPER_QMM_VERSION_MAJOR) "." STRINGIFY(STRIPPER_QMM_VERSION_MINOR) "." STRINGIFY(STRIPPER_QMM_VERSION_REV)

#if defined(_WIN32)
#define STRIPPER_QMM_OS             "Windows"
#ifdef _WIN64
#define STRIPPER_QMM_ARCH           "x86_64"
#else
#define STRIPPER_QMM_ARCH           "x86"
#endif
#elif defined(__linux__)
#define STRIPPER_QMM_OS             "Linux"
#ifdef __LP64__
#define STRIPPER_QMM_ARCH           "x86_64"
#else
#define STRIPPER_QMM_ARCH           "x86"
#endif
#endif

#define STRIPPER_QMM_VERSION_DWORD	STRIPPER_QMM_VERSION_MAJOR , STRIPPER_QMM_VERSION_MINOR , STRIPPER_QMM_VERSION_REV , 0
#define STRIPPER_QMM_COMPILE		__TIME__ " " __DATE__
#define STRIPPER_QMM_BUILDER		"Kevin Masterson"

#define STRIPPER_QMM_BROADCAST_STR  "STRIPPER_INIT"
#define STRIPPER_QMM_VERSION_INT    ((STRIPPER_QMM_VERSION_MAJOR << 16) | (STRIPPER_QMM_VERSION_MINOR << 8) | STRIPPER_QMM_VERSION_REV)
#endif // STRIPPER_QMM_VERSION_H

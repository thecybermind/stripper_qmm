/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __STRIPPER_QMM_GAME_H__
#define __STRIPPER_QMM_GAME_H__

#if defined(GAME_Q3A)
    #include <q3a/game/g_local.h>
#elif defined(GAME_RTCWMP)
    #include <rtcwmp/game/g_local.h>
#elif defined(GAME_RTCWSP)
    #include <rtcwsp/game/g_local.h>
#elif defined(GAME_JK2MP)
    #include <jk2mp/game/g_local.h>
#elif defined(GAME_JAMP)
    #include <jamp/game/g_local.h>
#elif defined(GAME_WET)
    #include <wet/game/g_local.h>
#elif defined(GAME_STVOYHM)
    #include <stvoyhm/game/g_local.h>
#elif defined(GAME_SOF2MP)
    #include <sof2mp/game/g_local.h>
#elif defined(GAME_STEF2)
    #include <stef2/game/g_local.h>
    #include <game_stef2.h>
    #define GAME_HAS_SPAWNENTS
#elif defined(GAME_MOHAA)
    #include <mohaa/fgame/g_local.h>
    #include <game_mohaa.h>
    #define GAME_HAS_SPAWNENTS
#elif defined(GAME_MOHSH)
    #include <mohsh/fgame/g_local.h>
    #include <game_mohsh.h>
    #define GAME_HAS_SPAWNENTS
#elif defined(GAME_MOHBT)
    #include <mohaa/fgame/g_local.h>
    #include <game_mohbt.h>
    #define GAME_HAS_SPAWNENTS
#elif defined(GAME_QUAKE2)
    #include <quake2/game/g_local.h>
    #include <game_quake2.h>
    #define GAME_HAS_SPAWNENTS
    #define CVAR_NORESTART 0
#elif defined(GAME_Q2R)
    #include <q2r/rerelease/g_local.h>
    #include <game_q2r.h>
    #define GAME_HAS_SPAWNENTS
    #define CVAR_NORESTART 0
#endif

#endif // __STRIPPER_QMM_GAME_H__

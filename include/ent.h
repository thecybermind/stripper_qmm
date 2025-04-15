/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#ifndef __STRIPPER_QMM_ENT_H__
#define __STRIPPER_QMM_ENT_H__

#include <vector>
#include <map>
#include <string>
#include "CLinkList.h"
#include "game.h"

// this represents a single entity
struct ent_t {
	std::string classname;
	std::map<std::string, std::string> keyvals;
};

// this stores all the ents loaded from the map
// we save this so we can dump them to file if needed
extern std::vector<ent_t> g_mapents;

// this stores all the ents that should be passed to the mod (g_mapents +/- modifications)
extern std::vector<ent_t> g_modents;

// this stores all info for entities that should be replaced
// nodes are read and removed from this list when a "with" entity is found
extern std::vector<ent_t> g_replaceents;

#if defined(GAME_Q3A) || defined(GAME_RTCWMP) || defined(GAME_RTCWSP) || defined(GAME_JK2MP) || defined(GAME_JAMP) || defined(GAME_STVOYHM) || defined(GAME_WET)
// passes the next entity token to the mod
intptr_t ent_next_token(char* buf, intptr_t len);
#endif // Q3A || RTCWMP || RTCWSP || JK2MP || JAMP || STVOYMH || WET

#if defined(GAME_STEF2) || defined(GAME_MOHAA) || defined(GAME_MOHSH) || defined(GAME_MOHBT) || defined(GAME_Q2R) || defined(GAME_QUAKE2)
// generate an entstring to pass to the mod
const char* ents_generate_entstring(std::vector<ent_t>& list);
#endif // STEF2 || MOHAA || MOHSH || MOHBT || Q2R || QUAKE2

// gets all the entity tokens from the engine
void ents_load_tokens(std::vector<ent_t>& list, const char* entstring = nullptr);

// outputs ent list to a file
void ents_dump_to_file(std::vector<ent_t>& list, std::string file);

// load and parse config file
void ent_load_config(std::string file);

#endif // __STRIPPER_QMM_ENT_H__

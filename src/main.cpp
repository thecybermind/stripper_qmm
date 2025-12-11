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
#include "ent.h"
#include "util.h"

pluginres_t* g_result = nullptr;
plugininfo_t g_plugininfo = {
	QMM_PIFV_MAJOR,									// plugin interface version major
	QMM_PIFV_MINOR,									// plugin interface version minor
	"Stripper",										// name of plugin
	STRIPPER_QMM_VERSION,							// version of plugin
	"Change map entities during load",				// description of plugin
	STRIPPER_QMM_BUILDER,							// author of plugin
	"https://github.com/thecybermind/stripper_qmm", // website of plugin
};
eng_syscall_t g_syscall = nullptr;
mod_vmMain_t g_vmMain = nullptr;
pluginfuncs_t* g_pluginfuncs = nullptr;
pluginvars_t* g_pluginvars = nullptr;


C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo) {
	QMM_GIVE_PINFO();
}


C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc, pluginres_t* presult, pluginfuncs_t* pluginfuncs, pluginvars_t* pluginvars) {
	QMM_SAVE_VARS();
	return 1;
}


C_DLLEXPORT void QMM_Detach() {
}


// handle retrieving map entities, loading stripper configs, and modifying entities
static bool s_load_and_modify_ents();

#if defined(GAME_JAMP)
// store mapents/modents lists per subbsp index
static std::map<intptr_t, std::vector<ent_t>> s_subbsp_mapents;
static std::map<intptr_t, std::vector<ent_t>> s_subbsp_modents;
// store active subbsp index
static int subbsp_index = -1;
#endif // GAME_JAMP


C_DLLEXPORT intptr_t QMM_vmMain(intptr_t cmd, intptr_t* args) {
	if (cmd == GAME_INIT) {
		// init msg
		QMM_WRITEQMMLOG(QMM_VARARGS("Stripper v" STRIPPER_QMM_VERSION " (%s) by " STRIPPER_QMM_BUILDER " is loaded\n", QMM_GETGAMEENGINE()), QMMLOG_NOTICE, "STRIPPER");
		// register cvar
		g_syscall(G_CVAR_REGISTER, nullptr, "stripper_version", STRIPPER_QMM_VERSION, CVAR_ROM | CVAR_SERVERINFO | CVAR_NORESTART);
		g_syscall(G_CVAR_SET, "stripper_version", STRIPPER_QMM_VERSION);

#if !defined(GAME_HAS_SPAWNENTS)

		// games without a GAME_SPAWN_ENTITIES msg get entities and load configs here during QMM_vmMain(GAME_INIT).
		// entities are passed to the mod with the QMM_syscall(G_GET_ENTITY_TOKEN) hook

		s_load_and_modify_ents();

		QMM_WRITEQMMLOG("Stripper loading complete.\n", QMMLOG_NOTICE, "STRIPPER");

#endif // !GAME_HAS_SPAWNENTS

	}
	// handle stripper_dump command
	else if (cmd == GAME_CONSOLE_COMMAND) {
		char buf[20];
// Quake 2 and Quake 2 Remastered use 1-based command arguments (presumably the 0th arg is "sv"?)
#if defined(GAME_Q2R) || defined(GAME_QUAKE2)
		int argn = 1;
#else
		int argn = 0;
#endif
		QMM_ARGV(argn, buf, sizeof(buf));
		if (str_striequal(buf, "stripper_dump")) {
			ents_dump_to_file(g_mapents, QMM_VARARGS("qmmaddons/stripper/dumps/%s.txt", QMM_GETSTRCVAR("mapname")));
			ents_dump_to_file(g_modents, QMM_VARARGS("qmmaddons/stripper/dumps/%s_modents.txt", QMM_GETSTRCVAR("mapname")));
			// don't pass this to mod since we handled the command
			QMM_RET_SUPERCEDE(1);
		}
	}

#if defined(GAME_HAS_SPAWNENTS)

	// games with a GAME_SPAWN_ENTITIES msg load configs here during QMM_vmMain(GAME_SPAWN_ENTITIES).
	// entities are passed to the mod by changing the entstring parameter.

	// specifically not "else if" since in JK2SP/JASP/STVOYSP, GAME_SPAWN_ENTITIES is really just an alias for GAME_INIT
	if (cmd == GAME_SPAWN_ENTITIES) {
		// moh??:   void (*SpawnEntities)(char *entstring, int levelTime);
		// stef2:   void (*SpawnEntities)(const char *mapname, const char *entstring, int levelTime);
		// quake2:  void (*SpawnEntities)(const char *mapname, const char *entstring, const char *spawnpoint);
		// q2r:     void (*SpawnEntities)(const char *mapname, const char *entstring, const char *spawnpoint);
		// jk2sp:   void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
		// jasp:    void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
		// stvoysp: void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
#if defined(GAME_STEF2) || defined(GAME_Q2R) || defined(GAME_QUAKE2)
		int entarg = 1;
#elif defined(GAME_JK2SP) || defined(GAME_JASP) || defined(GAME_STVOYSP)
		int entarg = 3;
#elif defined(GAME_MOHAA) || defined(GAME_MOHSH) || defined(GAME_MOHBT)
		int entarg = 0;
#else
		int entarg = 0;
#endif

		if (s_load_and_modify_ents()) {
			// generate new entstring from g_modents to pass to mod
			const char* entstring = ents_generate_entstring(g_modents);

			// replace entstring arg for passing to mod
			args[entarg] = (intptr_t)entstring;
		}

		QMM_WRITEQMMLOG("Stripper loading complete.\n", QMMLOG_NOTICE, "STRIPPER");
	}

#endif

	QMM_RET_IGNORED(1);
}


C_DLLEXPORT intptr_t QMM_syscall(intptr_t cmd, intptr_t* args) {
#if !defined(GAME_HAS_SPAWNENTS)

 #if !defined(GAME_JAMP)
	
	// loop through the ent list and return a single token
	if (cmd == G_GET_ENTITY_TOKEN) {
		char* entity = (char*)args[0];
		intptr_t length = args[1];
		intptr_t ret = ent_next_token(g_modents, entity, length);
		// don't pass this to engine since we already pulled all entities from the engine
		QMM_RET_SUPERCEDE(ret);
	}

 #else
	
	// loop through the ent list and return a single token
	if (cmd == G_GET_ENTITY_TOKEN) {
		// if not in a subbsp, just return from g_modents
		if (subbsp_index == -1) {
			char* entity = (char*)args[0];
			intptr_t length = args[1];
			intptr_t ret = ent_next_token(g_modents, entity, length);
			// don't pass this to engine since we already pulled all entities from the engine
			QMM_RET_SUPERCEDE(ret);
		}
		// in a subbsp
		else {
			char* entity = (char*)args[0];
			intptr_t length = args[1];
			intptr_t ret = ent_next_token(s_subbsp_modents[subbsp_index], entity, length);
			// don't pass this to engine since we already pulled all entities from the engine
			QMM_RET_SUPERCEDE(ret);
		}
	}

 #endif // GAME_JAMP

#endif // !GAME_HAS_SPAWNENTS

#if defined(GAME_JASP)

	/* Jedi Academy singleplayer has a SetActiveSubBSP engine function that returns the subbsp entstring directly
	   for the mod to parse just like the main one passed to Init. So all we need to do here is call the engine's
	   SetActiveSubBSP function directly and parse the returned entstring into entities, then load the global+map
	   configs as normal, then generate a new entstring to pass to the mod.
	*/
	if (cmd == G_SET_ACTIVE_SUBBSP) {
		intptr_t index = args[0];
		// -1 is used to tell the engine that the mod is done with the SubBSP
		if (index < 0)
			QMM_RET_IGNORED(1);
		QMM_WRITEQMMLOG(QMM_VARARGS("Parsing SubBSP entity list %d\n", index), QMMLOG_INFO, "STRIPPER");

		// get the entity string from the engine, parse it, and save to mapents
		const char* ret = (const char*)g_syscall(G_SET_ACTIVE_SUBBSP, index);
		std::vector<std::string> tokens = ent_parse_entstring(ret);
		std::vector<ent_t> mapents;
		ents_load_tokens(mapents, tokens);

		// check for valid entity list
		if (mapents.empty()) {
			QMM_WRITEQMMLOG(QMM_VARARGS("Empty SubBSP entity list %d from engine\n", index), QMMLOG_DEBUG, "STRIPPER");
			// return exact pointer from the engine
			QMM_RET_SUPERCEDE((intptr_t)ret);
		}

		// modents starts as a copy of mapents
		std::vector<ent_t> modents = mapents;

		// load global config
		QMM_WRITEQMMLOG(QMM_VARARGS("Loading global config for SubBSP entity list %d\n", index), QMMLOG_DEBUG, "STRIPPER");
		ent_load_config(modents, "qmmaddons/stripper/global.ini");

		// load map-specific config
		QMM_WRITEQMMLOG(QMM_VARARGS("Loading map-specific config for SubBSP entity list %d: %s\n", index, QMM_GETSTRCVAR("mapname")), QMMLOG_DEBUG, "STRIPPER");
		ent_load_config(modents, QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));

		// generate new entstring from modents to pass to mod
		const char* entstring = ents_generate_entstring(modents);

		// add "_subbsp_index=index" keyval to each entity in mapents and modents to provide
		// some extra information in stripper_dump files
		std::string strindex = std::to_string(index);
		for (auto& ent : mapents)
			ent.keyvals["_subbsp_index"] = strindex;
		for (auto& ent : modents)
			ent.keyvals["_subbsp_index"] = strindex;

		// add these temp mapents and modents to the global ones (so stripper_dump shows all)
		g_mapents.insert(g_mapents.end(), mapents.begin(), mapents.end());
		g_modents.insert(g_modents.end(), modents.begin(), modents.end());

		QMM_WRITEQMMLOG(QMM_VARARGS("Completed parsing SubBSP entity list %d\n", index), QMMLOG_INFO, "STRIPPER");

		QMM_RET_SUPERCEDE((intptr_t)entstring);
	}

#endif // GAME_JASP

	QMM_RET_IGNORED(1);
}


C_DLLEXPORT intptr_t QMM_vmMain_Post(intptr_t cmd, intptr_t* args) {
#if defined(GAME_JAMP)

	// mod init is done, tag all the subbsp lists and add them to g_mapents and g_modents for stripper_dump
	if (cmd == GAME_INIT) {
		// add "_subbsp_index=index" keyval to each entity in mapents and modents to provide
		// some extra information in stripper_dump files. these are ignored by the game
		for (auto& list : s_subbsp_mapents) {
			std::string strindex = std::to_string(list.first);
			for (auto& ent : list.second)
				ent.keyvals["_subbsp_index"] = strindex;

			// add this list to the global one
			g_mapents.insert(g_mapents.end(), list.second.begin(), list.second.end());
		}
		for (auto& list : s_subbsp_modents) {
			std::string strindex = std::to_string(list.first);
			for (auto& ent : list.second)
				ent.keyvals["_subbsp_index"] = strindex;

			// add this list to the global one
			g_mapents.insert(g_modents.end(), list.second.begin(), list.second.end());
		}
	}

#endif // GAME_JAMP

	return 0;
}


C_DLLEXPORT intptr_t QMM_syscall_Post(intptr_t cmd, intptr_t* args) {
#if defined(GAME_JAMP)

	/* Jedi Academy multiplayer has a SetActiveSubBSP engine function where a "misc_bsp" map entity can have the
	   engine load an entire map and load it into another map. This then triggers another round of
	   G_GET_ENTITY_TOKEN which we must handle. So all we need to do here is call the engine's G_GET_ENTITY_TOKEN
	   repeatedly to load more entities, store them in a map keyed with the unique subbsp index argument, then
	   read from that specific entity list in G_GET_ENTITY_TOKEN. Once G_SET_ACTIVE_SUBBSP is called with -1,
	   then we can continue passing the normal entity list in G_GET_ENTITY_TOKEN.
	*/
	// hook this in post so that the engine has already set up what it needs to regarding subbsp stuff
	if (cmd == G_SET_ACTIVE_SUBBSP) {
		subbsp_index = args[0];
		// do nothing if the mod is done loading subbsp entity info
		if (subbsp_index < 0)
			return 0;

		// get the entities from the engine and save to mapents
		std::vector<ent_t> mapents;
		ents_load_tokens(mapents);

		// modents starts as a copy of mapents
		std::vector<ent_t> modents = mapents;

		// load global config
		QMM_WRITEQMMLOG(QMM_VARARGS("Loading global config for SubBSP entity list %d\n", subbsp_index), QMMLOG_DEBUG, "STRIPPER");
		ent_load_config(modents, "qmmaddons/stripper/global.ini");

		// load map-specific config
		QMM_WRITEQMMLOG(QMM_VARARGS("Loading map-specific config for SubBSP entity list %d: %s\n", subbsp_index, QMM_GETSTRCVAR("mapname")), QMMLOG_DEBUG, "STRIPPER");
		ent_load_config(modents, QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));

		// store entity lists for later reading in G_GET_ENTITY_TOKEN hook
		s_subbsp_mapents[subbsp_index] = mapents;
		s_subbsp_modents[subbsp_index] = modents;

		QMM_WRITEQMMLOG(QMM_VARARGS("Completed parsing SubBSP entity list %d\n", subbsp_index), QMMLOG_INFO, "STRIPPER");
	}

#endif // GAME_JAMP

	return 0;
}


// handle retrieving map entities, loading stripper configs, and modifying entities
static bool s_load_and_modify_ents() {
	// some games can load new maps without unloading the mod DLL
	g_mapents.clear();

	// get all the entity tokens from the engine and save to g_mapents
	QMM_WRITEQMMLOG("Parsing entity list\n", QMMLOG_DEBUG, "STRIPPER");
	ents_load_tokens(g_mapents);

	// check for valid entity list
	if (g_mapents.empty()) {
		QMM_WRITEQMMLOG("Empty ent list from engine - possibly a trailer/menu?\n", QMMLOG_DEBUG, "STRIPPER");
		return false;
	}

	// g_modents starts as a copy of g_mapents
	g_modents = g_mapents;

	// load global config
	QMM_WRITEQMMLOG("Loading global config\n", QMMLOG_DEBUG, "STRIPPER");
	ent_load_config(g_modents, "qmmaddons/stripper/global.ini");

	// load map-specific config
	QMM_WRITEQMMLOG(QMM_VARARGS("Loading map-specific config: %s\n", QMM_GETSTRCVAR("mapname")), QMMLOG_DEBUG, "STRIPPER");
	ent_load_config(g_modents, QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));

	return true;
}

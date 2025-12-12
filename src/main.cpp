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

#if defined(GAME_JAMP) || defined(GAME_JASP)
// store mapents/modents lists per subbsp index
static std::map<intptr_t, MapEntities> s_subbsp_mapents;
static std::map<intptr_t, MapEntities> s_subbsp_modents;
// store active subbsp index
static int subbsp_index = -1;
#endif // GAME_JAMP || GAME_JASP


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
			const char* mapfile = QMM_VARARGS("qmmaddons/stripper/dumps/%s.txt", QMM_GETSTRCVAR("mapname"));
			const char* modfile = QMM_VARARGS("qmmaddons/stripper/dumps/%s_modents.txt", QMM_GETSTRCVAR("mapname"));
			g_mapents.dump_to_file(mapfile);
			g_modents.dump_to_file(modfile);

#if defined(GAME_JAMP) || defined(GAME_JASP)
			// we also need to print out the subbsp lists (true = append)
			for (auto& subbsp : s_subbsp_mapents)
				subbsp.second.dump_to_file(mapfile, true);
			for (auto& subbsp : s_subbsp_modents)
				subbsp.second.dump_to_file(modfile, true);
#endif
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
			const char* entstring = g_modents.get_entstring().c_str();

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
	// loop through the ent list and return a single token
	if (cmd == G_GET_ENTITY_TOKEN) {
		char* entity = (char*)args[0];
		intptr_t length = args[1];
		MapEntities& entlist = 
#if defined(GAME_JAMP)
			(subbsp_index >= 0) ? s_subbsp_modents[subbsp_index] :
#endif
			g_modents;
		intptr_t ret = entlist.get_next_token(entity, length);
		// don't pass this to engine since we already pulled all entities from the engine
		QMM_RET_SUPERCEDE(ret);
	}
#endif // !GAME_HAS_SPAWNENTS

#if defined(GAME_JAMP) || defined(GAME_JASP)
	/* Jedi Academy has a feature that allows a "misc_bsp" entity in a map to basically cause another map to be
	   loaded in-place. When a mod encounters the misc_bsp entity, it should call the engine's SetActiveSubBSP
	   function with a unique ID that is part of the entity. This lets the engine give the mod the SubBSP's entity
	   info.
	   In multiplayer (JAMP), this call simply allows the mod to then call G_GET_ENTITY_TOKEN again (like in Init)
	   in order to get the SubBSP entity info. 
	   In singleplayer (JASP), this call will return the SubBSP entity info as an entstring (like in Init).
	   In both, once the mod has loaded all the new SubBSP entities, the mod calls SetActiveSubBSP(-1).
	   
	   Here, we store the given index, and then exit if it's -1.
	   Then we get the entity info, apply configs, and save for later processing/dumping.
	*/
	if (cmd == G_SET_ACTIVE_SUBBSP) {
		subbsp_index = args[0];
		// -1 is used to tell the engine that the mod is done with the SubBSP
		// we will just let this pass through directly to the engine
		if (subbsp_index < 0)
			QMM_RET_IGNORED(0);
		QMM_WRITEQMMLOG(QMM_VARARGS("Parsing SubBSP entity list %d\n", subbsp_index), QMMLOG_INFO, "STRIPPER");

		// get the entities from the engine and save to mapents
		MapEntities mapents;
		// since we will supercede this call, we need to call the engine function ourselves
		const char* ret = (const char*)g_syscall(G_SET_ACTIVE_SUBBSP, subbsp_index);
#if defined(GAME_JAMP)
		// load entities from G_GET_ENTITY_TOKEN
		mapents.make_from_engine();
#elif defined(GAME_JASP)
		// load entities from entstring returned from engine
		mapents.make_from_entstring(ret);
#endif // GAME_JAMP / GAME_JASP

		// check for valid entity list
		if (mapents.get_entlist().empty()) {
			QMM_WRITEQMMLOG(QMM_VARARGS("Empty SubBSP entity list %d from engine\n", subbsp_index), QMMLOG_DEBUG, "STRIPPER");
			// return whatever the engine did (void in JAMP, so irrelevant)
			QMM_RET_SUPERCEDE((intptr_t)ret);
		}

		// modents starts as a copy of mapents
		MapEntities modents = mapents;

		// load global config
		QMM_WRITEQMMLOG(QMM_VARARGS("Loading global config for SubBSP entity list %d\n", subbsp_index), QMMLOG_DEBUG, "STRIPPER");
		modents.apply_config("qmmaddons/stripper/global.ini");

		// load map-specific config
		QMM_WRITEQMMLOG(QMM_VARARGS("Loading map-specific config for SubBSP entity list %d: %s\n", subbsp_index, QMM_GETSTRCVAR("mapname")), QMMLOG_DEBUG, "STRIPPER");
		modents.apply_config(QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));

		// store these ent lists in subbsp table
		s_subbsp_mapents[subbsp_index] = mapents;
		s_subbsp_modents[subbsp_index] = modents;

		QMM_WRITEQMMLOG(QMM_VARARGS("Completed parsing SubBSP entity list %d\n", subbsp_index), QMMLOG_INFO, "STRIPPER");

		// generate new entstring from modents to pass to mod
		// this is fine even in JAMP since trap_SetActiveSubBSP is void so return value is ignored
		static EntString entstring;
		entstring = modents.get_entstring().c_str();
		QMM_RET_SUPERCEDE((intptr_t)entstring.c_str());
	}
#endif // GAME_JAMP || GAME_JASP

	QMM_RET_IGNORED(1);
}


C_DLLEXPORT intptr_t QMM_vmMain_Post(intptr_t cmd, intptr_t* args) {

#if defined(GAME_JAMP) || defined(GAME_JASP)
	// mod init is done, tag all the subbsp lists for stripper_dump
	if (cmd == GAME_INIT) {
		// add "_subbsp_index=index" keyval to each entity in mapents and modents to provide
		// some extra information in stripper_dump files. this is after the mod receives all
		// the entity info, but the mod should ignore unknown fields anyway so it's fine.
		for (auto& list : s_subbsp_mapents) {
			std::string strindex = std::to_string(list.first);
			list.second.add_keyval("_subbsp_index", strindex);
		}
		for (auto& list : s_subbsp_modents) {
			std::string strindex = std::to_string(list.first);
			list.second.add_keyval("_subbsp_index", strindex);
		}
	}
#endif // GAME_JAMP || GAME_JASP

	return 0;
}


C_DLLEXPORT intptr_t QMM_syscall_Post(intptr_t cmd, intptr_t* args) {
	return 0;
}


// handle retrieving map entities, loading stripper configs, and modifying entities
static bool s_load_and_modify_ents() {
	// some games can load new maps without unloading the mod DLL
	g_mapents = {};

	// get all the entity tokens from the engine and save to g_mapents
	QMM_WRITEQMMLOG("Parsing entity list\n", QMMLOG_DEBUG, "STRIPPER");
	g_mapents.make_from_engine();

	// check for valid entity list
	if (g_mapents.get_entlist().empty()) {
		QMM_WRITEQMMLOG("Empty ent list from engine - possibly a trailer/menu?\n", QMMLOG_DEBUG, "STRIPPER");
		return false;
	}

	// g_modents starts as a copy of g_mapents
	g_modents = g_mapents;

	// load global config
	QMM_WRITEQMMLOG("Loading global config\n", QMMLOG_DEBUG, "STRIPPER");
	g_modents.apply_config("qmmaddons/stripper/global.ini");

	// load map-specific config
	QMM_WRITEQMMLOG(QMM_VARARGS("Loading map-specific config: %s\n", QMM_GETSTRCVAR("mapname")), QMMLOG_DEBUG, "STRIPPER");
	g_modents.apply_config(QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));

	return true;
}

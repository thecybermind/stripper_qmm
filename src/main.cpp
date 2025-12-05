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


C_DLLEXPORT intptr_t QMM_vmMain(intptr_t cmd, intptr_t* args) {
	if (cmd == GAME_INIT) {
		// init msg
		QMM_WRITEQMMLOG(QMM_VARARGS("Stripper v" STRIPPER_QMM_VERSION " (%s) by " STRIPPER_QMM_BUILDER " is loaded\n", QMM_GETGAMEENGINE()), QMMLOG_INFO, "STRIPPER");
		// register cvar
		g_syscall(G_CVAR_REGISTER, nullptr, "stripper_version", STRIPPER_QMM_VERSION, CVAR_ROM | CVAR_SERVERINFO | CVAR_NORESTART);
		g_syscall(G_CVAR_SET, "stripper_version", STRIPPER_QMM_VERSION);

// games without a GAME_SPAWN_ENTITIES msg get entities and load configs here during QMM_vmMain(GAME_INIT).
// entities are passed to the mod with the QMM_syscall(G_GET_ENTITY_TOKEN) hook
#if !defined(GAME_HAS_SPAWNENTS)
		// some games can load new maps without unloading the mod
		g_mapents.clear();

		// get all the entity tokens from the engine with G_GET_ENTITY_TOKEN and save to lists
		ents_load_tokens(g_mapents);

		// g_modents starts as a copy of g_mapents
		g_modents = g_mapents;

		// load global config
		ent_load_config("qmmaddons/stripper/global.ini");

		// load map-specific config
		ent_load_config(QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));
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

	// games with a GAME_SPAWN_ENTITIES msg load configs here during QMM_vmMain(GAME_SPAWN_ENTITIES).
	// entities are passed to the mod by changing the entstring parameter.
	// QMM also gets the entities and stores them for returning in a G_GET_ENTITY_TOKEN polyfill
#if defined(GAME_HAS_SPAWNENTS)
	// moh??:   void (*SpawnEntities)(char *entstring, int levelTime);
	// stef2:   void (*SpawnEntities)(const char *mapname, const char *entstring, int levelTime);
	// quake2:  void (*SpawnEntities)(const char *mapname, const char *entstring, const char *spawnpoint);
	// q2r:     void (*SpawnEntities)(const char *mapname, const char *entstring, const char *spawnpoint);
	// jk2sp:   void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
	// jasp:    void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
	// stvoysp: void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);

	// specifically not "else if" since in JK2SP/JASP/STVOYSP, GAME_SPAWN_ENTITIES is really just an alias for GAME_INIT
	if (cmd == GAME_SPAWN_ENTITIES) {
		// some games can load new maps without unloading the mod DLL
		g_mapents.clear();

		// get all the entity tokens from the engine and save to g_mapents
		ents_load_tokens(g_mapents);

		// g_modents starts as a copy of g_mapents
		g_modents = g_mapents;

		// load global config
		ent_load_config("qmmaddons/stripper/global.ini");

		// load map-specific config
		ent_load_config(QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));

		// generate new entstring from g_modents to pass to mod
		const char* entstring = ents_generate_entstring(g_modents);

#if defined(GAME_STEF2) || defined(GAME_Q2R) || defined(GAME_QUAKE2)
		int entarg = 1;
#elif defined(GAME_JK2SP) || defined(GAME_JASP) || defined(GAME_STVOYSP)
		int entarg = 3;
#elif defined(GAME_MOHAA) || defined(GAME_MOHSH) || defined(GAME_MOHBT)
		int entarg = 0;
#else
		int entarg = 0;
#endif

		// replace entstring arg for passing to mod
		args[entarg] = (intptr_t)entstring;
	}
#endif

	QMM_RET_IGNORED(1);
}


#if defined(GAME_HAS_SUBBSP)
static int insubbsp = 0;
#endif // GAME_HAS_SUBBSP

C_DLLEXPORT intptr_t QMM_syscall(intptr_t cmd, intptr_t* args) {
#if !defined(GAME_HAS_SPAWNENTS)
	// loop through the ent list and return a single token
	if (cmd == G_GET_ENTITY_TOKEN
 #if defined(GAME_HAS_SUBBSP)
		// if this is JAMP/JASP and we are in a sub bsp, don't handle this and let the engine handle it
		&& !insubbsp
 #endif // GAME_HAS_SUBBSP
		) {
		char* entity = (char*)args[0];
		intptr_t length = (intptr_t)args[1];
		intptr_t ret = ent_next_token(entity, length);
		// don't pass this to engine since we already pulled all entities from the engine
		QMM_RET_SUPERCEDE(ret);
	}
#endif
	QMM_RET_IGNORED(1);
}


C_DLLEXPORT intptr_t QMM_vmMain_Post(intptr_t cmd, intptr_t* args) {
	QMM_RET_IGNORED(1);
}


C_DLLEXPORT intptr_t QMM_syscall_Post(intptr_t cmd, intptr_t* args) {
#if defined(GAME_HAS_SUBBSP)
	/* Jedi Academy has a feature where a "misc_bsp" map entity can have the engine load an entire map and load it into another map.
	   This then triggers another round of G_GET_ENTITY_TOKEN which we must handle.
	   The mod tells the engine that it should load a map with the trap_SetActiveSubBSP function:

	      void trap_SetActiveSubBSP(int index)

	   where 'index' is a unique value for each sub bsp to be loaded. It represents the index of the first model from this sub bsp
	   in the global models list, it retrieves this from the misc_bsp entity itself before calling G_GET_ENTITY_TOKEN again:

	      trap_SetActiveSubBSP(ent->s.modelindex);
		  G_SpawnEntitiesFromString(qtrue);
		  trap_SetActiveSubBSP(-1);

	   For now, until we can refactor the code to better handle this, we will just store whether or not we are working with a
	   sub bsp and if so, just pass G_GET_ENTITY_TOKEN calls onto the engine directly.

	   In the future, we should check for misc_bsp entities during ents_load_tokens and pull the entities ourselves, storing them
	   in other lists per misc_bsp entity.
	*/
	if (cmd == G_SET_ACTIVE_SUBBSP) {
		insubbsp = (args[0] != -1);
	}
#endif // GAME_HAS_SUBBSP

	QMM_RET_IGNORED(1);
}

/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

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
	"STRIPPER",										// log tag
};
eng_syscall_t g_syscall = nullptr;
mod_vmMain_t g_vmMain = nullptr;
pluginfuncs_t* g_pluginfuncs = nullptr;
pluginvars_t* g_pluginvars = nullptr;


std::string mapname;


C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo) {
	QMM_GIVE_PINFO();
}


C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc, pluginres_t* presult, pluginfuncs_t* pluginfuncs, pluginvars_t* pluginvars) {
	QMM_SAVE_VARS();
	return 1;
}


C_DLLEXPORT void QMM_Detach() {
}


// flag to disable if another stripper plugin is loaded for some reason
bool s_disabled = false;

// this stores all the ents loaded from the map
// we save this so we can dump them to file if needed
MapEntities s_mapents;
// this stores all the ents that should be passed to the mod (s_mapents +/- modifications)
MapEntities s_modents;


// handle retrieving map entities, loading stripper configs, and modifying entities for normal Init/SpawnEntities mod loading
static bool s_load_and_modify_ents();

// these next vars are for JASP/JAMP, but leaving them available to all games makes code a bit cleaner in some places
// store mapents/modents lists per subbsp index
static std::map<intptr_t, MapEntities> s_subbsp_mapents;
static std::map<intptr_t, MapEntities> s_subbsp_modents;
// store active subbsp index
static int s_subbsp_index = -1;


C_DLLEXPORT intptr_t QMM_vmMain(intptr_t cmd, intptr_t* args) {
	if (s_disabled)
		QMM_RET_IGNORED(0);

	if (cmd == GAME_INIT) {
		// broadcast our version so other stripper plugins may disable themselves
		QMM_PLUGIN_BROADCAST(PLID, STRIPPER_QMM_BROADCAST_STR, nullptr, STRIPPER_QMM_VERSION_INT);
		// if this resulted in us being disabled, cancel init
		if (s_disabled)
			QMM_RET_IGNORED(0);

		// init msg
		QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Stripper v" STRIPPER_QMM_VERSION " (%s) by " STRIPPER_QMM_BUILDER " is loaded\n", QMM_GETGAMEENGINE(PLID)), QMMLOG_NOTICE);
		// register cvar
		g_syscall(G_CVAR_REGISTER, nullptr, "stripper_version", STRIPPER_QMM_VERSION, CVAR_ROM | CVAR_SERVERINFO | CVAR_NORESTART);
		g_syscall(G_CVAR_SET, "stripper_version", STRIPPER_QMM_VERSION);

		mapname = QMM_GETSTRCVAR(PLID, "mapname");

#if !defined(GAME_HAS_SPAWN_ENTITIES)
		// games without a GAME_SPAWN_ENTITIES msg get entities and load configs here during QMM_vmMain(GAME_INIT).
		// entities are passed to the mod with the QMM_syscall(G_GET_ENTITY_TOKEN) hook

		s_load_and_modify_ents();

		QMM_WRITEQMMLOG(PLID, "Stripper loading complete.\n", QMMLOG_NOTICE);
#endif // !GAME_HAS_SPAWN_ENTITIES

	}
	// handle stripper_dump command
	else if (cmd == GAME_CONSOLE_COMMAND) {
		char buf[20];

		QMM_ARGV(PLID, 0, buf, sizeof(buf));
		
		// if command is "sv", then check the next arg
		if (str_striequal(buf, "sv"))
			QMM_ARGV(PLID, 1, buf, sizeof(buf));

		if (str_striequal(buf, "stripper_dump")) {
			std::string mapfile = QMM_VARARGS(PLID, "qmmaddons/stripper/dumps/%s.txt", mapname.c_str());
			std::string modfile = QMM_VARARGS(PLID, "qmmaddons/stripper/dumps/%s_modents.txt", mapname.c_str());

			s_mapents.dump_to_file(mapfile);
			s_modents.dump_to_file(modfile);

			// we also need to print out the subbsp lists (true = append)
			for (auto& subbsp : s_subbsp_mapents)
				subbsp.second.dump_to_file(mapfile, true);
			for (auto& subbsp : s_subbsp_modents)
				subbsp.second.dump_to_file(modfile, true);

			// don't pass this to mod since we handled the command
			QMM_RET_SUPERCEDE(1);
		}
	}

#if defined(GAME_HAS_SPAWN_ENTITIES)

	// games with a GAME_SPAWN_ENTITIES msg load configs here during QMM_vmMain(GAME_SPAWN_ENTITIES).
	// entities are passed to the mod by changing the entstring parameter.

	// specifically not "else if" since in JK2SP/JASP/STVOYSP, GAME_SPAWN_ENTITIES is really just an alias for GAME_INIT
	if (cmd == GAME_SPAWN_ENTITIES) {
		// moh??:   void (*SpawnEntities)(char *entstring, int levelTime);
		// stef2:   void (*SpawnEntities)(const char *mapname, const char *entstring, int levelTime);
		// sin:		void (*SpawnEntities)(const char *mapname, const char *entstring, const char *spawnpoint);
		// quake2:  void (*SpawnEntities)(char* mapname, char* entstring, char* spawnpoint);
		// q2r:     void (*SpawnEntities)(char* mapname, char* entstring, char* spawnpoint);
		// jk2sp:   void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
		// jasp:    void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
		// stvoysp: void (*Init)(const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition);
#if defined(GAME_STEF2) || defined(GAME_Q2R) || defined(GAME_QUAKE2) || defined(GAME_SIN)
		int entarg = 1;
		mapname = (char*)args[0];
#elif defined(GAME_JK2SP) || defined(GAME_JASP) || defined(GAME_STVOYSP)
		int entarg = 3;
		mapname = (char*)args[0];
#elif defined(GAME_MOHAA) || defined(GAME_MOHSH) || defined(GAME_MOHBT)
		int entarg = 0;
		mapname = QMM_GETSTRCVAR(PLID, "mapname");
#else
		int entarg = 0;
		mapname = QMM_GETSTRCVAR(PLID, "mapname");
#endif

		if (s_load_and_modify_ents()) {
			// generate new entstring from s_modents to pass to mod
			const char* entstring = s_modents.get_entstring().c_str();

			// replace entstring arg for passing to mod
			args[entarg] = (intptr_t)entstring;
		}

		QMM_WRITEQMMLOG(PLID, "Stripper loading complete.\n", QMMLOG_NOTICE);
	}
#endif

	QMM_RET_IGNORED(0);
}


C_DLLEXPORT intptr_t QMM_syscall(intptr_t cmd, intptr_t* args) {
	if (s_disabled)
		QMM_RET_IGNORED(0);

	// this section won't actually trigger in GAME_HAS_SPAWN_ENTITIES games
	// but we leave it to handle warnings for unused 's_subbsp_index'

	// return next entity token back to the game
	if (cmd == G_GET_ENTITY_TOKEN) {
		char* entity = (char*)args[0];
		intptr_t length = args[1];
		MapEntities& entlist = (s_subbsp_index >= 0) ? s_subbsp_modents[s_subbsp_index] : s_modents;
		intptr_t ret = entlist.get_next_token(entity, length);

		if (ret && s_subbsp_index >= 0)
			QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "G_GET_ENTITY_TOKEN: Passing SubBSP %d entity to mod: \"%s\"\n", s_subbsp_index, entity), QMMLOG_TRACE);
		else if (ret)
			QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "G_GET_ENTITY_TOKEN: Passing entity token to mod: \"%s\"\n", entity), QMMLOG_TRACE);
		else
			QMM_WRITEQMMLOG(PLID, "G_GET_ENTITY_TOKEN: No more entities to pass to mod\n", QMMLOG_TRACE);

		// don't pass this to engine since we already pulled all entities from the engine
		QMM_RET_SUPERCEDE(ret);
	}

#if defined(GAME_HAS_SUBBSP)
	/* Jedi Academy has a feature that allows a "misc_bsp" entity in a map to basically cause another map to be
	   loaded in-place. When a mod encounters the misc_bsp entity, it should call the engine's SetActiveSubBSP
	   function with a unique ID that is part of the entity. This lets the engine give the mod the SubBSP's entity
	   info.
	   In multiplayer (JAMP), this call simply allows the mod to then call G_GET_ENTITY_TOKEN again (like in Init)
	   in order to get the SubBSP entity info.
	   In singleplayer (JASP), this call will return the SubBSP entity info as an entstring (like in Init).
	   In both, once the mod has loaded all the new SubBSP entities, the mod calls SetActiveSubBSP(-1).
	   
	   Here, we store the given index (exit if it's -1), then we get the entity info, apply configs, and save for
	   later processing/dumping. The G_GET_ENTITY_TOKEN hook will check the stored index and load the appropriate
	   subbsp MapEntities object for passing to the mod.

	   Edit: SOF2MP also supports SubBSPs in the same manner as JAMP
	*/
	if (cmd == G_SET_ACTIVE_SUBBSP) {
		s_subbsp_index = args[0];
		// -1 is used to tell the engine that the mod is done with the SubBSP
		// we will just let this pass through directly to the engine
		if (s_subbsp_index < 0)
			QMM_RET_IGNORED(0);
		QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Parsing SubBSP entity list %d\n", s_subbsp_index), QMMLOG_DEBUG);

		// get the entities from the engine and save to mapents
		MapEntities mapents;
		// since we will supercede this call, we need to call the engine function ourselves
		const char* ret = (const char*)g_syscall(G_SET_ACTIVE_SUBBSP, s_subbsp_index);
#if defined(GAME_JAMP) || defined(SOF2MP)
		// load entities from G_GET_ENTITY_TOKEN
		mapents.make_from_engine();
#elif defined(GAME_JASP)
		// load entities from entstring returned from engine
		mapents.make_from_entstring(ret);
#else
		// ?
#endif

		// check for valid entity list
		if (mapents.get_entlist().empty()) {
			QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Empty SubBSP entity list %d from engine\n", s_subbsp_index), QMMLOG_DEBUG);
			// return whatever the engine did (void in JAMP, so irrelevant)
			QMM_RET_SUPERCEDE((intptr_t)ret);
		}

		QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "SubBSP entity list %d loaded, found %d entities\n", s_subbsp_index, mapents.get_entlist().size()), QMMLOG_DEBUG);

		// modents starts as a copy of mapents
		MapEntities modents = mapents;

		// load global config
		QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Loading global config for SubBSP entity list %d\n", s_subbsp_index), QMMLOG_DEBUG);
		modents.apply_config("qmmaddons/stripper/global.ini");

		// load map-specific config
		QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Loading map-specific config for SubBSP entity list %d: %s\n", s_subbsp_index, mapname.c_str()), QMMLOG_DEBUG);
		modents.apply_config(QMM_VARARGS(PLID, "qmmaddons/stripper/maps/%s.ini", mapname.c_str()));

		// store these ent lists in subbsp tables
		s_subbsp_mapents[s_subbsp_index] = mapents;
		s_subbsp_modents[s_subbsp_index] = modents;
		
		QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Completed parsing SubBSP entity list %d, passing %d entities to mod\n", s_subbsp_index, modents.get_entlist().size()), QMMLOG_DEBUG);

		// generate new entstring from modents to pass to mod
		// this is fine even in JAMP since trap_SetActiveSubBSP is void so return value is ignored
		static EntString entstring;
		entstring = modents.get_entstring();
		QMM_RET_SUPERCEDE((intptr_t)entstring.c_str());
	}
#endif // GAME_HAS_SUBBSP

	QMM_RET_IGNORED(0);
}


C_DLLEXPORT intptr_t QMM_vmMain_Post(intptr_t cmd, intptr_t* args) {
	if (s_disabled)
		QMM_RET_IGNORED(0);

#if defined(GAME_HAS_SUBBSP)
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
#endif // GAME_HAS_SUBBSP

	QMM_RET_IGNORED(0);
}


C_DLLEXPORT intptr_t QMM_syscall_Post(intptr_t cmd, intptr_t* args) {
	QMM_RET_IGNORED(0);
}


C_DLLEXPORT void QMM_PluginMessage(plid_t from_plid, const char* message, void* buf, intptr_t buflen) {
	if (s_disabled)
		return;

	// if this is a message from another stripper plugin
	if (!strcmp(message, STRIPPER_QMM_BROADCAST_STR) && !buf) {
		// if the passed version is greater or equal to ours, disable ourselves
		if (buflen >= STRIPPER_QMM_VERSION_INT) {
			QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Another stripper_qmm with version %X detected, our version is %X. This plugin is disabling itself.", buflen, STRIPPER_QMM_VERSION_INT), QMMLOG_WARNING);
			s_disabled = true;
		}
		// we have a higher version, so broadcast out our version and the other one will disable itself
		else {
			QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Another stripper_qmm with version %X detected, our version is %X. Broadcasting our version.", buflen, STRIPPER_QMM_VERSION_INT), QMMLOG_INFO);
			QMM_PLUGIN_BROADCAST(PLID, STRIPPER_QMM_BROADCAST_STR, nullptr, STRIPPER_QMM_VERSION_INT);
		}
	}
}


// handle retrieving map entities, loading stripper configs, and modifying entities
static bool s_load_and_modify_ents() {
	// some games can load new maps without unloading the mod DLL, so start fresh
	s_mapents = {};
	s_subbsp_mapents.clear();
	s_subbsp_modents.clear();

	// get all the entity tokens from the engine and save to s_mapents
	QMM_WRITEQMMLOG(PLID, "Parsing entity list\n", QMMLOG_DEBUG);
	// QMM has a polyfill for G_GET_ENTITY_TOKEN in games where an entstring is passed to Init/SpawnEntities,
	// so we can just use it for all games. subbsp stuff will be handled specially.
	s_mapents.make_from_engine();

	// check for valid entity list
	if (s_mapents.get_entlist().empty()) {
		QMM_WRITEQMMLOG(PLID, "Empty entity list from engine - possibly a trailer/menu?\n", QMMLOG_DEBUG);
		return false;
	}

	QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Entity list loaded, found %d entities\n", s_mapents.get_entlist().size()), QMMLOG_DEBUG);

	// modents starts as a copy of mapents
	s_modents = s_mapents;

	// load global config
	QMM_WRITEQMMLOG(PLID, "Loading global config\n", QMMLOG_INFO);
	s_modents.apply_config("qmmaddons/stripper/global.ini");

	// load map-specific config
	QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Loading map-specific config: %s\n", mapname.c_str()), QMMLOG_INFO);
	s_modents.apply_config(QMM_VARARGS(PLID, "qmmaddons/stripper/maps/%s.ini", mapname.c_str()));
		

	QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Completed parsing entity list, passing %d entities to mod\n", s_modents.get_entlist().size()), QMMLOG_NOTICE);

	return true;
}

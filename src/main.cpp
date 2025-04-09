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
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "ent.h"

pluginres_t* g_result = NULL;
plugininfo_t g_plugininfo = {
	"Stripper",							// name of plugin
	STRIPPER_QMM_VERSION,				// version of plugin
	"Change map entities during load",	// description of plugin
	STRIPPER_QMM_BUILDER,				// author of plugin
	"http://www.q3mm.org/",				// website of plugin
	0,									// reserved
	0,									// reserved
	0,									// reserved
	QMM_PIFV_MAJOR,						// plugin interface version major
	QMM_PIFV_MINOR						// plugin interface version minor
};
eng_syscall_t g_syscall = NULL;
mod_vmMain_t g_vmMain = NULL;
pluginfuncs_t* g_pluginfuncs = NULL;
intptr_t g_vmbase = 0;
pluginvars_t* g_pluginvars = NULL;

C_DLLEXPORT void QMM_Query(plugininfo_t** pinfo) {
	QMM_GIVE_PINFO();
}

C_DLLEXPORT int QMM_Attach(eng_syscall_t engfunc, mod_vmMain_t modfunc, pluginres_t* presult, pluginfuncs_t* pluginfuncs, intptr_t vmbase, pluginvars_t* pluginvars) {
	QMM_SAVE_VARS();

	return 1;
}

C_DLLEXPORT void QMM_Detach(intptr_t reserved) {
	g_mapents.empty();

	reserved = 0;
}

C_DLLEXPORT intptr_t QMM_vmMain(intptr_t cmd, intptr_t* args) {
	if (cmd == GAME_INIT) {
		//init msg
		QMM_WRITEQMMLOG("Stripper v" STRIPPER_QMM_VERSION " by " STRIPPER_QMM_BUILDER " is loaded\n", QMMLOG_INFO, "STRIPPER_QMM");

		//register cvar
		g_syscall(G_CVAR_REGISTER, NULL, "stripper_version", STRIPPER_QMM_VERSION, CVAR_ROM | CVAR_SERVERINFO | CVAR_NORESTART);
		g_syscall(G_CVAR_SET, "stripper_version", STRIPPER_QMM_VERSION);

#if !defined(GAME_STEF2) && !defined(GAME_MOHAA) && !defined(GAME_MOHSH) && !defined(GAME_MOHBT)
		//get all the entity tokens from the engine and save to lists
		get_entity_tokens();

		//load global config
		load_config("qmmaddons/stripper/global.ini");

		//load map-specific config
		load_config(QMM_VARARGS("qmmaddons/stripper/maps/%s.ini", QMM_GETSTRCVAR("mapname")));
#endif
	}
	else if (cmd == GAME_CONSOLE_COMMAND) {
		char buf[20];
		QMM_ARGV(0, buf, sizeof(buf));
		if (!strcmp(buf, "stripper_dump")) {
			dump_ents();
			QMM_RET_SUPERCEDE(1);
		}
	}
#if defined(GAME_STEF2) || defined(GAME_MOHAA) || defined(GAME_MOHSH) || defined(GAME_MOHBT)
	// mohaa: void (*SpawnEntities)(char *entstring, int levelTime);
	// stef2: void (*SpawnEntities)(const char *mapname, const char *entstring, int levelTime);
	else if (cmd == GAME_SPAWN_ENTITIES) {
		char* entstring = (char*)
#ifdef GAME_STEF2
		args[1];
#else
		args[0];
#endif
		QMM_WRITEQMMLOG(".\n", QMMLOG_INFO, "STRIPPER_QMM");
		QMM_WRITEQMMLOG(".\n", QMMLOG_INFO, "STRIPPER_QMM");
		QMM_WRITEQMMLOG("Entstring: ", QMMLOG_INFO, "STRIPPER_QMM");
		QMM_WRITEQMMLOG(entstring, QMMLOG_INFO, "STRIPPER_QMM");
		QMM_WRITEQMMLOG("\n", QMMLOG_INFO, "STRIPPER_QMM");
		QMM_WRITEQMMLOG(".\n", QMMLOG_INFO, "STRIPPER_QMM");
		QMM_WRITEQMMLOG(".\n", QMMLOG_INFO, "STRIPPER_QMM");
	}
#endif

	QMM_RET_IGNORED(1);
}

#ifdef GAME_JAMP
static int insubbsp = 0;
#endif // GAME_JAMP

C_DLLEXPORT intptr_t QMM_syscall(intptr_t cmd, intptr_t* args) {
#if defined(GAME_Q3A) || defined(GAME_RTCWMP) || defined(GAME_RTCWSP) || defined(GAME_JK2MP) || defined(GAME_JAMP) || defined(GAME_STVOYHM) || defined(GAME_WET)
	// loop through the ent list and return a single token
	if (cmd == G_GET_ENTITY_TOKEN
#ifdef GAME_JAMP
		&& !insubbsp
#endif
		) {
		char* entity = (char*)args[0];
		int length = (int)args[1];
		QMM_RET_SUPERCEDE(get_next_entity_token(entity, length));
	}
#endif // dllEntry games
	QMM_RET_IGNORED(1);
}

C_DLLEXPORT intptr_t QMM_vmMain_Post(intptr_t cmd, intptr_t* args) {
	QMM_RET_IGNORED(1);
}

C_DLLEXPORT intptr_t QMM_syscall_Post(intptr_t cmd, intptr_t* args) {
#ifdef GAME_JAMP
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
	   sub bsp and if so, just pass G_GET_ENTITY_TOKEN calls onto the engine directly
	*/
	if (cmd == G_SET_ACTIVE_SUBBSP) {
		insubbsp = (args[0] != -1);
	}
#endif // GAME_JAMP

	QMM_RET_IGNORED(1);
}

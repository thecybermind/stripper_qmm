/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#define _CRT_SECURE_NO_WARNINGS 1
#include "version.h"
#include <qmmapi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "game.h"
#include "ent.h"
#include "util.h"

// returns true if "test" has all the same keyvals that "contains" has
static bool s_ent_match(ent_t& test, ent_t& contains);
// removes all matching entities from list
static void s_ents_filter(std::vector<ent_t>& list, ent_t& filterent);
// adds an entity to list (puts worldspawn at the beginning)
static void s_ents_add(std::vector<ent_t>& list, ent_t& addent);
// finds all entities in list matching all stored replaceents and replaces with a withent
static void s_ents_replace(std::vector<ent_t>& list, ent_t& withent);
// replaces all applicable keyvals on an ent
static void s_ent_replace(ent_t& replaceent, ent_t& withent);


// this stores all the ents loaded from the map
// we save this so we can dump them to file if needed
std::vector<ent_t> g_mapents;

// this stores all the ents that should be passed to the mod (g_mapents +/- modifications)
std::vector<ent_t> g_modents;

// this stores all info for entities that should be replaced
// nodes are read and removed from this list when a "with" entity is found
std::vector<ent_t> g_replaceents;


#if defined(GAME_HAS_SPAWNENTS)
// generate an entstring to pass to the mod
const char* ents_generate_entstring(std::vector<ent_t>& list) {
	static std::string str;
	str = "";

	for (auto& ent : list) {
		str += "{\n";
		for (auto& keyval : ent.keyvals) {
			str += ("\"" + keyval.first + "\" \"" + keyval.second + "\"\n");
		}
		str += "}\n";
	}

	return str.c_str();
}


#else


// passes the next entity token to the mod
intptr_t ent_next_token(char* buf, intptr_t len) {
	// iterator for current ent
	static std::vector<ent_t>::iterator it_ent = g_modents.begin();

	// iterator for current keyval
	static decltype(ent_t::keyvals)::iterator it_keyval;

	// keep track of token status
	static bool inside_ent = false;	// false = between ents, true = inside an ent
	static bool is_key = true;		// true = expecting key, false = expecting val

	// if we have sent all the entities, return an EOF
	if (it_ent == g_modents.end())
		return 0;
	
	// if we are starting a new entity, send a {
	if (!inside_ent) {
		inside_ent = true;
		is_key = true;
		it_keyval = it_ent->keyvals.begin();
		strncpy(buf, "{", len);
	}
	// if we are inside an ent
	else {
		// if a keyval exists
		if (it_keyval != it_ent->keyvals.end()) {
			// if we need to send a key
			if (is_key) {
				is_key = false;
				strncpy(buf, it_keyval->first.c_str(), len);
			}
			// if we need to send a val
			else {
				is_key = true;
				strncpy(buf, it_keyval->second.c_str(), len);
				++it_keyval;
			}
		}
		// if no more keyval exists, send a }
		else {
			inside_ent = false;
			++it_ent;
			strncpy(buf, "}", len);
		}
	}
	
	return 1;
}
#endif // GAME_HAS_SPAWNENTS


// gets all the entity tokens from the engine and stores them in a list
void ents_load_tokens(std::vector<ent_t>& list) {
	// the current ent
	ent_t ent;
	// store key. when a val is received, make a new entry into ent
	std::string key;

	char buf[MAX_TOKEN_CHARS];

	bool inside_ent = false;	// false = between ents, true = inside an ent
	bool is_key = true;			// true = expecting key, false = expecting val

	// loop through all tokens from engine
	while (1) {
		// get token, check for EOF
		if (!g_syscall(G_GET_ENTITY_TOKEN, buf, sizeof(buf)))
			break;

		// got an opening brace while already inside an entity, error
		if (buf[0] == '{' && inside_ent)
			break;

		// got a closing brace when not inside an entity, error
		if (buf[0] == '}' && !inside_ent)
			break;

		// if this is a closing brace when expecting a val, error
		if (buf[0] == '}' && !is_key)
			break;

		// if this is a valid closing brace, save ent to list and continue
		if (buf[0] == '}') {
			inside_ent = false;
			key = "";
			list.push_back(ent);
			ent = {};
			continue;
		}

		// if this is a valid opening brace, start a new ent
		if (buf[0] == '{') {
			inside_ent = true;
			is_key = true;
			key = "";
			ent = {};
			continue;
		}

		// this is a key
		if (is_key) {
			is_key = false;
			key = buf;
		}
		// this is a val
		else {
			is_key = true;
			std::string val = buf;
			// store keyval in ent
			ent.keyvals[key] = val;
			// store classname for easier lookup
			if (key == "classname")
				ent.classname = val;
		}
	} // while(1)
}


// outputs ent list to a file
void ents_dump_to_file(std::vector<ent_t>& list, std::string file) {
	fileHandle_t f;
	if (g_syscall(G_FS_FOPEN_FILE, file.c_str(), &f, FS_WRITE) < 0) {
		QMM_WRITEQMMLOG(QMM_VARARGS("Unable to write ent dump to %s\n", file.c_str()), QMMLOG_INFO, "STRIPPER");
		return;
	}
	for (auto& ent : list) {
		g_syscall(G_FS_WRITE, "{\n", 2, f);
		for (auto& keyval : ent.keyvals) {
			std::string s = "\t" + keyval.first + "=" + keyval.second + "\n";
			g_syscall(G_FS_WRITE, s.c_str(), s.size(), f);
		}
		g_syscall(G_FS_WRITE, "}\n", 2, f);
	}
	g_syscall(G_FS_FCLOSE_FILE, f);
	QMM_WRITEQMMLOG(QMM_VARARGS("Ent dump written to %s\n", file.c_str()), QMMLOG_INFO, "STRIPPER");
}


// load and parse config file
void ent_load_config(std::string file) {
	fileHandle_t f;
#if defined(GAME_MOHAA)
	int cmdopen = G_FS_FOPEN_FILE_QMM;
	int cmdclose = G_FS_FCLOSE_FILE_QMM;
#else
	int cmdopen = G_FS_FOPEN_FILE;
	int cmdclose = G_FS_FCLOSE_FILE;
#endif
	if (g_syscall(cmdopen, file.c_str(), &f, FS_READ) < 0)
			return;

	// the current ent
	ent_t ent;

	// each line of text
	std::string line = "";

	// count how many ents are loaded
	int num_filtered = 0, num_added = 0, num_replaced = 0;

	// clear the replace list
	g_replaceents.clear();

	// what the current entity mode is
	enum Mode {
		mode_filter,
		mode_add,
		mode_replace,
		mode_with,
	} mode = mode_filter;

	bool inside_ent = false;	// false = between ents, true = inside an ent

	// go through every line
	while (1) {
		// get line, check for EOF
		line = "";
		if (!read_line(f, line))
			break;

		// skip comments and blank lines
		if (line.empty() || line[0] == '#' || line[0] == ';' || line.substr(0, 2) == "//")
			continue;

		// if not inside an entity, we can either start a new entity or switch modes
		if (!inside_ent) {
			// got a closing brace when not inside an entity, error
			if (line[0] == '}')
				continue;

			// look for mode lines
			if (str_striequal(line, "filter:")) {
				mode = mode_filter;
			}
			else if (str_striequal(line, "add:")) {
				mode = mode_add;
			}
			else if (str_striequal(line, "replace:")) {
				mode = mode_replace;
			}
			else if (str_striequal(line, "with:")) {
				mode = mode_with;
			}
			// valid opening brace, make a new entity
			else if (line[0] == '{') {
				inside_ent = true;
				ent = {};
			}
		}
		// inside an entity. we can either have a keyval pair or end the entity
		else {
			// got an opening brace while already inside an entity, error
			if (line[0] == '{')
				continue;

			// if this is a valid closing brace, handle the entity filter
			if (line[0] == '}') {
				inside_ent = false;

				// don't actually do anything with the entity if it's empty
				if (ent.keyvals.empty())
					continue;

				if (mode == mode_filter) {
					s_ents_filter(g_modents, ent);
					++num_filtered;
				}
				else if (mode == mode_add) {
					s_ents_add(g_modents, ent);
					++num_added;
				}
				else if (mode == mode_replace) {
					g_replaceents.push_back(ent);	// store until a "with" ent comes along
					++num_replaced;
				}
				else if (mode == mode_with) {
					s_ents_replace(g_modents, ent);
				}
			}
			// it's a key/val pair line or something else
			else {
				size_t eq = line.find('=');
				// if no '=', then skip
				if (eq == std::string::npos)
					continue;

				// store key=val
				std::string key = line.substr(0, eq);
				std::string val = line.substr(eq + 1);
				ent.keyvals[key] = val;
				if (key == "classname")
					ent.classname = val;
			}
		}
	} // while(1)

	g_syscall(cmdclose, f);

	QMM_WRITEQMMLOG(QMM_VARARGS("Loaded %d filters, %d adds, and %d replaces from %s\n", num_filtered, num_added, num_replaced, file.c_str()), QMMLOG_INFO, "STRIPPER");
}


// returns true if "test" has at least all the same keyvals that "contains" has
static bool s_ent_match(ent_t& test, ent_t& contains) {
	for (auto& matchkeyval : contains.keyvals) {
		// look up match key in test ent
		auto iter = test.keyvals.find(matchkeyval.first);
		// if key doesn't exist on test, or val doesn't match
		if (iter == test.keyvals.end() || iter->second != matchkeyval.second)
			return false;
	}
	return true;
}


// removes all matching entities from list
static void s_ents_filter(std::vector<ent_t>& list, ent_t& filterent) {
	auto it = list.begin();
	while (it != list.end()) {
		if (s_ent_match(*it, filterent))
			it = list.erase(it);
		else
			++it;
	}
}


// adds an entity to list (puts worldspawn at the beginning)
static void s_ents_add(std::vector<ent_t>& list, ent_t& addent) {
	if (addent.classname == "worldspawn")
		list.insert(list.begin(), addent);
	else
		list.push_back(addent);
}


// finds all entities in list matching all stored replaceents and replaces with a withent
static void s_ents_replace(std::vector<ent_t>& list, ent_t& withent) {
	// go through all replaceents
	for (ent_t& repent : g_replaceents) {
		// find any matching ents in given list
		for (ent_t& ent : list) {
			// replace with withent
			if (s_ent_match(ent, repent))
				s_ent_replace(ent, withent);
		}
	}

	g_replaceents.clear();
}


// replaces all applicable keyvals on an ent
static void s_ent_replace(ent_t& replaceent, ent_t& withent) {
	// go through all keyvals on withent
	for (auto& withkeyval : withent.keyvals) {
		// add/replace val on replaceent
		replaceent.keyvals[withkeyval.first] = withkeyval.second;
	}
}

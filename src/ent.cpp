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
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <vector>
#include <map>
#include <string>
#include <regex>

#include "game.h"
#include "ent.h"
#include "util.h"


MapEntities::MapEntities() : tokeniter(tokenlist.begin()) { }


MapEntities::MapEntities(const MapEntities& other) {
	*this = other;
}


MapEntities& MapEntities::operator=(const MapEntities& other) {
	this->entlist = other.entlist;
	this->tokenlist = other.tokenlist;
	this->entstring = other.entstring;

	auto other_offset = other.tokeniter - other.tokenlist.begin();
	this->tokeniter = this->tokenlist.begin() + other_offset;

	return *this;
}


MapEntities::MapEntities(MapEntities&& other) noexcept {
	*this = other;
}


MapEntities& MapEntities::operator=(MapEntities&& other) noexcept {
	this->entlist = other.entlist;
	this->tokenlist = other.tokenlist;
	this->entstring = other.entstring;

	auto other_offset = other.tokeniter - other.tokenlist.begin();
	this->tokeniter = this->tokenlist.begin() + other_offset;

	other.entlist.clear();
	other.tokenlist.clear();
	other.entstring.clear();
	other.tokeniter = other.tokenlist.end();

	return *this;
}


// populate MapEntities from entstring
void MapEntities::make_from_entstring(EntString entstring) {
	TokenList tokenlist = tokenlist_from_entstring(entstring);
	this->entlist = entlist_from_tokenlist(tokenlist);

	// entlist should be the definitive source that the other fields are generated from
	this->tokenlist = tokenlist_from_entlist(this->entlist);
	this->tokeniter = this->tokenlist.begin();

	this->entstring = entstring_from_entlist(this->entlist);
}


// populate MapEntities from engine tokens
void MapEntities::make_from_engine() {
	TokenList tokenlist = tokenlist_from_engine();
	this->entlist = entlist_from_tokenlist(tokenlist);

	// entlist should be the definitive source that the other fields are generated from
	this->tokenlist = tokenlist_from_entlist(this->entlist);
	this->tokeniter = this->tokenlist.begin();

	this->entstring = entstring_from_entlist(this->entlist);
}


// load and parse config file
void MapEntities::apply_config(std::string file) {
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
	Ent ent;

	// this stores all info for entities that should be replaced
	// nodes are read and removed from this list when a "with" entity is found
	EntList replace_entlist;

	// each line of text
	std::string line = "";

	// count how many ents are loaded
	int num_filtered = 0, num_added = 0, num_replaced = 0;

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

				// filter mode, don't accept empty entity
				if (mode == mode_filter && !ent.keyvals.empty()) {
					filter_ents(ent);
					++num_filtered;
				}
				// add mode, don't accept empty entity or one without a classname
				else if (mode == mode_add && !ent.keyvals.empty() && !ent.classname.empty()) {
					add_ent(ent);
					++num_added;
				}
				// replace mode, accept empty entity to match all
				else if (mode == mode_replace) {
					replace_entlist.push_back(ent);	// store until a "with" ent comes along
					++num_replaced;
				}
				// with mode, don't accept empty entity
				else if (mode == mode_with && !ent.keyvals.empty()) {
					replace_ents(replace_entlist, ent);
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
	} // while(1) - go through every line

	g_syscall(cmdclose, f);

	this->tokenlist = this->tokenlist_from_entlist(this->entlist);
	this->tokeniter = this->tokenlist.begin();
	this->entstring = entstring_from_entlist(this->entlist);

	QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Loaded %d filters, %d adds, and %d replaces from %s\n", num_filtered, num_added, num_replaced, file.c_str()), QMMLOG_INFO);
}


// add keyval to all entities
void MapEntities::add_keyval(std::string key, std::string val) {
	for (auto& ent : this->entlist)
		ent.keyvals[key] = val;

	this->tokenlist = this->tokenlist_from_entlist(this->entlist);
	this->tokeniter = this->tokenlist.begin();
	this->entstring = entstring_from_entlist(this->entlist);
}


// return the next token
intptr_t MapEntities::get_next_token(char* buf, intptr_t len) {
	if (this->tokeniter == this->tokenlist.end())
		return 0;

	strncpyz(buf, this->tokeniter->c_str(), len);

	this->tokeniter++;

	return 1;
}


// return tokenlist
const TokenList& MapEntities::get_tokenlist() {
	return this->tokenlist;
}


// return entlist
const EntList& MapEntities::get_entlist() {
	return this->entlist;
}


// return entstring
const EntString& MapEntities::get_entstring() {
	return this->entstring;
}


// dump to file
void MapEntities::dump_to_file(std::string file, bool append) {
	fileHandle_t f;
	if (g_syscall(G_FS_FOPEN_FILE, file.c_str(), &f, append ? FS_APPEND : FS_WRITE) < 0) {
		QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Unable to write ent dump to %s\n", file.c_str()), QMMLOG_INFO);
		return;
	}
	for (auto& ent : this->entlist) {
		g_syscall(G_FS_WRITE, "{\n", 2, f);
		for (auto& keyval : ent.keyvals) {
			std::string s = "\t" + keyval.first + "=" + keyval.second + "\n";
			g_syscall(G_FS_WRITE, s.c_str(), s.size(), f);
		}
		g_syscall(G_FS_WRITE, "}\n", 2, f);
	}
	g_syscall(G_FS_FCLOSE_FILE, f);
	QMM_WRITEQMMLOG(PLID, QMM_VARARGS(PLID, "Ent dump written to %s\n", file.c_str()), QMMLOG_INFO);
}


// MapEntities private functions
// =============================


// returns true if "test" has at least all the same keyvals that "contains" has
bool MapEntities::is_ent_match(Ent& test, Ent& contains) {
	for (auto& containskeyval : contains.keyvals) {
		// containskeyval.first is the key
		// containskeyval.second is the val

		// look up match key in test ent
		auto iter = test.keyvals.find(containskeyval.first);
		// if key doesn't exist on test
		if (iter == test.keyvals.end())
			return false;

		// first check val for leading and trailing "/" to do a regex match
		std::string match = containskeyval.second;
		if (match[0] == '/' && match[match.size() - 1] == '/') {
			// generate a regex pattern using the val with leading and trailing "/" removed
			std::regex re(match.substr(1, match.size() - 2));
			// doesn't match
			if (!std::regex_match(iter->second, re))
				return false;
		}
		// no regex, val doesn't match
		else if (iter->second != match)
			return false;
	}
	return true;
}


// removes all matching entities from list
void MapEntities::filter_ents(Ent& filterent) {
	auto it = this->entlist.begin();
	while (it != this->entlist.end()) {
		if (is_ent_match(*it, filterent))
			it = this->entlist.erase(it);
		else
			++it;
	}
}


// adds an entity to list (puts worldspawn at the beginning)
void MapEntities::add_ent(Ent& addent) {
	if (addent.classname == "worldspawn")
		this->entlist.insert(this->entlist.begin(), addent);
	else
		this->entlist.push_back(addent);
}


// finds all entities in list matching all stored replaceents and replaces with a withent
void MapEntities::replace_ents(EntList replace_entlist, Ent& withent) {
	// go through all replaceents
	for (auto& repent : replace_entlist) {
		// find any matching ents in given list
		for (auto& ent : this->entlist) {
			// empty repent matches all
			if (repent.keyvals.empty() || is_ent_match(ent, repent))
				// replace with withent
				replace_ent(ent, withent);
		}
	}

	replace_entlist.clear();
}


// replaces all applicable keyvals on an ent
void MapEntities::replace_ent(Ent& replaceent, Ent& withent) {
	// go through all keyvals on withent
	for (auto& withkeyval : withent.keyvals) {
		// add/replace val on replaceent
		replaceent.keyvals[withkeyval.first] = withkeyval.second;
	}
}


// generate a tokenlist from entstring
TokenList MapEntities::tokenlist_from_entstring(EntString entstring) {
	TokenList tokenlist;
	std::string build;
	bool buildstr = false;

	for (auto& c : entstring) {
		// end if null (shouldn't happen)
		if (!c)
			break;
		// skip whitespace outside strings
		else if (std::isspace(c) && !buildstr)
			continue;
		// handle opening braces
		else if (c == '{')
			tokenlist.push_back("{");
		// handle closing braces
		else if (c == '}')
			tokenlist.push_back("}");
		// handle quote, start of a key or value
		else if (c == '"' && !buildstr) {
			build.clear();
			buildstr = true;
		}
		// handle quote, end of a key or value
		else if (c == '"' && buildstr) {
			tokenlist.push_back(build);
			build.clear();
			buildstr = false;
		}
		// all other chars, add to build string
		else
			build.push_back(c);
	}

	return tokenlist;
}


// generate a tokenlist from engine tokens
TokenList MapEntities::tokenlist_from_engine() {
	TokenList tokenlist;
	char buf[MAX_TOKEN_CHARS];

	// get token from engine/QMM
	while (g_syscall(G_GET_ENTITY_TOKEN, buf, sizeof(buf))) {
		buf[sizeof(buf) - 1] = '\0';
		tokenlist.push_back(buf);
	}

	return tokenlist;
}


// generate a tokenlist from entlist
TokenList MapEntities::tokenlist_from_entlist(EntList entlist) {
	TokenList tokenlist;

	for (auto& ent : entlist) {
		tokenlist.push_back("{");
		for (auto& keyval : ent.keyvals) {
			tokenlist.push_back(keyval.first);
			tokenlist.push_back(keyval.second);
		}
		tokenlist.push_back("}");
	}

	return tokenlist;
}


// generate an entlist from engine tokens
EntList MapEntities::entlist_from_tokenlist(TokenList tokenlist) {
	EntList entlist;

	// the current ent
	Ent ent;
	// store key. when a val is received, make a new entry into ent
	std::string key;

	// each token from list
	std::string token;
	TokenList::iterator iter = tokenlist.begin();

	bool inside_ent = false;	// false = between ents, true = inside an ent
	bool is_key = true;			// true = expecting key, false = expecting val

	// loop through all tokens from engine
	while (iter != tokenlist.end()) {
		token = *(iter++);

		// got an opening brace while already inside an entity, error
		if (token[0] == '{' && inside_ent)
			break;

		// got a closing brace when not inside an entity, error
		if (token[0] == '}' && !inside_ent)
			break;

		// if this is a closing brace when expecting a val, error
		if (token[0] == '}' && !is_key)
			break;

		// if this is a valid closing brace, save ent to list and continue
		if (token[0] == '}') {
			inside_ent = false;
			key = "";
			entlist.push_back(ent);
			ent = {};
			continue;
		}

		// if this is a valid opening brace, start a new ent
		if (token[0] == '{') {
			inside_ent = true;
			is_key = true;
			key = "";
			ent = {};
			continue;
		}

		// this is a key
		if (is_key) {
			is_key = false;
			key = token;
		}
		// this is a val
		else {
			is_key = true;
			// store keyval in ent
			ent.keyvals[key] = token;
			// store classname for easier lookup
			if (key == "classname")
				ent.classname = token;
		}
	} // while(1)

	return entlist;
}


// generate an entstring from entlist
EntString MapEntities::entstring_from_entlist(EntList entlist) {
	static EntString entstring;
	entstring = "";

	for (auto& ent : entlist) {
		entstring += "{\n";
		for (auto& keyval : ent.keyvals) {
			entstring += ("\"" + keyval.first + "\" \"" + keyval.second + "\"\n");
		}
		entstring += "}\n";
	}

	return entstring;
}

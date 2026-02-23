/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2026
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
	// grab other's data
	this->entlist = other.entlist;
	this->tokenlist = other.tokenlist;
	this->entstring = other.entstring;

	// calculate other's tokeniter offset to set ours to point to the same entity
	auto other_offset = other.tokeniter - other.tokenlist.begin();
	this->tokeniter = this->tokenlist.begin() + other_offset;

	return *this;
}


MapEntities::MapEntities(MapEntities&& other) noexcept {
	*this = other;
}


MapEntities& MapEntities::operator=(MapEntities&& other) noexcept {
	// grab other's data
	this->entlist = other.entlist;
	this->tokenlist = other.tokenlist;
	this->entstring = other.entstring;

	// calculate other's tokeniter offset to set ours to point to the same entity
	auto other_offset = other.tokeniter - other.tokenlist.begin();
	this->tokeniter = this->tokenlist.begin() + other_offset;

	// clear other's data
	other.entlist.clear();
	other.tokenlist.clear();
	other.entstring.clear();
	other.tokeniter = other.tokenlist.end();

	return *this;
}


// populate MapEntities from entstring
void MapEntities::make_from_entstring(EntString entstring) {
	TokenList tokenlist = tokenlist_from_entstring(entstring);

	// entlist should be the definitive source that the other fields are generated from
	this->entlist = entlist_from_tokenlist(tokenlist);

	this->tokenlist = tokenlist_from_entlist(this->entlist);
	this->tokeniter = this->tokenlist.begin();

	this->entstring = entstring_from_entlist(this->entlist);
}


// populate MapEntities from engine tokens
void MapEntities::make_from_engine() {
	TokenList tokenlist = tokenlist_from_engine();

	// entlist should be the definitive source that the other fields are generated from
	this->entlist = entlist_from_tokenlist(tokenlist);

	this->tokenlist = tokenlist_from_entlist(this->entlist);
	this->tokeniter = this->tokenlist.begin();

	this->entstring = entstring_from_entlist(this->entlist);
}


// load and parse config file
void MapEntities::apply_config(std::string file) {
	fileHandle_t f = 0;
	intptr_t size = g_syscall(G_FS_FOPEN_FILE, file.c_str(), &f, FS_READ);
	// file failed to load
	if (size <= 0 || !f) {
		if (f)
			g_syscall(G_FS_FCLOSE_FILE, f);
		QMM_WRITEQMMLOG(QMM_VARARGS("MapEntities::apply_config(): Failed to open file \"%s\" for reading.\n", file.c_str()), QMMLOG_ERROR);
		return;
	}

	// read entire file
	char* buf = (char*)malloc(size + 1);
	g_syscall(G_FS_READ, buf, size, f);
	g_syscall(G_FS_FCLOSE_FILE, f);
	buf[size] = '\0';

	// check for '=' to warn that it likely won't load
	if (strchr(buf, '='))
		QMM_WRITEQMMLOG(QMM_VARARGS("MapEntities::apply_config(): Possible old config format detected in \"%s\", likely will fail to load.\n", file.c_str()), QMMLOG_WARNING);

	// tokenize it
	TokenList tokens = tokenlist_from_entstring(buf);
	free(buf);

	// the current ent we are building
	Ent ent;
	// store key. when a val is received, make a new entry into ent
	std::string key;

	// this stores all info for entities that should be replaced
	// nodes are read and removed from this list when a "with" entity is found
	EntList replace_entlist;

	// count how many entities are loaded
	int num_filters = 0, num_adds = 0, num_replaces = 0, num_withs = 0;

	// count how many actual map ents are affected
	int num_filtered = 0, num_added = 0, num_replaced = 0;

	// what the current entity mode is
	enum Mode {
		mode_filter,
		mode_add,
		mode_replace,
		mode_with,
	} mode = mode_filter;

	bool inside_ent = false;	// false = between ents, true = inside an ent
	bool is_key = true;			// true = expecting key, false = expecting val 

	// go through every token
	for (const auto& token : tokens) {
		// if not inside an entity, we can either start a new entity or switch modes
		if (!inside_ent) {
			// look for mode tokens
			if (str_striequal(token, "filter:")) {
				mode = mode_filter;
			}
			else if (str_striequal(token, "add:")) {
				mode = mode_add;
			}
			else if (str_striequal(token, "replace:")) {
				mode = mode_replace;
			}
			else if (str_striequal(token, "with:")) {
				mode = mode_with;
			}

			// valid opening brace, make a new entity
			else if (token == "{") {
				inside_ent = true;
				is_key = true;
				ent = {};
			}

			// unknown token
			else {
				QMM_WRITEQMMLOG(QMM_VARARGS("MapEntities::apply_config(): Unexpected token \"%s\", expected \"filter:\", \"add:\", \"replace:\", \"with:\", or \"{\"; ignoring.\n", token.c_str()), QMMLOG_WARNING);
			}
		}
		// inside an entity. we can either have a key, value, or end the entity
		else {
			// if this is a valid closing brace, handle the entity filter
			if (token == "}") {
				inside_ent = false;

				// if entity ended between key and val, print warning
				if (!is_key) {
					QMM_WRITEQMMLOG(QMM_VARARGS("MapEntities::apply_config(): Unexpected end of entity with hanging key \"%s\"; ignoring.\n", key.c_str()), QMMLOG_WARNING);
				}

				// filter mode, don't accept empty entity
				if (mode == mode_filter) {
					if (ent.keyvals.empty()) {
						QMM_WRITEQMMLOG("MapEntities::apply_config(): Empty \"filter\" entity found; ignoring.\n", QMMLOG_WARNING);
					}
					else {
						num_filters++;
						num_filtered += this->filter_ents(ent);
					}
				}
				// add mode, don't accept empty entity or one without a classname
				else if (mode == mode_add) {
					if (ent.keyvals.empty()) {
						QMM_WRITEQMMLOG("MapEntities::apply_config(): Empty \"add\" entity found; ignoring.\n", QMMLOG_WARNING);
					}
					else if (ent.classname.empty()) {
						QMM_WRITEQMMLOG("MapEntities::apply_config(): \"add\" entity found without \"classname\"; ignoring.\n", QMMLOG_WARNING);
					}
					else {
						num_adds++;
						num_added += this->add_ent(ent);
					}
				}
				// replace mode, accept empty entity to match all
				else if (mode == mode_replace) {
					num_replaces++;
					replace_entlist.push_back(ent);	// store until a "with" ent comes along
				}
				// with mode, don't accept empty entity
				else if (mode == mode_with) {
					if (ent.keyvals.empty()) {
						QMM_WRITEQMMLOG("MapEntities::apply_config(): Empty \"with\" entity found; ignoring.\n", QMMLOG_WARNING);
					}
					else {
						num_withs++;
						num_replaced += this->replace_ents(replace_entlist, ent);
						// "with" entry uses up all prior "replace" entities
						replace_entlist.clear();
					}
				}
			}

			// look for mode tokens or opening brace
			else if (
				str_striequal(token, "filter:")
				|| str_striequal(token, "add:")
				|| str_striequal(token, "replace:")
				|| str_striequal(token, "with:")
				|| token == "{"
				) {
				QMM_WRITEQMMLOG(QMM_VARARGS("MapEntities::apply_config(): Unexpected \"%s\" token found inside an entity; ignoring.\n", token.c_str()), QMMLOG_WARNING);
			}

			// it's a key or val
			else {
				// this is a key
				if (is_key) {
					// if key is empty, skip it
					if (token.empty()) {
						QMM_WRITEQMMLOG("MapEntities::apply_config(): Unexpected empty token found, expected key; ignoring.\n", QMMLOG_WARNING);
					}
					else {
						is_key = false;
						key = token;
					}
				}
				// this is a value
				else {
					is_key = true;

					// don't allow value to be empty in "add:" block
					if (mode == mode_add && token.empty()) {
						QMM_WRITEQMMLOG(QMM_VARARGS("MapEntities::apply_config(): Unexpected empty value for key \"%s\" found in \"add\" entity; ignoring.\n", key.c_str()), QMMLOG_WARNING);
					}
					else {
						// store keyval in ent
						ent.keyvals[key] = token;
						// store classname for easier lookup
						if (key == "classname") {
							ent.classname = token;
						}
					}
				}
			}
		}
	}

	this->tokenlist = tokenlist_from_entlist(this->entlist);
	this->tokeniter = this->tokenlist.begin();
	this->entstring = entstring_from_entlist(this->entlist);

	QMM_WRITEQMMLOG(QMM_VARARGS("Loaded %d filters, %d adds, %d replace, and %d withs from %s.\n", num_filters, num_adds, num_replaces, num_withs, file.c_str()), QMMLOG_INFO);
	QMM_WRITEQMMLOG(QMM_VARARGS("Removed %d entities, added %d entities, and replaced %d entities.\n", num_filtered, num_added, num_replaced), QMMLOG_INFO);
}


// add keyval to all entities
void MapEntities::add_keyval(std::string key, std::string val) {
	for (auto& ent : this->entlist)
		ent.keyvals[key] = val;

	this->tokenlist = tokenlist_from_entlist(this->entlist);
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
		QMM_WRITEQMMLOG(QMM_VARARGS("Unable to write ent dump to %s\n", file.c_str()), QMMLOG_INFO);
		return;
	}
	// output entities in engine entity format: {} on separate lines, tabbed indents, and "" surrounding key and val
	for (auto& ent : this->entlist) {
		g_syscall(G_FS_WRITE, "{\n", 2, f);
		for (auto& keyval : ent.keyvals) {
			std::string s = "\t\"" + keyval.first + "\" \"" + keyval.second + "\"\n";
			g_syscall(G_FS_WRITE, s.c_str(), s.size(), f);
		}
		g_syscall(G_FS_WRITE, "}\n", 2, f);
	}
	g_syscall(G_FS_FCLOSE_FILE, f);
	QMM_WRITEQMMLOG(QMM_VARARGS("Ent dump written to %s\n", file.c_str()), QMMLOG_INFO);
}


// MapEntities private functions
// =============================


// returns true if "test" has at least all the same keyvals that "contains" has
bool MapEntities::is_ent_match(Ent& test, Ent& contains) {
	for (auto& containskeyval : contains.keyvals) {
		const std::string& matchkey = containskeyval.first;
		const std::string& matchval = containskeyval.second;

		// look up matchkey in test ent
		auto iter = test.keyvals.find(matchkey);
		// if matchkey doesn't exist in test
		if (iter == test.keyvals.end()) {
			// an empty matchval means test should NOT have the matchkey, so continue to the next match keyval
			if (matchval.empty())
				continue;
			// matchkey missing, test ent doesn't match
			return false;
		}

		const std::string& testval = iter->second;

		// check matchval for leading and trailing "/" to do a regex match
		if (matchval[0] == '/' && matchval[matchval.size() - 1] == '/') {
			// generate a regex pattern using the matchval with leading and trailing "/" removed
			std::regex pattern(matchval.substr(1, matchval.size() - 2));
			// testval doesn't match
			if (!std::regex_match(testval, pattern))
				return false;
		}
		// no regex, val doesn't match
		else if (testval != matchval)
			return false;
	}
	return true;
}


// removes all matching entities from internal list
int MapEntities::filter_ents(Ent& filterent) {
	int total = 0;
	auto it = this->entlist.begin();
	while (it != this->entlist.end()) {
		if (is_ent_match(*it, filterent)) {
			it = this->entlist.erase(it);
			total++;
		}
		else
			++it;
	}
	return total;
}


// adds an entity to internal list (puts worldspawn at the beginning)
int MapEntities::add_ent(Ent& addent) {
	if (addent.classname == "worldspawn")
		this->entlist.insert(this->entlist.begin(), addent);
	else
		this->entlist.push_back(addent);
	return 1;
}


// finds all entities in internal list matching all stored replaceents and replaces with a withent
int MapEntities::replace_ents(EntList& replace_entlist, Ent& withent) {
	int total = 0;
	// go through all replace_ents
	for (auto& repent : replace_entlist) {
		// go through all ents in internal list
		for (auto& ent : this->entlist) {
			// find any matching ent; empty repent matches all
			if (repent.keyvals.empty() || is_ent_match(ent, repent)) {
				total++;
				// replace with withent
				replace_ent(ent, withent);
			}
		}
	}
	return total;
}


// adds all keyvals from withent into replaceent (replacing the val if a key already exists)
void MapEntities::replace_ent(Ent& replaceent, Ent& withent) {
	// go through all keyvals on withent
	for (auto& withkeyval : withent.keyvals) {
		// empty val means to remove key if it exists
		if (withkeyval.second.empty())
			replaceent.keyvals.erase(withkeyval.first);
		// add/replace val on replaceent
		else
			replaceent.keyvals[withkeyval.first] = withkeyval.second;
	}
}


// generate a tokenlist from entstring
TokenList MapEntities::tokenlist_from_entstring(const EntString& entstring) {
	TokenList tokenlist;
	std::string build;
	bool buildstr = false;

	for (size_t i = 0; i < entstring.size(); i++) {
		auto& c = entstring[i];
		// whitespace: end current token
		if (std::isspace(c)) {
			if (!build.empty())
				tokenlist.push_back(build);
			build.clear();
		}
		// non-printable characters, just skip it
		else if (c < ' ' || c == 127) {
			continue;
		}
		// closing braces: end current token
		else if (c == '{') {
			if (!build.empty())
				tokenlist.push_back(build);
			build.clear();
			tokenlist.push_back("{");
		}
		// closing braces: end current token
		else if (c == '}') {
			if (!build.empty())
				tokenlist.push_back(build);
			build.clear();
			tokenlist.push_back("}");
		}
		// quotes: end current token
		// then scan forward until next quote and store whole string as 1 token
		else if (c == '"') {
			if (!build.empty())
				tokenlist.push_back(build);
			build.clear();

			i++;
			size_t start = i;
			while (i < entstring.size() && entstring[i] != '"')
				i++;
			tokenlist.push_back(entstring.substr(start, i - start));
		}
		// all other characters, add to build string
		else {
			build += c;
		}
	}

	// any remaining token being built
	if (!build.empty())
		tokenlist.push_back(build);

	return tokenlist;
}


// generate a tokenlist from engine tokens
TokenList MapEntities::tokenlist_from_engine() {
	TokenList tokenlist;
	char buf[MAX_TOKEN_CHARS];

	// get tokens from engine/QMM and add to tokenlist
	while (g_syscall(G_GET_ENTITY_TOKEN, buf, sizeof(buf))) {
		buf[sizeof(buf) - 1] = '\0';
		tokenlist.push_back(buf);
	}

	return tokenlist;
}


// generate a tokenlist from entlist
TokenList MapEntities::tokenlist_from_entlist(const EntList& entlist) {
	TokenList tokenlist;

	// for every entity, add a "{", all the keyvals, and "}"
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
EntList MapEntities::entlist_from_tokenlist(const TokenList& tokenlist) {
	EntList entlist;

	// the current ent
	Ent ent;
	// store key. when a val is received, make a new entry into ent
	std::string key;

	// each token from list
	std::string token;
	TokenList::const_iterator iter = tokenlist.begin();

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
EntString MapEntities::entstring_from_entlist(const EntList& entlist) {
	EntString entstring;

	// for every entity, add a "{", all the keyvals, and "}" 
	for (auto& ent : entlist) {
		entstring += "{\n";
		for (auto& keyval : ent.keyvals) {
			entstring += ("\"" + keyval.first + "\" \"" + keyval.second + "\"\n");
		}
		entstring += "}\n";
	}

	return entstring;
}

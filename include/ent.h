/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2024
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#ifndef __STRIPPER_QMM_ENT_H__
#define __STRIPPER_QMM_ENT_H__

#include "game.h"
#include <vector>
#include <map>
#include <string>

// this represents a single entity
struct Ent {
	std::string classname; // classname is stored when encountered for easier retrieval
	std::map<std::string, std::string> keyvals;
};

// typedefs for common types used in MapEntities
typedef std::vector<std::string> TokenList;
typedef std::vector<Ent> EntList;
typedef std::string EntString;

// this represents a map's worth of entities
struct MapEntities {
    public:
        // need constructors to handle re-initialization of iterator member
        MapEntities();
        MapEntities(const MapEntities& other);
        MapEntities& operator=(const MapEntities& other);
        MapEntities(MapEntities&& other) noexcept;
        MapEntities& operator=(MapEntities&& other) noexcept;

        // populate MapEntities from entstring
        void make_from_entstring(EntString entstring);
        // populate MapEntities from engine tokens
        void make_from_engine();

        // load and parse config file and apply to ents
        void apply_config(std::string file);
        // add keyval to all entities
        void add_keyval(std::string key, std::string val);

        // return the next token
        intptr_t get_next_token(char* buf, intptr_t len);

        // return tokenlist
        const TokenList& get_tokenlist();
        // return entlist
        const EntList& get_entlist();
        // return entstring
        const EntString& get_entstring();

        // dump entlist to file
        void dump_to_file(std::string file, bool append = false);

    private:
        TokenList tokenlist;
        EntList entlist;
        EntString entstring;

        TokenList::iterator tokeniter;

        // returns true if "test" has all the same keyvals that "contains" has
        static bool is_ent_match(Ent& test, Ent& contains);
        // removes all matching entities from list
        void filter_ents(Ent& filterent);
        // adds an entity to list (puts worldspawn at the beginning)
        void add_ent(Ent& addent);
        // finds all entities in list matching all stored replaceents and replaces with a withent
        void replace_ents(EntList replace_entlist, Ent& withent);
        // replaces all applicable keyvals on an ent
        static void replace_ent(Ent& replaceent, Ent& withent);

        // generate a tokenlist from entstring
        static TokenList tokenlist_from_entstring(EntString entstring);
        // generate a tokenlist from engine tokens
        static TokenList tokenlist_from_engine();
        // generate a tokenlist from entlist
        static TokenList tokenlist_from_entlist(EntList entlist);
        // generate an entlist from engine tokens
        static EntList entlist_from_tokenlist(TokenList tokenlist);
        // generate an entstring from entlist
        static EntString entstring_from_entlist(EntList entlist);
};
#endif // __STRIPPER_QMM_ENT_H__

/*
Stripper - Dynamic Map Entity Modification
Copyright 2004-2026
https://github.com/thecybermind/stripper_qmm/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#ifndef STRIPPER_QMM_ENT_HPP
#define STRIPPER_QMM_ENT_HPP

#include <vector>
#include <map>
#include <string>

// this represents a single entity's worth of keyvals
struct Ent {
    std::string classname; // classname is stored when encountered for easier retrieval
    std::map<std::string, std::string> keyvals;
};

/* Aliases for common types used in MapEntities */

// List of string tokens ("{", "}", "key", "val", "add:")
using TokenList = std::vector<std::string>;
// List of entities
using EntList = std::vector<Ent> ;
// Entity string, given by some engines
using EntString = std::string;

// This represents a map's worth of entities
struct MapEntities {
    public:
        MapEntities();
        // Copy constructor will also re-calculate stored iterator
        MapEntities(const MapEntities& other);
        // Assignment operator will also re-calculate stored iterator
        MapEntities& operator=(const MapEntities& other);
        MapEntities(MapEntities&& other) noexcept;
        MapEntities& operator=(MapEntities&& other) noexcept;

        // Populate MapEntities from entstring
        void make_from_entstring(const EntString& entstring);
        // Populate MapEntities from engine tokens
        void make_from_engine();

        // Load and parse config file and apply to entities
        void apply_config(std::string file);
        // Add keyval to all entities
        void add_keyval(const std::string& key, const std::string& val);

        // Return the next token for G_GET_ENTITY_TOKEN hook
        intptr_t get_next_token(char* buf, intptr_t len);

        // Return stored TokenList
        const TokenList& get_tokenlist();
        // Return stored EntList
        const EntList& get_entlist();
        // Return stored EntString
        const EntString& get_entstring();

        // Dump entities to file
        void dump_to_file(std::string file, bool append = false);

    private:
        TokenList tokenlist;
        EntList entlist;
        EntString entstring;

        TokenList::iterator tokeniter;  // Iterator into tokenlist used by get_next_token()

        // Returns true if "test" has all the same keyvals that "contains" has
        static bool is_ent_match(const Ent& test, const Ent& contains);
        // Removes all matching entities from list
        int filter_ents(const Ent& filterent);
        // Adds an entity to list (puts worldspawn at the beginning)
        int add_ent(const Ent& addent);
        // Finds all entities in list matching all stored replaceents and replaces with a withent
        int replace_ents(EntList& replace_entlist, const Ent& withent);
        // Replaces all applicable keyvals on replaceent with those from withent
        static void replace_ent(Ent& replaceent, const Ent& withent);

        // Generate a TokenList from EntString
        static TokenList tokenlist_from_entstring(const EntString& entstring);
        // Generate a TokenList from engine tokens
        static TokenList tokenlist_from_engine();
        // Generate a TokenList from EntList
        static TokenList tokenlist_from_entlist(const EntList& entlist);
        // Generate an EntList from engine tokens
        static EntList entlist_from_tokenlist(const TokenList& tokenlist);
        // Generate an EntString from EntList
        static EntString entstring_from_entlist(const EntList& entlist);
};
#endif // STRIPPER_QMM_ENT_HPP

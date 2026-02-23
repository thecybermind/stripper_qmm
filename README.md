# Stripper  
Stripper - Dynamic Map Entity Modification  
Copyright 2004-2026  
https://github.com/thecybermind/stripper_qmm/  
3-clause BSD license: https://opensource.org/license/bsd-3-clause  

Created By: Kevin Masterson < k.m.masterson@gmail.com >

---

1. Install QMM ( https://github.com/thecybermind/qmm2/wiki/Installation )
2. Make a qmmaddons/stripper/ directory inside your mod directory and place stripper_qmm.dll file here
3. Add the path to stripper_qmm.dll as an entry in the plugins list in qmm2.json

## Setup:
### Server Commands:
* stripper_dump - Dumps the current maps' default entity list to `qmmaddons/stripper/dumps/{mapname}.txt` and the modified entity list to `qmmaddons/stripper/dumps/{mapname}_modent.txt`

### Configuration Files:
There are 2 files loaded per map. One is the global configuration file that is loaded for every map, and the other is specific to the current map.

The global configuration file is located at `qmmaddons/stripper/global.ini` and the map-specific file is located at `qmmaddons/stripper/maps/{mapname}.ini`. A sample global.ini and q3dm1.ini are provided in the release.

#### Syntax
In Stripper v2.5.0, the configuration format changed to match the entity token format used in the engine (with the addition of the "type:" tokens). The old "key=val" format will no longer work, and comments are no longer supported.

Configuration files have the following format:

    filter:
    {
       "key" "val"
       "key2" "val2"
    }
    {
       "key3" "val3"
    }
    add:
    {
       "key" "val"
    }
    replace:
    {
       "key" "val"
    }
    {
       "key" "val2"
    }
    with:
    {
       "key" "val3"
    }
    
#### Sections
A section affects all entities after it, until the next section is encountered. The section token must be used outside of an entity (i.e. not inside braces {}).

- `filter`:  
    This section specifies entity masks for entities that should be removed from the map.

    If an entity has all the keys provided in a mask\*, and the values associated with them match, the entity is removed.
	
    \* A `filter` key with an empty value matches if the key does *not* exist on the entity.
	
    For example, this would remove all entities with an `angle` of `90` and without a `spawnflags` key:

    ```C
    filter:
    {
	"spawnflags" ""
	"angle" "90"
    }
	```

- `add`:  
    This section specifies complete entities to add to the map. Entities are added with only the exact keys and values provided in the block.
    
    You must provide at least a `classname` key. Values cannot be empty.

    If you add an entity with the `classname` of `worldspawn`, it will be added at the start of the entity list. Note: it is unclear what will happen if the mod receives 2 or more `worldspawn` entities; at best it will ignore the extra and at worst the game will exit with an error.

- `replace`:  
    This section specifies entity masks for entities that should be replaced/modified.
    
    If an entity has all the keys provided in a mask\*, and the values associated with them match, the entity will be replaced.

    \* A `replace` key with an empty value matches if the key does *not* exist on the entity.

    For example, this will set the `gametype` of `ffa` on all entities that don't already have a `gametype` key:

    ```C
    replace:
    {
      "gametype" ""
    }
    with:
    {
      "gametype" "ffa"
    }
    ```

    \* A `replace` mask with no keys will match with all entities.

    For example, this will set the `gametype` of `ffa` on all entities (even those that already have a `gametype` key):
    
    ```C
    replace:
    {
	
    }
    with:
    {
      "gametype" "ffa"
    }
    ``` 

- `with`:  
    This section specifies what keys and values should be set on any entity that matches a previous `replace` mask.
    
    The entity in a `with` section will be associated with all `replace` masks since the previous `with` block (or the start of the file if none). A `with` block should only have 1 entity given, since the first one will have "used up" all the available `replace` masks.

    Only the keys provided in the `with` block will be affected.
    
    A `with` key with an empty value means that key will be removed (if it exists) from an affected entity.

    For example, this will remove the `spawnflags` key for any entity with a `classname` of either `info_player_deathmatch` or `light`:

    ```C
    replace:
    {
     "classname" "info_player_deathmatch"
    }
    {
     "classname" "light"
    }
    with:
    {
     "spawnflags" ""
    }
    ```

#### Regex
As of v2.4.2, `filter` and `replace` value matches now support regex matching (using C++11 <regex>). Simply surround the value with `/` to trigger regex matching:

    # replace all weapons with railguns
    replace:
    {
       "classname" "/weapon_.*/"
    }
    with:
    {
       "classname" "weapon_railgun"
    }

Note that Stripper will test the regex against the entire value string. 

#### Notes
`filter`, `add`, and `with` entities modify the entity list in the order they appear. For example, the following will result in no new entities being added, since the second `filter` section will cause the added health kit to be removed:

    filter:
    {
       "classname" "item_health"
    }
    add:
    {
       "classname" "item_health"
       "origin" "100 100 10"
    }
    filter:
    {
       "classname" "item_health"
    }

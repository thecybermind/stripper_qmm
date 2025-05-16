# Stripper  
Stripper - Dynamic Map Entity Modification  
Copyright 2004-2025  
https://github.com/thecybermind/stripper_qmm/  
3-clause BSD license: https://opensource.org/license/bsd-3-clause  

Created By: Kevin Masterson < cybermind@gmail.com >

---

1. Install QMM ( https://github.com/thecybermind/qmm2/wiki/Installation )
2. Make a qmmaddons/stripper/ directory inside your mod directory and place stripper_qmm.dll file here
3. Add the path to stripper_qmm.dll as an entry in the plugins list in qmm2.json

## Setup:
### Server Commands:
* stripper_dump - Dumps the current maps' default entity list to qmmaddons/stripper/dumps/<mapname>.txt

### Configuration Files:
There are 2 files loaded per map. One is the global configuration file that is loaded for every map, and the other is specific to the current map.

The global configuration file is located at qmmaddons/stripper/global.ini and the map-specific file is located at qmmaddons/stripper/maps/[mapname].ini. A sample global.ini and q3dm1.ini are provided in the package.

Configuration files have the following format:

    ;this line is a comment
    #this line is a comment
    //this line is a comment
    
    filter:
    {
       key=val
    }
    {
       key=val
    }
    add:
    {
       key=val
    }
    replace:
    {
       key=val
    }
    {
       key=val
    }
    with:
    {
       key=val
    }
    
- A "`filter`" section specifies entity masks for entities that should be removed from the map. If an entity has all the keys provided in a mask, and the values associated with them match, the entity is removed.
- An "`add`" section specifies complete entity information to add to the map. Entities are added with only the keys and values provided in the block.
- A "`replace`" section specifies entity masks for entities that should be replaced or modified. If an entity has all the keys provided in a mask, and the values associated with them match, the entity will be replaced or modified.
- A "`with`" section specifies what keys and values should be replaced on any associated "`replace`" masks. Only the keys provided in the "`with`" block will be replaced. A "`with`" section will affect all "`replace`" masks created after the previous "`with`" block (or the start of the file if none).

"`filter`", "`add`", and "`with`"/"`replace`" modify the entity list in the order they appear. For example, the following will result in no new entities being added, since the second "filter" section will cause the added item to be removed:

    filter:
    {
       classname=item_health
    }
    add:
    {
       classname=item_health
       origin=100 100 10
    }
    filter:
    {
       classname=item_health
    }

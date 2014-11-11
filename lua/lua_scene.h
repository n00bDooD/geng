#ifndef LUASCENE_H
#define LUASCENE_H

#include <lua.h>

#include "../scene.h"

int register_scene(lua_State*);

scene* luaG_checkscene(lua_State*, int);

#endif /* LUASCENE_H */

#ifndef LUASCENE_H
#define LUASCENE_H

#include <lua.h>

#include "../scene.h"

int register_scene(lua_State*, scene*);

scene* luaG_checkscene(lua_State*, int);

scene* get_scene_registry(lua_State* l);

void step_scene(scene*, double ts);

#endif /* LUASCENE_H */

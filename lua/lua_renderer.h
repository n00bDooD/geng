#ifndef LUAREND_H
#define LUAREND_H

#include <lua.h>

#include "../services/sdl_renderer.h"

int register_renderer(lua_State*, sdl_renderer*);

sdl_renderer* get_renderer_registry(lua_State* l);

sdl_renderer* luaG_checkrenderer(lua_State*, int);

#endif /* LUAREND_H */

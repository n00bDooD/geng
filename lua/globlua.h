#ifndef GLOBLUA_H
#define GLOBLUA_H

#include <lua.h>
#include <lauxlib.h>
#include "../scene.h"
#include "../services/inputaxis.h"
#include "../services/sdl_audio.h"

void luaG_register_all(lua_State*, scene*, inputaxis_data*,sdl_audio*);

void luaG_getreg(lua_State*, const char*);
void luaG_setreg(lua_State*, const char*);
void luaG_copy_state(lua_State* from, lua_State* to);
void luaG_init_state(lua_State*);
lua_State* luaG_newstate(lua_State*);

void dbg_printstack(lua_State*);

#endif /* GLOBLUA_H */

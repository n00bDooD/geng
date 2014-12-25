#ifndef GLOBLUA_H
#define GLOBLUA_H

#include <lua.h>
#include "../scene.h"
#include "../services/inputaxis.h"
#include "../services/sdl_audio.h"

void luaG_register_all(lua_State*, scene*, inputaxis_data*,sdl_audio*);

void dbg_printstack(lua_State*);

#endif /* GLOBLUA_H */

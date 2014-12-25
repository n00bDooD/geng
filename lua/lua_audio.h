#ifndef LUA_AUDIO_H
#define LUA_AUDIO_H

#include <lua.h>

#include "../services/sdl_audio.h"

int register_audio(lua_State*);
int register_config_audio(lua_State*,sdl_audio*);

sdl_audio* luaG_checkaudio(lua_State*, int);

#endif /* LUA_AUDIO_H */

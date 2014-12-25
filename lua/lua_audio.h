#ifndef LUA_AUDIO_H
#define LUA_AUDIO_H

#include <lua.h>

#include "../services/sdl_audio.h"

int register_audio(lua_State*, sdl_audio*);
int register_config_audio(lua_State*,sdl_audio*);

sdl_audio* luaG_checkaudio(lua_State*, int);

sdl_audio* get_audio_registry(lua_State*);

#endif /* LUA_AUDIO_H */

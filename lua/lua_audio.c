#include "lua_audio.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <string.h>

#include <SDL2/SDL_mixer.h>

#define TYPE_NAME "audio"

static int lua_openaudio(lua_State* l)
{
	// Frequecy default 44100khz
	int freq = luaL_optinteger(l, 1, 44100);
	// Default two channels
	int channels = luaL_optinteger(l, 2, 2);
	// Default 2048 byte chunks
	int chunksize = luaL_optinteger(l, 3, 2048);

	Mix_OpenAudio(freq, MIX_DEFAULT_FORMAT, channels, chunksize);
	return 0;
}

static const luaL_Reg methods[] = {
	{NULL, NULL}
};

static const luaL_Reg private_methods[] = {
	{"open", lua_openaudio},
	{NULL, NULL}
};

int register_audio(lua_State *L)
{
	/* Create methods table & add it to globals */
	luaL_openlib(L, TYPE_NAME, methods, 0);
	/* Create metatable for object, and add it to registry */
	luaL_newmetatable(L, TYPE_NAME);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3); 		/* duplicate methods table */
	lua_rawset(L, -3);		/* metatable.__index = methods */

	lua_pop(L, 1); 			/* drop metatable */
	return 1;			/* leave methods on stack */
}

int register_config_audio(lua_State *L)
{
	luaL_register(L, TYPE_NAME, private_methods);
	lua_pop(L, 1);
	return 0;
}

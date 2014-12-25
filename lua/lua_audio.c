#include "lua_audio.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <string.h>

#include <SDL2/SDL_mixer.h>

#include "../services/sdl_audio.h"

#define TYPE_NAME "audio"
#define REGISTRY_KEY "geng.renderer"

sdl_audio* luaG_checkaudio(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	sdl_audio* s = NULL;
	s = (sdl_audio*)luaL_checkudata(L, index, TYPE_NAME);
	if (s == NULL) luaL_typerror(L, index, TYPE_NAME);
	return s;
}

sdl_audio* get_audio_registry(lua_State* l)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_rawget(l, LUA_REGISTRYINDEX);
	sdl_audio* ret = (sdl_audio*)lua_touserdata(l, -1);
	if(ret == NULL) {
		luaL_error(l, "No current audio player");
	}
	lua_pop(l, 1);
	return ret;
}

static void set_audio_registry(lua_State* l, sdl_audio* s)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_pushlightuserdata(l, s);
	/* registry['REGISTRY_KEY'] = s */
	lua_rawset(l, LUA_REGISTRYINDEX);
}

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

static int lua_loadchunk(lua_State* l)
{
	sdl_audio* a = get_audio_registry(l);

	// Get name of file
	const char* fname = lua_tolstring(l, 1, NULL);
	if (fname == NULL) luaL_error(l, "chunk name cannot be NULL");

	// Load the file
	Mix_Chunk* s = Mix_LoadWAV(fname);
	if (!s) {
		luaL_error(l, Mix_GetError());
	}

	// If successful, add the file to the array of chunks
	size_t idx = 0;
	if (a->chunks == NULL) {
		idx = 1;
	} else {
		while(a->chunks[idx++] != NULL);
	}
	Mix_Chunk** e = realloc(a->chunks, idx * sizeof(Mix_Chunk*));
	if (e == NULL) {
		luaL_error(l, "realloc");
	}
	
	a->chunks = e;
	a->chunks[idx-1] = s;
	a->chunks[idx] = NULL;

	lua_pushnumber(l, idx+1);
	return 1;
}

static int lua_get_chunk_volume(lua_State* l)
{
	sdl_audio* a = get_audio_registry(l);

	int id = luaL_checkinteger(l, 1);
	int vol = Mix_VolumeChunk(a->chunks[id-1],
			luaL_optinteger(l, 2, -1));
	lua_pushnumber(l, vol);
	return 1;
}

static int lua_play_chunk(lua_State* l)
{
	sdl_audio* a = get_audio_registry(l);

	// -1 plays on first free channel
	int channel = luaL_optinteger(l, 2, -1);
	// The chunk to play
	int idx = luaL_checkinteger(l, 1);
	Mix_Chunk* c = a->chunks[idx-1];
	// Number of loops. -1 means infinite
	int loops = luaL_optinteger(l, 3, -1);
	// Number of milliseconds to play,
	// -1 means infinite
	int ms = luaL_optinteger(l, 3, -1);
	int fade = luaL_optinteger(l, 4, -1);
	int res = 0;
	if (fade == -1) {
		res = Mix_PlayChannelTimed(
			channel,
			c,
			loops,
			ms);
	} else {
		res = Mix_FadeInChannelTimed(
				channel,
				c,
				loops,
				fade,
				ms);
	}
	if (res == -1) {
		luaL_error(l, Mix_GetError());
	}

	return 0;
}

static int lua_channels(lua_State* l)
{
	lua_pushnumber(l,
		Mix_AllocateChannels(
			luaL_optinteger(l, 1, -1)
			)
		);
	return 1;
}

static const luaL_Reg methods[] = {
	{"volume", lua_get_chunk_volume},
	{"play", lua_play_chunk},
	{NULL, NULL}
};

static const luaL_Reg private_methods[] = {
	{"open", lua_openaudio},
	{"channels", lua_channels},
	{"load_chunk", lua_loadchunk},
	//{"load_music", lua_loadmusic},
	{NULL, NULL}
};

int register_audio(lua_State *L, sdl_audio* a)
{
	/* Create methods table & add it to globals */
	luaL_openlib(L, TYPE_NAME, methods, 0);
	/* Create metatable for object, and add it to registry */
	luaL_newmetatable(L, TYPE_NAME);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3); 		/* duplicate methods table */
	lua_rawset(L, -3);		/* metatable.__index = methods */

	lua_pop(L, 1); 			/* drop metatable */
	set_audio_registry(L, a);
	return 1;			/* leave methods on stack */
}

int register_config_audio(lua_State *L, sdl_audio* a)
{
	luaL_register(L, TYPE_NAME, private_methods);
	lua_pop(L, 1);
	set_audio_registry(L, a);
	return 0;
}

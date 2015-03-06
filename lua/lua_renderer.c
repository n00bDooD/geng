#include "lua_renderer.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define TYPE_NAME "renderer"
#define REGISTRY_KEY "geng.renderer"


sdl_renderer* luaG_checkrenderer(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	sdl_renderer* s = NULL;
	s = (sdl_renderer*)luaL_checkudata(L, index, TYPE_NAME);
	if (s == NULL) luaL_typerror(L, index, TYPE_NAME);
	return s;
}

sdl_renderer* get_renderer_registry(lua_State* l)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_rawget(l, LUA_REGISTRYINDEX);
	sdl_renderer* ret = (sdl_renderer*)lua_touserdata(l, -1);
	if(ret == NULL) {
		luaL_error(l, "No current renderer");
	}
	lua_pop(l, 1);
	return ret;
}

static void set_renderer_registry(lua_State* l, sdl_renderer* s)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_pushlightuserdata(l, s);
	/* registry['REGISTRY_KEY'] = s */
	lua_rawset(l, LUA_REGISTRYINDEX);
}

static int lua_add_texture(lua_State* l)
{
	sdl_renderer* r = get_renderer_registry(l);
	const char* str = luaL_checklstring(l, 1, NULL);

	texhandle t = sdl_renderer_add_texture(r,
			create_tex_from_file(r, str));
	lua_pushinteger(l, (lua_Integer)t);
	return 1;
}

static int lua_add_sprite(lua_State* l)
{
	sdl_renderer* r = get_renderer_registry(l);
	texhandle tex = (texhandle)luaL_checkinteger(l, 1);
	int xoff = luaL_checkint(l, 2);
	int yoff = luaL_checkint(l, 3);
	int texx = luaL_optint(l, 4, -1);
	int texy = luaL_optint(l, 5, -1);
	int texh = luaL_optint(l, 6, -1);
	int texw = luaL_optint(l, 7, -1);

	spritehandle s = sdl_renderer_add_sprite(r, tex,
			                         xoff, yoff,
					         texx, texy,
					         texh, texw);
	if (s == 0) {
		luaL_error(l, "Failure to create sprite. \
				Is the texture handle valid?");
	}
	lua_pushinteger(l, (lua_Integer)s);
	return 1;
}

static int lua_set_background(lua_State* l)
{
	sdl_renderer* r = get_renderer_registry(l);
	texhandle tex = (texhandle)luaL_checkinteger(l, 1);

	r->background = tex;
	return 0;
}

static int lua_set_texture_blendmode(lua_State* l)
{
	sdl_renderer* r = get_renderer_registry(l);
	texhandle tex = (texhandle)luaL_checkinteger(l, 1);

	SDL_BlendMode m = SDL_BLENDMODE_BLEND;
	long mode = luaL_checkinteger(l, 2);
	switch(mode) {
		case 0:
			m = SDL_BLENDMODE_NONE;
			break;
		case 1:
			m = SDL_BLENDMODE_BLEND;
			break;
		case 2:
			m = SDL_BLENDMODE_ADD;
			break;
		case 3:
			m = SDL_BLENDMODE_MOD;
			break;
	}
	int res = SDL_SetTextureBlendMode(r->textures[tex-1], m);
	if(res != 0) {
		luaL_error(l, SDL_GetError());
	}
	return 0;
}

static int lua_set_scale(lua_State* l)
{
	sdl_renderer* r = get_renderer_registry(l);
	r->cam.scale = luaL_checknumber(l, 1);

	return 0;
}

static int lua_set_pos(lua_State* l)
{
	sdl_renderer* r = get_renderer_registry(l);
	r->cam.x = luaL_checknumber(l, 1);
	r->cam.y = luaL_checknumber(l, 2);
	return 0;
}

static const luaL_Reg methods[] = {
	{"add_texture", lua_add_texture},
	{"add_sprite", lua_add_sprite},
	{"set_background", lua_set_background},
	{"set_blendmode", lua_set_texture_blendmode},
	{"set_cam_scale", lua_set_scale},
	{"set_cam_pos", lua_set_pos},
	{NULL, NULL}
};

/*
static const luaL_Reg meta_methods[] = {
	{NULL, NULL}
};
*/

int register_renderer(lua_State *L, sdl_renderer* s)
{
	set_renderer_registry(L, s);

	luaL_register(L, TYPE_NAME, methods);
	lua_pop(L, 1);
	return 0;
}

#include "lua_scene.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define TYPE_NAME "scene"
#define REGISTRY_KEY "geng.scene"

#include "../scene.h"
#include "lua_object.h"

scene* luaG_checkscene(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	scene* s = NULL;
	s = (scene*)luaL_checkudata(L, index, TYPE_NAME);
	if (s == NULL) luaL_typerror(L, index, TYPE_NAME);
	return s;
}

scene* get_scene_registry(lua_State* l)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_rawget(l, LUA_REGISTRYINDEX);
	scene* ret = (scene*)lua_touserdata(l, -1);
	if(ret == NULL) luaL_error(l, "No current scene");
	return ret;
}

void set_scene_registry(lua_State* l, scene* s)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_pushlightuserdata(l, s);
	/* registry['REGISTRY_KEY'] = s */
	lua_rawset(l, LUA_REGISTRYINDEX);
}

static int lua_new_object(lua_State* l)
{
	scene* s = get_scene_registry(l);
	object* o = create_object(s);
	luaG_pushobject(l, o);
	return 1;
}

static const luaL_Reg methods[] = {
	{"newobject", lua_new_object},
	{NULL, NULL}
};

static const luaL_Reg meta_methods[] = {
	{NULL, NULL}
};

int register_scene(lua_State *L, scene* s)
{
	set_scene_registry(L, s);

	luaL_register(L, TYPE_NAME, methods);
	lua_pop(L, 1);
	return 0;
}

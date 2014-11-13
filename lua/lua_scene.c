#include "lua_scene.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define TYPE_NAME "scene"
#define REGISTRY_KEY "geng.scene"

#include "../scene.h"
#include "lua_object.h"

#include <string.h>

char* get_prefab_by_name(scene* s, const char* name)
{
	if (s->num_prefabs == 0) return NULL;

	size_t i = 0;
	do {
		if(strcmp(name, s->prefab_names[i]) == 0){
			return s->prefabs[i];
		}
	} while (s->prefab_names[++i] != NULL) ;
	return NULL;
}

object* create_prefab(lua_State* l, scene* s, const char* name)
{
	char* code = get_prefab_by_name(s, name);
	if (code == NULL) {
		luaL_error(l, "Unknown prefab");
	}
	int load_result = luaL_loadstring(l, code);
	switch(load_result) {
		case 0:
			// OK
			break;
		case LUA_ERRSYNTAX:
			luaL_error(l, "Syntax error in prefab.");
		case LUA_ERRMEM:
			luaL_error(l, "Memory allocation error on load of prefab.");
	}
	int run_result = lua_pcall(l, 0, LUA_MULTRET, 0);
	switch(run_result) {
		case 0: {
			// OK
			object_ref* res = luaG_checkobject(l, -1);
			if(res == NULL) return NULL;
			return res->o;
			}
		case LUA_ERRRUN:
			luaL_error(l, "Runtime error when executing prefab");
		case LUA_ERRMEM:
			luaL_error(l, "Memory error when executing prefab");
		case LUA_ERRERR:
			luaL_error(l, "Error when executing prefab");
	}
	return NULL;
}

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

static int lua_spawn_prefab(lua_State* l)
{
	scene* s = get_scene_registry(l);
	const char* name = luaL_checklstring(l, 1, NULL);
	if(name == NULL) luaL_error(l, "Name required");
	object* o = create_prefab(l, s, name);
	if(o == NULL) luaL_error(l, "Object NULL error");
	luaG_pushobject(l, o);
	return 1;
}

static const luaL_Reg methods[] = {
	{"newobject", lua_new_object},
	{"spawn_prefab", lua_spawn_prefab},
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

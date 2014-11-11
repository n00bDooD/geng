#include "lua_scene.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define TYPE_NAME "scene"

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

static int lua_new_object(lua_State* l)
{
	scene* s = luaG_checkscene(l, 1);
	luaG_pushobject(l, create_object(s));
	return 0;
}

static const luaL_reg methods[] = {
	{"newobject", lua_new_object},
	{NULL, NULL}
};

static const luaL_reg meta_methods[] = {
	{NULL, NULL}
};

int register_scene(lua_State *L)
{
	/* Create methods table & add it to globals */
	luaL_openlib(L, TYPE_NAME, methods, 0);
	/* Create metatable for object, and add it to registry */
	luaL_newmetatable(L, TYPE_NAME);
	luaL_openlib(L, 0, meta_methods, 0); /* dill metatable */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3); 		/* duplicate methods table */
	lua_rawset(L, -3);		/* metatable.__index = methods */
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);		/* duplicate methods table */
	lua_rawset(L, -3);		/* hide metatable:
					   metatabme.__metatable = methods */

	lua_pop(L, 1); 			/* drop metatable */
	return 1;			/* leave methods on stack */
}

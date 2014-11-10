#include "lua_object.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <string.h>

#include "../object.h"

#include "lua_vector.h"

#define TYPE_NAME "object"

object* luaG_checkobject(lua_State* L, int index)
{
	object* o;
	luaL_checktype(L, index, LUA_TUSERDATA);
	o = (object*)luaL_checkudata(L, index, TYPE_NAME);
	if (o == NULL) luaL_typerror(L, index, TYPE_NAME);
	return o;
}

object* luaG_pushobject(lua_State *L)
{
	object* o = (object*)lua_newuserdata(L, sizeof(object));
	luaL_getmetatable(L, TYPE_NAME);
	lua_setmetatable(L, -2);
	return o;
}

static int lua_object_new(lua_State* l)
{
	size_t len = 0;
	const char* name = lua_tolstring(l, 1, &len);
	if (name == NULL) luaL_typerror(l, 1, "string");
	object* o = luaG_pushobject(l);
	o->name = (char*)malloc(sizeof(len + 1));
	if (name == NULL) luaL_error(l, "Cannot allocate string of length %i.", len);
	strncpy(o->name, name, len);
	return 1;
}

static int lua_object_tostring(lua_State *L)
{
	object* o = luaG_checkobject(L, 1);
	lua_pushstring(L, o->name);
	return 1;
}

static int lua_object_position(lua_State* l)
{
	object* o = luaG_checkobject(l, 1);

	cpVect* pos = luaG_pushvect(l);
	*pos = get_object_position(o);
	return 1;
}

static int lua_object_angle(lua_State* l)
{
	object* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_angle(o));
	return 1;
}

static const luaL_reg methods[] = {
	{"new", lua_object_new},
	{"pos", lua_object_position},
	{"angle", lua_object_angle},
	{NULL, NULL}
};

static const luaL_reg meta_methods[] = {
	{"__tostring", lua_object_tostring},
	{NULL, NULL}
};

int register_object(lua_State *L)
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




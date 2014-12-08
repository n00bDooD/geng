#include "lua_box.h"

#include "lua_vector.h"

#include <lua.h>
#include <lauxlib.h>

#include <chipmunk/chipmunk.h>

#define TYPE_NAME "box"

cpBB* luaG_checkbox(lua_State* L, int index)
{
	cpBB* o;
	luaL_checktype(L, index, LUA_TUSERDATA);
	o = (cpBB*)luaL_checkudata(L, index, TYPE_NAME);
	if (o == NULL) luaL_typerror(L, index, TYPE_NAME);
	return o;
}

cpBB* luaG_pushbox(lua_State* L)
{
	cpBB* v = (cpBB*)lua_newuserdata(L, sizeof(cpBB));
	luaL_getmetatable(L, TYPE_NAME);
	lua_setmetatable(L, -2);
	return v;
}


static int lua_box_intersects(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpBB* b = luaG_checkbox(l, 2);
	lua_pushboolean(l, cpBBIntersects(*a, *b));
	return 1;
}

static int lua_box_contains(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpBB* b = luaG_checkbox(l, 2);
	lua_pushboolean(l, cpBBContainsBB(*a, *b));
	return 1;
}

static int lua_box_containsvect(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpVect* b = luaG_checkvect(l, 2);
	lua_pushboolean(l, cpBBContainsVect(*a, *b));
	return 1;
}

static const luaL_reg methods[] = {
	{"intersects", lua_box_intersects},
	{"contains", lua_box_contains},
	{"contains_vect", lua_box_containsvect},
	{NULL, NULL}
};



static const luaL_reg meta_methods[] = {
	{NULL, NULL}
};

int register_box(lua_State *L)
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

#include "lua_collisionpair.h"
#include <stdlib.h>
#include <lualib.h>
#include <lauxlib.h>

#define TYPE_NAME "collpair"

collision_pair* luaG_checkcollpair(lua_State* L, int index)
{
	collision_pair* c;
	luaL_checktype(L, index, LUA_TUSERDATA);
	c = (collision_pair*)luaL_checkudata(L, index, TYPE_NAME);
	if (c == NULL) luaL_typerror(L, index, TYPE_NAME);
	return c;
}


collision_pair* luaG_pushcollpair(lua_State* l)
{
	collision_pair* c = (collision_pair*)lua_newuserdata(l,
			sizeof(collision_pair));
	luaL_getmetatable(l, TYPE_NAME);
	lua_setmetatable(l, -2);
	return c;
}

static const luaL_reg methods [] = {
	{NULL, NULL}
};

static const luaL_reg meta_methods [] = {
	{NULL, NULL}
};

int register_collpair(lua_State* L)
{
	luaL_openlib(L, TYPE_NAME, methods, 0);
	luaL_newmetatable(L, TYPE_NAME);
	luaL_openlib(L, 0, meta_methods, 0);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 1); 
	return 1;
}


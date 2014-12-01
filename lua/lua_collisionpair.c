#include "lua_collisionpair.h"
#include <stdlib.h>
#include <lualib.h>
#include <lauxlib.h>

#include <chipmunk/chipmunk.h>

#include "lua_vector.h"

#define TYPE_NAME "collpair"

collision_pair* luaG_checkcollpair(lua_State* L, int index)
{
	collision_pair* c;
	luaL_checktype(L, index, LUA_TUSERDATA);
	c = (collision_pair*)luaL_checkudata(L, index, TYPE_NAME);
	if (c == NULL) luaL_typerror(L, index, TYPE_NAME);
	return c;
}


collision_pair* luaG_pushcollpair(lua_State* l, void* data)
{
	collision_pair* c = (collision_pair*)lua_newuserdata(l,
			sizeof(collision_pair));
	luaL_getmetatable(l, TYPE_NAME);
	lua_setmetatable(l, -2);
	c->data = data;
	return c;
}

static int lua_collpair_get_elasticity(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	lua_pushnumber(l, cpArbiterGetElasticity(p->data));
	return 1;
}

static int lua_collpair_get_friction(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	lua_pushnumber(l, cpArbiterGetFriction(p->data));
	return 1;
}

static int lua_collpair_get_surfvel(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	cpVect* v = luaG_pushvect(l);
	*v = cpArbiterGetSurfaceVelocity(p->data);
	return 1;
}

static const luaL_reg methods [] = {
	{"get_elasticity", lua_collpair_get_elasticity},
	{"get_friction", lua_collpair_get_friction},
	{"get_surfvel", lua_collpair_get_surfvel},
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


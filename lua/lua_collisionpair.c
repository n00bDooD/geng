#include "lua_collisionpair.h"
#include <stdlib.h>
#include <string.h>
#include <lualib.h>
#include <lauxlib.h>

#include <chipmunk/chipmunk.h>

#include "lua_vector.h"
#include "lua_object.h"
#include "lua_colliders.h"

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

static int lua_collpair_set_elasticity(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	if (p->current != COLL_PRESOLVE) {
		luaL_error(l, "Setting elasticity outside of a preSolve \
				callback will not have an effect");
	}
	cpArbiterSetElasticity(p->data, luaL_checknumber(l, 2));
	return 1;
}

static int lua_collpair_get_friction(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	lua_pushnumber(l, cpArbiterGetFriction(p->data));
	return 1;
}

static int lua_collpair_set_friction(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	if (p->current != COLL_PRESOLVE) {
		luaL_error(l, "Setting friction outside of a preSolve \
				callback will not have an effect");
	}
	cpArbiterSetFriction(p->data, luaL_checknumber(l, 2));
	return 1;
}

static int lua_collpair_get_surfvel(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	cpVect* v = luaG_pushvect(l);
	*v = cpArbiterGetSurfaceVelocity(p->data);
	return 1;
}

static int lua_collpair_set_surfvel(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	if (p->current != COLL_PRESOLVE) {
		luaL_error(l, "Setting surface velocity outside of a preSolve \
				callback will not have an effect");
	}
	cpVect* v = luaG_checkvect(l, 2);
	cpArbiterSetSurfaceVelocity(p->data, *v);
	return 1;
}

static int lua_collpair_get_impulse_f(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	cpVect* v = luaG_pushvect(l);
	*v = cpArbiterTotalImpulseWithFriction(p->data);
	return 1;
}

static int lua_collpair_get_impulse(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	cpVect* v = luaG_pushvect(l);
	*v = cpArbiterTotalImpulse(p->data);
	return 1;
}

static int lua_collpair_get_ke(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	if(p->current != COLL_POSTSOLVE) {
		luaL_error(l, "Can only be called from a postStep callback");
	}
	lua_pushnumber(l, cpArbiterTotalKE(p->data));
	return 1;
}

static int lua_collpair_get_objects(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	CP_ARBITER_GET_BODIES(p->data, a, b);
	luaG_pushobject(l, cpBodyGetUserData(a));
	luaG_pushobject(l, cpBodyGetUserData(b));
	return 2;
}

static int lua_collpair_get_shapes(lua_State* l)
{
	collision_pair* p = luaG_checkcollpair(l, 1);
	collider* a = luaG_pushcoll(l);
	collider* b = luaG_pushcoll(l);
	cpArbiterGetShapes(p->data, &a->shape, &b->shape);
	return 2;
}

static const luaL_reg methods [] = {
	{"get_elasticity", lua_collpair_get_elasticity},
	{"get_friction", lua_collpair_get_friction},
	{"get_surfvel", lua_collpair_get_surfvel},
	{"set_elasticity", lua_collpair_set_elasticity},
	{"set_friction", lua_collpair_set_friction},
	{"set_surfvel", lua_collpair_set_surfvel},
	{"get_impulse", lua_collpair_get_impulse_f},
	{"get_impulse_without_friction", lua_collpair_get_impulse},
	{"get_ke", lua_collpair_get_ke},
	{"get_objects", lua_collpair_get_objects},
	{"get_colliders", lua_collpair_get_shapes},
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


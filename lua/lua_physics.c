#include "lua_physics.h"

#include <lua.h>
#include <lauxlib.h>

#include "lua_scene.h"
#include "lua_colliders.h"
#include "lua_vector.h"
#include "lua_box.h"

#include <chipmunk/chipmunk.h>

#define TYPE_NAME "physics"

static int lua_query_nearest(lua_State* l)
{
	scene* s = get_scene_registry(l);

	cpNearestPointQueryInfo qi;
	//cpQueryNearestPointQueryNearest(s->physics_data, *luaG_checkvect(l, 1), luaL_checknumber(l, 2), 0, 0, &qi);
	collider* c = luaG_pushcoll(l);
	c->shape = qi.shape;

	cpVect* v = luaG_pushvect(l);
	v->x = qi.p.x;
	v->y = qi.p.y;
	lua_pushnumber(l, qi.d);
	cpVect* v2 = luaG_pushvect(l);
	v2->x = qi.g.x;
	v2->y = qi.g.y;
	return 4;
}

void bbqueryfunc(cpShape* sh, void* data)
{
	lua_State* l = (lua_State*)data;
	// Functions are deleted after calls,
	// so we'll save the function for later.
	lua_pushvalue(l, -1);
	collider* c = luaG_pushcoll(l);
	c->shape = sh;
	lua_pcall(l, 1, 0, 0);
}

static int lua_query_aabb(lua_State* l)
{
	scene* s = get_scene_registry(l);
	
	cpBB foo = {.l=1, .b=2, .r=3, .t=4};
	luaL_checktype(l, 1, LUA_TFUNCTION);
	cpSpaceBBQuery(s->physics_data, foo, 0, 0, bbqueryfunc, l);
	return 0;
}

static const luaL_Reg methods[] = {
	{"nearest", lua_query_nearest},
	{"box", lua_query_aabb},
	{NULL, NULL}
};

static const luaL_Reg meta_methods[] = {
	{NULL, NULL}
};

int register_physics(lua_State *L)
{
	luaL_register(L, TYPE_NAME, methods);
	lua_pop(L, 1);
	return 0;
}


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
	if (cpSpaceNearestPointQueryNearest(
			s->physics_data,
			*luaG_checkvect(l, 1),
			luaL_checknumber(l, 2),
			CP_ALL_LAYERS,
			CP_NO_GROUP,
			&qi) == NULL) {
		lua_pushnil(l);
		return 1;
	}
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
	cpBB* box = luaG_checkbox(l, 1);

	luaL_checktype(l, 2, LUA_TFUNCTION);
	// Func has to be at the top of the stack
	lua_settop(l, 2);
	cpSpaceBBQuery(s->physics_data, *box,
			CP_ALL_LAYERS,
			CP_NO_GROUP,
			bbqueryfunc, l);
	return 0;
}

void segmentqueryfunc(cpShape* sh, cpFloat t, cpVect n, void* data)
{
	lua_State* l = (lua_State*)data;
	lua_pushvalue(l, -1);
	collider* c = luaG_pushcoll(l);
	c->shape = sh;
	lua_pushnumber(l, t);
	cpVect* v = luaG_pushvect(l);
	*v = n;
	lua_pcall(l, 3, 0, 0);
}

static int lua_query_segment(lua_State* l)
{
	scene* s = get_scene_registry(l);
	cpVect* start = luaG_checkvect(l, 1);
	cpVect* end = luaG_checkvect(l, 2);

	cpSpaceSegmentQuery(s->physics_data,
			*start, *end,
			CP_ALL_LAYERS,
			CP_NO_GROUP,
			segmentqueryfunc, l);
	return 0;
}

static int lua_query_segment_first(lua_State* l)
{
	scene* s = get_scene_registry(l);
	cpVect* start = luaG_checkvect(l, 1);
	cpVect* end = luaG_checkvect(l, 2);

	cpSegmentQueryInfo i;
	if(cpSpaceSegmentQueryFirst(s->physics_data,
				*start, *end,
				CP_ALL_LAYERS,
				CP_NO_GROUP,
				&i) == NULL) {
		lua_pushnil(l);
		return 1;
	}
	collider* c = luaG_pushcoll(l);
	c->shape = i.shape;
	lua_pushnumber(l, i.t);
	cpVect* v = luaG_pushvect(l);
	*v = i.n;
	return 3;
}

static const luaL_Reg methods[] = {
	{"nearest", lua_query_nearest},
	{"box", lua_query_aabb},
	{"segment", lua_query_segment},
	{"segment_first", lua_query_segment_first},
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

